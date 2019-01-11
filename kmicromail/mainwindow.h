
// CHANGED 2004-08-06 Lutz Rogowski
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <klistview.h>
#include <qaction.h>
#include <qlineedit.h>

#include <qtoolbar.h>
#ifdef DESKTOP_VERSION
#include <qmenubar.h>
#define QPEMenuBar QMenuBar
#else
#include <qpe/qpemenubar.h>
#endif

#include "accountview.h"
#include "statuswidget.h"

#include <libmailwrapper/mailtypes.h>
#include <opie2/osmartpointer.h>

class RecMail;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0, WFlags flags = 0 );
    virtual ~MainWindow();

public slots:
    virtual void slotAdjustColumns();
    virtual void slotAdjustColumnsWide();
    virtual void appMessage(const QCString &msg, const QByteArray &data);
    virtual void slotComposeMail();

protected slots:
    virtual void setInfoFields(QListViewItem* );
    virtual void slotSendQueued();
    virtual void slotEditAccounts();
    virtual void slotShowFolders( bool show );
    virtual void refreshMailView(const QValueList<RecMailP>&);
    virtual void displayMail();
    virtual void slotGetMail() = 0;
    virtual void slotGetAllMail() = 0;
    virtual void slotDeleteMail();
    virtual void slotDeleteAllMail() = 0;
    virtual void slotSetCodec(int);
    virtual void mailHold(int, QListViewItem *,const QPoint&,int);
    virtual void slotAdjustLayout();
    virtual void slotEditSettings();
    virtual void slotEditGlobalSettings();
    virtual void mailLeftClicked( QListViewItem * );
    void showLicence();
    void showAbout();
    void showEtpanLicence(); 

protected:
    QToolBar *toolBar;
    StatusWidget *statusWidget;
    QPEMenuBar *menuBar;
    QPopupMenu *mailMenu, *settingsMenu, *codecMenu;
    QAction *composeMail, *sendQueued, *showFolders, *searchMails, *deleteMails,
            *editSettings, *editAccounts, *syncFolders;
    AccountView *folderView;
    KListView *mailView;
    QLineEdit* toLE,*fromLE,*subLE;
    //QBoxLayout *layout;
};

#endif
