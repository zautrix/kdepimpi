/*
    This file is part of KAddressbook.
    Copyright (c) 2004 Ulf Schenk

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

$Id: sharpdtm_xxport.cpp,v 1.2 2004/08/03 20:49:20 ulf69 Exp $
*/

#include "addressee.h"
#include "xxportmanager.h"
#include "sharpdtm_xxport.h"

/*US
class SharpDTMXXPortFactory : public XXPortFactory
{
  public:
    XXPortObject *xxportObject( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new SharpDTMXXPort( ab, parent, name );
    }
};
*/

extern "C"
{
  void *init_microkaddrbk_sharpdtm_xxport()
  {
    return ( new SharpDTMXXPortFactory() );
  }
}

SharpDTMXXPort::SharpDTMXXPort( KABC::AddressBook *ab, QWidget *parent, const char *name )
  : XXPortResourceObject( ab, parent, name )
{
  if ( mFactory && !mFactory->typeName( "sharpdtm" ).isEmpty() )
  {
    createImportAction( i18n( "Import Sharp..." ) );
    createExportAction( i18n( "Export Sharp..." ) );
  }
}

bool SharpDTMXXPort::exportContacts( const KABC::AddresseeList &list, const QString &identifier )
{
  return _exportContacts( list, identifier, QString::null );
}

KABC::AddresseeList SharpDTMXXPort::importContacts( const QString &identifier ) const
{
  return _importContacts( identifier, QString::null );
}
