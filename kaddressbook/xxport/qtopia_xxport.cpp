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

$Id: qtopia_xxport.cpp,v 1.4 2004/10/29 11:27:16 zautrix Exp $
*/

#include <qdir.h>

#include "addressee.h"
#include "xxportmanager.h"
#include "qtopia_xxport.h"
#include <kfiledialog.h>

/*US
class QtopiaXXPortFactory : public XXPortFactory
{
  public:
    XXPortObject *xxportObject( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new QtopiaXXPort( ab, parent, name );
    }
};
*/

extern "C"
{
  void *init_microkaddrbk_qtopia_xxport()
  {
    return ( new QtopiaXXPortFactory() );
  }
}



QtopiaXXPort::QtopiaXXPort( KABC::AddressBook *ab, QWidget *parent, const char *name )
  : XXPortResourceObject( ab, parent, name )
{
  defaultFileName = QDir::homeDirPath() + "/Applications/addressbook/addressbook.xml";

  if ( mFactory && !mFactory->typeName( "qtopia" ).isEmpty() )
  {
    createImportAction( i18n( "Import xml (Qtopia)..." ) );
    createExportAction( i18n( "Export xml (Qtopia)..." ) );
  }
}

bool QtopiaXXPort::exportContacts( const KABC::AddresseeList &list, const QString &identifier )
{
    QString fn =KFileDialog::getSaveFileName( defaultFileName, i18n("Import xml file"), 0 );
    if ( fn == "" )
        return false;
    
  return _exportContacts( list, identifier, fn );
}

KABC::AddresseeList QtopiaXXPort::importContacts( const QString &identifier ) const
{
    QString fn =KFileDialog::getOpenFileName( defaultFileName, i18n("Import xml file"), 0 );
    if ( fn == "" )
        return KABC::AddresseeList() ;
  return _importContacts( identifier, fn );
}
