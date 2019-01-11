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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: filtereditdialog.cpp,v 1.4 2005/03/25 00:08:56 zautrix Exp $
*/

#include <qbuttongroup.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qwidget.h>

#include <kapplication.h>
#include <kbuttonbox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klistbox.h>
#include <klistview.h>
#include <klocale.h>
#include <kglobal.h>

#include "kabprefs.h"
#include "filtereditdialog.h"

FilterEditDialog::FilterEditDialog( QWidget *parent, const char *name )
  : KDialogBase( Plain, i18n( "Edit Address Book Filter" ),
                 Help | Ok | Cancel, Ok, parent, name, /*US false*/ true, true )
{
  initGUI();

  QStringList cats = KABPrefs::instance()->mCustomCategories;

  QStringList::Iterator iter;
  for ( iter = cats.begin(); iter != cats.end(); ++iter )
    mCategoriesView->insertItem( new QCheckListItem( mCategoriesView, (*iter), QCheckListItem::CheckBox ) );
  filterNameTextChanged( mNameEdit->text() );
}

FilterEditDialog::~FilterEditDialog()
{
}

void FilterEditDialog::setFilter( const Filter &filter )
{
  mNameEdit->setText( filter.name() );

  QStringList categories = filter.categories();
  QListViewItem *item = mCategoriesView->firstChild();
  while ( item != 0 ) {
    if ( categories.contains( item->text( 0 ) ) ) {
      QCheckListItem *checkItem = static_cast<QCheckListItem*>( item );
      checkItem->setOn( true );
    }

    item = item->nextSibling();
  }

  if ( filter.matchRule() == Filter::Matching )
    mMatchRuleGroup->setButton( 0 );
  else
    mMatchRuleGroup->setButton( 1 );

  int c = filter.criteria() ;
  mPublic->setChecked(c &Filter::ShowPublic);
  mPrivate->setChecked(c & Filter::ShowPrivate);
  mConfidential->setChecked(c & Filter::ShowConfidential);
}

Filter FilterEditDialog::filter()
{
  Filter filter;

  filter.setName( mNameEdit->text() );

  QStringList categories;
  QListViewItem *item = mCategoriesView->firstChild();
  while ( item != 0 ) {
    QCheckListItem *checkItem = static_cast<QCheckListItem*>( item );
    if ( checkItem->isOn() )
      categories.append( item->text( 0 ) );

    item = item->nextSibling();
  }
  filter.setCategories( categories );

  if ( mMatchRuleGroup->find( 0 )->isOn() )
    filter.setMatchRule( Filter::Matching );
  else
    filter.setMatchRule( Filter::NotMatching );

  int c = 0;
  if (mPublic->isChecked()) c |= Filter::ShowPublic;
  if (mPrivate->isChecked()) c |= Filter::ShowPrivate;
  if (mConfidential->isChecked()) c |= Filter::ShowConfidential;
  filter.setCriteria( c ) ;

  return filter;
}

void FilterEditDialog::initGUI()
{
#ifndef KAB_EMBEDDED  
  resize( 490, 300 );
#else //KAB_EMBEDDED  
  resize( KMIN(KGlobal::getDesktopWidth()-10, 490), KMIN(KGlobal::getDesktopHeight()-50, 300));
#endif //KAB_EMBEDDED  
  

  QWidget *page = plainPage();
  QLabel *label;

  QGridLayout *topLayout = new QGridLayout( page, 3, 2, 0, spacingHint() );

  label = new QLabel( i18n( "Name" ), page );
  mNameEdit = new KLineEdit( page );
  mNameEdit->setFocus();
  topLayout->addWidget( label, 0, 0 );
  topLayout->addWidget( mNameEdit, 0, 1 );
  connect( mNameEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( filterNameTextChanged( const QString&) ) );

  mCategoriesView = new KListView( page );
  mCategoriesView->addColumn( i18n( "Categories" ) );
  topLayout->addMultiCellWidget( mCategoriesView, 1, 1, 0, 1 );

  mMatchRuleGroup = new QHButtonGroup( i18n( "Category rule" ), page );
  mMatchRuleGroup->setExclusive( true );
  QRadioButton *radio = new QRadioButton( i18n( "Include categories" ), mMatchRuleGroup );
  //mMatchRuleGroup->insert( radio );
  radio = new QRadioButton( i18n( "Exclude categories" ), mMatchRuleGroup );
  radio->setChecked( true );
  //mMatchRuleGroup->insert( radio );
  topLayout->addMultiCellWidget( mMatchRuleGroup, 2, 2, 0, 1 );

  QHButtonGroup * mMatchPPCGroup = new QHButtonGroup(i18n( "Include contacts, that are:" ), page );
  mPublic = new QCheckBox( i18n( "public" ), mMatchPPCGroup );
  mPrivate = new QCheckBox( i18n( "private" ), mMatchPPCGroup );
  mConfidential = new QCheckBox( i18n( "confidential" ), mMatchPPCGroup );
  mPublic->setChecked( true );
  mPrivate->setChecked( true );
  mConfidential->setChecked( true );
  topLayout->addMultiCellWidget( mMatchPPCGroup, 3, 3, 0, 1 );

}

void FilterEditDialog::filterNameTextChanged( const QString &text )
{
  enableButtonOK( !text.isEmpty() );
}

void FilterEditDialog::slotHelp()
{
#ifndef KAB_EMBEDDED
  kapp->invokeHelp( "using-filters" );
#endif //KAB_EMBEDDED
}

FilterDialog::FilterDialog( QWidget *parent, const char *name )
  : KDialogBase( Plain, i18n( "Edit Address Book Filters" ),
                 Ok | Cancel, Ok, parent, name, /*US false*/true, true )
{
  initGUI();
}

FilterDialog::~FilterDialog()
{
}

void FilterDialog::setFilters( const Filter::List &list )
{
  mFilterList.clear();
  mInternalFilterList.clear();

  Filter::List::ConstIterator it;
  for ( it = list.begin(); it != list.end(); ++it ) {
    if ( (*it).isInternal() )
      mInternalFilterList.append( *it );
    else
      mFilterList.append( *it );
  }

  refresh();
}

Filter::List FilterDialog::filters() const
{
  Filter::List list = mFilterList + mInternalFilterList;
  return list;
}

void FilterDialog::add()
{
#ifndef KAB_EMBEDDED  
  FilterEditDialog dlg( this );
#else //KAB_EMBEDDED  
  FilterEditDialog dlg( this->parentWidget() );
#endif //KAB_EMBEDDED  

  if ( dlg.exec() )
    mFilterList.append( dlg.filter() );

  refresh();

  mFilterListBox->setCurrentItem( mFilterListBox->count() - 1 );
}

void FilterDialog::edit()
{
  FilterEditDialog dlg( this );

  uint pos = mFilterListBox->currentItem();

  dlg.setFilter( mFilterList[ pos ] );

  if ( dlg.exec() ) {
    mFilterList.remove( mFilterList.at( pos ) );
    mFilterList.insert( mFilterList.at( pos ), dlg.filter() );
  }

  refresh();

  mFilterListBox->setCurrentItem( pos );
}

void FilterDialog::remove()
{
  mFilterList.remove( mFilterList.at( mFilterListBox->currentItem() ) );

  selectionChanged( 0 );

  refresh();
}

void FilterDialog::refresh()
{
  mFilterListBox->clear();

  Filter::List::Iterator iter;
  for ( iter = mFilterList.begin(); iter != mFilterList.end(); ++iter )
    mFilterListBox->insertItem( (*iter).name() );
}

void FilterDialog::selectionChanged( QListBoxItem *item )
{
  bool state = ( item != 0 );

  mEditButton->setEnabled( state );
  mRemoveButton->setEnabled( state );
}

void FilterDialog::initGUI()
{
#ifndef KAB_EMBEDDED  
  resize( 330, 200 );
#else //KAB_EMBEDDED  
  resize( KMIN(KGlobal::getDesktopWidth()-10, 330), KMIN(KGlobal::getDesktopHeight()-50, 200));
#endif //KAB_EMBEDDED  

  QWidget *page = plainPage();

  QGridLayout *topLayout = new QGridLayout( page, 1, 2, 0, spacingHint() );

  mFilterListBox = new KListBox( page );
  topLayout->addWidget( mFilterListBox, 0, 0 );
  connect( mFilterListBox, SIGNAL( selectionChanged( QListBoxItem * ) ),
           SLOT( selectionChanged( QListBoxItem * ) ) );
  connect( mFilterListBox, SIGNAL( doubleClicked ( QListBoxItem * ) ),
           SLOT( edit() ) );

  KButtonBox *buttonBox = new KButtonBox( page, Vertical );
  buttonBox->addButton( i18n( "&Add..." ), this, SLOT( add() ) );
  mEditButton = buttonBox->addButton( i18n( "&Edit..." ), this, SLOT( edit() ) );
  mEditButton->setEnabled( false );
  mRemoveButton = buttonBox->addButton( i18n( "&Remove" ), this, SLOT( remove() ) );
  mRemoveButton->setEnabled( false );

  buttonBox->layout();
  topLayout->addWidget( buttonBox, 0, 1 );
}

#ifndef KAB_EMBEDDED
#include "filtereditdialog.moc"
#endif //KAB_EMBEDDED
