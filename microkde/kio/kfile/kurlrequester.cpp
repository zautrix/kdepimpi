/* This file is part of the KDE libraries
    Copyright (C) 1999,2000,2001 Carsten Pfeiffer <pfeiffer@kde.org>

    library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/


#include <sys/stat.h>
#ifdef _WIN32_

#else
#include <unistd.h>
#endif
#include <qstring.h>
//US #include <qtooltip.h>

#include <qpushbutton.h>

//US #include <kaccel.h>
//US #include <kcombobox.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
//US #include <kurlcompletion.h>
//US #include <kurldrag.h>
//US #include <kprotocolinfo.h>


#include "kurlrequester.h"


class KURLDragPushButton : public QPushButton
{
public:
    KURLDragPushButton( QWidget *parent, const char *name=0 )
	: QPushButton( parent, name ) {
//US    	setDragEnabled( true );
    }
    ~KURLDragPushButton() {}

    void setURL( const KURL& url ) {
	m_urls.clear();
	m_urls.append( url );
    }

    /* not needed so far
    void setURLs( const KURL::List& urls ) {
	m_urls = urls;	
    }
    const KURL::List& urls() const { return m_urls; }
    */

protected:
/*US
    virtual QDragObject *dragObject() {
	if ( m_urls.isEmpty() )
	    return 0L;

	QDragObject *drag = KURLDrag::newDrag( m_urls, this, "url drag" );
	return drag;
    }
*/
private:
    KURL::List m_urls;

};


/*
*************************************************************************
*/

class KURLRequester::KURLRequesterPrivate
{
public:
    KURLRequesterPrivate() {
	edit = 0L;
//US	combo = 0L;
//US        fileDialogMode = KFile::File | KFile::ExistingOnly | KFile::LocalOnly;
    }

    void setText( const QString& text ) {
/*US    
	if ( combo )
	{
	    if (combo->editable())
	    {
               combo->setEditText( text );
            }
            else
            {
               combo->insertItem( text );
               combo->setCurrentItem( combo->count()-1 );
            }
        }
	else
*/  
	{
	    edit->setText( text );
	}
    }

    void connectSignals( QObject *receiver ) {
	QObject *sender;
/*US	if ( combo )
	    sender = combo;
	else
*/  
	    sender = edit;

	connect( sender, SIGNAL( textChanged( const QString& )),
		 receiver, SIGNAL( textChanged( const QString& )));
	connect( sender, SIGNAL( returnPressed() ),
		 receiver, SIGNAL( returnPressed() ));
//US	connect( sender, SIGNAL( returnPressed( const QString& ) ),
//US		 receiver, SIGNAL( returnPressed( const QString& ) ));
    }
/*US
    void setCompletionObject( KCompletion *comp ) {
	if ( combo )
	    combo->setCompletionObject( comp );
	else
	    edit->setCompletionObject( comp );
    }
*/	
    /**
     * replaces ~user or $FOO, if necessary
     */
    QString url() {
        QString txt = /*US combo ? combo->currentText() : */ edit->text();
/*US        KURLCompletion *comp;
        if ( combo )
            comp = dynamic_cast<KURLCompletion*>(combo->completionObject());
        else
            comp = dynamic_cast<KURLCompletion*>(edit->completionObject());

        if ( comp )
            return comp->replacedPath( txt );
        else
*/        
            return txt;
    }

    KLineEdit *edit;
//US    KComboBox *combo;
    int fileDialogMode;
    QString fileDialogFilter;
};


/*US
KURLRequester::KURLRequester( QWidget *editWidget, QWidget *parent,
			      const char *name )
  : QHBox( parent, name )
{
    d = new KURLRequesterPrivate;

    // must have this as parent
    editWidget->reparent( this, 0, QPoint(0,0) );
//US    d->edit = dynamic_cast<KLineEdit*>( editWidget );
    d->edit = (KLineEdit*)( editWidget );
//US    d->combo = dynamic_cast<KComboBox*>( editWidget );

    init();
}
*/

KURLRequester::KURLRequester( QWidget *parent, const char *name )
  : QHBox( parent, name )
{
    d = new KURLRequesterPrivate;
    init();
}


KURLRequester::KURLRequester( const QString& url, QWidget *parent,
			      const char *name )
  : QHBox( parent, name )
{
    d = new KURLRequesterPrivate;
    init();
    setURL( url );
}


KURLRequester::~KURLRequester()
{
//US    delete myCompletion;
    delete myFileDialog;
    delete d;
}


void KURLRequester::init()
{
    myFileDialog    = 0L;
    myShowLocalProt = false;
    mPathIsDir = false;
    if (/*US !d->combo && */ !d->edit )
	d->edit = new KLineEdit( this, "KURLRequester::KLineEdit" );

    myButton = new KURLDragPushButton( this, "kfile button");
    QIconSet iconSet = SmallIconSet("fileopen");
    QPixmap pixMap = iconSet.pixmap( QIconSet::Small, QIconSet::Normal );
    myButton->setIconSet( iconSet );
    myButton->setFixedSize( pixMap.width()+8, pixMap.height()+8 );
//US    QToolTip::add(myButton, i18n("Open file dialog"));

    connect( myButton, SIGNAL( pressed() ), SLOT( slotUpdateURL() ));

    setSpacing( KDialog::spacingHint() );

    QWidget *widget = /*US d->combo ? (QWidget*) d->combo : */ (QWidget*) d->edit;
    setFocusProxy( widget );

    d->connectSignals( this );
    connect( myButton, SIGNAL( clicked() ), this, SLOT( slotOpenDialog() ));
/*US
    myCompletion = new KURLCompletion();
    d->setCompletionObject( myCompletion );

    KAccel *accel = new KAccel( this );
    accel->insert( KStdAccel::Open, this, SLOT( slotOpenDialog() ));
    accel->readSettings();
*/    
}


void KURLRequester::setURL( const QString& url )
{
    bool hasLocalPrefix = (url.startsWith("file:"));

    if ( !myShowLocalProt && hasLocalPrefix )
	d->setText( url.mid( 5, url.length()-5 ));
    else
	d->setText( url );
}

void KURLRequester::setCaption( const QString& caption )
{
//US	fileDialog()->setCaption( caption );
//US	QWidget::setCaption( caption );
}

QString KURLRequester::url() const
{
    return d->url();
}


void KURLRequester::slotOpenDialog()
{
    emit openFileDialog( this );

//US use our special KFIleDialog instead
    KURL u( url() );
    //QString fn = u.url();
    QString fn = d->edit->text();
    if ( mPathIsDir ) 
        fn =  KFileDialog::getExistingDirectory ( fn, "", this );
    else
        fn =  KFileDialog::getSaveFileName( fn, "", this );

    if ( fn == "" )
        return;
   
	  setURL( fn );
    emit urlSelected( d->url() );
/*US    
    KFileDialog *dlg = fileDialog();
    if ( !d->url().isEmpty() ) {
        KURL u( url() );
        // If we won't be able to list it (e.g. http), then don't try :)
        if ( KProtocolInfo::supportsListing( u.protocol() ) )
	    dlg->setSelection( u.url() );
    }

    if ( dlg->exec() == QDialog::Accepted )
    {
	setURL( dlg->selectedURL().prettyURL() );
        emit urlSelected( d->url() );
    }
*/
        
}

void KURLRequester::setMode(unsigned int mode)
{
/*US
    Q_ASSERT( (mode & KFile::Files) == 0 );
    d->fileDialogMode = mode;
    if ( (mode & KFile::Directory) && !(mode & KFile::File) )
        myCompletion->setMode( KURLCompletion::DirCompletion );
    
    if (myFileDialog)
       myFileDialog->setMode( d->fileDialogMode );
*/
}

void KURLRequester::setFilter(const QString &filter)
{
/*US
    d->fileDialogFilter = filter;
    if (myFileDialog)
       myFileDialog->setFilter( d->fileDialogFilter );
*/
}

KFileDialog * KURLRequester::fileDialog() const
{
/*US
    if ( !myFileDialog ) {
	QWidget *p = parentWidget();
	myFileDialog = new KFileDialog( QString::null, QString::null, p,
					"file dialog", true );
	
	myFileDialog->setMode( d->fileDialogMode );
        myFileDialog->setFilter( d->fileDialogFilter );
    }

    return myFileDialog;
*/    
  return 0;
}


void KURLRequester::setShowLocalProtocol( bool b )
{
    if ( myShowLocalProt == b )
	return;

    myShowLocalProt = b;
    setURL( url() );
}

void KURLRequester::clear()
{
    d->setText( QString::null );
}

KLineEdit * KURLRequester::lineEdit() const
{
    return d->edit;
}
/*US
KComboBox * KURLRequester::comboBox() const
{
    return d->combo;
}
*/
void KURLRequester::slotUpdateURL()
{
    // bin compat, myButton is declared as QPushButton
//US    KURL u( QDir::currentDirPath() + '/', url() );
    KURL u( url() );
    (static_cast<KURLDragPushButton *>( myButton))->setURL( u );
}

QPushButton * KURLRequester::button() const
{
    return myButton;
}
/*US
KEditListBox::CustomEditor KURLRequester::customEditor()
{
    setSizePolicy(QSizePolicy( QSizePolicy::Preferred,
                               QSizePolicy::Fixed));

    KLineEdit *edit = d->edit;
    if ( !edit && d->combo )
        edit = dynamic_cast<KLineEdit*>( d->combo->lineEdit() );

#ifndef NDEBUG
    if ( !edit )
        kdWarning() << "KURLRequester's lineedit is not a KLineEdit!??\n";
#endif

    KEditListBox::CustomEditor editor( this, edit );
    return editor;
}
*/
void KURLRequester::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

/*US
KURLComboRequester::KURLComboRequester( QWidget *parent,
			      const char *name )
  : KURLRequester( new KComboBox(false), parent, name)
{
}
*/

//US #include "kurlrequester.moc"
