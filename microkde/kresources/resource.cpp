/*
    This file is part of libkresources.

    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
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

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>

#include "resource.h"

using namespace KRES;

class Resource::ResourcePrivate
{
  public:
#ifdef QT_THREAD_SUPPORT
    QMutex mMutex;
#endif
    int mOpenCount;
    QString mType;
    QString mIdentifier;
    bool mReadOnly;
    bool mIncludeInSync;
    bool mIncludeInSyncCopy;
    QString mName;
    bool mActive;
    bool mIsOpen;
};

Resource::Resource( const KConfig* config )
  : QObject( 0, "" ), d( new ResourcePrivate )
{
  d->mOpenCount = 0;
  d->mIsOpen = false;

  //US compiler claimed that const discards qualifier
  KConfig* cfg = (KConfig*)config;
  if ( cfg ) {
#ifdef _WIN32_
      // we use plugins on win32. the group is stored in a static variable
      // such that group info not available on win32 plugins
      // to fix that, it would be a looooot of work
      if ( !cfg->tempGroup().isEmpty() )
          cfg->setGroup( cfg->tempGroup() );
#endif
    d->mType = cfg->readEntry( "ResourceType" );
    d->mName = cfg->readEntry( "ResourceName" );
    d->mReadOnly = cfg->readBoolEntry( "ResourceIsReadOnly", false );
    d->mIncludeInSync = cfg->readBoolEntry( "ResourceIncludeInSync", true );;
    d->mActive = cfg->readBoolEntry( "ResourceIsActive", true );
    d->mIdentifier = cfg->readEntry( "ResourceIdentifier" );
  } else {
    d->mType = "type";
    d->mName = "resource-name";
    d->mReadOnly = false;
    d->mIncludeInSync = true;
    d->mActive = true;
    d->mIdentifier = KApplication::randomString( 10 );
  }
}

Resource::~Resource()
{
  delete d;
  d = 0;
}

void Resource::writeConfig( KConfig* config )
{
  config->writeEntry( "ResourceType", d->mType );
  config->writeEntry( "ResourceName", d->mName );
  config->writeEntry( "ResourceIsReadOnly", d->mReadOnly );
  config->writeEntry( "ResourceIncludeInSync", d->mIncludeInSync );
  config->writeEntry( "ResourceIsActive", d->mActive );
  config->writeEntry( "ResourceIdentifier", d->mIdentifier );
}

bool Resource::open()
{
  d->mIsOpen = true;
#ifdef QT_THREAD_SUPPORT
  QMutexLocker guard( &(d->mMutex) );
#endif
  if ( !d->mOpenCount ) {
    kdDebug(5650) << "Opening resource " << resourceName() << endl;
    d->mIsOpen = doOpen();
  }
  d->mOpenCount++;
  return d->mIsOpen;
}

void Resource::close()
{
#ifdef QT_THREAD_SUPPORT
  QMutexLocker guard( &(d->mMutex) );
#endif
  if ( !d->mOpenCount ) {
    kdDebug(5650) << "ERROR: Resource " << resourceName() << " closed more times than previously opened" << endl;
    return;
  }
  d->mOpenCount--;
  if ( !d->mOpenCount ) {
    kdDebug(5650) << "Closing resource " << resourceName() << endl;
    doClose();
    d->mIsOpen = false;
  } else {
    kdDebug(5650) << "Not yet closing resource " << resourceName() << ", open count = " << d->mOpenCount << endl;
  }
}

bool Resource::isOpen() const
{
  return d->mIsOpen;
}

void Resource::setIdentifier( const QString& identifier )
{
  d->mIdentifier = identifier;
}

QString Resource::identifier() const
{
  return d->mIdentifier;
}

void Resource::setType( const QString& type )
{
  d->mType = type;
}

QString Resource::type() const
{
  return d->mType;
}

void Resource::setIncludeInSync( bool value )
{
    d->mIncludeInSync = value;
}
void Resource::storeIncludeInSync()
{
    d->mIncludeInSyncCopy = d->mIncludeInSync;
}
void Resource::restoreIncludeInSync()
{
    d->mIncludeInSync = d->mIncludeInSyncCopy;
}
bool Resource::includeInSync() const
{
   return  d->mIncludeInSync;
}
void Resource::setReadOnly( bool value )
{
  d->mReadOnly = value;
}

bool Resource::readOnly() const
{
  return d->mReadOnly;
}

void Resource::setResourceName( const QString &name )
{
  d->mName = name;
}

QString Resource::resourceName() const
{
  return d->mName;
}

void Resource::setActive( bool value )
{
  d->mActive = value;
}

bool Resource::isActive() const
{
  return d->mActive;
}

void Resource::dump() const
{
    qDebug("Resource::dump() ");
  kdDebug(5650) << "Resource:" << endl;
  kdDebug(5650) << "  Name: " << d->mName << endl;
  kdDebug(5650) << "  Identifier: " << d->mIdentifier << endl;
  kdDebug(5650) << "  Type: " << d->mType << endl;
  kdDebug(5650) << "  OpenCount: " << d->mOpenCount << endl;
  kdDebug(5650) << "  ReadOnly: " << ( d->mReadOnly ? "yes" : "no" ) << endl;
  kdDebug(5650) << "  IncludeInSync: " << ( d->mIncludeInSync ? "yes" : "no" ) << endl;
  kdDebug(5650) << "  Active: " << ( d->mActive ? "yes" : "no" ) << endl;
  kdDebug(5650) << "  IsOpen: " << ( d->mIsOpen ? "yes" : "no" ) << endl;
}

