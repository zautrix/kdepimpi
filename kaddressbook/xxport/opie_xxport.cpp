/*
    This file is part of KAddressbook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: opie_xxport.cpp,v 1.2 2004/08/03 20:49:20 ulf69 Exp $
*/

#include <qdir.h>

#include <kfiledialog.h>

#include "addressee.h"
#include "xxportmanager.h"
#include "opie_xxport.h"

/*US
class OpieXXPortFactory : public XXPortFactory
{
  public:
    XXPortObject *xxportObject( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new OpieXXPort( ab, parent, name );
    }
};
*/


extern "C"
{
  void *init_microkaddrbk_opie_xxport()
  {
    return ( new OpieXXPortFactory() );
  }
}


OpieXXPort::OpieXXPort( KABC::AddressBook *ab, QWidget *parent, const char *name )
  : XXPortResourceObject( ab, parent, name )
{
  defaultFileName = QDir::homeDirPath() + "/Applications/addressbook/addressbook.xml";

  if ( mFactory && !mFactory->typeName( "opie" ).isEmpty() )
  {
    createImportAction( i18n( "Import Opie..." ) );
    createExportAction( i18n( "Export Opie..." ) );
  }
}

bool OpieXXPort::exportContacts( const KABC::AddresseeList &list, const QString &identifier )
{
  QString fileName = KFileDialog::getSaveFileName( defaultFileName, i18n("Save file"), parentWidget() );

  if ( fileName.isEmpty() )
    return false;

  return _exportContacts( list, identifier, fileName );
}


KABC::AddresseeList OpieXXPort::importContacts( const QString &identifier ) const
{
  QString fileName = KFileDialog::getOpenFileName( defaultFileName, i18n("Load file"), parentWidget() );

  if ( fileName.isEmpty() )
    return KABC::AddresseeList();

  return _importContacts( identifier, fileName );
}
