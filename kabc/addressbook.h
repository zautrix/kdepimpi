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

$Id: addressbook.h,v 1.27 2008/09/24 22:12:29 zautrix Exp $
*/

#ifndef KABC_ADDRESSBOOK_H
#define KABC_ADDRESSBOOK_H

#include <qobject.h>

#include <kresources/manager.h>
#include <qptrlist.h>

#include "addressee.h"
#include "field.h"

namespace KABC {

class ErrorHandler;
class Resource;
class Ticket;

/**
  @short Address Book

  This class provides access to a collection of address book entries.
*/
class AddressBook : public QObject
{
    Q_OBJECT

    friend QDataStream &operator<<( QDataStream &, const AddressBook & );
    friend QDataStream &operator>>( QDataStream &, AddressBook & );
    friend class StdAddressBook;

  public:
    /**
      @short Address Book Iterator

      This class provides an iterator for address book entries.
    */
    class Iterator
    {
      public:
        Iterator();
        Iterator( const Iterator & );
      	~Iterator();

      	Iterator &operator=( const Iterator & );
        const Addressee &operator*() const;
        Addressee &operator*();
        Addressee* operator->();
        Iterator &operator++();
        Iterator &operator++(int);
        Iterator &operator--();
        Iterator &operator--(int);
        bool operator==( const Iterator &it );
        bool operator!=( const Iterator &it );

      	struct IteratorData;
        IteratorData *d;
    };

    /**
      @short Address Book Const Iterator

      This class provides a const iterator for address book entries.
    */
    class ConstIterator
    {
      public:
        ConstIterator();
        ConstIterator( const ConstIterator & );
      	~ConstIterator();

      	ConstIterator &operator=( const ConstIterator & );
        const Addressee &operator*() const;
        const Addressee* operator->() const;
        ConstIterator &operator++();
        ConstIterator &operator++(int);
        ConstIterator &operator--();
        ConstIterator &operator--(int);
        bool operator==( const ConstIterator &it );
        bool operator!=( const ConstIterator &it );

      	struct ConstIteratorData;
        ConstIteratorData *d;
    };

    /**
      Constructs a address book object.

      @param format File format class.
    */
    AddressBook();
    AddressBook( const QString &config );
    AddressBook( const QString &config, const QString &family );
    virtual ~AddressBook();

    /**
      Requests a ticket for saving the addressbook. Calling this function locks
      the addressbook for all other processes. If the address book is already
      locked the function returns 0. You need the returned @ref Ticket object
      for calling the @ref save() function.

      @see save()
    */
    Ticket *requestSaveTicket( Resource *resource=0 );

    /**
      Load address book from file.
    */
    bool load();

    /**
      Save address book. The address book is saved to the file, the Ticket
      object has been requested for by @ref requestSaveTicket().

      @param ticket a ticket object returned by @ref requestSaveTicket()
    */
    bool save( Ticket *ticket );
    bool saveAB( );
    bool saveABphone( QString fileName );
    void smplifyAddressees(); 
    void removeSyncInfo( QString syncProfile); 
    void preparePhoneSync( QString currentSyncDevice, bool isPreSync );
    void export2File( QString fileName, QString resourceName = "" );
    bool export2PhoneFormat(  QStringList uids ,QString fileName );
    int importFromFile( QString fileName,  bool replaceLabel = false, bool removeOld = false, QString resource = "" );
    void setUntagged( bool setNonSyncTagged = false, QString resource = "" );
    void removeUntagged();
    void findNewExtIds(  QString fileName, QString currentSyncDevice  );
    void storeIncludeInSync();
    void restoreIncludeInSync();
    void setIncludeInSyncExclusive( QString resource );
    void restoreIncludeInSyncExclusive();
    /**
      Returns a iterator for first entry of address book.
    */
    Iterator begin();

    /**
      Returns a const iterator for first entry of address book.
    */
    ConstIterator begin() const;

    /**
      Returns a iterator for first entry of address book.
    */
    Iterator end();

    /**
      Returns a const iterator for first entry of address book.
    */
    ConstIterator end() const;

    /**
      Removes all entries from address book.
    */
    void clear();

    /**
      Insert an Addressee object into address book. If an object with the same
      unique id already exists in the address book it it replaced by the new
      one. If not the new object is appended to the address book.
    */
    void insertAddressee( const Addressee &, bool setRev = true, bool takeResource = false);

    /**
      Removes entry from the address book.
    */
    void removeAddressee( const Addressee & );

    /**
      This is like @ref removeAddressee() just above, with the difference that
      the first element is a iterator, returned by @ref begin().
    */
    void removeAddressee( const Iterator & );

    /**
      Find the specified entry in address book. Returns end(), if the entry
      couldn't be found.
    */
    Iterator find( const Addressee & );

    /**
      Find the entry specified by an unique id. Returns an empty Addressee
      object, if the address book does not contain an entry with this id.
    */
    Addressee findByUid( const QString & );


    /**
      Returns a list of all addressees in the address book. This list can
      be sorted with @ref KABC::AddresseeList for example.
    */
    Addressee::List allAddressees();

    /**
      Find all entries with the specified name in the address book. Returns
      an empty list, if no entries could be found.
    */
    Addressee::List findByName( const QString & );

    /**
      Find all entries with the specified email address  in the address book.
      Returns an empty list, if no entries could be found.
    */
    Addressee::List findByEmail( const QString & );

    /**
      Find all entries wich have the specified category in the address book.
      Returns an empty list, if no entries could be found.
    */
    Addressee::List findByCategory( const QString & );

    /**
      Return a string identifying this addressbook.
    */
    virtual QString identifier();

    /**
      Used for debug output.
    */
    void dump() const;

    void emitAddressBookLocked() { emit addressBookLocked( this ); }
    void emitAddressBookUnlocked() { emit addressBookUnlocked( this ); }
    void emitAddressBookChanged() { emit addressBookChanged( this ); }

    /**
      Return list of all Fields known to the address book which are associated
      with the given field category.
    */
    Field::List fields( int category = Field::All );

    /**
      Add custom field to address book.

      @param label    User visible label of the field.
      @param category Ored list of field categories.
      @param key      Identifier used as key for reading and writing the field.
      @param app      String used as application key for reading and writing
                      the field.
    */
    bool addCustomField( const QString &label, int category = Field::All,
                         const QString &key = QString::null,
                         const QString &app = QString::null );


    /**
      Add address book resource.
    */
    bool addResource( Resource * );

    /**
      Remove address book resource.
    */
    void removeResources();
    bool removeResource( Resource * );

    /**
      Return pointer list of all resources.
    */
    QPtrList<Resource> resources();

    /**
      Set the @p ErrorHandler, that is used by @ref error() to
      provide gui-independend error messages.
    */
    void setErrorHandler( ErrorHandler * );

    /**
      Shows gui independend error messages.
    */
    void error( const QString& );

    /**
      Query all resources to clean up their lock files
     */
    void cleanUp();

    // sync stuff 
    //Addressee::List getExternLastSyncAddressees();
    void resetTempSyncStat(); 
    QStringList uidList(); 
    void removeSyncAddressees( bool removeDeleted = false );
    void  mergeAB( AddressBook *aBook, const QString& profile, bool isSubset );
    const Addressee findByExternUid( const QString& uid , const QString& profile  ) const;
    bool   containsExternalUid( const QString& uid  );

    void preExternSync( AddressBook* aBook, const QString& csd, bool isSubset );
    void preOLSync( AddressBook* aBook, const QString& csd);
    void postExternSync( AddressBook* aBook, const QString& csd , bool setID );
  signals:
    /**
      Emitted, when the address book has changed on disk.
    */
    void addressBookChanged( AddressBook * );

    /**
      Emitted, when the address book has been locked for writing.
    */
    void addressBookLocked( AddressBook * );

    /**
      Emitted, when the address book has been unlocked.
    */
    void addressBookUnlocked( AddressBook * );

  protected:
    void deleteRemovedAddressees();
    void setStandardResource( Resource * );
    Resource *standardResource();
    KRES::Manager<Resource> *resourceManager();

    void init(const QString &config, const QString &family);

  private:
//US    QPtrList<Resource> mDummy; // Remove in KDE 4


    struct AddressBookData;
    AddressBookData *d;
    bool blockLSEchange;
    Resource *mSavedStandardResource;
};

QDataStream &operator<<( QDataStream &, const AddressBook & );
QDataStream &operator>>( QDataStream &, AddressBook & );

}

#endif
