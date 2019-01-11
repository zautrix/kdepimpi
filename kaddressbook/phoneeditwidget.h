#ifndef PHONEEDITWIDGET_H
#define PHONEEDITWIDGET_H
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

#include <kdialogbase.h>
#include <kiconloader.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qapplication.h> 

#include <qptrlist.h>
#include <qscrollview.h>

#include "addresseeconfig.h"
#include "typecombo.h"

class QButtonGroup;
class QCheckBox;
class PhoneTypeNumberEdit;

#include <klineedit.h>
#include <kcombobox.h>
#include <kabc/phonenumber.h>

typedef TypeCombo<KABC::PhoneNumber> PhoneTypeCombo;

/**
  Widget for editing phone numbers.
*/
class PhoneEditWidget : public QWidget
{
  Q_OBJECT

  public:
    PhoneEditWidget( QWidget *parent, const char *name = 0 );
    ~PhoneEditWidget();
    
    void setPhoneNumbers( const KABC::PhoneNumber::List &list );
    KABC::PhoneNumber::List phoneNumbers();

    //  void updateTypeCombo( const KABC::PhoneNumber::List&, KComboBox* );
    //KABC::PhoneNumber currentPhoneNumber( KComboBox*, int );

  signals:
    void modified();
    void  typeChange( int oldType, int newType );

  private slots:
    void bottomVisible();
    void addNumberInt( int );
    void deleteEdit(  PhoneTypeNumberEdit* ew );
    void addNumber();
    void pendingDelete();
  protected:

  private:
    QScrollView* sv;
    QPopupMenu *mPopup;
    int mPopupCount;
    PhoneTypeNumberEdit* mPendingDelete;
    void setDefaults();
    PhoneTypeNumberEdit* appendEditCombo();
    QWidget* mw;
    QVBoxLayout* mainLayout;
    QPtrList <PhoneTypeNumberEdit> mTypeNumberEditList;
    
};
  

class PhoneTypeNumberEdit : public QWidget
{
  Q_OBJECT
public:
    PhoneTypeNumberEdit( QWidget *parent, const char *name = 0 ) :QWidget( parent )
      {
          QHBoxLayout * lay = new QHBoxLayout( this );
          lay->setSpacing( 2 );
          lay->setMargin( 0 );
          mMinusButton = new QPushButton ( this );
          mMinusButton->setPixmap ( SmallIcon("minus"));
          mCombo = new KComboBox( this );
          if ( QApplication::desktop()->width() <= 640 )
              mCombo->setSizeLimit ( 6 );
          mNumber = new KLineEdit( this );
          mMinusButton->setMaximumHeight( mNumber->sizeHint().height() + 6);
          lay->addWidget( mMinusButton  );
          lay->addWidget( mCombo );
          lay->addWidget( mNumber  );
          connect( mMinusButton , SIGNAL ( clicked() ), this, SLOT ( deleteNumber() ) );
          connect( mCombo , SIGNAL ( activated ( int ) ), this, SLOT ( comboTypeChange( int ) ) );
          connect( mNumber  , SIGNAL (  textChanged ( const QString & ) ), 
                   this, SLOT (  textChanged ( const QString & ) ) );
          mCombo->insertStringList(  PhoneNumber::supportedTypeListNames() );
      }
        ~PhoneTypeNumberEdit() {
            // qDebug("~PhoneTypeNumberEdit() ");
        }
  void setPhoneNumber( const KABC::PhoneNumber &phoneNumber )
  {
      mPhoneNumber = phoneNumber;
      int index = PhoneNumber::typeListIndex4Type( mPhoneNumber.type() );
      mCombo->setCurrentItem( index );
      mNumber->setText( mPhoneNumber.number() );
      show();
      mNumber->setFocus();
  }
  KABC::PhoneNumber phoneNumber()
      {
          mPhoneNumber.setNumber( mNumber->text() );
          int index = mCombo->currentItem();
          mPhoneNumber.setType( PhoneNumber::supportedTypeList()[index]  );
          return mPhoneNumber;
      }
  bool isValid()
  {
      if ( mNumber->text().isEmpty() )return false;
      return true;
  }
  int currentType()
  {
      return mCombo->currentItem();
  }
  private slots:
  void  typeExternalChanged( int oldType, int newType )
  {
      if ( mPhoneNumber.type() == newType ) {
          mPhoneNumber.setType(oldType);
          int index = PhoneNumber::typeListIndex4Type( mPhoneNumber.type() );
          mCombo->setCurrentItem( index );
      }
  }
  void deleteNumber()
  { 
      emit deleteMe( this );
  }
  void comboTypeChange( int index )
  {
      int old = mPhoneNumber.type();
      int newT = PhoneNumber::supportedTypeList()[index];
      if ( old != newT ) {
          emit modified();
          if ( newT != PhoneNumber::Voice )
              emit typeChange ( old, newT );
          mPhoneNumber.setType(newT );
      }

  }
  void textChanged ( const QString & )
  {
      emit modified();
  }
 signals:
void  typeChange( int oldType, int newType );
 void modified();
 void deleteMe(  PhoneTypeNumberEdit* );

private:
  KABC::PhoneNumber mPhoneNumber;
  QPushButton* mMinusButton;
  KComboBox *mCombo;
  KLineEdit *mNumber;
};

#endif
