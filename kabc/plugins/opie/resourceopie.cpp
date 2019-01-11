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

$Id: resourceopie.cpp,v 1.14 2004/10/13 21:27:24 ulf69 Exp $
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
//US #include <ksavefile.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

//US #include "formatfactory.h"
//US #include <qpe/qpeapplication.h>

#include <opie/ocontactaccess.h>
#include <opie/ocontactaccessbackend_xml.h>

#include "resourceopieconfig.h"
#include "stdaddressbook.h"

#include "opieconverter.h"

#include "resourceopie.h"

using namespace KABC;
extern "C"
{
  void *init_microkabc_opie()
  {
    return new KRES::PluginFactory<ResourceOpie,ResourceOpieConfig>();
  }
}

ResourceOpie::ResourceOpie( const KConfig *config )
  : Resource( config ), mAccess(0), mConverter (0)
{
  QString fileName = QDir::homeDirPath() + "/Applications/addressbook/addressbook.xml";

  KConfig *cfg = (KConfig *)config;
  if ( cfg ) {
    fileName = cfg->readEntry( "FileName", fileName );

  }

  init( fileName );
}

ResourceOpie::ResourceOpie( const QString &fileName  )
  : Resource( 0 )
{
  init( fileName );
}

void ResourceOpie::init( const QString &fileName )
{
  qDebug("ResourceOpie::init()");

  connect( &mDirWatch, SIGNAL( dirty(const QString&) ), SLOT( fileChanged() ) );
  connect( &mDirWatch, SIGNAL( created(const QString&) ), SLOT( fileChanged() ) );
  connect( &mDirWatch, SIGNAL( deleted(const QString&) ), SLOT( fileChanged() ) );

  setFileName( fileName );
}

ResourceOpie::~ResourceOpie()
{
  if (mConverter != 0)
    delete mConverter;

  if(mAccess != 0)
    delete mAccess;
}

void ResourceOpie::writeConfig( KConfig *config )
{
  Resource::writeConfig( config );

  config->writeEntry( "FileName", fileName() );
}

Ticket *ResourceOpie::requestSaveTicket()
{
  kdDebug(5700) << "ResourceOpie::requestSaveTicket()" << endl;

  qDebug("ResourceOpie::requestSaveTicket: %s", fileName().latin1());

  if ( !addressBook() ) return 0;

  if ( !lock( fileName() ) ) {
    kdDebug(5700) << "ResourceOpie::requestSaveTicket(): Unable to lock file '"
                  << fileName() << "'" << endl;
    return 0;
  }
  return createTicket( this );
}


bool ResourceOpie::doOpen()
{
  qDebug("ResourceOpie::doOpen: %s", fileName().latin1());

  OContactAccessBackend_XML* backend = new OContactAccessBackend_XML( "KDEPim/Pi", fileName() );
  mAccess = new OContactAccess("KDEPim/Pi", 0l, backend, false);

  if ( !mAccess ) {
    qDebug("Unable to load file() %s", fileName().latin1());
    return false;
  }

  mAccess->setReadAhead( 32 ); // Use ReadAhead-Cache if available


  if (mConverter == 0)
  {
    mConverter = new OpieConverter();
    bool res = mConverter->init();
    if ( !res )
    {
      qDebug("Unable to initialize opie converter. Most likely a problem with the category file");
      delete mAccess;
      mAccess = 0;
      return false;
    }
  }



  return true;
}

void ResourceOpie::doClose()
{
  qDebug("ResourceOpie::doClose: %s", fileName().latin1());

  if(mAccess)
  {
    delete mAccess;
    mAccess = 0;
  }
  // it seems so, that deletion of access deletes backend as well
  //delete backend;

  return;

}


bool ResourceOpie::load()
{
  qDebug("ResourceOpie::load: %s", fileName().latin1());

  bool res = false;

  OContactAccess::List::Iterator it;
  OContactAccess::List allList = mAccess->allRecords();
  for ( it = allList.begin(); it != allList.end(); ++it )
  {
    const OContact c = (*it);

    KABC::Addressee addressee;

    res = mConverter->opieToAddressee( c, addressee );

    if ( !addressee.isEmpty() && res )
    {
      addressee.setResource( this );
      addressBook()->insertAddressee( addressee );
    }

  }


  return true;
}

bool ResourceOpie::save( Ticket *ticket )
{
  qDebug("ResourceOpie::save: %s", fileName().latin1());

  mDirWatch.stopScan();

  KABC::AddressBook::Iterator it;
  bool res;

  for ( it = addressBook()->begin(); it != addressBook()->end(); ++it ) {
    OContact c;
    KABC::Addressee addressee = (*it);

    res = mConverter->addresseeToOpie( *it, c );
    if (res == true)
    {
      res = mAccess->add(c);
      if (res == false)
        qDebug("Unable to append Contact %s", c.fullName().latin1());
    }
    else
    {
      qDebug("Unable to convert Addressee %s", addressee.formattedName().latin1());
    }
  }

  mAccess->save();

  mDirWatch.startScan();

  delete ticket;
  unlock( fileName() );

  return true;

}

bool ResourceOpie::lock( const QString &lockfileName )
{
  qDebug("ResourceOpie::lock: %s", fileName().latin1());

  kdDebug(5700) << "ResourceOpie::lock()" << endl;

  QString fn = lockfileName;

  KURL url(fn);
  QString lockName = locateLocal( "data", "kabc/lock/" + url.fileName() + ".lock" );

  kdDebug(5700) << "-- lock name: " << lockName << endl;

  if (QFile::exists( lockName ))
  {
    qDebug("ResourceOpie::lock: lockfile %s already exists. (Delete it before continuing if nobody else is accessing the resourcefile %s)", lockName.latin1(), fileName().latin1());
    return false;
  }

  QString lockUniqueName;
  lockUniqueName = fn + KApplication::randomString( 8 );

  url = lockUniqueName;
//US  mLockUniqueName = locateLocal( "data", "kabc/lock/" + lockUniqueName );
  mLockUniqueName = locateLocal( "data", "kabc/lock/" + url.fileName() );
  kdDebug(5700) << "-- lock unique name: " << mLockUniqueName << endl;

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
}

void ResourceOpie::unlock( const QString &fileName )
{
  qDebug("ResourceOpie::unlock() %s", fileName.latin1());

  QString fn = fileName;
  KURL url(fn);
  QString lockName = locateLocal( "data", "kabc/lock/" + url.fileName() + ".lock" );

  QFile::remove( lockName );
  QFile::remove( mLockUniqueName );
  addressBook()->emitAddressBookUnlocked();
}

void ResourceOpie::setFileName( const QString &newFileName )
{
  mDirWatch.stopScan();
  mDirWatch.removeFile( fileName() );

  Resource::setFileName( newFileName );

  mDirWatch.addFile( fileName() );
  mDirWatch.startScan();

}


void ResourceOpie::fileChanged()
{
  // There is a small theoretical chance that KDirWatch calls us before
  // we are fully constructed
  if (!addressBook())
    return;

  QString text( i18n( "Opie resource '%1'<br> has been changed by third party.<br>Do you want to reload?").arg( fileName() ) );
  if ( readOnly() || KMessageBox::questionYesNo( 0, text ) == KMessageBox::Yes ) {
    load();
    addressBook()->emitAddressBookChanged();
  }

  load();
  addressBook()->emitAddressBookChanged();
}

void ResourceOpie::removeAddressee( const Addressee &addr )
{
}

void ResourceOpie::cleanUp()
{
//  qDebug("ResourceOpie::cleanup() %s", mFileName.latin1());

  unlock( fileName() );
}


//US #include "resourceopie.moc"
