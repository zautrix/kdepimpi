/*
    This file is part of KOrganizer.
    Copyright (c) 2001,2002 Cornelius Schumacher <schumacher@kde.org>

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

#include <qstring.h>
#include <qkeycode.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qframe.h>
#include <qlabel.h>
#include <qapplication.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>

#include "koglobals.h"
#include "koprefs.h"
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif

#include <kcalendarsystem.h>

#include "navigatorbar.h"

#include "kdatenavigator.h"

KDateNavigator::KDateNavigator( QWidget *parent, const char *name )
  : QFrame(parent, name),
    updateTimer(0L)
{
    setFrameStyle(QFrame::NoFrame);
    QDate startDate = QDate::currentDate();
  QGridLayout *topLayout = new QGridLayout(this,8,8);

  if (! startDate.isValid()) {
      qDebug("KDateNavigator::invalid startdate ");
    startDate = QDate::currentDate();
  }
  mMonthSignalOffset = 0;
  mSelectedDates.append(startDate);
  m_MthYr = startDate;
  m_bShowWeekNums = true;
  setFont( KOPrefs::instance()->mDateNavigatorFont );
  mNavigatorBar = new NavigatorBar( startDate, this );
  topLayout->addMultiCellWidget( mNavigatorBar, 0, 0, 0, 7 );
  //mNavigatorBar->resize( 1,1);
  connect( mNavigatorBar, SIGNAL( goPrevYear() ), SIGNAL( goPrevYear() ) );
  connect( mNavigatorBar, SIGNAL( goPrevMonth() ), SIGNAL( goPrevMonth() ) );
  connect( mNavigatorBar, SIGNAL( goNextMonth() ), SIGNAL( goNextMonth() ) );
  connect( mNavigatorBar, SIGNAL( goNextYear() ), SIGNAL( goNextYear() ) );
  connect( mNavigatorBar, SIGNAL( monthSelected( int ) ), SLOT( slotMonthSelected( int ) ) );
  
  // get the day of the week on the first day
  QDate dayone(m_MthYr.year(), m_MthYr.month(), 1);
  m_fstDayOfWk = dayone.dayOfWeek();

  int i;

  // Set up the heading fields.
  for( i = 0; i < 7; i++ ) {
    headings[i] = new QLabel("",this);
    //headings[i]->setFont(QFont("Arial", 10, QFont::Bold));
    headings[i]->setAlignment(AlignCenter);
    headings[i]->installEventFilter(this);

    topLayout->addWidget(headings[i],1,i+1);
  }

  // Create the weeknumber labels
  for( i = 0; i < 6; i++ ) {
    weeknos[i] = new QLabel(this);
    weeknos[i]->setAlignment(AlignCenter );
    //weeknos[i]->setFont(QFont("Arial", 10));
    if(!m_bShowWeekNums) {
      weeknos[i]->hide();
    }
    weeknos[i]->installEventFilter(this);

    topLayout->addWidget(weeknos[i],i+2,0);
  }

  daymatrix = new KODayMatrix( this, "KDateNavigator::DayMatrix");
  daymatrix->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  daymatrix->setLineWidth(1);

  connect( daymatrix, SIGNAL( selected( const KCal::DateList & ) ),
           SIGNAL( datesSelected( const KCal::DateList & ) ) );

  connect( daymatrix, SIGNAL( eventDropped( Event * ) ),
           SIGNAL( eventDropped( Event * ) ) );

  topLayout->addMultiCellWidget(daymatrix,2,7,1,7);

  // read settings from configuration file.
  updateConfig();
  enableRollover(FollowMonth);
  mySizeHint = sizeHintTwoButtons();
  myFullSizeHint = sizeHintTwoButtons( 4 );
  mFontChanged = false;
  //resize ( 3,3 );
  
}
void KDateNavigator::changeFont ( QFont fo ) 
{
        setFont( fo );
        mNavigatorBar->resetFont( fo );
}
QFont KDateNavigator::yourFontHint( QSize si , bool *b)
{
    QFont fo =  KOPrefs::instance()->mDateNavigatorFont;
    *b = false;
    int fontPoint = fo.pointSize();
    while ( fontPoint > 5 ) {
        --fontPoint;
        fo.setPointSize( fontPoint );
        setFont( fo );
        mFontChanged = true;
        mNavigatorBar->resetFont( fo );
        QSize sh = sizeHintTwoButtons( 2 );
        //qDebug("fp %d %d %d %d %d", fontPoint,   si.width() , sh.width() ,   si.height() , sh.height()                 );
        if ( si.width()  > sh.width() &&   si.height()  > sh.height()) {
            if (  si.width()  / sh.width() == 1 ) {
                if (  si.width() < sizeHintTwoButtons( 4 ).width())
                    continue;
            }
            *b = true;
            break;
        }
    }
    return fo;
}
QSize KDateNavigator::sizeHint() const
{
    QFontMetrics fm ( font() );
    QSize day =  daymatrix->sizeHint();
    QSize nav =  mNavigatorBar->sizeHint();
    int wid = fm.width( "30") + day.width()+3;
    int hei = fm.height() +day.height()+nav.height()+2;
    if ( wid <  nav.width() )
        wid = nav.width() ;
    //qDebug("KDateNavigator+++++++++++++ %d %d", wid , hei);
  return   QSize ( wid, hei );
}
QSize KDateNavigator::sizeHintTwoButtons( int butnum ) const
{
    QFontMetrics fm ( font() );
    QSize day =  daymatrix->sizeHint();
    QSize nav =  mNavigatorBar->sizeHintTwoButtons( butnum );
    int wid = fm.width( "30") + day.width()+3;
    int hei = fm.height() +day.height()+nav.height()+2;
    if ( wid <  nav.width() )
        wid = nav.width() ;
    //qDebug("KDateNavigator+++++++++++++ %d %d", wid , hei);
  return   QSize ( wid, hei );
}
void KDateNavigator::slotMonthSelected( int m )
{
    if ( m_MthYr.month() <= mMonthSignalOffset)
      m += 12;
    //qDebug("%d mMonthSignalOffset %d emit %d", m, mMonthSignalOffset, m - mMonthSignalOffset);
    emit monthSelected( m - mMonthSignalOffset );

}
void KDateNavigator::setCalendar( Calendar *cal )
{
  daymatrix->setCalendar( cal );
}

void KDateNavigator::setBaseDate( const QDate &date , bool doRepaint ) // = true
{
  m_MthYr  = date;
  //qDebug("KDateNavigator::setBaseDate %s ", date.toString().latin1());
  
  updateDates();
  updateView();

  KCal::DateList dates;
  dates.append( date );
  mNavigatorBar->selectDates( dates );

  daymatrix->clearSelection();
  if ( doRepaint )
      daymatrix->repaint( false );
}

void KDateNavigator::enableRollover(RolloverType r)
{
  switch(r)
  {
  case None :
    if (updateTimer)
    {
      updateTimer->stop();
      delete updateTimer;
      updateTimer=0L;
    }
    break;
  case FollowDay :
  case FollowMonth :
    if (!updateTimer)
    {
      updateTimer = new QTimer(this);
      QObject::connect(updateTimer,SIGNAL(timeout()),
        this,SLOT(possiblyPastMidnight()));
    }
    updateTimer->start(0,true);
    lastDayChecked = QDate::currentDate();
  }
  updateRollover=r;
}


KDateNavigator::~KDateNavigator()
{
}


void KDateNavigator::passedMidnight()
{
    QDate today = QDate::currentDate();
    bool emitMonth = false;

    if (today.month() != lastDayChecked.month())
    {
       if (updateRollover==FollowMonth &&
           daymatrix->isEndOfMonth()) {
         goNextMonth();
	 emitMonth=true;
       }
    }
    daymatrix->recalculateToday();
    daymatrix->repaint( false );
    emit dayPassed(today);
    if (emitMonth) { emit monthPassed(today); }
}

/* slot */ void KDateNavigator::possiblyPastMidnight()
{
  if (lastDayChecked!=QDate::currentDate())
  {
    passedMidnight();
    lastDayChecked=QDate::currentDate();
  }
  // Set the timer to go off 1 second after midnight
  // or after 8 minutes, whichever comes first.
  if (updateTimer)
  {
    QTime now = QTime::currentTime();
    QTime midnight = QTime(23,59,59);
    int msecsWait = QMIN(480000,now.msecsTo(midnight)+2000);

    // qDebug(QString("Waiting %1 msec from %2 to %3.").arg(msecsWait))
    //	.arg(now.toString()).arg(midnight.toString()));

    updateTimer->stop();
    updateTimer->start(msecsWait,true);
  }
}

void KDateNavigator::updateDates()
{
  // Find the first day of the week of the current month.
  //int d1 = KOGlobals::self()->calendarSystem()->day( m_MthYr );
  QDate dayone( m_MthYr.year(), m_MthYr.month(), m_MthYr.day() );
  int d2 = KOGlobals::self()->calendarSystem()->day( dayone );
  //int di = d1 - d2 + 1;
  dayone = dayone.addDays( -d2 + 1 );

  int m_fstDayOfWkCalsys = KOGlobals::self()->calendarSystem()->dayOfWeek( dayone );

  // If month begins on Monday and Monday is first day of week,
  // month should begin on second line. Sunday doesn't have this problem.
  int nextLine = ( ( m_fstDayOfWkCalsys == 1) &&
                   ( KGlobal::locale()->weekStartsMonday() == 1 ) ) ? 7 : 0;

  // update the matrix dates
  int index = (KGlobal::locale()->weekStartsMonday() ? 1 : 0) - m_fstDayOfWkCalsys - nextLine;


  daymatrix->updateView(dayone.addDays(index));
//each updateDates is followed by an updateView -> repaint is issued there !
//  daymatrix->repaint();
}

void KDateNavigator::updateDayMatrix()
{
  daymatrix->updateView();
  //daymatrix->repaint();
}


void KDateNavigator::updateView()
{
   
  setUpdatesEnabled( false );

  int i;

//  kdDebug() << "updateView() -> daymatrix->updateView()" << endl;
  daymatrix->updateView();
  int sub = 4; 
  if ( !  KGlobal::locale()->weekStartsMonday() )
      --sub;
  // set the week numbers.
  for(i = 0; i < 6; i++) {
    // remember, according to ISO 8601, the first week of the year is the
    // first week that contains a thursday.  Thus we must subtract off 4,
    // not just 1.

    //ET int dayOfYear = buttons[(i + 1) * 7 - 4]->date().dayOfYear();
    int dayOfYear = KOGlobals::self()->calendarSystem()->dayOfYear((daymatrix->getDate((i+1)*7-sub)));
    int weekNo;
    if (dayOfYear % 7 != 0)
      weekNo = (dayOfYear / 7 + 1);
    else
      weekNo = (dayOfYear / 7);
    weeknos[i]->setText(QString::number( weekNo ));
  }

  setUpdatesEnabled( true );
//  kdDebug() << "updateView() -> repaint()" << endl;
  repaint();
  // daymatrix->repaint();
}

void KDateNavigator::updateConfig()
{
  int day;
  for(int i=0; i<7; i++) {
    // take the first letter of the day name to be the abbreviation
    if (KGlobal::locale()->weekStartsMonday()) {
      day = i+1;
    } else {
      if (i==0) day = 7;
      else day = i;
    }
    QString dayName = KOGlobals::self()->calendarSystem()->weekDayName( day,
                                                                        true );
    if ( KOPrefs::instance()->mCompactDialogs ) dayName = dayName.left( 1 );
    headings[i]->setText( dayName );
  }
  updateDates();
  updateView();
}

void KDateNavigator::setShowWeekNums(bool enabled)
{
   
  m_bShowWeekNums = enabled;
  for(int i=0; i<6; i++) {
    if(enabled)
      weeknos[i]->show();
    else
      weeknos[i]->hide();
  }
  resize(size());
}

void KDateNavigator::selectDates(const DateList& dateList)
{
    
  if (dateList.count() > 0) {
    mNavigatorBar->selectDates( dateList );
    mSelectedDates = dateList;

    // set our record of the month and year that this datetbl is
    // displaying.
    m_MthYr = mSelectedDates.first();


    // set our record of the first day of the week of the current
    // month. This needs to be done before calling dayToIndex, since it
    // relies on this information being up to date.
    QDate dayone(m_MthYr.year(), m_MthYr.month(), 1);
    m_fstDayOfWk = dayone.dayOfWeek();

    updateDates();

    daymatrix->setSelectedDaysFrom(*(dateList.begin()), *(--dateList.end()));

    updateView();
  }
}

int KDateNavigator::dayNum(int row, int col)
{
  return 7 * (row - 1) + (col + 1) - m_fstDayOfWk;
}

int KDateNavigator::dayToIndex(int dayNum)
{
  int row, col;

  row = (dayNum+m_fstDayOfWk-1-(KGlobal::locale()->weekStartsMonday() ? 1 : 0)) / 7;
  if (KGlobal::locale()->weekStartsMonday() && (m_fstDayOfWk == 1))
    row++;
  col = (dayNum+m_fstDayOfWk-1-(KGlobal::locale()->weekStartsMonday() ? 1 : 0)) % 7;
  return row * 7 + col;
}

void KDateNavigator::wheelEvent (QWheelEvent *e)
{
  if(e->delta()>0) emit goPrevious();
  else emit goNext();

  e->accept();
}

bool KDateNavigator::eventFilter (QObject *o,QEvent *e)
{
  if (e->type() == QEvent::MouseButtonPress) {
    int i;
    for(i=0;i<6;++i) {
      if (o == weeknos[i]) {
        QDate weekstart = daymatrix->getDate(i*7);
        emit weekClicked(weekstart);
        break;
      }
    }
    for(i=0;i<7;++i) {
      if (o == headings[i]) {
          KCal::DateList selDays;
          QDate date = daymatrix->getDate(14);
          emit showMonth(date );
#if 0
          int dio = date.daysInMonth();
          int j;
          int ye =  date.year();
          int mo = date.month();
          for ( j = 1; j <= dio; ++j ) {
              selDays.append( QDate( ye, mo, j ) );
          }
          emit datesSelected( selDays );
#endif
        break;
      }
    }
    return true;
  } else {
    return false;
  }
}

//#include "kdatenavigator.moc"
