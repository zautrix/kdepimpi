/*
    This file is part of libkabc.
    Copyright (c) 2002 Jost Schenck <jost@schenck.de>
                  2003 Tobias Koenig <tokoe@kde.org>

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

$Id: addresseelist.cpp,v 1.1.1.1 2004/06/26 19:01:38 zautrix Exp $
*/

#include <kdebug.h>
//US
#include <qtl.h>


#include "addresseelist.h"
#include "field.h"

using namespace KABC;

//
//
// Traits
//
//

bool SortingTraits::Uid::eq( const Addressee &a1, const Addressee &a2 )
{
  // locale awareness doesn't make sense sorting ids
  return ( QString::compare( a1.uid(), a2.uid() ) == 0 );
}

bool SortingTraits::Uid::lt( const Addressee &a1, const Addressee &a2 )
{
  // locale awareness doesn't make sense sorting ids
  return ( QString::compare( a1.uid(), a2.uid() ) < 0 );
}

bool SortingTraits::Name::eq( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  return ( QString::compare( a1.name(), a2.name() ) == 0 );
}

bool SortingTraits::Name::lt( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  return ( QString::compare( a1.name(), a2.name() ) < 0 );
}

bool SortingTraits::FormattedName::eq( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  return ( QString::compare( a1.formattedName(), a2.formattedName() ) == 0 );
}

bool SortingTraits::FormattedName::lt( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  return ( QString::compare( a1.formattedName(), a2.formattedName() ) < 0 );
}

bool SortingTraits::FamilyName::eq( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  return ( QString::compare( a1.familyName(), a2.familyName() ) == 0 
           && QString::compare( a1.givenName(), a2.givenName() ) == 0 );
}

bool SortingTraits::FamilyName::lt( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  int family = QString::compare( a1.familyName(), a2.familyName() );
  if ( 0 == family ) {
    return ( QString::compare( a1.givenName(), a2.givenName() ) < 0 );
  } else {
    return family < 0;
  }
}

bool SortingTraits::GivenName::eq( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  return ( QString::compare( a1.givenName(), a2.givenName() ) == 0 
           && QString::compare( a1.familyName(), a2.familyName() ) == 0 );
}

bool SortingTraits::GivenName::lt( const Addressee &a1, const Addressee &a2 )
{
//US QString::localeAwareCompare is not available in my distribution. Redefine it to compare
  int given = QString::compare( a1.givenName(), a2.givenName() );
  if ( 0 == given ) {
    return ( QString::compare( a1.familyName(), a2.familyName() ) < 0 );
  } else {
    return given < 0;
  }
}

//
//
// AddresseeList
//
//

AddresseeList::AddresseeList()
  : QValueList<Addressee>()
{
  mReverseSorting = false;
  mActiveSortingCriterion = FormattedName;
  mActiveSortingField = 0;
}

AddresseeList::~AddresseeList()
{
}

AddresseeList::AddresseeList( const AddresseeList &l )
  : QValueList<Addressee>( l )
{
  mReverseSorting = l.reverseSorting();
  mActiveSortingCriterion = l.sortingCriterion();
}

AddresseeList::AddresseeList( const QValueList<Addressee> &l )
  : QValueList<Addressee>( l )
{
  mReverseSorting = false;
}

void AddresseeList::dump() const
{
  kdDebug(5700) << "AddresseeList {" << endl;
  kdDebug(5700) << "reverse order: " << ( mReverseSorting ? "true" : "false" ) << endl;

  QString crit;
  if ( Uid == mActiveSortingCriterion ) {
    crit = "Uid";
  } else if ( Name == mActiveSortingCriterion ) {
    crit = "Name";
  } else if ( FormattedName == mActiveSortingCriterion ) {
    crit = "FormattedName";
  } else if ( FamilyName == mActiveSortingCriterion ) {
    crit = "FamilyName";
  } else if ( GivenName == mActiveSortingCriterion ) {
    crit = "GivenName";
  } else {
    crit = "unknown -- update dump method";
  }

  kdDebug(5700) << "sorting criterion: " << crit << endl;

//US  
//US  for ( const_iterator it = begin(); it != end(); ++it )
  for ( ConstIterator it = begin(); it != end(); ++it )
    (*it).dump();

  kdDebug(5700) << "}" << endl;
}

void AddresseeList::sortBy( SortingCriterion c )
{
  mActiveSortingCriterion = c;
  if ( Uid == c ) {
    sortByTrait<SortingTraits::Uid>();
  } else if ( Name == c ) {
    sortByTrait<SortingTraits::Name>();
  } else if ( FormattedName == c ) {
    sortByTrait<SortingTraits::FormattedName>();
  } else if ( FamilyName == c ) {
    sortByTrait<SortingTraits::FamilyName>();
  } else if ( GivenName==c ) {
    sortByTrait<SortingTraits::GivenName>();
  } else {
    kdError(5700) << "AddresseeList sorting criterion passed for which a trait is not known. No sorting done." << endl;
  }
}

void AddresseeList::sort()
{
  sortBy( mActiveSortingCriterion );
}

template<class Trait>
void AddresseeList::sortByTrait()
{
  // FIXME: better sorting algorithm, bubblesort is not acceptable for larger lists.
  //
  // for i := 1 to n - 1 
  //   do for j := 1 to n - i 
  //     do if A[j] > A[j+1] 
  //       then temp :=  A[j] 
  //         A[j] := A[j + 1] 
  //         A[j + 1 ] := temp 

//US  iterator i1 = begin();
  Iterator i1 = begin();
//US  iterator endIt = end();
  Iterator endIt = end();
  --endIt;
  if ( i1 == endIt ) // don't need sorting
    return;

//US  iterator i2 = endIt;
  Iterator i2 = endIt;
  while( i1 != endIt ) {
//US    iterator j1 = begin();
    Iterator j1 = begin();
//US    iterator j2 = j1;
    Iterator j2 = j1;
    ++j2;
    while( j1 != i2 ) {
      if ( !mReverseSorting && Trait::lt( *j2, *j1 )
           || mReverseSorting && Trait::lt( *j1, *j2 ) ) {
        qSwap( *j1, *j2 );
      }
      ++j1;
      ++j2;
    }
    ++i1;
    --i2;
  }
}

void AddresseeList::sortByField( Field *field )
{
  if ( field )
    mActiveSortingField = field;

  if ( !mActiveSortingField ) {
    kdWarning(5700) << "sortByField called with no active sort field" << endl;
    return;
  }

  if ( count() == 0 )
    return;

  quickSortByField( 0, count() - 1 );
}

void AddresseeList::quickSortByField( int left, int right )
{
  int i = left;
  int j = right;
  int mid = ( left + right ) / 2;

//US  iterator x = at( mid );
    ConstIterator x = at( mid );

  do {
    if ( !mReverseSorting ) {
      //US  QString::localeAwareCompare was not available. Used compare instead.
      while ( QString::compare( mActiveSortingField->value( *at( i ) ).upper(), mActiveSortingField->value( *x ).upper() ) < 0 )
        i++;
      //US  QString::localeAwareCompare was not available. Used compare instead.
      while ( QString::compare( mActiveSortingField->value( *at( j ) ).upper(), mActiveSortingField->value( *x ).upper() ) > 0 )
        j--;
    } else {
      //US  QString::localeAwareCompare was not available. Used compare instead.
      while ( QString::compare( mActiveSortingField->value( *at( i ) ).upper(), mActiveSortingField->value( *x ).upper() ) > 0 )
        i++;
      //US  QString::localeAwareCompare was not available. Used compare instead.
      while ( QString::compare( mActiveSortingField->value( *at( j ) ).upper(), mActiveSortingField->value( *x ).upper() ) < 0 )
        j--;
    }
    if ( i <= j ) {
      qSwap( *at( i ), *at( j ) );
      i++;
      j--;
    }
  } while ( i <= j );

  if ( left < j ) quickSortByField( left, j );
  if ( right > i ) quickSortByField( i, right );
}
