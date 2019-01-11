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

$Id: resourceqtopia.cpp,v 1.21 2004/10/29 11:24:47 zautrix Exp $
*/
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32_
#include <unistd.h>
#endif
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <qregexp.h>
//US #include <qtimer.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
//US #include <ksavefile.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>


#include "resourceqtopiaconfig.h"
#include "stdaddressbook.h"

#include "qtopiaconverter.h"

#include "resourceqtopia.h"

using namespace KABC;

extern "C"
#ifdef _WIN32_
__declspec(dllexport)
#else
{
#endif
  void *init_microkabc_qtopia()
  {
    return new KRES::PluginFactory<ResourceQtopia,ResourceQtopiaConfig>();
  }
#ifndef _WIN32_
}
#endif

ResourceQtopia::ResourceQtopia( const KConfig *config  )
  : Resource( config ), mConverter (0)
{
  // we can not choose the filename. Therefore use the default to display
  QString fileName = QDir::homeDirPath() + "/Applications/addressbook/addressbook.xml";
  init( fileName );
}

ResourceQtopia::ResourceQtopia( const QString &fileName  )
  : Resource( 0 )
{
  init( fileName );
}

void ResourceQtopia::init( const QString &fileName )
{
#ifdef _USE_DIRWATCH_
  connect( &mDirWatch, SIGNAL( dirty(const QString&) ), SLOT( fileChanged() ) );
  connect( &mDirWatch, SIGNAL( created(const QString&) ), SLOT( fileChanged() ) );
  connect( &mDirWatch, SIGNAL( deleted(const QString&) ), SLOT( fileChanged() ) );
#endif
  setFileName( fileName );
}

ResourceQtopia::~ResourceQtopia()
{
  if (mConverter != 0)
    delete mConverter;

}

void ResourceQtopia::writeConfig( KConfig *config )
{
  Resource::writeConfig( config );
}

Ticket *ResourceQtopia::requestSaveTicket()
{
  kdDebug(5700) << "ResourceQtopia::requestSaveTicket()" << endl;

  qDebug("ResourceQtopia::requestSaveTicket: %s", fileName().latin1());

  if ( !addressBook() ) return 0;

  if ( !lock( fileName() ) ) {
    kdDebug(5700) << "ResourceQtopia::requestSaveTicket(): Unable to lock file '"
                  << fileName() << "'" << endl;
    return 0;
  }
  return createTicket( this );
}


bool ResourceQtopia::doOpen()
{
  qDebug("ResourceQtopia::doOpen(): %s", fileName().latin1());


  if (mConverter == 0)
  {
    mConverter = new QtopiaConverter();
    bool res = mConverter->init();
    if ( !res )
    {
	  QString msg("Unable to initialize qtopia converter. Most likely a problem with the category file");
      qDebug(msg);
      return false;
    }
  }

  return true;
}

void ResourceQtopia::doClose()
{
  qDebug("ResourceQtopia::doClose: %s", fileName().latin1());

 
  // it seems so, that deletion of access deletes backend as well
  //delete backend;

  return;
}

bool ResourceQtopia::load()
{

  QFile file( fileName() );
  if ( !file.open(IO_ReadOnly ) ) {
    return false;
  }

  QDomDocument doc("mydocument" );
  if ( !doc.setContent( &file ) ) {
    file.close();
    return false; 
  }
  bool res;
  QDomElement docElem = doc.documentElement( );
  QDomNode n =  docElem.firstChild();
  while ( !n.isNull() ) {
    QDomElement e = n.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == QString::fromLatin1( "Contacts" ) ) { 
        QDomNode no = e.firstChild();
        while ( !no.isNull() ) {
          QDomElement el = no.toElement();
          if ( !el.isNull() ) {
              KABC::Addressee addressee;
              res = mConverter->qtopiaToAddressee( el,  addressee ); 
              if ( !addressee.isEmpty() && res )
                  {
                      addressee.setResource( this );
                      addressBook()->insertAddressee( addressee );
                  }
          }

          no = no.nextSibling();
        }
      }
    }

    n = n.nextSibling();
  }
  return true;
}

bool ResourceQtopia::save( Ticket *ticket )
{
#ifdef _USE_DIRWATCH_
    mDirWatch.stopScan();
#endif
    KABC::AddressBook::Iterator it;
    bool res;  
    QFile file( fileName() );
    if (!file.open( IO_WriteOnly ) ) {
        return false;
    }
    QTextStream ts( &file );
    QTextStream *stream = &ts;
    stream->setEncoding( QTextStream::UnicodeUTF8 );
    *stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE Addressbook ><AddressBook>" << endl;
    *stream << " <Groups>" << endl;
    *stream << " </Groups>" << endl;
    *stream << " <Contacts> " << endl;
    // for all entries
    KABC::Addressee ab;
    for ( it = addressBook()->begin(); it != addressBook()->end(); ++it ) {
        KABC::Addressee addressee = (*it);
        res = mConverter->addresseeToQtopia( addressee, stream );
        if (!res == true)
            {
                qDebug("Unable to convert Addressee %s", addressee.formattedName().latin1());
            }
    }
    *stream << "</Contacts>" << endl;
    *stream << "</AddressBook>" << endl;
    file.close();
#ifdef _USE_DIRWATCH_  
    mDirWatch.startScan();
#endif
    delete ticket;
    unlock( fileName() );
  return true; 
}

bool ResourceQtopia::lock( const QString &lockfileName )
{
    //disabled
    return true;
  qDebug("ResourceQtopia::lock: %s", fileName().latin1());
  QString fn = lockfileName;

  KURL url(fn);
  QString lockName = locateLocal( "data", "kabc/lock/" + url.fileName() + ".lock" );

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

void ResourceQtopia::unlock( const QString &fileName )
{
    //disabled
    return;
  qDebug("ResourceQtopia::unlock() %s", fileName.latin1());

  QString fn = fileName;
  KURL url(fn);
  QString lockName = locateLocal( "data", "kabc/lock/" + url.fileName() + ".lock" );

  QFile::remove( lockName );
  QFile::remove( mLockUniqueName );
  addressBook()->emitAddressBookUnlocked();
}

void ResourceQtopia::setFileName( const QString &newFileName )
{
#ifdef _USE_DIRWATCH_
  mDirWatch.stopScan();
#endif
  mDirWatch.removeFile( fileName() );

  Resource::setFileName( newFileName );

  mDirWatch.addFile( fileName() );
#ifdef _USE_DIRWATCH_
  mDirWatch.startScan();
#endif
}


void ResourceQtopia::fileChanged()
{
  // There is a small theoretical chance that KDirWatch calls us before
  // we are fully constructed
  if (!addressBook())
    return;

  QString text( i18n( "Qtopia resource '%1'<br> has been changed by third party.<br>Do you want to reload?").arg( fileName() ) );
  if ( readOnly() || KMessageBox::questionYesNo( 0, text ) == KMessageBox::Yes ) {
    load();
    addressBook()->emitAddressBookChanged();
  }
}

void ResourceQtopia::removeAddressee( const Addressee &addr )
{
}

void ResourceQtopia::cleanUp()
{
  unlock( fileName() );
}

//US #include "resourceqtopia.moc"
