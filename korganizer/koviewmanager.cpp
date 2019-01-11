/*
  This file is part of KOrganizer.

  Copyright (c) 2001
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

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

#include <qwidgetstack.h>

#include <kconfig.h>
#include <kglobal.h>
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif
#include <qdatetime.h>
#include "calendarview.h"
#include "datenavigator.h"
#include "kotodoview.h"
#include "koagendaview.h"
#include "kodialogmanager.h"
#include "komonthview.h"
#include "kolistview.h"
#include "kowhatsnextview.h"
#include "kojournalview.h"
#include "kotimespanview.h"
#include "koprefs.h"
#include "navigatorbar.h"
#include "kdatenavigator.h"

#include "koviewmanager.h"
//extern bool externFlagMonthviewBlockPainting;

//bool globalFlagBlockPainting = false;
int globalFlagBlockAgenda = 0;
int globalFlagBlockLabel = 0;
int globalFlagBlockAgendaItemPaint = 1;
int globalFlagBlockAgendaItemUpdate = 1;


KOViewManager::KOViewManager( CalendarView *mainView ) :
    QObject(), mMainView( mainView )
{

    lastMode = 0;
    lastCount = 0;
    lastNDMode = false;
    selecteddatescount = 0;
    selecteddate = QDate ( 2000, 1, 1 );
    baseCycleDate = QDate ( 2000, 1, 1 );
    mCurrentView = 0;
    flagResetViewChangeDate = 0;
    mWhatsNextView = 0;
    mTodoView = 0;
    mAgendaView = 0;
    mMonthView = 0;
    mListView = 0;
    mJournalView = 0;
    mTimeSpanView = 0; 
    mCurrentAgendaView = 0 ;
    mFlagShowNextxDays = false;
}

KOViewManager::~KOViewManager()
{
}


KOrg::BaseView *KOViewManager::currentView()
{
    return mCurrentView;
}

void KOViewManager::readSettings(KConfig *config)
{
    config->setGroup("General");
    QString view = config->readEntry("Current View");
    if (view == "WhatsNext") showWhatsNextView();
    else if (view == "Month") {
        if ( !KOPrefs::instance()->mMonthViewWeek )
            showMonthView();
        else
            showMonthViewWeek();
    }
    else if (view == "List") showListView();
    else if (view == "Journal") showJournalView();
    else if (view == "TimeSpan") showTimeSpanView();
    else if (view == "Todo") showTodoView();
    else {
        config->setGroup( "Views" );
        int dateCount = config->readNumEntry( "ShownDatesCount", 7 );
        mCurrentAgendaView = dateCount;
        showAgendaView();
        mCurrentAgendaView = dateCount;
#ifdef DESKTOP_VERSION
        QTimer::singleShot( 1000, mAgendaView, SLOT ( setInitStartHour() ) );
#endif
    }
}


void KOViewManager::showDateView( int view, QDate date)
{

    //qDebug("date %d %s", view, date.toString().latin1());

    if (view != 9)
        lastMode = 0;
    //qDebug("%d %d  ", lastNDMode, mFlagShowNextxDays );
    bool savemFlagShowNextxDays = mFlagShowNextxDays;
    mFlagShowNextxDays = false;
    if ( view == 3 ) {
        //mCurrentAgendaView = 1 ;
        lastDate = mMainView->dateNavigator()->selectedDates().first();
        lastCount = mMainView->dateNavigator()->selectedDates().count();
        lastNDMode = savemFlagShowNextxDays;
        mMainView->dateNavigator()->selectDate( date );
        lastMode = 1;
        mCurrentAgendaView = 1 ;
    } else if (view == 4 ) {
        mCurrentAgendaView = 7 ;
        mMainView->dateNavigator()->selectDates( date, 7 );
    } else if (view == 5 ) {
        mCurrentAgendaView = 14 ;
        mMainView->dateNavigator()->selectDates( date, 14);
    } else if (view == 6 ) {
        resetDateSilent( date,1);
        showMonthView();
    } else if (view == 7 ) {
        mMainView->dateNavigator()->selectDate( date );
        showJournalView();
    } else if (view == 8 ) {
        globalFlagBlockAgenda = 1;
        if ( mCurrentAgendaView != 3 )
            mCurrentAgendaView = -1;
        showAgendaView(KOPrefs::instance()->mFullViewMonth);
        globalFlagBlockAgenda = 2;
        mMainView->dateNavigator()->selectDates( date ,
                                                 KOPrefs::instance()->mNextXDays );
        mFlagShowNextxDays = true;
        mCurrentAgendaView = 3 ;
    } if (view == 9) { // return behaviour, for getting back from mode == 3 (single day mode )
        if ( lastMode ) {
            mCurrentAgendaView = lastCount ;
            mMainView->dateNavigator()->selectDates( lastDate, lastCount);
            mFlagShowNextxDays = lastNDMode;
            if ( mFlagShowNextxDays  ) {
                mCurrentAgendaView = 3 ;
            }
        } else
            showWeekView();
    } else if (view == 10) {
        mMainView->dateNavigator()->selectDates( date,mMainView->dateNavigator()->selectedDates().count() );
    } 
}

   

void KOViewManager::writeSettings(KConfig *config)
{
     config->setGroup("General");

    QString view;
    if (mCurrentView == mWhatsNextView) view = "WhatsNext";
    else if (mCurrentView == mMonthView) view = "Month";
    else if (mCurrentView == mListView) view = "List";
    else if (mCurrentView == mJournalView) view = "Journal";
    else if (mCurrentView == mTimeSpanView) view = "TimeSpan";
    else if (mCurrentView == mTodoView) view = "Todo";
    else view = "Agenda";

    config->writeEntry("Current View",view);

    if (mAgendaView) {
        mAgendaView->writeSettings(config);
    }
    if (mTimeSpanView) {
        mTimeSpanView->writeSettings(config);
    }
    if (mListView) {
        mListView->writeSettings(config);
    }
    if (mTodoView) {
        mTodoView->saveLayout(config,"Todo View");
    }
}
void KOViewManager::showNextView()
{ 

    int newCount = mMainView->dateNavigator()->selectedDates().count();
    if ( selecteddatescount != newCount && flagResetViewChangeDate == 0 ) {
        flagResetViewChangeDate = 1;
    }
    if ( selecteddate != mMainView->dateNavigator()->selectedDates().first() )
        flagResetViewChangeDate = 1;
    if ( flagResetViewChangeDate > 0 ) {
        baseCycleDate = mMainView->dateNavigator()->selectedDates().first();
        //qDebug("newCycle ");
    }
    if (mCurrentView == mWhatsNextView) goto NEXT_X;
    if (mCurrentView == mAgendaView && mFlagShowNextxDays)  goto JOURNAL;
    if  (mCurrentView == mJournalView )  goto DAY_1;
    if (mCurrentView == mAgendaView && mCurrentAgendaView == 1 )  goto DAY_5;
    if (mCurrentView == mAgendaView && mCurrentAgendaView == 5 )  goto DAY_7;
    if  (mCurrentView == mAgendaView )  goto DAY_6;
    if  (mCurrentView == mMonthView && KOPrefs::instance()->mMonthViewWeek)  goto MONTH;
    if  (mCurrentView == mMonthView )  goto LIST;
    if (mCurrentView == mListView ) goto TODO;
    // if  (mCurrentView == mTodoView )  goto NEXT;
 NEXT:
    if ( KOPrefs::instance()->mShowIconNext )  { showWhatsNextView();goto ENTE ;}
 NEXT_X:
    if ( KOPrefs::instance()->mShowIconNextDays )  { 
        showNextXView();
        goto ENTE ;
    } 
 JOURNAL:
    if ( KOPrefs::instance()->mShowIconJournal )  { 
        resetDateSilent( baseCycleDate , 1  );  
        showJournalView() ;goto ENTE ;}
 DAY_1:
    if ( KOPrefs::instance()->mShowIconDay1 )  { 
        resetDateSilent( baseCycleDate , 2  );    
        showDayView() ;goto ENTE ;}
 DAY_5:
    if ( KOPrefs::instance()->mShowIconDay5 )  {  
        resetDateSilent( baseCycleDate , 2  );   
        showWorkWeekView() ;goto ENTE ;}
 DAY_7:
    if ( KOPrefs::instance()->mShowIconDay7 )   {  
        resetDateSilent( baseCycleDate , 2  );   
        showWeekView();goto ENTE ;}
 DAY_6:
    if ( KOPrefs::instance()->mShowIconDay6 )   { 
        resetDateSilent( baseCycleDate , 2  );  
        showMonthViewWeek();goto ENTE ;}
 MONTH:
    if ( KOPrefs::instance()->mShowIconMonth )   { 
        resetDateSilent( baseCycleDate , 2  ); 
        showMonthView();goto ENTE ;}
 LIST:
    if ( KOPrefs::instance()->mShowIconList )  {  
        resetDateSilent( baseCycleDate , 2  );
        showListView() ;goto ENTE ;}
 TODO:
    if ( KOPrefs::instance()->mShowIconTodoview ) {  showTodoView() ;goto ENTE ;}
    if ( KOPrefs::instance()->mShowIconNext )  { goto NEXT ;}
    if ( KOPrefs::instance()->mShowIconNextDays )  { goto NEXT_X ;}
    if ( KOPrefs::instance()->mShowIconJournal )  { goto  JOURNAL;}
    if ( KOPrefs::instance()->mShowIconDay1 )  { goto DAY_1 ;}
    if ( KOPrefs::instance()->mShowIconDay5 )  { goto DAY_5 ;}
    if ( KOPrefs::instance()->mShowIconDay7 )   { goto DAY_7 ;}
    if ( KOPrefs::instance()->mShowIconDay6 )   { goto DAY_6 ;}
    if ( KOPrefs::instance()->mShowIconMonth )   {goto MONTH ;}
    if ( KOPrefs::instance()->mShowIconList )  {  goto LIST ;}
    //if ( KOPrefs::instance()->mShowIconTodoview ) {  showTodoView() ;goto ENTE ;}
 ENTE:
    flagResetViewChangeDate = 0;
    selecteddatescount =  mMainView->dateNavigator()->selectedDates().count();
    selecteddate = mMainView->dateNavigator()->selectedDates().first();

}
void KOViewManager::resetDateSilent( QDate date , int days )
{
    mMainView->dateNavigator()->blockSignals( true );
    mMainView->dateNavigator()->selectDates( date , days ); 
    mMainView->dateNavigator()->blockSignals( false );
}
void KOViewManager::setDefaultCalendar(int)
{
    if ( mJournalView && mCurrentView == mJournalView )
        mJournalView->updateView();
}

void KOViewManager::showView(KOrg::BaseView *view, bool fullScreen )
{
    if ( flagResetViewChangeDate < 10 )
        ++flagResetViewChangeDate;
    //mFlagShowNextxDays = false;
    //if(view == mCurrentView) return;  
    if ( view == 0 ) {
        view = mCurrentView;
        if ( view == 0 )
            return;
    }
    bool callupdate = !(view == mCurrentView);
    bool full = fullScreen;
    if(view == mCurrentView && view != mWhatsNextView ) {
        if ( mCurrentAgendaView < 0 )
            return;
        if ( view != mMonthView )
            full = mMainView->leftFrame()->isVisible();
    } else {
        if ( view == mMonthView && mMonthView)
            ;//mMonthView->skipResize = true ; 
        mCurrentView = view;
        // bool full = fullScreen;
        bool isFull = !mMainView->leftFrame()->isVisible();
        if ( isFull && KOPrefs::instance()->mViewChangeHoldFullscreen )
            full = true;
        if ( !isFull && KOPrefs::instance()->mViewChangeHoldNonFullscreen )
            full = false;
    }
    if ( mAgendaView ) mAgendaView->deleteSelectedDateTime();
    //raiseCurrentView( full );
    mMainView->processIncidenceSelection( 0 );
    //mMainView->updateView();
    raiseCurrentView( full, callupdate );
    mMainView->adaptNavigationUnits();
    mMainView->updateUnmanagedViews();
}

void KOViewManager::raiseCurrentView( bool fullScreen, bool callUpdateView  )
{
    mCurrentAgendaView = 0;
    if ( fullScreen ) {
        mMainView->leftFrame()->hide();
    } else {
        mMainView->leftFrame()->show();
    }
    //if ( mCurrentView == mMonthView )  qApp->processEvents();
    emit signalFullScreen( !fullScreen );
    if ( callUpdateView )
        mMainView->updateView();
        
    if ( globalFlagBlockAgenda ==  5 ) {
        globalFlagBlockAgenda =  4;
        globalFlagBlockAgendaItemPaint = 1;
    }
    mMainView->viewStack()->raiseWidget(mCurrentView);
    if ( globalFlagBlockAgenda ==  4 ) {
        if ( mCurrentView == mAgendaView ) {
            //globalFlagBlockAgenda =1 ;
            if ( KOPrefs::instance()->mSetTimeToDayStartAt )
                mAgendaView->setStartHour( KOPrefs::instance()->mDayBegins  );
            else if ( KOPrefs::instance()->mCenterOnCurrentTime )
                mAgendaView->setStartHour( QTime::currentTime ().hour() );
            qApp->processEvents();  
            //qDebug("qApp->processEvents() ");
            globalFlagBlockAgenda = 0;
            mAgendaView->repaintAgenda();

        } 
        globalFlagBlockAgenda = 0;
    }
    emit signalAgendaView( mCurrentView == mAgendaView );
    //qDebug("raiseCurrentView ende ");

}

void KOViewManager::clearAllViews()
{

    if ( mTodoView ) mTodoView->clearList();
    if ( mListView ) mListView->clearList();

    if ( mAgendaView ) mAgendaView->clearList();
    if ( mMonthView ) mMonthView->clearList();
    if ( mWhatsNextView ) mWhatsNextView->clearList();
    if ( mJournalView ) mJournalView->clearList();

}
void KOViewManager::updateView()
{
    // qDebug("KOViewManager::updateView() ");
    // if we are updating mTodoView, we get endless recursion
    if ( mTodoView == mCurrentView )
        return;
    if ( mCurrentView ) mCurrentView->updateView();

}

void KOViewManager::updateView(const QDate &start, const QDate &end)
{
    //  kdDebug() << "KOViewManager::updateView()" << endl;

    if (mCurrentView) mCurrentView->showDates(start, end);

    if (mTodoView && mTodoView == mCurrentView ) mTodoView->updateView();
}


void KOViewManager::updateWNview()
{
    if ( mCurrentView == mWhatsNextView && mWhatsNextView )
        mWhatsNextView->updateView();
    if ( mCurrentView == mMonthView && mMonthView )
        mMonthView->updateView();
        
}
void KOViewManager::showWhatsNextView()
{
    if (!mWhatsNextView) {
        mWhatsNextView = new KOWhatsNextView(mMainView->calendar(),mMainView->viewStack(),
                                             "KOViewManager::WhatsNextView");
        mWhatsNextView->setEventViewer( mMainView->getEventViewerDialog());
        connect(mMainView, SIGNAL(configChanged()), mWhatsNextView, SLOT(updateConfig()));
        addView(mWhatsNextView);
         connect(this, SIGNAL( printWNV() ),
             mWhatsNextView, SLOT( printMe() ) );
    }
    globalFlagBlockAgenda = 1;
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_WN_VIEW;
    showView(mWhatsNextView, true );
    //mWhatsNextView->updateView();
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_WN_VIEW;
}

void  KOViewManager::slotprintWNV()
{
    if (!mWhatsNextView) 
        showWhatsNextView();
    emit  printWNV();

}
void KOViewManager::showListView()
{
    if (!mListView) {
        mListView = new KOListView(mMainView->calendar(), mMainView->viewStack(), "KOViewManager::ListView");
        addView(mListView);

        connect(mListView, SIGNAL(showIncidenceSignal(Incidence *)),
                mMainView, SLOT(showIncidence(Incidence *)));
        connect(mListView, SIGNAL(editIncidenceSignal(Incidence *)),
                mMainView, SLOT(editIncidence(Incidence *)));
        connect(mListView, SIGNAL(deleteIncidenceSignal(Incidence *)),
                mMainView, SLOT(deleteIncidence(Incidence *)));
        connect( mListView, SIGNAL( incidenceSelected( Incidence * ) ),
                 mMainView, SLOT( processMainViewSelection( Incidence * ) ) ); 
        connect( mListView, SIGNAL( signalNewEvent() ),
                 mMainView, SLOT( newEvent() ) );
        connect(mMainView, SIGNAL(configChanged()), mListView, SLOT(updateConfig())); 
        connect( mListView, SIGNAL( cloneIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( cloneIncidence( Incidence * ) ) ); 
        connect( mListView, SIGNAL( cancelIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( cancelIncidence( Incidence * ) ) ); 
        connect( mListView, SIGNAL( moveIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( moveIncidence( Incidence * ) ) );
        connect( mListView, SIGNAL( beamIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( beamIncidence( Incidence * ) ) );
        connect( mListView, SIGNAL( beamIncidenceList( QPtrList<Incidence>  ) ),
                 mMainView, SLOT ( beamIncidenceList( QPtrList<Incidence>  ) ) );
    }
    // bool temp = mFlagShowNextxDays;
    //globalFlagBlockPainting = true;
    globalFlagBlockAgenda = 1;
    if ( KOPrefs::instance()->mListViewMonthTimespan ) {
        mMainView->setBlockShowDates( true );
        mMainView->dateNavigator()->selectMonth();
        mMainView->setBlockShowDates( false );
    }
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_L_VIEW;
    showView(mListView, KOPrefs::instance()->mFullViewTodo);
    //mFlagShowNextxDays = temp;
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_L_VIEW;
    mMainView->setScrollBarStep( 7 );
}

void KOViewManager::showAgendaView( bool fullScreen )
{

    mMainView->dialogManager()->hideSearchDialog();
    // qDebug("KOViewManager::showAgendaView ");
    bool full;
    full = fullScreen;
    if (!mAgendaView) {
        full = false; 
        mAgendaView = new KOAgendaView(mMainView->calendar(), mMainView->viewStack(), "KOViewManager::AgendaView"); 
        addView(mAgendaView);
#ifndef DESKTOP_VERSION
        QPEApplication::setStylusOperation( mAgendaView, QPEApplication::RightOnHold );
#endif
        connect( mAgendaView, SIGNAL( incidenceChanged(Incidence  *, int )),
                 mMainView, SLOT( changeIncidenceDisplay( Incidence  *, int ) ));

        // SIGNALS/SLOTS FOR DAY/WEEK VIEW

        connect(mAgendaView,SIGNAL(showDateView( int, QDate )),SLOT(showDateView( int, QDate )));

        connect(mAgendaView,SIGNAL(newTodoSignal(QDateTime,bool)),
                mMainView, SLOT(newTodoDateTime(QDateTime,bool)));
        connect(mAgendaView,SIGNAL(newEventSignal(QDateTime)),
                mMainView, SLOT(newEvent(QDateTime)));
        connect(mAgendaView,SIGNAL(newEventSignal(QDateTime,QDateTime)),
             mMainView, SLOT(newEvent(QDateTime,QDateTime)));
        connect(mAgendaView,SIGNAL(newEventSignal(QDate)),
                mMainView, SLOT(newEvent(QDate)));

        connect(mAgendaView, SIGNAL(editIncidenceSignal(Incidence *)),
                mMainView, SLOT(editIncidence(Incidence *)));
        connect(mAgendaView, SIGNAL(showIncidenceSignal(Incidence *)),
                mMainView, SLOT(showIncidence(Incidence *)));
        connect(mAgendaView, SIGNAL(deleteIncidenceSignal(Incidence *)),
                mMainView, SLOT(deleteIncidence(Incidence *)));

        connect( mAgendaView, SIGNAL( incidenceSelected( Incidence * ) ),
                 mMainView, SLOT( processMainViewSelection( Incidence * ) ) );

        connect(mAgendaView, SIGNAL( toggleExpand() ),
                mMainView, SLOT( toggleExpand() ) );
      
        connect( mAgendaView, SIGNAL( cloneIncidenceSignal(Incidence *) ), 
                 mMainView, SLOT(cloneIncidence(Incidence *) ) ) ;
        connect( mAgendaView, SIGNAL( cancelIncidenceSignal(Incidence *) ), 
                 mMainView, SLOT(cancelIncidence(Incidence *) ) ) ;
        connect(mMainView, SIGNAL(configChanged()), mAgendaView, SLOT(updateConfig())); 
        connect( mMainView, SIGNAL( todoModified( Todo *, int )), mAgendaView,
                 SLOT( updateTodo( Todo *, int ) ) );  
        connect(  mAgendaView,SIGNAL( todoMoved( Todo *, int )),
                  mMainView, SIGNAL( todoModified( Todo *, int )));
        connect( mAgendaView, SIGNAL( moveIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( moveIncidence( Incidence * ) ) );
        connect( mAgendaView, SIGNAL( beamIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( beamIncidence( Incidence * ) ) );
        connect( mAgendaView, SIGNAL( selectWeekNum( int ) ),
                 mMainView->dateNavigator(), SLOT ( selectWeek( int ) ) );
        mAgendaView->readSettings(); 
        mAgendaView->updateConfig(); 
    }
   
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_A_VIEW;
    showView( mAgendaView, full);
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_A_VIEW;
    mMainView->setScrollBarStep( 1 );
}

void KOViewManager::showDayView()
{ 
    mCurrentAgendaView = mMainView->dateNavigator()->selectedDates().count();
    mFlagShowNextxDays =  false;
    globalFlagBlockLabel = 1;
    globalFlagBlockAgenda = 1;
    if ( mCurrentAgendaView != 1 )
        mCurrentAgendaView = -1;
    showAgendaView();
    qApp->processEvents();  
    globalFlagBlockAgenda = 2;
    globalFlagBlockLabel = 0;
    mMainView->dateNavigator()->selectDates( 1 );
    mCurrentAgendaView = 1 ;
    
}

void KOViewManager::showWorkWeekView()
{
    mCurrentAgendaView = mMainView->dateNavigator()->selectedDates().count();
    mFlagShowNextxDays =  false;
    globalFlagBlockAgenda = 1;
    globalFlagBlockLabel = 1;
    if ( mCurrentAgendaView != 5 )
        mCurrentAgendaView = -1;
    showAgendaView();
    qApp->processEvents();  
    globalFlagBlockAgenda = 2;
    globalFlagBlockLabel = 0;
    mMainView->dateNavigator()->selectWorkWeek();
    mCurrentAgendaView = 5 ;
   
}

void KOViewManager::showWeekView()
{
   
    mCurrentAgendaView = mMainView->dateNavigator()->selectedDates().count();
    mFlagShowNextxDays = false;
    globalFlagBlockAgenda = 1;
    globalFlagBlockLabel = 1;
    if ( mCurrentAgendaView != 7 )
        mCurrentAgendaView = -1;
    showAgendaView();
    qApp->processEvents();  
    globalFlagBlockAgenda = 2;
    globalFlagBlockLabel = 0;  
    mMainView->dateNavigator()->selectWeek();
    mCurrentAgendaView = 7 ;
}

void KOViewManager::showNextXView()
{
      
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_NX_VIEW;
    globalFlagBlockAgenda = 1;
    if ( mCurrentAgendaView != 3 )
        mCurrentAgendaView = -1;
    showAgendaView(KOPrefs::instance()->mFullViewMonth);
    globalFlagBlockAgenda = 2;
    mMainView->dateNavigator()->selectDates( QDate::currentDate(),
                                             KOPrefs::instance()->mNextXDays );
    mFlagShowNextxDays = true;
    mCurrentAgendaView = 3 ;
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_NX_VIEW;
    mMainView->setScrollBarStep( KOPrefs::instance()->mNextXDays );
}
bool KOViewManager::showsNextDays() 
{
    return mFlagShowNextxDays;
}
void KOViewManager::createMonthView()
{
if (!mMonthView) {
        mMonthView = new KOMonthView(mMainView->calendar(), mMainView->viewStack(), "KOViewManager::MonthView");
     
        addView(mMonthView);
        // mMonthView->show();
        // SIGNALS/SLOTS FOR MONTH VIEW

        connect(mMonthView,SIGNAL(showJournalSignal( int, QDate )),SLOT(showDateView( int, QDate )));
 
        connect(mMonthView, SIGNAL(newTodoSignal(QDateTime,bool)),
                mMainView, SLOT(newTodoDateTime(QDateTime, bool)));


        connect(mMonthView, SIGNAL(newEventSignal(QDateTime)),
                mMainView, SLOT(newEvent(QDateTime)));

        connect(mMonthView, SIGNAL(showIncidenceSignal(Incidence *)),
                mMainView, SLOT(showIncidence(Incidence *)));
        connect(mMonthView, SIGNAL(editIncidenceSignal(Incidence *)),
                mMainView, SLOT(editIncidence(Incidence *)));
        connect(mMonthView, SIGNAL(deleteIncidenceSignal(Incidence *)),
                mMainView, SLOT(deleteIncidence(Incidence *)));

        connect( mMonthView, SIGNAL( incidenceSelected( Incidence * ) ),
                 mMainView, SLOT( processMainViewSelection( Incidence * ) ) );
        connect( mMonthView, SIGNAL( cloneIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( cloneIncidence( Incidence * ) ) );
        connect( mMonthView, SIGNAL( cancelIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( cancelIncidence( Incidence * ) ) );

        connect( mMonthView, SIGNAL( moveIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( moveIncidence( Incidence * ) ) );
        connect( mMonthView, SIGNAL( beamIncidenceSignal( Incidence *  ) ),
                 mMainView, SLOT ( beamIncidence( Incidence * ) ) );
        connect( mMonthView, SIGNAL( selectWeekNum( int ) ),
                 mMainView->dateNavigator(), SLOT ( selectWeekFromMonthView( int ) ) );
        connect( mMonthView, SIGNAL( selectMonth() ),
                 mMainView->dateNavigator(), SLOT ( selectMonthFromMonthview() ) );
        connect( mMonthView, SIGNAL( showDaySignal( QDate ) ),
                 mMainView, SLOT ( showDay( QDate ) ) );
        connect(mMainView, SIGNAL(configChanged()), mMonthView, SLOT(updateConfig()));
        connect( mMonthView, SIGNAL(nextMonth() ),
                 mMonthView->navigatorBar(), SIGNAL(goNextMonth() ) );
        connect( mMonthView, SIGNAL(prevMonth() ),
                 mMonthView->navigatorBar(), SIGNAL(goPrevMonth() ) );
        connect( mMonthView->navigatorBar(), SIGNAL( goPrevYear() ),
                 mMainView->dateNavigator(), SLOT( selectPreviousYear() ) );
        connect( mMonthView->navigatorBar(), SIGNAL( goNextYear() ),
                 mMainView->dateNavigator(), SLOT( selectNextYear() ) );
        connect( mMonthView->navigatorBar(), SIGNAL( goPrevMonth() ),
                 mMainView->dateNavigator(), SLOT( selectPreviousMonth() ) );
        connect( mMonthView->navigatorBar(), SIGNAL( goNextMonth() ),
                 mMainView->dateNavigator(), SLOT( selectNextMonth() ) );
        connect( mMonthView->navigatorBar(), SIGNAL( goPrevWeek() ),
                 mMainView->dateNavigator(), SLOT( selectPreviousWeek() ) );
        connect( mMonthView->navigatorBar(), SIGNAL( goNextWeek() ),
                 mMainView->dateNavigator(), SLOT( selectNextWeek() ) );
        
        connect( mMainView->dateNavigator(), SIGNAL( datesSelected( const KCal::DateList & ) ),
                 mMonthView->navigatorBar(), SLOT( selectDates( const KCal::DateList & ) ) );
        
        
        connect( mMonthView->navigatorBar(), SIGNAL( monthSelected ( int ) ),
                 mMainView->dateNavigator(), SLOT( slotMonthSelect( int ) ) );

    }
}
void KOViewManager::showMonthViewWeek()
{
    createMonthView();
    KOPrefs::instance()->mCurrentDisplayedView =  VIEW_ML_VIEW ;
    globalFlagBlockAgenda = 1;
    bool full = true;
    if ( mCurrentView == mMonthView)
        full = mMainView->leftFrame()->isVisible();
    if ( !KOPrefs::instance()->mMonthViewWeek ) {
        mMonthView->switchView();
        if ( KOPrefs::instance()->mViewChangeHoldNonFullscreen && mMainView->leftFrame()->isVisible() )
            full = false;
        else
            full = true;
    }
    mMainView->dateNavigator()->selectWeek();
    showView(mMonthView, full );
    mMonthView->setKeyBFocus();
    KOPrefs::instance()->mCurrentDisplayedView =  VIEW_ML_VIEW ;
    mMainView->setScrollBarStep( 7 );
}

void KOViewManager::showMonth( const QDate & date )
{
    mMainView->dateNavigator()->blockSignals( true );
    mMainView->dateNavigator()->selectDate( date );
    mMainView->dateNavigator()->blockSignals( false );
    showMonthView();
}
void KOViewManager::showMonthView()
 {
    
     createMonthView();
    KOPrefs::instance()->mCurrentDisplayedView =  VIEW_M_VIEW ;
    globalFlagBlockAgenda = 1;
    //mFlagShowNextxDays = false;
    bool full = true;
    if ( mCurrentView == mMonthView)
        full = mMainView->leftFrame()->isVisible();
    // if(mMonthView == mCurrentView) return; 
    if ( KOPrefs::instance()->mMonthViewWeek ) {
        mMonthView->switchView();
        if ( KOPrefs::instance()->mViewChangeHoldNonFullscreen && mMainView->leftFrame()->isVisible() )
            full = false;
        else
            full = true;
    }
    mMainView->dateNavigator()->selectMonth();

    showView(mMonthView, full );
    mMonthView->setKeyBFocus();
    KOPrefs::instance()->mCurrentDisplayedView =  VIEW_M_VIEW ;
    mMainView->setScrollBarStep( 7 );
   
}

void KOViewManager::showTodoView()
{
    //mFlagShowNextxDays = false;
    if ( !mTodoView ) {
        mTodoView = new KOTodoView( mMainView->calendar(), mMainView->viewStack(),
                                    "KOViewManager::TodoView" );
    
        addView( mTodoView ); 
        // QPEApplication::setStylusOperation( mTodoView, QPEApplication::RightOnHold );
  
        // SIGNALS/SLOTS FOR TODO VIEW
        connect( mTodoView, SIGNAL( newTodoSignal() ),
                 mMainView, SLOT( newTodo() ) );
        connect( mTodoView, SIGNAL( newSubTodoSignal( Todo * ) ),
                 mMainView, SLOT( newSubTodo( Todo *) ) );
        connect( mTodoView, SIGNAL( showTodoSignal( Todo *) ),
                 mMainView, SLOT( showTodo( Todo * ) ) );
        connect( mTodoView, SIGNAL( editTodoSignal( Todo * ) ),
                 mMainView, SLOT( editTodo( Todo * ) ) );
        connect( mTodoView, SIGNAL( deleteTodoSignal( Todo * ) ),
                 mMainView, SLOT( deleteTodo( Todo * ) ) );
        connect( mTodoView, SIGNAL( purgeCompletedSignal() ),
                 mMainView, SLOT( purgeCompleted() ) );

        connect( mTodoView, SIGNAL( incidenceSelected( Incidence * ) ),
                 mMainView, SLOT( processMainViewSelection( Incidence * ) ) );

        connect( mMainView, SIGNAL( configChanged() ), mTodoView,
                 SLOT( updateConfig() ) ); 
        connect( mMainView, SIGNAL( todoModified( Todo *, int )), mTodoView,
                 SLOT( updateTodo( Todo *, int ) ) );
        connect( mTodoView, SIGNAL( todoModifiedSignal( Todo *, int ) ),
                 mMainView, SIGNAL ( todoModified( Todo *, int ) ) );
        connect( mTodoView, SIGNAL( cloneTodoSignal( Incidence *  ) ),
                 mMainView, SLOT ( cloneIncidence( Incidence * ) ) );
        connect( mTodoView, SIGNAL( cancelTodoSignal( Incidence *  ) ),
                 mMainView, SLOT ( cancelIncidence( Incidence * ) ) );
        connect( mTodoView, SIGNAL( unparentTodoSignal( Todo * ) ),
                 mMainView, SLOT ( todo_unsub( Todo * ) ) );
        connect( mTodoView, SIGNAL( reparentTodoSignal( Todo *,Todo * ) ),
                 mMainView, SLOT ( todo_resub( Todo *, Todo *) ) );
        connect( mTodoView, SIGNAL( moveTodoSignal( Incidence *  ) ),
                 mMainView, SLOT ( moveIncidence( Incidence * ) ) );
        connect( mTodoView, SIGNAL( beamTodoSignal( Incidence *  ) ),
                 mMainView, SLOT ( beamIncidence( Incidence * ) ) );
        KConfig *config = KOGlobals::config();
        mTodoView->restoreLayout(config,"Todo View");
        mTodoView->setNavigator( mMainView->dateNavigator() );
    }

    KOPrefs::instance()->mCurrentDisplayedView =  VIEW_T_VIEW ;
    globalFlagBlockAgenda = 1;
    showView( mTodoView, true );
    KOPrefs::instance()->mCurrentDisplayedView =  VIEW_T_VIEW ;
    mMainView->setScrollBarStep( 7 );

}

void KOViewManager::showJournalView()
{
    //mFlagShowNextxDays = false;
    if (!mJournalView) {
        mJournalView = new KOJournalView(mMainView->calendar(),mMainView->viewStack(),
                                         "KOViewManager::JournalView"); 
        connect( mMainView, SIGNAL( configChanged() ), mJournalView,
                 SLOT( updateConfig() ) );
        connect(mJournalView, SIGNAL(deleteJournal(Journal *) ), mMainView,SLOT(deleteJournal(Journal *)) );
        addView(mJournalView);
    }

    KOPrefs::instance()->mCurrentDisplayedView = VIEW_J_VIEW  ;
    showView(mJournalView);
    mMainView->dateNavigator()->selectDates( 1 );
    KOPrefs::instance()->mCurrentDisplayedView = VIEW_J_VIEW  ;
    mMainView->setScrollBarStep( 1 );
}

void KOViewManager::showTimeSpanView()
{
    //mFlagShowNextxDays = false;
    if (!mTimeSpanView) {
        mTimeSpanView = new KOTimeSpanView(mMainView->calendar(),mMainView->viewStack(),
                                           "KOViewManager::TimeSpanView");
        addView(mTimeSpanView);

        mTimeSpanView->readSettings();
    }

    showView(mTimeSpanView);
}

Incidence *KOViewManager::currentSelection()
{
    if (!mCurrentView) return 0;
    if ( mCurrentView == mListView ) {
        if ( mListView->currentItem() )
            return mListView->currentItem();
    }
    return mCurrentView->selectedIncidences().first();
}

QDate KOViewManager::currentSelectionDate()
{
    QDate qd;
    if (mCurrentView) {
        DateList qvl = mCurrentView->selectedDates();
        if (!qvl.isEmpty()) qd = qvl.first();
    }
    return qd;
}

void KOViewManager::addView(KOrg::BaseView *view)
{
#if QT_VERSION >= 0x030000
    mMainView->viewStack()->addWidget( view );
#else
    mMainView->viewStack()->addWidget( view, 1 );
#endif
}

void KOViewManager::setDocumentId( const QString &id )
{
    if (mTodoView) {
      mTodoView->setDocumentId( id );
    }
}
