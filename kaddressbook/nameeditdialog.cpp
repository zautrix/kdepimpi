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

#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qstring.h>

#ifndef KAB_EMBEDDED
#include <kaccelmanager.h>
#else //KAB_EMBEDDED
#include <kstandarddirs.h>
#endif //KAB_EMBEDDED

#include <kapplication.h>
#include <kbuttonbox.h>
#include <kconfig.h>
#include <klineedit.h>
#include <klistview.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "nameeditdialog.h"

NameEditDialog::NameEditDialog( const KABC::Addressee &addr, int type,
                                QWidget *parent, const char *name )
  : KDialogBase( Plain, i18n( "Edit Contact Name" ), Help | Ok | Cancel,
                 Ok, parent, name, true )
{

  QWidget *page = plainPage();
  QGridLayout *layout = new QGridLayout( page, 5, 3 );
  layout->setSpacing( spacingHint() );
  layout->addColSpacing( 2, 100 );
  QLabel *label;
  
  label = new QLabel( i18n( "Honorific prefixes:" ), page );
  layout->addWidget( label, 0, 0 );
  mPrefixCombo = new KComboBox( page );
  mPrefixCombo->setDuplicatesEnabled( false );
  mPrefixCombo->setEditable( true );
  label->setBuddy( mPrefixCombo );
  layout->addMultiCellWidget( mPrefixCombo, 0, 0, 1, 2 );
  
  label = new QLabel( i18n( "Given name:" ), page );
  layout->addWidget( label, 1, 0 );
  mGivenNameEdit = new KLineEdit( page );
  label->setBuddy( mGivenNameEdit );
  layout->addMultiCellWidget( mGivenNameEdit, 1, 1, 1, 2 );

  label = new QLabel( i18n( "Additional names:" ), page );
  layout->addWidget( label, 2, 0 );
  mAdditionalNameEdit = new KLineEdit( page );
  label->setBuddy( mAdditionalNameEdit );
  layout->addMultiCellWidget( mAdditionalNameEdit, 2, 2, 1, 2 );
  
  label = new QLabel( i18n( "Family names:" ), page );
  layout->addWidget( label, 3, 0 );
  mFamilyNameEdit = new KLineEdit( page );
  label->setBuddy( mFamilyNameEdit );
  layout->addMultiCellWidget( mFamilyNameEdit, 3, 3, 1, 2 );
  
  label = new QLabel( i18n( "Honorific suffixes:" ), page );
  layout->addWidget( label, 4, 0 );
  mSuffixCombo = new KComboBox( page );
  mSuffixCombo->setDuplicatesEnabled( false );
  mSuffixCombo->setEditable( true );
  label->setBuddy( mSuffixCombo );
  layout->addMultiCellWidget( mSuffixCombo, 4, 4, 1, 2 );

  mFormattedNameCombo = new KComboBox( page );
  mFormattedNameCombo->setMaximumWidth(100);
  
  layout->addMultiCellWidget( mFormattedNameCombo, 5, 5, 0, 0 );
  connect( mFormattedNameCombo, SIGNAL( activated( int ) ), SLOT( typeChanged( int ) ) );

  mFormattedNameEdit = new KLineEdit( page );
  mFormattedNameEdit->setEnabled( type == CustomName );
  layout->addMultiCellWidget( mFormattedNameEdit, 5, 5, 1, 2 );

  mParseBox = new QCheckBox( i18n( "Parse name automatically" ), page );
  connect( mParseBox, SIGNAL( toggled(bool) ), SLOT( parseBoxChanged(bool) ) );
  connect( mParseBox, SIGNAL( toggled(bool) ), SLOT( modified() ) );
  layout->addMultiCellWidget( mParseBox, 6, 6, 0, 2 );

  // Fill in the values
  mFamilyNameEdit->setText( addr.familyName() );
  mGivenNameEdit->setText( addr.givenName() );
  mAdditionalNameEdit->setText( addr.additionalName() );
  mFormattedNameEdit->setText( addr.formattedName() );

  // Prefix and suffix combos
//US  KConfig config( "kabcrc" );
  KConfig config( locateLocal("config", "kabcrc") );
  config.setGroup( "General" );

  QStringList sTitle;
  sTitle += i18n( "Dr." );
  sTitle += i18n( "Miss" );
  sTitle += i18n( "Mr." );
  sTitle += i18n( "Mrs." );
  sTitle += i18n( "Ms." );
  sTitle += i18n( "Prof." );
  sTitle += config.readListEntry( "Prefixes" );
  sTitle.sort();

  QStringList sSuffix;
  sSuffix += i18n( "I" );
  sSuffix += i18n( "II" );
  sSuffix += i18n( "III" );
  sSuffix += i18n( "Jr." );
  sSuffix += i18n( "Sr." );
  sSuffix += config.readListEntry( "Suffixes" );
  sSuffix.sort();
  
  mPrefixCombo->insertStringList( sTitle );
  mSuffixCombo->insertStringList( sSuffix );
  
#ifndef KAB_EMBEDDED    
  mPrefixCombo->setCurrentText( addr.prefix() );
  mSuffixCombo->setCurrentText( addr.suffix() );
#else //KAB_EMBEDDED    
  mPrefixCombo->setEditText( addr.prefix() );
  mSuffixCombo->setEditText( addr.suffix() );
#endif //KAB_EMBEDDED    

  AddresseeConfig::instance()->setUid( addr.uid() );
  mParseBox->setChecked( AddresseeConfig::instance()->automaticNameParsing() );

#ifndef KAB_EMBEDDED
  KAcceleratorManager::manage( this );
#endif //KAB_EMBEDDED

  connect( mPrefixCombo, SIGNAL( textChanged( const QString& ) ), SLOT( modified() ) );
  connect( mPrefixCombo, SIGNAL( textChanged( const QString& ) ), SLOT( updateTypeCombo() ) );
  connect( mGivenNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( modified() ) );
  connect( mGivenNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( updateTypeCombo() ) );
  connect( mAdditionalNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( modified() ) );
  connect( mAdditionalNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( updateTypeCombo() ) );
  connect( mFamilyNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( modified() ) );
  connect( mFamilyNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( updateTypeCombo() ) );
  connect( mSuffixCombo, SIGNAL( textChanged( const QString& ) ), SLOT( modified() ) );
  connect( mSuffixCombo, SIGNAL( textChanged( const QString& ) ), SLOT( updateTypeCombo() ) );
  connect( mFormattedNameCombo, SIGNAL( activated( int ) ), SLOT( modified() ) );
  connect( mFormattedNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( modified() ) );

  updateTypeCombo();
  mFormattedNameCombo->setCurrentItem( type );


  mChanged = false;
}
    
NameEditDialog::~NameEditDialog() 
{
}
    
QString NameEditDialog::familyName() const
{
  return mFamilyNameEdit->text();
}
    
QString NameEditDialog::givenName() const
{
  return mGivenNameEdit->text();
}
    
QString NameEditDialog::prefix() const
{
  return mPrefixCombo->currentText();
}
    
QString NameEditDialog::suffix() const
{
  return mSuffixCombo->currentText();
}
    
QString NameEditDialog::additionalName() const
{
  return mAdditionalNameEdit->text();
}

QString NameEditDialog::customFormattedName() const
{
  return mFormattedNameEdit->text();
}

int NameEditDialog::formattedNameType() const
{
  return mFormattedNameCombo->currentItem();
}

bool NameEditDialog::changed() const
{
  return mChanged;
}

QString NameEditDialog::formattedName( const KABC::Addressee &addr, int type )
{
    QString retval;
  switch ( type ) {
    case SimpleName:
      retval = addr.givenName() + " " + addr.familyName();
      break;
    case FullName:
      retval =addr.prefix() + " " + addr.givenName() + " " +
             addr.additionalName() + " " + addr.familyName() + " " +
             addr.suffix();
      break;
    case ReverseName:
        if ( !addr.familyName().isEmpty() ) {
            retval = addr.familyName();
            if ( !addr.givenName().isEmpty() )
                retval += ", " + addr.givenName();
        } else
            retval =  addr.givenName();
      break;
    default:
      return "";
      break;
  }
  return retval.stripWhiteSpace ();
}

void NameEditDialog::parseBoxChanged( bool value )
{
    //AddresseeConfig::instance()->setUid( addr.uid() );
  AddresseeConfig::instance()->setAutomaticNameParsing( value );
}

void NameEditDialog::typeChanged( int pos )
{
  mFormattedNameEdit->setEnabled( pos == 0 );
}

void NameEditDialog::modified()
{
  mChanged = true;
}

void NameEditDialog::updateTypeCombo()
{
  KABC::Addressee addr;
  addr.setPrefix( mPrefixCombo->currentText() );
  addr.setGivenName( mGivenNameEdit->text() );
  addr.setAdditionalName( mAdditionalNameEdit->text() );
  addr.setFamilyName( mFamilyNameEdit->text() );
  addr.setSuffix( mSuffixCombo->currentText() );

  int pos = mFormattedNameCombo->currentItem();

  mFormattedNameCombo->clear();
  mFormattedNameCombo->insertItem( i18n( "Custom" ) );
  mFormattedNameCombo->insertItem( formattedName( addr, SimpleName ) );
  mFormattedNameCombo->insertItem( formattedName( addr, FullName ) );
  mFormattedNameCombo->insertItem( formattedName( addr, ReverseName ) );

  mFormattedNameCombo->setCurrentItem( pos );
}

void NameEditDialog::slotHelp()
{
#ifndef KAB_EMBEDDED
  kapp->invokeHelp( "managing-contacts-automatic-nameparsing" );
#else //KAB_EMBEDDED
qDebug("NameEditDialog::slotHelp Help is not supported yet");
#endif //KAB_EMBEDDED
}

#ifndef KAB_EMBEDDED
#include "nameeditdialog.moc"
#endif //KAB_EMBEDDED
