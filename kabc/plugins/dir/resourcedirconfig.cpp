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

$Id: resourcedirconfig.cpp,v 1.1.1.1 2004/06/26 19:01:44 zautrix Exp $
*/

#include <qlabel.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdialog.h>

//US #include "formatfactory.h"
#include "resourcedir.h"
#include "stdaddressbook.h"

#include "resourcedirconfig.h"

using namespace KABC;

ResourceDirConfig::ResourceDirConfig( QWidget* parent,  const char* name )
    : KRES::ConfigWidget( parent, name )
{
  QGridLayout *mainLayout = new QGridLayout( this, 2, 2, 0, 
      KDialog::spacingHint() );

  QLabel *label = new QLabel( i18n( "Format:" ), this );
  mFormatBox = new KComboBox( this );

  mainLayout->addWidget( label, 0, 0 );
  mainLayout->addWidget( mFormatBox, 0, 1 );

  label = new QLabel( i18n( "Location:" ), this );
  mFileNameEdit = new KURLRequester( this );
//US  mFileNameEdit->setMode( KFile::Directory );

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

void ResourceDirConfig::setEditMode( bool value )
{
  mFormatBox->setEnabled( !value );
  mInEditMode = value;
}

void ResourceDirConfig::loadSettings( KRES::Resource *res )
{
//US  ResourceDir *resource = dynamic_cast<ResourceDir*>( res );
  ResourceDir *resource = (ResourceDir*)( res );
  
  if ( !resource ) {
    kdDebug(5700) << "ResourceDirConfig::loadSettings(): cast failed" << endl;
    return;
  }

  mFormatBox->setCurrentItem( mFormatTypes.findIndex( resource->format() ) );

  mFileNameEdit->setURL( resource->path() );
  if ( mFileNameEdit->url().isEmpty() )
    mFileNameEdit->setURL( KABC::StdAddressBook::directoryName() );
}

void ResourceDirConfig::saveSettings( KRES::Resource *res )
{
//US  ResourceDir *resource = dynamic_cast<ResourceDir*>( res );
  ResourceDir *resource = (ResourceDir*)( res );
  
  if ( !resource ) {
    kdDebug(5700) << "ResourceDirConfig::loadSettings(): cast failed" << endl;
    return;
  }

  if ( mInEditMode )
    resource->setFormat( mFormatTypes[ mFormatBox->currentItem() ] );

  resource->setPath( mFileNameEdit->url() );
}

//US #include "resourcedirconfig.moc"
