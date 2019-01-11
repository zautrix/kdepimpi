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
#ifndef TODO_H
#define TODO_H
//
// Todo component, representing a VTODO object
//

#include "incidence.h"

#include <qtimer.h>

namespace KCal {

/**
  This class provides a Todo in the sense of RFC2445.
*/
    class Todo : public QObject,public Incidence
{
 Q_OBJECT
  public:
    Todo();
    Todo(const Todo &);
    ~Todo();
    typedef ListBase<Todo> List;
    QCString type() const { return "Todo"; }
    IncTypeID typeID() const { return todoID; }

    /** Return an exact copy of this todo. */
    Incidence *clone();
    QDateTime getNextAlarmDateTime( bool * ok, int * offset, QDateTime start_dt ) const;

    /** for setting the todo's due date/time with a QDateTime. */
    void setDtDue(const QDateTime &dtDue);
    /** returns an event's Due date/time as a QDateTime. */
    QDateTime dtDue() const;
    /** returns an event's due time as a string formatted according to the
     users locale settings */
    QString dtDueTimeStr() const;
    /** returns an event's due date as a string formatted according to the
     users locale settings */
    QString dtDueDateStr(bool shortfmt=true) const;
    /** returns an event's due date and time as a string formatted according
     to the users locale settings */
    QString dtDueStr(bool shortfmt=true) const;

    /** returns TRUE or FALSE depending on whether the todo has a due date */
    bool hasDueDate() const;
    /** sets the event's hasDueDate value. */
    void setHasDueDate(bool f);

    /*
      Looks for a subtodo (including itself ) which is not complete and is
      - overdue, or
      - due today.
      It returns 0 for nothing found,
      1 for found a todo which is due today and no overdue found
      2 for found a overdue todo
    */
    int hasDueSubTodo( bool checkSubtodos = true );
    /* same as above, but a specific date can be specified*/
    int hasDueSubTodoForDate( const  QDate & date, bool checkSubtodos );

 
    /** sets the event's status to the string specified.  The string
     * must be a recognized value for the status field, i.e. a string
     * equivalent of the possible status enumerations previously described. */
//    void setStatus(const QString &statStr);
    /** sets the event's status to the value specified.  See the enumeration
     * above for possible values. */
//    void setStatus(int);
    /** return the event's status. */
//    int status() const;
    /** return the event's status in string format. */
//    QString statusStr() const;

    /** return, if this todo is completed */
    bool isCompleted() const;
    /** set completed state of this todo */
    void setCompleted(bool);
    
    /**
      Return how many percent of the task are completed. Returns a value
      between 0 and 100.
    */
    int percentComplete() const;
    /**
      Set how many percent of the task are completed. Valid values are in the
      range from 0 to 100.
    */
    void setPercentComplete(int);

    /** return date and time when todo was completed */
    QDateTime completed() const;
    QString completedStr(bool shortF = true) const;
    /** set date and time of completion */
    void setCompleted(const QDateTime &completed);

    /** Return true, if todo has a date associated with completion */
    bool hasCompletedDate() const;
    bool contains ( Todo*);
    void checkSetCompletedFalse();
    bool setRecurDates();
    bool isRunning() {return mRunning;}
    bool hasRunningSub();
    void setRunning( bool );
    void setRunningFalse( QString );
    void stopRunning();
    int runTime();
    QDateTime runStart () const { return mRunStart;}
    void saveRunningInfo( QString comment, QDateTime start, QDateTime end );
    public slots:
    void saveRunningInfoToFile( QString st );
    void saveRunningInfoToFile( );
    void saveParents();
    QString durationText();
  private slots:
    void timerSlotSaveRunningInfoToFile( );
  private:
    QString mLastSavedFileName;
    void restartSaveTimer( int secs );
    int mCurrentTimerDelay;
    bool mRunning;
    QTimer * mRunSaveTimer;
    QDateTime mRunStart;
    QDateTime mRunLastSave;
    QDateTime mRunEnd;
    bool accept(Visitor &v) { return v.visit(this); }

    QDateTime mDtDue;                     // due date of todo

    bool mHasDueDate;                    // if todo has associated due date

//    int  mStatus;                         // confirmed/delegated/tentative/etc

    QDateTime mCompleted;
    bool mHasCompletedDate;

    int mPercentComplete;
};

    bool operator==( const Todo&, const Todo& );
}

#endif
