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

$Id: address.cpp,v 1.4 2005/08/22 15:42:41 zautrix Exp $
*/

//US added kglobal.h
#include <kglobal.h>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>

#include <qfile.h>

#include "address.h"

using namespace KABC;

QMap<QString, QString> Address::mISOMap;

Address::Address() :
  mEmpty( true ), mType( 0 )
{
  mId = KApplication::randomString( 10 );
}

Address::Address( int type ) :
  mEmpty( true ), mType( type )
{
  mId = KApplication::randomString( 10 );
}

bool Address::operator==( const Address &a ) const
{
  if ( mPostOfficeBox != a.mPostOfficeBox ) return false;
  if ( mExtended != a.mExtended ) return false;
  if ( mStreet != a.mStreet ) return false;
  if ( mLocality != a.mLocality ) return false;
  if ( mRegion != a.mRegion ) return false;
  if ( mPostalCode != a.mPostalCode ) return false;
  if ( mCountry != a.mCountry ) return false;
  if ( mLabel != a.mLabel ) return false;
  
  return true;
}

bool Address::operator!=( const Address &a ) const
{
  return !( a == *this );
}

bool Address::isEmpty() const
{
  if ( mPostOfficeBox.isEmpty() &&
       mExtended.isEmpty() &&
       mStreet.isEmpty() &&
       mLocality.isEmpty() &&
       mRegion.isEmpty() &&
       mPostalCode.isEmpty() &&
       mCountry.isEmpty() &&
       mLabel.isEmpty() ) {
    return true;
  }
  return false;
}

QStringList Address::asList()
{
    QStringList result;
    if ( ! mPostOfficeBox.isEmpty() )result.append(mPostOfficeBox);
    if ( ! mExtended.isEmpty())result.append(mExtended);
    if ( ! mStreet.isEmpty())result.append(mStreet);
    if ( ! mLocality.isEmpty() )result.append(mLocality);
    if ( ! mRegion.isEmpty())result.append(mRegion);
    if ( ! mPostalCode.isEmpty())result.append(mPostalCode);
    if ( ! mCountry.isEmpty())result.append(mCountry);
    if ( ! mLabel.isEmpty() )result.append(mLabel);
    return result;
}

bool Address::matchAddress( QRegExp* re ) const
{

#if QT_VERSION >= 0x030000
    if (re->search( mPostOfficeBox ) == 0) return true;
    if (re->search( mExtended     ) == 0) return true;
    if (re->search( mStreet     ) == 0) return true;
    if (re->search( mLocality      ) == 0) return true;
    if (re->search( mRegion      ) == 0) return true;
    if (re->search( mPostalCode     ) == 0) return true;
    if (re->search( mCountry     ) == 0) return true;
    if (re->search( mLabel     ) == 0) return true;
#else
        if (re->match( mPostOfficeBox    ) == 0) return true;
        if (re->match( mExtended     ) == 0) return true;
        if (re->match( mStreet     ) == 0) return true;
        if (re->match( mLocality     ) == 0) return true;
        if (re->match( mRegion     ) == 0) return true;
        if (re->match( mPostalCode     ) == 0) return true;
        if (re->match( mCountry     ) == 0) return true;
        if (re->match( mLabel    ) == 0) return true;
#endif   

    return false;
}

void Address::clear()
{
  *this = Address();
}

void Address::setId( const QString &id )
{
  mEmpty = false;

  mId = id;
}

QString Address::id() const
{
  return mId;
}

void Address::setType( int type )
{
  mEmpty = false;

  mType = type;
}

int Address::type() const
{
  return mType;
}

QString Address::typeLabel() const
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

void Address::setPostOfficeBox( const QString &s )
{
  mEmpty = false;

  mPostOfficeBox = s;
}

QString Address::postOfficeBox() const
{
  return mPostOfficeBox;
}

QString Address::postOfficeBoxLabel()
{
  return i18n("Post Office Box");
}


void Address::setExtended( const QString &s )
{
  mEmpty = false;

  mExtended = s;
}

QString Address::extended() const
{
  return mExtended;
}

QString Address::extendedLabel()
{
  return i18n("Extended Address Information");
}


void Address::setStreet( const QString &s )
{
  mEmpty = false;

  mStreet = s;
}

QString Address::street() const
{
  return mStreet;
}

QString Address::streetLabel()
{
  return i18n("Street");
}


void Address::setLocality( const QString &s )
{
  mEmpty = false;

  mLocality = s;
}

QString Address::locality() const
{
  return mLocality;
}

QString Address::localityLabel()
{
  return i18n("Locality");
}


void Address::setRegion( const QString &s )
{
  mEmpty = false;

  mRegion = s;
}

QString Address::region() const
{
  return mRegion;
}

QString Address::regionLabel()
{
  return i18n("Region");
}


void Address::setPostalCode( const QString &s )
{
  mEmpty = false;

  mPostalCode = s;
}

QString Address::postalCode() const
{
  return mPostalCode;
}

QString Address::postalCodeLabel()
{
  return i18n("Postal Code");
}


void Address::setCountry( const QString &s )
{
  mEmpty = false;

  mCountry = s;
}

QString Address::country() const
{
  return mCountry;
}

QString Address::countryLabel()
{
  return i18n("Country");
}


void Address::setLabel( const QString &s )
{
  mEmpty = false;

  mLabel = s;
}

QString Address::label() const
{
  return mLabel;
}

QString Address::labelLabel()
{
  return i18n("Delivery Label");
}

Address::TypeList Address::typeList()
{
  TypeList list;

  list << Dom << Intl << Postal << Parcel << Home << Work << Pref;

  return list;
}

QString Address::typeLabel( int type )
{
    QString label;
    if ( type & Dom )
        label +=  i18n("Domestic")+" ";
    if ( type & Intl )
        label +=  i18n("International")+" ";
    if ( type &  Postal )
        label +=  i18n("Postal")+" ";
    if ( type & Parcel  )
        label +=  i18n("Parcel")+" ";
    if ( type & Work )
        label +=  i18n("Work Address", "Work")+" ";
    if ( type & Home )
        label +=  i18n("Home Address", "Home") +" ";
    if ( type & Pref )
        label +=  i18n("Preferred Address", "(p)");
    if ( label.isEmpty() )
        label =  i18n("Other");
    return label;

#if 0
  switch ( type ) {
    case Dom:
      return i18n("Domestic");
      break;
    case Intl:
      return i18n("International");
      break;
    case Postal:
      return i18n("Postal");
      break;
    case Parcel:
      return i18n("Parcel");
      break;
    case Home:
      return i18n("Home Address", "Home");
      break;
    case Work:
      return i18n("Work Address", "Work");
      break;
    case Pref:
      return i18n("Preferred Address");
      break;
    default:
      return i18n("Other");
      break;
  }
#endif
}

void Address::dump() const
{
    qDebug("Address::dump() +++++++++++++++++ ");
#if 0
  kdDebug(5700) << "  Address {" << endl;
  kdDebug(5700) << "    Id: " << id() << endl;
  kdDebug(5700) << "    Extended: " << extended() << endl;
  kdDebug(5700) << "    Street: " << street() << endl;
  kdDebug(5700) << "    Postal Code: " << postalCode() << endl;
  kdDebug(5700) << "    Locality: " << locality() << endl;
  kdDebug(5700) << "  }" << endl;
#endif
}


QString Address::formattedAddress( const QString &realName
                                 , const QString &orgaName ) const
{
  QString ciso;
  QString addrTemplate;
  QString ret;

  // **************************************************************
  // LR:  currently we have no iso handling - we will format the address manually here

 QString text;
      if ( !street().isEmpty() )
        text += street() + "\n";

      if ( !postOfficeBox().isEmpty() )
        text += postOfficeBox() + "\n";

      text += locality() + QString(" ") + region();

      if ( !postalCode().isEmpty() )
        text += QString(", ") + postalCode();

      text += "\n";

      if ( !country().isEmpty() )
        text += country() + "\n";

      text += extended();


  return text;
  // **************************************************************

  // FIXME: first check for iso-country-field and prefer that one
  if ( !country().isEmpty() ) {
    ciso = countryToISO( country() );
  } else {
    // fall back to our own country
    ciso = KGlobal::locale()->country();
  }
  //qDebug("ciso %s ",ciso.latin1() );
  KSimpleConfig entry( locate( "locale", 
        QString( "l10n/" ) + ciso + QString( "/entry.desktop" ) ) );
  entry.setGroup( "KCM Locale" );
  
  // decide whether this needs special business address formatting
  if ( orgaName.isNull() ) {
    addrTemplate = entry.readEntry( "AddressFormat" );
  } else {
    addrTemplate = entry.readEntry( "BusinessAddressFormat" );
    if ( addrTemplate.isEmpty() )
      addrTemplate = entry.readEntry( "AddressFormat" );
  }
  
  // in the case there's no format found at all, default to what we've always
  // used:
  if ( addrTemplate.isEmpty() ) {
      qDebug("address format database incomplete****************** ");
    kdWarning(5700) << "address format database incomplete "
        << "(no format for locale " << ciso 
        << " found). Using default address formatting." << endl;
    addrTemplate = "%0(%n\\n)%0(%cm\\n)%0(%s\\n)%0(PO BOX %p\\n)%0(%l%w%r)%,%z";
  }
  
  // scan
  parseAddressTemplateSection( addrTemplate, ret, realName, orgaName );

  // now add the country line if needed (formatting this time according to
  // the rules of our own system country )
  if ( !country().isEmpty() ) {
    KSimpleConfig entry( locate( "locale", QString( "l10n/" )
          + KGlobal::locale()->country() + QString( "/entry.desktop" ) ) );
    entry.setGroup( "KCM Locale" );
    QString cpos = entry.readEntry( "AddressCountryPosition" );
    if ( "BELOW" == cpos || cpos.isEmpty() ) {
      ret = ret + "\n\n" + country().upper();
    } else if ( "below" == cpos ) {
      ret = ret + "\n\n" + country();
    } else if ( "ABOVE" == cpos ) {
      ret = country().upper() + "\n\n" + ret;
    } else if ( "above" == cpos ) {
      ret = country() + "\n\n" + ret;
    }
  }
  
  return ret;
}

bool Address::parseAddressTemplateSection( const QString &tsection, 
    QString &result, const QString &realName, const QString &orgaName ) const
{
  // This method first parses and substitutes any bracketed sections and
  // after that replaces any tags with their values. If a bracketed section
  // or a tag evaluate to zero, they are not just removed but replaced
  // with a placeholder. This is because in the last step conditionals are
  // resolved which depend on information about zero-evaluations.
  result = tsection;
  int stpos = 0;
  bool ret = false;
  
  // first check for brackets that have to be evaluated first 
  int fpos = result.find( KABC_FMTTAG_purgeempty, stpos );
  while ( -1 != fpos ) {
    int bpos1 = fpos + KABC_FMTTAG_purgeempty.length();
    int bpos2;
    // expect opening bracket and find next balanced closing bracket. If 
    // next char is no opening bracket, continue parsing (no valid tag)
    if ( '(' == result[bpos1] ) {
      bpos2 = findBalancedBracket( result, bpos1 );
      if ( -1 != bpos2 ) {
        // we have balanced brackets, recursively parse:
        QString rplstr;
        bool purge = !parseAddressTemplateSection( result.mid( bpos1+1,
                                                   bpos2-bpos1-1 ), rplstr,
                                                   realName, orgaName );
        if ( purge ) {
          // purge -> remove all
          // replace with !_P_!, so conditional tags work later
          result.replace( fpos, bpos2 - fpos + 1, "!_P_!" );
          // leave stpos as it is
        } else {
          // no purge -> replace with recursively parsed string
          result.replace( fpos, bpos2 - fpos + 1, rplstr );
          ret = true;
          stpos = fpos + rplstr.length();
        }
      } else {
        // unbalanced brackets:  keep on parsing (should not happen 
        // and will result in bad formatting)
        stpos = bpos1; 
      }
    }
    fpos = result.find( KABC_FMTTAG_purgeempty, stpos );
  }

  // after sorting out all purge tags, we just search'n'replace the rest,
  // keeping track of whether at least one tag evaluates to something.
  // The following macro needs QString for R_FIELD
  // It substitutes !_P_! for empty fields so conditional tags work later
#define REPLTAG(R_TAG,R_FIELD) \
  if ( result.contains(R_TAG, false) ) { \
    QString rpl = R_FIELD.isEmpty() ? QString("!_P_!") : R_FIELD; \
    result.replace( R_TAG, rpl ); \
    if ( !R_FIELD.isEmpty() ) { \
      ret = true; \
    } \
  }
  REPLTAG( KABC_FMTTAG_realname, realName );
  REPLTAG( KABC_FMTTAG_REALNAME, realName.upper() );
  REPLTAG( KABC_FMTTAG_company, orgaName );
  REPLTAG( KABC_FMTTAG_COMPANY, orgaName.upper() );
  REPLTAG( KABC_FMTTAG_pobox, postOfficeBox() );
  REPLTAG( KABC_FMTTAG_street, street() );
  REPLTAG( KABC_FMTTAG_STREET, street().upper() );
  REPLTAG( KABC_FMTTAG_zipcode, postalCode() );
  REPLTAG( KABC_FMTTAG_location, locality() );
  REPLTAG( KABC_FMTTAG_LOCATION, locality().upper() );
  REPLTAG( KABC_FMTTAG_region, region() );
  REPLTAG( KABC_FMTTAG_REGION, region().upper() );
  result.replace( KABC_FMTTAG_newline, "\n" );
#undef REPLTAG
 
  // conditional comma 
  fpos = result.find( KABC_FMTTAG_condcomma, 0 );
  while ( -1 != fpos ) {
    QString str1 = result.mid( fpos - 5, 5 );
    QString str2 = result.mid( fpos + 2, 5 );
    if ( str1 != "!_P_!" && str2 != "!_P_!" ) {
      result.replace( fpos, 2, ", " );
    } else {
      result.remove( fpos, 2 );
    }
    fpos = result.find( KABC_FMTTAG_condcomma, fpos );
  }
  // conditional whitespace
  fpos = result.find( KABC_FMTTAG_condwhite, 0 );
  while ( -1 != fpos ) {
    QString str1 = result.mid( fpos - 5, 5 );
    QString str2 = result.mid( fpos + 2, 5 );
    if ( str1 != "!_P_!" && str2 != "!_P_!" ) {
      result.replace( fpos, 2, " " );
    } else {
      result.remove( fpos, 2 );
    }
    fpos = result.find( KABC_FMTTAG_condwhite, fpos );
  }

  // remove purged:
//US my QT version does not support remove. So lets do it the old fashioned way.  
//US  result.remove( "!_P_!" );
  int n = result.find("!_P_!");
  if (n >= 0)
      result.remove( n, 5 );

  return ret;
}

int Address::findBalancedBracket( const QString &tsection, int pos ) const
{
  int balancecounter = 0;
  for( unsigned int i = pos + 1; i < tsection.length(); i++ ) {
    if ( ')' == tsection.at(i) && 0 == balancecounter ) {
      // found end of brackets
      return i;
    } else
    if ( '(' == tsection.at(i) ) {
      // nested brackets
      balancecounter++;
    }
  }
  return -1;
}

QString Address::countryToISO( const QString &cname )
{
  // we search a map file for translations from country names to
  // iso codes, storing caching things in a QMap for faster future 
  // access.
/*US
  
  QString isoCode = mISOMap[ cname ];
  if ( !isoCode.isEmpty() )
    return isoCode;

  QString mapfile = KGlobal::dirs()->findResource( "data", 
          QString::fromLatin1( "kabc/countrytransl.map" ) );

  QFile file( mapfile );
  if ( file.open( IO_ReadOnly ) ) {
    QTextStream s( &file );
    QString strbuf = s.readLine();
    while( !strbuf.isNull() ) {
      if ( strbuf.startsWith( cname ) ) {
        int index = strbuf.findRev('\t');
        strbuf = strbuf.mid(index+1, 2);
        file.close();
        mISOMap[ cname ] = strbuf;
        return strbuf;
      }
      strbuf = s.readLine();
    }
    file.close();
  }
*/  
  // fall back to system country
  mISOMap[ cname ] = KGlobal::locale()->country();
  return KGlobal::locale()->country();
}

QString Address::ISOtoCountry( const QString &ISOname )
{
/*US
  // get country name from ISO country code (e.g. "no" -> i18n("Norway"))
  QString mapfile = KGlobal::dirs()->findResource( "data", 
          QString::fromLatin1( "kabc/countrytransl.map" ) );

kdWarning() << "MAPFILE : " << mapfile << endl;
  QFile file( mapfile );
  if ( file.open( IO_ReadOnly ) ) {
    QTextStream s( &file );
    QString searchStr = "\t" + ISOname.simplifyWhiteSpace().lower();
kdWarning() << "Suche : " << searchStr << endl;
    QString strbuf = s.readLine();
    int pos;
    while( !strbuf.isNull() ) {
      if ( (pos=strbuf.find( searchStr )) != -1 ) {
        file.close();
        return i18n(strbuf.left(pos).utf8());
      }
      strbuf = s.readLine();
    }
    file.close();
  }
*/
  return ISOname;
}

QDataStream &KABC::operator<<( QDataStream &s, const Address &addr )
{
    return s << addr.mId << addr.mType << addr.mPostOfficeBox <<
	    addr.mExtended << addr.mStreet << addr.mLocality <<
	    addr.mRegion << addr.mPostalCode << addr.mCountry <<
	    addr.mLabel;
}

QDataStream &KABC::operator>>( QDataStream &s, Address &addr )
{
    s >> addr.mId >> addr.mType >> addr.mPostOfficeBox >> addr.mExtended >>
	    addr.mStreet >> addr.mLocality >> addr.mRegion >>
	    addr.mPostalCode >> addr.mCountry >> addr.mLabel;

    addr.mEmpty = false;

    return s;
}
