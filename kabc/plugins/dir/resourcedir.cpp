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

$Id: resourcedir.cpp,v 1.14 2005/08/19 13:29:22 zautrix Exp $
*/


#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32_
#include <unistd.h>
#endif

#include <qregexp.h>
#include <qtimer.h>
#include <qwidget.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
//US #include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <kmessagebox.h>

#include "addressbook.h"

#include "formatfactory.h"

#include "resourcedirconfig.h"
#include "stdaddressbook.h"

//US
#include <qdir.h>

#define NO_DIRWATCH
#include "resourcedir.h"

//#define ALLOW_LOCKING

using namespace KABC;

extern "C"
#ifdef _WIN32_
__declspec(dllexport)
#else
{
#endif

//US  void *init_kabc_dir()
  void *init_microkabc_dir()
  {
    return new KRES::PluginFactory<ResourceDir,ResourceDirConfig>();
  }
#ifndef _WIN32_
}
#endif

ResourceDir::ResourceDir( const KConfig *config  )
    : Resource( config )
{
  QString path;

  KConfig *cfg = (KConfig *)config;
  if ( cfg ) {
#ifdef _WIN32_
      // we use plugins on win32. the group is stored in a static variable
      // such that group info not available on win32 plugins
      // to fix that, it would be a looooot of work
      if ( !cfg->tempGroup().isEmpty() )
          cfg->setGroup( cfg->tempGroup() );
#endif
//US     path = config->readEntry( "FilePath" );
     path = cfg->readEntry( "FilePath", StdAddressBook::directoryName() );
//US     mFormatName = config->readEntry( "FileFormat" );
     mFormatName = cfg->readEntry( "FileFormat", "vcard" );
  } else {
    path = StdAddressBook::directoryName();
    mFormatName = "vcard";
  }


  FormatFactory *factory = FormatFactory::self();
  mFormat = factory->format( mFormatName );

  if ( !mFormat ) {
    mFormatName = "vcard";
    mFormat = factory->format( mFormatName );
  }

/*US
//US  qDebug("ResourceDir::ResourceDir initialized with format %s ", mFormatName.latin1());
  if (mFormatName == "vcard")
    mFormat = new VCardFormatPlugin2();
  else if (mFormatName == "binary")
    mFormat = new BinaryFormat();
  else
    qDebug("ResourceFile::init format unknown !!! %s ", mFormatName.latin1());
*/
#ifndef NO_DIRWATCH
  connect( &mDirWatch, SIGNAL( dirty(const QString&) ), SLOT( pathChanged() ) );
  connect( &mDirWatch, SIGNAL( created(const QString&) ), SLOT( pathChanged() ) );
  connect( &mDirWatch, SIGNAL( deleted(const QString&) ), SLOT( pathChanged() ) );
#endif
  setPath( path );
}

ResourceDir::~ResourceDir()
{
  delete mFormat;
  mFormat = 0;
}

void ResourceDir::writeConfig( KConfig *config )
{
    config->setGroup( "Resource_" + identifier() );
  Resource::writeConfig( config );

  config->writeEntry( "FilePath", mPath );
  config->writeEntry( "FileFormat", mFormatName );
}

Ticket *ResourceDir::requestSaveTicket()
{
  

  if ( !addressBook() ) return 0;
#ifdef ALLOW_LOCKING
  if ( !lock( mPath ) ) {
    
    return 0;
  }
#endif
  return createTicket( this );

}


bool ResourceDir::doOpen()
{
  QDir dir( mPath );
  if ( !dir.exists() ) { // no directory available
    return dir.mkdir( dir.path() );
  } else {
    QString testName = dir.entryList( QDir::Files )[0];
    if ( testName.isNull() || testName.isEmpty() ) // no file in directory
      return true;

    QFile file( mPath + "/" + testName );
    if ( file.open( IO_ReadOnly ) )
      return true;

    if ( file.size() == 0 )
      return true;

    bool ok = mFormat->checkFormat( &file );
    file.close();
    return ok;
  }
}

void ResourceDir::doClose()
{
}

bool ResourceDir::load()
{
  QDir dir( mPath );
  QStringList files = dir.entryList( QDir::Files );

  QStringList::Iterator it;
  bool ok = true;
  for ( it = files.begin(); it != files.end(); ++it ) {
    QFile file( mPath + "/" + (*it) );

    if ( !file.open( IO_ReadOnly ) ) {
      addressBook()->error( i18n( "Unable to open file '%1' for reading" ).arg( file.name() ) );
      ok = false;
      continue;
    }

    if ( !mFormat->loadAll( addressBook(), this, &file ) )
      ok = false;

    file.close();
  }

  return ok;
}

bool ResourceDir::save( Ticket *ticket )
{
  AddressBook::Iterator it;
  bool ok = true;
#ifndef NO_DIRWATCH
  mDirWatch.stopScan();
#endif
  for ( it = addressBook()->begin(); it != addressBook()->end(); ++it ) {
    if ( (*it).resource() != this || !(*it).changed() )
      continue;

    QFile file( mPath + "/" + (*it).uid() );
    if ( !file.open( IO_WriteOnly ) ) {
      addressBook()->error( i18n( "Unable to open file '%1' for writing" ).arg( file.name() ) );
      continue;
    }

    mFormat->save( *it, &file );

    // mark as unchanged
    (*it).setChanged( false );

    file.close();
  }
#ifndef NO_DIRWATCH
  mDirWatch.startScan();
#endif
  delete ticket;
#ifdef ALLOW_LOCKING
  unlock( mPath );
#endif
  return ok;
}

bool ResourceDir::lock( const QString &path )
{
#ifdef ALLOW_LOCKING
  QString p = path;
//US change the implementation how the lockfilename is getting created
//US  p.replace( QRegExp("/"), "_" );
//US  QString lockName = locateLocal( "data", "kabc/lock/" + p + ".lock" );
  KURL url(p);
  QString lockName = locateLocal( "data", "kabc/lock/" + url.fileName() + ".lock" );


  if ( QFile::exists( lockName ) ) return false;

  QString lockUniqueName;
  lockUniqueName = p + KApplication::randomString( 8 );

  url = lockUniqueName;
//US  mLockUniqueName = locateLocal( "data", "kabc/lock/" + lockUniqueName );
  mLockUniqueName = locateLocal( "data", "kabc/lock/" + url.fileName() );

  // Create unique file
  QFile file( mLockUniqueName );
  file.open( IO_WriteOnly );
  file.close();

  // Create lock file
  int result = 0;
#ifndef _WIN32_
  result = ::link( QFile::encodeName( mLockUniqueName ),
                       QFile::encodeName( lockName ) );
#endif
  if ( result == 0 ) {
    addressBook()->emitAddressBookLocked();
    return true;
  }

  // TODO: check stat

  return false;
#else
  return true;
#endif
}

void ResourceDir::unlock( const QString &path )
{
#ifdef ALLOW_LOCKING
  QString p = path;
//US change the implementation how the lockfilename is getting created
//US  p.replace( QRegExp( "/" ), "_" );
//US  QString lockName = locate( "data", "kabc/lock/" + p + ".lock" );
  KURL url(p);
  QString lockName = locate( "data", "kabc/lock/" + url.fileName() + ".lock" );

  ::unlink( QFile::encodeName( lockName ) );
  QFile::remove( mLockUniqueName );
  addressBook()->emitAddressBookUnlocked();
#else
  return;
#endif
}

void ResourceDir::setPath( const QString &path )
{
#ifndef NO_DIRWATCH
  mDirWatch.stopScan();
  mDirWatch.removeDir( mPath );

  mPath = path;

  mDirWatch.addDir( mPath, true );
  mDirWatch.startScan();
#else
  mPath = path;
#endif
//US simulate KDirWatch event
//US  pathChanged();

}

QString ResourceDir::path() const
{
  return mPath;
}

void ResourceDir::setFormat( const QString &format )
{
  mFormatName = format;

  if ( mFormat )
    delete mFormat;

  FormatFactory *factory = FormatFactory::self();
  mFormat = factory->format( mFormatName );
/*US
qDebug("ResourceDir::setFormat initialized with format %s ", format.latin1());
  if (mFormatName == "vcard")
    mFormat = new VCardFormatPlugin2();
  else if (mFormatName == "binary")
    mFormat = new BinaryFormat();
  else
    qDebug("ResourceDir::setFormat format unknown !!! %s ", format.latin1());
*/

}

QString ResourceDir::format() const
{
  return mFormatName;
}

void ResourceDir::pathChanged()
{
  if ( !addressBook() )
    return;

  QString text( i18n( "Dir resource '%1'<br> has been changed by third party.<br>Do you want to reload?").arg( mPath ) );
  if ( readOnly() || KMessageBox::questionYesNo( 0, text ) == KMessageBox::Yes ) {
    load();
    addressBook()->emitAddressBookChanged();
  }
}

void ResourceDir::removeAddressee( const Addressee& addr )
{
    QFile::remove( mPath + "/" + addr.uid() );
}

void ResourceDir::cleanUp()
{
  unlock( mPath );
}

//US #include "resourcedir.moc"
