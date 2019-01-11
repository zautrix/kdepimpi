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
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qbuttongroup.h>
#include <qhbox.h>
#include <qcursor.h>
#include <qtimer.h>
#include <qapplication.h> 

#include <kbuttonbox.h>
#include <klistview.h>
#include <kapplication.h>
#include <qapplication.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>

#include <kabc/phonenumber.h>

#include "typecombo.h"

#include "phoneeditwidget.h"

PhoneEditWidget::PhoneEditWidget( QWidget *parent, const char *name )
  : QWidget(parent,name)
{
    QGridLayout* gridLayout = new QGridLayout ( this, 2,2 );

    QLabel *temp = new QLabel( "", this );
    temp->setAlignment( Qt::AlignCenter );
    temp->setPixmap( KGlobal::iconLoader()->loadIcon( "kaddressbook", KIcon::Desktop, 0 ) );
    QPushButton *addBut = new QPushButton ( "add", this );
    addBut->setPixmap ( SmallIcon("plus"));
    addBut->setMaximumSize( addBut->sizeHint().height(),addBut->sizeHint().height() );
    connect(addBut,SIGNAL(clicked ()),SLOT(addNumber()));

    sv = new QScrollView( this );
    sv->setFrameStyle ( QFrame::Plain );
    sv->setLineWidth ( 0 ); 
    sv->setMidLineWidth ( 0 ); 
    mw = new QWidget ( sv->viewport() );
    sv->addChild(mw);
    sv->setResizePolicy( QScrollView::AutoOneFit );
    mainLayout = new QVBoxLayout ( mw );
    mainLayout->setMargin( 0 );
    mainLayout->setSpacing( 0 );
    gridLayout->setMargin( 2 );
    gridLayout->setSpacing( 4 );
    if ( QApplication::desktop()->width() == 240 || QApplication::desktop()->width() == 480 ) {
        gridLayout->addWidget( addBut, 0, 0 );
        gridLayout->addWidget( temp, 0, 1 );
        gridLayout->addMultiCellWidget( sv, 1,1 , 0,1 );
    } else {
        gridLayout->addWidget( temp, 1, 0 );
        gridLayout->addWidget( addBut, 0, 0 );
        gridLayout->addMultiCellWidget( sv, 0, 1, 1,1 );
    }
    setDefaults();
    mTypeNumberEditList.setAutoDelete( true );
    mPopup = new QPopupMenu( this );
    QStringList list = PhoneNumber::supportedTypeListNames();
    mPopupCount = list.count();
    int i = 0;
    while ( i < mPopupCount ) {
        mPopup->insertItem( list[ i ], i );
        ++i;
    }
    connect(mPopup,SIGNAL(activated(int)),this,SLOT(addNumberInt( int)));
    
}

PhoneEditWidget::~PhoneEditWidget()
{
}
void  PhoneEditWidget::setDefaults()
{
    mTypeNumberEditList.clear();
    PhoneTypeNumberEdit* edit = appendEditCombo();
    KABC::PhoneNumber phoneNumber;
    phoneNumber.setType( KABC::PhoneNumber::Home | KABC::PhoneNumber::Pref );
    edit->setPhoneNumber( phoneNumber  );
    edit = appendEditCombo();
    KABC::PhoneNumber phoneNumber2;
    phoneNumber2.setType( KABC::PhoneNumber::Work | KABC::PhoneNumber::Pref );
    edit->setPhoneNumber( phoneNumber2  );
    edit = appendEditCombo();
    KABC::PhoneNumber phoneNumber3;
    phoneNumber3.setType( KABC::PhoneNumber::Cell );
    edit->setPhoneNumber( phoneNumber3 );

}
void  PhoneEditWidget::addNumberInt( int index )
{
    PhoneTypeNumberEdit* edit = appendEditCombo();
    KABC::PhoneNumber phoneNumber;
    phoneNumber.setType( PhoneNumber::supportedTypeList()[index] );
    edit->setPhoneNumber( phoneNumber  );
    //verticalScrollBar()->setValue( 1024);
    QTimer::singleShot( 0, this, SLOT ( bottomVisible() ) ); 
}
void  PhoneEditWidget::bottomVisible()
{
    sv->setContentsPos ( 0, 1024 );
}
void  PhoneEditWidget::addNumber()
{
    int i = 0;
    while ( i < mPopupCount ) {
        mPopup->setItemEnabled( i, true );
        ++i;
    }
    PhoneTypeNumberEdit* edit = mTypeNumberEditList.first();
    while ( edit ) {
        if ( edit->currentType() < mPopupCount -1 )
            mPopup->setItemEnabled( edit->currentType(), false );
        edit = mTypeNumberEditList.next();
    }
    mPopup->popup( QCursor::pos() );
}
PhoneTypeNumberEdit* PhoneEditWidget::appendEditCombo()
{
    PhoneTypeNumberEdit* edit = new PhoneTypeNumberEdit( mw );
    connect ( edit, SIGNAL ( typeChange( int , int) ), this, SIGNAL ( typeChange( int , int)) );
    connect ( edit, SIGNAL ( modified() ), this, SIGNAL ( modified() ) );
    connect ( edit, SIGNAL ( deleteMe( PhoneTypeNumberEdit* ) ), this, SLOT ( deleteEdit(  PhoneTypeNumberEdit*) ) );
    connect ( this, SIGNAL ( typeChange( int , int)), edit, SLOT ( typeExternalChanged( int, int)) );

    mainLayout->add( edit );
    mTypeNumberEditList.append( edit );
    return edit;
}

void PhoneEditWidget::deleteEdit(  PhoneTypeNumberEdit* ew )
{
    mPendingDelete = ew;
    QTimer::singleShot( 0, this, SLOT ( pendingDelete() ) ); 
}
void PhoneEditWidget::pendingDelete()
{ 
    mTypeNumberEditList.removeRef( mPendingDelete );
    emit modified();
}

void PhoneEditWidget::setPhoneNumbers( const KABC::PhoneNumber::List &li )
{
    if ( li.isEmpty() ) {
        setDefaults();
        return;
    }   
    mTypeNumberEditList.clear();
    KABC::PhoneNumber::List::Iterator it;
    KABC::PhoneNumber::List list2 = li;
    KABC::PhoneNumber::List list ;

    PhoneNumber::TypeList  tList = PhoneNumber::supportedTypeList();
    int i = 0;
    int max = tList.count();
    while ( i < max-1 ) {
        for ( it = list2.begin(); it != list2.end(); ++it ) {
            if ( (*it).type() == tList[i] ) {
                list.append( (*it ) ); 
                break; 
            }
        }
        ++i;   
    }
    for ( it = list2.begin(); it != list2.end(); ++it ) {
            if ( (*it).type() == tList[ max-1 ] )
                list.append( (*it ) );  
    }
    for ( it = list.begin(); it != list.end(); ++it ) {
            PhoneTypeNumberEdit* editNew = appendEditCombo();
            editNew->setPhoneNumber( (*it ) );
    }
    
}
KABC::PhoneNumber::List PhoneEditWidget::phoneNumbers()
{
  KABC::PhoneNumber::List retList;

  PhoneTypeNumberEdit* edit = mTypeNumberEditList.first();
  while ( edit ) {
      if ( edit->isValid() ) {
          retList.append( edit->phoneNumber());
      }
      edit = mTypeNumberEditList.next();

  }
  return retList;
}

#if 0
PhoneEditWidget::PhoneEditWidget( QWidget *parent, const char *name )
  : QWidget( parent, name )
{
  QGridLayout *layout = new QGridLayout( this, 4, 1 );
//US  layout->setSpacing( KDialog::spacingHint() );
  layout->setSpacing( KDialogBase::spacingHintSmall() );



 QLabel* label = new QLabel( this );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "kaddressbook", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "kaddressbook", KIcon::Desktop, 0 ) );
  label->setAlignment( AlignCenter );
//US  layout->addMultiCellWidget( label, 0, 1, 3, 3 );
  layout->addWidget( label, 0, 0 ); 

  QPushButton *editButton = new QPushButton( i18n( "Edit Phone Numbers..." ),
                                             this ); 
  if ( QApplication::desktop()->width() < 640 ) 
      layout->addWidget( editButton, 0, 1 );
  else
      layout->addMultiCellWidget( editButton, 0, 0, 1, 3);

  mPrefCombo = new PhoneTypeCombo( mPhoneList, this );
  mPrefEdit = new KLineEdit( this );
  //mPrefEdit->setMinimumWidth( int(mPrefEdit->sizeHint().width() * 1.5) );
  mPrefCombo->setLineEdit( mPrefEdit );
  layout->addWidget( mPrefCombo, 1, 0 );
  layout->addWidget( mPrefEdit, 1, 1 );
  int x = 1, y = 2;
  if ( QApplication::desktop()->width() < 640 ) {
      ++x;
      y = 0;
  }
  mSecondCombo = new PhoneTypeCombo( mPhoneList, this );
  mSecondEdit = new KLineEdit( this );
  mSecondCombo->setLineEdit( mSecondEdit );
  layout->addWidget( mSecondCombo, x, y++ );
  layout->addWidget( mSecondEdit, x, y++ );

  y = 0;
  ++x;
  mThirdCombo = new PhoneTypeCombo( mPhoneList, this );
  mThirdEdit = new KLineEdit( this );
  mThirdCombo->setLineEdit( mThirdEdit );
  layout->addWidget( mThirdCombo, x, y++ );
  layout->addWidget( mThirdEdit, x, y++ );
  if ( QApplication::desktop()->width() < 640 ) {
      ++x;
      y = 0;
  }
  mFourthCombo = new PhoneTypeCombo( mPhoneList, this );
  mFourthEdit = new KLineEdit( this );
  mFourthCombo->setLineEdit( mFourthEdit );
  layout->addWidget( mFourthCombo, x, y++ );
  layout->addWidget( mFourthEdit, x, y++ );

  // Four numbers don't fit in the current dialog
  if ( QApplication::desktop()->width() < 640 ) {
      mFourthCombo->hide();
      mFourthEdit->hide();
  } else {
      QFontMetrics fm ( font () ) ;
      int wid = fm.width( "Messenger" ) +60;
      mPrefCombo->setMaximumWidth( wid  );
      mSecondCombo->setMaximumWidth( wid  );
      mThirdCombo->setMaximumWidth( wid );
      mFourthCombo->setMaximumWidth( wid );
  }
 

  connect( mPrefEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( slotPrefEditChanged() ) );
  connect( mSecondEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( slotSecondEditChanged() ) );
  connect( mThirdEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( slotThirdEditChanged() ) );
  connect( mFourthEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( slotFourthEditChanged() ) );

  connect( editButton, SIGNAL( clicked() ), SLOT( edit() ) );

  connect( mPrefCombo, SIGNAL( activated( int ) ),
           SLOT( updatePrefEdit() ) );
  connect( mSecondCombo, SIGNAL( activated( int ) ),
           SLOT( updateSecondEdit() ) );
  connect( mThirdCombo, SIGNAL( activated( int ) ),
           SLOT( updateThirdEdit() ) );
  connect( mFourthCombo, SIGNAL( activated( int ) ),
           SLOT( updateFourthEdit() ) );
}

PhoneEditWidget::~PhoneEditWidget()
{
}

void PhoneEditWidget::setPhoneNumbers( const KABC::PhoneNumber::List &list )
{
  mPhoneList.clear();

  // Insert types for existing numbers.
  mPrefCombo->insertTypeList( list );

  QValueList<int> defaultTypes;
  defaultTypes << KABC::PhoneNumber::Home;
  defaultTypes << KABC::PhoneNumber::Work;
  defaultTypes << KABC::PhoneNumber::Cell;
  defaultTypes << ( KABC::PhoneNumber::Work | KABC::PhoneNumber::Fax );
  defaultTypes << ( KABC::PhoneNumber::Home | KABC::PhoneNumber::Fax );

  // Insert default types.
  // Doing this for mPrefCombo is enough because the list is shared by all
  // combos.
  QValueList<int>::ConstIterator it;
  for( it = defaultTypes.begin(); it != defaultTypes.end(); ++it ) {
    if ( !mPrefCombo->hasType( *it ) )
      mPrefCombo->insertType( list, *it, PhoneNumber( "", *it ) );
  }

  updateCombos();

  mPrefCombo->selectType( defaultTypes[ 0 ] );
  mSecondCombo->selectType( defaultTypes[ 1 ] );
  mThirdCombo->selectType( defaultTypes[ 2 ] );
  mFourthCombo->selectType( defaultTypes[ 3 ] );

  updateLineEdits();
}

void PhoneEditWidget::updateLineEdits()
{
  updatePrefEdit();
  updateSecondEdit();
  updateThirdEdit();
  updateFourthEdit();
}

void PhoneEditWidget::updateCombos()
{
  mPrefCombo->updateTypes();
  mSecondCombo->updateTypes();
  mThirdCombo->updateTypes();
  mFourthCombo->updateTypes();
}

KABC::PhoneNumber::List PhoneEditWidget::phoneNumbers()
{
  KABC::PhoneNumber::List retList;

  KABC::PhoneNumber::List::Iterator it;
  for ( it = mPhoneList.begin(); it != mPhoneList.end(); ++it )
    if ( !(*it).number().isEmpty() )
      retList.append( *it );

  return retList;
}

void PhoneEditWidget::edit()
{
  PhoneEditDialog dlg( mPhoneList, this );

  if ( dlg.exec() ) {
    if ( dlg.changed() ) {
      KABC::PhoneNumber::List list = dlg.phoneNumbers();
      setPhoneNumbers( list );
      updateCombos();
      updateLineEdits();
      emit modified();
    }
  }
}

void PhoneEditWidget::updatePrefEdit()
{
  updateEdit( mPrefCombo );
}

void PhoneEditWidget::updateSecondEdit()
{
  updateEdit( mSecondCombo );
}

void PhoneEditWidget::updateThirdEdit()
{
  updateEdit( mThirdCombo );
}

void PhoneEditWidget::updateFourthEdit()
{
  updateEdit( mFourthCombo );
}

void PhoneEditWidget::updateEdit( PhoneTypeCombo *combo )
{
  QLineEdit *edit = combo->lineEdit();
  if ( !edit )
    return;

#if 0
  if ( edit == mPrefEdit ) kdDebug(5720) << " prefEdit" << endl;
  if ( edit == mSecondEdit ) kdDebug(5720) << " secondEdit" << endl;
  if ( edit == mThirdEdit ) kdDebug(5720) << " thirdEdit" << endl;
  if ( edit == mFourthEdit ) kdDebug(5720) << " fourthEdit" << endl;
#endif

  PhoneNumber::List::Iterator it = combo->selectedElement();
  if ( it != mPhoneList.end() ) {
    edit->setText( (*it).number() );
  } else {
    kdDebug(5720) << "PhoneEditWidget::updateEdit(): no selected element" << endl;
  }
}

void PhoneEditWidget::slotPrefEditChanged()
{
  updatePhoneNumber( mPrefCombo );
}

void PhoneEditWidget::slotSecondEditChanged()
{
  updatePhoneNumber( mSecondCombo );
}

void PhoneEditWidget::slotThirdEditChanged()
{
  updatePhoneNumber( mThirdCombo );
}

void PhoneEditWidget::slotFourthEditChanged()
{
  updatePhoneNumber( mFourthCombo );
}

void PhoneEditWidget::updatePhoneNumber( PhoneTypeCombo *combo )
{
  QLineEdit *edit = combo->lineEdit();
  if ( !edit ) return;

  PhoneNumber::List::Iterator it = combo->selectedElement();
  if ( it != mPhoneList.end() ) {
    (*it).setNumber( edit->text() );
  } 

  updateOtherEdit( combo, mPrefCombo );
  updateOtherEdit( combo, mSecondCombo );
  updateOtherEdit( combo, mThirdCombo );
  updateOtherEdit( combo, mFourthCombo );

  emit modified();
}

void PhoneEditWidget::updateOtherEdit( PhoneTypeCombo *combo, PhoneTypeCombo *otherCombo )
{
  if ( combo == otherCombo ) return;

  if ( combo->currentItem() == otherCombo->currentItem() ) {
    updateEdit( otherCombo );
  }
}

///////////////////////////////////////////
// PhoneEditDialog

class PhoneViewItem : public QListViewItem
{
public:
  PhoneViewItem( QListView *parent, const KABC::PhoneNumber &number );

  void setPhoneNumber( const KABC::PhoneNumber &number )
  {
    mPhoneNumber = number;
    makeText();
  }

  QString key() { return mPhoneNumber.id(); }
  QString country() { return ""; }
  QString region() { return ""; }
  QString number() { return ""; }

  KABC::PhoneNumber phoneNumber() { return mPhoneNumber; }

private:
  void makeText();

  KABC::PhoneNumber mPhoneNumber;
};

PhoneViewItem::PhoneViewItem( QListView *parent, const KABC::PhoneNumber &number )
  : QListViewItem( parent ), mPhoneNumber( number )
{ 
#ifdef DESKTOP_VERSION
    setRenameEnabled ( 0, true );
#endif
  makeText();
}

void PhoneViewItem::makeText()
{
  /**
   * Will be used in future versions of kaddressbook/libkabc

    setText( 0, mPhoneNumber.country() );
    setText( 1, mPhoneNumber.region() );
    setText( 2, mPhoneNumber.number() );
    setText( 3, mPhoneNumber.typeLabel() );
   */

  setText( 0, mPhoneNumber.number() );
  setText( 1, mPhoneNumber.typeLabel() );
}

PhoneEditDialog::PhoneEditDialog( const KABC::PhoneNumber::List &list, QWidget *parent, const char *name )
  : KDialogBase( KDialogBase::Plain, i18n( "Edit Phone Numbers" ),
                 KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok,
                 parent, name, true)
{
  mPhoneNumberList = list;

  QWidget *page = plainPage();

  QGridLayout *layout = new QGridLayout( page, 1, 2 );
  layout->setSpacing( spacingHint() );

  mListView = new KListView( page );
  mListView->setAllColumnsShowFocus( true );
  mListView->addColumn( i18n( "Number" ) );
  mListView->addColumn( i18n( "Type" ) );

  KButtonBox *buttonBox = new KButtonBox( page, Vertical );

  buttonBox->addButton( i18n( "&Add..." ), this, SLOT( slotAddPhoneNumber() ) );
  mEditButton = buttonBox->addButton( i18n( "&Edit..." ), this, SLOT( slotEditPhoneNumber() ) );
  mEditButton->setEnabled( false );
  mRemoveButton = buttonBox->addButton( i18n( "&Remove" ), this, SLOT( slotRemovePhoneNumber() ) );
  mRemoveButton->setEnabled( false );
  buttonBox->layout();

  layout->addWidget( mListView, 0, 0 );
  layout->addWidget( buttonBox, 0, 1 );

  connect( mListView, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()) );
  connect( mListView, SIGNAL(doubleClicked( QListViewItem *, const QPoint &, int  )), this, SLOT( slotEditPhoneNumber()));

  KABC::PhoneNumber::List::Iterator it;
  for ( it = mPhoneNumberList.begin(); it != mPhoneNumberList.end(); ++it )
    new PhoneViewItem( mListView, *it );
  if (QApplication::desktop()->width() < 480 )    
      showMaximized();
  else
      resize( 400, 400 );
  mChanged = false;
}

PhoneEditDialog::~PhoneEditDialog()
{
    qDebug("PhoneEditDialog::~PhoneEditDialog() ");
}

void PhoneEditDialog::slotAddPhoneNumber()
{
  KABC::PhoneNumber tmp( "", 0 );
  PhoneTypeDialog dlg( tmp, this );

  if ( dlg.exec() ) {
      QListViewItem* i = mListView->firstChild();
      KABC::PhoneNumber phoneNumber = dlg.phoneNumber();
      bool insert = true;
      while ( i ) {
          PhoneViewItem* p = ( PhoneViewItem* ) i;
          KABC::PhoneNumber pn = p->phoneNumber();
          if ( (pn.type() | KABC::PhoneNumber::Pref)  == (phoneNumber.type() | KABC::PhoneNumber::Pref)  ) {
              if ( p->text(0).isEmpty()) {
                  p->setPhoneNumber( phoneNumber  );
                  mPhoneNumberList.remove( pn );
                  mPhoneNumberList.append( phoneNumber );
                  insert = false;
                  break;
              }
          }
          i = i->nextSibling();
      }
      if ( insert ) {
          mPhoneNumberList.append( phoneNumber );
          new PhoneViewItem( mListView, phoneNumber );
      }
      mChanged = true;
  }
}

void PhoneEditDialog::slotRemovePhoneNumber()
{
  PhoneViewItem *item = static_cast<PhoneViewItem*>( mListView->currentItem() );
  if ( !item )
    return;

  mPhoneNumberList.remove( item->phoneNumber() );
  QListViewItem *currItem = mListView->currentItem();
  mListView->takeItem( currItem );
  delete currItem;

  mChanged = true;
}

void PhoneEditDialog::slotEditPhoneNumber()
{
  PhoneViewItem *item = static_cast<PhoneViewItem*>( mListView->currentItem() );
  if ( !item )
    return;

  PhoneTypeDialog dlg( item->phoneNumber(), this );

  if ( dlg.exec() ) {
    slotRemovePhoneNumber();
    KABC::PhoneNumber phoneNumber = dlg.phoneNumber();
    mPhoneNumberList.append( phoneNumber );
    new PhoneViewItem( mListView, phoneNumber );

    mChanged = true;
  }
}

void PhoneEditDialog::slotSelectionChanged()
{
  bool state = ( mListView->currentItem() != 0 );

  mRemoveButton->setEnabled( state );
  mEditButton->setEnabled( state );
}

const KABC::PhoneNumber::List &PhoneEditDialog::phoneNumbers()
{
  return mPhoneNumberList;
}

bool PhoneEditDialog::changed() const
{
  return mChanged;
}

///////////////////////////////////////////
// PhoneTypeDialog
PhoneTypeDialog::PhoneTypeDialog( const KABC::PhoneNumber &phoneNumber,
                               QWidget *parent, const char *name)
  : KDialogBase( KDialogBase::Plain, i18n( "Edit Phone Number" ),
                KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok,
                parent, name, true), mPhoneNumber( phoneNumber )
{
  QWidget *page = plainPage();
  QLabel *label = 0;
  QGridLayout *layout = new QGridLayout( page, 3, 2, marginHint(), spacingHint() );

  label = new QLabel( i18n( "Number:" ), page );
  layout->addWidget( label, 0, 0 );
  mNumber = new KLineEdit( page );
  layout->addWidget( mNumber, 0, 1 );

  mPreferredBox = new QCheckBox( i18n( "This is the preferred phone number" ), page );
  layout->addMultiCellWidget( mPreferredBox, 1, 1, 0, 1 );

  mGroup = new QButtonGroup( 2, Horizontal, i18n( "Types" ), page );
  layout->addMultiCellWidget( mGroup, 2, 2, 0, 1 );

  // fill widgets
  mNumber->setText( mPhoneNumber.number() );

  mTypeList = KABC::PhoneNumber::typeList();
  mTypeList.remove( KABC::PhoneNumber::Pref );

  KABC::PhoneNumber::TypeList::Iterator it;
  for ( it = mTypeList.begin(); it != mTypeList.end(); ++it )
    new QCheckBox( KABC::PhoneNumber::typeLabel( *it ), mGroup );

  for ( int i = 0; i < mGroup->count(); ++i ) {
    int type = mPhoneNumber.type();
    QCheckBox *box = (QCheckBox*)mGroup->find( i );
    box->setChecked( type & mTypeList[ i ] );
  }

  mPreferredBox->setChecked( mPhoneNumber.type() & KABC::PhoneNumber::Pref );
  mNumber->setFocus();
  mNumber->setSelection( 0, 1024);
}

KABC::PhoneNumber PhoneTypeDialog::phoneNumber()
{
  mPhoneNumber.setNumber( mNumber->text() );

  int type = 0;
  for ( int i = 0; i < mGroup->count(); ++i ) {
    QCheckBox *box = (QCheckBox*)mGroup->find( i );
    if ( box->isChecked() )
      type += mTypeList[ i ];
  }

  if ( mPreferredBox->isChecked() )
    mPhoneNumber.setType( type | KABC::PhoneNumber::Pref );
  else
    mPhoneNumber.setType( type & ~KABC::PhoneNumber::Pref );

  return mPhoneNumber;
}
#endif
#ifndef KAB_EMBEDDED
#include "phoneeditwidget.moc"
#endif //KAB_EMBEDDED
