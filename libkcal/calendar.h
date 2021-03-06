/*
    This file is part of libkcal.
    Copyright (c) 1998 Preston Brown
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

#ifndef CALENDAR_H
#define CALENDAR_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qptrlist.h>
#include <qdict.h>

#include "customproperties.h"
#include "event.h"
#include "todo.h"
#include "journal.h"
#include "calfilter.h"

//#define _TIME_ZONE "-0500" /* hardcoded, overridden in config file. */

class KConfig;

namespace KCal {


/**
  This is the main "calendar" object class for KOrganizer.  It holds
  information like all appointments/events, user information, etc. etc.
  one calendar is associated with each CalendarView (@see calendarview.h).
  This is an abstract base class defining the interface to a calendar. It is
  implemented by subclasses like @see CalendarLocal, which use different
  methods to store and access the data.

  Ownership of events etc. is handled by the following policy: As soon as an
  event (or any other subclass of IncidenceBase) object is added to the
  Calendar by addEvent() it is owned by the Calendar object. The Calendar takes
  care of deleting it. All Events returned by the query functions are returned
  as pointers, that means all changes to the returned events are immediately
  visible in the Calendar. You shouldn't delete any Event object you get from
  Calendar.
*/
class Calendar : public QObject, public CustomProperties,
                 public IncidenceBase::Observer
{
    Q_OBJECT
public:
    Calendar();
    Calendar(const QString &timeZoneId);
    virtual ~Calendar();
    Incidence * undoIncidence() { return mUndoIncidence; };
    bool undoDeleteIncidence();
    void deleteIncidence(Incidence *in);
    void resetTempSyncStat();
    void resetPilotStat(int id);
    /**
      Clears out the current calendar, freeing all used memory etc.
    */
    virtual void close() = 0;
    virtual void addCalendar( Calendar*  ) = 0;
    virtual bool addCalendarFile( QString name, int id ) = 0;
    virtual bool mergeCalendarFile( QString name ) = 0;
    virtual Incidence* incidenceForUid( const QString& uid, bool doNotCheckDuplicates , bool enabledOnly = false ,int * isDup = 0 ) = 0;
    virtual Todo* todoForUid( const QString& uid, bool doNotCheckDuplicates = true, bool enabledOnly = false  ,int * isDup = 0) = 0;
    virtual void setSyncEventsReadOnly() = 0;
    virtual void setSyncEventsEnabled() = 0;
    virtual void stopAllTodos() = 0;
    virtual void clearUndo( Incidence * newUndo );
  
    /**
      Sync changes in memory to persistant storage.
    */
    virtual void save() = 0;
    virtual QPtrList<Event> getExternLastSyncEvents() = 0;
    virtual void removeSyncInfo( QString syncProfile) = 0; 
    virtual bool isSaving() { return false; }

    /**
      Return the owner of the calendar's full name.
    */
    const QString &getOwner() const;
    /**
      Set the owner of the calendar. Should be owner's full name.
    */
    void setOwner( const QString &os );
    /**
      Return the email address of the calendar owner.
    */
    const QString &getEmail();
    /**
      Set the email address of the calendar owner.
    */
    void setEmail( const QString & );
  
    /**
      Set time zone from a timezone string (e.g. -2:00)
    */
    void setTimeZone( const QString &tz );
    /**
      Set time zone from a minutes value (e.g. -60)
    */
    void setTimeZone( int tz );
    /**
      Return time zone as offest in minutes.
    */
    int getTimeZone() const;
    /**
      Compute an ISO 8601 format string from the time zone.
    */
    QString getTimeZoneStr() const;
    /**
      Set time zone id (see /usr/share/zoneinfo/zone.tab for list of legal
      values).
    */
    void setTimeZoneId( const QString & );
    /**
      Return time zone id.
    */
    QString timeZoneId() const;
    /**
      Use local time, not UTC or a time zone.
    */
    void setLocalTime();
    /**
      Return whether local time is being used.
    */
    bool isLocalTime() const;
  
    /**
      Add an incidence to calendar.
      
      @return true on success, false on error.
    */
    virtual bool addIncidence( Incidence * ); 

    // Adds an incidence and all relatedto incidences to the cal 
    void addIncidenceBranch( Incidence * );  
    /**
      Return filtered list of all incidences of this calendar.
    */
    virtual QPtrList<Incidence> incidences();

    /**
      Return unfiltered list of all incidences of this calendar.
    */
    virtual QPtrList<Incidence> rawIncidences();

    /**
      Adds a Event to this calendar object.
      @param anEvent a pointer to the event to add
      
      @return true on success, false on error.
    */
    virtual bool addEventNoDup( Event *event ) = 0;
    virtual bool addAnniversaryNoDup( Event *event ) = 0;
    virtual bool addEvent( Event *anEvent ) = 0;
    /**
      Delete event from calendar.
    */
    virtual void deleteEvent( Event * ) = 0;
    /**
      Retrieves an event on the basis of the unique string ID.
    */
    virtual Event *event( const QString &UniqueStr ) = 0;
    virtual Event *event( QString, QString ) = 0;
    /**
      Builds and then returns a list of all events that match for the
      date specified. useful for dayView, etc. etc.
      The calendar filter is applied.
    */
    QPtrList<Event> events( const QDate &date, bool sorted = false);
    /**
      Get events, which occur on the given date.
      The calendar filter is applied.
    */
    QPtrList<Event> events( const QDateTime &qdt );
    /**
      Get events in a range of dates. If inclusive is set to true, only events
      are returned, which are completely included in the range.
      The calendar filter is applied.
    */
    QPtrList<Event> events( const QDate &start, const QDate &end,
                            bool inclusive = false);
    /**
      Return filtered list of all events in calendar.
    */
    virtual QPtrList<Event> events();
    /**
      Return unfiltered list of all events in calendar.
    */
    virtual QPtrList<Event> rawEvents() = 0;

    /**
      Add a todo to the todolist.
      
      @return true on success, false on error.
    */
    virtual bool addTodo( Todo *todo ) = 0;
    virtual bool addTodoNoDup( Todo *todo ) = 0;
    /**
      Remove a todo from the todolist.
    */
    virtual void deleteTodo( Todo * ) = 0;
    virtual void deleteJournal( Journal * ) = 0;
    /**
      Return filterd list of todos.
    */
    virtual QPtrList<Todo> todos();
    /**
      Searches todolist for an event with this unique string identifier,
      returns a pointer or null.
    */
    virtual Todo *todo( const QString &uid ) = 0;
    virtual Todo *todo( QString, QString ) = 0;
    /**
      Returns list of todos due on the specified date.
    */
    virtual QPtrList<Todo> todos( const QDate &date ) = 0;
    /**
      Return unfiltered list of todos.
    */
    virtual QPtrList<Todo> rawTodos() = 0;

    /**
      Add a Journal entry to calendar.
      
      @return true on success, false on error.
    */
    virtual bool addJournal( Journal * ) = 0;
    /**
      Return Journal for given date.
    */
    virtual Journal *journal( const QDate & ) = 0;
    virtual  QPtrList<Journal> journals4Date( const QDate & ) = 0;
    /**
      Return Journal with given UID.
    */
    virtual Journal *journal( const QString &UID ) = 0;
    /**
      Return list of all Journal entries.
    */
    virtual QPtrList<Journal> journals() = 0;

    /**
      Searches all incidence types for an incidence with this unique
      string identifier, returns a pointer or null.
    */
    Incidence* incidence( const QString&UID );

    /**
      Setup relations for an incidence.
    */
    virtual void setupRelations( Incidence * );
    /**
      Remove all relations to an incidence
    */
    virtual void removeRelations( Incidence * );
    
    /**
      Set calendar filter, which filters events for the events() functions.
      The Filter object is owned by the caller.
    */
    void setFilter( CalFilter * );
    /**
      Return calendar filter.
    */
    CalFilter *filter();
    virtual QDateTime  nextAlarm(  int daysTo ) = 0;
    virtual QString  nextSummary( ) const  = 0;
    virtual void reInitAlarmSettings() = 0;
    virtual QDateTime nextAlarmEventDateTime()  const = 0;
    virtual void  checkAlarmForIncidence( Incidence *, bool )  = 0;
    /**
      Return all alarms, which ocur in the given time interval.
    */
    virtual Alarm::List alarms( const QDateTime &from,
                                const QDateTime &to ) = 0;

    class Observer {
    public:
        virtual void calendarModified( bool, Calendar * ) = 0;
    };
  
    void registerObserver( Observer * );

    void setModified( bool );

    /**
      Set product id returned by loadedProductId(). This function is only
      useful for the calendar loading code.
    */
    void setLoadedProductId( const QString & );
    /**
      Return product id taken from file that has been loaded. Returns
      QString::null, if no calendar has been loaded.
    */
    QString loadedProductId();
    int defaultCalendar();
    void setDontDeleteIncidencesOnClose ();
    Incidence *getCurrentNextAlarmIncidence();
    public slots:
    void setDefaultCalendar( int );
    virtual void setCalendarEnabled( int id, bool enable ) = 0;
    virtual void setAllCalendarEnabled( bool enable ) = 0;
    virtual void setAlarmEnabled( int id, bool enable ) = 0;
    virtual void setReadOnly( int id, bool enable ) = 0;
    virtual void setDefaultCalendarEnabledOnly() = 0;
    virtual void setCalendarRemove( int id ) = 0;
    virtual void getIncidenceCount( int calId, int& events, int & todos, int & journals) = 0;
    
  signals:
    void calendarChanged();
    void calendarSaved();
    void calendarLoaded();
    void addAlarm(const QDateTime &qdt, const QString &noti  );
    void removeAlarm(const QDateTime &qdt, const QString &noti  );

  protected:
    /**
      Get unfiltered events, which occur on the given date.
    */
    virtual QPtrList<Event> rawEventsForDate( const QDateTime &qdt ) = 0;
    /**
      Get unfiltered events, which occur on the given date.
    */
    virtual QPtrList<Event> rawEventsForDate( const QDate &date,
                                              bool sorted = false ) = 0;  
    /**
      Get events in a range of dates. If inclusive is set to true, only events
      are returned, which are completely included in the range.
    */
    virtual QPtrList<Event> rawEvents( const QDate &start, const QDate &end,
                                       bool inclusive = false ) = 0; 

    Incidence *mNextAlarmIncidence;
    Incidence *mUndoIncidence;
    int mDefaultCalendar;
    bool mDeleteIncidencesOnClose;

private:
    void init();
  
    QString mOwner;        // who the calendar belongs to
    QString mOwnerEmail;   // email address of the owner
    int mTimeZone;         // timezone OFFSET from GMT (MINUTES)
    bool mLocalTime;       // use local time, not UTC or a time zone


    CalFilter *mFilter;
    CalFilter *mDefaultFilter;

    
    QString mTimeZoneId;

    Observer *mObserver;
    bool mNewObserver;
    
    bool mModified;

    QString mLoadedProductId;

    // This list is used to put together related todos
    QDict<Incidence> mOrphans;
    QDict<Incidence> mOrphanUids;
};
  
}

#endif
