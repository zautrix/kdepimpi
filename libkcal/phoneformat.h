/*
    This file is part of libkcal.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef PHONEFORMAT_H
#define PHONEFORMAT_H

#include <qstring.h>

#include "scheduler.h"

#include "vcalformat.h"
#include "calformat.h"

namespace KCal {

/**
  This class implements the calendar format used by Phone.
*/
    class Event;
    class Todo;
class PhoneFormat : public QObject {
  public:
    /** Create new iCalendar format. */
    PhoneFormat(QString profileName, QString device,QString connection, QString model);
    virtual ~PhoneFormat();

    bool load( Calendar * ,Calendar * );
    bool save( Calendar * );
    bool fromString( Calendar *, const QString & );  
    QString toString( Calendar * );
    static ulong getCsum( const QStringList & );
    static ulong getCsumTodo( Todo* to );
    static ulong getCsumEvent( Event* ev );
    static bool writeToPhone( Calendar * );
  private:
    void copyEvent( Event* to,  Event* from );
    void copyTodo( Todo* to,  Todo* from );
    //int initDevice(GSM_StateMachine *s);
    QString mProfileName;
    void afterSave( Incidence* ,const QString&,const QString&);
};

}

#endif
