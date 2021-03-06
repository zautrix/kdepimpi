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

#ifndef KAB_EMBEDDED
#include <qapplication.h>

#include <kabc/distributionlistdialog.h>
#include <kconfig.h>
#include <klocale.h>

#include "viewmanager.h"

#endif //KAB_EMBEDDED
#include <qlayout.h>

#include <kabc/distributionlistdialog.h>
#include <kabc/addressbook.h>
#include <kdebug.h>

#include "kaddressbookview.h"

KAddressBookView::KAddressBookView( KABC::AddressBook *ab, QWidget *parent,
                                    const char *name )
    : QWidget( parent, name ), mAddressBook( ab ), mFieldList()
{
 
  initGUI();
}

KAddressBookView::~KAddressBookView()
{
 
}

QRegExp KAddressBookView::getRegExp( const QString s )
{
    QRegExp re;
    
    if ( s.length() == 3  && s.mid(1,1) == "-" ) {
        QString pattern = "^[" + s.lower() +"]";
        re.setCaseSensitive(false);
        re.setPattern( pattern );
    } else {
        QString pattern = s.lower()+"*";
        re.setWildcard(true); // most people understand these better.
        re.setCaseSensitive(false);
        re.setPattern( pattern );
    }
    return re;
}
        
void KAddressBookView::readConfig( KConfig *config )
{
  mFieldList = KABC::Field::restoreFields( config, "KABCFields" );

  if ( mFieldList.isEmpty() )
    mFieldList = KABC::Field::defaultFields();

  mDefaultFilterType = (DefaultFilterType)config->readNumEntry( "DefaultFilterType", 1 );
  mDefaultFilterName = config->readEntry( "DefaultFilterName", QString::null );
}

void KAddressBookView::writeConfig( KConfig* )
{
  // Most of writing the config is handled by the ConfigureViewDialog
}

QString KAddressBookView::selectedEmails()
{
  bool first = true;
  QString emailAddrs;
  QStringList uidList = selectedUids();
  KABC::Addressee addr;
  QString email;

  QStringList::Iterator it;
  for ( it = uidList.begin(); it != uidList.end(); ++it ) {
    addr = mAddressBook->findByUid( *it );

    if ( !addr.isEmpty() ) {
      QString m = QString::null;

      if ( addr.emails().count() > 1 )
        m = KABC::EmailSelector::getEmail( addr.emails(), addr.preferredEmail(), this );

      email = addr.fullEmail( m );

      if ( !first )
        emailAddrs += ", ";
      else
        first = false;

      emailAddrs += email;
    }
  }

  return emailAddrs;
}

KABC::Addressee::List KAddressBookView::addressees()
{
  KABC::Addressee::List addresseeList;

  KABC::AddressBook::Iterator it;
  for (it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) {
    if ( mFilter.filterAddressee( *it ) )
      addresseeList.append( *it );
  }

  return addresseeList;
}

void KAddressBookView::initGUI()
{
  // Create the layout
  QVBoxLayout *layout = new QVBoxLayout( this );

  // Add the view widget
  mViewWidget = new QWidget( this );
  layout->addWidget( mViewWidget );
}

KABC::Field::List KAddressBookView::fields() const
{
  return mFieldList;
}
KABC::Field::List KAddressBookView::allFields() const
{
  return KABC::Field::allFields(); 
}

void KAddressBookView::setFilter( const Filter &filter )
{
  mFilter = filter;
}

KAddressBookView::DefaultFilterType KAddressBookView::defaultFilterType() const
{
  return mDefaultFilterType;
}

const QString &KAddressBookView::defaultFilterName() const
{
  return mDefaultFilterName;
}

KABC::AddressBook *KAddressBookView::addressBook() const
{
  return mAddressBook;
}

QWidget *KAddressBookView::viewWidget()
{
  return mViewWidget;
}

ViewConfigureWidget *ViewFactory::configureWidget( KABC::AddressBook *ab,
                                                   QWidget *parent,
                                                   const char *name )
{
  return new ViewConfigureWidget( ab, parent, name );
}

 

#ifndef KAB_EMBEDDED
#include "kaddressbookview.moc"
#endif //KAB_EMBEDDED
