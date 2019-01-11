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

$Id: resourcefile.cpp,v 1.22 2005/08/19 13:29:22 zautrix Exp $
*/

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32_
#include <unistd.h>
#endif

#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qtimer.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
//US #include <ksavefile.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>

#include "formatfactory.h"

#include "resource.h"
#include "resourcefileconfig.h"
#include "stdaddressbook.h"
#define NO_DIRWATCH
#include "resourcefile.h"

//#define ALLOW_LOCKING


using namespace KABC;

extern "C"
#ifdef _WIN32_
__declspec(dllexport)
#else
{
#endif

//US  void *init_kabc_file()
  void *init_microkabc_file()
  {
    return new KRES::PluginFactory<ResourceFile,ResourceFileConfig>();
  }
#ifndef _WIN32_
}
#endif

ResourceFile::ResourceFile( const KConfig *config )
  : Resource( config ) , mFormat( 0 )
{
  QString fileName, formatName, default_fileName;

  default_fileName = StdAddressBook::fileName();
  mLastBackupDate = -1;
  KConfig *cfg = (KConfig *)config;
  if ( cfg ) {
#ifdef _WIN32_
      // we use plugins on win32. the group is stored in a static variable
      // such that group info not available on win32 plugins
      // to fix that, it would be a looooot of work
      if ( !cfg->tempGroup().isEmpty() )
          cfg->setGroup( cfg->tempGroup() );
#endif
    fileName = cfg->readEntry( "FileName", default_fileName );
    formatName = cfg->readEntry( "FileFormat", "vcard" );
    mFamily =  cfg->readEntry( "ResourceName", "std" );
    mLastBackupDate = cfg->readNumEntry( "LastBackupDate", 0 );
  } else {
    fileName = default_fileName;
    formatName = "vcard";
  }

  init( fileName, formatName );
}

ResourceFile::ResourceFile( const QString &fileName ,
                            const QString &formatName )
  : Resource( 0 )
{
//  qDebug("ResourceFile::ResourceFile : 3 %s, %s", fileName.latin1(), formatName.latin1());
   

    mLastBackupDate = -1;
  init( fileName, formatName );
}

void ResourceFile::init( const QString &fileName, const QString &formatName )
{
  mFormatName = formatName;

  FormatFactory *factory = FormatFactory::self();
  mFormat = factory->format( mFormatName );

  if ( !mFormat ) {
    mFormatName = "vcard";
    mFormat = factory->format( mFormatName );
  }

#ifndef NO_DIRWATCH
  connect( &mDirWatch, SIGNAL( dirty(const QString&) ), SLOT( fileChanged() ) );
  connect( &mDirWatch, SIGNAL( created(const QString&) ), SLOT( fileChanged() ) );
  connect( &mDirWatch, SIGNAL( deleted(const QString&) ), SLOT( fileChanged() ) );
#endif

  QString localKdeDir;
    localKdeDir = readEnvPath("LOCALMICROKDEHOME");
    if ( ! localKdeDir.isEmpty() ) {
        qDebug("LOCALMICROKDEHOME is set to: %s",localKdeDir.latin1() );
        QFileInfo fi ( fileName );
        QString localname = localKdeDir + "/apps/kabc/" + fi.fileName ();
        QFileInfo fi2 ( localname );
        if ( ! fi2.exists() ||  mFamily == "sync_res" ) {
            if (  fi.exists() && mFamily == "sync_res") {
                qDebug("LOCAL mode SYNC mode using absolute file path ");
                setFileName( fileName );
                return;
            } else {
                KMessageBox::error(0,i18n("Addressbook resource file not found:\n '%1'.\nIn LOCAL mode only resource files in\n'%2'\nare supported.\n(i.e. in the dir ./apps/kabc/ relative to the kapi(.exe) binary.)\n\nIf you are starting KA/Pi for the very first time\nyou will get this error message as well.\nIt will create the missing file automatically for you.").arg(localname).arg(localKdeDir+"/apps/kabc/") );
                setFileName( localname );
                return;
            }

        } else {
            qDebug("Local resource file found. Changing filename to: %s",localname.latin1() );
            setFileName( localname );
            return;
        }

    }
    setFileName( fileName );
}

ResourceFile::~ResourceFile()
{
  delete mFormat;
  mFormat = 0;
}

void ResourceFile::writeConfig( KConfig *config )
{

    config->setGroup( "Resource_" + identifier() );
  Resource::writeConfig( config );

  config->writeEntry( "FileName", fileName() );
  config->writeEntry( "FileFormat", mFormatName );

//    qDebug("ResourceFile::writeConfig format %s, %s", mFileName.latin1(), mFormatName.latin1());

}

Ticket *ResourceFile::requestSaveTicket()
{
  kdDebug(5700) << "ResourceFile::requestSaveTicket()" << endl;

  if ( !addressBook() ) return 0;

#ifdef ALLOW_LOCKING
  if ( !lock( mFileName ) ) {
      qDebug("unablt to lock file ");
    return 0;
  }
#endif
  return createTicket( this );
}


bool ResourceFile::doOpen()
{
  QFile file( fileName() );
  qDebug("ResourceFile::openfile %s ", fileName().latin1());

  if ( !file.exists() ) {
    // try to create the file
    bool ok = file.open( IO_WriteOnly );
    if ( ok )
      file.close();

    return ok;
  } else {
    if ( !file.open( IO_ReadWrite ) )
      return false;

    if ( file.size() < 10 ) {
      file.close();
      return true;
    }

    bool ok = mFormat->checkFormat( &file );
    file.close();

    return ok;
  }
}

void ResourceFile::doClose()
{
}

bool ResourceFile::load()
{

  QFile file( fileName() );
  if ( !file.open( IO_ReadOnly ) ) {
    addressBook()->error( i18n( "Unable to open file '%1'." ).arg( fileName() ) );
    return false;
  }

//    qDebug("ResourceFile::load format %s, %s", mFileName.latin1(), mFormatName.latin1());

  return mFormat->loadAll( addressBook(), this, &file );
}

bool ResourceFile::save( Ticket *ticket )
{
//  qDebug("ResourceFile::save format %s, %s", mFileName.latin1(), mFormatName.latin1());


  // create backup file
  QString extension = "_" + QString::number( QDate::currentDate().dayOfWeek() );

/*US we use a simpler method to create a backupfile

  (void) KSaveFile::backupFile( mFileName, QString::null
  ,extension );

  KSaveFile saveFile( mFileName );
  bool ok = false;
  if ( saveFile.status() == 0 && saveFile.file() )
  {
    mFormat->saveAll( addressBook(), this, saveFile.file() );
    ok = saveFile.close();
  }
*/

//US ToDo: write backupfile
#ifndef NO_DIRWATCH
  mDirWatch.stopScan();
#endif
  if ( mLastBackupDate >= 0  && mFamily != "sync_res") {
      KConfig conf (locateLocal("config","microkdeglobalrc"));
      conf.setGroup( "BackupSettings" );
      bool b_enabled =   conf.readBoolEntry( "BackupEnabled" );
      if ( b_enabled ) {
          int num = conf.readNumEntry( "BackupNumbers" );
          int d_count = conf.readNumEntry( "BackupDayCount" );
          bool stdDir = conf.readBoolEntry( "BackupUseDefaultDir" );
          QString bupDir = conf.readEntry( "BackupDatadir" );
          QDate reference ( 2000,1,1 );
          int daysTo = reference.daysTo ( QDate::currentDate() ); 
          bool saveDate = false;
          if ( daysTo - d_count >= mLastBackupDate ) {
                qDebug("KA: Last backup was %d days ago ", daysTo - mLastBackupDate );
                if ( stdDir )
                    bupDir = KGlobalSettings::backupDataDir();
                int retval =  KApplication::createBackup(  fileName(), bupDir, num );
                if ( retval == 0 ) {
                    qDebug("KO: Backup cancelled. Will try again tomorrow ");
                    // retval == 0 : backup skipped for today, try again tomorrow
                    mLastBackupDate = daysTo - d_count+1;
                    saveDate = true;
                } else if ( retval == 1 ){
                    qDebug("KO: Backup created.");
                    // backup ok
                    mLastBackupDate =  daysTo;
                    saveDate = true;
                } else if ( retval == 2 ){
                    qDebug("KO: Backup globally cancelled.");
                    // backup globally cancelled
                    b_enabled = false;
                }
                if ( !b_enabled ) {
                    conf.writeEntry( "mBackupEnabled", false );
                }
                if ( saveDate ) {
                    KConfig config ( locateLocal("config","kabcrc") );
                    config.setGroup( "Resource_" + identifier() );
                    config.writeEntry( "LastBackupDate", mLastBackupDate );
                }
          }
      }
  }
  QFile info;
  info.setName( fileName() );
  bool ok = info.open( IO_WriteOnly );
  if ( ok ) {
    mFormat->saveAll( addressBook(), this, &info );

    info.close();
    ok = true;
  }
  else {

  }

  if ( !ok )
    addressBook()->error( i18n( "Unable to save file '%1'." ).arg( fileName() ) );
#ifndef NO_DIRWATCH
  mDirWatch.startScan();
#endif
  delete ticket;
#ifdef ALLOW_LOCKING
  unlock( mFileName );
#endif

  return ok;
}

bool ResourceFile::lock( const QString &fileName )
{
#ifdef ALLOW_LOCKING


  QString fn = fileName;

//US change the implementation how the lockfilename is getting created
//US  fn.replace( QRegExp("/"), "_" );
//US  QString lockName = locateLocal( "data", "kabc/lock/" + fn + ".lock" );

  KURL url(fn);
  QString lockName = locateLocal( "data", "kabc/lock/" + url.fileName() + ".lock" );



  if (QFile::exists( lockName )) return false;

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
#else
  return true;
#endif
}

void ResourceFile::unlock( const QString &fileName )
{
#ifdef ALLOW_LOCKING
  QString fn = fileName;
//US change the implementation how the lockfilename is getting created
//US  fn.replace( QRegExp( "/" ), "_" );
//US  QString lockName = locateLocal( "data", "kabc/lock/" + fn + ".lock" );
//US  QString lockName = fn + ".lock";
  KURL url(fn);
  QString lockName = locateLocal( "data", "kabc/lock/" + url.fileName() + ".lock" );

  QFile::remove( lockName );
  QFile::remove( mLockUniqueName );
  addressBook()->emitAddressBookUnlocked();
#else
  return;
#endif
}

void ResourceFile::setFileName( const QString &fileName )
{
#ifndef NO_DIRWATCH
  mDirWatch.stopScan();
  mDirWatch.removeFile( mFileName );
  mFileName = fileName;


  mDirWatch.addFile( mFileName );
  mDirWatch.startScan();
#else
  mFileName2 = fileName;
#endif

//US simulate KDirWatch event
//US  fileChanged();
}

QString ResourceFile::fileName() const
{
  return mFileName2;
}

void ResourceFile::setFormat( const QString &format )
{
  mFormatName = format;
  delete mFormat;

  FormatFactory *factory = FormatFactory::self();
  mFormat = factory->format( mFormatName );
/*US
//qDebug("ResourceFile::setFormat initialized with format %s ", format.latin1());
  if (mFormatName == "vcard") {
    mFormat = new VCardFormatPlugin2();
//    qDebug("ResourceFile::setFormat format %s", mFormatName.latin1());
  }
  else if (mFormatName == "binary") {
    mFormat = new BinaryFormat();
//    qDebug("ResourceFile::setFormat format %s", mFormatName.latin1());
  }
  else
    qDebug("ResourceFile::setFormat format unknown !!! %s ", format.latin1());
*/

}

QString ResourceFile::format() const
{
  return mFormatName;
}

void ResourceFile::fileChanged()
{
  // There is a small theoretical chance that KDirWatch calls us before
  // we are fully constructed
  if (!addressBook())
    return;


  QString text( i18n( "File resource '%1'<br> has been changed by third party.<br>Do you want to reload?").arg( fileName() ) );
  if ( readOnly() || KMessageBox::questionYesNo( 0, text ) == KMessageBox::Yes ) {
    load();
    addressBook()->emitAddressBookChanged();
  }
}

void ResourceFile::removeAddressee( const Addressee &addr )
{
  QFile::remove( QFile::encodeName( locateLocal( "data", "kabc/photos/" ) + addr.uid() ) );
  QFile::remove( QFile::encodeName( locateLocal( "data", "kabc/logos/" ) + addr.uid() ) );
  QFile::remove( QFile::encodeName( locateLocal( "data", "kabc/sounds/" ) + addr.uid() ) );
}

void ResourceFile::cleanUp()
{
  unlock( fileName() );
}

//US #include "resourcefile.moc"
