/*
   This file is part of KAddressBook.
   Copyright (C) 2003 Tobias Koenig <tokoe@kde.org>
                 based on the code of KSpread's CSV Import Dialog 

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

$Id: csvimportdialog.cpp,v 1.5 2005/10/28 21:15:03 zautrix Exp $
*/


#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#ifdef DESKTOP_VERSION
#include <qinputdialog.h>
#else
#include <qtcompat/qinputdialog.h>
#endif
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtable.h>
#include <qlayout.h>
#include <qtextstream.h>
#include <qfile.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kfiledialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>

#ifdef DESKTOP_VERSION
#include "qtable.h"
#else
#include "qcombotableitem.h"
#endif
#include "csvimportdialog.h"

CSVImportDialog::CSVImportDialog( KABC::AddressBook *ab, QWidget *parent,
                                  const char * name )
  : KDialogBase( Plain, i18n ( "CSV Import Dialog" ), Ok | Cancel | User1 | 
                 User2, Ok, parent, name, true, true ),
    mAdjustRows( false ),
    mStartLine( 0 ),
    mTextQuote( '"' ),
    mDelimiter( "," ),
    mAddressBook( ab )
{
  initGUI();

  mTypeMap.insert( i18n( "Undefined" ), Undefined );
  mTypeMap.insert( KABC::Addressee::formattedNameLabel(), FormattedName );
  mTypeMap.insert( KABC::Addressee::familyNameLabel(), FamilyName );
  mTypeMap.insert( KABC::Addressee::givenNameLabel(), GivenName );
  mTypeMap.insert( KABC::Addressee::additionalNameLabel(), AdditionalName );
  mTypeMap.insert( KABC::Addressee::prefixLabel(), Prefix );
  mTypeMap.insert( KABC::Addressee::suffixLabel(), Suffix );
  mTypeMap.insert( KABC::Addressee::nickNameLabel(), NickName );
  mTypeMap.insert( KABC::Addressee::birthdayLabel(), Birthday );

  mTypeMap.insert( KABC::Addressee::homeAddressStreetLabel(), HomeAddressStreet );
  mTypeMap.insert( KABC::Addressee::homeAddressLocalityLabel(), 
                   HomeAddressLocality );
  mTypeMap.insert( KABC::Addressee::homeAddressRegionLabel(), HomeAddressRegion );
  mTypeMap.insert( KABC::Addressee::homeAddressPostalCodeLabel(), 
                   HomeAddressPostalCode );
  mTypeMap.insert( KABC::Addressee::homeAddressCountryLabel(), 
                   HomeAddressCountry );
  mTypeMap.insert( KABC::Addressee::homeAddressLabelLabel(), HomeAddressLabel );

  mTypeMap.insert( KABC::Addressee::businessAddressStreetLabel(), 
                   BusinessAddressStreet );
  mTypeMap.insert( KABC::Addressee::businessAddressLocalityLabel(), 
                   BusinessAddressLocality );
  mTypeMap.insert( KABC::Addressee::businessAddressRegionLabel(), 
                   BusinessAddressRegion );
  mTypeMap.insert( KABC::Addressee::businessAddressPostalCodeLabel(), 
                   BusinessAddressPostalCode );
  mTypeMap.insert( KABC::Addressee::businessAddressCountryLabel(), 
                   BusinessAddressCountry );
  mTypeMap.insert( KABC::Addressee::businessAddressLabelLabel(), 
                   BusinessAddressLabel );

  mTypeMap.insert( KABC::Addressee::homePhoneLabel(), HomePhone );
  mTypeMap.insert( KABC::Addressee::businessPhoneLabel(), BusinessPhone );
  mTypeMap.insert( KABC::Addressee::mobilePhoneLabel(), MobilePhone );
  mTypeMap.insert( KABC::Addressee::homeFaxLabel(), HomeFax );
  mTypeMap.insert( KABC::Addressee::businessFaxLabel(), BusinessFax );
  mTypeMap.insert( KABC::Addressee::mobileWorkPhoneLabel(), MobileWorkPhone );
  mTypeMap.insert( KABC::Addressee::isdnLabel(), Isdn );
  mTypeMap.insert( KABC::Addressee::pagerLabel(), Pager );
  mTypeMap.insert( KABC::Addressee::emailLabel(), Email );
  mTypeMap.insert( KABC::Addressee::mailerLabel(), Mailer );
  mTypeMap.insert( KABC::Addressee::titleLabel(), Title );
  mTypeMap.insert( KABC::Addressee::roleLabel(), Role );
  mTypeMap.insert( KABC::Addressee::organizationLabel(), Organization );
  mTypeMap.insert( KABC::Addressee::noteLabel(), Note );
  mTypeMap.insert( KABC::Addressee::urlLabel(), URL );
  mTypeMap.insert( KABC::Addressee::categoryLabel(), Categories );

  mCustomCounter = mTypeMap.count();
  int count = mCustomCounter;

  KABC::Field::List fields = mAddressBook->fields( KABC::Field::CustomCategory );
  KABC::Field::List::Iterator it;
  for ( it = fields.begin(); it != fields.end(); ++it, ++count )
    mTypeMap.insert( (*it)->label(), count );

  connect( mDelimiterBox, SIGNAL( clicked( int ) ),
           this, SLOT( delimiterClicked( int ) ) );
  connect( mDelimiterEdit, SIGNAL( returnPressed() ),
           this, SLOT( returnPressed() ) );
  connect( mDelimiterEdit, SIGNAL( textChanged ( const QString& ) ),
           this, SLOT( textChanged ( const QString& ) ) );
  connect( mComboLine, SIGNAL( activated( const QString& ) ),
           this, SLOT( lineSelected( const QString& ) ) );
  connect( mComboCodec, SIGNAL( activated( const QString& ) ),
           this, SLOT( codecChanged( const QString& ) ) );
  connect( mComboQuote, SIGNAL( activated( const QString& ) ),
           this, SLOT( textquoteSelected( const QString& ) ) );
  connect( mIgnoreDuplicates, SIGNAL( stateChanged( int ) ),
           this, SLOT( ignoreDuplicatesChanged( int ) ) );

  connect( mUrlRequester, SIGNAL( returnPressed( const QString& ) ),
           this, SLOT( setFile( const QString& ) ) );
  connect( mUrlRequester, SIGNAL( urlSelected( const QString& ) ),
           this, SLOT( setFile( const QString& ) ) );
  connect( mUrlRequester->lineEdit(), SIGNAL( textChanged ( const QString& ) ),
           this, SLOT( urlChanged( const QString& ) ) );

  connect( this, SIGNAL( user1Clicked() ),
           this, SLOT( applyTemplate() ) );
 
  connect( this, SIGNAL( user2Clicked() ),
           this, SLOT( saveTemplate() ) );

  // if ( QApplication::desktop()->width() < 321 ) 
      QIconSet  icon = SmallIcon("filesave");
     
    findButton( User2 )->setIconSet (icon ) ;
    icon = SmallIcon("fileopen");
    findButton( User1 )->setIconSet (icon ) ;
    int wid = findButton( User2 )->sizeHint().height();
    findButton( User2 )->setMaximumWidth( wid+4  );
    findButton( User1 )->setMaximumWidth( wid+4  );
}

CSVImportDialog::~CSVImportDialog()
{
}

KABC::AddresseeList CSVImportDialog::contacts() const
{
  KABC::AddresseeList contacts;

  for ( int row = 1; row < mTable->numRows(); ++row ) {
    KABC::Addressee a;
    bool emptyRow = true;
    KABC::Address addrHome( KABC::Address::Home );
    KABC::Address addrWork( KABC::Address::Work );
    for ( int col = 0; col < mTable->numCols(); ++col ) {
      
      QComboTableItem *item = static_cast<QComboTableItem*>( mTable->item( 0, col ) );

      if ( !item ) {
        qDebug( "ERROR: item cast failed" );
        continue;
      }

      QString value = mTable->text( row, col );
      if ( !value.isEmpty() )
        emptyRow = false;

      switch ( posToType( item->currentItem() ) ) 
      {
        case Undefined:
          continue;
          break;
        case FormattedName:
          a.setFormattedName( value );
          break;
        case GivenName:
          a.setGivenName( value );
          break;
        case FamilyName:
          a.setFamilyName( value );
          break;
        case AdditionalName:
          a.setAdditionalName( value );
          break;
        case Prefix:
          a.setPrefix( value );
          break;
        case Suffix:
          a.setSuffix( value );
          break;
        case NickName:
          a.setNickName( value );
          break;
        case Birthday:
//US
//the generated code had the following format: a.setBirthday( QDate::fromString( value, Qt::ISODate ) );
// But Qt::IsoDate and QDate::fromString was not specified. Do I have the wrong QT version ?
          {
            QDate dt = KGlobal::locale()->readDate( value, "%Y-%m-%d");  // = Qt::ISODate
            a.setBirthday(dt);
          }
          break;
        case Email:
          if ( !value.isEmpty() )
            a.insertEmail( value, true );
          break;
        case Role:
          a.setRole( value );
          break;
        case Title:
          a.setTitle( value );
          break;
        case Mailer:
          a.setMailer( value );
          break;
        case URL:
          a.setUrl( value );
          break;
        case Organization:
          a.setOrganization( value );
          break;
        case Note:
            if ( a.note().isEmpty() )
                a.setNote( value );
            else {
                a.setNote( a.note()+"\n"+value );
            }
          break;
      case Categories:
          a.insertCategory( value );
          break;
        case HomePhone:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Home );
            a.insertPhoneNumber( number );
          }
          break;
        case BusinessPhone:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Work );
            a.insertPhoneNumber( number );
          }
          break;
        case MobilePhone:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Cell );
            a.insertPhoneNumber( number );
          }
          break;
        case HomeFax:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Home |
                                             KABC::PhoneNumber::Fax );
            a.insertPhoneNumber( number );
          }
          break;
        case BusinessFax:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Work |
                                             KABC::PhoneNumber::Fax );
            a.insertPhoneNumber( number );
          }
          break;
        case MobileWorkPhone:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Car );
            a.insertPhoneNumber( number );
          }
          break;
        case Isdn:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Isdn );
            a.insertPhoneNumber( number );
          }
          break;
        case Pager:
          if ( !value.isEmpty() ) {
            KABC::PhoneNumber number( value, KABC::PhoneNumber::Pager );
            a.insertPhoneNumber( number );
          }
          break;

        case HomeAddressStreet:
          addrHome.setStreet( value );
          break;
        case HomeAddressLocality:
          addrHome.setLocality( value );
          break;
        case HomeAddressRegion:
          addrHome.setRegion( value );
          break;
        case HomeAddressPostalCode:
          addrHome.setPostalCode( value );
          break;
        case HomeAddressCountry:
          addrHome.setCountry( value );
          break;
        case HomeAddressLabel:
          addrHome.setLabel( value );
          break;

        case BusinessAddressStreet:
          addrWork.setStreet( value );
          break;
        case BusinessAddressLocality:
          addrWork.setLocality( value );
          break;
        case BusinessAddressRegion:
          addrWork.setRegion( value );
          break;
        case BusinessAddressPostalCode:
          addrWork.setPostalCode( value );
          break;
        case BusinessAddressCountry:
          addrWork.setCountry( value );
          break;
        case BusinessAddressLabel:
          addrWork.setLabel( value );
          break;
        default:        
          KABC::Field::List fields = mAddressBook->fields( KABC::Field::CustomCategory );
          KABC::Field::List::Iterator it;

          int counter = 0;
          for ( it = fields.begin(); it != fields.end(); ++it ) {
            if ( counter == (int)( posToType( item->currentItem() ) - mCustomCounter ) ) 
            {
              (*it)->setValue( a, value );
              break;
            }
            ++counter;
          }
          break;
      }
    }

    if ( !addrHome.isEmpty() )
      a.insertAddress( addrHome );
    if ( !addrWork.isEmpty() )
      a.insertAddress( addrWork );

    if ( !emptyRow && !a.isEmpty() )
      contacts.append( a );
  }

  return contacts;
}

void CSVImportDialog::initGUI()
{
  QWidget* page = plainPage();

  QGridLayout *layout = new QGridLayout( page, 1, 1, marginHintSmall(), 
                                         spacingHintSmall() );
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->setSpacing( spacingHint() );
  
  QLabel *label = new QLabel( i18n( "File to import:" ), page );
  hbox->addWidget( label );

  mUrlRequester = new KURLRequester( page );
  mUrlRequester->setFilter( "*.csv" );
  hbox->addWidget( mUrlRequester );

  layout->addMultiCellLayout( hbox, 0, 0, 0, 2 );

  // Delimiter: comma, semicolon, tab, space, other
  mDelimiterBox = new QButtonGroup( i18n( "Delimiter" ), page );
  mDelimiterBox->setColumnLayout( 0, Qt::Vertical );
  mDelimiterBox->layout()->setSpacing( spacingHint() );
  mDelimiterBox->layout()->setMargin( marginHint() );
  QGridLayout *delimiterLayout = new QGridLayout( mDelimiterBox->layout() );
  delimiterLayout->setAlignment( Qt::AlignTop );
  layout->addMultiCellWidget( mDelimiterBox, 1, 1, 0, 2 );

  mRadioComma = new QRadioButton( i18n( "Comma" ), mDelimiterBox );
  mRadioComma->setChecked( true );
  delimiterLayout->addWidget( mRadioComma, 0, 0 );

  mRadioSemicolon = new QRadioButton( i18n( "Semicolon" ), mDelimiterBox );
  delimiterLayout->addWidget( mRadioSemicolon, 0, 1 );

  mRadioTab = new QRadioButton( i18n( "Tabulator" ), mDelimiterBox );
  delimiterLayout->addWidget( mRadioTab, 1, 0 );

  mRadioSpace = new QRadioButton( i18n( "Space" ), mDelimiterBox );
  delimiterLayout->addWidget( mRadioSpace, 1, 1 );

  mRadioOther = new QRadioButton( i18n( "Other" ), mDelimiterBox );
  delimiterLayout->addWidget( mRadioOther, 0, 2 );

  mDelimiterEdit = new QLineEdit( mDelimiterBox );
  delimiterLayout->addWidget( mDelimiterEdit, 1, 2 );

  mComboLine = new QComboBox( false, page );
  mComboLine->insertItem( i18n( "1" ) );
  layout->addWidget( mComboLine, 3, 1 );

  mComboQuote = new QComboBox( false, page );
  mComboQuote->insertItem( i18n( "\"" ), 0 );
  mComboQuote->insertItem( i18n( "'" ), 1 );
  mComboQuote->insertItem( i18n( "None" ), 2 );
  layout->addWidget( mComboQuote, 3, 0 );
  mComboCodec = new QComboBox( false, page );
  mComboCodec->insertItem( i18n( "UTF8" ), 0 );
  mComboCodec->insertItem( i18n( "LATIN1" ), 1 );
  mComboCodec->insertItem( i18n( "LOCALE" ), 2 );
  layout->addWidget( mComboCodec, 3, 2 );
  label = new QLabel( i18n( "Start at line:" ), page );
  layout->addWidget( label, 2, 1 );

  label = new QLabel( i18n( "Textquote:" ), page );
  layout->addWidget( label, 2, 0 );
  label = new QLabel( i18n( "Codec:" ), page );
  layout->addWidget( label, 2, 2 );

  mIgnoreDuplicates = new QCheckBox( page );
  mIgnoreDuplicates->setText( i18n( "Ignore duplicate delimiters" ) );
  layout->addMultiCellWidget( mIgnoreDuplicates, 4, 4, 0, 2 );

  mTable = new QTable( 0, 0, page );
  mTable->setSelectionMode( QTable::NoSelection );
  //mTable->horizontalHeader()->hide();
  layout->addMultiCellWidget( mTable, 5, 5, 0, 2 );
/*US
  setButtonText( User1, i18n( "Apply Template" ) );
  setButtonText( User2, i18n( "Save Template" ) );
*/  

  enableButtonOK( false );

  findButton( User1 )->setEnabled( false );
  findButton( User2 )->setEnabled( false );
 
#ifdef DESKTOP_VERSION
  resize( 640, 480 );
#else
  showMaximized();
#endif
}

void CSVImportDialog::fillTable()
{
  int row, column;
  bool lastCharDelimiter = false;
  bool ignoreDups = mIgnoreDuplicates->isChecked();
  enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_END_OF_QUOTED_FIELD,
         S_MAYBE_NORMAL_FIELD, S_NORMAL_FIELD } state = S_START;

  QChar x;
  QString field = "";

  // store previous assignment
  QValueList<int> mTypeOld = mTypeStore;
  
  mTypeStore.clear();
  for ( column = 0; column < mTable->numCols(); ++column ) {
    QComboTableItem *item = static_cast<QComboTableItem*>( mTable->item( 0, column ) );

    if ( !item || mClearTypeStore )
      mTypeStore.append( typeToPos( Undefined ) );
    else if ( item )
      mTypeStore.append( item->currentItem() );
  }

  clearTable();
  
  row = column = 1;
  if ( mComboCodec->currentItem () == 0 ) {
      mData = QString::fromUtf8( mFileArray.data() );
  } else  if ( mComboCodec->currentItem () == 1 ) {
      mData = QString::fromLatin1( mFileArray.data() );
  } else  {
      mData = QString::fromLocal8Bit( mFileArray.data() );
  } 

  QTextStream inputStream( mData, IO_ReadOnly );

  if ( mComboCodec->currentItem () == 0 ) {
      inputStream.setEncoding( QTextStream::UnicodeUTF8 );
  } else  if ( mComboCodec->currentItem () == 1 ) {
      inputStream.setEncoding( QTextStream::Latin1 );
  } else  {
      inputStream.setEncoding( QTextStream::Locale );
  } 

  int maxColumn = 0;
  while ( !inputStream.atEnd() ) {
    inputStream >> x; // read one char

    if ( x == '\r' ) inputStream >> x; // eat '\r', to handle DOS/LOSEDOWS files correctly

    switch ( state ) {
     case S_START :
      if ( x == mTextQuote ) {
        state = S_QUOTED_FIELD;
      } else if ( x == mDelimiter ) {
        if ( ( ignoreDups == false ) || ( lastCharDelimiter == false ) )
          ++column;
        lastCharDelimiter = true;
      } else if ( x == '\n' ) {
        ++row;
        column = 1;
      } else {
        field += x;
        state = S_MAYBE_NORMAL_FIELD;
      }
      break;
     case S_QUOTED_FIELD :
      if ( x == mTextQuote ) {
        state = S_MAYBE_END_OF_QUOTED_FIELD;
      } else if ( x == '\n' &&  mTextQuote.isNull() ) {
        setText( row - mStartLine + 1, column, field );
        field = "";
        if ( x == '\n' ) {
          ++row;
          column = 1;
        } else {
          if ( ( ignoreDups == false ) || ( lastCharDelimiter == false ) )
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      } else {
        field += x;
      }
      break;
     case S_MAYBE_END_OF_QUOTED_FIELD :
      if ( x == mTextQuote ) {
        field += x;
        state = S_QUOTED_FIELD;
      } else if ( x == mDelimiter || x == '\n' ) {
        setText( row - mStartLine + 1, column, field );
        field = "";
        if ( x == '\n' ) {
          ++row;
          column = 1;
        } else {
          if ( ( ignoreDups == false ) || ( lastCharDelimiter == false ) )
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      } else {
        state = S_END_OF_QUOTED_FIELD;
      }
      break;
     case S_END_OF_QUOTED_FIELD :
      if ( x == mDelimiter || x == '\n' ) {
        setText( row - mStartLine + 1, column, field );
        field = "";
        if ( x == '\n' ) {
          ++row;
          column = 1;
        } else {
          if ( ( ignoreDups == false ) || ( lastCharDelimiter == false ) )
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      } else {
        state = S_END_OF_QUOTED_FIELD;
      }
      break;
     case S_MAYBE_NORMAL_FIELD :
      if ( x == mTextQuote ) {
        field = "";
        state = S_QUOTED_FIELD;
        break;
      }
     case S_NORMAL_FIELD :
      if ( x == mDelimiter || x == '\n' ) {
        setText( row - mStartLine + 1, column, field );
        field = "";
        if ( x == '\n' ) {
          ++row;
          column = 1;
        } else {
          if ( ( ignoreDups == false ) || ( lastCharDelimiter == false ) )
            ++column;
          lastCharDelimiter = true;
        }
        state = S_START;
      } else {
        field += x;
      }
    }
    if ( x != mDelimiter )
      lastCharDelimiter = false;

    if ( column > maxColumn )
      maxColumn = column;
  }

  // file with only one line without '\n'
  if ( field.length() > 0 ) {
    setText( row - mStartLine + 1, column, field );
    ++row;
    field = "";
  }

  adjustRows( row - mStartLine );
  mTable->setNumCols( maxColumn );

//US begin
  QStringList keys;  
  uint iii = 0, count = mTypeMap.count();
  while ( iii < count ) {
      keys << "dummy";
      ++iii;
  }
  QMap<QString, uint>::ConstIterator it;
  for ( it = mTypeMap.begin(); it != mTypeMap.end(); ++it)
      keys[( it.data() )] =  it.key();
//US end
  
  for ( column = 0; column < mTable->numCols(); ++column ) {
   
//US    QComboTableItem *item = new QComboTableItem( mTable, mTypeMap.keys() );
    QComboTableItem *item = new QComboTableItem( mTable, keys );
    mTable->setItem( 0, column, item );
    if ( column < mTypeStore.count() )
      item->setCurrentItem( mTypeStore[ column ] );
    else
      item->setCurrentItem( typeToPos( Undefined ) );
    
    mTable->adjustColumn( column );
  }
}

void CSVImportDialog::clearTable()
{
  for ( int row = 0; row < mTable->numRows(); ++row )
    for ( int column = 0; column < mTable->numCols(); ++column )
      mTable->clearCell( row, column );
}

void CSVImportDialog::fillComboBox()
{
  mComboLine->clear();
  for ( int row = 1; row < mTable->numRows() + 1; ++row )
    mComboLine->insertItem( QString::number( row ), row - 1 );
}

void CSVImportDialog::setText( int row, int col, const QString& text )
{
  if ( row < 1 ) // skipped by the user
    return;

  if ( mTable->numRows() < row ) {
    mTable->setNumRows( row + 5000 ); // We add 5000 at a time to limit recalculations
    mAdjustRows = true;
  }

  if ( mTable->numCols() < col )
    mTable->setNumCols( col + 50 ); // We add 50 at a time to limit recalculation

  mTable->setText( row - 1, col - 1, text );
}

/*
 * Called after the first fillTable() when number of rows are unknown.
 */
void CSVImportDialog::adjustRows( int rows )
{
  if ( mAdjustRows ) {
    mTable->setNumRows( rows );
    mAdjustRows = false;
  }
}

void CSVImportDialog::returnPressed()
{
  if ( mDelimiterBox->id( mDelimiterBox->selected() ) != 4 )
    return;

  mDelimiter = mDelimiterEdit->text();
  fillTable();
}

void CSVImportDialog::textChanged ( const QString& )
{
  mRadioOther->setChecked ( true );
  delimiterClicked( 4 ); // other
}
void CSVImportDialog::codecChanged ( const QString& )
{
    fillTable();
}

void CSVImportDialog::delimiterClicked( int id )
{
  switch ( id ) {
   case 0: // comma
    mDelimiter = ",";
    break;
   case 4: // other
    mDelimiter = mDelimiterEdit->text();
    break;
   case 2: // tab
    mDelimiter = "\t";
    break;
   case 3: // space
    mDelimiter = " ";
    break;
   case 1: // semicolon
    mDelimiter = ";";
    break;
  }

  fillTable();
}

void CSVImportDialog::textquoteSelected( const QString& mark )
{
  if ( mComboQuote->currentItem() == 2 )
    mTextQuote = 0;
  else
    mTextQuote = mark[ 0 ];

  fillTable();
}

void CSVImportDialog::lineSelected( const QString& line )
{
  mStartLine = line.toInt() - 1;
  fillTable();
}

void CSVImportDialog::slotOk()
{
  bool assigned = false;

  for ( int column = 0; column < mTable->numCols(); ++column ) {
    QComboTableItem *item = static_cast<QComboTableItem*>( mTable->item( 0, 
                                                           column ) );
    if ( item && posToType( item->currentItem() ) != Undefined )
      assigned = true;
  }
    
  if ( assigned )
    KDialogBase::slotOk();
  else
    KMessageBox::sorry( this, i18n( "You have to assign at least one column." ) );
}

void CSVImportDialog::applyTemplate()
{
  QMap<uint,int> columnMap;
  QMap<QString, QString> fileMap;
  QStringList templates;

  // load all template files
/*US  QStringList list = KGlobal::dirs()->findAllResources( "data" , QString( kapp->name() ) +
      "/csv-templates/*.desktop", true, true );
*/      
  QStringList list = KGlobal::dirs()->findAllResources( "data" , KGlobal::getAppName() +
      "/csv-templates/*.desktop", true, true );

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    
//US    KSimpleConfig config( *it, true );
    KConfig config( *it );

    if ( !config.hasGroup( "csv column map" ) )
	    continue;

    config.setGroup( "Misc" );
    templates.append( config.readEntry( "Name" ) );
    fileMap.insert( config.readEntry( "Name" ), *it );
  }
 
  // let the user chose, what to take
  bool ok = false;
  QString tmp;
  tmp = QInputDialog::getItem( i18n( "Template Selection" ),
                  i18n( "Please select a template\nthat matches the CSV file." ),
                  templates, 0, false, &ok, this );

  if ( !ok )
    return;

//US  KSimpleConfig config( fileMap[ tmp ], true );
  KConfig config( fileMap[ tmp ] );
  config.setGroup( "General" );
//US  uint numColumns = config.readUnsignedNumEntry( "Columns" );
  uint numColumns = (uint)config.readNumEntry( "Columns" );
  
  mDelimiterEdit->setText( config.readEntry( "DelimiterOther" ) );
  mDelimiterBox->setButton( config.readNumEntry( "DelimiterType" ) );
  delimiterClicked( config.readNumEntry( "DelimiterType" ) );
  int quoteType = config.readNumEntry( "QuoteType" );
  mComboQuote->setCurrentItem( quoteType );
  textquoteSelected( mComboQuote->currentText() );

  // create the column map
  config.setGroup( "csv column map" );
  for ( uint i = 0; i < numColumns; ++i ) {
    int col = config.readNumEntry( QString::number( i ) );
    columnMap.insert( i, col );
  }
  
  // apply the column map
  for ( uint column = 0; column < columnMap.count(); ++column ) {
    int type = columnMap[ column ];
    QComboTableItem *item = static_cast<QComboTableItem*>( mTable->item( 0,
                                                           column ) );
    if ( item )
      item->setCurrentItem( typeToPos( type ) );
  }
}

void CSVImportDialog::saveTemplate()
{
/*US
  QString fileName = KFileDialog::getSaveFileName(
                     locateLocal( "data", QString( kapp->name() ) + "/csv-templates/" ),
                     "*.desktop", this );
*/  
  QString fileName = KFileDialog::getSaveFileName(
                     locateLocal( "data", KGlobal::getAppName() + "/csv-templates/" )+
                     "*.desktop",i18n("Save file name") , this );

  if ( fileName.isEmpty() )
    return;

  if ( !fileName.contains( ".desktop" ) ) 
    fileName += ".desktop";

  QString name = QInputDialog::getText( i18n( "Template name" ), i18n( "Please enter a name for the template" ) );

  if ( name.isEmpty() )
    return;

  KConfig config( fileName );
  config.setGroup( "General" );
  config.writeEntry( "Columns", mTable->numCols() );
  config.writeEntry( "DelimiterType", mDelimiterBox->id( mDelimiterBox->selected() ) );
  config.writeEntry( "DelimiterOther", mDelimiterEdit->text() );
  config.writeEntry( "QuoteType", mComboQuote->currentItem() );

  config.setGroup( "Misc" );
  config.writeEntry( "Name", name );

  config.setGroup( "csv column map" );

  for ( uint column = 0; column < mTable->numCols(); ++column ) {
    QComboTableItem *item = static_cast<QComboTableItem*>( mTable->item( 0, 
                                                           column ) );
    if ( item )
      config.writeEntry( QString::number( column ), posToType( 
                         item->currentItem() ) );
    else
      config.writeEntry( QString::number( column ), 0 );
  }

  config.sync();
}

QString CSVImportDialog::getText( int row, int col )
{
  return mTable->text( row, col );
}

uint CSVImportDialog::posToType( int pos ) const
{
    return pos;
#if 0
  uint counter = 0;
  QMap<QString, uint>::ConstIterator it;
  for ( it = mTypeMap.begin(); it != mTypeMap.end(); ++it, ++counter )
    if ( counter == (uint)pos )
      return it.data();

  return 0;
#endif
}

int CSVImportDialog::typeToPos( uint type ) const
{
    return type;
#if 0
  uint counter = 0;
  QMap<QString, uint>::ConstIterator it;
  for ( it = mTypeMap.begin(); it != mTypeMap.end(); ++it, ++counter )
    if ( it.data() == type )
      return counter;

  return -1;
#endif
}

void CSVImportDialog::ignoreDuplicatesChanged( int )
{
  fillTable();
}

void CSVImportDialog::setFile( const QString &fileName )
{
  if ( fileName.isEmpty() )
    return;

  QFile file( fileName );
  if ( !file.open( IO_ReadOnly ) ) {
    KMessageBox::sorry( this, i18n( "Cannot open input file!" ) );
    file.close();
    return;
  }

  mFileArray = file.readAll();
  file.close();

  mClearTypeStore = true;
  clearTable();
  mTable->setNumCols( 0 );
  mTable->setNumRows( 0 );
  fillTable();
  mClearTypeStore = false;

  fillComboBox();
}

void CSVImportDialog::urlChanged( const QString &file )
{
  bool state = !file.isEmpty();

  enableButtonOK( state );
 
  findButton( User1 )->setEnabled( state );
  findButton( User2 )->setEnabled( state );
 
}

#ifndef KAB_EMBEDDED
#include <csvimportdialog.moc>
#endif //KAB_EMBEDDED
