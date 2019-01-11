/*
    This file is part of libkcal.
    Copyright (c) 1998 Preston Brown
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <stdlib.h>
#include <time.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

#include "exceptions.h"
#include "calfilter.h"

#include "calendar.h"
#include "syncdefines.h"

using namespace KCal;

Calendar::Calendar()
{
  
  init();
  setTimeZoneId( " 00:00 Europe/London(UTC)" );
}

Calendar::Calendar( const QString &timeZoneId )
{ 

    init();
    setTimeZoneId(timeZoneId);
}

void Calendar::init()
{
  mObserver = 0;
  mNewObserver = false;
  mUndoIncidence = 0;
  mDeleteIncidencesOnClose = true;
  mModified = false;
  mDefaultCalendar = 1;
  // Setup default filter, which does nothing
  mDefaultFilter = new CalFilter;
  mFilter = mDefaultFilter;
  mFilter->setEnabled(false);

  // initialize random numbers.  This is a hack, and not
  // even that good of one at that.
//  srandom(time(0));

  // user information...
  setOwner(i18n("Unknown Name"));
  setEmail(i18n("unknown@nowhere"));

#if 0
  tmpStr = KOPrefs::instance()->mTimeZone;
//  kdDebug(5800) << "Calendar::Calendar(): TimeZone: " << tmpStr << endl;
  int dstSetting = KOPrefs::instance()->mDaylightSavings;
  extern long int timezone;
  struct tm *now;
  time_t curtime;
  curtime = time(0);
  now = localtime(&curtime);
  int hourOff = - ((timezone / 60) / 60);
  if (now->tm_isdst)
    hourOff += 1;
  QString tzStr;
  tzStr.sprintf("%.2d%.2d",
		hourOff,
		abs((timezone / 60) % 60));

  // if no time zone was in the config file, write what we just discovered.
  if (tmpStr.isEmpty()) {
//    KOPrefs::instance()->mTimeZone = tzStr;
  } else {
    tzStr = tmpStr;
  }

  // if daylight savings has changed since last load time, we need
  // to rewrite these settings to the config file.
  if ((now->tm_isdst && !dstSetting) ||
      (!now->tm_isdst && dstSetting)) {
    KOPrefs::instance()->mTimeZone = tzStr;
    KOPrefs::instance()->mDaylightSavings = now->tm_isdst;
  }

  setTimeZone(tzStr);
#endif

//  KOPrefs::instance()->writeConfig();
}

Calendar::~Calendar()
{
  delete mDefaultFilter;
  clearUndo( 0 );
} 
void Calendar::clearUndo( Incidence * newUndo )
{
    
    if ( mUndoIncidence ) {
        if ( mUndoIncidence->typeID() == eventID )
            delete  ((Event*) mUndoIncidence) ;
        else if ( mUndoIncidence->typeID() == todoID )
            delete ( (Todo*) mUndoIncidence );
        else if ( mUndoIncidence->typeID() == journalID )
            delete ( (Journal*) mUndoIncidence );
        else 
            delete mUndoIncidence;
    }
    mUndoIncidence =  newUndo;
    if ( mUndoIncidence ) {
        mUndoIncidence->clearRelations();
    }
    
}   

void Calendar::setDontDeleteIncidencesOnClose ()
{
    mDeleteIncidencesOnClose = false;
}
void Calendar::setDefaultCalendar( int d )
{
    mDefaultCalendar = d;
}
int Calendar::defaultCalendar()
{
    return mDefaultCalendar;
}
const QString &Calendar::getOwner() const
{
  return mOwner;
}

bool Calendar::undoDeleteIncidence()
{
    if (!mUndoIncidence)
        return false;
    addIncidence(mUndoIncidence);
    mUndoIncidence = 0;
    return true;
}
void Calendar::setOwner(const QString &os)
{
  int i;
  mOwner = os;
  i = mOwner.find(',');
  if (i != -1)
    mOwner = mOwner.left(i);

  setModified( true );
}

void Calendar::setTimeZone(const QString & tz)
{
  bool neg = FALSE;
  int hours, minutes;
  QString tmpStr(tz);

  if (tmpStr.left(1) == "-")
    neg = TRUE;
  if (tmpStr.left(1) == "-" || tmpStr.left(1) == "+")
    tmpStr.remove(0, 1);
  hours = tmpStr.left(2).toInt();
  if (tmpStr.length() > 2)
    minutes = tmpStr.right(2).toInt();
  else
    minutes = 0;
  mTimeZone = (60*hours+minutes);
  if (neg)
    mTimeZone = -mTimeZone;
  mLocalTime = false;

  setModified( true );
}

QString Calendar::getTimeZoneStr() const
{
  if (mLocalTime)
    return "";
  QString tmpStr;
  int hours = abs(mTimeZone / 60);
  int minutes = abs(mTimeZone % 60);
  bool neg = mTimeZone < 0;

  tmpStr.sprintf("%c%.2d%.2d",
		 (neg ? '-' : '+'),
		 hours, minutes);
  return tmpStr;
}

void Calendar::setTimeZone(int tz)
{
  mTimeZone = tz;
  mLocalTime = false;

  setModified( true );
}

int Calendar::getTimeZone() const
{
  return mTimeZone;
}

void Calendar::setTimeZoneId(const QString &id)
{ 
    mTimeZoneId = id;
    mLocalTime = false;
    mTimeZone  = KGlobal::locale()->timezoneOffset(mTimeZoneId);
    if ( mTimeZone > 1000)
         setLocalTime();
    //qDebug("Calendar::setTimeZoneOffset %s %d ",mTimeZoneId.latin1(), mTimeZone);
    setModified( true );
}

QString Calendar::timeZoneId() const
{
  return mTimeZoneId;
}

void Calendar::setLocalTime()
{ 
    //qDebug("Calendar::setLocalTime() ");
    mLocalTime = true;
    mTimeZone = 0;
    mTimeZoneId = "";

  setModified( true );
}

bool Calendar::isLocalTime() const
{
  return mLocalTime;
}

const QString &Calendar::getEmail()
{
  return mOwnerEmail;
}

void Calendar::setEmail(const QString &e)
{
  mOwnerEmail = e;

  setModified( true );
}

void Calendar::setFilter(CalFilter *filter)
{
  mFilter = filter;
}

CalFilter *Calendar::filter()
{
  return mFilter;
}

QPtrList<Incidence> Calendar::incidences()
{
  QPtrList<Incidence> incidences;
  
  Incidence *i;

  QPtrList<Event> e = events();
  for( i = e.first(); i; i = e.next() ) incidences.append( i );

  QPtrList<Todo> t = todos();
  for( i = t.first(); i; i = t.next() ) incidences.append( i );

  QPtrList<Journal> j = journals();
  for( i = j.first(); i; i = j.next() ) incidences.append( i );

  return incidences;
}

void Calendar::resetPilotStat(int id )
{
 QPtrList<Incidence> incidences;
  
  Incidence *i;

  QPtrList<Event> e = rawEvents();
  for( i = e.first(); i; i = e.next() ) i->setPilotId( id );

  QPtrList<Todo> t = rawTodos();
  for( i = t.first(); i; i = t.next() ) i->setPilotId( id );

  QPtrList<Journal> j = journals();
  for( i = j.first(); i; i = j.next() ) i->setPilotId( id );
}
void Calendar::resetTempSyncStat()
{
 QPtrList<Incidence> incidences;
  
  Incidence *i;

  QPtrList<Event> e = rawEvents();
  for( i = e.first(); i; i = e.next() ) i->setTempSyncStat( SYNC_TEMPSTATE_INITIAL );

  QPtrList<Todo> t = rawTodos();
  for( i = t.first(); i; i = t.next() ) i->setTempSyncStat( SYNC_TEMPSTATE_INITIAL );

  QPtrList<Journal> j = journals();
  for( i = j.first(); i; i = j.next() ) i->setTempSyncStat( SYNC_TEMPSTATE_INITIAL );
}
QPtrList<Incidence> Calendar::rawIncidences()
{
  QPtrList<Incidence> incidences;
  
  Incidence *i;

  QPtrList<Event> e = rawEvents();
  for( i = e.first(); i; i = e.next() ) incidences.append( i );

  QPtrList<Todo> t = rawTodos();
  for( i = t.first(); i; i = t.next() ) incidences.append( i );

  QPtrList<Journal> j = journals();
  for( i = j.first(); i; i = j.next() ) incidences.append( i );

  return incidences;
}

QPtrList<Event> Calendar::events( const QDate &date, bool sorted )
{
  QPtrList<Event> el = rawEventsForDate(date,sorted);
  mFilter->apply(&el);
  return el;
}

QPtrList<Event> Calendar::events( const QDateTime &qdt )
{
  QPtrList<Event> el = rawEventsForDate(qdt);
  mFilter->apply(&el);
  return el;
}

QPtrList<Event> Calendar::events( const QDate &start, const QDate &end,
                                  bool inclusive)
{
  QPtrList<Event> el = rawEvents(start,end,inclusive);
  mFilter->apply(&el);
  return el;
}

QPtrList<Event> Calendar::events()
{
  QPtrList<Event> el = rawEvents();
  mFilter->apply(&el);
  return el;
}
void Calendar::addIncidenceBranch(Incidence *i)
{
    addIncidence( i );
    Incidence * inc;
    QPtrList<Incidence> Relations = i->relations();
    for (inc=Relations.first();inc;inc=Relations.next()) {
       addIncidenceBranch( inc );
    } 
}

bool Calendar::addIncidence(Incidence *i)
{
  Incidence::AddVisitor<Calendar> v(this);
  if ( i->calID() == 0 )
      i->setCalID_block( mDefaultCalendar );
  i->setCalEnabled( true );
  return i->accept(v);
}
void Calendar::deleteIncidence(Incidence *in)
{
    if ( in->typeID() == eventID )
        deleteEvent( (Event*) in );
    else if ( in->typeID() == todoID )
        deleteTodo( (Todo*) in);
    else if ( in->typeID() == journalID )
        deleteJournal( (Journal*) in );
}

Incidence * Calendar::getCurrentNextAlarmIncidence()
{
    return mNextAlarmIncidence;
}

Incidence* Calendar::incidence( const QString& uid )
{
  Incidence* i;

  if( (i = todo( uid )) != 0 )
    return i;
  if( (i = event( uid )) != 0 )
    return i;
  if( (i = journal( uid )) != 0 )
      return i;

  return 0;
}

QPtrList<Todo> Calendar::todos()
{
  QPtrList<Todo> tl = rawTodos();
  mFilter->apply( &tl );
  return tl;
}

// When this is called, the todo have already been added to the calendar.
// This method is only about linking related todos
void Calendar::setupRelations( Incidence *incidence )
{
  QString uid = incidence->uid();
  //qDebug("Calendar::setupRelations %s", incidence->summary().latin1());
  // First, go over the list of orphans and see if this is their parent
  while( Incidence* i = mOrphans[ uid ] ) {
    mOrphans.remove( uid );
    i->setRelatedTo( incidence );
    //qDebug("Add child %s ti inc  %s", i->summary().latin1(),incidence->summary().latin1());
    incidence->addRelation( i );
    mOrphanUids.remove( i->uid() );
  }

  // Now see about this incidences parent
  if( !incidence->relatedTo() && !incidence->relatedToUid().isEmpty() ) {
    // This incidence has a uid it is related to, but is not registered to it yet
    // Try to find it
    //qDebug("Test parent for   %s", incidence->summary().latin1());
    Incidence* parent = this->incidenceForUid( incidence->relatedToUid(), true );
    if( parent ) {
      // Found it
        // qDebug("parent found for for   %s", incidence->summary().latin1());
      incidence->setRelatedTo( parent );
      parent->addRelation( incidence );
    } else {
        // qDebug("NO parent found for for   %s", incidence->summary().latin1());
      // Not found, put this in the mOrphans list
      mOrphans.insert( incidence->relatedToUid(), incidence );
      mOrphanUids.insert( incidence->uid(), incidence );
    }
  }
}

// If a task with subtasks is deleted, move it's subtasks to the orphans list
void Calendar::removeRelations( Incidence *incidence )
{
    // qDebug("Calendar::removeRelations ");
    QString uid = incidence->uid();

    QPtrList<Incidence> relations = incidence->relations();
    for( Incidence* i = relations.first(); i; i = relations.next() )
        if( !mOrphanUids.find( i->uid() ) ) {
            mOrphans.insert( uid, i );
            mOrphanUids.insert( i->uid(), i );
            i->setRelatedTo( 0 );
            i->setRelatedToUid( uid );
        }

    // If this incidence is related to something else, tell that about it
    if( incidence->relatedTo() )
        incidence->relatedTo()->removeRelation( incidence );

    // Remove this one from the orphans list
    if( mOrphanUids.remove( uid ) ) {
        QString r2uid = incidence->relatedToUid();
        QPtrList<Incidence> tempList;
        while( Incidence* i = mOrphans[ r2uid ] ) {
            mOrphans.remove( r2uid );
            if ( i != incidence ) tempList.append( i );
        }
        Incidence* inc = tempList.first();
        while ( inc ) {
            mOrphans.insert( r2uid, inc );
            inc = tempList.next();
        }
    }
    // LR: and another big bad bug found
#if 0
        // This incidence is located in the orphans list - it should be removed
        if( !( incidence->relatedTo() != 0 && mOrphans.remove( incidence->relatedTo()->uid() ) ) ) {
            // Removing wasn't that easy
            for( QDictIterator<Incidence> it( mOrphans ); it.current(); ++it ) {
                if( it.current()->uid() == uid ) {
                    mOrphans.remove( it.currentKey() );
                    break;
                }
            }
        }
#endif
}

void Calendar::registerObserver( Observer *observer )
{
  mObserver = observer;
  mNewObserver = true;
}

void Calendar::setModified( bool modified )
{
    if ( mObserver ) mObserver->calendarModified( modified, this );
  if ( modified != mModified || mNewObserver ) {
    mNewObserver = false;
    // if ( mObserver ) mObserver->calendarModified( modified, this );
    mModified = modified;
  }
}

void Calendar::setLoadedProductId( const QString &id )
{
  mLoadedProductId = id;
}

QString Calendar::loadedProductId()
{
  return mLoadedProductId;
}

//#include "calendar.moc"
