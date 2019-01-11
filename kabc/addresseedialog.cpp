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

#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qapplication.h>
#include <qregexp.h>
#include <qvbox.h>
#include <qlabel.h>

#include <klocale.h>
#include <kdebug.h>
#include <kglobalsettings.h>

#include "stdaddressbook.h"

#include "addresseedialog.h"
#include "KDGanttMinimizeSplitter.h"
//#include "addresseedialog.moc"

using namespace KABC;

AddresseeItem::AddresseeItem( QListView *parent, const Addressee &addressee ) :
  QListViewItem( parent ),
  mAddressee( addressee )
{
    setText( Name,addressee.realName());
    setText( Email, addressee.preferredEmail() );
    setText( Category, addressee.categories().join(";") );
}

QString AddresseeItem::key( int column, bool ) const
{

  if (column == Email) {
    QString value = text(Email);
    int val = value.findRev("@");
    return value.mid( val) + value.left( val );
  }
  return text(column).lower();
}

AddresseeDialog::AddresseeDialog( QWidget *parent, bool multiple ) :
  KDialogBase( KDialogBase::Plain, i18n("Select Addressee"),
               Ok|Cancel, No, parent ), mMultiple( multiple )
{
    qDebug("NEW AddresseeDialog ");
    

  KDGanttMinimizeSplitter* mMiniSplitter = new KDGanttMinimizeSplitter( Qt::Horizontal, this );
  mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Right );
  setMainWidget( mMiniSplitter );

  QWidget *listWidget = new QWidget( mMiniSplitter );

  QVBoxLayout *listLayout = new QVBoxLayout (listWidget) ;
  //topLayout->addLayout( listLayout );

  mAddresseeList = new KListView( listWidget );
  mAddresseeList->addColumn( i18n("Name") );
  mAddresseeList->addColumn( i18n("Email") );
  mAddresseeList->addColumn( i18n("Category") );
  mAddresseeList->setAllColumnsShowFocus( true );
  mAddresseeList->setFullWidth( true );
  listLayout->addWidget( mAddresseeList );
  connect( mAddresseeList, SIGNAL( doubleClicked( QListViewItem * ) ),
           SLOT( slotOk() ) );
  //QHBox* searchBox = new QHBox ( listWidget );
  QHBox * hb = new QHBox ( listWidget );
  listLayout->addWidget (  hb );
  mAddresseeEdit = new QLineEdit(  hb );
  connect( mAddresseeEdit, SIGNAL( returnPressed() ),
           SLOT( loadAddressBook() ) );
  mAddresseeEdit->setFocus();
  QPushButton *searchButton = new QPushButton( i18n("Search!"),   hb ); 
  connect ( searchButton, SIGNAL( clicked() ), SLOT( loadAddressBook() ) );
   if ( mMultiple ) {
     
    mSelectedList = new KListView( mMiniSplitter );
    mSelectedList->addColumn( i18n("Selected Name") );
    mSelectedList->addColumn( i18n("Email") );
    mSelectedList->setAllColumnsShowFocus( true );
    mSelectedList->setFullWidth( true );
    //connect( mSelectedList, SIGNAL( doubleClicked( QListViewItem * ) ),
    //       SLOT( removeSelected() ) );
    connect( mSelectedList, SIGNAL( clicked( QListViewItem * ) ),
             SLOT( removeSelected() ) );
    connect( mSelectedList, SIGNAL( returnPressed( QListViewItem *) ),
             SLOT( removeSelected() ) );

#if 0
    QPushButton *unselectButton = new QPushButton( i18n("Unselect"), selectedGroup ); 
    connect ( unselectButton, SIGNAL( clicked() ), SLOT( removeSelected() ) );

#endif
    connect( mAddresseeList, SIGNAL( clicked( QListViewItem * ) ),
             SLOT( addSelected( QListViewItem * ) ) );
    connect( mAddresseeList, SIGNAL( returnPressed( QListViewItem * ) ),
             SLOT( selectNextItem( QListViewItem * ) ) );

  }

  mAddressBook = StdAddressBook::self( true );
  connect( mAddressBook, SIGNAL( addressBookChanged( AddressBook* ) ),
           SLOT( addressBookChanged() ) );
#if 0
  connect( mAddressBook, SIGNAL( loadingFinished( Resource* ) ),
           SLOT( addressBookChanged() ) );
#endif
  loadAddressBook();  
  QValueList<int> splitterSize; 
  splitterSize.append( ( width() / 5 ) * 3 );
  splitterSize.append( ( width() / 5 ) *2 );
  mMiniSplitter->setSizes( splitterSize );
}

AddresseeDialog::~AddresseeDialog()
{
    qDebug("DELETE AddresseeDialog ");
}

void AddresseeDialog::loadAddressBook()
{
  mAddresseeList->clear();
  mItemDict.clear();
  if ( mAddresseeEdit->text().isEmpty() ) {
      AddressBook::Iterator it;
      for( it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) {
      if ( (*it).uid().left(2) == "la" && (*it).uid().left(19) == QString("last-syncAddressee-") )
            continue; 
          new AddresseeItem( mAddresseeList, (*it) );
      }
      return;
  }
  //mAddresseeEdit->completionObject()->clear();
  QRegExp re;
  re.setWildcard(true); // most people understand these better.
  re.setCaseSensitive(false);
  re.setPattern( "*"+ mAddresseeEdit->text() + "*");

  AddressBook::Iterator it;
  for( it = mAddressBook->begin(); it != mAddressBook->end(); ++it ) { 
      if ( (*it).uid().left(2) == "la" && (*it).uid().left(19) == QString("last-syncAddressee-") )
            continue; 
      QString name = (*it).realName();
      name += (*it).preferredEmail();
      name += (*it).categories().join(";");
#if QT_VERSION >= 0x030000
      if (re.search(name) != -1)
#else
      if (re.match(name) != -1)
#endif
          AddresseeItem *item = new AddresseeItem( mAddresseeList, (*it) );
  }
}

void AddresseeDialog::addCompletionItem( const QString &str, QListViewItem *item )
{
  if ( str.isEmpty() ) return;

  mItemDict.insert( str, item );
  //mAddresseeEdit->completionObject()->addItem( str );
}

void AddresseeDialog::selectItem( const QString &str )
{
  if ( str.isEmpty() ) return;

  QListViewItem *item = mItemDict.find( str );
  if ( item ) {
    mAddresseeList->blockSignals( true );
    mAddresseeList->setSelected( item, true );
    mAddresseeList->ensureItemVisible( item );
    mAddresseeList->blockSignals( false );
  }
}

void AddresseeDialog::updateEdit( QListViewItem *item )
{
  mAddresseeEdit->setText( item->text( 0 ) );
  mAddresseeEdit->setSelection( 0, item->text( 0 ).length() );
}

void AddresseeDialog::selectNextItem( QListViewItem *item )
{
    addSelected( item );
    QListViewItem *next = item->nextSibling();
    if ( next ) {
        next->setSelected( true );
        item->setSelected( false );
        mAddresseeList->setCurrentItem( next );
    }
}
void AddresseeDialog::addSelected( QListViewItem *item )
{
    AddresseeItem *addrItem = (AddresseeItem *)( item );
  if ( !addrItem ) return;

  Addressee a = addrItem->addressee();

  QListViewItem *selectedItem = mSelectedDict.find( a.uid() );
  if ( !selectedItem ) {
    selectedItem = new AddresseeItem( mSelectedList, a );
    mSelectedDict.insert( a.uid(), selectedItem );
  }
 
}

void AddresseeDialog::removeSelected()
{
  QListViewItem *item = mSelectedList->selectedItem();
  AddresseeItem *addrItem = (AddresseeItem *)( item );
  if ( !addrItem ) return;
  QListViewItem *next =  item->nextSibling();
  mSelectedDict.remove( addrItem->addressee().uid() );
  delete addrItem;
  if ( next )
      next->setSelected( true );
}

Addressee AddresseeDialog::addressee()
{
  AddresseeItem *aItem = 0;
  
  if ( mMultiple )
      aItem = (AddresseeItem *)( mSelectedList->firstChild() );
  else
      aItem = (AddresseeItem *)( mAddresseeList->selectedItem() );
 
  if (aItem) return aItem->addressee();
  return Addressee();
}

Addressee::List AddresseeDialog::addressees()
{
  Addressee::List al;
  AddresseeItem *aItem = 0;

  if ( mMultiple ) {
    QListViewItem *item = mSelectedList->firstChild();
    while( item ) {
        aItem = (AddresseeItem *)( item );
      if ( aItem ) al.append( aItem->addressee() );
      item = item->nextSibling();
    }
  }
  else
  {
      aItem = (AddresseeItem *)( mAddresseeList->selectedItem() );
    if (aItem) al.append( aItem->addressee() );
  }

  return al;
}

Addressee AddresseeDialog::getAddressee( QWidget *parent )
{
    AddresseeDialog *dlg = new AddresseeDialog( parent );
    Addressee addressee;
#ifdef DESKTOP_VERSION
    static int geoX = 0;
    static int geoY = 0;
    static int geoW = 0;
    static int geoH = 0;
    if ( !geoX  && ! geoY && !geoW &&!geoH ) {
        geoX = dlg->geometry().x();
        geoY = dlg->geometry().y();
        geoW = dlg->width();
        geoH = dlg->height();
    } else {
        dlg->show();
        dlg->setGeometry(geoX , geoY,geoW , geoH  );

    }
#endif
    int result = dlg->exec();
#ifdef DESKTOP_VERSION
    geoX = dlg->geometry().x();
    geoY = dlg->geometry().y();
    geoW = dlg->width();
    geoH = dlg->height();
#endif
    if ( result == QDialog::Accepted ) {
        addressee =  dlg->addressee();
    }

    delete dlg;
    return addressee;
}

Addressee::List AddresseeDialog::getAddressees( QWidget *parent )
{
    Addressee::List addressees;
    static bool running = false;
    if ( running ) return addressees;
    running = true;
    AddresseeDialog *dlg = new AddresseeDialog( parent, true );
#ifdef DESKTOP_VERSION
    static int geoX = 0;
    static int geoY = 0;
    static int geoW = 0;
    static int geoH = 0;
    if ( !geoX  && ! geoY && !geoW &&!geoH ) {
        geoX = dlg->geometry().x();
        geoY = dlg->geometry().y();
        geoW = dlg->width();
        geoH = dlg->height();
    } 
    dlg->show();
    dlg->setGeometry(geoX , geoY,geoW , geoH  );
#else
    dlg->showMaximized();
#endif
    int result = dlg->exec();
#ifdef DESKTOP_VERSION
    geoX = dlg->geometry().x();
    geoY = dlg->geometry().y();
    geoW = dlg->width();
    geoH = dlg->height();
#endif
    if ( result == QDialog::Accepted ) {
        addressees =  dlg->addressees();
    }

    delete dlg;
    running = false;
    return addressees;
}

void AddresseeDialog::addressBookChanged()
{
  loadAddressBook();
}
