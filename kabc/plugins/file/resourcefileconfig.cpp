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

$Id: resourcefileconfig.cpp,v 1.1.1.1 2004/06/26 19:01:44 zautrix Exp $
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdialog.h>
#ifndef _WIN32_
#include <unistd.h>
#endif
//US #include "formatfactory.h"
#include <qfile.h>
#include "resourcefile.h"
#include "stdaddressbook.h"

#include "resourcefileconfig.h"

using namespace KABC;

ResourceFileConfig::ResourceFileConfig( QWidget* parent,  const char* name )
    : ConfigWidget( parent, name )
{
//qDebug("ResourceFileConfig::ResourceFileConfig");
  
  QGridLayout *mainLayout = new QGridLayout( this, 2, 2, 0,
      KDialog::spacingHint() );

  QLabel *label = new QLabel( i18n( "Format:" ), this );
  mFormatBox = new KComboBox( this );

  mainLayout->addWidget( label, 0, 0 );
  mainLayout->addWidget( mFormatBox, 0, 1 );

  label = new QLabel( i18n( "Location:" ), this );
  mFileNameEdit = new KURLRequester( this );

  connect( mFileNameEdit, SIGNAL( textChanged( const QString & ) ),
           SLOT( checkFilePermissions( const QString & ) ) );

  mainLayout->addWidget( label, 1, 0 );
  mainLayout->addWidget( mFileNameEdit, 1, 1 );


/*US lets hardcode the formats instead of using a factory
  FormatFactory *factory = FormatFactory::self();
  QStringList formats = factory->formats();
  QStringList::Iterator it;
  
  for ( it = formats.begin(); it != formats.end(); ++it ) {
    FormatInfo *info = factory->info( *it );
    if ( info ) {
      mFormatTypes << (*it);
      mFormatBox->insertItem( info->nameLabel );
    }
  }
*/  
  mFormatTypes << "vcard";
  mFormatTypes << "binary";
  mFormatBox->insertItem( "vcard" );
  mFormatBox->insertItem( "binary" );

  mInEditMode = false;
}

void ResourceFileConfig::setEditMode( bool value )
{
  mFormatBox->setEnabled( !value );
  mInEditMode = value;
}

void ResourceFileConfig::loadSettings( KRES::Resource *res  )
{
//US  ResourceFile *resource = dynamic_cast<ResourceFile*>( res );
  ResourceFile *resource = (ResourceFile*)( res );
  
  if ( !resource ) {
    kdDebug(5700) << "ResourceFileConfig::loadSettings(): cast failed" << endl;
    return;
  }

  mFormatBox->setCurrentItem( mFormatTypes.findIndex( resource->format() ) );

  mFileNameEdit->setURL( resource->fileName() );
  if ( mFileNameEdit->url().isEmpty() )
    mFileNameEdit->setURL( KABC::StdAddressBook::fileName() );
}

void ResourceFileConfig::saveSettings( KRES::Resource *res )
{
//US  ResourceFile *resource = dynamic_cast<ResourceFile*>( res );
  ResourceFile *resource = (ResourceFile*)( res );
  
  if ( !resource ) {
    kdDebug(5700) << "ResourceFileConfig::saveSettings(): cast failed" << endl;
    return;
  }

  if ( !mInEditMode )
    resource->setFormat( mFormatTypes[ mFormatBox->currentItem() ] );

  resource->setFileName( mFileNameEdit->url() );
}
void ResourceFileConfig::checkFilePermissions( const QString& fileName )
{
  // If file exist but is not writeable...
#ifdef _WIN32_
  QFileInfo fi ( QFile::encodeName( fileName ) );
  if ( fi.exists() )
       emit setReadOnly(!fi.isReadable() );
#else
  if ( access( QFile::encodeName( fileName ), F_OK ) == 0 )
      emit setReadOnly( access( QFile::encodeName( fileName ), W_OK ) < 0 );
#endif
}

//US #include "resourcefileconfig.moc"
