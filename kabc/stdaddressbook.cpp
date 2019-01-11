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

$Id: stdaddressbook.cpp,v 1.12 2005/08/23 14:22:11 zautrix Exp $
*/

#include <qdir.h>
#include "resource.h"
#include <kresources/manager.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstaticdeleter.h>
#include <kstandarddirs.h>

#include "stdaddressbook.h"

using namespace KABC;

StdAddressBook *StdAddressBook::mSelf = 0;
bool StdAddressBook::mAutomaticSave = false;

static KStaticDeleter<StdAddressBook> addressBookDeleter;

QString StdAddressBook::fileName()
{
  return locateLocal( "data", "kabc/std.vcf" );
}

QString StdAddressBook::directoryName()
{
  return locateLocal( "data", "kabc/stdvcf" );
}

void StdAddressBook::handleCrash()
{
  StdAddressBook::self()->cleanUp();
}
StdAddressBook *StdAddressBook::selfNoLoad()
{

  if ( !mSelf )
  {
      QString appdir = StdAddressBook::setTempAppDir();
// US im am not sure why I have to use the other format here??
#ifdef KAB_EMBEDDED
      mSelf = addressBookDeleter.setObject( new StdAddressBook ( QString() ) );
#else //KAB_EMBEDDED
    addressBookDeleter.setObject( mSelf, new StdAddressBook( QString() ) );
#endif //KAB_EMBEDDED
    KStandardDirs::setAppDir( appdir );
  }

  return mSelf;
}

StdAddressBook *StdAddressBook::self()
{

  if ( !mSelf )
  {
      QString appdir = StdAddressBook::setTempAppDir();
// US im am not sure why I have to use the other format here??
#ifdef KAB_EMBEDDED
    mSelf = addressBookDeleter.setObject( new StdAddressBook );
#else //KAB_EMBEDDED
    addressBookDeleter.setObject( mSelf, new StdAddressBook );
#endif //KAB_EMBEDDED
    KStandardDirs::setAppDir( appdir );
  }

  return mSelf;
}

QString StdAddressBook::setTempAppDir()
{
    QString appDIR = KStandardDirs::appDir();
#ifdef DESKTOP_VERSION
    QString appdir = QDir::homeDirPath();
      if ( appdir.right(1) == "\\" ||   appdir.right(1) == "/" )
          appdir += "kaddressbook/";
      else
          appdir += "/kaddressbook/";
      KStandardDirs::setAppDir( QDir::convertSeparators( appdir ));
#else
      QString appdir = QDir::homeDirPath() + "/kdepim/apps/kaddressbook";

      KStandardDirs::setAppDir( appdir );
#endif

    return appDIR;
}
StdAddressBook *StdAddressBook::self( bool onlyFastResources )
{

   if ( !mSelf )
  {
      QString appdir =StdAddressBook::setTempAppDir();
#ifdef KAB_EMBEDDED
    mSelf = addressBookDeleter.setObject( new StdAddressBook( onlyFastResources ) );
#else //KAB_EMBEDDED
    addressBookDeleter.setObject( mSelf, new StdAddressBook( onlyFastResources ) );
#endif //KAB_EMBEDDED
    KStandardDirs::setAppDir( appdir );
  }
  return mSelf;
}
StdAddressBook::StdAddressBook( QString )
  : AddressBook( "kabcrc" )
{

}

StdAddressBook::StdAddressBook()
  : AddressBook( "kabcrc" )
{

    init( false );
}

StdAddressBook::StdAddressBook( bool onlyFastResources )
  : AddressBook( "kabcrc" )
{

    init( onlyFastResources );
}

StdAddressBook::~StdAddressBook()
{
  if ( mAutomaticSave )
    save();
}

void StdAddressBook::init( bool )
{
  KRES::Manager<Resource> *manager = resourceManager();
  KRES::Manager<Resource>::ActiveIterator it;

  for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
    (*it)->setAddressBook( this );
    if ( !(*it)->open() )
      error( QString( "Unable to open resource '%1'!" ).arg( (*it)->resourceName() ) );
  }

  Resource *res = standardResource();
  if ( !res ) {
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

  load();
}

bool StdAddressBook::save()
{
  kdDebug(5700) << "StdAddressBook::save()" << endl;

  bool ok = true;
  AddressBook *ab = self();

  ab->deleteRemovedAddressees();
  Iterator ait;
  for ( ait = ab->begin(); ait != ab->end(); ++ait ) {
      if ( !(*ait).IDStr().isEmpty() ) {
          (*ait).insertCustom( "KADDRESSBOOK", "X-ExternalID", (*ait).IDStr() );
    }
  }
  KRES::Manager<Resource>::ActiveIterator it;
  KRES::Manager<Resource> *manager = ab->resourceManager();
  for ( it = manager->activeBegin(); it != manager->activeEnd(); ++it ) {
    if ( !(*it)->readOnly() && (*it)->isOpen() ) {
      Ticket *ticket = ab->requestSaveTicket( *it );
//        qDebug("StdAddressBook::save '%s'", (*it)->resourceName().latin1() );
      if ( !ticket ) {
        ab->error( i18n( "Unable to save to resource '%1'. It is locked." )
                   .arg( (*it)->resourceName() ) );
        return false;
      }

      if ( !ab->save( ticket ) )
        ok = false;
    }
  }

  return ok;
}

void StdAddressBook::close()
{
//US destructObject is not defined on my system???. Is setObject(0) the same ???
//US  addressBookDeleter.destructObject();
  addressBookDeleter.setObject(0);

}

void StdAddressBook::setAutomaticSave( bool enable )
{
  mAutomaticSave = enable;
}

bool StdAddressBook::automaticSave()
{
  return mAutomaticSave;
}

// should get const for 4.X
Addressee StdAddressBook::whoAmI()
{
//US  KConfig config( "kabcrc" );
  KConfig config( locateLocal("config", "kabcrc") );
  config.setGroup( "General" );

  return findByUid( config.readEntry( "WhoAmI" ) );
}

void StdAddressBook::setWhoAmI( const Addressee &addr )
{
//US  KConfig config( "kabcrc" );
  KConfig config( locateLocal("config", "kabcrc") );
  config.setGroup( "General" );

  config.writeEntry( "WhoAmI", addr.uid() );
}
