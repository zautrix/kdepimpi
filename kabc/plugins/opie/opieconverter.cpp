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

$Id: opieconverter.cpp,v 1.1 2004/07/26 22:10:28 ulf69 Exp $
*/

//US
#include "kglobal.h"


#include "opieconverter.h"

#include <qpe/categories.h>
#include <qpe/categoryselect.h>


using namespace KABC;

OpieConverter::OpieConverter() : catDB(0)
{
}

OpieConverter::~OpieConverter()
{
  deinit();
}

bool OpieConverter::init()
{
  catDB = new Categories();
  if (catDB) 
  {
    catDB->load( categoryFileName() );
    return true;
  }
  else 
  {
    return false;
  }

}

void OpieConverter::deinit()
{
  if (catDB) 
  {
    delete catDB;
    catDB = 0;
  }
}
  
bool OpieConverter::opieToAddressee( const OContact &contact, Addressee &addr )
{
    // name
    addr.setFormattedName(contact.fileAs());
    addr.setFamilyName( contact.lastName() );
    addr.setGivenName( contact.firstName() );
    addr.setAdditionalName( contact.middleName() );
    addr.setPrefix( contact.title() );
    addr.setSuffix( contact.suffix() );
  
    
    // email
    QStringList emails = contact.emailList();
    for ( QStringList::Iterator it = emails.begin(); it != emails.end(); ++it ) {
      addr.insertEmail( *it, ((*it) == contact.defaultEmail()) );
    }

    if (!contact.defaultEmail().isEmpty()) 
      addr.insertEmail(contact.defaultEmail(), true);
    
    // home
    if ((!contact.homeStreet().isEmpty()) ||
        (!contact.homeCity().isEmpty()) ||
        (!contact.homeState().isEmpty()) ||
        (!contact.homeZip().isEmpty()) ||
        (!contact.homeCountry().isEmpty()))
    {
      Address homeaddress;
      homeaddress.setType(Address::Home);
//US    homeaddress.setPostOfficeBox( "" );
//US    homeaddress.setExtended( "" );
      homeaddress.setStreet( contact.homeStreet() );
      homeaddress.setLocality( contact.homeCity() );
      homeaddress.setRegion( contact.homeState() );
      homeaddress.setPostalCode( contact.homeZip() );
      homeaddress.setCountry( contact.homeCountry() );
    
      addr.insertAddress( homeaddress );
    }
    
    if (!contact.homePhone().isEmpty())
    {
      PhoneNumber homephone;
      homephone.setType( PhoneNumber::Home );
      homephone.setNumber( contact.homePhone() );
      addr.insertPhoneNumber( homephone );
    }
    
    if (!contact.homeFax().isEmpty())
    {
      PhoneNumber homefax;
      homefax.setType( PhoneNumber::Home | PhoneNumber::Fax );
      homefax.setNumber( contact.homeFax() );
      addr.insertPhoneNumber( homefax );
    }
    
    if (!contact.homeMobile().isEmpty())
    {
      PhoneNumber homemobile;
      homemobile.setType( PhoneNumber::Home | PhoneNumber::Cell );
      homemobile.setNumber( contact.homeMobile() );
      addr.insertPhoneNumber( homemobile );
    }
    
    addr.setUrl( contact.homeWebpage() );
                

    // business
    if ((!contact.businessStreet().isEmpty()) ||
        (!contact.businessCity().isEmpty()) ||
        (!contact.businessState().isEmpty()) ||
        (!contact.businessZip().isEmpty()) ||
        (!contact.businessCountry().isEmpty()))
    {
      Address businessaddress;
      businessaddress.setType(Address::Work);
//US    businessaddress.setPostOfficeBox( "" );
//US    businessaddress.setExtended( "" );
      businessaddress.setStreet( contact.businessStreet() );
      businessaddress.setLocality( contact.businessCity() );
      businessaddress.setRegion( contact.businessState() );
      businessaddress.setPostalCode( contact.businessZip() );
      businessaddress.setCountry( contact.businessCountry() );
    
      addr.insertAddress( businessaddress );
    }
    
    
    if (!contact.businessPhone().isEmpty())
    {
      PhoneNumber businessphone;
      businessphone.setType( PhoneNumber::Work );
      businessphone.setNumber( contact.businessPhone() );
      addr.insertPhoneNumber( businessphone );
    }
    
    if (!contact.businessFax().isEmpty())
    {
      PhoneNumber businessfax;
      businessfax.setType( PhoneNumber::Work | PhoneNumber::Fax );
      businessfax.setNumber( contact.businessFax() );
      addr.insertPhoneNumber( businessfax );
    }
    
    if (!contact.businessMobile().isEmpty())
    {
      PhoneNumber businessmobile;
      businessmobile.setType( PhoneNumber::Work | PhoneNumber::Cell );
      businessmobile.setNumber( contact.businessMobile() );
      addr.insertPhoneNumber( businessmobile );
    }

    if (!contact.businessPager().isEmpty())
    {
      PhoneNumber businesspager;
      businesspager.setType( PhoneNumber::Work | PhoneNumber::Pager );
      businesspager.setNumber( contact.businessPager() );
      addr.insertPhoneNumber( businesspager );
    }
        
    addr.setRole( contact.jobTitle() ); //?
    addr.setOrganization( contact.company() );
    addr.insertCustom( "KADDRESSBOOK", "X-Profession", contact.profession() );
    addr.insertCustom( "KADDRESSBOOK", "X-AssistantsName", contact.assistant() );
    addr.insertCustom( "KADDRESSBOOK", "X-Department", contact.department() );
    addr.insertCustom( "KADDRESSBOOK", "X-ManagersName", contact.manager() );
    addr.insertCustom( "KADDRESSBOOK", "X-Office", contact.office() );

    //personal
    addr.insertCustom( "KADDRESSBOOK", "X-SpousesName", contact.spouse() );
    if (contact.gender() == 1)
      addr.insertCustom( "KADDRESSBOOK", "X-Gender", "female" );
    else if (contact.gender() == 2)
      addr.insertCustom( "KADDRESSBOOK", "X-Gender", "male" );
    
    if (contact.anniversary().isValid()) {
      QString dt = KGlobal::locale()->formatDate(contact.anniversary(), true, KLocale::ISODate);
//US      
      qDebug("OpieConverter::opieToAddressee found:%s", dt.latin1());
      addr.insertCustom( "KADDRESSBOOK", "X-Anniversary", dt);
    }
    
    addr.insertCustom( "KADDRESSBOOK", "X-Children", contact.children() );
    if (contact.birthday().isValid())
      addr.setBirthday( contact.birthday() );
     
    addr.setNickName( contact.nickname() );

    // others
    //US I put opies BusinessWebPage into Ka/Pi's notes block, because no other native field is available.
    QString notes = contact.notes();
    notes += "\nBusinessWebPage: " + contact.businessWebpage() + "\n";
        
    addr.setNote( contact.notes() );

    
        
//US    QString groups() const { return find( Qtopia::Groups ); }
//US    QStringList groupList() const;

    
    QStringList cats = contact.categoryNames("Contacts");
    addr.setCategories( cats );
//    for ( QStringList::Iterator it = cats.begin(); it != cats.end(); ++it ) {
//      qDebug("Cat: %s", (*it).latin1());
//    }
        

    return true;
}

bool OpieConverter::addresseeToOpie( const Addressee &addr, OContact &contact )
{
    // name
    contact.setLastName(addr.familyName());
    contact.setFirstName(addr.givenName());
    contact.setMiddleName(addr.additionalName());
    contact.setTitle(addr.prefix());
    contact.setSuffix(addr.suffix());
    contact.setFileAs();
    
    
    // email
    QStringList emails = addr.emails();
    for ( QStringList::Iterator it = emails.begin(); it != emails.end(); ++it ) {
      contact.insertEmail(*it);
    }
    contact.setDefaultEmail( addr.preferredEmail() );
    

    // home
    const Address homeaddress = addr.address(Address::Home);
    if (!homeaddress.isEmpty()) {
      contact.setHomeStreet(homeaddress.street());
      contact.setHomeCity(homeaddress.locality());
      contact.setHomeState(homeaddress.region());
      contact.setHomeZip(homeaddress.postalCode());
      contact.setHomeCountry(homeaddress.country());
    }

    PhoneNumber homephone = addr.phoneNumber( PhoneNumber::Home );
    if (!homephone.number().isEmpty())
      contact.setHomePhone(homephone.number());
    
    PhoneNumber homefax = addr.phoneNumber( PhoneNumber::Home | PhoneNumber::Fax );
    if (!homefax.number().isEmpty())
      contact.setHomeFax(homefax.number());
    
    PhoneNumber homemobile = addr.phoneNumber( PhoneNumber::Home | PhoneNumber::Cell );
    if (!homemobile.number().isEmpty())
      contact.setHomeMobile(homemobile.number());
    
    contact.setHomeWebpage(addr.url().url());
                

    // business
    const Address businessaddress = addr.address(Address::Work);
    if (!businessaddress.isEmpty()) {
      contact.setBusinessStreet(businessaddress.street());
      contact.setBusinessCity(businessaddress.locality());
      contact.setBusinessState(businessaddress.region());
      contact.setBusinessZip(businessaddress.postalCode());
      contact.setBusinessCountry(businessaddress.country());
    }

    PhoneNumber businessphone = addr.phoneNumber( PhoneNumber::Work );
    if (!businessphone.number().isEmpty())
      contact.setBusinessPhone(businessphone.number());
    
    PhoneNumber businessfax = addr.phoneNumber( PhoneNumber::Work | PhoneNumber::Fax );
    if (!businessfax.number().isEmpty())
      contact.setBusinessFax(businessfax.number());
    
    PhoneNumber businessmobile = addr.phoneNumber( PhoneNumber::Work | PhoneNumber::Cell );
    if (!businessmobile.number().isEmpty())
      contact.setBusinessMobile(businessmobile.number());
        
    PhoneNumber businesspager = addr.phoneNumber( PhoneNumber::Work | PhoneNumber::Pager );
    if (!businesspager.number().isEmpty())
      contact.setBusinessPager(businesspager.number());

    contact.setJobTitle(addr.role());
    contact.setCompany(addr.organization());

    contact.setProfession(addr.custom( "KADDRESSBOOK", "X-Profession" ));
    contact.setAssistant(addr.custom( "KADDRESSBOOK", "X-AssistantsName" ));
    contact.setDepartment(addr.custom( "KADDRESSBOOK", "X-Department" ));
    contact.setManager(addr.custom( "KADDRESSBOOK", "X-ManagersName" ));
    contact.setOffice(addr.custom( "KADDRESSBOOK", "X-Office" ));

    //personal
    contact.setSpouse(addr.custom( "KADDRESSBOOK", "X-Spouse" ));
    QString gend = addr.custom( "KADDRESSBOOK", "X-Gender" );
    if (gend == "female")
      contact.setGender("1");
    else if (gend == "male")
      contact.setGender("2");
        
    QDate dt = KGlobal::locale()->readDate( 
            addr.custom("KADDRESSBOOK", "X-Anniversary" ), "%Y-%m-%d"); // = Qt::ISODate
    contact.setAnniversary( dt );
    
    contact.setChildren(addr.custom( "KADDRESSBOOK", "X-Children" ));
    
    contact.setBirthday(addr.birthday().date());
    contact.setNickname(addr.nickName());

    // other
    contact.setNotes(addr.note());
    
//US    QString groups() const { return find( Qtopia::Groups ); }
//US    QStringList groupList() const;

    QStringList cats = addr.categories();
    
    QArray<int> iar;
    if ( !cats.isEmpty() ) {
      QArray<int> iar = catDB->ids("contact", cats);
      contact.setCategories(iar);
    }

    return true;
}
