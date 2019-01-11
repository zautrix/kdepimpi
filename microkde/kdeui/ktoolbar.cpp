/* This file is part of the KDE libraries
    Copyright
    (C) 2000 Reginald Stadlbauer (reggie@kde.org)
    (C) 1997, 1998 Stephan Kulow (coolo@kde.org)
    (C) 1997, 1998 Mark Donohoe (donohoe@kde.org)
    (C) 1997, 1998 Sven Radej (radej@kde.org)
    (C) 1997, 1998 Matthias Ettrich (ettrich@kde.org)
    (C) 1999 Chris Schlaeger (cs@kde.org)
    (C) 1999 Kurt Granroth (granroth@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifdef KDE_USE_FINAL
#undef Always
#include <qdockwindow.h>
#endif



#include "ktoolbar.h"
#include "kmainwindow.h"

#include <string.h>

#include <qpainter.h>
#include <qtooltip.h>
#include <qdrawutil.h>
#include <qstring.h>
#include <qrect.h>
#include <qobjectlist.h>
#include <qtimer.h>
#include <qstyle.h>
#include <qapplication.h>

//US #include <config.h>

#include "klineedit.h"
#include "kseparator.h"
#include <klocale.h>
#include <kapplication.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kcombobox.h>
//US #include <kpopupmenu.h>
//US #include <kanimwidget.h>
//US #include <kipc.h>
//US #include <kwin.h>
#include <kdebug.h>
#include <qlayout.h>

#include "ktoolbarbutton.h"

//US 
#include "kconfigbase.h"

#include <qpopupmenu.h>
#include <qmainwindow.h>

enum {
    CONTEXT_TOP = 0,
    CONTEXT_LEFT = 1,
    CONTEXT_RIGHT = 2,
    CONTEXT_BOTTOM = 3,
    CONTEXT_FLOAT = 4,
    CONTEXT_FLAT = 5,
    CONTEXT_ICONS = 6,
    CONTEXT_TEXT = 7,
    CONTEXT_TEXTRIGHT = 8,
    CONTEXT_TEXTUNDER = 9,
    CONTEXT_ICONSIZES = 50 // starting point for the icon size list, put everything else before
};

class KToolBarPrivate
{
public:
    KToolBarPrivate() {
        m_iconSize     = 0;
        m_iconText     = KToolBar::IconOnly;
        m_highlight    = true;
        m_transparent  = true;
        m_honorStyle   = false;

        m_enableContext  = true;

        m_xmlguiClient   = 0;
        m_configurePlugged = false;

//US        oldPos = Qt::DockUnmanaged;
        oldPos = QMainWindow::Unmanaged;

        modified = m_isHorizontal = positioned = FALSE;

        HiddenDefault = false;
        IconSizeDefault = 0;
        IconTextDefault = "IconOnly";
        IndexDefault = -1;
        NewLineDefault = false;
        OffsetDefault = -1;
        PositionDefault = "Top";
        idleButtons.setAutoDelete(true);
    }

    int m_iconSize;
    KToolBar::IconText m_iconText;
    bool m_highlight : 1;
    bool m_transparent : 1;
    bool m_honorStyle : 1;
    bool m_isHorizontal : 1;
    bool m_enableContext : 1;
    bool m_configurePlugged : 1;
    bool modified : 1;
    bool positioned : 1;

    QWidget *m_parent;

    QMainWindow::ToolBarDock oldPos;

    KXMLGUIClient *m_xmlguiClient;

    struct ToolBarInfo
    {
//US        ToolBarInfo() : index( 0 ), offset( -1 ), newline( FALSE ), dock( Qt::DockTop ) {}
        ToolBarInfo() : index( 0 ), offset( -1 ), newline( FALSE ), dock( QMainWindow::Top ) {}
//US        ToolBarInfo( Qt::Dock d, int i, bool n, int o ) : index( i ), offset( o ), newline( n ), dock( d ) {}
        ToolBarInfo( QMainWindow::ToolBarDock d, int i, bool n, int o ) : index( i ), offset( o ), newline( n ), dock( d ) {}
        int index, offset;
        bool newline;
//US        Qt::Dock dock;
        QMainWindow::ToolBarDock dock;
    };

    ToolBarInfo toolBarInfo;
    QValueList<int> iconSizes;
    QTimer repaintTimer;

  // Default Values.
  bool HiddenDefault;
  int IconSizeDefault;
  QString IconTextDefault;
  int IndexDefault;
  bool NewLineDefault;
  int OffsetDefault;
  QString PositionDefault;

   QPtrList<QWidget> idleButtons;
};

KToolBarSeparator::KToolBarSeparator(Orientation o , bool l, QToolBar *parent,
                                     const char* name )
    :QFrame( parent, name ), line( l )
{
    connect( parent, SIGNAL(orientationChanged(Orientation)),
             this, SLOT(setOrientation(Orientation)) );
    setOrientation( o );
    setBackgroundMode( parent->backgroundMode() );
    setBackgroundOrigin( ParentOrigin );
}

void KToolBarSeparator::setOrientation( Orientation o )
{
    orient = o;
    if ( line ) {
        if ( orientation() == Vertical )
            setFrameStyle( HLine + Sunken );
        else
            setFrameStyle( VLine + Sunken );
    } else {
            setFrameStyle( NoFrame );
    }
}

void KToolBarSeparator::styleChange( QStyle& )
{
    setOrientation( orient );
}

QSize KToolBarSeparator::sizeHint() const
{
    return orientation() == Vertical ? QSize( 0, 6 ) : QSize( 6, 0 );
}

QSizePolicy KToolBarSeparator::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}

KToolBar::KToolBar( QWidget *parent, const char *name, bool honorStyle, bool readConfig )
#ifdef DESKTOP_VERSION
    : QToolBar( QString::fromLatin1( name ),
      parent && parent->inherits( "QMainWindow" ) ? static_cast<QMainWindow*>(parent) : 0,
      parent, FALSE,
      name ? name : "mainToolBar")
#else
  : QPEToolBar( parent && parent->inherits( "QMainWindow" ) ? static_cast<QMainWindow*>(parent) : 0,
                QString::fromLatin1( name ))


#endif
{
    init( readConfig, honorStyle );
}

KToolBar::KToolBar( QMainWindow *parentWindow, QMainWindow::ToolBarDock dock, bool newLine, const char *name, bool honorStyle, bool readConfig )
#ifdef DESKTOP_VERSION
    : QToolBar( QString::fromLatin1( name ),
      parentWindow, dock, newLine,
      name ? name : "mainToolBar")
#else
  : QPEToolBar( parentWindow,QString::fromLatin1( name ))


#endif

{
    init( readConfig, honorStyle );
}

KToolBar::KToolBar( QMainWindow *parentWindow, QWidget *dock, bool newLine, const char *name, bool honorStyle, bool readConfig )
#ifdef DESKTOP_VERSION
    : QToolBar( QString::fromLatin1( name ),
      parentWindow, dock, newLine,
      name ? name : "mainToolBar")
#else
  : QPEToolBar( parentWindow,QString::fromLatin1( name ))


#endif

{
    init( readConfig, honorStyle );
}

KToolBar::~KToolBar()
{
    inshutdownprocess = true;
     emit toolbarDestroyed();
     delete d;
}

void KToolBar::init( bool readConfig, bool honorStyle )
{
    sizeHintW = 240;
    sizeHintH = 22;
    inshutdownprocess = false;
    d = new KToolBarPrivate;
    setFullSize( TRUE );
    d->m_honorStyle = honorStyle;
    context = 0;
    layoutTimer = new QTimer( this );
    connect( layoutTimer, SIGNAL( timeout() ),
             this, SLOT( rebuildLayout() ) );
    connect( &(d->repaintTimer), SIGNAL( timeout() ),
             this, SLOT( slotRepaint() ) );
/*US
    if ( kapp ) { // may be null when started inside designer
        connect(kapp, SIGNAL(toolbarAppearanceChanged(int)), this, SLOT(slotAppearanceChanged()));
        // request notification of changes in icon style
        kapp->addKipcEventMask(KIPC::IconChanged);
        connect(kapp, SIGNAL(iconChanged(int)), this, SLOT(slotIconChanged(int)));
    }
*/
    // finally, read in our configurable settings
    if ( readConfig )
        slotReadConfig();

    if ( mainWindow() )
        connect( mainWindow(), SIGNAL( toolBarPositionChanged( QToolBar * ) ),
                 this, SLOT( toolBarPosChanged( QToolBar * ) ) );

    // Hack to make sure we recalculate our size when we dock.
//US    connect( this, SIGNAL(placeChanged(QDockWindow::Place)), SLOT(rebuildLayout()) );
}

int KToolBar::insertButton(const QString& icon, int id, bool enabled,
                            const QString& text, int index/*US, KInstance *_instance*/ )
{
    KToolBarButton *button = new KToolBarButton( icon, id, this, 0, text/*US, _instance*/ );

    insertWidgetInternal( button, index, id );
    button->setEnabled( enabled );
    doConnections( button );
    return index;
}


int KToolBar::insertButton(const QString& icon, int id, const char *signal,
                            const QObject *receiver, const char *slot,
                            bool enabled, const QString& text, int index/*US, KInstance *_instance*/ )
{
    KToolBarButton *button = new KToolBarButton( icon, id, this, 0, text/*US, _instance*/);
    insertWidgetInternal( button, index, id );
    button->setEnabled( enabled );
    connect( button, signal, receiver, slot );
    doConnections( button );
    return index;
}


int KToolBar::insertButton(const QPixmap& pixmap, int id, bool enabled,
                            const QString& text, int index )
{
    KToolBarButton *button = new KToolBarButton( pixmap, id, this, 0, text);
    insertWidgetInternal( button, index, id );
    button->setEnabled( enabled );
    doConnections( button );
    return index;
}
#if 0
 bar->insertButton( icon, id_, SIGNAL( clicked() ), this,
                           SLOT( slotActivated() ),
                           d->isEnabled(), d->plainText(), index/*US, instance*/ );
#endif

int KToolBar::insertButton(const QPixmap& pixmap, int id, const char *signal,
                            const QObject *receiver, const char *slot,
                            bool enabled, const QString& text,
                            int index )
{
    KToolBarButton *button = new KToolBarButton( pixmap, id, this, 0, text);
    insertWidgetInternal( button, index, id );
    button->setEnabled( enabled );
    connect( button, signal, receiver, slot );
    doConnections( button );
    return index;
}


int KToolBar::insertButton(const QString& icon, int id, QPopupMenu *popup,
                            bool enabled, const QString &text, int index )
{
    KToolBarButton *button = new KToolBarButton( icon, id, this, 0, text );
    insertWidgetInternal( button, index, id );
    button->setEnabled( enabled );
    button->setPopup( popup );
    doConnections( button );
    return index;
}


int KToolBar::insertButton(const QPixmap& pixmap, int id, QPopupMenu *popup,
                            bool enabled, const QString &text, int index )
{
    KToolBarButton *button = new KToolBarButton( pixmap, id, this, 0, text );
    insertWidgetInternal( button, index, id );
    button->setEnabled( enabled );
    button->setPopup( popup );
    doConnections( button );
    return index;
}


int KToolBar::insertLined (const QString& text, int id,
                            const char *signal,
                            const QObject *receiver, const char *slot,
                            bool enabled ,
                            const QString& toolTipText,
                            int size, int index )
{
    KLineEdit *lined = new KLineEdit ( this, 0 );
    if ( !toolTipText.isEmpty() )
        QToolTip::add( lined, toolTipText );
    if ( size > 0 )
        lined->setMinimumWidth( size );
    insertWidgetInternal( lined, index, id );
    connect( lined, signal, receiver, slot );
    lined->setText(text);
    lined->setEnabled( enabled );
    return index;
}

int KToolBar::insertCombo (const QStringList &list, int id, bool writable,
                            const char *signal, const QObject *receiver,
                            const char *slot, bool enabled,
                            const QString& tooltiptext,
                            int size, int index,
                            QComboBox::Policy policy )
{
//US    KComboBox *combo = new KComboBox ( writable, this );
    KComboBox *combo = new KComboBox ( this );
    combo->setEditable(writable);
    
    insertWidgetInternal( combo, index, id );
    combo->insertStringList (list);
    combo->setInsertionPolicy(policy);
    combo->setEnabled( enabled );
    if ( !tooltiptext.isEmpty() )
        QToolTip::add( combo, tooltiptext );
    if ( size > 0 )
        combo->setMinimumWidth( size );
    if (!tooltiptext.isNull())
        QToolTip::add( combo, tooltiptext );

    if ( signal && receiver && slot )
        connect ( combo, signal, receiver, slot );
    return index;
}


int KToolBar::insertCombo (const QString& text, int id, bool writable,
                            const char *signal, QObject *receiver,
                            const char *slot, bool enabled,
                            const QString& tooltiptext,
                            int size, int index,
                            QComboBox::Policy policy )
{
//US    KComboBox *combo = new KComboBox ( writable, this );
    KComboBox *combo = new KComboBox ( this );
    combo->setEditable(writable);
    
    insertWidgetInternal( combo, index, id );
    combo->insertItem (text);
    combo->setInsertionPolicy(policy);
    combo->setEnabled( enabled );
    if ( !tooltiptext.isEmpty() )
        QToolTip::add( combo, tooltiptext );
    if ( size > 0 )
        combo->setMinimumWidth( size );
    if (!tooltiptext.isNull())
        QToolTip::add( combo, tooltiptext );
    connect (combo, signal, receiver, slot);
    return index;
}

int KToolBar::insertSeparator(int index, int id)
{
    QWidget *w = new KToolBarSeparator( orientation(), FALSE, this, "tool bar separator" );
    insertWidgetInternal( w, index, id );
    return index;
}

int KToolBar::insertLineSeparator(int index, int id)
{
    QWidget *w = new KToolBarSeparator( orientation(), TRUE, this, "tool bar separator" );
    insertWidgetInternal( w, index, id );
    return index;
}


int KToolBar::insertWidget(int id, int /*width*/, QWidget *widget, int index)
{
    // removeWidgetInternal( widget ); // in case we already have it ?
    insertWidgetInternal( widget, index, id );
    return index;
}
/*US
int KToolBar::insertAnimatedWidget(int id, QObject *receiver, const char *slot,
                                    const QString& icons, int index )
{
    KAnimWidget *anim = new KAnimWidget( icons, d->m_iconSize, this );
    insertWidgetInternal( anim, index, id );

    if ( receiver )
        connect( anim, SIGNAL(clicked()), receiver, slot);

    return index;
}

KAnimWidget *KToolBar::animatedWidget( int id )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return 0;
    if ( (*it) && (*it)->inherits( "KAnimWidget" ) )
        return (KAnimWidget*)(*it);
    QObjectList *l = queryList( "KAnimWidget" );
    if ( !l || !l->first() ) {
        delete l;
        return 0;
    }

    for ( QObject *o = l->first(); o; o = l->next() ) {
        if ( o->inherits( "KAnimWidget" ) )
        {
            delete l;
            return (KAnimWidget*)o;
        }
    }

    delete l;
    return 0;
}
*/

void KToolBar::addConnection (int id, const char *signal,
                               const QObject *receiver, const char *slot)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
    if ( (*it) )
        connect( (*it), signal, receiver, slot );
}

void KToolBar::setItemEnabled( int id, bool enabled )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
    if ( (*it) )
        (*it)->setEnabled( enabled );
}


void KToolBar::setButtonPixmap( int id, const QPixmap& _pixmap )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->setPixmap( _pixmap );
}


void KToolBar::setButtonIcon( int id, const QString& _icon )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->setIcon( _icon );
}

void KToolBar::setButtonIconSet( int id, const QIconSet& iconset )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->setIconSet( iconset );
}


void KToolBar::setDelayedPopup (int id , QPopupMenu *_popup, bool toggle )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->setDelayedPopup( _popup, toggle );
}


void KToolBar::setAutoRepeat (int id, bool flag)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->setAutoRepeat( flag );
}


void KToolBar::setToggle (int id, bool flag )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->setToggle( flag );
}


void KToolBar::toggleButton (int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->toggle();
}


void KToolBar::setButton (int id, bool flag)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if ( button )
        button->on( flag );
}


bool KToolBar::isButtonOn (int id) const
{
    Id2WidgetMap::ConstIterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return false;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    return button ? button->isOn() : false;
}


void KToolBar::setLinedText (int id, const QString& text)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    QLineEdit * lineEdit = dynamic_cast<QLineEdit *>( *it );
    QLineEdit * lineEdit = (QLineEdit *)( *it );
    if ( lineEdit )
        lineEdit->setText( text );
}


QString KToolBar::getLinedText (int id) const
{
    Id2WidgetMap::ConstIterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return QString::null;
//US    QLineEdit * lineEdit = dynamic_cast<QLineEdit *>( *it );
    QLineEdit * lineEdit = (QLineEdit *)( *it );
    return lineEdit ? lineEdit->text() : QString::null;
}


void KToolBar::insertComboItem (int id, const QString& text, int index)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    QComboBox * comboBox = dynamic_cast<QComboBox *>( *it );
    QComboBox * comboBox = (QComboBox *)( *it );
    if (comboBox)
        comboBox->insertItem( text, index );
}

void KToolBar::insertComboList (int id, const QStringList &list, int index)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    QComboBox * comboBox = dynamic_cast<QComboBox *>( *it );
    QComboBox * comboBox = (QComboBox *)( *it );
    if (comboBox)
        comboBox->insertStringList( list, index );
}


void KToolBar::removeComboItem (int id, int index)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    QComboBox * comboBox = dynamic_cast<QComboBox *>( *it );
    QComboBox * comboBox = (QComboBox *)( *it );
    if (comboBox)
        comboBox->removeItem( index );
}


void KToolBar::setCurrentComboItem (int id, int index)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    QComboBox * comboBox = dynamic_cast<QComboBox *>( *it );
    QComboBox * comboBox = (QComboBox *)( *it );
    if (comboBox)
        comboBox->setCurrentItem( index );
}


void KToolBar::changeComboItem  (int id, const QString& text, int index)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    QComboBox * comboBox = dynamic_cast<QComboBox *>( *it );
    QComboBox * comboBox = (QComboBox *)( *it );
    if (comboBox)
        comboBox->changeItem( text, index );
}


void KToolBar::clearCombo (int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    QComboBox * comboBox = dynamic_cast<QComboBox *>( *it );
    QComboBox * comboBox = (QComboBox *)( *it );
    if (comboBox)
        comboBox->clear();
}


QString KToolBar::getComboItem (int id, int index) const
{
    Id2WidgetMap::ConstIterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return QString::null;
//US    QComboBox * comboBox = dynamic_cast<QComboBox *>( *it );
    QComboBox * comboBox = (QComboBox *)( *it );
    return comboBox ? comboBox->text( index ) : QString::null;
}


KComboBox * KToolBar::getCombo(int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return 0;
//US    return dynamic_cast<KComboBox *>( *it );
    return (KComboBox *)( *it );
}


KLineEdit * KToolBar::getLined (int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return 0;
//US    return dynamic_cast<KLineEdit *>( *it );
    return (KLineEdit *)( *it );
}


KToolBarButton * KToolBar::getButton (int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return 0;
//US    return dynamic_cast<KToolBarButton *>( *it );
    return (KToolBarButton *)( *it );
}


void KToolBar::alignItemRight (int id, bool right )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
    if ( rightAligned && !right && (*it) == rightAligned )
        rightAligned = 0;
    if ( (*it) && right )
        rightAligned = (*it);
}


QWidget *KToolBar::getWidget (int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    return ( it == id2widget.end() ) ? 0 : (*it);
}


void KToolBar::setItemAutoSized (int id, bool yes )
{
    QWidget *w = getWidget(id);
    if ( w && yes )
        setStretchableWidget( w );
}


void KToolBar::clear ()
{
    QToolBar::clear();
    widget2id.clear();
    id2widget.clear();
}


void KToolBar::removeItem(int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
    {
        kdDebug(220) << "KToolBar::removeItem item " << id << " not found" << endl;
        return;
    }
    QWidget * w = (*it);
    id2widget.remove( id );
    widget2id.remove( w );
    widgets.removeRef( w );
    delete w;
}


void KToolBar::removeItemDelayed(int id)
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
    {
        kdDebug(220) << "KToolBar::removeItem item " << id << " not found" << endl;
        return;
    }
    QWidget * w = (*it);
    id2widget.remove( id );
    widget2id.remove( w );
    widgets.removeRef( w );

    w->blockSignals(true);
    d->idleButtons.append(w);
    layoutTimer->start( 50, TRUE );
}


void KToolBar::hideItem (int id)
{
    QWidget *w = getWidget(id);
    if ( w )
        w->hide();
}


void KToolBar::showItem (int id)
{
    QWidget *w = getWidget(id);
    if ( w )
        w->show();
}


int KToolBar::itemIndex (int id)
{
    QWidget *w = getWidget(id);
    return w ? widgets.findRef(w) : -1;
}


void KToolBar::setFullSize(bool flag )
{
    setHorizontalStretchable( flag );
    setVerticalStretchable( flag );
}


bool KToolBar::fullSize() const
{
    return isHorizontalStretchable() || isVerticalStretchable();
}


void KToolBar::enableMoving(bool flag )
{
//US    setMovingEnabled(flag);
  this->mainWindow()->setToolBarsMovable(flag);    
}


void KToolBar::setBarPos (BarPosition bpos)
{
    if ( !mainWindow() )
        return;
//US    mainWindow()->moveDockWindow( this, (Dock)bpos );
    mainWindow()->moveToolBar( this, (QMainWindow::ToolBarDock)bpos );
}


const KToolBar::BarPosition KToolBar::barPos()
{
    if ( !(QMainWindow*)mainWindow() )
        return KToolBar::Top;
//US    Dock dock;
    QMainWindow::ToolBarDock dock;
    int dm1, dm2;
    bool dm3;
    ((QMainWindow*)mainWindow())->getLocation( (QToolBar*)this, dock, dm1, dm3, dm2 );
//US    if ( dock == DockUnmanaged ) {
    if ( dock == QMainWindow::Unmanaged ) {
        return (KToolBar::BarPosition)Top;
    }
    return (BarPosition)dock;
}


bool KToolBar::enable(BarStatus stat)
{
    bool mystat = isVisible();

    if ( (stat == Toggle && mystat) || stat == Hide )
        hide();
    else
        show();

    return isVisible() == mystat;
}


void KToolBar::setMaxHeight ( int h )
{
    setMaximumHeight( h );
}

int KToolBar::maxHeight()
{
    return maximumHeight();
}


void KToolBar::setMaxWidth (int dw)
{
    setMaximumWidth( dw );
}


int KToolBar::maxWidth()
{
    return maximumWidth();
}


void KToolBar::setTitle (const QString& _title)
{
    setLabel( _title );
}


void KToolBar::enableFloating (bool )
{
}


void KToolBar::setIconText(IconText it)
{
    setIconText( it, true );
}


void KToolBar::setIconText(IconText icontext, bool update)
{
    bool doUpdate=false;

    if (icontext != d->m_iconText) {
        d->m_iconText = icontext;
        doUpdate=true;
    }

    if (update == false)
        return;

    if (doUpdate)
        emit modechange(); // tell buttons what happened

    // ugly hack to force a QMainWindow::triggerLayout( TRUE )
    if ( mainWindow() ) {
        QMainWindow *mw = mainWindow();
        mw->setUpdatesEnabled( FALSE );
        mw->setToolBarsMovable( !mw->toolBarsMovable() );
        mw->setToolBarsMovable( !mw->toolBarsMovable() );
        mw->setUpdatesEnabled( TRUE );
    }
}


KToolBar::IconText KToolBar::iconText() const
{
    return d->m_iconText;
}


void KToolBar::setIconSize(int size)
{
    setIconSize( size, true );
}

void KToolBar::setIconSize(int size, bool update)
{
    bool doUpdate=false;

    if ( size != d->m_iconSize ) {
            d->m_iconSize = size;
            doUpdate=true;
    }

    if (update == false)
        return;

    if (doUpdate)
        emit modechange(); // tell buttons what happened

    // ugly hack to force a QMainWindow::triggerLayout( TRUE )
    if ( mainWindow() ) {
        QMainWindow *mw = mainWindow();
        mw->setUpdatesEnabled( FALSE );
        mw->setToolBarsMovable( !mw->toolBarsMovable() );
        mw->setToolBarsMovable( !mw->toolBarsMovable() );
        mw->setUpdatesEnabled( TRUE );
    }
}


int KToolBar::iconSize() const
{
/*US
    if ( !d->m_iconSize ) // default value?
    {
         if (!::qstrcmp(QObject::name(), "mainToolBar"))
             return KGlobal::iconLoader()->currentSize(KIcon::MainToolbar);
         else
             return KGlobal::iconLoader()->currentSize(KIcon::Toolbar);
    }
    return d->m_iconSize;
*/
    int ret = 18;
    if ( QApplication::desktop()->width() > 320   )
        ret = 30;
   return ret;
}


void KToolBar::setEnableContextMenu(bool enable )
{
    d->m_enableContext = enable;
}


bool KToolBar::contextMenuEnabled() const
{
    return d->m_enableContext;
}


void KToolBar::setItemNoStyle(int id, bool no_style )
{
    Id2WidgetMap::Iterator it = id2widget.find( id );
    if ( it == id2widget.end() )
        return;
//US    KToolBarButton * button = dynamic_cast<KToolBarButton *>( *it );
    KToolBarButton * button = (KToolBarButton *)( *it );
    if (button)
        button->setNoStyle( no_style );
}


void KToolBar::setFlat (bool flag)
{
    if ( !mainWindow() )
        return;
    if ( flag )
//US        mainWindow()->moveDockWindow( this, DockMinimized );
        mainWindow()->moveToolBar( this, QMainWindow::Minimized );
    else
//US        mainWindow()->moveDockWindow( this, DockTop );
        mainWindow()->moveToolBar( this, QMainWindow::Top );
    // And remember to save the new look later
/*US    
    if ( mainWindow()->inherits( "KMainWindow" ) )
        static_cast<KMainWindow *>(mainWindow())->setSettingsDirty();
*/        
}


int KToolBar::count() const
{
    return id2widget.count();
}


void KToolBar::saveState()
{
/*US
    // first, try to save to the xml file
    if ( d->m_xmlguiClient && !d->m_xmlguiClient->xmlFile().isEmpty() ) {
        // go down one level to get to the right tags
        QDomElement elem = d->m_xmlguiClient->domDocument().documentElement().toElement();
        elem = elem.firstChild().toElement();
        QString barname(!::qstrcmp(name(), "unnamed") ? "mainToolBar" : name());
        QDomElement current;
        // now try to find our toolbar
        d->modified = false;
        for( ; !elem.isNull(); elem = elem.nextSibling().toElement() ) {
            current = elem;

            if ( current.tagName().lower() != "toolbar" )
                continue;

            QString curname(current.attribute( "name" ));

            if ( curname == barname ) {
                saveState( current );
                break;
            }
        }
        // if we didn't make changes, then just return
        if ( !d->modified )
            return;

        // now we load in the (non-merged) local file
        QString local_xml(KXMLGUIFactory::readConfigFile(d->m_xmlguiClient->xmlFile(), true, d->m_xmlguiClient->instance()));
        QDomDocument local;
        local.setContent(local_xml);

        // make sure we don't append if this toolbar already exists locally
        bool just_append = true;
        elem = local.documentElement().toElement();
        KXMLGUIFactory::removeDOMComments( elem );
        elem = elem.firstChild().toElement();
        for( ; !elem.isNull(); elem = elem.nextSibling().toElement() ) {
            if ( elem.tagName().lower() != "toolbar" )
                continue;

            QString curname(elem.attribute( "name" ));

            if ( curname == barname ) {
                just_append = false;
                local.documentElement().replaceChild( current, elem );
                break;
            }
        }

        if (just_append)
            local.documentElement().appendChild( current );

        KXMLGUIFactory::saveConfigFile(local, d->m_xmlguiClient->localXMLFile(), d->m_xmlguiClient->instance() );

        return;
    }
*/
    // if that didn't work, we save to the config file
    KConfig *config = KGlobal::config();
    saveSettings(config, QString::null);
    config->sync();
}

QString KToolBar::settingsGroup()
{
    QString configGroup;
    if (!::qstrcmp(name(), "unnamed") || !::qstrcmp(name(), "mainToolBar"))
        configGroup = "Toolbar style";
    else
        configGroup = QString(name()) + " Toolbar style";
    if ( this->mainWindow() )
    {
        configGroup.prepend(" ");
        configGroup.prepend( this->mainWindow()->name() );
    }
    return configGroup;
}

void KToolBar::saveSettings(KConfig *config, const QString &_configGroup)
{
    return;
    QString configGroup = _configGroup;
    if (configGroup.isEmpty())
        configGroup = settingsGroup();
    //kdDebug(220) << "KToolBar::saveSettings group=" << _configGroup << " -> " << configGroup << endl;

    QString position, icontext;
    int index;
    getAttributes( position, icontext, index );

    //kdDebug(220) << "KToolBar::saveSettings " << name() << " newLine=" << newLine << endl;

    KConfigGroupSaver saver(config, configGroup);

    if ( position != d->PositionDefault )
      config->writeEntry("Position", position);
    else
      config->deleteEntry("Position");

    if ( icontext != d->IconTextDefault )
      config->writeEntry("IconText", icontext);
    else
      config->deleteEntry("IconText");

    if ( iconSize() != d->IconSizeDefault )
      config->writeEntry("IconSize", iconSize());
    else
      config->deleteEntry("IconSize");

    if ( isHidden() != d->HiddenDefault )
      config->writeEntry("Hidden", isHidden());
    else
      config->deleteEntry("Hidden");

    if ( index != d->IndexDefault )
      config->writeEntry( "Index", index );
    else
      config->deleteEntry("Index");
//US the older version of KDE (used on the Zaurus) has no Offset property
/*    if ( offset() != d->OffsetDefault )
      config->writeEntry( "Offset", offset() );
    else
*/    
      config->deleteEntry("Offset");
    
//US the older version of KDE (used on the Zaurus) has no NewLine property
/*    
    if ( newLine() != d->NewLineDefault )
      config->writeEntry( "NewLine", newLine() );
    else
*/    
      config->deleteEntry("NewLine");
}

void KToolBar::setXMLGUIClient( KXMLGUIClient *client )
{
    d->m_xmlguiClient = client;
}

void KToolBar::setText( const QString & txt )
{
//US    setLabel( txt + " ( " + kapp->caption() + " ) " );
    setLabel( txt + " ( " + KGlobal::getAppName() + " ) " );
}


QString KToolBar::text() const
{
    return label();
}


void KToolBar::doConnections( KToolBarButton *button )
{
    connect(button, SIGNAL(clicked(int)), this, SIGNAL( clicked( int ) ) );
    connect(button, SIGNAL(doubleClicked(int)), this, SIGNAL( doubleClicked( int ) ) );
    connect(button, SIGNAL(released(int)), this, SIGNAL( released( int ) ) );
    connect(button, SIGNAL(pressed(int)), this, SIGNAL( pressed( int ) ) );
    connect(button, SIGNAL(toggled(int)), this, SIGNAL( toggled( int ) ) );
    connect(button, SIGNAL(highlighted(int, bool)), this, SIGNAL( highlighted( int, bool ) ) );
}

void KToolBar::mousePressEvent ( QMouseEvent *m )
{
    if ( !mainWindow() )
        return;
    QMainWindow *mw = mainWindow();
    if ( mw->toolBarsMovable() && d->m_enableContext ) {
        if ( m->button() == RightButton ) {
            int i = contextMenu()->exec( m->globalPos(), 0 );
            switch ( i ) {
            case -1:
                return; // popup cancelled
            case CONTEXT_LEFT:
//US                mw->moveDockWindow( this, DockLeft );
                mw->moveToolBar( this, QMainWindow::Left );
                break;
            case CONTEXT_RIGHT:
//US                mw->moveDockWindow( this, DockRight );
                mw->moveToolBar( this, QMainWindow::Right );
                break;
            case CONTEXT_TOP:
//US                mw->moveDockWindow( this, DockTop );
                mw->moveToolBar( this, QMainWindow::Top );
                break;
            case CONTEXT_BOTTOM:
//US                mw->moveDockWindow( this, DockBottom );
                mw->moveToolBar( this, QMainWindow::Bottom );
                break;
            case CONTEXT_FLOAT:
                break;
            case CONTEXT_FLAT:
//US                mw->moveDockWindow( this, DockMinimized );
                mw->moveToolBar( this, QMainWindow::Minimized );
                break;
            case CONTEXT_ICONS:
                setIconText( IconOnly );
                break;
            case CONTEXT_TEXTRIGHT:
                setIconText( IconTextRight );
                break;
            case CONTEXT_TEXT:
                setIconText( TextOnly );
                break;
            case CONTEXT_TEXTUNDER:
                setIconText( IconTextBottom );
                break;
            default:
                if ( i >= CONTEXT_ICONSIZES )
                    setIconSize( i - CONTEXT_ICONSIZES );
                else
                    return; // assume this was an action handled elsewhere, no need for setSettingsDirty()
            }
/*US            
            if ( mw->inherits("KMainWindow") )
                static_cast<KMainWindow *>(mw)->setSettingsDirty();
*/                
        }
    }
}


void KToolBar::rebuildLayout()
{
 
    for(QWidget *w=d->idleButtons.first(); w; w=d->idleButtons.next())
       w->blockSignals(false);
    d->idleButtons.clear();

    layoutTimer->stop();
    QApplication::sendPostedEvents( this, QEvent::ChildInserted );
    QBoxLayout *l = boxLayout();
    l->setMargin( 1 );
    // clear the old layout
    QLayoutIterator it = l->iterator();
    
    while ( it.current() ) {
        it.deleteCurrent();
    }
        for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
        if ( w == rightAligned ) {
            continue;
        }
        if ( w->inherits( "KToolBarSeparator" ) &&
             !( (KToolBarSeparator*)w )->showLine() ) {
            l->addSpacing( 6 );
            w->hide();
            continue;
        }
        if ( w->inherits( "QPopupMenu" ) )
            continue;
        l->addWidget( w );
        w->show();
    }
    if ( rightAligned ) {
        l->addStretch();
        l->addWidget( rightAligned );
        rightAligned->show();
    }

    if ( fullSize() ) {
        // This code sucks. It makes the last combo in a toolbar VERY big (e.g. zoom combo in kword).
        //if ( !stretchableWidget && widgets.last() &&
        //     !widgets.last()->inherits( "QButton" ) && !widgets.last()->inherits( "KAnimWidget" ) )
        //    setStretchableWidget( widgets.last() );
        if ( !rightAligned )
            l->addStretch();
        if ( stretchableWidget )
            l->setStretchFactor( stretchableWidget, 10 );
    }
    l->invalidate();
    QApplication::postEvent( this, new QEvent( QEvent::LayoutHint ) );
    //#endif //DESKTOP_VERSION
}

void KToolBar::childEvent( QChildEvent *e )
{

    if ( e->child()->isWidgetType() ) {
        QWidget * w = (QWidget*)e->child();
        if ( e->type() == QEvent::ChildInserted ) {
            if ( !e->child()->inherits( "QPopupMenu" ) &&
                 ::qstrcmp( "qt_dockwidget_internal", e->child()->name() ) != 0 ) {

                // prevent items that have been explicitly inserted by insert*() from
                // being inserted again
                if ( !widget2id.contains( w ) )
                {
                    int dummy = -1;
                    insertWidgetInternal( w, dummy, -1 );
                }
            }
        } else {
            removeWidgetInternal( w );
        }
        if ( isVisibleTo( 0 ) )
        {
            QBoxLayout *l = boxLayout();
            // QLayout *l = layout();

            // clear the old layout so that we don't get unnecassery layout
            // changes till we have rebuild the thing
            QLayoutIterator it = l->iterator();
            while ( it.current() ) {
               it.deleteCurrent();
            }
            layoutTimer->start( 50, TRUE );
        }
    }
    QToolBar::childEvent( e );
}

void KToolBar::insertWidgetInternal( QWidget *w, int &index, int id )
{
    // we can't have it in widgets, or something is really wrong
    //widgets.removeRef( w );

    connect( w, SIGNAL( destroyed() ),
             this, SLOT( widgetDestroyed() ) );
    if ( index == -1 || index > (int)widgets.count() ) {
        widgets.append( w );
        index = (int)widgets.count();
    }
    else
        widgets.insert( index, w );
    if ( id == -1 )
        id = id2widget.count();
    id2widget.insert( id, w );
    widget2id.insert( w, id );
}
void KToolBar::repaintMe()
{
    setUpdatesEnabled( true );
    QToolBar::repaint( true );
    qDebug(" KToolBar::repaintMe() ");
}

void KToolBar::showEvent( QShowEvent *e )
{
    rebuildLayout();
    QToolBar::showEvent( e );
}

void KToolBar::setStretchableWidget( QWidget *w )
{
    QToolBar::setStretchableWidget( w );
    stretchableWidget = w;
}

QSizePolicy KToolBar::sizePolicy() const
{
    if ( orientation() == Horizontal )
        return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    else
        return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
}

QSize KToolBar::sizeHint() const
{  
    QSize sh = QToolBar::sizeHint();
    //qDebug("%x KToolBar::sizeHint() %d %d ",this, QToolBar::sizeHint().width(),QToolBar::sizeHint().height() );
    if ( sh.height() <= 20 || sh.width() < 60 )
        return QSize(  sizeHintW, sizeHintH );
    KToolBar* ttt =  (KToolBar*) this;
    ttt->sizeHintW = sh.width();
    ttt->sizeHintH = sh.height();
    return sh;
    //return QToolBar::sizeHint();
#if 0
    QWidget::polish();
    static int iii = 0;
    ++iii;
    qDebug("++++++++ KToolBar::sizeHint() %d ", iii );
    int margin = static_cast<QWidget*>(ncThis)->layout()->margin();
    switch( barPos() )
    {
     case KToolBar::Top:
     case KToolBar::Bottom:
       for ( QWidget *w = widgets.first(); w; w =widgets.next() )
       {
          if ( w->inherits( "KToolBarSeparator" ) &&
             !( static_cast<KToolBarSeparator*>(w)->showLine() ) )
          {
             minSize += QSize(6, 0);
          }
          else
          {
             QSize sh = w->sizeHint();
             if (!sh.isValid())
                sh = w->minimumSize();
             minSize = minSize.expandedTo(QSize(0, sh.height()));
             minSize += QSize(sh.width()+1, 0);
          }
       }
/*US       
       minSize += QSize(QApplication::style().pixelMetric( QStyle::PM_DockWindowHandleExtent ), 0);
*/       
       minSize += QSize(margin*2, margin*2);
       break;

     case KToolBar::Left:
     case KToolBar::Right:
       for ( QWidget *w = widgets.first(); w; w = widgets.next() )
       {
          if ( w->inherits( "KToolBarSeparator" ) &&
             !( static_cast<KToolBarSeparator*>(w)->showLine() ) )
          {
             minSize += QSize(0, 6);
          }
          else
          {
             QSize sh = w->sizeHint();
             if (!sh.isValid())
                sh = w->minimumSize();
             minSize = minSize.expandedTo(QSize(sh.width(), 0));
             minSize += QSize(0, sh.height()+1);
          }
       }
/*US       
       minSize += QSize(0, QApplication::style().pixelMetric( QStyle::PM_DockWindowHandleExtent ));
*/       
       minSize += QSize(margin*2, margin*2);
       break;

     default:
       minSize = QToolBar::sizeHint();
       break;
    }
    return minSize;
#endif
}

QSize KToolBar::minimumSize() const
{
    return minimumSizeHint();
}

QSize KToolBar::minimumSizeHint() const
{
    return sizeHint();
}

bool KToolBar::highlight() const
{
    return d->m_highlight;
}

void KToolBar::hide()
{
    QToolBar::hide();
}

void KToolBar::show()
{
    QToolBar::show();
}

void KToolBar::resizeEvent( QResizeEvent *e )
{
    bool b = isUpdatesEnabled();
    setUpdatesEnabled( FALSE );
    QToolBar::resizeEvent( e );
    if (b)
       d->repaintTimer.start( 100, true );
}

void KToolBar::slotIconChanged(int group)
{
    if ((group != KIcon::Toolbar) && (group != KIcon::MainToolbar))
        return;
    if ((group == KIcon::MainToolbar) != !::qstrcmp(name(), "mainToolBar"))
        return;

    emit modechange();
    if (isVisible())
        updateGeometry();
}

void KToolBar::slotReadConfig()
{
    //kdDebug(220) << "KToolBar::slotReadConfig" << endl;
    // Read appearance settings (hmm, we used to do both here,
    // but a well behaved application will call applyMainWindowSettings
    // anyway, right ?)
    applyAppearanceSettings(KGlobal::config(), QString::null );
}

void KToolBar::slotAppearanceChanged()
{
    // Read appearance settings from global file.
    applyAppearanceSettings(KGlobal::config(), QString::null, true /* lose local settings */ );
    // And remember to save the new look later
/*US    
    if ( mainWindow() && mainWindow()->inherits( "KMainWindow" ) )
        static_cast<KMainWindow *>(mainWindow())->setSettingsDirty();
*/        
}

//static
bool KToolBar::highlightSetting()
{
    QString grpToolbar(QString::fromLatin1("Toolbar style"));
    KConfigGroupSaver saver(KGlobal::config(), grpToolbar);
    return KGlobal::config()->readBoolEntry(QString::fromLatin1("Highlighting"),true);
}

//static
bool KToolBar::transparentSetting()
{
    QString grpToolbar(QString::fromLatin1("Toolbar style"));
    KConfigGroupSaver saver(KGlobal::config(), grpToolbar);
    return KGlobal::config()->readBoolEntry(QString::fromLatin1("TransparentMoving"),true);
}

//static
KToolBar::IconText KToolBar::iconTextSetting()
{
    QString grpToolbar(QString::fromLatin1("Toolbar style"));
    KConfigGroupSaver saver(KGlobal::config(), grpToolbar);
    QString icontext = KGlobal::config()->readEntry(QString::fromLatin1("IconText"),QString::fromLatin1("IconOnly"));
    if ( icontext == "IconTextRight" )
        return IconTextRight;
    else if ( icontext == "IconTextBottom" )
        return IconTextBottom;
    else if ( icontext == "TextOnly" )
        return TextOnly;
    else
        return IconOnly;
}

void KToolBar::applyAppearanceSettings(KConfig *config, const QString &_configGroup, bool forceGlobal)
{
    return;
    QString configGroup = _configGroup.isEmpty() ? settingsGroup() : _configGroup;
    //kdDebug(220) << "KToolBar::applyAppearanceSettings: configGroup=" << configGroup << endl;
    // We have application-specific settings in the XML file,
    // and nothing in the application's config file
    // -> don't apply the global defaults, the XML ones are preferred
    // See applySettings for a full explanation
/*US :we do not support xml files    
    if ( d->m_xmlguiClient && !d->m_xmlguiClient->xmlFile().isEmpty() &&
           !config->hasGroup(configGroup) )
    {
        //kdDebug(220) << "skipping global defaults, using XML ones instead" << endl;
        return;
    }
*/
    if ( !config->hasGroup(configGroup) )
    {
        //kdDebug(220) << "skipping global defaults, using XML ones instead" << endl;
        return;
    }


    KConfig *gconfig = KGlobal::config();
/*US
    static const QString &attrIconText  = KGlobal::staticQString("IconText");
    static const QString &attrHighlight = KGlobal::staticQString("Highlighting");
    static const QString &attrTrans     = KGlobal::staticQString("TransparentMoving");
    static const QString &attrSize      = KGlobal::staticQString("IconSize");
*/
    // we actually do this in two steps.
    // First, we read in the global styles [Toolbar style] (from the KControl module).
    // Then, if the toolbar is NOT 'mainToolBar', we will also try to read in [barname Toolbar style]
    bool highlight;
    int transparent;
    QString icontext;
    int iconsize = 0;

    // this is the first iteration
    QString grpToolbar(QString::fromLatin1("Toolbar style"));
    { // start block for KConfigGroupSaver
        KConfigGroupSaver saver(gconfig, grpToolbar);

        // first, get the generic settings
//US        highlight   = gconfig->readBoolEntry(attrHighlight, true);
        highlight   = gconfig->readBoolEntry("Highlighting", true);
//US        transparent = gconfig->readBoolEntry(attrTrans, true);
        transparent = gconfig->readBoolEntry("TransparentMoving", true);

        // we read in the IconText property *only* if we intend on actually
        // honoring it
        if (d->m_honorStyle)
//US            d->IconTextDefault = gconfig->readEntry(attrIconText, d->IconTextDefault);
            d->IconTextDefault = gconfig->readEntry("IconText", d->IconTextDefault);
        else
            d->IconTextDefault = "IconOnly";

        // Use the default icon size for toolbar icons.
//US        d->IconSizeDefault = gconfig->readNumEntry(attrSize, d->IconSizeDefault);
        d->IconSizeDefault = gconfig->readNumEntry("IconSize", d->IconSizeDefault);

        if ( !forceGlobal && config->hasGroup(configGroup) )
        {
            config->setGroup(configGroup);

            // first, get the generic settings
//US            highlight   = config->readBoolEntry(attrHighlight, highlight);
            highlight   = config->readBoolEntry("Highlighting", highlight);
//US            transparent = config->readBoolEntry(attrTrans, transparent);
            transparent = config->readBoolEntry("TransparentMoving", transparent);
            // now we always read in the IconText property
//US            icontext = config->readEntry(attrIconText, d->IconTextDefault);
            icontext = config->readEntry("IconText", d->IconTextDefault);

            // now get the size
//US            iconsize = config->readNumEntry(attrSize, d->IconSizeDefault);
            iconsize = config->readNumEntry("IconSize", d->IconSizeDefault);
        }
        else
        {
            iconsize = d->IconSizeDefault;
            icontext = d->IconTextDefault;
        }

        // revert back to the old group
    } // end block for KConfigGroupSaver

    bool doUpdate = false;

    IconText icon_text;
    if ( icontext == "IconTextRight" )
        icon_text = IconTextRight;
    else if ( icontext == "IconTextBottom" )
        icon_text = IconTextBottom;
    else if ( icontext == "TextOnly" )
        icon_text = TextOnly;
    else
        icon_text = IconOnly;

    // check if the icon/text has changed
    if (icon_text != d->m_iconText) {
        //kdDebug(220) << "KToolBar::applyAppearanceSettings setIconText " << icon_text << endl;
        setIconText(icon_text, false);
        doUpdate = true;
    }

    // ...and check if the icon size has changed
    if (iconsize != d->m_iconSize) {
        setIconSize(iconsize, false);
        doUpdate = true;
    }

    QMainWindow *mw = mainWindow();

    // ...and if we should highlight
    if ( highlight != d->m_highlight ) {
        d->m_highlight = highlight;
        doUpdate = true;
    }

    // ...and if we should move transparently
    if ( mw && transparent != (!mw->opaqueMoving()) ) {
        mw->setOpaqueMoving( !transparent );
    }

    if (doUpdate)
        emit modechange(); // tell buttons what happened
    if (isVisible ())
        updateGeometry();
}

void KToolBar::applySettings(KConfig *config, const QString &_configGroup)
{
    //kdDebug(220) << "KToolBar::applySettings group=" << _configGroup << endl;

    QString configGroup = _configGroup.isEmpty() ? settingsGroup() : _configGroup;

    /*
      Let's explain this a bit more in details.
      The order in which we apply settings is :
       Global config / <appnamerc> user settings                        if no XMLGUI is used
       Global config / App-XML attributes / <appnamerc> user settings   if XMLGUI is used

      So in the first case, we simply read everything from KConfig as below,
      but in the second case we don't do anything here if there is no app-specific config,
      and the XMLGUI uses the static methods of this class to get the global defaults.

      Global config doesn't include position (index, offset, newline and hidden/shown).
    */

    // First the appearance stuff - the one which has a global config
    applyAppearanceSettings( config, _configGroup );

    // ...and now the position stuff
    if ( config->hasGroup(configGroup) )
    {
        KConfigGroupSaver cgs(config, configGroup);
/*US
        static const QString &attrPosition  = KGlobal::staticQString("Position");
        static const QString &attrIndex  = KGlobal::staticQString("Index");
        static const QString &attrOffset  = KGlobal::staticQString("Offset");
        static const QString &attrNewLine  = KGlobal::staticQString("NewLine");
        static const QString &attrHidden  = KGlobal::staticQString("Hidden");
        
        QString position = config->readEntry(attrPosition, d->PositionDefault);
        int index = config->readNumEntry(attrIndex, d->IndexDefault);
        int offset = config->readNumEntry(attrOffset, d->OffsetDefault);
        bool newLine = config->readBoolEntry(attrNewLine, d->NewLineDefault);
        bool hidden = config->readBoolEntry(attrHidden, d->HiddenDefault);
*/

        QString position = config->readEntry("Position", d->PositionDefault);
        int index = config->readNumEntry("Index", d->IndexDefault);
        int offset = config->readNumEntry("Offset", d->OffsetDefault);
        bool newLine = config->readBoolEntry("NewLine", d->NewLineDefault);
        bool hidden = config->readBoolEntry("Hidden", d->HiddenDefault);

/*US        Dock pos(DockTop);
        if ( position == "Top" )
            pos = DockTop;
        else if ( position == "Bottom" )
            pos = DockBottom;
        else if ( position == "Left" )
            pos = DockLeft;
        else if ( position == "Right" )
            pos = DockRight;
        else if ( position == "Floating" )
            pos = DockTornOff;
        else if ( position == "Flat" )
            pos = DockMinimized;
*/
        QMainWindow::ToolBarDock pos(QMainWindow::Top);
        if ( position == "Top" )
            pos = QMainWindow::Top;
        else if ( position == "Bottom" )
            pos = QMainWindow::Bottom;
        else if ( position == "Left" )
            pos = QMainWindow::Left;
        else if ( position == "Right" )
            pos = QMainWindow::Right;
        else if ( position == "Floating" )
            pos = QMainWindow::TornOff;
        else if ( position == "Flat" )
            pos = QMainWindow::Minimized;
            
        //kdDebug(220) << "KToolBar::applySettings hidden=" << hidden << endl;
        if (hidden)
            hide();
        else
            show();

        if ( mainWindow() )
        {
            QMainWindow *mw = mainWindow();

            //kdDebug(220) << "KToolBar::applySettings updating ToolbarInfo" << endl;
            d->toolBarInfo = KToolBarPrivate::ToolBarInfo( pos, index, newLine, offset );

            // moveDockWindow calls QDockArea which does a reparent() on us with
            // showIt = true, so we loose our visibility status
            bool doHide = isHidden();

//US            mw->moveDockWindow( this, pos, newLine, index, offset );
            mw->moveToolBar( this, pos, newLine, index, offset );
      
            //kdDebug(220) << "KToolBar::applySettings " << name() << " moveDockWindow with pos=" << pos << " newLine=" << newLine << " idx=" << index << " offs=" << offset << endl;
            if ( doHide )
                hide();
        }
        if (isVisible ())
            updateGeometry();
    }
}

bool KToolBar::event( QEvent *e )
{
    if ( (e->type() == QEvent::LayoutHint) && isUpdatesEnabled() )
       d->repaintTimer.start( 100, true );

    if (e->type() == QEvent::ChildInserted )
    {
       // By pass QToolBar::event,
       // it will show() the inserted child and we don't want to
       // do that until we have rebuild the layout.
       childEvent((QChildEvent *)e);
       return true;
    }

    return QToolBar::event( e );
}

void KToolBar::slotRepaint()
{
    setUpdatesEnabled( FALSE );
    // Send a resizeEvent to update the "toolbar extension arrow"
    // (The button you get when your toolbar-items don't fit in
    // the available space)
    QResizeEvent ev(size(), size());
    resizeEvent(&ev);
    //#ifdef DESKTOP_VERSION
    QApplication::sendPostedEvents( this, QEvent::LayoutHint );
    //#endif //DESKTOP_VERSION   
    setUpdatesEnabled( TRUE );
    repaint( TRUE );
}

void KToolBar::toolBarPosChanged( QToolBar *tb )
{
    if ( tb != this )
        return;
//US    if ( d->oldPos == DockMinimized )
    if ( d->oldPos == QMainWindow::Minimized )
        rebuildLayout();
    d->oldPos = (QMainWindow::ToolBarDock)barPos();
/*US    
    if ( mainWindow() && mainWindow()->inherits( "KMainWindow" ) )
        static_cast<KMainWindow *>(mainWindow())->setSettingsDirty();
*/        
}

/*US
void KToolBar::loadState( const QDomElement &element )
{
    //kdDebug(220) << "KToolBar::loadState " << this << endl;
    if ( !mainWindow() )
        return;

    {
        QCString text = element.namedItem( "text" ).toElement().text().utf8();
        if ( text.isEmpty() )
            text = element.namedItem( "Text" ).toElement().text().utf8();
        if ( !text.isEmpty() )
            setText( i18n( text ) );
    }

    {
        QCString attrFullWidth = element.attribute( "fullWidth" ).lower().latin1();
        if ( !attrFullWidth.isEmpty() )
            setFullSize( attrFullWidth == "true" );
    }

    Dock dock = DockTop;
    {
        QCString attrPosition = element.attribute( "position" ).lower().latin1();
        //kdDebug(220) << "KToolBar::loadState attrPosition=" << attrPosition << endl;
        if ( !attrPosition.isEmpty() ) {
            if ( attrPosition == "top" )
                dock = DockTop;
            else if ( attrPosition == "left" )
                dock = DockLeft;
            else if ( attrPosition == "right" )
                dock = DockRight;
            else if ( attrPosition == "bottom" )
                dock = DockBottom;
            else if ( attrPosition == "floating" )
                dock = DockTornOff;
            else if ( attrPosition == "flat" )
                dock = DockMinimized;
        }
    }

    {
        QCString attrIconText = element.attribute( "iconText" ).lower().latin1();
        if ( !attrIconText.isEmpty() ) {
            //kdDebug(220) << "KToolBar::loadState attrIconText=" << attrIconText << endl;
            if ( attrIconText == "icontextright" )
                setIconText( KToolBar::IconTextRight );
            else if ( attrIconText == "textonly" )
                setIconText( KToolBar::TextOnly );
            else if ( attrIconText == "icontextbottom" )
                setIconText( KToolBar::IconTextBottom );
            else if ( attrIconText == "icononly" )
                setIconText( KToolBar::IconOnly );
        } else
            // Use global setting
            setIconText( iconTextSetting() );
    }

    {
        QString attrIconSize = element.attribute( "iconSize" ).lower();
        if ( !attrIconSize.isEmpty() )
            d->IconSizeDefault = attrIconSize.toInt();
        setIconSize( d->IconSizeDefault );
    }

    {
        QString attrIndex = element.attribute( "index" ).lower();
        if ( !attrIndex.isEmpty() )
            d->IndexDefault = attrIndex.toInt();
    }

    {
        QString attrOffset = element.attribute( "offset" ).lower();
        if ( !attrOffset.isEmpty() )
            d->OffsetDefault = attrOffset.toInt();
    }

    {
        QString attrNewLine = element.attribute( "newline" ).lower();
        if ( !attrNewLine.isEmpty() )
            d->NewLineDefault = attrNewLine == "true";
    }

    {
        QString attrHidden = element.attribute( "hidden" ).lower();
        if ( !attrHidden.isEmpty() )
            d->HiddenDefault  = attrHidden  == "true";
    }

    d->toolBarInfo = KToolBarPrivate::ToolBarInfo( dock, d->IndexDefault, d->NewLineDefault, d->OffsetDefault );
    mainWindow()->addDockWindow( this, dock, d->NewLineDefault );
//US    mainWindow()->moveDockWindow( this, dock, d->NewLineDefault, d->IndexDefault, d->OffsetDefault );
    mainWindow()->moveToolBar( this, dock, d->NewLineDefault, d->IndexDefault, d->OffsetDefault );

    // Apply the highlight button setting
    d->m_highlight = highlightSetting();

    // Apply transparent-toolbar-moving setting (ok, this is global to the mainwindow,
    // but we do it only if there are toolbars...)
    if ( transparentSetting() != !mainWindow()->opaqueMoving() )
        mainWindow()->setOpaqueMoving( !transparentSetting() );

    if ( d->HiddenDefault )
        hide();
    else
        show();

    getAttributes( d->PositionDefault, d->IconTextDefault, d->IndexDefault );
}
*/

void KToolBar::getAttributes( QString &position, QString &icontext, int &index )
{
    // get all of the stuff to save
    switch ( barPos() ) {
    case KToolBar::Flat:
        position = "Flat";
        break;
    case KToolBar::Bottom:
        position = "Bottom";
        break;
    case KToolBar::Left:
        position = "Left";
        break;
    case KToolBar::Right:
        position = "Right";
        break;
    case KToolBar::Floating:
        position = "Floating";
        break;
    case KToolBar::Top:
    default:
        position = "Top";
        break;
    }

    if ( mainWindow() ) {
        QMainWindow::ToolBarDock dock;
        bool newLine;
        int offset;
        mainWindow()->getLocation( this, dock, index, newLine, offset );
    }

    switch (d->m_iconText) {
    case KToolBar::IconTextRight:
        icontext = "IconTextRight";
        break;
    case KToolBar::IconTextBottom:
        icontext = "IconTextBottom";
        break;
    case KToolBar::TextOnly:
        icontext = "TextOnly";
        break;
    case KToolBar::IconOnly:
    default:
        icontext = "IconOnly";
        break;
    }
}
/*US
void KToolBar::saveState( QDomElement &current )
{
    QString position, icontext;
    int index = -1;
    getAttributes( position, icontext, index );

    current.setAttribute( "noMerge", "1" );
    current.setAttribute( "position", position );
    current.setAttribute( "iconText", icontext );
    current.setAttribute( "index", index );
    current.setAttribute( "offset", offset() );
    current.setAttribute( "newline", newLine() );
    if ( isHidden() )
        current.setAttribute( "hidden", "true" );
    d->modified = true;
}
*/

void KToolBar::positionYourself( bool force )
{
    if (force)
        d->positioned = false;

    if ( d->positioned || !mainWindow() )
    {
        //kdDebug(220) << "KToolBar::positionYourself d->positioned=true  ALREADY DONE" << endl;
        return;
    }
    // we can't test for ForceHide after moveDockWindow because QDockArea
    // does a reparent() with showIt == true
    bool doHide = isHidden();
    //kdDebug(220) << "positionYourself " << name() << " dock=" << d->toolBarInfo.dock << " newLine=" << d->toolBarInfo.newline << " offset=" << d->toolBarInfo.offset << endl;
/*US    mainWindow()->moveDockWindow( this, d->toolBarInfo.dock,
                               d->toolBarInfo.newline,
                               d->toolBarInfo.index,
                               d->toolBarInfo.offset );
*/                               
    mainWindow()->moveToolBar( this, d->toolBarInfo.dock, d->NewLineDefault, d->IndexDefault, d->OffsetDefault );
                               
    if ( doHide )
        hide();
    // This method can only have an effect once - unless force is set
    d->positioned = TRUE;
}

//US KPopupMenu *KToolBar::contextMenu()
QPopupMenu *KToolBar::contextMenu()
{
  if ( context )
    return context;

  // Construct our context popup menu. Name it qt_dockwidget_internal so it
  // won't be deleted by QToolBar::clear().
//US  context = new KPopupMenu( this, "qt_dockwidget_internal" );
  context = new QPopupMenu( this, "qt_dockwidget_internal" );
//US  context->insertTitle(i18n("Toolbar Menu"));

//US  KPopupMenu *orient = new KPopupMenu( context, "orient" );
  QPopupMenu *orient = new QPopupMenu( context, "orient" );
  orient->insertItem( i18n("toolbar position string","Top"),  CONTEXT_TOP );
  orient->insertItem( i18n("toolbar position string","Left"), CONTEXT_LEFT );
  orient->insertItem( i18n("toolbar position string","Right"), CONTEXT_RIGHT );
  orient->insertItem( i18n("toolbar position string","Bottom"), CONTEXT_BOTTOM );
  orient->insertSeparator(-1);
  //orient->insertItem( i18n("toolbar position string","Floating"), CONTEXT_FLOAT );
  orient->insertItem( i18n("min toolbar", "Flat"), CONTEXT_FLAT );

//US  KPopupMenu *mode = new KPopupMenu( context, "mode" );
  QPopupMenu *mode = new QPopupMenu( context, "mode" );
  mode->insertItem( i18n("Icons Only"), CONTEXT_ICONS );
  mode->insertItem( i18n("Text Only"), CONTEXT_TEXT );
  mode->insertItem( i18n("Text Alongside Icons"), CONTEXT_TEXTRIGHT );
  mode->insertItem( i18n("Text Under Icons"), CONTEXT_TEXTUNDER );

//US  KPopupMenu *size = new KPopupMenu( context, "size" );
  QPopupMenu *size = new QPopupMenu( context, "size" );
  size->insertItem( i18n("Default"), CONTEXT_ICONSIZES );
  // Query the current theme for available sizes
  QValueList<int> avSizes;
/*US  
  KIconTheme *theme = KGlobal::instance()->iconLoader()->theme();
  if (!::qstrcmp(QObject::name(), "mainToolBar"))
      avSizes = theme->querySizes( KIcon::MainToolbar);
  else
      avSizes = theme->querySizes( KIcon::Toolbar);
*/
  avSizes << 16;
  avSizes << 32;
  
  d->iconSizes = avSizes;

  QValueList<int>::Iterator it;
  for (it=avSizes.begin(); it!=avSizes.end(); it++) {
      QString text;
      if ( *it < 19 )
          text = i18n("Small (%1x%2)").arg(*it).arg(*it);
      else if (*it < 25)
          text = i18n("Medium (%1x%2)").arg(*it).arg(*it);
      else
          text = i18n("Large (%1x%2)").arg(*it).arg(*it);
      //we use the size as an id, with an offset
      size->insertItem( text, CONTEXT_ICONSIZES + *it );
  }

  context->insertItem( i18n("Orientation"), orient );
  orient->setItemChecked(CONTEXT_TOP, true);
  context->insertItem( i18n("Text Position"), mode );
  context->setItemChecked(CONTEXT_ICONS, true);
  context->insertItem( i18n("Icon Size"), size );

/*US  
  if (mainWindow()->inherits("KMainWindow"))
  {
      if ( (static_cast<KMainWindow*>(mainWindow())->toolBarMenuAction()) &&
		(static_cast<KMainWindow*>(mainWindow())->hasMenuBar()) )

      (static_cast<KMainWindow*>(mainWindow()))->toolBarMenuAction()->plug(context);
  }
*/

  connect( context, SIGNAL( aboutToShow() ), this, SLOT( slotContextAboutToShow() ) );
  return context;
}

void KToolBar::slotContextAboutToShow()
{
  if (!d->m_configurePlugged)
  {
    // try to find "configure toolbars" action

    KXMLGUIClient *xmlGuiClient = d->m_xmlguiClient;
    if ( !xmlGuiClient && mainWindow() && mainWindow()->inherits( "KMainWindow" ) )
      xmlGuiClient = (KXMLGUIClient *)mainWindow();
    if ( xmlGuiClient )
    {
        KAction *configureAction = xmlGuiClient->actionCollection()->action(KStdAction::stdName(KStdAction::ConfigureToolbars));
        if ( configureAction )
        {
          configureAction->plug(context);
          d->m_configurePlugged = true;
        }
    }
  }

  for(int i = CONTEXT_ICONS; i <= CONTEXT_TEXTUNDER; ++i)
    context->setItemChecked(i, false);

  switch( d->m_iconText )
  {
        case IconOnly:
        default:
            context->setItemChecked(CONTEXT_ICONS, true);
            break;
        case IconTextRight:
            context->setItemChecked(CONTEXT_TEXTRIGHT, true);
            break;
        case TextOnly:
            context->setItemChecked(CONTEXT_TEXT, true);
            break;
        case IconTextBottom:
            context->setItemChecked(CONTEXT_TEXTUNDER, true);
            break;
  }

  QValueList<int>::ConstIterator iIt = d->iconSizes.begin();
  QValueList<int>::ConstIterator iEnd = d->iconSizes.end();
  for (; iIt != iEnd; ++iIt )
      context->setItemChecked( CONTEXT_ICONSIZES + *iIt, false );

  context->setItemChecked( CONTEXT_ICONSIZES, false );

  context->setItemChecked( CONTEXT_ICONSIZES + d->m_iconSize, true );

  for ( int i = CONTEXT_TOP; i <= CONTEXT_FLAT; ++i )
      context->setItemChecked( i, false );

  switch ( barPos() )
  {
  case KToolBar::Flat:
      context->setItemChecked( CONTEXT_FLAT, true );
      break;
  case KToolBar::Bottom:
      context->setItemChecked( CONTEXT_BOTTOM, true );
      break;
  case KToolBar::Left:
      context->setItemChecked( CONTEXT_LEFT, true );
      break;
  case KToolBar::Right:
      context->setItemChecked( CONTEXT_RIGHT, true );
      break;
  case KToolBar::Floating:
      context->setItemChecked( CONTEXT_FLOAT, true );
      break;
  case KToolBar::Top:
      context->setItemChecked( CONTEXT_TOP, true );
      break;
  default: break;
  }
}

void KToolBar::widgetDestroyed()
{
    removeWidgetInternal( (QWidget*)sender() );
}

void KToolBar::removeWidgetInternal( QWidget * w )
{ 
    if ( inshutdownprocess )
        return;
    widgets.removeRef( w );
    QMap< QWidget*, int >::Iterator it = widget2id.find( w );
    if ( it == widget2id.end() )
        return;
    id2widget.remove( *it );
    widget2id.remove( it );
}

void KToolBar::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

//US #include "ktoolbar.moc"

