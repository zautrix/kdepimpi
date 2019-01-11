/*** Warning! This file has been generated by the script makeaddressee ***/
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

$Id: addressee.h,v 1.25 2006/02/24 18:49:56 zautrix Exp $
*/

#ifndef KABC_ADDRESSEE_H
#define KABC_ADDRESSEE_H

#include <qdatetime.h>
#include <qstring.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include <ksharedptr.h>
#include <kurl.h>

#include "address.h"
#include "agent.h"
#include "geo.h"
#include "key.h"
#include "phonenumber.h"
#include "picture.h"
#include "secrecy.h"
#include "sound.h"
#include "timezone.h"

namespace KABC {

class Resource;

/**
  @short address book entry

  This class represents an entry in the address book.

  The data of this class is implicitly shared. You can pass this class by value.

  If you need the name of a field for presenting it to the user you should use
  the functions ending in Label(). They return a translated string which can be
  used as label for the corresponding field.

  About the name fields:

  givenName() is the first name and familyName() the last name. In some
  countries the family name comes first, that's the reason for the
  naming. formattedName() is the full name with the correct formatting.
  It is used as an override, when the correct formatting can't be generated
  from the other name fields automatically.

  realName() returns a fully formatted name(). It uses formattedName, if set,
  otherwise it constucts the name from the name fields. As fallback, if
  nothing else is set it uses name().

  name() is the NAME type of RFC2426. It can be used as internal name for the
  data enty, but shouldn't be used for displaying the data to the user.
 */
class Addressee
{
    friend QDataStream &operator<<( QDataStream &, const Addressee & );
    friend QDataStream &operator>>( QDataStream &, Addressee & );

  public:
    typedef QValueList<Addressee> List;

    /**
      Construct an empty address book entry.
     */
    Addressee();
    ~Addressee();

    Addressee( const Addressee & );
    Addressee &operator=( const Addressee & );

    bool operator==( const Addressee & ) const;
    bool operator!=( const Addressee & ) const;
    // sync stuff
    void setTempSyncStat(int id);
    int tempSyncStat() const;
    void setIDStr( const QString & );
    const QString IDStr() const;
    void setID( const QString &, const QString & );
    const QString  getID( const QString & ) const;
    void setCsum( const QString &, const QString & );
    const QString getCsum( const QString & ) const ;
    void removeID(const QString &);
    void computeCsum(const QString &dev);
    ulong getCsum4List( const QStringList &  attList);
    /**
      Return, if the address book entry is empty.
     */
    bool isEmpty() const;
    void setExternalUID( const QString &id );
    const QString externalUID() const;
    void setOriginalExternalUID( const QString &id );
    QString originalExternalUID() const;
    void mergeContact(  const Addressee& ad, bool isSubSet ); 
    void mergeOLContact(  const Addressee& ad ); 
    void simplifyEmails();
    void simplifyAddresses();
    void simplifyPhoneNumbers();
    void simplifyPhoneNumberTypes();
    void makePhoneNumbersOLcompatible();
    int hasPhoneNumberType( int type );
    bool removeVoice();
    bool containsAdr(const Addressee& addr );
    
    /**
      Set unique identifier.
     */
    void setUid( const QString &uid );
    /**
      Return unique identifier.
     */
    const QString uid() const;
    /**
      Return translated label for uid field.
     */
    static QString uidLabel();

    /**
      Set name.
     */
    void setName( const QString &name );
    /**
      Return name.
     */
    QString name() const;
    /**
      Return translated label for name field.
     */
    static QString nameLabel();

    /**
      Set formatted name.
     */
    void setFormattedName( const QString &formattedName );
    /**
      Return formatted name.
     */
    QString formattedName() const;
    /**
      Return translated label for formattedName field.
     */
    static QString formattedNameLabel();

    /**
      Set family name.
     */
    void setFamilyName( const QString &familyName );
    /**
      Return family name.
     */
    QString familyName() const;
    /**
      Return translated label for familyName field.
     */
    static QString familyNameLabel();

    /**
      Set given name.
     */
    void setGivenName( const QString &givenName );
    /**
      Return given name.
     */
    QString givenName() const;
    /**
      Return translated label for givenName field.
     */
    static QString givenNameLabel();

    /**
      Set additional names.
     */
    void setAdditionalName( const QString &additionalName );
    /**
      Return additional names.
     */
    QString additionalName() const;
    /**
      Return translated label for additionalName field.
     */
    static QString additionalNameLabel();

    /**
      Set honorific prefixes.
     */
    void setPrefix( const QString &prefix );
    /**
      Return honorific prefixes.
     */
    QString prefix() const;
    /**
      Return translated label for prefix field.
     */
    static QString prefixLabel();

    /**
      Set honorific suffixes.
     */
    void setSuffix( const QString &suffix );
    /**
      Return honorific suffixes.
     */
    QString suffix() const;
    /**
      Return translated label for suffix field.
     */
    static QString suffixLabel();

    /**
      Set nick name.
     */
    void setNickName( const QString &nickName );
    /**
      Return nick name.
     */
    QString nickName() const;
    /**
      Return translated label for nickName field.
     */
    static QString nickNameLabel();

    /**
      Set birthday.
     */
    void setBirthday( const QDateTime &birthday );
    /**
      Return birthday.
     */
    QDateTime birthday() const;
    /**
      Return translated label for birthday field.
     */
    static QString birthdayLabel();

    /**
      Return translated label for homeAddressStreet field.
     */
    static QString homeAddressStreetLabel();

    /**
      Return translated label for homeAddressLocality field.
     */
    static QString homeAddressLocalityLabel();

    /**
      Return translated label for homeAddressRegion field.
     */
    static QString homeAddressRegionLabel();

    /**
      Return translated label for homeAddressPostalCode field.
     */
    static QString homeAddressPostalCodeLabel();

    /**
      Return translated label for homeAddressCountry field.
     */
    static QString homeAddressCountryLabel();

    /**
      Return translated label for homeAddressLabel field.
     */
    static QString homeAddressLabelLabel();

    /**
      Return translated label for businessAddressStreet field.
     */
    static QString businessAddressStreetLabel();

    /**
      Return translated label for businessAddressLocality field.
     */
    static QString businessAddressLocalityLabel();

    /**
      Return translated label for businessAddressRegion field.
     */
    static QString businessAddressRegionLabel();

    /**
      Return translated label for businessAddressPostalCode field.
     */
    static QString businessAddressPostalCodeLabel();

    /**
      Return translated label for businessAddressCountry field.
     */
    static QString businessAddressCountryLabel();

    /**
      Return translated label for businessAddressLabel field.
     */
    static QString businessAddressLabelLabel();

    /**
      Return translated label for homePhone field.
     */
    static QString homePhoneLabel();

    /**
      Return translated label for businessPhone field.
     */
    static QString businessPhoneLabel();

    /**
      Return translated label for mobilePhone field.
     */
    static QString mobilePhoneLabel();
    static QString mobileWorkPhoneLabel();

    /**
      Return translated label for homeFax field.
     */
    static QString homeFaxLabel();

    /**
      Return translated label for businessFax field.
     */
    static QString businessFaxLabel();

  
    /**
      Return translated label for isdn field.
     */
    static QString isdnLabel();

    /**
      Return translated label for pager field.
     */
    static QString pagerLabel();
    static QString otherPhoneLabel();
    /**
      Return translated label for sip field.
     */
    static QString sipLabel();

    /**
      Return translated label for email field.
     */
    static QString emailLabel();

    /**
      Set mail client.
     */
    void setMailer( const QString &mailer );
    /**
      Return mail client.
     */
    QString mailer() const;
    /**
      Return translated label for mailer field.
     */
    static QString mailerLabel();

    /**
      Set time zone.
     */
    void setTimeZone( const TimeZone &timeZone );
    /**
      Return time zone.
     */
    TimeZone timeZone() const;
    /**
      Return translated label for timeZone field.
     */
    static QString timeZoneLabel();

    /**
      Set geographic position.
     */
    void setGeo( const Geo &geo );
    /**
      Return geographic position.
     */
    Geo geo() const;
    /**
      Return translated label for geo field.
     */
    static QString geoLabel();

    /**
      Set title.
     */
    void setTitle( const QString &title );
    /**
      Return title.
     */
    QString title() const;
    /**
      Return translated label for title field.
     */
    static QString titleLabel();

    /**
      Set role.
     */
    void setRole( const QString &role );
    /**
      Return role.
     */
    QString role() const;
    /**
      Return translated label for role field.
     */
    static QString roleLabel();

    /**
      Set organization.
     */
    void setOrganization( const QString &organization );
    /**
      Return organization.
     */
    QString organization() const;
    /**
      Return translated label for organization field.
     */
    static QString organizationLabel();

    /**
      Set note.
     */
    void setNote( const QString &note );
    /**
      Return note.
     */
    QString note() const;
    /**
      Return translated label for note field.
     */
    static QString noteLabel();

    /**
      Set product identifier.
     */
    void setProductId( const QString &productId );
    /**
      Return product identifier.
     */
    QString productId() const;
    /**
      Return translated label for productId field.
     */
    static QString productIdLabel();

    /**
      Set revision date.
     */
    void setRevision( const QDateTime &revision );
    /**
      Return revision date.
     */
    QDateTime revision() const;
    /**
      Return translated label for revision field.
     */
    static QString revisionLabel();

    /**
      Set sort string.
     */
    void setSortString( const QString &sortString );
    /**
      Return sort string.
     */
    QString sortString() const;
    /**
      Return translated label for sortString field.
     */
    static QString sortStringLabel();

    /**
      Set URL.
     */
    void setUrl( const KURL &url );
    /**
      Return URL.
     */
    KURL url() const;
    /**
      Return translated label for url field.
     */
    static QString urlLabel();

    /**
      Set security class.
     */
    void setSecrecy( const Secrecy &secrecy );
    /**
      Return security class.
     */
    Secrecy secrecy() const;
    /**
      Return translated label for secrecy field.
     */
    static QString secrecyLabel();

    /**
      Set logo.
     */
    void setLogo( const Picture &logo );
    /**
      Return logo.
     */
    Picture logo() const;
    /**
      Return translated label for logo field.
     */
    static QString logoLabel();

    /**
      Set photo.
     */
    void setPhoto( const Picture &photo );
    /**
      Return photo.
     */
    Picture photo() const;
    /**
      Return translated label for photo field.
     */
    static QString photoLabel();

    /**
      Set sound.
     */
    void setSound( const Sound &sound );
    /**
      Return sound.
     */
    Sound sound() const;
    /**
      Return translated label for sound field.
     */
    static QString soundLabel();

    /**
      Set agent.
     */
    void setAgent( const Agent &agent );
    /**
      Return agent.
     */
    Agent agent() const;
    /**
      Return translated label for agent field.
     */
    static QString agentLabel();

    /**
      Set name fields by parsing the given string and trying to associate the
      parts of the string with according fields. This function should probably
      be a bit more clever.
     */
    void setNameFromString( const QString & );

    /**
      Return the name of the addressee. This is calculated from all the name
      fields.
     */
    QString realName() const;

    /**
      Return the name that consists of all name parts.
     */
    QString assembledName() const;

    /**
      Return email address including real name.

      @param email Email address to be used to construct the full email string.
                   If this is QString::null the preferred email address is used.
     */
    QString fullEmail( const QString &email=QString::null ) const;

    /**
      Insert an email address. If the email address already exists in this
      addressee it is not duplicated.

      @param email Email address
      @param preferred Set to true, if this is the preferred email address of
                       the addressee.
     */
    void insertEmail( const QString &email, bool preferred=false );

    /**
      Remove email address. If the email address doesn't exist, nothing happens.
     */
    void removeEmail( const QString &email );

    /**
      Return preferred email address. This is the first email address or the
      last one added with @ref insertEmail() with a set preferred parameter.
     */
    QString preferredEmail() const;

    /**
      Return list of all email addresses.
     */
    QStringList emails() const;

    /**
       Set the emails to @param.
       The first email address gets the preferred one!
       @param list The list of email addresses.
     */
    void setEmails( const QStringList& list);

    /**
      Insert a phone number. If a phone number with the same id already exists
      in this addressee it is not duplicated.
     */
    void insertPhoneNumber( const PhoneNumber &phoneNumber );

    /**
      Remove phone number. If no phone number with the given id exists for this
      addresse nothing happens.
     */
    void removePhoneNumber( const PhoneNumber &phoneNumber );

    /**
      Return phone number, which matches the given type.
     */
    PhoneNumber phoneNumber( int type ) const;
    QString phoneNumberString( int type ) const;

    bool matchPhoneNumber( QRegExp* searchExp ) const;
    bool matchAddress( QRegExp* searchExp ) const;

    /**
      Return list of all phone numbers.
     */
    PhoneNumber::List phoneNumbers() const;

    /**
      Return list of phone numbers with a special type.
     */
    PhoneNumber::List phoneNumbers( int type ) const;

    /**
      Return phone number with the given id.
     */
    PhoneNumber findPhoneNumber( const QString &id ) const;

    /**
      Insert a key. If a key with the same id already exists
      in this addressee it is not duplicated.
     */
    void insertKey( const Key &key );

    /**
      Remove a key. If no key with the given id exists for this
      addresse nothing happens.
     */
    void removeKey( const Key &key );

    /**
      Return key, which matches the given type.
      If @p type == Key::Custom you can specify a string
      that should match. If you leave the string empty, the first
      key with a custom value is returned.
     */
    Key key( int type, QString customTypeString = QString::null ) const;

    /**
      Return list of all keys.
     */
    Key::List keys() const;

    /**
       Set the list of keys
       @param keys The keys to be set.
     */
    void setKeys( const Key::List& keys);

    /**
      Return list of keys with a special type.
      If @p type == Key::Custom you can specify a string
      that should match. If you leave the string empty, all custom
      keys will be returned.
     */
    Key::List keys( int type, QString customTypeString = QString::null  ) const;

    /**
      Return key with the given id.
     */
    Key findKey( const QString &id ) const;

    /**
      Insert an address. If an address with the same id already exists
      in this addressee it is not duplicated.
     */
    void insertAddress( const Address &address );

    /**
      Remove address. If no address with the given id exists for this
      addresse nothing happens.
     */
    void removeAddress( const Address &address );

    /**
      Return address, which matches the given type.
     */
    Address address( int type ) const;

    /**
      Return list of all addresses.
     */
    Address::List addresses() const;
    Address otherAddress() const;
    /**
      Return list of addresses with a special type.
     */
    Address::List addresses( int type ) const;

    /**
      Return address with the given id.
     */
    Address findAddress( const QString &id ) const;

    /**
      Insert category. If the category already exists it is not duplicated.
     */
    void insertCategory( const QString & );

    /**
      Remove category.
     */
    void removeCategory( const QString & );

    /**
      Return, if addressee has the given category.
     */
    bool hasCategory( const QString & ) const;

    /**
      Set categories to given value.
     */
    void setCategories( const QStringList & );

    /**
      Return list of all set categories.
     */
    QStringList categories() const;

    /**
      Insert custom entry. The entry is identified by the name of the inserting
      application and a unique name. If an entry with the given app and name
      already exists its value is replaced with the new given value.
     */
    void insertCustom( const QString &app, const QString &name,
                       const QString &value );

    /**
      Remove custom entry.
     */
    void removeCustom( const QString &app, const QString &name );

    /**
      Return value of custom entry, identified by app and entry name.
     */
    QString custom( const QString &app, const QString &name ) const;

    /**
      Set all custom entries.
     */
    void setCustoms( const QStringList & );

    /**
      Return list of all custom entries.
     */
    QStringList customs() const;

    /**
      Parse full email address. The result is given back in fullName and email.
     */
    static void parseEmailAddress( const QString &rawEmail, QString &fullName,
                                   QString &email );

    /**
      Debug output.
     */
    void dump() const;

    /**
      Returns string representation of the addressee.
     */
    QString asString() const;

    /**
      Set resource where the addressee is from.
     */
    void setResource( Resource *resource );

    /**
      Return pointer to resource.
     */
    Resource *resource() const;

    /**
      Return resourcelabel.
     */
    //US
    static QString resourceLabel();
    static QString categoryLabel();
    /**
	    Mark addressee as changed.
	   */
    void setChanged( bool value );

  	/**
	    Return whether the addressee is changed.
	   */
    bool changed() const;

    void setTagged( bool value );
    bool tagged() const;

  private:
    Addressee copy();
    void detach();

    struct AddresseeData;
    mutable KSharedPtr<AddresseeData> mData;
};

QDataStream &operator<<( QDataStream &, const Addressee & );
QDataStream &operator>>( QDataStream &, Addressee & );

}

#endif