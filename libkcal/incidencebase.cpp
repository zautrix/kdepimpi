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
#include <kidmanager.h>

#include "calformat.h"
#include "syncdefines.h"

#include "incidencebase.h"

using namespace KCal;

IncidenceBase::IncidenceBase() :
  mReadOnly(false), mFloats(true), mDuration(0), mHasDuration(false),
  mPilotId(0), mSyncStatus(SYNCMOD)
{
  blockLastModified = false;
  setUid(CalFormat::createUniqueId());
  mOrganizer = "";
  mFloats = false;
  mDuration = 0;
  mHasDuration = false;
  mPilotId = 0;
  mExternalId = ":";
  mTempSyncStat =  SYNC_TEMPSTATE_INITIAL;
  mSyncStatus = 0;
  mAttendees.setAutoDelete( true );
  mCalEnabled = true;
  mAlarmEnabled = true;
  mCalID = 0;
}

IncidenceBase::IncidenceBase(const IncidenceBase &i) :
  CustomProperties( i )
{

  blockLastModified = false;
  mReadOnly = i.mReadOnly;
  mDtStart = i.mDtStart;
  mDuration = i.mDuration;
  mHasDuration = i.mHasDuration;
  mOrganizer = i.mOrganizer;
  mUid = i.mUid;
  mCalEnabled = i.mCalEnabled;
  mAlarmEnabled = i.mAlarmEnabled;
  mCalID = i.mCalID;
  QPtrList<Attendee> attendees = i.attendees();
  for( Attendee *a = attendees.first(); a; a = attendees.next() ) {
    mAttendees.append( new Attendee( *a ) );
  }
  mFloats = i.mFloats;
  mLastModified = i.mLastModified;
  mPilotId = i.mPilotId;
  mTempSyncStat = i.mTempSyncStat;
  mSyncStatus = i.mSyncStatus;
  mExternalId = i.mExternalId;
  // The copied object is a new one, so it isn't observed by the observer
  // of the original object.
  mObservers.clear();
  
  mAttendees.setAutoDelete( true );
}

IncidenceBase::~IncidenceBase()
{
}


bool KCal::operator==( const IncidenceBase& i1, const IncidenceBase& i2 )
{
    // do not compare mSyncStatus and mExternalId 
    if( i1.attendees().count() != i2.attendees().count() ) {
        return false; // no need to check further
    }
    if ( i1.attendees().count() > 0 ) {
        Attendee * a1 = i1.attendees().first(), *a2 =i2.attendees().first() ;
        while ( a1 ) {
            if ( !( (*a1) == (*a2)) ) 
                {
                    //qDebug("Attendee not equal ");
                    return false;
                }
            a1 = i1.attendees().next();
            a2 = i2.attendees().next();
        }
    }
    //if ( i1.dtStart() != i2.dtStart() )
    //  return false;
#if 0
    qDebug("1 %d ",i1.doesFloat() == i2.doesFloat() );
    qDebug("1 %d ",i1.duration() == i2.duration() );
    qDebug("3 %d ",i1.hasDuration() == i2.hasDuration() );
    qDebug("1 %d ",i1.pilotId() == i2.pilotId() );
    qDebug("1 %d %d %d",i1.syncStatus() == i2.syncStatus() , i1.syncStatus(),i2.syncStatus() );
    qDebug("6 %d ",i1.organizer() == i2.organizer() );
  
#endif
    if ( i1.hasDuration() == i2.hasDuration() ) {
        if ( i1.hasDuration() ) {
            if ( i1.duration() != i2.duration() )
                return false;
        }
    } else {
        return false;
    }

    return ( i1.organizer() == i2.organizer() &&
             // i1.uid() == i2.uid() &&
             // Don't compare lastModified, otherwise the operator is not
             // of much use. We are not comparing for identity, after all.
             i1.doesFloat() == i2.doesFloat() &&
             i1.pilotId() == i2.pilotId() );// &&  i1.syncStatus() == i2.syncStatus() );
    // no need to compare mObserver
}

  
QDateTime IncidenceBase::getEvenTime( QDateTime dt )
{
    QTime t =  dt.time();
    dt.setTime( QTime (t.hour (), t.minute (), t.second () )  );
    return dt;
}     

bool IncidenceBase::isTagged() const
{
    return mIsTagged;
}
void IncidenceBase::setTagged( bool b)
{
    mIsTagged = b;
}
void IncidenceBase::setCalID( int id )
{
    if ( mCalID > 0 ) {
        updated();
    }
    mCalID = id;
}
void IncidenceBase::setCalID_block( int id )
{
    if ( mCalID > 0 ) {
        blockLastModified = true;
        updated();
        blockLastModified = false;
    }
    mCalID = id;
}
int IncidenceBase::calID() const
{
    return mCalID;
}
void IncidenceBase::setCalEnabled( bool b )
{
    mCalEnabled = b;
}
bool IncidenceBase::calEnabled() const
{
    return mCalEnabled;
}

void IncidenceBase::setAlarmEnabled( bool b )
{
    mAlarmEnabled = b;
}
bool IncidenceBase::alarmEnabled() const
{
    return mAlarmEnabled;
}


void IncidenceBase::setUid(const QString &uid)
{
  mUid = uid;
  updated();
}

QString IncidenceBase::uid() const
{
  return mUid;
}
void IncidenceBase::setLastModifiedSubInvalid()
{
    // virtual method
}
void IncidenceBase::setLastModified(const QDateTime &lm)
{
    if ( blockLastModified ) return;
  // DON'T! updated() because we call this from
  // Calendar::updateEvent().
  mLastModified = getEvenTime(lm);
  mLastModifiedKey.sprintf("%04d%02d%02d%02d%02d%02d",
                           mLastModified.date().year(),
                           mLastModified.date().month(),
                           mLastModified.date().day(),
                           mLastModified.time().hour(),
                           mLastModified.time().minute(),
                           mLastModified.time().second() );
  setLastModifiedSubInvalid();
  //qDebug("IncidenceBase::setLastModified %s ",lm.toString().latin1());
}
QString IncidenceBase::lastModifiedSortKey() const
{  
    return mLastModifiedKey;
}

QDateTime IncidenceBase::lastModified() const
{
  return mLastModified;
}

void IncidenceBase::setOrganizer(const QString &o)
{
  // we don't check for readonly here, because it is
  // possible that by setting the organizer we are changing
  // the event's readonly status...
  mOrganizer = o;
  if (mOrganizer.left(7).upper() == "MAILTO:")
    mOrganizer = mOrganizer.remove(0,7);

  updated();
}

QString IncidenceBase::organizer() const
{
  return mOrganizer;
}

void IncidenceBase::setReadOnly( bool readOnly )
{
  mReadOnly = readOnly;
}

void IncidenceBase::setDtStart(const QDateTime &dtStart)
{
//  if (mReadOnly) return;
  mDtStart = getEvenTime(dtStart);
  updated();
}


QDateTime IncidenceBase::dtStart() const
{
  return mDtStart;
}

QString IncidenceBase::dtStartTimeStr() const
{
  return KGlobal::locale()->formatTime(dtStart().time());
}

QString IncidenceBase::dtStartDateStr(bool shortfmt) const
{
  return KGlobal::locale()->formatDate(dtStart().date(),shortfmt);
}

QString IncidenceBase::dtStartStr(bool shortfmt) const
{
    if ( doesFloat() )
        return KGlobal::locale()->formatDate(dtStart().date(),shortfmt);
    return KGlobal::locale()->formatDateTime(dtStart(), shortfmt);
}


bool IncidenceBase::doesFloat() const
{
  return mFloats;
}

void IncidenceBase::setFloats(bool f)
{
  if (mReadOnly) return;
  mFloats = f;
  updated();
}


bool IncidenceBase::addAttendee(Attendee *a, bool doupdate)
{
  if (mReadOnly) return false;
  if (a->name().left(7).upper() == "MAILTO:")
    a->setName(a->name().remove(0,7));

  QPtrListIterator<Attendee> qli(mAttendees);
  
  qli.toFirst();
  while (qli) {
      if (*qli.current() == *a)
          return false;
      ++qli;
  }
  mAttendees.append(a);
  if (doupdate) updated();
  return true;
}

#if 0
void IncidenceBase::removeAttendee(Attendee *a)
{
  if (mReadOnly) return;
  mAttendees.removeRef(a);
  updated();
}

void IncidenceBase::removeAttendee(const char *n)
{
  Attendee *a;

  if (mReadOnly) return;
  for (a = mAttendees.first(); a; a = mAttendees.next())
    if (a->getName() == n) {
      mAttendees.remove();
      break;
    }
}
#endif

void IncidenceBase::clearAttendees()
{
  if (mReadOnly) return;
  mAttendees.clear();
}

#if 0
Attendee *IncidenceBase::getAttendee(const char *n) const
{
  QPtrListIterator<Attendee> qli(mAttendees);

  qli.toFirst();
  while (qli) {
    if (qli.current()->getName() == n)
      return qli.current();
    ++qli;
  }
  return 0L;
}
#endif

Attendee *IncidenceBase::attendeeByMail(const QString &email)
{
  QPtrListIterator<Attendee> qli(mAttendees);

  qli.toFirst();
  while (qli) {
      if (qli.current()->email().lower() == email.lower())
      return qli.current();
    ++qli;
  }
  return 0L;
}

Attendee *IncidenceBase::attendeeByMails(const QStringList &emails, const QString& email)
{
  QPtrListIterator<Attendee> qli(mAttendees);

  QStringList mails = emails;
  if (!email.isEmpty()) {
    mails.append(email);
  }
  qli.toFirst();
  while (qli) {
    for ( QStringList::Iterator it = mails.begin(); it != mails.end(); ++it ) {
        if (qli.current()->email().lower() == (*it).lower())
        return qli.current();
     }

    ++qli;
  }
  return 0L;
}

void IncidenceBase::setDuration(int seconds)
{
  mDuration = seconds;
  setHasDuration(true);
}

int IncidenceBase::duration() const
{
  return mDuration;
}

void IncidenceBase::setHasDuration(bool b)
{
  mHasDuration = b;
}

bool IncidenceBase::hasDuration() const
{
  return mHasDuration;
}

void IncidenceBase::setSyncStatus(int stat)
{
  if (mReadOnly) return;
  mSyncStatus = stat;
}

int IncidenceBase::syncStatus() const
{
  return mSyncStatus;
}

void IncidenceBase::setPilotId( int id )
{
  if (mReadOnly) return;
  mPilotId = id;
}

int IncidenceBase::pilotId() const
{
  return mPilotId;
}

int IncidenceBase::tempSyncStat() const
{
  return mTempSyncStat;
}
void IncidenceBase::setTempSyncStat( int id )
{
  if (mReadOnly) return;
  mTempSyncStat = id;
}

void IncidenceBase::removeID(const QString &prof)
{
    if ( prof.isEmpty() )
        mExternalId = ":";
    else
        mExternalId = KIdManager::removeId ( mExternalId, prof);

}
void IncidenceBase::setID( const QString & prof , const QString & id )
{
    mExternalId = KIdManager::setId ( mExternalId, prof, id );
}
QString IncidenceBase::getID( const QString &  prof)
{
    return KIdManager::getId ( mExternalId, prof ); 
}

// example  :Sharp_DTM;22;23566:TP;-1;8654:TPP;18;0:
// format name;III;JJJ:  III >= 0, may be -1.  JJJ always >= 0  
void IncidenceBase::setCsum( const QString & prof , const QString & id )
{
    mExternalId = KIdManager::setCsum ( mExternalId, prof, id );
}
QString IncidenceBase::getCsum( const QString &  prof)
{
    return KIdManager::getCsum ( mExternalId, prof ); 
}

void IncidenceBase::setIDStr( const QString & s )
{
  if (mReadOnly) return;
  mExternalId = s;
}

QString IncidenceBase::IDStr() const
{
    return mExternalId ;
}
void IncidenceBase::registerObserver( IncidenceBase::Observer *observer )
{
  if( !mObservers.contains(observer) ) mObservers.append( observer );
}

void IncidenceBase::unRegisterObserver( IncidenceBase::Observer *observer )
{
  mObservers.remove( observer );
}

void IncidenceBase::updated()
{
  QPtrListIterator<Observer> it(mObservers);
  while( it.current() ) {
    Observer *o = it.current();
    ++it;
    o->incidenceUpdated( this );
  }
}
