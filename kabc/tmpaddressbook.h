/*
    This file is part of libkabc.
    Copyright (c) 2004 Ulf Schenk

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

$Id: tmpaddressbook.h,v 1.1 2004/08/02 18:34:28 ulf69 Exp $
*/

#ifndef KABC_TMPADDRESSBOOK_H
#define KABC_TMPADDRESSBOOK_H

#include "addressbook.h"

namespace KABC {


/**
  Temporary KDE address book

  This class provides access to an addressbook object that has no relationship
  to other resources then the one set as standardresource.
  This class can be used to create a quick adressbook, like we need for the import/export
  functionality.
*/
class TmpAddressBook : public AddressBook
{
    Q_OBJECT

  public:

    /**
      Destructor.
     */
    ~TmpAddressBook();

    TmpAddressBook();
    TmpAddressBook( const QString &config );
};

}


#endif
