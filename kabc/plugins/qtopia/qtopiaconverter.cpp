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

$Id: qtopiaconverter.cpp,v 1.9 2005/10/28 03:41:02 zautrix Exp $
*/

//US
#include "kglobal.h"
#include "klocale.h"


#include "qtopiaconverter.h"

#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
//#include <.h>

#include <libkdepim/ksyncprofile.h>


using namespace KABC;

QtopiaConverter::QtopiaConverter()
{
    m_edit = 0;
}

QtopiaConverter::~QtopiaConverter()
{
  deinit();
}

bool QtopiaConverter::init()
{
    QString fn = QDir::homeDirPath() +"/Settings/Categories.xml";
    m_edit = new CategoryEdit( fn);
    return true;
}

void QtopiaConverter::deinit()
{
  if (m_edit)
  {
    delete m_edit;
    m_edit = 0;
  }
}
QString QtopiaConverter::categoriesToNumber( const QStringList &list, const QString &app )
{
 startover:
    QStringList dummy;
    QValueList<OpieCategories>::ConstIterator catIt;
    QValueList<OpieCategories> categories = m_edit->categories();
    bool found = false;
    for ( QStringList::ConstIterator listIt = list.begin(); listIt != list.end(); ++listIt ) {
        /* skip empty category name */
        if ( (*listIt).isEmpty() ) continue;

        found  = false;
        for ( catIt = categories.begin(); catIt != categories.end(); ++catIt ) {
	    /*
	     * We currently do not take app into account
	     * if name matches and the id isn't already in dummy we'll add it
	     */
            if ( (*catIt).name() == (*listIt) && !dummy.contains(( *catIt).id() )  ) { // the same name
                found= true;
                dummy << (*catIt).id();
            }
        }
        /* if not found and the category is not empty
         *
         * generate a new category and start over again
         * ugly goto to reiterate
         */

        if ( !found && !(*listIt).isEmpty() ){
            m_edit->addCategory( app, (*listIt) );  // generate a new category
            goto startover;
	}
    }

    return dummy.join(";");
}


// FROM TT timeconversion.cpp GPLed
QDate QtopiaConverter::fromString( const QString &datestr )
{
    if (datestr.isEmpty() )
        return QDate();

    int monthPos = datestr.find('.');
    int yearPos = datestr.find('.', monthPos+1 );
    if ( monthPos == -1 || yearPos == -1 ) {
	return QDate();
    }
    int d = datestr.left( monthPos ).toInt();
    int m = datestr.mid( monthPos+1, yearPos - monthPos - 1 ).toInt();
    int y = datestr.mid( yearPos+1 ).toInt();
    QDate date ( y,m,d );


    return date;
}

QDate QtopiaConverter::dateFromString( const QString& s )
{
    QDate date;

    if ( s.isEmpty() )
        return date;

    // Be backward compatible to old Opie format:
    // Try to load old format. If it fails, try new ISO-Format!
    date = fromString ( s );
    if ( date.isValid() )
        return date;

    // Read ISO-Format (YYYYMMDD)
    int year = s.mid(0, 4).toInt();
    int month = s.mid(4,2).toInt();
    int day = s.mid(6,2).toInt();

    // do some quick sanity checking
    if ( year < 1900 || year > 3000 )
        return date;

    if ( month < 0 || month > 12 )
        return date;

    if ( day < 0 || day > 31 )
        return date;


    date.setYMD( year, month, day );

    if ( !date.isValid() )
        return QDate();


    return date;
}
QString QtopiaConverter::dateToString( const QDate &d )
{
    if ( d.isNull() || !d.isValid() )
        return QString::null;

    // ISO format in year, month, day (YYYYMMDD); e.g. 20021231
    QString year = QString::number( d.year() );
    QString month = QString::number( d.month() );
    month = month.rightJustify( 2, '0' );
    QString day = QString::number( d.day() );
    day = day.rightJustify( 2, '0' );

    QString str = year + month + day;

    return str;
}

bool QtopiaConverter::qtopiaToAddressee( const QDomElement& el, Addressee &adr )
{
   { //LR
          
            adr.setUid(  el.attribute("Uid" ) );
            adr.setFamilyName( el.attribute( "LastName" ) );
            adr.setGivenName( el.attribute( "FirstName" ) );
            adr.setAdditionalName( el.attribute( "MiddleName" )  );
            adr.setSuffix( el.attribute( "Suffix" ) );
            adr.setNickName( el.attribute( "Nickname" ) );

            QDate date = dateFromString( el.attribute( "Birthday" ) );
            if ( date.isValid() )
              adr.setBirthday( date );

            adr.setRole( el.attribute( "JobTitle" ) );
            if ( !el.attribute( "FileAs" ).isEmpty() )
              adr.setFormattedName( el.attribute( "FileAs" ) );

            adr.setOrganization( el.attribute( "Company" ) );

            KABC::PhoneNumber businessPhoneNum( el.attribute( "BusinessPhone" ),
                                                KABC::PhoneNumber::Work | KABC::PhoneNumber::Pref );
            KABC::PhoneNumber businessFaxNum( el.attribute( "BusinessFax" ),
                                              KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax );
            KABC::PhoneNumber businessMobile( el.attribute( "BusinessMobile" ),
                                              KABC::PhoneNumber::Car );
            KABC::PhoneNumber businessPager( el.attribute( "BusinessPager" ),
                                             KABC::PhoneNumber::Pager );
            if ( !businessPhoneNum.number().isEmpty() )
              adr.insertPhoneNumber( businessPhoneNum );
            if ( !businessFaxNum.number().isEmpty() )
              adr.insertPhoneNumber( businessFaxNum );
            if ( !businessMobile.number().isEmpty() )
              adr.insertPhoneNumber( businessMobile );
            if ( !businessPager.number().isEmpty() )
              adr.insertPhoneNumber( businessPager  );

            // Handle multiple mail addresses
            QString DefaultEmail = el.attribute( "DefaultEmail" );
            if ( !DefaultEmail.isEmpty() )
              adr.insertEmail( DefaultEmail, true ); // preferred

            QStringList Emails = QStringList::split(" ",el.attribute("Emails"));
            int i;
            for (i = 0;i < Emails.count();++i) {
                if ( Emails[i] != DefaultEmail )
                    adr.insertEmail( Emails[i], false );
            }

            KABC::PhoneNumber homePhoneNum( el.attribute( "HomePhone" ),
                                            KABC::PhoneNumber::Home | KABC::PhoneNumber::Pref);
            KABC::PhoneNumber homeFax( el.attribute( "HomeFax" ),
                                       KABC::PhoneNumber::Home | KABC::PhoneNumber::Fax );

            KABC::PhoneNumber homeMobile( el.attribute( "HomeMobile" ),
                                          KABC::PhoneNumber::Cell );

            if ( !homePhoneNum.number().isEmpty() )
              adr.insertPhoneNumber( homePhoneNum );
            if ( !homeFax.number().isEmpty() )
              adr.insertPhoneNumber( homeFax );
            if ( !homeMobile.number().isEmpty() )
              adr.insertPhoneNumber( homeMobile );

            KABC::Address business( KABC::Address::Work );
            business.setStreet( el.attribute( "BusinessStreet" ) );
            business.setLocality( el.attribute( "BusinessCity"  ) );
            business.setRegion( el.attribute( "BusinessState" ) );
            business.setPostalCode( el.attribute( "BusinessZip" )  );
            business.setCountry( el.attribute( "BusinessCountry" ) );

            if ( !business.isEmpty() )
              adr.insertAddress( business );

            KABC::Address home( KABC::Address::Home );
            home.setStreet( el.attribute( "HomeStreet" ) );
            home.setLocality( el.attribute( "HomeCity" ) );
            home.setRegion( el.attribute( "HomeState" ) );
            home.setPostalCode( el.attribute( "HomeZip" ) );
            home.setCountry( el.attribute( "HomeCountry" ) );

            if ( !home.isEmpty() )
              adr.insertAddress( home );

            adr.setNickName( el.attribute( "Nickname" ) );
            adr.setNote( el.attribute( "Notes" ) );

            {
              QStringList categories = QStringList::split(";", el.attribute("Categories" ) );
              QString cat;
              QStringList added;
              for ( uint i = 0; i < categories.count(); i++ ) {
                cat = m_edit->categoryById( categories[ i ], "Contacts" );

                // if name is not empty and we did not add the
                // cat try to repair broken files
                if ( !cat.isEmpty() && !added.contains( cat ) ) {
                  adr.insertCategory( cat );
                  added << cat;
                }
              }
            }

            if ( !el.attribute( "Department" ).isEmpty() )
              adr.insertCustom( "KADDRESSBOOK", "X-Department",  el.attribute( "Department" ) );
            if ( !el.attribute( "HomeWebPage" ).isEmpty() )
              adr.insertCustom( "opie", "HomeWebPage", el.attribute( "HomeWebPage" ) );
            if ( !el.attribute( "Spouse" ).isEmpty() )
              adr.insertCustom( "KADDRESSBOOK", "X-SpousesName", el.attribute( "Spouse" ) );
            if ( !el.attribute( "Gender" ).isEmpty() ) {
                if ( el.attribute( "Gender" ) == "1" ) 
                    adr.insertCustom( "KADDRESSBOOK", "X-Gender", "male" );
                else if ( el.attribute( "Gender" ) == "2" ) 
                    adr.insertCustom( "KADDRESSBOOK", "X-Gender", "female" );
            }
            QDate ann = dateFromString( el.attribute( "Anniversary" ) );
            if ( ann.isValid() ) { 
                QString dt = KGlobal::locale()->formatDate(ann, true, KLocale::ISODate);
              adr.insertCustom( "KADDRESSBOOK", "X-Anniversary", dt );
            }

            if ( !el.attribute( "Children" ).isEmpty() )
              adr.insertCustom("KADDRESSBOOK", "X-Children", el.attribute("Children") );
            if ( !el.attribute( "Office" ).isEmpty() )
              adr.insertCustom("KADDRESSBOOK", "X-Office", el.attribute("Office") );
            if ( !el.attribute( "Profession" ).isEmpty() )
              adr.insertCustom("KADDRESSBOOK", "X-Profession", el.attribute("Profession") );
            if ( !el.attribute( "Assistant" ).isEmpty() )
              adr.insertCustom("KADDRESSBOOK", "X-AssistantsName", el.attribute("Assistant") );
            if ( !el.attribute( "Manager" ).isEmpty() )
              adr.insertCustom("KADDRESSBOOK", "X-ManagersName", el.attribute("Manager") );

            
          }
    return true;
}

bool QtopiaConverter::addresseeToQtopia( const Addressee &ab, QTextStream *stream )
{
            *stream << "<Contact ";
            *stream << "FirstName=\"" << escape(ab.givenName()) << "\" ";
            *stream << "MiddleName=\"" << escape(ab.additionalName()) << "\" ";
            *stream << "LastName=\"" << escape(ab.familyName()) << "\" ";
            *stream << "Suffix=\"" << escape(ab.suffix()) << "\" ";

            QString sortStr;
            sortStr = ab.formattedName();
            /* is formattedName is empty we use the assembled name as fallback */
            if (sortStr.isEmpty() )
                sortStr = ab.assembledName();
            *stream << "FileAs=\"" << escape(sortStr) << "\" ";

            *stream << "JobTitle=\"" << escape(ab.role()) << "\" ";
            *stream << "Department=\"" << escape(ab.custom( "KADDRESSBOOK", "X-Department" )) << "\" ";
            *stream << "Company=\"" << escape(ab.organization()) << "\" ";

            KABC::PhoneNumber businessPhoneNum = ab.phoneNumber(KABC::PhoneNumber::Work );
            *stream << "BusinessPhone=\"" << escape( businessPhoneNum.number() ) << "\" ";

            KABC::PhoneNumber businessFaxNum = ab.phoneNumber(KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax );
            *stream << "BusinessFax=\"" << escape( businessFaxNum.number() )<< "\" ";

            KABC::PhoneNumber businessMobile = ab.phoneNumber(KABC::PhoneNumber::Work | KABC::PhoneNumber::Cell );
            *stream << "BusinessMobile=\"" << escape( businessMobile.number() ) << "\" ";

            *stream << "DefaultEmail=\"" << escape( ab.preferredEmail() ) << "\" ";
            QStringList list = ab.emails();
            if ( list.count() > 0 ) {
		QStringList::Iterator it = list.begin();
                *stream << "Emails=\"" << escape( *it );
		while (++it != list.end())
		  *stream << ' ' << escape( *it );
                *stream << "\" ";
	    }

            KABC::PhoneNumber homePhoneNum = ab.phoneNumber(KABC::PhoneNumber::Home );
            *stream << "HomePhone=\"" << escape( homePhoneNum.number() ) << "\" ";

            KABC::PhoneNumber homeFax = ab.phoneNumber( KABC::PhoneNumber::Home | KABC::PhoneNumber::Fax );
            *stream << "HomeFax=\"" << escape( homeFax.number() ) << "\" ";

            KABC::PhoneNumber homeMobile = ab.phoneNumber( KABC::PhoneNumber::Cell );
            *stream << "HomeMobile=\"" << escape( homeMobile.number() ) << "\" ";

            KABC::Address business = ab.address(KABC::Address::Work  );
            *stream << "BusinessStreet=\"" << escape( business.street() ) << "\" ";
            *stream << "BusinessCity=\"" << escape( business.locality() ) << "\" ";
            *stream << "BusinessZip=\"" << escape( business.postalCode() ) << "\" ";
            *stream << "BusinessCountry=\"" << escape( business.country() ) << "\" ";
            *stream << "BusinessState=\"" << escape( business.region() ) << "\" ";
            //stream << "BusinessPager=\"" << << "\" ";
            *stream << "Office=\"" << escape( ab.custom( "KADDRESSBOOK",  "X-Office" ) ) << "\" ";
            *stream << "Profession=\"" << escape( ab.custom( "KADDRESSBOOK",  "X-Profession" ) ) << "\" ";
            *stream << "Assistant=\"" << escape( ab.custom( "KADDRESSBOOK",  "X-AssistantsName") ) << "\" ";
            *stream << "Manager=\"" << escape( ab.custom( "KADDRESSBOOK",  "X-ManagersName" ) ) << "\" ";

            KABC::Address home = ab.address( KABC::Address::Home );
            *stream << "HomeStreet=\"" << escape( home.street() ) << "\" ";
            *stream << "HomeCity=\"" <<  escape( home.locality() ) << "\" ";
            *stream << "HomeState=\"" <<  escape( home.region() ) << "\" ";
            *stream << "HomeZip=\"" <<  escape( home.postalCode() ) << "\" ";
            *stream << "HomeCountry=\"" << escape( home.country() ) << "\" ";

            *stream << "HomeWebPage=\"" << escape( ab.custom( "opie", "HomeWebPage" ) ) << "\" ";
            *stream << "Spouse=\"" << escape( ab.custom( "KADDRESSBOOK",  "X-SpousesName") ) << "\" ";
            QString gen = "0";
            if ( ab.custom( "KADDRESSBOOK",  "X-Gender") == "male" )
                gen = "1";
            else if ( ab.custom( "KADDRESSBOOK",  "X-Gender") == "female" )
                gen = "2";
            *stream << "Gender=\"" << escape( gen ) << "\" ";

            if ( ab.birthday().date().isValid() )
                *stream << "Birthday=\"" << escape( dateToString(ab.birthday().date() ) ) << "\" ";

            { 
                QDate ann = KGlobal::locale()->readDate( ab.custom("KADDRESSBOOK", "X-Anniversary" ), 
                                                    "%Y-%m-%d"); 
                if (ann.isValid() ) {
                    *stream << "Anniversary=\"" << escape( dateToString( ann )  ) << "\" ";
                }
            }
            *stream << "Nickname=\"" << escape( ab.nickName() ) << "\" ";
            *stream << "Children=\"" << escape( ab.custom("KADDRESSBOOK", "X-Children" ) ) << "\" ";
            *stream << "Notes=\"" << escape( ab.note() ) << "\" ";
            *stream << "Categories=\"" << categoriesToNumber( ab.categories(),  "Contacts") << "\" ";

            QString uid =  ab.uid();
            *stream << "Uid=\"" <<  uid << "\" ";
            //*stream << map.toString( "addressbook", uid );
            *stream << " />" << "\n";

    return true;
}


#if 0

KTempFile* AddressBook::fromKDE( KSync::AddressBookSyncee *syncee, ExtraMap& map )
{
  
}

QStringList AddressBook::attributes()const {
    QStringList lst;
    lst << "FirstName";
    lst << "MiddleName";
    lst << "LastName";
    lst << "Suffix";
    lst << "FileAs";
    lst << "JobTitle";
    lst << "Department";
    lst << "Company";
    lst << "BusinessPhone";
    lst << "BusinessFax";
    lst << "BusinessMobile";
    lst << "DefaultEmail";
    lst << "Emails";
    lst << "HomePhone";
    lst << "HomeFax";
    lst << "HomeMobile";
    lst << "BusinessStreet";
    lst << "BusinessCity";
    lst << "BusinessZip";
    lst << "BusinessCountry";
    lst << "BusinessState";
    lst << "Office";
    lst << "Profession";
    lst << "Assistant";
    lst << "Manager";
    lst << "HomeStreet";
    lst << "HomeCity";
    lst << "HomeState";
    lst << "HomeZip";
    lst << "HomeCountry";
    lst << "HomeWebPage";
    lst << "Spouse";
    lst << "Gender";
    lst << "Anniversary";
    lst << "Nickname";
    lst << "Children";
    lst << "Notes";
    lst << "Categories";
    lst << "Uid";
    lst << "Birthday";

    return lst;
}



#endif


CategoryEdit::CategoryEdit(){
}
CategoryEdit::CategoryEdit(const QString &fileName){
    parse( fileName );
}
CategoryEdit::~CategoryEdit(){
}
void CategoryEdit::save(const QString& fileName)const{
    QFile file( fileName );
    QString endl = "\n";
    if ( file.open( IO_WriteOnly ) ) {
        QTextStream stream( &file );
        stream.setEncoding( QTextStream::UnicodeUTF8 );
        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
        stream << "<!DOCTYPE CategoryList>" << endl;
        stream << "<Categories>" << endl;
        for ( QValueList<OpieCategories>::ConstIterator it = m_categories.begin();
              it != m_categories.end(); ++it )
        {
            stream << "<Category id=\""<< ( (*it).id() ) << "\" ";

            if ( !(*it).app().isEmpty() )
                stream << " app=\""<< ( (*it).app() ) <<  "\" ";

            stream << "name=\"" << ( (*it).name() ) << "\" ";
            stream << " />" << endl;
        }
        stream << "</Categories>" << endl;
        file.close();
    }
}
int CategoryEdit::addCategory( const QString &name, int id ){
    return addCategory( QString::null, name, id );
}
int CategoryEdit::addCategory( const QString &appName,  const QString &name,  int id ){
    if ( id == 0 ) {
        // code from tt
        //generate uid
        QDateTime dt = QDateTime::currentDateTime();
        id = -1 * (int) dt.secsTo( QDateTime(QDate( 2000,1,1)) );
        while ( ids.contains( id ) ){
            id += -1;
            if ( id > 0 )
                id = -1;
        }
    }
    ids.insert( id,  TRUE );
    OpieCategories categories(QString::number(id),  name,  appName);
    //pending FIXME LR m_categories.remove( categories);
    m_categories.append( categories);
    return id;
}
/*
 * we parse the simple Category File here
 * We also keep track of global Cats
 * and Of Organizer and Contact cats and then
 * we will add them to the kde side...
 */
void CategoryEdit::parse( const QString &tempFile ){
    clear();

    QDomDocument doc( "mydocument" );
    QFile f( tempFile );
    if ( !f.open( IO_ReadOnly ) )
	return;

    if ( !doc.setContent( &f ) ) {
	f.close();
	return;
    }
    f.close();

    QStringList global, contact, organizer;

    // print out the element names of all elements that are a direct child
    // of the outermost element.
    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();
    if( docElem.nodeName() == QString::fromLatin1("Categories") ){
	while( !n.isNull() ) {
	    QDomElement e = n.toElement(); // try to convert the node to an element.
	    if( !e.isNull() ) { // the node was really an element.
		QString id = e.attribute("id" );
		QString app = e.attribute("app" );
		QString name = e.attribute("name");

                /*
                 * see where it belongs default to global
                 */
                if (app == QString::fromLatin1("Calendar") || app == QString::fromLatin1("Todo List") )
                    organizer.append( name );
                else if ( app == QString::fromLatin1("Contacts") )
                    contact.append( name );
                else
                    global.append( name );

		OpieCategories category( id, name, app );
		m_categories.append( category ); // cheater
	    }
	    n = n.nextSibling();
	}
    }
    updateKDE( "kaddressbookrc", global + contact );
    updateKDE( "korganizerrc", global + organizer );

}
void CategoryEdit::clear()
{
    ids.clear();
    m_categories.clear();
}
QString CategoryEdit::categoryById( const QString &id,  const QString &app )const
{
    QValueList<OpieCategories>::ConstIterator it;
    QString category;
    QString fallback;
    for( it = m_categories.begin(); it != m_categories.end(); ++it ){
	if( id.stripWhiteSpace() == (*it).id().stripWhiteSpace() ){
	    if( app == (*it).app() ){
                category = (*it).name();
                break;
            }else{
                fallback = (*it).name();
            }
        }
    }
    return category.isEmpty() ? fallback : category;
}
QStringList CategoryEdit::categoriesByIds( const QStringList& ids,
                                           const QString& app) {

    QStringList list;
    QStringList::ConstIterator it;
    QString temp;
    for ( it = ids.begin(); it != ids.end(); ++it ) {
        temp = categoryById( (*it), app );
        if (!temp.isEmpty() )
            list << temp;
    }

    return list;
}
void CategoryEdit::updateKDE( const QString& configFile,  const QStringList& cats ) {
    KConfig conf(configFile);
    conf.setGroup("General");
    QStringList avail = conf.readListEntry("Custom Categories");
    for (QStringList::ConstIterator it = cats.begin(); it !=  cats.end(); ++it ) {
        if (!avail.contains( (*it) ) )
            avail << (*it);
    }
    conf.writeEntry("Custom Categories", avail );
}



OpieCategories::OpieCategories()
{

}
OpieCategories::OpieCategories(const QString &id, const QString &name, const QString &app )
{
    m_name = name;
    m_id = id;
    m_app = app;
}
OpieCategories::OpieCategories(const OpieCategories &op )
{
    (*this) = op;
}
QString OpieCategories::id() const
{
    return m_id;
}
QString OpieCategories::name() const
{
    return m_name;
}
QString OpieCategories::app() const
{
    return m_app;
}
OpieCategories &OpieCategories::operator=(const OpieCategories &op )
{
    m_name = op.m_name;
    m_app = op.m_app;
    m_id = op.m_id;
    return (*this);
}


bool operator== (const OpieCategories& a,  const OpieCategories &b )
{
    if ( a.id() == b.id() && a.name() == b.name() && a.app() == b.app() )
        return true;
    return false;
}

