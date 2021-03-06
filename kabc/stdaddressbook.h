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

$Id: stdaddressbook.h,v 1.3 2005/08/23 14:22:11 zautrix Exp $
*/

#ifndef KABC_STDADDRESSBOOK_H
#define KABC_STDADDRESSBOOK_H

#include "addressbook.h"

namespace KABC {

/**
  Standard KDE address book
 
  This class provides access to the standard KDE address book shared by all
  applications.
 
  It's implemented as a singleton. Use @ref self() to get the address book
  object. On the first self() call the address book also gets loaded.
  
  Example:
 
  <pre>
  KABC::AddressBook *ab = KABC::StdAddressBook::self();
 
  KABC::AddressBook::Iterator it;
  for ( it = ab->begin(); it != ab->end(); ++it ) {
    kdDebug() << "UID=" << (*it).uid() << endl;
 
    // do some other stuff
  }
 
  KABC::StdAddressBook::save();
  </pre>
*/
class StdAddressBook : public AddressBook
{
  public:

    /**
      Destructor.
     */
    ~StdAddressBook();

    /**
      Return the standard addressbook object. It also loads slow resources.
      It is the same as self(false); .
    */
    static StdAddressBook *self();
    static StdAddressBook *selfNoLoad();

    /**
      This is the same as above, but with specified
      behaviour of resource loading.

      @param onlyFastResource Only resources marked as 'fast' should be loaded
    */
    // FIXME for KDE4 return StdAddressBook and merge with the metod above -zecke
    static StdAddressBook *self( bool onlyFastResources );

    /**
      Save the standard address book to disk.
    */
    static bool save();

    /**
      Call this method in your crash handler to allow the library clean up
      possible locks.
     */
    static void handleCrash();

    /**
      Returns the default file name for vcard-based addressbook
    */
    static QString fileName();

    /**
      Returns the default directory name for vcard-based addressbook
    */
    static QString directoryName();
    
    /**
      Set the automatic save property of the address book.
      If @p enable is TRUE (default) the address book is saved at
      destruction time otherwise you have to call @ref save() to
      explicitely save it.
     */
    static void setAutomaticSave( bool enable );

    /**
      Closes the address book. Depending on @ref automaticSave() it will
      save the address book first.
    */
    static void close();

    /**
      Returns whether the address book is saved at destruction time.
      See also @ref setAutomaticSave().
     */
    static bool automaticSave();

    /**
      Returns the contact, that is associated with the owner of the
      address book. This contact should be used by other programs
      to access user specific data.
     */
    Addressee whoAmI();

    /**
      Sets the users contact. See @ref whoAmI() for more information.

      @param uid The uid of the users contact.
     */
    void setWhoAmI( const Addressee &addr );
    void init( bool onlyFastResources );

  protected:
    StdAddressBook();
    StdAddressBook( QString );
    StdAddressBook( bool onlyFastResources );


  private:
    static  QString setTempAppDir();
    static StdAddressBook *mSelf;
    static bool mAutomaticSave;
};

}
#endif
