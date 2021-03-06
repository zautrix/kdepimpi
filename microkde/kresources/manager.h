/*
    This file is part of libkresources.

    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>
    Copyright (c) 2002 Jan-Pascal van Best <janpascal@vanbest.org>
    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
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

$Id: manager.h,v 1.4 2005/01/11 11:10:16 zautrix Exp $
*/

#ifndef KRESOURCES_MANAGER_H
#define KRESOURCES_MANAGER_H

#include <qdict.h>
#include <qstringlist.h>

#include "factory.h"
#include "managerimpl.h"

namespace KRES {

class Resource;

template<class T>
class ManagerListener
{
  public:
    virtual void resourceAdded( T *resource ) = 0;
    virtual void resourceModified( T *resource ) = 0;
    virtual void resourceDeleted( T *resource ) = 0;
};

// TODO:
// The resource manager should provide some signals
// to warn applications that resources have been added,
// removed or modified.
//
// The manager should also keep track of which (or at least
// how many) applications hve opened a resource, so that it
// is only closed if none of them is using it any more

template<class T>
class Manager : private ManagerImplListener
{
  public:
    class Iterator
    {
        friend class Manager;
      public:
        Iterator() {};
        Iterator( const Iterator &it ) { mIt = it.mIt; }

        T *operator*() { return static_cast<T *>( *mIt ); }
        Iterator &operator++() { mIt++; return *this; }
        Iterator &operator++(int) { mIt++; return *this; }
        Iterator &operator--() { mIt--; return *this; }
        Iterator &operator--(int) { mIt--; return *this; }
        bool operator==( const Iterator &it ) { return mIt == it.mIt; }
        bool operator!=( const Iterator &it ) { return mIt != it.mIt; }

      private:
        Resource::List::Iterator mIt;
    };

    Iterator begin()
    {
      Iterator it;
      it.mIt = mImpl->resourceList()->begin();
      return it;
    }

    Iterator end()
    {
      Iterator it;
      it.mIt = mImpl->resourceList()->end();
      return it;
    }

    class ActiveIterator
    {
        friend class Manager;
      public:
        ActiveIterator() : mList( 0 ) {};
        ActiveIterator( const ActiveIterator &it )
        {
          mIt = it.mIt;
          mList = it.mList;
        }

        T *operator*() { return static_cast<T *>( *mIt ); }
        ActiveIterator &operator++()
        {
          do { mIt++; } while ( checkActive() );
          return *this;
        }
        ActiveIterator &operator++(int)
        {
          do { mIt++; } while ( checkActive() );
          return *this;
        }
        ActiveIterator &operator--()
        {
          do { mIt--; } while ( checkActive() );
          return *this;
        }
        ActiveIterator &operator--(int)
        {
          do { mIt--; } while ( checkActive() );
          return *this;
        }
        bool operator==( const ActiveIterator &it ) { return mIt == it.mIt; }
        bool operator!=( const ActiveIterator &it ) { return mIt != it.mIt; }

      private:
        /**
          Check if iterator needs to be advanced once more.
        */
        bool checkActive()
        {
          if ( !mList || mIt == mList->end() ) return false;
          return !(*mIt)->isActive();
        }

        Resource::List::Iterator mIt;
        Resource::List *mList;
    };

    ActiveIterator activeBegin()
    {
      ActiveIterator it;
      it.mIt = mImpl->resourceList()->begin();
      it.mList = mImpl->resourceList();
      if ( it.mIt != mImpl->resourceList()->end() ) {
        if ( !(*it)->isActive() ) it++;
      }
      return it;
    }

    ActiveIterator activeEnd()
    {
      ActiveIterator it;
      it.mIt = mImpl->resourceList()->end();
      it.mList = mImpl->resourceList();
      return it;
    }

    bool isEmpty() const { return mImpl->resourceList()->isEmpty(); }

    Manager( const QString &family )
    {
      mFactory = Factory::self( family );
      // The managerimpl will use the same Factory object as the manager
      // because of the Factory::self() pattern
      mImpl = new ManagerImpl( family );
      mImpl->setListener( this );

      mListeners = new QPtrList<ManagerListener<T> >;
    }

    virtual ~Manager()
    {
      mImpl->setListener( 0 );
      delete mListeners;
      delete mImpl;
    }

    /**
      Recreate Resource objects from configuration file. If cfg is 0, read standard
      configuration file.
    */
    void readConfig( KConfig *cfg = 0 )
    {
      mImpl->readConfig( cfg );
    }

    /**
      Write configuration of Resource objects to configuration file. If cfg is 0, write
      to standard configuration file.
    */
    void writeConfig( KConfig *cfg = 0 )
    {
      mImpl->writeConfig( cfg );
    }

    /**
      Add resource to manager. This passes ownership of the Resource object
      to the manager.
    */
    void add( Resource *resource )
    {
      if ( resource ) mImpl->add( resource );
    }

    void remove( Resource *resource )
    {
      if ( resource ) mImpl->remove( resource );
    }

    T* standardResource()
    {
      return static_cast<T *>( mImpl->standardResource() );
    }

    void setStandardResource( T *resource )
    {
      if ( resource ) mImpl->setStandardResource( resource );
    }

    void setActive( Resource *resource, bool active )
    {
      if ( resource ) mImpl->setActive( resource, active );
    }

    /**
      Returns a list of the names of the reources managed by the
      Manager for this family.
    */
    QStringList resourceNames() const
    {
      return mImpl->resourceNames();
    }

    ConfigWidget *configWidget( const QString& type, QWidget *parent = 0 )
    {
      return mFactory->configWidget( type, parent );
    }

    /**
      Creates a new resource of type @param type, with default
      settings. The resource is
      not added to the manager, the application has to do that.
      Returns a pointer to a resource object or a null pointer
      if resource type doesn't exist.

      @param type   The type of the resource, one of those returned
                    by @ref resourceTypeNames()
    */
    T *createResource( const QString& type )
    {
      return (T *)( mFactory->resource( type, 0 ) );
    }

    /**
      Returns a list of the names of all available resource types.
    */
    QStringList resourceTypeNames() const
    {
      return mFactory->typeNames();
    }

    QStringList resourceTypeDescriptions() const
    {
      QStringList typeDescs;
      QStringList types = mFactory->typeNames();

      for ( QStringList::ConstIterator it = types.begin(); it != types.end(); ++it ) {
        QString desc = mFactory->typeName( *it );
        if ( !mFactory->typeDescription( *it ).isEmpty() )
          desc += " (" + mFactory->typeDescription( *it ) + ")";

        typeDescs.append( desc );
      }

      return typeDescs;
    }

    void resourceChanged( T *resource )
    {
      mImpl->resourceChanged( resource );
    }

    void addListener( ManagerListener<T> *listener )
    {
      mListeners->append( listener );
    }

    void removeListener( ManagerListener<T> *listener )
    {
      mListeners->remove( listener );
    }

    virtual void resourceAdded( Resource *res )
    {
      T* resource = (T *)( res );
      ManagerListener<T> *listener;
      for ( listener = mListeners->first(); listener; listener = mListeners->next() )
        listener->resourceAdded( resource );
    }

    virtual void resourceModified( Resource *res )
    {
     
      T* resource = (T *)( res );
      ManagerListener<T> *listener;
      for ( listener = mListeners->first(); listener; listener = mListeners->next() )
        listener->resourceModified( resource );
    }

    virtual void resourceDeleted( Resource *res )
    {
     
      T* resource = (T *)( res );
      ManagerListener<T> *listener;
      for ( listener = mListeners->first(); listener; listener = mListeners->next() ) {
        listener->resourceDeleted( resource );
      }
    }

  private:
    ManagerImpl *mImpl;
    Factory *mFactory;
    QPtrList<ManagerListener<T> > *mListeners;
};

}

#endif
