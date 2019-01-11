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
#ifndef SHARPFORMAT_H
#define SHARPAFORMAT_H

#include <qstring.h>

#include "scheduler.h"

#include "calformat.h"

namespace KCal {

/**
  This class implements the calendar format used by Sharp.
*/
class SharpFormat : public QObject {
  public:
    /** Create new iCalendar format. */
    SharpFormat();
    virtual ~SharpFormat();

    bool load( Calendar * ,Calendar *);
    bool save( Calendar * );
    bool fromString2Cal( Calendar *, Calendar *, const QString & , const QString & );  
    bool fromString( Calendar *, const QString & );  
    QString toString( Calendar * );
    static ulong getCsum( const QStringList & );

  private:
    QString getEventString( Event* );
    QString getTodoString( Todo* );
    QString dtToString( const QDateTime& dt, bool useTZ = true );
    
    int  getNumFromRecord( QString answer,Incidence* inc  ) ;
    QString getPart( const QString & text, bool &ok, int &start );
};

}

#endif
