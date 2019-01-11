
// CHANGED 2004-08-06 Lutz Rogowski
#include <qlabel.h>
#include <qvbox.h>
#include <qheader.h>
#include <qtimer.h>
#include <qlayout.h>
//#include <kdialog.h>
#include <kiconloader.h>
#include <kapplication.h>

#ifdef DESKTOP_VERSION
#include <qapplication.h>
#include <qstatusbar.h> 
#include <kabc/stdaddressbook.h>
extern QStatusBar* globalSstatusBarMainWindow;
#else
#include <qpe/qpeapplication.h>
#include <klocale.h>
#endif
#include "defines.h"
#include "koprefs.h"
#include "mainwindow.h"
#include "mailistviewitem.h"
#include <KDGanttMinimizeSplitter.h>
#include <libkdepim/kpimglobalprefs.h>

#include "koprefs.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags flags )
    : QMainWindow( parent, name ) //, flags )
{
#ifdef DESKTOP_VERSION
    globalSstatusBarMainWindow = statusBar();
#endif
    setCaption( i18n( "KOpieMail/Pi" ) );
    setToolBarsMovable( false ); 
    //KABC::StdAddressBook::self();
    toolBar = new QToolBar( this );
    menuBar = new QPEMenuBar( toolBar );
    mailMenu = new QPopupMenu( menuBar );
    menuBar->insertItem( i18n( "Mail" ), mailMenu );
    settingsMenu = new QPopupMenu( menuBar );
    menuBar->insertItem( i18n( "Settings" ), settingsMenu );

    addToolBar( toolBar );
    toolBar->setHorizontalStretchable( true );
    QAction* getMail = new QAction( i18n( "Get all new mails" ), SmallIcon("enter"),
                               0, 0, this );
    connect(getMail, SIGNAL( activated() ),
            SLOT( slotGetAllMail() ) );
    getMail->addTo( mailMenu );

    getMail = new QAction( i18n( "Get new messages" ), SmallIcon("add"),
                               0, 0, this );
    getMail->addTo( toolBar );
    getMail->addTo( mailMenu );
    connect(getMail, SIGNAL( activated() ),
            SLOT( slotGetMail() ) );

    composeMail = new QAction( i18n( "Compose new mail" ), SmallIcon("composemail"),
                               0, 0, this );
    composeMail->addTo( toolBar );
    composeMail->addTo( mailMenu );

    sendQueued = new QAction( i18n( "Send queued mails" ), SmallIcon("sendqueued") ,
                              0, 0, this );
    sendQueued->addTo( toolBar );
    sendQueued->addTo( mailMenu );

    /*
    syncFolders = new QAction( i18n( "Sync mailfolders" ), ICON_SYNC,
                               0, 0, this );
    syncFolders->addTo( toolBar );
    syncFolders->addTo( mailMenu );
    */

    showFolders = new QAction( i18n( "Show/Hide folders" ), SmallIcon("showfolders") ,
                               0, 0, this, 0, true );
    showFolders->addTo( toolBar );
    showFolders->addTo( mailMenu );
    showFolders->setOn( true );
    connect(showFolders, SIGNAL( toggled(bool) ),
            SLOT( slotShowFolders(bool) ) );

    /*
      searchMails = new QAction( i18n( "Search mails" ), SmallIcon("find") ),
                               0, 0, this );
    searchMails->kopddTo( toolBar );
    searchMails->addTo( mailMenu );
    */

    deleteMails = new QAction(i18n("Delete Mail"),  SmallIcon("trash"), 0, 0, this);
    deleteMails->addTo( toolBar );
    deleteMails->addTo( mailMenu );
    connect( deleteMails, SIGNAL( activated() ),
             SLOT( slotDeleteAllMail() ) );

    editSettings = new QAction( i18n( "Configure OM/Pi..." ), SmallIcon("SettingsIcon")  ,
                                0, 0, this );
    editSettings->addTo( settingsMenu );
    connect( editSettings, SIGNAL( activated() ),
             SLOT( slotEditSettings() ) );


    QAction * editSettings2 = new QAction( i18n( "Global Settings..." ), SmallIcon("SettingsIcon")  ,
                                0, 0, this );
    editSettings2->addTo( settingsMenu );
    connect( editSettings2, SIGNAL( activated() ),
             SLOT( slotEditGlobalSettings() ) );



    editAccounts = new QAction( i18n( "Configure accounts" ),  SmallIcon("editaccounts")  ,
                                0, 0, this );
    editAccounts->addTo( settingsMenu );
    codecMenu = new QPopupMenu( menuBar );
    codecMenu->insertItem( "Western (iso-8859-1)",0,0);
    codecMenu->insertItem( "Cyrillic (iso-8859-5)",1,1);
    codecMenu->insertItem( "Western (iso-8859-15)",2,2);
    codecMenu->insertItem( "Chinese (big-5)",3,3);
    codecMenu->insertItem( "Unicode (utf-8)",4,4);
    codecMenu->insertItem( "Userdefined ("+KOPrefs::instance()->mSendCodec+")",5,5);
    //disabled
    //settingsMenu->insertItem( i18n("Codec for new mails"), codecMenu);
    //setCentralWidget( view );

    QVBox* wrapperBox = new QVBox( this );
    setCentralWidget( wrapperBox );

    // QWidget *view = new QWidget( wrapperBox );
    KDGanttMinimizeSplitter* splithor = new KDGanttMinimizeSplitter( Qt::Vertical, wrapperBox);
    splithor->setMinimizeDirection( KDGanttMinimizeSplitter::Down);
    KDGanttMinimizeSplitter* split = new KDGanttMinimizeSplitter( Qt::Horizontal, splithor);
    split->setMinimizeDirection( KDGanttMinimizeSplitter::Left);
    //layout = new QBoxLayout ( split, QBoxLayout::LeftToRight );
    subLE = 0;
    fromLE = 0;
    toLE = 0;
    if ( KOPrefs::instance()->mShowInfoSub ||  KOPrefs::instance()->mShowInfoFrom ||  KOPrefs::instance()->mShowInfoTo ) {
        QWidget* infoBox = new QWidget( splithor );
        QGridLayout *griLay = new QGridLayout( infoBox, 2,2);
        if ( KOPrefs::instance()->mShowInfoSub ) {
            griLay->addWidget( new QLabel ( i18n("Su:"), infoBox ),0,0 );
            griLay->addWidget( subLE = new QLineEdit( infoBox ),0,1) ;
        }
        if (  KOPrefs::instance()->mShowInfoFrom ) {
            griLay->addWidget( new QLabel ( i18n("Fr:"), infoBox ),1,0 );
            griLay->addWidget( fromLE = new QLineEdit( infoBox ),1,1) ;
        } 
        if ( KOPrefs::instance()->mShowInfoTo ) {
            griLay->addWidget( new QLabel ( i18n("To:"), infoBox ),2,0 );
            griLay->addWidget( toLE = new QLineEdit( infoBox ),2,1) ;
        } 
        infoBox->setMaximumHeight( infoBox->sizeHint().height() );
        if ( !KOPrefs::instance()->mShowInfoStart ) {
            QTimer::singleShot( 1,splithor, SLOT  ( toggle() ) );
        }
    }


    folderView = new AccountView( split );
    folderView->header()->hide();
    folderView->setRootIsDecorated( false );
    folderView->addColumn( i18n( "Mailbox" ) );

    //layout->addWidget( folderView );

    mailView = new KListView( split );
    mailView->addColumn( i18n( " " ) );
    mailView->addColumn( i18n( "Subject" ),QListView::Manual );
    mailView->addColumn( i18n( "Sender" ),QListView::Manual );
    mailView->addColumn( i18n( "Size" ),QListView::Manual);
    mailView->addColumn( i18n( "Date" ),QListView::Manual);
    if ( KOPrefs::instance()->mShowToField )
        mailView->addColumn( i18n( "To" ),QListView::Manual);
    mailView->setAllColumnsShowFocus(true);
    //mailView->setSorting(-1);
    mailView->setRootIsDecorated( false );
    statusWidget = new StatusWidget( wrapperBox );
    statusWidget->hide();
    mailView->setSelectionMode( QListView::Multi );   
    mailView->setMultiSelection( true);
    mailView->setAlternateBackground(KPimGlobalPrefs::instance()->mAlternateColor );
    //layout->addWidget( mailView );
    //layout->setStretchFactor( folderView, 1 );
    //layout->setStretchFactor( mailView, 2 );

    slotAdjustLayout();
#ifndef DESKTOP_VERSION
    QPEApplication::setStylusOperation( mailView->viewport(),QPEApplication::RightOnHold);
    QPEApplication::setStylusOperation( folderView->viewport(),QPEApplication::RightOnHold);
    if ( subLE ) 
        QPEApplication::setStylusOperation( subLE ,QPEApplication::RightOnHold);
    if ( fromLE ) 
        QPEApplication::setStylusOperation( fromLE ,QPEApplication::RightOnHold);
    if ( toLE ) 
        QPEApplication::setStylusOperation( toLE ,QPEApplication::RightOnHold);
#endif
    connect( mailView, SIGNAL(  doubleClicked (QListViewItem* )),this,
             SLOT( mailLeftClicked(QListViewItem*) ) );
    connect( mailView, SIGNAL(  returnPressed (QListViewItem* )),this,
             SLOT( mailLeftClicked(QListViewItem*) ) );
    connect( mailView, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int) ),this,
             SLOT( mailHold(int,QListViewItem*,const QPoint&,int) ) );
    connect(folderView, SIGNAL(refreshMailview(const QValueList<RecMailP>&)),
            this,SLOT(refreshMailView(const QValueList<RecMailP>&)));

    connect( mailView, SIGNAL(  currentChanged  (QListViewItem* )),this,
             SLOT( setInfoFields(QListViewItem*) ) );

    connect( composeMail, SIGNAL( activated() ), SLOT( slotComposeMail() ) );
    connect( sendQueued, SIGNAL( activated() ), SLOT( slotSendQueued() ) );
//    connect( searchMails, SIGNAL( activated() ), SLOT( slotSearchMails() ) );
    connect( editAccounts, SIGNAL( activated() ), SLOT( slotEditAccounts() ) );
    //mailView->setMultiSelection ( true );
    //mailView->setSelectionMode( QListView::Extended );
    QValueList<int> list;
    int fw = 100;
    if ( QApplication::desktop()->width() > 320 )
        fw = 50;
    list.append( fw );
    list.append( 100 );
    split->setSizes( list );
    QTimer::singleShot( 100, this, SLOT( slotAdjustColumns() ) );
    mailView->setShowSortIndicator ( true ); 
    QLabel *spacer = new QLabel( toolBar );
    spacer->setBackgroundMode( QWidget::PaletteButton );
    toolBar->setStretchableWidget( spacer );

    QAction* closeMail = new QAction(i18n("Close"),SmallIcon("exit"), 0, 0, this); 
    connect( closeMail, SIGNAL( activated() ), SLOT( close() ) );
    if ( QApplication::desktop()->width() > 320 )
        closeMail->addTo(toolBar);
	closeMail->addTo(mailMenu);


   QPopupMenu* helpMenu = new QPopupMenu( menuBar );
   menuBar->insertItem( i18n( "Help" ), helpMenu ); 
   QAction* li = new QAction(i18n("About"), QPixmap(), 0, 0, this); 
   connect( li, SIGNAL( activated() ), SLOT( showAbout()) );
   li->addTo(helpMenu);
   li = new QAction(i18n("Licence"),QPixmap(), 0, 0, this); 
   connect( li, SIGNAL( activated() ), SLOT( showLicence()) );
   li->addTo(helpMenu); 
   li = new QAction(i18n("LibEtPan Licence"), QPixmap(), 0, 0, this); 
   connect( li, SIGNAL( activated() ), SLOT( showEtpanLicence()) );
   li->addTo(helpMenu);
   connect( codecMenu, SIGNAL( activated(int) ), this, SLOT( slotSetCodec( int )) );
   slotSetCodec( KOPrefs::instance()->mCurrentCodec );
   menuBar->setMaximumWidth( menuBar->sizeHint().width());
   //menuBar->setMaximumSize( menuBar->sizeHint());
#ifdef DESKTOP_VERSION
   resize ( 640, 480 );
#endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::setInfoFields(QListViewItem* item )
{
    if ( item == 0) {
    if ( subLE ) subLE->setText("");
    if ( fromLE ) fromLE->setText("");
    if ( toLE ) toLE->setText("");
        return;
    }
    RecMailP mail = ((MailListViewItem*)item)->data();
    if ( subLE ) subLE->setText(mail->getSubject());
    if ( fromLE ) fromLE->setText(mail->getFrom());
    if ( toLE ) toLE->setText(mail->To().join(";" ));
    if ( subLE ) subLE->setCursorPosition(0);
    if ( fromLE ) fromLE->setCursorPosition(0);
    if ( toLE ) toLE->setCursorPosition(0);

}
void MainWindow::slotSetCodec( int codec )
{ 
    codecMenu->setItemChecked(KOPrefs::instance()->mCurrentCodec, false  );
    //qDebug("codec %d ", codec);
    KOPrefs::instance()->mCurrentCodec = codec;
    KOPrefs::instance()->isDirty = true;
    QString name;
    switch ( codec ) {
    case 0:
        name = "iso-8859-1";
        break;
    case 1:
        name = "iso-8859-5";
        break;
    case 2:
        name = "iso-8859-15";
        break;
    case 3:
        name = "big-5";
        break;
    case 4:
        name = "utf-8";
        break;
    case 5:
        name = KOPrefs::instance()->mSendCodec.lower();
        break;
    }
    KOPrefs::instance()->mCurrentCodeName = name ;
    codecMenu->changeItem ( 5, "Userdefined ("+KOPrefs::instance()->mSendCodec+")");
    codecMenu->setItemChecked(KOPrefs::instance()->mCurrentCodec, true );
}
void  MainWindow::showLicence()
{
    KApplication::showLicence();
}
void  MainWindow::showAbout()
{
    QString version;
#include <../version>

    QString cap = "About KOpieMail/Pi";
	QString text =	i18n("KOpieMail/Platform-independent\n") +
                        "(OM/Pi) " + version + " - "
                      
#ifdef DESKTOP_VERSION
                        "Desktop Edition\n"
#else
                       "PDA-Edition\nfor: Zaurus 5x00 / 7x0 / 8x0\n"
#endif
		"www.rogowski.info\n\n"



"Copyright (c) 2004 Lutz Rogowski <zautrix@rogowski.info>\n"
		"KOpieMail/Pi is based on Opie Mail\n"
		"Copyright (c) Rajko Albrecht and the Opie team\n"
		"KOpieMail/Pi is licensed under the GPL\n"
		"\n"
		"KOpieMail/Pi uses LibEtPan - a mail stuff library\n"
        "Copyright (C) 2001, 2002 - DINH Viet Hoa\n"
		"libEtPan has its own licence - see LibEtPan licence\n";

    KApplication::showText( cap, text );
}
void  MainWindow::showEtpanLicence()
{
    KApplication::showFile( "LibEtPan licence", "kdepim/kopiemail/COPYRIGHTlibetpan" );

} 
void MainWindow::appMessage(const QCString &, const QByteArray &)
{
    qDebug("appMessage implemented by subclass");
}

void MainWindow::slotAdjustLayout() {

    /*
  QWidget *d = QApplication::desktop();

  if ( d->width() < d->height() ) {
    layout->setDirection( QBoxLayout::TopToBottom );
    } else {
    layout->setDirection( QBoxLayout::LeftToRight );
  }
    */
}

void MainWindow::slotAdjustColumns()
{
   
    if ( !folderView->isHidden() )
        folderView->setColumnWidth( 0, folderView->visibleWidth() );

    mailView->setColumnWidth( 0, 10 );
    mailView->setColumnWidth( 1, 100 );
    mailView->setColumnWidth( 2, 100 );
    mailView->setColumnWidth( 3, 70 );
    mailView->setColumnWidth( 4, 180 );
    if ( KOPrefs::instance()->mShowToField )
        mailView->setColumnWidth( 5, 100 );
    mailView->setColumnAlignment( 3, AlignRight);
}
void MainWindow::slotAdjustColumnsWide()
{
    if ( !folderView->isHidden() )
        folderView->setColumnWidth( 0, folderView->visibleWidth() );

    mailView->setColumnWidth( 0, 10 );
    mailView->setColumnWidth( 1, 200 );
    mailView->setColumnWidth( 2, 200 );
    mailView->setColumnWidth( 3, 70 );
    mailView->setColumnWidth( 4, 180 );
    if ( KOPrefs::instance()->mShowToField )
        mailView->setColumnWidth( 5, 100 );
    mailView->setColumnAlignment( 3, AlignRight);
}

void MainWindow::slotEditSettings()
{
}
void MainWindow::slotEditGlobalSettings()
{
}

void MainWindow::slotShowFolders( bool )
{
    qDebug("not implemented:  ");
}

void MainWindow::refreshMailView(const QValueList<RecMailP>&)
{ 
    qDebug("not implemented:  ");
}

void MainWindow::mailLeftClicked(QListViewItem * )
{
    qDebug("not implemented:  ");
}

void MainWindow::displayMail()
{
    qDebug("not implemented:  ");
}

void MainWindow::slotDeleteMail()
{
    qDebug("not implemented:  ");
}

void MainWindow::mailHold(int, QListViewItem *,const QPoint&,int  )
{
    qDebug("not implemented:  ");
}

void MainWindow::slotSendQueued()
{
    qDebug("not implemented:  ");
}

void MainWindow::slotEditAccounts()
{
    qDebug("not implemented:  ");
}

void MainWindow::slotComposeMail()
{
    qDebug("not implemented:  ");
}
