/*
    This file is part of libkcal.

    Copyright (c) 1998 Preston Brown
    Copyright (c) 2001,2003 Cornelius Schumacher <schumacher@kde.org>

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

#include <qdatetime.h>
#include <qstring.h>
#include <qptrlist.h>

#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

#include "vcaldrag.h"
#include "vcalformat.h"
#include "icalformat.h"
#include "exceptions.h"
#include "incidence.h"
#include "journal.h"
#include "filestorage.h"
#include "calfilter.h"

#include "calendarlocal.h"

// #ifndef DESKTOP_VERSION
// #include <qtopia/alarmserver.h> 
// #endif
using namespace KCal;

CalendarLocal::CalendarLocal()
  : Calendar()
{
  init();
}

CalendarLocal::CalendarLocal(const QString &timeZoneId)
  : Calendar(timeZoneId)
{
  init();
}

void CalendarLocal::init()
{
    mNextAlarmIncidence = 0;
}


CalendarLocal::~CalendarLocal()
{
    registerObserver( 0 );
    if ( mDeleteIncidencesOnClose )
        close();
}
bool CalendarLocal::mergeCalendarFile( QString name )
{
    CalendarLocal calendar( timeZoneId() );
    calendar.setDefaultCalendar( 1 );
    if ( calendar.load( name ) ) {
        mergeCalendar( &calendar  );
        return true;
    }
    return false;
}

Todo* CalendarLocal::todoForUid( const QString& uid, bool doNotCheckDuplicates, bool enabledOnly,int * isDup )
{

    int calID = 0;
    if ( isDup  && *isDup > 0 )
        calID = *isDup;
    Todo *todo;;
    Todo *retVal = 0;
    for ( todo = mTodoList.first(); todo; todo = mTodoList.next() ) {
        if ( todo->uid() == uid ) {
            if( calID ) {
                if ( todo->calID() != calID )
                    continue;
            }
            else {
                if ( enabledOnly ) {
                    if ( !todo->calEnabled() ) {
                        if ( isDup )
                            *isDup = todo->calID();
                        continue;
                    }
                }
            }
            if ( doNotCheckDuplicates ) return todo;
            if ( retVal ) {
                if ( retVal->calID() > todo->calID() ) {
                    if ( isDup )
                        *isDup = retVal->calID();
                    retVal = todo;
                }
            } else {
                retVal = todo;
            }
        } 
    }
    return retVal;
} 
//if ( isDup) and * isDup == 0: store duplicate found cal id in  isDup
//if ( isDup) and * isDup > 0: search only in calendar with ID *isDup, ignore enabledOnly

Incidence* CalendarLocal::incidenceForUid( const QString& uid , bool doNotCheckDuplicates, bool enabledOnly, int * isDup ) 
{
    int calID = 0;
    if ( isDup  && *isDup > 0 )
        calID = *isDup;
    Incidence *retVal = todoForUid( uid , doNotCheckDuplicates,enabledOnly, isDup );
    if ( retVal ) return retVal;
    Event *event;
    for ( event = mEventList.first(); event; event = mEventList.next() ) {
        if (  event->uid() == uid ) {
            if( calID ) {
                if ( event->calID() != calID )
                    continue;
            }
            else {
                if ( enabledOnly ) {
                    if ( !event->calEnabled() )  {
                        if ( isDup )
                            *isDup =event->calID() ;
                        continue;
                    }
                }
            }
            if ( doNotCheckDuplicates ) return event;
            if ( retVal ) {
                if ( retVal->calID() > event->calID() ) {
                    if ( isDup )
                        *isDup = retVal->calID();
                    retVal = event;
                }
            } else {
                retVal = event;
            }
        }
    }
    if ( retVal ) return retVal;
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->uid() == uid ) {
            if( calID ) {
                if ( event->calID() != calID )
                    continue;
            }
            else {
                if ( enabledOnly ) {
                    if ( !it->calEnabled() ) {
                        if ( isDup )
                            *isDup = it->calID();
                        continue;
                    }
                }
            }
            if ( doNotCheckDuplicates ) return it;
            if ( retVal ) {
                if ( retVal->calID() > it->calID() ) { 
                    if ( isDup )
                        *isDup = retVal->calID();
                    retVal = it;
                }
            } else {
                retVal = it;
            }
        }
    return retVal;
}

bool CalendarLocal::mergeCalendar( Calendar* remote  )
{
    // 1 look for raw inc in local
    //   if inc not in remote, delete in local
    // 2 look for raw inc in remote
    //   if inc in local, replace it
    //   if not in local, add it to default calendar
    QPtrList<Incidence> localInc = rawIncidences(); 
    Incidence* inL = localInc.first(); 
    while ( inL ) {
        if ( ! inL->isReadOnly () )
            if ( !remote->incidenceForUid( inL->uid(), true ))
                deleteIncidence( inL );
        inL = localInc.next();
    }
    QPtrList<Incidence> er = remote->rawIncidences(); 
    Incidence* inR = er.first();
    while ( inR ) {
        inL = incidenceForUid( inR->uid(),false );
        if ( inL ) {
            if ( ! inL->isReadOnly () || inL->uid().left(15) == QString("last-syncEvent-") ) {
                int calID = inL->calID();
                deleteIncidence( inL );
                inL = inR->clone();
                inL->setCalID_block( calID );
                addIncidence( inL );
            }
        } else {
            inL = inR->clone();
            inL->setCalID_block( 0 );// add to default cal
            addIncidence( inL );
        }
        inR = er.next();
    }
    return true;
}


bool CalendarLocal::addCalendarFile( QString name, int id )
{
    CalendarLocal calendar( timeZoneId() );
    calendar.setDefaultCalendar( id );
    if ( calendar.load( name ) ) {
        addCalendar( &calendar  );
        return true;
    }
    return false;
}
void CalendarLocal::setSyncEventsEnabled()
{
    Event * ev;
    ev = mEventList.first();
    while ( ev ) {
        if ( ev->uid().left(15) == QString("last-syncEvent-") )
            ev->setCalEnabled( true );
        ev =  mEventList.next();
    } 
}
void CalendarLocal::setSyncEventsReadOnly()
{
    Event * ev;
    ev = mEventList.first();
    while ( ev ) {
        if ( ev->uid().left(15) == QString("last-syncEvent-") ) {
            ev->setReadOnly( true ); 
        }
        ev =  mEventList.next();
    } 
}

void CalendarLocal::addCalendar( Calendar* cal )
{
    cal->setDontDeleteIncidencesOnClose();
    setSyncEventsEnabled();
    QPtrList<Incidence> incList;
    {
        QPtrList<Event> EventList = cal->rawEvents();
        Event * ev = EventList.first();
        while ( ev ) {
            if ( ev->uid().left( 15 ) == QString("last-syncEvent-") ) {   
                ev->setCalID_block( 1 );
            }
            Event * se = event( ev->uid() );
            if ( se )
                incList.append( se );    
            ev->unRegisterObserver( cal );
            ev->registerObserver( this );
            mEventList.append( ev );
            ev = EventList.next();
        }
         
    }
    {

        QPtrList<Todo> TodoList = cal->rawTodos();
        Todo * ev = TodoList.first();
        while ( ev ) {
            ev->resetRelatedTo();
            Todo * se = todo( ev->uid() );
            if ( se )
                deleteTodo( se ); 
            ev = TodoList.next();   
        }
        //TodoList = cal->rawTodos(); 
        ev = TodoList.first();
        while ( ev ) { 
            ev->unRegisterObserver( cal );
            ev->registerObserver( this );
            mTodoList.append( ev );
            setupRelations( ev );
            ev = TodoList.next();
        }
    }
    {
        QPtrList<Journal> JournalList = cal->journals();
        Journal * ev = JournalList.first();
        while ( ev ) {
            Journal * se = journal( ev->uid() );
            if ( se )
                incList.append( se );    
            ev->unRegisterObserver( cal );
            ev->registerObserver( this );
            mJournalList.append( ev );
            ev = JournalList.next();
        }
    }
    {
        for (Incidence * inc = incList.first(); inc; inc = incList.next() ) {
            deleteIncidence ( inc );
        }
    }
    setModified( true );
}
bool CalendarLocal::load( const QString &fileName )
{
  FileStorage storage( this, fileName );
  return storage.load();
}

bool CalendarLocal::save( const QString &fileName, CalFormat *format )
{
  FileStorage storage( this, fileName, format );
  return storage.save();
}

void CalendarLocal::stopAllTodos()
{
    for ( Todo *it = mTodoList.first(); it; it = mTodoList.next() )
        it->setRunning( false );

}
void CalendarLocal::close()
{

  Todo * i;
  for( i = mTodoList.first(); i; i = mTodoList.next() ) i->setRunning(false);

  mEventList.setAutoDelete( true );
  mTodoList.setAutoDelete( true );
  mJournalList.setAutoDelete( true );

  mEventList.clear();
  mTodoList.clear();
  mJournalList.clear();

  mEventList.setAutoDelete( false );
  mTodoList.setAutoDelete( false );
  mJournalList.setAutoDelete( false );

  setModified( false );
}

bool CalendarLocal::addAnniversaryNoDup( Event *event )
{
    QString cat;
    bool isBirthday = true;
    if(  event->categoriesStr() == i18n( "Anniversary" ) ) {
        isBirthday = false;
        cat = i18n( "Anniversary" );
    } else if(  event->categoriesStr() == i18n( "Birthday" ) ) {
        isBirthday = true;
        cat = i18n( "Birthday" );
    } else {
        qDebug("addAnniversaryNoDup called without fitting category! ");
        return false;
    }
    Event * eve;
    for ( eve = mEventList.first(); eve ; eve = mEventList.next() ) {
        if ( !(eve->categories().contains( cat ) ))
            continue;
        // now we have an event with fitting category
        if ( eve->dtStart().date() != event->dtStart().date() )
            continue;
        // now we have an event with fitting category+date
        if ( eve->summary() != event->summary() )
            continue;
        // now we have an event with fitting category+date+summary
        return false;
    }
    return addEvent( event );

}
bool CalendarLocal::addEventNoDup( Event *event )
{
    Event * eve;
    for ( eve = mEventList.first(); eve ; eve = mEventList.next() ) {
        if ( *eve == *event ) {
            //qDebug("CalendarLocal::Duplicate event found! Not inserted! ");
            return false;
        }
    }
    return addEvent( event );
}

bool CalendarLocal::addEvent( Event *event )
{
  insertEvent( event );

  event->registerObserver( this );

  setModified( true );
  if ( event->calID() == 0 )
      event->setCalID_block( mDefaultCalendar );
  event->setCalEnabled( true );

  return true;
}

void CalendarLocal::deleteEvent( Event *event )
{
    clearUndo(event);
    if ( mEventList.removeRef( event ) ) {
        setModified( true );
    }
}


Event *CalendarLocal::event( const QString &uid )
{
    Event *event;
    Event *retVal = 0;
    for ( event = mEventList.first(); event; event = mEventList.next() ) {
        if ( event->calEnabled() && event->uid() == uid ) {
            if ( retVal ) {
                if ( retVal->calID() > event->calID() ) {
                    retVal = event;
                }
            } else {
                retVal = event;
            }
        }
    }
  return retVal;
}
bool CalendarLocal::addTodoNoDup( Todo *todo )
{
  Todo * eve;
    for ( eve = mTodoList.first(); eve ; eve = mTodoList.next() ) {
        if ( *eve == *todo ) {
            //qDebug("duplicate todo found! not inserted! ");
            return false;
        }
    }
    return addTodo( todo );
}
bool CalendarLocal::addTodo( Todo *todo )
{
  mTodoList.append( todo );

  todo->registerObserver( this );

  // Set up subtask relations
  setupRelations( todo );

  setModified( true );
  if ( todo->calID() == 0 )
  todo->setCalID_block( mDefaultCalendar );
  todo->setCalEnabled( true );
  return true;
}

void CalendarLocal::deleteTodo( Todo *todo )
{
    QString uid =  todo->uid();
  // Handle orphaned children
    removeRelations( todo );
    clearUndo(todo);

  if ( mTodoList.removeRef( todo ) ) {
    setModified( true );
  }
   Todo* dup = todoForUid(  uid );
   if ( dup )
       setupRelations( dup );
}

QPtrList<Todo> CalendarLocal::rawTodos()
{
    QPtrList<Todo> el;
    for ( Todo *it = mTodoList.first(); it; it = mTodoList.next() )
        if ( it->calEnabled() ) el.append( it );
    return el;
}
Todo *CalendarLocal::todo( QString syncProf, QString id )
{
  Todo *todo;
  for ( todo = mTodoList.first(); todo; todo = mTodoList.next() ) {
      if ( todo->calEnabled() && todo->getID( syncProf ) == id ) return todo;
  }

  return 0;
}
void CalendarLocal::removeSyncInfo( QString syncProfile)
{
    QPtrList<Incidence> all = rawIncidences() ;
    Incidence *inc;
    for ( inc = all.first(); inc; inc = all.next() ) {
        inc->removeID( syncProfile );
    }
    if ( syncProfile.isEmpty() ) {
        QPtrList<Event> el;
        Event *todo;
        for ( todo = mEventList.first(); todo; todo = mEventList.next() ) {
            if ( todo->uid().left( 15 ) == QString("last-syncEvent-") )
                    el.append( todo );
        }
        for ( todo = el.first(); todo; todo = el.next() ) {
           deleteIncidence ( todo );
        }
    } else {
        Event *lse = event( "last-syncEvent-"+ syncProfile);
        if ( lse )
            deleteIncidence ( lse );
    }
}
QPtrList<Event> CalendarLocal::getExternLastSyncEvents()
{
    QPtrList<Event> el;
    Event *todo;
    for ( todo = mEventList.first(); todo; todo = mEventList.next() ) {
        if ( todo->uid().left( 15 ) == QString("last-syncEvent-") )
            if ( todo->summary().left(3) == "E: " )
                el.append( todo );
    }

    return el;

}
Event *CalendarLocal::event( QString syncProf,  QString id )
{
  Event *todo;
  for ( todo = mEventList.first(); todo; todo = mEventList.next() ) {
      if ( todo->calEnabled() && todo->getID( syncProf ) == id ) return todo;
  }

  return 0;
}
Todo *CalendarLocal::todo( const QString &uid )
{
  Todo *todo;
  Todo *retVal = 0;
  for ( todo = mTodoList.first(); todo; todo = mTodoList.next() ) {
      if ( todo->calEnabled() && todo->uid() == uid ) {
          if ( retVal ) {
                if ( retVal->calID() > todo->calID() ) {
                    retVal = todo;
                }
            } else {
                retVal = todo;
            }
      } 
  }
  return retVal;
}
void CalendarLocal::getIncidenceCount( int calId, int& events, int & todos, int & journals)
{
    events = 0;
    todos = 0;
    journals = 0;
    {
        Todo *todo;
        for ( todo = mTodoList.first(); todo; todo = mTodoList.next() ) {
            if ( todo->calID() == calId )
                ++todos;
        } 
    }
    {
        Event *todo;
        for ( todo = mEventList.first(); todo; todo = mEventList.next() ) {
            if ( todo->calID() == calId )
                ++events;

        }
    }
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->calID() == calId ) ++journals;

}
QString CalendarLocal::nextSummary() const
{
    return mNextSummary;
}
QDateTime CalendarLocal::nextAlarmEventDateTime()  const
{
    return mNextAlarmEventDateTime;
}
void  CalendarLocal::checkAlarmForIncidence( Incidence * incidence, bool deleted)
{
    //mNextAlarmIncidence
    //mNextAlarmDateTime
    //return mNextSummary;
    //return mNextAlarmEventDateTime;
    bool newNextAlarm = false; 
    bool computeNextAlarm = false;
    bool ok;
    int offset;
    QDateTime nextA;
    // QString nextSum;
    //QDateTime nextEvent;
    if ( mNextAlarmIncidence == 0 || incidence == 0 ) {
        computeNextAlarm = true;
    } else {
        if ( ! deleted ) {
            nextA = incidence->getNextAlarmDateTime(& ok, &offset, QDateTime::currentDateTime() ) ;
            if ( ok ) {
                if ( nextA < mNextAlarmDateTime  ) {
                    deRegisterAlarm();
                    mNextAlarmDateTime = nextA;
                    mNextSummary  = incidence->summary();
                    mNextAlarmEventDateTime = nextA.addSecs(offset ) ; 
                    mNextAlarmEventDateTimeString = KGlobal::locale()->formatDateTime(mNextAlarmEventDateTime);
                    newNextAlarm = true;
                    mNextAlarmIncidence = incidence;
                } else {
                    if ( incidence == mNextAlarmIncidence ) {
                        computeNextAlarm = true;
                    }
                }
            } else {
                if ( mNextAlarmIncidence == incidence ) {
                    computeNextAlarm = true;
                }
            }
        } else { // deleted
            if ( incidence == mNextAlarmIncidence ) {
                computeNextAlarm = true;
            }
        }
    }
    if ( computeNextAlarm ) {
        deRegisterAlarm();
        nextA = nextAlarm( 1000 );  
        if (! mNextAlarmIncidence ) {
            return;
        }
        newNextAlarm = true;
    }
    if ( newNextAlarm )
        registerAlarm();
}
QString CalendarLocal:: getAlarmNotification()
{ 
    QString ret;
    // this should not happen
    if (! mNextAlarmIncidence ) 
            return "cal_alarm"+ mNextSummary.left( 25 )+"\n"+mNextAlarmEventDateTimeString;
    Alarm* alarm =  mNextAlarmIncidence->alarms().first();
    if ( alarm->type() == Alarm::Procedure ) {
        ret = "proc_alarm" + alarm->programFile()+"+++";
    } else {
        ret = "audio_alarm" +alarm->audioFile() +"+++";
    }
    ret += "cal_alarm"+ mNextSummary.left( 25 );
    if ( mNextSummary.length() > 25 )
        ret += "\n" + mNextSummary.mid(25, 25 );
    ret+= "\n"+mNextAlarmEventDateTimeString;
    return ret;
}
void CalendarLocal::registerAlarm()
{
    mLastAlarmNotificationString = getAlarmNotification();
    // qDebug("++ register Alarm %s %s",mNextAlarmDateTime.toString().latin1(), mLastAlarmNotificationString.latin1() );
    emit addAlarm ( mNextAlarmDateTime, mLastAlarmNotificationString );
// #ifndef DESKTOP_VERSION
//     AlarmServer::addAlarm ( mNextAlarmDateTime,"koalarm", mLastAlarmNotificationString.latin1() );
// #endif     
}
void CalendarLocal::deRegisterAlarm()
{
    if ( mLastAlarmNotificationString.isNull() )
        return;
    //qDebug("-- deregister Alarm %s ", mLastAlarmNotificationString.latin1() );

    emit removeAlarm ( mNextAlarmDateTime, mLastAlarmNotificationString );
    mNextAlarmEventDateTime = QDateTime();
// #ifndef DESKTOP_VERSION
//     AlarmServer::deleteAlarm (mNextAlarmDateTime ,"koalarm" ,mLastAlarmNotificationString.latin1() );
// #endif     
}

QPtrList<Todo> CalendarLocal::todos( const QDate &date )
{
  QPtrList<Todo> todos;

  Todo *todo;
  for ( todo = mTodoList.first(); todo; todo = mTodoList.next() ) {
      if ( !todo->calEnabled() ) continue; 
    if ( todo->hasDueDate() && todo->dtDue().date() == date ) {
      todos.append( todo );
    }
  }

 filter()->apply( &todos );
  return todos;
}
void CalendarLocal::reInitAlarmSettings()
{
    if ( !mNextAlarmIncidence  ) {
        nextAlarm( 1000 );
    }
    deRegisterAlarm();
    mNextAlarmIncidence = 0;
    checkAlarmForIncidence( 0, false );
    
}



QDateTime  CalendarLocal::nextAlarm( int daysTo )
{
  QDateTime nextA = QDateTime::currentDateTime().addDays( daysTo );
    QDateTime start = QDateTime::currentDateTime().addSecs( 30 );
    QDateTime next;
    Event *e;
    bool ok;
    bool found = false;
    int offset;
    mNextAlarmIncidence = 0;
    for( e = mEventList.first(); e; e = mEventList.next() ) {
        if ( !e->calEnabled() ) continue;
        next = e->getNextAlarmDateTime(& ok, &offset, QDateTime::currentDateTime() ) ;
        if ( ok ) {
            if (  next < nextA ) {
                nextA = next;
                found = true;
                mNextSummary = e->summary();
                mNextAlarmEventDateTime = next.addSecs(offset ) ; 
                mNextAlarmIncidence = (Incidence *) e;
            }
        }
    }
    Todo *t;
    for( t = mTodoList.first(); t; t = mTodoList.next() ) {
      if ( !t->calEnabled() ) continue; 
        next = t->getNextAlarmDateTime(& ok, &offset, QDateTime::currentDateTime() ) ;
        if ( ok ) {
            if (  next < nextA ) {
                nextA = next;
                found = true;
                mNextSummary = t->summary();
                mNextAlarmEventDateTime = next.addSecs(offset ); 
                mNextAlarmIncidence = (Incidence *) t;
            }
        }
    } 
    if ( mNextAlarmIncidence  ) {
        mNextAlarmEventDateTimeString = KGlobal::locale()->formatDateTime(mNextAlarmEventDateTime);
        mNextAlarmDateTime = nextA;
    }
    return nextA;
}
Alarm::List CalendarLocal::alarmsTo( const QDateTime &to )
{
  return alarms( QDateTime( QDate( 1900, 1, 1 ) ), to );
}

Alarm::List CalendarLocal::alarms( const QDateTime &from, const QDateTime &to )
{
 
  Alarm::List alarms;

  Event *e;

  for( e = mEventList.first(); e; e = mEventList.next() ) {
      if ( !e->calEnabled() ) continue;
    if ( e->doesRecur() ) appendRecurringAlarms( alarms, e, from, to );
    else appendAlarms( alarms, e, from, to );
  }

  Todo *t;
  for( t = mTodoList.first(); t; t = mTodoList.next() ) {
      if ( !t->calEnabled() ) continue; 
      appendAlarms( alarms, t, from, to );
  }

  return alarms;
}

void CalendarLocal::appendAlarms( Alarm::List &alarms, Incidence *incidence,
                                  const QDateTime &from, const QDateTime &to )
{
  QPtrList<Alarm> alarmList = incidence->alarms();
  Alarm *alarm;
  for( alarm = alarmList.first(); alarm; alarm = alarmList.next() ) {
//    kdDebug(5800) << "CalendarLocal::appendAlarms() '" << alarm->text()
//                  << "': " << alarm->time().toString() << " - " << alarm->enabled() << endl;
    if ( alarm->enabled() ) {
      if ( alarm->time() >= from && alarm->time() <= to ) {
        alarms.append( alarm );
      }
    }
  }
}

void CalendarLocal::appendRecurringAlarms( Alarm::List &alarms,
                                           Incidence *incidence,
                                           const QDateTime &from,
                                           const QDateTime &to )
{
 
  QPtrList<Alarm> alarmList = incidence->alarms();
  Alarm *alarm;
  QDateTime qdt;
  for( alarm = alarmList.first(); alarm; alarm = alarmList.next() ) {
    if (incidence->recursOn(from.date())) {
      qdt.setTime(alarm->time().time());
      qdt.setDate(from.date());
    }
    else qdt = alarm->time();
    // qDebug("1 %s %s %s", qdt.toString().latin1(),  from.toString().latin1(), to.toString().latin1());
    if ( alarm->enabled() ) {
      if ( qdt >= from && qdt <= to ) {
        alarms.append( alarm );
      }
    }
  }
}


/****************************** PROTECTED METHODS ****************************/

// after changes are made to an event, this should be called.
void CalendarLocal::update( IncidenceBase *incidence )
{
  incidence->setSyncStatus( Event::SYNCMOD );
  incidence->setLastModified( QDateTime::currentDateTime() );
  // we should probably update the revision number here,
  // or internally in the Event itself when certain things change.
  // need to verify with ical documentation.

  setModified( true );
}

void CalendarLocal::insertEvent( Event *event )
{
  if ( mEventList.findRef( event ) < 0 ) mEventList.append( event );
}


QPtrList<Event> CalendarLocal::rawEventsForDate( const QDate &qd, bool sorted )
{
  QPtrList<Event> eventList;

  Event *event;
  for( event = mEventList.first(); event; event = mEventList.next() ) {
      if ( !event->calEnabled() ) continue;
    if ( event->doesRecur() ) {
      if ( event->isMultiDay() ) {
        int extraDays = event->dtStart().date().daysTo( event->dtEnd().date() );
        int i;
        for ( i = 0; i <= extraDays; i++ ) {
            if ( event->recursOn( qd.addDays( -i ) ) ) {
                eventList.append( event );
                break;
            }
        }
      } else {
        if ( event->recursOn( qd ) )
          eventList.append( event );
      }
    } else {
      if ( event->dtStart().date() <= qd && event->dtEnd().date() >= qd ) {
        eventList.append( event );
      }
    }
  }

  if ( !sorted ) {
    return eventList;
  }

  //  kdDebug(5800) << "Sorting events for date\n" << endl;
  // now, we have to sort it based on dtStart.time()
  QPtrList<Event> eventListSorted;
  Event *sortEvent;
  for ( event = eventList.first(); event; event = eventList.next() ) {
    sortEvent = eventListSorted.first();
    int i = 0;
    while ( sortEvent && event->dtStart().time()>=sortEvent->dtStart().time() )
    {
      i++;
      sortEvent = eventListSorted.next();
    }
    eventListSorted.insert( i, event );
  }
  return eventListSorted;
}


QPtrList<Event> CalendarLocal::rawEvents( const QDate &start, const QDate &end,
                                          bool inclusive )
{
  Event *event = 0;

  QPtrList<Event> eventList;

  // Get non-recurring events
  for( event = mEventList.first(); event; event = mEventList.next() ) {
      if ( !event->calEnabled() ) continue;
    if ( event->doesRecur() ) {
      QDate rStart = event->dtStart().date();
      bool found = false;
      if ( inclusive ) {
        if ( rStart >= start && rStart <= end ) {
          // Start date of event is in range. Now check for end date.
          // if duration is negative, event recurs forever, so do not include it.
          if ( event->recurrence()->duration() == 0 ) {  // End date set
            QDate rEnd = event->recurrence()->endDate();
            if ( rEnd >= start && rEnd <= end ) {  // End date within range
              found = true;
            }
          } else if ( event->recurrence()->duration() > 0 ) {  // Duration set
            // TODO: Calculate end date from duration. Should be done in Event
            // For now exclude all events with a duration.
          }
        }
      } else {
          bool founOne;
          QDate next = event->getNextOccurence( QDateTime(  start ), &founOne ).date();
          if ( founOne ) {
              if ( next <= end ) {
                  found = true;
              }
          }
          
          /*
          // crap !!!
        if ( rStart <= end ) {  // Start date not after range
          if ( rStart >= start ) {  // Start date within range
            found = true;
          } else if ( event->recurrence()->duration() == -1 ) {  // Recurs forever
            found = true;
          } else if ( event->recurrence()->duration() == 0 ) {  // End date set
            QDate rEnd = event->recurrence()->endDate();
            if ( rEnd >= start && rEnd <= end ) {  // End date within range
              found = true;
            }
          } else {  // Duration set
            // TODO: Calculate end date from duration. Should be done in Event
            // For now include all events with a duration.
            found = true;
          }
        }
          */

      }

      if ( found ) eventList.append( event );
    } else {
      QDate s = event->dtStart().date();
      QDate e = event->dtEnd().date();

      if ( inclusive ) {
        if ( s >= start && e <= end ) {
          eventList.append( event );
        }
      } else {
        if ( ( e >= start && s <= end ) ) {
          eventList.append( event );
        }
      }
    }
  }

  return eventList;
}

QPtrList<Event> CalendarLocal::rawEventsForDate( const QDateTime &qdt )
{
  return rawEventsForDate( qdt.date() );
}

QPtrList<Event> CalendarLocal::rawEvents()
{
    QPtrList<Event> el;
    for ( Event *it = mEventList.first(); it; it = mEventList.next() )
        if ( it->calEnabled() ) el.append( it );
    return el;
}

bool CalendarLocal::addJournal(Journal *journal)
{
    mJournalList.append(journal);

  journal->registerObserver( this );

  setModified( true );
  if ( journal->calID() == 0 )
      journal->setCalID_block( mDefaultCalendar );
  journal->setCalEnabled( true );
  return true;
}

void CalendarLocal::deleteJournal( Journal *journal )
{
    clearUndo(journal);
  if ( mJournalList.removeRef(journal) ) {
    setModified( true );
  }
}

QPtrList<Journal> CalendarLocal::journals4Date( const QDate & date )
{
    QPtrList<Journal> el;
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->calEnabled() && it->dtStart().date() == date) el.append( it );
    return el;
}
Journal *CalendarLocal::journal( const QDate &date )
{
//  kdDebug(5800) << "CalendarLocal::journal() " << date.toString() << endl;

  for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
      if ( it->calEnabled() && it->dtStart().date() == date )
      return it;

  return 0;
}

Journal *CalendarLocal::journal( const QString &uid )
{
    Journal * retVal = 0;
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->calEnabled() && it->uid() == uid ) {
            if ( retVal ) {
                if ( retVal->calID() > it->calID() ) {
                    retVal = it;
                }
            } else {
                retVal = it;
            }
        }
    return retVal;
}

QPtrList<Journal> CalendarLocal::journals()
{
    QPtrList<Journal> el;
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->calEnabled() ) el.append( it );
    return el;
}
void CalendarLocal::setCalendarRemove( int id )
{
   
    {
        QPtrList<Event> EventList = mEventList;
        Event * ev = EventList.first();
        while ( ev ) {
            if ( ev->calID() == id && ev->uid().left( 15 ) != QString("last-syncEvent-") )
                deleteEvent( ev );
            ev = EventList.next();
        }
    }
    {

        QPtrList<Todo> TodoList = mTodoList;
        Todo * ev = TodoList.first();
        while ( ev ) {
            if ( ev->calID() == id )
                deleteTodo( ev );
            ev = TodoList.next();
        }
    }
    {
        QPtrList<Journal> JournalList = mJournalList;
        Journal * ev = JournalList.first();
        while ( ev ) {
            if ( ev->calID() == id )
                deleteJournal( ev );
            ev = JournalList.next();
        }
    }

    clearUndo(0);

}

void CalendarLocal::setAllCalendarEnabled( bool enable )
{
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        it->setCalEnabled( enable );

    for ( Event *it = mEventList.first(); it; it = mEventList.next() )
        it->setCalEnabled( enable );

    for ( Todo *it = mTodoList.first(); it; it = mTodoList.next() )
        it->setCalEnabled( enable );


}
void CalendarLocal::setCalendarEnabled( int id, bool enable )
{
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->calID() == id ) it->setCalEnabled( enable );

    for ( Event *it = mEventList.first(); it; it = mEventList.next() )
        if ( it->calID() == id ) it->setCalEnabled( enable );

    for ( Todo *it = mTodoList.first(); it; it = mTodoList.next() )
        if ( it->calID() == id ) it->setCalEnabled( enable );

}

void CalendarLocal::setReadOnly( int id, bool enable )
{
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->calID() == id ) it->setReadOnly( enable );

    for ( Event *it = mEventList.first(); it; it = mEventList.next() )
        if ( it->calID() == id ) it->setReadOnly( enable );

    for ( Todo *it = mTodoList.first(); it; it = mTodoList.next() )
        if ( it->calID() == id ) it->setReadOnly( enable );

}

void CalendarLocal::setAlarmEnabled( int id, bool enable )
{
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        if ( it->calID() == id ) it->setAlarmEnabled( enable );

    for ( Event *it = mEventList.first(); it; it = mEventList.next() )
        if ( it->calID() == id ) it->setAlarmEnabled( enable );

    for ( Todo *it = mTodoList.first(); it; it = mTodoList.next() )
        if ( it->calID() == id ) it->setAlarmEnabled( enable );
    reInitAlarmSettings();

}
void CalendarLocal::setDefaultCalendarEnabledOnly()
{
    for ( Journal *it = mJournalList.first(); it; it = mJournalList.next() )
        it->setCalEnabled( it->calID() == mDefaultCalendar );

    for ( Event *it = mEventList.first(); it; it = mEventList.next() )
        it->setCalEnabled( it->calID() == mDefaultCalendar);

    for ( Todo *it = mTodoList.first(); it; it = mTodoList.next() )
        it->setCalEnabled( it->calID() == mDefaultCalendar);

}
