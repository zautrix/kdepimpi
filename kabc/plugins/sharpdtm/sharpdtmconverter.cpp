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

$Id: sharpdtmconverter.cpp,v 1.12 2005/10/28 03:41:02 zautrix Exp $
*/

#include "kglobal.h"
#include <qregexp.h>


#include "sharpdtmconverter.h"

#include <sl/slcategories.h>
#include <libkdepim/ksyncprofile.h>
//US #include <qpe/categoryselect.h>


using namespace KABC;
using namespace SlCategory;

SharpDTMConverter::SharpDTMConverter() : catDB(0)
{
}

SharpDTMConverter::~SharpDTMConverter()
{
  deinit();
}

bool SharpDTMConverter::init()
{
  catDB = new SlCategory::SlCategories();

  if (!catDB)
    return false;

//  catDB->load( categoryFileName() );
  return true;
}

void SharpDTMConverter::deinit()
{
  if (catDB)
  {
    delete catDB;
    catDB = 0;
  }
}

bool SharpDTMConverter::sharpToAddressee( const CardId &contact, const SlZDataBase* database, Addressee &addr )
{
	SlZDataBase* db = (SlZDataBase*)database;

    // for syncing: we need setting of the two fields
    addr.setExternalUID( QString::number( contact ) );
    addr.setOriginalExternalUID( QString::number( contact ) ); 
    addr.setTempSyncStat(  SYNC_TEMPSTATE_NEW_EXTERNAL );


    // name
    //qDebug("SharpDTMConverter::sharpToAddressee check if the fileAs transformation works!!");
    addr.setFormattedName(db->readField(ZdbAdrs::FileAs)); 
    //addr.setName(db->readField(ZdbAdrs::FullName));

    addr.setFamilyName( db->readField(ZdbAdrs::LastName) );
    addr.setGivenName( db->readField(ZdbAdrs::FirstName) );
    addr.setAdditionalName( db->readField(ZdbAdrs::MiddleName) );
    addr.setPrefix( db->readField(ZdbAdrs::Title) );
    addr.setSuffix( db->readField(ZdbAdrs::Suffix) );


    QString emailstr = db->readField(ZdbAdrs::Emails);
    emailstr.replace( QRegExp(","), " " ); 
    emailstr.replace( QRegExp(";"), " " ); 
    emailstr.replace( QRegExp(":"), " " ); 
    //qDebug("SharpDTMConverter::sharpToAddressee whats the character to seperate the emailadresses? %s ", emailstr.latin1());
    QStringList emails = QStringList::split(" ", emailstr.simplifyWhiteSpace());
    bool defE = false;
    bool found = false;
    for ( QStringList::Iterator it = emails.begin(); it != emails.end(); ++it ) {
        if (found )
            defE = false;
        else
            found = defE = ((*it).lower() == db->readField(ZdbAdrs::DefaultEmail).lower());
      addr.insertEmail( *it, defE );
    }
    if ( ! found )
        if (!db->readField(ZdbAdrs::DefaultEmail).isEmpty())
            addr.insertEmail(db->readField(ZdbAdrs::DefaultEmail), true);

    // home
    if ((!db->readField(ZdbAdrs::HomeStreet).isEmpty()) ||
        (!db->readField(ZdbAdrs::HomeCity).isEmpty()) ||
        (!db->readField(ZdbAdrs::HomeState).isEmpty()) ||
        (!db->readField(ZdbAdrs::HomeZip).isEmpty()) ||
        (!db->readField(ZdbAdrs::HomeCountry).isEmpty()))
    {
      Address homeaddress;
      homeaddress.setType(Address::Home);
//US    homeaddress.setPostOfficeBox( "" );
//US    homeaddress.setExtended( "" );
      homeaddress.setStreet( db->readField(ZdbAdrs::HomeStreet).replace( QRegExp("\\r"), ""));
      homeaddress.setLocality( db->readField(ZdbAdrs::HomeCity) );
      homeaddress.setRegion( db->readField(ZdbAdrs::HomeState) );
      homeaddress.setPostalCode( db->readField(ZdbAdrs::HomeZip) );
      homeaddress.setCountry( db->readField(ZdbAdrs::HomeCountry) );

      addr.insertAddress( homeaddress );
    }

    if (!db->readField(ZdbAdrs::HomePhone).isEmpty())
    {
      PhoneNumber homephone;
      homephone.setType( PhoneNumber::Home | PhoneNumber::Pref );
      homephone.setNumber( db->readField(ZdbAdrs::HomePhone) );
      addr.insertPhoneNumber( homephone );
    }

    if (!db->readField(ZdbAdrs::HomeFax).isEmpty())
    {
      PhoneNumber homefax;
      homefax.setType( PhoneNumber::Home | PhoneNumber::Fax );
      homefax.setNumber( db->readField(ZdbAdrs::HomeFax) );
      addr.insertPhoneNumber( homefax );
    }

    if (!db->readField(ZdbAdrs::HomeMobile).isEmpty())
    {
      PhoneNumber homemobile;
      homemobile.setType( PhoneNumber::Cell );
      homemobile.setNumber( db->readField(ZdbAdrs::HomeMobile) );
      addr.insertPhoneNumber( homemobile );
    }

    addr.setUrl( db->readField(ZdbAdrs::HomeWebPage) );


    // business
    if ((!db->readField(ZdbAdrs::BusinessStreet).isEmpty()) ||
        (!db->readField(ZdbAdrs::BusinessCity).isEmpty()) ||
        (!db->readField(ZdbAdrs::BusinessState).isEmpty()) ||
        (!db->readField(ZdbAdrs::BusinessZip).isEmpty()) ||
        (!db->readField(ZdbAdrs::BusinessCountry).isEmpty()))
    {
      Address businessaddress;
      businessaddress.setType(Address::Work);
//US    businessaddress.setPostOfficeBox( "" );
//US    businessaddress.setExtended( "" );
      businessaddress.setStreet( db->readField(ZdbAdrs::BusinessStreet).replace( QRegExp("\\r"), "") );
      businessaddress.setLocality( db->readField(ZdbAdrs::BusinessCity) );
      businessaddress.setRegion( db->readField(ZdbAdrs::BusinessState) );
      businessaddress.setPostalCode( db->readField(ZdbAdrs::BusinessZip) );
      businessaddress.setCountry( db->readField(ZdbAdrs::BusinessCountry) );

      addr.insertAddress( businessaddress );
    }


    if (!db->readField(ZdbAdrs::BusinessPhone).isEmpty())
    {
      PhoneNumber businessphone;
      businessphone.setType( PhoneNumber::Work | PhoneNumber::Pref );
      businessphone.setNumber( db->readField(ZdbAdrs::BusinessPhone) );
      addr.insertPhoneNumber( businessphone );
    }

    if (!db->readField(ZdbAdrs::BusinessFax).isEmpty())
    {
      PhoneNumber businessfax;
      businessfax.setType( PhoneNumber::Work | PhoneNumber::Fax );
      businessfax.setNumber( db->readField(ZdbAdrs::BusinessFax) );
      addr.insertPhoneNumber( businessfax );
    }

    if (!db->readField(ZdbAdrs::BusinessMobile).isEmpty())
    {
      PhoneNumber businessmobile;
      businessmobile.setType( PhoneNumber::Car );
      businessmobile.setNumber( db->readField(ZdbAdrs::BusinessMobile) );
      addr.insertPhoneNumber( businessmobile );
    }

    if (!db->readField(ZdbAdrs::BusinessPager).isEmpty())
    {
      PhoneNumber businesspager;
      businesspager.setType(  PhoneNumber::Pager );
      businesspager.setNumber( db->readField(ZdbAdrs::BusinessPager) );
      addr.insertPhoneNumber( businesspager );
    }

    addr.setRole( db->readField(ZdbAdrs::JobTitle) );
    addr.setOrganization( db->readField(ZdbAdrs::Company) );
    addr.insertCustom( "KADDRESSBOOK", "X-Profession", db->readField(ZdbAdrs::Profession) );
    addr.insertCustom( "KADDRESSBOOK", "X-AssistantsName", db->readField(ZdbAdrs::Assistant) );
    addr.insertCustom( "KADDRESSBOOK", "X-Department", db->readField(ZdbAdrs::Department) );
    addr.insertCustom( "KADDRESSBOOK", "X-ManagersName", db->readField(ZdbAdrs::Manager) );
    addr.insertCustom( "KADDRESSBOOK", "X-Office", db->readField(ZdbAdrs::Office) );

    //personal
    addr.insertCustom( "KADDRESSBOOK", "X-SpousesName", db->readField(ZdbAdrs::Spouse) );

	QString gen = db->readField(ZdbAdrs::Gender);
	//qDebug("SharpDTMConverter::sharpToAddressee pleas check that gender works!! : Gender: %s", gen.latin1());
	//qDebug("SharpDTMConverter::sharpToAddressee: may be int db->readUshortField(\"ZdbAdrs::Gender\") is here better suited");
    if (gen == "1")
      addr.insertCustom( "KADDRESSBOOK", "X-Gender", "male");
    else if (gen == "2")
      addr.insertCustom( "KADDRESSBOOK", "X-Gender", "female");
    else 
        addr.insertCustom( "KADDRESSBOOK", "X-Gender", "undef");


    QDate ann = KGlobal::locale()->readDate( db->readField(ZdbAdrs::Anniversary) ); 
    if (ann.isValid()) {
      QString dt = KGlobal::locale()->formatDate(ann, true, KLocale::ISODate);
      //qDebug("qtopiaToAddressee annyversary found:%s", dt.latin1());
      addr.insertCustom( "KADDRESSBOOK", "X-Anniversary", dt);
    } else
        addr.insertCustom( "KADDRESSBOOK", "X-Anniversary", " ");
        
  

    addr.insertCustom( "KADDRESSBOOK", "X-Children", db->readField(ZdbAdrs::Children) );


    QDate birthd = KGlobal::locale()->readDate( db->readField(ZdbAdrs::Birthday) );
    //qDebug("birtd %s ", birthd.toString().latin1());
    if (birthd.isValid())
      addr.setBirthday( birthd );

    addr.setNickName( db->readField(ZdbAdrs::Nickname) );

    // others
    //US I put opies BusinessWebPage into Ka/Pi's notes block, because no other native field is available.
    //QString notes = db->readField(ZdbAdrs::Notes);
    //notes += "\nBusinessWebPage: " + db->readField(ZdbAdrs::BusinessWebPage) + "\n";
    if ( addr.url().isEmpty() )
        addr.setUrl( db->readField(ZdbAdrs::BusinessWebPage) );
    addr.setNote( db->readField(ZdbAdrs::Notes).replace( QRegExp("\\r"), ""));



//US    QString groups() const { return find( Qtopia::Groups ); }
//US    QStringList groupList() const;

	//qDebug("SharpDTMConverter::sharpToAddressee please check that the categories will be converted" );
    QArray<int> catArray = db->readCategories();
    QString cat;

    for ( unsigned int i=0; i < catArray.size(); i++ ) {
      cat = catDB->label(catArray[i]);
      if ( cat.isEmpty() )
        addr.insertCategory(QString::number(catArray[i]));
      else
        addr.insertCategory( cat );
    }

    return true;
}

bool SharpDTMConverter::addresseeToSharp( const Addressee &addr, SlZDataBase* database , const CardId &contact )
{
    bool cellHome = false, cellWork = false;
    // name
    database->writeField(ZdbAdrs::LastName, addr.familyName());
    database->writeField(ZdbAdrs::FirstName, addr.givenName());
    database->writeField(ZdbAdrs::MiddleName, addr.additionalName());
    database->writeField(ZdbAdrs::Title, addr.prefix());
    database->writeField(ZdbAdrs::Suffix, addr.suffix());

    //qDebug("SharpDTMConverter::addresseeToSharp check if the fileAs transformation works!!\n%s",addr.formattedName().latin1() );
    QString formattedName = addr.formattedName();
    if ( formattedName.isEmpty() ) {
        if ( !addr.familyName().isEmpty() ) {
            formattedName = addr.familyName();
            if ( !addr.givenName().isEmpty() ) {
                formattedName += ", ";
                formattedName += addr.givenName();
            }
        } else
            formattedName = addr.givenName();
    }
    database->writeField(ZdbAdrs::FileAs, formattedName);
    database->writeField(ZdbAdrs::FullName, formattedName);

    // email
    //qDebug("SharpDTMConverter::addresseeToSharp check which seperator we need here for the emails!!");
    //qDebug("SharpDTMConverter::addresseeToSharp its probably the same from sharpToAddressee");
    QString emails = addr.emails().join(" ");
    database->writeField(ZdbAdrs::Emails, emails );

    database->writeField(ZdbAdrs::DefaultEmail, addr.preferredEmail() );
    // home
    const Address homeaddress = addr.address(Address::Home);
    database->writeField(ZdbAdrs::HomeStreet, homeaddress.street());
    database->writeField(ZdbAdrs::HomeCity, homeaddress.locality());
    database->writeField(ZdbAdrs::HomeState, homeaddress.region());
    database->writeField(ZdbAdrs::HomeZip, homeaddress.postalCode());
    database->writeField(ZdbAdrs::HomeCountry, homeaddress.country());
    
    PhoneNumber homephone = addr.phoneNumber( PhoneNumber::Home );
    database->writeField(ZdbAdrs::HomePhone, homephone.number());
    PhoneNumber homefax = addr.phoneNumber( PhoneNumber::Home | PhoneNumber::Fax );
    database->writeField(ZdbAdrs::HomeFax, homefax.number());
    PhoneNumber homemobile = addr.phoneNumber( PhoneNumber::Home | PhoneNumber::Cell );
    database->writeField(ZdbAdrs::HomeMobile, homemobile.number());
    if (!homemobile.number().isEmpty()) {
      cellHome = true;
    }
    database->writeField(ZdbAdrs::HomeWebPage, addr.url().url());
    // business
    const Address businessaddress = addr.address(Address::Work);
        //qDebug("write business address ");
    database->writeField(ZdbAdrs::BusinessStreet, businessaddress.street());
    database->writeField(ZdbAdrs::BusinessCity, businessaddress.locality());
    database->writeField(ZdbAdrs::BusinessState, businessaddress.region());
    database->writeField(ZdbAdrs::BusinessZip, businessaddress.postalCode());
    database->writeField(ZdbAdrs::BusinessCountry, businessaddress.country());
    
    PhoneNumber businessphone = addr.phoneNumber( PhoneNumber::Work );
    database->writeField(ZdbAdrs::BusinessPhone, businessphone.number());

    PhoneNumber businessfax = addr.phoneNumber( PhoneNumber::Work | PhoneNumber::Fax );
    database->writeField(ZdbAdrs::BusinessFax, businessfax.number());

    PhoneNumber businessmobile = addr.phoneNumber( PhoneNumber::Work | PhoneNumber::Cell );
    database->writeField(ZdbAdrs::BusinessMobile, businessmobile.number());
    if (!businessmobile.number().isEmpty()) {
        cellWork = true;
    }
    PhoneNumber mobile = addr.phoneNumber( PhoneNumber::Cell );
    if (! mobile.number().isEmpty()) {
        if ( ! cellHome ) 
            database->writeField(ZdbAdrs::HomeMobile, mobile.number());
        else if (! cellWork )
            database->writeField(ZdbAdrs::BusinessMobile, mobile.number());
    }

    PhoneNumber businesspager = addr.phoneNumber( PhoneNumber::Work | PhoneNumber::Pager );
    database->writeField(ZdbAdrs::BusinessPager, businesspager.number());

    database->writeField(ZdbAdrs::JobTitle, addr.role());
    database->writeField(ZdbAdrs::Company, addr.organization());

    database->writeField(ZdbAdrs::Profession, addr.custom( "KADDRESSBOOK", "X-Profession" ));
    database->writeField(ZdbAdrs::Assistant, addr.custom( "KADDRESSBOOK", "X-AssistantsName" ));
    database->writeField(ZdbAdrs::Department, addr.custom( "KADDRESSBOOK", "X-Department" ));
    database->writeField(ZdbAdrs::Manager, addr.custom( "KADDRESSBOOK", "X-ManagersName" ));
    database->writeField(ZdbAdrs::Office, addr.custom( "KADDRESSBOOK", "X-Office" ));

    //personal
    database->writeField(ZdbAdrs::Spouse, addr.custom( "KADDRESSBOOK", "X-Spouse" ));

    QString gt = addr.custom( "KADDRESSBOOK", "X-Gender" );
	//qDebug("SharpDTMConverter::addresseeToSharp please check that gender works!! : Gender: %s", gt.latin1());
	//qDebug("SharpDTMConverter::addresseeToSharp: may be writeField(\"ZdbAdrs::Gender\", ushort) is here better suited?");
	//qDebug("SharpDTMConverter::addresseeToSharp: check also the reverse functionality in sharpToAddressee");
    if (gt == "male")
      database->writeField(ZdbAdrs::Gender, "1");
    else if (gt == "female")
      database->writeField(ZdbAdrs::Gender, "2");
    else
      database->writeField(ZdbAdrs::Gender, "");

    QString dateS ;
    QDate dt = KGlobal::locale()->readDate(addr.custom("KADDRESSBOOK", "X-Anniversary" ), "%Y-%m-%d"); // = Qt::ISODate
    if ( dt.isValid() ) {
        dateS =  KGlobal::locale()->formatDate(dt, true  );
    }
    database->writeField(ZdbAdrs::Anniversary, dateS );
    database->writeField(ZdbAdrs::Children, addr.custom( "KADDRESSBOOK", "X-Children" ));
    dt = addr.birthday().date(); 
    dateS = ""; 
    if ( dt.isValid() ) {
        dateS =  KGlobal::locale()->formatDate(dt, true );
    }
    database->writeField(ZdbAdrs::Birthday, dateS);
    database->writeField(ZdbAdrs::Nickname, addr.nickName());

    // other
    database->writeField(ZdbAdrs::Notes, addr.note());

//US    QString groups() const { return find( Qtopia::Groups ); }
//US    QStringList groupList() const; 


	//qDebug("SharpDTMConverter::addresseeToSharp please check if category transformation works");

    return true;
}


bool SharpDTMConverter::setCategories( const Addressee &addr, SlZDataBase* database , const CardId &contact )
{  
    QStringList list = addr.categories();

	QArray<int> ids(list.count());
	uint index = 0;
	for(uint i=0; i<ids.size(); i++){
		if(catDB->exists(list[i])){
			ids[index] = catDB->id(list[i]);
            //qDebug("set exist cat %d %s ",ids[index] , list[i].latin1());
			index++;
		} else {
			ids[index] = catDB->addCategory(list[i]);
            //qDebug("add new cat %d %s ",ids[index] , list[i].latin1());
			index++;
		}
	}
    bool res ;

    if ( !(res = database->updateCategories(contact, ids) ))
        qDebug("SharpDTMConverter::Error updating categories");

    return res;
}

QDate SharpDTMConverter::convertDate( QString  s)
{
    QDate dt = KGlobal::locale()->readDate( s ); 
    return dt;
    return QDate ();
}
