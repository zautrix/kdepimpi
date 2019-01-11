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

$Id: resourceqtopiaconfig.cpp,v 1.4 2004/10/23 17:33:10 zautrix Exp $
*/

#include <qlabel.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdialog.h>

//#include <unistd.h>

#include <qdir.h>
#include <qfile.h>
#include "resourceqtopia.h"

#include "resourceqtopiaconfig.h"

using namespace KABC;

ResourceQtopiaConfig::ResourceQtopiaConfig( QWidget* parent,  const char* name )
    : ConfigWidget( parent, name )
{
  QGridLayout *mainLayout = new QGridLayout( this, 1, 2, 0,
      KDialog::spacingHint() );

  QLabel *label = new QLabel( i18n( "Location:" ), this );
  mFileNameEdit = new KURLRequester( this );

  connect( mFileNameEdit, SIGNAL( textChanged( const QString & ) ),
           SLOT( checkFilePermissions( const QString & ) ) );

  mainLayout->addWidget( label, 0, 0 );
  mainLayout->addWidget( mFileNameEdit, 0, 1 );
 
}

void ResourceQtopiaConfig::loadSettings( KRES::Resource *res  )
{
//US  ResourceFile *resource = dynamic_cast<ResourceFile*>( res );
  ResourceQtopia *resource = (ResourceQtopia*)( res );
  
  if ( !resource ) {
    kdDebug(5700) << "ResourceQtopiaConfig::loadSettings(): cast failed" << endl;
    return;
  }

  mFileNameEdit->setURL( resource->fileName() );
  if ( mFileNameEdit->url().isEmpty() )
    mFileNameEdit->setURL( QDir::homeDirPath() + "/Applications/addressbook/addressbook.xml" );

  //US Qtopia resources are ReadOnly by definition
  emit setPersistentReadOnly( true );
  
  //US we can not choose the filename for the qtopia backend => make it readonly.
  mFileNameEdit->setEnabled( false );
  
}

void ResourceQtopiaConfig::saveSettings( KRES::Resource *res )
{
//US  ResourceFile *resource = dynamic_cast<ResourceFile*>( res );
  ResourceQtopia *resource = (ResourceQtopia*)( res );
  
  if ( !resource ) {
    kdDebug(5700) << "ResourceQtopiaConfig::saveSettings(): cast failed" << endl;
    return;
  }

  resource->setFileName( mFileNameEdit->url() );
}

void ResourceQtopiaConfig::checkFilePermissions( const QString& fileName )
{
  // If file exist but is not writeable...
/*US  
  if ( access( QFile::encodeName( fileName ), F_OK ) == 0 )
      emit setReadOnly( access( QFile::encodeName( fileName ), W_OK ) < 0 );
*/
}

//US #include "resourceopieconfig.moc"
