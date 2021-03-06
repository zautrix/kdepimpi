/*
    This file is part of libkabc.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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

#include <qdatastream.h>
#include <qstring.h>
#include <qregexp.h>

#include "agent.h"
#include "key.h"
#include "picture.h"
#include "secrecy.h"
#include "sound.h"

#include "vcardtool.h"

using namespace KABC;

VCardTool::VCardTool()
{
  mAddressTypeMap.insert( "dom", Address::Dom );
  mAddressTypeMap.insert( "intl", Address::Intl );
  mAddressTypeMap.insert( "postal", Address::Postal );
  mAddressTypeMap.insert( "parcel", Address::Parcel );
  mAddressTypeMap.insert( "home", Address::Home );
  mAddressTypeMap.insert( "work", Address::Work );
  mAddressTypeMap.insert( "pref", Address::Pref );

  mPhoneTypeMap.insert( "HOME", PhoneNumber::Home );
  mPhoneTypeMap.insert( "WORK", PhoneNumber::Work );
  mPhoneTypeMap.insert( "MSG", PhoneNumber::Msg );
  mPhoneTypeMap.insert( "PREF", PhoneNumber::Pref );
  mPhoneTypeMap.insert( "VOICE", PhoneNumber::Voice );
  mPhoneTypeMap.insert( "FAX", PhoneNumber::Fax );
  mPhoneTypeMap.insert( "CELL", PhoneNumber::Cell );
  mPhoneTypeMap.insert( "VIDEO", PhoneNumber::Video );
  mPhoneTypeMap.insert( "BBS", PhoneNumber::Bbs );
  mPhoneTypeMap.insert( "MODEM", PhoneNumber::Modem );
  mPhoneTypeMap.insert( "CAR", PhoneNumber::Car );
  mPhoneTypeMap.insert( "ISDN", PhoneNumber::Isdn );
  mPhoneTypeMap.insert( "PCS", PhoneNumber::Pcs );
  mPhoneTypeMap.insert( "PAGER", PhoneNumber::Pager );
}

VCardTool::~VCardTool()
{
}

// TODO: make list a const&
QString VCardTool::createVCards( Addressee::List list, VCard::Version version )
{
  VCard::List vCardList;
  static const QRegExp semiExp(";");

  Addressee::List::ConstIterator addrIt;
  Addressee::List::ConstIterator listEnd( list.end() );
  for ( addrIt = list.begin(); addrIt != listEnd; ++addrIt ) {
    VCard card;
    QStringList::ConstIterator strIt;

    // ADR + LABEL
    const Address::List addresses = (*addrIt).addresses();
    for ( Address::List::ConstIterator it = addresses.begin(); it != addresses.end(); ++it ) {
      QStringList address;

      bool isEmpty = ( (*it).postOfficeBox().isEmpty() &&
                     (*it).extended().isEmpty() &&
                     (*it).street().isEmpty() &&
                     (*it).locality().isEmpty() &&
                     (*it).region().isEmpty() &&
                     (*it).postalCode().isEmpty() &&
                     (*it).country().isEmpty() );

      address.append( (*it).postOfficeBox().replace( semiExp, "\\;" ) );
      address.append( (*it).extended().replace( semiExp, "\\;" ) );
      address.append( (*it).street().replace( semiExp, "\\;" ) );
      address.append( (*it).locality().replace( semiExp, "\\;" ) );
      address.append( (*it).region().replace( semiExp, "\\;" ) );
      address.append( (*it).postalCode().replace( semiExp, "\\;" ) );
      address.append( (*it).country().replace( semiExp, "\\;" ) );

      VCardLine adrLine( "ADR", address.join( ";" ) );
      if ( version == VCard::v2_1 ) {
        adrLine.addParameter( "CHARSET", "UTF-8" );
        adrLine.addParameter( "ENCODING", "8BIT" );
      }

      VCardLine labelLine( "LABEL", (*it).label() );
      if ( version == VCard::v2_1 ) {
        labelLine.addParameter( "CHARSET", "UTF-8" );
        labelLine.addParameter( "ENCODING", "8BIT" );
      }

      bool hasLabel = !(*it).label().isEmpty();
      QMap<QString, int>::ConstIterator typeIt;
      for ( typeIt = mAddressTypeMap.begin(); typeIt != mAddressTypeMap.end(); ++typeIt ) {
        if ( typeIt.data() & (*it).type() ) {
          adrLine.addParameter( "TYPE", typeIt.key() );
          if ( hasLabel )
            labelLine.addParameter( "TYPE",  typeIt.key() );
        }
      }

      if ( !isEmpty )
        card.addLine( adrLine );
      if ( hasLabel )
        card.addLine( labelLine );
    }

    // AGENT
    card.addLine( createAgent( version, (*addrIt).agent() ) );

    // BDAY
    card.addLine( VCardLine( "BDAY", createDateTime( (*addrIt).birthday() ) ) );

    // CATEGORIES
    if ( version == VCard::v3_0 ) {
      QStringList categories = (*addrIt).categories();
      QStringList::Iterator catIt;
      for ( catIt = categories.begin(); catIt != categories.end(); ++catIt )
        (*catIt).replace( QRegExp(","), "\\," );

      VCardLine catLine( "CATEGORIES", categories.join( "," ) );
      if ( version == VCard::v2_1 ) {
        catLine.addParameter( "CHARSET", "UTF-8" );
        catLine.addParameter( "ENCODING", "8BIT" );
      }

      card.addLine( catLine );
    }

    // CLASS
    if ( version == VCard::v3_0 ) {
      card.addLine( createSecrecy( (*addrIt).secrecy() ) );
    }

    // EMAIL
    const QStringList emails = (*addrIt).emails();
    bool pref = true;
    for ( strIt = emails.begin(); strIt != emails.end(); ++strIt ) {
      VCardLine line( "EMAIL", *strIt );
      if ( version == VCard::v2_1 ) {
        line.addParameter( "CHARSET", "UTF-8" );
        line.addParameter( "ENCODING", "8BIT" );
      }
      if ( pref == true && emails.count() > 1 ) {
        line.addParameter( "TYPE", "PREF" );
        pref = false;
      }
      card.addLine( line );
    }

    // FN
    VCardLine fnLine( "FN", (*addrIt).formattedName() );
    if ( version == VCard::v2_1 ) {
      fnLine.addParameter( "CHARSET", "UTF-8" );
      fnLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( fnLine );

    // GEO
    Geo geo = (*addrIt).geo();
    if ( geo.isValid() ) {
      QString str;
      str.sprintf( "%.6f;%.6f", geo.latitude(), geo.longitude() );
      card.addLine( VCardLine( "GEO", str ) );
    }

    // KEY
    const Key::List keys = (*addrIt).keys();
    Key::List::ConstIterator keyIt;
    for ( keyIt = keys.begin(); keyIt != keys.end(); ++keyIt )
      card.addLine( createKey( *keyIt ) );

    // LOGO
    card.addLine( createPicture( "LOGO", (*addrIt).logo() ) );

    // MAILER
    VCardLine mailerLine( "MAILER", (*addrIt).mailer() );
    if ( version == VCard::v2_1 ) {
      mailerLine.addParameter( "CHARSET", "UTF-8" );
      mailerLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( mailerLine );

    // N
    QStringList name;
    name.append( (*addrIt).familyName().replace( semiExp, "\\;" ) );
    name.append( (*addrIt).givenName().replace( semiExp, "\\;" ) );
    name.append( (*addrIt).additionalName().replace( semiExp, "\\;" ) );
    name.append( (*addrIt).prefix().replace( semiExp, "\\;" ) );
    name.append( (*addrIt).suffix().replace( semiExp, "\\;" ) );

    VCardLine nLine( "N", name.join( ";" ) );
    if ( version == VCard::v2_1 ) {
      nLine.addParameter( "CHARSET", "UTF-8" );
      nLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( nLine );

    // NAME
    VCardLine nameLine( "NAME", (*addrIt).name() );
    if ( version == VCard::v2_1 ) {
      nameLine.addParameter( "CHARSET", "UTF-8" );
      nameLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( nameLine );

    // NICKNAME
    if ( version == VCard::v3_0 )
      card.addLine( VCardLine( "NICKNAME", (*addrIt).nickName() ) );

    // NOTE
    VCardLine noteLine( "NOTE", (*addrIt).note() );
    if ( version == VCard::v2_1 ) {
      noteLine.addParameter( "CHARSET", "UTF-8" );
      noteLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( noteLine );

    // ORG
    VCardLine orgLine( "ORG", (*addrIt).organization() );
    if ( version == VCard::v2_1 ) {
      orgLine.addParameter( "CHARSET", "UTF-8" );
      orgLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( orgLine );

    // PHOTO
    card.addLine( createPicture( "PHOTO", (*addrIt).photo() ) );

    // PROID
    if ( version == VCard::v3_0 )
      card.addLine( VCardLine( "PRODID", (*addrIt).productId() ) );

    // REV
    card.addLine( VCardLine( "REV", createDateTime( (*addrIt).revision() ) ) );

    // ROLE
    VCardLine roleLine( "ROLE", (*addrIt).role() );
    if ( version == VCard::v2_1 ) {
      roleLine.addParameter( "CHARSET", "UTF-8" );
      roleLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( roleLine );

    // SORT-STRING
    if ( version == VCard::v3_0 )
      card.addLine( VCardLine( "SORT-STRING", (*addrIt).sortString() ) );

    // SOUND
    card.addLine( createSound( (*addrIt).sound() ) );

    // TEL
    const PhoneNumber::List phoneNumbers = (*addrIt).phoneNumbers();
    PhoneNumber::List::ConstIterator phoneIt;
    for ( phoneIt = phoneNumbers.begin(); phoneIt != phoneNumbers.end(); ++phoneIt ) {
      VCardLine line( "TEL", (*phoneIt).number() );

      QMap<QString, int>::ConstIterator typeIt;
      for ( typeIt = mPhoneTypeMap.begin(); typeIt != mPhoneTypeMap.end(); ++typeIt ) {
        if ( typeIt.data() & (*phoneIt).type() )
          line.addParameter( "TYPE", typeIt.key() );
      }

      card.addLine( line );
    }

    // TITLE
    VCardLine titleLine( "TITLE", (*addrIt).title() );
    if ( version == VCard::v2_1 ) {
      titleLine.addParameter( "CHARSET", "UTF-8" );
      titleLine.addParameter( "ENCODING", "8BIT" );
    }
    card.addLine( titleLine );

    // TZ
    TimeZone timeZone = (*addrIt).timeZone();
    if ( timeZone.isValid() ) {
      QString str;

      int neg = 1;
      if ( timeZone.offset() < 0 )
        neg = -1;

      str.sprintf( "%c%02d:%02d", ( timeZone.offset() >= 0 ? '+' : '-' ),
                                  ( timeZone.offset() / 60 ) * neg,
                                  ( timeZone.offset() % 60 ) * neg );

      card.addLine( VCardLine( "TZ", str ) );
    }

    // UID
    card.addLine( VCardLine( "UID", (*addrIt).uid() ) );

    // URL
    card.addLine( VCardLine( "URL", (*addrIt).url().url() ) );

    // VERSION
    if ( version == VCard::v2_1 )
      card.addLine( VCardLine( "VERSION", "2.1" ) );
    if ( version == VCard::v3_0 )
      card.addLine( VCardLine( "VERSION", "3.0" ) );

    // X-
    const QStringList customs = (*addrIt).customs();
    for ( strIt = customs.begin(); strIt != customs.end(); ++strIt ) {
      QString identifier = "X-" + (*strIt).left( (*strIt).find( ":" ) );
      QString value = (*strIt).mid( (*strIt).find( ":" ) + 1 );
      if ( value.isEmpty() )
        continue;

      VCardLine line( identifier, value );
      if ( version == VCard::v2_1 ) {
        line.addParameter( "CHARSET", "UTF-8" );
        line.addParameter( "ENCODING", "8BIT" );
      }
      card.addLine( line );
    }

    vCardList.append( card );
  }

  return VCardParser::createVCards( vCardList );
}

Addressee::List VCardTool::parseVCards( const QString& vcard )
{
  static const QChar semicolonSep( ';' );
  static const QChar commaSep( ',' );
  QString identifier;

  Addressee::List addrList;
  const VCard::List vCardList = VCardParser::parseVCards( vcard );

  VCard::List::ConstIterator cardIt;
  VCard::List::ConstIterator listEnd( vCardList.end() );
  for ( cardIt = vCardList.begin(); cardIt != listEnd; ++cardIt ) {
    Addressee addr;

    const QStringList idents = (*cardIt).identifiers();
    QStringList::ConstIterator identIt;
    QStringList::ConstIterator identEnd( idents.end() );
    for ( identIt = idents.begin(); identIt != identEnd; ++identIt ) {
      const VCardLine::List lines = (*cardIt).lines( (*identIt) );
      VCardLine::List::ConstIterator lineIt;

      // iterate over the lines
      for ( lineIt = lines.begin(); lineIt != lines.end(); ++lineIt ) {
        identifier = (*lineIt).identifier().lower();
        // ADR
        if ( identifier == "adr" ) {
          Address address;
          const QStringList addrParts = splitString( semicolonSep, (*lineIt).valueString() );
          if ( addrParts.count() > 0 )
            address.setPostOfficeBox( addrParts[ 0 ] );
          if ( addrParts.count() > 1 )
            address.setExtended( addrParts[ 1 ] );
          if ( addrParts.count() > 2 )
            address.setStreet( addrParts[ 2 ] );
          if ( addrParts.count() > 3 )
            address.setLocality( addrParts[ 3 ] );
          if ( addrParts.count() > 4 )
            address.setRegion( addrParts[ 4 ] );
          if ( addrParts.count() > 5 )
            address.setPostalCode( addrParts[ 5 ] );
          if ( addrParts.count() > 6 )
            address.setCountry( addrParts[ 6 ] );

          int type = 0;

          const QStringList types = (*lineIt).parameters( "type" );
          for ( QStringList::ConstIterator it = types.begin(); it != types.end(); ++it )
            type += mAddressTypeMap[ (*it).lower() ];

          address.setType( type );
          addr.insertAddress( address );
        }

        // AGENT
        else if ( identifier == "agent" )
          addr.setAgent( parseAgent( *lineIt ) );

        // BDAY
        else if ( identifier == "bday" )
          addr.setBirthday( parseDateTime( (*lineIt).valueString() ) );

        // CATEGORIES
        else if ( identifier == "categories" ) {
          const QStringList categories = splitString( commaSep, (*lineIt).valueString() );
          addr.setCategories( categories );
        }

        // CLASS
        else if ( identifier == "class" )
          addr.setSecrecy( parseSecrecy( *lineIt ) );

        // EMAIL
        else if ( identifier == "email" ) {
          const QStringList types = (*lineIt).parameters( "type" );
          addr.insertEmail( (*lineIt).valueString(), types.findIndex( "PREF" ) != -1 );
        }

        // FN
        else if ( identifier == "fn" )
          addr.setFormattedName( (*lineIt).valueString() );

        // GEO
        else if ( identifier == "geo" ) {
          Geo geo;

          const QStringList geoParts = QStringList::split( ';', (*lineIt).valueString(), true );
          geo.setLatitude( geoParts[ 0 ].toFloat() );
          geo.setLongitude( geoParts[ 1 ].toFloat() );

          addr.setGeo( geo );
        }

        // KEY
        else if ( identifier == "key" )
          addr.insertKey( parseKey( *lineIt ) );

        // LABEL
        else if ( identifier == "label" ) {
          int type = 0;

          const QStringList types = (*lineIt).parameters( "type" );
          for ( QStringList::ConstIterator it = types.begin(); it != types.end(); ++it )
            type += mAddressTypeMap[ (*it).lower() ];

          bool available = false;
          KABC::Address::List addressList = addr.addresses();
          KABC::Address::List::Iterator it;
          for ( it = addressList.begin(); it != addressList.end(); ++it ) {
            if ( (*it).type() == type ) {
              (*it).setLabel( (*lineIt).valueString() );
              addr.insertAddress( *it );
              available = true;
              break;
            }
          }

          if ( !available ) { // a standalone LABEL tag
            KABC::Address address( type );
            address.setLabel( (*lineIt).valueString() );
            addr.insertAddress( address );
          }
        }

        // LOGO
        else if ( identifier == "logo" )
          addr.setLogo( parsePicture( *lineIt ) );

        // MAILER
        else if ( identifier == "mailer" )
          addr.setMailer( (*lineIt).valueString() );

        // N
        else if ( identifier == "n" ) {
          const QStringList nameParts = splitString( semicolonSep, (*lineIt).valueString() );
          if ( nameParts.count() > 0 )
            addr.setFamilyName( nameParts[ 0 ] );
          if ( nameParts.count() > 1 )
            addr.setGivenName( nameParts[ 1 ] );
          if ( nameParts.count() > 2 )
            addr.setAdditionalName( nameParts[ 2 ] );
          if ( nameParts.count() > 3 )
            addr.setPrefix( nameParts[ 3 ] );
          if ( nameParts.count() > 4 )
            addr.setSuffix( nameParts[ 4 ] );
        }

        // NAME
        else if ( identifier == "name" )
          addr.setName( (*lineIt).valueString() );

        // NICKNAME
        else if ( identifier == "nickname" )
          addr.setNickName( (*lineIt).valueString() );

        // NOTE
        else if ( identifier == "note" )
          addr.setNote( (*lineIt).valueString() );

        // ORGANIZATION
        else if ( identifier == "org" )
          addr.setOrganization( (*lineIt).valueString() );

        // PHOTO
        else if ( identifier == "photo" )
          addr.setPhoto( parsePicture( *lineIt ) );

        // PROID
        else if ( identifier == "prodid" )
          addr.setProductId( (*lineIt).valueString() );

        // REV
        else if ( identifier == "rev" )
          addr.setRevision( parseDateTime( (*lineIt).valueString() ) );

        // ROLE
        else if ( identifier == "role" )
          addr.setRole( (*lineIt).valueString() );

        // SORT-STRING
        else if ( identifier == "sort-string" )
          addr.setSortString( (*lineIt).valueString() );

        // SOUND
        else if ( identifier == "sound" )
          addr.setSound( parseSound( *lineIt ) );

        // TEL
        else if ( identifier == "tel" ) {
          PhoneNumber phone;
          phone.setNumber( (*lineIt).valueString() );

          int type = 0;

          const QStringList types = (*lineIt).parameters( "type" );
          for ( QStringList::ConstIterator it = types.begin(); it != types.end(); ++it )
            type += mPhoneTypeMap[(*it).upper()];

          phone.setType( type );

          addr.insertPhoneNumber( phone );
        }

        // TITLE
        else if ( identifier == "title" )
          addr.setTitle( (*lineIt).valueString() );

        // TZ
        else if ( identifier == "tz" ) {
          TimeZone tz;
          const QString date = (*lineIt).valueString();

          int hours = date.mid( 1, 2).toInt();
          int minutes = date.mid( 4, 2 ).toInt();
          int offset = ( hours * 60 ) + minutes;
          offset = offset * ( date[ 0 ] == '+' ? 1 : -1 );

          tz.setOffset( offset );
          addr.setTimeZone( tz );
        }

        // UID
        else if ( identifier == "uid" )
          addr.setUid( (*lineIt).valueString() );

        // URL
        else if ( identifier == "url" )
          addr.setUrl( KURL( (*lineIt).valueString() ) );

        // X-
        else if ( identifier.startsWith( "x-" ) ) {
          const QString key = (*lineIt).identifier().mid( 2 );
          int dash = key.find( "-" );
          addr.insertCustom( key.left( dash ), key.mid( dash + 1 ), (*lineIt).valueString() );
        }
      }
    }
    addr.makePhoneNumbersOLcompatible();
    addrList.append( addr );
  }

  return addrList;
}

QDateTime VCardTool::parseDateTime( const QString &str )
{
  QDateTime dateTime;

  if ( str.find( '-' ) == -1 ) { // is base format (yyyymmdd)
    dateTime.setDate( QDate( str.left( 4 ).toInt(), str.mid( 4, 2 ).toInt(),
                             str.mid( 6, 2 ).toInt() ) );

    if ( str.find( 'T' ) ) // has time information yyyymmddThh:mm:ss
      dateTime.setTime( QTime( str.mid( 11, 2 ).toInt(), str.mid( 14, 2 ).toInt(),
                               str.mid( 17, 2 ).toInt() ) );

  } else { // is extended format yyyy-mm-dd
    dateTime.setDate( QDate( str.left( 4 ).toInt(), str.mid( 5, 2 ).toInt(),
                             str.mid( 8, 2 ).toInt() ) );

    if ( str.find( 'T' ) ) // has time information yyyy-mm-ddThh:mm:ss
      dateTime.setTime( QTime( str.mid( 11, 2 ).toInt(), str.mid( 14, 2 ).toInt(),
                               str.mid( 17, 2 ).toInt() ) );
  }

  return dateTime;
}

QString VCardTool::createDateTime( const QDateTime &dateTime )
{
  QString str;

  if ( dateTime.date().isValid() ) {
    str.sprintf( "%4d-%02d-%02d", dateTime.date().year(), dateTime.date().month(),
                 dateTime.date().day() );
    if ( dateTime.time().isValid() ) {
      QString tmp;
      tmp.sprintf( "T%02d:%02d:%02dZ", dateTime.time().hour(), dateTime.time().minute(),
                   dateTime.time().second() );
      str += tmp;
    }
  }

  return str;
}

Picture VCardTool::parsePicture( const VCardLine &line )
{
  Picture pic;

  const QStringList params = line.parameterList();
  if ( params.findIndex( "encoding" ) != -1 ) {
    QImage img;
    img.loadFromData( line.valueBytes() );
    pic.setData( img );
  } else if ( params.findIndex( "value" ) != -1 ) {
    if ( line.parameter( "value" ).lower() == "uri" )
      pic.setUrl( line.valueString() );
  }

  if ( params.findIndex( "type" ) != -1 )
    pic.setType( line.parameter( "type" ) );

  return pic;
}

VCardLine VCardTool::createPicture( const QString &identifier, const Picture &pic )
{
  VCardLine line( identifier );

  if ( pic.isIntern() ) {
    if ( !pic.data().isNull() ) {
      QByteArray input;
      QDataStream s( input, IO_WriteOnly );
      s.setVersion( 4 );
      s << pic.data();
      line.setValueBytes( input );
      line.addParameter( "encoding", "b" );
      line.addParameter( "type", "image/png" );
    }
  } else if ( !pic.url().isEmpty() ) {
    QByteArray ba;
    line.setValueString( pic.url() );
    line.addParameter( "value", "URI" );
  }

  return line;
}

Sound VCardTool::parseSound( const VCardLine &line )
{
  Sound snd;

  const QStringList params = line.parameterList();
  if ( params.findIndex( "encoding" ) != -1 )
    snd.setData( line.valueBytes() );
  else if ( params.findIndex( "value" ) != -1 ) {
    if ( line.parameter( "value" ).lower() == "uri" )
      snd.setUrl( line.valueString() );
  }

/* TODO: support sound types
  if ( params.contains( "type" ) )
    snd.setType( line.parameter( "type" ) );
*/

  return snd;
}

VCardLine VCardTool::createSound( const Sound &snd )
{
  VCardLine line( "SOUND" );

  if ( snd.isIntern() ) {
    if ( !snd.data().isEmpty() ) {
      line.setValueBytes( snd.data() );
      line.addParameter( "encoding", "b" );
      // TODO: need to store sound type!!!
    }
  } else if ( !snd.url().isEmpty() ) {
    line.setValueString( snd.url() );
    line.addParameter( "value", "URI" );
  }

  return line;
}

Key VCardTool::parseKey( const VCardLine &line )
{
  Key key;

  const QStringList params = line.parameterList();
  if ( params.findIndex( "encoding" ) != -1 )
    key.setBinaryData( line.valueBytes() );
  else
    key.setTextData( line.valueString() );

  if ( params.findIndex( "type" ) != -1 ) {
    if ( line.parameter( "type" ).lower() == "x509" )
      key.setType( Key::X509 );
    else if ( line.parameter( "type" ).lower() == "pgp" )
      key.setType( Key::PGP );
    else {
      key.setType( Key::Custom );
      key.setCustomTypeString( line.parameter( "type" ) );
    }
  }

  return key;
}

VCardLine VCardTool::createKey( const Key &key )
{
  VCardLine line( "KEY" );

  if ( key.isBinary() ) {
    if ( !key.binaryData().isEmpty() ) {
      line.setValueBytes( key.binaryData() );
      line.addParameter( "encoding", "b" );
    }
  } else if ( !key.textData().isEmpty() )
    line.setValueString( key.textData() );

  if ( key.type() == Key::X509 )
    line.addParameter( "type", "X509" );
  else if ( key.type() == Key::PGP )
    line.addParameter( "type", "PGP" );
  else if ( key.type() == Key::Custom )
    line.addParameter( "type", key.customTypeString() );

  return line;
}

Secrecy VCardTool::parseSecrecy( const VCardLine &line )
{
  Secrecy secrecy;

  if ( line.valueString().lower() == "public" )
    secrecy.setType( Secrecy::Public );
  if ( line.valueString().lower() == "private" )
    secrecy.setType( Secrecy::Private );
  if ( line.valueString().lower() == "confidential" )
    secrecy.setType( Secrecy::Confidential );

  return secrecy;
}

VCardLine VCardTool::createSecrecy( const Secrecy &secrecy )
{
  VCardLine line( "CLASS" );

  int type = secrecy.type();

  if ( type == Secrecy::Public )
    line.setValueString( "PUBLIC" );
  else if ( type == Secrecy::Private )
    line.setValueString( "PRIVATE" );
  else if ( type == Secrecy::Confidential )
    line.setValueString( "CONFIDENTIAL" );

  return line;
}

Agent VCardTool::parseAgent( const VCardLine &line )
{
  Agent agent;

  const QStringList params = line.parameterList();
  if ( params.findIndex( "value" ) != -1 ) {
    if ( line.parameter( "value" ).lower() == "uri" )
      agent.setUrl( line.valueString() );
  } else {
    QString str = line.valueString();
    str.replace( QRegExp("\\\\n"), "\r\n" );
    str.replace( QRegExp("\\\\N"), "\r\n" );
    str.replace( QRegExp("\\\\;"), ";" );
    str.replace( QRegExp("\\\\:"), ":" );
    str.replace( QRegExp("\\\\,"), "," );

    const Addressee::List list = parseVCards( str );
    if ( list.count() > 0 ) {
      Addressee *addr = new Addressee;
      *addr = list[ 0 ];
      agent.setAddressee( addr );
    }
  }

  return agent;
}

VCardLine VCardTool::createAgent( VCard::Version version, const Agent &agent )
{
  VCardLine line( "AGENT" );

  if ( agent.isIntern() ) {
    if ( agent.addressee() != 0 ) {
      Addressee::List list;
      list.append( *agent.addressee() );

      QString str = createVCards( list, version );
      str.replace( QRegExp("\\r\\n"), "\\n" );
      str.replace( QRegExp(";"), "\\;" );
      str.replace( QRegExp(":"), "\\:" );
      str.replace( QRegExp(","), "\\," );
      line.setValueString( str );
    }
  } else if ( !agent.url().isEmpty() ) {
    line.setValueString( agent.url() );
    line.addParameter( "value", "URI" );
  }

  return line;
}

QStringList VCardTool::splitString( const QChar &sep, const QString &str )
{
  QStringList list;
  QString value( str );

  int start = 0;
  int pos = value.find( sep, start );

  while ( pos != -1 ) {
    if ( value[ pos - 1 ] != '\\' ) {
      if ( pos > start && pos <= (int)value.length() )
        list << value.mid( start, pos - start );
      else
        list << QString::null;

      start = pos + 1;
      pos = value.find( sep, start );
    } else {
      if ( pos != 0 ) {
        value.replace( pos - 1, 2, sep );
        pos = value.find( sep, pos );
      } else
        pos = value.find( sep, pos + 1 );
    }
  }

  int l = value.length() - 1;
  if ( value.mid( start, l - start + 1 ).length() > 0 )
    list << value.mid( start, l - start + 1 );
  else
    list << QString::null;

  return list;
}
