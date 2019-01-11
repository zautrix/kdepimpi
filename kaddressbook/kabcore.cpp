/*
    This file is part of KAddressbook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    Async a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

/*s
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: kabcore.cpp,v 1.194 2008/09/24 22:12:30 zautrix Exp $
*/

#include "kabcore.h"

#include <stdaddressbook.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <libkdepim/phoneaccess.h>

#ifndef KAB_EMBEDDED
#include <qclipboard.h>
#include <qdir.h> 
#include <qfile.h>
#include <qapplicaton.h>
#include <qprogressbar.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qvbox.h>
#include <kabc/addresseelist.h>
#include <kabc/errorhandler.h>
#include <kabc/resource.h>
#include <kabc/vcardconverter.h>
#include <kapplication.h>
#include <kactionclasses.h>
#include <kcmultidialog.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kkeydialog.h>
#include <kmessagebox.h>
#include <kprinter.h>
#include <kprotocolinfo.h>
#include <kresources/selectdialog.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kxmlguiclient.h>
#include <kaboutdata.h>
#include <libkdepim/categoryselectdialog.h>

#include "addresseeutil.h"
#include "addresseeeditordialog.h"
#include "extensionmanager.h"
#include "kstdaction.h"
#include "kaddressbookservice.h"
#include "ldapsearchdialog.h"
#include "printing/printingwizard.h"
#else // KAB_EMBEDDED

#include <kapplication.h>
#include "KDGanttMinimizeSplitter.h"
#include "kaddressbookmain.h"
#include "kactioncollection.h"
#include "addresseedialog.h"
//US
#include <addresseeview.h>

#include <qapp.h>
#include <qmenubar.h>
//#include <qtoolbar.h>
#include <qmessagebox.h>
#include <kdebug.h>
#include <kiconloader.h> // needed for SmallIcon
#include <kresources/kcmkresources.h>
#include <ktoolbar.h>
#include <kprefsdialog.h>


//#include <qlabel.h>


#ifndef DESKTOP_VERSION
#include <qpe/ir.h>
#include <qpe/qpemenubar.h>
#include <qtopia/qcopenvelope_qws.h>
#else

#include <qmenubar.h>
#endif

#endif // KAB_EMBEDDED
#include "kcmconfigs/kcmkabconfig.h"
#include "kcmconfigs/kcmkdepimconfig.h"
#include "kpimglobalprefs.h"
#include "externalapphandler.h"
#include "xxportselectdialog.h"


#include <kresources/selectdialog.h>
#include <kmessagebox.h>

#include <picture.h>
#include <resource.h>

//US#include <qsplitter.h>
#include <qmap.h>
#include <qdir.h>
#include <qfile.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qclipboard.h>
#include <qtextstream.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include <libkdepim/categoryselectdialog.h>
#include <libkdepim/categoryeditdialog.h>
#include <kabc/vcardconverter.h>


#include "addresseeutil.h"
#include "undocmds.h"
#include "addresseeeditordialog.h"
#include "viewmanager.h"
#include "details/detailsviewcontainer.h"
#include "kabprefs.h"
#include "xxportmanager.h"
#include "incsearchwidget.h"
#include "jumpbuttonbar.h"
#include "extensionmanager.h"
#include "addresseeconfig.h"
#include "nameeditdialog.h"
#include <kcmultidialog.h>

#ifdef _WIN32_
#ifdef  _OL_IMPORT_
#include "kaimportoldialog.h"
#include <libkdepim/ol_access.h>
#endif
#else
#include <unistd.h>
#endif
// sync includes
#include <libkdepim/ksyncprofile.h>
#include <libkdepim/ksyncprefsdialog.h>


class KABCatPrefs : public QDialog
{
  public:
    KABCatPrefs( QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("Manage new Categories") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 3 );
        lay->setMargin( 3 );
        QLabel * lab = new QLabel( i18n("After importing/loading/syncing\nthere may be new categories in\naddressees\nwhich are not in the category list.\nPlease choose what to do:\n "), this );
        lay->addWidget( lab ); 
        QButtonGroup* format = new  QButtonGroup( 1, Horizontal, i18n("New categories not in list:"), this );
        lay->addWidget( format ); 
        format->setExclusive ( true ) ;
        addCatBut = new QRadioButton(i18n("Add to category list"), format );
        new QRadioButton(i18n("Remove from addressees"), format );
        addCatBut->setChecked( true );
        QPushButton * ok = new QPushButton( i18n("OK"), this );
        lay->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        lay->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
        connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
        resize( 200, 200 );
    }
      
    bool addCat() { return addCatBut->isChecked(); }
private:
    QRadioButton* addCatBut;
};

class KABFormatPrefs : public QDialog
{
  public:
    KABFormatPrefs( QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("Set formatted name") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 3 );
        lay->setMargin( 3 );
        QLabel * lab = new QLabel( i18n("You can set the formatted name\nfor a list of contacts in one go."), this );
        lay->addWidget( lab ); 
        QButtonGroup* format = new QButtonGroup( 1, Horizontal, i18n("Set formatted name to:"), this );
        lay->addWidget( format ); 
        format->setExclusive ( true ) ;
        simple = new QRadioButton(i18n("Simple: James Bond"), format );
        full = new QRadioButton(i18n("Full: Mr. James 007 Bond I"), format );
        reverse = new QRadioButton(i18n("Reverse: Bond, James"), format );
        company = new QRadioButton(i18n("Organization: MI6"), format );
        simple->setChecked( true );
        setCompany = new QCheckBox(i18n("Set formatted name to\norganization, if name empty"), this);
        lay->addWidget( setCompany );
        QPushButton * ok = new QPushButton( i18n("Select contact list"), this );
        lay->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        lay->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
        connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
        //resize( 200, 200 );
       
    }
public:
    QRadioButton* simple, *full, *reverse, *company;
    QCheckBox* setCompany;
};



class KAex2phonePrefs : public QDialog
{
  public:
    KAex2phonePrefs( QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("Export to phone options") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 3 );
        lay->setMargin( 3 );
        QLabel *lab;
        lay->addWidget(lab = new QLabel( i18n("Please read Help-Sync Howto\nto know what settings to use."), this ) );  
        lab->setAlignment (AlignHCenter  ); 
        QHBox* temphb;
        temphb = new QHBox( this );
        new QLabel( i18n("I/O device: "), temphb ); 
        mPhoneDevice = new QLineEdit( temphb);
        lay->addWidget( temphb );
        temphb = new QHBox( this );
        new QLabel( i18n("Connection: "), temphb ); 
        mPhoneConnection = new QLineEdit( temphb);
        lay->addWidget( temphb );
        temphb = new QHBox( this );
        new QLabel( i18n("Model(opt.): "), temphb ); 
        mPhoneModel = new QLineEdit( temphb);
        lay->addWidget( temphb );
        // mWriteToSim = new QCheckBox( i18n("Write Contacts to SIM card\n(if not, write to phone memory)"), this );
        // lay->addWidget( mWriteToSim );
        lay->addWidget(lab = new QLabel( i18n("NOTE: This will remove all old\ncontact data on phone!"), this ) );  
        lab->setAlignment (AlignHCenter); 
        QPushButton * ok = new QPushButton( i18n("Export to mobile phone!"), this );
        lay->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        lay->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
        connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
        resize( 220, 240 );
        
    }
   
public:
    QLineEdit* mPhoneConnection, *mPhoneDevice, *mPhoneModel; 
    QCheckBox* mWriteToSim;
};


bool pasteWithNewUid = true;

#ifdef KAB_EMBEDDED
KABCore::KABCore( KAddressBookMain *client, bool readWrite, QWidget *parent, const char *name )
  : QWidget( parent, name ), KSyncInterface(), mGUIClient( client ), mViewManager( 0 ),
    mExtensionManager( 0 ),mConfigureDialog( 0 ),/*US  mLdapSearchDialog( 0 ),*/
    mReadWrite( readWrite ), mModified( false ), mMainWindow(client)
#else //KAB_EMBEDDED
KABCore::KABCore( KXMLGUIClient *client, bool readWrite, QWidget *parent, const char *name )
  : QWidget( parent, name ), KSyncInterface(), mGUIClient( client ), mViewManager( 0 ),
    mExtensionManager( 0 ), mConfigureDialog( 0 ), mLdapSearchDialog( 0 ),
    mReadWrite( readWrite ), mModified( false )
#endif //KAB_EMBEDDED
{
  //  syncManager = new KSyncManager((QWidget*)this, (KSyncInterface*)this, KSyncManager::KAPI, KABPrefs::instance(), syncMenu);
  //  syncManager->setBlockSave(false);
  mIncSearchWidget = 0;
    mMiniSplitter = 0;
  mExtensionBarSplitter = 0;
  mIsPart = !parent->inherits( "KAddressBookMain" );
  mAddressBook = KABC::StdAddressBook::selfNoLoad();
  KABC::StdAddressBook::setAutomaticSave( false );

#ifndef KAB_EMBEDDED
  mAddressBook->setErrorHandler( new KABC::GUIErrorHandler );
#endif //KAB_EMBEDDED

  connect( mAddressBook, SIGNAL( addressBookChanged( AddressBook * ) ),
           SLOT( addressBookChanged() ) );

#if 0
  // LR moved to addressbook init method
  mAddressBook->addCustomField( i18n( "Department" ), KABC::Field::Organization,
                                "X-Department", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "Profession" ), KABC::Field::Organization,
                                "X-Profession", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "Assistant's Name" ), KABC::Field::Organization,
                                "X-AssistantsName", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "Manager's Name" ), KABC::Field::Organization,
                                "X-ManagersName", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "Spouse's Name" ), KABC::Field::Personal,
                                "X-SpousesName", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "Office" ), KABC::Field::Personal,
                                "X-Office", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "IM Address" ), KABC::Field::Personal,
                                "X-IMAddress", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "Anniversary" ), KABC::Field::Personal,
                                "X-Anniversary", "KADDRESSBOOK" );

  //US added this field to become compatible with Opie/qtopia addressbook
  // values can be "female" or "male" or "". An empty field represents undefined.
  mAddressBook->addCustomField( i18n( "Gender" ), KABC::Field::Personal,
                                "X-Gender", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "Children" ), KABC::Field::Personal,
                                "X-Children", "KADDRESSBOOK" );
  mAddressBook->addCustomField( i18n( "FreeBusyUrl" ), KABC::Field::Personal,
                                "X-FreeBusyUrl", "KADDRESSBOOK" );
#endif
  initGUI();

  mIncSearchWidget->setFocus();


  connect( mViewManager, SIGNAL( selected( const QString& ) ),
           SLOT( setContactSelected( const QString& ) ) );
  connect( mViewManager, SIGNAL( executed( const QString& ) ),
           SLOT( executeContact( const QString& ) ) );

  connect( mViewManager, SIGNAL( deleteRequest( ) ),
           SLOT( deleteContacts( ) ) );
  connect( mViewManager, SIGNAL( modified() ),
           SLOT( setModified() ) );

  connect( mExtensionManager, SIGNAL( modified( const KABC::Addressee::List& ) ), this, SLOT( extensionModified( const KABC::Addressee::List& ) ) );
  connect( mExtensionManager, SIGNAL( changedActiveExtension( int ) ), this, SLOT( extensionChanged( int ) ) );

  connect( mXXPortManager, SIGNAL( modified() ),
           SLOT( setModified() ) );

  connect( mJumpButtonBar, SIGNAL( jumpToLetter( const QString& ) ),
           SLOT( incrementalSearchJump( const QString& ) ) );
  connect( mIncSearchWidget, SIGNAL( fieldChanged() ),
           mJumpButtonBar, SLOT( recreateButtons() ) );

  connect( mDetails, SIGNAL( sendEmail( const QString& ) ),
           SLOT( sendMail( const QString& ) ) );


  connect( ExternalAppHandler::instance(), SIGNAL (requestForNameEmailUidList(const QString&, const QString&)),this, SLOT(requestForNameEmailUidList(const QString&, const QString&)));
  connect( ExternalAppHandler::instance(), SIGNAL (requestForDetails(const QString&, const QString&, const QString&, const QString&, const QString&)),this, SLOT(requestForDetails(const QString&, const QString&, const QString&, const QString&, const QString&)));
  connect( ExternalAppHandler::instance(), SIGNAL (requestForBirthdayList(const QString&, const QString&)),this, SLOT(requestForBirthdayList(const QString&, const QString&)));
  connect( ExternalAppHandler::instance(), SIGNAL (nextView()),this, SLOT(setDetailsToggle()));
  connect( ExternalAppHandler::instance(), SIGNAL (doRingSync()),this, SLOT( doRingSync()));
  connect( ExternalAppHandler::instance(), SIGNAL (callContactdialog()),this, SLOT(callContactdialog()));


#ifndef KAB_EMBEDDED
  connect( mViewManager, SIGNAL( urlDropped( const KURL& ) ),
           mXXPortManager, SLOT( importVCard( const KURL& ) ) );

  connect( mDetails, SIGNAL( browse( const QString& ) ),
           SLOT( browse( const QString& ) ) );


  mAddressBookService = new KAddressBookService( this );

#endif //KAB_EMBEDDED

  mMessageTimer = new QTimer( this ); 
  connect( mMessageTimer, SIGNAL( timeout() ), this, SLOT( setCaptionBack() ) );
  mEditorDialog = 0;
  createAddresseeEditorDialog( this );
  setModified( false );  
  mBRdisabled = false;
#ifndef DESKTOP_VERSION
  infrared  = 0;
#endif
  //toggleBeamReceive( );
  mMainWindow->toolBar()->show();
  // we have a toolbar repainting error on the Zaurus when starting KA/Pi
  //QTimer::singleShot( 10, this , SLOT ( updateToolBar())); 
    QTimer::singleShot( 100, this, SLOT ( loadDataAfterStart() ));
}
void KABCore::receiveStart( const QCString& cmsg, const QByteArray& data )
{
    //qDebug("KO: QCOP start message received: %s ", cmsg.data()  );
    mCStringMess = cmsg;
    mByteData = data;
}

void  KABCore::loadDataAfterStart()
{
    //qDebug("KABCore::loadDataAfterStart() ");
    ((StdAddressBook*)mAddressBook)->init( true );
        mViewManager->refreshView();

#ifndef DESKTOP_VERSION  
        disconnect(qApp, SIGNAL (appMessage ( const QCString &, const QByteArray & )), this, SLOT (receiveStart ( const QCString &, const QByteArray & )));
        
      QObject::connect(qApp, SIGNAL (appMessage ( const QCString &, const QByteArray & )), ExternalAppHandler::instance(), SLOT (appMessage ( const QCString &, const QByteArray & )));
      if ( !mCStringMess.isEmpty() ) 
          ExternalAppHandler::instance()->appMessage( mCStringMess, mByteData );
#endif
      // QTimer::singleShot( 10, this , SLOT ( updateToolBar())); 
    setCaptionBack();
}
void KABCore::updateToolBar()
{
    static int iii = 0;
    ++iii;
    mMainWindow->toolBar()->repaintMe();
    if ( iii < 4 )
        QTimer::singleShot( 100*iii, this , SLOT ( updateToolBar())); 
}
KABCore::~KABCore()
{
    //  save();
    //saveSettings();
    //KABPrefs::instance()->writeConfig();
    delete AddresseeConfig::instance();
    mAddressBook = 0;
    KABC::StdAddressBook::close();
    
    delete syncManager;
#ifndef DESKTOP_VERSION
    if ( infrared )
        delete infrared;
#endif
}
void KABCore::receive( const QCString& cmsg, const QByteArray& data )
{
    //qDebug("KA: QCOP message received: %s ", cmsg.data()  );
    if ( cmsg == "setDocument(QString)" ) {
        QDataStream stream( data, IO_ReadOnly );
        QString fileName;
        stream >> fileName;
        recieve( fileName );
        return;
    }
}
void KABCore::toggleBeamReceive( )
{
    if ( mBRdisabled  )
        return;
#ifndef DESKTOP_VERSION
    if ( infrared ) {
        qDebug("KA: AB disable BeamReceive ");
        delete infrared;
        infrared = 0;
        mActionBR->setChecked(false);
        return;
    } 
    qDebug("KA: AB enable BeamReceive ");
    mActionBR->setChecked(true);

    infrared = new QCopChannel("QPE/Application/addressbook",this, "channelAB" ) ;   
    QObject::connect( infrared, SIGNAL (received ( const QCString &, const QByteArray & )),this, SLOT(receive(  const QCString&, const QByteArray& )));
#endif
}


void KABCore::disableBR(bool b)
{
#ifndef DESKTOP_VERSION
    if ( b ) {
        if ( infrared ) {
            toggleBeamReceive( );
        }
        mBRdisabled = true;
    } else {
        if ( mBRdisabled ) {
            mBRdisabled = false;
            //toggleBeamReceive( );
        }
    }
#endif

}
void KABCore::recieve( QString fn  )
{
    //qDebug("KABCore::recieve ");
    int count = mAddressBook->importFromFile( fn, true );
    if ( count )
        setModified( true );
    mViewManager->refreshView();
    message(i18n("%1 contact(s) received!").arg( count ));
    topLevelWidget()->showMaximized();
    topLevelWidget()->raise();
}
void KABCore::restoreSettings()
{
  mMultipleViewsAtOnce = KABPrefs::instance()->mMultipleViewsAtOnce;

  bool state;

  if (mMultipleViewsAtOnce)
    state = KABPrefs::instance()->mDetailsPageVisible;
  else
    state = false;

  mActionDetails->setChecked( state );
  setDetailsVisible( state );

  state = KABPrefs::instance()->mJumpButtonBarVisible;

  mActionJumpBar->setChecked( state );
  setJumpButtonBarVisible( state );
/*US
 QValueList<int> splitterSize = KABPrefs::instance()->mDetailsSplitter;
  if ( splitterSize.count() == 0 ) {
    splitterSize.append( width() / 2 );
    splitterSize.append( width() / 2 );
  }
  mMiniSplitter->setSizes( splitterSize );
  if (  mExtensionBarSplitter ) {
  splitterSize = KABPrefs::instance()->mExtensionsSplitter;
  if ( splitterSize.count() == 0 ) {
    splitterSize.append( width() / 2 );
    splitterSize.append( width() / 2 );
  }
  mExtensionBarSplitter->setSizes( splitterSize );

  }
*/
  mViewManager->restoreSettings();
  mIncSearchWidget->setCurrentItem( KABPrefs::instance()->mCurrentIncSearchField );
  mExtensionManager->restoreSettings();
#ifdef DESKTOP_VERSION
  int wid = width();
  if ( wid < 10 )
      wid = 400;
#else
  int wid = QApplication::desktop()->width();
  if ( wid < 640 )
      wid = QApplication::desktop()->height();
#endif
  QValueList<int> splitterSize;// = KABPrefs::instance()->mDetailsSplitter;
  if ( true /*splitterSize.count() == 0*/ ) {
      splitterSize.append( wid / 2 );
      splitterSize.append( wid / 2 );
  }
  mMiniSplitter->setSizes( splitterSize );
  if (  mExtensionBarSplitter ) {
      //splitterSize = KABPrefs::instance()->mExtensionsSplitter;
      if ( true /*splitterSize.count() == 0*/ ) {
          splitterSize.append( wid / 2 );
          splitterSize.append( wid / 2 );
      }
      mExtensionBarSplitter->setSizes( splitterSize );

  }
#ifdef DESKTOP_VERSION
  KConfig *config = KABPrefs::instance()->getConfig();
  config->setGroup("WidgetLayout");
  QStringList list;
  list = config->readListEntry("MainLayout");
  int x,y,w,h;
  if ( ! list.isEmpty() ) {
      x = list[0].toInt();
      y = list[1].toInt();
      w = list[2].toInt();
      h = list[3].toInt(); 
      KApplication::testCoords( &x,&y,&w,&h );
      topLevelWidget()->setGeometry(x,y,w,h);
      
  } else {
      topLevelWidget()->setGeometry( 40 ,40 , 640, 440);
  }
#endif
}

void KABCore::saveSettings()
{
  KABPrefs::instance()->mJumpButtonBarVisible = mActionJumpBar->isChecked();
  if ( mExtensionBarSplitter )
    KABPrefs::instance()->mExtensionsSplitter = mExtensionBarSplitter->sizes();
  KABPrefs::instance()->mDetailsPageVisible = mActionDetails->isChecked();
  KABPrefs::instance()->mDetailsSplitter = mMiniSplitter->sizes();
#ifndef KAB_EMBEDDED

  KABPrefs::instance()->mExtensionsSplitter = mExtensionBarSplitter->sizes();
  KABPrefs::instance()->mDetailsSplitter = mDetailsSplitter->sizes();
#endif //KAB_EMBEDDED
  mExtensionManager->saveSettings();
  mViewManager->saveSettings();

  KABPrefs::instance()->mCurrentIncSearchField = mIncSearchWidget->currentItem();

  KABPrefs::instance()->writeConfig();
  //qDebug("KA: KABCore::saveSettings() ");
}

KABC::AddressBook *KABCore::addressBook() const
{
  return mAddressBook;
}

KConfig *KABCore::config()
{
#ifndef KAB_EMBEDDED
  return KABPrefs::instance()->config();
#else //KAB_EMBEDDED
  return KABPrefs::instance()->getConfig();
#endif //KAB_EMBEDDED
}

KActionCollection *KABCore::actionCollection() const
{
  return mGUIClient->actionCollection();
}

KABC::Field *KABCore::currentSearchField() const
{
   if (mIncSearchWidget)
    return mIncSearchWidget->currentField();
  else
    return 0;
}

QStringList KABCore::selectedUIDs() const
{
  return mViewManager->selectedUids();
}

KABC::Resource *KABCore::requestResource( QWidget *parent )
{
  QPtrList<KABC::Resource> kabcResources = addressBook()->resources();

  QPtrList<KRES::Resource> kresResources;
  QPtrListIterator<KABC::Resource> resIt( kabcResources );
  KABC::Resource *resource;
  while ( ( resource = resIt.current() ) != 0 ) {
    ++resIt;
    if ( !resource->readOnly() ) {
      KRES::Resource *res = static_cast<KRES::Resource*>( resource );
      if ( res )
        kresResources.append( res );
    }
  }

  KRES::Resource *res = KRES::SelectDialog::getResource( kresResources, parent );
  return static_cast<KABC::Resource*>( res );
}

#ifndef KAB_EMBEDDED
KAboutData *KABCore::createAboutData()
#else //KAB_EMBEDDED
void KABCore::createAboutData()
#endif //KAB_EMBEDDED
{


  QString version;
#include <../version>
    QMessageBox::about( this, "About KAddressbook/Pi",
                        "KAddressbook/Platform-independent\n"
                        "(KA/Pi) " +version + " - " +
#ifdef DESKTOP_VERSION
                        "Desktop Edition\n"
#else
                        "PDA-Edition\n"
                        "for: Zaurus 5500 / 7x0 / 8x0\n"
#endif

                        "(c) 2004 Ulf Schenk\n"
                        "(c) 2004-2008 Lutz Rogowski\nrogowski@kde.org\n"
                        "(c) 1997-2003, The KDE PIM Team\n"
  "Tobias Koenig Maintainer\n"
  "Don Sanders Original author\n"
  "Cornelius Schumacher Co-maintainer\n"
  "Mike Pilone GUI and framework redesign\n"
  "Greg Stern DCOP interface\n"
  "Mark Westcot Contact pinning\n"
  "Michel Boyer de la Giroday LDAP Lookup\n"
  "Steffen Hansen LDAP Lookup"
#ifdef _WIN32_
 "(c) 2004 Lutz Rogowski Import from OL\nrogowski@kde.org\n"
#endif
 );
}

void KABCore::setContactSelected( const QString &uid )
{
  KABC::Addressee addr = mAddressBook->findByUid( uid );
  if ( !mDetails->isHidden() )
    mDetails->setAddressee( addr );

  if ( !addr.isEmpty() ) {
    emit contactSelected( addr.formattedName() );
    KABC::Picture pic = addr.photo();
    if ( pic.isIntern() ) {
//US      emit contactSelected( pic.data() );
//US instead use:
      QPixmap px;
      if (pic.data().isNull() != true)
      {
        px.convertFromImage(pic.data());
      }

      emit contactSelected( px );
    }
  }


  mExtensionManager->setSelectionChanged();

  // update the actions
  bool selected = !uid.isEmpty();

  if ( mReadWrite ) {
    mActionCut->setEnabled( selected );
    mActionPaste->setEnabled( selected );
  }

  mActionCopy->setEnabled( selected );
  mActionDelete->setEnabled( selected );
  mActionEditAddressee->setEnabled( selected );
  mActionMail->setEnabled( selected );
  mActionMailVCard->setEnabled( selected );
  //if (mActionBeam)
  //mActionBeam->setEnabled( selected );
  mActionWhoAmI->setEnabled( selected );
}

void KABCore::sendMail()
{
  sendMail( mViewManager->selectedEmails().join( ", " ) );
}

void KABCore::sendMail( const QString& emaillist )
{
  // the parameter has the form "name1 <abc@aol.com>,name2 <abc@aol.com>;... "
  if (emaillist.contains(",") > 0)
    ExternalAppHandler::instance()->mailToMultipleContacts( emaillist, QString::null );
  else
    ExternalAppHandler::instance()->mailToOneContact( emaillist );
}



void KABCore::mailVCard()
{
  QStringList uids = mViewManager->selectedUids();
  if ( !uids.isEmpty() )
    mailVCard( uids );
}

void KABCore::mailVCard( const QStringList& uids )
{
  QStringList urls;

//  QString tmpdir = locateLocal("tmp", KGlobal::getAppName());

  QString dirName = "/tmp/" + KApplication::randomString( 8 );



  QDir().mkdir( dirName, true );

  for( QStringList::ConstIterator it = uids.begin(); it != uids.end(); ++it ) {
    KABC::Addressee a = mAddressBook->findByUid( *it );

    if ( a.isEmpty() )
      continue;

    QString name = a.givenName() + "_" + a.familyName() + ".vcf";

    QString fileName = dirName + "/" + name;

    QFile outFile(fileName);

    if ( outFile.open(IO_WriteOnly) ) {  // file opened successfully
      KABC::VCardConverter converter;
      QString vcard;

      converter.addresseeToVCard( a, vcard );

      QTextStream t( &outFile );  // use a text stream
      t.setEncoding( QTextStream::UnicodeUTF8 );
      t << vcard;

      outFile.close();

      urls.append( fileName );
    }
  }

  bool result = ExternalAppHandler::instance()->mailToMultipleContacts( QString::null, urls.join(", ") );


/*US
  kapp->invokeMailer( QString::null, QString::null, QString::null,
                      QString::null,  // subject
                      QString::null,  // body
                      QString::null,
                      urls );  // attachments
*/

}

/**
  Beams the "WhoAmI contact.
*/
void KABCore::beamMySelf()
{
  KABC::Addressee a = KABC::StdAddressBook::self()->whoAmI();
  if (!a.isEmpty())
  {
    QStringList uids;
    uids << a.uid();

	beamVCard(uids);
  } else {
      KMessageBox::information( this, i18n( "Your personal contact is\nnot set! Please select it\nand set it with menu:\nSettings - Set Who Am I\n" ) );


  }
}
void  KABCore::updateMainWindow()
{
    mMainWindow->showMaximized();
    //mMainWindow->repaint();
}
void KABCore::resizeEvent(QResizeEvent* e )
{
    if ( !mMiniSplitter ) {
        QWidget::resizeEvent( e );
        return;
    }
#ifndef DESKTOP_VERSION
    static int desktop_width = 0;
    //qDebug("KABCore::resizeEvent %d %d ",desktop_width,QApplication::desktop()->width()   );
    if ( desktop_width != QApplication::desktop()->width() )
    if ( QApplication::desktop()->width() >= 480 ) {
        if (QApplication::desktop()->width() == 640 ) { // e.g. 640x480
            //qDebug("640 ");
            if ( mMiniSplitter->orientation() == Qt::Vertical ) {
                //qDebug("switch V->H ");
                mMiniSplitter->setOrientation(  Qt::Horizontal);
                mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Right );
            }
            if ( QApplication::desktop()->width() <= 640 ) {
                bool shot = mMainWindow->isVisible();
                mMainWindow->showMinimized();
                //mMainWindow->setMaximumSize( QApplication::desktop()->size()  );
                mViewManager->getFilterAction()->setComboWidth( 150 );
                if ( mIncSearchWidget )
                    mIncSearchWidget->setSize();
                if ( shot )
                    QTimer::singleShot( 1, this , SLOT ( updateMainWindow()));
            }
            
        } else if (QApplication::desktop()->width() == 480 ){// e.g. 480x640
            //qDebug("480 ");
            if ( mMiniSplitter->orientation() == Qt::Horizontal ) {
                //qDebug("switch H->V ");
                mMiniSplitter->setOrientation( Qt::Vertical );
                mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Down );
            }
            if ( QApplication::desktop()->width() <= 640 ) {
                //mMainWindow->setMaximumSize( QApplication::desktop()->size()  );
                bool shot = mMainWindow->isVisible();
                mMainWindow->showMinimized();
                if ( KABPrefs::instance()->mHideSearchOnSwitch ) { 
                    if ( mIncSearchWidget ) {
                        mIncSearchWidget->setSize();
                    }
                } else {
                    mViewManager->getFilterAction()->setComboWidth( 0 );
                }
                if ( shot )
                    QTimer::singleShot( 1, this , SLOT ( updateMainWindow()));
            }
        }
    }
    desktop_width = QApplication::desktop()->width();
#endif
    QWidget::resizeEvent( e );

}
void KABCore::export2phone()
{

    QStringList uids;
    XXPortSelectDialog dlg( this, false, this );
    if ( dlg.exec() )
        uids = dlg.uids();
    else
        return;
    if ( uids.isEmpty() )
        return;
    // qDebug("count %d ", uids.count());
   
    KAex2phonePrefs ex2phone;
    ex2phone.mPhoneConnection->setText( KPimGlobalPrefs::instance()->mEx2PhoneConnection );
    ex2phone.mPhoneDevice->setText( KPimGlobalPrefs::instance()->mEx2PhoneDevice );
    ex2phone.mPhoneModel->setText( KPimGlobalPrefs::instance()->mEx2PhoneModel );

    if ( !ex2phone.exec() ) {
        return;
    }
    KPimGlobalPrefs::instance()->mEx2PhoneConnection  = ex2phone.mPhoneConnection->text();
    KPimGlobalPrefs::instance()->mEx2PhoneDevice = ex2phone.mPhoneDevice->text();
    KPimGlobalPrefs::instance()->mEx2PhoneModel = ex2phone.mPhoneModel->text();


    PhoneAccess::writeConfig( KPimGlobalPrefs::instance()->mEx2PhoneDevice,
                              KPimGlobalPrefs::instance()->mEx2PhoneConnection,
                              KPimGlobalPrefs::instance()->mEx2PhoneModel );

    QString  fileName = getPhoneFile();
    if ( ! mAddressBook->export2PhoneFormat( uids ,fileName ) )
        return;
    
    message(i18n("Exporting to phone..."));
    QTimer::singleShot( 1, this , SLOT ( writeToPhone()));

}
QString KABCore::getPhoneFile()
{
#ifdef DESKTOP_VERSION
    return  locateLocal("tmp", "phonefile.vcf");
#else
    return "/tmp/phonefile.vcf";
#endif

}
void KABCore::writeToPhone( )
{
    if ( PhoneAccess::writeToPhone( getPhoneFile() ) )
        message(i18n("Export to phone finished!"));
    else
        qDebug(i18n("KA: Error exporting to phone"));
}
void KABCore::beamVCard()
{ 
    QStringList uids;
    XXPortSelectDialog dlg( this, false, this );
    if ( dlg.exec() )
        uids = dlg.uids();
    else
        return;
    if ( uids.isEmpty() )
        return;
    beamVCard( uids );
}


void KABCore::beamVCard(const QStringList& uids)
{

    // LR: we should use the /tmp dir on the Zaurus,
    //  because: /tmp = RAM, (HOME)/kdepim = flash memory
   
#ifdef DESKTOP_VERSION
  QString fileName = locateLocal("tmp", "kapibeamfile.vcf");
#else
  QString fileName = "/tmp/kapibeamfile.vcf";
#endif

  KABC::VCardConverter converter;
  QString description;
  QString datastream;
  for( QStringList::ConstIterator it = uids.begin(); it != uids.end(); ++it ) {
      KABC::Addressee a = mAddressBook->findByUid( *it );

      if ( a.isEmpty() )
          continue;

      if (description.isEmpty())
          description = a.formattedName();

      QString vcard;
      converter.addresseeToVCard( a, vcard );
      int start = 0;
      int next;
      while ( (next = vcard.find("TYPE=", start) )>= 0 ) {
          int semi = vcard.find(";", next);
          int dopp = vcard.find(":", next);
          int sep;
          if ( semi < dopp && semi >= 0 )
              sep = semi ;
          else
              sep = dopp;
          datastream +=vcard.mid( start, next - start);
          datastream +=vcard.mid( next+5,sep -next -5 ).upper();
          start = sep;
      }
      datastream += vcard.mid( start,vcard.length() );
  }
#ifndef DESKTOP_VERSION
  QFile outFile(fileName);
  if ( outFile.open(IO_WriteOnly) ) { 
      datastream.replace ( QRegExp("VERSION:3.0") , "VERSION:2.1" );
      QTextStream t( &outFile );  // use a text stream
      //t.setEncoding( QTextStream::UnicodeUTF8 );
      t.setEncoding( QTextStream::Latin1 );
      t <<datastream.latin1();
      outFile.close();
      Ir *ir = new Ir( this );
      connect( ir, SIGNAL( done(Ir*) ), this, SLOT( beamDone(Ir*) ) );
      ir->send( fileName, description, "text/x-vCard" );
  } else {
      qDebug("KA: Error open temp  beam file ");
      return;
  }
#endif

}

void KABCore::beamDone( Ir *ir )
{
#ifndef DESKTOP_VERSION
  delete ir;
#endif
  topLevelWidget()->raise();
  message( i18n("Beaming finished!") );
}


void KABCore::browse( const QString& url )
{
#ifndef KAB_EMBEDDED
  kapp->invokeBrowser( url );
#else //KAB_EMBEDDED
  qDebug("KABCore::browse must be fixed");
#endif //KAB_EMBEDDED
}

void KABCore::selectAllContacts()
{
  mViewManager->setSelected( QString::null, true );
}

void KABCore::deleteContacts()
{
  QStringList uidList = mViewManager->selectedUids();
  deleteContacts( uidList );
}

void KABCore::deleteContacts( const QStringList &uids )
{
  
  if ( uids.count() > 0 ) {
      
      if ( KABPrefs::instance()->mAskForDelete ) {
          int count =  uids.count();
          if ( count > 5 ) count = 5;
          QString cNames;
          int i;
          for ( i = 0; i < count ; ++i ) {
              cNames +=  KGlobal::formatMessage( mAddressBook->findByUid( uids[i] ).realName() ,0) + "\n";
          }
          if ( uids.count() > 5 )
              cNames +=  i18n("...and %1 more\ncontact(s) selected").arg(  uids.count() - 5 );
          QString text =  i18n( "Do you really\nwant to delete the\nsetected contact(s)?\n\n" ) + cNames  ;
          if ( KMessageBox::questionYesNo( this, text )  != KMessageBox::Yes )
              return;
      }
    PwDeleteCommand *command = new PwDeleteCommand( mAddressBook, uids );
    UndoStack::instance()->push( command );
    RedoStack::instance()->clear();

    // now if we deleted anything, refresh
    setContactSelected( QString::null );
    setModified( true );
  }
}

void KABCore::copyContacts()
{
  KABC::Addressee::List addrList = mViewManager->selectedAddressees();

  QString clipText = AddresseeUtil::addresseesToClipboard( addrList );

  kdDebug(5720) << "KABCore::copyContacts: " << clipText << endl;

  QClipboard *cb = QApplication::clipboard();
  cb->setText( clipText );
}

void KABCore::cutContacts()
{
  QStringList uidList = mViewManager->selectedUids();

//US  if ( uidList.size() > 0 ) {
  if ( uidList.count() > 0 ) {
    PwCutCommand *command = new PwCutCommand( mAddressBook, uidList );
    UndoStack::instance()->push( command );
    RedoStack::instance()->clear();

    setModified( true );
  }
}

void KABCore::pasteContacts()
{
  QClipboard *cb = QApplication::clipboard();

  KABC::Addressee::List list = AddresseeUtil::clipboardToAddressees( cb->text() );

  pasteContacts( list );
}

void KABCore::pasteContacts( KABC::Addressee::List &list  )
{
  KABC::Resource *resource = requestResource( this );
  KABC::Addressee::List::Iterator it;
  for ( it = list.begin(); it != list.end(); ++it )
    (*it).setResource( resource );

  PwPasteCommand *command = new PwPasteCommand( this, list );
  UndoStack::instance()->push( command );
  RedoStack::instance()->clear();

  setModified( true );
}

void KABCore::setWhoAmI()
{
  KABC::Addressee::List addrList = mViewManager->selectedAddressees();

  if ( addrList.count() > 1 ) {
    KMessageBox::sorry( this, i18n( "Please select only one contact." ) );
    return;
  }

  QString text( i18n( "<qt>Do you really want to use <b>%1</b> as your new personal contact?</qt>" ) );
  if ( KMessageBox::questionYesNo( this, text.arg( addrList[ 0 ].realName() ) ) == KMessageBox::Yes )
    static_cast<KABC::StdAddressBook*>( KABC::StdAddressBook::self() )->setWhoAmI( addrList[ 0 ] );
}
void KABCore::editCategories()
{ 
    KPIM::CategoryEditDialog dlg ( KABPrefs::instance(), this, "", true );
    dlg.exec();
}
void KABCore::setCategories()
{

    QStringList uids;
    XXPortSelectDialog dlgx( this, false, this );
    if ( dlgx.exec() )
        uids = dlgx.uids();
    else
        return;
    if ( uids.isEmpty() )
        return;
    // qDebug("count %d ", uids.count());


  KPIM::CategorySelectDialog dlg( KABPrefs::instance(), this, "", true );
  if ( !dlg.exec() ) {
      message( i18n("Setting categories cancelled") );
      return;
  }
  bool merge = false;
  QString msg = i18n( "Merge with existing categories?" );
  if ( KMessageBox::questionYesNo( this, msg ) == KMessageBox::Yes )
    merge = true;

  message( i18n("Setting categories ... please wait!") );
  QStringList categories = dlg.selectedCategories();

  //QStringList uids = mViewManager->selectedUids();
  QStringList::Iterator it;
  for ( it = uids.begin(); it != uids.end(); ++it ) {
    KABC::Addressee addr = mAddressBook->findByUid( *it );
    if ( !addr.isEmpty() ) {
      if ( !merge )
        addr.setCategories( categories );
      else {
        QStringList addrCategories = addr.categories();
        QStringList::Iterator catIt;
        for ( catIt = categories.begin(); catIt != categories.end(); ++catIt ) {
          if ( !addrCategories.contains( *catIt ) )
            addrCategories.append( *catIt );
        }
        addr.setCategories( addrCategories );
      }
      mAddressBook->insertAddressee( addr );
    }
  }

  if ( uids.count() > 0 )
    setModified( true );
  message( i18n("Setting categories completed!") );
}

void KABCore::setSearchFields( const KABC::Field::List &fields )
{
  mIncSearchWidget->setFields( fields );
}

void KABCore::incrementalSearch( const QString& text )
{
    QString stext;
    if ( KABPrefs::instance()->mAutoSearchWithWildcard ) {
        stext = "*" + text;
    } else {
        stext = text;
    }
    mViewManager->doSearch( stext, mIncSearchWidget->currentField() );
}
void KABCore::incrementalSearchJump( const QString& text )
{
    mViewManager->doSearch( text, mIncSearchWidget->currentField() );
}

void KABCore::setModified()
{
  setModified( true );
}

void KABCore::setModifiedWOrefresh()
{
    // qDebug("KABCore::setModifiedWOrefresh() ");
    mModified = true;
    mActionSave->setEnabled( mModified );


}
void KABCore::setModified( bool modified )
{
  mModified = modified;
  mActionSave->setEnabled( mModified );

  if ( modified )
    mJumpButtonBar->recreateButtons();

  mViewManager->refreshView();

}

bool KABCore::modified() const
{
  return mModified;
}

void KABCore::contactModified( const KABC::Addressee &addr  )
{
    addrModified( addr );
#if 0 // debug only
    KABC::Addressee ad = addr;
    ad.computeCsum( "123");
#endif
}

void KABCore::addrModified( const KABC::Addressee &addr ,bool updateDetails )
{

  Command *command = 0;
  QString uid;

  // check if it exists already
  KABC::Addressee origAddr = mAddressBook->findByUid( addr.uid() );
  if ( origAddr.isEmpty() )
    command = new PwNewCommand( mAddressBook, addr );
  else {
    command = new PwEditCommand( mAddressBook, origAddr, addr );
    uid = addr.uid();
  }

  UndoStack::instance()->push( command );
  RedoStack::instance()->clear();
  if ( updateDetails )
      mDetails->setAddressee( addr );
  setModified( true );
}

void KABCore::newContact()
{


  QPtrList<KABC::Resource> kabcResources = mAddressBook->resources();

  QPtrList<KRES::Resource> kresResources;
  QPtrListIterator<KABC::Resource> it( kabcResources );
  KABC::Resource *resource;
  while ( ( resource = it.current() ) != 0 ) {
    ++it;
    if ( !resource->readOnly() ) {
      KRES::Resource *res = static_cast<KRES::Resource*>( resource );
      if ( res )
        kresResources.append( res );
    }
  }

  KRES::Resource *res = KRES::SelectDialog::getResource( kresResources, this );
  resource = static_cast<KABC::Resource*>( res );

  if ( resource ) {
    KABC::Addressee addr;
    addr.setResource( resource );
    mEditorDialog->setAddressee( addr );
    mEditorDialog->setCaption( i18n("Edit new contact"));
    KApplication::execDialog ( mEditorDialog );

  } else
    return;

  // mEditorDict.insert( dialog->addressee().uid(), dialog );


}

void KABCore::addEmail( QString aStr )
{
#ifndef KAB_EMBEDDED
  QString fullName, email;

  KABC::Addressee::parseEmailAddress( aStr, fullName, email );

  // Try to lookup the addressee matching the email address
  bool found = false;
  QStringList emailList;
  KABC::AddressBook::Iterator it;
  for ( it = mAddressBook->begin(); !found && (it != mAddressBook->end()); ++it ) {
    emailList = (*it).emails();
    if ( emailList.contains( email ) > 0 ) {
      found = true;
      (*it).setNameFromString( fullName );
      editContact( (*it).uid() );
    }
  }

  if ( !found ) {
    KABC::Addressee addr;
    addr.setNameFromString( fullName );
    addr.insertEmail( email, true );

    mAddressBook->insertAddressee( addr );
    mViewManager->refreshView( addr.uid() );
    editContact( addr.uid() );
  }
#else //KAB_EMBEDDED
  qDebug("KABCore::addEmail finsih method");
#endif //KAB_EMBEDDED
}

void KABCore::importVCard( const KURL &url, bool showPreview )
{
  mXXPortManager->importVCard( url, showPreview );
}
void KABCore::importFromOL()
{
#ifdef _OL_IMPORT_
    KABC::Addressee::List list = OL_access::instance()->importOLcontacts();
    if ( list.count() > 0 ) {
        KABC::Addressee::List listNew;
        KABC::Addressee::List listExisting;
        KABC::Addressee::List::Iterator it;
        KABC::AddressBook::Iterator iter;
        for ( it = list.begin(); it != list.end(); ++it ) {
            if ( mAddressBook->findByUid((*it).uid() ).isEmpty())
                listNew.append( (*it) );
            else
                listExisting.append( (*it) );
        }
        QString mess =  i18n("%1 contacts read from OL.\n\n%2 contacts added to addressbook!").arg( list.count()).arg( listNew.count() );
        if ( listExisting.count()  > 0 )
            mess += "\n\n"+ i18n("%1 contacts not added to addressbook\nbecause they were already in the addressbook!").arg( listExisting.count() );

        KMessageBox::information( this, mess );
        if ( listNew.count() > 0 ) {
            pasteWithNewUid = false;
            pasteContacts( listNew );
            pasteWithNewUid = true;
        }
    }
#endif
}

bool KABCore::readOLdata(  KABC::AddressBook* local )
{
#ifdef _OL_IMPORT_
    QStringList folderList = OL_access::instance()->getFolderSelection( OL_CONTACT_DATA , i18n("Select Folder to sync"));
    KABC::Addressee::List list;
    if ( folderList.count() ) {
        OL_access::instance()->readContactData( OL_access::instance()->getFolderFromID( 0, folderList[1] ) , &list, true );
        KABC::Addressee::List::Iterator it;
        for ( it = list.begin(); it != list.end(); ++it ) {
            (*it).setExternalUID( (*it).uid() ); 
            (*it).setOriginalExternalUID( (*it).uid() ); 
            (*it).setTempSyncStat(  SYNC_TEMPSTATE_NEW_EXTERNAL );
            local->insertAddressee( (*it) , false, false );
        }
        mOLsyncFolderID = folderList[1];
        //KMessageBox::information( this,  i18n("OLsync folder ID ") +  mOLsyncFolderID );
    }
    return list.count() > 0;
#else
    return false;
#endif
}
bool KABCore::writeOLdata(  KABC::AddressBook* aBook )
{
#ifdef _OL_IMPORT_
    if ( !OL_access::instance()->setSelectedFolder( mOLsyncFolderID ) )
        return false;
    KABC::AddressBook::Iterator it;
    for ( it = aBook->begin(); it != aBook->end(); ++it ) {
        if ( (*it).tempSyncStat() != SYNC_TEMPSTATE_NEW_EXTERNAL ) {
            KABC::Addressee addressee = (*it);
            if ( (*it).tempSyncStat() == SYNC_TEMPSTATE_ADDED_EXTERNAL ) {
                (*it) = OL_access::instance()->addAddressee( (*it) );
                (*it).setTempSyncStat( SYNC_TEMPSTATE_NEW_ID );
            } else if ( (*it).tempSyncStat() == SYNC_TEMPSTATE_DELETE ) {
                OL_access::instance()->deleteAddressee( (*it) );
            } else if ( (*it).tempSyncStat() != SYNC_TEMPSTATE_NEW_EXTERNAL ) {
                //changed
                (*it) = OL_access::instance()->changeAddressee( (*it) );
                (*it).setTempSyncStat( SYNC_TEMPSTATE_NEW_CSUM );
            }
        }
    }
    return true;
#else
    return false;
#endif
}
void KABCore::importVCard( const QString &vCard, bool showPreview )
{
  mXXPortManager->importVCard( vCard, showPreview );
}

//US added a second method without defaultparameter
void KABCore::editContact2() {
  editContact( QString::null );
}

void KABCore::editContact( const QString &uid )
{

  if ( mExtensionManager->isQuickEditVisible() )
    return;

  // First, locate the contact entry
  QString localUID = uid;
  if ( localUID.isNull() ) {
    QStringList uidList = mViewManager->selectedUids();
    if ( uidList.count() > 0 )
      localUID = *( uidList.at( 0 ) );
  }

  KABC::Addressee addr = mAddressBook->findByUid( localUID );
  if ( !addr.isEmpty() ) {
      mEditorDialog->setAddressee( addr );
      KApplication::execDialog ( mEditorDialog );
  }
}

/**
  Shows or edits the detail view for the given uid. If the uid is QString::null,
  the method will try to find a selected addressee in the view.
 */
void KABCore::executeContact( const QString &uid /*US = QString::null*/ )
{
  if ( mMultipleViewsAtOnce )
  {
    editContact( uid );
  }
  else
  {
    setDetailsVisible( true );
    mActionDetails->setChecked(true);
  }

}

void KABCore::save()
{
     if (syncManager->blockSave())
        return;
    if ( !mModified )
        return;

    syncManager->setBlockSave(true);
   QString text = i18n( "There was an error while attempting to save\n the "
    "address book. Please check that some \nother application is "
    "not using it. " );
   message(i18n("Saving ... please wait! "), false);
   //qApp->processEvents();
#ifndef KAB_EMBEDDED
  KABC::StdAddressBook *b = dynamic_cast<KABC::StdAddressBook*>( mAddressBook );
  if ( !b || !b->save() ) {
    KMessageBox::error( this, text, i18n( "Unable to Save" ) );
  }
#else //KAB_EMBEDDED
  KABC::StdAddressBook *b = (KABC::StdAddressBook*)( mAddressBook );
  if ( !b || !b->save() ) {
    QMessageBox::critical( this, i18n( "Unable to Save" ), text,  i18n("Ok"));
  }
#endif //KAB_EMBEDDED

  message(i18n("Addressbook saved!"));
  setModified( false );
  syncManager->setBlockSave(false);
}


void KABCore::undo()
{
  UndoStack::instance()->undo();

  // Refresh the view
  mViewManager->refreshView();
}

void KABCore::redo()
{
  RedoStack::instance()->redo();

  // Refresh the view
  mViewManager->refreshView();
}
void KABCore::setJumpButtonBar( bool visible )
{
    setJumpButtonBarVisible(visible );
    saveSettings();
}
void KABCore::setJumpButtonBarVisible( bool visible )
{
  if (mMultipleViewsAtOnce)
  {
    if ( visible )
      mJumpButtonBar->show();
    else
      mJumpButtonBar->hide();
  }
  else
  {
	// show the jumpbar only if "the details are hidden" == "viewmanager are shown"
    if (mViewManager->isVisible())
    {
      if ( visible )
        mJumpButtonBar->show();
      else
        mJumpButtonBar->hide();
	}
	else
	{
      mJumpButtonBar->hide();
	}
  }
  if ( visible ) {
      if ( mIncSearchWidget->currentItem() == 0 ) {
          message( i18n("Change search field enable jump bar") );
      }
  }
}


void KABCore::setDetailsToState()
{
    setDetailsVisible(  mActionDetails->isChecked() );
}
void KABCore::setDetailsToggle()
{
    mActionDetails->setChecked( !mActionDetails->isChecked() );
    setDetailsToState();
}



void KABCore::setDetailsVisible( bool visible )
{
  if (visible && mDetails->isHidden())
  {
    KABC::Addressee::List addrList = mViewManager->selectedAddressees();
    if ( addrList.count() > 0 )
      mDetails->setAddressee( addrList[ 0 ] );
  }

  // mMultipleViewsAtOnce=false: mDetails is always visible. But we switch between
  // the listview and the detailview. We do that by changing the splitbar size.
  if (mMultipleViewsAtOnce)
  {
    if ( visible )
      mDetails->show();
    else
      mDetails->hide();
  }
  else
  {
    if ( visible ) {
      mViewManager->hide();
      mDetails->show();
      mIncSearchWidget->setFocus();
    }
    else {
      mViewManager->show();
      mDetails->hide();
      mViewManager->setFocusAV();
    }
    setJumpButtonBarVisible( !visible );
  }

}

void KABCore::extensionChanged( int id )
{
  //change the details view only for non desktop systems
#ifndef DESKTOP_VERSION

  if (id == 0)
  {
    //the user disabled the extension.

    if (mMultipleViewsAtOnce)
    { // enable detailsview again
      setDetailsVisible( true );
      mActionDetails->setChecked( true );
    }
    else
    {  //go back to the listview
      setDetailsVisible( false );
      mActionDetails->setChecked( false );
      mActionDetails->setEnabled(true);
	}

  }
  else
  {
    //the user enabled the extension.
    setDetailsVisible( false );
    mActionDetails->setChecked( false );

    if (!mMultipleViewsAtOnce)
    {
      mActionDetails->setEnabled(false);
	}

    mExtensionManager->setSelectionChanged();

  }

#endif// DESKTOP_VERSION

}


void KABCore::extensionModified( const KABC::Addressee::List &list )
{

  if ( list.count() != 0 ) {
    KABC::Addressee::List::ConstIterator it;
    for ( it = list.begin(); it != list.end(); ++it )
      mAddressBook->insertAddressee( *it );
    if ( list.count() > 1 )
        setModified();
    else
        setModifiedWOrefresh();
  }
  if ( list.count() == 0 )
    mViewManager->refreshView();
  else
     mViewManager->refreshView( list[ 0 ].uid() );



}

QString KABCore::getNameByPhone( const QString &phone )
{
#ifndef KAB_EMBEDDED
  QRegExp r( "[/*/-/ ]" );
  QString localPhone( phone );

  bool found = false;
  QString ownerName = "";
  KABC::AddressBook::Iterator iter;
  KABC::PhoneNumber::List::Iterator phoneIter;
  KABC::PhoneNumber::List phoneList;
  for ( iter = mAddressBook->begin(); !found && ( iter != mAddressBook->end() ); ++iter ) {
    phoneList = (*iter).phoneNumbers();
    for ( phoneIter = phoneList.begin(); !found && ( phoneIter != phoneList.end() );
          ++phoneIter) {
      // Get rid of separator chars so just the numbers are compared.
      if ( (*phoneIter).number().replace( r, "" ) == localPhone.replace( r, "" ) ) {
        ownerName = (*iter).formattedName();
        found = true;
      }
    }
  }

  return ownerName;
#else //KAB_EMBEDDED
  qDebug("KABCore::getNameByPhone finsih method");
  return "";
#endif //KAB_EMBEDDED

}
void KABCore::openConfigGlobalDialog()
{
    KPimPrefsGlobalDialog gc ( this );
    gc.exec();
}
void KABCore::openConfigDialog()
{
    KDialogBase * ConfigureDialog = new KDialogBase (  KDialogBase::Plain , i18n("Configure KA/Pi"), KDialogBase::Default |KDialogBase::Cancel | KDialogBase::Apply | KDialogBase::Ok, KDialogBase::Ok,0, "name", true, true);

    KCMKabConfig* kabcfg = new KCMKabConfig( ConfigureDialog , "KCMKabConfig" );
    ConfigureDialog->setMainWidget( kabcfg );
    connect( ConfigureDialog, SIGNAL( applyClicked() ),
           this, SLOT( configurationChanged() ) );
    connect( ConfigureDialog, SIGNAL( applyClicked() ),
            kabcfg, SLOT( save() ) );
    connect( ConfigureDialog, SIGNAL( acceptClicked() ),
             this, SLOT( configurationChanged() ) );
    connect( ConfigureDialog, SIGNAL( acceptClicked() ),
             kabcfg, SLOT( save() ) );
    connect( ConfigureDialog, SIGNAL( defaultClicked() ),
             kabcfg, SLOT( defaults() ) );
    saveSettings();
    kabcfg->load();
#ifndef DESKTOP_VERSION
    if ( QApplication::desktop()->height() <= 480 )
        ConfigureDialog->hideButtons(); 
    ConfigureDialog->showMaximized();
#endif
    if ( ConfigureDialog->exec() )
        KMessageBox::information( this,  i18n("Some changes are only\neffective after a restart!\n") );
    delete ConfigureDialog;
}

void KABCore::openLDAPDialog()
{
#ifndef KAB_EMBEDDED
  if ( !mLdapSearchDialog ) {
    mLdapSearchDialog = new LDAPSearchDialog( mAddressBook, this );
    connect( mLdapSearchDialog, SIGNAL( addresseesAdded() ), mViewManager,
            SLOT( refreshView() ) );
    connect( mLdapSearchDialog, SIGNAL( addresseesAdded() ), this,
            SLOT( setModified() ) );
  } else
    mLdapSearchDialog->restoreSettings();

  if ( mLdapSearchDialog->isOK() )
    mLdapSearchDialog->exec();
#else //KAB_EMBEDDED
  qDebug("KABCore::openLDAPDialog() finsih method");
#endif //KAB_EMBEDDED
}

void KABCore::print()
{
#ifndef KAB_EMBEDDED
  KPrinter printer;
  if ( !printer.setup( this ) )
    return;

  KABPrinting::PrintingWizard wizard( &printer, mAddressBook,
                                      mViewManager->selectedUids(), this );

  wizard.exec();
#else //KAB_EMBEDDED
  qDebug("KABCore::print() finsih method");
#endif //KAB_EMBEDDED

}


void KABCore::addGUIClient( KXMLGUIClient *client )
{
  if ( mGUIClient )
    mGUIClient->insertChildClient( client );
  else
    KMessageBox::error( this, "no KXMLGUICLient");
}


void KABCore::configurationChanged()
{
  mExtensionManager->reconfigure();
}

void KABCore::addressBookChanged()
{
/*US
  QDictIterator<AddresseeEditorDialog> it( mEditorDict );
  while ( it.current() ) {
    if ( it.current()->dirty() ) {
      QString text = i18n( "Data has been changed externally. Unsaved "
                           "changes will be lost." );
      KMessageBox::information( this, text );
    }
    it.current()->setAddressee( mAddressBook->findByUid( it.currentKey() ) );
    ++it;
  }
*/
  if (mEditorDialog)
  {
    if (mEditorDialog->dirty())
    {
      QString text = i18n( "Data has been changed externally. Unsaved "
                           "changes will be lost." );
      KMessageBox::information( this, text );
    }
    QString currentuid = mEditorDialog->addressee().uid();
    mEditorDialog->setAddressee( mAddressBook->findByUid( currentuid ) );
  }
  mViewManager->refreshView();


}

AddresseeEditorDialog *KABCore::createAddresseeEditorDialog( QWidget *parent,
                                                             const char *name )
{

    if ( mEditorDialog == 0 ) {
  mEditorDialog = new AddresseeEditorDialog( this, parent,
                                                 name ? name : "editorDialog" );


  connect( mEditorDialog, SIGNAL( contactModified( const KABC::Addressee& ) ),
           SLOT( contactModified( const KABC::Addressee& ) ) );
  //connect( mEditorDialog, SIGNAL( editorDestroyed( const QString& ) ),
  //       SLOT( slotEditorDestroyed( const QString& ) ) ;
    }

  return mEditorDialog;
}

void KABCore::slotEditorDestroyed( const QString &uid )
{
    //mEditorDict.remove( uid );
}

void KABCore::initGUI()
{
#ifndef KAB_EMBEDDED
  QHBoxLayout *topLayout = new QHBoxLayout( this );
  topLayout->setSpacing( KDialogBase::spacingHint() );

  mExtensionBarSplitter = new QSplitter( this );
  mExtensionBarSplitter->setOrientation( Qt::Vertical );

  mDetailsSplitter = new QSplitter( mExtensionBarSplitter );

  QVBox *viewSpace = new QVBox( mDetailsSplitter );
  mIncSearchWidget = new IncSearchWidget( viewSpace );
  connect( mIncSearchWidget, SIGNAL( doSearch( const QString& ) ),
           SLOT( incrementalSearch( const QString& ) ) );

  mViewManager = new ViewManager( this, viewSpace );
  viewSpace->setStretchFactor( mViewManager, 1 );

  mDetails = new ViewContainer( mDetailsSplitter );

  mJumpButtonBar = new JumpButtonBar( this, this );

  mExtensionManager = new ExtensionManager( this, mExtensionBarSplitter );

  topLayout->addWidget( mExtensionBarSplitter );
  topLayout->setStretchFactor( mExtensionBarSplitter, 100 );
  topLayout->addWidget( mJumpButtonBar );
  //topLayout->setStretchFactor( mJumpButtonBar, 1 );

  mXXPortManager = new XXPortManager( this, this );

#else //KAB_EMBEDDED
  //US initialize viewMenu before settingup viewmanager.
  //  Viewmanager needs this menu to plugin submenues.
  viewMenu = new QPopupMenu( this );
  settingsMenu = new QPopupMenu( this );
  //filterMenu = new QPopupMenu( this );
  ImportMenu = new QPopupMenu( this );
  ExportMenu = new QPopupMenu( this );
  syncMenu = new QPopupMenu( this );
  changeMenu= new QPopupMenu( this );
  beamMenu= new QPopupMenu( this );

//US since we have no splitter for the embedded system, setup
// a layout with two frames. One left and one right.

  QBoxLayout *topLayout;

  // = new QHBoxLayout( this );
//  QBoxLayout *topLayout = (QBoxLayout*)layout();

//  QWidget *mainBox = new QWidget( this );
//  QBoxLayout * mainBoxLayout  = new QHBoxLayout(mainBox);

#ifdef DESKTOP_VERSION
  topLayout = new QHBoxLayout( this );


  mMiniSplitter = new KDGanttMinimizeSplitter( Qt::Horizontal, this);
  mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Right );

  topLayout->addWidget(mMiniSplitter );

  mExtensionBarSplitter = new KDGanttMinimizeSplitter( Qt::Vertical,mMiniSplitter );
  mExtensionBarSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Down );
  mViewManager = new ViewManager( this, mExtensionBarSplitter  );
  mDetails = new ViewContainer( mMiniSplitter );
  mExtensionManager = new ExtensionManager( this, mExtensionBarSplitter );
#else
  if ( QApplication::desktop()->width() > 480 ) {
      topLayout = new QHBoxLayout( this );
      mMiniSplitter = new KDGanttMinimizeSplitter( Qt::Horizontal, this);
      mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Right );
  } else {

      topLayout = new QHBoxLayout( this );
      mMiniSplitter = new KDGanttMinimizeSplitter( Qt::Vertical, this);
      mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Down );
  }

  topLayout->addWidget(mMiniSplitter );
  mViewManager = new ViewManager( this,  mMiniSplitter );
  mDetails = new ViewContainer( mMiniSplitter );


   mExtensionManager = new ExtensionManager( this, mMiniSplitter );
#endif
   //eh->hide();
  // topLayout->addWidget(mExtensionManager );


/*US
#ifndef KAB_NOSPLITTER
  QHBoxLayout *topLayout = new QHBoxLayout( this );
//US  topLayout->setSpacing( KDialogBase::spacingHint() );
  topLayout->setSpacing( 10 );

  mDetailsSplitter = new QSplitter( this );

  QVBox *viewSpace = new QVBox( mDetailsSplitter );

  mViewManager = new ViewManager( this, viewSpace );
  viewSpace->setStretchFactor( mViewManager, 1 );

  mDetails = new ViewContainer( mDetailsSplitter );

  topLayout->addWidget( mDetailsSplitter );
  topLayout->setStretchFactor( mDetailsSplitter, 100 );
#else //KAB_NOSPLITTER
  QHBoxLayout *topLayout = new QHBoxLayout( this );
//US  topLayout->setSpacing( KDialogBase::spacingHint() );
  topLayout->setSpacing( 10 );

//  mDetailsSplitter = new QSplitter( this );

  QVBox *viewSpace = new QVBox( this );

  mViewManager = new ViewManager( this, viewSpace );
  viewSpace->setStretchFactor( mViewManager, 1 );

  mDetails = new ViewContainer( this );

  topLayout->addWidget( viewSpace );
//  topLayout->setStretchFactor( mDetailsSplitter, 100 );
  topLayout->addWidget( mDetails );
#endif //KAB_NOSPLITTER
*/
   mMultiResourceSync = false;
  syncManager = new KSyncManager((QWidget*)this, (KSyncInterface*)this, KSyncManager::KAPI, KABPrefs::instance(), syncMenu);
  syncManager->setBlockSave(false);

    connect(syncManager , SIGNAL( request_file(const QString &) ), this, SLOT( syncFileRequest(const QString &) ) );
    connect(syncManager , SIGNAL( getFile( bool ,const QString &)), this, SLOT(getFile( bool ,const QString &) ) );
    connect(syncManager , SIGNAL( multiResourceSyncStart( bool )), this, SLOT( multiResourceSyncStart( bool ) ) );
    QString sync_file = sentSyncFile();
    //qDebug("KABCore::initGUI()::setting tmp sync file to:%s ",sync_file.latin1());
    syncManager->setDefaultFileName( sync_file );
    //connect(syncManager , SIGNAL( ), this, SLOT( ) );

#endif //KAB_EMBEDDED
  initActions();

#ifdef KAB_EMBEDDED
  addActionsManually();
  //US make sure the export and import menues are initialized before creating the xxPortManager.
  mXXPortManager = new XXPortManager( this, this );

  // LR mIncSearchWidget = new IncSearchWidget( mMainWindow->getIconToolBar() );
  //mMainWindow->toolBar()->insertWidget(-1, 4, mIncSearchWidget);
  // mActionQuit->plug ( mMainWindow->toolBar());
  //mIncSearchWidget = new IncSearchWidget( mMainWindow->toolBar() );
  //mMainWindow->toolBar()->insertWidget(-1, 0, mIncSearchWidget);
  // mIncSearchWidget->hide();
  connect( mIncSearchWidget, SIGNAL( doSearch( const QString& ) ),
           SLOT( incrementalSearch( const QString& ) ) );
  connect( mIncSearchWidget, SIGNAL( scrollUP() ),mViewManager, SLOT( scrollUP() ) );
  connect( mIncSearchWidget, SIGNAL( scrollDOWN() ),mViewManager, SLOT( scrollDOWN() ) );

  mJumpButtonBar = new JumpButtonBar( this, this );

  topLayout->addWidget( mJumpButtonBar );
//US  topLayout->setStretchFactor( mJumpButtonBar, 10 );

// mMainWindow->getIconToolBar()->raise();

#endif //KAB_EMBEDDED

}
void KABCore::multiResourceSyncStart( bool start )
{
    mMultiResourceSync = start;
}
void KABCore::initActions()
{
//US  qDebug("KABCore::initActions(): mIsPart %i", mIsPart);

#ifndef KAB_EMBEDDED
  connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
           SLOT( clipboardDataChanged() ) );
#endif //KAB_EMBEDDED

  // file menu
 
    mActionMail = KStdAction::mail( this, SLOT( sendMail() ), actionCollection() );
    //mActionPrint = KStdAction::print( this, SLOT( print() ), actionCollection() );
    mActionPrint = new KAction( i18n( "&Print View" ), "fileprint", CTRL + Key_P, mViewManager,
                 SLOT( printView() ), actionCollection(), "kaddressbook_print" );
  

    mActionPrintDetails = new KAction( i18n( "&Print Details" ), "fileprint", 0, mDetails,
                 SLOT( printView() ), actionCollection(), "kaddressbook_print2" );

  mActionSave = new KAction( i18n( "&Save" ), "filesave", CTRL+Key_S, this,
                             SLOT( save() ), actionCollection(), "file_sync" );

  mActionNewContact = new KAction( i18n( "&New Contact..." ), "filenew", CTRL+Key_N, this,
                             SLOT( newContact() ), actionCollection(), "file_new_contact" );

  mActionMailVCard = new KAction(i18n("Mail &vCard..."), "mail_post_to", 0,
                                 this, SLOT( mailVCard() ),
                                 actionCollection(), "file_mail_vcard");

  mActionExport2phone = new KAction( i18n( "Export to phone" ), "ex2phone", 0, this,
                               SLOT( export2phone() ), actionCollection(),
                               "kaddressbook_ex2phone" );

  mActionBeamVCard = 0;
 mActionBeam = 0;

#ifndef DESKTOP_VERSION
  if ( Ir::supported() ) {
    mActionBeamVCard = new KAction( i18n( "Beam v&Card(s)..." ), "beam", 0, this,
                               SLOT( beamVCard() ), actionCollection(),
                               "kaddressbook_beam_vcard" );

    mActionBeam = new KAction( i18n( "&Beam personal vCard" ), "beam", 0, this,
                               SLOT( beamMySelf() ), actionCollection(),
                               "kaddressbook_beam_myself" );
  }
#endif

  mActionEditAddressee = new KAction( i18n( "&Edit Contact..." ), "edit", 0,
                                      this, SLOT( editContact2() ),
                                      actionCollection(), "file_properties" );

#ifdef KAB_EMBEDDED
  // mActionQuit = KStdAction::quit( mMainWindow, SLOT( exit() ), actionCollection() );
  mActionQuit = new KAction( i18n( "&Exit" ), "exit", 0,
                                      mMainWindow, SLOT( exit() ),
                                      actionCollection(), "quit" );
#endif //KAB_EMBEDDED

  // edit menu
  if ( mIsPart ) {
    mActionCopy = new KAction( i18n( "&Copy" ), "editcopy", CTRL + Key_C, this,
                               SLOT( copyContacts() ), actionCollection(),
                               "kaddressbook_copy" );
    mActionCut = new KAction( i18n( "Cu&t" ), "editcut", CTRL + Key_X, this,
                              SLOT( cutContacts() ), actionCollection(),
                              "kaddressbook_cut" );
    mActionPaste = new KAction( i18n( "&Paste" ), "editpaste", CTRL + Key_V, this,
                                SLOT( pasteContacts() ), actionCollection(),
                                "kaddressbook_paste" );
    mActionSelectAll = new KAction( i18n( "Select &All" ), CTRL + Key_A, this,
                  SLOT( selectAllContacts() ), actionCollection(),
                 "kaddressbook_select_all" );
    mActionUndo = new KAction( i18n( "&Undo" ), "undo", CTRL + Key_Z, this,
                               SLOT( undo() ), actionCollection(),
                               "kaddressbook_undo" );
    mActionRedo = new KAction( i18n( "Re&do" ), "redo", CTRL + SHIFT + Key_Z,
                               this, SLOT( redo() ), actionCollection(),
                               "kaddressbook_redo" );
  } else {
    mActionCopy = KStdAction::copy( this, SLOT( copyContacts() ), actionCollection() );
    mActionCut = KStdAction::cut( this, SLOT( cutContacts() ), actionCollection() );
    mActionPaste = KStdAction::paste( this, SLOT( pasteContacts() ), actionCollection() );
    mActionSelectAll = KStdAction::selectAll( this, SLOT( selectAllContacts() ), actionCollection() );
    mActionUndo = KStdAction::undo( this, SLOT( undo() ), actionCollection() );
    mActionRedo = KStdAction::redo( this, SLOT( redo() ), actionCollection() );
  }

  mActionDelete = new KAction( i18n( "&Delete Contact" ), "editdelete",
                               Key_Delete, this, SLOT( deleteContacts() ),
                               actionCollection(), "edit_delete" );

  mActionUndo->setEnabled( false );
  mActionRedo->setEnabled( false );

  // settings menu
#ifdef KAB_EMBEDDED
//US special menuentry to configure the addressbook resources. On KDE
// you do that through the control center !!!
  mActionConfigResources = new KAction( i18n( "Configure &Resources..." ), "configure_resources", 0, this,
               SLOT( configureResources() ), actionCollection(),
               "kaddressbook_configure_resources" );
#endif //KAB_EMBEDDED

  if ( mIsPart ) {
    mActionConfigKAddressbook = new KAction( i18n( "&Configure KAddressBook..." ), "configure", 0, this,
                 SLOT( openConfigDialog() ), actionCollection(),
                 "kaddressbook_configure" );

    //US not implemented yet
    //mActionConfigShortcuts = new KAction( i18n( "Configure S&hortcuts..." ), "configure_shortcuts", 0,
    //             this, SLOT( configureKeyBindings() ), actionCollection(),
    //             "kaddressbook_configure_shortcuts" );
#ifdef KAB_EMBEDDED
  mActionConfigureToolbars = KStdAction::configureToolbars( this, SLOT( mMainWindow->configureToolbars() ), actionCollection() );
  mActionConfigureToolbars->setEnabled( false );
#endif //KAB_EMBEDDED

  } else {
      mActionConfigKAddressbook = new KAction( i18n( "&Configure KA/Pi..." ), "configure", 0, this,
                                               SLOT( openConfigDialog() ), actionCollection(),
                                               "kaddressbook_configure" );  
      mActionConfigGlobal = new KAction( i18n( "Global Settings..." ), "configure", 0, this,
                                         SLOT( openConfigGlobalDialog() ), actionCollection(),
                                         "kaddressbook_configure" );
  }
  mActionJumpBar = new KToggleAction( i18n( "Show Jump Bar" ), 0, 0,
                                      actionCollection(), "options_show_jump_bar" );
  connect( mActionJumpBar, SIGNAL( toggled( bool ) ), SLOT( setJumpButtonBar( bool ) ) );

  mActionDetails = new KToggleAction( i18n( "Show Details" ), "listview", 0,
                                      actionCollection(), "options_show_details" );
  connect( mActionDetails, SIGNAL( toggled( bool ) ), SLOT( setDetailsVisible( bool ) ) );


  mActionBR = new KToggleAction( i18n( "Beam receive enabled" ), "beam", 0, this,
                               SLOT( toggleBeamReceive() ), actionCollection(),
                               "kaddressbook_beam_rec" );


  // misc
  // only enable LDAP lookup if we can handle the protocol
#ifndef KAB_EMBEDDED
  if ( KProtocolInfo::isKnownProtocol( KURL( "ldap://localhost" ) ) ) {
    new KAction( i18n( "&Lookup Addresses in Directory" ), "find", 0,
                 this, SLOT( openLDAPDialog() ), actionCollection(),
                 "ldap_lookup" );
  }
#else //KAB_EMBEDDED
  //qDebug("KABCore::initActions() LDAP has to be implemented");
#endif //KAB_EMBEDDED


  mActionWhoAmI = new KAction( i18n( "Set Who Am I" ), "personal", 0, this,
                               SLOT( setWhoAmI() ), actionCollection(),
                               "set_personal" );


  mActionCategories = new KAction( i18n( "Set Categories for Contacts..." ), 0, this,
                                   SLOT( setCategories() ), actionCollection(),
                                   "edit_set_categories" );
  mActionEditCategories = new KAction( i18n( "Edit Category List..." ), 0, this,
                                   SLOT( editCategories() ), actionCollection(),
                                   "edit__categories" );

  mActionRemoveVoice = new KAction( i18n( "Remove \"voice\"..." ), 0, this,
                                    SLOT( removeVoice() ), actionCollection(),
                                   "remove_voice" );
  mActionSetFormattedName = new KAction( i18n( "Set formatted name..." ), 0, this,
                                    SLOT( setFormattedName() ), actionCollection(),
                                   "set_formatted" );

  mActionManageCategories= new KAction( i18n( "Manage new categories..." ), 0, this,
                                    SLOT( manageCategories() ), actionCollection(),
                                   "remove_voice" );


  mActionImportOL = new KAction( i18n( "Import from Outlook..." ), 0, this,
                                    SLOT( importFromOL() ), actionCollection(),
                                   "import_OL" );
#ifdef KAB_EMBEDDED
 mActionLicence = new KAction( i18n( "Licence" ), 0,
                 this, SLOT( showLicence() ), actionCollection(),
                 "licence_about_data" );
 mActionFaq = new KAction( i18n( "Faq" ), 0,
                 this, SLOT( faq() ), actionCollection(),
                 "faq_about_data" );
 mActionWN = new KAction( i18n( "What's New?" ), 0,
                 this, SLOT( whatsnew() ), actionCollection(),
                 "wn" );


  
 mActionStorageHowto = new KAction( i18n( "Storage HowTo" ), 0,
                 this, SLOT( storagehowto() ), actionCollection(),
                 "storage" );
  
 mActionSyncHowto = new KAction( i18n( "Sync HowTo" ), 0,
                 this, SLOT( synchowto() ), actionCollection(),
                 "sync" );
 mActionKdeSyncHowto = new KAction( i18n( "Kde Sync HowTo" ), 0,
                 this, SLOT( kdesynchowto() ), actionCollection(),
                 "kdesync" );
 mActionMultiSyncHowto = new KAction( i18n( "Multi Sync HowTo" ), 0,
                 this, SLOT( multisynchowto() ), actionCollection(),
                 "multisync" );

  mActionAboutKAddressbook = new KAction( i18n( "&About KAddressBook" ), "kaddressbook2", 0,
                 this, SLOT( createAboutData() ), actionCollection(),
                 "kaddressbook_about_data" );
#endif //KAB_EMBEDDED

  clipboardDataChanged();
  connect( UndoStack::instance(), SIGNAL( changed() ), SLOT( updateActionMenu() ) );
  connect( RedoStack::instance(), SIGNAL( changed() ), SLOT( updateActionMenu() ) );
}

//US we need this function, to plug all actions into the correct menues.
// KDE uses a XML format to plug the actions, but we work her without this overhead.
void KABCore::addActionsManually()
{
//US  qDebug("KABCore::initActions(): mIsPart %i", mIsPart);

#ifdef KAB_EMBEDDED
  QPopupMenu *fileMenu = new QPopupMenu( this );
  QPopupMenu *editMenu = new QPopupMenu( this );
  QPopupMenu *helpMenu = new QPopupMenu( this );

  KToolBar* tb =  mMainWindow->toolBar();
  mMainWindow->setToolBarsMovable (false ); 
#ifndef DESKTOP_VERSION
  if ( KABPrefs::instance()->mFullMenuBarVisible ) {
#endif
      QMenuBar* mb = mMainWindow->menuBar();

      //US setup menubar.
      //Disable the following block if you do not want to have a menubar.
      mb->insertItem( i18n("&File"), fileMenu );
      mb->insertItem( i18n("&Edit"), editMenu );
      mb->insertItem( i18n("&View"), viewMenu );
      mb->insertItem( i18n("&Settings"), settingsMenu );
#ifdef DESKTOP_VERSION
      mb->insertItem( i18n("Synchronize"), syncMenu );
#else
      mb->insertItem( i18n("Sync"), syncMenu );
#endif
      //mb->insertItem( i18n("&Change"), changeMenu );
      mb->insertItem( i18n("&Help"), helpMenu );
      mIncSearchWidget = new IncSearchWidget( tb );
      // tb->insertWidget(-1, 0, mIncSearchWidget);
#ifndef DESKTOP_VERSION
  } else  {
      //US setup toolbar
      QPEMenuBar   *menuBarTB = new QPEMenuBar( tb );
      QPopupMenu *popupBarTB = new QPopupMenu( this );
      menuBarTB->insertItem( SmallIcon( "z_menu" ) , popupBarTB);
      tb->insertWidget(-1, 0, menuBarTB);
      mIncSearchWidget = new IncSearchWidget( tb );
      tb->enableMoving(false);
      popupBarTB->insertItem( i18n("&File"), fileMenu );
      popupBarTB->insertItem( i18n("&Edit"), editMenu );
      popupBarTB->insertItem( i18n("&View"), viewMenu );
      popupBarTB->insertItem( i18n("&Settings"), settingsMenu );
      popupBarTB->insertItem( i18n("Synchronize"), syncMenu );
      mViewManager->getFilterAction()->plug ( popupBarTB);
      //popupBarTB->insertItem( i18n("&Change selected"), changeMenu );
      popupBarTB->insertItem( i18n("&Help"), helpMenu );
      if (QApplication::desktop()->width() > 320 ) {
          // mViewManager->getFilterAction()->plug ( tb);
      }
  }
#endif
  mIncSearchWidget->setSize();
  // mActionQuit->plug ( mMainWindow->toolBar());



  //US Now connect the actions with the menue entries.
#ifdef DESKTOP_VERSION
  mActionPrint->plug( fileMenu );
  mActionPrintDetails->plug( fileMenu );
  fileMenu->insertSeparator();
#endif
  mActionMail->plug( fileMenu );
  fileMenu->insertSeparator();

  mActionNewContact->plug( editMenu );
  mActionNewContact->plug( tb );

  mActionEditAddressee->plug( editMenu );
  editMenu->insertSeparator();
  // if ((KGlobal::getDesktopSize() > KGlobal::Small ) ||
  //  (!KABPrefs::instance()->mMultipleViewsAtOnce ))
    mActionEditAddressee->plug( tb );

    // fileMenu->insertSeparator();
  mActionSave->plug( fileMenu );
  fileMenu->insertItem( "&Import", ImportMenu );
  fileMenu->insertItem( "&Export", ExportMenu );
  editMenu->insertItem( i18n("&Change"), changeMenu );
  editMenu->insertSeparator();
#ifndef DESKTOP_VERSION
  if ( Ir::supported() )  fileMenu->insertItem( i18n("&Beam"), beamMenu );
#endif
#if 0 
  // PENDING fix MailVCard
  fileMenu->insertSeparator();
  mActionMailVCard->plug( fileMenu );
#endif
#ifndef DESKTOP_VERSION
  if ( Ir::supported() ) mActionBR->plug( beamMenu );
  if ( Ir::supported() ) mActionBeamVCard->plug( beamMenu );
  if ( Ir::supported() ) mActionBeam->plug( beamMenu );
#endif
  fileMenu->insertSeparator();
  mActionQuit->plug( fileMenu );
#ifdef _OL_IMPORT_
  mActionImportOL->plug( ImportMenu  );
#endif
  // edit menu
  mActionUndo->plug( editMenu );
  mActionRedo->plug( editMenu );
  editMenu->insertSeparator();
  mActionCut->plug( editMenu );
  mActionCopy->plug( editMenu );
  mActionPaste->plug( editMenu );
  mActionDelete->plug( editMenu );
  editMenu->insertSeparator();
  mActionSelectAll->plug( editMenu );

  mActionSetFormattedName->plug( changeMenu );
  mActionRemoveVoice->plug( changeMenu );
  // settingsmings menu
//US special menuentry to configure the addressbook resources. On KDE
// you do that through the control center !!!
  // settingsMenu->insertSeparator();

  mActionConfigKAddressbook->plug( settingsMenu, 0 );
  mActionConfigGlobal->plug( settingsMenu, 1 );
  mActionConfigResources->plug( settingsMenu,2 );
  settingsMenu->insertSeparator(3);

  if ( mIsPart ) {
    //US not implemented yet
    //mActionConfigShortcuts->plug( settingsMenu );
    //mActionConfigureToolbars->plug( settingsMenu );

  } else {
    //US not implemented yet
    //mActionKeyBindings->plug( settingsMenu );
  }

  mActionEditCategories->plug( settingsMenu );
  mActionManageCategories->plug( settingsMenu );
  mActionJumpBar->plug( viewMenu,0 );
  mActionDetails->plug( viewMenu,0 );
  //if (!KABPrefs::instance()->mMultipleViewsAtOnce || KGlobal::getDesktopSize() == KGlobal::Desktop  )
  mActionDetails->plug( tb );
  settingsMenu->insertSeparator();
#ifndef DESKTOP_VERSION
  if ( Ir::supported() ) mActionBR->plug(settingsMenu );
  settingsMenu->insertSeparator();
#endif

  mActionWhoAmI->plug( settingsMenu );
  //mActionEditCategories->plug( changeMenu );
  mActionCategories->plug( changeMenu );
  //mActionManageCategories->plug( changeMenu );

  //mActionCategories->plug( settingsMenu );

     
  mActionWN->plug( helpMenu );
  mActionStorageHowto->plug( helpMenu );
  mActionSyncHowto->plug( helpMenu );
  mActionKdeSyncHowto->plug( helpMenu );
  mActionMultiSyncHowto->plug( helpMenu );
  mActionFaq->plug( helpMenu );
  mActionLicence->plug( helpMenu );
  mActionAboutKAddressbook->plug( helpMenu );

  if (KGlobal::getDesktopSize() > KGlobal::Small ) {

      mActionSave->plug( tb );
      mViewManager->getFilterAction()->plug ( tb);
      //LR hide filteraction on started in 480x640
      if (QApplication::desktop()->width() == 480 ) {
          mViewManager->getFilterAction()->setComboWidth( 0 );
      }
      mActionUndo->plug( tb );
      mActionDelete->plug( tb );
      mActionRedo->plug( tb );
  } else {
      mActionSave->plug( tb );
      tb->enableMoving(false);
  }
  //mActionQuit->plug ( tb );
  //tb->insertWidget(-1, 0, mIncSearchWidget, 6);

  //US link the searchwidget first to this.
  // The real linkage to the toolbar happens later.
//US  mIncSearchWidget->reparent(tb, 0, QPoint(50,0), TRUE);
//US  tb->insertItem( mIncSearchWidget );
/*US
  mIncSearchWidget = new IncSearchWidget( tb );
  connect( mIncSearchWidget, SIGNAL( doSearch( const QString& ) ),
           SLOT( incrementalSearch( const QString& ) ) );

  mJumpButtonBar = new JumpButtonBar( this, this );

//US topLayout->addWidget( mJumpButtonBar );
 this->layout()->add( mJumpButtonBar );
*/

#endif //KAB_EMBEDDED

  mActionExport2phone->plug( ExportMenu );
 connect ( syncMenu, SIGNAL( activated ( int )  ), syncManager, SLOT (slotSyncMenu( int ) ) );
 syncManager->fillSyncMenu();

}
void KABCore::showLicence()
{
    KApplication::showLicence();
}

void KABCore::manageCategories( )
{
    KABCatPrefs* cp = new KABCatPrefs(); 
    cp->show();
    int w =cp->sizeHint().width() ;
    int h = cp->sizeHint().height() ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    cp->setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    if ( !cp->exec() ) {
        delete cp;
        return;
    }
    int count = 0;
    int cc = 0;
    message( i18n("Please wait, processing categories..."));
    if ( cp->addCat() ) {
        KABC::AddressBook::Iterator it;
        QStringList catList = KABPrefs::instance()->mCustomCategories;
        for( it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) { 
            ++cc;
            if ( cc %10 == 0)
                message(i18n("Processing contact #%1").arg(cc));
            QStringList catIncList = (*it).categories();
            int i;
            for( i = 0; i<   catIncList.count(); ++i ) {
                if ( !catList.contains (catIncList[i])) {
                    catList.append( catIncList[i] );
                    //qDebug("add cat %s ",  catIncList[i].latin1());
                    ++count;
                }
            }
        }
        catList.sort();
        KABPrefs::instance()->mCustomCategories = catList;
        KABPrefs::instance()->writeConfig();
        message(QString::number( count )+ i18n(" categories added to list! "));
    } else {
        QStringList catList = KABPrefs::instance()->mCustomCategories;
        QStringList catIncList;
        QStringList newCatList;
        KABC::AddressBook::Iterator it;
        for( it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) { 
            ++cc;
            if ( cc %10 == 0)
                message(i18n("Processing contact #%1").arg(cc));
            QStringList catIncList = (*it).categories();
            int i;
            if ( catIncList.count() ) {
                newCatList.clear();
                for( i = 0; i<   catIncList.count(); ++i ) {
                    if ( catList.contains (catIncList[i])) {
                        newCatList.append( catIncList[i] );
                    }
                }
                newCatList.sort();
                (*it).setCategories( newCatList );
                mAddressBook->insertAddressee( (*it) );
            }
        } 
        setModified( true );
        mViewManager->refreshView();
        message( i18n("Removing categories done!"));
    }
    delete cp;
}
void KABCore::removeVoice()
{
    if ( KMessageBox::questionYesNo( this, i18n("After importing, phone numbers\nmay have two or more types.\n(E.g. work+voice)\nThese numbers are shown as \"other\".\nClick Yes to remove the voice type\nfrom numbers with more than one type.\n\nRemove voice type?") ) == KMessageBox::No )
        return;
    XXPortSelectDialog dlg( this, false, this );
    if ( !dlg.exec() )
        return;
    mAddressBook->setUntagged();
    dlg.tagSelected();
    message(i18n("Removing voice..."), false );
    KABC::AddressBook::Iterator it;
    for ( it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) {
        if ( (*it).tagged() ) {
            (*it).removeVoice();
        }
    }
    message(i18n("Refreshing view...") );
    mViewManager->refreshView( "" );
    Addressee add;
    mDetails->setAddressee( add );
    message(i18n("Remove voice completed!") );



}

void KABCore::setFormattedName()
{
    KABFormatPrefs setpref;
    if ( !setpref.exec() ) {
        return;
    }
    XXPortSelectDialog dlg( this, false, this );
    if ( !dlg.exec() )
        return;
    mAddressBook->setUntagged();
    dlg.tagSelected();
    int count = 0;
    KABC::AddressBook::Iterator it;
    bool modified = false;
    for ( it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) {
        if ( (*it).tagged() ) { 
            if ( (*it).uid().left( 2 ) == "la" )
                if ( (*it).uid().left( 19 ) == QString("last-syncAddressee-") )
                    continue;
            ++count;
            if ( count %10 == 0 )
                message(i18n("Changing contact #%1").arg( count ) );
            QString fName;
            if ( setpref.simple->isChecked() )
                fName = NameEditDialog::formattedName( (*it), NameEditDialog::SimpleName );
            else if ( setpref.full->isChecked() )
                fName = NameEditDialog::formattedName( (*it), NameEditDialog::FullName );
            else if ( setpref.reverse->isChecked() )
                fName = NameEditDialog::formattedName( (*it), NameEditDialog::ReverseName );
            else 
                fName = (*it).organization();
            if ( setpref.setCompany->isChecked() )
                if ( fName.isEmpty() || fName =="," )
                    fName = (*it).organization();
            (*it).setFormattedName( fName );
            (*it).setChanged( true );
            modified = true;
            (*it).setRevision( QDateTime::currentDateTime() ); 
        }
    }
    message(i18n("Refreshing view...") );
    if ( modified )
        setModified( true );
    Addressee add;
    mDetails->setAddressee( add );
    if ( count == 0 )
        message(i18n("No contact changed!") );
    else
        message(i18n("%1 contacts changed!").arg( count ) );
}

void KABCore::clipboardDataChanged()
{

  if ( mReadWrite )
    mActionPaste->setEnabled( !QApplication::clipboard()->text().isEmpty() );

}

void KABCore::updateActionMenu()
{
  UndoStack *undo = UndoStack::instance();
  RedoStack *redo = RedoStack::instance();

  if ( undo->isEmpty() )
    mActionUndo->setText( i18n( "Undo" ) );
  else
    mActionUndo->setText( i18n( "Undo %1" ).arg( undo->top()->name() ) );

  mActionUndo->setEnabled( !undo->isEmpty() );

  if ( !redo->top() )
    mActionRedo->setText( i18n( "Redo" ) );
  else
    mActionRedo->setText( i18n( "Redo %1" ).arg( redo->top()->name() ) );

  mActionRedo->setEnabled( !redo->isEmpty() );
}

void KABCore::configureKeyBindings()
{
#ifndef KAB_EMBEDDED
  KKeyDialog::configure( actionCollection(), true );
#else //KAB_EMBEDDED
  qDebug("KABCore::configureKeyBindings() not implemented");
#endif //KAB_EMBEDDED
}

#ifdef KAB_EMBEDDED
void KABCore::configureResources()
{
  KRES::KCMKResources dlg( this, "" , 0 );

  if ( !dlg.exec() )
    return;
  KMessageBox::information( this,  i18n("Please restart to get the \nchanged resources (re)loaded!\n") );
}
#endif //KAB_EMBEDDED


/* this method will be called through the QCop interface from Ko/Pi to select addresses
 * for the attendees list of an event.
 */
void KABCore::requestForNameEmailUidList(const QString& sourceChannel, const QString& uid)
{
    qDebug("KABCore::requestForNameEmailUidList ");
  bool ok = false;
  mEmailSourceChannel = sourceChannel;
  mEmailSourceUID = uid; 
  QTimer::singleShot( 10,this, SLOT ( callContactdialog() ) );
    //callContactdialog();
#if 0
  int wid = uid.toInt( &ok );
  qDebug("UID %s ", uid.latin1());
  if ( ok ) {
      if ( wid != QApplication::desktop()->width() ) {
          qDebug("KA/Pi: Request from different desktop geometry. Resizing ...");
          message( i18n("Resizing, please wait...") );
          mMainWindow->showMinimized();
          /*
          {
              QCopEnvelope e("QPE/Application/kapi", "callContactdialog()");
          }
          */
          QTimer::singleShot( 1,this, SLOT ( resizeAndCallContactdialog() ) );
          return;
      }

  } else {
      qDebug("KABCore::requestForNameEmailUidList:: Got invalid uid ");
  }
  callContactdialog();
  //QCopEnvelope e("QPE/Application/kapi", "callContactdialog()");
#endif
}
void KABCore::resizeAndCallContactdialog()
{
    updateMainWindow();
    QTimer::singleShot( 10,this, SLOT ( callContactdialog() ) );
}

void KABCore::doRingSync()
{
    topLevelWidget()->raise();
    syncManager->multiSync( false );
}
void KABCore::callContactdialog()
{
    static bool running = false;
    if (running) return;
    running = true;
  QStringList nameList;
  QStringList emailList;
  QStringList uidList; 
  qDebug(" KABCore::callContactdialog:DESKTOP WIDTH %d ",   QApplication::desktop()->width()       );
  KABC::Addressee::List list = KABC::AddresseeDialog::getAddressees(this);
  uint i=0;
  for (i=0; i < list.count(); i++)
  {
	nameList.append(list[i].realName());
	emailList.append(list[i].preferredEmail());
	uidList.append(list[i].uid());
  }
  QString uid = mEmailSourceUID;
  //qDebug("%s %s ", sourceChannel.latin1(), uid.latin1());
  bool res = ExternalAppHandler::instance()->returnNameEmailUidListFromKAPI(mEmailSourceChannel, uid, nameList, emailList, uidList);
  running = false;
}
/* this method will be called through the QCop interface from Ko/Pi to select birthdays
 * to put them into the calendar.
 */
void KABCore::requestForBirthdayList(const QString& sourceChannel, const QString& uid)
{
  //  qDebug("KABCore::requestForBirthdayList");
  QStringList birthdayList;
  QStringList anniversaryList;
  QStringList realNameList;
  QStringList preferredEmailList;
  QStringList assembledNameList;
  QStringList uidList;

  KABC::AddressBook::Iterator it;

    int count = 0;
    for( it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) { 
        ++count;
    }
    QProgressBar bar(count,0 );
    int w = 300;
    if ( QApplication::desktop()->width() < 320 )
        w = 220;
    int h = bar.sizeHint().height() ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    bar.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    bar.show();
    bar.setCaption (i18n("Collecting birthdays - close to abort!") );
    qApp->processEvents();

    QDate bday;
    QString anni;
    QString formattedbday;

  for( it = mAddressBook->begin(); it != mAddressBook->end(); ++it )  
  {
        if ( ! bar.isVisible() )
            return;
        bar.setProgress( count++ );
        qApp->processEvents();
	bday = (*it).birthday().date();
	anni = (*it).custom("KADDRESSBOOK", "X-Anniversary" );

        if ( bday.isValid() || !anni.isEmpty())
	{
	  if (bday.isValid()) 
	    formattedbday = KGlobal::locale()->formatDate(bday,  true, KLocale::ISODate);
	  else
	    formattedbday = "NOTVALID";
	  if (anni.isEmpty())
	    anni = "INVALID";
	    
	  birthdayList.append(formattedbday);
	  anniversaryList.append(anni); //should be ISODate
	  realNameList.append((*it).realName());
	  preferredEmailList.append((*it).preferredEmail());
	  assembledNameList.append((*it).realName());
	  uidList.append((*it).uid());

	  //qDebug("found birthday in KA/Pi: %s,%s,%s,%s: %s, %s", (*it).realName().latin1(), (*it).preferredEmail().latin1(), (*it).assembledName().latin1(), (*it).uid().latin1(), formattedbday.latin1(), anni.latin1() );
	}
  }

  bool res = ExternalAppHandler::instance()->returnBirthdayListFromKAPI(sourceChannel, uid, birthdayList, anniversaryList, realNameList, preferredEmailList, assembledNameList, uidList);

}

/* this method will be called through the QCop interface from other apps to show details of a contact.
 */
void KABCore::requestForDetails(const QString& sourceChannel, const QString& sessionuid, const QString& name, const QString& email, const QString& uid)
{
    //qDebug("KABCore::requestForDetails %s %s %s %s %s", sourceChannel.latin1(), sessionuid.latin1(), name.latin1(), email.latin1(), uid.latin1());

    QString foundUid = QString::null;
    if ( ! uid.isEmpty() ) {
        Addressee adrr = mAddressBook->findByUid( uid );
        if (  !adrr.isEmpty() ) {
            foundUid = uid;
        }
            if ( email == "sendbacklist" ) {
                //qDebug("ssssssssssssssssssssssend ");
                QStringList nameList;
                QStringList emailList;
                QStringList uidList;
                nameList.append(adrr.realName());
                emailList = adrr.emails();
                uidList.append( adrr.preferredEmail());
                bool res = ExternalAppHandler::instance()->returnNameEmailUidListFromKAPI("QPE/Application/ompi", uid, nameList, emailList, uidList);
                return;
            }
        
    }
  
  if ( email == "sendbacklist" ) 
      return;
  if (foundUid.isEmpty())
  {
    //find the uid of the person first
    Addressee::List namelist;
    Addressee::List emaillist;

    if (!name.isEmpty())
      namelist = mAddressBook->findByName( name );

    if (!email.isEmpty())
      emaillist = mAddressBook->findByEmail( email );
    //qDebug("count %d %d ",  namelist.count(),emaillist.count() );
    //check if we have a match in Namelist and Emaillist
    if ((namelist.count() == 0) && (emaillist.count() > 0)) {
      foundUid = emaillist[0].uid();
    }
    else if ((namelist.count() > 0) && (emaillist.count() == 0))
      foundUid = namelist[0].uid();
    else
    {
      for (int i = 0; i < namelist.count(); i++)
      {
        for (int j = 0; j < emaillist.count(); j++)
        {
          if (namelist[i] == emaillist[j])
          {
            foundUid = namelist[i].uid();
          }
        }
      }
    }
  }
  else
  {
    foundUid = uid;
  }

  if (!foundUid.isEmpty())
  {
   
    // raise Ka/Pi if it is in the background
#ifndef DESKTOP_VERSION
#ifndef KORG_NODCOP
      //QCopEnvelope e("QPE/Application/kapi", "raise()");
#endif
#endif
    
    mMainWindow->showMaximized();
    mMainWindow-> raise();

    mViewManager->setSelected( "", false);
    mViewManager->refreshView( "" );
    mViewManager->setSelected( foundUid, true );
    mViewManager->refreshView( foundUid );

    if ( !mMultipleViewsAtOnce )
    {
      setDetailsVisible( true );
      mActionDetails->setChecked(true);
    }
  }
}
void KABCore::storagehowto()
{
    KApplication::showFile( "KDE-Pim/Pi Storage HowTo", "kdepim/storagehowto.txt" );
}
void KABCore::whatsnew()
{
    KApplication::showFile( "KDE-Pim/Pi Version Info", "kdepim/WhatsNew.txt" );
}
void KABCore::synchowto()
{
    KApplication::showFile( "KDE-Pim/Pi Synchronization HowTo", "kdepim/SyncHowto.txt" );
}
void KABCore::kdesynchowto()
{
    KApplication::showFile( "KDE-Pim/Pi Synchronization HowTo", "kdepim/Zaurus-KDE_syncHowTo.txt" );
}
void KABCore::multisynchowto()
{
    KApplication::showFile( "KDE-Pim/Pi Synchronization HowTo", "kdepim/MultiSyncHowTo.txt" );
}
void  KABCore::faq()
{
  KApplication::showFile( "KA/Pi FAQ", "kdepim/kaddressbook/kapiFAQ.txt" );
}

#include <libkcal/syncdefines.h>

QString KABCore::getLastSyncDT4device( QString syncDevice )
{
    Addressee lse; 
    lse = mAddressBook->findByUid( "last-syncAddressee-" + syncDevice ); 
    if (lse.isEmpty() )
      return i18n("Never synced before");
    return KGlobal::locale()->formatDateTime( lse.revision(), true);
}


KABC::Addressee KABCore::getLastSyncAddressee()
{
    Addressee lse; 
    QString mCurrentSyncDevice = syncManager->getCurrentSyncDevice();

    //qDebug("CurrentSyncDevice %s ",mCurrentSyncDevice .latin1() );
    lse =   mAddressBook->findByUid( "last-syncAddressee-"+mCurrentSyncDevice );
    if (lse.isEmpty()) {
        qDebug("KA: Creating new last-syncAddressee ");
        lse.setUid( "last-syncAddressee-"+mCurrentSyncDevice ); 
        QString sum = "";
        if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL )
            sum = "E: ";
        lse.setFamilyName("!"+sum+mCurrentSyncDevice + i18n(" - sync event"));
        lse.setRevision( mLastAddressbookSync );
        lse.setCategories( i18n("SyncEvent") );
        mAddressBook->insertAddressee( lse );
    }
    return lse;
}
int  KABCore::takeAddressee( KABC::Addressee* local,  KABC::Addressee* remote, int mode , bool full )
{
 
    //void setZaurusId(int id);
    // int zaurusId() const;
    //  void setZaurusUid(int id);
   // int zaurusUid() const;
   // void setZaurusStat(int id);
   // int zaurusStat() const;
    // 0 equal
    // 1 take local
    // 2 take remote
    // 3 cancel 
    QDateTime lastSync = mLastAddressbookSync;
    QDateTime localMod = local->revision();
    QDateTime remoteMod = remote->revision();

    QString mCurrentSyncDevice = syncManager->getCurrentSyncDevice();

    if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
        bool remCh, locCh;
        remCh = ( remote->getCsum(mCurrentSyncDevice) != local->getCsum(mCurrentSyncDevice) );
        //if ( remCh )
        //  qDebug("loc %s  rem %s", local->getCsum(mCurrentSyncDevice).latin1(), remote->getCsum(mCurrentSyncDevice).latin1() );
        locCh = ( localMod > mLastAddressbookSync );
        //qDebug("cahnged rem %d loc %d",remCh, locCh  );
        if ( !remCh && ! locCh ) {
            //qDebug("both not changed ");
            lastSync = localMod.addDays(1);
            if ( mode <= SYNC_PREF_ASK )
                return 0;
        } else {
            if ( locCh ) {
                //qDebug("loc changed %s %s",  localMod.toString().latin1(), mLastAddressbookSync.toString().latin1());
                lastSync = localMod.addDays( -1 );
                if ( !remCh )
                    remoteMod =( lastSync.addDays( -1 ) );
            } else {
                //qDebug(" not loc changed ");
                lastSync = localMod.addDays( 1 );
                if ( remCh ) {
                    //qDebug("rem changed ");
                    remoteMod =( lastSync.addDays( 1 ) );
                }
                
            }
        }
        full = true;  
        if ( mode < SYNC_PREF_ASK )
            mode = SYNC_PREF_ASK;
    } else {
          if ( localMod == remoteMod )
                return 0;
        
    }
    //qDebug("%s %s --- %d %d", localMod.toString().latin1() , remoteMod.toString().latin1(), localMod.time().msec(), remoteMod.time().msec());  
    //qDebug("lastsync %s ", lastSync.toString().latin1()    );
    //full = true; //debug only
    if ( full ) {
        bool equ = ( (*local) == (*remote) );
        if ( equ ) {
            //qDebug("equal ");
            if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                local->setCsum( mCurrentSyncDevice, remote->getCsum(mCurrentSyncDevice) );
            }
            if ( mode < SYNC_PREF_FORCE_LOCAL )
                return 0;
              
        }//else //debug only
        //qDebug("not equal %s %s ", local->summary().latin1(), remote->summary().latin1());
    }
    int result;
    bool localIsNew;
    //qDebug("%s -- %s mLastCalendarSync %s lastsync %s --- local %s remote %s ",local->summary().latin1(), remote->summary().latin1(),mLastCalendarSync.toString().latin1() ,lastSync.toString().latin1()  ,  local->lastModified().toString().latin1() , remote->lastModified().toString().latin1()  );

    if ( full && mode < SYNC_PREF_NEWEST )
        mode = SYNC_PREF_ASK;

    switch( mode ) {
    case SYNC_PREF_LOCAL:
        if ( lastSync > remoteMod )
            return 1;
        if ( lastSync > localMod )
            return 2;
        return 1;
        break;
    case SYNC_PREF_REMOTE:
        if ( lastSync > localMod )
            return 2;
        if ( lastSync > remoteMod )
            return 1;
        return 2;
        break;
    case SYNC_PREF_NEWEST:
        if ( localMod > remoteMod )
            return 1;
        else
            return 2;
        break;
    case SYNC_PREF_ASK: 
        //qDebug("lsy %s  ---  lo %s  --- re %s ", lastSync.toString().latin1(), localMod.toString().latin1(), remoteMod.toString().latin1()  );
        if ( lastSync > remoteMod && lastSync > localMod)
            return 0;
        if ( lastSync > remoteMod )
            return 1;
        if ( lastSync > localMod ) {
            return 2;
        }
        localIsNew = localMod >= remoteMod;
        //qDebug("conflict! ************************************** ");
        {
            KABC::AddresseeChooser acd ( *local,*remote, localIsNew , this );
            result = acd.executeD(localIsNew);
            return result;
        }
        break;
    case SYNC_PREF_FORCE_LOCAL:
        return 1;
        break;
    case SYNC_PREF_FORCE_REMOTE:
        return 2;
        break;
       
    default:
        // SYNC_PREF_TAKE_BOTH not implemented
        break;
    }
    return 0;
}


bool KABCore::synchronizeAddressbooks( KABC::AddressBook* local, KABC::AddressBook* remote,int mode)
{
    bool syncOK = true;
    int addedAddressee = 0;
    int addedAddresseeR = 0;
    int deletedAddresseeR = 0;
    int deletedAddresseeL = 0;
    int changedLocal = 0;
    int changedRemote = 0;
    int filteredIN = 0;
    int filteredOUT = 0;

    QString mCurrentSyncName = syncManager->getCurrentSyncName();
    QString mCurrentSyncDevice = syncManager->getCurrentSyncDevice();

    //QPtrList<Addressee> el = local->rawAddressees(); 
    Addressee addresseeR;
    QString uid;
    int take;
    Addressee addresseeL;
    Addressee addresseeRSync;
    Addressee addresseeLSync;
    // KABC::Addressee::List  addresseeRSyncSharp = remote->getExternLastSyncAddressees();
    //KABC::Addressee::List  addresseeLSyncSharp = local->getExternLastSyncAddressees();
    bool fullDateRange = false;
    local->resetTempSyncStat();
    mLastAddressbookSync = QDateTime::currentDateTime();
    if ( syncManager->syncWithDesktop() ) {
        // remote->removeSyncInfo( QString());//remove all info
        if (  KSyncManager::mRequestedSyncEvent.isValid() ) {
            mLastAddressbookSync = KSyncManager::mRequestedSyncEvent;
            qDebug("KA: using extern time for calendar sync: %s ", mLastAddressbookSync.toString().latin1() );
        } else {
            qDebug("KA: KSyncManager::mRequestedSyncEvent has invalid datatime ");
        }
    }
    QDateTime modifiedCalendar = mLastAddressbookSync;
    addresseeLSync = getLastSyncAddressee();
    qDebug("KA: Last Sync %s ", addresseeLSync.revision().toString().latin1());
    addresseeR = remote->findByUid("last-syncAddressee-"+mCurrentSyncName );
    if ( !addresseeR.isEmpty() ) {
        addresseeRSync = addresseeR;
        remote->removeAddressee(addresseeR );

    } else {
        if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) { 
            addresseeRSync = addresseeLSync         ;
        } else {
            //qDebug("FULLDATE 1");
            fullDateRange = true;
            Addressee newAdd;
            addresseeRSync = newAdd;
            addresseeRSync.setFamilyName(mCurrentSyncName + i18n(" - sync addressee"));
            addresseeRSync.setUid("last-syncAddressee-"+mCurrentSyncName ); 
            addresseeRSync.setRevision( mLastAddressbookSync );
            addresseeRSync.setCategories( i18n("SyncAddressee") );
        }
    }
    if ( addresseeLSync.revision() == mLastAddressbookSync ) {
        // qDebug("FULLDATE 2");
        fullDateRange = true;
    }
    //debug only
    //KMessageBox::information( this, i18n( "Local %1  --  remote %2, FULL  %3" ).arg( addresseeLSync.revision().toString()) .arg( addresseeRSync.revision().toString()).arg( QString::number( (int)fullDateRange )));
    if ( ! fullDateRange  ) {
        if ( addresseeLSync.revision() != addresseeRSync.revision() ) {
            
            // qDebug("set fulldate to true %s %s" ,addresseeLSync->dtStart().toString().latin1(), addresseeRSync->dtStart().toString().latin1() ); 
            //qDebug("%d %d %d %d ", addresseeLSync->dtStart().time().second(), addresseeLSync->dtStart().time().msec() , addresseeRSync->dtStart().time().second(), addresseeRSync->dtStart().time().msec());
            fullDateRange = true;
            //qDebug("FULLDATE 3 %s %s", addresseeLSync.revision().toString().latin1() , addresseeRSync.revision().toString().latin1() );
        } 
    }
    // fullDateRange = true; // debug only!
    if ( fullDateRange )
        mLastAddressbookSync = QDateTime::currentDateTime().addDays( -100*365);
    else
        mLastAddressbookSync = addresseeLSync.revision();
    // for resyncing if own file has changed
    // PENDING fixme later when implemented
#if 0
    if ( mCurrentSyncDevice == "deleteaftersync" ) {
        mLastAddressbookSync = loadedFileVersion;
        qDebug("setting mLastAddressbookSync ");
    }
#endif
    

    // ********** setting filters ****************
    Filter filterIN  = mViewManager->getFilterByName( syncManager->mFilterInAB );
    Filter filterOUT = mViewManager->getFilterByName( syncManager->mFilterOutAB );
   
    //qDebug("*************************** ");
    // qDebug("mLastAddressbookSync %s ",mLastAddressbookSync.toString().latin1() );
    QStringList er = remote->uidList(); 
    Addressee inR ;//= er.first();
    Addressee  inL;

    syncManager->showProgressBar(0, i18n("Syncing - close to abort!"), er.count());

    int modulo = (er.count()/10)+1;
    int incCounter = 0;
    while ( incCounter < er.count()) {
	if (syncManager->isProgressBarCanceled())
            return false;
        if ( incCounter % modulo == 0 )
	  syncManager->showProgressBar(incCounter);

        uid = er[ incCounter ]; 
        bool skipIncidence = false;
        if ( uid.left(19) == QString("last-syncAddressee-") )
            skipIncidence = true;
        QString idS,OidS;
        qApp->processEvents();
        if ( !skipIncidence ) {
            inL = local->findByUid( uid );
            inR = remote->findByUid( uid );
            //inL.setResource( 0 );
            //inR.setResource( 0 );
            if ( !inL.isEmpty() ) { // maybe conflict - same uid in both calendars
                if ( !inL.resource() || inL.resource()->includeInSync() ) {
                    if ( (take = takeAddressee( &inL, &inR, mode, fullDateRange )) ) {
                        //qDebug("take %d %s ", take, inL.summary().latin1());
                        if ( take == 3 )
                            return false;
                        if ( take == 1  ) {// take local **********************
                            if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                                inL.setCsum( mCurrentSyncDevice, inR.getCsum(mCurrentSyncDevice) );
                                inL.setID( mCurrentSyncDevice, inR.getID(mCurrentSyncDevice) );
                                local->insertAddressee(  inL, false );
                                idS = inR.externalUID();
                                OidS = inR.originalExternalUID();
                            }
                            else
                                idS = inR.IDStr();
                            remote->removeAddressee( inR );
                            inR = inL;
                            inR.setTempSyncStat( SYNC_TEMPSTATE_INITIAL );
                            if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                                inR.setOriginalExternalUID( OidS );
                                inR.setExternalUID( idS );
                                if ( syncManager->syncWithDesktop() ) {
                                    inR.setIDStr("changed" );
                                }
                                    //inR.insertCustom( "KADDRESSBOOK", "X-KDESYNC","changed" );
                            } else {
                                inR.setIDStr( idS );
                            }
                            inR.setResource( 0 );
                            remote->insertAddressee(  inR , false);
                            ++changedRemote;
                        } else { // take == 2  take remote **********************
                            if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                                if ( inR.revision().date().year() < 2004 )
                                    inR.setRevision( modifiedCalendar );
                            }
                            idS = inL.IDStr();
                            local->removeAddressee( inL );
                            inL = inR;
                            inL.setIDStr( idS );
                            if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                                inL.setCsum( mCurrentSyncDevice, inR.getCsum(mCurrentSyncDevice) );
                                inL.setID( mCurrentSyncDevice, inR.getID(mCurrentSyncDevice) );
                            }
                            inL.setResource( 0 );
                            local->insertAddressee(  inL , false );
                            ++changedLocal;
                        }
                    }
                }
            } else { //  no conflict ********** add or delete remote 
                if ( filterIN.name().isEmpty() || filterIN.filterAddressee( inR ) ) {
                    if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                        QString des = addresseeLSync.note(); 
                        if ( des.find( inR.getID(mCurrentSyncDevice)  +"," ) >= 0 && mode != 5) { // delete it
                            inR.setTempSyncStat( SYNC_TEMPSTATE_DELETE );
                            remote->insertAddressee( inR, false );
                            ++deletedAddresseeR; 
                        } else {
                            inR.setRevision( modifiedCalendar );
                            remote->insertAddressee( inR, false );
                            inL = inR; 
                            inL.setIDStr( ":" );
                            inL.setCsum( mCurrentSyncDevice, inR.getCsum(mCurrentSyncDevice) );
                            inL.setID( mCurrentSyncDevice, inR.getID(mCurrentSyncDevice) );
                            inL.setResource( 0 );
                            local->insertAddressee( inL , false);
                            ++addedAddressee;
                        }
                    } else {
                        if ( inR.revision() > mLastAddressbookSync || mode == 5 ) {
                            inR.setRevision( modifiedCalendar );
                            remote->insertAddressee( inR, false );
                            inR.setResource( 0 );
                            local->insertAddressee( inR, false );
                            ++addedAddressee;
                        } else {
                            // pending checkExternSyncAddressee(addresseeRSyncSharp, inR);
                            remote->removeAddressee( inR );
                            ++deletedAddresseeR; 
                        }
                    }
                } else {
                    ++filteredIN; 
                }
            }
        }
        ++incCounter;
    }
    er.clear();
    QStringList el = local->uidList(); 
    modulo = (el.count()/10)+1;

    syncManager->showProgressBar(0, i18n("Add / remove addressees"), el.count());
    incCounter = 0;
    while ( incCounter < el.count()) {
        qApp->processEvents();
	if (syncManager->isProgressBarCanceled())
            return false;
        if ( incCounter % modulo == 0 )
	  syncManager->showProgressBar(incCounter);
        uid = el[ incCounter ]; 
        bool skipIncidence = false;
        if ( uid.left(19) == QString("last-syncAddressee-") )
            skipIncidence = true;
        if ( !skipIncidence )  {
            inL = local->findByUid( uid );
            if ( !inL.resource() || inL.resource()->includeInSync() ) {
                inR = remote->findByUid( uid );
                if ( inR.isEmpty() ){
                    if ( filterOUT.name().isEmpty() ||  filterOUT.filterAddressee( inL )  ) { 
                        //  no conflict ********** add or delete local
                        if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                            if ( !inL.getID(mCurrentSyncDevice).isEmpty() && mode != 4 ) {
                                // pending checkExternSyncAddressee(addresseeLSyncSharp, inL);
                                local->removeAddressee( inL );
                                ++deletedAddresseeL; 
                            } else {
                                if ( ! syncManager->mWriteBackExistingOnly ) {
                                    inL.removeID(mCurrentSyncDevice );
                                    ++addedAddresseeR;
                                    inL.setRevision( modifiedCalendar );
                                    local->insertAddressee( inL, false );
                                    inR = inL;
                                    inR.setTempSyncStat( SYNC_TEMPSTATE_ADDED_EXTERNAL );
                                    inR.setResource( 0 );
                                    remote->insertAddressee( inR, false );
                                }
                            }
                        } else {
                            if ( inL.revision() < mLastAddressbookSync && mode != 4 ) {
                                //qDebug("data %s ", inL.revision().toString().latin1());
                                // pending checkExternSyncAddressee(addresseeLSyncSharp, inL);
                                local->removeAddressee( inL );
                                ++deletedAddresseeL; 
                            } else {
                                if ( ! syncManager->mWriteBackExistingOnly ) {
                                    ++addedAddresseeR; 
                                    inL.setRevision( modifiedCalendar );
                                    local->insertAddressee( inL, false );
                                    inR = inL;
                                    inR.setIDStr( ":" );
                                    inR.setResource( 0 );
                                    remote->insertAddressee( inR, false );
                                }
                            }
                        }
                    } else {
                        ++filteredOUT; 
                    }
                }
            }
        }
        ++incCounter;
    }
    el.clear();
    syncManager->hideProgressBar();
    mLastAddressbookSync = QDateTime::currentDateTime().addSecs( 1 );
    // get rid of micro seconds
    QTime t =  mLastAddressbookSync.time();
    mLastAddressbookSync.setTime( QTime (t.hour (), t.minute (), t.second () )  );

    if ( mMultiResourceSync )  {
      remote->removeSyncInfo( "" ); //all
      qDebug("KA: Removing all sync info on remote ");
    } else {
      addresseeLSync.setRevision( mLastAddressbookSync );
      addresseeRSync.setRevision( mLastAddressbookSync );
      addresseeRSync.setRole( i18n("!Remote from: ")+mCurrentSyncName ) ;
      addresseeLSync.setRole(i18n("!Local from: ") + mCurrentSyncName  );
      addresseeRSync.setGivenName( i18n("!DO NOT EDIT!") ) ;
      addresseeLSync.setGivenName(i18n("!DO NOT EDIT!")   );
      addresseeRSync.setOrganization( "!"+mLastAddressbookSync.toString() ) ;
      addresseeLSync.setOrganization("!"+ mLastAddressbookSync.toString() );
      addresseeRSync.setNote( "" ) ;
      addresseeLSync.setNote( "" );
    }
    if ( mGlobalSyncMode ==  SYNC_MODE_NORMAL && !mMultiResourceSync )
        remote->insertAddressee( addresseeRSync, false );
    local->insertAddressee( addresseeLSync, false );
    QString mes;
    mes .sprintf(  i18n("Synchronization summary:\n\n %d items added to local\n %d items added to remote\n %d items updated on local\n %d items updated on remote\n %d items deleted on local\n %d items deleted on remote\n %d incoming filtered out\n %d outgoing filtered out\n"),addedAddressee, addedAddresseeR, changedLocal, changedRemote, deletedAddresseeL, deletedAddresseeR, filteredIN, filteredOUT ); 
    qDebug( mes ); 
    mes = i18n("Local device: %2\nRemote host: %1\n").arg(mCurrentSyncDevice).arg(mCurrentSyncName  ) +mes;
    if ( syncManager->mShowSyncSummary ) { 
        if ( KMessageBox::Cancel == KMessageBox::warningContinueCancel(this, mes,
                                                                       i18n("KA/Pi  Synchronization"),i18n("Write back"))) {
            qDebug("KA: WB cancelled ");
            syncManager->mWriteBackFile = false; 
            return syncOK;
        }
    }
    return syncOK;
}


//this is a overwritten callbackmethods from the syncinterface
bool KABCore::sync(KSyncManager* manager, QString filename, int mode,QString resource)
{ 

    //pending prepare addresseeview for output
    //pending detect, if remote file has REV field. if not switch to external sync
    mGlobalSyncMode = SYNC_MODE_NORMAL;
    if ( manager != syncManager )
        qDebug("KABCore::sync:: ERROR! :: manager != syncManager ");
    QString mCurrentSyncDevice = manager->getCurrentSyncDevice();

    AddressBook abLocal(filename,"syncContact");
    bool syncOK = false;
    if ( abLocal.load() ) {
        qDebug("KA: Sync::AB loaded %s,sync mode %d",filename.latin1(), mode );
        bool external = false;
        bool isXML = false;
        if ( filename.right(4) == ".xml") {
            mGlobalSyncMode = SYNC_MODE_EXTERNAL;
            isXML = true;
            abLocal.preExternSync( mAddressBook ,mCurrentSyncDevice, true );
        } else {
            external = !manager->mIsKapiFile;
            if ( external ) {
                qDebug("KA: Sync::Setting vcf mode to external ");
                mGlobalSyncMode = SYNC_MODE_EXTERNAL;
                AddressBook::Iterator it;  
                for ( it = abLocal.begin(); it != abLocal.end(); ++it ) {
                    (*it).setID( mCurrentSyncDevice, (*it).uid() );
                    (*it).computeCsum( mCurrentSyncDevice );
                }
            }
        }
        //AddressBook::Iterator it;
        //QStringList vcards;
        //for ( it = abLocal.begin(); it != abLocal.end(); ++it ) {
        //  qDebug("Name %s ", (*it).familyName().latin1());
        //}
	if ( !resource.isEmpty() ) {
	  mAddressBook->setIncludeInSyncExclusive( resource );
	}
        syncOK = synchronizeAddressbooks( mAddressBook, &abLocal, mode );
	if ( !resource.isEmpty() ) {
	  mAddressBook->restoreIncludeInSyncExclusive();
	}
        syncManager->hideProgressBar();
        if (  syncOK ) {
            if ( syncManager->mWriteBackFile )
                {
                    if ( external )
                        abLocal.removeSyncAddressees( !isXML);
                    qDebug("KA: Sync::Saving remote AB ");
                    if ( ! abLocal.saveAB()) 
                        qDebug("KA: sync::Error writing back AB to file ");
                    if (  external ) {
                        // afterwrite processing
                        abLocal.postExternSync( mAddressBook,mCurrentSyncDevice ,isXML);
                    }
                }
        }
        setModified();
        
    }
    abLocal.removeResources();
    if ( syncOK )
        mViewManager->refreshView();
    return syncOK;

}
void  KABCore::removeSyncInfo( QString syncProfile) 
{
    qDebug("KA: AB:removeSyncInfo for profile %s ", syncProfile.latin1());
    mAddressBook->removeSyncInfo( syncProfile );
    setModified();
}

bool KABCore::syncOL()
{
    disableBR( true );
    QString mCurrentSyncDevice = syncManager->getCurrentSyncDevice();
    AddressBook abLocal;
    if ( ! readOLdata( &abLocal ) )
        return false;
    bool syncOK = false;
    message(i18n("Data from OL loaded..."), false);
    mGlobalSyncMode = SYNC_MODE_EXTERNAL;
    message(i18n("Sync preprocessing..."),false);
    abLocal.preOLSync( mAddressBook ,mCurrentSyncDevice );
    message(i18n("Synchronizing..."),false);
    syncOK = synchronizeAddressbooks( mAddressBook, &abLocal, syncManager->mSyncAlgoPrefs );
    syncManager->hideProgressBar();
    if (  syncOK ) {
        if ( syncManager->mWriteBackFile ) {
            abLocal.removeSyncAddressees( false );
            message(i18n("Saving address data to OL..."),false);
            //abLocal.saveAB();
            writeOLdata( &abLocal );
            message(i18n("Sync postprocessing..."),false);
            abLocal.postExternSync( mAddressBook,mCurrentSyncDevice, true );
        }
    } else
        message( i18n("Sync cancelled or failed.") );
    setModified();
    abLocal.removeResources();
    if ( syncOK ) {
        mViewManager->refreshView();
        message(i18n("OL syncing finished."));
    }
    disableBR( false );
    return syncOK;
}

//this is a overwritten callbackmethods from the syncinterface
bool KABCore::syncExternal(KSyncManager* manager, QString  resource)
{
    if (  resource == "phone" )
        return syncPhone();
    if (  resource == "ol" )
        return syncOL();
    disableBR( true );
   if ( manager != syncManager )
        qDebug("KABCore::syncExternal:: ERROR! :: manager != syncManager ");
    QString mCurrentSyncDevice = manager->getCurrentSyncDevice();

    AddressBook abLocal( resource,"syncContact");
    bool syncOK = false;
    message(i18n("Loading DTM address data..."), false);
    if ( abLocal.load() ) {
        qDebug("KA: AB sharp loaded ,sync device %s",mCurrentSyncDevice.latin1());
        mGlobalSyncMode = SYNC_MODE_EXTERNAL;
        message(i18n("Sync preprocessing..."),false);
        abLocal.preExternSync( mAddressBook ,mCurrentSyncDevice, false );
        message(i18n("Synchronizing..."),false);
        syncOK = synchronizeAddressbooks( mAddressBook, &abLocal, syncManager->mSyncAlgoPrefs );
        syncManager->hideProgressBar();
        if (  syncOK ) {
            if ( syncManager->mWriteBackFile ) {
                abLocal.removeSyncAddressees( false );
                message(i18n("Saving DTM address data..."),false);
                abLocal.saveAB();
                message(i18n("Sync postprocessing..."),false);
                abLocal.postExternSync( mAddressBook,mCurrentSyncDevice, true );
            }
        } else
            message( i18n("Sync cancelled or failed.") );
        setModified();
    }
    abLocal.removeResources();
    if ( syncOK ) {
        mViewManager->refreshView();
        message(i18n("DTM syncing finished."));
    }
    disableBR( false );
    return syncOK;

}
void KABCore::message( QString m, bool startTimer)
{
    topLevelWidget()->setCaption( m );
    qApp->processEvents();
    if ( startTimer )
        mMessageTimer->start( 15000, true );
    else
        mMessageTimer->stop();
}
bool KABCore::syncPhone()
{
    QString mCurrentSyncDevice = syncManager->getCurrentSyncDevice();
    QString fileName = getPhoneFile();
    if ( !PhoneAccess::readFromPhone( fileName) ) {
        message(i18n("Phone access failed!"));
        return false;
    }   
    AddressBook abLocal( fileName,"syncContact");
    bool syncOK = false;
     {
         abLocal.importFromFile( fileName );
        qDebug("KA: AB phone loaded ,sync device %s",mCurrentSyncDevice.latin1());
        mGlobalSyncMode = SYNC_MODE_EXTERNAL;
        abLocal.preparePhoneSync( mCurrentSyncDevice, true );
        abLocal.preExternSync( mAddressBook ,mCurrentSyncDevice, true );
        syncOK = synchronizeAddressbooks( mAddressBook, &abLocal, syncManager->mSyncAlgoPrefs );
        syncManager->hideProgressBar();
        if (  syncOK ) {
            if ( syncManager->mWriteBackFile ) {
                abLocal.removeSyncAddressees( true );
                abLocal.saveABphone( fileName );
                abLocal.findNewExtIds( fileName, mCurrentSyncDevice );
                //abLocal.preparePhoneSync( mCurrentSyncDevice, false );
                abLocal.postExternSync( mAddressBook,mCurrentSyncDevice, true );
            }
        }
        setModified();
    }
    abLocal.removeResources();
    if ( syncOK )
        mViewManager->refreshView();
    return syncOK;
}
void KABCore::getFile( bool success ,const QString & resource)
{
    if ( ! success ) {
        message( i18n("Error receiving file. Nothing changed!") );
        return;
    }
    //KMessageBox::information( this, i18n( "getting data for resource %1" ).arg( resource ) );
    QString res;
    if ( resource != "ALL" )
      res = resource;
    int count = mAddressBook->importFromFile( sentSyncFile() , false, true ,res);
    if ( count )
        setModified( true );
    message( i18n("Pi-Sync successful!") );
    mViewManager->refreshView();
}
void KABCore::syncFileRequest(const QString & resource)
{ 
    if ( KABPrefs::instance()->mPassiveSyncWithDesktop ) {
        syncManager->slotSyncMenu( 999 );
    } 

    if ( resource == "ALL" ) {
        mAddressBook->export2File( sentSyncFile() );
    }
    else
        mAddressBook->export2File( sentSyncFile(), resource);
}
QString KABCore::sentSyncFile()
{
#ifdef DESKTOP_VERSION
    return  locateLocal( "tmp", "copysyncab.vcf" );
#else
    return QString( "/tmp/copysyncab.vcf" );
#endif 
}

void KABCore::setCaptionBack()
{
    mMessageTimer->stop();
    topLevelWidget()->setCaption( i18n("KA/Pi") );
}
