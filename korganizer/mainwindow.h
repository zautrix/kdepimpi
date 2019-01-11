#ifndef KORGE_MAINWINDOW_H
#define KORGE_MAINWINDOW_H

#include <qmainwindow.h>
#include <qtimer.h>
#include <qdict.h>
#include <qfile.h>
#include <qmenubar.h>
#include <qtextstream.h>
#include <qregexp.h>

#include <libkcal/incidence.h>
#include <ksyncmanager.h>
#include <kpopupmenu.h>
#ifndef DESKTOP_VERSION
#include <qcopchannel_qws.h> 
#endif
class QAction;
class CalendarView;
class KSyncProfile;
#ifdef DESKTOP_VERSION

#define QPEToolBar QToolBar
#endif
class QPEToolBar;


namespace KCal {
class CalendarLocal;
}

using namespace KCal;

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow( QWidget *parent = 0, const char *name = 0 );
    ~MainWindow();
    bool beamReceiveEnabled();
    static  QString defaultFileName();
    static  QString syncFileName();
    static QString resourcePath();
 public  slots:  
    void setUsesBigPixmaps ( bool );
    void setCaption ( const QString & );
    void updateWeekNum(const KCal::DateList &);
    void updateWeek(QDate);
    void updateFilterToolbar();
    virtual void showMaximized (); 
     void configureAgenda( int );
    void recieve( const QCString& msg, const QByteArray& data );
    void receiveStart( const QCString& msg, const QByteArray& data );
 protected slots: 
    void autoSaveWarning();
    void loadDataAfterStart();
    void calHint(); 
    void startMultiSync();
    void setCaptionToDates();
    void weekAction();
    void about(); 
    void licence(); 
    void faq(); 
    void usertrans(); 
    void features(); 
    void synchowto();   
    void storagehowto();   
    void timetrackinghowto();   
    void kdesynchowto();   
    void multisynchowto();   
    void whatsNew();    
    void keyBindings();
    void aboutAutoSaving();;
    void aboutKnownBugs();

    void processIncidenceSelection( Incidence * );

    void importQtopia();
    void importBday();
    void importOL();
    void importIcal();
    void importFile( QString, bool );
    void quickImportIcal();
    
    void slotModifiedChanged( bool );
    
    void save();
    void backupAllFiles();
    void saveStopTimer();
    void configureToolBar( int );
    void printSel();
    void printCal();
    void printListView();
    void saveCalendar();
    void loadCalendar();
    void exportVCalendar();
    void exportICalendar();
    void exportCalendar( bool );
    void fillFilterMenu();
    void fillFilterMenuTB();
    void selectFilter( int );
    void fillFilterMenuPopup();
    void selectFilterPopup( int );
    void exportToPhone( int );
    void toggleBeamReceive();
    void disableBR(bool);
 signals:
    void selectWeek ( int ); 
  private slots:
    void slotResetFocus();
    void slotResetFocusLoop();
    void showConfigureAgenda();
    void getFile( bool ,const QString &);
    void syncFileRequest(const QString &);
    void slot_setdaysinWNview( int );
    
  protected:
    int mFocusLoop;
    void hideEvent ( QHideEvent * );
    QString sentSyncFile();
    void displayText( QString, QString);
    void enableIncidenceActions( bool );
    bool askForQuitOnSaveError();
    
  private:
    bool mAutoSaveDisabled;
    bool checkAutosave();
    QCString mCStringMess;
    QByteArray mByteData;

    //void setMenuBar( QMenuBar * );
    bool mBRdisabled;
#ifndef DESKTOP_VERSION
    QCopChannel* infrared;
#endif
    QAction* brAction; 
    KSyncManager* mSyncManager;
    bool mClosed;
    void saveOnClose();
    bool  mFlagKeyPressed;
    bool  mBlockAtStartup;
    KMenuBar *menuBar1;
    QPEToolBar *iconToolBar;
    QPEToolBar *viewToolBar;
    QPEToolBar *navigatorToolBar;
    QPEToolBar *filterToolBar;
    KMenuBar *filterMenubar;
    QPopupMenu * filterPopupMenu;
    QPopupMenu * mCurrentItemMenu;
    void initActions();
    void setDefaultPreferences(); 
    void resizeEvent( QResizeEvent* e);
    void keyPressEvent ( QKeyEvent * ) ;
    void keyReleaseEvent ( QKeyEvent * ) ;
    QPopupMenu *configureToolBarMenu;
    QPopupMenu *selectFilterMenu;
    QPopupMenu *selectFilterMenuTB;
    QPopupMenu *configureAgendaMenu, *syncMenu;
    CalendarLocal *mCalendar;
    CalendarView *mView;
    QAction *mNewSubTodoAction;
    QAction *mWeekAction;
    QFont mWeekFont;
    QPixmap mWeekPixmap;
    QColor mWeekBgColor;
    
    QAction *mShowAction;
    QAction *mEditAction;
    QAction *mDeleteAction;
    QAction *mCloneAction;
    QAction *mMoveAction;
    QAction *mBeamAction;
    QAction *mCancelAction;
    QAction *mPrintSelAction;

    QAction *mToggleNav;
    QAction *mToggleFilter;
    QAction *mToggleAllday;
    QAction *actionFilterMenuTB;

    void closeEvent( QCloseEvent* ce );
    QTimer mSaveTimer;
    QDateTime mSaveTimerStart;
    int mSaveDelay;
    //bool mBlockSaveFlag;
    bool mCalendarModifiedFlag;
    QPixmap loadPixmap( QString );
    QPixmap listviewPix, listviewPix0, listviewPix20, listviewPix40, listviewPix60, listviewPix80, journalPix;
};


#endif
