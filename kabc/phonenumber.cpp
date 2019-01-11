/*
    This file is part of libkabc.
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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: phonenumber.cpp,v 1.20 2006/02/24 18:49:56 zautrix Exp $
*/

#include <kapplication.h>
#include <klocale.h>

#include "phonenumber.h"

using namespace KABC;

PhoneNumber::PhoneNumber() :
  mType( Home )
{
  init();
}

PhoneNumber::PhoneNumber( const QString &number, int type ) :
  mType( type ), mNumber( number )
{
  init();
}

PhoneNumber::~PhoneNumber()
{
}

void PhoneNumber::init()
{
  mId = KApplication::randomString( 8 );
}

bool PhoneNumber::operator==( const PhoneNumber &p ) const
{
  if ( mNumber != p.mNumber ) return false;
  if ( mType != p.mType ) return false;
  
  return true;
}

bool PhoneNumber::operator!=( const PhoneNumber &p ) const
{
  return !( p == *this );
}
void PhoneNumber::makeCompat()
{
    mType = getCompatType( mType );
}
int PhoneNumber::getCompatType( int type )
{
  
    if ((type & Cell) == Cell) {
        if ((type & Work) == Work)
            return Car;
        return Cell;
    }
  if ((type & Home) == Home) {
      if ((type & Pref) == Pref) 
          return (Home | Pref);
      if ((type & Fax) == Fax) 
          return (Home | Fax);
      return  (Home);
  }
  if ((type & Work) == Work) {
      if ((type & Pref) == Pref) 
          return (Work| Pref);
      if ((type & Fax) == Fax) 
          return (Fax |Work);
      if ((type & Msg) == Msg) {
          if ((type & Voice) == Voice)
              return ( Msg | Voice |Work);
          return ( Msg | Work);
      }
      return  Work;
  }
  if ((type & Pcs) == Pcs) {
      if ((type & Pref) == Pref) 
          return Pcs | Pref;
      if ((type & Voice) == Voice) 
          return Pcs | Voice;
      return  Pcs;
  }
  if ((type & Car) == Car)
      return Car;
  if ((type & Pager) == Pager)
      return Pager;
  if ((type & Isdn) == Isdn)
      return Isdn;
#if 0
  if ((type & Video) == Video)
      return Video;
#endif
  if ((type & Msg) == Msg)
      return Msg;
  if ((type & Fax) == Fax)
    return Fax;

  if ((type & Pref) == Pref)
      return  Pref;

  return Voice;

}
bool PhoneNumber::simplifyNumber()
{
    QString Number;
    int i;
    Number = mNumber.stripWhiteSpace ();
    mNumber = "";
    for ( i = 0; i < Number.length(); ++i) {
        if ( Number.at(i).isDigit() || Number.at(i) == '+'|| Number.at(i) == '*'|| Number.at(i) == '#' )
            mNumber += Number.at(i);
    }
    return ( mNumber.length() > 0 );
}
// make cellphone compatible
void PhoneNumber::simplifyType()
{
    if ( mType & Fax ) mType = Fax;
    else if ( mType & Cell ) mType = Cell;
    else if ( mType & Work  ) mType = Work  ;
    else if ( mType & Home ) mType = Home;
    else  mType = Pref;
}
bool PhoneNumber::contains( const PhoneNumber &p  )
{
    PhoneNumber myself;
    PhoneNumber other;
    myself = *this;
    other = p;
    myself.simplifyNumber();
    other.simplifyNumber();
    if ( myself.number() != other.number ())
        return false;
    myself.simplifyType();
    other.simplifyType();
    if ( myself.type() == other.type())
        return true;
    return false;
}

void PhoneNumber::setId( const QString &id )
{
  mId = id;
}

QString PhoneNumber::id() const
{
  return mId;
}

void PhoneNumber::setNumber( const QString &number )
{
  mNumber = number;
}

QString PhoneNumber::number() const
{
  return mNumber;
}

void PhoneNumber::setType( int type )
{
  mType = type;
}

int PhoneNumber::type() const
{
  return mType;
}

QString PhoneNumber::typeLabel() const
{
  QString label;
  bool first = true;

  TypeList list = typeList();

  TypeList::Iterator it;
  for ( it = list.begin(); it != list.end(); ++it ) {
    if ( ( type() & (*it) ) && ( (*it) != Pref ) ) {
      label.append( ( first ? "" : "/" ) + typeLabel( *it ) );
      if ( first )
        first = false;
    }
  }

  return label;
}

QString PhoneNumber::label() const
{
  return typeLabel( type() );
}

PhoneNumber::TypeList PhoneNumber::typeList()
{
  TypeList list;
  
  list << Home << Work << Msg << Pref << Voice << Fax << Cell << Video
       << Bbs << Modem << Car << Isdn << Pcs << Pager;

  return list;
}
PhoneNumber::TypeList PhoneNumber::supportedTypeList()
{
    static TypeList list;
    if ( list.count() == 0 ) 
        list << (Home| Pref) << (Work| Pref) << Cell <<(Pcs|Pref)<<  (Pcs|Voice)<<  Home << Work << Car  <<   Pcs <<(Work|  Msg | Voice)  << (Work|  Msg)    << (Home | Fax) <<   (Work| Fax) <<  Fax<<   Pager  << Isdn  << Msg << Pref << Voice;
    return list;
}

#if 0
Home| Pref          i18n("Home")         Home
Work| Pref          i18n("Work")         Business
Cell                i18n("Mobile")       Mobile
Pcs|Pref            i18n("SiP")          Radio
Pcs|Voice           i18n("VoIP")         TTY/TTD
Home                i18n("Home2")        Home 2
Work                i18n("Work2")        Business 2
Car                 i18n("Mobile2")      Car    
Pcs                 i18n("SiP2")         Telex
Work|  Msg | Voice  i18n("Assistent")    Assistent
Work|  Msg          i18n("Company")      Company
Home | Fax          i18n("Fax (Home)")   Home Fax
Work| Fax           i18n("Fax (Work)")   Business Fax
Fax                 i18n("Fax (Other)")  Other Fax
Pager               i18n("Pager")        Pager
Isdn                i18n("ISDN")         Isdn
Msg                 i18n("Callback")     Callback
Pref                i18n("Primary")      Primary
Voice;              i18n("Other")        Other

#endif

QStringList PhoneNumber::supportedTypeListNames()
{
  static QStringList list;
  if ( list.count() == 0 ) 
      list << i18n("Home") <<  i18n("Work") << i18n("Mobile") << i18n("SiP") << i18n("VoIP") <<i18n("Home2")<< i18n("Work2") << i18n("Mobile2") << i18n("SiP2") << i18n("Assistent") << i18n("Company") << i18n("Fax (Home)") << i18n("Fax (Work)") << i18n("Fax (Other)")  << i18n("Pager") << i18n("ISDN")  << i18n("Callback") << i18n("Primary")<< i18n("Other");
  return list;
}

int PhoneNumber::typeListIndex4Type(int type )
{
    TypeList list = supportedTypeList();
    int i = 0;
    while ( i <  list.count() ) {
        if ( list [i] == type )
            return i;
        ++i;
    }
    return list.count()-1;
}

QString PhoneNumber::label( int type )
{
  return typeLabel( type );
}

QString PhoneNumber::typeLabel( int type )
{
  if ((type & Cell) == Cell)
      return i18n("Mobile");
  if ((type & Home) == Home) {
      if ((type & Pref) == Pref) 
          return i18n("Home");
      if ((type & Fax) == Fax) 
          return i18n("Fax (Home)");
      return  i18n("Home2");
  }
  
  if ((type & Work) == Work) {
      if ((type & Pref) == Pref) 
          return i18n("Work");
      if ((type & Fax) == Fax) 
          return i18n("Fax (Work)");
      if ((type & Msg) == Msg) {
          if ((type & Voice) == Voice)
              return i18n("Assistent");
          return i18n("Company");
      }
      return  i18n("Work2");
  }
  if ((type & Pcs) == Pcs) {
      if ((type & Pref) == Pref) 
          return i18n("SiP");
      if ((type & Voice) == Voice) 
          return i18n("VoIP");
      return i18n("SiP2");
  }
  if ((type & Car) == Car)
      return i18n("Mobile2");
  if ((type & Pager) == Pager)
      return i18n("Pager");
  if ((type & Isdn) == Isdn)
      return i18n("ISDN");
  if ((type & Video) == Video)
      return i18n("Video");

  if ((type & Msg) == Msg)
      return i18n("Callback");
  if ((type & Fax) == Fax)
    return i18n("Fax (Other)");

  if ((type & Pref) == Pref)
      return i18n("Primary");


  return i18n("Other");


#if 0



  QString typeString;


  if ((type & Cell) == Cell)
    typeString += i18n("Mobile") +" ";
  if ((type & Home) == Home)
    typeString += i18n("Home")+" ";
  else if ((type & Work) == Work)
    typeString += i18n("Work")+" ";
   
   if ((type & Sip) == Sip)
    typeString += i18n("SIP")+" ";
  if ((type & Car) == Car)
    typeString += i18n("Car")+" ";

  if ((type & Fax) == Fax)
    typeString += i18n("Fax");
  else if ((type & Msg) == Msg)
    typeString += i18n("Messenger");
  else if ((type & Video) == Video)
    typeString += i18n("Video");
  else if ((type & Bbs) == Bbs)
    typeString += i18n("Mailbox");
  else if ((type & Modem) == Modem)
    typeString += i18n("Modem");
  else if ((type & Isdn) == Isdn)
    typeString += i18n("ISDN");
  else if ((type & Pcs) == Pcs)
    typeString += i18n("PCS");
  else if ((type & Pager) == Pager)
    typeString += i18n("Pager");
  // add the prefered flag    
  /*
  if ((type & Pref) == Pref)
    typeString += i18n("(p)");
  */
  //if we still have no match, return "other"        
  if (typeString.isEmpty()) {
      if ((type & Voice) == Voice)
          return i18n("Voice");
      else
          return i18n("Other");
  }
      
  return typeString.stripWhiteSpace();
#endif
}

QDataStream &KABC::operator<<( QDataStream &s, const PhoneNumber &phone )
{
    return s << phone.mId << phone.mType << phone.mNumber;
}

QDataStream &KABC::operator>>( QDataStream &s, PhoneNumber &phone )
{
    s >> phone.mId >> phone.mType >> phone.mNumber;

    return s;
}
