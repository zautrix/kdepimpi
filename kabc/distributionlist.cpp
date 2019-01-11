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

#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include "distributionlist.h"

using namespace KABC;

DistributionList::DistributionList( DistributionListManager *manager,
                                    const QString &name ) :
  mManager( manager ), mName( name )
{
  mManager->insert( this );
}

DistributionList::~DistributionList()
{
  mManager->remove( this );
}

void DistributionList::setName( const QString &name )
{
  mName = name;
}

QString DistributionList::name() const
{
  return mName;
}

void DistributionList::insertEntry( const Addressee &a, const QString &email )
{
    QString em = email;
    if (em.isNull() )
        em = a.preferredEmail();
    Entry e( a, em );
  QValueList<Entry>::Iterator it;
  for( it = mEntries.begin(); it != mEntries.end(); ++it ) {
    if ( (*it).addressee.uid() == a.uid() ) {
      /**
        We have to check if both email addresses contains no data,
        a simple 'email1 == email2' wont work here
       */
      if ( ( (*it).email.isNull() && em.isEmpty() ) ||
           ( (*it).email.isEmpty() && em.isNull() ) ||
           ( (*it).email == em ) ) {
          //*it = e;
        return;
      }
    }
  }
  mEntries.append( e );
}

void DistributionList::removeEntry( const Addressee &a, const QString &email )
{
  QValueList<Entry>::Iterator it;
  for( it = mEntries.begin(); it != mEntries.end(); ++it ) {
    if ( (*it).addressee.uid() == a.uid() && (*it).email == email ) {
      mEntries.remove( it  );
      return;
    }
  }
}

QStringList DistributionList::emails() const
{
  QStringList emails;

  Entry::List::ConstIterator it;
  for( it = mEntries.begin(); it != mEntries.end(); ++it ) {
    Addressee a = (*it).addressee;
    QString email = (*it).email.isEmpty() ? a.fullEmail() :
                                            a.fullEmail( (*it).email );

    if ( !email.isEmpty() ) {
      emails.append( email );
    }
  }
  
  return emails;
}

DistributionList::Entry::List DistributionList::entries() const
{
  return mEntries;
}


DistributionListManager::DistributionListManager( AddressBook *ab ) :
  mAddressBook( ab )
{
}

DistributionListManager::~DistributionListManager()
{
}

DistributionList *DistributionListManager::list( const QString &name )
{
  DistributionList *list;
  for( list = mLists.first(); list; list = mLists.next() ) {
    if ( list->name() == name ) return list;
  }
  
  return 0;
}

void DistributionListManager::insert( DistributionList *l )
{
  DistributionList *list;
  for( list = mLists.first(); list; list = mLists.next() ) {
    if ( list->name() == l->name() ) {
      mLists.remove( list );
      break;
    }
  }
  mLists.append( l );
}

void DistributionListManager::remove( DistributionList *l )
{
  DistributionList *list;
  for( list = mLists.first(); list; list = mLists.next() ) {
    if ( list->name() == l->name() ) {
      mLists.remove( list );
      return;
    }
  }
}

QStringList DistributionListManager::listNames()
{
  QStringList names;

  DistributionList *list;
  for( list = mLists.first(); list; list = mLists.next() ) {
    names.append( list->name() );
  }

  return names;
}

bool DistributionListManager::load()
{
  KSimpleConfig cfg( locateLocal( "data", "kabc/distlists" ) );

/*US  
  QMap<QString,QString> entryMap = cfg.entryMap( mAddressBook->identifier() );
  if ( entryMap.isEmpty() ) {
    kdDebug(5700) << "No distlists for '" << mAddressBook->identifier() << "'" << endl;
    return false;
  }

  cfg.setGroup( mAddressBook->identifier() );

  QMap<QString,QString>::ConstIterator it;
  for( it = entryMap.begin(); it != entryMap.end(); ++it ) {
    QString name = it.key();
*/    
  cfg.setGroup( mAddressBook->identifier() );
  //US we work in microkde with a list of distributionlists
  QStringList distlists = cfg.readListEntry( "__Lists__List__" );
  if ( distlists.isEmpty() ) {
      qDebug("no distlist for AB ");
    return false;
  }

  QStringList::ConstIterator it;
  for( it = distlists.begin(); it != distlists.end(); ++it ) {
    QString name = *it;
    
    
    QStringList value = cfg.readListEntry( name );

    

    DistributionList *list = new DistributionList( this, name );

    QStringList::ConstIterator it2 = value.begin();
    while( it2 != value.end() ) {
      QString id = *it2++;
      QString email = *it2;
      
      
      
      Addressee a = mAddressBook->findByUid( id );
      if ( !a.isEmpty() ) {
        list->insertEntry( a, email );
      }
      
      if ( it2 == value.end() ) break;
      ++it2;
    }
  }
  
  return true;
}

bool DistributionListManager::save()
{
  

  KSimpleConfig cfg( locateLocal( "data", "kabc/distlists" ) );

  cfg.deleteGroup( mAddressBook->identifier() );
  cfg.setGroup( mAddressBook->identifier() );

  DistributionList *list;
  for( list = mLists.first(); list; list = mLists.next() ) {
    kdDebug(5700) << "  Saving '" << list->name() << "'" << endl;
    QStringList value;
    DistributionList::Entry::List entries = list->entries();
    DistributionList::Entry::List::ConstIterator it;
    for( it = entries.begin(); it != entries.end(); ++it ) {
      value.append( (*it).addressee.uid() );
      if (( *it).email.isEmpty())  
          value.append( " " );
      else
          value.append( (*it).email );
      // qDebug("uid *%s* email *%s* ", (*it).addressee.uid().latin1(),(*it).email.latin1() );
    }
    cfg.writeEntry( list->name(), value );
  }

//US for microKDE we have not yet sophisticated methods to load maps.
//   Because of that we store also a list of all distributionlists.
  QStringList namelist;
  for( list = mLists.first(); list; list = mLists.next() ) {
    namelist.append( list->name() );
  }  
  cfg.writeEntry( "__Lists__List__", namelist );
  

  
  
  
      
  cfg.sync();
  
  return true;
}
#if 0
DistributionListWatcher* DistributionListWatcher::mSelf = 0;

DistributionListWatcher::DistributionListWatcher()
 : QObject( 0, "DistributionListWatcher" )
{

  mDirWatch = new KDirWatch;
  mDirWatch->addFile( locateLocal( "data", "kabc/distlists" ) );
  
  connect( mDirWatch, SIGNAL( dirty( const QString& ) ), SIGNAL( changed() ) );
  mDirWatch->startScan();
}

DistributionListWatcher::~DistributionListWatcher()
{
  delete mDirWatch;
  mDirWatch = 0;
}

DistributionListWatcher *DistributionListWatcher::self()
{
  if ( !mSelf )
    mSelf = new DistributionListWatcher();

  return mSelf;
}
#endif
//US #include "distributionlist.moc"

