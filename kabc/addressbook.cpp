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

$Id: addressbook.cpp,v 1.52 2008/09/24 22:12:28 zautrix Exp $
*/

/*US

#include <qfile.h>
#include <qregexp.h>
#include <qtimer.h>

#include <kapplication.h>
#include <kinstance.h>
#include <kstandarddirs.h>

#include "errorhandler.h"
*/
#include <qptrlist.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qregexp.h>

#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <libkcal/syncdefines.h>
#include <libkdepim/phoneaccess.h>
#include "addressbook.h"
#include "resource.h"
#include "vcardconverter.h"
#include "vcardparser/vcardtool.h"

//US #include "addressbook.moc"

using namespace KABC;

struct AddressBook::AddressBookData
{
  Addressee::List mAddressees;
  Addressee::List mRemovedAddressees;
  Field::List mAllFields;
  KConfig *mConfig;
  KRES::Manager<Resource> *mManager;
//US  ErrorHandler *mErrorHandler;
};

struct AddressBook::Iterator::IteratorData
{
  Addressee::List::Iterator mIt;
};

struct AddressBook::ConstIterator::ConstIteratorData
{
  Addressee::List::ConstIterator mIt;
};

AddressBook::Iterator::Iterator()
{
  d = new IteratorData;
}

AddressBook::Iterator::Iterator( const AddressBook::Iterator &i )
{
  d = new IteratorData;
  d->mIt = i.d->mIt;
}

AddressBook::Iterator &AddressBook::Iterator::operator=( const AddressBook::Iterator &i )
{
  if( this == &i ) return *this; // guard against self assignment
  delete d; // delete the old data the Iterator was completely constructed before
  d = new IteratorData;
  d->mIt = i.d->mIt;
  return *this;
}

AddressBook::Iterator::~Iterator()
{
  delete d;
}

const Addressee &AddressBook::Iterator::operator*() const
{
  return *(d->mIt);
}

Addressee &AddressBook::Iterator::operator*()
{
  return *(d->mIt);
}

Addressee *AddressBook::Iterator::operator->()
{
  return &(*(d->mIt));
}

AddressBook::Iterator &AddressBook::Iterator::operator++()
{
  (d->mIt)++;
  return *this;
}

AddressBook::Iterator &AddressBook::Iterator::operator++(int)
{
  (d->mIt)++;
  return *this;
}

AddressBook::Iterator &AddressBook::Iterator::operator--()
{
  (d->mIt)--;
  return *this;
}

AddressBook::Iterator &AddressBook::Iterator::operator--(int)
{
  (d->mIt)--;
  return *this;
}

bool AddressBook::Iterator::operator==( const Iterator &it )
{
  return ( d->mIt == it.d->mIt );
}

bool AddressBook::Iterator::operator!=( const Iterator &it )
{
  return ( d->mIt != it.d->mIt );
}


AddressBook::ConstIterator::ConstIterator()
{
  d = new ConstIteratorData;
}

AddressBook::ConstIterator::ConstIterator( const AddressBook::ConstIterator &i )
{
  d = new ConstIteratorData;
  d->mIt = i.d->mIt;
}

AddressBook::ConstIterator &AddressBook::ConstIterator::operator=( const AddressBook::ConstIterator &i )
{
  if( this  == &i ) return *this; // guard for self assignment
  delete d; // delete the old data because the Iterator was really constructed before
  d = new ConstIteratorData;
  d->mIt = i.d->mIt;
  return *this;
}

AddressBook::ConstIterator::~ConstIterator()
{
  delete d;
}

const Addressee &AddressBook::ConstIterator::operator*() const
{
  return *(d->mIt);
}

const Addressee* AddressBook::ConstIterator::operator->() const
{
  return &(*(d->mIt));
}

AddressBook::ConstIterator &AddressBook::ConstIterator::operator++()
{
  (d->mIt)++;
  return *this;
}

AddressBook::ConstIterator &AddressBook::ConstIterator::operator++(int)
{
  (d->mIt)++;
  return *this;
}

AddressBook::ConstIterator &AddressBook::ConstIterator::operator--()
{
  (d->mIt)--;
  return *this;
}

AddressBook::ConstIterator &AddressBook::ConstIterator::operator--(int)
{
  (d->mIt)--;
  return *this;
}

bool AddressBook::ConstIterator::operator==( const ConstIterator &it )
{
  return ( d->mIt == it.d->mIt );
}

bool AddressBook::ConstIterator::operator!=( const ConstIterator &it )
{
  return ( d->mIt != it.d->mIt );
}


AddressBook::AddressBook()
{
  init(0, "contact");
}

AddressBook::AddressBook( const QString &config )
{
  init(config, "contact");
}

AddressBook::AddressBook( const QString &config, const QString &family )
{
  init(config, family);

}

// the default family is "contact"
void AddressBook::init(const QString &config, const QString &family )
{
  blockLSEchange = false;
  d = new AddressBookData;
  QString fami = family;
  if (config != 0) {
      if ( family == "syncContact" ) {
          qDebug("creating sync config ");
          fami = "contact";
          KConfig* con = new KConfig( locateLocal("config", "syncContactrc") );
          con->setGroup( "General" );
          con->writeEntry( "ResourceKeys", QString("sync") );
          con->writeEntry( "Standard", QString("sync") );
          con->setGroup( "Resource_sync" );
          con->writeEntry( "FileName", config );
          con->writeEntry( "FileFormat", QString("vcard") );
          con->writeEntry( "ResourceIdentifier", QString("sync") );
          con->writeEntry( "ResourceName", QString("sync_res") );
          if ( config.right(4) == ".xml" )
              con->writeEntry( "ResourceType", QString("qtopia") );
          else if (  config == "sharp"            ) {
              con->writeEntry( "ResourceType", QString("sharp") );
          } else {
              con->writeEntry( "ResourceType", QString("file") );
          }
          //con->sync();
          d->mConfig = con;
      }
      else
    d->mConfig = new KConfig( locateLocal("config", config) );
//    qDebug("AddressBook::init 1  config=%s",config.latin1() );
  }
  else {
    d->mConfig = 0;
//    qDebug("AddressBook::init 1  config=0");
  }

//US  d->mErrorHandler = 0;
  d->mManager = new KRES::Manager<Resource>( fami );
  d->mManager->readConfig( d->mConfig );
  if ( family == "syncContact" ) {
      KRES::Manager<Resource> *manager = d->mManager;
      KRES::Manager<Resource>::ActiveIterator it;
      for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
          (*it)->setAddressBook( this );
          if ( !(*it)->open() )
              error( QString( "Unable to open resource '%1'!" ).arg( (*it)->resourceName() ) );
      }
      Resource *res = standardResource();
      if ( !res ) {
          qDebug("ERROR: no standard resource");
          res = manager->createResource( "file" );
          if ( res )
              {
                  addResource( res );
              }
          else
              qDebug(" No resource available!!!");
      }
      setStandardResource( res );
      manager->writeConfig();
  }
  addCustomField( i18n( "Department" ), KABC::Field::Organization,
                                "X-Department", "KADDRESSBOOK" );
  addCustomField( i18n( "Profession" ), KABC::Field::Organization,
                                "X-Profession", "KADDRESSBOOK" );
  addCustomField( i18n( "Assistant's Name" ), KABC::Field::Organization,
                                "X-AssistantsName", "KADDRESSBOOK" );
  addCustomField( i18n( "Manager's Name" ), KABC::Field::Organization,
                                "X-ManagersName", "KADDRESSBOOK" );
  addCustomField( i18n( "Spouse's Name" ), KABC::Field::Personal,
                                "X-SpousesName", "KADDRESSBOOK" );
  addCustomField( i18n( "Office" ), KABC::Field::Personal,
                                "X-Office", "KADDRESSBOOK" );
  addCustomField( i18n( "IM Address" ), KABC::Field::Personal,
                                "X-IMAddress", "KADDRESSBOOK" );
  addCustomField( i18n( "Anniversary" ), KABC::Field::Personal,
                                "X-Anniversary", "KADDRESSBOOK" );

  //US added this field to become compatible with Opie/qtopia addressbook
  // values can be "female" or "male" or "". An empty field represents undefined.
  addCustomField( i18n( "Gender" ), KABC::Field::Personal,
                                "X-Gender", "KADDRESSBOOK" );
  addCustomField( i18n( "Children" ), KABC::Field::Personal,
                                "X-Children", "KADDRESSBOOK" );
  addCustomField( i18n( "FreeBusyUrl" ), KABC::Field::Personal,
                                "X-FreeBusyUrl", "KADDRESSBOOK" );
  addCustomField( i18n( "ExternalID" ), KABC::Field::Personal,
                                "X-ExternalID", "KADDRESSBOOK" );
}

AddressBook::~AddressBook()
{
  delete d->mConfig; d->mConfig = 0;
  delete d->mManager; d->mManager = 0;
//US  delete d->mErrorHandler; d->mErrorHandler = 0;
  delete d; d = 0;
}

bool AddressBook::load()
{

  clear();
  KRES::Manager<Resource>::ActiveIterator it;
  bool ok = true;
  for ( it = d->mManager->activeBegin(); it != d->mManager->activeEnd(); ++it )
    if ( !(*it)->load() ) {
      qDebug( i18n("Unable to load resource '%1'").arg( (*it)->resourceName() ) );
      ok = false;
    } else {
      qDebug( i18n("Resource loaded: '%1'").arg( (*it)->resourceName() ) );
    }
  // mark all addressees as unchanged
  Addressee::List::Iterator addrIt;
  for ( addrIt = d->mAddressees.begin(); addrIt != d->mAddressees.end(); ++addrIt ) {
    (*addrIt).setChanged( false );
    QString id = (*addrIt).custom( "KADDRESSBOOK", "X-ExternalID" );
    if ( !id.isEmpty() ) {
        //qDebug("setId aa %s ", id.latin1());
        (*addrIt).setIDStr(id );
    }
  }
  blockLSEchange = true;
  return ok;
}

bool AddressBook::save( Ticket *ticket )
{
  kdDebug(5700) << "AddressBook::save()"<< endl;

  if ( ticket->resource() ) {
    deleteRemovedAddressees();
    return ticket->resource()->save( ticket );
  }

  return false;
}
// exports all Addressees, which are syncable
// for a specific resource we add all sync addressees
void  AddressBook::export2File( QString fileName, QString resourceName )
{
  
  QFile outFile( fileName );
  if ( !outFile.open( IO_WriteOnly ) ) {
    QString text = i18n( "<qt>Unable to open file <b>%1</b> for export.</qt>" );
    KMessageBox::error( 0, text.arg( fileName ) );
    return ;
  }
  QTextStream t( &outFile );
  t.setEncoding( QTextStream::UnicodeUTF8 );
  Iterator it;
  KABC::VCardConverter::Version version;
  version = KABC::VCardConverter::v3_0;
  for ( it = begin(); it != end(); ++it ) { 
      if ( (*it).resource()  ) {
          bool include = (*it).resource()->includeInSync();
          if ( !resourceName.isEmpty()  ) {
	    include = ((resourceName == (*it).resource()->resourceName()) || ((*it).uid().left( 19 ) == QString("last-syncAddressee-")) );
	  }
          if ( include ) {
              //qDebug(QString ("Exporting resource %1 to file %2").arg( (*it).resource()->resourceName() ).arg( fileName ) );
              if ( !(*it).IDStr().isEmpty() ) {
                  (*it).insertCustom( "KADDRESSBOOK", "X-ExternalID", (*it).IDStr() );
              }
              KABC::VCardConverter converter;
              QString vcard; 
              //Resource *resource() const;
              converter.addresseeToVCard( *it, vcard, version );
              t << vcard << "\r\n";
          }
      }
  }
  t <<  "\r\n\r\n";
  outFile.close(); 
}
// if QStringList uids is empty, all are exported
bool  AddressBook::export2PhoneFormat( QStringList uids ,QString fileName )
{
  KABC::VCardConverter converter;
  QString datastream;
  Iterator it; 
  bool all = uids.isEmpty();
  for ( it = begin(); it != end(); ++it ) {
      // for( QStringList::ConstIterator it = uids.begin(); it != uids.end(); ++it ) {
      if ( ! all ) {
          if ( ! ( uids.contains((*it).uid() ) ))
              continue;
      } 
      KABC::Addressee a = ( *it );
      if ( a.isEmpty() )
          continue;
      if ( all && a.resource() && !a.resource()->includeInSync() )
          continue;
      a.simplifyEmails();
      a.simplifyPhoneNumbers();
      a.simplifyPhoneNumberTypes();
      a.simplifyAddresses();

      QString vcard;
      QString vcardnew;
      converter.addresseeToVCard( a, vcard );
      int start = 0;
      int next;
      while ( (next = vcard.find("TYPE=", start) )>= 0 ) {
          int semi = vcard.find(";", next);
          int dopp = vcard.find(":", next);
          int sep;
          if ( semi < dopp && semi >= 0 )
              sep = semi ;
          else
              sep = dopp;
          vcardnew +=vcard.mid( start, next - start);
          vcardnew +=vcard.mid( next+5,sep -next -5 ).upper();
          start = sep;
      }
      vcardnew += vcard.mid( start,vcard.length() );
      vcard = "";
      start = 0;
      while ( (next = vcardnew.find("ADR", start) )>= 0 ) {
          int sep = vcardnew.find(":", next);
          vcard +=vcardnew.mid( start, next - start+3);
          start = sep;
      }
      vcard += vcardnew.mid( start,vcardnew.length() );
      vcard.replace ( QRegExp(";;;") , "" );
      vcard.replace ( QRegExp(";;") , "" );
      datastream += vcard;
     
  }
  
  QFile outFile(fileName);
  if ( outFile.open(IO_WriteOnly) ) { 
      datastream.replace ( QRegExp("VERSION:3.0") , "VERSION:2.1" );
      QTextStream t( &outFile );  // use a text stream
      t.setEncoding( QTextStream::UnicodeUTF8 );
      t <<datastream;
      t <<  "\r\n\r\n";
      outFile.close();
          
  } else {
      qDebug("Error open temp file ");
      return false;
  }
  return true;

}
int  AddressBook::importFromFile( QString fileName, bool replaceLabel, bool removeOld, QString resource   )
{

    if ( removeOld )
        setUntagged( true, resource );
    KABC::Addressee::List list;
    QFile file( fileName );
    file.open( IO_ReadOnly );
    QByteArray rawData = file.readAll();
    file.close();
    QString data;
    if ( replaceLabel ) { 
        data = QString::fromLatin1( rawData.data(), rawData.size() + 1 ); 
        data.replace ( QRegExp("LABEL") , "ADR" );
        data.replace ( QRegExp("CHARSET=ISO-8859-1") , "" );
    } else
        data = QString::fromUtf8( rawData.data(), rawData.size() + 1 ); 
    KABC::VCardTool tool;
    list =  tool.parseVCards( data );
    KABC::Addressee::List::Iterator it;

    Resource * setRes = 0;
    if ( !resource.isEmpty() ) {
        KRES::Manager<Resource>::ActiveIterator it;
        KRES::Manager<Resource> *manager = d->mManager;
        for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
            //qDebug("SaveAB::checking resource..." );
            if ( (*it)->resourceName() == resource  ) {
                setRes = (*it);
                qDebug("KA: AB: Inserting imported contacs to resource %s", resource.latin1());
                break;
            }
        }
    }
    for ( it = list.begin(); it != list.end(); ++it ) {
        QString id = (*it).custom( "KADDRESSBOOK", "X-ExternalID" );
        if ( !id.isEmpty() ) 
            (*it).setIDStr(id );
        (*it).setResource(  setRes );
        if ( replaceLabel )
            (*it).removeVoice();
        if ( removeOld )
            (*it).setTagged( true );
        insertAddressee( (*it), false, true );
    }
    if ( removeOld )
        removeUntagged();
    return list.count();
}
void AddressBook::storeIncludeInSync()
{
  KRES::Manager<Resource>::ActiveIterator it;
  KRES::Manager<Resource> *manager = d->mManager;
  for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
    (*it)->storeIncludeInSync();
  }
}
void AddressBook::restoreIncludeInSync()
{
  KRES::Manager<Resource>::ActiveIterator it;
  KRES::Manager<Resource> *manager = d->mManager;
  for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
    (*it)->restoreIncludeInSync();
  }
}
void AddressBook::setIncludeInSyncExclusive( QString resource )
{
  mSavedStandardResource = standardResource();
  storeIncludeInSync();
  Resource * setRes = 0;
  KRES::Manager<Resource>::ActiveIterator it;
  KRES::Manager<Resource> *manager = d->mManager;
  for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
    if ( (*it)->resourceName() == resource  ) {
      setStandardResource((*it));
      (*it)->setIncludeInSync( true );
    } else
      (*it)->setIncludeInSync( false );
  }
}


void AddressBook::restoreIncludeInSyncExclusive()
{
  setStandardResource( mSavedStandardResource );
  restoreIncludeInSync();

}

void AddressBook::setUntagged(bool setNonSyncTagged, QString resource) // = false , "")
{
    Iterator ait;
    if ( !resource.isEmpty() ) {
        for ( ait = begin(); ait != end(); ++ait ) {
            if ( (*ait).resource() && (*ait).resource()->resourceName() == resource ) {
                (*ait).setTagged( false );
            }
            else
                (*ait).setTagged( true );
        }
    } else {
        for ( ait = begin(); ait != end(); ++ait ) {
            if ( setNonSyncTagged ) {
                if ( (*ait).resource() && ! (*ait).resource()->includeInSync() ) {
                    (*ait).setTagged( true );
                } else
                    (*ait).setTagged( false );
            } else
                (*ait).setTagged( false );
        }
    }
}
void AddressBook::removeUntagged()
{
  Iterator ait;
  bool todelete = false;
  Iterator todel;
  for ( ait = begin(); ait != end(); ++ait ) {
      if ( todelete )
          removeAddressee( todel );
      if (!(*ait).tagged()) {
          todelete = true;
          todel = ait;
      } else
          todelete = false;
  } 
  if ( todelete )
      removeAddressee( todel );
  deleteRemovedAddressees();
}
void AddressBook::smplifyAddressees() 
{
    Iterator ait;
    for ( ait = begin(); ait != end(); ++ait ) {
        (*ait).simplifyEmails();
        (*ait).simplifyPhoneNumbers();
        (*ait).simplifyPhoneNumberTypes();
        (*ait).simplifyAddresses();
    }
}
void AddressBook::removeSyncInfo( QString syncProfile)
{
    Iterator ait;
    for ( ait = begin(); ait != end(); ++ait ) {
        (*ait).removeID( syncProfile );
    } 
    if ( syncProfile.isEmpty() ) {
        Iterator it = begin();
        Iterator it2 ;
        while ( it != end() ) {
            if ( (*it).uid().left( 19 ) == QString("last-syncAddressee-") ) {
                it2 = it;
                //qDebug("removing %s ",(*it).uid().latin1() );
                ++it;
                removeAddressee( it2 );
            } else  {
                //qDebug("skipping %s ",(*it).uid().latin1() );
                ++it;
            }
        }
    } else { 
        Addressee lse;
        lse =  findByUid( "last-syncAddressee-"+ syncProfile );
        if ( ! lse.isEmpty() )
            removeAddressee( lse );  
    }

}
void AddressBook::preparePhoneSync( QString currentSyncDevice, bool isPreSync )
{
    Iterator ait;
    for ( ait = begin(); ait != end(); ++ait ) {
        QString id = (*ait).IDStr();
        (*ait).setIDStr( ":");
        (*ait).setExternalUID( id );
        (*ait).setOriginalExternalUID( id ); 
        if ( isPreSync )
            (*ait).setTempSyncStat( SYNC_TEMPSTATE_NEW_EXTERNAL );
        else {
            (*ait).setTempSyncStat( SYNC_TEMPSTATE_NEW_ID );
            (*ait).setID( currentSyncDevice,id );
            
        }
    }
}
void AddressBook::findNewExtIds(  QString fileName, QString currentSyncDevice  )
{
 
    setUntagged();
    KABC::Addressee::List list;
    QFile file( fileName );
    file.open( IO_ReadOnly );
    QByteArray rawData = file.readAll();
    file.close();
    QString data;

    data = QString::fromUtf8( rawData.data(), rawData.size() + 1 ); 
    KABC::VCardTool tool;
    list =  tool.parseVCards( data );
    KABC::Addressee::List::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        Iterator ait;
        for ( ait = begin(); ait != end(); ++ait ) {
            if ( !(*ait).tagged() ) {
                if ( (*ait).containsAdr(*it)) {
                    (*ait).setTagged(true);
                    QString id = (*it).custom( "KADDRESSBOOK", "X-ExternalID" );
                    (*it).setIDStr( ":");
                    (*it).setID( currentSyncDevice,id );
                    (*it).setExternalUID( id );
                    (*it).setTempSyncStat( SYNC_TEMPSTATE_NEW_ID );
                    (*it).setUid( ( (*ait).uid() ));
                    break;
                }
            }
            
        }
        if ( ait == end() )
            qDebug("ERROR:: no ext ID found for uid: %s", (*it).uid().latin1());      
    }
    clear();
    for ( it = list.begin(); it != list.end(); ++it ) {
        insertAddressee( (*it)  );
    }
}

bool AddressBook::saveABphone( QString fileName )
{
    //smplifyAddressees();
    qDebug("saveABphone:: saving AB... ");
    if ( ! export2PhoneFormat( QStringList() ,fileName ) )
        return false;
    qDebug("saveABphone:: writing to phone... ");   
    if ( !PhoneAccess::writeToPhone( fileName) ) {
        return false;
    }
    qDebug("saveABphone:: re-reading from phone... ");   
    if ( !PhoneAccess::readFromPhone( fileName) ) {
        return false;
    }
    return true;
}
bool AddressBook::saveAB()
{
  bool ok = true;

  deleteRemovedAddressees();
  Iterator ait;
  for ( ait = begin(); ait != end(); ++ait ) {
      if ( !(*ait).IDStr().isEmpty() ) {
          (*ait).insertCustom( "KADDRESSBOOK", "X-ExternalID", (*ait).IDStr() );
    }
  }
  KRES::Manager<Resource>::ActiveIterator it;
  KRES::Manager<Resource> *manager = d->mManager;
  qDebug("SaveAB::saving..." );
  for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
      qDebug("SaveAB::checking resource..." );
      if ( (*it)->readOnly() )
          qDebug("SaveAB::resource is readonly." );
      if ( (*it)->isOpen() )
          qDebug("SaveAB::resource is open" );
      
    if ( !(*it)->readOnly() && (*it)->isOpen() ) {
      Ticket *ticket = requestSaveTicket( *it );
      qDebug("SaveAB::StdAddressBook::save '%s'", (*it)->resourceName().latin1() );
      if ( !ticket ) {
        qDebug( i18n( "SaveAB::Unable to save to resource '%1'. It is locked." )
                   .arg( (*it)->resourceName() ) );
        return false;
      }

      //if ( !save( ticket ) ) 
      if ( ticket->resource() ) {
          QString name = ticket->resource()->resourceName();
          if ( ! ticket->resource()->save( ticket )  )
              ok = false;
          else
              qDebug("SaveAB::resource saved '%s'", name.latin1() );
              
      } else 
          ok = false;
          
    }
  }
  return ok;
}

AddressBook::Iterator AddressBook::begin()
{
  Iterator it = Iterator();
  it.d->mIt = d->mAddressees.begin();
  return it;
}

AddressBook::ConstIterator AddressBook::begin() const
{
  ConstIterator it = ConstIterator();
  it.d->mIt = d->mAddressees.begin();
  return it;
}

AddressBook::Iterator AddressBook::end()
{
  Iterator it = Iterator();
  it.d->mIt = d->mAddressees.end();
  return it;
}

AddressBook::ConstIterator AddressBook::end() const
{
  ConstIterator it = ConstIterator();
  it.d->mIt = d->mAddressees.end();
  return it;
}

void AddressBook::clear()
{
  d->mAddressees.clear();
}

Ticket *AddressBook::requestSaveTicket( Resource *resource )
{
  kdDebug(5700) << "AddressBook::requestSaveTicket()" << endl;

  if ( !resource )
  {
    qDebug("AddressBook::requestSaveTicket no resource" );
    resource = standardResource();
  }

  KRES::Manager<Resource>::ActiveIterator it;
  for ( it = d->mManager->activeBegin(); it != d->mManager->activeEnd(); ++it ) {
    if ( (*it) == resource ) {
      if ( (*it)->readOnly() || !(*it)->isOpen() )
        return 0;
      else
        return (*it)->requestSaveTicket();
    }
  }

  return 0;
}
//void insertAddressee( const Addressee &, bool setRev = true, bool takeResource = false);
void AddressBook::insertAddressee( const Addressee &a, bool setRev, bool takeResource )
{
    if ( blockLSEchange && setRev && a.uid().left( 19 ) == QString("last-syncAddressee-") ) {
        //qDebug("block insert ");
        return;
    }
    //qDebug("inserting.... %s ",a.uid().latin1() );
    bool found = false;
    Addressee::List::Iterator it;
    for ( it = d->mAddressees.begin(); it != d->mAddressees.end(); ++it ) {
        if ( a.uid() == (*it).uid() ) {
       
            bool changed = false;
            Addressee addr = a;
            if ( addr != (*it) )
                changed = true;

            if ( takeResource ) {
                Resource * res = (*it).resource();
                (*it) = a;
                (*it).setResource( res );
            } else {
                (*it) = a;
                if ( (*it).resource() == 0 )
                    (*it).setResource( standardResource() );
            }
            if ( changed ) {
                if ( setRev ) {
                    (*it).setRevision( QDateTime::currentDateTime() ); 
                }
                (*it).setChanged( true );
            }

            found = true;
        } else {
            if ( (*it).uid().left( 19 ) == QString("last-syncAddressee-") ) {
                QString name = (*it).uid().mid( 19 );
                Addressee b = a;
                QString id = b.getID( name );
                if (  ! id.isEmpty() ) {
                    QString des = (*it).note();
                    int startN;
                    if( (startN = des.find( id ) ) >= 0 ) {
                        int endN = des.find( ",", startN+1 );
                        des =  des.left( startN ) + des.mid( endN+1 );
                        (*it).setNote( des );
                    }
                }
            }
        }
    }
    if ( found )
        return; 
    
    d->mAddressees.append( a );
    Addressee& addr = d->mAddressees.last();
    if ( addr.resource() == 0 )
        addr.setResource( standardResource() );
    addr.setChanged( true );
}

void AddressBook::removeAddressee( const Addressee &a )
{
  Iterator it;
  Iterator it2;
  bool found = false;
  for ( it = begin(); it != end(); ++it ) {
    if ( a.uid() == (*it).uid() ) {
        found = true;
        it2 = it;
    } else {
        if ( (*it).uid().left( 19 ) == QString("last-syncAddressee-") ) {
            QString name = (*it).uid().mid( 19 );
            Addressee b = a;
            QString id = b.getID( name );
            if ( ! id.isEmpty() ) {
                QString des = (*it).note();
                if( des.find( id ) < 0 ) {
                    des +=   id  + ",";
                    (*it).setNote( des );
                }
            }
        }
        
    }
  }

  if ( found )
      removeAddressee( it2 );
      
}

void AddressBook::removeSyncAddressees( bool removeDeleted )
{
    Iterator it = begin();
    Iterator it2 ;
    QDateTime dt ( QDate( 2003,1,1) );
    while ( it != end() ) {
        (*it).setRevision( dt );
        if (( *it).IDStr() != "changed" ) { 
            // "changed" is used for tagging changed addressees when syncing with KDE or OL
            (*it).removeCustom( "KADDRESSBOOK", "X-ExternalID" );
            (*it).setIDStr(":");
        }
        if ( ( (*it).tempSyncStat() == SYNC_TEMPSTATE_DELETE && removeDeleted  )|| (*it).uid().left( 19 ) == QString("last-syncAddressee-")) {
            it2 = it;
            //qDebug("removing %s ",(*it).uid().latin1() );
            ++it;
            removeAddressee( it2 );
        } else  {
            //qDebug("skipping %s ",(*it).uid().latin1() );
            if ( removeDeleted ) {
                // we have no postprocessing in the resource, we have to do it here
                // we have to compute csum for all, because it could be the first sync
                (*it).setTempSyncStat( SYNC_TEMPSTATE_NEW_ID );
            }
            ++it;
        }
    }
    deleteRemovedAddressees();
}

void AddressBook::removeAddressee( const Iterator &it )
{
  d->mRemovedAddressees.append( (*it) );
  d->mAddressees.remove( it.d->mIt );
}

AddressBook::Iterator AddressBook::find( const Addressee &a )
{
  Iterator it;
  for ( it = begin(); it != end(); ++it ) {
    if ( a.uid() == (*it).uid() ) {
      return it;
    }
  }
  return end();
}

Addressee AddressBook::findByUid( const QString &uid )
{
  Iterator it;
  for ( it = begin(); it != end(); ++it ) {
    if ( uid == (*it).uid() ) {
      return *it;
    }
  }
  return Addressee();
}
void AddressBook::preExternSync( AddressBook* aBook, const QString& csd , bool isSubset )
{
    //qDebug("AddressBook::preExternSync ");
    AddressBook::Iterator it;  
    for ( it = begin(); it != end(); ++it ) {
        (*it).setID( csd, (*it).externalUID() );
        (*it).computeCsum( csd );
    }
    mergeAB( aBook ,csd, isSubset );  
}
void AddressBook::preOLSync( AddressBook* aBook, const QString& csd  )
{
    //qDebug("AddressBook::preExternSync ");
    AddressBook::Iterator it;  
    for ( it = begin(); it != end(); ++it ) {
        (*it).setID( csd, (*it).externalUID() );
        (*it).computeCsum( csd );
    }

    Addressee ad;
    for ( it = begin(); it != end(); ++it ) {
        ad = aBook->findByExternUid( (*it).externalUID(), csd );
            if ( !ad.isEmpty() ) {
                (*it).mergeOLContact( ad );
            }
    }
}
void AddressBook::postExternSync( AddressBook* aBook , const QString& csd, bool setID)
{
    //qDebug("AddressBook::postExternSync ");
    AddressBook::Iterator it;  
    int foundEmpty = 0;
    for ( it = begin(); it != end(); ++it ) {
        //qDebug("check uid %s ", (*it).uid().latin1() );
        if ( (*it).tempSyncStat() == SYNC_TEMPSTATE_NEW_ID ||
             (*it).tempSyncStat() == SYNC_TEMPSTATE_NEW_CSUM ||
             (*it).tempSyncStat() == SYNC_TEMPSTATE_ADDED_EXTERNAL) {
            Addressee ad = aBook->findByUid( ( (*it).uid() ));
            if ( ad.isEmpty() ) {
                ++foundEmpty;
                //qDebug("postExternSync:addressee is empty: %s ", (*it).uid().latin1());
                //qDebug("-- formatted name %s ",(*it).formattedName().latin1() );
            } else {
                (*it).setIDStr(":");
                if ( setID ) {
                    if ( (*it).tempSyncStat() == SYNC_TEMPSTATE_NEW_ID ) 
                        ad.setID( csd, (*it).externalUID() );{
                    }
                } else
                    ad.setID( csd, (*it).uid() );
                (*it).computeCsum( csd );
                ad.setCsum( csd, (*it).getCsum( csd ) );
                //qDebug("CSUM %s ",(*it).getCsum( csd ).latin1() );
                aBook->insertAddressee( ad , false);
            }
        }
    }
    if ( foundEmpty ) {
        qDebug("postExternSync:%d empty addressees found:\n  probably filtered out by incoming sync filter.",foundEmpty  );
    }
        
}

bool   AddressBook::containsExternalUid( const QString& uid  )
{
    Iterator it;
    for ( it = begin(); it != end(); ++it ) {
        if ( uid == (*it).externalUID( ) )
            return true;
    }
    return false;
}
const Addressee   AddressBook::findByExternUid( const QString& uid , const QString& profile  ) const
{
    ConstIterator it;
    for ( it = begin(); it != end(); ++it ) {
        if ( uid == (*it).getID( profile ) )
            return (*it);
    }
    return Addressee();
}
void  AddressBook::mergeAB( AddressBook *aBook, const QString& profile , bool isSubset )
{
    Iterator it;
    Addressee ad;
    for ( it = begin(); it != end(); ++it ) {
        ad = aBook->findByExternUid( (*it).externalUID(), profile );
            if ( !ad.isEmpty() ) {
                (*it).mergeContact( ad ,isSubset);
            }
    }
#if 0 
    // test only
    for ( it = begin(); it != end(); ++it ) {
        
        qDebug("uid %s ", (*it).uid().latin1());
    }
#endif
}

#if 0
Addressee::List AddressBook::getExternLastSyncAddressees()
{
    Addressee::List results;

  Iterator it;
  for ( it = begin(); it != end(); ++it ) {
    if ( (*it).uid().left( 19 ) == "last-syncAddressee-"  ) {
        if ( (*it).familyName().left(4) == "!E: " )
            results.append( *it );
    }
  }

  return results;
}
#endif
void AddressBook::resetTempSyncStat()
{
    Iterator it;
  for ( it = begin(); it != end(); ++it ) {
      (*it).setTempSyncStat ( SYNC_TEMPSTATE_INITIAL );
  }

}
 
QStringList AddressBook:: uidList()
{
    QStringList results;
    Iterator it;
    for ( it = begin(); it != end(); ++it ) {
        results.append( (*it).uid() );
    }
    return results;
}


Addressee::List AddressBook::allAddressees()
{
   return d->mAddressees; 
 
}

Addressee::List AddressBook::findByName( const QString &name )
{
  Addressee::List results;

  Iterator it;
  for ( it = begin(); it != end(); ++it ) {
    if ( name == (*it).realName() ) {
      results.append( *it );
    }
  }

  return results;
}

Addressee::List AddressBook::findByEmail( const QString &email )
{
  Addressee::List results;
  QStringList mailList;

  Iterator it;
  for ( it = begin(); it != end(); ++it ) {
    mailList = (*it).emails();
    for ( QStringList::Iterator ite = mailList.begin(); ite != mailList.end(); ++ite ) {
      if ( email == (*ite) ) {
        results.append( *it );
      }
    }
  }

  return results;
}

Addressee::List AddressBook::findByCategory( const QString &category )
{
  Addressee::List results;

  Iterator it;
  for ( it = begin(); it != end(); ++it ) {
    if ( (*it).hasCategory( category) ) {
      results.append( *it );
    }
  }

  return results;
}

void AddressBook::dump() const
{
  kdDebug(5700) << "AddressBook::dump() --- begin ---" << endl;

  ConstIterator it;
  for( it = begin(); it != end(); ++it ) {
    (*it).dump();
  }

  kdDebug(5700) << "AddressBook::dump() ---  end  ---" << endl;
}

QString AddressBook::identifier()
{
  QStringList identifier;


  KRES::Manager<Resource>::ActiveIterator it;
  for ( it = d->mManager->activeBegin(); it != d->mManager->activeEnd(); ++it ) {
    if ( !(*it)->identifier().isEmpty() )
      identifier.append( (*it)->identifier() );
  }

  return identifier.join( ":" );
}

Field::List AddressBook::fields( int category )
{
  if ( d->mAllFields.isEmpty() ) {
    d->mAllFields = Field::allFields();
  }

  if ( category == Field::All ) return d->mAllFields;

  Field::List result;
  Field::List::ConstIterator it;
  for( it = d->mAllFields.begin(); it != d->mAllFields.end(); ++it ) {
    if ( (*it)->category() & category ) result.append( *it );
  }

  return result;
}

bool AddressBook::addCustomField( const QString &label, int category,
                                  const QString &key, const QString &app )
{
  if ( d->mAllFields.isEmpty() ) {
    d->mAllFields = Field::allFields();
  }
//US  QString a = app.isNull() ? KGlobal::instance()->instanceName() : app;
  QString a = app.isNull() ? KGlobal::getAppName() : app;

  QString k = key.isNull() ? label : key;

  Field *field = Field::createCustomField( label, category, k, a );

  if ( !field ) return false;

  d->mAllFields.append( field );

  return true;
}

QDataStream &KABC::operator<<( QDataStream &s, const AddressBook &ab )
{
  if (!ab.d) return s;

  return s << ab.d->mAddressees;
}

QDataStream &KABC::operator>>( QDataStream &s, AddressBook &ab )
{
  if (!ab.d) return s;

  s >> ab.d->mAddressees;

  return s;
}

bool AddressBook::addResource( Resource *resource )
{
  if ( !resource->open() ) {
    kdDebug(5700) << "AddressBook::addResource(): can't add resource" << endl;
    return false;
  }

  resource->setAddressBook( this );

  d->mManager->add( resource );
  return true;
}
void AddressBook::removeResources()
{
    //remove all possible resources. This should cleanup the configfile.
    QPtrList<KABC::Resource> mResources = resources();
    
    QPtrListIterator<KABC::Resource> it(mResources);
    for ( ; it.current(); ++it ) {
        KABC::Resource *res = it.current();
        removeResource(res);
    }
}

bool AddressBook::removeResource( Resource *resource )
{
  resource->close();

  if ( resource == standardResource() )
    d->mManager->setStandardResource( 0 );

  resource->setAddressBook( 0 );

  d->mManager->remove( resource );
  return true;
}

QPtrList<Resource> AddressBook::resources()
{
  QPtrList<Resource> list;

//  qDebug("AddressBook::resources() 1");

  KRES::Manager<Resource>::ActiveIterator it;
  for ( it = d->mManager->activeBegin(); it != d->mManager->activeEnd(); ++it )
    list.append( *it );

  return list;
}

/*US
void AddressBook::setErrorHandler( ErrorHandler *handler )
{
  delete d->mErrorHandler;
  d->mErrorHandler = handler;
}
*/

void AddressBook::error( const QString& msg )
{
/*US
  if ( !d->mErrorHandler ) // create default error handler
    d->mErrorHandler = new ConsoleErrorHandler;

  if ( d->mErrorHandler )
    d->mErrorHandler->error( msg );
  else
    kdError(5700) << "no error handler defined" << endl;
*/
    kdDebug(5700) << "msg" << endl;
    qDebug(msg);
}

void AddressBook::deleteRemovedAddressees()
{
  Addressee::List::Iterator it;
  for ( it = d->mRemovedAddressees.begin(); it != d->mRemovedAddressees.end(); ++it ) {
    Resource *resource = (*it).resource();
    if ( resource && !resource->readOnly() && resource->isOpen() )
      resource->removeAddressee( *it );
  }

  d->mRemovedAddressees.clear();
}

void AddressBook::setStandardResource( Resource *resource )
{
//  qDebug("AddressBook::setStandardResource 1");
  d->mManager->setStandardResource( resource );
}

Resource *AddressBook::standardResource()
{
  return d->mManager->standardResource();
}

KRES::Manager<Resource> *AddressBook::resourceManager()
{
  return d->mManager;
}

void AddressBook::cleanUp()
{
  KRES::Manager<Resource>::ActiveIterator it;
  for ( it = d->mManager->activeBegin(); it != d->mManager->activeEnd(); ++it ) {
    if ( !(*it)->readOnly() && (*it)->isOpen() )
      (*it)->cleanUp();
  }
}
