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
#ifndef KCAL_INCIDENCEBASE_H
#define KCAL_INCIDENCEBASE_H
//
// Incidence - base class of calendaring components
//

#include <qdatetime.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qptrlist.h>

#include "customproperties.h"
#include "attendee.h"

namespace KCal {

typedef QValueList<QDate> DateList;
 enum IncTypeID { eventID,todoID,journalID,freebusyID };

/**
  This class provides the base class common to all calendar components.
*/
class IncidenceBase : public CustomProperties
{
  public:
    class Observer {
      public:
        virtual void incidenceUpdated( IncidenceBase * ) = 0;
    };

    IncidenceBase();
    IncidenceBase(const IncidenceBase &);
    virtual ~IncidenceBase();

    virtual QCString type() const = 0;
    virtual  IncTypeID typeID() const = 0;

    /** Set the unique id for the event */
    void setUid(const QString &);
    /** Return the unique id for the event */
    QString uid() const;

    /** Sets the time the incidence was last modified. */
    void setLastModified(const QDateTime &lm);
    /** Return the time the incidence was last modified. */
    QDateTime lastModified() const;
    QString lastModifiedSortKey() const;

    /** sets the organizer for the event */
    void setOrganizer(const QString &o);
    QString organizer() const;

    /** Set readonly status. */
    virtual void setReadOnly( bool );
    /** Return if the object is read-only. */
    bool isReadOnly() const { return mReadOnly; }

    /** for setting the event's starting date/time with a QDateTime. */
    virtual void setDtStart(const QDateTime &dtStart);
    /** returns an event's starting date/time as a QDateTime. */
    virtual QDateTime dtStart() const;
    /** returns an event's starting time as a string formatted according to the
     users locale settings */
    QString dtStartTimeStr() const;
    /** returns an event's starting date as a string formatted according to the
     users locale settings */
    QString dtStartDateStr(bool shortfmt=true) const;
    /** returns an event's starting date and time as a string formatted according
     to the users locale settings */
    QString dtStartStr(bool shortfmt=true) const;

    virtual void setDuration(int seconds);
    int duration() const;
    void setHasDuration(bool);
    bool hasDuration() const;

    /** Return true or false depending on whether the incidence "floats,"
     * i.e. has a date but no time attached to it. */
    bool doesFloat() const;
    /** Set whether the incidence floats, i.e. has a date but no time attached to it. */
    void setFloats(bool f);

    /**
      Add Attendee to this incidence. IncidenceBase takes ownership of the
      Attendee object.
    */
    bool addAttendee(Attendee *a, bool doupdate=true );
//    void removeAttendee(Attendee *a);
//    void removeAttendee(const char *n);
    /** Remove all Attendees. */
    void clearAttendees();
    /** Return list of attendees. */
    QPtrList<Attendee> attendees() const { return mAttendees; };
    /** Return number of attendees. */
    int attendeeCount() const { return mAttendees.count(); };
    /** Return the Attendee with this email */
    Attendee* attendeeByMail(const QString &);
    /** Return first Attendee with one of this emails */
    Attendee* attendeeByMails(const QStringList &, const QString& email = QString::null);

    /** pilot syncronization states */
    enum { SYNCNONE = 0, SYNCMOD = 1, SYNCDEL = 3 };
    /** Set synchronisation satus. */
    void setSyncStatus(int stat);
    /** Return synchronisation status. */
    int syncStatus() const;

    /** Set Pilot Id. */
    void setPilotId(int id);
    /** Return Pilot Id. */
    int pilotId() const;

    void setTempSyncStat(int id);
    int tempSyncStat() const;
    void setIDStr( const QString & );
    QString IDStr() const;
    void setID( const QString &, const QString & );
    QString  getID( const QString & );
    void setCsum( const QString &, const QString & );
    QString getCsum( const QString & );
    void removeID(const QString &);

    void registerObserver( Observer * );
    void unRegisterObserver( Observer * );
    void updated();
    void setCalID( int id );
    void setCalID_block( int id );
    int calID() const;
    void setCalEnabled( bool );
    bool calEnabled() const; 
    void setAlarmEnabled( bool );
    bool alarmEnabled() const;
    bool isTagged() const;
    void setTagged( bool );
    virtual void setLastModifiedSubInvalid();
  protected:
    bool blockLastModified;
    bool mIsTagged;
    QDateTime mDtStart;
    bool mReadOnly;
    QDateTime getEvenTime( QDateTime );

  private:
    // base components
    QString mOrganizer;
    QString mLastModifiedKey;
    QString mUid;
    int mCalID;
    bool mCalEnabled;
    bool mAlarmEnabled;
    QDateTime mLastModified;
    QPtrList<Attendee> mAttendees;

    bool mFloats;

    int mDuration;
    bool mHasDuration;
    QString  mExternalId;
    int mTempSyncStat;

    // PILOT SYNCHRONIZATION STUFF
    int mPilotId;                         // unique id for pilot sync
    int mSyncStatus;                      // status (for sync)

    QPtrList<Observer> mObservers;
};

bool operator==( const IncidenceBase&, const IncidenceBase& );
}

#endif
