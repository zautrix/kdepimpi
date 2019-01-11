/*
  This file is part of KOrganizer.

  Requires the Qt and KDE widget libraries, available at no cost at
  http://www.troll.no and http://www.kde.org respectively

  Copyright (c) savecale1997, 1998, 1999
  Preston Brown (preton.brown@yale.edu)
  Fester Zigterman (F.J.F.ZigtermanRustenburg@student.utwente.nl)
  Ian Dawes (iadawes@globalserve.net)
  Laszlo Boloni (boloni@cs.purdue.edu)
  Copyright (c) 2000, 2001, 2002
  Cornelius Schumacher <schumacher@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdlib.h>

#include <qapplication.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qmultilineedit.h>
#include <qtimer.h>
#include <qwidgetstack.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qgroupbox.h>
#include <qfile.h>
#include <qdir.h>
#ifndef KORG_NOSPLITTER
#include <qsplitter.h>
#endif

#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kconfig.h>

#include <libkdepim/ksyncprefsdialog.h>
#include <krun.h>
#include <kdirwatch.h>
#include <libkdepim/kdatepicker.h>
#include <libkdepim/ksyncprofile.h>
#include <libkdepim/kpimglobalprefs.h>

#include <libkcal/vcaldrag.h>
#include <libkcal/icaldrag.h>
#include <libkcal/icalformat.h>
#include <libkcal/vcalformat.h>
#include <libkcal/scheduler.h>
#include <libkcal/calendarlocal.h>
#include <libkcal/journal.h>
#include <libkcal/calfilter.h>
#include <libkcal/attendee.h>
#include <libkcal/dndfactory.h>
#include <libkcal/freebusy.h>
#include <libkcal/filestorage.h>
#include <libkcal/calendarresources.h>
#include <libkcal/qtopiaformat.h>
#include "../kalarmd/alarmdialog.h"

#ifndef DESKTOP_VERSION
#include <libkcal/sharpformat.h>
#include <externalapphandler.h>
#endif
#include <libkcal/phoneformat.h>
#ifndef KORG_NOMAIL
#include "komailclient.h"
#endif
#ifndef KORG_NOPRINTER
#include "calprinter.h"
#endif
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif
#include "koeventeditor.h"
#include "kotodoeditor.h"
#include "koprefs.h"
#include "koeventviewerdialog.h"
#include "publishdialog.h"
#include "kofilterview.h"
#include "koglobals.h"
#include "koviewmanager.h"
#include "koagendaview.h"
#include "koagenda.h"
#include "kodialogmanager.h"
#include "outgoingdialog.h"
#include "incomingdialog.h"
#include "datenavigatorcontainer.h"
#include "statusdialog.h"
#include "kdatenavigator.h"
#include "kotodoview.h"
#include "datenavigator.h"
#include "resourceview.h"
#include "navigatorbar.h"
#include "searchdialog.h"
#include "mainwindow.h"
#include "categoryeditdialog.h"

#include "calendarview.h"
#ifndef DESKTOP_VERSION
#include <qtopia/alarmserver.h> 
#endif
#ifndef _WIN32_
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#else
#include <qprocess.h>
#endif

#ifdef DESKTOP_VERSION
#include <kabc/stdaddressbook.h>
#endif
using namespace KOrg;
using namespace KCal;
extern int globalFlagBlockAgenda;
extern int globalFlagBlockStartup;


MissedAlarmTextBrowser::MissedAlarmTextBrowser(QWidget *parent, QPtrList<Incidence> alarms,QDateTime start ) : QTextBrowser(parent) 

{
    mAlarms = alarms;
    viewport()->setBackgroundColor( QColor( 255, 255, 255 ) );
    QString mText = "<table width=\"100%\">\n";
    //mText += "<tr bgcolor=\"#3679AD\"><td><h2>"; 
#ifdef DESKTOP_VERSION
    mText += "<tr bgcolor=\"#5699CD\"><td align=\"center\"><h2>";
#else
    mText += "<tr bgcolor=\"#5699CD\"><td align=\"center\"><h3>";
#endif
    //  mText += "<img src=\"";
    //   mText += ipath;
    //   mText += "\">";
    //mEventDate = QDate::currentDate();
#ifdef DESKTOP_VERSION
    mText += "<font color=\"#FFFFFF\"> <em>" +  i18n("You missed the alarms for the following events or todos:")+"</em></font></h2>";
#else
    mText += "<font color=\"#FFFFFF\"> <em>" +  i18n("You missed the alarms for the following events or todos:")+"</em></font></h3>";
#endif
    //mText += "</td></tr>\n<tr  bgcolor=\"#FF997D\"><td>";

    Incidence * inc = getNextInc( start );
    int time = 0;
    //mText += "<table>";
    while ( inc ) {
        QDateTime dt ;
        QString tempText = "<a ";
        bool ok;
        dt = inc->getNextOccurence( start, &ok );
        if ( !ok ) continue;
        if ( inc->typeID() == eventID ) {
            tempText += "href=\"event:";
        } else if ( inc->typeID() == todoID ) {
            tempText += "href=\"todo:";
        }
        tempText += inc->uid() + "\">"; 
        if ( inc->typeID() == todoID )
            tempText += i18n("Todo: ");
        if ( inc->summary().length() > 0 )
            tempText += inc->summary();
        else
            tempText += i18n("-no summary-"); 
        QString timestr;
        if (!inc->doesFloat())
                timestr = KGlobal::locale()->formatDateTime( dt, KOPrefs::instance()->mShortDateInViewer) +": ";
            else
                timestr = KGlobal::locale()->formatDate( dt.date() , KOPrefs::instance()->mShortDateInViewer) +": ";
        if ( dt.date() < QDate::currentDate() && time == 0 ) {
            mText += "</td></tr>\n<tr  bgcolor=\"#FF997D\"><td>";
            mText += "<table>";
            time = 1;
        }
        if ( dt.date() == QDate::currentDate() && time <= 1 ) {
            if ( time > 0 )
                mText +="</table>";
            mText += "</td></tr>\n<tr  bgcolor=\"#FFDC64\"><td>";
            mText += "<table>";
            time = 2;
            
        } 
        if ( dt.date() > QDate::currentDate() && time <= 2 ) {
            if ( time > 0 )
                mText +="</table>";
            mText += "</td></tr>\n<tr  bgcolor=\"#6AFF6A\"><td>";
            mText += "<table>";
            time = 3;
        }
        mText +="<tr><td><b>";
        mText += timestr;
        mText += "</b></td><td>";
        mText += tempText;
        mText += "</td></tr>\n";
        inc = getNextInc( start );
    }
    mText +="</table>";
    setText( mText );
}

MissedAlarmTextBrowser::~MissedAlarmTextBrowser()
{
    //qDebug("delete MissedAlarmTextBrowser::~MissedAlarmTextBrowser() ");
}
Incidence *  MissedAlarmTextBrowser::getNextInc( QDateTime start )
{
    QDateTime dt ;
    Incidence * retInc;
    Incidence * inc = mAlarms.first();
    if ( inc == 0 )
        return 0;  
    bool ok;
    dt = inc->getNextOccurence( start, &ok );
    if ( ! ok ) return 0;
    QDateTime dtn ;
    retInc = inc;
    inc = mAlarms.next();
    while ( inc ) {
        dtn = inc->getNextOccurence( start, &ok );
        if ( ! ok ) return 0;
        if ( dtn < dt ) {
            dt = dtn;
            retInc = inc;
        }
        inc = mAlarms.next();
    }
    mAlarms.remove( retInc );
    return retInc;
    
}
void MissedAlarmTextBrowser::setSource(const QString & n)
{
    if (n.startsWith("event:")) {
#ifdef DESKTOP_VERSION
        emit showIncidence(n.mid(8));
#else
        emit showIncidence(n.mid(6));
#endif
        return;
    } else if (n.startsWith("todo:")) {
#ifdef DESKTOP_VERSION
        emit showIncidence(n.mid(7));
#else
        emit showIncidence(n.mid(5));
#endif
        return;
    } 
}


class KOBeamPrefs : public QDialog
{
  public:
    KOBeamPrefs( QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("Beam Options") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 3 );
        lay->setMargin( 3 );
        QButtonGroup* format = new  QButtonGroup( 1, Horizontal, i18n("File format"), this );
        lay->addWidget( format ); 
        format->setExclusive ( true ) ;
        QButtonGroup* time = new  QButtonGroup(1, Horizontal, i18n("Time format"), this );
        lay->addWidget( time ); time->setExclusive ( true ) ;
        vcal = new QRadioButton(" vCalendar ", format );
        ical = new QRadioButton(" iCalendar ", format );
        vcal->setChecked( true );
        tz = new QRadioButton(i18n(" With timezone "), time );
        local = new QRadioButton(i18n(" Local time "), time );
        tz->setChecked( true );
        QPushButton * ok = new QPushButton( i18n("Beam via IR!"), this );
        lay->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        lay->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
        connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
        resize( 200, 200 );
    }
      
    bool beamVcal() { return vcal->isChecked(); }
    bool beamLocal() { return local->isChecked(); }
private:
    QRadioButton* vcal, *ical, *local, *tz;
};
class KOCatPrefs : public QDialog
{
  public:
    KOCatPrefs( QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("Manage new Categories") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 3 );
        lay->setMargin( 3 );
        QLabel * lab = new QLabel( i18n("After importing/loading/syncing there may be new categories in events or todos which are not added automatically to the category list. Please choose what to do <b>now</b>:"), this );
        lay->addWidget( lab ); 
        QButtonGroup* format = new  QButtonGroup( 1, Horizontal, i18n("New categories not in list:"), this );
        lay->addWidget( format ); 
        format->setExclusive ( true ) ;
        addCatBut = new QRadioButton(i18n("Add to category list"), format );
        new QRadioButton(i18n("Remove from Events/Todos"), format );
        addCatBut->setChecked( true );
        QPushButton * ok = new QPushButton( i18n("Change category list now!"), this );
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



CalendarView::CalendarView( CalendarResources *calendar,
                            QWidget *parent, const char *name )
    : CalendarViewBase( parent, name ),
      mCalendar( calendar ),
      mResourceManager( calendar->resourceManager() )
{

    mEventEditor = 0;
    mTodoEditor = 0;
 
    init();
}

CalendarView::CalendarView( Calendar *calendar,
                            QWidget *parent, const char *name )
    : CalendarViewBase( parent, name ),
      mCalendar( calendar ),
      mResourceManager( 0 )
{
 
    mEventEditor = 0;
    mTodoEditor = 0;
    init();
}

void CalendarView::init()
{ 
    mMultiResourceSync = false; 
    flag_blockConflict = false; 
    flag_blockScrollBar = false;
    flag_checkFileFirsttime = true;
    flag_clearallviewsEventDisplay = false;
    flag_clearallviewsupdateView = false;
    mNextAlarmDateTime = QDateTime::currentDateTime();
    setFocusPolicy (NoFocus ); 
    mViewerCallerIsSearchDialog = false;
    mBlockShowDates = false;
    mConflictingEvent = 0;
    mDatePickerMode = 0;
    mCurrentSyncDevice = "";
    mViewManager = new KOViewManager( this );
    mDialogManager = new KODialogManager( this );
    mEventViewerDialog = 0;
    mModified = false;
    mReadOnly = false;
    mSelectedIncidence = 0;
    mCalPrinter = 0;
    mFilters.setAutoDelete(true);

    mCalendar->registerObserver( this );
    // TODO: Make sure that view is updated, when calendar is changed.

    mStorage = new FileStorage( mCalendar );
    mNavigator = new DateNavigator( this, "datevav", mViewManager );

    QBoxLayout *topLayout = (QBoxLayout*)layout();
#ifndef KORG_NOSPLITTER
    // create the main layout frames.
    mPanner = new QSplitter(QSplitter::Horizontal,this,"CalendarView::Panner");
    topLayout->addWidget(mPanner);

    mLeftSplitter = new QSplitter(QSplitter::Vertical,mPanner,
                                  "CalendarView::LeftFrame");
    mPanner->setResizeMode(mLeftSplitter,QSplitter::KeepSize);

    mDateNavigator = new DateNavigatorContainer( mLeftSplitter,
                                               "CalendarView::DateNavigator" );

    mLeftSplitter->setResizeMode(mDateNavigator,QSplitter::KeepSize);
    mTodoList = new KOTodoView(mCalendar, mLeftSplitter, "todolist_small2");
    mTodoList->setNavigator( mNavigator );
    mFilterView = new KOFilterView(&mFilters,mLeftSplitter,"CalendarView::FilterView");

#ifdef KORG_NORESOURCEVIEW 
    mResourceView = 0;
#else
    if ( mResourceManager ) {
        mResourceView = new ResourceView( mResourceManager, mLeftSplitter );
        mResourceView->updateView();
        connect( mResourceView, SIGNAL( resourcesChanged() ),
                 SLOT( updateView() ) );
    } else {
        mResourceView = 0;
    }
#endif
    QWidget *rightBox = new QWidget( mPanner );
    QBoxLayout *rightLayout = new QVBoxLayout( rightBox );

    mRightFrame = new QWidgetStack( rightBox );
    rightLayout->addWidget( mRightFrame, 1 );

    mLeftFrame = mLeftSplitter;
#else
    //QWidget *mainBox = new QWidget( this );
    //QWidget *leftFrame = new QWidget( mainBox );
    //QBoxLayout * mainBoxLayout;
    if ( KOPrefs::instance()->mVerticalScreen ) {
        //mainBoxLayout  = new QVBoxLayout(mainBox); 
        //leftFrameLayout = new QHBoxLayout(leftFrame ); 
        mMainFrame = new KDGanttMinimizeSplitter( Qt::Vertical, this );
        mMainFrame->setMinimizeDirection ( KDGanttMinimizeSplitter::Up );
        mLeftFrame = new KDGanttMinimizeSplitter( Qt::Horizontal, mMainFrame);;
        mLeftFrame->setMinimizeDirection ( KDGanttMinimizeSplitter::Right );
    } else {
        //mainBoxLayout  = new QHBoxLayout(mainBox);
        //leftFrameLayout = new  QVBoxLayout(leftFrame ); 
        mMainFrame = new KDGanttMinimizeSplitter( Qt::Horizontal, this);
        mMainFrame->setMinimizeDirection ( KDGanttMinimizeSplitter::Left);
        mLeftFrame = new KDGanttMinimizeSplitter( Qt::Vertical, mMainFrame);
        mLeftFrame->setMinimizeDirection ( KDGanttMinimizeSplitter::Up );
    }
    mMainFrame->setSizePolicy( QSizePolicy (QSizePolicy::Expanding,QSizePolicy::Expanding)   ); 
    //QBoxLayout * leftFrameLayout;
    topLayout->addWidget( mMainFrame );
#ifdef DESKTOP_VERSION
    mDateScrollBar = new QScrollBar ( 1, 365, 1,30, 200,QScrollBar::Horizontal, this );
    topLayout->addWidget( mDateScrollBar ); 
    connect( mDateScrollBar, SIGNAL( valueChanged ( int )  ),this, SLOT( scrollBarValue( int )) );
    if ( QApplication::desktop()->width() < 800 ) 
        mDateScrollBar->hide();
#endif
    //mainBoxLayout->addWidget (mLeftFrame);  
    mDateNavigator = new DateNavigatorContainer( mLeftFrame,
                                               "CalendarView::DateNavigator" );
#if 0
    // FIXME
    mDateNavigator = new KDateNavigator(mLeftFrame, mCalendar, TRUE,
                                        "CalendarView::DateNavigator", QDate::currentDate());
#endif
    // mDateNavigator->blockSignals( true );
    //leftFrameLayout->addWidget( mDateNavigator  );
    mTodoList = new KOTodoView(mCalendar, mLeftFrame, "todolistsmall");
    mFilterView = new KOFilterView(&mFilters,mLeftFrame,"CalendarView::FilterView"); 
    mCalEditView = new KOCalEditView(mLeftFrame,"CalendarView::CaleditView");
    connect( mCalEditView, SIGNAL( calendarEnabled (int,bool) ),mCalendar, SLOT( setCalendarEnabled(int,bool)) );
    connect( mCalEditView, SIGNAL( alarmEnabled(int,bool) ),mCalendar, SLOT( setAlarmEnabled(int,bool)) );
    connect( mCalEditView, SIGNAL( calendarReadonly(int,bool) ),this, SLOT( setCalReadOnly(int,bool)) );
    connect( mCalEditView, SIGNAL( setCalendarDefault(int) ),mCalendar, SLOT( setDefaultCalendar(int)) );
    connect( mCalEditView, SIGNAL( setCalendarDefault(int) ),mViewManager, SLOT( setDefaultCalendar(int)) );
    connect( mCalEditView, SIGNAL( removeCalendar(int) ),mCalendar, SLOT( setCalendarRemove(int)) );
    connect( mCalEditView, SIGNAL( calendarAdded(int) ),this, SLOT( addCalendarId(int)) );
    connect( mCalEditView, SIGNAL( needsUpdate() ),this, SLOT( updateView()) );
    connect( mCalEditView, SIGNAL( checkCalendar() ),this, SLOT( checkFiles()  ));
    connect( mCalEditView, SIGNAL( needsUpdate() ),this, SLOT( updateUnmanagedViews()) );
    connect( mCalEditView, SIGNAL( requestCalendarInfo( int ) ),this, SLOT( displayCalendarInfo( int ) ) );
   

    mTodoList->setNavigator( mNavigator );
#if 0
    if (  QApplication::desktop()->width() < 480 ) {
        leftFrameLayout->addWidget(mFilterView);
        leftFrameLayout->addWidget(mTodoList, 2 );
      
    } else {
        leftFrameLayout->addWidget(mTodoList,2 );
        leftFrameLayout->addWidget(mFilterView );
    } 
#endif
    mFilterView->hide();
    mCalEditView->hide();
    QWidget *rightBox = new QWidget( mMainFrame );
    //mainBoxLayout->addWidget ( rightBox, 10 );
    QBoxLayout *rightLayout = new QVBoxLayout( rightBox );
    mRightFrame = new QWidgetStack( rightBox );
    rightLayout->addWidget( mRightFrame, 10 );

    //mLeftFrame = (QWidget *)leftFrame;
    if ( KOPrefs::instance()->mVerticalScreen ) {
        //mDateNavigator->setFixedHeight( mDateNavigator->sizeHint().height() );
        //mDateNavigator->setMinimumWidth( mDateNavigator->sizeHint().width() );
        //mTodoList->setFixedHeight( mDateNavigator->sizeHint().height() );
        //leftFrame->setFixedHeight( mDateNavigator->sizeHint().height() );
    } else {
        //mDateNavigator->setFixedWidth( mDateNavigator->sizeHint().width() );
        //mTodoList->setFixedWidth( mDateNavigator->sizeHint().width() );
        //leftFrame->setFixedWidth( mDateNavigator->sizeHint().width() );
    }
    if ( !KOPrefs::instance()->mShowDateNavigator)
        mDateNavigator->hide();
    //qDebug("Calendarview Size %d %d ", width(), height());
#endif
 
    connect( mNavigator, SIGNAL( datesSelected( const KCal::DateList & ) ),
             SLOT( showDates( const KCal::DateList & ) ) );
   
    connect( mNavigator, SIGNAL( datesSelected( const KCal::DateList & ) ),
             mDateNavigator, SLOT( selectDates( const KCal::DateList & ) ) );
  
    
    
    connect( mDateNavigator, SIGNAL( showMonth( const QDate & ) ),
             mViewManager, SLOT(  showMonth( const QDate & ) ) );

    connect( mDateNavigator, SIGNAL( weekClicked( const QDate & ) ),
             mNavigator, SLOT( selectWeek( const QDate & ) ) );

    connect( mDateNavigator, SIGNAL( goPrevYear() ),
             mNavigator, SLOT( selectPreviousYear() ) );
    connect( mDateNavigator, SIGNAL( goNextYear() ),
             mNavigator, SLOT( selectNextYear() ) );
    connect( mDateNavigator, SIGNAL( goPrevMonth() ),
             mNavigator, SLOT( selectPreviousMonth() ) );
    connect( mDateNavigator, SIGNAL( goNextMonth() ),
             mNavigator, SLOT( selectNextMonth() ) );

    connect( mDateNavigator, SIGNAL( goPrevious() ),
             mNavigator, SLOT( selectPrevious() ) );
    connect( mDateNavigator, SIGNAL( goNext() ),
             mNavigator, SLOT( selectNext() ) );
    connect( mDateNavigator, SIGNAL( monthSelected ( int ) ),
             mNavigator, SLOT( slotMonthSelect( int ) ) );

    connect( mDateNavigator, SIGNAL( datesSelected( const KCal::DateList & ) ),
             mNavigator, SLOT( selectDates( const KCal::DateList & ) ) );
#if 0
    connect( mDateNavigator, SIGNAL( incidenceDropped( Incidence * ) ),
             SLOT( incidenceAdded(  Incidence *) ) );
#endif
    // connect(mDateNavigator,SIGNAL(dayPassed(QDate)),SLOT(updateView()));

    connect( this, SIGNAL( configChanged() ),
             mDateNavigator, SLOT( updateConfig() ) );

    connect( mTodoList, SIGNAL( newTodoSignal() ),
             SLOT( newTodo() ) );
    connect( mTodoList, SIGNAL( newSubTodoSignal( Todo *) ),
             SLOT( newSubTodo( Todo * ) ) );
    connect( mTodoList, SIGNAL( editTodoSignal( Todo * ) ),
             SLOT( editTodo( Todo * ) ) );
    connect( mTodoList, SIGNAL( showTodoSignal( Todo * ) ),
             SLOT( showTodo( Todo *) ) );
    connect( mTodoList, SIGNAL( deleteTodoSignal( Todo *) ),
             SLOT( deleteTodo( Todo *) ) );
    connect( this, SIGNAL( configChanged()), mTodoList, SLOT( updateConfig() ) );
    connect( mTodoList, SIGNAL( purgeCompletedSignal() ),
             SLOT( purgeCompleted() ) );
    connect( mTodoList, SIGNAL( todoModifiedSignal( Todo *, int ) ),
             SIGNAL( todoModified( Todo *, int ) ) );

    connect( mTodoList, SIGNAL( cloneTodoSignal( Incidence *  ) ),
             this, SLOT ( cloneIncidence( Incidence * ) ) );
    connect( mTodoList, SIGNAL( cancelTodoSignal( Incidence *  ) ),
             this, SLOT (cancelIncidence( Incidence * ) ) );

    connect( mTodoList, SIGNAL( moveTodoSignal( Incidence *  ) ),
             this, SLOT ( moveIncidence( Incidence * ) ) );
    connect( mTodoList, SIGNAL( beamTodoSignal( Incidence *  ) ),
            this, SLOT ( beamIncidence( Incidence * ) ) );
  
    connect( mTodoList, SIGNAL( unparentTodoSignal( Todo * ) ),
             this, SLOT ( todo_unsub( Todo * ) ) );
  
    connect( mTodoList, SIGNAL( reparentTodoSignal( Todo *,Todo * ) ),
                 this, SLOT ( todo_resub( Todo *,Todo * ) ) );
    connect( this, SIGNAL( todoModified( Todo *, int )), mTodoList,
             SLOT( updateTodo( Todo *, int ) ) );
    connect( this, SIGNAL( todoModified( Todo *, int )), this,
             SLOT( changeTodoDisplay( Todo *, int ) ) );


    connect( mFilterView, SIGNAL( filterChanged() ), SLOT( updateFilter() ) );
    connect( mFilterView, SIGNAL( editFilters() ), SLOT( editFilters() ) );
    connect( mCalendar, SIGNAL( addAlarm(const QDateTime &, const QString &  ) ), SLOT( addAlarm(const QDateTime &, const QString &  ) ) );
    connect( mCalendar, SIGNAL( removeAlarm(const QDateTime &, const QString &  ) ), SLOT( removeAlarm(const QDateTime &, const QString & ) ) );
  




    connect(QApplication::clipboard(),SIGNAL(dataChanged()),
            SLOT(checkClipboard()));
    connect( mTodoList,SIGNAL( incidenceSelected( Incidence * ) ),
             SLOT( processTodoListSelection( Incidence * ) ) );
    connect(mTodoList,SIGNAL(isModified(bool)),SLOT(setModified(bool)));

    // kdDebug() << "CalendarView::CalendarView() done" << endl; 

    mDateFrame = new QVBox(0,0,WType_Popup);
    //mDateFrame->setFrameStyle(QFrame::PopupPanel | QFrame::Raised);
    mDateFrame->setFrameStyle( QFrame::WinPanel |QFrame::Raised );
    mDateFrame->setLineWidth(3);
    mDateFrame->hide(); 
    mDateFrame->setCaption( i18n( "Pick a date to display"));
    mDatePicker = new KDatePicker ( mDateFrame , QDate::currentDate() );
 
    connect(mDatePicker,SIGNAL(dateSelected(QDate)),SLOT(slotSelectPickerDate(QDate)));
 
    mEventEditor = mDialogManager->getEventEditor();  
    mTodoEditor = mDialogManager->getTodoEditor();  
   
    mFlagEditDescription = false; 

    mSuspendTimer = new QTimer( this );
    mAlarmTimer = new QTimer( this );
    mRecheckAlarmTimer = new QTimer( this );
    connect( mRecheckAlarmTimer, SIGNAL(  timeout ()  ), SLOT( recheckTimerAlarm() ) );
    connect( mSuspendTimer, SIGNAL(  timeout ()  ), SLOT( suspendAlarm() ) );
    connect( mAlarmTimer, SIGNAL(  timeout ()  ), SLOT( timerAlarm() ) );
    mAlarmDialog = new AlarmDialog( this );
    connect( mAlarmDialog, SIGNAL( addAlarm(const QDateTime &, const QString &  ) ), SLOT( addSuspendAlarm(const QDateTime &, const QString &  ) ) );
    mAlarmDialog->setServerNotification( false );
    mAlarmDialog->setSuspendTime( KOPrefs::instance()->mAlarmSuspendTime );


#ifndef DESKTOP_VERSION
//US listen for arriving address resultsets
  connect(ExternalAppHandler::instance(), SIGNAL(receivedBirthdayListEvent(const QString&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&)),
          this, SLOT(insertBirthdays(const QString&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&)));
#endif
  mDateNavigator->setCalendar( mCalendar );
}


CalendarView::~CalendarView()
{
    // kdDebug() << "~CalendarView()" << endl;
    //qDebug("CalendarView::~CalendarView() ");
    delete mDialogManager;
    delete mViewManager;
    delete mStorage;
    delete mDateFrame ;
    delete mEventViewerDialog;
    //kdDebug() << "~CalendarView() done" << endl;
}


void CalendarView::nextConflict( bool all, bool allday )
{
    if ( flag_blockConflict ) return;
    flag_blockConflict = true;
    QPtrList<Event> testlist = mCalendar->events();
    Event * test = testlist.first();
    while ( test ) {
        test->setTagged( false );
        test = testlist.next();
    }
    QTime st ( 0,0,0);
    if ( mViewManager->currentView() == mViewManager->agendaView()  )
        st = mViewManager->agendaView()->agenda()->getEndTime();
    //qDebug("time %s ", st.toString().latin1());
    QDateTime startDT = QDateTime (mNavigator->selectedDates().first(),st);
    QDateTime conflict;
    QDateTime retVal;
    bool found = false;
    Event * cE = 0;
    Event * cE2 = 0;
    QPtrList<Event> testlist2 = testlist;
    test = testlist.first();
    bool skip = false;
    topLevelWidget()->setCaption( i18n("Checking conflicts ... please wait") );
    //QTime tm;
    //tm.start();
    while ( test ) {
        qApp->processEvents();
        skip = false;
        if ( !all ) skip = ( allday != test->doesFloat() );
        if ( !skip ) {
            if ( found )
                skip =  !test->matchTime( &startDT, &conflict );
            else 
               skip =  !test->matchTime( &startDT, 0 ); 
        }
        if ( !skip ) {
            Event * test2 = testlist2.first();
            while ( test2 ) { 
                skip = test2->isTagged();
                if ( !skip  && !all ) skip = ( allday != test2->doesFloat() );
                if ( !skip ) {
                    if ( found )
                        skip =  !test2->matchTime( &startDT, &conflict );
                    else 
                        skip =  !test2->matchTime( &startDT, 0 ); 
                }
                if ( !skip ) {
                    if ( test->isOverlapping ( test2, &retVal, &startDT ) ) {
                        //qDebug("overlap ");
                        if ( ! found ) {
                            if ( retVal >= startDT ) {
                                conflict = retVal; 
                                cE = test;
                                cE2 = test2;
                                found = true;
                            }
                        } else {
                            if ( retVal >= startDT && retVal < conflict ) {
                                conflict = retVal; 
                                cE = test;
                                cE2 = test2;
                            }
                        }
                    }
                }
                test2 = testlist2.next();
            }
        }
        test->setTagged( true );
        test = testlist.next();
    }
    //qDebug("Search time : %d", tm.elapsed());
    if ( found ) {
        if ( mViewManager->currentView() != mViewManager->agendaView() || mNavigator->selectedDates().count() > 1 )
            mViewManager->showDayView();
        mNavigator->slotDaySelect( conflict.date() );
        int hour = conflict.time().hour();
        mViewManager->agendaView()->setStartHour( hour  );
        topLevelWidget()->setCaption( i18n("Conflict %1 <-> %2"). arg( cE->summary().left( 20 ) ).arg( cE2->summary().left( 20 ) ) );
        flag_blockConflict = false;
        return;
    }

    topLevelWidget()->setCaption( i18n("No conflict found") );
    //qDebug("No conflict found ");
    flag_blockConflict = false;
    return;
}

void CalendarView::conflictAll()
{
    nextConflict ( true, true );
}
void CalendarView::conflictAllday()
{
    nextConflict ( false, true );
}
void CalendarView::conflictNotAll()
{
    nextConflict ( false, false );
}

void CalendarView::setCalReadOnly( int id, bool readO )
{
    if ( readO ) {
        emit save();
    }
    mCalendar->setReadOnly( id, readO );   
}
void CalendarView::setScrollBarStep(int val )
{
#ifdef DESKTOP_VERSION
    mDateScrollBar->setLineStep ( val );
#else
    Q_UNUSED( val );
#endif
}
void CalendarView::scrollBarValue(int val )
{
#ifdef DESKTOP_VERSION
    if ( QApplication::desktop()->width() < 800 ) return;
    if ( flag_blockScrollBar ) return;
    flag_blockScrollBar = true;
    int count = mNavigator->selectedDates().count();
    int day = mNavigator->selectedDates().first().dayOfYear();
    int  stepdays = val;
    if ( mDateScrollBar->lineStep () <= count ) {
        //val = val/mDateScrollBar->lineStep ()*mDateScrollBar->lineStep();
        //qDebug("VAL %d ",val );
        stepdays = (val-day)/mDateScrollBar->lineStep ()*mDateScrollBar->lineStep();
        stepdays = day+stepdays;
        if ( stepdays < 0 ) stepdays = 0;
    }
    if ( stepdays == day ) {
        flag_blockScrollBar = false;
        return;
    }
    int year =  mNavigator->selectedDates().first().year();
    QDate d ( year,1,1 );
    mNavigator->selectDates( d.addDays(  stepdays-1) , count );
    flag_blockScrollBar = false;
#else
    Q_UNUSED( val );
#endif

}
void CalendarView::updateView(const QDate &start, const QDate &end)
{
#ifdef DESKTOP_VERSION
    if ( ! mDateScrollBar->draggingSlider () ) {
        int dof = start.dayOfYear();
        //qDebug("dof %d day %d val %d ", dof, start.dayOfYear(),mDateScrollBar->value() );
        if (  dof != mDateScrollBar->value() ) {
            mDateScrollBar->blockSignals( true );
            mDateScrollBar->setValue( start.dayOfYear()); 
            mDateScrollBar->blockSignals( false );
        }
    }
#endif
    mTodoList->updateView();
    mViewManager->updateView(start, end);
    //mDateNavigator->updateView();
}



void CalendarView::checkFiles()
{
    QString message;
    QPtrList<KopiCalendarFile> calendars = KOPrefs::instance()->mCalendars;
    KopiCalendarFile * cal = calendars.first();
    while ( cal ) {
        if (  cal->mErrorOnLoad ) {
            message +=  cal->mName +"\n"+KGlobal::formatMessage ( "(" +i18n( "Filepath: ")+ cal->mFileName+")" ,0 )+"\n";
        }
        cal = calendars.next();
    }
    if ( !message.isEmpty() ) {
        message = KGlobal::formatMessage( i18n("Calendar(s) not loaded:"),0 )+"\n" + message +KGlobal::formatMessage(i18n("You can try to reload the calendar in the Resource View!"),0); 
        KMessageBox::error(this,message, i18n("Loading of calendar(s) failed"));
    }
    if ( flag_checkFileFirsttime ) {
        flag_checkFileFirsttime = false;
        QTimer::singleShot( 2000, this, SLOT ( checkAlarms() ));
    }
}
void CalendarView::checkAlarms()
{   
    KConfig *config = KOGlobals::config();
    config->setGroup( "AppRun" );
    QDateTime dt ( QDate (2005,1,1), QTime( 0,0,0 ) );
    int daysto = dt.daysTo( QDate::currentDate() );
    int days = config->readNumEntry( "LatestProgramStopDays" , daysto);
    dt =  dt.addDays( days );
    int secto = dt.secsTo( QDateTime::currentDateTime() );
    int secs = config->readNumEntry( "LatestProgramStopSecs" , secto) - 30;
    //qDebug("KO: Reading program stop %d ", secs);
    //secs -= ( 3600 * 24*3 ); // debug only 
    QDateTime latest = dt.addSecs ( secs );
    qDebug("KO: Last termination on %s ", latest.toString().latin1());
    //qDebug("KO: Current Time %s ",QDateTime::currentDateTime().toString().latin1() );
    QPtrList<Incidence> el = mCalendar->rawIncidences(); 
    QPtrList<Incidence> al;
    Incidence* inL = el.first(); 
    QDateTime cur = QDateTime::currentDateTime().addSecs(-59);
    qDebug("KO: Checking alarm until %s ", cur.toString().latin1());
    while ( inL ) {  
        bool ok = false;
        int offset = 0;
        QDateTime next = inL->getNextAlarmDateTime(& ok, &offset, latest ) ;
        if ( ok ) {
            //qDebug("OK %s",next.toString().latin1());
            if ( next < cur ) {
                al.append( inL );
                //qDebug("found missed alarm: %s ", inL->summary().latin1() );
            }
        }
        inL = el.next();
    }
    if ( al.count() ) {
        QDialog* dia = new QDialog( this, "huhu", false, WDestructiveClose | WStyle_StaysOnTop  );
        dia->setCaption( i18n("KO/Pi: Missing alarms!") );
        QVBoxLayout* lay = new QVBoxLayout( dia );
        lay->setSpacing( 0 );
        lay->setMargin( 0 );
        MissedAlarmTextBrowser* matb = new MissedAlarmTextBrowser ( dia, al, latest );
        connect( matb, SIGNAL( showIncidence( QString ) ),SLOT( showIncidence( QString ) ));
        lay->addWidget( matb ); 
        if ( QApplication::desktop()->width() == 480 || QApplication::desktop()->width() == 640 ) {
            int wid = 210;
            int x = QApplication::desktop()->width() - wid - 7;
            int y = QApplication::desktop()->height() - wid - 70;
            dia->setGeometry ( x,y,wid,wid);
        } else {
            int si = 220;
            if ( QApplication::desktop()->width() > 470 )
                si = 400;
            dia->resize(si,si/2);
        }
        dia->setBackgroundColor( QColor( 255, 255, 255 ) );
        dia->show();

    }
#if 0
    // for creating timetracker test data
    qDebug("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCc ");
    Todo * nt;
    mCalendar->close();
    {
        Todo * root1 = new Todo;
        mCalendar->addTodo( root1 );
        root1->setSummary( "Project1");
        root1->setPriority( 1 );
        root1->setCategories( QString( "Cat_Pro1") );
        root1->setDescription( "This is a test description of a root Project" );
        root1->setPercentComplete( 20 );
        updateView();
        QDateTime start = QDateTime ( QDate( 2004,11,1), QTime ( 7,30,0) );
        QDateTime end = QDateTime ( QDate( 2005,8,5), QTime ( 0,0,0) );
       
        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Planning Project1");
        nt->setPriority( 1 );
        nt->setDescription( "This is a test description of  Planning Project1" );
        nt->setPercentComplete( 20 );
        nt->setCategories( QString( "Cat_Pro1,Cat_Plan_Pro1") );
    

        Todo * sub1 = nt;
        sub1->setRelatedTo( root1 );

        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Planning Project1: Lutz");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setPercentComplete( 20 );
        nt->setCategories( QString( "Cat_Pro1,Cat_Plan_Pro1,Cat_Work_Lutz") );
        Todo * workLutz11 = nt;
        workLutz11->setRelatedTo( sub1 );

        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Planning Project1: Norbert");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setPercentComplete( 20 );
        nt->setCategories( QString( "Cat_Pro1,Cat_Plan_Pro1,Cat_Work_Norbert") );
        Todo * workNorbert11 = nt;
        workNorbert11->setRelatedTo( sub1 );
      
        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Work on 1");
        nt->setPriority( 1 );
        nt->setDescription( "This is a test description of  Work Project_1" );
        nt->setCategories( QString( "Cat_Pro1,Cat_Work_Pro1") );
        nt->setPercentComplete( 20 );

        Todo * sub2 = nt;
        sub2->setRelatedTo( root1 );


        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Work on 1: Lutz");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setCategories( QString( "Cat_Pro1,Cat_Work_Pro1,Cat_Work_Lutz") );
        nt->setPercentComplete( 20 );
        Todo * workLutz12 = nt;
        workLutz12->setRelatedTo( sub2 );

        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Work on 1: Norbert");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setCategories( QString( "Cat_Pro1,Cat_Work_Pro1,Cat_Work_Norbert") );
        nt->setPercentComplete( 20 );
        Todo * workNorbert12 = nt;
        workNorbert12->setRelatedTo( sub2 );

        int secLenRunning = 7200;
        int secLenPausing = 3600 * 3;
        int dayInterval = 1;
        //createRunningDate4Todo( root1, start, end, secLenRunning, secLenPausing, dayInterval );
        createRunningDate4Todo( root1, start, end, secLenRunning*24, secLenPausing, 14 );
        createRunningDate4Todo( sub1, start.addSecs( secLenRunning ), start.addDays( 10 ), secLenRunning*4, secLenPausing, 1);
        createRunningDate4Todo( sub2, start.addDays( 8 ), end, secLenRunning*4, secLenPausing, 3);
        createRunningDate4Todo( workLutz11, start, start.addDays( 8 ), secLenRunning, secLenPausing, 0);
        createRunningDate4Todo( workNorbert11, start, start.addDays( 8 ), secLenRunning*2, secLenPausing, 1);
        createRunningDate4Todo( workLutz12, start.addDays( 8 ),end, secLenRunning*5, secLenPausing, 3);
        createRunningDate4Todo( workNorbert12, start.addDays( 8 ), end, secLenRunning, secLenPausing*3, 0);

    }

    {
        Todo * root1 = new Todo;
        mCalendar->addTodo( root1 );
        root1->setSummary( "Project2");
        root1->setPriority( 1 );
        root1->setDescription( "This is a test description of a root Project 2" );
        root1->setPercentComplete( 20 );
        root1->setCategories( QString( "Cat_Pro2") );
        updateView();
        QDateTime start = QDateTime ( QDate( 2004,11,1), QTime ( 7,30,0) );
        QDateTime end = QDateTime ( QDate( 2005,8,5), QTime ( 0,0,0) );
        int secLenRunning = 7200;
        int secLenPausing = 3600 * 3;
        int dayInterval = 1;
    
        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Planning Project2");
        nt->setPriority( 1 );
        nt->setDescription( "This is a test description of  Planning_Project_2" );
        nt->setCategories( QString( "Cat_Pro2,Cat_Plan_Pro2") );
        nt->setPercentComplete( 20 );
    

        Todo * sub1 = nt;
        sub1->setRelatedTo( root1 );

        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Planning Project2: Lutz");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setCategories( QString( "Cat_Pro2,Cat_Plan_Pro2,Cat_Work_Lutz") );
        nt->setPercentComplete( 20 );
        Todo * workLutz11 = nt;
        workLutz11->setRelatedTo( sub1 );

        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Planning Project2: Norbert");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setCategories( QString( "Cat_Pro2,Cat_Plan_Pro2,Cat_Work_Norbert") );
        nt->setPercentComplete( 20 );
        Todo * workNorbert11 = nt;
        workNorbert11->setRelatedTo( sub1 );
      
        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Work on 2");
        nt->setPriority( 1 );
        nt->setDescription( "This is a test description of  Work Project_2" );
        nt->setCategories( QString( "Cat_Pro2,Cat_Work_Pro2") );
        nt->setPercentComplete( 20 );

        Todo * sub2 = nt;
        sub2->setRelatedTo( root1 );


        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Work on 2: Lutz");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setCategories( QString( "Cat_Pro2,Cat_Work_Pro2,Cat_Work_Lutz") );
        nt->setPercentComplete( 20 );
        Todo * workLutz12 = nt;
        workLutz12->setRelatedTo( sub2 );

        nt = new Todo;
        mCalendar->addTodo( nt );
        nt->setSummary( "Work on 2: Norbert");
        nt->setPriority( 1 );
        nt->setDescription( "This todo counts the actual work of a person on a project" );
        nt->setCategories( QString( "Cat_Pro2,Cat_Work_Pro2,Cat_Work_Norbert") );
        nt->setPercentComplete( 20 );
        Todo * workNorbert12 = nt;
        workNorbert12->setRelatedTo( sub2 );  
        createRunningDate4Todo( root1, start, end, secLenRunning, secLenPausing, 10 );
        createRunningDate4Todo( sub1, start.addSecs( secLenRunning*3 ), start.addDays( 20 ), secLenRunning*2, secLenPausing, 2);
        createRunningDate4Todo( sub2, start.addDays( 8 ), end, secLenRunning*3, secLenPausing, 7);
        createRunningDate4Todo( workLutz11, start, start.addDays( 18 ), secLenRunning/2, secLenPausing*5, 0);
        createRunningDate4Todo( workNorbert11, start.addDays( 8 ), start.addDays( 18 ), secLenRunning*5, secLenPausing*5, 1);
        createRunningDate4Todo( workLutz12, start.addDays( 8 ),end, secLenRunning, secLenPausing*8, 0);
        createRunningDate4Todo( workNorbert12, start.addDays( 28 ), end, secLenRunning/8, secLenPausing*6, 0);
    }
    updateView();
#endif

}
void CalendarView::createRunningDate4Todo(  Todo * runT,  QDateTime start ,  QDateTime end,  int secLenRunning, int secLenPausing,int dayInterval ) 
{
    static int ccc = 0;
    ++ccc;
    QDateTime t_start = start;
    runT->setPriority( 5 );
    runT->setPercentComplete( 0 );
    int count = 0;
    int prio = 5;
    int complete = 0;
    while ( t_start < end ) {
        ++count;
        if ( count > ccc ) {
            count = 0;
            --prio;
            if ( prio == 0 ) prio = 5;
            complete += 20;
            if ( complete > 100 ) complete = 0;
            runT->setPriority( prio );
            runT->setPercentComplete( complete );
        }
        runT->setRunning( true );
        runT->saveRunningInfo( "Additional tt comment: running on "+ t_start.toString(), t_start, t_start.addSecs( secLenRunning ) );
        if ( dayInterval )
            t_start = t_start.addDays( dayInterval );
        else {
            t_start = t_start.addSecs( secLenRunning + secLenPausing );
        }
    }
}

void CalendarView::showDay( QDate d )
{
    dateNavigator()->blockSignals( true );
    dateNavigator()->selectDate( d );
    dateNavigator()->blockSignals( false );
    mViewManager->showDayView();
    //dateNavigator()->selectDate( d );
}
void CalendarView::timerAlarm()
{
    //qDebug("CalendarView::timerAlarm() ");
    computeAlarm(mAlarmNotification );
}

void CalendarView::suspendAlarm()
{
    //qDebug(" CalendarView::suspendAlarm() ");
    computeAlarm(mSuspendAlarmNotification );

}

void  CalendarView::startAlarm( QString mess , QString filename)
{

    topLevelWidget()->showNormal();
    topLevelWidget()->setActiveWindow();
    topLevelWidget()->raise(); 

    mAlarmDialog->eventNotification( mess, KOPrefs::instance()->mAlarmPlayBeeps, filename, true,KOPrefs::instance()->mAlarmBeepInterval ,KOPrefs::instance()->mAlarmSuspendCount );
    QTimer::singleShot( 2000, this, SLOT( checkNextTimerAlarm() ) );

}
    
void CalendarView::checkNextTimerAlarm()
{
    mCalendar->checkAlarmForIncidence( 0, true );
}

void  CalendarView::computeAlarm( QString msg )
{

 QString mess = msg; 
    QString  mAlarmMessage = mess.mid( 9 ); 
    QString filename =  MainWindow::resourcePath();
    filename += "koalarm.wav";
    QString tempfilename;
    if ( mess.left( 13 )  == "suspend_alarm") {  
        bool error = false;
        int len = mess.mid( 13 ).find("+++");
        if ( len < 2 )
            error = true;
        else {
            tempfilename = mess.mid( 13, len );
            if ( !QFile::exists( tempfilename  ) )
                error = true;
        }
        if ( ! error ) {
            filename = tempfilename;
        }
        mAlarmMessage = mess.mid( 13+len+3 );
        //qDebug("suspend file %s ",tempfilename.latin1() );
        startAlarm( mAlarmMessage, filename);
        return;
    }
    if ( mess.left( 11 )  == "timer_alarm") {
        //mTimerTime = 0;
        startAlarm( mess.mid( 11 ), filename );
        return;
    } 
    if ( mess.left( 10 )  == "proc_alarm") {
        bool error = false;
        int len = mess.mid( 10 ).find("+++");
        if ( len < 2 )
            error = true;
        else {
            tempfilename = mess.mid( 10, len );
            if ( !QFile::exists( tempfilename  ) )
                error = true;
        }
        if ( error ) {
            mAlarmMessage = "Procedure Alarm\nError - File not found\n";
            mAlarmMessage += mess.mid( 10+len+3+9 );
        } else {
            //QCopEnvelope e("QPE/Application/kopi", "-writeFileSilent");
            //qDebug("-----system command %s ",tempfilename.latin1() );
#ifndef _WIN32_
            if ( vfork () == 0 ) {
                execl ( tempfilename.latin1(), 0 );
                return;
	    }
#else
		QProcess* p = new QProcess();
		p->addArgument(  tempfilename.latin1()  );
		p->start();
		return;
#endif
            
            return;
        }
       
        //qDebug("+++++++system command %s ",tempfilename.latin1() );
    }  
    if ( mess.left( 11 )  == "audio_alarm") {
        bool error = false;
        int len = mess.mid( 11 ).find("+++");
        if ( len < 2 )
            error = true;
        else {
            tempfilename = mess.mid( 11, len );
            if ( !QFile::exists( tempfilename  ) )
                error = true;
        }
        if ( ! error ) {
            filename = tempfilename;
        }
        mAlarmMessage = mess.mid( 11+len+3+9 );
        //qDebug("audio file command %s ",tempfilename.latin1() );
    } 
    if ( mess.left( 9 )  == "cal_alarm") {
        mAlarmMessage = mess.mid( 9 ) ;
    }
    
    startAlarm( mAlarmMessage, filename );


}

void CalendarView::addSuspendAlarm(const QDateTime &qdt, const QString &noti  )
{
    //qDebug("+++++addSUSPENDAlarm %s %s ", qdt.toString().latin1() , noti.latin1() );
   
    mSuspendAlarmNotification = noti;
    int ms = QDateTime::currentDateTime().secsTo( qdt )*1000;
    //qDebug("Suspend Alarm timer started with secs: %d ", ms/1000);
    mSuspendTimer->start( ms , true );
#ifdef DESKTOP_VERSION
    if ( QApplication::desktop()->width() < 1024 ) {
        QString mess = qdt.toString( "yyyy-MM-dd hh:mm:ss" ) + "\n" + noti;
        //qDebug("nextsuspendalarm = \n%s ",mess.latin1() );
        QString fn = QDir::homeDirPath() + "/.kopi_suspend_alarm";
        QFile file( fn );
        if (!file.open( IO_WriteOnly ) ) {
            qDebug("KO: Error writing next suspend alarm file %s\nContent: \n%s ", fn.latin1(), mess.latin1());
        } else {
            QTextStream ts( &file );
            ts << mess;
            file.close();
        }
    }
#endif

}

void CalendarView::addAlarm(const QDateTime &qdt, const QString &noti  )
{
    mNextAlarmDateTime = qdt;
    //qDebug("+++++addAlarm %s %s ", qdt.toString().latin1() , noti.latin1() );
    if ( ! KOPrefs::instance()->mUseInternalAlarmNotification ) {
#ifndef DESKTOP_VERSION
        AlarmServer::addAlarm ( qdt,"koalarm", noti.utf8() );
#endif     
        return;
    }
#ifdef DESKTOP_VERSION
    if ( QApplication::desktop()->width() < 1024 ) {
        QString mess = qdt.toString( "yyyy-MM-dd hh:mm:ss" ) + "\n" + noti;
        //qDebug("nextalarm = \n%s ",mess.latin1() );
        QString fn = QDir::homeDirPath() + "/.kopi_next_alarm";
        QFile file( fn );
        if (!file.open( IO_WriteOnly ) ) {
            qDebug("KO: Error writing next alarm file %s\nContent: \n%s ", fn.latin1(), mess.latin1());
        } else {
            QTextStream ts( &file );
            ts << mess;
            file.close();
        }
    }
#endif
    int maxSec;
    //maxSec = 5; //testing only 
    maxSec = 86400+3600; // one day+1hour
    mAlarmNotification = noti;
    int sec = QDateTime::currentDateTime().secsTo( qdt );
    if ( sec > maxSec ) {
        mRecheckAlarmTimer->start( maxSec * 1000 );
        //  qDebug("recheck Alarm timer started with secs: %d next alarm in sec:%d", maxSec,sec  );
        return;
    } else {
        mRecheckAlarmTimer->stop();
    }
    //qDebug("Alarm timer started with secs: %d ", sec);
    mAlarmTimer->start( sec * 1000 , true );

}
// called by mRecheckAlarmTimer to get next alarm
// we need this, because a QTimer has only a max range of 25 days
void CalendarView::recheckTimerAlarm()
{
    mAlarmTimer->stop();
    mRecheckAlarmTimer->stop();
    mCalendar->checkAlarmForIncidence( 0, true );
}
#ifndef DESKTOP_VERSION
void CalendarView::removeAlarm(const QDateTime &qdt, const QString &noti  )
#else
void CalendarView::removeAlarm(const QDateTime &, const QString &  )
#endif
{
    //qDebug("-----removeAlarm %s %s ", qdt.toString().latin1() , noti.latin1() );
    if ( ! KOPrefs::instance()->mUseInternalAlarmNotification ) {
#ifndef DESKTOP_VERSION
        AlarmServer::deleteAlarm (qdt ,"koalarm" ,noti.utf8() );
#endif   
        return;
    }
    mAlarmTimer->stop();
}
void CalendarView::selectWeekNum ( int num  )
{
    dateNavigator()->blockSignals( true );
    dateNavigator()->selectWeek( num );
    dateNavigator()->blockSignals( false );
    mViewManager->showWeekView();
}
KOViewManager *CalendarView::viewManager()
{
    return mViewManager;
}

KODialogManager *CalendarView::dialogManager()
{
    return mDialogManager;
}

QDate CalendarView::startDate()
{
    DateList dates = mNavigator->selectedDates();

    return dates.first();
}

QDate CalendarView::endDate()
{
    DateList dates = mNavigator->selectedDates();

    return dates.last();
}


void CalendarView::createPrinter()
{
#ifndef KORG_NOPRINTER
    if (!mCalPrinter) {
        mCalPrinter = new CalPrinter(this, mCalendar);
        connect(this, SIGNAL(configChanged()), mCalPrinter, SLOT(updateConfig()));
    }
#endif
}


//KOPrefs::instance()->mWriteBackFile
//KOPrefs::instance()->mWriteBackExistingOnly

//  0  syncPrefsGroup->addRadio(i18n("Take local entry on conflict"));
//  1  syncPrefsGroup->addRadio(i18n("Take remote entry on conflict"));
//  2  syncPrefsGroup->addRadio(i18n("Take newest entry on conflict"));
//  3  syncPrefsGroup->addRadio(i18n("Ask for every entry on conflict"));
//  4  syncPrefsGroup->addRadio(i18n("Force take local entry always"));
//  5  syncPrefsGroup->addRadio(i18n("Force take remote entry always"));

int  CalendarView::takeEvent( Incidence* local,  Incidence* remote, int mode , bool full )
{
 
    // 0 equal
    // 1 take local
    // 2 take remote
    // 3 cancel 
    QDateTime lastSync = mLastCalendarSync;
    QDateTime localMod = local->lastModified();
    QDateTime remoteMod = remote->lastModified();
    if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
        bool remCh, locCh;
        remCh = ( remote->getCsum(mCurrentSyncDevice) != local->getCsum(mCurrentSyncDevice) );
        //if ( remCh )        
            //qDebug("loc %s  rem %s", local->getCsum(mCurrentSyncDevice).latin1(), remote->getCsum(mCurrentSyncDevice).latin1() );
        locCh = ( localMod > mLastCalendarSync );
        if ( !remCh && ! locCh ) {
            //qDebug("both not changed ");
            lastSync = localMod.addDays(1);
            if ( mode <= SYNC_PREF_ASK )
                return 0;
        } else {
            if ( locCh ) {
                //qDebug("loc changed %d %s %s", local->revision() , localMod.toString().latin1(), mLastCalendarSync.toString().latin1());
                lastSync = localMod.addDays( -1 );
                if ( !remCh )
                    remoteMod = ( lastSync.addDays( -1 ) );
            } else {
                //qDebug(" not loc changed ");
                lastSync = localMod.addDays( 1 );
                if ( remCh )
                    remoteMod =( lastSync.addDays( 1 ) );
                
            }
        }
        full = true;  
        if ( mode < SYNC_PREF_ASK )
            mode = SYNC_PREF_ASK;
    } else {
          if ( localMod == remoteMod )
              // if ( local->revision() == remote->revision() )
                return 0;
        
    }
    // qDebug(" %d %d conflict on %s %s ", mode, full, local->summary().latin1(), remote->summary().latin1() );

    //qDebug("%s %d  %s %d", localMod.toString().latin1() , local->revision(), remoteMod.toString().latin1(), remote->revision());  
    //qDebug("%d %d %d %d ", localMod.time().second(), localMod.time().msec(),  remoteMod.time().second(), remoteMod.time().msec() );
    //full = true; //debug only
    if ( full ) {
        bool equ = false;
        if ( local->typeID() == eventID ) {
            equ =  (*((Event*) local) == *((Event*) remote));
        }
        else if ( local->typeID() == todoID )
            equ =  (*((Todo*) local) == (*(Todo*) remote));
        else if ( local->typeID() == journalID )
            equ =  (*((Journal*) local) == *((Journal*) remote));
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
    //qDebug("%s -- %s mLastCalendarSync %s lastsync %s --- local %s remote %s ",local->summary().latin1(), remote->summary().latin1(),mLastCalendarSync.toString().latin1() ,lastSync.toString().latin1()  ,  localMod.toString().latin1() , remoteMod.toString().latin1()  );


    // ************************************************
    // ************************************************
    // ************************************************
    // We may have that lastSync > remoteMod  AND  lastSync > localMod 
    // BUT  remoteMod != localMod 


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
        if ( localMod >= remoteMod )
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
        if ( lastSync > localMod )
            return 2;
        //qDebug("lsy %s  ---  lo %s  --- re %s ", lastSync.toString().latin1(), localMod.toString().latin1(), remoteMod.toString().latin1()  );
        localIsNew = localMod >= remoteMod;
        if ( localIsNew )
            getEventViewerDialog()->setColorMode( 1 );
         else 
            getEventViewerDialog()->setColorMode( 2 );
        getEventViewerDialog()->setIncidence(local);
        if ( localIsNew )
            getEventViewerDialog()->setColorMode( 2 );
         else 
            getEventViewerDialog()->setColorMode( 1 );
        getEventViewerDialog()->addIncidence(remote);
        getEventViewerDialog()->setColorMode( 0 );
        //qDebug("local %d remote %d ",local->relatedTo(),remote->relatedTo()  );
        getEventViewerDialog()->setCaption( mCurrentSyncDevice +i18n(" : Conflict! Please choose entry!"));
        getEventViewerDialog()->showMe();
        result = getEventViewerDialog()->executeS( localIsNew );
        return result;
          
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

QString CalendarView::getLastSyncDT4device( QString syncDevice )
{
  Event* lse; 
  mCalendar->setSyncEventsEnabled(); 
  lse = mCalendar->event( "last-syncEvent-"+ syncDevice );
  if ( ! lse ) return i18n("Never synced before");
  return KGlobal::locale()->formatDateTime( lse->dtStart(), true);
}
Event* CalendarView::getLastSyncEvent()
{
    Event* lse; 
    //qDebug("CurrentSyncDevice %s ",mCurrentSyncDevice .latin1() );
    mCalendar->setSyncEventsEnabled();
    lse =   mCalendar->event( "last-syncEvent-"+mCurrentSyncDevice );
    if (!lse) {
        qDebug("KO: New last Syncevent created for local: %s",mCurrentSyncDevice.latin1() );
        lse = new Event();
        lse->setUid( "last-syncEvent-"+mCurrentSyncDevice ); 
        QString sum = "";
        if ( mSyncManager->mExternSyncProfiles.contains( mCurrentSyncDevice ) )
            sum = "E: ";
        lse->setSummary(sum+mCurrentSyncDevice + i18n(" - sync event"));
        lse->setDtStart( mLastCalendarSync );
        lse->setDtEnd( mLastCalendarSync.addSecs( 7200 ) );
        lse->setCategories( i18n("SyncEvent") );
        lse->setReadOnly( true );
        lse->setCalID( 1 );
        mCalendar->addEvent( lse );
    } else
        qDebug("KO: Last Syncevent on local found");

    return lse;

}

// we check, if the to delete event has a id for a profile
//  if yes, we set this id in the profile to delete
void CalendarView::checkExternSyncEvent( QPtrList<Event> lastSync , Incidence* toDelete )
{
    if ( lastSync.count() == 0 ) {
        //qDebug(" lastSync.count() == 0");
        return;
    }
    if ( toDelete->typeID() == journalID )
        return;

    Event* eve = lastSync.first();

    while ( eve ) {
        QString id = toDelete->getID( eve->uid().mid( 15 ) ); // this is the sync profile name
        if ( !id.isEmpty() ) {
            QString des = eve->description();
            QString pref = "e";
            if ( toDelete->typeID() == todoID )
                pref = "t";
            des += pref+  id  + ",";
            eve->setReadOnly( false );
            eve->setDescription( des );
            //qDebug("setdes %s ", des.latin1());
            eve->setReadOnly( true );
        }
        eve = lastSync.next();
    }

}
void CalendarView::checkExternalId( Incidence * inc )
{
    QPtrList<Event> lastSync = mCalendar->getExternLastSyncEvents() ;
    checkExternSyncEvent( lastSync, inc );

}
// SSSSSSSSSSSSSSSSSSSSSS
bool  CalendarView::synchronizeCalendar( Calendar* local,  Calendar* remote, int mode )
{

    bool syncOK = true;
    int addedEvent = 0;
    int addedEventR = 0;
    int deletedEventR = 0;
    int deletedEventL = 0;
    int changedLocal = 0;
    int changedRemote = 0;
    int filteredIN = 0;
    int filteredOUT = 0;
    //QPtrList<Event> el = local->rawEvents(); 
    Event* eventR;
    QString uid;
    int take;
    Event* eventRSync;
    Event* eventLSync;
    clearAllViews();
    QPtrList<Event>  eventRSyncSharp = remote->getExternLastSyncEvents();
    QPtrList<Event>  eventLSyncSharp = local->getExternLastSyncEvents();
    bool fullDateRange = false;
    local->resetTempSyncStat();
    mLastCalendarSync = QDateTime::currentDateTime();
    if ( mSyncManager->syncWithDesktop() ) {
        remote->resetPilotStat(1);
        if (  KSyncManager::mRequestedSyncEvent.isValid() ) {
            mLastCalendarSync = KSyncManager::mRequestedSyncEvent;
            qDebug("KO: using extern time for calendar sync: %s ", mLastCalendarSync.toString().latin1() );
            } else {
            qDebug("KSyncManager::mRequestedSyncEvent has invalid datatime ");
        }
    }
    QDateTime modifiedCalendar = mLastCalendarSync;
    eventLSync = getLastSyncEvent();
    eventR = remote->event("last-syncEvent-"+mCurrentSyncName );
    if ( eventR ) {
        qDebug("KO: Last-syncEvent on remote found ");
        eventRSync = (Event*) eventR->clone();
        remote->deleteEvent(eventR );

    } else {
        if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL || mSyncManager->syncWithDesktop()) {
            eventRSync = (Event*)eventLSync->clone();
        } else {
            fullDateRange = true;
            eventRSync = new Event();
            eventRSync->setSummary(mCurrentSyncName + i18n(" - sync event"));
            eventRSync->setUid("last-syncEvent-"+mCurrentSyncName ); 
            eventRSync->setDtStart( mLastCalendarSync );
            eventRSync->setDtEnd( mLastCalendarSync.addSecs( 7200 ) );
            eventRSync->setCategories( i18n("SyncEvent") );
        }
    }
    if ( eventLSync->dtStart() == mLastCalendarSync )
        fullDateRange = true;
   
    if ( ! fullDateRange  ) {
        if ( eventLSync->dtStart() != eventRSync->dtStart() ) {
            
            qDebug("KO: Sync: Set fulldate to true! Local: %s --- Remote: %s" ,eventLSync->dtStart().toString().latin1(), eventRSync->dtStart().toString().latin1() ); 
            //qDebug("%d %d %d %d ", eventLSync->dtStart().time().second(), eventLSync->dtStart().time().msec() , eventRSync->dtStart().time().second(), eventRSync->dtStart().time().msec());
            fullDateRange = true;
        } 
    }
    if ( mSyncManager->syncWithDesktop() ) {
        fullDateRange = ( eventLSync->dtStart() <= mLastCalendarSync && eventLSync->dtStart().addSecs(1) >= mLastCalendarSync  ); 
    }
    if ( fullDateRange )
        mLastCalendarSync = QDateTime::currentDateTime().addDays( -100*365);
    else
        mLastCalendarSync = eventLSync->dtStart();
    // for resyncing if own file has changed
    if ( mCurrentSyncDevice == "deleteaftersync" ) {
        mLastCalendarSync = loadedFileVersion;
        //qDebug("setting mLastCalendarSync ");
    }
    //qDebug("*************************** ");
    qDebug("KO: mLastCalendarSync %s .Full: %d",mLastCalendarSync.toString().latin1(), fullDateRange);
    //KMessageBox::information( this, i18n("KO: mLastCalendarSync %1 .Full: %2").arg(mLastCalendarSync.toString()).arg(QString::number(fullDateRange)), i18n("Last sync info"));
    QPtrList<Incidence> er = remote->rawIncidences(); 
    Incidence* inR = er.first();
    Incidence* inL;
    QProgressBar bar( er.count(),0 );
    bar.setCaption (i18n("Syncing - close to abort!") );
    
    // ************** setting up filter *************
    CalFilter *filterIN = 0;
    CalFilter *filterOUT = 0;
    CalFilter *filter = mFilters.first();
    while(filter) {
        if ( filter->name() == mSyncManager->mFilterInCal )
            filterIN = filter;
        if ( filter->name() == mSyncManager->mFilterOutCal )
            filterOUT = filter;
        filter = mFilters.next();
    }
    int w = 300;
    if ( QApplication::desktop()->width() < 320 )
        w = 220;
    int h = bar.sizeHint().height() ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    bar.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    bar.show();
    int modulo = (er.count()/10)+1;
    int incCounter = 0;
    while ( inR ) {
        if ( ! bar.isVisible() )
            return false;
        if ( incCounter % modulo == 0 )
            bar.setProgress( incCounter );
        ++incCounter;
        uid = inR->uid(); 
        bool skipIncidence = false;
        if ( uid.left(15) == QString("last-syncEvent-") )
            skipIncidence = true;
        QString idS;
        qApp->processEvents();
        if ( !skipIncidence ) {
            int hasCalId = 0;
            inL = local->incidenceForUid( uid , false , true, &hasCalId );
            if ( hasCalId && !inL )
                inL = local->incidenceForUid( uid , false , true, &hasCalId );
            else
                hasCalId = 0;
            if ( inL ) { // maybe conflict - same uid in both calendars
                
                if ( (take = takeEvent( inL, inR, mode, fullDateRange )) > 0 ) {
                    //qDebug("take %d %s ", take, inL->summary().latin1());
                    if ( take == 3 )
                        return false;
                    if ( take == 1  ) {// take local **********************
                        if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL )
                            inL->setCsum( mCurrentSyncDevice, inR->getCsum(mCurrentSyncDevice) );
                        else
                            idS = inR->IDStr();
                        int calID = inR->calID();
                        remote->deleteIncidence( inR );
                        if ( !hasCalId  ) {
                            inR = inL->clone();
                            inR->setCalID_block( calID );
                            inR->setTempSyncStat( SYNC_TEMPSTATE_INITIAL );
                            if ( mGlobalSyncMode != SYNC_MODE_EXTERNAL )
                                inR->setIDStr( idS );
                            remote->addIncidence(  inR );
                            if ( mSyncManager->syncWithDesktop() )
                                inR->setPilotId( 2 );
                            ++changedRemote;
                        } else
                            ++deletedEventR; 
                    } else {// take remote  **********************
                        if ( !inL->isReadOnly() ) {
                            idS = inL->IDStr();
                            int pid = inL->pilotId();
                            int calID = inL->calID();
                            if ( hasCalId  )
                                calID = 0;// add to default calendar
                            local->deleteIncidence( inL );
                            inL = inR->clone();
                            inL->setCalID_block( calID );
                            if ( mSyncManager->syncWithDesktop() )
                                inL->setPilotId( pid  );
                            inL->setIDStr( idS );
                            if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                                inL->setCsum( mCurrentSyncDevice, inR->getCsum(mCurrentSyncDevice) );
                                inL->setID( mCurrentSyncDevice, inR->getID(mCurrentSyncDevice) );
                            }
                            local->addIncidence(  inL  );
                            ++changedLocal;
                        }
                    }
                } else {
                    // take == 0; events equal
                    if ( hasCalId  ) {
                        qDebug("KO: Changing Cal id %d to %d for : %s  ",  hasCalId, local->defaultCalendar(),inL->summary().latin1());
                        inL->setCalID( local->defaultCalendar() );
                    }

                }
                
                
            } else { //  no conflict ********** add or delete remote 
                if ( !filterIN || filterIN->filterCalendarItem( inR  ) ){
                    if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                        QString des = eventLSync->description(); 
                        QString pref = "e";
                        if ( inR->typeID() == todoID )
                            pref = "t";
                        if ( des.find(pref+ inR->getID(mCurrentSyncDevice)  +"," ) >= 0 && mode != 5) { // delete it
                            inR->setTempSyncStat( SYNC_TEMPSTATE_DELETE );
                            //remote->deleteIncidence( inR );
                            ++deletedEventR; 
                        } else {
                            inR->setLastModified( modifiedCalendar );
                            inL = inR->clone();
                            inL->setIDStr( ":" );
                            inL->setCsum( mCurrentSyncDevice, inR->getCsum(mCurrentSyncDevice) );
                            inL->setID( mCurrentSyncDevice, inR->getID(mCurrentSyncDevice) );
                            
                            inL->setCalID_block( 0 );// add to default cal
                            local->addIncidence( inL );
                            ++addedEvent;
                        
                        }
                    } else {
                        if ( inR->lastModified() > mLastCalendarSync || mode == 5 ) {
                            inR->setLastModified( modifiedCalendar );
                            inL = inR->clone();
                            inL->setIDStr( ":" );
                            inL->setCalID_block( 0 );// add to default cal
                            local->addIncidence( inL );
                            ++addedEvent;
                        
                        } else {
                            checkExternSyncEvent(eventRSyncSharp, inR);
                            remote->deleteIncidence( inR );
                            ++deletedEventR; 
                        }
                    }
                } else {
                    ++filteredIN;
                }
            }
        }
        inR =  er.next();
    }
    QPtrList<Incidence> el = local->rawIncidences(); 
    inL = el.first();
    modulo = (el.count()/10)+1;
    bar.setCaption (i18n("Add / remove events") ); 
    bar.setTotalSteps (  el.count()  ) ;
    bar.show();
    incCounter = 0;
   
    while ( inL ) {
           
        qApp->processEvents();
        if ( ! bar.isVisible() )
            return false;
        if ( incCounter % modulo == 0 )
            bar.setProgress( incCounter );
        ++incCounter;
        uid = inL->uid();
        bool skipIncidence = false;
        if ( uid.left(15) == QString("last-syncEvent-") )
            skipIncidence = true;
        if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL && inL->typeID() == journalID )
            skipIncidence = true;
        if ( !skipIncidence )  {
            inR = remote->incidenceForUid( uid , true, true );
            if ( ! inR ) { 
                if ( !filterOUT ||  filterOUT->filterCalendarItem( inL ) ){
                    //  no conflict ********** add or delete local
                    if ( mGlobalSyncMode == SYNC_MODE_EXTERNAL ) {
                        if ( !inL->getID(mCurrentSyncDevice).isEmpty() && mode != 4 ) {
                            checkExternSyncEvent(eventLSyncSharp, inL);
                            local->deleteIncidence( inL );
                            ++deletedEventL; 
                        } else {
                            if ( ! mSyncManager->mWriteBackExistingOnly ) {
                                inL->removeID(mCurrentSyncDevice );
                                ++addedEventR; 
                                //qDebug("remote added Incidence %s ", inL->summary().latin1());
                                inL->setLastModified( modifiedCalendar );
                                inR = inL->clone();
                                inR->setIDStr( ":" );
                                inR->setTempSyncStat( SYNC_TEMPSTATE_INITIAL );
                                inR->setCalID_block( 0 );// add to default cal
                                remote->addIncidence(  inR );
                            }
                        }
                    } else {
                        if ( inL->lastModified() < mLastCalendarSync && mode != 4 ) {
                            checkExternSyncEvent(eventLSyncSharp, inL);
                            local->deleteIncidence( inL );
                            ++deletedEventL; 
                        } else {
                            if ( ! mSyncManager->mWriteBackExistingOnly ) {
                                ++addedEventR; 
                                inL->setLastModified( modifiedCalendar );
                                inR = inL->clone();
                                inR->setIDStr( ":" );
                                inR->setCalID_block( 0 );// add to default cal
                                remote->addIncidence(  inR );
                            }
                        }
                    }
                } else {
                    ++filteredOUT;
                }
            }
        }
        inL = el.next();
    }
    int delFut = 0;
    int remRem = 0;
    if ( mSyncManager->mWriteBackInFuture ) {
        er = remote->rawIncidences(); 
        remRem = er.count();
        inR = er.first();
        QDateTime dt;
        QDateTime cur = QDateTime::currentDateTime().addDays( -(mSyncManager->mWriteBackInPast * 7) );
        QDateTime end = QDateTime::currentDateTime().addDays( (mSyncManager->mWriteBackInFuture ) *7 );
        while ( inR ) {
            if ( inR->typeID() == todoID ) {
                Todo * t = (Todo*)inR;
                if ( t->hasDueDate() )
                    dt = t->dtDue();
                else
                    dt = cur.addSecs( 62 );
            }
            else if (inR->typeID() == eventID ) {
                bool ok;
                dt =  inR->getNextOccurence( cur, &ok );
                    if ( !ok )
                        dt = cur.addSecs( -62 );
                }
            else
                dt = inR->dtStart();
            if ( dt < cur || dt > end ) {
                remote->deleteIncidence( inR );
                ++delFut;
            }
            inR =  er.next();
        }
    }
    bar.hide();
    mLastCalendarSync = QDateTime::currentDateTime().addSecs( 1 );
    if ( mMultiResourceSync ) {
        remote->removeSyncInfo( "" ); //all
        qDebug("KO: Removing all sync info on remote ");
    } else {
        eventLSync->setReadOnly( false );
        eventLSync->setDtStart( mLastCalendarSync );
        eventRSync->setDtStart( mLastCalendarSync );
        eventLSync->setDtEnd( mLastCalendarSync.addSecs( 3600 ) );
        eventRSync->setDtEnd( mLastCalendarSync.addSecs( 3600 ) );
        eventRSync->setLocation( i18n("Remote from: ")+mCurrentSyncName ) ;
        eventLSync->setLocation(i18n("Local from: ") + mCurrentSyncName  );
        eventLSync->setReadOnly( true );
    }
    qDebug("KO: Normal sync: %d ",mGlobalSyncMode ==  SYNC_MODE_NORMAL );
    if ( mGlobalSyncMode ==  SYNC_MODE_NORMAL && !mSyncManager->syncWithDesktop() && !mMultiResourceSync ) // kde is abnormal...
        remote->addEvent( eventRSync );
    else
        delete eventRSync;
    qDebug("KO: Sync with desktop %d ",mSyncManager->syncWithDesktop() );
    QString mes;
    mes .sprintf(  i18n("Synchronization summary:\n\n %d items added to local\n %d items added to remote\n %d items updated on local\n %d items updated on remote\n %d items deleted on local\n %d items deleted on remote\n %d incoming filtered out\n %d outgoing filtered out\n"),addedEvent, addedEventR, changedLocal, changedRemote, deletedEventL, deletedEventR, filteredIN, filteredOUT ); 
    QString delmess;
    if ( delFut  ) {
        delmess.sprintf(  i18n("%d items skipped on remote,\nbecause they are more\nthan %d weeks in the past or\nmore than %d weeks in the future.\nAfter skipping, remote has\n%d calendar/todo items."), delFut,mSyncManager->mWriteBackInPast,mSyncManager->mWriteBackInFuture, remRem-delFut); 
        mes += delmess;
    }
    mes = i18n("Local device: %2\nRemote host: %1\n").arg(mCurrentSyncDevice).arg(mCurrentSyncName  ) +mes;
    mCalendar->checkAlarmForIncidence( 0, true );
    qDebug( mes );
    if ( mSyncManager->mShowSyncSummary ) {
        if ( KMessageBox::Cancel == KMessageBox::warningContinueCancel(this, mes,
                                                 i18n("KO/Pi Synchronization Results"),i18n("Write back"))) {
            qDebug("KO: WB cancelled ");
            mSyncManager->mWriteBackFile = false;
            return syncOK;
        }
    }
    return syncOK;
}

void CalendarView::setSyncDevice( QString s )
{
    mCurrentSyncDevice= s;
}
void CalendarView::setSyncName( QString s )
{
    mCurrentSyncName= s;
}
bool CalendarView::syncCalendar(QString filename, int mode)
{
    //qDebug("syncCalendar %s ", filename.latin1());
    mGlobalSyncMode = SYNC_MODE_NORMAL;
    CalendarLocal* calendar = new CalendarLocal();
    calendar->setTimeZoneId(KPimGlobalPrefs::instance()->mTimeZoneId);
    FileStorage* storage = new FileStorage( calendar );
    bool syncOK = false;
    storage->setFileName( filename );
    // qDebug("loading ... ");
    if ( storage->load() ) {
        getEventViewerDialog()->setSyncMode( true );
        syncOK = synchronizeCalendar( mCalendar, calendar, mode );
        getEventViewerDialog()->setSyncMode( false );
        if (  syncOK ) {
           if ( mSyncManager->mWriteBackFile )
            {
                storage->setSaveFormat( new ICalFormat() );
                storage->save();
            }
        }
        setModified( true );
    }
    delete storage;
    delete calendar;
    if ( syncOK )
        updateView();
    return syncOK;
}

void CalendarView::syncExternal( int mode  )
{
    mGlobalSyncMode = SYNC_MODE_EXTERNAL;
    qApp->processEvents();
    CalendarLocal* calendar = new CalendarLocal();
    calendar->setTimeZoneId(KPimGlobalPrefs::instance()->mTimeZoneId);
    bool syncOK = false;
    bool loadSuccess = false;
    PhoneFormat* phoneFormat = 0;
    emit tempDisableBR(true);
#ifndef DESKTOP_VERSION
    SharpFormat* sharpFormat = 0;
    if ( mode == 0 ) { // sharp
        sharpFormat = new SharpFormat () ;
        loadSuccess = sharpFormat->load( calendar, mCalendar ); 

    } else 
#endif
        if ( mode == 1 ) { // phone 
        phoneFormat = new PhoneFormat (mCurrentSyncDevice,
                                         mSyncManager->mPhoneDevice,
                                         mSyncManager->mPhoneConnection,
                                         mSyncManager->mPhoneModel);
        loadSuccess = phoneFormat->load( calendar,mCalendar); 
        
        } else {
            emit tempDisableBR(false);
            return;
        }
    if ( loadSuccess ) {
        getEventViewerDialog()->setSyncMode( true );
        syncOK = synchronizeCalendar( mCalendar, calendar, mSyncManager->mSyncAlgoPrefs );
        getEventViewerDialog()->setSyncMode( false );
        qApp->processEvents();
        if (  syncOK ) {
            if ( mSyncManager->mWriteBackFile )
            {
                QPtrList<Incidence> iL = mCalendar->rawIncidences(); 
                Incidence* inc = iL.first();
                if ( phoneFormat ) {
                    while ( inc ) {
                        inc->removeID(mCurrentSyncDevice);
                        inc =  iL.next();
                    }
                }
#ifndef DESKTOP_VERSION
                if ( sharpFormat )
                    sharpFormat->save(calendar);
#endif
                if ( phoneFormat )
                    phoneFormat->save(calendar);
                iL = calendar->rawIncidences(); 
                inc = iL.first();
                Incidence* loc;
                while ( inc ) {
                    if ( inc->tempSyncStat() == SYNC_TEMPSTATE_NEW_ID ) {
                        loc = mCalendar->incidence(inc->uid() );
                        if ( loc ) {
                            loc->setID(mCurrentSyncDevice, inc->getID(mCurrentSyncDevice) );
                            loc->setCsum( mCurrentSyncDevice, inc->getCsum(mCurrentSyncDevice) );
                        }
                    }
                    inc =  iL.next();
                }
                Incidence* lse = getLastSyncEvent();
                if ( lse ) {
                    lse->setReadOnly( false );
                    lse->setDescription( "" );
                    lse->setReadOnly( true );
                }
            }
        } else {
            topLevelWidget()->setCaption( i18n("Sync cancelled or failed.") );
        }
        setModified( true );
    } else {
        QString question = i18n("Sorry, the database access\ncommand failed!\n\nNothing synced!\n") ;
        QMessageBox::information( 0, i18n("KO/Pi Import - ERROR"),
                                      question, i18n("Ok")) ;

    }
    delete calendar;
    updateView();
    emit tempDisableBR(false);
    return ;//syncOK;

}

bool CalendarView::importBday()
{
#ifndef KORG_NOKABC

#ifdef DESKTOP_VERSION
    int curCal = mCalendar->defaultCalendar();
    int bd = mCalEditView->getBirtdayID();
    if ( bd == 0 )
        return false;
    mCalendar->setDefaultCalendar( bd );
    KABC::StdAddressBook* AddressBook = KABC::StdAddressBook::self( true );
    KABC::AddressBook::Iterator it;
    int count = 0;
    for( it = AddressBook->begin(); it != AddressBook->end(); ++it ) { 
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
    bar.setCaption (i18n("Reading addressbook - close to abort!") );
    qApp->processEvents();
    count = 0;
    int addCount = 0;
    KCal::Attendee* a = 0;
    for( it = AddressBook->begin(); it != AddressBook->end(); ++it ) { 
        if ( ! bar.isVisible() )
            return false;
        bar.setProgress( count++ );
        qApp->processEvents();
        //qDebug("add BDay %s %s", (*it).realName().latin1(),(*it).birthday().date().toString().latin1() );
        if ( (*it).birthday().date().isValid() ){
            a = new KCal::Attendee( (*it).realName(), (*it).preferredEmail(),false,KCal::Attendee::NeedsAction,KCal::Attendee::ReqParticipant,(*it).uid()) ;
            if ( addAnniversary(  (*it).birthday().date(), (*it).assembledName(),  a, true ) )
            ++addCount;
        } 
        QDate anni = KGlobal::locale()->readDate( (*it).custom("KADDRESSBOOK", "X-Anniversary" ), "%Y-%m-%d"); 
        if ( anni.isValid() ){ 
            a = new KCal::Attendee( (*it).realName(), (*it).preferredEmail(),false,KCal::Attendee::NeedsAction,KCal::Attendee::ReqParticipant,(*it).uid()) ;
            if ( addAnniversary(  anni, (*it).assembledName(),  a, false ) )
            ++addCount;
        }
    }
    mCalendar->setDefaultCalendar( curCal );
    updateView(); 
    topLevelWidget()->setCaption(QString::number( addCount )+ i18n(" birthdays/anniversaries added!"));
#else //DESKTOP_VERSION

  ExternalAppHandler::instance()->requestBirthdayListFromKAPI("QPE/Application/kopi", this->name() /* name is here the unique uid*/);
  // the result should now arrive through method insertBirthdays

#endif //DESKTOP_VERSION

#endif //KORG_NOKABC


    return true;
}
 
// This method will be called from Ka/Pi as a response to requestBirthdayListFromKAPI
void CalendarView::insertBirthdays(const QString& uid, const QStringList& birthdayList, 
				   const QStringList& anniversaryList, const QStringList& realNameList,
				   const QStringList& emailList, const QStringList& assembledNameList, 
				   const QStringList& uidList)
{
  
    //qDebug("KO::CalendarView::insertBirthdays");
  if (uid == this->name())
  {  
      int curCal = mCalendar->defaultCalendar();
    int bd = mCalEditView->getBirtdayID();
    if ( bd == 0 )
        return;
    mCalendar->setDefaultCalendar( bd ); 


    int count = birthdayList.count();
    int addCount = 0;
    KCal::Attendee* a = 0;

    //qDebug("CalView 1 %i", count);

    QProgressBar bar(count,0 );
    int w = 300;
    if ( QApplication::desktop()->width() < 320 )
        w = 220;
    int h = bar.sizeHint().height() ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    bar.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    bar.show();
    bar.setCaption (i18n("inserting birthdays - close to abort!") );
    qApp->processEvents();

    QDate birthday;
    QDate anniversary;
    QString realName;
    QString email;
    QString assembledName;
    QString uid;
    bool ok = true;
    for ( int i = 0; i < count; i++)
        {
            if ( ! bar.isVisible() )
                return;
            bar.setProgress( i );
            qApp->processEvents();

            birthday = KGlobal::locale()->readDate(birthdayList[i], KLocale::ISODate, &ok);
            if (!ok) {
                ;//qDebug("CalendarView::insertBirthdays found invalid birthday: %s",birthdayList[i].latin1());
            }

            anniversary = KGlobal::locale()->readDate(anniversaryList[i], KLocale::ISODate, &ok);
            if (!ok) {
                ;//qDebug("CalendarView::insertBirthdays found invalid anniversary: %s",anniversaryList[i].latin1());
            }
            realName = realNameList[i]; 
            email = emailList[i];
            assembledName = assembledNameList[i];
            uid = uidList[i];
            //qDebug("insert birthday in KO/Pi: %s,%s,%s,%s: %s, %s", realName.latin1(), email.latin1(), assembledName.latin1(), uid.latin1(), birthdayList[i].latin1(), anniversaryList[i].latin1() );

            if ( birthday.isValid() ){
                a = new KCal::Attendee( realName, email,false,KCal::Attendee::NeedsAction,
                                        KCal::Attendee::ReqParticipant,uid) ;
                if ( addAnniversary(  birthday, assembledName,  a, true ) )
                    ++addCount;
            } 

            if ( anniversary.isValid() ){ 
                a = new KCal::Attendee( realName, email,false,KCal::Attendee::NeedsAction,
                                        KCal::Attendee::ReqParticipant,uid) ;
                if ( addAnniversary(  anniversary, assembledName,  a, false ) )
                    ++addCount;
            }
        }

    mCalendar->setDefaultCalendar( curCal );
    updateView(); 
    topLevelWidget()->setCaption(QString::number( addCount )+ i18n(" birthdays/anniversaries added!"));

  }

}



bool  CalendarView::addAnniversary(  QDate date, QString name, KCal::Attendee* a,  bool birthday)
{
    //qDebug("addAnni ");
    Event * ev = new Event();
    ev->setOrganizer(KOPrefs::instance()->email());
    if ( a ) {
        ev->addAttendee( a );
    }
    QString kind;
    if ( birthday ) {
        kind = i18n( "Birthday" );
        ev->setSummary( name + " (" + QString::number(date.year()) +")"); 
    }
    else {
        kind = i18n( "Anniversary" );
        ev->setSummary( name + " (" + QString::number(date.year()) +") " + kind ); 
    }
    ev->setCategories( kind );
    ev->setDtStart( QDateTime(date) );
    ev->setDtEnd(  QDateTime(date) );
    ev->setFloats( true );
    Recurrence * rec = ev->recurrence();
    rec->setYearly(Recurrence::rYearlyMonth,1,-1);
    rec->addYearlyNum( date.month() );
    if ( !mCalendar->addAnniversaryNoDup( ev ) ) {
        delete ev;
        return false;
    }
    return true;

}
bool CalendarView::importQtopia( const QString &categories,
                                 const QString &datebook,
                                 const QString &todolist )
{
    
   QtopiaFormat qtopiaFormat;
    qtopiaFormat.setCategoriesList (  &(KOPrefs::instance()->mCustomCategories));
    if ( !categories.isEmpty() ) qtopiaFormat.load( mCalendar, categories );
    if ( !datebook.isEmpty() ) qtopiaFormat.load( mCalendar, datebook );
    if ( !todolist.isEmpty() ) qtopiaFormat.load( mCalendar, todolist );
   
    updateView();
    return true;

#if 0
 mGlobalSyncMode = SYNC_MODE_QTOPIA;
    mCurrentSyncDevice = "qtopia-XML";
    if ( mSyncManager->mAskForPreferences )
        edit_sync_options(); 
    qApp->processEvents();
    CalendarLocal* calendar = new CalendarLocal();
    calendar->setTimeZoneId(KPimGlobalPrefs::instance()->mTimeZoneId);
    bool syncOK = false;
 QtopiaFormat qtopiaFormat;
    qtopiaFormat.setCategoriesList (  &(KOPrefs::instance()->mCustomCategories));
    bool loadOk = true;
    if ( !categories.isEmpty() ) 
        loadOk = qtopiaFormat.load( calendar, categories );
    if ( loadOk && !datebook.isEmpty() ) 
        loadOk = qtopiaFormat.load( calendar, datebook );
    if ( loadOk && !todolist.isEmpty() ) 
        loadOk = qtopiaFormat.load( calendar, todolist );

    if ( loadOk  ) {
        getEventViewerDialog()->setSyncMode( true );
        syncOK = synchronizeCalendar( mCalendar, calendar, mSyncManager->mSyncAlgoPrefs );
        getEventViewerDialog()->setSyncMode( false );
        qApp->processEvents();
        if (  syncOK ) {
            if ( mSyncManager->mWriteBackFile )
            {
                // write back XML file

            }
        setModified( true );
        }
    } else {
        QString question = i18n("Sorry, the file loading\ncommand failed!\n\nNothing synced!\n") ;
        QMessageBox::information( 0, i18n("KO/Pi Sync - ERROR"),
                                      question, i18n("Ok")) ;
    }
    delete calendar;
    updateView();
    return syncOK;


#endif

}
    
void CalendarView::setSyncEventsReadOnly()
{
    mCalendar->setSyncEventsReadOnly(); 
}

bool CalendarView::loadCalendars()
{
    QPtrList<KopiCalendarFile> calendars = KOPrefs::instance()->mCalendars;
    KopiCalendarFile * cal = calendars.first();
    mCalendar->setDefaultCalendar( 1 );
    openCalendar( MainWindow::defaultFileName(), false );
    cal = calendars.next();
    while ( cal ) {
        addCalendar( cal );
        cal = calendars.next();
    }
    restoreCalendarSettings();
    return true;
}
bool CalendarView::restoreCalendarSettings()
{
    QPtrList<KopiCalendarFile> calendars = KOPrefs::instance()->mCalendars;
    KopiCalendarFile * cal = calendars.first();
    while ( cal ) {
        mCalendar->setCalendarEnabled( cal->mCalNumber,cal->isEnabled );
        mCalendar->setAlarmEnabled( cal->mCalNumber, cal->isAlarmEnabled );
        mCalendar->setReadOnly( cal->mCalNumber, cal->isReadOnly );
        if ( cal->isStandard )
            mCalendar->setDefaultCalendar( cal->mCalNumber );
        cal = calendars.next();
    }
    setSyncEventsReadOnly();
    mCalendar->reInitAlarmSettings();
    updateUnmanagedViews();
    updateView();
    return true;
}
void CalendarView::addCalendarId( int id )
{
    KopiCalendarFile * cal = KOPrefs::instance()->getCalendar( id );
    if ( cal )
        addCalendar( cal );    
}
bool CalendarView::addCalendar( KopiCalendarFile * cal )
{
    cal->mErrorOnLoad = false;
    if ( mCalendar->addCalendarFile( cal->mFileName, cal->mCalNumber )) {
        cal->mLoadDt = QDateTime::currentDateTime();
        return true;
    }
    qDebug("KO: Error adding calendar file %s ",cal->mFileName.latin1() );
    cal->mErrorOnLoad = true;
    return false;
}
bool CalendarView::openCalendar(QString filename, bool merge)
{

    if (filename.isEmpty()) {
        return false;
    }

    if (!QFile::exists(filename)) {
        KMessageBox::error(this,i18n("File does not exist:\n '%1'.").arg(filename));
        return false;
    }

    globalFlagBlockAgenda = 1;
    clearAllViews();
    if (!merge) {
      mViewManager->setDocumentId( filename );
      mCalendar->close();
    }
    mStorage->setFileName( filename );

    if ( mStorage->load() ) {
        if ( merge ) ;//setModified( true );
        else {
            //setModified( true );
            mViewManager->setDocumentId( filename );
            mDialogManager->setDocumentId( filename );
            mTodoList->setDocumentId( filename );
        }
        globalFlagBlockAgenda = 2;
        // if ( getLastSyncEvent() )
        //  getLastSyncEvent()->setReadOnly( true );
        mCalendar->reInitAlarmSettings();
        setSyncEventsReadOnly();
        //updateUnmanagedViews();
        //updateView();
        if (  filename != MainWindow::defaultFileName() ) {
            saveCalendar( MainWindow::defaultFileName() );
        } else {
            QFileInfo finf ( MainWindow::defaultFileName());   
            if ( finf.exists() ) {
                setLoadedFileVersion( finf.lastModified () );
            }
        }
        return true;
    } else {
        // while failing to load, the calendar object could
        // have become partially populated.  Clear it out.
        if ( !merge ) {
            mCalendar->close();
            mViewManager->setDocumentId( filename );
            mDialogManager->setDocumentId( filename );
            mTodoList->setDocumentId( filename );
        }

        //KMessageBox::error(this,i18n("Couldn't load calendar\n '%1'.").arg(filename));

        QTimer::singleShot ( 1, this, SLOT ( showOpenError() ) ); 
        globalFlagBlockAgenda = 2;
        mCalendar->reInitAlarmSettings();
        setSyncEventsReadOnly();
        updateUnmanagedViews();
        updateView();
    }
    return false;
}
void CalendarView::mergeFile( QString fn )
{
    clearAllViews();
    mCalendar->mergeCalendarFile( fn ); 
    mCalendar->reInitAlarmSettings();
    setSyncEventsReadOnly();
    updateUnmanagedViews();
    updateView();
}
void CalendarView::mergeFileResource( QString fn ,QString resource )
{

    if ( resource == "ALL" ) {
        mCalendar->setAllCalendarEnabled( true ); 
        mergeFile( fn );
        restoreCalendarSettings();
        return;
    }

    int exclusiveResource = KOPrefs::instance()->getFuzzyCalendarID( resource );
    if ( !exclusiveResource ) {
        qDebug("KO: CalendarView::mergeFileResource: resource not found %s", resource.latin1() );
        return;
    }
    clearAllViews();
    mCalendar->setCalendarRemove( exclusiveResource );
    mCalendar->setDefaultCalendar(exclusiveResource);
    mCalendar->setAllCalendarEnabled( true );
    if ( !mCalendar->addCalendarFile( fn, exclusiveResource )) {
        qDebug("KO: CalendarView::mergeFileResource: error adding file %s", fn.latin1() );
    }
    restoreCalendarSettings();
}
void CalendarView::showOpenError()
{
   KMessageBox::error(this,i18n("Couldn't load calendar\n."));
}
void CalendarView::setLoadedFileVersion(QDateTime dt)
{
    loadedFileVersion = dt;
}
bool CalendarView::checkFileChanged(QString fn)
{
    QFileInfo finf ( fn ); 
    if ( !finf.exists() )
        return true;
    QDateTime dt = finf.lastModified ();
    if ( dt <= loadedFileVersion )
        return false;
    return true;

}
void CalendarView::watchSavedFile()
{
  QFileInfo finf ( MainWindow::defaultFileName());   
  if ( !finf.exists() )
    return;
  QDateTime dt = finf.lastModified ();
  if ( dt < loadedFileVersion ) {
      //qDebug("watch %s %s ",  dt.toString().latin1(), loadedFileVersion.toString().latin1());
    QTimer::singleShot( 1000 , this, SLOT ( watchSavedFile()  ) );
    return;
  }
  loadedFileVersion = dt;
}
bool CalendarView::checkAllFileVersions()
{
    QPtrList<KopiCalendarFile> calendars = KOPrefs::instance()->mCalendars;
    KopiCalendarFile * cal = calendars.first();
    mCalendar->setDefaultCalendar( 1 ); 
    mCalendar->setDefaultCalendarEnabledOnly();
    if ( !cal->isReadOnly && !cal->mErrorOnLoad ) {
        if ( !checkFileVersion(MainWindow::defaultFileName())) {
            restoreCalendarSettings();
            return false;
        }
    }
    cal = calendars.next();
    QDateTime storeTemp = loadedFileVersion;
    while ( cal ) {
        if ( !cal->isReadOnly && !cal->mErrorOnLoad ) {
            mCalendar->setDefaultCalendar( cal->mCalNumber ); 
            mCalendar->setDefaultCalendarEnabledOnly();
            loadedFileVersion = cal->mLoadDt.addSecs( 15 );
            if ( !checkFileVersion(cal->mFileName )) {
                loadedFileVersion = storeTemp;
                restoreCalendarSettings();
                return false;
            }
        }
        cal = calendars.next();
    }
    loadedFileVersion = storeTemp;
    return true;
}
bool CalendarView::checkFileVersion(QString fn)
{
    QFileInfo finf ( fn );  
    if ( !finf.exists() )
        return true;
    QDateTime dt = finf.lastModified ();
    //qDebug("loaded file version %s %s", fn.latin1(), loadedFileVersion.toString().latin1());
    //qDebug("file on disk version  %s %s", fn.latin1(),dt.toString().latin1());
    if ( dt <= loadedFileVersion )
        return true;
    int km = KMessageBox::warningYesNoCancel(this, i18n("\nThe file\n%1\non disk has changed!\nFile size: %2 bytes.\nLast modified: %3\nDo you want to:\n\n - Save and overwrite file?\n - Sync with file, then save?\n - Cancel without saving? \n").arg(KGlobal::formatMessage(fn,0)).arg( QString::number( finf.size())).arg( KGlobal::locale()->formatDateTime(finf.lastModified (), true, true)) ,
                                                 i18n("KO/Pi Warning"),i18n("Overwrite"),
                                                 i18n("Sync+save"));
   
   if ( km ==  KMessageBox::Cancel )
       return false;
   if ( km ==  KMessageBox::Yes )
       return true;

    setSyncDevice("deleteaftersync" );
    mSyncManager->mAskForPreferences = true;
    mSyncManager->mSyncAlgoPrefs = 3;
    mSyncManager->mWriteBackFile = false;
    mSyncManager->mWriteBackExistingOnly = false;
    mSyncManager->mShowSyncSummary = false;
    mMultiResourceSync = false;
    syncCalendar( fn, 3 );
    Event * e = getLastSyncEvent();
    if ( e )
        mCalendar->deleteEvent( e );
    return true;
}
bool CalendarView::saveCalendars()
{
    QPtrList<KopiCalendarFile> calendars = KOPrefs::instance()->mCalendars;
    KopiCalendarFile * cal = calendars.first();
    mCalendar->setDefaultCalendar( 1 ); 
    mCalendar->setDefaultCalendarEnabledOnly();
    QString saveError;
    if ( !saveCalendar( MainWindow::defaultFileName() ) )
        saveError =  cal->mName +"\n";
    cal = calendars.next();
    while ( cal ) {
        if ( !cal->isReadOnly && !cal->mErrorOnLoad ) {
            mCalendar->setDefaultCalendar( cal->mCalNumber ); 
            mCalendar->setDefaultCalendarEnabledOnly();
            if ( saveCalendar( cal->mFileName ) )
                cal->mLoadDt = QDateTime::currentDateTime();
            else
                saveError += cal->mName + "\n";
        }
        cal = calendars.next();
    }
    restoreCalendarSettings();
    //saveError = "test error";
    if ( !saveError.isEmpty() ) {
        saveError = KGlobal::formatMessage( i18n("Calendar(s) not saved:"),0 )+"\n" + saveError;
        KMessageBox::error(this, saveError, i18n("Error saving data"));
        return false;
    }
    return true;
}
bool CalendarView::saveCalendarResource(QString filename, QString resource)
{
    if ( resource == "ALL" ) {
        mCalendar->setAllCalendarEnabled( true ); 
        bool retval =  saveCalendar( filename );
        restoreCalendarSettings();
        return retval;
    }
    int exclusiveResource = KOPrefs::instance()->getFuzzyCalendarID( resource );
    if ( !exclusiveResource ) {
        qDebug("KO: CalendarView::saveCalendarResource: resource not found %s", resource.latin1() );
        return false;
    }
    mCalendar->setDefaultCalendar( exclusiveResource ); 
    mCalendar->setDefaultCalendarEnabledOnly(); 
    mCalendar->setSyncEventsEnabled();
    bool res = saveCalendar( filename );
    restoreCalendarSettings();
    return res;
    
}
bool CalendarView::saveCalendar( QString filename )
{
 
    // Store back all unsaved data into calendar object
    // qDebug("file %s %d ",  filename.latin1()           , mViewManager->currentView()         );
    if ( mViewManager->currentView() )
        mViewManager->currentView()->flushView();

    
    QDateTime lfv = QDateTime::currentDateTime().addSecs( -2);
    mStorage->setSaveFormat( new ICalFormat() );
    mStorage->setFileName( filename );
    bool success;
    success = mStorage->save();
    if ( !success ) {
        return false;
    }
    if ( filename == MainWindow::defaultFileName() ) {
        setLoadedFileVersion( lfv );
        watchSavedFile();
    }
    return true;
}

void CalendarView::closeCalendar()
{
 
    // child windows no longer valid
    clearAllViews();
    emit closingDown();

    mCalendar->close();
    setModified(false);
    updateView();
}

void CalendarView::archiveCalendar()
{
    mDialogManager->showArchiveDialog();
}


void CalendarView::readSettings()
{
 
  
    // mViewManager->showAgendaView();
    QString str;
    //qDebug("CalendarView::readSettings() ");
    // read settings from the KConfig, supplying reasonable
    // defaults where none are to be found
    KConfig *config = KOGlobals::config();
#ifndef KORG_NOSPLITTER
    config->setGroup("KOrganizer Geometry");

    QValueList<int> sizes = config->readIntListEntry("Separator1");
    if (sizes.count() != 2) {
        sizes << mDateNavigator->minimumSizeHint().width();
        sizes << 300;
    }
    mPanner->setSizes(sizes);

    sizes = config->readIntListEntry("Separator2");
    if ( ( mResourceView && sizes.count() == 4 ) ||
         ( !mResourceView && sizes.count() == 3 ) ) {
        mLeftSplitter->setSizes(sizes);
    }
#endif
    globalFlagBlockAgenda = 1;
    mViewManager->showAgendaView();
    //mViewManager->readSettings( config );
    mTodoList->restoreLayout(config,QString("Todo Layout"));
    readFilterSettings(config);
   
#ifdef DESKTOP_VERSION
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
    list = config->readListEntry("EditEventLayout");
    if ( ! list.isEmpty() ) {
        x = list[0].toInt();
        y = list[1].toInt();
        w = list[2].toInt();
        h = list[3].toInt();
        KApplication::testCoords( &x,&y,&w,&h );
        mEventEditor->setGeometry(x,y,w,h);

    }
    list = config->readListEntry("EditTodoLayout");
    if ( ! list.isEmpty() ) {
        x = list[0].toInt();
        y = list[1].toInt();
        w = list[2].toInt();
        h = list[3].toInt();
        KApplication::testCoords( &x,&y,&w,&h );
        mTodoEditor->setGeometry(x,y,w,h);

    }
    list = config->readListEntry("ViewerLayout");
    if ( ! list.isEmpty() ) {
        x = list[0].toInt();
        y = list[1].toInt();
        w = list[2].toInt();
        h = list[3].toInt();
        KApplication::testCoords( &x,&y,&w,&h );
        getEventViewerDialog()->setGeometry(x,y,w,h);
    }
#endif
    config->setGroup( "Views" );
    int dateCount = config->readNumEntry( "ShownDatesCount", 7 );

    QValueList<int> sizes = config->readIntListEntry("Left Splitter Frame");
   
    int resetval = 0;
    int maxVal = 0;
    if (sizes.count() != 3) { 
        if ( KOPrefs::instance()->mVerticalScreen ) {
            resetval = mDateNavigator->sizeHint().width()+2;
        } else {
            resetval = mDateNavigator->sizeHint().height()+2;
        }
    }
    if ( resetval ) {
        sizes.clear();
        if ( KOPrefs::instance()->mVerticalScreen ) {
            maxVal = QApplication::desktop()->width() -10;
        } else {
            maxVal = QApplication::desktop()->height()-10;
        }
        sizes << resetval;
        if ( maxVal < resetval + resetval)
            resetval = maxVal - resetval;
        sizes << resetval;
        sizes << 100;
    }
    mLeftFrame->setSizes(sizes);
    sizes = config->readIntListEntry("Main Splitter Frame");
    resetval = 0;
    maxVal = 0;
    if (sizes.count() != 2) { 
        if ( !KOPrefs::instance()->mVerticalScreen ) {
            resetval = mDateNavigator->sizeHint().width()+2;
        } else {
            resetval = mDateNavigator->sizeHint().height()+2;
        }
    }
    if ( resetval ) {
        sizes.clear();
        if ( !KOPrefs::instance()->mVerticalScreen ) {
            maxVal = QApplication::desktop()->width() -10;
        } else {
            maxVal = QApplication::desktop()->height()-10;
        }
        sizes << resetval;
        if ( maxVal < resetval + resetval)
            resetval = maxVal - resetval;
        sizes << resetval;
    }
    mMainFrame->setSizes(sizes);
    if ( dateCount == 5 ) mNavigator->selectWorkWeek();
    else if ( dateCount == 7 ) mNavigator->selectWeek();
    else mNavigator->selectDates( dateCount );
    // mViewManager->readSettings( config );
    updateConfig(); 
    globalFlagBlockAgenda = 2;
    mViewManager->readSettings( config );
    QTimer::singleShot( 1, mDateNavigator, SLOT ( setResizeEnabled() ) );
}

void CalendarView::checkSuspendAlarm()
{
    if ( mSuspendTimer->isActive() ) {
        KMessageBox::information( this, i18n("<b>WARNING:</b> There is a pending suspended alarm!"), i18n("Pending Suspend Alarm"));
    }
}
void CalendarView::writeSettings()
{
    //  kdDebug() << "CalendarView::writeSettings" << endl;
   
    KConfig *config = KOGlobals::config();

    mViewManager->writeSettings( config );
    mTodoList->saveLayout(config,QString("Todo Layout"));
    mDialogManager->writeSettings( config );
    //KOPrefs::instance()->usrWriteConfig();
    KOPrefs::instance()->writeConfig();

    writeFilterSettings(config);
    config->setGroup( "AppRun" );
    QDateTime dt ( QDate (2005,1,1), QTime( 0,0,0 ) );
    int days = dt.daysTo( QDate::currentDate() );
    dt =  dt.addDays( days );
    int secs = dt.secsTo( QDateTime::currentDateTime() );
    config->writeEntry( "LatestProgramStopDays", days );
    config->writeEntry( "LatestProgramStopSecs", secs );
    //qDebug("KO: Writing stop time: %d ", secs);
    //qDebug("KO: Current Time %s ",QDateTime::currentDateTime().toString().latin1() );
    //QDateTime latest = dt.addSecs ( secs );
    //qDebug("KO: Termination on %s ", latest.toString().latin1());
    config->setGroup( "Views" );
    config->writeEntry( "ShownDatesCount", (int)mNavigator->selectedDates().count() );

#if 0
    qDebug("********************* ");
    qDebug("Testcode secsto ");
    QDateTime dt_nodaylight ( QDate (2005,3,26), QTime( 0,0,0 ) );
    QDateTime dt_daylight ( QDate (2005,3,29), QTime( 0,0,0 ) );
    int secsto = dt_nodaylight.secsTo( dt_daylight );
    QDateTime dt_daylight_wrong = dt_nodaylight.addSecs( secsto );
    qDebug("dt nodaylight %s ",dt_nodaylight.toString().latin1() );
    qDebug("dt daylight %s ",dt_daylight.toString().latin1() );
    qDebug("dt daylight_wrong %s ",dt_daylight_wrong.toString().latin1() );
    qDebug("Computed secsTo %d . in minutes: %d . in hours: %d ", secsto, secsto/60, secsto/3600);
    qDebug("********************* testcode end");

#endif

    QValueList<int> listINT = mLeftFrame->sizes();
    config->writeEntry("Left Splitter Frame",listINT);
    QValueList<int> listINT2 = mMainFrame->sizes();
    config->writeEntry("Main Splitter Frame",listINT2);
#ifdef DESKTOP_VERSION
    config->setGroup("WidgetLayout");
    QStringList list ;//= config->readListEntry("MainLayout");
    int x,y,w,h;
    QWidget* wid;
    wid = topLevelWidget();
    x = wid->geometry().x();
    y = wid->geometry().y();
    w = wid->width();
    h = wid->height();
    list.clear();
    list << QString::number( x );
    list << QString::number( y );
    list << QString::number( w );
    list << QString::number( h );
    config->writeEntry("MainLayout",list );
    
    wid = mEventEditor;
    x = wid->geometry().x();
    y = wid->geometry().y();
    w = wid->width();
    h = wid->height();
    list.clear();
    list << QString::number( x );
    list << QString::number( y );
    list << QString::number( w );
    list << QString::number( h );
    config->writeEntry("EditEventLayout",list );
 
    wid = mTodoEditor;
    x = wid->geometry().x();
    y = wid->geometry().y();
    w = wid->width();
    h = wid->height();
    list.clear();
    list << QString::number( x );
    list << QString::number( y );
    list << QString::number( w );
    list << QString::number( h );
    config->writeEntry("EditTodoLayout",list ); 
    wid = getEventViewerDialog();
    x = wid->geometry().x();
    y = wid->geometry().y();
    w = wid->width();
    h = wid->height();
    list.clear();
    list << QString::number( x );
    list << QString::number( y );
    list << QString::number( w );
    list << QString::number( h );
    config->writeEntry("ViewerLayout",list ); 
    wid = mDialogManager->getSearchDialog();
    if ( wid ) {
        x = wid->geometry().x();
        y = wid->geometry().y();
        w = wid->width();
        h = wid->height();
        list.clear();
        list << QString::number( x );
        list << QString::number( y );
        list << QString::number( w );
        list << QString::number( h );
        config->writeEntry("SearchLayout",list );
    }
#endif


    config->sync();
}

void CalendarView::readFilterSettings(KConfig *config)
{
    //  kdDebug() << "CalendarView::readFilterSettings()" << endl;

    mFilters.clear();

    config->setGroup("General");
    QStringList filterList = config->readListEntry("CalendarFilters");

    QStringList::ConstIterator it = filterList.begin();
    QStringList::ConstIterator end = filterList.end();
    while(it != end) {
        //    kdDebug() << "  filter: " << (*it) << endl;

        CalFilter *filter;
        filter = new CalFilter(*it);
        config->setGroup("Filter_" + (*it).utf8());
        //qDebug("readFilterSettings %d ",config->readNumEntry("Criteria",0) );
        filter->setCriteria(config->readNumEntry("Criteria",0));
        filter->setCategoryList(config->readListEntry("CategoryList"));
        mFilters.append(filter);

        ++it;
    }

    if (mFilters.count() == 0) {
        CalFilter *filter = new CalFilter(i18n("Default"));
        mFilters.append(filter);
    }
    mFilterView->updateFilters();
    config->setGroup("FilterView");

    mFilterView->blockSignals(true);
    mFilterView->setFiltersEnabled(config->readBoolEntry("FilterEnabled"));
    mFilterView->setSelectedFilter(config->readEntry("Current Filter"));
    mFilterView->blockSignals(false);
    // We do it manually to avoid it being done twice by the above calls
    updateFilter();
}

void CalendarView::writeFilterSettings(KConfig *config)
{
    //  kdDebug() << "CalendarView::writeFilterSettings()" << endl;

    QStringList filterList;

    CalFilter *filter = mFilters.first();
    while(filter) {
        //    kdDebug() << " fn: " << filter->name() << endl;
        filterList << filter->name();
        config->setGroup("Filter_" + filter->name().utf8());
        config->writeEntry("Criteria",filter->criteria());
        config->writeEntry("CategoryList",filter->categoryList());
        filter = mFilters.next();
    }
    config->setGroup("General");
    config->writeEntry("CalendarFilters",filterList);

    config->setGroup("FilterView");
    config->writeEntry("FilterEnabled",mFilterView->filtersEnabled());
    config->writeEntry("Current Filter",mFilterView->selectedFilter()->name());
}


void CalendarView::goToday()
{
    if ( mViewManager->currentView()->isMonthView() )
        mNavigator->selectTodayMonth();
    else
        mNavigator->selectToday();
}

void CalendarView::goNext()
{
    mNavigator->selectNext();
}

void CalendarView::goPrevious()
{
    mNavigator->selectPrevious();
}
void CalendarView::goNextMonth()
{
    mNavigator->selectNextMonth();
}

void CalendarView::goPreviousMonth()
{
    mNavigator->selectPreviousMonth();
}

void CalendarView::updateConfig()
{
    if ( KOPrefs::instance()->mUseAppColors )
        QApplication::setPalette( QPalette (KOPrefs::instance()->mAppColor1, KOPrefs::instance()->mAppColor2), true );
    emit configChanged();
    mTodoList->updateConfig();
    // mDateNavigator->setFont ( KOPrefs::instance()->mDateNavigatorFont);
    mCalendar->setTimeZoneId(KPimGlobalPrefs::instance()->mTimeZoneId);
    // To make the "fill window" configurations work
    //mViewManager->raiseCurrentView();
}


void CalendarView::eventChanged(Event *event)
{
    changeEventDisplay(event,KOGlobals::EVENTEDITED);
    //updateUnmanagedViews();
}

void CalendarView::eventAdded(Event *event)
{
    changeEventDisplay(event,KOGlobals::EVENTADDED);
}

void CalendarView::eventToBeDeleted(Event *)
{
    kdDebug() << "CalendarView::eventToBeDeleted(): to be implemented" << endl;
}

void CalendarView::eventDeleted()
{
    changeEventDisplay(0,KOGlobals::EVENTDELETED);
}
void CalendarView::changeTodoDisplay(Todo *which, int action)
{
    changeIncidenceDisplay((Incidence *)which,  action);
    mDateNavigator->updateView(); //LR
    //mDialogManager->updateSearchDialog();

    if (which) {
        mViewManager->updateWNview();
        //mTodoList->updateView();
    }
   
}

void CalendarView::changeIncidenceDisplay(Incidence *which, int action)
{
    updateUnmanagedViews();
    //qDebug(" CalendarView::changeIncidenceDisplay++++++++++++++++++++++++++  %d  %d ",which, action );
    if ( action == KOGlobals::EVENTDELETED ) { //delete
        mCalendar->checkAlarmForIncidence( 0, true );
        if ( mEventViewerDialog )
            mEventViewerDialog->hide();
    } 
    else
        mCalendar->checkAlarmForIncidence( which , false );
}

// most of the changeEventDisplays() right now just call the view's
// total update mode, but they SHOULD be recoded to be more refresh-efficient.
void CalendarView::changeEventDisplay(Event *which, int action)
{
    //  kdDebug() << "CalendarView::changeEventDisplay" << endl;
    changeIncidenceDisplay((Incidence *)which,  action);
    if ( KOPrefs::instance()->mGlobalUpdateDisabled ) {
        if ( flag_clearallviewsEventDisplay ) {
            clearAllViews();
            flag_clearallviewsEventDisplay = false;
        }
        return;
    }
    flag_clearallviewsEventDisplay = true;
    mDateNavigator->updateView();
    //mDialogManager->updateSearchDialog();
    if (which) {
        // If there is an event view visible update the display
        mViewManager->currentView()->changeEventDisplay(which,action);
        // TODO: check, if update needed
        //    if (which->getTodoStatus()) {
        mTodoList->updateView();
        if (  action != KOGlobals::EVENTDELETED && KOPrefs::instance()->mDetectConflicts ) {
            mConflictingEvent = which ;
            int time = 1000;
#ifdef DESKTOP_VERSION
            time = 500;
#endif
            bool checkC = false;
            if ( mConflictingEvent->doesFloat() ) {
                checkC = KOPrefs::instance()->mCheckConflictsAllDayAllDay 
                    || KOPrefs::instance()->mCheckConflictsAllDayNonAD;
            } else {
                checkC = KOPrefs::instance()->mCheckConflictsNonADAllDay  
                    || KOPrefs::instance()->mCheckConflictsNonADNonAD;
            }
            if ( !KOPrefs::instance()->mIncludeFree && mConflictingEvent->transparency() == KCal::Event::Transparent )
                checkC = false;
            if ( checkC )
                QTimer::singleShot( time, this, SLOT ( checkConflictForEvent() ) );
        }
        //    }
    } else {
        mViewManager->currentView()->updateView();
    }
}
void CalendarView::checkConflictForEvent()
{

    if (!KOPrefs::instance()->mConfirm)
        return;
    if ( ! mConflictingEvent ) return;
    Event * conflictingEvent = mConflictingEvent;
    mConflictingEvent = 0;
    QDateTime current = QDateTime::currentDateTime();
    if ( ! conflictingEvent->matchTime( &current, 0 ) ) {
        return;
    }
    CalFilter *filterIN = 0;
    CalFilter *filterALL = 0;
    CalFilter *filter = mFilters.first();
    while(filter) {
        if ( filter->name() == KOPrefs::instance()->mFilterConflictEditItem)
            filterIN = filter;
        if ( filter->name() == KOPrefs::instance()->mFilterConflictAllItem )
            filterALL = filter;
        filter = mFilters.next();
    }
    if ( filterIN ) {
        if ( !filterIN->filterCalendarItem( conflictingEvent ) ) {
            return;   
        }
    }
    QPtrList<Event> testlist = mCalendar->events();
    Event * test = testlist.first();
    QDateTime conflict;
    QDateTime retVal;
    bool found = false;
    Event * cE = 0;
    bool chAD = KOPrefs::instance()->mCheckConflictsAllDayAllDay;
    bool chNad = KOPrefs::instance()->mCheckConflictsAllDayNonAD;
    if ( !conflictingEvent->doesFloat() ) {
        chAD = KOPrefs::instance()->mCheckConflictsNonADAllDay;
        chNad = KOPrefs::instance()->mCheckConflictsNonADNonAD;
    }
    topLevelWidget()->setCaption( i18n("Checking conflicts ... please wait") );
    while ( test ) {
        qApp->processEvents(); 
        bool skip = ( test->doesFloat() && !chAD ) || ( !test->doesFloat() && !chNad );

        if ( !skip ) {
            if ( !KOPrefs::instance()->mIncludeFree && test->transparency() == KCal::Event::Transparent )
                skip = true;
        }
        if ( !skip ) {
            if ( filterALL ) {
                if ( !filterALL->filterCalendarItem( test ) ) {
                    skip = true;
                }
            } 
            if ( !skip ) {
                if ( found )
                    skip =  !test->matchTime( &current, &conflict );
                else 
                    skip =  !test->matchTime( &current, 0 ); 
                if ( !skip ) {
                    if ( conflictingEvent->isOverlapping ( test, &retVal, &current ) ) {
                        if ( ! found ) {
                            conflict = retVal; 
                            cE = test;
                        } else {
                            if ( retVal < conflict ) {
                                conflict = retVal; 
                                cE = test;
                            }
                        }
                        found = true;
                    }
                }
            }
        }
        test = testlist.next();
    }
    topLevelWidget()->setCaption( i18n("KO/Pi") );
    if ( found ) {
        QString mess = i18n("The event\n%1\nconflicts with event\n%2\nat date\n%3.\n").arg(KGlobal::formatMessage ( conflictingEvent->summary(),0 ) ).arg( KGlobal::formatMessage ( cE->summary(),0 )).arg(KGlobal::locale()->formatDate(conflict.date()) ) ;
        qApp->processEvents();
        int km = KMessageBox::warningContinueCancel(this,mess,
                                                    i18n("KO/Pi Conflict detected"),i18n("Show date"),i18n("No problem!"));
        if ( km != KMessageBox::Continue ) {
            return;
        }
        if ( mViewManager->currentView() != mViewManager->agendaView() || mNavigator->selectedDates().count() > 1 )
            mViewManager->showDayView();
        mNavigator->slotDaySelect( conflict.date() );
        int hour = conflict.time().hour();
        mViewManager->agendaView()->setStartHour( hour  );
        topLevelWidget()->setCaption( i18n("Conflict %1 <-> %2"). arg( conflictingEvent->summary().left( 20 ) ).arg( cE->summary().left( 20 ) ) );
    } else
        topLevelWidget()->setCaption( i18n("No conflict found") );
    return;
   
}

void CalendarView::updateTodoViews()
{
    mTodoList->updateView();
    mViewManager->currentView()->updateView();
 
}



void CalendarView::clearAllViews()
{
    mTodoList->clearList();
    mViewManager->clearAllViews();
    SearchDialog * sd = mDialogManager->getSearchDialog();
    if ( sd ) {
        KOListView* kol = sd->listview();
        if ( kol )
            kol->clearList();
    }
}
void CalendarView::updateView()
{
    if ( KOPrefs::instance()->mGlobalUpdateDisabled ) {
        if ( flag_clearallviewsupdateView ) {
            clearAllViews();
            flag_clearallviewsupdateView = false;
        }
        return;
    }
    flag_clearallviewsupdateView = true;
    DateList tmpList = mNavigator->selectedDates();

    if ( KOPrefs::instance()->mHideNonStartedTodos ) 
        mTodoList->updateView();
    // We assume that the navigator only selects consecutive days.
    updateView( tmpList.first(), tmpList.last() );
}

void CalendarView::updateUnmanagedViews()
{
    mDateNavigator->updateDayMatrix();
}

int CalendarView::msgItemDelete(const QString name)
{
    return KMessageBox::warningContinueCancel(this,name +"\n\n"+
                                              i18n("This item will be\npermanently deleted."),
                                              i18n("KO/Pi Confirmation"),i18n("Delete"));
}


void CalendarView::edit_cut()
{
    Event *anEvent=0;

    Incidence *incidence = mViewManager->currentView()->selectedIncidences().first();

    if (mViewManager->currentView()->isEventView()) {
        if ( incidence && incidence->typeID() == eventID ) {
            anEvent = static_cast<Event *>(incidence);
        }
    }

    if (!anEvent) {
        KNotifyClient::beep();
        return;
    }
    DndFactory factory( mCalendar );
    factory.cutIncidence(anEvent);
    changeEventDisplay(anEvent, KOGlobals::EVENTDELETED);
}

void CalendarView::edit_copy()
{
    Event *anEvent=0;

    Incidence *incidence = mViewManager->currentView()->selectedIncidences().first();

    if (mViewManager->currentView()->isEventView()) {
        if ( incidence && incidence->typeID() == eventID ) {
            anEvent = static_cast<Event *>(incidence);
        }
    }

    if (!anEvent) {
        KNotifyClient::beep();
        return;
    }
    DndFactory factory( mCalendar );
    factory.copyIncidence(anEvent);
}

void CalendarView::edit_paste()
{
    QDate date = mNavigator->selectedDates().first();

    DndFactory factory( mCalendar );
    Event *pastedEvent = (Event *)factory.pasteIncidence( date );

    changeEventDisplay( pastedEvent, KOGlobals::EVENTADDED );
}
void CalendarView::edit_global_options()
{
    QString tz = KPimGlobalPrefs::instance()->mTimeZoneId;
    emit save();
    emit saveStopTimer();
    mDialogManager->showGlobalOptionsDialog();
    if ( tz != KPimGlobalPrefs::instance()->mTimeZoneId) {
        emit saveStopTimer();
        if ( KMessageBox::Cancel == KMessageBox::warningContinueCancel(this, i18n("The timezone has changed!\nShould the calendar be reloaded\nto shift the time of the events?\nPlease read Menu: Help->FAQ:\n\"How do I change the timezone?\"\nas well!"),
                                                 i18n("Timezone settings"),i18n("Reload"))) {
            qDebug("KO: TZ reload cancelled ");
            mCalendar->setTimeZoneId(KPimGlobalPrefs::instance()->mTimeZoneId);
            return;
        }
        qDebug("KO: Timezone change ");
        loadCalendars();
        setModified(true);
    }
    else
        qDebug("KO: No tz change ");
}
void CalendarView::edit_options()
{
    mDialogManager->showOptionsDialog();
}


void  CalendarView::slotSelectPickerDate( QDate  d)
{
    mDateFrame->hide();
    if ( mDatePickerMode == 1 ) {
        mNavigator->slotDaySelect( d );
    } else  if ( mDatePickerMode == 2 ) {
        if ( mMoveIncidence->typeID() == todoID ) {
            Todo * to = (Todo *) mMoveIncidence;
            QTime tim;
            int len  = 0;
            if ( to->hasStartDate() &&  to->hasDueDate() )
                len = to->dtStart().secsTo( to->dtDue());
            if ( to->hasDueDate() ) 
                tim = to->dtDue().time();
            else {
                tim = QTime ( 0,0,0 );
                to->setFloats( true );
                to->setHasDueDate( true );
            }
            QDateTime dt ( d,tim );
            to->setDtDue( dt );

            if ( to->hasStartDate() ) {
                if ( len>0 ) 
                    to->setDtStart(to->dtDue().addSecs( -len ));
                else
                    if (to->dtStart() > to->dtDue() )
                        to->setDtStart(to->dtDue().addDays( -3 ));
            }
            
            todoChanged( to );
        } else if ( mMoveIncidence->typeID() == eventID ) { 
            if ( mMoveIncidence->doesRecur() ) {
#if 0 
                // PENDING implement this
                Incidence* newInc = mMoveIncidence->recreateCloneException( mMoveIncidenceOldDate );
                mCalendar()->addIncidence( newInc ); 
                if ( mMoveIncidence->typeID() == todoID )
                    emit  todoMoved((Todo*)mMoveIncidence,  KOGlobals::EVENTEDITED );
                else
                    emit incidenceChanged(mMoveIncidence, KOGlobals::EVENTEDITED);
                mMoveIncidence = newInc;
            
#endif 
            }
            QTime tim = mMoveIncidence->dtStart().time(); 
            int secs = mMoveIncidence->dtStart().secsTo(  mMoveIncidence->dtEnd());
            QDateTime dt ( d,tim );
            mMoveIncidence->setDtStart( dt );
            ((Event*)mMoveIncidence)->setDtEnd( dt.addSecs( secs ) );
            changeEventDisplay((Event*)mMoveIncidence, KOGlobals::EVENTEDITED);
        } else if ( mMoveIncidence->typeID() == journalID ) { 
            QTime tim = mMoveIncidence->dtStart().time(); 
            QDateTime dt ( d,tim );
            mMoveIncidence->setDtStart( dt );
            updateView();
        }
        mMoveIncidence->setRevision( mMoveIncidence->revision()+1  );
    }
}

void CalendarView::removeCategories()
{
    QPtrList<Incidence> incList  = mCalendar->rawIncidences(); 
    QStringList catList = KOPrefs::instance()->mCustomCategories;
    QStringList catIncList;
    QStringList newCatList;
    Incidence* inc = incList.first();
    uint i;
    while ( inc ) {
        newCatList.clear();
        catIncList =  inc->categories() ;
        for( i = 0; i<   catIncList.count(); ++i ) {
            if ( catList.contains (catIncList[i]))
                newCatList.append( catIncList[i] );
        }
        newCatList.sort();
        inc->setCategories( newCatList.join(",") );
        inc = incList.next();
    }
}

int CalendarView::addCategories()
{ 
    QPtrList<Incidence> incList  = mCalendar->rawIncidences(); 
    QStringList catList = KOPrefs::instance()->mCustomCategories;
    QStringList catIncList;
    Incidence* inc = incList.first();
    uint i;
    int count = 0;
     while ( inc ) {
        catIncList = inc->categories() ;
        for( i = 0; i<   catIncList.count(); ++i ) {
            if ( !catList.contains (catIncList[i])) {
                catList.append( catIncList[i] );
                //qDebug("add cat %s ",  catIncList[i].latin1());
                ++count;
            }
        }
        inc = incList.next();
    }
    catList.sort();
    KOPrefs::instance()->mCustomCategories = catList;
    return count;
}

void CalendarView::editCategories()
{
    qDebug("CalendarView::editCategories() ");
    KPIM::CategoryEditDialog ced (KOPrefs::instance(),this );
    ced.exec(); 
}
void CalendarView::manageCategories()
{
    KOCatPrefs* cp = new KOCatPrefs(); 
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
    if ( cp->addCat() ) {
        count = addCategories();
        if ( count ) {
            topLevelWidget()->setCaption(QString::number( count )+ i18n(" Categories added to list! "));
            writeSettings();
        } else
            topLevelWidget()->setCaption(QString::number( 0 )+ i18n(" Categories added to list! "));
    } else {
        removeCategories();
        updateView();
    }
    delete cp;
}

void CalendarView::beamIncidence(Incidence * Inc) 
{
    QPtrList<Incidence> delSel ;
    delSel.append(Inc);
    beamIncidenceList( delSel );
} 
void CalendarView::beamCalendar() 
{
    QPtrList<Incidence> delSel  = mCalendar->rawIncidences(); 
    //qDebug("beamCalendar()  ");
    beamIncidenceList( delSel );
}
void CalendarView::beamFilteredCalendar() 
{
    QPtrList<Incidence> delSel  = mCalendar->incidences(); 
    //qDebug("beamFilteredCalendar()  ");
    beamIncidenceList( delSel );
}
void  CalendarView::beamIncidenceList(QPtrList<Incidence> delSel )
{
    
    KOBeamPrefs beamDialog;
    if ( beamDialog.exec () == QDialog::Rejected ) 
        return;
#ifdef DESKTOP_VERSION
    QString fn =  locateLocal( "tmp", "kopibeamfile" );
#else
    QString fn = "/tmp/kopibeamfile";
#endif
    QString mes;
    bool createbup = true;
    if ( createbup ) {
        QString description = "\n";
        CalendarLocal* cal = new CalendarLocal();
        if ( beamDialog.beamLocal() )
            cal->setLocalTime();
        else
            cal->setTimeZoneId(KPimGlobalPrefs::instance()->mTimeZoneId);
        Incidence *incidence = delSel.first();
        bool addText = false;
        if ( delSel.count() < 10 )
            addText = true;
        else {
            description.sprintf(i18n("  %d items?"),delSel.count() );
        }
        while ( incidence ) {
            Incidence *in = incidence->clone();
            if ( ! in->summary().isEmpty() ) {
                in->setDescription("");
            } else {
                in->setSummary( in->description().left(20));
                in->setDescription("");
            }
            if ( addText )
                description += in->summary() + "\n";
            cal->addIncidence( in );
            incidence = delSel.next();
        }
        if ( beamDialog.beamVcal() ) {
            fn += ".vcs";
            FileStorage storage( cal, fn, new VCalFormat );
            storage.save();
        } else {
            fn += ".ics";
            FileStorage storage( cal, fn, new ICalFormat( ) );
            storage.save();
        }
        delete cal;
        mes = i18n("KO/Pi: Ready for beaming");
        topLevelWidget()->setCaption(mes); 
        KApplication::convert2latin1( fn );
#ifndef DESKTOP_VERSION
        Ir *ir = new Ir( this ); 
        connect( ir, SIGNAL( done( Ir * ) ), this, SLOT( beamDone( Ir * ) ) );
        ir->send( fn, description, "text/x-vCalendar" );
#endif
    }
}

#ifndef DESKTOP_VERSION
void  CalendarView::beamDone( Ir *ir )
{
    delete ir;
    topLevelWidget()->setCaption( i18n("KO/Pi: Beaming done.") ); 
    topLevelWidget()->raise();
}
#else 
void  CalendarView::beamDone( Ir *){;}
#endif
void CalendarView::moveIncidence(Incidence * inc ) 
{
    if ( !inc ) return;
    showDatePickerPopup(); 
    mDatePickerMode = 2;
    mMoveIncidence = inc ;
    QDate da;
    if ( mMoveIncidence->typeID() == todoID ) {
        Todo * to = (Todo *) mMoveIncidence;
            if ( to->hasDueDate() ) 
                da = to->dtDue().date();
            else
                da = QDate::currentDate();
    } else {
        da = mMoveIncidence->dtStart().date();
    }
    //PENDING set date for recurring incidence to date of recurrence
    //mMoveIncidenceOldDate;
    mDatePicker->setDate( da );
}
void CalendarView::showDatePickerPopup() 
{
 if ( mDateFrame->isVisible() )
        mDateFrame->hide();
    else {
        int offX = 0, offY = 0;
#ifdef DESKTOP_VERSION
        int w =mDatePicker->sizeHint().width() ;
        int h = mDatePicker->sizeHint().height() ;
        int dw = topLevelWidget()->width();
        int dh = topLevelWidget()->height();
        offX = topLevelWidget()->x();
        offY = topLevelWidget()->y();
#else
        int w =mDatePicker->sizeHint().width() ;
        int h = mDatePicker->sizeHint().height() ;
        int dw = QApplication::desktop()->width();
        int dh = QApplication::desktop()->height();
#endif
        mDateFrame->setGeometry( (dw-w)/2+offX, (dh - h )/2+offY ,w,h );
        mDateFrame->show();
    }
}
void CalendarView::showDatePicker( ) 
{
    showDatePickerPopup(); 
    mDatePickerMode = 1;
    mDatePicker->setDate( mNavigator->selectedDates().first() );
}

void CalendarView::showEventEditor()
{
#ifdef DESKTOP_VERSION
    int x,y,w,h;
    x = mEventEditor->geometry().x();
    y = mEventEditor->geometry().y();
    w = mEventEditor->width();
    h = mEventEditor->height();
    mEventEditor->show();
    mEventEditor->setGeometry(x,y,w,h);
#else
    if ( mEventEditor->width() < QApplication::desktop()->width() -60 || mEventEditor->width() > QApplication::desktop()->width() ) {
        topLevelWidget()->setCaption( i18n("Recreating edit dialog. Please wait...") );
        qDebug("KO: CalendarView: recreate mEventEditor %d %d",  mEventEditor->width(), QApplication::desktop()->width() );
        qApp->processEvents();
        delete mEventEditor;
        mEventEditor = mDialogManager->getEventEditor(); 
        topLevelWidget()->setCaption( i18n("") );
    }
    mEventEditor->showMaximized();
#endif
}
void CalendarView::showTodoEditor()
{
#ifdef DESKTOP_VERSION
    int x,y,w,h;
    x = mTodoEditor->geometry().x();
    y = mTodoEditor->geometry().y();
    w = mTodoEditor->width();
    h = mTodoEditor->height();
    mTodoEditor->show();
    mTodoEditor->setGeometry(x,y,w,h);
#else
    if ( mTodoEditor->width() < QApplication::desktop()->width() -60|| mTodoEditor->width() > QApplication::desktop()->width() ) {
        topLevelWidget()->setCaption( i18n("Recreating edit dialog. Please wait...") );
        qDebug("KO: CalendarView: recreate mTodoEditor %d %d ",  mTodoEditor->width() ,QApplication::desktop()->width()     );
        qApp->processEvents();
        delete mTodoEditor;  
        mTodoEditor = mDialogManager->getTodoEditor(); 
        topLevelWidget()->setCaption( i18n("") );
    }
    mTodoEditor->showMaximized();
#endif
}

void CalendarView::cloneIncidence()
{
    Incidence *incidence = currentSelection();
    if ( !incidence ) incidence = mTodoList->selectedIncidences().first();
    if ( incidence ) {
        cloneIncidence(incidence);
    }
}
void CalendarView::moveIncidence()
{
  Incidence *incidence = currentSelection();
    if ( !incidence ) incidence = mTodoList->selectedIncidences().first();
    if ( incidence ) {
        moveIncidence(incidence);
    }
}
void CalendarView::beamIncidence()
{
  Incidence *incidence = currentSelection();
    if ( !incidence ) incidence = mTodoList->selectedIncidences().first();
    if ( incidence ) {
        beamIncidence(incidence);
    }
}
void CalendarView::toggleCancelIncidence()
{
  Incidence *incidence = currentSelection();
    if ( !incidence ) incidence = mTodoList->selectedIncidences().first();
    if ( incidence ) {
        cancelIncidence(incidence);
    }
}


void CalendarView::cancelIncidence(Incidence * inc )
{
    inc->setCancelled( ! inc->cancelled() );
    changeIncidenceDisplay( inc,KOGlobals::EVENTEDITED );
    updateView();
}
void CalendarView::cloneIncidence(Incidence * orgInc )
{ 
    Incidence * newInc = orgInc->clone();
    newInc->recreate();
 
    if ( newInc->typeID() == todoID ) {
        Todo* t = (Todo*) newInc; 
        bool cloneSub = false;
        if (  orgInc->relations().count() ) {
            int result = KMessageBox::warningYesNoCancel(this, 
                                                         i18n("The todo\n%1\nwill be cloned!\nIt has subtodos!\nDo you want to clone\nall subtodos as well?").arg( KGlobal::formatMessage ( newInc->summary(),0 ) ),
                                                         i18n("Todo has subtodos"),
                                                         i18n("Yes"),
                                                         i18n("No"));
            
            if ( result == KMessageBox::Cancel ) {
                delete t;
                return;
            }
            if (result == KMessageBox::Yes) cloneSub = true;
        }
        showTodoEditor();
        mTodoEditor->editTodo( t );
        if ( mTodoEditor->exec() ) {
            if ( cloneSub ) {
                orgInc->cloneRelations( t );
                mCalendar->addIncidenceBranch( t );
                updateView();

            } else {
                mCalendar->addTodo( t );
                updateView();
            }
        } else {
            delete t;
        }
    }
    else if ( newInc->typeID() == eventID ) {
        Event*  e = (Event*)  newInc;
        showEventEditor();
        mEventEditor->editEvent( e );
        if ( mEventEditor->exec() ) {
            mCalendar->addEvent( e );
            updateView();
        } else {
            delete e;
        }
    } else if ( newInc->typeID() == journalID ) {
        mCalendar->addJournal( (Journal*) newInc );
        editJournal( (Journal*) newInc );
    }
    setActiveWindow();
}

void CalendarView::newEvent()
{
    // TODO: Replace this code by a common eventDurationHint of KOBaseView.
    KOAgendaView *aView = mViewManager->agendaView();
    if (aView) {
        if (aView->selectionStart().isValid()) {
            if (aView->selectedIsAllDay()) {
                newEvent(aView->selectionStart(),aView->selectionEnd(),true);
            } else {
                newEvent(aView->selectionStart(),aView->selectionEnd());
            }
            return;
        }
    }

    QDate date = mNavigator->selectedDates().first();
#if 0
    QDateTime current =  QDateTime::currentDateTime();
    if ( date <= current.date() ) {
        int hour = current.time().hour() +1;
        newEvent( QDateTime(  current.date(), QTime( hour, 0, 0 ) ),
                  QDateTime( current.date(), QTime( hour+ KOPrefs::instance()->mDefaultDuration, 0, 0 ) ) );
    } else
#endif
        newEvent( QDateTime( date, QTime( KOPrefs::instance()->mStartTime, 0, 0 ) ),
                  QDateTime( date, QTime( KOPrefs::instance()->mStartTime +
                                          KOPrefs::instance()->mDefaultDuration, 0, 0 ) ) );
}

void CalendarView::newEvent(QDateTime fh)
{
    newEvent(fh,
             QDateTime(fh.addSecs(3600*KOPrefs::instance()->mDefaultDuration)));
}

void CalendarView::newEvent(QDate dt)
{
    newEvent(QDateTime(dt, QTime(0,0,0)),
             QDateTime(dt, QTime(0,0,0)), true);
}
void CalendarView::newEvent(QDateTime fromHint, QDateTime toHint)
{
    newEvent(fromHint,  toHint, false);
}
void CalendarView::newEvent(QDateTime fromHint, QDateTime toHint, bool allDay)
{
  
    showEventEditor();
    mEventEditor->newEvent(fromHint,toHint,allDay);
    if ( mFilterView->filtersEnabled() ) {
        CalFilter *filter = mFilterView->selectedFilter();
        if (filter && filter->showCategories()) {
            mEventEditor->setCategories(filter->categoryList().join(",") );
        }
        if ( filter )
            mEventEditor->setSecrecy( filter->getSecrecy() );
    }
    mEventEditor->exec();
    setActiveWindow();
}
void  CalendarView::todoAdded(Todo * t)
{
  
    changeTodoDisplay ( t ,KOGlobals::EVENTADDED);
    updateTodoViews();
}
void  CalendarView::todoChanged(Todo * t)
{
    emit todoModified( t, 4 );
    // updateTodoViews();
}
void  CalendarView::todoToBeDeleted(Todo *)
{
    //qDebug("todoToBeDeleted(Todo *) ");
    updateTodoViews();
}
void  CalendarView::todoDeleted()
{
    //qDebug(" todoDeleted()");
    updateTodoViews();
}


void CalendarView::newTodoDateTime( QDateTime dt, bool allday )
{ 
    showTodoEditor();
    mTodoEditor->newTodo(dt,0,allday);
    if ( mFilterView->filtersEnabled() ) {
        CalFilter *filter = mFilterView->selectedFilter();
        if (filter && filter->showCategories()) {
            mTodoEditor->setCategories(filter->categoryList().join(",") );
        }
        if ( filter )
            mTodoEditor->setSecrecy( filter->getSecrecy() );
    }
    mTodoEditor->exec();
    setActiveWindow();
}

void CalendarView::newTodo()
{ 
    newTodoDateTime( QDateTime(),true );
}

void CalendarView::newSubTodo()
{
    Todo *todo = selectedTodo();
    if ( todo ) newSubTodo( todo );
}

void CalendarView::newSubTodo(Todo *parentEvent)
{
  
    showTodoEditor();
    mTodoEditor->newTodo(QDateTime(),parentEvent,true); 
    mTodoEditor->exec();
    setActiveWindow();
}

void CalendarView::newFloatingEvent()
{
    DateList tmpList = mNavigator->selectedDates();
    QDate date = tmpList.first();

    newEvent( QDateTime( date, QTime( 12, 0, 0 ) ),
              QDateTime( date, QTime( 12, 0, 0 ) ), true );
}


void CalendarView::editEvent( Event *event )
{

    if ( !event ) return;
    if ( event->isReadOnly() ) {
        showEvent( event );
        return;
    }
    showEventEditor();
    mEventEditor->editEvent( event , mFlagEditDescription);
    mEventEditor->exec();
    setActiveWindow();
    
}
void CalendarView::editJournal( Journal *jour )
{ 
    if ( !jour ) return;
    mDialogManager->hideSearchDialog();
    mViewManager->showJournalView();
    mNavigator->slotDaySelect( jour->dtStart().date() );
}
void CalendarView::editTodo( Todo *todo )
{
    if ( !todo ) return;
 
    if ( todo->isReadOnly() ) {
        showTodo( todo );
        return;
    }
    showTodoEditor();
    mTodoEditor->editTodo( todo ,mFlagEditDescription);
    mTodoEditor->exec();
    setActiveWindow();

}

KOEventViewerDialog* CalendarView::getEventViewerDialog()
{
    if ( !mEventViewerDialog ) {
        mEventViewerDialog = new KOEventViewerDialog(0);
        connect( mEventViewerDialog, SIGNAL(  editIncidence( Incidence* )), this, SLOT(editIncidence( Incidence* )  ) );
        connect( this, SIGNAL(configChanged()), mEventViewerDialog, SLOT(updateConfig()));
        connect( mEventViewerDialog, SIGNAL(jumpToTime( const QDate &)), 
                 dateNavigator(), SLOT( selectWeek( const QDate & ) ) );
        connect( mEventViewerDialog, SIGNAL(showAgendaView( bool ) ), 
                 viewManager(), SLOT( showAgendaView( bool ) ) );
        connect( mEventViewerDialog, SIGNAL(signalViewerClosed()), 
                 this, SLOT( slotViewerClosed() ) );
        connect( mEventViewerDialog, SIGNAL( todoCompleted(Todo *) ), 
                 this, SLOT( todoChanged(Todo *) ) );
        connect( mEventViewerDialog, SIGNAL( showIncidence( QString ) ),SLOT( showIncidence( QString ) ));
        mEventViewerDialog->resize( 640, 480 );
        
    }
    return mEventViewerDialog;
}
void CalendarView::showEvent(Event *event)
{
    getEventViewerDialog()->setEvent(event);
    getEventViewerDialog()->showMe();
}

void CalendarView::showTodo(Todo *event)
{
    getEventViewerDialog()->setTodo(event);
    getEventViewerDialog()->showMe();
}
void CalendarView::showJournal( Journal *jour )
{ 
    getEventViewerDialog()->setJournal(jour);
    getEventViewerDialog()->showMe();
   
}
// void CalendarView::todoModified (Todo *event, int changed)
// {
//  //  if (mDialogList.find (event) != mDialogList.end ()) {
// //     kdDebug() << "Todo modified and open" << endl;
// //     KOTodoEditor* temp = (KOTodoEditor *) mDialogList[event];
// //     temp->modified (changed);

// //   }

//   mViewManager->updateView();
// }

void CalendarView::appointment_show()
{
    Event *anEvent = 0;

    Incidence *incidence = mViewManager->currentView()->selectedIncidences().first();

    if (mViewManager->currentView()->isEventView()) {
        if ( incidence && incidence->typeID() == eventID ) {
            anEvent = static_cast<Event *>(incidence);
        }
    }

    if (!anEvent) {
        KNotifyClient::beep();
        return;
    }

    showEvent(anEvent);
}

void CalendarView::appointment_edit()
{
    Event *anEvent = 0;

    Incidence *incidence = mViewManager->currentView()->selectedIncidences().first();

    if (mViewManager->currentView()->isEventView()) {
        if ( incidence && incidence->typeID() == eventID ) {
            anEvent = static_cast<Event *>(incidence);
        }
    }

    if (!anEvent) {
        KNotifyClient::beep();
        return;
    }

    editEvent(anEvent);
}

void CalendarView::appointment_delete()
{
    Event *anEvent = 0;

    Incidence *incidence = mViewManager->currentView()->selectedIncidences().first();

    if (mViewManager->currentView()->isEventView()) {
        if ( incidence && incidence->typeID() == eventID ) {
            anEvent = static_cast<Event *>(incidence);
        }
    }

    if (!anEvent) {
        KNotifyClient::beep();
        return;
    }

    deleteEvent(anEvent);
}

void CalendarView::todo_resub( Todo * parent, Todo * sub )
{
    if (!sub) return;
    if ( sub->relatedTo() == parent )
        return;
    sub->setRelatedTo(parent);
    sub->updated();
    setModified(true);
    updateView();
}
void CalendarView::todo_unsub(Todo *anTodo )
{
    todo_resub( 0, anTodo );
}

void CalendarView::deleteTodo(Todo *todo)
{
    if (!todo) {
        KNotifyClient::beep();
        return;
    }
    if (KOPrefs::instance()->mConfirm) {
        QString text = KGlobal::formatMessage ( todo->summary(),0 );
        if (!todo->relations().isEmpty()) {
            text += i18n("\nhas sub-todos.\nAll completed sub-todos\nwill be deleted as well!");

        }
        switch (msgItemDelete(i18n("Todo:") +"\n"+text)) {
        case KMessageBox::Continue: // OK
            bool deleteT = false;
            if (!todo->relations().isEmpty()) {
                deleteT = removeCompletedSubTodos( todo );
            } 
            // deleteT == true: todo already deleted in removeCompletedSubTodos
            if ( !deleteT ) {
                checkExternalId(  todo );
                calendar()->deleteTodo(todo);
                changeTodoDisplay( todo,KOGlobals::EVENTDELETED );
            }
            updateView();
            break;
        } // switch
    } else {
        checkExternalId(  todo );
        mCalendar->deleteTodo(todo);
        changeTodoDisplay( todo,KOGlobals::EVENTDELETED );
        updateView();
    }
    
    emit updateSearchDialog();
}
void CalendarView::deleteJournal(Journal *jour)
{
    if (!jour) {
        KNotifyClient::beep();
        return;
    }
    if (KOPrefs::instance()->mConfirm) {

        QString des;
        if ( !jour->summary().isEmpty() ) {
            des = jour->summary();
        } else {
            des = jour->description().left(30);
            des = des.simplifyWhiteSpace ();
            des.replace (QRegExp ("\\n"),"" );
            des.replace (QRegExp ("\\r"),"" );
        }
        switch (msgItemDelete( i18n("Journal:") +"\n"+KGlobal::formatMessage ( des,0 ))) {
        case KMessageBox::Continue: // OK
            calendar()->deleteJournal(jour);
            updateView();
            break;
        } // switch
    } else {
        calendar()->deleteJournal(jour);;
        updateView();
    }
    emit updateSearchDialog();
}

void CalendarView::deleteEvent(Event *anEvent)
{
    if (!anEvent) {
        KNotifyClient::beep();
        return;
    }

    if (anEvent->doesRecur()) {
        QDate itemDate = mViewManager->currentSelectionDate();
        int km;
        if (!itemDate.isValid()) {
            //kdDebug() << "Date Not Valid" << endl;
            if (KOPrefs::instance()->mConfirm) {
                km = KMessageBox::warningContinueCancel(this,KGlobal::formatMessage ( anEvent->summary(),0 ) +
                                                        i18n("\nThis event recurs\nover multiple dates.\nAre you sure you want\nto delete this event\nand all its recurrences?"),
                                                        i18n("KO/Pi Confirmation"),i18n("Delete All"));
                if ( km == KMessageBox::Continue )
                    km = KMessageBox::No; // No = all below
            } else
                km = KMessageBox::No;
        } else {
            km = KMessageBox::warningYesNoCancel(this,KGlobal::formatMessage ( anEvent->summary(),0 ) +
                                                 i18n("\nThis event recurs\nover multiple dates.\nDo you want to delete\nall it's recurrences,\nor only the current one on:\n")+
                                                      KGlobal::locale()->formatDate(itemDate)+i18n(" ?\n\nDelete:\n"),
                                                 i18n("KO/Pi Confirmation"),i18n("Current"),
                                                 i18n("All"));
        }
        switch(km) {

        case KMessageBox::No: // Continue // all
            //qDebug("KMessageBox::No ");
            if (anEvent->organizer()==KOPrefs::instance()->email() && anEvent->attendeeCount()>0)
                schedule(Scheduler::Cancel,anEvent);

            checkExternalId( anEvent);
            mCalendar->deleteEvent(anEvent);
            changeEventDisplay(anEvent,KOGlobals::EVENTDELETED);
            break;

            // Disabled because it does not work
            //#if 0
        case KMessageBox::Yes: // just this one
            //QDate qd = mNavigator->selectedDates().first();
            //if (!qd.isValid()) {
            //  kdDebug() << "no date selected, or invalid date" << endl;
            //  KNotifyClient::beep();
            //  return;
            //}
            //while (!anEvent->recursOn(qd)) qd = qd.addDays(1);
            if (itemDate!=QDate(1,1,1) || itemDate.isValid()) {
                anEvent->addExDate(itemDate);
                int duration = anEvent->recurrence()->duration();
                if ( duration > 0 ) {
                    anEvent->recurrence()->setDuration( duration - 1 );
                }
                changeEventDisplay(anEvent, KOGlobals::EVENTEDITED);
            }
            break;
            //#endif
        } // switch
    } else {
        if (KOPrefs::instance()->mConfirm) {
            switch (KMessageBox::warningContinueCancel(this,KGlobal::formatMessage ( anEvent->summary(),0 ) +
                                                       i18n("\nAre you sure you want\nto delete this event?"),
                                                       i18n("KO/Pi Confirmation"),i18n("Delete"))) {
            case KMessageBox::Continue: // OK
                if (anEvent->organizer()==KOPrefs::instance()->email() && anEvent->attendeeCount()>0)
                    schedule(Scheduler::Cancel,anEvent);
                checkExternalId( anEvent);
                mCalendar->deleteEvent(anEvent);
                changeEventDisplay(anEvent, KOGlobals::EVENTDELETED);
                break;
            } // switch
        } else {
            if (anEvent->organizer()==KOPrefs::instance()->email() && anEvent->attendeeCount()>0)
                schedule(Scheduler::Cancel,anEvent);
            checkExternalId( anEvent);
            mCalendar->deleteEvent(anEvent);
            changeEventDisplay(anEvent, KOGlobals::EVENTDELETED);
        }
    } // if-else
    emit updateSearchDialog();
}

bool CalendarView::deleteEvent(const QString &uid)
{
    Event *ev = mCalendar->event(uid);
    if (ev) {
        deleteEvent(ev);
        return true;
    } else {
        return false;
    }
}

/*****************************************************************************/

void CalendarView::action_mail()
{
#ifndef KORG_NOMAIL
    KOMailClient mailClient;

    Incidence *incidence = currentSelection();

    if (!incidence) {
        KMessageBox::sorry(this,i18n("Can't generate mail:\nNo event selected."));
        return;
    }
    if(incidence->attendeeCount() == 0 ) {
        KMessageBox::sorry(this,
                           i18n("Can't generate mail:\nNo attendees defined.\n"));
        return;
    }

    CalendarLocal cal_tmp;
    Event *event = 0;
    Event *ev = 0;
    if ( incidence && incidence->typeID() == eventID ) {
        event = static_cast<Event *>(incidence);
        ev = new Event(*event);
        cal_tmp.addEvent(ev);
    }
    ICalFormat mForm();
    QString attachment = mForm.toString( &cal_tmp );
    if (ev) delete(ev);

    mailClient.mailAttendees(currentSelection(), attachment);

#endif

#if 0
    Event *anEvent = 0;
    if (mViewManager->currentView()->isEventView()) {
        anEvent = dynamic_cast<Event *>((mViewManager->currentView()->selectedIncidences()).first());
    }

    if (!anEvent) {
        KMessageBox::sorry(this,i18n("Can't generate mail:\nNo event selected."));
        return;
    }
    if(anEvent->attendeeCount() == 0 ) {
        KMessageBox::sorry(this,
                           i18n("Can't generate mail:\nNo attendees defined.\n"));
        return;
    }

    mailobject.emailEvent(anEvent);
#endif
}


void CalendarView::schedule_publish(Incidence *incidence)
{
    Event *event = 0;
    Todo *todo = 0;

    if (incidence == 0) {
        incidence = mViewManager->currentView()->selectedIncidences().first();
        if (incidence == 0) {
            incidence = mTodoList->selectedIncidences().first();
        }
    }
    if ( incidence && incidence->typeID() == eventID ) {
        event = static_cast<Event *>(incidence);
    } else {
        if ( incidence && incidence->typeID() == todoID ) {
            todo = static_cast<Todo *>(incidence);
        }
    }

    if (!event && !todo) {
        KMessageBox::sorry(this,i18n("No event selected."));
        return;
    }

    PublishDialog *publishdlg = new PublishDialog();
    if (incidence->attendeeCount()>0) {
        QPtrList<Attendee> attendees = incidence->attendees();
        attendees.first();
        while ( attendees.current()!=0 ) {
            publishdlg->addAttendee(attendees.current());
            attendees.next();
        }
    }
    bool send = true;
    if ( KOPrefs::instance()->mMailClient == KOPrefs::MailClientSendmail ) {
        if ( publishdlg->exec() != QDialog::Accepted )
            send = false;
    }
    if ( send ) {
        OutgoingDialog *dlg = mDialogManager->outgoingDialog();
        if ( event ) {
            Event *ev = new Event(*event);
            ev->registerObserver(0);
            ev->clearAttendees();
            if (!dlg->addMessage(ev,Scheduler::Publish,publishdlg->addresses())) {
                delete(ev);
            }
        } else {
            if ( todo ) {
                Todo *ev = new Todo(*todo);
                ev->registerObserver(0);
                ev->clearAttendees();
                if (!dlg->addMessage(ev,Scheduler::Publish,publishdlg->addresses())) {
                    delete(ev);
                }
            }
        }
    }
    delete publishdlg;
}

void CalendarView::schedule_request(Incidence *incidence)
{
    schedule(Scheduler::Request,incidence);
}

void CalendarView::schedule_refresh(Incidence *incidence)
{
    schedule(Scheduler::Refresh,incidence);
}

void CalendarView::schedule_cancel(Incidence *incidence)
{
    schedule(Scheduler::Cancel,incidence);
}

void CalendarView::schedule_add(Incidence *incidence)
{
    schedule(Scheduler::Add,incidence);
}

void CalendarView::schedule_reply(Incidence *incidence)
{
    schedule(Scheduler::Reply,incidence);
}

void CalendarView::schedule_counter(Incidence *incidence)
{
    schedule(Scheduler::Counter,incidence);
}

void CalendarView::schedule_declinecounter(Incidence *incidence)
{
    schedule(Scheduler::Declinecounter,incidence);
}

void CalendarView::schedule_publish_freebusy(int daysToPublish)
{
    QDateTime start = QDateTime::currentDateTime();
    QDateTime end = start.addDays(daysToPublish);

    FreeBusy *freebusy = new FreeBusy(mCalendar, start, end);
    freebusy->setOrganizer(KOPrefs::instance()->email());


    PublishDialog *publishdlg = new PublishDialog();
    if ( publishdlg->exec() == QDialog::Accepted ) {
        OutgoingDialog *dlg = mDialogManager->outgoingDialog();
        if (!dlg->addMessage(freebusy,Scheduler::Publish,publishdlg->addresses())) {
            delete(freebusy);
        }
    }
    delete publishdlg;
}

void CalendarView::schedule(Scheduler::Method method, Incidence *incidence)
{
    Event *event = 0;
    Todo *todo = 0;

    if (incidence == 0) {
        incidence = mViewManager->currentView()->selectedIncidences().first();
        if (incidence == 0) {
            incidence = mTodoList->selectedIncidences().first();
        }
    }
    if ( incidence && incidence->typeID() == eventID ) {
        event = static_cast<Event *>(incidence);
    }
    if ( incidence && incidence->typeID() == todoID ) {
        todo = static_cast<Todo *>(incidence);
    }

    if (!event && !todo) {
        KMessageBox::sorry(this,i18n("No event selected."));
        return;
    }

    if( incidence->attendeeCount() == 0 && method != Scheduler::Publish ) {
        KMessageBox::sorry(this,i18n("The event has no attendees."));
        return;
    }

    Event *ev = 0;
    if (event) ev = new Event(*event);
    Todo *to = 0;
    if (todo) to = new Todo(*todo);

    if (method == Scheduler::Reply || method == Scheduler::Refresh) {
        Attendee *me = incidence->attendeeByMails(KOPrefs::instance()->mAdditionalMails,KOPrefs::instance()->email());
        if (!me) {
            KMessageBox::sorry(this,i18n("Could not find your attendee entry.\nPlease check the emails."));
            return;
        }
        if (me->status()==Attendee::NeedsAction && me->RSVP() && method==Scheduler::Reply) {
            StatusDialog *statdlg = new StatusDialog(this);
            if (!statdlg->exec()==QDialog::Accepted) return;
            me->setStatus( statdlg->status() );
            delete(statdlg);
        }
        Attendee *menew = new Attendee(*me);
        if (ev) {
            ev->clearAttendees();
            ev->addAttendee(menew,false);
        } else {
            if (to) {
                todo->clearAttendees();
                todo->addAttendee(menew,false);
            }
        }
    }

    OutgoingDialog *dlg = mDialogManager->outgoingDialog();
    if (ev) {
        if ( !dlg->addMessage(ev,method) ) delete(ev);
    } else {
        if (to) {
            if ( !dlg->addMessage(to,method) ) delete(to);
        }
    }
}

void CalendarView::openAddressbook()
{
    KRun::runCommand("kaddressbook");
}

void CalendarView::setModified(bool modified)
{
    //qDebug("CalendarView::setModified %d ", modified);
    if ( modified )
        emit signalmodified();
    mModified = modified;
    if ( mModified )
        emit modifiedChanged(mModified);
  
}

bool CalendarView::isReadOnly()
{
    return mReadOnly;
}

void CalendarView::setReadOnly(bool readOnly)
{
    if (mReadOnly != readOnly) {
        mReadOnly = readOnly;
        emit readOnlyChanged(mReadOnly);
    }
}

bool CalendarView::isModified()
{
    return mModified;
}
void CalendarView::slotprintSelInc()
{
    if ( currentSelection() == 0 ) {
        KMessageBox::sorry(this,i18n("There is nothing selected!"));
        return;
    }
    showIncidence();
    getEventViewerDialog()->print();
    
}
void CalendarView::printSetup()
{
#ifndef KORG_NOPRINTER
    createPrinter();

    mCalPrinter->setupPrinter();
#endif
}

void CalendarView::print()
{
#ifndef KORG_NOPRINTER
    createPrinter();

    DateList tmpDateList = mNavigator->selectedDates();
    mCalPrinter->print(CalPrinter::Month,
                       tmpDateList.first(), tmpDateList.last());
#endif
}

void CalendarView::printPreview()
{
#ifndef KORG_NOPRINTER
    kdDebug() << "CalendarView::printPreview()" << endl;

    createPrinter();

    DateList tmpDateList = mNavigator->selectedDates();

    mViewManager->currentView()->printPreview(mCalPrinter,tmpDateList.first(),
                                              tmpDateList.last());
#endif
}
bool CalendarView::exportICalendar( QString filename )
{
    // Force correct extension
    if (filename.right(4) != ".ics") filename += ".ics";

    FileStorage storage( mCalendar, filename, new ICalFormat() );
    return storage.save();
}

bool CalendarView::exportICalendar()
{
    return exportICalendar( KFileDialog::getSaveFileName("icalout.ics",i18n("*.ics|ICalendars"),this) );
}

bool CalendarView::exportVCalendar( QString filename )
{
    if (mCalendar->journals().count() > 0) {
        int result = KMessageBox::warningContinueCancel(this,
                                                        i18n("The journal entries can not be\nexported to a vCalendar file."),
                                                        i18n("Data Loss Warning"),i18n("Proceed"),i18n("Cancel"),
                                                        true);
        if (result != KMessageBox::Continue) return false;
    }

    //QString filename = KFileDialog::getSaveFileName("vcalout.vcs",i18n("*.vcs|VCalendars"),this);

    // Force correct extension
    if (filename.right(4) != ".vcs") filename += ".vcs";

    FileStorage storage( mCalendar, filename, new VCalFormat );
    return storage.save();
 
}

void CalendarView::eventUpdated(Incidence *)
{
    setModified();
    // Don't call updateView here. The code, which has caused the update of the
    // event is responsible for updating the view.
    //  updateView();
}

void CalendarView::adaptNavigationUnits()
{
    if (mViewManager->currentView()->isEventView()) {
        int days = mViewManager->currentView()->currentDateCount();
        if (days == 1) {
            emit changeNavStringPrev(i18n("&Previous Day"));
            emit changeNavStringNext(i18n("&Next Day"));
        } else {
            emit changeNavStringPrev(i18n("&Previous Week"));
            emit changeNavStringNext(i18n("&Next Week"));
        }
    }
}

void CalendarView::processMainViewSelection( Incidence *incidence )
{
    if ( incidence ) mTodoList->clearSelection();
    processIncidenceSelection( incidence );
}

void CalendarView::processTodoListSelection( Incidence *incidence )
{
    if ( incidence && mViewManager->currentView() ) {
        mViewManager->currentView()->clearSelection();
    }
    processIncidenceSelection( incidence );
}

void CalendarView::processIncidenceSelection( Incidence *incidence )
{
    emit incidenceSelected( incidence );
    if ( incidence == mSelectedIncidence ) return;
    mSelectedIncidence = incidence;

    if ( incidence && incidence->typeID() == eventID ) {
        Event *event = static_cast<Event *>( incidence );
        if ( event->organizer() == KOPrefs::instance()->email() ) {
            emit organizerEventsSelected( true );
        } else {
            emit organizerEventsSelected(false);
        }
        if (event->attendeeByMails( KOPrefs::instance()->mAdditionalMails,
                                    KOPrefs::instance()->email() ) ) {
            emit groupEventsSelected( true );
        } else {
            emit groupEventsSelected(false);
        }
        return;
    } else {
        if  ( incidence && incidence->typeID() == todoID ) {
            emit todoSelected( true );
            Todo *event = static_cast<Todo *>( incidence );
            if ( event->organizer() == KOPrefs::instance()->email() ) {
                emit organizerEventsSelected( true );
            } else {
                emit organizerEventsSelected(false);
            }
            if (event->attendeeByMails( KOPrefs::instance()->mAdditionalMails,
                                        KOPrefs::instance()->email() ) ) {
                emit groupEventsSelected( true );
            } else {
                emit groupEventsSelected(false);
            }
            return;
        } else {
            emit todoSelected( false );
            emit organizerEventsSelected(false);
            emit groupEventsSelected(false);
        }
        return;
    }

    /*  if  ( incidence && incidence->typeID() == todoID ) {
        emit todoSelected( true );
        } else {
        emit todoSelected( false );
        }*/
}


void CalendarView::checkClipboard()
{
#ifndef KORG_NODND
    if (ICalDrag::canDecode(QApplication::clipboard()->data())) {
        emit pasteEnabled(true);
    } else {
        emit pasteEnabled(false);
    }
#endif
}

void CalendarView::showDates(const DateList &selectedDates)
{
    //  kdDebug() << "CalendarView::selectDates()" << endl;
  

    if ( !mBlockShowDates ) {
        if ( mViewManager->currentView() ) {
            updateView( selectedDates.first(), selectedDates.last() );
        } else {
            mViewManager->showAgendaView();
        }
    }

    QDate date = selectedDates.first();
    if ( ! date.isValid() ) {
        topLevelWidget()->setCaption("");
        return;
    }

    QString selDates;
    selDates = KGlobal::locale()->formatDate( date, true);
    if  (selectedDates.first() < selectedDates.last() )
        selDates += " - " + KGlobal::locale()->formatDate( selectedDates.last(),true);
    else {
        QString addString;
        if ( date == QDateTime::currentDateTime().date() )
            addString = i18n("Today");
        else if (  date == QDateTime::currentDateTime().date().addDays(1) )
            addString = i18n("Tomorrow");
        else if ( date  == QDateTime::currentDateTime().date().addDays(-1) )
            addString = i18n("Yesterday");
        else if ( date == QDateTime::currentDateTime().date().addDays(-2) )
            addString = i18n("Day before yesterday");
        else if ( date == QDateTime::currentDateTime().date().addDays(2) )
            addString = i18n("Day after tomorrow");
        if ( !addString.isEmpty() ) {
            topLevelWidget()->setCaption( addString+", " + selDates );
            return;
        }
    }
    topLevelWidget()->setCaption( i18n("Dates: ") + selDates );
  
}

QPtrList<CalFilter> CalendarView::filters()
{
    return mFilters;

}
void CalendarView::editFilters()
{
    //  kdDebug() << "CalendarView::editFilters()" << endl;

    CalFilter *filter = mFilters.first();
    while(filter) {
        filter = mFilters.next();
    }

    mDialogManager->showFilterEditDialog(&mFilters);
    updateFilter();
}
void CalendarView::toggleFilter()
{
    if ( mLeftFrame->isHidden() ) {
        toggleExpand();
        showFilter( true );
    } else 
        showFilter(! mCalEditView->isVisible());
}

KOFilterView *CalendarView::filterView()
{
    return mFilterView;
}
void CalendarView::selectFilter( int fil )
{
    mFilterView->setSelectedFilter( fil );
    updateUnmanagedViews();
}
void CalendarView::showFilter(bool visible)
{
#if 1
    if (visible)  {
        mCalEditView->readConfig(); 
        mCalEditView->show(); 
        QValueList<int> sizes;
        sizes = mLeftFrame->sizes();
        if ( sizes.count() == 4 &&  sizes[3] < 20 ) {
            sizes.clear();
            sizes << 100;
            sizes << 0;
            sizes << 0;
            sizes << 100;
            mLeftFrame->setSizes(sizes);
        }
#if 0
        sizes = mLeftFrame->sizes();
        int ccc = 0;
        while ( ccc < sizes.count()) {
            qDebug("size %d %d ", ccc, sizes[ccc]);
            ++ccc;    
        }
#endif
        
    }
    else {
        mCalEditView->hide();
    }
#else
    if (visible) mFilterView->show();
    else mFilterView->hide();
#endif
}
void CalendarView::toggleFilerEnabled( )
{
    mFilterView->setFiltersEnabled ( !mFilterView->filtersEnabled()  );
    if ( !mFilterView->filtersEnabled()   ) 
        topLevelWidget()->setCaption( i18n("Filter disabled ") );
    updateUnmanagedViews();
        
}
void CalendarView::updateFilter()
{
    CalFilter *filter = mFilterView->selectedFilter();
    if (filter) {
        QString mess;
        if (mFilterView->filtersEnabled()) {
            mess =  i18n("Filter selected: ")+filter->name();
            filter->setEnabled(true);
        }
        else filter->setEnabled(false);
        mCalendar->setFilter(filter);
        updateView();
        if ( !mess.isEmpty() )
            topLevelWidget()->setCaption( mess );
            
    }
    emit filtersUpdated();
}

void CalendarView::filterEdited()
{
    mFilterView->updateFilters();
    updateFilter();
    writeSettings();
}


void CalendarView::takeOverEvent()
{
    Incidence *incidence = currentSelection();

    if (!incidence) return;

    incidence->setOrganizer(KOPrefs::instance()->email());
    incidence->recreate();
    incidence->setReadOnly(false);

    updateView();
}

void CalendarView::takeOverCalendar()
{
    // TODO: Create Calendar::allIncidences() function and use it here

    clearAllViews();
    QPtrList<Event> events = mCalendar->events();
    for(uint i=0; i<events.count(); ++i) {
        events.at(i)->setOrganizer(KOPrefs::instance()->email());
        events.at(i)->recreate();
        events.at(i)->setReadOnly(false);
    }

    QPtrList<Todo> todos = mCalendar->todos();
    for(uint i=0; i<todos.count(); ++i) {
        todos.at(i)->setOrganizer(KOPrefs::instance()->email());
        todos.at(i)->recreate();
        todos.at(i)->setReadOnly(false);
    }

    QPtrList<Journal> journals = mCalendar->journals();
    for(uint i=0; i<journals.count(); ++i) {
        journals.at(i)->setOrganizer(KOPrefs::instance()->email());
        journals.at(i)->recreate();
        journals.at(i)->setReadOnly(false);
    }

    updateView();
}

void CalendarView::showIntro()
{
    kdDebug() << "To be implemented." << endl;
}

QWidgetStack *CalendarView::viewStack()
{
    return mRightFrame;
}

QWidget *CalendarView::leftFrame()
{
    return ( QWidget *)mLeftFrame;
}

DateNavigator *CalendarView::dateNavigator()
{
    return mNavigator;
}

KDateNavigator* CalendarView::dateNavigatorWidget()
{
    return mDateNavigator->navigatorView();
}
void CalendarView::toggleDateNavigatorWidget()
{
    KOPrefs::instance()->mShowDateNavigator = !KOPrefs::instance()->mShowDateNavigator ;
   
    if (!KOPrefs::instance()->mShowDateNavigator )
        mDateNavigator->hide();
    else
        mDateNavigator->show();
}
void CalendarView::addView(KOrg::BaseView *view)
{
    mViewManager->addView(view);
}

void CalendarView::showView(KOrg::BaseView *view)
{
    mViewManager->showView(view, mLeftFrame->isVisible());
}

Incidence *CalendarView::currentSelection()
{
    return mViewManager->currentSelection();
}
void CalendarView::toggleAllDaySize()
{
    /*
    if ( KOPrefs::instance()->mAllDaySize > 47 )
        KOPrefs::instance()->mAllDaySize = KOPrefs::instance()->mAllDaySize /2;
    else
        KOPrefs::instance()->mAllDaySize = KOPrefs::instance()->mAllDaySize *2;
    */
    viewManager()->agendaView()->toggleAllDay();
}
void CalendarView::toggleExpand()
{
    //  if ( mLeftFrame->isHidden() ) {
    //     mLeftFrame->show(); 
    //     emit calendarViewExpanded( false );
    //   } else {
    //     mLeftFrame->hide();
    //     emit calendarViewExpanded( true );
    //   }
    //qDebug(" CalendarView::toggleExpand()");
    globalFlagBlockAgenda = 1;
    emit calendarViewExpanded( !mLeftFrame->isHidden() );
    globalFlagBlockAgenda = 5;
    mViewManager->raiseCurrentView( !mLeftFrame->isHidden() );
    //mViewManager->showView( 0, true );
}

void CalendarView::calendarModified( bool modified, Calendar * )
{
    setModified( modified );
}

Todo *CalendarView::selectedTodo()
{
    Incidence *incidence = currentSelection();
    if ( incidence && incidence->typeID() == todoID ) {
        return static_cast<Todo *>( incidence );
    }

    incidence = mTodoList->selectedIncidences().first();
    if ( incidence && incidence->typeID() == todoID ) {
        return static_cast<Todo *>( incidence );
    }

    return 0;
}

void CalendarView::showIncidence()
{
    mViewerCallerIsSearchDialog = false;
    Incidence *incidence = currentSelection();
    if ( !incidence ) incidence = mTodoList->selectedIncidences().first();
    if ( incidence ) {
        ShowIncidenceVisitor v;
        v.act( incidence, this );
    }
}
void CalendarView::editIncidenceDescription()
{ 
    mFlagEditDescription = true;
    editIncidence();
    mFlagEditDescription = false;
}
void CalendarView::editIncidence()
{
    // qDebug("editIncidence() ");
    Incidence *incidence = currentSelection();
    if ( !incidence ) incidence = mTodoList->selectedIncidences().first();
    if ( incidence ) {
        EditIncidenceVisitor v;
        v.act( incidence, this );
    }
}

void CalendarView::deleteIncidence()
{
    Incidence *incidence = currentSelection();
    if ( !incidence ) incidence = mTodoList->selectedIncidences().first();
    if ( incidence ) {
        deleteIncidence(incidence);
    }
}
void CalendarView::showIncidence(QString uid)
{
    Incidence *inc = mCalendar->incidence( uid );
    if ( inc )
        showIncidence( inc );
}
void CalendarView::showIncidence(Incidence *incidence)
{
    mViewerCallerIsSearchDialog = false;
    //qDebug("%x %x ",sender (), mDialogManager->getSearchDialog() );
    if ( sender() && mDialogManager->getSearchDialog()  ) {
        if ( sender () == mDialogManager->getSearchDialog()->listview() ) {
            mViewerCallerIsSearchDialog = true;
        }
    }
    if ( incidence ) {
        ShowIncidenceVisitor v;
        v.act( incidence, this );
    }
}

void CalendarView::editIncidence(Incidence *incidence)
{ 
    if ( incidence ) {
       
        EditIncidenceVisitor v;
        v.act( incidence, this );
        
    }
}

void CalendarView::deleteIncidence(Incidence *incidence)
{
    //qDebug(" CalendarView::deleteIncidence ");
    if ( incidence == 0 ) {
        updateView();
        emit updateSearchDialog();
        return;
    }
    if ( incidence ) {
        DeleteIncidenceVisitor v;
        v.act( incidence, this );
    }
}


void CalendarView::lookForOutgoingMessages()
{
    OutgoingDialog *ogd = mDialogManager->outgoingDialog();
    ogd->loadMessages();
}

void CalendarView::lookForIncomingMessages()
{
    IncomingDialog *icd = mDialogManager->incomingDialog();
    icd->retrieve();
}

bool  CalendarView::removeCompletedSubTodos( Todo* t )
{
    bool deleteTodo = true;
    QPtrList<Incidence> subTodos;
    Incidence *aTodo;
    subTodos = t->relations();
    for (aTodo = subTodos.first(); aTodo; aTodo = subTodos.next()) {
        if (! removeCompletedSubTodos( (Todo*) aTodo ))
            deleteTodo = false;
    }
    if ( deleteTodo ) {
        if ( t->isCompleted() && !t->doesRecur()) {
            checkExternalId(  t );
            mCalendar->deleteTodo( t );
            changeTodoDisplay( t,KOGlobals::EVENTDELETED );
        }
        else
            deleteTodo = false;
    }
    return deleteTodo;

}
void CalendarView::purgeCompleted()
{
    int result = KMessageBox::warningContinueCancel(this,
                                                    i18n("Delete all completed todos?\n(Completed recurring todos\nwill not be deleted!)"),i18n("Purge Todos"),i18n("Purge"));

    if (result == KMessageBox::Continue) {

        QPtrList<Todo> todoCal;
        QPtrList<Todo> rootTodos;
        //QPtrList<Incidence> rel;
        Todo *aTodo;
        todoCal = calendar()->todos();
        for (aTodo = todoCal.first(); aTodo; aTodo = todoCal.next()) {
            if ( !aTodo->relatedTo() )
                rootTodos.append( aTodo );
        }
        for (aTodo = rootTodos.first(); aTodo; aTodo = rootTodos.next()) {
            removeCompletedSubTodos( aTodo );
        }

        updateView();
    }
}

void CalendarView::slotCalendarChanged()
{
   ; 
}

void CalendarView::keyPressEvent ( QKeyEvent *e) 
{
    //qDebug("CalendarView::keyPressEvent ");
     e->ignore();
}

void CalendarView::multiResourceSyncStart( bool start )
{
    mMultiResourceSync = start;

}
bool CalendarView::sync(KSyncManager* manager, QString filename, int mode,QString resource)
{
   
    if ( manager != mSyncManager)
        qDebug("KO: Internal error-1. SyncManager mismatch ");
    if ( filename == QDir::homeDirPath ()+"/.kdecalendardump.ics" ) {
        qDebug("KO: SyncKDE request detected!");
    }
    mCurrentSyncDevice = mSyncManager->getCurrentSyncDevice();
    mCurrentSyncName = mSyncManager->getCurrentSyncName();
    if ( !resource.isEmpty() ) {
        int exclusiveSyncResource = KOPrefs::instance()->getFuzzyCalendarID( resource );
        qDebug( "KO: Sync request for resource: %s", resource.latin1() );
        
        if ( !exclusiveSyncResource ) {
            qDebug( "KO: Requested sync resource not found: %s", resource.latin1() );
            return false;
        }
	topLevelWidget()->setCaption( i18n("Syncing local resource: %1").arg( resource ) );
        mCalendar->setDefaultCalendar( exclusiveSyncResource ); 
        mCalendar->setDefaultCalendarEnabledOnly();
    }
    bool result =  syncCalendar( filename, mode );
    if ( !resource.isEmpty() )
         restoreCalendarSettings();
    return result;
}
bool CalendarView::syncExternal(KSyncManager* manager, QString resource)
{
    //mSyncManager = manager;
    if ( manager != mSyncManager)
        qDebug("KO: Internal error-2. SyncManager mismatch ");
    mCurrentSyncDevice = mSyncManager->getCurrentSyncDevice();
    mCurrentSyncName = mSyncManager->getCurrentSyncName();
    if ( resource == "sharp" )
        syncExternal( 0 );
    if ( resource == "phone" )
         syncExternal( 1 );  
    // pending setmodified
    return true;
}
void CalendarView::setSyncManager(KSyncManager* manager)
{
   mSyncManager = manager;
}

void  CalendarView::removeSyncInfo( QString syncProfile) 
{
    qDebug("KO: removeSyncInfo for profile %s ", syncProfile.latin1());
    mCalendar->removeSyncInfo( syncProfile );

}

void CalendarView::undo_delete()
{
    //qDebug("undo_delete() ");
    Incidence* undo = mCalendar->undoIncidence();
    if ( !undo ) {
        KMessageBox::sorry(this,i18n("There is nothing to undo!"),
                           i18n("KO/Pi"));
        return;
    }
    if ( KMessageBox::Continue ==KMessageBox::warningContinueCancel(this,KGlobal::formatMessage ( undo->summary(),0 ) +
                                                       i18n("\nAre you sure you want\nto restore this?"),
                                                                    i18n("KO/Pi Confirmation"),i18n("Restore"))) {
        mCalendar->undoDeleteIncidence();
        updateView();
    }
}

void  CalendarView::slotViewerClosed()
{
    QTimer::singleShot( 50, this, SLOT ( resetFocus() ) );
}

void CalendarView::resetFocus()
{
    if ( mViewerCallerIsSearchDialog ) {
        if ( mDialogManager->getSearchDialog()->isVisible() ){
            mDialogManager->getSearchDialog()->raise();
            mDialogManager->getSearchDialog()->setActiveWindow();
            mDialogManager->getSearchDialog()->listview()->resetFocus(); 
        } else
            mViewerCallerIsSearchDialog = false;
    }
    if ( !mViewerCallerIsSearchDialog ) {
        //mViewManager->currentView()->setFocus();
        //qDebug("sssssssssssssssset focus ");
        topLevelWidget()->raise();
        setActiveWindow();
        //setFocus();
    }
    mViewerCallerIsSearchDialog = false;
}

void CalendarView::showNextAlarms()
{
    QString message;
    QDateTime nextAl = mCalendar->nextAlarmEventDateTime();
    bool found = false; 
    QString sum;
    Incidence * alarmIncidence = mCalendar->getCurrentNextAlarmIncidence();
    if (  nextAl.isValid() && mNextAlarmDateTime > QDateTime::currentDateTime() && alarmIncidence ) {
        sum = mCalendar->nextSummary();
        QDateTime nextA = mNextAlarmDateTime;
        QDateTime cur = QDateTime::currentDateTime();
        int  secs = cur.secsTo( nextA );
        int min = secs /60; 
        int hours = min /60;
        min = min % 60;
        int days = hours /24;
        hours = hours % 24;
        
        //message = i18n("The next alarm is in:\n");
        if ( days > 1 )
            message += i18n("%1 days\n").arg( days );
        else if ( days == 1 )
            message += i18n("1 day\n");
        if ( hours > 1 )
            message += i18n("%1 hours\n").arg( hours );
        else if ( hours == 1 )
            message += i18n("1 hour\n");
        if ( min > 1 )
            message += i18n("%1 minutes\n").arg( min );
        else if ( min == 1 )
            message += i18n("1 minute\n");
        if ( message.isEmpty() ) 
            message = i18n("The next alarm is in\nless than one minute!");
        else 
            message = i18n("The next alarm is in:\n") + message;
        QString offset_text =i18n("( %1 before )").arg( alarmIncidence->alarms().first()->offsetText() );
        message += i18n("(%1)\n%4\n++++++++++++++++++++\n%2\n%3\n++++++++++++++++++++").arg( KGlobal::locale()->formatDateTime(nextA , false)).arg(sum ).arg( KGlobal::locale()->formatDateTime(nextAl , false)).arg( offset_text ) ;
        found = true;
    } else {
        message = i18n("There is no next alarm.");

    }
#ifdef DESKTOP_VERSION
    if ( ! KOPrefs::instance()->mUseInternalAlarmNotification ) {
        message += i18n("\nThe internal alarm notification is disabled!\n");
        message += i18n("Enable it in the settings menu, TAB alarm.");
    }

#endif
    if ( found ) {
        int result = 0;
        if ( result = QMessageBox::information ( this, i18n("Next alarm: %1").arg(sum), message, 
                                        i18n("OK"), i18n("Show in\nagenda"), i18n("Show\ndetails")) ) {
            if ( result == 1 ) {
                KOPrefs::instance()->mHourSize=4;
                viewManager()->agendaView()->updateConfig();
                mViewManager->showNextXView();
                mNavigator->slotDaySelect( nextAl.date() );
                int hour = nextAl.time().hour();
                mViewManager->agendaView()->setStartHour( hour  );
            } else {
                showIncidence( alarmIncidence );

            }
        }
    } else {
        KMessageBox::information( this, message);
    }
}

void CalendarView::displayCalendarInfo( int id )
{
   int e, t,j;
   KopiCalendarFile * kkf = KOPrefs::instance()->getCalendar( id );
   QString name = kkf->mName;
   mCalendar->getIncidenceCount( id, e, t, j );
   QString file = KGlobal::formatMessage ( kkf->mFileName ,0 );
   QString mess = i18n("The calendar <b>%1</b> is displaying file <b>%2</b>").arg(name).arg(file);
   mess += i18n("<br>The calendar contains<br><b>%1 events<br>%2 todos<br>%3 journals</b>").arg( e ).arg( t ).arg( j );
   KMessageBox::information( this, mess );


}
