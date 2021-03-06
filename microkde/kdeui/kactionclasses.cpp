/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>
              (C) 2002 Joseph Wenninger <jowenn@kde.org>

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

#include "kactionclasses.h"

#include <assert.h>

#include <qfontdatabase.h>
#include <qobjectlist.h>
//US#include <qwhatsthis.h>
#include <qtimer.h>

//US#include <kaccel.h>
//US#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
//US#include <kfontcombo.h>
//US#include <kmainwindow.h>
//US#include <kmenubar.h>
//US#include <kpopupmenu.h>
#include <kcombobox.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>
#include <kurl.h>

//US added the following includefiles
#include <kconfigbase.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qmainwindow.h>


static QFontDatabase *fontDataBase = 0;

static void cleanupFontDatabase()
{
    delete fontDataBase;
    fontDataBase = 0;
}

static void get_fonts( QStringList &lst )
{
    if ( !fontDataBase ) {
        fontDataBase = new QFontDatabase();
        qAddPostRoutine( cleanupFontDatabase );
    }
    lst.clear();
    QStringList families = fontDataBase->families();
    for ( QStringList::Iterator it = families.begin(); it != families.end(); ++it )
    {
        QString family = *it;
        if ( family. contains('-') ) // remove foundry
            family = family.right( family.length() - family.find('-' ) - 1);
        if ( !lst.contains( family ) )
            lst.append( family );
    }
    lst.sort();
}

static QValueList<int> get_standard_font_sizes()
{
    if ( !fontDataBase ) {
        fontDataBase = new QFontDatabase();
        qAddPostRoutine( cleanupFontDatabase );
    }
    return fontDataBase->standardSizes();
}

class KToggleAction::KToggleActionPrivate
{
public:
  KToggleActionPrivate()
  {
    m_checked = false;
  }

  bool m_checked;
  QString m_exclusiveGroup;
};

KToggleAction::KToggleAction( const QString& text, const KShortcut& cut,
                              QObject* parent,
                              const char* name )
    : KAction( text, cut, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const KShortcut& cut,
                              const QObject* receiver, const char* slot,
                              QObject* parent, const char* name )
  : KAction( text, cut, receiver, slot, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QIconSet& pix,
                              const KShortcut& cut,
                              QObject* parent, const char* name )
  : KAction( text, pix, cut, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QString& pix,
                              const KShortcut& cut,
                              QObject* parent, const char* name )
 : KAction( text, pix, cut, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QIconSet& pix,
                              const KShortcut& cut,
                              const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, cut, receiver, slot, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( const QString& text, const QString& pix,
                              const KShortcut& cut,
                              const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, cut, receiver, slot, parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::KToggleAction( QObject* parent, const char* name )
    : KAction( parent, name )
{
  d = new KToggleActionPrivate;
}

KToggleAction::~KToggleAction()
{
    delete d;
}

int KToggleAction::plug( QWidget* widget, int index )
{
  if ( !widget->inherits("QPopupMenu") && !widget->inherits("KToolBar") )
  {
    kdWarning() << "Can not plug KToggleAction in " << widget->className() << endl;
    return -1;
  }

/*US
  if (kapp && !kapp->authorizeKAction(name()))
    return -1;
*/

  int _index = KAction::plug( widget, index );
  if ( _index == -1 )
    return _index;

  if ( widget->inherits("QPopupMenu") )
  {
    int id = itemId( _index );
    
    static_cast<QPopupMenu*>(widget)->setItemChecked( id, d->m_checked );
  } else if ( widget->inherits( "KToolBar" ) ) {

    KToolBar *bar = static_cast<KToolBar *>( widget );

    bar->setToggle( itemId( _index ), true );
    bar->setButton( itemId( _index ), isChecked() );
  }

  return _index;
}

void KToggleAction::setChecked( bool c )
{
  if ( c == d->m_checked )
    return;
  //kdDebug(129) << "KToggleAction::setChecked(" << c << ") " << this << " " << name() << endl;

  d->m_checked = c;

  int len = containerCount();

  for( int i = 0; i < len; ++i )
    updateChecked( i );

  if ( c && parent() && !exclusiveGroup().isEmpty() ) {
    const QObjectList *list = parent()->children();
    if ( list ) {
      QObjectListIt it( *list );
      for( ; it.current(); ++it ) {
        if ( it.current()->inherits( "KToggleAction" ) && it.current() != this &&
            static_cast<KToggleAction*>(it.current())->exclusiveGroup() == exclusiveGroup() ) {
	  KToggleAction *a = static_cast<KToggleAction*>(it.current());
	  if( a->isChecked() ) {
	    a->setChecked( false );
	    emit a->toggled( false );
	  }
        }
      }
    }
  }
}

void KToggleAction::updateChecked( int id )
{
  QWidget *w = container( id );

  if ( w->inherits( "QPopupMenu" ) )
    static_cast<QPopupMenu*>(w)->setItemChecked( itemId( id ), d->m_checked );
  else if ( w->inherits( "QMenuBar" ) )
    static_cast<QMenuBar*>(w)->setItemChecked( itemId( id ), d->m_checked );
  else if ( w->inherits( "KToolBar" ) )
  {
    QWidget* r = static_cast<KToolBar*>( w )->getButton( itemId( id ) );
    if ( r && r->inherits( "KToolBarButton" ) )
      static_cast<KToolBar*>( w )->setButton( itemId( id ), d->m_checked );
  }
}

void KToggleAction::slotActivated()
{
  setChecked( !isChecked() );
  emit activated();
  emit toggled( isChecked() );
}

bool KToggleAction::isChecked() const
{
  return d->m_checked;
}

void KToggleAction::setExclusiveGroup( const QString& name )
{
  d->m_exclusiveGroup = name;
}

QString KToggleAction::exclusiveGroup() const
{
  return d->m_exclusiveGroup;
}


KRadioAction::KRadioAction( const QString& text, const KShortcut& cut,
                            QObject* parent, const char* name )
: KToggleAction( text, cut, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const KShortcut& cut,
                            const QObject* receiver, const char* slot,
                            QObject* parent, const char* name )
: KToggleAction( text, cut, receiver, slot, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QIconSet& pix,
                            const KShortcut& cut,
                            QObject* parent, const char* name )
: KToggleAction( text, pix, cut, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QString& pix,
                            const KShortcut& cut,
                            QObject* parent, const char* name )
: KToggleAction( text, pix, cut, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QIconSet& pix,
                            const KShortcut& cut,
                            const QObject* receiver, const char* slot,
                            QObject* parent, const char* name )
: KToggleAction( text, pix, cut, receiver, slot, parent, name )
{
}

KRadioAction::KRadioAction( const QString& text, const QString& pix,
                            const KShortcut& cut,
                            const QObject* receiver, const char* slot,
                            QObject* parent, const char* name )
: KToggleAction( text, pix, cut, receiver, slot, parent, name )
{
}

KRadioAction::KRadioAction( QObject* parent, const char* name )
: KToggleAction( parent, name )
{
}

void KRadioAction::slotActivated()
{
  if ( isChecked() )
  {
    const QObject *senderObj = sender();

    if ( !senderObj || !senderObj->inherits( "KToolBarButton" ) )
      return;

    qDebug("KRadioAction::slotActivated has to be fixed");
    const_cast<KToolBarButton *>( static_cast<const KToolBarButton *>( senderObj ) )->on( true );

    return;
  }

  KToggleAction::slotActivated();
}

class KSelectAction::KSelectActionPrivate
{
public:
  KSelectActionPrivate()
  {
    m_edit = false;
    m_menuAccelsEnabled = true;
    m_menu = 0;
    m_current = -1;
    m_comboWidth = -1;
  }
  bool m_edit;
  bool m_menuAccelsEnabled;
  QPopupMenu *m_menu;
  int m_current;
  int m_comboWidth;
  QStringList m_list;

  QString makeMenuText( const QString &_text )
  {
      if ( m_menuAccelsEnabled )
        return _text;
      QString text = _text;
      uint i = 0;
      while ( i < text.length() ) {
          if ( text.at( i ) == '&' ) {
              text.insert( i, '&' );
              i += 2;
          }
          else
              ++i;
      }
      return text;
  }
};

KSelectAction::KSelectAction( const QString& text, const KShortcut& cut,
                              QObject* parent, const char* name )
  : KAction( text, cut, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const KShortcut& cut,
                              const QObject* receiver, const char* slot,
                              QObject* parent, const char* name )
  : KAction( text, cut, receiver, slot, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QIconSet& pix,
                              const KShortcut& cut,
                              QObject* parent, const char* name )
  : KAction( text, pix, cut, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QString& pix,
                              const KShortcut& cut,
                              QObject* parent, const char* name )
  : KAction( text, pix, cut, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QIconSet& pix,
                              const KShortcut& cut,
                              const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, cut, receiver, slot, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( const QString& text, const QString& pix,
                              const KShortcut& cut,
                              const QObject* receiver,
                              const char* slot, QObject* parent,
                              const char* name )
  : KAction( text, pix, cut, receiver, slot, parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::KSelectAction( QObject* parent, const char* name )
  : KAction( parent, name )
{
  d = new KSelectActionPrivate;
}

KSelectAction::~KSelectAction()
{
  assert(d);
  delete d->m_menu;
  delete d; d = 0;
}

void KSelectAction::setCurrentItem( int id )
{
    if ( id >= (int)d->m_list.count() ) {
        ASSERT(id < (int)d->m_list.count());
        return;
    }

    if ( d->m_menu )
    {
        if ( d->m_current >= 0 )
            d->m_menu->setItemChecked( d->m_current, false );
        if ( id >= 0 )
        {
        //US qDebug("KSelectAction::setCurrentItem %i", id);
            d->m_menu->setItemChecked( id, true );
        }
    }

    d->m_current = id;

    int len = containerCount();

    for( int i = 0; i < len; ++i )
        updateCurrentItem( i );

    //    emit KAction::activated();
    //    emit activated( currentItem() );
    //    emit activated( currentText() );
}

void KSelectAction::setComboWidth( int width )
{
  if ( width < 0 )
    return;

  d->m_comboWidth=width;

  int len = containerCount();

  for( int i = 0; i < len; ++i )
    updateComboWidth( i );

}
QPopupMenu* KSelectAction::popupMenu() const
{
	kdDebug(129) << "KSelectAction::popupMenu()" << endl; // remove -- ellis
  if ( !d->m_menu )
  {
//US    d->m_menu = new KPopupMenu(0L, "KSelectAction::popupMenu()");
    d->m_menu = new QPopupMenu(0L, "QSelectAction::popupMenu()");
    setupMenu();
    if ( d->m_current >= 0 )
      d->m_menu->setItemChecked( d->m_current, true );
  }

  return d->m_menu;
}

void KSelectAction::setupMenu() const
{
    if ( !d->m_menu )
        return;
    d->m_menu->clear();

    QStringList::ConstIterator it = d->m_list.begin();
    for( uint id = 0; it != d->m_list.end(); ++it, ++id ) {
        QString text = *it;
        if ( !text.isEmpty() )
            d->m_menu->insertItem( d->makeMenuText( text ), this, SLOT( slotActivated( int ) ), 0, id );
        else
            d->m_menu->insertSeparator();
    }
}

void KSelectAction::changeItem( int index, const QString& text )
{
  if ( index < 0 || index >= (int)d->m_list.count() )
  {
    kdWarning() << "KSelectAction::changeItem Index out of scope" << endl;
    return;
  }

  d->m_list[ index ] = text;

  if ( d->m_menu )
    d->m_menu->changeItem( index, d->makeMenuText( text ) );

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    changeItem( i, index, text );
}

void KSelectAction::changeItem( int id, int index, const QString& text)
{
  if ( index < 0 )
        return;
  
  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) )
  {
     QWidget* r = (static_cast<KToolBar*>( w ))->getWidget( itemId( id ) );
     if ( r->inherits( "QComboBox" ) )
     {
        QComboBox *b = static_cast<QComboBox*>( r );
        b->changeItem(text, index );
     }
  }
  
}

void KSelectAction::setItems( const QStringList &lst )
{
	kdDebug(129) << "KAction::setItems()" << endl; // remove -- ellis
  d->m_list = lst;
  d->m_current = -1;

  setupMenu();

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    updateItems( i );

  // Disable if empty and not editable
  setEnabled ( lst.count() > 0 || d->m_edit );
}

QStringList KSelectAction::items() const
{
  return d->m_list;
}

QString KSelectAction::currentText() const
{
  if ( currentItem() < 0 )
    return QString::null;

  return d->m_list[ currentItem() ];
}

int KSelectAction::currentItem() const
{
  return d->m_current;
}

void KSelectAction::updateCurrentItem( int id )
{
  if ( d->m_current < 0 )
        return;

  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) ) {
    QWidget* r = static_cast<KToolBar*>( w )->getWidget( itemId( id ) );
    if ( r->inherits( "QComboBox" ) ) {
      QComboBox *b = static_cast<QComboBox*>( r );
      b->setCurrentItem( d->m_current );
    }
  }
}

int KSelectAction::comboWidth() const
{
  return d->m_comboWidth;
}

void KSelectAction::updateComboWidth( int id )
{
  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) ) {
    QWidget* r = static_cast<KToolBar*>( w )->getWidget( itemId( id ) );
    if ( r->inherits( "QComboBox" ) ) {
      QComboBox *cb = static_cast<QComboBox*>( r );
      cb->setMinimumWidth( d->m_comboWidth );
      cb->setMaximumWidth( d->m_comboWidth );
    }
  }
}

void KSelectAction::updateItems( int id )
{
	kdDebug(129) << "KAction::updateItems( " << id << ", lst )" << endl; // remove -- ellis
  
  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) ) {

    QWidget* r = static_cast<KToolBar*>( w )->getWidget( itemId( id ) );
    if ( r->inherits( "QComboBox" ) ) {
      QComboBox *cb = static_cast<QComboBox*>( r );
      cb->clear();
      QStringList lst = comboItems();
      QStringList::ConstIterator it = lst.begin();
      for( ; it != lst.end(); ++it )
        cb->insertItem( *it );
      // Ok, this currently doesn't work due to a bug in QComboBox
      // (the sizehint is cached for ever and never recalculated)
      // Bug reported (against Qt 2.3.1).
      cb->setMinimumWidth( cb->sizeHint().width() );
    }
  }
   
}

int KSelectAction::plug( QWidget *widget, int index )
{
//US  if (kapp && !kapp->authorizeKAction(name()))
//US    return -1;
  
  kdDebug(129) << "KAction::plug( " << widget << ", " << index << " )" << endl; // remove -- ellis
  if ( widget->inherits("QPopupMenu") )
  {
    // Create the PopupMenu and store it in m_menu
    (void)popupMenu();

    QPopupMenu* menu = static_cast<QPopupMenu*>( widget );
    int id;
    
    if ( hasIconSet() )
      id = menu->insertItem( iconSet(), text(), d->m_menu, -1, index );
    else
      id = menu->insertItem( text(), d->m_menu, -1, index );
      
    if ( !isEnabled() )
        menu->setItemEnabled( id, false );

    QString wth = whatsThis();
    if ( !wth.isEmpty() )
        menu->setWhatsThis( id, wth );
    
    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits("KToolBar") )
  {

    KToolBar* bar = static_cast<KToolBar*>( widget );
    int id_ = KAction::getToolButtonID();
    bar->insertCombo( comboItems(), id_, isEditable(),
                      SIGNAL( activated( const QString & ) ), this,
                      SLOT( slotActivated( const QString & ) ), isEnabled(),
                      toolTip(), -1, index );

    KComboBox *cb = bar->getCombo( id_ );
    if ( cb )
    {
         cb->setMaximumHeight( 34 );
      if (!isEditable()) cb->setFocusPolicy(QWidget::NoFocus);
      cb->setMinimumWidth( cb->sizeHint().width() );
      if ( d->m_comboWidth > 0 )
      {
        cb->setMinimumWidth( d->m_comboWidth );
        cb->setMaximumWidth( d->m_comboWidth );
      }
      cb->setInsertionPolicy( QComboBox::NoInsertion );
//US      QWhatsThis::add( cb, whatsThis() );
    }

    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    updateCurrentItem( containerCount() - 1 );

    return containerCount() - 1;
    
  }
  kdWarning() << "Can not plug KAction in " << widget->className() << endl;
  return -1;
}

QStringList KSelectAction::comboItems() const
{
    //qDebug("KSelectAction::comboItems has to be fixed");
  if( d->m_menuAccelsEnabled ) 
  {
    QStringList lst;
    QStringList::ConstIterator it = d->m_list.begin();
    for( ; it != d->m_list.end(); ++it )
    {
      QString item = *it;
      int i = item.find( '&' );
      if ( i > -1 )
        item = item.remove( i, 1 );
      lst.append( item );
    }
    return lst;
  } 
  else
  {
    return d->m_list;
  }
}

void KSelectAction::clear()
{
  if ( d->m_menu )
    d->m_menu->clear();

  int len = containerCount();
  for( int i = 0; i < len; ++i )
    updateClear( i );
}

void KSelectAction::updateClear( int id )
{

  QWidget* w = container( id );
  if ( w->inherits( "KToolBar" ) ) {
    QWidget* r = static_cast<KToolBar*>( w )->getWidget( itemId( id ) );
    if ( r->inherits( "QComboBox" ) ) {
      QComboBox *b = static_cast<QComboBox*>( r );
      b->clear();
    }
  }
}

void KSelectAction::slotActivated( int id )
{
  if ( d->m_current == id )
    return;

  setCurrentItem( id );
  // Delay this. Especially useful when the slot connected to activated() will re-create
  // the menu, e.g. in the recent files action. This prevents a crash.

  QTimer::singleShot( 0, this, SLOT( slotActivated() ) );
}

void KSelectAction::slotActivated( const QString &text )
{
  if ( isEditable() )
  {
    QStringList lst = items();
    if(lst.contains(text)==0)
    {
      lst.append( text );
      setItems( lst );
    }
  }

  int i = items().findIndex( text );
  if ( i > -1 )
      setCurrentItem( i );
  else
      setCurrentItem( comboItems().findIndex( text ) );
  // Delay this. Especially useful when the slot connected to activated() will re-create
  // the menu, e.g. in the recent files action. This prevents a crash.
  
  QTimer::singleShot( 0, this, SLOT( slotActivated() ) );
}

void KSelectAction::slotActivated()
{
  KAction::slotActivated();
  kdDebug(129) << "KSelectAction::slotActivated currentItem=" << currentItem() << " currentText=" << currentText() << endl;
  emit activated( currentItem() );
  emit activated( currentText() );
}

void KSelectAction::setEditable( bool edit )
{
  d->m_edit = edit;
}

bool KSelectAction::isEditable() const
{
  return d->m_edit;
}

void KSelectAction::setRemoveAmpersandsInCombo( bool b )
{
  setMenuAccelsEnabled( b );
}

bool KSelectAction::removeAmpersandsInCombo() const
{
  return menuAccelsEnabled( );
}

void KSelectAction::setMenuAccelsEnabled( bool b )
{
  d->m_menuAccelsEnabled = b;
}

bool KSelectAction::menuAccelsEnabled() const
{
  return d->m_menuAccelsEnabled;
}

class KListAction::KListActionPrivate
{
public:
  KListActionPrivate()
  {
    m_current = 0;
  }
  int m_current;
};

KListAction::KListAction( const QString& text, const KShortcut& cut,
                          QObject* parent, const char* name )
  : KSelectAction( text, cut, parent, name )
{
  d = new KListActionPrivate;
}

KListAction::KListAction( const QString& text, const KShortcut& cut,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
  : KSelectAction( text, cut, parent, name )
{
  d = new KListActionPrivate;
  if ( receiver )
    connect( this, SIGNAL( activated( int ) ), receiver, slot );
}

KListAction::KListAction( const QString& text, const QIconSet& pix,
                          const KShortcut& cut,
                          QObject* parent, const char* name )
  : KSelectAction( text, pix, cut, parent, name )
{
  d = new KListActionPrivate;
}

KListAction::KListAction( const QString& text, const QString& pix,
                          const KShortcut& cut,
                          QObject* parent, const char* name )
  : KSelectAction( text, pix, cut, parent, name )
{
  d = new KListActionPrivate;
}

KListAction::KListAction( const QString& text, const QIconSet& pix,
                          const KShortcut& cut, const QObject* receiver,
                          const char* slot, QObject* parent,
                          const char* name )
  : KSelectAction( text, pix, cut, parent, name )
{
  d = new KListActionPrivate;
  if ( receiver )
    connect( this, SIGNAL( activated( int ) ), receiver, slot );
}

KListAction::KListAction( const QString& text, const QString& pix,
                          const KShortcut& cut, const QObject* receiver,
                          const char* slot, QObject* parent,
                          const char* name )
  : KSelectAction( text, pix, cut, parent, name )
{
  d = new KListActionPrivate;
  if ( receiver )
    connect( this, SIGNAL( activated( int ) ), receiver, slot );
}

KListAction::KListAction( QObject* parent, const char* name )
  : KSelectAction( parent, name )
{
  d = new KListActionPrivate;
}

KListAction::~KListAction()
{
  delete d; d = 0;
}

void KListAction::setCurrentItem( int index )
{
  KSelectAction::setCurrentItem( index );
  d->m_current = index;

  //  emit KAction::activated();
  //  emit activated( currentItem() );
  // emit activated( currentText() );
}

QString KListAction::currentText() const
{
  if ( currentItem() < 0 )
      return QString::null;

  return items()[ currentItem() ];
}

int KListAction::currentItem() const
{
  return d->m_current;
}

class KRecentFilesAction::KRecentFilesActionPrivate
{
public:
  KRecentFilesActionPrivate()
  {
    m_maxItems = 0;
  }
  uint m_maxItems;
};

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const KShortcut& cut,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, cut, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  init();
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const KShortcut& cut,
                                        const QObject* receiver,
                                        const char* slot,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, cut, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  init();

  if ( receiver )
    connect( this,     SIGNAL(urlSelected(const KURL&)),
             receiver, slot );
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QIconSet& pix,
                                        const KShortcut& cut,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, cut, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  init();
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QString& pix,
                                        const KShortcut& cut,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, cut, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  init();
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QIconSet& pix,
                                        const KShortcut& cut,
                                        const QObject* receiver,
                                        const char* slot,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, cut, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  init();

  if ( receiver )
    connect( this,     SIGNAL(urlSelected(const KURL&)),
             receiver, slot );
}

KRecentFilesAction::KRecentFilesAction( const QString& text,
                                        const QString& pix,
                                        const KShortcut& cut,
                                        const QObject* receiver,
                                        const char* slot,
                                        QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( text, pix, cut, parent, name)
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  init();

  if ( receiver )
    connect( this,     SIGNAL(urlSelected(const KURL&)),
             receiver, slot );
}

KRecentFilesAction::KRecentFilesAction( QObject* parent, const char* name,
                                        uint maxItems )
  : KListAction( parent, name )
{
  d = new KRecentFilesActionPrivate;
  d->m_maxItems = maxItems;

  init();
}

void KRecentFilesAction::init()
{
  connect( this, SIGNAL( activated( const QString& ) ),
           this, SLOT( itemSelected( const QString& ) ) );

  setMenuAccelsEnabled( false );
}

KRecentFilesAction::~KRecentFilesAction()
{
  delete d; d = 0;
}

uint KRecentFilesAction::maxItems() const
{
    return d->m_maxItems;
}

void KRecentFilesAction::setMaxItems( uint maxItems )
{
    QStringList lst = items();
    uint oldCount   = lst.count();

    // set new maxItems
    d->m_maxItems = maxItems;

    // remove all items that are too much
    while( lst.count() > maxItems )
    {
        // remove last item
        lst.remove( lst.last() );
    }

    // set new list if changed
    if( lst.count() != oldCount )
        setItems( lst );
}

void KRecentFilesAction::addURL( const KURL& url )
{
    QString     file = url.prettyURL();
    QStringList lst = items();

    // remove file if already in list
    lst.remove( file );

    // remove las item if already maxitems in list
    if( lst.count() == d->m_maxItems )
    {
        // remove last item
        lst.remove( lst.last() );
    }

    // add file to list
    lst.prepend( file );
    setItems( lst );
}

void KRecentFilesAction::removeURL( const KURL& url )
{
    QStringList lst = items();
    QString     file = url.prettyURL();

    // remove url
    if( lst.count() > 0 )
    {
        lst.remove( file );
        setItems( lst );
    }
}

void KRecentFilesAction::clearURLList()
{
    clear();
}

void KRecentFilesAction::loadEntries( KConfig* config, QString groupname)
{
    QString     key;
    QString     value;
    QString     oldGroup;
    QStringList lst;

    oldGroup = config->group();

    if (groupname.isEmpty())
      groupname = "RecentFiles";
    config->setGroup( groupname );

    // read file list
    for( unsigned int i = 1 ; i <= d->m_maxItems ; i++ )
    {
        key = QString( "File%1" ).arg( i );
        value = config->readEntry( key, QString::null );

        if (!value.isNull())
            lst.append( value );
    }

    // set file
    setItems( lst );

    config->setGroup( oldGroup );
}

void KRecentFilesAction::saveEntries( KConfig* config, QString groupname )
{
    QString     key;
    QString     value;
    QStringList lst = items();

    if (groupname.isEmpty())
      groupname = "RecentFiles";

    config->deleteGroup( groupname);
          
    KConfigGroupSaver( config, groupname );
    
    // write file list
    for( unsigned int i = 1 ; i <= lst.count() ; i++ )
    {
        key = QString( "File%1" ).arg( i );
        value = lst[ i - 1 ];
        config->writeEntry( key, value );
    }
}

void KRecentFilesAction::itemSelected( const QString& text )
{
    emit urlSelected( KURL( text ) );
}

class KFontAction::KFontActionPrivate
{
public:
  KFontActionPrivate()
  {
  }
  QStringList m_fonts;
};

KFontAction::KFontAction( const QString& text,
                          const KShortcut& cut, QObject* parent,
                          const char* name )
  : KSelectAction( text, cut, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( true );
}

KFontAction::KFontAction( const QString& text, const KShortcut& cut,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
    : KSelectAction( text, cut, receiver, slot, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( true );
}

KFontAction::KFontAction( const QString& text, const QIconSet& pix,
                          const KShortcut& cut,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, cut, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( true );
}

KFontAction::KFontAction( const QString& text, const QString& pix,
                          const KShortcut& cut,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, cut, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( true );
}

KFontAction::KFontAction( const QString& text, const QIconSet& pix,
                          const KShortcut& cut,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, cut, receiver, slot, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( true );
}

KFontAction::KFontAction( const QString& text, const QString& pix,
                          const KShortcut& cut,
                          const QObject* receiver, const char* slot,
                          QObject* parent, const char* name )
    : KSelectAction( text, pix, cut, receiver, slot, parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( true );
}


KFontAction::KFontAction( QObject* parent, const char* name )
  : KSelectAction( parent, name )
{
    d = new KFontActionPrivate;
    get_fonts( d->m_fonts );
    KSelectAction::setItems( d->m_fonts );
    setEditable( true );
}

KFontAction::~KFontAction()
{
    delete d;
    d = 0;
}

/*
 * Maintenance note: Keep in sync with KFontCombo::setCurrentFont()
 */
void KFontAction::setFont( const QString &family )
{
    QString lowerName = family.lower();
    int i = 0;
    for ( QStringList::Iterator it = d->m_fonts.begin(); it != d->m_fonts.end(); ++it, ++i )
    {
       if ((*it).lower() == lowerName)
       {
          setCurrentItem(i);
          return;
       }
    }
    i = lowerName.find(" [");
    if (i>-1)
    {
       lowerName = lowerName.left(i);
       i = 0;
       for ( QStringList::Iterator it = d->m_fonts.begin(); it != d->m_fonts.end(); ++it, ++i )
       {
          if ((*it).lower() == lowerName)
          {
             setCurrentItem(i);
             return;
          }
       }
    }

    lowerName += " [";
    i = 0;
    for ( QStringList::Iterator it = d->m_fonts.begin(); it != d->m_fonts.end(); ++it, ++i )
    {
       if ((*it).lower().startsWith(lowerName))
       {
          setCurrentItem(i);
          return;
       }
    }
    kdDebug(129) << "Font not found " << family.lower() << endl;
}

int KFontAction::plug( QWidget *w, int index )
{
  qDebug("KFontAction::plug ha to be fixed");
/*US
  if (kapp && !kapp->authorizeKAction(name()))
    return -1;
  if ( w->inherits("KToolBar") )
  {
    KToolBar* bar = static_cast<KToolBar*>( w );
    int id_ = KAction::getToolButtonID();
    KFontCombo *cb = new KFontCombo( items(), bar );
    connect( cb, SIGNAL( activated( const QString & ) ),
             SLOT( slotActivated( const QString & ) ) );
    cb->setEnabled( isEnabled() );
    bar->insertWidget( id_, comboWidth(), cb, index );
    cb->setMinimumWidth( cb->sizeHint().width() );

    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    updateCurrentItem( containerCount() - 1 );

    return containerCount() - 1;
  }
  else return KSelectAction::plug( w, index );
*/  
  return 3;
}

class KFontSizeAction::KFontSizeActionPrivate
{
public:
  KFontSizeActionPrivate()
  {
  }
};

KFontSizeAction::KFontSizeAction( const QString& text,
                                  const KShortcut& cut,
                                  QObject* parent, const char* name )
  : KSelectAction( text, cut, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text,
                                  const KShortcut& cut,
                                  const QObject* receiver, const char* slot,
                                  QObject* parent, const char* name )
  : KSelectAction( text, cut, receiver, slot, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QIconSet& pix,
                                  const KShortcut& cut,
                                  QObject* parent, const char* name )
  : KSelectAction( text, pix, cut, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QString& pix,
                                  const KShortcut& cut,
                                  QObject* parent, const char* name )
  : KSelectAction( text, pix, cut, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QIconSet& pix,
                                  const KShortcut& cut,
                                  const QObject* receiver,
                                  const char* slot, QObject* parent,
                                  const char* name )
    : KSelectAction( text, pix, cut, receiver, slot, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( const QString& text, const QString& pix,
                                  const KShortcut& cut,
                                  const QObject* receiver,
                                  const char* slot, QObject* parent,
                                  const char* name )
  : KSelectAction( text, pix, cut, receiver, slot, parent, name )
{
  init();
}

KFontSizeAction::KFontSizeAction( QObject* parent, const char* name )
  : KSelectAction( parent, name )
{
  init();
}

KFontSizeAction::~KFontSizeAction()
{
    delete d;
    d = 0;
}

void KFontSizeAction::init()
{
    d = new KFontSizeActionPrivate;

    setEditable( true );
    QValueList<int> sizes = get_standard_font_sizes();
    QStringList lst;
    for ( QValueList<int>::Iterator it = sizes.begin(); it != sizes.end(); ++it )
        lst.append( QString::number( *it ) );

    setItems( lst );
}

void KFontSizeAction::setFontSize( int size )
{
    if ( size == fontSize() ) {
        setCurrentItem( items().findIndex( QString::number( size ) ) );
        return;
    }

    if ( size < 1 ) {
        kdWarning() << "KFontSizeAction: Size " << size << " is out of range" << endl;
        return;
    }

    int index = items().findIndex( QString::number( size ) );
    if ( index == -1 ) {
        // Insert at the correct position in the list (to keep sorting)
        QValueList<int> lst;
        // Convert to list of ints
        QStringList itemsList = items();
        for (QStringList::Iterator it = itemsList.begin() ; it != itemsList.end() ; ++it)
            lst.append( (*it).toInt() );
        // New size
        lst.append( size );
        // Sort the list
qDebug("KFontSizeAction::setFontSize heapsort not found.");
//US has to be fixed
//US        qHeapSort( lst );
        // Convert back to string list
        QStringList strLst;
        for (QValueList<int>::Iterator it = lst.begin() ; it != lst.end() ; ++it)
            strLst.append( QString::number(*it) );
        KSelectAction::setItems( strLst );
        // Find new current item
        index = lst.findIndex( size );
        setCurrentItem( index );
    }
    else
        setCurrentItem( index );


    //emit KAction::activated();
    //emit activated( index );
    //emit activated( QString::number( size ) );
    //emit fontSizeChanged( size );
}

int KFontSizeAction::fontSize() const
{
  return currentText().toInt();
}

void KFontSizeAction::slotActivated( int index )
{
  KSelectAction::slotActivated( index );

  emit fontSizeChanged( items()[ index ].toInt() );
}

void KFontSizeAction::slotActivated( const QString& size )
{
  setFontSize( size.toInt() ); // insert sorted first
  KSelectAction::slotActivated( size );
  emit fontSizeChanged( size.toInt() );
}

class KActionMenu::KActionMenuPrivate
{
public:
  KActionMenuPrivate()
  {
//US    m_popup = new KPopupMenu(0L,"KActionMenu::KActionMenuPrivate");
    m_popup = new QPopupMenu(0L,"KActionMenu::KActionMenuPrivate");
    m_delayed = true;
    m_stickyMenu = true;
  }
  ~KActionMenuPrivate()
  {
    delete m_popup; m_popup = 0;
  }
  
//US  KPopupMenu *m_popup;
  QPopupMenu *m_popup;
  bool m_delayed;
  bool m_stickyMenu;
};

KActionMenu::KActionMenu( QObject* parent, const char* name )
  : KAction( parent, name )
{
  d = new KActionMenuPrivate;
  setShortcutConfigurable( false );
}

KActionMenu::KActionMenu( const QString& text, QObject* parent,
                          const char* name )
  : KAction( text, 0, parent, name )
{
  d = new KActionMenuPrivate;
  setShortcutConfigurable( false );
}

KActionMenu::KActionMenu( const QString& text, const QIconSet& icon,
                          QObject* parent, const char* name )
  : KAction( text, icon, 0, parent, name )
{
  d = new KActionMenuPrivate;
  setShortcutConfigurable( false );
}

KActionMenu::KActionMenu( const QString& text, const QString& icon,
                          QObject* parent, const char* name )
  : KAction( text, icon, 0, parent, name )
{
  d = new KActionMenuPrivate;
  setShortcutConfigurable( false );
}

KActionMenu::~KActionMenu()
{
    unplugAll();
    kdDebug(129) << "KActionMenu::~KActionMenu()" << endl; // ellis
    delete d; d = 0;
}

void KActionMenu::popup( const QPoint& global )
{
  popupMenu()->popup( global );
}


//US KPopupMenu* KActionMenu::popupMenu() const
QPopupMenu* KActionMenu::popupMenu() const
{
  return d->m_popup;
}

void KActionMenu::insert( KAction* cmd, int index )
{
  if ( cmd )
    cmd->plug( d->m_popup, index );
}

void KActionMenu::remove( KAction* cmd )
{
  if ( cmd )
    cmd->unplug( d->m_popup );
}

bool KActionMenu::delayed() const {
    return d->m_delayed;
}

void KActionMenu::setDelayed(bool _delayed) {
    d->m_delayed = _delayed;
}

bool KActionMenu::stickyMenu() const {
    return d->m_stickyMenu;
}

void KActionMenu::setStickyMenu(bool sticky) {
    d->m_stickyMenu = sticky;
}

int KActionMenu::plug( QWidget* widget, int index )
{
/*US  
  if (kapp && !kapp->authorizeKAction(name()))
    return -1;
*/    
  kdDebug(129) << "KAction::plug( " << widget << ", " << index << " )" << endl; // remove -- ellis
  if ( widget->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = static_cast<QPopupMenu*>( widget );
    int id;
    if ( hasIconSet() )
      id = menu->insertItem( iconSet(), text(), d->m_popup, -1, index );
    else
      id = menu->insertItem( text(), d->m_popup, -1, index );

    if ( !isEnabled() )
      menu->setItemEnabled( id, false );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    if ( m_parentCollection )
      m_parentCollection->connectHighlight( menu, this );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = static_cast<KToolBar *>( widget );

    int id_ = KAction::getToolButtonID();

    if ( icon().isEmpty() && !iconSet().isNull() )
      bar->insertButton( iconSet().pixmap(), id_, SIGNAL( clicked() ), this,
                         SLOT( slotActivated() ), isEnabled(), plainText(),
                         index );
    else
    {
    /*US
      KInstance *instance;

      if ( m_parentCollection )
        instance = m_parentCollection->instance();
      else
        instance = KGlobal::instance();
*/
      bar->insertButton( icon(), id_, SIGNAL( clicked() ), this,
                         SLOT( slotActivated() ), isEnabled(), plainText(),
                         index/*US, instance */);
    }

    addContainer( bar, id_ );
/*US
    if (!whatsThis().isEmpty())
      QWhatsThis::add( bar->getButton(id_), whatsThis() );
*/
    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    if (delayed()) {
        bar->setDelayedPopup( id_, popupMenu(), stickyMenu() );
    } else {
        bar->getButton(id_)->setPopup(popupMenu(), stickyMenu() );
    }

    if ( m_parentCollection )
      m_parentCollection->connectHighlight( bar, this );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "QMenuBar" ) )
  {
    QMenuBar *bar = static_cast<QMenuBar *>( widget );

    int id;

    id = bar->insertItem( text(), popupMenu(), -1, index );

    if ( !isEnabled() )
        bar->setItemEnabled( id, false );

    addContainer( bar, id );
    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }

  return -1;
}

////////

KToolBarPopupAction::KToolBarPopupAction( const QString& text,
                                          const QString& icon,
                                          const KShortcut& cut,
                                          QObject* parent, const char* name )
  : KAction( text, icon, cut, parent, name )
{
  m_popup = 0;
  m_delayed = true;
  m_stickyMenu = true;
}

KToolBarPopupAction::KToolBarPopupAction( const QString& text,
                                          const QString& icon,
                                          const KShortcut& cut,
                                          const QObject* receiver,
                                          const char* slot, QObject* parent,
                                          const char* name )
  : KAction( text, icon, cut, receiver, slot, parent, name )
{
  m_popup = 0;
  m_delayed = true;
  m_stickyMenu = true;
}

KToolBarPopupAction::KToolBarPopupAction( const KGuiItem& item,
                                          const KShortcut& cut,
                                          const QObject* receiver,
                                          const char* slot, KActionCollection* parent,
                                          const char* name )
  : KAction( item, cut, receiver, slot, parent, name )
{
  m_popup = 0;
  m_delayed = true;
  m_stickyMenu = true;
}


KToolBarPopupAction::~KToolBarPopupAction()
{
  if ( m_popup )
    delete m_popup;
}

bool KToolBarPopupAction::delayed() const {
    return m_delayed;
}

void KToolBarPopupAction::setDelayed(bool delayed) {
    m_delayed = delayed;
}

bool KToolBarPopupAction::stickyMenu() const {
    return m_stickyMenu;
}

void KToolBarPopupAction::setStickyMenu(bool sticky) {
    m_stickyMenu = sticky;
}

int KToolBarPopupAction::plug( QWidget *widget, int index )
{
/*US
  if (kapp && !kapp->authorizeKAction(name()))
    return -1;
*/    
  // This is very related to KActionMenu::plug.
  // In fact this class could be an interesting base class for KActionMenu
  if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = (KToolBar *)widget;

    int id_ = KAction::getToolButtonID();
/*US
    KInstance * instance;
    if ( m_parentCollection )
        instance = m_parentCollection->instance();
    else
        instance = KGlobal::instance();
*/
    bar->insertButton( icon(), id_, SIGNAL( clicked() ), this,
                       SLOT( slotActivated() ), isEnabled(), plainText(),
                       index/*US, instance*/ );

    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    if (delayed()) {
        bar->setDelayedPopup( id_, popupMenu(), stickyMenu() );
    } else {
        bar->getButton(id_)->setPopup(popupMenu(), stickyMenu());
    }
/*US    
    if ( !whatsThis().isEmpty() )
        QWhatsThis::add( bar->getButton( id_ ), whatsThisWithIcon() );
*/        
    return containerCount() - 1;
  }


  return KAction::plug( widget, index );
}

//US KPopupMenu *KToolBarPopupAction::popupMenu() const
QPopupMenu *KToolBarPopupAction::popupMenu() const
{
    if ( !m_popup ) {
        KToolBarPopupAction *that = const_cast<KToolBarPopupAction*>(this);
//US        that->m_popup = new KPopupMenu;
        that->m_popup = new QPopupMenu;
    }
    return m_popup;
}

////////

KToggleToolBarAction::KToggleToolBarAction( const char* toolBarName,
         const QString& text, KActionCollection* parent, const char* name )
  : KToggleAction( text, KShortcut(), parent, name )
  , m_toolBarName( toolBarName )
  , m_toolBar( 0L )
{
}

KToggleToolBarAction::KToggleToolBarAction( KToolBar *toolBar, const QString &text,
                                            KActionCollection *parent, const char *name )
  : KToggleAction( text, KShortcut(), parent, name )
  , m_toolBarName( 0 )
  , m_toolBar( toolBar )
{
}

KToggleToolBarAction::~KToggleToolBarAction()
{
}

int KToggleToolBarAction::plug( QWidget* w, int index )
{
  qDebug("KToggleToolBarAction::plug has to be fixed");
/*US
  if (kapp && !kapp->authorizeKAction(name()))
      return -1;

  if ( !m_toolBar ) {
    // Note: topLevelWidget() stops too early, we can't use it.
    QWidget * tl = w;
    QWidget * n;
    while ( !tl->isDialog() && ( n = tl->parentWidget() ) ) // lookup parent and store
      tl = n;

//US    KMainWindow * mw = dynamic_cast<KMainWindow *>(tl); // try to see if it's a kmainwindow
    QMainWindow * mw = 0;
    if ( tl->inherits("QMainWindow") )
      mw = (QMainWindow *)(tl); // try to see if it's a kmainwindow

    if ( mw )
        m_toolBar = mw->toolBar( m_toolBarName );
  }

  if( m_toolBar ) {
    setChecked( m_toolBar->isVisible() );
    connect( m_toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(setChecked(bool)) );
    // Also emit toggled when the toolbar's visibility changes (see comment in header)
    connect( m_toolBar, SIGNAL(visibilityChanged(bool)), this, SIGNAL(toggled(bool)) );
  } else {
    setEnabled( false );
  }
*/
  return KToggleAction::plug( w, index );
}

void KToggleToolBarAction::setChecked( bool c )
{
  if( m_toolBar && c != m_toolBar->isVisible() ) {
    if( c ) {
      m_toolBar->show();
    } else {
      m_toolBar->hide();
    }
  qDebug("KToggleToolBarAction::setChecked has to be fixed");
/*US
    QMainWindow* mw = m_toolBar->mainWindow();
    if ( mw && mw->inherits( "KMainWindow" ) )
      static_cast<KMainWindow *>( mw )->setSettingsDirty();
*/      
  }
  KToggleAction::setChecked( c );
  
}

////////

KWidgetAction::KWidgetAction( QWidget* widget,
    const QString& text, const KShortcut& cut,
    const QObject* receiver, const char* slot,
    KActionCollection* parent, const char* name )
  : KAction( text, cut, receiver, slot, parent, name )
  , m_widget( widget )
  , m_autoSized( false )
{
}

KWidgetAction::~KWidgetAction()
{
}

void KWidgetAction::setAutoSized( bool autoSized )
{
  if( m_autoSized == autoSized )
    return;

  m_autoSized = autoSized;

  if( !m_widget || !isPlugged() )
    return;

  KToolBar* toolBar = (KToolBar*)m_widget->parent();
  int i = findContainer( toolBar );
  if ( i == -1 )
    return;
  int id = itemId( i );

  toolBar->setItemAutoSized( id, m_autoSized );
  
}

int KWidgetAction::plug( QWidget* w, int index )
{
/*US
  if (kapp && !kapp->authorizeKAction(name()))
      return -1;
*/
  if ( !w->inherits( "KToolBar" ) ) {
    kdError() << "KWidgetAction::plug: KWidgetAction must be plugged into KToolBar." << endl;
    return -1;
  }
  if ( !m_widget ) {
    kdError() << "KWidgetAction::plug: Widget was deleted or null!" << endl;
    return -1;
  }

  KToolBar* toolBar = static_cast<KToolBar*>( w );

  int id = KAction::getToolButtonID();

  m_widget->reparent( toolBar, QPoint() );
  toolBar->insertWidget( id, 0, m_widget, index );
  toolBar->setItemAutoSized( id, m_autoSized );

//US  QWhatsThis::add( m_widget, whatsThis() );
  addContainer( toolBar, id );

  connect( toolBar, SIGNAL( toolbarDestroyed() ), this, SLOT( slotToolbarDestroyed() ) );
  connect( toolBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

  return containerCount() - 1;
}

void KWidgetAction::unplug( QWidget *w )
{
  if( !m_widget || !isPlugged() )
    return;

  KToolBar* toolBar = (KToolBar*)m_widget->parent();
  if ( toolBar == w )
  {
      disconnect( toolBar, SIGNAL( toolbarDestroyed() ), this, SLOT( slotToolbarDestroyed() ) );
      m_widget->reparent( 0L, QPoint(), false ); // false = showit
  }
  KAction::unplug( w );
}

void KWidgetAction::slotToolbarDestroyed()
{
  //Q_ASSERT( m_widget ); // When exiting the app the widget could be destroyed before the toolbar.

  ASSERT( isPlugged() );
  if( !m_widget || !isPlugged() )
    return;

  // Don't let a toolbar being destroyed, delete my widget.
  m_widget->reparent( 0L, QPoint(), false /*showIt*/ );
}

////////

KActionSeparator::KActionSeparator( QObject *parent, const char *name )
  : KAction( parent, name )
{
}

KActionSeparator::~KActionSeparator()
{
}

int KActionSeparator::plug( QWidget *widget, int index )
{
  if ( widget->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = static_cast<QPopupMenu*>( widget );

    int id = menu->insertSeparator( index );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "QMenuBar" ) )
  {
    QMenuBar *menuBar = static_cast<QMenuBar *>( widget );

    int id = menuBar->insertSeparator( index );

    addContainer( menuBar, id );

    connect( menuBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *toolBar = static_cast<KToolBar *>( widget );

    int id = toolBar->insertSeparator( index );
//        toolBar->addSeparator();

    addContainer( toolBar, id );

    connect( toolBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  
  return -1;
}

void KToggleAction::virtual_hook( int id, void* data )
{ KAction::virtual_hook( id, data ); }

void KRadioAction::virtual_hook( int id, void* data )
{ KToggleAction::virtual_hook( id, data ); }

void KSelectAction::virtual_hook( int id, void* data )
{ KAction::virtual_hook( id, data ); }

void KListAction::virtual_hook( int id, void* data )
{ KSelectAction::virtual_hook( id, data ); }

void KRecentFilesAction::virtual_hook( int id, void* data )
{ KListAction::virtual_hook( id, data ); }

void KFontAction::virtual_hook( int id, void* data )
{ KSelectAction::virtual_hook( id, data ); }

void KFontSizeAction::virtual_hook( int id, void* data )
{ KSelectAction::virtual_hook( id, data ); }

void KActionMenu::virtual_hook( int id, void* data )
{ KAction::virtual_hook( id, data ); }

void KToolBarPopupAction::virtual_hook( int id, void* data )
{ KAction::virtual_hook( id, data ); }

void KToggleToolBarAction::virtual_hook( int id, void* data )
{ KToggleAction::virtual_hook( id, data ); }

void KWidgetAction::virtual_hook( int id, void* data )
{ KAction::virtual_hook( id, data ); }

void KActionSeparator::virtual_hook( int id, void* data )
{ KAction::virtual_hook( id, data ); }

/* vim: et sw=2 ts=2
 */

/*US 
#include "kactionclasses.moc"
*/
