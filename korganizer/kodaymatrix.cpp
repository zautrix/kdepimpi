/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Eitzenberger Thomas <thomas.eitzenberger@siemens.at>
    Parts of the source code have been copied from kdpdatebutton.cpp

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
    without including the source ode for Qt in the source distribution.
*/

#include <qevent.h>
#include <qpainter.h>
#include <qptrlist.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include <kglobal.h>
#include <kdebug.h>
#include <klocale.h>

#include <libkcal/vcaldrag.h>
#include <libkcal/icaldrag.h>
#include <libkcal/dndfactory.h>
#include <libkcal/calendarresources.h>
#include <libkcal/resourcecalendar.h>
#include <kresources/resourceselectdialog.h>

#include <kcalendarsystem.h>

#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif
#include "koprefs.h"
#include "koglobals.h"

#include "kodaymatrix.h"

// ============================================================================
//  D Y N A M I C   T I P
// ============================================================================

DynamicTip::DynamicTip( QWidget * parent )
    : QToolTip( parent )
{
    matrix = (KODayMatrix*)parent;
}

class KODaymatrixWhatsThis :public QWhatsThis
{
public:
    KODaymatrixWhatsThis( KODayMatrix* view ) : QWhatsThis( view ),_view (view) { ;};
    ~KODaymatrixWhatsThis() { ;  };

protected:
  virtual QString text( const QPoint& p )
  {
    return _view->getWhatsThisText( p ) ;
  }
private:
  KODayMatrix * _view;
};

void DynamicTip::maybeTip( const QPoint &pos )
{
  //calculate which cell of the matrix the mouse is in
  QRect sz = matrix->frameRect();
  int dheight = sz.height()*7 / 42;
  int dwidth = sz.width() / 7;
  int row = pos.y()/dheight;
  int col = pos.x()/dwidth;

  QRect rct(col*dwidth, row*dheight, dwidth, dheight);

//  kdDebug() << "DynamicTip::maybeTip matrix cell index [" <<
//                col << "][" << row << "] => " <<(col+row*7) << endl;

  //show holiday names only
  QString str = matrix->getHolidayLabel(col+row*7);
  if (str.isEmpty()) return;
  tip(rct, str);
}


// ============================================================================
//  K O D A Y M A T R I X
// ============================================================================

const int KODayMatrix::NOSELECTION = -1000;
const int KODayMatrix::NUMDAYS = 42;

KODayMatrix::KODayMatrix( QWidget *parent, const char *name )
    : QFrame( parent, name , Qt::WRepaintNoErase ), mCalendar( 0 )


{ 
    mLastView = -1;
    oldW = 0;
    oldH = 0;
    myPix.resize( 150, 120 );
    mRedrawNeeded = true;
  mKODaymatrixWhatsThis =  new KODaymatrixWhatsThis(this);
  mPendingUpdateBeforeRepaint = false;
  mouseDown = false;
  // initialize dynamic arrays
  bDays.resize ( NUMDAYS );
  pDays.resize ( NUMDAYS );
  hDays.resize ( NUMDAYS );
  eDays.resize ( NUMDAYS );
  days = new QDate[NUMDAYS];
  daylbls = new QString[NUMDAYS];
  //events = new int[NUMDAYS];
  mToolTip = new DynamicTip(this);

  // set default values used for drawing the matrix
  mDefaultBackColor = palette().active().base();
  mDefaultTextColor = palette().active().foreground();
  mDefaultTextColorShaded = getShadedColor(mDefaultTextColor);
  mHolidayColorShaded = getShadedColor(KOPrefs::instance()->mHolidayColor);
  mSelectedDaysColor = QColor("white");
  mTodayMarginWidth = 2;
  mSelEnd = mSelStart = NOSELECTION;

  setAcceptDrops(true);
  //setFont( QFont("Arial", 10) );

  mUpdateTimer = new QTimer( this );
  connect (mUpdateTimer ,SIGNAL(timeout()), this, SLOT ( updateViewTimed() ));
  mRepaintTimer = new QTimer( this );
  connect (mRepaintTimer ,SIGNAL(timeout()), this, SLOT ( repaintViewTimed() ));
  mDayChanged = false;
  updateView();
}
QString KODayMatrix::getWhatsThisText( QPoint p )
{

    int tmp = getDayIndexFrom(p.x(), p.y());
    if ( tmp < 0 || tmp > NUMDAYS-1 || !mCalendar )
        return QString();
    QDate mDate = days[tmp];
    QPtrList<Event> eventlist = mCalendar->events(mDate);
    Event *event;
    QStringList mToolTip;
    for(event=eventlist.first();event != 0;event=eventlist.next()) {
        QString mToolTipText;
        QString text;
        int multiday = 0;// 1 = start, 2 = midddle, 3 = end day
        if (event->isMultiDay()) {
            QString prefix = "<->";multiday = 2; 
            QString time;
            if ( event->doesRecur() ) {
                if ( event->recursOn( mDate) ) {
                    prefix ="->" ;multiday = 1;
                }
                else {
                    int days =  event->dtStart().date().daysTo ( event->dtEnd().date() );
                    if ( event->recursOn( mDate.addDays( -days)) ) {
                        prefix ="<-" ;multiday = 3;
                    }
                } 
            } else {
                if (mDate == event->dtStart().date()) {
                    prefix ="->" ;multiday = 1;
                } else if (mDate == event->dtEnd().date()) {
                    prefix ="<-" ;multiday = 3;
                }
            }
            if ( !event->doesFloat() ) {
                if ( mDate == event->dtStart().date () )
                    time = KGlobal::locale()->formatTime(event->dtStart().time())+" ";
                else if ( mDate == event->dtEnd().date () )
                    time = KGlobal::locale()->formatTime(event->dtEnd().time())+" ";

            }
            text = time + event->summary();
            mToolTipText += prefix + text;
        } else {
            if (event->doesFloat()) {
                text = event->summary();
                mToolTipText += text;
            }
            else {
                text = KGlobal::locale()->formatTime(event->dtStart().time());
                text += " " + event->summary();
                mToolTipText +=  KGlobal::locale()->formatTime(event->dtStart().time()) +"-"+KGlobal::locale()->formatTime(event->dtEnd().time())+" " + event->summary();
            }
        }
        if ( !event->location().isEmpty() )
            mToolTipText += " (" + event->location() + ")";
        //qDebug("TTT: %s ", mToolTipText.latin1());
        mToolTip.append( deTag( mToolTipText ) );
    }
    mToolTip.sort();
    if ( KOPrefs::instance()->mCurrentDisplayedView == VIEW_T_VIEW ) {
        QPtrList<Todo> todolist = mCalendar->todos(mDate);
        Todo  *todo;
        for(todo=todolist.first();todo != 0;todo=todolist.next()) { 
            QString mToolTipText;
            if ( !todo->doesFloat() )
                mToolTipText += KGlobal::locale()->formatTime(todo->dtDue().time())+" ";
            mToolTipText +=  todo->summary();
            if ( !todo->location().isEmpty() )
                mToolTipText += " (" + todo->location() + ")";
            mToolTipText = deTag( mToolTipText);
            mToolTipText = "<b>" + i18n("Todo: ") + "</b>"+ mToolTipText;
            mToolTip.append(  mToolTipText );
        }               
    }
    if (KOPrefs::instance()->mCurrentDisplayedView == VIEW_J_VIEW ) {
        QPtrList<Journal> j_list = mCalendar->journals4Date( mDate );
        Journal *j = j_list.first();
        while ( j ) {
            QString mToolTipText;
            if ( !j->summary().isEmpty() ) {
                mToolTipText = j->summary().left(30);
                if ( j->summary().length() > 30 )
                    mToolTipText += " ...";
            } else {
                mToolTipText = j->description().left(25);
                if ( j->description().length() > 25 )
                    mToolTipText += " ...";
                mToolTipText = deTag( mToolTipText);
            }
            mToolTipText = "<b>" + i18n("Journal: ") + "</b>"+ mToolTipText;
            mToolTip.append(  mToolTipText );
            j = j_list.next();
        }
    }
    return  "<b>"+KGlobal::locale()->formatDate(days[tmp]) + "</b><br>" + mToolTip.join("<br>");
}


QString KODayMatrix::deTag( QString mToolTipText  )
{

#if QT_VERSION >= 0x030000
    mToolTipText.replace( '<' , "&lt;" );
    mToolTipText.replace( '>' , "&gt;" );
#else
    if ( mToolTipText.find ('<') >= 0 ) {
        mToolTipText.replace( QRegExp("<") , "&lt;" );
    }
    if ( mToolTipText.find ('>') >= 0 ) {
        mToolTipText.replace( QRegExp(">") , "&gt;" );
    }
#endif
    return mToolTipText;
}
void KODayMatrix::setCalendar( Calendar *cal )
{
  mCalendar = cal;

  setAcceptDrops( mCalendar );

  updateEvents();
}

QColor KODayMatrix::getShadedColor(QColor color)
{
  QColor shaded;
  int h=0;
  int s=0;
  int v=0;
  color.hsv(&h,&s,&v);
  s = s/4;
  v = 192+v/4;
  shaded.setHsv(h,s,v);

  return shaded;
}

KODayMatrix::~KODayMatrix()
{
#if QT_VERSION >= 0x030000

#else
   delete mKODaymatrixWhatsThis;
#endif 

    //  delete mKODaymatrixWhatsThis;
  delete [] days;
  delete [] daylbls;
  //delete [] events;
  delete mToolTip;
}

/*
void KODayMatrix::setStartDate(QDate start)
{
  updateView(start);
}
*/

void KODayMatrix::addSelectedDaysTo(DateList& selDays)
{
 
  if (mSelStart == NOSELECTION) {
    return;
  }

  //cope with selection being out of matrix limits at top (< 0)
  int i0 = mSelStart;
  if (i0 < 0) {
    for (int i = i0; i < 0; i++) {
      selDays.append(days[0].addDays(i));
    }
    i0 = 0;
  }

  //cope with selection being out of matrix limits at bottom (> NUMDAYS-1)
  if (mSelEnd > NUMDAYS-1) {
    for (int i = i0; i <= NUMDAYS-1; i++) {
      selDays.append(days[i]);
    }
    for (int i = NUMDAYS; i < mSelEnd; i++) {
      selDays.append(days[0].addDays(i));
    }

  // apply normal routine to selection being entirely within matrix limits
  } else {
    for (int i = i0; i <= mSelEnd; i++) {
      selDays.append(days[i]);
    }
  }
}

bool KODayMatrix::setSelectedDaysFrom(const QDate& start, const QDate& end)
{  
    mRedrawNeeded = true;
    bool noSel = (mSelEnd == NOSELECTION && mSelStart == NOSELECTION );
  mSelStart = startdate.daysTo(start);
  if ( mSelStart < 0 )
      mSelStart = 0;
  mSelEnd = startdate.daysTo(end);
  if ( mSelEnd  > NUMDAYS-1 )
      mSelEnd = NUMDAYS-1;
  if ( mSelEnd < 0 || mSelStart >  NUMDAYS-1 ) {
      clearSelection();
      if ( noSel )
          return false;
  }
 
  return true;
}
void KODayMatrix::clearSelection()
{
    mSelEnd = mSelStart = NOSELECTION;
}


void KODayMatrix::recalculateToday()
{
    today = -1;
    for (int i=0; i<NUMDAYS; i++) {
        //events[i] = 0;
      days[i] = startdate.addDays(i);
      daylbls[i] = QString::number( KOGlobals::self()->calendarSystem()->day( days[i] ));
      
      // if today is in the currently displayed month, hilight today
      if (days[i].year() == QDate::currentDate().year() &&
          days[i].month() == QDate::currentDate().month() &&
          days[i].day() == QDate::currentDate().day()) {
        today = i;
      }
    }
    // qDebug(QString("Today is visible at %1.").arg(today));
}

void KODayMatrix::updateView()
{
  updateView(startdate);
}
void KODayMatrix::repaintViewTimed()
{ 
    mRedrawNeeded = true;
    // bDays.fill( false);
    //pDays.fill( false);
    //hDays.fill( false);
    //eDays.fill( false);
    mRepaintTimer->stop(); 
    int startDay = days[0].dayOfWeek(); // 1...7 7 = sunday
    int i;
    for(i = 0; i < NUMDAYS; i++) {
        if ( ( (i+startDay) % 7 == 0 ) ) {
            pDays.setBit(i);
        }
    }
    repaint(false);
}
void KODayMatrix::computeEvent(Event *event, int i )
{
    QString holiStr = mHolidays[i];
    if ( event->isHoliday()) {
        pDays.setBit(i);
        hDays.setBit(i);
        if ( !holiStr.isEmpty() )
            holiStr += "\n";
        holiStr += event->summary();
        if ( !event->location().isEmpty() )
            holiStr += " (" + event->location() + ")";
        mHolidays[i] =holiStr ;
        eDays.setBit(i);
    }
    if ( KOPrefs::instance()->mCurrentDisplayedView != VIEW_T_VIEW && KOPrefs::instance()->mCurrentDisplayedView != VIEW_J_VIEW ) {
        if ( event->isBirthday()) {
            pDays.setBit(i);
            if ( !holiStr.isEmpty() )
                holiStr += "\n";
            holiStr += i18n("Birthday") + ": "+event->summary();
            if ( !event->location().isEmpty() )
                holiStr += " (" + event->location() + ")";
            bDays.setBit(i);
            mHolidays[i] =holiStr ;
        }
    }
    if ( KOPrefs::instance()->mCurrentDisplayedView != VIEW_J_VIEW )
        eDays.setBit(i);
}
void KODayMatrix::updateViewTimed()
{
    mUpdateTimer->stop(); 
    if ( !mCalendar ) {
        qDebug("NOT CAL ");
        return;
    }
#if 1

   
 int i;
    int timeSpan = NUMDAYS-1;
    QPtrList<Event> events = mCalendar->events();
    Event *event;
    QDateTime dt;
    bool ok;
    bDays.fill( false);
    pDays.fill( false);
    hDays.fill( false);
    eDays.fill( false);
    mHolidays.clear();
    QDate mStartDate = days[0];
    QDate endDate =  mStartDate.addDays( timeSpan );
    for( event = events.first(); event; event = events.next() ) { // for event
        ushort recurType = event->doesRecur();
        if ((recurType == Recurrence::rDaily && !KOPrefs::instance()->mDailyRecur) ||
            (recurType == Recurrence::rWeekly && !KOPrefs::instance()->mWeeklyRecur)) {
            continue;
        }
        if ( !KOPrefs::instance()->mLongAllday && event->doesFloat()&& event->isMultiDay() ) {
            continue;
        }
        if ( event->doesRecur() ) {
            bool last;
            QDateTime incidenceStart = event->recurrence()->getPreviousDateTime( QDateTime( mStartDate ) , &last );
            QDateTime incidenceEnd;
            int eventlen = event->dtStart().date().daysTo ( event->dtEnd().date()  );
            bool invalid = false;
            while( true ) {
                    if ( incidenceStart.isValid() ) {
                        incidenceEnd = incidenceStart.addDays( eventlen );
                        int st =   incidenceStart.date().daysTo( endDate );
                        if ( st >= 0 ) { // start before timeend
                            int end = mStartDate.daysTo( incidenceEnd.date() );
                            if ( end >= 0 ) { // end after timestart --- got one!
                                //normalize
                                st =  timeSpan - st;
                                if ( st < 0 ) st = 0;
                                if ( end > timeSpan ) end = timeSpan;
                                int iii;
                                //qDebug("found %s %d %d ",event->summary().latin1(), st, end );
                                for ( iii = st;iii<= end;++iii) {
                                    computeEvent( event, iii );
                                }
                            }
                        }
                    } else {
                        if ( invalid )
                            break;
                        invalid = true;
                        //qDebug("invalid %s", event->summary().latin1());
                        incidenceStart = QDateTime( mStartDate ).addSecs( -2 );;
                    }
                    if ( last )
                        break;
                    bool ok;
                    incidenceStart = event->getNextOccurence( incidenceStart.addSecs( 1 ) ,&ok );
                    if ( ! ok )
                        break;
                    if ( incidenceStart.date() > endDate )
                        break;
            }
        } else { // no recur 
            int st =  event->dtStart().date().daysTo( endDate );
            if ( st >= 0 ) { // start before timeend
                int end = mStartDate.daysTo( event->dtEnd().date() );
                if ( end >= 0 ) { // end after timestart --- got one!
                    //normalize
                    st =  timeSpan - st;
                    if ( st < 0 ) st = 0;
                    if ( end > timeSpan ) end = timeSpan;
                    int iii;
                    for ( iii = st;iii<= end;++iii)
                        computeEvent( event, iii );
                }
            }
        }
    }
    int startDay = days[0].dayOfWeek(); // 1...7 7 = sunday
    for(i = 0; i < NUMDAYS; i++) {
        if ( ( (i+startDay) % 7 == 0 ) ) {
            pDays.setBit(i);
        }
    }
    if ( KOPrefs::instance()->mCurrentDisplayedView == VIEW_T_VIEW  ) {
        bDays.fill( false);
        // insert due todos
        QPtrList<Todo> todos = mCalendar->todos( );
        Todo *todo;
        for(todo = todos.first(); todo; todo = todos.next()) {
            //insertTodo( todo );
            if ( todo->hasDueDate() ) {
                int day = mStartDate.daysTo( todo->dtDue().date() );
                if ( day >= 0 && day < timeSpan + 1)  {
                    int i = day;
                    QString holiStr = mHolidays[i];
                    pDays.setBit(i);
                    if ( !holiStr.isEmpty() )
                        holiStr += "\n";
                    holiStr += i18n("Todo") + ": "+todo->summary();
                    if ( !todo->location().isEmpty() )
                        holiStr += " (" + todo->location() + ")";
                    bDays.setBit(i);
                    mHolidays[i] =holiStr ;
                    eDays.setBit(i);
                }
            }
        }
    } 
    if ( KOPrefs::instance()->mCurrentDisplayedView == VIEW_J_VIEW  ) {
        bDays.fill( false);
        // insert due todos
        QPtrList<Journal> todos = mCalendar->journals( );
        Journal *todo;
        for(todo = todos.first(); todo; todo = todos.next()) {
            int day = mStartDate.daysTo( todo->dtStart().date() );
            if ( day >= 0 && day < timeSpan + 1)  {
                int i = day;
                QString holiStr = mHolidays[i];
                pDays.setBit(i);
                if ( !holiStr.isEmpty() )
                    holiStr += "\n";
                holiStr += i18n("Journal: ")+todo->summary().left(25);
                if ( todo->summary().length() > 25 )
                    holiStr +="...";
                bDays.setBit(i);
                mHolidays[i] =holiStr ;
                eDays.setBit(i);
                
            }
        }
    }
#else
    //qDebug("KODayMatrix::updateViewTimed ");
    for(int i = 0; i < NUMDAYS; i++) {
        // if events are set for the day then remember to draw it bold
        QPtrList<Event> eventlist = mCalendar->events(days[i]);
        Event *event;
        int numEvents = eventlist.count();
        QString holiStr = "";
        bDays.clearBit(i);
        hDays.clearBit(i);
        eDays.clearBit(i);
        for(event=eventlist.first();event != 0;event=eventlist.next()) {
            ushort recurType = event->recurrence()->doesRecur();
            if ((recurType == Recurrence::rDaily && !KOPrefs::instance()->mDailyRecur) ||
                (recurType == Recurrence::rWeekly && !KOPrefs::instance()->mWeeklyRecur)) {
                numEvents--;
            }
            if ( event->isHoliday()) {
                hDays.setBit(i);
                if ( !holiStr.isEmpty() )
                    holiStr += "\n";
                holiStr += event->summary();
                if ( !event->location().isEmpty() )
                     holiStr += " (" + event->location() + ")";
            }
            if ( event->isBirthday()) {
                if ( !holiStr.isEmpty() )
                    holiStr += "\n";
                holiStr += i18n("Birthday") + ": "+event->summary();
                if ( !event->location().isEmpty() )
                     holiStr += " (" + event->location() + ")";
                bDays.setBit(i);
            }
        }
        if ( numEvents ) 
            eDays.setBit(i);
        //if it is a holy day then draw it red. Sundays are consider holidays, too
        if ( (KOGlobals::self()->calendarSystem()->dayOfWeek(days[i]) == KOGlobals::self()->calendarSystem()->weekDayOfPray()) ||
             !holiStr.isEmpty()) {
            mHolidays[i] = holiStr;
        } else {
            mHolidays[i] = QString::null;
        }
    }
#endif
    mRedrawNeeded = true;
    if ( ! mPendingUpdateBeforeRepaint )
        repaint(false);
}
void KODayMatrix::updateView(QDate actdate)
{
    if ( mLastView != KOPrefs::instance()->mCurrentDisplayedView )
        mRedrawNeeded = true;
    mLastView = KOPrefs::instance()->mCurrentDisplayedView;
    if ( ! actdate.isValid() ) {
        //qDebug("date not valid ");
        return;
    }
    mDayChanged = false;
    //flag to indicate if the starting day of the matrix has changed by this call
    //mDayChanged = false;
    // if a new startdate is to be set then apply Cornelius's calculation
    // of the first day to be shown
    if (actdate != startdate) {
        // reset index of selection according to shift of starting date from startdate to actdate
        if (mSelStart != NOSELECTION) {
            int tmp = actdate.daysTo(startdate);
            //kdDebug() <<  "Shift of Selection1: " << mSelStart << " - " << mSelEnd << " -> " << tmp << "(" << offset << ")" << endl;
            // shift selection if new one would be visible at least partly !

            if (mSelStart+tmp < NUMDAYS && mSelEnd+tmp >= 0) {
                // nested if is required for next X display pushed from a different month - correction required
                // otherwise, for month forward and backward, it must be avoided
                if( mSelStart > NUMDAYS || mSelStart < 0 )
                    mSelStart = mSelStart + tmp;
                if( mSelEnd > NUMDAYS || mSelEnd < 0 )
                    mSelEnd = mSelEnd + tmp;
            }	
        }
        startdate = actdate;
        mDayChanged = true;
        recalculateToday();
        mRedrawNeeded = true;
    }
    //qDebug("restart Timer %d vis: %d", mDayChanged, isVisible() );
    if ( !isVisible() ) {
        mPendingUpdateBeforeRepaint = true; 
    } else {
#ifdef DESKTOP_VERSION
        //mRepaintTimer->start( 100 );
        //updateViewTimed();
     mUpdateTimer->start( 50 );
#else
        mRepaintTimer->start( 250 ); 
        mUpdateTimer->start( 500 );
#endif
    }
}
void KODayMatrix::updateEvents()
{
  if ( !mCalendar ) return;

  for( int i = 0; i < NUMDAYS; i++ ) {
    // if events are set for the day then remember to draw it bold
    QPtrList<Event>  eventlist = mCalendar->events( days[ i ] );
    int numEvents = eventlist.count();
    Event *event;
    for( event = eventlist.first(); event != 0;event=eventlist.next()) {
      ushort recurType = event->doesRecur();

      if ( ( recurType == Recurrence::rDaily &&
             !KOPrefs::instance()->mDailyRecur ) ||
           ( recurType == Recurrence::rWeekly &&
             !KOPrefs::instance()->mWeeklyRecur ) ) {
        numEvents--;
      }
      if ( !KOPrefs::instance()->mLongAllday && event->doesFloat()&& event->isMultiDay() ) {
          numEvents--;
      }
    } 
    if ( numEvents ) 
        eDays.setBit(i);
    else
        eDays.clearBit(i);
  }
}

const QDate& KODayMatrix::getDate(int offset)
{
  if (offset < 0 || offset > NUMDAYS-1) {
      qDebug("Wrong offset2 %d", offset);
    return days[0];
  }
  return days[offset];
}

QString KODayMatrix::getHolidayLabel(int offset)
{
  if (offset < 0 || offset > NUMDAYS-1) {
      qDebug("Wrong offset1 %d", offset);
      return QString();
  }
  return mHolidays[offset];
}

int KODayMatrix::getDayIndexFrom(int x, int y)
{ 
  int colModulo = (width()-2)  % 7;
  int rowModulo = (height()-2)  % 6;
#if 0
  return 7*(y/daysize.height()) + (KOGlobals::self()->reverseLayout() ? 
            6 - x/daysize.width() : x/daysize.width());
#endif
  int xVal = (x-colModulo/2-2)/daysize.width();
  int yVal = (y-rowModulo/2-2)/daysize.height();
  
  
  return 7*(yVal) + xVal;

}

// ----------------------------------------------------------------------------
//  M O U S E   E V E N T   H A N D L I N G
// ----------------------------------------------------------------------------

void KODayMatrix::mousePressEvent (QMouseEvent* e)
{
  
    if ( e->button() == LeftButton )
        mouseDown = true; 
  mSelStart = getDayIndexFrom(e->x(), e->y());
  if (mSelStart > NUMDAYS-1) mSelStart=NUMDAYS-1;
  mSelInit = mSelStart;
  mSelEnd = mSelStart; 
  mRedrawNeeded = true;
  repaint(false);
}

void KODayMatrix::mouseReleaseEvent (QMouseEvent* e)
{
  mRedrawNeeded = true;
  if ( e->button() == LeftButton )
      if ( ! mouseDown ) {
          return;
      }
      else
          mouseDown = false; 
  int tmp = getDayIndexFrom(e->x(), e->y());
  if (tmp > NUMDAYS-1) tmp=NUMDAYS-1;

  if (mSelInit > tmp) {
    mSelEnd = mSelInit;
    if (tmp != mSelStart) {
      mSelStart = tmp;
      repaint(false);
    }
  } else {
    mSelStart = mSelInit;

    //repaint only if selection has changed
    if (tmp != mSelEnd) {
      mSelEnd = tmp;
      repaint(false);
    }
  }

  DateList daylist;
    if ( mSelStart < 0 )
        mSelStart = 0;
  for (int i = mSelStart; i <= mSelEnd; i++) {
    daylist.append(days[i]);
  }
  emit selected((const DateList)daylist);

}

void KODayMatrix::mouseMoveEvent (QMouseEvent* e)
{ 
    if ( ! mouseDown ) {
        return;
    }
  mRedrawNeeded = true;
  int tmp = getDayIndexFrom(e->x(), e->y());
  if (tmp > NUMDAYS-1) tmp=NUMDAYS-1;

  if (mSelInit > tmp) {
    mSelEnd = mSelInit;
    if (tmp != mSelStart) {
      mSelStart = tmp;
      repaint(false);
    }
  } else {
    mSelStart = mSelInit;

    //repaint only if selection has changed
    if (tmp != mSelEnd) {
      mSelEnd = tmp;
      repaint(false);
    }
  }
}

// ----------------------------------------------------------------------------
//  D R A G ' N   D R O P   H A N D L I N G
// ----------------------------------------------------------------------------

void KODayMatrix::dragEnterEvent(QDragEnterEvent *e)
{
#ifndef KORG_NODND
  if ( !ICalDrag::canDecode( e ) && !VCalDrag::canDecode( e ) ) {
    e->ignore();
    return;
  }

  // some visual feedback
//  oldPalette = palette();
//  setPalette(my_HilitePalette);
//  update();
#endif
}

void KODayMatrix::dragMoveEvent(QDragMoveEvent *e)
{
#ifndef KORG_NODND
  if ( !ICalDrag::canDecode( e ) && !VCalDrag::canDecode( e ) ) {
    e->ignore();
    return;
  }

  e->accept();
#endif
}

void KODayMatrix::dragLeaveEvent(QDragLeaveEvent */*dl*/)
{
#ifndef KORG_NODND
//  setPalette(oldPalette);
//  update();
#endif
}

void KODayMatrix::dropEvent(QDropEvent *e)
{
#ifndef KORG_NODND
//  kdDebug() << "KODayMatrix::dropEvent(e) begin" << endl;

  if (!mCalendar || !ICalDrag::canDecode( e ) && !VCalDrag::canDecode( e ) ) {
    e->ignore();
    return;
  }

  DndFactory factory( mCalendar );
  Event *event = factory.createDrop(e);

  if (event) {
    e->acceptAction();

    Event *existingEvent = mCalendar->event(event->uid());

    if(existingEvent) {
      // uniquify event
      event->recreate();
/*
      KMessageBox::sorry(this,
              i18n("Event already exists in this calendar."),
              i18n("Drop Event"));
      delete event;
      return;
*/
    }
//      kdDebug() << "Drop new Event" << endl;
    // Adjust date
    QDateTime start = event->dtStart();
    QDateTime end = event->dtEnd();
    int duration = start.daysTo(end);
    int idx = getDayIndexFrom(e->pos().x(), e->pos().y());

    start.setDate(days[idx]);
    end.setDate(days[idx].addDays(duration));

    event->setDtStart(start);
    event->setDtEnd(end);
    mCalendar->addEvent(event);

    emit eventDropped(event);
  } else {
//    kdDebug() << "KODayMatrix::dropEvent(): Event from drop not decodable" << endl;
    e->ignore();
  }
#endif
}

// ----------------------------------------------------------------------------
//  P A I N T   E V E N T   H A N D L I N G
// ----------------------------------------------------------------------------

void KODayMatrix::paintEvent(QPaintEvent * pevent)
{
  
    if ( width() <= 0  || height() <= 0 )
        return;
    if ( mPendingUpdateBeforeRepaint ) {
        updateViewTimed();
        mPendingUpdateBeforeRepaint = false;
    }
#if 0
    if ( myPix.width() != width() || myPix.height()!=height() ) {
        myPix.resize(size() );
        mRedrawNeeded = true;
    }
#endif 
    if ( oldW != width() || oldH !=height() )
        mRedrawNeeded = true;
    
    oldH = height() ;
    oldW = width();
    if ( myPix.width() < width() || myPix.height() < height() ) {
        myPix.resize(size() );
        mRedrawNeeded = true;
        
    }
    
    if ( mRedrawNeeded ) {
        //qDebug("REDRAW ");
        QPainter p(&myPix);
        p.setFont(font());


        int dheight = daysize.height();
        int dwidth = daysize.width();
        int row,col;
        int selw, selh;
        int xyOff = frameWidth();
        int colModulo = (width()-2)  % 7;
        int rowModulo = (height()-2)  % 6;
        //qDebug("col %d  row %d ",colModulo,rowModulo  );
  
        bool isRTL = KOGlobals::self()->reverseLayout();

        // draw background and topleft frame
        p.fillRect(0,0,width(),height(), mDefaultBackColor);
        //p.setPen(mDefaultTextColor);
        //p.drawRect(0, 0, width(), height());
        int mSelStartT = mSelStart;
        int mSelEndT = mSelEnd;
        if ( mSelEndT >=  NUMDAYS )
            mSelEndT = NUMDAYS-1;
        // draw selected days with highlighted background color
        if (mSelStart != NOSELECTION) {
            bool skip = false;
            if ( ! mouseDown ) {
                int mo = days[20].month();
                //qDebug("--  %d %d ", mSelStartT, mSelEndT);
                //qDebug("%d %d %d - d %d", mo,  days[mSelStartT].month() , days[mSelEndT].month(),  days[mSelEndT].day() );
                int startMo = days[mSelStartT].month();
                int endMo = days[mSelEndT].month();
                if ( startMo == 12 && mo == 1 && endMo <= 2 )
                    startMo = 1;
                if ( endMo == 1 && mo == 12 )
                    endMo = 12;
                if ( mo == 12 && startMo == 1 )
                    startMo = 13;
                if ( (startMo > mo || endMo < mo) ) {
                    skip = true;
                } else {
                    if ( days[mSelStartT].month() != mo ) {
                        int add = days[mSelStartT].daysInMonth ()-days[mSelStartT].day();
                        mSelStartT += add +1;
                    }
                    if ( days[mSelEndT].month() != mo ) {
                        int sub = days[mSelEndT].day();
                        mSelEndT -= sub ;
                    }
                }
            }
            //qDebug("SKIP %d ", skip);
            if ( ! skip ) {
                row = mSelStartT/7;
                col = mSelStartT -row*7;
                QColor selcol = KOPrefs::instance()->mHighlightColor;
                int addCol = 0;
                int addRow = 0; 
                int addRow2 = 0; 
                int addCol2 = 0;
                if (row == mSelEndT/7) { 
                    if ( rowModulo ) {
                        if ( row >= 6 - rowModulo )
                            addRow = row - 5 + rowModulo;
                    }  
                    if ( colModulo ) {
                        int colt1 = mSelEndT%7;
                        //qDebug("colt1 %d ", colt1 );
                        if ( colt1 >= 7 - colModulo )
                            addCol =  colt1 - 7 + colModulo+1; 
                        int colt = mSelStartT%7;
                        if ( colt >= 7 - colModulo )
                            addCol2 =  colt - 7 + colModulo;
                        addCol -= addCol2;
                        //qDebug("COL %d %d %d %d ",col , colt1 ,addCol ,addCol2 );
                    }
                    // Single row selection
                    //if ( row == 0)
                    //  addRow = 1;
                    p.fillRect(isRTL ? (7 - (mSelEndT-mSelStartT+1) - col)*dwidth : col*dwidth+1+addCol2,
                               row*dheight+addRow, (mSelEndT-mSelStartT+1)*dwidth+addCol, dheight+1, selcol);
                } else {
                    // draw first row to the right
                    if ( colModulo ) {
                        if ( col >= 7 - colModulo )
                            addCol2 =  col - 7 + colModulo;
                    }
                    if ( rowModulo ) {
                        if ( row >= 6 - rowModulo )
                            addRow = row - 5 + rowModulo;
                    }  
                    //if ( row == 0)
                    //  addRow = 1; 
                    int drawWid = width()-(col*dwidth+1+addCol2)-1;
                    p.fillRect(isRTL ? 0 : col*dwidth+1+addCol2, row*dheight+addRow, drawWid,
                               dheight+1, selcol);
                    // draw full block till last line
                    selh = mSelEndT/7-row; 
                    addRow = 0;
                    if ( rowModulo ) {
                        if ( mSelEndT/7 >= 6 - rowModulo )
                            addRow = mSelEndT/7 - 5 + rowModulo;
                    }  
                    //qDebug("%d %d %d ",selh, row, addRow );
                    int addrow2 = addRow-selh+1;
                    if ( addrow2 < 0 )
                         addrow2 = 0;
                    if (selh > 1) {
                        p.fillRect(1, (row+1)*dheight+addrow2, 7*dwidth+colModulo, (selh-1)*dheight+addRow,selcol);
                    }
                    // draw last block from left to mSelEndT
                    selw = mSelEndT-7*(mSelEndT/7)+1;
                    //qDebug("esl %d ",selw );
                    int add = 0;
                    if ( colModulo ) {
                        add = 7 - colModulo;
                        if ( selw > add )
                            add = selw - add;
                        else
                            add = 0;
                    }
                    //qDebug("add %d ", add);
                    p.fillRect(isRTL ? (7-selw)*dwidth : 1, (row+selh)*dheight+addRow,
                               selw*dwidth+add, dheight+1, selcol);
                }
            }
        }

        // iterate over all days in the matrix and draw the day label in appropriate colors
        QColor actcol = mDefaultTextColorShaded;
        p.setPen(actcol);
        QPen tmppen;
        for(int i = 0; i < NUMDAYS; i++) {
            row = i/7;
            col = isRTL ? 6-(i-row*7) : i-row*7;

            // if it is the first day of a month switch color from normal to shaded and vice versa
            if ( KOGlobals::self()->calendarSystem()->day( days[i] ) == 1) {
                if (actcol == mDefaultTextColorShaded) {
                    actcol = mDefaultTextColor;
                } else {
                    actcol = mDefaultTextColorShaded;
                }
                p.setPen(actcol);
            }
            if (actcol == mDefaultTextColorShaded) { 
                if ( ! mouseDown ) {
                    continue;
                }
            }
            //Reset pen color after selected days block
            if (i == mSelEndT+1) {
                p.setPen(actcol);
            }

            // if today then draw rectangle around day
            if (today == i) {
                tmppen = p.pen();
                QPen mTodayPen(p.pen());
                if ( daysize.width() < 20 )
                    mTodayPen.setWidth(1);
                else
                    mTodayPen.setWidth(mTodayMarginWidth);
                //draw red rectangle for holidays
                if (pDays.testBit(i)) {
                    if (actcol == mDefaultTextColor) {
                        mTodayPen.setColor(KOPrefs::instance()->mHolidayColor);
                    } else {
                        mTodayPen.setColor(mHolidayColorShaded);
                    }
                }
                //draw gray rectangle for today if in selection
                if (i >= mSelStartT && i <= mSelEndT) {
                    QColor grey("grey");
                    mTodayPen.setColor(grey);
                }
                p.setPen(mTodayPen);


                int addCol = 0;
                int addRow = 0;
                if (rowModulo) { 
                    if ( row >= 6 - rowModulo )
                        addRow = row - 5 + rowModulo;
                }  
                if ( colModulo ) {
                    if ( col >= 7 - colModulo )
                        addCol = col - 6 + colModulo-1;
                }
         
                addCol += 1;
                if ( row == 0 )
                    addRow = 1;
                p.drawRect(col*dwidth+addCol, row*dheight+addRow, dwidth+1, dheight+1);
                p.setPen(tmppen);
            }

            // if any events are on that day then draw it using a bold font
            if ( eDays.testBit(i) ) {
                QFont myFont = font();
                myFont.setBold(true);
                p.setFont(myFont);
            }

            // if it is a holiday then use the default holiday color
            if ( pDays.testBit(i)) {
                if (  bDays.testBit(i) ) {
                    if ( hDays.testBit(i) )
                        p.setPen(QColor(0,200,0));
                    else
                        p.setPen(QColor(Qt::blue));
                } else {
                    if (actcol == mDefaultTextColor ) { 
                        p.setPen(KOPrefs::instance()->mHolidayColor);
                    } else {
                        p.setPen(mHolidayColorShaded);
                    }
                }
            }

            // draw selected days with special color
            // DO NOT specially highlight holidays in selection !
            if (i >= mSelStartT && i <= mSelEndT) {
                ;//p.setPen(mSelectedDaysColor);
            }

            int addCol = 0;
            int addRow = 0;
            if ( colModulo ) {
                if ( col >= 7 - colModulo )
                    addCol = col - 7 + colModulo;
            }
            if ( rowModulo ) {
                if ( row >= 6 - rowModulo )
                    addRow = row - 5 + rowModulo;
            }
            //qDebug("add %d %d -- %d %d ", col, addCol, row, addRow);
            ++addCol;//++addCol;
            if ( row == 0)
                addRow = 1;
            p.drawText(col*dwidth+addCol, row*dheight+addRow, dwidth, dheight,
                       Qt::AlignHCenter | Qt::AlignVCenter,  daylbls[i]);

            // reset color to actual color
            if (pDays.testBit(i)) {
                p.setPen(actcol);
            }
            // reset bold font to plain font
            if ( eDays.testBit(i)) {
                QFont myFont = font();
                myFont.setBold(false);
                p.setFont(myFont);
            }
        }
        p.setPen(mDefaultTextColor);
        p.drawRect(0, 0, width(), height());
    } else {
        //qDebug("NO  redraw ");
    }
    
    bitBlt (this, pevent->rect().topLeft(), &myPix , pevent->rect() ,CopyROP);
    mRedrawNeeded = false;
}

// ----------------------------------------------------------------------------
//  R E SI Z E   E V E N T   H A N D L I N G
// ----------------------------------------------------------------------------

void KODayMatrix::resizeEvent(QResizeEvent * e)
{
  QRect sz = frameRect();
  daysize.setHeight(sz.height()*7 / NUMDAYS);
  daysize.setWidth(sz.width() / 7);
  QFrame::resizeEvent( e );
}

QSize KODayMatrix::sizeHint() const
{

  QFontMetrics fm ( font() );
  int wid = fm.width( "30") *7+3;
  int hei = fm.height() * 6+3;
  //qDebug("KODayMatrix::sizeHint()********************* %d %d", wid , hei);
  return   QSize ( wid, hei );

}
