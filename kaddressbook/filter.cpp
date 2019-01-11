/*                                                                      
    This file is part of KAddressBook.                                  
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>                   
                                                                        
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

#include <kconfig.h>
#include <kconfigbase.h>
#include <kdebug.h>

#include "kabprefs.h"

#include "filter.h"
#include <secrecy.h>

Filter::Filter()
  : mName( QString::null ), mMatchRule( NotMatching ), mEnabled( true ),
    mInternal( false )
{
    mCriteria = ShowPublic | ShowPrivate| ShowConfidential ;
    noName = true;
}

Filter::Filter( const QString &name )
  : mName( name ), mMatchRule( NotMatching ), mEnabled( true ),
    mInternal( false )
{
    mCriteria = ShowPublic | ShowPrivate| ShowConfidential ;
    noName = false;
}

Filter::~Filter()
{
}

void Filter::setName( const QString &name )
{
  mName = name;
  noName = false;
}

const QString &Filter::name() const
{
  return mName;
}

bool Filter::isInternal() const
{
  return mInternal;
}

void Filter::apply( KABC::Addressee::List &addresseeList )
{  
    if ( noName )
        return;
  KABC::Addressee::List::Iterator iter;
  for ( iter = addresseeList.begin(); iter != addresseeList.end(); ) {
    if ( filterAddressee( *iter ) )
      ++iter;
    else
    {
#ifndef KAB_EMBEDDED
      iter = addresseeList.erase( iter );
#else //KAB_EMBEDDED
      iter = addresseeList.remove( iter );
#endif //KAB_EMBEDDED
    }
  }
}

bool Filter::filterAddressee( const KABC::Addressee &a )
{
  
    if ( noName )
        return true;
    //qDebug("Filter::filterAddressee %s",mName.latin1() );
    switch ( a.secrecy().type()) {
    case KABC::Secrecy::Public: 
        if (! (mCriteria & ShowPublic ))
            return false;
        break;
    case KABC::Secrecy::Private:
        if (! (mCriteria & ShowPrivate ))
            return false;
        break;
    case KABC::Secrecy::Confidential:
        if (! (mCriteria & ShowConfidential ))
            return false;
        break;
    default:
        return false;
        break;
    }
  QStringList::Iterator iter;
  iter = mCategoryList.begin();

  if ( iter == mCategoryList.end() )
      return ( !(mMatchRule == Matching) );

  for ( ; iter != mCategoryList.end(); ++iter ) {
    if ( a.hasCategory( *iter ) )
      return ( mMatchRule == Matching );
  }
  
  return !( mMatchRule == Matching );
}

void Filter::setEnabled( bool on )
{
  mEnabled = on;
}

bool Filter::isEnabled() const
{
  return mEnabled;
}

void Filter::setCategories( const QStringList &list )
{
  mCategoryList = list;
}
    
const QStringList &Filter::categories() const
{
  return mCategoryList;
}

void Filter::save( KConfig *config )
{
  config->writeEntry( "Name", mName );
  config->writeEntry( "Enabled", mEnabled );
  config->writeEntry( "Categories", mCategoryList );
  config->writeEntry( "MatchRule", (int)mMatchRule );
  config->writeEntry( "Criteria", (int)mCriteria );
}

void Filter::restore( KConfig *config )
{ 
    noName = false;
  mName = config->readEntry( "Name", "<internal error>" );
  mEnabled = config->readBoolEntry( "Enabled", true );
  mCategoryList = config->readListEntry( "Categories" );
  mMatchRule = (MatchRule)config->readNumEntry( "MatchRule", Matching );
  mCriteria = config->readNumEntry( "Criteria", (ShowPublic | ShowPrivate| ShowConfidential ) );
}

void Filter::save( KConfig *config, QString baseGroup, Filter::List &list )
{
  {
    KConfigGroupSaver s( config, baseGroup );

    // remove the old filters
    uint count = config->readNumEntry( "Count" );
    /*   // memory access violation here
    for ( uint i = 0; i < count; ++i )
      config->deleteGroup( QString( "%1_%2" ).arg( baseGroup ).arg( i ) );
    */
  }
  
  int index = 0;
  Filter::List::Iterator iter;
  for ( iter = list.begin(); iter != list.end(); ++iter ) {
    if ( !(*iter).mInternal ) {
      KConfigGroupSaver s( config, QString( "%1_%2" ).arg( baseGroup ).arg( index ) );
                                                     
      (*iter).save( config );
      index++;
    }
  }

  KConfigGroupSaver s( config, baseGroup );

  config->writeEntry( "Count", index );

}
                     
Filter::List Filter::restore( KConfig *config, QString baseGroup )
{
  Filter::List list;
  int count = 0;
  Filter f;
  
  {
    KConfigGroupSaver s( config, baseGroup );
    count = config->readNumEntry( "Count", 0 );
  }
  
  for ( int i = 0; i < count; i++ ) {
    {
      KConfigGroupSaver s( config, QString( "%1_%2" ).arg( baseGroup ).arg( i ) );
      f.restore( config );
    }

    list.append( f );
  }

  if ( list.isEmpty()) {
      QStringList cats = KABPrefs::instance()->mCustomCategories;
      for ( QStringList::Iterator it = cats.begin(); it != cats.end(); ++it ) {
          Filter filter; 
          filter.noName = false;
          filter.mName = *it;
          filter.mEnabled = true;
          filter.mCategoryList = *it;
          filter.mMatchRule = Matching;
          filter.mInternal = true;
          list.append( filter );
      }
  }
  return list;
}

void Filter::setMatchRule( MatchRule rule )
{
  mMatchRule = rule;
}
    
Filter::MatchRule Filter::matchRule() const
{
  return mMatchRule;
}
