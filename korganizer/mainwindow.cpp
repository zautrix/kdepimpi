#include <stdlib.h>

#include <qaction.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qtextcodec.h>
#include <qfile.h>
#include <qdir.h>
#include <qapp.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qmap.h>
#include <qwmatrix.h>
#include <qtextbrowser.h>
#include <qtextstream.h>
#ifndef DESKTOP_VERSION
#include <qpe/global.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qtopia/alarmserver.h>
#include <qtopia/qcopenvelope_qws.h>
//#include <unistd.h> // for sleep
#else
#include <qtoolbar.h>
#include <qapplication.h>
//#include <resource.h>

#endif
#include <libkcal/calendarlocal.h>
#include <libkcal/todo.h>
#include <libkcal/phoneformat.h>
#include <libkdepim/ksyncprofile.h>
#include <libkdepim/phoneaccess.h>
#include <libkcal/kincidenceformatter.h>
#include <libkdepim/kpimglobalprefs.h>

#include "calendarview.h"
#include "koviewmanager.h"
#include "datenavigator.h"
#include "koagendaview.h"
#include "kojournalview.h"
#include "koagenda.h"
#include "kodialogmanager.h"
#include "kdialogbase.h"
#include "kapplication.h"
#include "kofilterview.h"
#include "kstandarddirs.h"
#include "koprefs.h"
#include "kfiledialog.h"
#include "koglobals.h"
#include "kglobal.h"
#include "ktoolbar.h"
#include "klocale.h"
#include "kconfig.h"
#include "externalapphandler.h"
#include <kglobalsettings.h>

using namespace KCal;
#ifndef _WIN32_
#include <unistd.h>
#else
#ifdef _OL_IMPORT_
#include "koimportoldialog.h"
#endif
#endif
#include "mainwindow.h"


class KOex2phonePrefs : public QDialog
{
  public:
    KOex2phonePrefs( QWidget *parent=0, const char *name=0 ) :
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
        mWriteBackFuture= new QCheckBox( i18n("Write back events in future only"), this );
        mWriteBackFuture->setChecked( true );
        lay->addWidget( mWriteBackFuture );
        temphb = new QHBox( this );
        new QLabel( i18n("Max. weeks in future: ") , temphb );
        mWriteBackFutureWeeks= new QSpinBox(1,104, 1, temphb);
        mWriteBackFutureWeeks->setValue( 8 );
        lay->addWidget( temphb );  
        lay->addWidget(lab = new QLabel( i18n("NOTE: This will remove all old\ntodo/calendar data on phone!"), this ) );  
        lab->setAlignment (AlignHCenter  ); 
        QPushButton * ok = new QPushButton( i18n("Export to mobile phone!"), this );
        lay->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        lay->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
        connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
        resize( 220, 240 );
        qApp->processEvents();
        int dw = QApplication::desktop()->width();
        int dh = QApplication::desktop()->height();
        move( (dw-width())/2, (dh - height() )/2 );
    }
   
public:
    QLineEdit* mPhoneConnection, *mPhoneDevice, *mPhoneModel; 
    QCheckBox* mWriteBackFuture;
    QSpinBox* mWriteBackFutureWeeks;
};

QPixmap* sgListViewCompletedPix[6];
QPixmap* sgListViewJournalPix;


int globalFlagBlockStartup;
MainWindow::MainWindow( QWidget *parent, const char *name ) :
    QMainWindow( parent, name )
{
    sgListViewCompletedPix[5] = &listviewPix;
    sgListViewCompletedPix[0] = &listviewPix0;
    sgListViewCompletedPix[1] = &listviewPix20;
    sgListViewCompletedPix[2] = &listviewPix40;
    sgListViewCompletedPix[3] = &listviewPix60;
    sgListViewCompletedPix[4] = &listviewPix80;
        //int size = 12;
    {
        sgListViewCompletedPix[5]->resize( 11, 11 );
        sgListViewCompletedPix[5]->fill( Qt::white );
        QPainter p ( sgListViewCompletedPix[5] );
        p.drawRect( 0,0,11,11);
        p.drawLine ( 2, 5, 4 , 7 ) ;
        p.drawLine ( 4 , 7 ,  8, 3) ;
        int iii = 0;
        for ( iii = 0; iii < 5; ++iii ) {
            sgListViewCompletedPix[iii]->resize( 11, 11 );
            sgListViewCompletedPix[iii]->fill( Qt::white );
            QPainter p ( sgListViewCompletedPix[iii] );
            p.drawRect( 0,0,11,11);
            if ( iii )
                p.fillRect( 1,1,iii*2,9,Qt::gray );
        }
        sgListViewJournalPix = &journalPix;
        sgListViewJournalPix->resize( 11, 11 );
        sgListViewJournalPix->fill( Qt::white ); 
        {
            QPainter p ( sgListViewJournalPix );
            p.drawRect( 0,0,11,11);
            p.drawLine( 2,3,5,3);
            p.drawLine( 2,5,8,5);
            p.drawLine( 2,7,6,7);
        }
    }
    mAutoSaveDisabled = false;
    mClosed = false;
    //QString confFile = KStandardDirs::appDir() + "config/korganizerrc";
    QString confFile = locateLocal("config","korganizerrc");
    QFileInfo finf ( confFile );
    bool showWarning =  !finf.exists();
    setIcon(SmallIcon( "ko24" )  );
    mBlockAtStartup = true;
    mFlagKeyPressed = false;
    setCaption("KO/Pi");
    KOPrefs *p = KOPrefs::instance();
    //KPimGlobalPrefs::instance()->setGlobalConfig();
    p->mCurrentDisplayedView = 0;
    if ( p->mHourSize >  22 )
        p->mHourSize = 22;
    QMainWindow::ToolBarDock tbd;
    if ( p->mToolBarHor ) {
        if ( p->mToolBarUp )
            tbd = Bottom;
        else
            tbd = Top;
    }
    else {
        if ( p->mToolBarUp )
            tbd = Right;
        else
            tbd = Left;
    }
    if ( KOPrefs::instance()->mUseAppColors )
        QApplication::setPalette( QPalette (KOPrefs::instance()->mAppColor1, KOPrefs::instance()->mAppColor2), true );
    globalFlagBlockStartup = 1;
    iconToolBar = new QPEToolBar( this );
    addToolBar (iconToolBar , tbd );

#ifdef DESKTOP_VERSION
    if ( KOPrefs::instance()->mShowIconFilter ) 
#else
        if ( KOPrefs::instance()->mShowIconFilter || ( !p->mShowIconOnetoolbar &&!p->mShowIconFilterview ) )
#endif

{
        if ( p->mToolBarHorF ) {
            if ( p->mToolBarUpF )
                tbd = Bottom;
            else
                tbd = Top;
        }
        else {
            if ( p->mToolBarUpF )
            tbd = Right;
            else
                tbd = Left;
        }
        filterToolBar = new QPEToolBar ( this );
        filterMenubar = new KMenuBar( 0 );
        QFontMetrics fm ( filterMenubar->font() );
#ifndef DESKTOP_VERSION    
        filterToolBar->setFocusPolicy( NoFocus );
        filterMenubar->setFocusPolicy( NoFocus );
#endif     
        filterPopupMenu = new QPopupMenu( this );
        filterMenubar->insertItem( i18n("No Filter"), filterPopupMenu,0  );
        QString addTest = "A";  
        filterMenubar->setMinimumWidth( fm.width( i18n("No Filter")+addTest ) );
#ifdef DESKTOP_VERSION
        addTest = "AAAAAABBBCCCx"; 
#else
        addTest = "AAAAAx";  
#endif 
        filterMenubar->setMaximumWidth( fm.width( i18n("No Filter")+addTest ) );
        addToolBar (filterToolBar , tbd );
        connect ( filterPopupMenu , SIGNAL( activated ( int )  ), this, SLOT (selectFilterPopup( int ) ) );
        connect ( filterPopupMenu , SIGNAL( aboutToShow() ), this, SLOT (fillFilterMenuPopup() ) );
        if ( !KOPrefs::instance()->mShowIconFilter && !p->mShowIconOnetoolbar )
            filterToolBar->hide();
    } else {
        filterToolBar = 0;
        filterMenubar  = 0;
        filterPopupMenu = 0;
    }
    if ( p->mShowIconOnetoolbar ) {
        viewToolBar = iconToolBar ;
        navigatorToolBar = iconToolBar ;
    } else {
#ifndef DESKTOP_VERSION
        setToolBarsMovable( false );
#endif
        if ( p->mToolBarHorV ) {
            if ( p->mToolBarUpV )
                tbd = Bottom;
            else
                tbd = Top;
        }
        else {
            if ( p->mToolBarUpV )
                tbd = Right;
            else
                tbd = Left;
        }
        viewToolBar = new QPEToolBar( this );
        addToolBar (viewToolBar , tbd );
        if ( p->mToolBarHorN ) {
            if ( p->mToolBarUpN )
                tbd = Bottom;
            else
                tbd = Top;
        }
        else {
            if ( p->mToolBarUpN )
                tbd = Right;
            else
                tbd = Left;
        }
        navigatorToolBar = new QPEToolBar( this );
        addToolBar (navigatorToolBar , tbd );
    }

  

    mCalendarModifiedFlag = false;
    // QLabel* splash  = new QLabel(i18n("KO/Pi is starting ... "), this );
    //splash->setAlignment ( AlignCenter );
    //setCentralWidget( splash );
#ifndef DESKTOP_VERSION
    //showMaximized();
#endif

    //qDebug("Mainwidget x %d y %d w %d  h %d",  x(), y(), width(), height ());
    setDefaultPreferences();
    mCalendar = new CalendarLocal();
    mView = new CalendarView( mCalendar, this,"mCalendar " );
    setCentralWidget( mView );
    //mView->hide();
    //mView->resize(splash->size() );
    initActions(); 
    mSyncManager = new KSyncManager((QWidget*)this, (KSyncInterface*)mView, KSyncManager::KOPI, KOPrefs::instance(), syncMenu);
    mSyncManager->setBlockSave(false);
    mView->setSyncManager(mSyncManager);
#ifndef DESKTOP_VERSION
    iconToolBar->show();
    qApp->processEvents();
#endif
    //qDebug("Splashwidget x %d y %d w %d  h %d", splash-> x(), splash->y(), splash->width(),splash-> height ());
    int vh = height() ;
    int vw = width();
    //qDebug("Toolbar hei %d ",iconToolBar->height() );
    if ( iconToolBar->orientation () == Qt:: Horizontal ) {
        vh -=  iconToolBar->height();
    } else {
        vw -= iconToolBar->height();
    }
    //mView->setMaximumSize( splash->size() );
    //mView->resize( splash->size() );
    //qDebug("MainView x %d y %d w %d  h %d",  mView->x(),mView-> y(), mView->width(), mView->height ());
    mView->readSettings();
    bool newFile = false;
    if( !QFile::exists( defaultFileName() ) ) {
        QFileInfo finfo ( defaultFileName() );
        QString oldFile = QDir::convertSeparators(  QDir::homeDirPath()+"/Applications/korganizer/mycalendar.ics");
        QString message = "You are starting KO/Pi for the\nfirst time after updating to a\nversion >= 1.9.1. The location of the\ndefault calendar file has changed.\nA mycalendar.ics file was detected\nat the old location.\nThis file will be loaded now\nand stored at the new location!\n(Config file location has changed, too!)\nPlease read menu Help-What's New!\n";
        finfo.setFile( oldFile );
        if (finfo.exists() ) {
            KMessageBox::information( this, message);
            mView->openCalendar( oldFile );
            qApp->processEvents();
        } else {
            oldFile = QDir::convertSeparators(  QDir::homeDirPath()+"/korganizer/mycalendar.ics");
            finfo.setFile( oldFile );
            if (finfo.exists() ) {
                KMessageBox::information( this, message);
                mView->openCalendar( oldFile );
                qApp->processEvents();
            }
        }
        mView->saveCalendar( defaultFileName() );
        newFile = true;
    }

    //QTime neededSaveTime = QDateTime::currentDateTime().time();
    //mView->loadCalendars();
    //int msNeeded = neededSaveTime.msecsTo( QDateTime::currentDateTime().time() );
    //qDebug("KO: Calendar loading time: %d ms",msNeeded );

    if ( KPimGlobalPrefs::instance()->mPreferredLanguage != KOPrefs::instance()->mOldLoadedLanguage ) {
        KOPrefs::instance()->setAllDefaults();
    }


    connect( mView, SIGNAL( tempDisableBR(bool) ),
             SLOT( disableBR(bool) ) );
    connect( &mSaveTimer, SIGNAL( timeout() ), SLOT( save() ) );
    setCentralWidget( mView );
    globalFlagBlockStartup = 0;
    //mView->show();
    //delete splash;
    if ( newFile )
        mView->updateConfig();
    // qApp->processEvents();
    //qDebug("MainView x %d y %d w %d  h %d",  mView->x(),mView-> y(), mView->width(), mView->height ());
    //fillSyncMenu();
   
    
    connect(mSyncManager , SIGNAL( save() ), this, SLOT( save() ) );
    connect(mView , SIGNAL( saveStopTimer() ), this, SLOT( saveStopTimer() ) );
    connect(mSyncManager , SIGNAL( request_file(const QString &) ), this, SLOT( syncFileRequest(const QString &) ) );
    connect(mSyncManager , SIGNAL( getFile( bool, const QString &)), this, SLOT(getFile( bool,const QString & ) ) );
    connect(mSyncManager , SIGNAL( multiResourceSyncStart( bool )), mView, SLOT( multiResourceSyncStart( bool ) ) );

    mSyncManager->setDefaultFileName( sentSyncFile()); 
    connect ( syncMenu, SIGNAL( activated ( int )  ), mSyncManager, SLOT (slotSyncMenu( int ) ) );
    mSyncManager->fillSyncMenu();



    mView->viewManager()->agendaView()->setStartHour( KOPrefs::instance()->mDayBegins  );
    if ( showWarning ) {
        KMessageBox::information( this,
                         "You are starting KO/Pi for the first time.\nPlease read menu: Help-What's New,\nif you did an update!\nPlease choose your timezone in the \nConfigure Dialog TAB Time Zone!\nPlease choose your language\nin the TAB Locale!\nYou get the Configure Dialog\nvia Menu: Actions - Configure....\nClick OK to show the Configure Dialog!\n", "KO/Pi information");
        //qApp->processEvents();
        mView->dialogManager()->showSyncOptions();
    }

  //US listen for result adressed from Ka/Pi

#ifndef DESKTOP_VERSION
    infrared = 0;
#endif
    updateFilterToolbar();
    updateWeek( mView->startDate() );
    connect( mView->dateNavigator(), SIGNAL( datesSelected( const KCal::DateList & ) ),
             SLOT( updateWeekNum( const KCal::DateList & ) ) );
    mBRdisabled = false;
    //toggleBeamReceive();

    setCaption(i18n("Loading calendar files ... please wait" ));
    mSaveDelay = 0;
    QTimer::singleShot( 10, this, SLOT ( loadDataAfterStart() ));
}
MainWindow::~MainWindow()
{
    //qDebug("MainWindow::~MainWindow() ");
    //save toolbar location
    delete mCalendar;
    delete mSyncManager;
#ifndef DESKTOP_VERSION
    if ( infrared )
        delete infrared;
#endif


}

void  MainWindow::loadDataAfterStart()
{
    qDebug("KO: Start loading files..." );
    QTime neededSaveTime = QDateTime::currentDateTime().time();
    mView->loadCalendars();
    int msNeeded = neededSaveTime.msecsTo( QDateTime::currentDateTime().time() );
    qDebug("KO: Calendar loading time: %d ms",msNeeded );
    //QTimer::singleShot( 1000, mView, SLOT ( checkFiles() ));
    mView->setModified( false );
    mBlockAtStartup = false;
    mView->setModified( false );
    connect(mView , SIGNAL( save() ), this, SLOT( save() ) );
    processIncidenceSelection( 0 );
    connect( mView, SIGNAL( incidenceSelected( Incidence * ) ),
             SLOT( processIncidenceSelection( Incidence * ) ) );
    connect( mView, SIGNAL( modifiedChanged( bool ) ),
             SLOT( slotModifiedChanged( bool ) ) );
#ifndef DESKTOP_VERSION
    connect(qApp, SIGNAL (appMessage ( const QCString &, const QByteArray & )), ExternalAppHandler::instance(), SLOT (appMessage ( const QCString &, const QByteArray & )));
    connect( qApp, SIGNAL (appMessage ( const QCString &, const QByteArray & )), this, SLOT(recieve(  const QCString&, const QByteArray& )));
    disconnect( qApp, SIGNAL (appMessage ( const QCString &, const QByteArray & )), this, SLOT(receiveStart(  const QCString&, const QByteArray& )));
    if ( !mCStringMess.isEmpty() ) 
        recieve( mCStringMess, mByteData );
#endif
    QTimer::singleShot( 1000, mView, SLOT ( checkFiles() ));
}

void MainWindow::slotResetFocus()
{
    //qDebug(" CalendarView::slotResetFocus() %x  %x %x %x", qApp->focusWidget(), menuBar1, mView,iconToolBar);
    mFocusLoop = 3;
    QTimer::singleShot( 0, this, SLOT(slotResetFocusLoop()  ));
}
void MainWindow::slotResetFocusLoop()
{
    --mFocusLoop;
    QWidget* fw = mView->viewManager()->currentView();
    if ( fw ) {
        //qDebug("loop ");
        fw->setFocus();
        if ( qApp->focusWidget() != fw && mFocusLoop > 0 )
            QTimer::singleShot( 0, this, SLOT(slotResetFocusLoop()  ));
    }
        
}
void MainWindow::disableBR(bool b)
{
#ifndef DESKTOP_VERSION
    if ( b ) {
        if ( infrared ) {
            toggleBeamReceive();
            mBRdisabled = true;
        }
        mBRdisabled = true;
    } else {
        if ( mBRdisabled ) {
            mBRdisabled = false;
            //makes no sense,because other cal ap is probably running
            // toggleBeamReceive(); 
        }
    }
#else
    Q_UNUSED(b);
#endif

}
bool MainWindow::beamReceiveEnabled()
{
#ifndef DESKTOP_VERSION
    return ( infrared != 0 );
#endif
    return false;
}

void MainWindow::toggleBeamReceive()
{
    if ( mBRdisabled  )
        return;
#ifndef DESKTOP_VERSION
    if ( infrared ) {
        qDebug("KO: Disable BeamReceive ");
        delete infrared;
        infrared = 0;
        brAction->setOn(false);
        return;
    } 
    qDebug("KO: Enable BeamReceive ");
    brAction->setOn(true);
    infrared = new QCopChannel("QPE/Application/datebook",this, "channel" ) ;   
    QObject::connect( infrared, SIGNAL (received ( const QCString &, const QByteArray & )),this, SLOT(recieve(  const QCString&, const QByteArray& )));
#endif
}
void MainWindow::showMaximized () 
{
#ifndef DESKTOP_VERSION
    if ( ! globalFlagBlockStartup )
        if ( mClosed )
            mView->goToday();
#endif
    QWidget::showMaximized () ;
    mClosed = false;
}

bool MainWindow::askForQuitOnSaveError()
{
    bool retval = false;
    switch( QMessageBox::information( this, "KO/Pi",
                                      i18n("Error saving data") + "!\n" +
                                      i18n("You can save all data\nto another file via\nFile->Export->Export All Data") + "!\n" +
                                      i18n("Do you really want\nto close KO/Pi?"),
                                      i18n(" Yes, close "), i18n("No"),
                                      0, 1 ) ) {
    case 0:
        retval = true;
        break;
    default:
        break;
    }
    return retval;
}
bool MainWindow::checkAutosave()
{
    bool savedata = true;
    if ( mAutoSaveDisabled && mCalendarModifiedFlag  ) {
        switch( QMessageBox::information( this, "KO/Pi",
                                          i18n("Calendar is modified\nbut Autosave is disabled!\nDo you want\nto save the data?"),
                                          i18n("Yes, Save!"), i18n("No"),
                                          0, 0 ) ) {
        case 1:  
        case 2:
            savedata = false;
            break;
        default:
            break;
        }
    }
    return savedata;
}
void MainWindow::closeEvent( QCloseEvent* ce )
{
    if ( ! KOPrefs::instance()->mAskForQuit ) {
        if ( checkAutosave() ) {
            saveOnClose();
            if ( mCalendarModifiedFlag && !askForQuitOnSaveError() ) {
                ce->ignore();
                return;
            }
        }
        mClosed = true;
        ce->accept();
        return;

    }

    switch( QMessageBox::information( this, "KO/Pi",
                                      i18n("Do you really want\nto close KO/Pi?"),
                                      i18n("Close"), i18n("No"),
                                      0, 0 ) ) {
    case 0:  
        if ( checkAutosave() ) {
            saveOnClose();
            if ( mCalendarModifiedFlag  && !askForQuitOnSaveError() ) {
                ce->ignore();
                return;
            }
        }
        mClosed = true;
        ce->accept();
        break;
    case 1:
        ce->ignore();
        break;
    case 2:

    default:
        break;
    }


}
void MainWindow::receiveStart( const QCString& cmsg, const QByteArray& data )
{
    qDebug("KO: QCOP start message received: %s ", cmsg.data()  );
    mCStringMess = cmsg;
    mByteData = data;
}
void MainWindow::recieve( const QCString& cmsg, const QByteArray& data )
{
    QDataStream stream( data, IO_ReadOnly );
    // QMessageBox::about( this, "About KOrganizer/Pi", "*" +msg +"*" );
    //QString datamess;
    //qDebug("message ");
    qDebug("KO: QCOP message received: %s ", cmsg.data()  );
 
    if ( cmsg == "setDocument(QString)" ) {
        QDataStream stream( data, IO_ReadOnly );
        QString fileName;
        stream >> fileName;
        //qDebug("filename %s ", fileName.latin1());
        showMaximized();
        raise();
        KOPrefs::instance()->mLastSyncedLocalFile = fileName ;
        mSyncManager->slotSyncMenu( 1002 );
        return;
    }

    if ( cmsg == "-writeFile" ) {
        // I made from the "-writeFile" an "-writeAlarm"
        mView->viewManager()->showWhatsNextView();
        mCalendar->checkAlarmForIncidence( 0, true);
        showMaximized();
        raise();
        return;

    }
    if ( cmsg == "-writeFileSilent" ) {
        // I made from the "-writeFile" an "-writeAlarm"
        // mView->viewManager()->showWhatsNextView();
        mCalendar->checkAlarmForIncidence( 0, true);
        //showMaximized();
        //raise();
        //hide();
        return;
    }
    if ( cmsg == "-newCountdown" ) {
        qDebug("newCountdown ");

    }
    QString msg ;
    QString allmsg = cmsg;
    while ( allmsg.length() > 0 ) {
        int nextC = allmsg.find( "-", 1 );
        if ( nextC == -1 ) {
            msg = allmsg;
            allmsg = "";
        } else{
            msg = allmsg.left( nextC );
            allmsg = allmsg.mid( nextC,  allmsg.length()-nextC );
        }
        //qDebug("msg: %s  all: %s ", msg.latin1(), allmsg.latin1() );
        if ( msg == "-newEvent" ) {
            QTimer::singleShot( 0, mView, SLOT ( newEvent()));
        }
        if ( msg == "-newTodo" ) {
            QTimer::singleShot( 0, mView, SLOT ( newTodo()));
        }
        if ( msg == "-showWN" ) {
            mView->viewManager()->showWhatsNextView();
        }
        if ( msg == "-showList"  ) {
            mView->viewManager()->showListView();
        }
        else if ( msg == "-showDay" ) {
            mView->viewManager()->showDayView();
        }
        else if ( msg == "-showWWeek"  ) {
            mView->viewManager()->showWorkWeekView();
        }
        else if ( msg == "-ringSync"  ) {
            QTimer::singleShot( 0, this, SLOT (startMultiSync()));
        }
        else if ( msg == "-showWeek" ) {
            mView->viewManager()->showWeekView();
        }
        else if ( msg == "-showTodo" ) {
            mView->viewManager()->showTodoView();
        }
        else if ( msg == "-showJournal"  ) {
            mView->dateNavigator()->selectDates( 1 );
            mView->dateNavigator()->selectToday();
            mView->viewManager()->showJournalView();
        }
        else if ( msg == "-showKO" ) {
            //mView->viewManager()->showNextXView();
            mView->showNextAlarms();
        }
        else if ( msg == "-showWNext"  ) {
            mView->viewManager()->showWhatsNextView();
        }
        else if ( msg == "nextView()" ) {
            mView->viewManager()->showNextView();
        }
        else if ( msg == "-showNextXView" ) {
            mView->viewManager()->showNextXView();
        }


    }
#ifndef DESKTOP_VERSION
    showMaximized();
#endif
    raise();
}
void MainWindow::startMultiSync()
{
    QString question = i18n("Do you really want\nto multiple sync\nwith all checked profiles?\nSyncing takes some\ntime - all profiles\nare synced twice!");
    if ( QMessageBox::information( this, i18n("KDE-Pim Sync"),
                                   question,
                                   i18n("Yes"), i18n("No"),
                                   0, 0 ) != 0 ) {
        setCaption(i18n("Aborted! Nothing synced!"));
        return;
    }
    mSyncManager->multiSync( false );
#ifndef DESKTOP_VERSION
    QCopEnvelope e("QPE/Application/kapi", "doRingSync");
#endif
}
QPixmap MainWindow::loadPixmap( QString name )
{
    return SmallIcon( name );

}
void MainWindow::setUsesBigPixmaps ( bool b )
{
    qDebug("KO: MainWindow::setUsesBigPixmaps %d called", b);
    if ( b )
        qDebug("KO: BigPixmaps are not supported ");
}
void MainWindow::initActions()
{
    //KOPrefs::instance()->mShowFullMenu
    iconToolBar->clear();
    KOPrefs *p = KOPrefs::instance();
    //QPEMenuBar *menuBar1;// = new QPEMenuBar( iconToolBar );

    QPopupMenu *viewMenu = new QPopupMenu( this );
    QPopupMenu *actionMenu = new QPopupMenu( this );
    mCurrentItemMenu = new QPopupMenu ( this );
    QPopupMenu *nextConflictMenu = new QPopupMenu ( this );
    QPopupMenu *importMenu = new QPopupMenu( this );
    QPopupMenu *importMenu_X = new QPopupMenu( this );
    QPopupMenu *exportMenu_X = new QPopupMenu( this );
    QPopupMenu *beamMenu_X = new QPopupMenu( this );
    selectFilterMenu = new QPopupMenu( this );
    selectFilterMenu->setCheckable( true );
    syncMenu = new QPopupMenu( this );
    configureAgendaMenu = new QPopupMenu( this );
    configureToolBarMenu = new QPopupMenu( this );
    QPopupMenu *daysinWNviewMenu = new QPopupMenu( this );
    QPopupMenu *helpMenu = new QPopupMenu( this );
    QIconSet icon;
    int pixWid = 22, pixHei = 22;
    QString pathString = "";
    if ( !p->mToolBarMiniIcons ) {
        if ( QApplication::desktop()->width() < 480 /*|| QApplication::desktop()->height() < 320*/) {
            pathString += "icons16/";
            pixWid = 18; pixHei = 16;
        }
    } else {
        pathString += "iconsmini/";
        pixWid = 18; pixHei = 16;
    }
    
    if ( KOPrefs::instance()->mShowFullMenu ) {
        menuBar1 = new KMenuBar( this );//menuBar();
         //setMenuBar( menuBar1 );
        menuBar1->show();
        menuBar1->insertItem( i18n("File"), importMenu );
        menuBar1->insertItem( i18n("View"), viewMenu );
        menuBar1->insertItem( i18n("Edit"), mCurrentItemMenu );
        menuBar1->insertItem( i18n("Action"), actionMenu );
#ifdef DESKTOP_VERSION
        menuBar1->insertItem( i18n("Synchronize"), syncMenu );
        menuBar1->insertItem( i18n("AgendaSize"),configureAgendaMenu  );
#else
        menuBar1->insertItem( i18n("Sync"), syncMenu );
        menuBar1->insertItem( i18n("Agenda"),configureAgendaMenu  );
#endif
        //menuBar1->insertItem( i18n("Toolbar"),configureToolBarMenu );
        menuBar1->insertItem( i18n("Filter"),selectFilterMenu  );
        menuBar1->insertItem( i18n("Help"), helpMenu );
    } else {
        menuBar1 = new KMenuBar( iconToolBar );
        QPopupMenu *menuBar = new QPopupMenu( this );
        icon = loadPixmap( pathString + "z_menu" );
        menuBar1->insertItem( icon.pixmap(), menuBar);
        //menuBar1->insertItem( i18n("ME"), menuBar);
        menuBar->insertItem( i18n("File"), importMenu );
        menuBar->insertItem( i18n("View"), viewMenu );
        menuBar->insertItem( i18n("Edit"), mCurrentItemMenu );
        menuBar->insertItem( i18n("Action"), actionMenu );
        menuBar->insertItem( i18n("Synchronize"), syncMenu );
        menuBar->insertItem( i18n("AgendaSize"),configureAgendaMenu  );
        menuBar->insertItem( i18n("Toolbar"),configureToolBarMenu );
        menuBar->insertItem( i18n("Filter"),selectFilterMenu  );
        menuBar->insertItem( i18n("Help"), helpMenu );
        //menuBar1->setMaximumWidth( menuBar1->sizeHint().width() );
        menuBar1->setMaximumSize( menuBar1->sizeHint( ));
        connect ( menuBar, SIGNAL( aboutToHide () ), this, SLOT ( slotResetFocus() ) );
    }
    connect ( menuBar1, SIGNAL( lostFocus ()  ), this, SLOT ( slotResetFocus() ) );
    //connect ( menuBar1, SIGNAL( lostFocus ()  ), this, SLOT ( slotResetFocus() ) );
    connect ( selectFilterMenu, SIGNAL( activated ( int )  ), this, SLOT (selectFilter( int ) ) );
    connect ( selectFilterMenu, SIGNAL( aboutToShow() ), this, SLOT (fillFilterMenu() ) );

   
    mWeekBgColor = iconToolBar->backgroundColor();
    mWeekPixmap.resize( pixWid , pixHei );
    mWeekPixmap.fill( mWeekBgColor );
    icon = mWeekPixmap;
    mWeekAction = new QAction( i18n("Select week number"),icon, i18n("Select week number"), 0, this );
    if ( p-> mShowIconWeekNum )
        mWeekAction->addTo( iconToolBar );
    mWeekFont = font();

    int fontPoint = mWeekFont.pointSize();
    QFontMetrics f( mWeekFont );
    int fontWid =  f.width( "30" );
    while ( fontWid > pixWid ) {
        --fontPoint;
        mWeekFont.setPointSize( fontPoint );
        QFontMetrics f( mWeekFont );
        fontWid =  f.width( "30" );
        //qDebug("dec-- ");
    }

    connect( mWeekAction, SIGNAL( activated() ),
             this, SLOT( weekAction() ) );

    connect( this, SIGNAL( selectWeek ( int ) ), mView->dateNavigator(), SLOT( selectWeek ( int ) ) );
    if ( p->mShowIconFilterview ) {
        icon = loadPixmap( pathString + "filter" );
        actionFilterMenuTB = new QAction( i18n("Filter selector"), icon, i18n("Filter selector"), 0, this );
        connect( actionFilterMenuTB, SIGNAL( activated() ),
                 this, SLOT( fillFilterMenuTB() ) );
        actionFilterMenuTB->addTo( iconToolBar  );
        selectFilterMenuTB = new QPopupMenu( this );
        selectFilterMenuTB->setCheckable( true ); 
        connect ( selectFilterMenuTB, SIGNAL( activated ( int )  ), this, SLOT (selectFilter( int ) ) );
    }

    //#endif
    // ******************
    QAction *action;
    // QPopupMenu *configureMenu= new QPopupMenu( menuBar );
    configureToolBarMenu->setCheckable( true );

   
    configureAgendaMenu->setCheckable( true );
    int iii ;
    for ( iii = 1;iii<= 10 ;++iii ){
        configureAgendaMenu->insertItem(i18n("Size %1").arg(iii), (iii+1)*2 );
    }
    //configureMenu->insertItem( "AgendaSize",configureAgendaMenu  );

    connect( configureAgendaMenu, SIGNAL( aboutToShow()),
             this, SLOT( showConfigureAgenda( ) ) );
    icon = loadPixmap( pathString + "today" );
    QAction* today_action = new QAction( i18n("Go to Today"), icon, i18n("Go to Today"), 0, this );
    today_action->addTo( actionMenu );
    connect( today_action, SIGNAL( activated() ),
             mView, SLOT( goToday() ) );

    icon = loadPixmap( pathString + "picker" );
    QAction* dPickerAction = new QAction( i18n("Select Date..."), icon, i18n("Select Date..."), 0, this );
    dPickerAction->addTo( actionMenu );
    connect( dPickerAction, SIGNAL( activated() ),
             mView, SLOT( showDatePicker() ) );
    

    icon = loadPixmap( pathString + "search" );
    QAction* search_action = new QAction( i18n("Search"), icon, i18n("Search..."), 0, this );
    search_action->addTo( actionMenu );
    connect( search_action, SIGNAL( activated() ),
             mView->dialogManager(), SLOT( showSearchDialog() ) );

    action = new QAction( i18n("Show next alarm"), i18n("Show next alarm"), 0, this );
    action->addTo(  actionMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( showNextAlarms() ) );

    actionMenu->insertItem( i18n("Show next conflict for"), nextConflictMenu );
  
    action = new QAction( "Undo Delete", i18n("All events"), 0, this );
    action->addTo( nextConflictMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( conflictAll() ) );

    action = new QAction( "Undo Delete", i18n("Allday events"), 0, this );
    action->addTo( nextConflictMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( conflictAllday() ) );

    action = new QAction( "Undo Delete", i18n("Events with time"), 0, this );
    action->addTo( nextConflictMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( conflictNotAll() ) );
    
    actionMenu->insertSeparator();
    
     icon = loadPixmap( pathString + "newevent" );
    QAction* ne_action = new QAction( i18n("New Event..."), icon, i18n("New Event..."), 0, this );
    ne_action->addTo( mCurrentItemMenu );
    connect( ne_action, SIGNAL( activated() ),
             mView, SLOT( newEvent() ) );
    icon = loadPixmap( pathString + "newtodo" );
    QAction* nt_action = new QAction( i18n("New Todo..."), icon, i18n("New Todo..."), 0, this );
    nt_action->addTo( mCurrentItemMenu );
    connect( nt_action, SIGNAL( activated() ),
             mView, SLOT( newTodo() ) );
    mNewSubTodoAction = new QAction( "new_subtodo", i18n("New Sub-Todo..."), 0,
                                     this );
    mNewSubTodoAction->addTo(  mCurrentItemMenu );
    connect( mNewSubTodoAction, SIGNAL( activated() ),
             mView, SLOT( newSubTodo() ) );
   
    mCurrentItemMenu->insertSeparator();
    icon = loadPixmap( pathString + "newevent" );
    configureToolBarMenu->insertItem(i18n("Stretched TB"), 5 );
    configureToolBarMenu->insertItem(i18n("Only one toolbar"), 6 );
    configureToolBarMenu->insertSeparator();
    configureToolBarMenu->insertItem(i18n("Filtermenu"), 7 );
    configureToolBarMenu->insertSeparator();
    configureToolBarMenu->insertItem(i18n("Week Number"), 400);
    configureToolBarMenu->insertItem(icon, i18n("New Event..."), 10 );
    icon = loadPixmap( pathString + "newtodo" );
    configureToolBarMenu->insertItem(icon, i18n("New Todo..."), 20 );

    //actionMenu->insertItem (  i18n("Selected Item"),  mCurrentItemMenu);
    mShowAction = new QAction( "show_incidence", i18n("Show"), 0, this );
    mShowAction->addTo( mCurrentItemMenu  );
    connect( mShowAction, SIGNAL( activated() ),
             mView, SLOT( showIncidence() ) );

    mEditAction = new QAction( "edit_incidence", i18n("Edit..."), 0, this );
    mEditAction->addTo( mCurrentItemMenu  );
    connect( mEditAction, SIGNAL( activated() ),
             mView, SLOT( editIncidence() ) );

    mDeleteAction = new QAction( "delete_incidence", i18n("Delete..."), 0, this );
    mDeleteAction->addTo(  mCurrentItemMenu );
    connect( mDeleteAction, SIGNAL( activated() ),
             mView, SLOT( deleteIncidence() ) );


    mCloneAction = new QAction( "clone_incidence", i18n("Clone..."), 0, this );
    mCloneAction->addTo( mCurrentItemMenu  );
    connect( mCloneAction, SIGNAL( activated() ),
             mView, SLOT( cloneIncidence() ) );
    mMoveAction = new QAction( "Move_incidence", i18n("Move..."), 0, this );
    mMoveAction->addTo( mCurrentItemMenu  );
    connect( mMoveAction, SIGNAL( activated() ),
             mView, SLOT( moveIncidence() ) );
#ifndef DESKTOP_VERSION
    mBeamAction = new QAction( "Beam_incidence", i18n("Beam..."), 0, this );
    mBeamAction->addTo(mCurrentItemMenu  );
    connect( mBeamAction, SIGNAL( activated() ),
             mView, SLOT( beamIncidence() ) );
#endif
    mCancelAction = new QAction( "Cancel_incidence", i18n("Toggle Cancel"), 0, this );
    mCancelAction->addTo( mCurrentItemMenu  );
    connect( mCancelAction, SIGNAL( activated() ),
             mView, SLOT( toggleCancelIncidence() ) );

  
   mCurrentItemMenu->insertSeparator();
    action = new QAction( i18n("Undo Delete"), i18n("Undo Delete..."), 0, this );
    action->addTo( mCurrentItemMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( undo_delete() ) );

    // ***********************
    if ( KOPrefs::instance()->mVerticalScreen ) {
         icon = SmallIcon( "1updownarrow" );
    } else {
        icon = SmallIcon("1leftrightarrow" );
    }
    configureToolBarMenu->insertItem(icon, i18n("Toggle Fullscreen"), 28 );
    QAction* FSaction = new QAction( i18n("Toggle Fullscreen"), icon, i18n("Toggle Fullscreen"), 0, this );
    FSaction->addTo( viewMenu );
    connect( FSaction, SIGNAL( activated() ), mView, SLOT( toggleExpand() ));

   
    icon = loadPixmap( pathString + "filter" );
    configureToolBarMenu->insertItem(icon, i18n("Filter menu icon"), 26 );
    icon = loadPixmap( pathString + "configure" );
    action = new QAction( i18n("Toggle Resource View"), icon, i18n("Toggle Resource View"), 0, this );
    action->addTo( viewMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( toggleFilter() ) );
    mToggleFilter = action;
    icon = loadPixmap( pathString + "navi" );
    configureToolBarMenu->insertItem(icon, i18n("Toggle DateNavigator"), 22 );
    action = new QAction( i18n("Toggle DateNavigator"), icon, i18n("Toggle DateNavigator"), 0, this );
    action->addTo( viewMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( toggleDateNavigatorWidget() ) );
    mToggleNav = action ;
    icon = loadPixmap( pathString + "allday" );
    configureToolBarMenu->insertItem(icon, i18n("Toggle Allday"), 24 );
    action = new QAction( i18n("Toggle Allday"), icon,i18n("Toggle Allday"), 0, this );
    action->addTo( viewMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( toggleAllDaySize() ) );
    mToggleAllday = action;


    connect( mView->viewManager(), SIGNAL( signalFullScreen( bool ) ),
             mToggleNav, SLOT( setEnabled ( bool ) ) );
    //connect( mView->viewManager(), SIGNAL( signalFullScreen( bool ) ),
    //       mToggleFilter, SLOT( setEnabled ( bool ) ) );
    connect( mView->viewManager(), SIGNAL( signalAgendaView( bool ) ),
             mToggleAllday, SLOT( setEnabled ( bool ) ) );
    // connect( mView->viewManager(), SIGNAL( signalAgendaView( bool ) ),
    //       configureAgendaMenu, SLOT( setEnabled ( bool ) ) );

    
    dPickerAction->addTo( iconToolBar  );
    viewMenu->insertSeparator();

    if ( p-> mShowIconToggleFull )
        FSaction->addTo( iconToolBar );
    if ( p->mShowIconNavigator ) mToggleNav ->addTo( iconToolBar );
   
    //********************
    if ( p->mShowIconAllday ) mToggleAllday->addTo( iconToolBar );


    icon = loadPixmap( pathString + "whatsnext" );
    configureToolBarMenu->insertItem(icon, i18n("What's Next"), 110 );
    QAction* whatsnext_action = new QAction( i18n("What's Next"), icon, i18n("What's Next"), 0, this );
    whatsnext_action->addTo( viewMenu );
    connect( whatsnext_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showWhatsNextView() ) );

    icon = loadPixmap( pathString + "xdays" );
    configureToolBarMenu->insertItem(icon, i18n("Next days"), 100 );
    QAction* xdays_action = new QAction( i18n("Next days"), icon, i18n("Next days"), 0, this );
    xdays_action->addTo( viewMenu );
    connect( xdays_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showNextXView() ) );
   

    icon = loadPixmap( pathString + "journal" );
    configureToolBarMenu->insertItem(icon, i18n("Journal"), 90 );
    QAction* viewjournal_action = new QAction( i18n("Journal"), icon, i18n("Journal"), 0, this );
    viewjournal_action->addTo( viewMenu );
    connect( viewjournal_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showJournalView() ) );


    icon = loadPixmap( pathString + "day" );
    configureToolBarMenu->insertItem(icon, i18n("Day View"), 40 );
    QAction* day1_action = new QAction( i18n("Day View"), icon, i18n("Day View"), 0, this );
    day1_action->addTo( viewMenu );
    //  action->addTo( toolBar );
    connect( day1_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showDayView() ) );

    icon = loadPixmap( pathString + "workweek" );
    configureToolBarMenu->insertItem(icon, i18n("Work Week"), 50 );
    QAction* day5_action = new QAction( i18n("Work Week"), icon, i18n("Work Week"), 0, this );
    day5_action->addTo( viewMenu );
    connect( day5_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showWorkWeekView() ) );

    icon = loadPixmap( pathString + "week" );
    configureToolBarMenu->insertItem(icon, i18n("Week"), 60 );
    QAction* day7_action = new QAction( i18n("Week"), icon, i18n("Week"), 0, this );
    day7_action->addTo( viewMenu );
    connect( day7_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showWeekView() ) );
    
    icon = loadPixmap( pathString + "workweek2" );
    configureToolBarMenu->insertItem(icon, i18n("List week view"), 75 );
    QAction* day6_action = new QAction( i18n("List week"), icon, i18n("List week"), 0, this );
    day6_action->addTo( viewMenu );
    connect( day6_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showMonthViewWeek() ) );

    icon = loadPixmap( pathString + "month" );
    configureToolBarMenu->insertItem(icon, i18n("Month"), 70 );
    QAction* month_action = new QAction( i18n("Month"), icon, i18n("Month"), 0, this );
    month_action->addTo( viewMenu );
    connect( month_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showMonthView() ) );

     icon = loadPixmap( pathString + "list" );
    configureToolBarMenu->insertItem(icon, i18n("List View"), 30 );
    QAction* showlist_action = new QAction( i18n("List View"), icon, i18n("List View"), 0, this );
    showlist_action->addTo( viewMenu );
    connect( showlist_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showListView() ) );

    icon = loadPixmap( pathString + "todo" );
    configureToolBarMenu->insertItem(icon, i18n("Todo View"), 80 );
    QAction* todoview_action = new QAction( i18n("Todo View"), icon, i18n("Todo View"), 0, this );
    todoview_action->addTo( viewMenu );
    connect( todoview_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showTodoView() ) );



#if 0
    action = new QAction( "view_timespan", "Time Span", 0, this );
    action->addTo( viewMenu );
    connect( action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showTimeSpanView() ) );
#endif

 

    action = new QAction( "purge_completed", i18n("Purge Completed..."), 0,
                          this );
    action->addTo( actionMenu );
    connect( action, SIGNAL( activated() ), mView, SLOT( purgeCompleted() ) );

   
    icon = loadPixmap( pathString + "search" );
    configureToolBarMenu->insertItem(icon, i18n("Search"), 120 , 5);



    actionMenu->insertSeparator();
     action = new QAction( "manage cat", i18n("Edit category list..."), 0,
                          this );
    action->addTo(  actionMenu );
    connect( action, SIGNAL( activated() ), mView, SLOT( editCategories() ) );

     action = new QAction( "manage cat", i18n("Manage new categories..."), 0,
                          this );
    action->addTo(  actionMenu );
    connect( action, SIGNAL( activated() ), mView, SLOT( manageCategories() ) );


    actionMenu->insertSeparator();
    //lll
    daysinWNviewMenu->insertItem("3" + i18n(" days"), 3 );
    daysinWNviewMenu->insertItem("7" + i18n(" days"), 7 );
    daysinWNviewMenu->insertItem("14" + i18n(" days"), 14 );
    daysinWNviewMenu->insertItem("30" + i18n(" days"), 30 );
    daysinWNviewMenu->insertItem("90" + i18n(" days"), 90 );
    connect( daysinWNviewMenu, SIGNAL( activated( int ) ),this,  SLOT(slot_setdaysinWNview( int ) ) );

    actionMenu->insertItem( i18n("Set days in What's Next view"),daysinWNviewMenu  );
    actionMenu->insertSeparator();
    icon = loadPixmap( pathString + "configure" );
    action = new QAction( i18n("Configure"),icon, i18n("Configure KO/Pi..."), 0, this );
    action->addTo( actionMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( edit_options() ) );  
    action = new QAction( i18n("Configure"),icon, i18n("Configure Calendar Files..."), 0, this );
    action->addTo( actionMenu );
    connect( action, SIGNAL( activated() ),
             this, SLOT( calHint() ) );  
    action = new QAction( i18n("Configure"),icon, i18n("Global Settings..."), 0, this );
    action->addTo( actionMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( edit_global_options() ) );
    if ( KOPrefs::instance()->mShowFullMenu ) {
        actionMenu->insertItem( i18n("Configure Toolbar"),configureToolBarMenu );

    }
    // actionMenu->insertSeparator();
    action = new QAction( "import_qtopia", i18n("Import (*.ics/*.vcs) file"), 0,
                          this );
    action->addTo( importMenu_X );
    connect( action, SIGNAL( activated() ), SLOT( importIcal() ) );
    action = new QAction( "import_quick", i18n("Import last file"), 0,
                          this );
    action->addTo( importMenu_X );
    connect( action, SIGNAL( activated() ), SLOT( quickImportIcal() ) );
    importMenu_X->insertSeparator();
    action = new QAction( "import_bday", i18n("Import Birthdays (KA/Pi)"), 0,
                          this );
    action->addTo( importMenu_X );
    connect( action, SIGNAL( activated() ), SLOT( importBday() ) );
    //#ifndef DESKTOP_VERSION
    importMenu_X->insertSeparator();
    action = new QAction( "import_qtopia", i18n("Import Opie/Qtopia Cal."), 0,
                          this );
    action->addTo( importMenu_X );
    connect( action, SIGNAL( activated() ), SLOT( importQtopia() ) );
    //#else
#ifdef _OL_IMPORT_
    importMenu_X->insertSeparator();
    action = new QAction( "import_ol", i18n("Import from OL"), 0,
                          this );
    action->addTo( importMenu_X );
    connect( action, SIGNAL( activated() ), SLOT( importOL() ) );
#endif
    //#endif

    //importMenu->insertSeparator();
#if 0
    action = new QAction( "load_cal", i18n("Load Calendar Backup"), 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), SLOT( loadCalendar() ) );
#endif
    action = new QAction( "save_cal", i18n("Save Backup..."), 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), SLOT( saveCalendar() ) );
    importMenu->insertSeparator();
    importMenu->insertItem( i18n("Import"), importMenu_X );
    //importMenu->insertSeparator();
    action = new QAction( "export ical", i18n("Export All Data"), 0,
                          this );
    action->addTo( exportMenu_X );    
    connect( action, SIGNAL( activated() ), SLOT( exportICalendar() ) );
    action = new QAction( "import_qtopia", i18n("Export VCalendar"), 0,
                          this );
    action->addTo( exportMenu_X );    
    connect( action, SIGNAL( activated() ), SLOT( exportVCalendar() ) );


    //LR
    QPopupMenu *ex2phone = new QPopupMenu( this );
    ex2phone->insertItem(i18n("Complete calendar..."), 1 );
    ex2phone->insertItem(i18n("Filtered calendar..."), 2 );
    connect( ex2phone, SIGNAL( activated(int) ), this, SLOT( exportToPhone( int)) );
    exportMenu_X->insertItem( i18n("Export to phone"), ex2phone );

    importMenu->insertItem( i18n("Export"), exportMenu_X );
    mPrintSelAction = 0;
#ifndef DESKTOP_VERSION
    //importMenu->insertSeparator();
    brAction = new QAction( "beam toggle", i18n("Beam receive enabled"), 0,
                          this );
    brAction->addTo( beamMenu_X ); 
    brAction->setToggleAction (true ) ;
    connect( brAction, SIGNAL( activated() ), this, SLOT( toggleBeamReceive() ) );

    action = new QAction( "beam all", i18n("Beam complete calendar..."), 0,
                          this );
    action->addTo( beamMenu_X );
    connect( action, SIGNAL( activated() ), mView, SLOT( beamCalendar() ) );

    action = new QAction( "beam all", i18n("Beam filtered calendar..."), 0,
                          this );
    action->addTo( beamMenu_X );
    connect( action, SIGNAL( activated() ), mView, SLOT( beamFilteredCalendar()) );
    importMenu->insertItem( i18n("Beam"), beamMenu_X );
#else
    //importMenu->insertSeparator();
    icon = loadPixmap( pathString + "print" );
    action = new QAction( i18n("Print calendar..."),icon,i18n("Print calendar..."), 0, this );
    action->addTo( beamMenu_X );
    connect( action, SIGNAL( activated() ),
             this, SLOT( printCal() ) );

    icon = loadPixmap( pathString + "week" );
    action = new QAction( i18n("Print agenda selection..."),icon,i18n("Print agenda selection..."), 0, this );
    action->addTo( beamMenu_X );
    connect( action, SIGNAL( activated() ),
             this, SLOT( printSel() ) );  
    icon = loadPixmap( pathString + "whatsnext" );
    action = new QAction( i18n("Print What's Next View..."),icon,i18n("Print What's Next View..."), 0, this );
    action->addTo( beamMenu_X );
    connect( action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( slotprintWNV() ) );

 
    icon = loadPixmap( pathString + "list" );
    action = new QAction( i18n("Print List View..."),icon,i18n("Print List View..."), 0, this );
    action->addTo( beamMenu_X );
    connect( action, SIGNAL( activated() ),
             this, SLOT( printListView() ) );

    icon = loadPixmap( pathString + "newevent" );
   action = new QAction( i18n("Print selected event / todo..."),icon,i18n("Print selected event / todo..."), 0, this );
    action->addTo( beamMenu_X );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( slotprintSelInc() ) );
    mPrintSelAction = action;
    importMenu->insertItem( i18n("Print"), beamMenu_X );
#endif
    
    importMenu->insertSeparator();
    action = new QAction( "beam all", i18n("Save"), 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), this, SLOT( save() ) );
    action = new QAction( "beam all", i18n("Exit (+save)"), 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), this, SLOT( close() ) );

    //menuBar->insertItem( "Configure",configureMenu );
    //configureMenu->insertItem( "Toolbar",configureToolBarMenu  );
    icon = loadPixmap( "korganizer/korganizer" );

    action = new QAction( "Whats New", i18n("What's new?"), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( whatsNew() ) );
    action = new QAction( "featureHowto", i18n("Features + hints..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( features() ) );
    action = new QAction( "Keys + Colors", i18n("Keys + Colors..."), 0, this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( keyBindings() ) ); 
    action = new QAction( "Storage Howto", i18n("Storage HowTo..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( storagehowto() ) ); 
    action = new QAction( "Timetracking Howto", i18n("Timetracking HowTo..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( timetrackinghowto() ) );
    action = new QAction( "Sync Howto", i18n("Sync HowTo..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( synchowto() ) );
    action = new QAction( "KDE Sync Howto", i18n("KDE Sync HowTo..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( kdesynchowto() ) );
    action = new QAction( "Multi Sync Howto", i18n("Multi Sync HowTo..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( multisynchowto() ) );
    action = new QAction( "Auto saving", i18n("Auto saving..."), 0, this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( aboutAutoSaving() ) );
    action = new QAction( "Problemd", i18n("Known Problems..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( aboutKnownBugs() ) );
    action = new QAction( "Translate Howto", i18n("User translation..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( usertrans() ) );
    action = new QAction( "Frequently asked questions", i18n("FAQ..."), 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( faq() ) );
    action = new QAction( "licence", i18n("Licence..."), 0, this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( licence() ) );
    action = new QAction( "about", i18n("About..."), 0, this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( about() ) );
    //menuBar->insertSeparator();

    // ******************************************************
    // menubar icons


    
    //menuBar->insertItem( iconToolBar );
    //xdays_action
    if (p-> mShowIconNewEvent)
        ne_action->addTo( iconToolBar  );
    if (p->mShowIconNewTodo )
        nt_action->addTo( iconToolBar  );
    if (p-> mShowIconSearch)
        search_action->addTo( iconToolBar  );
    if (p-> mShowIconWhatsThis)
        QWhatsThis::whatsThisButton (  iconToolBar );
    if (p-> mShowIconNext)
        whatsnext_action->addTo( viewToolBar  );
    if (p-> mShowIconNextDays)
        xdays_action->addTo( viewToolBar  );
    if (p-> mShowIconJournal)
        viewjournal_action->addTo( viewToolBar  );
    if (p-> mShowIconDay1)
        day1_action->addTo( viewToolBar  );
    if (p-> mShowIconDay5)
        day5_action->addTo( viewToolBar  );
    if (p-> mShowIconDay7)
        day7_action->addTo( viewToolBar  );
    if (p-> mShowIconDay6)
        day6_action->addTo( viewToolBar  );
    if (p-> mShowIconMonth)
        month_action->addTo( viewToolBar  ); 
    if (p-> mShowIconList)
        showlist_action->addTo( viewToolBar  );
    if (p-> mShowIconTodoview)
        todoview_action->addTo( viewToolBar  );   
   
    icon = loadPixmap( pathString + "2leftarrowB" );
    configureToolBarMenu->insertItem(icon, i18n("Prev. month"), 200);
    if (p-> mShowIconBackFast) {
        action = new QAction( i18n("Prev. month"), icon, i18n("Prev. month"),0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goPreviousMonth() ) );
        action->addTo( navigatorToolBar  );
    }
    icon = loadPixmap( pathString + "1leftarrowB" );
    configureToolBarMenu->insertItem(icon, i18n("Go backward"), 210);
    if (p-> mShowIconBack) {
        action = new QAction( i18n("Go backward"), icon, i18n("Go backward"),0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goPrevious() ) );
        action->addTo( navigatorToolBar  );
    } 
    icon = loadPixmap( pathString + "today" );
    configureToolBarMenu->insertItem(icon, i18n("Go to Today"), 130);
    if (p-> mShowIconToday)
        today_action->addTo( navigatorToolBar  );
    icon = loadPixmap( pathString + "1rightarrowB" );
    configureToolBarMenu->insertItem(icon, i18n("Go forward"), 220);
    if (p-> mShowIconForward) {
        action = new QAction( i18n("Go forward"), icon, i18n("Go forward"),0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goNext() ) );
        action->addTo( navigatorToolBar  );
    }
    icon = loadPixmap( pathString + "2rightarrowB" );
    configureToolBarMenu->insertItem(icon, i18n("Next month"), 230);
    if (p-> mShowIconForwardFast) {
        action = new QAction( i18n("Next month"), icon, i18n("Next month"),0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goNextMonth() ) );
        action->addTo( navigatorToolBar  );
    }


    configureToolBarMenu->insertItem(i18n("What's This?"), 300, 6);


    if ( p->mShowIconNavigator ) configureToolBarMenu->setItemChecked( 22 , true);
    if ( p->mShowIconAllday ) configureToolBarMenu->setItemChecked( 24 , true);
    if ( p->mShowIconFilterview ) configureToolBarMenu->setItemChecked( 26 , true);
    if ( p->mShowIconToggleFull ) configureToolBarMenu->setItemChecked( 28 , true);

    if (p-> mShowIconNewEvent)
        configureToolBarMenu->setItemChecked( 10, true );
    if (p->mShowIconNewTodo )
        configureToolBarMenu->setItemChecked( 20, true );
    if (p-> mShowIconSearch)
        configureToolBarMenu->setItemChecked( 120, true );
    if (p-> mShowIconList)
        configureToolBarMenu->setItemChecked( 30, true );
    if (p-> mShowIconDay1)
        configureToolBarMenu->setItemChecked( 40, true );
    if (p-> mShowIconDay5)
        configureToolBarMenu->setItemChecked( 50, true );
    if (p-> mShowIconDay6)
        configureToolBarMenu->setItemChecked( 75, true );
    if (p-> mShowIconDay7)
        configureToolBarMenu->setItemChecked( 60, true );
    if (p-> mShowIconMonth)
        configureToolBarMenu->setItemChecked( 70, true );
    if (p-> mShowIconTodoview)
        configureToolBarMenu->setItemChecked( 80, true );
    if (p-> mShowIconBackFast)
        configureToolBarMenu->setItemChecked( 200, true );
    if (p-> mShowIconBack)
        configureToolBarMenu->setItemChecked( 210, true );
    if (p-> mShowIconToday)
        configureToolBarMenu->setItemChecked( 130, true );
    if (p-> mShowIconForward)
        configureToolBarMenu->setItemChecked( 220, true );
    if (p-> mShowIconForwardFast)
        configureToolBarMenu->setItemChecked( 230, true );
    if (p-> mShowIconNextDays)
        configureToolBarMenu->setItemChecked( 100, true );
    if (p-> mShowIconNext)
        configureToolBarMenu->setItemChecked( 110, true );
    if (p-> mShowIconJournal)
        configureToolBarMenu->setItemChecked( 90, true );
    if (p-> mShowIconWhatsThis)
        configureToolBarMenu->setItemChecked( 300, true );
    if (p-> mShowIconWeekNum)
        configureToolBarMenu->setItemChecked( 400, true );
    if (!p-> mShowIconStretch) {
        QLabel* dummy = new QLabel( iconToolBar );
        dummy->setBackgroundColor( iconToolBar->backgroundColor() );
        dummy->setMinimumWidth( 0 );
        iconToolBar->setStretchableWidget ( dummy ) ;
    }
    else {
        iconToolBar->setHorizontalStretchable (true );
        viewToolBar->setHorizontalStretchable (true );
        navigatorToolBar->setHorizontalStretchable (true );
        iconToolBar->setVerticalStretchable (true );
        viewToolBar->setVerticalStretchable (true );
        navigatorToolBar->setVerticalStretchable (true );
        configureToolBarMenu->setItemChecked( 5, true );
    }
    if (p-> mShowIconFilter) 
        configureToolBarMenu->setItemChecked( 7, true );
    if (p-> mShowIconOnetoolbar) 
        configureToolBarMenu->setItemChecked( 6, true );
   

    if ( filterMenubar ) {
        filterMenubar->reparent(filterToolBar,0,QPoint(0,0) );
        connect( mView, SIGNAL( filtersUpdated() ), SLOT( updateFilterToolbar() ) );
    }
    connect( configureToolBarMenu, SIGNAL( activated( int ) ),this,  SLOT(configureToolBar( int ) ) );
    configureAgenda( p->mHourSize );
    connect( configureAgendaMenu, SIGNAL( activated( int ) ),this,  SLOT(configureAgenda( int ) ) );
}
void MainWindow::calHint()
{
    QString message = i18n("You can use and display <b>more than one</b> calendar file in KO/Pi. A calendar file is called a <b>resource</b>. To add a calendar or change calendar settings please use menu: <b>View -> Toggle Resource View</b>.");
    
    KMessageBox::information( this, message);
}
void MainWindow::exportToPhone( int mode )
{

    //ex2phone->insertItem(i18n("Complete calendar..."), 1 );
    //ex2phone->insertItem(i18n("Filtered calendar..."), 2 );
    KOex2phonePrefs ex2phone; 

    ex2phone.mPhoneConnection->setText( KPimGlobalPrefs::instance()->mEx2PhoneConnection );
    ex2phone.mPhoneDevice->setText( KPimGlobalPrefs::instance()->mEx2PhoneDevice );
    ex2phone.mPhoneModel->setText( KPimGlobalPrefs::instance()->mEx2PhoneModel );
    if ( mode == 1 )
        ex2phone.setCaption(i18n("Export complete calendar")); 
    if ( mode == 2 )
        ex2phone.setCaption(i18n("Export filtered calendar")); 

    if ( !ex2phone.exec() ) {
        return;
    }
    KPimGlobalPrefs::instance()->mEx2PhoneConnection  = ex2phone.mPhoneConnection->text();
    KPimGlobalPrefs::instance()->mEx2PhoneDevice = ex2phone.mPhoneDevice->text();
    KPimGlobalPrefs::instance()->mEx2PhoneModel = ex2phone.mPhoneModel->text();

    int inFuture = 0;
    if ( ex2phone.mWriteBackFuture->isChecked() )
        inFuture =  ex2phone.mWriteBackFutureWeeks->value();
    QPtrList<Incidence> delSel;
    if ( mode == 1 )
        delSel  = mCalendar->rawIncidences(); 
    if ( mode == 2 )
        delSel  = mCalendar->incidences(); 
    CalendarLocal* cal = new CalendarLocal();
    cal->setLocalTime();
    Incidence *incidence = delSel.first();
    QDateTime cur = QDateTime::currentDateTime().addDays( -7 );
    QDateTime end = cur.addDays( ( inFuture +1 ) *7 );
    while ( incidence ) {
        if ( incidence->typeID() !=  journalID ) {
            bool add = true;
            if ( inFuture ) {
                QDateTime dt;
                if ( incidence->typeID() == todoID ) {
                    Todo * t = (Todo*)incidence;
                    if ( t->hasDueDate() )
                        dt = t->dtDue();
                    else
                        dt = cur.addSecs( 62 );
                }
                else  {
                    bool ok;
                    dt =  incidence->getNextOccurence( cur, &ok );
                    if ( !ok )
                        dt = cur.addSecs( -62 );
                }
                if ( dt < cur || dt > end ) {
                    add = false;
                }
            }
            if ( add ) {
                Incidence *in = incidence->clone();
                cal->addIncidence( in );
            }
        }
        incidence = delSel.next();
    } 
    PhoneAccess::writeConfig( KPimGlobalPrefs::instance()->mEx2PhoneDevice,
                              KPimGlobalPrefs::instance()->mEx2PhoneConnection,
                              KPimGlobalPrefs::instance()->mEx2PhoneModel );

    setCaption( i18n("Writing to phone..."));
    if ( PhoneFormat::writeToPhone( cal ) )
        setCaption( i18n("Export to phone successful!"));
    else
        setCaption( i18n("Error exporting to phone!"));
    delete cal;
}


void MainWindow::setDefaultPreferences()
{
    KOPrefs *p = KOPrefs::instance();

    p->mCompactDialogs = true;
    p->mConfirm = true;
    // p->mEnableQuickTodo = false;

}

QString MainWindow::resourcePath()
{
    return KGlobal::iconLoader()->iconPath();
}

void MainWindow::displayText( QString text ,QString cap  )
{
    QDialog dia( this, "name", true ); ;
    dia.setCaption( cap );
    QVBoxLayout* lay = new QVBoxLayout( &dia );
    lay->setSpacing( 3 );
    lay->setMargin( 3 );
    QTextBrowser tb ( &dia );
    lay->addWidget( &tb );
    tb.setText( text );
#ifdef DESKTOP_VERSION
    dia.resize( 640, 480);
#else
    dia.showMaximized();
#endif
    dia.exec();
}

void MainWindow::features()
{

    KApplication::showFile( i18n("KO/Pi Features and hints"), "kdepim/korganizer/featuresKOPI.txt" );
}

void MainWindow::usertrans()
{

    KApplication::showFile( i18n("KO/Pi User translation HowTo"), "kdepim/korganizer/usertranslationHOWTO.txt" );
}

void MainWindow::storagehowto()
{
    KApplication::showFile( "KDE-Pim/Pi Storage HowTo", "kdepim/storagehowto.txt" );
}
void MainWindow::timetrackinghowto()
{
    KApplication::showFile( "KO/Pi Timetracking HowTo", "kdepim/timetrackerhowto.txt" );
}
void MainWindow::kdesynchowto()
{
    KApplication::showFile( "KDE-Pim/Pi Synchronization HowTo", "kdepim/Zaurus-KDE_syncHowTo.txt" );
}
void MainWindow::multisynchowto()
{
    KApplication::showFile( "KDE-Pim/Pi Synchronization HowTo", "kdepim/MultiSyncHowTo.txt" );
}
void MainWindow::synchowto()
{
    KApplication::showFile( "KDE-Pim/Pi Synchronization HowTo", "kdepim/SyncHowto.txt" );
}
void  MainWindow::faq()
{
    KApplication::showFile( i18n("KO/Pi FAQ"), "kdepim/korganizer/kopiFAQ.txt" );

}
void  MainWindow::whatsNew()
{
    KApplication::showFile( "KDE-Pim/Pi Version Info", "kdepim/WhatsNew.txt" );

}
void  MainWindow::licence()
{
    KApplication::showLicence();

}
void MainWindow::about()
{
    QString version;
#include <../version>
    QMessageBox::about( this, i18n("About KOrganizer/Pi"),
                        i18n("KOrganizer/Platform-independent\n") +
                        "(KO/Pi) " + version + " - " +

#ifdef DESKTOP_VERSION
                        i18n("Desktop Edition\n") +
#else
                        i18n("PDA-Edition\nfor: Zaurus 5x00/7x0/860/3000/6000\n") +
#endif
                        i18n("(c)2004-2008 Lutz Rogowski (zautrix@rogowski.info)\nKO/Pi is based on KOrganizer\n(c)2002,2003 Cornelius Schumacher\n(schumacher@kde.org) and the KDE team.\nKOrganizer/Pi is licensed under the GPL.\nKO/Pi can be compiled for\nLinux, Zaurus-PDA and Windows\nwww.rogowski.info --- www.korganizer.org\nSpecial thanks to Michael and Ben\nfor intensive testing!") );
}
void MainWindow::keyBindings()
{
    QString cap = i18n("KO/Pi Keys + Colors");
    QString text =  i18n("<p><h2>KO/Pi key shortcuts:</h2></p>\n") +
        i18n("<p><b>H</b>: This help dialog | <b>S</b>: Search dialog</p>\n")+
        i18n("<p><b>N</b>: Switch to next view which has a toolbar icon</p>\n") +
        i18n("<p><b>A+(shift or ctrl)</b>: Show occurence of next alarm</p>\n") +
        i18n("<p><b>C+ctrl</b>: Dis/enable automatic saving</p>\n") +
        i18n("<p><b>I</b>: Show info for selected event/todo</p>\n") +
        i18n("<p><b>Space</b>: Toggle fullscreen | <b>P</b>: Date picker</p>\n")+
        i18n("<p><b>R</b>: Toggle Resource View |<b>F</b>: Edit filter </p>\n")+
        i18n("<p><b>O</b>: Filter On/Off | <b>J</b>: Journal view</p>\n")+
        i18n("<p><b>1-0</b> (+<b>ctrl</b>): Select filter 1-10 (11-20)</p>\n")+
        i18n("<p><b>X</b>: Next X days view| <b>W</b>: What's next view\n ")+
        i18n("<p><b>Q</b>: Show next date with conflicting events\n ")+
        i18n("<p><b>V</b>: Todo view | <b>L</b>: Event list view</p>\n")+
        i18n("<p><b>Z,Y</b>: Work week view | <b>U</b>: Week view</p>\n")+
        i18n("<p><b>D</b>: One day view | <b>M</b>: Month view</p>\n")+
        i18n("<p><b>K</b>: Week view in Month view syle</p>\n")+
        i18n("<p><b>E</b>: Edit selected item |<b> E+ctrl</b>: New Event</p>\n")+
        i18n("<p><b>T</b>: Goto today | <b>T+ctrl</b>: New Todo</p>\n")+
        i18n("<p><b>S+ctrl</b>: Add sub-todo | <b>X+ctrl</b>: Toggle datenavigator</p>\n")+
        i18n("<p><b>+,-</b> : Zoom in/out agenda | <b>A</b>: Toggle allday agenda height</p>\n")+
        i18n("<p><b>C</b>: Show current time in agenda view</p>\n")+
        i18n("<p><b>B</b>: Edit description (details) of selected item</p>\n")+
        i18n("<p><b>right</b>: Next week | <b>right+ctrl</b>: Next month</p>\n")+
        i18n("<p><b>left</b>: Prev. week | <b>left+ctrl</b>: Prev. month</p>\n")+
        i18n("<p><b>del,backspace</b>: Delete selected item</p>\n")+
        i18n("<p><h3>In agenda view:</h3></p>\n") +
        i18n("<p><b>up/down</b>: Scroll agenda view</p>\n")+
        i18n("<p><b>ctrl+up/down</b>: Scroll small todo view</p>\n")+
        i18n("<p><h3>In todo view:</h3></p>\n") +
        i18n("<p><b>shift+U</b>: <b>U</b>nparent todo (make root todo)</p>\n")+
        i18n("<p><b>shift+S</b>: Make <b>S</b>ubtodo (reparent todo)</p>\n")+
        i18n("<p><b>shift+P</b>: Make new <b>P</b>arent for todo selected with shift+S</p>\n")+
        i18n("<p><b>Q</b>: Toggle quick todo line edit.</p>\n")+
        i18n("<p><b>I</b>: Show info of current item+one step down.</p>\n")+
        i18n("<p><b>return</b>: Mark item as completed+one step down.</p>\n")+
        i18n("<p><b>return+shift</b>:  Mark item as not completed+one step down</p>\n")+
        i18n("<p><h3>In list view:</h3></p>\n") +
        i18n("<p><b>I</b>: Show info of current item+one step down.</p>\n")+
        i18n("<p><b>return</b>: Select item+one step down</p>\n")+
        i18n("<p><b>return+shift</b>: Deselect item+one step down</p>\n")+
        i18n("<p><b>up/down</b>: Next/prev item</p>\n")+
        i18n("<p><b>ctrl+up/down</b>:  Goto up/down by 20% of items</p>\n")+
        i18n("<p><b>shift+up/down</b>: Goto first/last item</p>\n")+
        i18n("<p><h3>In event/todo viewer:</h3></p>\n") +
        i18n("<p><b>I,C</b>: Close dialog.</p>\n")+
        i18n("<p><b>A</b>: Show agenda view.</p>\n")+
        i18n("<p><b>E</b>:  Edit item</p>\n") +
        i18n("<p><h2>KO/Pi icon colors:</h2></p>\n") +
        i18n("<p><b>(for square icons in agenda and month view)</b></p>\n") +
        i18n("<p><b>Cross</b>: Item cancelled.([c] in Whats'Next view)</p>\n")+
        i18n("<p><b>Red</b>: Alarm set.([a] in Whats'Next view)</p>\n")+
        i18n("<p><b>Blue</b>: Recurrent event.([r] in Whats'Next view)</p>\n")+
        i18n("<p><b>Dark green</b>: Information(description) available.([i] in WN view)</p>\n")+
        i18n("<p><b>Black</b>: Event/todo with attendees. You are the organizer!</p>\n")+
        i18n("<p><b>Dark yellow</b>:  Event/todo with attendees.</p>\n") +
        i18n("<p><b>White</b>: Item readonly</p>\n");
    displayText( text, cap);
}
void MainWindow::aboutAutoSaving()
{
    QString text = i18n("After changing something, the data is\nautomatically saved to the file\n~/kdepim/apps/korganizer/mycalendar.ics\nafter (configurable) three minutes.\nFor safety reasons there is  one autosaving\nafter 10 minutes (of idle time) again. The \ndata is saved automatically when closing KO/Pi\nYou can create a backup file \nwith: File - Save Calendar Backup\n");
  
    KApplication::showText( i18n("Auto Saving in  KOrganizer/Pi"), text);

}
void MainWindow::aboutKnownBugs()
{
    QMessageBox* msg;
    msg = new QMessageBox(  i18n("Known Problems in  KOrganizer/Pi"),
                            i18n("1) Importing *.vcs or *.ics files from\nother applications may not work properly,\nif there are events with properties\nKO/Pi does not support.\n")+
                            i18n("2) Audio alarm daemon\nfor Zaurus is available!\nas an additional small application\n")+
                            i18n("\nPlease report unexpected behaviour to\nzautrix@rogowski.info\n") +
                            i18n("\nor report them in the bugtracker on\n") +
                            i18n("\nhttp://sourceforge.net/projects/kdepimpi\n"),
                            QMessageBox::NoIcon,
                            QMessageBox::Ok,
                            QMessageBox::NoButton,
                            QMessageBox::NoButton);
    msg->exec();
    delete msg;

}

QString MainWindow::defaultFileName()
{
    return QDir::convertSeparators( locateLocal( "data", "korganizer/mycalendar.ics" ) );
}
QString MainWindow::syncFileName()
{
#ifdef DESKTOP_VERSION
    return  QDir::convertSeparators( locateLocal( "tmp", "synccalendar.ics" ) );
#else
    return QString( "/tmp/synccalendar.ics" );
#endif
}
#include "koglobals.h"
#include <kcalendarsystem.h>
void MainWindow::updateWeek(QDate seda)
{
    int weekNum = KGlobal::locale()->weekNum ( seda );
    mWeekPixmap.fill( mWeekBgColor );
    QPainter p ( &mWeekPixmap );
    p.setFont( mWeekFont );
    p.drawText( 0,0,mWeekPixmap.width(), mWeekPixmap.height(),AlignCenter, QString::number( weekNum) );
    p.end();   
    QIconSet icon3 ( mWeekPixmap );
    mWeekAction->setIconSet ( icon3 );

}
void MainWindow::updateWeekNum(const DateList &selectedDates)
{
    updateWeek( selectedDates.first() );
}
void MainWindow::processIncidenceSelection( Incidence *incidence )
{
    if ( !incidence ) {
        mShowAction->setMenuText( i18n("Show") );
        enableIncidenceActions( false );
        mNewSubTodoAction->setEnabled( false );
        setCaptionToDates();
        return;
    }
    QString startString = "";
    if ( incidence->typeID() != todoID ) {
        if ( incidence->dtStart().date() < incidence->dtEnd().date()   ) {
            if ( incidence->doesFloat() ) {
                startString += ": "+incidence->dtStartDateStr( true );
                startString += " --- "+((Event*)incidence)->dtEndDateStr( true );
            } else {
                startString = ": "+incidence->dtStartStr(true);
                startString += " --- "+((Event*)incidence)->dtEndStr(true);
            }
        } else {
            if ( incidence->dtStart().time() != incidence->dtEnd().time() )
                startString = ": "+KGlobal::locale()->formatTime(incidence->dtStart().time())+
                    "-"+KGlobal::locale()->formatTime(incidence->dtEnd().time());
            if ( incidence->isBirthday() || incidence->isAnniversary() ) {
                bool ok;
                QDateTime noc = incidence->getNextOccurence( mView->startDate().addDays(-1), &ok );
                if ( ok ) {
                    int years = noc.date().year() - incidence->dtStart().date().year();
                    startString += i18n(" (%1 y.)"). arg( years );
                }
            }
            else
                startString +=" "+KGlobal::locale()->formatDate( incidence->dtStart().date(), true);
        }
    }
    else {
        if ( (( KCal::Todo*)incidence)->percentComplete() == 100 )
            startString = i18n(": (Prio ") +QString::number( (( KCal::Todo*)incidence)->priority() ) +") "+i18n("completed on %1").arg( (( KCal::Todo*)incidence)->completedStr(true) );
        else
            startString = i18n(": (Prio ") +QString::number( (( KCal::Todo*)incidence)->priority() ) +") "+QString::number( (( KCal::Todo*)incidence)->percentComplete() ) +i18n("\% completed");
    }
    if ( !incidence->location().isEmpty() )
        startString += " (" +incidence->location()+")";
    setCaption( incidence->summary()+startString);
    enableIncidenceActions( true );
    if ( incidence->typeID() == eventID ) {
        mShowAction->setMenuText( i18n("Show Event") );
        mNewSubTodoAction->setEnabled( false );
    } else if ( incidence->typeID() == todoID ) {
        mShowAction->setMenuText( i18n("Show Todo") );
        mNewSubTodoAction->setEnabled( true );
    } else {
        mShowAction->setMenuText( i18n("Show") );
        mNewSubTodoAction->setEnabled( false );
    }
#ifdef DESKTOP_VERSION
    static QPixmap jP = SmallIcon( "journal" );
    static QPixmap eP = SmallIcon( "newevent" );
    static QPixmap tP = SmallIcon( "newtodo" );
    QIconSet icon;
    if ( incidence->typeID() == todoID )
        icon = QIconSet ( tP );
    else if ( incidence->typeID() == eventID )
        icon =  QIconSet ( eP );
    else if ( incidence->typeID() == journalID )
        icon =  QIconSet ( jP );
    mPrintSelAction->setIconSet ( icon );
#endif
}

void MainWindow::enableIncidenceActions( bool enabled )
{
    mShowAction->setEnabled( enabled );
    mEditAction->setEnabled( enabled );
    mDeleteAction->setEnabled( enabled );

    mCloneAction->setEnabled( enabled );
    mMoveAction->setEnabled( enabled );
#ifndef DESKTOP_VERSION
    mBeamAction->setEnabled( enabled );
#else
    mPrintSelAction->setEnabled( enabled );
#endif 
    mCancelAction->setEnabled( enabled );
}

void MainWindow::importOL()
{ 
#ifdef _OL_IMPORT_
      mView->clearAllViews();
      KOImportOLdialog *id = new KOImportOLdialog("Import from OL - select folder!" , mView->calendar(),this );
      id->exec();
      delete id;
      mView->calendar()->checkAlarmForIncidence( 0, true );
      mView->updateView();
#endif
}
void MainWindow::importBday()
{
    int result = QMessageBox::warning( this, i18n("KO/Pi import information!"),
                                       i18n("When importing birthdays twice\nduplicated events will be ignored,\nif the event has not been\nchanged in KO/Pi!\n"),
                                       i18n("Import!"), i18n("Cancel"), 0,
                                       0, 1 );
    if ( result == 0 ) {
        mView->importBday();

    }


}
void MainWindow::importQtopia()
{
    //#ifndef DESKTOP_VERSION
    QString mess = i18n("When importing a calendar twice\nduplicated events will be ignored!\nYou can create a backup file with\nFile - Save Calendar Backup\nto revert importing");
#ifdef DESKTOP_VERSION
    mess += i18n("The content of the following files will be\nimported (located in your home directory (hd)):\n(hd)/Applications/datebook/datebook.xml\n(hd)/Applications/todolist/todolist.xml\nThe following category file will be used:\n(hd)/Settings/Categories.xml");
#endif
    int result = QMessageBox::warning( this, i18n("KO/Pi: Warning!"),mess,
                                       i18n("Import!"), i18n("Cancel"), 0,
                                       0, 1 );
    if ( result == 0 ) {
#ifndef DESKTOP_VERSION
        QString datebook = Global::applicationFileName( "datebook", "datebook.xml");
        QString todolist = Global::applicationFileName( "todolist", "todolist.xml");
        QString categories = QString( getenv( "HOME" ) ) + "/Settings/Categories.xml";
#else
        QString datebook = QDir::homeDirPath()+ "/Applications/datebook/datebook.xml";
        QString todolist = QDir::homeDirPath()+ "/Applications/todolist/todolist.xml";
        QString categories = QDir::homeDirPath()+ "/Settings/Categories.xml";
#endif
        mView->importQtopia( categories, datebook, todolist );
    }
    mView->calendar()->reInitAlarmSettings();
#if 0
    int result = QMessageBox::warning( this, i18n("KO/Pi: Warning!"),
                                       i18n("Not supported \non desktop!\n"),
                                       i18n("Ok"), i18n("Cancel"), 0,
                                       0, 1 );

#endif
}

void MainWindow::saveOnClose()
{
    KOPrefs *p = KOPrefs::instance();
    p->mToolBarHor = ( iconToolBar->orientation () == Qt:: Horizontal );
    p->mToolBarHorV = ( viewToolBar->orientation () == Qt:: Horizontal );
    p->mToolBarHorN = ( navigatorToolBar->orientation () == Qt:: Horizontal );
    if ( filterToolBar ) {
        p->mToolBarHorF = ( filterToolBar->orientation () == Qt:: Horizontal );
    }
#ifdef DESKTOP_VERSION
    
    QPoint myP;
    myP = mapFromGlobal( iconToolBar->mapToGlobal( QPoint( 0,0) ) );
    if (  p->mToolBarHor )
        p->mToolBarUp = myP.y() > height()/2;
    else
        p->mToolBarUp = myP.x() > width()/2;
    myP = mapFromGlobal( viewToolBar->mapToGlobal( QPoint( 0,0) ) );
    if ( p->mToolBarHorV )
        p->mToolBarUpV = myP.y()  > height()/2;
    else
        p->mToolBarUpV = myP.x() > width()/2 ;
    myP = mapFromGlobal( navigatorToolBar->mapToGlobal( QPoint( 0,0) ) );
    if ( p->mToolBarHorN )
        p->mToolBarUpN = myP.y() > height()/2;
    else
        p->mToolBarUpN = myP.x() > width()/2 ;
    if ( filterToolBar ) {
        myP = mapFromGlobal( filterToolBar->mapToGlobal( QPoint( 0,0) ) );
        if ( p->mToolBarHorF )
            p->mToolBarUpF = myP.y() > height()/2;
        else
            p->mToolBarUpF = myP.x() > width()/2 ;
    }
#else
    if (  p->mToolBarHor )
        p->mToolBarUp = iconToolBar->y() > height()/2;
    else
        p->mToolBarUp = iconToolBar->x() > width()/2;
    if ( p->mToolBarHorV )
        p->mToolBarUpV = viewToolBar->y() > height()/2;
    else
        p->mToolBarUpV = viewToolBar->x() > width()/2 ;
    
    if ( p->mToolBarHorN )
        p->mToolBarUpN = navigatorToolBar->y() > height()/2;
    else
        p->mToolBarUpN = navigatorToolBar->x() > width()/2 ;
    if ( filterToolBar ) {
        if ( p->mToolBarHorF )
            p->mToolBarUpF = filterToolBar->y() > height()/2;
        else
            p->mToolBarUpF = filterToolBar->x() > width()/2 ;
    }
#endif
    save();
    mView->writeSettings();
    mView->checkSuspendAlarm();
}
void MainWindow::slot_setdaysinWNview( int days )
{
  KOPrefs::instance()->mWhatsNextDays = days;
  mView->updateView();
}
void MainWindow::slotModifiedChanged( bool )
{
    if ( mBlockAtStartup )
        return;
   
    int msec;
    if ( mAutoSaveDisabled ) {
        QTimer::singleShot( 0, this , SLOT ( autoSaveWarning() ) );
        qDebug("KO: Autosave disabled (data change deteced)!");
    }
    if ( mCalendarModifiedFlag ) {
        //qDebug(" MainWindow timer is running ");
        return;
    }
    mCalendarModifiedFlag = true;
    if ( mAutoSaveDisabled ) {
        return;
    }
    // we store the changes after 1 minute,
    // and for safety reasons after 10 minutes again
    if ( !mSyncManager->blockSave() )
        msec = (1000 * 60*KOPrefs::instance()->mAutoSaveInterval) +1000;
    else
        msec = 1000 * 600;
    mSaveTimer.start( msec, true ); // 1 minute
    mSaveTimerStart = QDateTime::currentDateTime();
    mSaveDelay = msec/1000;
    qDebug("KO: Saving File in %d secs!", msec/1000);
}
void MainWindow::autoSaveWarning()
{
    setCaption(i18n("Autosave disabled!" ));
}
void MainWindow::saveStopTimer()
{
    mSaveTimer.stop();
    mSaveDelay = 0;
}
void MainWindow::backupAllFiles()
{
    QDate reference ( 2000,1,1);
    int daysTo = reference.daysTo ( QDate::currentDate() );
    setCaption(i18n("Creating backup ... please wait ..." ));
    qDebug("KO: Last backup was %d days ago ", daysTo - KOPrefs::instance()->mLastBackupDate);
    // we need the file path, the backup dir and the number of bups as param
    QString bupDir = KPimGlobalPrefs::instance()->mBackupDatadir;
    if ( KPimGlobalPrefs::instance()->mBackupUseDefaultDir)
        bupDir = KGlobalSettings::backupDataDir();
    int retval =  KApplication::createBackup(  defaultFileName(), bupDir, KPimGlobalPrefs::instance()->mBackupNumbers );
    if ( retval == 0 ) {
        setCaption(i18n("Backup cancelled" ));
        qDebug("KO: Backup cancelled. Will try again tomorrow ");
        // retval == 0 : backup skipped for today, try again tomorrow
        KOPrefs::instance()->mLastBackupDate = daysTo- KPimGlobalPrefs::instance()->mBackupDayCount+1;
    } else if ( retval == 1 ){
        qDebug("KO: Backup created.");
        // backup ok
        QPtrList<KopiCalendarFile> calendars = KOPrefs::instance()->mCalendars;
        KopiCalendarFile * cal = calendars.first();
        cal = calendars.next();
        while ( cal ) {
            if ( !cal->mErrorOnLoad ) {
                KApplication::createBackup(  cal->mFileName, bupDir, KPimGlobalPrefs::instance()->mBackupNumbers );  
            }
            cal = calendars.next();
        }
        KOPrefs::instance()->mLastBackupDate =  daysTo;
        setCaption(i18n("Backup succesfully finished" ));
    } else if ( retval == 2 ){
        setCaption(i18n("Backup globally disabled" ));
        qDebug("KO: Backup globally cancelled.");
        // backup globally cancelled
        KPimGlobalPrefs::instance()->mBackupEnabled = false;
    }
    // retval == 3: do nothing, try again later
}
void MainWindow::save()
{
    if ( mSaveDelay ) {
        int elapsed = mSaveTimerStart.secsTo( QDateTime::currentDateTime() );
        if ( mSaveDelay < elapsed ) {
            qDebug("KO: Pending save after wakeup from suspend detected.");
            qDebug("KO: Save delay %d sec. Elapsed save time %d sec.", mSaveDelay, elapsed );
            qDebug("KO: Restarting save timer to save in 10 sec.");
            int msec = 10000;
            mSaveTimer.start( msec, true );
            mSaveTimerStart = QDateTime::currentDateTime();
            mSaveDelay = msec/1000;
            return;
        }
    }
    if ( mView->viewManager()->journalView() )
        mView->viewManager()->journalView()->checkModified();
    if ( !mCalendarModifiedFlag ) {
        qDebug("KO: Calendar not modified. Nothing saved.");
        return;
    }
    if ( mSyncManager->blockSave() ) {
        slotModifiedChanged( true );
        return;
    }
#ifndef DESKTOP_VERSION
    // QPEApplication::setTempScreenSaverMode ( QPEApplication::Disable );
#endif
    mSaveDelay = 0;
    mSyncManager->setBlockSave(true);
    if ( mView->checkAllFileVersions() ) {
        if ( KPimGlobalPrefs::instance()->mBackupEnabled ){
            QDate reference ( 2000,1,1);
            int daysTo = reference.daysTo ( QDate::currentDate() );
            if ( daysTo - KPimGlobalPrefs::instance()->mBackupDayCount >= KOPrefs::instance()->mLastBackupDate ) {
                backupAllFiles();
            }
            ; //  KPimGlobalPrefs::instance()->mLastBackupDate
        }
        QTime neededSaveTime;
        neededSaveTime.start();
        if ( !isMinimized () )
            setCaption(i18n("KO/Pi:Saving Data to File ..." ));
        qDebug("KO: Start saving data to file!");
        if ( mView->saveCalendars() )
            mCalendarModifiedFlag = false;
        int msNeeded = neededSaveTime.elapsed();
        qDebug("KO: Needed %d ms for saving.",msNeeded );
        QString savemes;
        savemes.sprintf(i18n("KO/Pi:File Saved. Needed %d sec, %d ms"),(msNeeded/1000)%100,msNeeded%1000  );
        if ( !isMinimized () )
            setCaption(savemes);
        else
            qDebug(savemes);
    } else {
        setCaption(i18n("Saving cancelled!"));
        mCalendarModifiedFlag = false;
        slotModifiedChanged( true );
    }
    mSyncManager->setBlockSave( false );
#ifndef DESKTOP_VERSION
    //QPEApplication::setTempScreenSaverMode ( QPEApplication::Enable );
#endif
}

void MainWindow::keyReleaseEvent ( QKeyEvent * e)
{
    if ( !e->isAutoRepeat() ) {
        mFlagKeyPressed = false;
    }
}
void MainWindow::keyPressEvent ( QKeyEvent * e )
{
    qApp->processEvents();
    if ( e->isAutoRepeat() && !mFlagKeyPressed ) {
        e->ignore();
        // qDebug("  ignore  %d",e->isAutoRepeat()  );
        return;
    }
    if (! e->isAutoRepeat() )
        mFlagKeyPressed = true;
    KOPrefs *p = KOPrefs::instance();
    bool showSelectedDates = false;
    int size;
    int pro = 0;
    //qDebug("MainWindow::keyPressEvent ");
    switch ( e->key() ) {
    case Qt::Key_Right:
        if ( e->state() == Qt::ControlButton ||  e->state() == Qt::ShiftButton)
            mView->goNextMonth();
        else
            mView->goNext();
        showSelectedDates = true;
        break;
    case Qt::Key_Left:
        if ( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton )
            mView->goPreviousMonth();
        else
            mView->goPrevious();
        showSelectedDates = true;
        break;
    case Qt::Key_Down:
        mView->viewManager()->agendaView()->scrollOneHourDown();
        break;
    case Qt::Key_Up:
        mView->viewManager()->agendaView()->scrollOneHourUp();
        break;
    case Qt::Key_K:
        mView->viewManager()->showMonthViewWeek();
        break;
    case Qt::Key_I:
        mView->showIncidence();
        break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        mView->deleteIncidence();
        break;
    case Qt::Key_D:
        mView->viewManager()->showDayView();
        showSelectedDates = true;
        break;
    case Qt::Key_O:
        mView->toggleFilerEnabled( );
        break;
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        pro =  e->key()-48;
        if ( pro == 0 )
            pro = 10;
        if ( e->state() == Qt::ControlButton)
            pro += 10;
        break;
    case Qt::Key_M:
        mView->viewManager()->showMonthView();
        showSelectedDates = true;
        break;
    case Qt::Key_Insert:
        mView->newEvent();
        break;
    case Qt::Key_S :
        if ( e->state() == Qt::ControlButton ||  e->state() == Qt::ShiftButton)
            mView->newSubTodo();
        else
            mView->dialogManager()->showSearchDialog();
        break;
    case Qt::Key_Y :
    case Qt::Key_Z :
        mView->viewManager()->showWorkWeekView();
        showSelectedDates = true;
        break;
    case Qt::Key_U :
        mView->viewManager()->showWeekView();
        showSelectedDates = true;
        break;
    case Qt::Key_H :
        keyBindings();
        break;
    case Qt::Key_W:
        mView->viewManager()->showWhatsNextView();
        break;
    case Qt::Key_L:
        mView->viewManager()->showListView();
        break;
    case Qt::Key_N:
        mView->viewManager()->showNextView();
        break;
    case Qt::Key_V:
        mView->viewManager()->showTodoView();
        break;
    case Qt::Key_C:  
        if ( e->state() == Qt::ControlButton ||  e->state() == Qt::ShiftButton ) {
            if ( mAutoSaveDisabled ) {
                mAutoSaveDisabled = false;
                setCaption(i18n("Autosave enabled!" ));
                return;
            } else {
                mAutoSaveDisabled = true;
                saveStopTimer();
                setCaption(i18n("Autosave disabled! Save timer stopped!" ));
            }
        } else
            mView->viewManager()->agendaView()->setStartHour( QTime::currentTime ().hour() );
        break;
    case Qt::Key_P:
        mView->showDatePicker( );
        break;
    case Qt::Key_F:
        mView->editFilters();
        break;
    case Qt::Key_R:
        mView->toggleFilter();
        break;
    case Qt::Key_X: 
        if ( e->state() == Qt::ControlButton )
            mView->toggleDateNavigatorWidget();
        else {
            mView->viewManager()->showNextXView();
            showSelectedDates = true;
        }
        break;
    case Qt::Key_Space:
        mView->toggleExpand();
        break;
    case Qt::Key_A:
        if ( e->state() == Qt::ControlButton ||  e->state() == Qt::ShiftButton )
            mView->showNextAlarms();
        else
            mView->toggleAllDaySize();
        break;
    case Qt::Key_T:
        if ( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton )
            mView->newTodo();
        else {
            mView->goToday();
            showSelectedDates = true;
        }
        break;
    case Qt::Key_Q:
        if ( e->state() == Qt::ControlButton )
            mView->conflictNotAll();
        else if ( e->state() == Qt::ShiftButton )
            mView->conflictAllday();
        else
            mView->conflictAll();
        break;
    case Qt::Key_J:
        mView->viewManager()->showJournalView();
        break;
    case Qt::Key_B:
        mView->editIncidenceDescription();;
        break;
        // case Qt::Key_Return:
    case Qt::Key_E:
        if ( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton )
            mView->newEvent();
        else
            mView->editIncidence();
        break;
    case Qt::Key_Plus:
        size = p->mHourSize +2;
        if ( size <= 22 )
            configureAgenda( size );
        break;
    case Qt::Key_Minus:
        size = p->mHourSize - 2;
        if ( size >= 4 )
            configureAgenda( size );
        break;


    default:
        e->ignore();
    }
    if ( pro > 0 ) {
        selectFilter(  pro+1 );
    }
    if ( showSelectedDates ) {
        ;//  setCaptionToDates();
    }

}
void MainWindow::fillFilterMenuTB()
{
    selectFilterMenuTB->clear();
    selectFilterMenuTB->insertItem(i18n ( "Edit Filters" ),  0 );
    selectFilterMenuTB->insertSeparator();
    selectFilterMenuTB->insertItem(i18n ( "No Filter" ),  1 );
   
    selectFilterMenuTB->insertSeparator();
    QPtrList<CalFilter> fili = mView->filters();
    CalFilter *curfilter = mView->filterView()->selectedFilter();
    CalFilter *filter = fili.first();
    int iii = 2;
    bool checkitem = mView->filterView()->filtersEnabled();
    while(filter) {
        selectFilterMenuTB->insertItem( filter->name(),  iii );
        if ( filter == curfilter)
            selectFilterMenuTB->setItemChecked( iii, checkitem );
        filter = fili.next();
        ++iii;
    }
    if ( !checkitem )
        selectFilterMenuTB->setItemChecked( 1, true );

    int x = 0;
    int y = iconToolBar->height();
    int dX = 0;
    int dY = 0;
    if ( iconToolBar->orientation () == Qt:: Horizontal ) {
        if ( iconToolBar->y() > height()/2 ) {
            dY = selectFilterMenuTB->sizeHint().height()+8;
            y = 0;
        }
    } else {
        if ( iconToolBar->x() > width()/2 ) { // right side
            x=0;
            dX= selectFilterMenuTB->sizeHint().width()+8;
            y = 0;
        } else {
            x= iconToolBar->width();
            y = 0;
        }
    }
    //qDebug("dax %d  dy %d         %d %d ", dX, dY, iconToolBar->x(), iconToolBar->y() );
    selectFilterMenuTB->popup(iconToolBar->mapToGlobal(QPoint(x,y)-QPoint( dX,dY)));
}
void MainWindow::fillFilterMenu()
{
    selectFilterMenu->clear();
    selectFilterMenu->insertItem(i18n ( "Edit Filters" ),  0 );
    selectFilterMenu->insertSeparator();
    selectFilterMenu->insertItem(i18n ( "No Filter" ),  1 );
   
    selectFilterMenu->insertSeparator();
    QPtrList<CalFilter> fili = mView->filters();
    CalFilter *curfilter = mView->filterView()->selectedFilter();
    CalFilter *filter = fili.first();
    int iii = 2;
    bool checkitem = mView->filterView()->filtersEnabled();
    while(filter) {
        selectFilterMenu->insertItem( filter->name(),  iii );
        if ( filter == curfilter)
            selectFilterMenu->setItemChecked( iii, checkitem );
        filter = fili.next();
        ++iii;
    }
    if ( !checkitem )
        selectFilterMenu->setItemChecked( 1, true );
}
void MainWindow::fillFilterMenuPopup()
{
    filterPopupMenu->clear();
    filterPopupMenu->insertItem(i18n ( "No Filter" ),  0 );
   
    filterPopupMenu->insertSeparator();
    QPtrList<CalFilter> fili = mView->filters();
    CalFilter *curfilter = mView->filterView()->selectedFilter();
    CalFilter *filter = fili.first();
    int iii = 1;
    bool checkitem = mView->filterView()->filtersEnabled();
    while(filter) {
        filterPopupMenu->insertItem( filter->name(),  iii );
        if ( filter == curfilter)
            filterPopupMenu->setItemChecked( iii, checkitem );
        filter = fili.next();
        ++iii;
    }
    if ( !checkitem )
        filterPopupMenu->setItemChecked( 0, true ); 
}
void MainWindow::selectFilter( int fil )
{

    if ( fil == 0 ) {
        mView->editFilters( );
    } else if ( fil == 1 ){
        if ( mView->filterView()->filtersEnabled() )
            mView->toggleFilerEnabled( );
    } else {
        if ( !mView->filterView()->filtersEnabled() ) {
            mView->filterView()->blockSignals( true );
            mView->toggleFilerEnabled( );
            mView->filterView()->blockSignals( false );
        }
        mView->selectFilter(  fil-2 );
    } 
}
void MainWindow::updateFilterToolbar()
{
    if ( filterMenubar ) {
        if ( !mView->filterView()->filtersEnabled() ) {
            filterMenubar->changeItem( 0, i18n("No Filter") );
        } else {
            CalFilter *curfilter = mView->filterView()->selectedFilter();
            if ( curfilter ) {
                QString name = curfilter->name();
                if ( name.length() > 12 )
                    name = name.left(10)+"...";
                filterMenubar->changeItem( 0, name );
            }
        }
    }
}
void MainWindow::selectFilterPopup( int fil )
{
    selectFilter( fil + 1 );
   
}
void MainWindow::configureToolBar( int item  )
{

    configureToolBarMenu->setItemChecked( item, !configureToolBarMenu-> isItemChecked ( item ) );
    KOPrefs *p = KOPrefs::instance();
    p-> mShowIconStretch= configureToolBarMenu->isItemChecked( 5 );
    p-> mShowIconOnetoolbar = configureToolBarMenu->isItemChecked( 6 );
    p-> mShowIconFilter = configureToolBarMenu->isItemChecked( 7 );
    p-> mShowIconNewEvent= configureToolBarMenu->isItemChecked( 10 );
    p->mShowIconNewTodo = configureToolBarMenu->isItemChecked( 20 );
    p->mShowIconNavigator = configureToolBarMenu->isItemChecked( 22 );
    p->mShowIconAllday = configureToolBarMenu->isItemChecked( 24 );
    p->mShowIconFilterview = configureToolBarMenu->isItemChecked( 26 );
    p->mShowIconToggleFull = configureToolBarMenu->isItemChecked( 28 );
    p-> mShowIconSearch= configureToolBarMenu->isItemChecked( 120 );
    p-> mShowIconList= configureToolBarMenu->isItemChecked( 30 );
    p-> mShowIconDay1= configureToolBarMenu->isItemChecked( 40 );
    p-> mShowIconDay5= configureToolBarMenu->isItemChecked( 50 );
    p-> mShowIconDay6= configureToolBarMenu->isItemChecked( 75 );
    p-> mShowIconDay7= configureToolBarMenu->isItemChecked( 60 );
    p-> mShowIconMonth= configureToolBarMenu->isItemChecked( 70 );
    p-> mShowIconTodoview= configureToolBarMenu->isItemChecked( 80 );
    p-> mShowIconBackFast= configureToolBarMenu->isItemChecked( 200 );
    p-> mShowIconBack = configureToolBarMenu->isItemChecked( 210 );
    p-> mShowIconToday= configureToolBarMenu->isItemChecked( 130 );
    p-> mShowIconForward= configureToolBarMenu->isItemChecked( 220 );
    p-> mShowIconForwardFast= configureToolBarMenu->isItemChecked( 230 );
    p-> mShowIconNextDays= configureToolBarMenu->isItemChecked( 100 );
    p-> mShowIconNext= configureToolBarMenu->isItemChecked( 110 );
    p-> mShowIconJournal= configureToolBarMenu->isItemChecked( 90 );
    p-> mShowIconWhatsThis= configureToolBarMenu->isItemChecked( 300 );
    p-> mShowIconWeekNum= configureToolBarMenu->isItemChecked( 400 );
    // initActions();
    setCaption ( i18n("Toolbar changes needs a restart!") ); 
}
void MainWindow::setCaption ( const QString & c ) 
{
    QString cap = c;
    cap.replace( QRegExp("\n"), " " );
    cap = cap.stripWhiteSpace();
    if ( cap.isEmpty() )
        cap = "KO/Pi";
    QWidget::setCaption( cap );
}
void MainWindow::setCaptionToDates()
{
    QString selDates;
    QDate date = mView->startDate();
    if ( ! date.isValid() ) {
        setCaption("");
        return;
    }
    selDates = KGlobal::locale()->formatDate( date, true);
    if  (mView->startDate() < mView->endDate() )
        selDates += " - " + KGlobal::locale()->formatDate(mView->endDate(), true); 
    else {
        QString addString;
        if ( date == QDateTime::currentDateTime().date() )
            addString = i18n("Today");
        else if ( date == QDateTime::currentDateTime().date().addDays(1) )
            addString = i18n("Tomorrow");
        if ( !addString.isEmpty() )
            selDates = addString+", "+selDates ;
    }
    setCaption( i18n("Dates: ") + selDates );

}
void MainWindow::showConfigureAgenda( )
{
    int iii;
    for ( iii = 1;iii<= 10 ;++iii ){
        configureAgendaMenu->setItemChecked( (iii+1)*2, false );
    }
    configureAgendaMenu->setItemChecked( (KOPrefs::instance()->mHourSize/2)*2, true );
}
void MainWindow::configureAgenda( int item )
{
    if ( KOPrefs::instance()->mHourSize == item )
        return;
    KOPrefs::instance()->mHourSize=item;
    mView->viewManager()->agendaView()->updateConfig();
}

void  MainWindow::saveCalendar()
{
    QString bupDir = KPimGlobalPrefs::instance()->mBackupDatadir;
    if ( KPimGlobalPrefs::instance()->mBackupUseDefaultDir)
        bupDir = KGlobalSettings::backupDataDir();
    bupDir = KGlobal::formatMessage ( bupDir, 0 );
    QString bupHint;
    if ( !KPimGlobalPrefs::instance()->mBackupEnabled )
        bupHint = i18n("(Hint: You can enable automatic backup in the global settings!)");
    if ( KMessageBox::warningContinueCancel( this,  i18n("This will <b>backup all calendar files</b> to the directory %1 %2").arg(bupDir).arg(bupHint),i18n("Information") ) != KMessageBox::Continue ) return;
    bool enabled = KPimGlobalPrefs::instance()->mBackupEnabled;
    KPimGlobalPrefs::instance()->mBackupEnabled = false;
    save();
#ifndef DESKTOP_VERSION
    //QPEApplication::setTempScreenSaverMode ( QPEApplication::Disable );
#endif
    KPimGlobalPrefs::instance()->mBackupEnabled = enabled;
    backupAllFiles();
#ifndef DESKTOP_VERSION
    //QPEApplication::setTempScreenSaverMode ( QPEApplication::Enable );
#endif
}
void  MainWindow::loadCalendar()
{

   
#if 0
    QString fn = KOPrefs::instance()->mLastLoadFile;
    fn =  KFileDialog::getOpenFileName( fn, i18n("Load backup filename"), this );

    if ( fn == "" )
        return;
    QFileInfo info;
    info.setFile( fn );
    QString mess;
    bool loadbup = true;
    if ( info. exists() ) {
        mess =   i18n("Backup file from:\n%1\nLoading  backup\nfile will delete\nyour current Data!\n").arg(KGlobal::locale()->formatDateTime(info.lastModified (), true, false ));
        int result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                           mess,
                                           i18n("Load!"), i18n("Cancel"), 0,
                                           0, 1 );
        if ( result != 0 ) {
            loadbup = false;
        }
    } else {
        QMessageBox::warning( this, i18n("KO/Pi: Warning!"),
                              i18n("Backup file\ndoes not exist!\nNothing loaded!"), 0, 0,
                              0, 1 );

        return;
    }
    if ( loadbup ) {
        mView->openCalendar( fn );
        KOPrefs::instance()->mLastLoadFile = fn;
        mess = i18n("KO/Pi:Loaded %1").arg(fn) ;
        setCaption(mess);
    }
#endif

}
void  MainWindow::quickImportIcal()
{
    importFile( KOPrefs::instance()->mLastImportFile, false );
}
void MainWindow::importFile( QString  fn, bool quick )
{
    QFileInfo info;
    info.setFile( fn );
    QString mess;
    if ( !info. exists() ) {
        mess =  i18n("Import file \n...%1\ndoes not exist!\nNothing imported!\n").arg(fn.right( 30));
        QMessageBox::warning( this, i18n("KO/Pi: Warning!"),
                                           mess );
        return;
    }
    int result = 0;
    if ( !quick ) {
        mess = i18n(  "Import file \n...%1\nfrom:\n%2\nDuplicated entries\nwill not be imported!\n").arg(fn.right( 25)).arg(KGlobal::locale()->formatDateTime(info.lastModified (), true, false ));
        result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                       mess,
                                       "Import", "Cancel", 0,
                                       0, 1 );
    }
    if ( result == 0 ) {
        if ( mView->openCalendar( fn, true )) {
            KOPrefs::instance()->mLastImportFile = fn;
            setCaption(i18n("Imported file successfully"));
        } else {
            setCaption(i18n("Error importing file"));
        }
        mView->updateView();
    }
}

void   MainWindow::importIcal()
{

    QString fn =KOPrefs::instance()->mLastImportFile;

    fn =KFileDialog:: getOpenFileName( fn, i18n("Import filename(*.ics/*.vcs)"), this );
    if ( fn == "" )
        return;
    importFile(  fn, true );

}
void MainWindow::exportCalendar( bool iCalFormat )
{
    QString fn =  KOPrefs::instance()->mLastVcalFile;
    if ( iCalFormat ) {
        fn =  QDir::homeDirPath()+"/kopiexport.ics";
        fn =  KFileDialog::getSaveFileName( fn, i18n("Export iCal filename(*.ics)"), this );
    }
    else
        fn =  KFileDialog::getSaveFileName( fn, i18n("Export vcal filename(*.vcs)"), this );
    if ( fn == "" )
        return;
    QFileInfo info;
    info.setFile( fn );
    QString mes;
    bool createbup = true;
    if ( info. exists() ) {
        mes = i18n("Save file\nalready exists!\nOld save file from:\n%1\nOverwrite?\n").arg (KGlobal::locale()->formatDateTime(info.lastModified (), true, false ) );
        int result = QMessageBox::warning( this, i18n("KO/Pi: Warning!"),mes,
                                           i18n("Overwrite!"), i18n("Cancel"), 0,
                                           0, 1 );
        if ( result != 0 ) {
            createbup = false;
        }
    }
    if ( createbup ) {
        bool success = false;
        if  ( iCalFormat )
            success = mView->exportICalendar( fn );
        else
            success = mView->exportVCalendar( fn );
        if ( success ) {
            if ( fn.length() > 20 )
                mes = i18n("KO/Pi:Exported to ...%1").arg(fn.right(20)) ;
            else
                mes = i18n("KO/Pi:Exported to %1").arg(fn );
            setCaption(mes);
        }
    }

}
void MainWindow::exportICalendar()
{
    exportCalendar( true );
}
void MainWindow::exportVCalendar()
{
    exportCalendar( false );
}
QString MainWindow::sentSyncFile()
{
#ifdef DESKTOP_VERSION
    return  QDir::convertSeparators( locateLocal( "tmp", "copysynccal.ics" ) );
#else
    return QString( "/tmp/copysynccal.ics" );
#endif 
}

void MainWindow::syncFileRequest(const QString& resource)
{
    while ( mSyncManager->blockSave() ) {
        qApp->processEvents(); 
    }
    mSyncManager->setBlockSave(true);

    if ( KOPrefs::instance()->mPassiveSyncWithDesktop ) {
        mSyncManager->slotSyncMenu( 999 );
    } 

    setCaption(i18n("Saving Data to temp file ..." ));
    mView->saveCalendarResource( sentSyncFile(), resource );
    setCaption(i18n("Data saved to temp file!" ));
    mSyncManager->setBlockSave( false );

}
void MainWindow::getFile( bool success ,const QString& resource)
{
    if ( ! success ) {
        setCaption( i18n("Error receiving file. Nothing changed!") );
        return;
    }
    mView->mergeFileResource( sentSyncFile(), resource); 
    if ( KOPrefs::instance()->mPassiveSyncWithDesktop ) {
        mSyncManager->slotSyncMenu( 999 );
    } 
    setCaption( i18n("Pi-Sync successful!") );
} 
void MainWindow::printListView()
{
    
    QString message = i18n("You can make a printout of the <b>List View</b> and the list view in the <b>Search Dialog</b>! To do this, please go to the <b>List View/Search Dialog</b>. Right click on the list. Select in the popup menu the entry <b>Print complete list</b>. That prints the list as you see it. You can remove items from the list before printing without deleting the corresponding event/todo! Simply select all items you do not want to print out. Then right click on one of the items and choose <b>Hide selected items</b>. After that you can print the list without these items.");
    
    KMessageBox::information( this, message);
}
void MainWindow::printSel( )
{
    mView->viewManager()->agendaView()->agenda()->printSelection();
}

void MainWindow::printCal()
{
    mView->print();//mCp->showDialog();
}


#include "libkdepim/kdatepicker.h"
#include <kdatetbl.h>

void MainWindow::weekAction()
{
    int month;
    KPopupFrame* popup = new KPopupFrame(this);
    KDateInternalWeekPicker* picker = new KDateInternalWeekPicker(popup);
    // -----
    picker->resize(picker->sizeHint());
    popup->setMainWidget(picker);
    picker->setFocus();
    connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
    int x = 0;
    int y = iconToolBar->height();
    int dX = 0;
    int dY = 0;
    if ( iconToolBar->orientation () == Qt:: Horizontal ) {
        if ( iconToolBar->y() > height()/2 ) {
            dY = picker->sizeHint().height()+8;
            y = 0;
        }
    } else {
        if ( iconToolBar->x() > width()/2 ) { // right side
            x=0;
            dX= picker->sizeHint().width()+8;
            y = 0;
        } else {
            x= iconToolBar->width();
            y = 0;
        }
    }
    //qDebug("dax %d  dy %d         %d %d ", dX, dY, iconToolBar->x(), iconToolBar->y() );
    if(popup->exec(iconToolBar->mapToGlobal(QPoint(x,y)-QPoint( dX,dY))))
        {
            month = picker->getResult();
            emit selectWeek ( month );
            //qDebug("weekSelected %d ", month);
        }
    delete popup;
}
  
void  MainWindow::hideEvent ( QHideEvent * )
{
    QString message; 
    QDateTime nextA = mCalendar->nextAlarmEventDateTime();
    if ( nextA.isValid() ) {
        QString sum = mCalendar->nextSummary();   
        
        message = i18n("%1 %2 - %3 (next event/todo with alarm)").arg( KGlobal::locale()->formatTime(nextA.time() , false)).arg(sum ).arg( KGlobal::locale()->formatDate(nextA.date() , false));
        setCaption( message );
    } 
}

void MainWindow::resizeEvent( QResizeEvent* e)
{
#ifndef DESKTOP_VERSION
    if ( filterToolBar ) {
        if ( !KOPrefs::instance()->mShowIconFilter && !KOPrefs::instance()->mShowIconOnetoolbar && QApplication::desktop()->width() > 320) {
            if (QApplication::desktop()->width() > QApplication::desktop()->height()  )
                filterToolBar->hide();
            else
                filterToolBar->show();
        }
    }
#endif
    QMainWindow::resizeEvent( e);
}
