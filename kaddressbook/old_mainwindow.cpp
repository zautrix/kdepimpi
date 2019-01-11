#include <qlabel.h>
#include <qapp.h>
#include <qmessagebox.h>
#include <qaction.h>

#include <kdebug.h>	// defined kdDebug()
#include <klocale.h>	// defines i18n (microkde)

#include "kabprefs.h"

#ifndef DESKTOP_VERSION
#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>
#else
#include <qtoolbar.h>
#include <qmenubar.h>
#include <resource.h>
#endif

/*
#include <stdlib.h>

#include <qaction.h>
#include <qpainter.h>
#include <qwhatsthis.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qwmatrix.h> 
#ifndef DESKTOP_VERSION
#include <qpe/global.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qtopia/alarmserver.h> 
#include <qtopia/qcopenvelope_qws.h> 
#else
#include <qtoolbar.h>
#include <qdir.h>
#include <qapplication.h>
//#include <resource.h>
#endif

#include <libkcal/calendarlocal.h>
#include <libkcal/todo.h>

//#include "calendarview.h"
//#include "koviewmanager.h"
//#include "koagendaview.h"
//#include "kodialogmanager.h"
//#include "kdialogbase.h"
//#include "koprefs.h"
//#include "kfiledialog.h"
//#include "koglobals.h"

//#include "kconfig.h"
//#include "simplealarmclient.h"
*/
//US using namespace KACore;


#include "mainwindow.h"

MainWindow::MainWindow( QWidget *parent, const char *name, QString msg) :
    QMainWindow( parent, name )
{
    
#ifdef DESKTOP_VERSION
    setFont( QFont("Arial"), 14 );
#endif
//    mBlockAtStartup = true;
    kdDebug() << "MainWindow()::MainWindow()" << endl;
//    mFlagKeyPressed = false;
    setCaption("KAddressbook/Pi");
    
    KABPrefs *p = KABPrefs::instance();
    if ( QApplication::desktop()->height() > 480 ) {
//US        if ( p->mHourSize == 4 )
//US            p->mHourSize = 6;
    }  
                    
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

    iconToolBar = new QPEToolBar( this );
    addToolBar (iconToolBar , tbd );
    
/*US    
    mBlockSaveFlag = false;
    mCalendarModifiedFlag = false;  
*/    
    QLabel* splash  = new QLabel(i18n("KA/Pi is starting ... "), this );
    splash->setAlignment ( AlignCenter );
    setCentralWidget( splash );
#ifndef DESKTOP_VERSION
    showMaximized();
#endif
    qDebug("Mainwidget x %d y %d w %d  h %d",  x(), y(), width(), height ());
//    setDefaultPreferences();
/*    mCalendar = new CalendarLocal();
    mView = new CalendarView( mCalendar, this,"mCalendar " );
    mView->hide();
    //mView->resize(splash->size() );
*/    
    initActions(); 
    
#ifndef DESKTOP_VERSION
//US    iconToolBar->show(); 
    qApp->processEvents();
#endif
    qDebug("Splashwidget x %d y %d w %d  h %d", splash-> x(), splash->y(), splash->width(),splash-> height ());
    
    int vh = height() ;
    int vw = width();
    qDebug("Toolbar hei %d ",iconToolBar->height() );
/*US    if ( iconToolBar->orientation () == Qt:: Horizontal ) {
        vh -=  iconToolBar->height();
    } else {
        vw -= iconToolBar->height();
    }
*/    
    //mView->setMaximumSize( splash->size() );
    //mView->resize( splash->size() );
    //qDebug("MainView x %d y %d w %d  h %d",  mView->x(),mView-> y(), mView->width(), mView->height ());
/*    mView->readSettings(); 
    if( !QFile::exists( defaultFileName() ) ) {
        mView->saveCalendar( defaultFileName() );
    }
    mView->openCalendar( defaultFileName() );
    processIncidenceSelection( 0 );
    connect( mView, SIGNAL( incidenceSelected( Incidence * ) ),
             SLOT( processIncidenceSelection( Incidence * ) ) );
    connect( mView, SIGNAL( modifiedChanged( bool ) ),
             SLOT( slotModifiedChanged( bool ) ) );
    connect( mView, SIGNAL( signalmodified() ),
             SLOT( checkAlarms( ) ) );

    connect( &mSaveTimer, SIGNAL( timeout() ), SLOT( save() ) );
    connect( &alarmTimer, SIGNAL( timeout() ), SLOT( writeAlarm() ) );
    mView->setModified( false );
    mBlockAtStartup = false;
    mView->setModified( false );
    setCentralWidget( mView );
    mView->show();
*/    
    delete splash; 
    
    qApp->processEvents();
//US    qDebug("MainView x %d y %d w %d  h %d",  mView->x(),mView-> y(), mView->width(), mView->height ());

}
MainWindow::~MainWindow()
{
    qDebug("MainWindow::~MainWindow() ");
    kdDebug() << "MainWindow()::~MainWindow()" << endl;
    //save toolbar location 
/*    
    KOPrefs *p = KOPrefs::instance();
    p->mToolBarHor = ( iconToolBar->orientation () == Qt:: Horizontal );
    p->mToolBarUp = iconToolBar->x() > width()/2 ||
        iconToolBar->y() > height()/2;
    alarmTimer.stop();
    mView->writeSettings();
    writeAlarm();
    if ( mCalendarModifiedFlag )
        save();
    delete mCalendar;
*/    
}


void MainWindow::closeEvent( QCloseEvent* ce )
{

    if ( ! KABPrefs::instance()->mAskForQuit ) {

        ce->accept();
        return;

    }

   switch( QMessageBox::information( this, "KA/Pi",
                                      i18n("Do you really want\nto close KA/Pi?"),
                                      i18n("Close!"), i18n("No"), 
				      0, 0 ) ) {
    case 0:
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


void MainWindow::recieve( const QCString& cmsg, const QByteArray& data )
{
/*
    QDataStream stream( data, IO_ReadOnly );
    // QMessageBox::about( this, "About KOrganizer/Pi", "*" +msg +"*" );
   
    
    if ( cmsg == "-writeFile" ) {
        mView->viewManager()->showWhatsNextView();
        save();
        setCaption( i18n("File written on AD request"));
        showMaximized();
        raise();
#ifndef DESKTOP_VERSION
        QCopEnvelope e3("kosaved", "blabla");
#endif
        return;
       
    }
    if ( cmsg == "-newCountdown" ) {
       qDebug("newCountdown ");
       
    }
    QString msg ;;
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
            mView->newEvent();
        }
        if ( msg == "-newTodo" ) {
            mView->newTodo();
            
        } 
        if ( msg == "-showWN" ) {
            mView->viewManager()->showWhatsNextView();
        }
        if ( msg == "-showTodo" ) {
            mView->viewManager()->showTodoView();
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
        else if ( msg == "-showWeek" ) {
            mView->viewManager()->showWeekView();
        }
        else if ( msg == "-showTodo" ) {
            mView->viewManager()->showTodoView();
        }
        else if ( msg == "-showJournal"  ) {
            mView->viewManager()->showJournalView();
        }
        else if ( msg == "-showKO" ) {
            mView->viewManager()->showNextXView();
        }
        else if ( msg == "-showWNext" ) {
            mView->viewManager()->showWhatsNextView();
        }
    }

    showMaximized();
    raise();
*/    
}


QPixmap MainWindow::loadPixmap( QString name )
{
  return KGlobal::iconLoader().loadPixmap( name );
/*US use the advanced version of the iconloader
#ifdef DESKTOP_VERSION
    QPixmap pixmapLoader; 
    QString file;
    file = QDir::homeDirPath()+"/kaddressbook/pics/" + name+".png";
    //qDebug("pixmap name %s ", file.latin1());
    pixmapLoader.load( file );
    return pixmapLoader;
#else
 return Resource::loadPixmap( name ); 
#endif
*/
}

void MainWindow::initActions()
{
    iconToolBar->clear();
    KABPrefs *p = KABPrefs::instance();
    QPEMenuBar *menuBar1 = new QPEMenuBar( iconToolBar );
    QPopupMenu *menuBar = new QPopupMenu( this );
    menuBar1->insertItem( "ME", menuBar);
    QPopupMenu *fileMenu = new QPopupMenu( this );
    QPopupMenu *editMenu = new QPopupMenu( this );
    QPopupMenu *viewMenu = new QPopupMenu( this );
    QPopupMenu *settingsMenu = new QPopupMenu( this );
    QPopupMenu *importMenu = new QPopupMenu( this );

    menuBar1->setMaximumWidth( menuBar1->sizeHint().width() );

    QIconSet icon;


    icon = loadPixmap( pathString + "newtodo" );
    configureToolBarMenu->insertItem(icon, "New todo", 20 );
    QAction* nt_action = new QAction( "New Todo", icon, "New Todo...", 0, this );
    nt_action->addTo( actionMenu );
    connect( nt_action, SIGNAL( activated() ),
             mView, SLOT( newTodo() ) );
    
/*
    QAction *action;
    QIconSet icon;
    // QPopupMenu *configureMenu= new QPopupMenu( menuBar );
    configureToolBarMenu = new QPopupMenu( this );
    configureToolBarMenu->setCheckable( true );
#ifdef DESKTOP_VERSION
    QString pathString = ""; 
#else
    QString pathString = "kaddressbook/"; 
#endif
    if ( QApplication::desktop()->width() < 480 ) 
        pathString += "icons16/";
    configureAgendaMenu = new QPopupMenu( menuBar );
    configureAgendaMenu->setCheckable( true );
    configureAgendaMenu->insertItem("Toggle Allday", 1 ); 
    configureAgendaMenu->insertSeparator(); 
    configureAgendaMenu->insertItem("Tiny", 4 ); 
    configureAgendaMenu->insertItem("Small", 6 );
    configureAgendaMenu->insertItem("Medium", 8 );
    configureAgendaMenu->insertItem("Normal", 10 );  
    configureAgendaMenu->insertItem("Large", 12 );  
    configureAgendaMenu->insertItem("Big", 14 );    
    configureAgendaMenu->insertItem("Bigger", 16 );   
    configureAgendaMenu->insertItem("Biggest", 18 );   
    //configureMenu->insertItem( "AgendaSize",configureAgendaMenu  );  
    icon = loadPixmap( pathString + "newevent" );  
    icon = loadPixmap( pathString + "newevent" ); 
    configureToolBarMenu->insertItem("Stretched TB", 5 ); 
    configureToolBarMenu->insertSeparator(); 
    configureToolBarMenu->insertItem(icon, "New event", 10 );
    QAction* ne_action = new QAction( "New Event", icon, "New Event...", 0, this );
    ne_action->addTo( actionMenu );
*/    
/*
    connect( ne_action, SIGNAL( activated() ),
             mView, SLOT( newEvent() ) );
    icon = loadPixmap( pathString + "newtodo" ); 
    configureToolBarMenu->insertItem(icon, "New todo", 20 ); 
    QAction* nt_action = new QAction( "New Todo", icon, "New Todo...", 0, this );
    nt_action->addTo( actionMenu );
    connect( nt_action, SIGNAL( activated() ),
             mView, SLOT( newTodo() ) );

    action = new QAction( "Toggle FilterView", QPixmap(), "Toggle FilterView", 0, this );
    action->addTo( viewMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( toggleFilter() ) );

    viewMenu->insertSeparator();
    icon = loadPixmap( pathString + "picker" );
    action = new QAction( "Date Picker", icon, "Date Picker", 0, this );
    action->addTo( viewMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( showDatePicker() ) );
    action->addTo( iconToolBar  );
    viewMenu->insertSeparator();
    icon = loadPixmap( pathString + "list" );  
    configureToolBarMenu->insertItem(icon, "Event list", 30 ); 
    QAction* showlist_action = new QAction( "List", icon, "List", 0, this );
    showlist_action->addTo( viewMenu );
    connect( showlist_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showListView() ) );


    icon = loadPixmap( pathString + "day" );  
    configureToolBarMenu->insertItem(icon, "One day", 40 ); 
    QAction* day1_action = new QAction( "Day", icon, "Day", 0, this );
    day1_action->addTo( viewMenu );
    //  action->addTo( toolBar );
    connect( day1_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showDayView() ) );

    icon = loadPixmap( pathString + "workweek" ); 
    configureToolBarMenu->insertItem(icon, "Work week", 50 );  
    QAction* day5_action = new QAction( "Work Week", icon, "Work Week", 0, this );
    day5_action->addTo( viewMenu );
    connect( day5_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showWorkWeekView() ) );

    icon = loadPixmap( pathString + "week" );
    configureToolBarMenu->insertItem(icon, "Week", 60 );    
    QAction* day7_action = new QAction( "Week", icon, "Week", 0, this );
    day7_action->addTo( viewMenu );
    connect( day7_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showWeekView() ) );

    icon = loadPixmap( pathString + "month" ); 
    configureToolBarMenu->insertItem(icon, "Month", 70 );  
    QAction* month_action = new QAction( "Month", icon, "Month", 0, this );
    month_action->addTo( viewMenu );
    connect( month_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showMonthView() ) );

    icon = loadPixmap( pathString + "todo" );  
    configureToolBarMenu->insertItem(icon, "Todo list", 80 );  
    QAction* todoview_action = new QAction( "Todo", icon, "Todo", 0, this );
    todoview_action->addTo( viewMenu );
    connect( todoview_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showTodoView() ) );

    icon = loadPixmap( pathString + "journal" );  
    configureToolBarMenu->insertItem(icon, "Journal", 90 );  
    QAction* viewjournal_action = new QAction( "Journal", icon, "Journal", 0, this );
    viewjournal_action->addTo( viewMenu );
    connect( viewjournal_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showJournalView() ) );

    icon = loadPixmap( pathString + "xdays" );  
    configureToolBarMenu->insertItem(icon, "Next days", 100,4 );  
    QAction* xdays_action = new QAction( "Next days", icon, "Next Days", 0, this );
    xdays_action->addTo( viewMenu );
    connect( xdays_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showNextXView() ) );

    icon = loadPixmap( pathString + "whatsnext" );  
    configureToolBarMenu->insertItem(icon, "Whats next", 110, 4 );  
    QAction* whatsnext_action = new QAction( "What's Next", icon, "What's Next", 0, this ); 
    whatsnext_action->addTo( viewMenu );
    connect( whatsnext_action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showWhatsNextView() ) );

#if 0
    action = new QAction( "view_timespan", "Time Span", 0, this );
    action->addTo( viewMenu );
    connect( action, SIGNAL( activated() ),
             mView->viewManager(), SLOT( showTimeSpanView() ) );
#endif

    menuBar->insertItem( "View", viewMenu );


#if 0
    QPopupMenu *navigationMenu = new QPopupMenu( menuBar );
  
    action = new QAction( "Go backward", "Go Backward", 0, navigationMenu );
    action->addTo( navigationMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( goPrevious() ) );

    action = new QAction( "Go forward", "Go Forward", 0, navigationMenu );
    action->addTo( navigationMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( goNext() ) );

    menuBar->insertItem( "Go", navigationMenu );
#endif


  

 

    mNewSubTodoAction = new QAction( "new_subtodo", "New Sub-Todo...", 0,
                                     this );
    mNewSubTodoAction->addTo( actionMenu );
    connect( mNewSubTodoAction, SIGNAL( activated() ),
             mView, SLOT( newSubTodo() ) );

    actionMenu->insertSeparator();

    mShowAction = new QAction( "show_incidence", "Show...", 0, this );
    mShowAction->addTo( actionMenu );
    connect( mShowAction, SIGNAL( activated() ),
             mView, SLOT( showIncidence() ) );

    mEditAction = new QAction( "edit_incidence", "Edit...", 0, this );
    mEditAction->addTo( actionMenu );
    connect( mEditAction, SIGNAL( activated() ),
             mView, SLOT( editIncidence() ) );

    mDeleteAction = new QAction( "delete_incidence", "Delete...", 0, this );
    mDeleteAction->addTo( actionMenu );
    connect( mDeleteAction, SIGNAL( activated() ),
             mView, SLOT( deleteIncidence() ) );

    actionMenu->insertSeparator();

    action = new QAction( "purge_completed", i18n("Purge Completed"), 0,
                          this );
    action->addTo( actionMenu );
    connect( action, SIGNAL( activated() ), mView, SLOT( purgeCompleted() ) );

    icon = loadPixmap( pathString + "search" );  
    QAction* search_action = new QAction( "search", icon, "Search...", 0, this );
    configureToolBarMenu->insertItem(icon, "Search", 120 , 4);  
    search_action->addTo( actionMenu );
    connect( search_action, SIGNAL( activated() ),
             mView->dialogManager(), SLOT( showSearchDialog() ) );

    icon = loadPixmap( pathString + "today" );  
    configureToolBarMenu->insertItem(icon, "Today", 130);  
    QAction* today_action = new QAction( "Today", icon, "Go to Today", 0, this );
    today_action->addTo( actionMenu );
    connect( today_action, SIGNAL( activated() ),
             mView, SLOT( goToday() ) );

    //#if 0
    actionMenu->insertSeparator();

    action = new QAction( "configure", "Configure...", 0, this );
    action->addTo( actionMenu );
    connect( action, SIGNAL( activated() ),
             mView, SLOT( edit_options() ) );
    //#endif

    // actionMenu->insertSeparator();
    action = new QAction( "import_qtopia", "Import (*.ics/*.vcs) file", 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), SLOT( importIcal() ) );
    action = new QAction( "import_quick", "Import last file", 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), SLOT( quickImportIcal() ) );
    importMenu->insertSeparator();
    action = new QAction( "import_qtopia", "Import Opie/Qtopia Cal.", 0,
                          this );
    action->addTo( importMenu );
    importMenu->insertSeparator();
    connect( action, SIGNAL( activated() ), SLOT( importQtopia() ) );

    action = new QAction( "load_cal", "Load Calendar Backup", 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), SLOT( loadCalendar() ) );

    action = new QAction( "save_cal", "Save Calendar Backup", 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), SLOT( saveCalendar() ) );

    importMenu->insertSeparator();
    action = new QAction( "import_qtopia", "Export VCalendar", 0,
                          this );
    action->addTo( importMenu );
    connect( action, SIGNAL( activated() ), SLOT( exportVCalendar() ) );
  

    QPopupMenu *syncMenu = new QPopupMenu( this );
    action = new QAction( "syncssh", "Remote via ssh/scp(not implemented)", 0,
                          this );
    action->addTo( syncMenu );
    connect( action, SIGNAL( activated() ), SLOT( syncSSH() ) );
    action = new QAction( "synclocal", "With local file(not implemented)", 0,
                          this );
    action->addTo( syncMenu );
    connect( action, SIGNAL( activated() ), SLOT( syncLocalFile() ) );
    action = new QAction( "quicksynclocal", "With last file(not implemented)", 0,
                          this );
    action->addTo( syncMenu );
    connect( action, SIGNAL( activated() ), SLOT( quickSyncLocalFile() ) );

    menuBar->insertItem( "Actions", actionMenu );
    menuBar->insertItem( "Load/Save", importMenu );
    menuBar->insertItem( "Synchronize", syncMenu );
    //menuBar->insertItem( "Configure",configureMenu );
    //configureMenu->insertItem( "Toolbar",configureToolBarMenu  ); 
    menuBar->insertItem( "AgendaSize",configureAgendaMenu  ); 
    menuBar->insertItem( "Toolbar",configureToolBarMenu ); 
    QPopupMenu *helpMenu = new QPopupMenu( menuBar );
    icon = loadPixmap( "korganizer/korganizer" ); 
    action = new QAction( "Key bindings", "Key Bindings...", 0, this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( keyBindings() ) ); 
    action = new QAction( "Auto saving", "Auto saving...", 0, this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( aboutAutoSaving() ) ); 
    action = new QAction( "Problemd", "Known Problems...", 0,this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( aboutKnownBugs() ) ); 
    action = new QAction( "about", "About...", 0, this );
    action->addTo( helpMenu );
    connect( action, SIGNAL( activated() ),
             SLOT( about() ) );
    menuBar->insertItem( "Help", helpMenu );  
*/
    //menuBar->insertSeparator();

    // ******************************************************
    // menubar icons 
    
    
    iconToolBar->setHorizontalStretchable (true ); 
    //menuBar->insertItem( iconToolBar );
    //xdays_action
/*
    if (p-> mShowIconNewEvent)
        ne_action->addTo( iconToolBar  );
    if (p->mShowIconNewTodo )
        nt_action->addTo( iconToolBar  );
    if (p-> mShowIconSearch)
        search_action->addTo( iconToolBar  );
    if (p-> mShowIconNext)
        whatsnext_action->addTo( iconToolBar  );
    if (p-> mShowIconNextDays)
        xdays_action->addTo( iconToolBar  );
    if (p-> mShowIconList)
        showlist_action->addTo( iconToolBar  );
    if (p-> mShowIconDay1)
        day1_action->addTo( iconToolBar  );
    if (p-> mShowIconDay5)
        day5_action->addTo( iconToolBar  );
    if (p-> mShowIconDay7)
        day7_action->addTo( iconToolBar  );
    if (p-> mShowIconMonth)
        month_action->addTo( iconToolBar  );
    if (p-> mShowIconTodoview)
        todoview_action->addTo( iconToolBar  );
    if (p-> mShowIconJournal)
        viewjournal_action->addTo( iconToolBar  );
    icon = loadPixmap( pathString + "2leftarrowB" );
    configureToolBarMenu->insertItem(icon, "Prev. month", 200, 14);  
    if (p-> mShowIconBackFast) {
        action = new QAction( "Prev month", icon, "Go Backward",0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goPreviousMonth() ) );
        action->addTo( iconToolBar  );
    }
    icon = loadPixmap( pathString + "1leftarrowB" );
    configureToolBarMenu->insertItem(icon, "Go previous", 210,15);  
    if (p-> mShowIconBack) {
        action = new QAction( "Go previous", icon, "Go Backward",0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goPrevious() ) );
        action->addTo( iconToolBar  );
    }
    if (p-> mShowIconToday) 
        today_action->addTo( iconToolBar  );
    icon = loadPixmap( pathString + "1rightarrowB" );
    configureToolBarMenu->insertItem(icon, "Go next", 220); 
    if (p-> mShowIconForward) {
        action = new QAction( "Go next", icon, "Go Backward",0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goNext() ) );
        action->addTo( iconToolBar  );
    }
    icon = loadPixmap( pathString + "2rightarrowB" );
    configureToolBarMenu->insertItem(icon, "Next month", 230);  
    if (p-> mShowIconForwardFast) {
        action = new QAction( "Next month", icon, "Go Backward",0 , this );
        connect( action, SIGNAL( activated() ),
                 mView, SLOT( goNextMonth() ) );
        action->addTo( iconToolBar  );
    }
*/  
   
    configureToolBarMenu->insertItem("WhatsThis", 300); 
/*
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

    QLabel* dummy = new QLabel( iconToolBar );
    dummy->setBackgroundColor( iconToolBar->backgroundColor() );
    if (!p-> mShowIconStretch)
        iconToolBar->setStretchableWidget ( dummy ) ;
    else
        configureToolBarMenu->setItemChecked( 5, true );
    if (p-> mShowIconWhatsThis)
        QWhatsThis::whatsThisButton (  iconToolBar );
    connect( configureToolBarMenu, SIGNAL( activated( int ) ),this,  SLOT(configureToolBar( int ) ) ); 
    configureAgenda( p->mHourSize );
    connect( configureAgendaMenu, SIGNAL( activated( int ) ),this,  SLOT(configureAgenda( int ) ) );
*/    
}
/*
void MainWindow::setDefaultPreferences()
{
    KOPrefs *p = KOPrefs::instance();

    p->mCompactDialogs = true;
    p->mConfirm = true;
    p->mEnableQuickTodo = false;
}
*/
void MainWindow::about()
{
    QMessageBox::about( this, "About KOrganizer/Pi",
                        "KOrganizer/Platform-independent\n"
                        "(KO/Pi) 1.6.0e - "
#ifdef DESKTOP_VERSION
                        "Desktop Edition\n"
#else
                        "PDA-Edition\n"
                        "for: Zaurus 5500 / 7x0 / 8x0\n"
#endif
                        "(c) 2004 Lutz Rogowski\n"
                        "Email:lutz@pi-sync.net\n"
                        "KO/Pi is based on KOrganizer\n"
                        "(c) 2002,2003 Cornelius Schumacher\n"
                        "Email: schumacher@kde.org\n"
                        "KOrganizer/Pi is licensed\n"
                        "under the GPL.\n"
                        "KO/Pi can be compiled for\n"
                        "Linux, Zaurus-PDA and Windows\n"
                        "www.korganizer.org\n" );
}
/*
void MainWindow::keyBindings()
{
    QMessageBox* msg;
    msg = new QMessageBox( "Key bindings  KOrganizer/Pi",
                           "Space: Toggle Fullscreen | P: Date Picker\n"
                           "H: This dialog | I: Show info | S: Search\n"
                           "F: Toggle Filterview | 1-4: Select Filter\n"
                           "N: Show next days | W: Whats next\n "
                           "V: Todo view | L: Event list view\n"
                           "T: Goto today | T+<ctrl>: New Todo\n"
                           "E: Edit item | E+<ctrl>: New Event\n"
                           "5: Work week view | 7: Week view\n"
                           "D: One day view | M: Month view\n"
                           "+,- : Zoom in/out Agenda | A: Toggle Allday\n"
                           "<ctrl>+<up>/<down>: Scroll todo view\n"
                           "<right>,C: Next week | <right>+<ctrl>: Next month\n"
                           "<left>,X: Prev. week | <left>+<ctrl>: Prev. month\n"
                           "<del>,<backspace>: Delete sel. item\n",
                           QMessageBox::NoIcon,
                           QMessageBox::Ok,
                           QMessageBox::NoButton,
                           QMessageBox::NoButton );
    msg->exec();
    delete msg;
  
}
void MainWindow::aboutAutoSaving()
{
    QMessageBox* msg;
    msg = new QMessageBox(  "Auto Saving in  KOrganizer/Pi",
                            "After changing something, the data is\n"
                            "automatically saved to the file\n"
                            "~/Applications/korganizer/mycalendar.ics\n "
                            "after (configureable) one minute.\n"
                            "For safety reasons there is  one autosaving\n"
                            "after 10 minutes (of idle time) again. The \n"
                            "data is saved automatically when closing KO/Pi\n"
                            "You can create a backup file \n"
                            "with: Load/Save - Save Calendar Backup\n",
                            QMessageBox::NoIcon,
                            QMessageBox::Ok,
                            QMessageBox::NoButton,
                            QMessageBox::NoButton);
    msg->exec();
    delete msg;
  
                          
}
void MainWindow::aboutKnownBugs()
{
    QMessageBox* msg;
    msg = new QMessageBox(  "Known Problems in  KOrganizer/Pi",
                            "1) The QWhatsThis help is not working.\n"
                            "There is only a black rectangle displayed\n"
                            "when clicking on an event.\n "
                            "2) Audio alarm is available!\n"
                            "as an additional small application\n"
                            "3) Syncing is missing.\n"
                            "Syncing via ssh/scp will be available in\n"
                            "KOrganizer/Pi 2.0\n"
                            "\n"
                            "Please report unexpected behaviour to\n"
                            "lutz@pi-sync.net\n",
                            QMessageBox::NoIcon,
                            QMessageBox::Ok,
                            QMessageBox::NoButton,
                            QMessageBox::NoButton);
    msg->exec();
    delete msg;
  
}

QString MainWindow::defaultFileName()
{
#ifndef DESKTOP_VERSION
    return Global::applicationFileName( "korganizer", "mycalendar.ics" );
#else
    // pending
    QString file;
    file = QDir::homeDirPath()+"/korganizer/mycalendar.ics";
    return QDir::convertSeparators( file );
#endif
}

void MainWindow::processIncidenceSelection( Incidence *incidence )
{
    if ( !incidence ) {
        enableIncidenceActions( false );
  
        mNewSubTodoAction->setEnabled( false );  
        setCaptionToDates();
        return;
        
  }
 
  QString startString = "";
  if ( incidence->type() != "Todo" ) { 
      if ( incidence->dtStart().date() < incidence->dtEnd().date()   ) { 
          startString = ": "+incidence->dtStart().time().toString().left(5);
          startString += " "+ incidence->dtStart().date().toString();
          startString += "-"+incidence->dtEnd().time().toString().left(5);
          startString += " "+ incidence->dtEnd().date().toString();
      
      } else {
          if ( incidence->dtStart().time() != incidence->dtEnd().time() )
              startString = ": "+incidence->dtStart().time().toString().left(5)+"-"+incidence->dtEnd().time().toString().left(5);
          startString +=" "+incidence->dtStart().date().toString(); 
      }
       
  }
   else
       startString = ": (Prio "+QString::number( (( KCal::Todo*)incidence)->priority() ) +") "+QString::number( (( KCal::Todo*)incidence)->percentComplete() ) +"\% completed";

   setCaption( incidence->summary()+startString);
   
   enableIncidenceActions( true );
  
  if ( incidence->type() == "Event" ) {
    mShowAction->setText( i18n("Show Event...") );
    mEditAction->setText( i18n("Edit Event...") );
    mDeleteAction->setText( i18n("Delete Event...") );

    mNewSubTodoAction->setEnabled( false );
  } else if ( incidence->type() == "Todo" ) {
    mShowAction->setText( i18n("Show Todo...") );
    mEditAction->setText( i18n("Edit Todo...") );
    mDeleteAction->setText( i18n("Delete Todo...") );

    mNewSubTodoAction->setEnabled( true );
  } else {
    mShowAction->setText( i18n("Show...") );
    mShowAction->setText( i18n("Edit...") );
    mShowAction->setText( i18n("Delete...") );

    mNewSubTodoAction->setEnabled( false );
  }
}

void MainWindow::enableIncidenceActions( bool enabled )
{
  mShowAction->setEnabled( enabled );
  mEditAction->setEnabled( enabled );
  mDeleteAction->setEnabled( enabled );
}

void MainWindow::importQtopia()
{
#ifndef DESKTOP_VERSION
    int result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                       "When importing a calendar twice\n"
                                       "duplicated events will be ignored!\n"
                                       "You can create a backup file with\n"
                                       "Load/Save - Save Calendar Backup\n"
                                       "to revert importing",
                                       "Import!", "Cancel", 0,
                                       0, 1 );
    if ( result == 0 ) {
        QString datebook = Global::applicationFileName( "datebook", "datebook.xml");
        QString todolist = Global::applicationFileName( "todolist", "todolist.xml");
        QString categories = QString( getenv( "HOME" ) ) + "/Settings/Categories.xml";
        mView->importQtopia( categories, datebook, todolist );
    }
#else
 int result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                       "NOT SUPPORTED \n"
                                       "ON DESKTOP!\n",
                                       "OK", "Cancel", 0,
                                       0, 1 );

#endif 
}
void MainWindow::checkAlarms()
{ 
    if ( mBlockAtStartup )
        return;
    alarmTimer.start( 1000 * 3);
  
}
void MainWindow::slotModifiedChanged( bool changed )
{
    if ( mBlockAtStartup )
        return;
    int msec;
  // we store the changes after 1 minute, 
  // and for safety reasons after 10 minutes again
  if ( !mBlockSaveFlag )
      msec = (1000 * 60*KOPrefs::instance()->mAutoSaveInterval) +1000;
  else
      msec = 1000 * 600;
  mSaveTimer.start( msec, true ); // 1 minute
  qDebug("KO: Saving File in %d secs!", msec/1000);
  mCalendarModifiedFlag = true;
}
void MainWindow::writeAlarm()
{
    if ( mBlockAtStartup )
        return;
    //QCopEnvelope e3("koalarm", "-showWN");
    QDateTime nextA ;
    QDateTime reference ( QDate( 2000, 1, 1 ) ); 
    QString summary; 
    alarmTimer.stop();
    Calendar* cal =  mView->calendar();
    if ( ! cal ){
        qDebug("KO: Calendar undefined. No alarm notification");
        return;
    }
    nextA = cal->nextAlarm( 720 );
    summary = cal->nextSummary();
    qDebug("KO: Next Alarm %s %s", nextA.toString().latin1(), summary.latin1());
    if (  nextA < QDateTime::currentDateTime().addDays(  720-1 ) ) {
        KConfig *config = KOGlobals::config();
        config->setGroup("NextSystemAlarm");
        QString date, sum;
        date = config->readEntry("TimeDateinSecs", "xxx");
        bool ok = false;
        int shsecs = date.toInt(&ok );
        QDateTime scheduledAlarm;
        QDateTime eventDateTime =  cal->nextAlarmEvent();
        sum = config->readEntry("Summary", "sss");
        if ( ok ) {
            scheduledAlarm = reference.addSecs( shsecs );
            //qDebug("sheduledAlarm %s ", scheduledAlarm.toString().latin1());
            if ( scheduledAlarm == nextA && sum == summary ) {
                qDebug("KO: Same alarm time+summary - nothing to do! ");
                return;
            }
        }
        if ( ok ) {
            // delete alarm from system
            //qDebug("delete Alarm %s ", scheduledAlarm.toString().latin1());
#ifndef DESKTOP_VERSION
            AlarmServer::deleteAlarm ( scheduledAlarm,"koalarm"  , sum.latin1() );
#endif          
        }
        // add new alarm to system
        qDebug("KO: Add Alarm: %s ", nextA.toString().latin1() );
#ifndef DESKTOP_VERSION
        AlarmServer::addAlarm ( nextA,"koalarm", summary.latin1() );
#endif  
        // write new alarm to config file
        //qDebug("Entry %s %s  ", date.latin1(), sum.latin1());
        int secs = reference.secsTo( nextA );
        //qDebug(" secs %d ", secs);
        setCaption( i18n("Next Alarm: ")+ nextA.toString() +"-"+summary );
        config->writeEntry("TimeDateinSecs",QString::number ( secs ));
        config->writeEntry("Summary",summary);
        config->writeEntry("TimeDate",nextA.toString());
        config->writeEntry("WriteDateTime",QDateTime::currentDateTime().toString()); 
        config->writeEntry("EventStartDateTime",eventDateTime.toString());
        config->writeEntry("SystemNotifyInSecs",QString::number (QDateTime::currentDateTime().secsTo( nextA  ) ));
        config->sync();
        
    }
}
void MainWindow::save()
{
    if ( mBlockSaveFlag ) 
        return;
 
  mBlockSaveFlag = true;
  QTime neededSaveTime = QDateTime::currentDateTime().time();
  setCaption("KO/Pi:Saving Data to File ..." );
  qDebug("KO: Start saving data to file!");
  mView->saveCalendar( defaultFileName() );
  int msNeeded = neededSaveTime.msecsTo( QDateTime::currentDateTime().time() );
  qDebug("KO: Needed %d ms for saving.",msNeeded );
  QString savemes;
  savemes.sprintf("KO/Pi:File Saved. Needed %d sec, %d ms",(msNeeded/1000)%100,msNeeded%1000  );
  setCaption(savemes); 
  mCalendarModifiedFlag = false;
  mBlockSaveFlag = false;
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
    //qDebug("MainWindow::keyPressEvent ");
    switch ( e->key() ) {
    case Qt::Key_Right:
        if ( e->state() == Qt::ControlButton )
            mView->goNextMonth();
        else
            mView->goNext();
        showSelectedDates = true;
        break;
    case Qt::Key_Left:
        if ( e->state() == Qt::ControlButton )
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
    case Qt::Key_0:
        mView->toggleFilerEnabled( );
        break;
    case Qt::Key_1:
        mView->selectFilter( 0 );
        break;
    case Qt::Key_2:
        mView->selectFilter( 1 );
        break;
    case Qt::Key_3:
        mView->selectFilter( 2 );
        break;
    case Qt::Key_4:
        mView->selectFilter( 3 );
        break;
    case Qt::Key_5:
        mView->viewManager()->showWorkWeekView();
        showSelectedDates = true;
        break;
    case Qt::Key_7:
        mView->viewManager()->showWeekView();
        showSelectedDates = true;
        break;
    case Qt::Key_M:
        mView->viewManager()->showMonthView();
        showSelectedDates = true;
        break;
    case Qt::Key_Insert:
        mView->newEvent();
        break;
    case Qt::Key_S :
            mView->dialogManager()->showSearchDialog();
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
        mView->viewManager()->showNextXView();
        showSelectedDates = true;
        break; 
    case Qt::Key_V:
        mView->viewManager()->showTodoView();
        break; 
    case Qt::Key_C:
        mView->goNext();
        showSelectedDates = true;
        break;       
    case Qt::Key_P:
        mView->showDatePicker( );
        break;      
    case Qt::Key_F:
        mView->toggleFilter();
        break;          
    case Qt::Key_X:
        mView->goPrevious();
        showSelectedDates = true;
        break;      
    case Qt::Key_Space:
        mView->toggleExpand();
        break;     
    case Qt::Key_A:
        mView->toggleAllDaySize();
        break; 
    case Qt::Key_T:
        if ( e->state() == Qt::ControlButton )
            mView->newTodo();
        else {
            mView->goToday();
            showSelectedDates = true;
        }
        break;
    case Qt::Key_J:
        mView->viewManager()->showJournalView();
        break;
    case Qt::Key_Return:  
    case Qt::Key_E:  
        if ( e->state() == Qt::ControlButton )
            mView->newEvent();
        else
            mView->editIncidence();
        break;
    case Qt::Key_Plus:
        size = p->mHourSize +2;
        if ( size <= 18 )
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
    
    if ( showSelectedDates ) {
        ;//  setCaptionToDates();
    }

}

void MainWindow::configureToolBar( int item  )
{

    configureToolBarMenu->setItemChecked( item, !configureToolBarMenu-> isItemChecked ( item ) );
    KOPrefs *p = KOPrefs::instance();
    p-> mShowIconStretch= configureToolBarMenu->isItemChecked( 5 );
    p-> mShowIconNewEvent= configureToolBarMenu->isItemChecked( 10 );
    p->mShowIconNewTodo = configureToolBarMenu->isItemChecked( 20 );
    p-> mShowIconSearch= configureToolBarMenu->isItemChecked( 120 );
    p-> mShowIconList= configureToolBarMenu->isItemChecked( 30 );
    p-> mShowIconDay1= configureToolBarMenu->isItemChecked( 40 );
    p-> mShowIconDay5= configureToolBarMenu->isItemChecked( 50 );
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
    initActions();
}

void MainWindow::setCaptionToDates()
{
    QString selDates;
    selDates = mView->startDate().toString();
    if  (mView->startDate() < mView->endDate() )
        selDates += " - " + mView->endDate().toString();
    setCaption( "Dates: " + selDates );
    
}
// parameter item == 0: reinit
void MainWindow::configureAgenda( int item )
{  

    KOPrefs *p = KOPrefs::instance();

    int i;
    if ( item == 1 ) {
        mView->toggleAllDaySize();
        return;
    }
    // do not allow 4 for widgets higher than 480
    if ( QApplication::desktop()->height() > 480 ) {
        if ( item == 4 )
            item = 6;
    }
    for ( i = 4; i <= 18; i= i+2 )
        configureAgendaMenu->setItemChecked( i, false );
    configureAgendaMenu->setItemChecked( item, true );
    if ( p->mHourSize == item )
        return;
    p->mHourSize=item;
    mView->viewManager()->agendaView()->updateConfig();
}

void  MainWindow::saveCalendar()
{ 
    QString fn =  KOPrefs::instance()->mLastSaveFile;
    fn =  KFileDialog::getSaveFileName( fn, "Save backup filename", this );

    if ( fn == "" )
        return;
   QFileInfo info;
   info.setFile( fn );
   QString mes;
   bool createbup = true;
   if ( info. exists() ) { 
       mes.sprintf(  "Backup file\nalready exists!\nOld backup file from:\n%s\nOverwrite?\n",info.lastModified ().toString().latin1()  );
       int result = QMessageBox::warning( this, "KO/Pi: Warning!",mes,
                                          "Overwrite!", "Cancel", 0,
                                          0, 1 );
       if ( result != 0 ) {
           createbup = false;
       }
   }
   if ( createbup ) {
       mView->saveCalendar( fn ); 
       mes.sprintf("KO/Pi:Saved %s",fn.latin1()  );
       KOPrefs::instance()->mLastSaveFile = fn;
       setCaption(mes);
   }
}
void  MainWindow::loadCalendar()
{
   
    QString fn =   KOPrefs::instance()->mLastLoadFile;
    fn =  KFileDialog::getOpenFileName( fn, "Load backup filename", this );
    
    if ( fn == "" )
        return;
    QFileInfo info;
    info.setFile( fn );
    QString mess;
    bool loadbup = true;
    if ( info. exists() ) {
       mess.sprintf(  "Backup file from:\n%s\nLoading  backup\nfile will delete\nyour current Data!\n",info.lastModified ().toString().latin1()  );
       int result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                          mess,
                                          "Load!", "Cancel", 0,
                                          0, 1 );
       if ( result != 0 ) {
           loadbup = false;
       }
    } else {
        QMessageBox::warning( this, "KO/Pi: Warning!",
                                          "Backup file\ndoes not exist!\n"
                                          "Nothing loaded!", 0, 0,
                                          0, 1 );

        return;
    }
   if ( loadbup ) {
       mView->openCalendar( fn );
       KOPrefs::instance()->mLastLoadFile = fn;
       mess.sprintf("KO/Pi:Loaded %s",fn.latin1()  );
       setCaption(mess);
   }

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
    bool loadbup = true;
    if ( !info. exists() ) {
        mess=  "Import file \n..."+fn.right( 30)+ "\ndoes not exist!\nNothing imported!\n";
       int result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                          mess );
       return;
    } 
    int result = 0;
    if ( !quick ) { 
    mess.sprintf(  "Import file \n..."+fn.right( 25)+"\nfrom:\n%s\nDuplicated entries\nwill not be imported!\n",info.lastModified ().toString().latin1()  );
     result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                       mess,
                                       "Import", "Cancel", 0,
                                       0, 1 );
    }
    if ( result == 0 ) {
        if ( mView->openCalendar( fn, true )) {
            KOPrefs::instance()->mLastImportFile = fn;
            setCaption(i18n("   "));
            setCaption(i18n("Imported file successfully"));
        } else {
            setCaption(i18n("Error importing file"));
        }
    }
}
void   MainWindow::importIcal()
{
 
    QString fn =KOPrefs::instance()->mLastImportFile;

    fn =KFileDialog:: getOpenFileName( fn, "Import filename(*.ics/*.vcs)", this );
    if ( fn == "" )
        return;
    importFile(  fn, true );
        
}

void MainWindow::exportVCalendar()
{
    QString fn =  KOPrefs::instance()->mLastVcalFile;
    fn =  KFileDialog::getSaveFileName( fn, "Export vcal filename(*.vcs)", this );
    if ( fn == "" )
        return;
   QFileInfo info;
   info.setFile( fn );
   QString mes;
   bool createbup = true;
   if ( info. exists() ) { 
       mes.sprintf(  i18n("Save file\nalready exists!\nOld save file from:\n%s\nOverwrite?\n"),info.lastModified ().toString().latin1()  );
       int result = QMessageBox::warning( this, "KO/Pi: Warning!",mes,
                                          "Overwrite!", "Cancel", 0,
                                          0, 1 );
       if ( result != 0 ) {
           createbup = false;
       }
   }
   if ( createbup ) {
       if ( mView->exportVCalendar( fn ) ) { 
       KOPrefs::instance()->mLastVcalFile = fn; 
       if ( fn.length() > 20 )
           mes.sprintf(i18n("KO/Pi:Exported to ...%s"),(fn.right(20)).latin1()  );
       else
           mes.sprintf(i18n("KO/Pi:Exported to %s"),fn.latin1()  );
       setCaption(mes);
       }
   }

}

QString  MainWindow::getPassword( )
{ 
    QString retfile = "";
    QDialog dia ( this, "input-dialog", true ); 
    QLineEdit lab ( &dia ); 
    lab.setEchoMode( QLineEdit::Password );
    QVBoxLayout lay( &dia );
    lay.setMargin(7); 
    lay.setSpacing(7); 
    lay.addWidget( &lab);
    dia.setFixedSize( 230,50 );
    dia.setCaption( i18n("Input password") );
    dia.show();
    int res = dia.exec();
    if ( res )
        retfile = lab.text();
    return retfile;

}
#ifndef _WIN32_
#include <unistd.h>
#endif
void MainWindow::syncLocalFile()
{

    QString fn =KOPrefs::instance()->mLastSyncedLocalFile;

    fn =KFileDialog:: getOpenFileName( fn, i18n("Sync filename(*.ics/*.vcs)"), this );
    if ( fn == "" )
        return;
    syncWithFile(  fn, false );

}

void  MainWindow::syncWithFile( QString fn , bool quick )
{
    QFileInfo info;
    info.setFile( fn );
    QString mess;
    bool loadbup = true;
    if ( !info. exists() ) {
        mess.sprintf( i18n( "Sync file \n...%s\ndoes not exist!\nNothing synced!\n"),fn.right( 30).latin1() );
       int result = QMessageBox::warning( this, "KO/Pi: Warning!",
                                          mess );
       return;
    } 
    int result = 0;
    if ( !quick ) { 
    mess.sprintf(  "Sync with file \n..."+fn.right( 25)+"\nfrom:\n%s\n",info.lastModified ().toString().latin1()  );
     result = QMessageBox::warning( this, i18n("KO/Pi: Warning!"),
                                       mess,
                                       i18n("Sync"), i18n("Cancel"), 0,
                                       0, 1 );
    }
    if ( KOPrefs::instance()->mAskForPreferences )
        mView->edit_sync_options(); 
    if ( result == 0 ) {
        qDebug("Now sycing ... ");
        mView->syncCalendar( fn, true );
        if ( ! quick )
            KOPrefs::instance()->mLastSyncedLocalFile = fn;
    }

}

void MainWindow::quickSyncLocalFile()
{
    qDebug("quickSyncLocalFile() ");
    syncWithFile( KOPrefs::instance()->mLastSyncedLocalFile, false );
}

void MainWindow::syncSSH()
{   
    QTime timer; 
    timer.start();
    qDebug("MainWindow::syncssh() ");
    KOPrefs *p = KOPrefs::instance();
    QString localFile =  p->mLocalTempFile;
    QString remoteIP = p->mRemoteIP;
    QString remoteUser = p->mRemoteUser;
    QString remoteFile = p->mRemoteFile;
    if ( p->mUsePassWd &&  p->mRemotePassWd.length() > 0  )
        remoteUser += ":" + p->mRemotePassWd;

    QString question = i18n("Do you really want\nto remote sync?\n \n") +
        i18n("IP: " ) +remoteIP  +"\n" +
        i18n("User: " ) + remoteUser +"\n" ;
    int maxlen = 30;
    if ( QApplication::desktop()->width() > 320 ) 
        maxlen += 25;
    if ( remoteFile.length() > maxlen )
        question  +=   i18n("Remote file:\n..." ) + remoteFile.right(maxlen) +"\n";
    else
        question  +=   i18n("Remote file:\n " ) + remoteFile +"\n";
    if ( localFile.length() > maxlen )
        question  +=   i18n("Local temp file:\n..." ) + localFile.right(maxlen) +"\n";
    else
        question  +=   i18n("Local temp file:\n " ) + localFile +"\n";
        
    if ( QMessageBox::information( this, "KO/Pi Sync",
                                   question,
                                   i18n("Yes"), i18n("No"), 
                                   0, 0 ) != 0 )
        return;
    if ( !p->mUsePassWd ) {
        QString pass = getPassword();
        if ( pass.length() > 0 )
            remoteUser += ":" + pass;
    }
    QString command = "scp " + remoteUser + "@" + remoteIP +":" + remoteFile +"  " +localFile;
    setCaption ( i18n( "Copy remote file to local machine..." ) );
    int fileSize = 0;
    int result = system ( command );
    // 0 : okay
    // 256: no such file or dir
    //
    qDebug("KO: Remote copy result(0 = okay): %d ",result );
    if ( result != 0 ) {
        int len = maxlen;
        while ( len <  command.length() ) {
            command.insert( len , "\n" );
            len += maxlen +2;
        }
        question = i18n("Sorry, the copy command failed!\nCommand was:\n") + command + "\n \nTry command on console to get more\ndetailed info about the reason.\n";
        QMessageBox::information( this, "KO/Pi Sync - ERROR",
                                  question,
                                  i18n("Okay!")) ;
        return;
    }


    setCaption ( i18n( "Copying succeed. Syncing not yet implemented" ) );
    syncWithFile(localFile , true );
    return;
#if 0
    system ("scp zaurus@192.168.0.65:/home/zaurus/Applications/korganizer/mycalendar.ics /home/polo/Applications/korganizer/z_sync.ics");
      while ( timer.elapsed() < 5000 )
          qApp->processEvents();

    qDebug("MainWindow::merging) ");
    mView->syncCalendar( "/home/polo/Applications/korganizer/z_sync.ics", 0 );
    while ( mBlockSaveFlag ) 
        qApp->processEvents();
    save();
     system ("scp /home/polo/Applications/korganizer/mycalendar.ics zaurus@192.168.0.65:/home/zaurus/Applications/korganizer/mycalendar.ics");
#endif

}
*/

