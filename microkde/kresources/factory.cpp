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

#include <kdebug.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kstaticdeleter.h>
//#ifndef DESKTOP_VERSION
#include <klibloader.h>
//#endif
#include <qfile.h>

#include "resource.h"
#include "factory.h"

#ifdef STATIC_RESOURCES
#include <file/resourcefile.h>
#include <dir/resourcedir.h>
#include <qtopia/resourceqtopia.h>
#endif 
using namespace KRES;

QDict<Factory> *Factory::mSelves = 0;
static KStaticDeleter< QDict<Factory> > staticDeleter;

Factory *Factory::self( const QString& resourceFamily)
{


  Factory *factory = 0;
  if ( !mSelves )
  {
    mSelves = staticDeleter.setObject( new QDict<Factory> );
  }

  factory = mSelves->find( resourceFamily );

  if ( !factory ) {
    factory = new Factory( resourceFamily);
    mSelves->insert( resourceFamily, factory );
  }

  return factory;
}

Factory::Factory( const QString& resourceFamily) :
  mResourceFamily( resourceFamily )
{
//US so far we have three types available for resourceFamily "contact"
// and that are "file", "dir", "ldap"
/*US

  KTrader::OfferList plugins = KTrader::self()->query( "KResources/Plugin", QString( "[X-KDE-ResourceFamily] == '%1'" )
                                                .arg( resourceFamily ) );
  KTrader::OfferList::ConstIterator it;
  for ( it = plugins.begin(); it != plugins.end(); ++it ) {
    QVariant type = (*it)->property( "X-KDE-ResourceType" );
    if ( !type.toString().isEmpty() )
      mTypeMap.insert( type.toString(), *it );
  }
*/

//US new
  PluginInfo* info = new PluginInfo;
  info->library = "microkabc_file";
  info->nameLabel = i18n( "file" );
  info->descriptionLabel = i18n( "One file" );
  mTypeMap.insert( "file", info );

  info = new PluginInfo;
  info->library = "microkabc_dir";
  info->nameLabel = i18n( "dir" );
  info->descriptionLabel = i18n( "A directory with many files" );
  mTypeMap.insert( "dir", info );

  info = new PluginInfo;
  info->library = "microkabc_ldap";
  info->nameLabel = i18n( "ldap" );
  info->descriptionLabel = i18n( "Connect to a directory server" );
  mTypeMap.insert( "ldap", info );

  //US add opie plugin only, if the library exists.
  /*US
  QString libname = "microkabc_opie";
  QString path = KLibLoader::findLibrary( QFile::encodeName( libname ) );
  if ( !path.isEmpty() )
  {
    info = new PluginInfo;
    info->library = libname;
    info->nameLabel = i18n( "opie" );
    info->descriptionLabel = i18n( "Opie PIM Addressbook." );
    mTypeMap.insert( "opie", info );
  }
  */
  //US add qtopia plugin only, if the library exists.
  QString libname = "microkabc_qtopia";
  QString path = KLibLoader::findLibrary( QFile::encodeName( libname ) );
  if ( !path.isEmpty() )
  {
    info = new PluginInfo;
    info->library = libname;
    info->nameLabel = i18n( "qtopia" );
    info->descriptionLabel = i18n( "Qtopia PIM Addressbook." );
    mTypeMap.insert( "qtopia", info );
  }

  //US add sharp plugin only, if the library exists.
  libname = "microkabc_sharpdtm";
  path = KLibLoader::findLibrary( QFile::encodeName( libname ) );
  if ( !path.isEmpty() )
  {
    info = new PluginInfo;
    info->library = libname;
    info->nameLabel = i18n( "sharp" );
    info->descriptionLabel = i18n( "Sharp DTM Addressbook." );
    mTypeMap.insert( "sharp", info );
  }


  //LR add ol plugin only, if the library exists.
  libname = "microkabc_olaccess";
  path = KLibLoader::findLibrary( QFile::encodeName( libname ) );
  if ( !path.isEmpty() )
  {
    info = new PluginInfo;
    info->library = libname;
    info->nameLabel = i18n( "olaccess" );
    info->descriptionLabel = i18n( "Outlook Addressbook." );
    mTypeMap.insert( "olaccess", info );
  }


}

Factory::~Factory()
{
}

QStringList Factory::typeNames() const
{
//US method QMap::keys() not available yet. SO collect the data manually
//US return mTypeMap.keys();

  QStringList result;

  QMap<QString, PluginInfo*>::ConstIterator it;
  for( it = mTypeMap.begin(); it != mTypeMap.end(); ++it ) {
    result << it.key().latin1();
//    qDebug("Factory::typeNames() : %s ", it.key().latin1());

  }
  return result;
}

ConfigWidget *Factory::configWidget( const QString& type, QWidget *parent )
{
  if ( type.isEmpty() || !mTypeMap.contains( type ) )
    return 0;

//US  KService::Ptr ptr = mTypeMap[ type ];
//US  KLibFactory *factory = KLibLoader::self()->factory( ptr->library().latin1() );
  PluginInfo* pi = mTypeMap[ type ];
  KLibFactory *factory = (KLibFactory *)KLibLoader::self()->factory( pi->library.latin1() );
  if ( !factory ) {
    qDebug("KRES::Factory::configWidget(): Factory creation failed for library %s", pi->library.latin1());
    kdDebug() << "KRES::Factory::configWidget(): Factory creation failed" << endl;
    return 0;
  }

  PluginFactoryBase *pluginFactory = static_cast<PluginFactoryBase *>( factory );

  if ( !pluginFactory ) {
    qDebug("KRES::Factory::configWidget(): no plugin factory for library %s", pi->library.latin1());
    kdDebug() << "KRES::Factory::configWidget(): no plugin factory." << endl;
    return 0;
  }

  ConfigWidget *wdg = pluginFactory->configWidget( parent );
  if ( !wdg ) {
//US    kdDebug() << "'" << ptr->library() << "' is not a " + mResourceFamily + " plugin." << endl;
      qDebug("%s is not a %s plugin.", pi->library.latin1(), mResourceFamily.latin1());
    return 0;
  }
  return wdg;

}

QString Factory::typeName( const QString &type ) const
{
  if ( type.isEmpty() || !mTypeMap.contains( type ) )
    return QString();


//US  KService::Ptr ptr = mTypeMap[ type ];
//US  return ptr->name();
  PluginInfo* pi = mTypeMap[ type ];
  return pi->nameLabel;

}

QString Factory::typeDescription( const QString &type ) const
{
  if ( type.isEmpty() || !mTypeMap.contains( type ) )
    return QString();

//US  KService::Ptr ptr = mTypeMap[ type ];
//US  return ptr->comment();
  PluginInfo* pi = mTypeMap[ type ];
  return pi->descriptionLabel;
}

Resource *Factory::resource( const QString& type, const KConfig *config )
{


  if ( type.isEmpty() || !mTypeMap.contains( type ) )
    return 0;

#ifdef STATIC_RESOURCES
  qDebug("NEW STATIC RESOURCE %s", type.latin1());
  Resource *resource = 0;
  if ( type == "file" ) {
      resource = (Resource *) new KABC::ResourceFile( config );
  } else if ( type == "dir" ) {
      resource = new KABC::ResourceDir( config );
  } else if ( type == "qtopia" ) {
      resource = new KABC::ResourceQtopia(  config );
  }
  if ( !resource)
      qDebug("Factory::resource:: resources are statically linked. resource type %s is not supported ",type.latin1() );
  else
      resource->setType( type );
  return resource;
#else



/*US load the lib not dynamicly. !!
  KService::Ptr ptr = mTypeMap[ type ];
  KLibFactory *factory = KLibLoader::self()->factory( ptr->library().latin1() );
  if ( !factory ) {
    kdDebug() << "KRES::Factory::resource(): Factory creation failed" << endl;
    return 0;
  }
*/
  PluginInfo* pi = mTypeMap[ type ];
  KLibFactory *factory = (KLibFactory *)KLibLoader::self()->factory( pi->library.latin1() );
  if ( !factory ) {
    qDebug("KRES::Factory::resource(): Factory creation failed for library %s", pi->library.latin1());
    kdDebug() << "KRES::Factory::resource(): Factory creation failed" << endl;
    return 0;
  }

  PluginFactoryBase *pluginFactory = static_cast<PluginFactoryBase *>( factory );

  if ( !pluginFactory ) {
    qDebug("KRES::Factory::resource(): no plugin factory for library %s", pi->library.latin1());
    kdDebug() << "KRES::Factory::resource(): no plugin factory." << endl;
    return 0;
  }

  Resource *resource = pluginFactory->resource( config );
  if ( !resource ) {
//US    kdDebug() << "'" << ptr->library() << "' is not a " + mResourceFamily + " plugin." << endl;
      qDebug("%s is not a %s plugin.", pi->library.latin1(), mResourceFamily.latin1());
    return 0;
  }

  resource->setType( type );

  return resource;

#endif 
}
