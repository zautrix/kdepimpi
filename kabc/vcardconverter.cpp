/*
    This file is part of libkabc.
    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>

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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: vcardconverter.cpp,v 1.1.1.1 2004/06/26 19:01:39 zautrix Exp $
*/

//US
#include "kglobal.h"

#include "vcard21parser.h"
#include "vcardformatimpl.h"

#include "vcardconverter.h"

using namespace KABC;

struct VCardConverter::VCardConverterData
{
  VCard21Parser vcard21parser;
  VCardFormatImpl vcard30parser;
};

VCardConverter::VCardConverter()
  : d( new VCardConverterData )
{
}

VCardConverter::~VCardConverter()
{
  delete d;
  d = 0;
}

bool VCardConverter::vCardToAddressee( const QString &str, Addressee &addr, Version version )
{
  if ( version == v2_1 ) {
    addr = d->vcard21parser.readFromString( str );
    return true;
  }

  if ( version == v3_0 )
    return d->vcard30parser.readFromString( str, addr );

  return false;
}

bool VCardConverter::addresseeToVCard( const Addressee &addr, QString &str, Version version )
{
  if ( version == v2_1 )
    return false;

  if ( version == v3_0 )
    return d->vcard30parser.writeToString( addr, str );

  return false;
}


/* Helper functions */

QString KABC::dateToVCardString( const QDateTime &dateTime )
{
qDebug("vcardconverter.cpp : KABC::dateToVCardString transformation does not work yet");
  return KGlobal::locale()->formatDate(dateTime.date(), true);
//US  return dateTime.toString("yyyyMMddThhmmssZ");
}

QString KABC::dateToVCardString( const QDate &date )
{
qDebug("vcardconverter.cpp : KABC::dateToVCardString transformation does not work yet");
  return KGlobal::locale()->formatDate(date, true);
//US  return date.toString("yyyyMMdd");
}

QDateTime KABC::VCardStringToDate( const QString &dateString )
{
  QDate date;
  QTime time;
  QString d( dateString );

//US I hope this is correct
//US  d = d.remove('-').remove(':');
 d = d.replace( QRegExp("-"), "" );
 d = d.replace( QRegExp(":"), "" );
  

  if (d.length()>=8)
    date = QDate( d.mid(0,4).toUInt(), d.mid(4,2).toUInt(), d.mid(6,2).toUInt() );
  if (d.length()>9 && d[8].upper()=='T')
    time = QTime( d.mid(9,2).toUInt(), d.mid(11,2).toUInt(), d.mid(13,2).toUInt() );

  return QDateTime( date, time );
}

