#ifndef KADR_MAINWINDOW_H
#define KADR_MAINWINDOW_H

#include <qmainwindow.h>
#include "kabcore.h"

//US#include <qtimer.h>

//US#include <libkcal/incidence.h>

//US#include "simplealarmclient.h"

//USclass QAction;
//USclass CalendarView;

//US#ifdef DESKTOP_VERSION
//US#define QPEToolBar QToolBar
//US#define QPEMenuBar QMenuBar
//US#endif

class KABCore;

//USclass QPEToolBar;

//US namespace KACore {
//USclass CalendarLocal;
//US}

//USusing namespace KACore;

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow( QWidget *parent = 0, const char *name = 0, QString command = "");
    ~MainWindow();
    
 public  slots:  
//    void configureAgenda( int );
 void recieve( const QCString& msg, const QByteArray& data );

  protected slots:
/*  
    void setCaptionToDates();
    
    void about();
    
    void keyBindings();
    void aboutAutoSaving();;
    void aboutKnownBugs();

    void processIncidenceSelection( Incidence * );

    void importQtopia();
    void importIcal();
    void importFile( QString, bool );
    void quickImportIcal();
    
    void slotModifiedChanged( bool );
    
    void save();
    void configureToolBar( int );
    void saveCalendar();
    void loadCalendar();
    void exportVCalendar();
    void checkAlarms();
    void writeAlarm();


    void syncSSH();
    void syncLocalFile();
    void syncWithFile( QString, bool );
    void quickSyncLocalFile();
    
    
  protected:
    QString defaultFileName();
    
    void enableIncidenceActions( bool );
*/
    
  private:
/*  
    bool  mFlagKeyPressed;
    bool  mBlockAtStartup;
    
    QPEToolBar *iconToolBar;
    
    void initActions();
*/    
/*    void setDefaultPreferences(); 
    void keyPressEvent ( QKeyEvent * ) ;
    void keyReleaseEvent ( QKeyEvent * ) ;
    
    QPopupMenu *configureToolBarMenu;
    QPopupMenu *configureAgendaMenu;
    CalendarLocal *mCalendar;
    CalendarView *mView;
    QString  getPassword();
    QAction *mNewSubTodoAction;
    
    QAction *mShowAction;
    QAction *mEditAction;
    QAction *mDeleteAction;
    
    void closeEvent( QCloseEvent* ce );
    
    SimpleAlarmClient mAlarmClient;

    QTimer mSaveTimer;
    QTimer alarmTimer;
    bool mBlockSaveFlag;
    bool mCalendarModifiedFlag;
    
    QPixmap loadPixmap( QString );
*/
    private:
      KABCore *mCore;
        
};

#endif
