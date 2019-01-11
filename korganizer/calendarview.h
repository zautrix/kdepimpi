/*
    This file is part of KOrganizer.
    Copyright (c) 2000, 2001
    Cornelius Schumacher <schumacher@kde.org>

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

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/
#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <qframe.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qptrlist.h>
#include <qvbox.h>
#include <qmap.h>
#include <qscrollbar.h>
#ifndef DESKTOP_VERSION
#include <qtopia/ir.h> 
#else
#define Ir char
#endif
#include <libkcal/calendar.h>
#include <libkcal/scheduler.h>
#include <libkcal/calendarresources.h>
#include <libkcal/resourcecalendar.h>
#include <KDGanttMinimizeSplitter.h>

#include <korganizer/calendarviewbase.h>

#include <ksyncmanager.h>
//#include <koprefs.h>

class QWidgetStack;
class QSplitter;
class KopiCalendarFile;
class CalPrinter;
class KOFilterView;
class KOCalEditView;
class KOViewManager;
class KODialogManager;
class KOTodoView;
class KDateNavigator;
class DateNavigatorContainer;
class DateNavigator;
class KOIncidenceEditor;
class KDatePicker;
class ResourceView;
class KOEventEditor;
class KOTodoEditor ;
class KOEventViewerDialog;
class KOBeamPrefs;
class KSyncProfile;
class AlarmDialog;
class KCal::Attendee;

namespace KCal { class FileStorage; }

using namespace KCal;

/**
  This is the main calendar widget. It provides the different vies on t he
  calendar data as well as the date navigator. It also handles synchronisation
  of the different views and controls the different dialogs like preferences,
  event editor, search dialog etc.
  
  @short main calendar view widget
  @author Cornelius Schumacher
*/

#include <qtextbrowser.h>
#include <qtextcodec.h>

class MissedAlarmTextBrowser : public QTextBrowser {
    Q_OBJECT
  public:
    MissedAlarmTextBrowser(QWidget *parent, QPtrList<Incidence> alarms ,QDateTime start);
    ~MissedAlarmTextBrowser();
    void setSource(const QString & n);
    
 private:
    Incidence * getNextInc(QDateTime start );
    QPtrList<Incidence> mAlarms;
  signals:
    void showIncidence( QString uid);
};


class CalendarView : public KOrg::CalendarViewBase, public KCal::Calendar::Observer, public KSyncInterface
{
    Q_OBJECT
  public:
    /**
      Constructs a new calendar view widget.

      @param calendar calendar document
      @param parent   parent window
      @param name     Qt internal widget object name
    */
    CalendarView( CalendarResources *calendar, QWidget *parent = 0,
                  const char *name = 0 );
    CalendarView( Calendar *calendar, QWidget *parent = 0,
                  const char *name = 0 );
    virtual ~CalendarView();
  
    Calendar *calendar() { return mCalendar; }

    KOViewManager *viewManager();
    KODialogManager *dialogManager();

    QDate startDate();
    QDate endDate();

    QWidgetStack *viewStack();
    QWidget *leftFrame();

    DateNavigator *dateNavigator();
    KDateNavigator *dateNavigatorWidget();

    void addView(KOrg::BaseView *);
    void showView(KOrg::BaseView *);
    KOEventViewerDialog* getEventViewerDialog();
    Incidence *currentSelection();
    void checkSuspendAlarm();
    void mergeFile( QString fn );
    void mergeFileResource( QString fn  ,QString res);

  signals:
    void save ();
    void saveStopTimer ();
    void tempDisableBR(bool);
    /** This todo has been modified */
    void todoModified(Todo *, int);
  
    /** when change is made to options dialog, the topwidget will catch this
     *  and emit this signal which notifies all widgets which have registered 
     *  for notification to update their settings. */
    void configChanged();
    /** emitted when the topwidget is closing down, so that any attached
        child windows can also close. */
    void closingDown();
    /** emitted right before we die */
    void closed(QWidget *);
    
    /** Emitted when state of modified flag changes */
    void modifiedChanged(bool);
    void signalmodified();
  
    /** Emitted when state of read-only flag changes */
    void readOnlyChanged(bool);
  
    /** Emitted when the unit of navigation changes */
    void changeNavStringPrev(const QString &);
    void changeNavStringNext(const QString &);
  
    /** Emitted when state of events selection has changed and user is organizer*/
    void organizerEventsSelected(bool);
    /** Emitted when state of events selection has changed and user is attendee*/
    void groupEventsSelected(bool);
    /**
      Emitted when an incidence gets selected. If the selection is cleared the
      signal is emitted with 0 as argument.
    */
    void incidenceSelected( Incidence * );
    /** Emitted, when a todoitem is selected or deselected. */
    void todoSelected( bool );

    /**
      Emitted, when clipboard content changes. Parameter indicates if paste
      is possible or not.
    */
    void pasteEnabled(bool);
    
    /** Emitted, when the number of incoming messages has changed. */
    void numIncomingChanged(int);

    /** Emitted, when the number of outgoing messages has changed. */
    void numOutgoingChanged(int);

    /** Send status message, which can e.g. be displayed in the status bar. */
    void statusMessage(const QString &);
    
    void calendarViewExpanded( bool );
    void updateSearchDialog();
    void filtersUpdated();


  public slots: 
    void multiResourceSyncStart( bool );
    void displayCalendarInfo( int id );
    void nextConflict( bool all, bool allday );
    void conflictAll();
    void conflictAllday();
    void conflictNotAll();
    void setCalReadOnly( int id, bool readO );
    void checkAlarms();
    void checkFiles();
    void slotprintSelInc();
    void showNextAlarms();
    void showOpenError();
    void watchSavedFile();
    void recheckTimerAlarm();
    void checkNextTimerAlarm();
    void addAlarm(const QDateTime &qdt, const QString &noti  );
    void addSuspendAlarm(const QDateTime &qdt, const QString &noti  );
    void removeAlarm(const QDateTime &qdt, const QString &noti  );

    /** options dialog made a changed to the configuration. we catch this
     *  and notify all widgets which need to update their configuration. */
    void updateConfig();

    void insertBirthdays(const QString& uid, const QStringList& birthdayList, 
			 const QStringList& anniversaryList, const QStringList& realNameList,
			 const QStringList& emailList, const QStringList& assembledNameList,
			 const QStringList& uidList);

    /**
      Load calendar from file \a filename. If \a merge is true, load 
      calendar into existing one, if it is false, clear calendar, before
      loading. Return true, if calendar could be successfully loaded.
    */
    bool openCalendar(QString filename, bool merge=false);
    bool loadCalendars();
    bool saveCalendars();
    bool restoreCalendarSettings();
    bool addCalendar( KopiCalendarFile * );
    void addCalendarId( int id );
    bool syncCalendar(QString filename,int mode = 0 );

    /**
      Save calendar data to file. Return true if calendar could be
      successfully saved.
    */
    bool saveCalendar(QString filename);
    bool saveCalendarResource(QString filename, QString resource);
  
    /**
      Close calendar. Clear calendar data and reset views to display an empty
      calendar.
    */
    void closeCalendar();
  
    /** Archive old events of calendar */
    void archiveCalendar();

    void showIncidence();
    void editIncidence();
    void editIncidenceDescription();
    void deleteIncidence();
    void cloneIncidence();
    void moveIncidence();
    void beamIncidence();
    void toggleCancelIncidence();
  
    /** create an editeventwin with supplied date/time, and if bool is true,
     * make the event take all day. */
    void newEvent(QDateTime, QDateTime, bool allDay );
    void newEvent(QDateTime, QDateTime);
    void newEvent(QDateTime fh);
    void newEvent(QDate dt);
    /** create new event without having a date hint. Takes current date as
     default hint. */ 
    void newEvent();
    void newFloatingEvent();

    /** Create a read-only viewer dialog for the supplied incidence. It calls the correct showXXX method*/
    void showIncidence(Incidence *);
    void showIncidence(QString uid);
    /** Create an editor for the supplied incidence. It calls the correct editXXX method*/
    void editIncidence(Incidence *);
    /** Delete the supplied incidence. It calls the correct deleteXXX method*/
    void deleteIncidence(Incidence *);
    void cloneIncidence(Incidence *);
    void cancelIncidence(Incidence *);
    /** Create an editor for the supplied event. */
    void editEvent(Event *);
    /** Delete the supplied event. */
    void deleteEvent(Event *);
    /** Delete the event with the given unique ID. Returns false, if event wasn't
    found. */
    bool deleteEvent(const QString &uid);
    /** Create a read-only viewer dialog for the supplied event. */
    void showEvent(Event *);

    void editJournal(Journal *);
    void showJournal(Journal *);
    void deleteJournal(Journal *);
    /** Create an editor dialog for a todo */
    void editTodo(Todo *);
    /** Create a read-only viewer dialog for the supplied todo */
    void showTodo(Todo *);
    /** create new todo */
    void newTodo();
    void newTodoDateTime(QDateTime, bool allday); 
    /** create new todo with a parent todo */
    void newSubTodo();
    /** create new todo with a parent todo */
    void newSubTodo(Todo *);
    /** Delete todo */
    void deleteTodo(Todo *);

            
    /** Check if clipboard contains vCalendar event. The signal pasteEnabled() is
     * emitted as result. */
    void checkClipboard();
    
    /** using the KConfig associated with the kapp variable, read in the
     * settings from the config file. 
     */
    void readSettings();
    
    /** write current state to config file. */
    void writeSettings();

    /** read settings for calendar filters */
    void readFilterSettings(KConfig *config);
    
    /** write settings for calendar filters */
    void writeFilterSettings(KConfig *config);

    /** passes on the message that an event has changed to the currently
     * activated view so that it can make appropriate display changes. */
    void changeEventDisplay(Event *, int);
    void changeIncidenceDisplay(Incidence *, int);
    void changeTodoDisplay(Todo *, int);
  
    void eventAdded(Event *);
    void eventChanged(Event *);
    void eventToBeDeleted(Event *);
    void eventDeleted();
  
    void todoAdded(Todo *);
    void todoChanged(Todo *);
    void todoToBeDeleted(Todo *);
    void todoDeleted();

    void updateView(const QDate &start, const QDate &end);
    void updateView();
    void clearAllViews();
  
    /** Full update of visible todo views */
    void updateTodoViews();

    void updateUnmanagedViews();
  
    /** cut the current appointment to the clipboard */
    void edit_cut();
  
    /** copy the current appointment(s) to the clipboard */
    void edit_copy();

    /** paste the current vobject(s) in the clipboard buffer into calendar */
    void edit_paste();
  
    /** edit viewing and configuration options. */
    void edit_options();
    void edit_global_options();
    /**
      Functions for printing, previewing a print, and setting up printing
      parameters.
    */
    void print();
    void printSetup();
    void printPreview();

    /** Export as iCalendar file */
    bool exportICalendar();
    bool exportICalendar( QString fn );
  
    /** Export as vCalendar file */
    bool exportVCalendar( QString fn);
  
    /** pop up a dialog to show an existing appointment. */
    void appointment_show();
    /**
     * pop up an Appointment Dialog to edit an existing appointment.	Get
     * information on the appointment from the list of unique IDs that is
     * currently in the View, called currIds.
     */
    void appointment_edit();
    /**
     * pop up dialog confirming deletion of currently selected event in the
     * View.
     */
    void appointment_delete();

    /** mails the currently selected event to a particular user as a vCalendar
      attachment. */
    void action_mail();

    /* frees a subtodo from it's relation */
    void todo_unsub( Todo * );
    void todo_resub( Todo * parent, Todo * sub );

    /** Take ownership of selected event. */
    void takeOverEvent();

    /** Take ownership of all events in calendar. */
    void takeOverCalendar();
  
    /** query whether or not the calendar is "dirty". */
    bool isModified();
    /** set the state of calendar. Modified means "dirty", i.e. needing a save. */
    void setModified(bool modified=true);

    /** query if the calendar is read-only. */
    bool isReadOnly();
    /** set state of calendar to read-only */
    void setReadOnly(bool readOnly=true);

    void eventUpdated(Incidence *);

    /* iTIP scheduling actions */
    void schedule_publish(Incidence *incidence = 0);
    void schedule_request(Incidence *incidence = 0);
    void schedule_refresh(Incidence *incidence = 0);
    void schedule_cancel(Incidence *incidence = 0);
    void schedule_add(Incidence *incidence = 0);
    void schedule_reply(Incidence *incidence = 0);
    void schedule_counter(Incidence *incidence = 0);
    void schedule_declinecounter(Incidence *incidence = 0);
    void schedule_publish_freebusy(int daysToPublish = 30);

    void openAddressbook();

    void editFilters();
    void toggleFilerEnabled();
    QPtrList<CalFilter> filters();
    void toggleFilter();
    void showFilter(bool visible);
    void updateFilter();
    void filterEdited();
    void selectFilter( int );
    KOFilterView *filterView();

    void showIntro();

    /** Move the curdatepient view date to today */
    void goToday();

    /** Move to the next date(s) in the current view */
    void goNext();
  
    /** Move to the previous date(s) in the current view */
    void goPrevious();
  /** Move to the next date(s) in the current view */
    void goNextMonth();
  
    /** Move to the previous date(s) in the current view */
    void goPreviousMonth();

    void toggleExpand();
    void toggleDateNavigatorWidget();
    void toggleAllDaySize();
  
    /** Look for new messages in the inbox */
    void lookForIncomingMessages();
   /** Look for new messages in the outbox */
    void lookForOutgoingMessages();

    void processMainViewSelection( Incidence * );
    void processTodoListSelection( Incidence * );
    
    void processIncidenceSelection( Incidence * );

    void purgeCompleted();
    bool removeCompletedSubTodos( Todo* );
    void slotCalendarChanged();
    bool importBday();
    bool addAnniversary( QDate data, QString name, KCal::Attendee* a , bool birthday  );
    bool importQtopia( const QString &categoriesFile,
                       const QString &datebookFile,
                       const QString &tasklistFile );
    void syncExternal( int mode );
    void slotSelectPickerDate( QDate ) ;
    void showDatePicker() ;
    void showDatePickerPopup() ;
    void moveIncidence(Incidence *) ;
    void beamIncidence(Incidence *) ;
    void beamCalendar() ;
    void beamFilteredCalendar() ;
    void beamIncidenceList(QPtrList<Incidence>) ;
    void manageCategories();
    void editCategories();
    int addCategories();
    void removeCategories();
    void setSyncDevice( QString );
    void setSyncName( QString );
    void showDay( QDate );
    void undo_delete();
  protected slots:
    void resetFocus();
    void scrollBarValue(int);
    void slotViewerClosed();
    void timerAlarm();
    void suspendAlarm();
    void beamDone( Ir *ir );
    /** Select a view or adapt the current view to display the specified dates. */
    void showDates( const KCal::DateList & );
    void selectWeekNum ( int );
    void checkConflictForEvent();
  
  public:
    void createRunningDate4Todo(  Todo * runT,  QDateTime start ,  QDateTime end,  int secLenRunning, int secLenPausing,int dayInterval );
    // show a standard warning
    // returns KMsgBox::yesNoCancel()
    int msgCalModified(); 
    virtual bool sync(KSyncManager* manager, QString filename, int mode, QString resource);

    virtual bool syncExternal(KSyncManager* manager, QString resource);
    virtual void removeSyncInfo( QString syncProfile);
    void setSyncManager(KSyncManager* manager);
    void setLoadedFileVersion(QDateTime);
    bool checkFileVersion(QString fn);
    bool checkAllFileVersions();
    bool checkFileChanged(QString fn);
    Event* getLastSyncEvent();
    /** Adapt navigation units correpsonding to step size of navigation of the
     * current view.
     */
    void adaptNavigationUnits();
    bool synchronizeCalendar( Calendar* local,  Calendar* remote, int mode );
    int  takeEvent( Incidence* local,  Incidence* remote, int mode, bool full = false );
    //Attendee* getYourAttendee(Event *event);
    void setBlockShowDates( bool b ) { mBlockShowDates = b ;}
    void setScrollBarStep(int val );
    QString getLastSyncDT4device( QString syncDevice );
  protected:
    bool mMultiResourceSync;
    Event *mConflictingEvent;
    void schedule(Scheduler::Method, Incidence *incidence = 0);
    
    // returns KMsgBox::OKCandel()
    int msgItemDelete(const QString name);
    void showEventEditor();
    void showTodoEditor();
    Todo *selectedTodo();
  private:
#ifdef DESKTOP_VERSION
    QScrollBar * mDateScrollBar;
#endif
    bool flag_blockConflict;
    bool flag_blockScrollBar;
    bool flag_checkFileFirsttime;
    bool flag_clearallviewsEventDisplay;
    bool flag_clearallviewsupdateView;
    QDateTime mNextAlarmDateTime;
    bool mViewerCallerIsSearchDialog;
    bool mBlockShowDates;
    KSyncManager* mSyncManager;
    AlarmDialog * mAlarmDialog;
    QString mAlarmNotification;
    QString mSuspendAlarmNotification;
    QTimer* mSuspendTimer;
    QTimer* mAlarmTimer;
    QTimer* mRecheckAlarmTimer;
    void computeAlarm( QString );
    void startAlarm( QString, QString );
    void setSyncEventsReadOnly();
    
    QDateTime loadedFileVersion;
    void checkExternSyncEvent( QPtrList<Event> lastSync , Incidence* toDelete );
    void checkExternalId( Incidence * inc );
    int mGlobalSyncMode;
    QString  mCurrentSyncDevice;
    QString  mCurrentSyncName;
    void init();
    int mDatePickerMode;
    bool mFlagEditDescription;
    QDateTime mLastCalendarSync;
    void createPrinter();

    void calendarModified( bool, Calendar * );

    CalPrinter *mCalPrinter;

    QSplitter    *mPanner;
    QSplitter    *mLeftSplitter;
    KDGanttMinimizeSplitter  *mLeftFrame, *mMainFrame;
    QWidgetStack *mRightFrame;

    KDatePicker* mDatePicker;
    QVBox* mDateFrame;

    DateNavigatorContainer *mDateNavigator;       // widget showing small month view.

    KOFilterView *mFilterView;
    KOCalEditView *mCalEditView;

    ResourceView *mResourceView;

    // calendar object for this viewing instance
    Calendar      *mCalendar;

    CalendarResourceManager *mResourceManager;

    FileStorage *mStorage;

    DateNavigator *mNavigator;

    KOViewManager *mViewManager;
    KODialogManager *mDialogManager;

    // Calendar filters
    QPtrList<CalFilter> mFilters;

    // various housekeeping variables.
    bool            mModified;	   // flag indicating if calendar is modified
    bool            mReadOnly; // flag indicating if calendar is read-only
    QDate mSaveSingleDate;

    Incidence *mSelectedIncidence;
    Incidence *mMoveIncidence;
    QDate mMoveIncidenceOldDate;
    KOTodoView *mTodoList;
    KOEventEditor * mEventEditor;
    KOTodoEditor  * mTodoEditor;
    KOEventViewerDialog * mEventViewerDialog;
    void keyPressEvent ( QKeyEvent *e) ;
    //QMap<Incidence*,KOIncidenceEditor*> mDialogList;
};


class CalendarViewVisitor : public Incidence::Visitor
{
  public:
    CalendarViewVisitor() : mView( 0 ) {}

    bool act( Incidence *incidence, CalendarView *view )
    {
      mView = view;
      return incidence->accept( *this );
    }
    
  protected:
    CalendarView *mView;
};

class ShowIncidenceVisitor : public CalendarViewVisitor
{
  protected:
    bool visit( Event *event ) { mView->showEvent( event ); return true; }  
    bool visit( Todo *todo ) { mView->showTodo( todo ); return true; }  
    bool visit( Journal * j ) { mView->showJournal( j );return true; }
};

class EditIncidenceVisitor : public CalendarViewVisitor
{
  protected:
    bool visit( Event *event ) { mView->editEvent( event ); return true; }  
    bool visit( Todo *todo ) { mView->editTodo( todo ); return true; }  
    bool visit( Journal *j ) {  mView->editJournal( j); return true; }
};

class DeleteIncidenceVisitor : public CalendarViewVisitor
{
  protected:
    bool visit( Event *event ) { mView->deleteEvent( event ); return true; }  
    bool visit( Todo *todo ) { mView->deleteTodo( todo ); return true; }  
    bool visit( Journal * j) {mView->deleteJournal( j ); return true; }
};

#endif
