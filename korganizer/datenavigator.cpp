/*
    This file is part of KOrganizer.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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

#include "datenavigator.h"

#include "koglobals.h"

#include <kcalendarsystem.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

using namespace KCal;

DateNavigator::DateNavigator( QObject *parent, const char *name , KOViewManager * v)
  : QObject( parent, name )
{
    mViewManager = v;
    mSelectedDates.append( QDate::currentDate() );
}

DateNavigator::~DateNavigator()
{
}

void DateNavigator::slotMonthSelect( int m )
{
    QDate firstSelected = mSelectedDates.first();
    int weekDay = firstSelected.dayOfWeek();
    int diff  = m - firstSelected.month() ;
    firstSelected = KOGlobals::self()->calendarSystem()->addMonths( firstSelected, diff );
    
    if ( mSelectedDates.first().day() == 1 && mSelectedDates.count() > 27 )
      selectMonthByDate( firstSelected );
  else
      selectWeekByDay( weekDay, firstSelected );
    
}

void DateNavigator::slotDaySelect( QDate d )
{
    QDate firstSelected = mSelectedDates.first();
    int weekDay = firstSelected.dayOfWeek();
    //int diff  = firstSelected.daysTo(  d  ) ;
    firstSelected = firstSelected.addDays( firstSelected .daysTo(  d  ) );
    
    selectWeekByDay( weekDay, firstSelected );
    
}

void DateNavigator::selectMonthByDate(  const QDate & firstSelected )
{
 
    int monthDay = firstSelected.day(); 
    QDate date = firstSelected.addDays( 1-monthDay );
    selectDates( date , date.daysInMonth ());
    
}
void DateNavigator::selectMonth()
{
 
    QDate date =mSelectedDates.first();
    selectMonthByDate( date );
}
void DateNavigator::selectMonthFromMonthview()
{
 
    QDate date =mSelectedDates.first().addDays( 7 );
    selectMonthByDate( date );
}

DateList DateNavigator::selectedDates()
{
  return mSelectedDates;
}

int DateNavigator::datesCount() const
{
  return mSelectedDates.count();
}

void DateNavigator::selectDates( const DateList& dateList )
{
  if (dateList.count() > 0) {
    mSelectedDates = dateList;
    emitSelected();
  }
}

void DateNavigator::selectDate( const QDate &date )
{
  QDate d = date;

  if ( !d.isValid() ) {
    d = QDate::currentDate();
  }

  mSelectedDates.clear();
  mSelectedDates.append( d );

  emitSelected();
}

void DateNavigator::selectDates( int count )
{

    QDate d = mSelectedDates.first();
    selectDates( d, count );
}

void DateNavigator::selectDates( const QDate &d, int count )
{
  DateList dates;

  int i;
  for( i = 0; i < count; ++i ) {
    dates.append( d.addDays( i ) );
  }
  
  mSelectedDates = dates;
  
  emitSelected();
}

void DateNavigator::selectWeekByDay( int weekDay, const QDate &d )
{
    //  qDebug("selectWeekByDay(  %d %s ", weekDay, d.toString().latin1());
  int dateCount = mSelectedDates.count();
  bool weekStart = ( weekDay == KGlobal::locale()->weekStartDay() );
  if ( weekDay == 1 && dateCount == 5 ) selectWorkWeek( d );
  else if ( weekStart && dateCount == 7 ) selectWeek( d );
  else selectDates( d, dateCount );
}

void DateNavigator::selectWeek()
{
    QDate d = mSelectedDates.first();
    selectWeek( d );
}
void DateNavigator::selectWeek( int num )
{
    int year = mSelectedDates.first().year();
    QDate d =  QDate ( year, 1,1);
    while ( d.dayOfWeek() != 4 )
        d = d.addDays( 1 );
    selectWeek( d.addDays ( (num-1) *7 ) );
}
void DateNavigator::selectWeekFromMonthView( int num )
{
    int year = mSelectedDates.first().year();
    if ( mSelectedDates.first().dayOfYear() > 300 && num < 10 )
        ++year;
    if ( mSelectedDates.first().dayOfYear() < 70 && num > 40 )
        --year;
    QDate d =  QDate ( year, 1,1);
    while ( d.dayOfWeek() != 4 )
        d = d.addDays( 1 );
    selectWeek( d.addDays ( (num-1) *7 ) );
}
void DateNavigator::selectWeek( const QDate &d )
{
  int dayOfWeek = KOGlobals::self()->calendarSystem()->dayOfWeek( d );

  int weekStart = KGlobal::locale()->weekStartDay();
  QDate firstDate = d.addDays( weekStart - dayOfWeek );

  if ( weekStart != 1 && dayOfWeek < weekStart ) {
      firstDate = firstDate.addDays(-7 );
  }


  selectDates( firstDate, 7 );
}

void DateNavigator::selectWorkWeek()
{ 
    QDate d = mSelectedDates.first();
  selectWorkWeek( d );
}

void DateNavigator::selectWorkWeek( const QDate &d )
{
  int dayOfWeek = KOGlobals::self()->calendarSystem()->dayOfWeek( d );

  QDate firstDate = d.addDays( 1 - dayOfWeek );

  int weekStart = KGlobal::locale()->weekStartDay();
  if ( weekStart != 1 && dayOfWeek >= weekStart ) {
    firstDate = firstDate.addDays( 7 );
  }

  selectDates( firstDate, 5 );
}

void DateNavigator::selectTodayMonth()
{
    QDate date = QDate::currentDate().addDays( 1-QDate::currentDate().day() );
    selectDates( date , date.daysInMonth ());

}
void DateNavigator::selectToday()
{
  QDate d = QDate::currentDate();

  int dateCount = mSelectedDates.count();
  if ( dateCount == 5 && d.dayOfWeek() < 6 ) selectWorkWeek( d );
  else if ( dateCount == 7 ) selectWeek( d );
  else selectDates( d, dateCount );
}

void DateNavigator::selectPreviousYear()
{
  QDate firstSelected = mSelectedDates.first();
  int weekDay = firstSelected.dayOfWeek();
  firstSelected = KOGlobals::self()->calendarSystem()->addYears( firstSelected, -1 );

  selectWeekByDay( weekDay, firstSelected );
}

void DateNavigator::selectPreviousMonth()
{
  QDate firstSelected = mSelectedDates.first();
  int weekDay = firstSelected.dayOfWeek();
  firstSelected = KOGlobals::self()->calendarSystem()->addMonths( firstSelected, -1 );

  
  if ( mSelectedDates.first().day() == 1 && mSelectedDates.count() > 27 )
      selectMonthByDate( firstSelected );
  else
      selectWeekByDay( weekDay, firstSelected ); 
}

void DateNavigator::selectNextMonth()
{
  QDate firstSelected = mSelectedDates.first();
  int weekDay = firstSelected.dayOfWeek();

  firstSelected = KOGlobals::self()->calendarSystem()->addMonths( firstSelected, 1 );
 
  if ( mSelectedDates.first().day() == 1 && mSelectedDates.count() > 27 )
      selectMonthByDate( firstSelected );
  else
      selectWeekByDay( weekDay, firstSelected ); 
 
}
void DateNavigator::selectPreviousWeek()
{
  selectDates( mSelectedDates.first().addDays( -7 ), datesCount() );
}

void DateNavigator::selectNextWeek()
{
  selectDates( mSelectedDates.first().addDays( 7 ), datesCount() );
}

void DateNavigator::selectNextYear()
{
  QDate firstSelected = mSelectedDates.first();
  int weekDay = firstSelected.dayOfWeek();
  firstSelected = KOGlobals::self()->calendarSystem()->addYears( firstSelected, 1 );

  selectWeekByDay( weekDay, firstSelected );
}

void DateNavigator::selectPrevious()
{
  int offset = -7;
  if ( datesCount() == 1 ) {
    offset = -1;
  }
 if ( mViewManager )
      if ( mViewManager->showsNextDays() )
          offset = -datesCount();
  selectDates( mSelectedDates.first().addDays( offset ), datesCount() );
}

void DateNavigator::selectNext()
{
  int offset = 7;
  if ( datesCount() == 1 ) {
    offset = 1;
  }
  if ( mViewManager )
      if ( mViewManager->showsNextDays() )
          offset = datesCount();
  selectDates( mSelectedDates.first().addDays( offset ), datesCount() );
}

void DateNavigator::emitSelected()
{
  emit datesSelected( mSelectedDates );
}

//#include "datenavigator.moc"
