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

#include "calformat.h"

#include "incidence.h"
#include "todo.h"

using namespace KCal;

Incidence::Incidence() :
  IncidenceBase(),
  mRelatedTo(0), mSecrecy(SecrecyPublic), mPriority(3)
{
    mRecurrence = 0;//new Recurrence(this);
  mCancelled = false;
  recreate();
  mHasStartDate = true;
  mAlarms.setAutoDelete(true);
  mAttachments.setAutoDelete(true);
  mHasRecurrenceID = false;
  mHoliday = false;
  mBirthday = false;
  mAnniversary = false;

}

Incidence::Incidence( const Incidence &i ) : IncidenceBase( i )
{
// TODO: reenable attributes currently commented out.
  mRevision = i.mRevision;
  mCreated = i.mCreated;
  mDescription = i.mDescription;
  mSummary = i.mSummary;
  mCategories = i.mCategories;
//  Incidence *mRelatedTo;          Incidence *mRelatedTo;
  mRelatedTo = 0;
  mRelatedToUid = i.mRelatedToUid; 
//  QPtrList<Incidence> mRelations;    QPtrList<Incidence> mRelations;
  mExDates = i.mExDates;
  QPtrListIterator<Attachment> itat( i.mAttachments );
  Attachment *at;
  while( (at = itat.current()) ) {
      Attachment *a = new Attachment( *at );
      mAttachments.append( a );
      ++itat;
  }
  mAttachments.setAutoDelete( true );
  mResources = i.mResources;
  mSecrecy = i.mSecrecy;
  mPriority = i.mPriority;
  mLocation = i.mLocation;
  mCancelled = i.mCancelled;
  mHasStartDate = i.mHasStartDate;
  QPtrListIterator<Alarm> it( i.mAlarms );
  const Alarm *a;
  while( (a = it.current()) ) {
    Alarm *b = new Alarm( *a );
    b->setParent( this );
    mAlarms.append( b );

    ++it;
  }
  mAlarms.setAutoDelete(true);
  mHasRecurrenceID = i.mHasRecurrenceID;
  mRecurrenceID = i.mRecurrenceID;
  if ( i.mRecurrence )
      mRecurrence = new Recurrence( *(i.mRecurrence), this );
  else
      mRecurrence = 0;
  mHoliday = i.mHoliday ;
  mBirthday = i.mBirthday;
  mAnniversary = i.mAnniversary;
}

Incidence::~Incidence()
{
   
  Incidence *ev;
   QPtrList<Incidence> Relations = relations();
  for (ev=Relations.first();ev;ev=Relations.next()) {
    if (ev->relatedTo() == this) ev->setRelatedTo(0);
  } 
  if (relatedTo()) relatedTo()->removeRelation(this);
  if ( mRecurrence ) 
      delete mRecurrence; 
 
}
QString Incidence::durationText()
{
    return "---";
}
QString Incidence::durationText4Time( int offset )
{
    int min = offset/60;
    int hours = min /60;
    min = min % 60;
    int days = hours /24;
    hours = hours % 24;

    if ( doesFloat() || ( min == 0 && hours == 0 )  ) {
        if ( days == 1 )
            return "1" + i18n(" day");
        else
            return QString::number( days )+   i18n(" days");

    }
    QString message = QString::number ( hours ) +":";
    if ( min < 10 ) message += "0";
    message += QString::number ( min );
    if ( days > 0 ) { 
        if ( days == 1 )
            message =  "1" + i18n(" day") + " "+message;
        else
            message =  QString::number( days )+   i18n(" days") + " "+message;
    }
    return message; 
}
bool Incidence::isHoliday() const
{
    return mHoliday;
}
bool Incidence::isBirthday() const
{

    return mBirthday ;
}
bool Incidence::isAnniversary() const
{
    return mAnniversary ;

}

bool Incidence::hasRecurrenceID() const
{
    return mHasRecurrenceID;
}

void Incidence::setHasRecurrenceID( bool b )
{
    mHasRecurrenceID = b;
}
   
void Incidence::setRecurrenceID(QDateTime d)
{
    mRecurrenceID = d;
    mHasRecurrenceID = true;
    updated(); 
}
QDateTime Incidence::recurrenceID () const
{
    return mRecurrenceID;
}

bool Incidence::cancelled() const
{
    return mCancelled;
}
void Incidence::setCancelled( bool b )
{
    mCancelled = b;
    updated();
}
bool Incidence::hasStartDate() const
{
  return mHasStartDate;
}

void Incidence::setHasStartDate(bool f)
{
  if (mReadOnly) return;
  mHasStartDate = f;
  updated();
}

// A string comparison that considers that null and empty are the same
static bool stringCompare( const QString& s1, const QString& s2 )
{
    if ( s1.isEmpty() && s2.isEmpty() )
        return true;
    return s1 == s2;
}

bool KCal::operator==( const Incidence& i1, const Incidence& i2 )
{
  
    if( i1.alarms().count() != i2.alarms().count() ) {
        return false; // no need to check further
    }
    if ( i1.alarms().count() > 0 ) {
        if ( !( *(i1.alarms().first()) == *(i2.alarms().first())) ) 
            {
            qDebug("alarm not equal ");
            return false;
        }
    }
#if 0
    QPtrListIterator<Alarm> a1( i1.alarms() );
    QPtrListIterator<Alarm> a2( i2.alarms() );
    for( ; a1.current() && a2.current(); ++a1, ++a2 ) {
        if( *a1.current() == *a2.current() ) {
            continue;
        }
        else {
            return false;
        }
    }
#endif
  
    if ( i1.hasRecurrenceID() == i2.hasRecurrenceID() ) {
        if ( i1.hasRecurrenceID() ) {
            if ( i1.recurrenceID() != i2.recurrenceID() )
                return false;
        }

    } else {
        return false;
    }

    if ( ! operator==( (const IncidenceBase&)i1, (const IncidenceBase&)i2 ) )
        return false;
    if ( i1.hasStartDate() == i2.hasStartDate() ) {
        if ( i1.hasStartDate() ) {
            if ( i1.dtStart() != i2.dtStart() )
                return false;
        }
    } else {
        return false;
    }
    if ( i1.mRecurrence != 0 && i2.mRecurrence != 0 ) {
        if (!( *i1.mRecurrence == *i2.mRecurrence) ) {
            //qDebug("recurrence is NOT equal ");
            return false;
        }
    } else {
        // one ( or both ) recurrence is 0
        if ( i1.mRecurrence == 0 ) {
            if ( i2.mRecurrence != 0 && i2.mRecurrence->doesRecur() != Recurrence::rNone )
                return false;
        } else {
            // i1.mRecurrence != 0
            // i2.mRecurrence == 0
            if ( i1.mRecurrence->doesRecur() != Recurrence::rNone )
                return false;
        }
    }
   
    return
        //  i1.created() == i2.created() &&
        stringCompare( i1.description(), i2.description() ) &&
        stringCompare( i1.summary(), i2.summary() ) &&
        i1.categories() == i2.categories() &&
        // no need to compare mRelatedTo
        stringCompare( i1.relatedToUid(), i2.relatedToUid() ) &&
        //      i1.relations() == i2.relations() &&
        i1.exDates() == i2.exDates() &&
        i1.attachments() == i2.attachments() &&
        i1.resources() == i2.resources() &&
        i1.secrecy() == i2.secrecy() &&
        i1.priority() == i2.priority() &&
        i1.cancelled() == i2.cancelled() &&
        stringCompare( i1.location(), i2.location() );
}

Incidence* Incidence::recreateCloneException( QDate d  )
{
    Incidence* newInc = clone();
    newInc->recreate();
    if ( doesRecur() ) {
        addExDate( d );
        newInc->recurrence()->unsetRecurs();
        if ( typeID() == eventID ) {
            int len = dtStart().secsTo( ((Event*)this)->dtEnd());
            QTime tim = dtStart().time();
            newInc->setDtStart( QDateTime(d, tim) );
            ((Event*)newInc)->setDtEnd( newInc->dtStart().addSecs( len ) );
        } else {
            int len = dtStart().secsTo( ((Todo*)this)->dtDue());
            QTime tim = ((Todo*)this)->dtDue().time();
            ((Todo*)newInc)->setDtDue( QDateTime(d, tim) );
            ((Todo*)newInc)->setDtStart( ((Todo*)newInc)->dtDue().addSecs( -len ) );
            ((Todo*)this)->setRecurDates();
        }
        newInc->setExDates( DateList () );
    }
    return newInc;
}

void Incidence::recreate()
{
  setCreated(QDateTime::currentDateTime());

  setUid(CalFormat::createUniqueId());

  setRevision(0);
  setIDStr( ":" );
  setLastModified(QDateTime::currentDateTime());
}
void Incidence::cloneRelations( Incidence * newInc )
{
    // newInc is already a clone of this incidence
    Incidence * inc;
    Incidence * cloneInc;
    QPtrList<Incidence> Relations = relations();
    for (inc=Relations.first();inc;inc=Relations.next()) {
        cloneInc = inc->clone();
        cloneInc->recreate();
        cloneInc->setRelatedTo( newInc );
        inc->cloneRelations( cloneInc );
    } 
}
void Incidence::setReadOnly( bool readOnly )
{
  IncidenceBase::setReadOnly( readOnly );
  if ( mRecurrence )
      mRecurrence->setRecurReadOnly( readOnly);
}
void Incidence::setLastModifiedSubInvalid()
{
    mLastModifiedSub = QDateTime();
    if ( mRelatedTo )
        mRelatedTo->setLastModifiedSubInvalid();
}
QString Incidence::lastModifiedSubSortKey() const
{
    if ( mLastModifiedSubSortKey.isEmpty() )
        return lastModifiedSortKey();
    return mLastModifiedSubSortKey;
}
QDateTime Incidence::lastModifiedSub()
{
    if ( !mRelations.count() )
        return lastModified();
    if ( mLastModifiedSub.isValid() )
        return mLastModifiedSub;
    mLastModifiedSub = lastModified();
    Incidence * inc;
    QPtrList<Incidence> Relations = relations();
    for (inc=Relations.first();inc;inc=Relations.next()) {
        if ( inc->lastModifiedSub() > mLastModifiedSub )
            mLastModifiedSub = inc->lastModifiedSub();
    }
    mLastModifiedSubSortKey.sprintf("%04d%02d%02d%02d%02d%02d",
                                    mLastModifiedSub.date().year(),
                                    mLastModifiedSub.date().month(),
                                    mLastModifiedSub.date().day(),
                                    mLastModifiedSub.time().hour(),
                                    mLastModifiedSub.time().minute(),
                                    mLastModifiedSub.time().second() );
    return mLastModifiedSub;
}
void Incidence::setCreated(QDateTime created)
{
  if (mReadOnly) return;
  mCreated = getEvenTime(created);
}

QDateTime Incidence::created() const
{
  return mCreated;
}

void Incidence::setRevision(int rev)
{
  if (mReadOnly) return;
  mRevision = rev;

  updated();
}

int Incidence::revision() const
{
  return mRevision;
}

void Incidence::setDtStart(const QDateTime &dtStart)
{

  QDateTime  dt = getEvenTime(dtStart);

  if ( mRecurrence )
      mRecurrence->setRecurStart( dt);
  IncidenceBase::setDtStart( dt );
}

void Incidence::setDescription(const QString &description)
{
  if (mReadOnly) return;
  mDescription = description;
  updated();
}

QString Incidence::description() const
{
  return mDescription;
}


void Incidence::setSummary(const QString &summary)
{
  if (mReadOnly) return;
  mSummary = summary;
  updated();
}

QString Incidence::summary() const
{
  return mSummary;
}
void Incidence::checkCategories()
{
    mHoliday =  mCategories.contains("Holiday") || mCategories.contains(i18n("Holiday"));
    mBirthday =  mCategories.contains("Birthday") || mCategories.contains(i18n("Birthday"));
    mAnniversary =  mCategories.contains("Anniversary") || mCategories.contains(i18n("Anniversary"));
}

void Incidence::addCategories(const QStringList &categories, bool addToRelations ) //addToRelations = false
{
    if (mReadOnly) return;
    int i;
    for( i = 0; i < categories.count(); ++i ) {
        if (  !mCategories.contains (categories[i]))
            mCategories.append( categories[i] );
    }
    checkCategories();
    updated();
    if ( addToRelations ) {
        Incidence * inc;
        QPtrList<Incidence> Relations = relations();
        for (inc=Relations.first();inc;inc=Relations.next()) {
            inc->addCategories( categories, true );
        } 
    }
}

void Incidence::setCategories(const QStringList &categories, bool setForRelations ) //setForRelations = false
{
  if (mReadOnly) return;
  mCategories = categories;
  checkCategories();
  updated();
  if ( setForRelations ) {
      Incidence * inc;
      QPtrList<Incidence> Relations = relations();
      for (inc=Relations.first();inc;inc=Relations.next()) {
          inc->setCategories( categories, true );
      } 
  }
}

// TODO: remove setCategories(QString) function
void Incidence::setCategories(const QString &catStr)
{
  if (mReadOnly) return;
  mCategories.clear();

  if (catStr.isEmpty()) return;

  mCategories = QStringList::split(",",catStr);

  QStringList::Iterator it;
  for(it = mCategories.begin();it != mCategories.end(); ++it) {
    *it = (*it).stripWhiteSpace();
  }
  checkCategories();
  updated();
}
// using this makes filtering 3 times faster
QStringList* Incidence::categoriesP()
{
  return &mCategories;
}

QStringList Incidence::categories() const
{
  return mCategories;
}

QString Incidence::categoriesStr()
{
  return mCategories.join(",");
}
QString Incidence::categoriesStrWithSpace()
{
  return mCategories.join(", ");
}

void Incidence::setRelatedToUid(const QString &relatedToUid)
{
  if (mReadOnly) return;
  mRelatedToUid = relatedToUid;
}
void Incidence::clearRelations()
{
    mRelatedTo = 0;
    mRelations.clear();
}
QString Incidence::relatedToUid() const
{
  return mRelatedToUid;
}
void Incidence::resetRelatedTo()
{
    QString store = mRelatedToUid;
    setRelatedTo( 0 );
    mRelatedToUid = store;
}

void Incidence::setRelatedTo(Incidence *relatedTo)
{
    //qDebug("Incidence::setRelatedTo %d ", relatedTo);
    //qDebug("setRelatedTo(Incidence *relatedTo) %s %s", summary().latin1(), relatedTo->summary().latin1() );
  if (mReadOnly || mRelatedTo == relatedTo) return;
  if(mRelatedTo) {
      // updated();
    mRelatedTo->removeRelation(this);
  }
  mRelatedTo = relatedTo;
  if (mRelatedTo)  {
      mRelatedTo->addRelation(this);
      mRelatedToUid = mRelatedTo->uid();
  } else {
      mRelatedToUid = "";
  }
}

Incidence *Incidence::relatedTo() const
{
  return mRelatedTo;
}

QPtrList<Incidence> Incidence::relations() const
{
  return mRelations;
}

void Incidence::addRelationsToList(QPtrList<Incidence> *rel)
{
    Incidence* inc;
    QPtrList<Incidence> Relations = relations();
    for (inc=Relations.first();inc;inc=Relations.next()) {
        inc->addRelationsToList( rel );
    } 
    if ( rel->findRef( this ) == -1 ) 
        rel->append( this );
}

void Incidence::addRelation(Incidence *event)
{
    setLastModifiedSubInvalid();
  if( mRelations.findRef( event ) == -1 ) {
    mRelations.append(event);
    //updated();
  }
}

void Incidence::removeRelation(Incidence *event)
{
    setLastModifiedSubInvalid();
  mRelations.removeRef(event);
//  if (event->getRelatedTo() == this) event->setRelatedTo(0);
}

bool Incidence::recursOn(const QDate &qd) const
{
  if (mRecurrence && mRecurrence->recursOnPure(qd) && !isException(qd)) return true;
  else return false;
}

void Incidence::setExDates(const DateList &exDates)
{
  if (mReadOnly) return;
  mExDates = exDates;
  recurrence()->setRecurExDatesCount(mExDates.count());

  updated();
}

void Incidence::addExDate(const QDate &date)
{
  if (mReadOnly) return;
  mExDates.append(date);

  recurrence()->setRecurExDatesCount(mExDates.count());

  updated();
}

DateList Incidence::exDates() const
{
  return mExDates;
}

bool Incidence::isException(const QDate &date) const
{
  DateList::ConstIterator it;
  for( it = mExDates.begin(); it != mExDates.end(); ++it ) {
    if ( (*it) == date ) {
      return true;
    }
  }

  return false;
}

void Incidence::addAttachment(Attachment *attachment)
{
  if (mReadOnly || !attachment) return;
  mAttachments.append(attachment);
  updated();
}

void Incidence::deleteAttachment(Attachment *attachment)
{
  mAttachments.removeRef(attachment);
}

void Incidence::deleteAttachments(const QString& mime)
{
  Attachment *at = mAttachments.first();
  while (at) {
    if (at->mimeType() == mime)
      mAttachments.remove();
    else
      at = mAttachments.next();
  }
}

QPtrList<Attachment> Incidence::attachments() const
{
  return mAttachments;
}

QPtrList<Attachment> Incidence::attachments(const QString& mime) const
{
  QPtrList<Attachment> attachments;
  QPtrListIterator<Attachment> it( mAttachments );
  Attachment *at;
  while ( (at = it.current()) ) {
    if (at->mimeType() == mime)
      attachments.append(at);
    ++it;
  }

  return attachments;
}

void Incidence::setResources(const QStringList &resources)
{
  if (mReadOnly) return;
  mResources = resources;
  updated();
}

QStringList Incidence::resources() const
{
  return mResources;
}


void Incidence::setPriority(int priority)
{
  if (mReadOnly) return;
  mPriority = priority;
  updated();
}

int Incidence::priority() const
{
  return mPriority;
}

void Incidence::setSecrecy(int sec)
{
  if (mReadOnly) return;
  mSecrecy = sec;
  updated();
}

int Incidence::secrecy() const
{
  return mSecrecy;
}

QString Incidence::secrecyStr() const
{
  return secrecyName(mSecrecy);
}

QString Incidence::secrecyName(int secrecy)
{
  switch (secrecy) {
    case SecrecyPublic:
      return i18n("Public");
      break;
    case SecrecyPrivate:
      return i18n("Private");
      break;
    case SecrecyConfidential:
      return i18n("Confidential");
      break;
    default:
      return i18n("Undefined");
      break;
  }
}

QStringList Incidence::secrecyList()
{
  QStringList list;
  list << secrecyName(SecrecyPublic);
  list << secrecyName(SecrecyPrivate);
  list << secrecyName(SecrecyConfidential);

  return list;
}


QPtrList<Alarm> Incidence::alarms() const
{
  return mAlarms;
}

Alarm* Incidence::newAlarm()
{
  Alarm* alarm = new Alarm(this);
  mAlarms.append(alarm);
//  updated();
  return alarm;
}

void Incidence::addAlarm(Alarm *alarm)
{
  mAlarms.append(alarm);
  updated();
}

void Incidence::removeAlarm(Alarm *alarm)
{
  mAlarms.removeRef(alarm);
  updated();
}

void Incidence::clearAlarms()
{
  mAlarms.clear();
  updated();
}

bool Incidence::isAlarmEnabled() const
{
  Alarm* alarm;
  for (QPtrListIterator<Alarm> it(mAlarms); (alarm = it.current()) != 0; ++it) {
    if (alarm->enabled())
      return true;
  }
  return false;
}
#include <stdlib.h>
Recurrence *Incidence::recurrence()
{
    if ( ! mRecurrence ) {
        mRecurrence = new Recurrence(this);
        mRecurrence->setRecurStart( dtStart() );
        mRecurrence->setRecurReadOnly( isReadOnly());
        //qDebug("creating new recurence ");
        //abort();
    }
  return mRecurrence;
}
void Incidence::setRecurrence( Recurrence * r) 
{
    if ( mRecurrence )
        delete mRecurrence;
    mRecurrence = r;
}

void Incidence::setLocation(const QString &location)
{
  if (mReadOnly) return;
  mLocation = location;
  updated();
}

QString Incidence::location() const
{
  return mLocation;
}
QString Incidence::recurrenceText() const
{
    if ( mRecurrence ) return mRecurrence->recurrenceText();
    return i18n("No");
}

ushort Incidence::doesRecur() const
{
  if ( mRecurrence ) return mRecurrence->doesRecur();
  else return Recurrence::rNone;
}

QDateTime Incidence::getNextOccurence( const QDateTime& dt, bool* ok ) const
{
    QDateTime incidenceStart = dt;
    *ok = false;
    if ( doesRecur() ) {
        bool last;
         mRecurrence->getPreviousDateTime( incidenceStart , &last );
         int count = 0;
         if ( !last ) {
             while ( !last ) {
                 ++count;
                 incidenceStart = mRecurrence->getNextDateTime( incidenceStart, &last );
                 if ( recursOn( incidenceStart.date() ) ) {
                     last = true; // exit while llop
                 } else {
                     if ( last ) { // no alarm on last recurrence
                         return QDateTime ();
                     }
                     int year = incidenceStart.date().year();
                     // workaround for bug in recurrence
                     if ( count == 100 || year < 1000 || year > 5000 ) {
                         return QDateTime ();
                     }
                     incidenceStart = incidenceStart.addSecs( 1 );
                 }
             }
         } else {
             return QDateTime ();
         }
    } else {
        if ( hasStartDate () ) {
            incidenceStart = dtStart(); 
        }
        if ( typeID() == todoID ) {
            if ( ((Todo*)this)->hasDueDate() )
                incidenceStart = ((Todo*)this)->dtDue();
        }
    }
    if ( incidenceStart > dt )
        *ok = true;
    return incidenceStart;
}
QDateTime Incidence::dtStart() const
{
    if ( doesRecur() ) {
        if ( typeID() == todoID ) {
            ((Todo*)this)->checkSetCompletedFalse();
        }
    }
    return mDtStart;
}
