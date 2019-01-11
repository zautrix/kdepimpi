/*
    This file is part of libkcal.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

#include "event.h"

using namespace KCal;

Event::Event() :
  mHasEndDate( false ), mTransparency( Opaque )
{
}

Event::Event(const Event &e) : Incidence(e)
{
  mDtEnd = e.mDtEnd;
  mHasEndDate = e.mHasEndDate;
  mTransparency = e.mTransparency;
}

Event::~Event()
{
}

Incidence *Event::clone()
{
  return new Event(*this);
}

bool KCal::operator==( const Event& e1, const Event& e2 )
{
    return operator==( (const Incidence&)e1, (const Incidence&)e2 ) &&
        e1.dtEnd() == e2.dtEnd() &&
        e1.hasEndDate() == e2.hasEndDate() &&
        e1.transparency() == e2.transparency();
}


bool Event::contains ( Event* from )
{

    if ( !from->summary().isEmpty() )
        if (  !summary().startsWith(  from->summary() ))
            return false; 
    if ( from->dtStart().isValid() )
        if (dtStart() != from->dtStart() )
            return false;
    if ( from->dtEnd().isValid() )
        if ( dtEnd() !=  from->dtEnd() )
            return false;
    if ( !from->location().isEmpty() )
        if (  !location().startsWith( from->location() ) )
            return false;
    if ( !from->description().isEmpty() )
        if (  !description().startsWith(  from->description() ))
            return false;
    if ( from->alarms().count() ) {
        Alarm *a = from->alarms().first();
        if (  a->enabled()  ){
            if ( !alarms().count() )
                return false;
            Alarm *b = alarms().first();
            if( ! b->enabled() )
                return false;
            if ( ! (a->offset() == b->offset() ))
                return false;
        }
    }
    QStringList cat = categories();
    QStringList catFrom = from->categories();
    QString nCat;
    unsigned int iii;
    for ( iii = 0; iii < catFrom.count();++iii ) {
        nCat = catFrom[iii];
        if ( !nCat.isEmpty() )
            if ( !cat.contains( nCat )) {
                return false;
            }
    }
    if ( from->doesRecur() ) 
        if ( from->doesRecur() != doesRecur() && ! (from->doesRecur()== Recurrence::rYearlyMonth && doesRecur()== Recurrence::rYearlyDay) )
            return false;
    return true;
}

void Event::setDtEnd(const QDateTime &dtEnd)
{
  if (mReadOnly) return;

  mDtEnd = getEvenTime( dtEnd );

  setHasEndDate(true);
  setHasDuration(false);

  updated();
}

QDateTime Event::dtEnd() const
{
  if (hasEndDate()) return mDtEnd;
  if (hasDuration()) return dtStart().addSecs(duration());

  return dtStart();
}

QString Event::dtEndTimeStr() const
{
  return KGlobal::locale()->formatTime(mDtEnd.time());
}

QString Event::dtEndDateStr(bool shortfmt) const
{
  return KGlobal::locale()->formatDate(mDtEnd.date(),shortfmt);
}

QString Event::dtEndStr(bool shortfmt) const
{
  return KGlobal::locale()->formatDateTime(mDtEnd, shortfmt);
}

void Event::setHasEndDate(bool b)
{
  mHasEndDate = b;
}

bool Event::hasEndDate() const
{
  return mHasEndDate;
}

bool Event::isMultiDay() const
{
  bool multi = !(dtStart().date() == dtEnd().date());
  return multi;
}

void Event::setTransparency(Event::Transparency transparency)
{
  if (mReadOnly) return;
  mTransparency = transparency;
  updated();
}

Event::Transparency Event::transparency() const
{
  return mTransparency;
}

void Event::setDuration(int seconds)
{
  setHasEndDate(false);
  Incidence::setDuration(seconds);
}
bool Event::matchTime(QDateTime*startDT, QDateTime* endDT)
{
    if ( cancelled() ) return false;
    if ( ! doesRecur() ) {
        if ( doesFloat() ) {
            if ( mDtEnd.addDays( 1 ) < *startDT)
                return false;
            if ( endDT && mDtStart > * endDT)
                return false;
        } else {
            if ( mDtEnd < *startDT )
                return false;
            if ( endDT && mDtStart > * endDT)
                return false;
        }
    } else {
        if ( endDT && mDtStart > * endDT)
            return false;
    }
    return true;
}
bool Event::isOverlapping (  Event* testEvent,  QDateTime* overlapDT, QDateTime* startDT )
{
    if ( testEvent == this )
        return false;
    if ( ! doesRecur() && !testEvent->doesRecur() ) {
        QDateTime te;
        if ( testEvent->doesFloat() )
            te = testEvent->mDtEnd.addDays( 1 );
        else
            te = testEvent->mDtEnd;
        QDateTime e; 
        if ( doesFloat() )
            e = mDtEnd.addDays( 1 );
        else
            e = mDtEnd;
        if ( mDtStart < te && testEvent->mDtStart < e ) {
            if ( mDtStart < testEvent->mDtStart )
                *overlapDT =  testEvent->mDtStart;
            else
                *overlapDT =  mDtStart;
            if ( startDT )
                return (*overlapDT >= *startDT );
            return true;
        }
        return false;
    }
    if ( !doesFloat() && !testEvent->doesFloat() && !isMultiDay() && !testEvent->isMultiDay() ) {
        if ( mDtStart.time() >=  testEvent->mDtEnd.time() || testEvent->mDtStart.time() >=  mDtEnd.time() ) {
            // no need to test. times mismatch
            //fprintf(stderr,"timi ");
            return false;
        }
    }
    Event *nonRecur = 0;
    Event *recurEvent = 0;
    if ( ! doesRecur() ) {
        nonRecur = this;
        recurEvent = testEvent;
    }
    else if ( !testEvent->doesRecur() ) {
        nonRecur = testEvent;
        recurEvent = this;
    }
    if ( nonRecur ) {
        QDateTime enr; 
        if ( nonRecur->doesFloat() )
            enr = nonRecur->mDtEnd.addDays( 1 );
        else
            enr = nonRecur->mDtEnd;
        if ( enr < recurEvent->mDtStart )
            return false;
        if ( startDT && enr < *startDT )
            return false;
        
        int recDuration = recurEvent->mDtStart.secsTo( recurEvent->mDtEnd );
        if ( recurEvent->doesFloat() )
            recDuration += 86400;
        bool ok = false;
        QDateTime recStart;
        if ( startDT ) {
            recStart = recurEvent->recurrence()->getPreviousDateTime( startDT->addSecs( 61 ), &ok );
        }
        if ( recStart.isValid() ) {
            //qDebug("%s start at %s ", startDT->  toString().latin1(),       recStart.toString().latin1());
            recStart =  recStart.addSecs( -300);
        }
        else
            recStart = recurEvent->mDtStart.addSecs( -300);
        ok = true;

        while ( ok ) {
            recStart = recurEvent->getNextOccurence( recStart.addSecs( 60 ), &ok );
            if ( ok ) {
                if ( recStart > enr )
                    return false;
                QDateTime recEnd = recStart.addSecs( recDuration  );
                if ( nonRecur->mDtStart < recEnd && recStart < nonRecur->mDtEnd ) {
                    if ( nonRecur->mDtStart < recStart )
                        *overlapDT =  recStart;
                    else
                        *overlapDT =  nonRecur->mDtStart;
                    if ( startDT  ) {
                        if ( *overlapDT >= *startDT )
                            return true;
                    } else
                        return true;
                }
            }
        }
        return false;
    }
     if ( !doesFloat() && !testEvent->doesFloat() && !isMultiDay() && !testEvent->isMultiDay() ) {
         if ( recurrence()->doesRecur() == Recurrence::rWeekly &&  testEvent->recurrence()->doesRecur() == Recurrence::rWeekly ) {
             bool found = false;
             uint i;
             for ( i=0; i< recurrence()->days().size();++i ) {
                 found =  found || (recurrence()->days().testBit( i ) && testEvent->recurrence()->days().testBit( i ) );
             }
             if ( ! found ) {
                 //qDebug("recurring days mismatch %s -- %s", summary().latin1(),testEvent->summary().latin1());
                 return false;
             }

         }
    }
    bool ok = true;
    QDateTime incidenceStart;// = mDtStart;
    QDateTime testincidenceStart;// = testEvent->mDtStart;
    if ( startDT ) {
        incidenceStart = recurrence()->getPreviousDateTime( startDT->addSecs( 61 ), &ok );
        testincidenceStart = testEvent->recurrence()->getPreviousDateTime( startDT->addSecs( 61 ), &ok );
    }
    if ( !testincidenceStart.isValid() )
        testincidenceStart = testEvent->mDtStart;
    if ( !incidenceStart.isValid() )
        incidenceStart = mDtStart;
    int duration = mDtStart.secsTo( mDtEnd );
    if ( doesFloat() )
            duration += 86400;
    int testduration = testEvent->mDtStart.secsTo( testEvent->mDtEnd );
    if ( testEvent->doesFloat() )
            testduration += 86400;
    bool computeThis = false;
    if ( incidenceStart < testincidenceStart )
        computeThis = true;
    if ( computeThis )
        incidenceStart = incidenceStart.addSecs( -300 );
    else
        testincidenceStart = testincidenceStart.addSecs( -300 );
    int count = 0;
    ok = true;
    int countbreak = 2000;
    QDateTime stopSearch;
    bool testStop = false;
    if ( startDT ) {
        stopSearch = startDT->addDays( 365*3 );
        testStop = true;
    }
    while ( ok ) {
        ++count;
        if ( count > countbreak ) break;
        if ( computeThis ) {
            if ( testStop ) 
                if ( testincidenceStart > stopSearch )
                    break;
            incidenceStart = getNextOccurence( incidenceStart.addSecs( 60 ), &ok );
        }
        else {
            if ( testStop ) 
                if ( incidenceStart > stopSearch )
                    break;
            testincidenceStart = testEvent->getNextOccurence( testincidenceStart.addSecs( 60 ), &ok );
        }
        if ( ok ) {
            if ( incidenceStart < testincidenceStart.addSecs( testduration ) && testincidenceStart < incidenceStart.addSecs( duration ) ) {
                if ( incidenceStart < testincidenceStart )
                    *overlapDT =  testincidenceStart;
                else
                    *overlapDT =  incidenceStart; 
                if (  startDT ) {
                    if ( *overlapDT >= *startDT )
                        return true;
                } else
                    return true;
            }
            computeThis =  ( incidenceStart < testincidenceStart );
        }
        
    }
    //qDebug("%d rec counter stopped at %d - %s %s", ok ,count, summary().latin1(),testEvent->summary().latin1() );
    return false;
}
QDateTime Event::getNextAlarmDateTime( bool * ok, int * offset, QDateTime start_dt ) const
{
    *ok = false;
    if ( !alarmEnabled() )
        return QDateTime ();
    bool yes;
     QDateTime incidenceStart = getNextOccurence( start_dt, &yes );
     if ( ! yes || cancelled() ) {
         *ok = false;
         return QDateTime ();
     }
  
     bool enabled = false;
     Alarm* alarm;
     int off = 0;
     QDateTime alarmStart = QDateTime::currentDateTime().addDays( 3650 );;
    //  if ( QDateTime::currentDateTime() > incidenceStart ){
//          *ok = false;
//          return incidenceStart;
//      }
     for (QPtrListIterator<Alarm> it(mAlarms); (alarm = it.current()) != 0; ++it) {
         if (alarm->enabled()) {
             if ( alarm->hasTime () ) {
                 if ( alarm->time() < alarmStart ) {
                     alarmStart = alarm->time();
                     enabled = true;
                     off = alarmStart.secsTo( incidenceStart );
                 }

             } else {
                 int secs = alarm->startOffset().asSeconds();
                 if ( incidenceStart.addSecs( secs ) < alarmStart ) {
                     alarmStart = incidenceStart.addSecs( secs );
                     enabled = true;
                     off = -secs;
                 }
             }
         }
     }
     if ( enabled ) {
         if ( alarmStart >  start_dt  ) {
             *ok = true;
             * offset = off;
             return alarmStart;
         }
     }
     *ok = false;
     return QDateTime ();

}

QString Event::durationText()
{
    int sec = mDtStart.secsTo( mDtEnd );
    if ( doesFloat() )
        sec += 86400;
    return durationText4Time( sec );
}
