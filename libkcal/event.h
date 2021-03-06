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

#ifndef EVENT_H
#define EVENT_H
//
// Event component, representing a VEVENT object
//

#include "incidence.h"
namespace KCal {

/**
  This class provides an Event in the sense of RFC2445.
*/
class Event : public Incidence
{
  public:
    enum Transparency { Opaque, Transparent };
    typedef ListBase<Event> List;
    Event();
    Event(const Event &);
    ~Event();
    bool matchTime(QDateTime*startDT, QDateTime* endDT);

    QCString type() const { return "Event"; }
    IncTypeID typeID() const { return eventID; }

    Incidence *clone();
    QDateTime getNextAlarmDateTime( bool * ok, int * offset ,QDateTime start_dt  ) const;

    /** for setting an event's ending date/time with a QDateTime. */
    void setDtEnd(const QDateTime &dtEnd);
    /** Return the event's ending date/time as a QDateTime. */
    virtual QDateTime dtEnd() const;
    /** returns an event's end time as a string formatted according to the
     users locale settings */
    QString dtEndTimeStr() const;
    /** returns an event's end date as a string formatted according to the
     users locale settings */
    QString dtEndDateStr(bool shortfmt=true) const;
    /** returns an event's end date and time as a string formatted according
     to the users locale settings */
    QString dtEndStr(bool shortfmt=true) const;
    void setHasEndDate(bool);
    /** Return whether the event has an end date/time. */
    bool hasEndDate() const;

    /** Return true if the event spans multiple days, otherwise return false. */
    bool isMultiDay() const;

    /** set the event's time transparency level. */
    void setTransparency(Transparency transparency);
    /** get the event's time transparency level. */
    Transparency transparency() const;

    void setDuration(int seconds);

    bool contains ( Event*);

    bool isOverlapping (  Event*,  QDateTime*, QDateTime* );
    QString durationText();

  private:
    bool accept(Visitor &v) { return v.visit(this); }

    QDateTime mDtEnd;
    bool mHasEndDate;
    Transparency mTransparency;
};

bool operator==( const Event&, const Event& );


}


#endif
