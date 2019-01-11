/*
    This file is part of KAddressBook.                                  
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>
                                                                        
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

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include <kbuttonbox.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>
#include <kglobal.h>
#include <klineedit.h>
#include <kstandarddirs.h>

#include "addresseewidget.h"

NamePartWidget::NamePartWidget( const QString &title, QWidget *parent,
                                const char *name )
  : QWidget( parent, name )
{
  if (KGlobal::getOrientation() == KGlobal::Portrait)
  {
    QGridLayout* layout = new QGridLayout( this, 1, 1, KDialog::marginHintSmall(),
                                         KDialog::spacingHintSmall() );
    
    QLabel *label = new QLabel( i18n( title ), this );
    layout->addWidget( label, 0, 1 );
    
    mBox = new QListBox( this );
    mBox->setMaximumSize(70, 70);
    layout->addMultiCellWidget( mBox, 0, 1, 0, 0 );

    KButtonBox *bbox = new KButtonBox( this, Qt::Vertical );
    mAddButton = bbox->addButton( i18n( "Add" ), this,  SLOT( add() ) );
    mRemoveButton = bbox->addButton( i18n( "Rem" ), this,  SLOT( remove() ) );
    bbox->layout();
    layout->addMultiCellWidget( bbox, 0, 1, 2,2);

    mEdit = new KLineEdit( this );
    layout->addWidget( mEdit, 1, 1 );
    //mEdit->setMinimumWidth(50);

//    layout->addWidget( group );
      
  }
  else
  {
    QHBoxLayout *layout = new QHBoxLayout( this );

    QGroupBox *group = new QGroupBox( 0, Qt::Vertical, title, this );
    QGridLayout *groupLayout = new QGridLayout( group->layout(), 2, 2,
                                              KDialog::spacingHint() );

    mBox = new QListBox( group );
    
    groupLayout->addWidget( mBox, 0, 0 );

    KButtonBox *bbox = new KButtonBox( group, Qt::Vertical );
    mAddButton = bbox->addButton( i18n( "Add" ), this,  SLOT( add() ) );
    mRemoveButton = bbox->addButton( i18n( "Remove" ), this,  SLOT( remove() ) );
    bbox->layout();
    groupLayout->addWidget( bbox, 0, 1 );

    mEdit = new KLineEdit( group );
    groupLayout->addMultiCellWidget( mEdit, 1, 1, 0, 1 );
    
    layout->addWidget( group );
    
  }  

  mAddButton->setEnabled( false );
  mRemoveButton->setEnabled( false );
    

  connect( mBox, SIGNAL( selectionChanged( QListBoxItem* ) ),
           SLOT( selectionChanged( QListBoxItem* ) ) );
  connect( mEdit, SIGNAL( textChanged( const QString& ) ),
             SLOT( textChanged( const QString& ) ) );
  connect( mEdit, SIGNAL( returnPressed() ), SLOT( add() ) );
               
}

NamePartWidget::~NamePartWidget()
{
}

void NamePartWidget::setNameParts( const QStringList &list )
{
  mBox->clear();
  mBox->insertStringList( list );
}

QStringList NamePartWidget::nameParts() const
{
  QStringList parts;
  for ( uint i = 0; i < mBox->count(); ++i )
    parts.append( mBox->text( i ) );

  return parts;
}

void NamePartWidget::add()
{
  if ( !mEdit->text().isEmpty() ) {
    mBox->insertItem( mEdit->text() );
    emit modified();
  }

  mEdit->setText( "" );
}

void NamePartWidget::remove()
{
  mBox->removeItem( mBox->currentItem() );
  if ( mBox->count() == 0 )
    selectionChanged( 0 );

  emit modified();
}

void NamePartWidget::selectionChanged( QListBoxItem *item )
{
  mRemoveButton->setEnabled( item != 0 );
}

void NamePartWidget::textChanged( const QString& text )
{
  mAddButton->setEnabled( !text.isEmpty() );
}


AddresseeWidget::AddresseeWidget( QWidget *parent, const char *name )
  : QWidget( parent, name )
{
  QGridLayout *layout;

  mPrefix = new NamePartWidget( i18n( "Prefixes" ), this );
  mInclusion = new NamePartWidget( i18n( "Inclusions" ), this );
  mSuffix = new NamePartWidget( i18n( "Suffixes" ), this );
  QString dfn;
  if (QApplication::desktop()->width() > 320 )
      dfn = i18n( "Default formatted name:" );
  else
      dfn = i18n( "Def. formatted name:" );
      
  QLabel *label = new QLabel( dfn, this );

  mFormattedNameCombo = new KComboBox( this );
  mFormattedNameCombo->insertItem( i18n( "Empty" ) );
  mFormattedNameCombo->insertItem( i18n( "Simple Name" ) );
  mFormattedNameCombo->insertItem( i18n( "Full Name" ) );
  mFormattedNameCombo->insertItem( i18n( "Reverse Name" ) );
    
  if (KGlobal::getOrientation() == KGlobal::Portrait)
  {
    layout = new QGridLayout( this, 4, 2, KDialog::marginHintSmall(),
                                         KDialog::spacingHintSmall() );

    layout->addMultiCellWidget( mPrefix, 0, 0, 0, 1 );
    layout->addMultiCellWidget( mInclusion, 1, 1, 0, 1 );
    layout->addMultiCellWidget( mSuffix, 2, 2, 0, 1 );
    layout->addWidget( label, 3, 0 );
    layout->addWidget( mFormattedNameCombo, 3, 1 );
    
  }
  else
  {
    layout = new QGridLayout( this, 2, 3, KDialog::marginHint(),
                                         KDialog::spacingHint() );

    layout->addWidget( mPrefix, 0, 0 );
    layout->addWidget( mInclusion, 0, 1 );
    layout->addWidget( mSuffix, 0, 2 );
    layout->addWidget( label, 1, 0 );
    layout->addMultiCellWidget( mFormattedNameCombo, 1, 1, 1, 2 );
  }

  connect( mPrefix, SIGNAL( modified() ), SIGNAL( modified() ) );
  connect( mInclusion, SIGNAL( modified() ), SIGNAL( modified() ) );
  connect( mSuffix, SIGNAL( modified() ), SIGNAL( modified() ) );
  connect( mFormattedNameCombo, SIGNAL( activated( int ) ), SIGNAL( modified() ) );
}

AddresseeWidget::~AddresseeWidget()
{
}

void AddresseeWidget::restoreSettings()
{
  KConfig config( locateLocal("config", "kabcrc") );
  config.setGroup( "General" );

  mPrefix->setNameParts( config.readListEntry( "Prefixes" ) );
  mInclusion->setNameParts( config.readListEntry( "Inclusions" ) );
  mSuffix->setNameParts( config.readListEntry( "Suffixes" ) );
  mFormattedNameCombo->setCurrentItem( config.readNumEntry( "FormattedNameType", 1 ) );
}

void AddresseeWidget::saveSettings()
{
    qDebug("AddresseeWidget::saveSettings() ");
  KConfig config( locateLocal("config","kabcrc") );
  config.setGroup( "General" );

  config.writeEntry( "Prefixes", mPrefix->nameParts() );
  config.writeEntry( "Inclusions", mInclusion->nameParts() );
  config.writeEntry( "Suffixes", mSuffix->nameParts() );
  config.writeEntry( "FormattedNameType", mFormattedNameCombo->currentItem() );
}

#ifndef KAB_EMBEDDED
#include "addresseewidget.moc"
#endif //KAB_EMBEDDED
