/*
    This file is part of libkresources.

    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>
    Copyright (c) 2002 Jan-Pascal van Best <janpascal@vanbest.org>
    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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

$Id: managerimpl.cpp,v 1.7 2005/04/03 21:04:10 zautrix Exp $
*/

#include <kglobal.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <qfile.h>

#include "resource.h"
#include "factory.h"
#include "managerimpl.h"



using namespace KRES;

ManagerImpl::ManagerImpl( const QString &family )
  : mFamily( family ), mConfig( 0 ), mStdConfig( 0 ), mStandard( 0 ),
    mFactory( 0 )

{
  kdDebug(5650) << "ManagerImpl::ManagerImpl()" << endl;


}

ManagerImpl::~ManagerImpl()
{
  kdDebug(5650) << "ManagerImpl::~ManagerImpl()" << endl;

  Resource::List::ConstIterator it;
  for ( it = mResources.begin(); it != mResources.end(); ++it ) {
    delete *it;
  }

  delete mStdConfig;
}

void ManagerImpl::createStandardConfig()
{
  if ( !mStdConfig ) {
    QString file = locateLocal( "data", KGlobal::getAppName()
                                + "/kresources/" + mFamily + "rc" ); 
    if (   mFamily == "tmpcontact" ) {
        if (QFile::exists ( file ) ){
            QFile::remove ( file ); 
            qDebug("removed tmp rc file: %s ", file.latin1());
        }
    }
    mStdConfig = new KConfig( file );
  }

  mConfig = mStdConfig;
}

void ManagerImpl::readConfig( KConfig *cfg )
{
  kdDebug(5650) << "ManagerImpl::readConfig()" << endl;

  delete mFactory;
  mFactory = Factory::self( mFamily );

  if ( !cfg ) {
    createStandardConfig();
  } else {
    mConfig = cfg;
  }

  mStandard = 0;

  mConfig->setGroup( "General" );

  QStringList keys = mConfig->readListEntry( "ResourceKeys" );
  keys += mConfig->readListEntry( "PassiveResourceKeys" );

  QString standardKey = mConfig->readEntry( "Standard" );

  for ( QStringList::Iterator it = keys.begin(); it != keys.end(); ++it ) {
    readResourceConfig( *it, false );
  }

}

void ManagerImpl::writeConfig( KConfig *cfg )
{
//USqDebug("ManagerImpl::writeConfig begin this= %ul cfg=%ul", this, cfg);


  kdDebug(5650) << "ManagerImpl::writeConfig()" << endl;

  if ( !cfg ) {
    createStandardConfig();
  } else {
    mConfig = cfg;
  }

  QStringList activeKeys;
  QStringList passiveKeys;

  // First write all keys, collect active and passive keys on the way
  Resource::List::Iterator it;
  for ( it = mResources.begin(); it != mResources.end(); ++it ) {
    writeResourceConfig( *it, false );

    QString key = (*it)->identifier();
    if( (*it)->isActive() )
      activeKeys.append( key );
    else
      passiveKeys.append( key );
  }

  // And then the general group

  kdDebug(5650) << "Saving general info" << endl;
  mConfig->setGroup( "General" );
  mConfig->writeEntry( "ResourceKeys", activeKeys );
  mConfig->writeEntry( "PassiveResourceKeys", passiveKeys );
  if ( mStandard )
    mConfig->writeEntry( "Standard", mStandard->identifier() );
  else
    mConfig->writeEntry( "Standard", "" );

  mConfig->sync();
  kdDebug(5650) << "ManagerImpl::save() finished" << endl;

//US qDebug("ManagerImpl::writeConfig end this= %ul cfg=%ul", this, cfg);

}

void ManagerImpl::add( Resource *resource, bool useDCOP )
{
    //qDebug("ManagerImpl::add begin this= %ul resource=%ul", this, resource);

  resource->setActive( true );

  if ( mResources.isEmpty() ) {
    mStandard = resource;
  }

  mResources.append( resource );

  writeResourceConfig( resource, true );

  //qDebug("ManagerImpl::add end this= %ul resource=%ul", this, resource);

}

void ManagerImpl::remove( Resource *resource, bool useDCOP )
{
  if ( mStandard == resource ) mStandard = 0;
  removeResource( resource );

  mResources.remove( resource );

  delete resource;

  kdDebug(5650) << "Finished ManagerImpl::remove()" << endl;
}

void ManagerImpl::setActive( Resource *resource, bool active )
{
  if ( resource && resource->isActive() != active ) {
    resource->setActive( active );
  }
}

Resource *ManagerImpl::standardResource()
{
  return mStandard;
}

void ManagerImpl::setStandardResource( Resource *resource )
{
  mStandard = resource;
}

void ManagerImpl::resourceChanged( Resource *resource )
{
  writeResourceConfig( resource, true );


//  ManagerIface_stub allManagers( "*", "ManagerIface_" + mFamily.utf8() );
//  allManagers.dcopResourceModified( resource->identifier() );
}

// DCOP asynchronous functions
//US since we work from inside the application, we call the methods directly.

QStringList ManagerImpl::resourceNames()
{
  QStringList result;

  Resource::List::ConstIterator it;
  for ( it = mResources.begin(); it != mResources.end(); ++it ) {
    result.append( (*it)->resourceName() );
  }
  return result;
}

Resource::List *ManagerImpl::resourceList()
{
  return &mResources;
}

QPtrList<Resource> ManagerImpl::resources()
{
  QPtrList<Resource> result;

  Resource::List::ConstIterator it;
  for ( it = mResources.begin(); it != mResources.end(); ++it ) {
    result.append( *it );
  }
  return result;
}

QPtrList<Resource> ManagerImpl::resources( bool active )
{
  QPtrList<Resource> result;

  Resource::List::ConstIterator it;
  for ( it = mResources.begin(); it != mResources.end(); ++it ) {
    if ( (*it)->isActive() == active ) {
      result.append( *it );
    }
  }
  return result;
}

void ManagerImpl::setListener( ManagerImplListener *listener )
{
  mListener = listener;
}

Resource* ManagerImpl::readResourceConfig( const QString& identifier,
                                                   bool checkActive )
{
  kdDebug() << "ManagerImpl::readResourceConfig() " << identifier << endl;

//  qDebug("ManagerImpl::readResourceConfig() %s", identifier.latin1());

  mConfig->setGroup( "Resource_" + identifier );
#ifdef _WIN32_
  // we use plugins on win32. the group is stored in a static variable
  // such that gourp info not avail on win32 plugins
  // to fix that, it would be a looooot of work
  mConfig->setTempGroup( "Resource_" + identifier );
#endif
  QString type = mConfig->readEntry( "ResourceType" );
  QString name = mConfig->readEntry( "ResourceName" );
  Resource *resource = mFactory->resource( type, mConfig );
  if ( !resource ) {
      qDebug("Failed to create resource with id %s ",identifier.latin1() );
      return 0;
  }

  if ( resource->identifier().isEmpty() )
    resource->setIdentifier( identifier );

  mConfig->setGroup( "General" );

  QString standardKey = mConfig->readEntry( "Standard" );
  if ( standardKey == identifier ) {
    mStandard = resource;
  }

  if ( checkActive ) {
    QStringList activeKeys = mConfig->readListEntry( "ResourceKeys" );
    resource->setActive( activeKeys.contains( identifier ) );
  }
  mResources.append( resource );

  return resource;
}

void ManagerImpl::writeResourceConfig( Resource *resource,
                                               bool checkActive )
{
  QString key = resource->identifier();

  kdDebug(5650) << "Saving resource " << key << endl;

  if ( !mConfig ) createStandardConfig();

  mConfig->setGroup( "Resource_" + key );
  resource->writeConfig( mConfig );

  mConfig->setGroup( "General" );
  QString standardKey = mConfig->readEntry( "Standard" );

  if ( resource == mStandard  && standardKey != key )
    mConfig->writeEntry( "Standard", resource->identifier() );
  else if ( resource != mStandard && standardKey == key )
    mConfig->writeEntry( "Standard", "" );

  if ( checkActive ) {
    QStringList activeKeys = mConfig->readListEntry( "ResourceKeys" );
    if ( resource->isActive() && !activeKeys.contains( key ) ) {
      activeKeys.append( resource->identifier() );
      mConfig->writeEntry( "ResourceKeys", activeKeys );
    } else if ( !resource->isActive() && activeKeys.contains( key ) ) {
      activeKeys.remove( key );
      mConfig->writeEntry( "ResourceKeys", activeKeys );
    }
  }

  mConfig->sync();
}

void ManagerImpl::removeResource( Resource *resource )
{
  QString key = resource->identifier();

  if ( !mConfig ) createStandardConfig();

  mConfig->setGroup( "General" );
  QStringList activeKeys = mConfig->readListEntry( "ResourceKeys" );
  if ( activeKeys.contains( key ) ) {
    activeKeys.remove( key );
    mConfig->writeEntry( "ResourceKeys", activeKeys );
  } else {
    QStringList passiveKeys = mConfig->readListEntry( "PassiveResourceKeys" );
    passiveKeys.remove( key );
    mConfig->writeEntry( "PassiveResourceKeys", passiveKeys );
  }

  QString standardKey = mConfig->readEntry( "Standard" );
  if ( standardKey == key ) {
    mConfig->writeEntry( "Standard", "" );
  }

  mConfig->deleteGroup( "Resource_" + resource->identifier() );

  mConfig->sync();
}

Resource* ManagerImpl::getResource( const QString& identifier )
{
  Resource::List::ConstIterator it;
  for ( it = mResources.begin(); it != mResources.end(); ++it ) {
    if ( (*it)->identifier() == identifier )
      return *it;
  }
  return 0;
}


