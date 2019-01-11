/*
    This file is part of KAddressBook.
    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>

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

#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qcombobox.h>

#include <kdialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kglobal.h>
#include <kglobal.h>
#include "kabprefs.h"

#include "incsearchwidget.h"

IncSearchWidget::IncSearchWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
#ifndef KAB_EMBEDDED
//US  setCaption( i18n( "Incremental Search" ) );
#endif //KAB_EMBEDDED

  QHBoxLayout *layout = new QHBoxLayout( this, 2, KDialog::spacingHint() );

#ifdef DESKTOP_VERSION
  QLabel *label = new QLabel( i18n( "Search:" ), this );
  label->setAlignment( QLabel::AlignVCenter | QLabel::AlignRight );
  layout->addWidget( label );
#endif //KAB_EMBEDDED

  mSearchText = new KLineEdit( this );
  layout->addWidget( mSearchText );
// #ifdef KAB_EMBEDDED
//   if (KGlobal::getOrientation() == KGlobal::Portrait)
//     mSearchText->setMaximumWidth(30);
// #endif //KAB_EMBEDDED
  //mSearchText->setMaximumWidth(60);

  
  mFieldCombo = new QComboBox( false, this );
  layout->addWidget( mFieldCombo );
  mFieldCombo->setMaximumHeight( 34 );
  QToolTip::add( mFieldCombo, i18n( "Select Incremental Search Field" ) );

// #ifndef KAB_EMBEDDED
//   resize( QSize(420, 50).expandedTo( sizeHint() ) );
// #else //KAB_EMBEDDED
//   resize( QSize(30, 10).expandedTo( sizeHint() ) );
// #endif //KAB_EMBEDDED


  // for performance reasons, we do a search on the pda only after return is pressed
  connect( mSearchText, SIGNAL( textChanged( const QString& ) ),
           SLOT( announceDoSearch2() ) );
  connect( mFieldCombo, SIGNAL( activated( const QString& ) ),
           SLOT( announceDoSearch2() ) );

  connect( mSearchText, SIGNAL( returnPressed() ),
           SLOT( announceDoSearch() ) );
  connect( mFieldCombo, SIGNAL( activated( const QString& ) ),
           SLOT( announceFieldChanged() ) );



  connect( mSearchText, SIGNAL( scrollUP() ), this, SIGNAL( scrollUP() ));
  connect( mSearchText, SIGNAL( scrollDOWN() ), this, SIGNAL( scrollDOWN() ));


  setFocusProxy( mSearchText );
}

IncSearchWidget::~IncSearchWidget()
{
 
}
void IncSearchWidget::announceDoSearch2()
{
    if ( KABPrefs::instance()->mSearchWithReturn )
        return;
    emit doSearch( mSearchText->text() );
    //qDebug("emit dosreach ");
}

void IncSearchWidget::announceDoSearch() 
{

  emit doSearch( mSearchText->text() );
  // qDebug("emit dosreach ");
}

void IncSearchWidget::announceFieldChanged()
{
  emit fieldChanged();
}
void IncSearchWidget::setSize()
{
    if ( KABPrefs::instance()->mHideSearchOnSwitch && QApplication::desktop()->width() == 480) {
        setCurrentItem( 0 );
        //mFieldCombo->setMaximumWidth( 0 );
        mFieldCombo->hide();
        mSearchText->setMaximumWidth( 200 );
    } else {
        mFieldCombo->show();
        QFontMetrics fm ( mFieldCombo->font() );
        int wid = fm.width(i18n( "All Fields" ) );
#ifdef DESKTOP_VERSION
        mFieldCombo->setMinimumWidth( wid+60 );
        wid = wid * 2;
#endif
        mFieldCombo->setMaximumWidth( wid+60 );
        mSearchText->setMaximumWidth( 1024 );
    }
}
void IncSearchWidget::setFields( const KABC::Field::List &list )
{
   
  mFieldCombo->clear();
  mFieldCombo->insertItem( i18n( "All Fields" ) );
 
  KABC::Field::List::ConstIterator it;
  for ( it = list.begin(); it != list.end(); ++it ) {
    mFieldCombo->insertItem( (*it)->label() );
  }

  mFieldList = list;

  announceDoSearch();
  announceFieldChanged();
  setSize();
}

KABC::Field::List IncSearchWidget::fields() const
{
  return mFieldList;
}

KABC::Field *IncSearchWidget::currentField()const
{
  if ( mFieldCombo->currentItem() == -1 || mFieldCombo->currentItem() == 0 )
    return 0;  // for error or 'use all fields'
  else
    return mFieldList[ mFieldCombo->currentItem() - 1 ];
}

void IncSearchWidget::setCurrentItem( int pos )
{
  mFieldCombo->setCurrentItem( pos );
  announceFieldChanged();
}

int IncSearchWidget::currentItem() const
{
  
  return mFieldCombo->currentItem();
}
#ifndef KAB_EMBEDDED
#include "incsearchwidget.moc"
#endif //KAB_EMBEDDED
