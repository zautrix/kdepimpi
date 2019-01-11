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

#include <qcheckbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qapplication.h>

#ifndef KAB_EMBEDDED    
#include <qtextedit.h>

#include <kaccelmanager.h>
#include "keywidget.h"
#include "soundwidget.h"

#else //KAB_EMBEDDED    
#include <qmultilineedit.h>
#endif //KAB_EMBEDDED    


#include "keywidget.h"
#include "geowidget.h"
#include "imagewidget.h"
#include "nameeditdialog.h"
#include "phoneeditwidget.h"
#include "secrecywidget.h"


#include <qtoolbutton.h>
#include <qtooltip.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kseparator.h>
#include <ksqueezedtextlabel.h>

#include <libkdepim/categoryeditdialog.h>
#include <libkdepim/categoryselectdialog.h>

#include <libkdepim/kdateedit.h>

#include "addresseditwidget.h"
#include "emaileditwidget.h"
#include "kabcore.h"
#include "kabprefs.h"

#include "addresseeeditorwidget.h"



AddresseeEditorWidget::AddresseeEditorWidget( KABCore *core, bool isExtension,
                                              QWidget *parent, const char *name )
  : ExtensionWidget( core, parent, name ), mIsExtension( isExtension ),
    mBlockSignals( false )
{ 

    mAConfig = AddresseeConfig::instance();
    
  mFormattedNameType = NameEditDialog::CustomName;
  initGUI();
  mCategoryDialog = 0;
  mCategoryEditDialog = 0;

  // Load the empty addressee as defaults
  load();
 
  mDirty = false;
}

AddresseeEditorWidget::~AddresseeEditorWidget()
{
  kdDebug(5720) << "~AddresseeEditorWidget()" << endl;
}  
  
void AddresseeEditorWidget::contactsSelectionChanged()
{
  KABC::Addressee::List list = selectedContacts();

  mAddressee = list[ 0 ];
  load();
}

void AddresseeEditorWidget::setAddressee( const KABC::Addressee &addr )
{
  mAddressee = addr;
  load();
}

const KABC::Addressee &AddresseeEditorWidget::addressee()
{
  return mAddressee;
}

void AddresseeEditorWidget::textChanged( const QString& )
{
  emitModified();
}
  
void AddresseeEditorWidget::initGUI()
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  mTabWidget = new QTabWidget( this );
  layout->addWidget( mTabWidget );

  setupTab1();
  setupTab1_1();
  setupTab2();
  setupTab2_1();
  setupTab3();
  setupTab3_1();

  mNameEdit->setFocus();
  
  connect( mTabWidget, SIGNAL( currentChanged(QWidget*) ),
           SLOT( pageChanged(QWidget*) ) );
}

void AddresseeEditorWidget::setupTab1()
{
  // This is the General tab
  QWidget *tab1 = new QWidget( mTabWidget );

//US  QGridLayout *layout = new QGridLayout( tab1, 11, 7 );
  bool horLayout = false;
  int maxCol = 1;
  if ( QApplication::desktop()->width() == 640 || QApplication::desktop()->width() == 320 ) {
      horLayout = true;
      maxCol = 3;
  }
  QGridLayout *layout = new QGridLayout( tab1, 7-maxCol, maxCol );

  layout->setMargin( KDialogBase::marginHintSmall() );
  layout->setSpacing( KDialogBase::spacingHintSmall() );
  
  QLabel *label;
  KSeparator* bar;
  QPushButton *button;
  
  //////////////////////////////////
  // Upper left group (person info)
  
  // Person icon
  /* LR
  label = new QLabel( tab1 );
//US ambiguous call. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop, 0 ) );
  layout->addMultiCellWidget( label, 0, 1, 0, 0 );
  */
  // First name
  button = new QPushButton( i18n( "Name..." ), tab1 );
//US  QToolTip::add( button, i18n( "Edit the contact's name" ) );
  mNameEdit = new KLineEdit( tab1, "mNameEdit" );
  connect( mNameEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( nameTextChanged( const QString& ) ) );
  connect( button, SIGNAL( clicked() ), SLOT( nameButtonClicked() ) );
  
  mNameLabel = new KSqueezedTextLabel( tab1 );
  mNameLabel->hide();
  
  layout->addWidget( button, 0, 0 );
  layout->addWidget( mNameEdit, 0, 1 );
  layout->addWidget( mNameLabel, 0, 1 );
  
  button = new QPushButton( i18n( "Role:" ), tab1 );
  connect( button, SIGNAL( clicked() ), SLOT( setRole2FN() ) );
  //label = new QLabel( i18n( "Role:" ), tab1 );
  mRoleEdit = new KLineEdit( tab1 );
  connect( mRoleEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  //label->setBuddy( mRoleEdit );
  if ( horLayout ) {
      layout->addWidget( button, 0, 2 );
      layout->addWidget( mRoleEdit, 0, 3 );
      
  } else {
      layout->addWidget( button, 1, 0 );
      layout->addWidget( mRoleEdit, 1, 1 );
  }
  // Organization
  button = new QPushButton( i18n( "Organization:" ), tab1 );
  connect( button, SIGNAL( clicked() ), SLOT( setCompany2FN() ) );
  //label = new QLabel( i18n( "Organization:" ), tab1 );
  mOrgEdit = new KLineEdit( tab1 );
  //label->setBuddy( mOrgEdit );
  connect( mOrgEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( textChanged( const QString& ) ) );
  if ( horLayout ) {
      layout->addWidget( button, 1, 2 );
      layout->addWidget( mOrgEdit, 1, 3 );

  } else {
      layout->addWidget( button, 2, 0 );
      layout->addWidget( mOrgEdit, 2, 1 );
  }
  
  // File as (formatted name)
  label = new QLabel( i18n( "Format.n.:" ), tab1 );
  mFormattedNameLabel = new KSqueezedTextLabel( tab1 );
  if ( horLayout ) {
      layout->addWidget( label, 1,0 );
      layout->addWidget( mFormattedNameLabel, 1, 1 );
  } else {
      layout->addWidget( label, 3, 0 );
      layout->addWidget( mFormattedNameLabel, 3, 1 );
  }
  /* LR
  // Left hand separator. This separator doesn't go all the way
  // across so the dialog still flows from top to bottom
  bar = new KSeparator( KSeparator::HLine, tab1 );
  layout->addMultiCellWidget( bar, 4, 4, 0, 2 );
  */
  //////////////////////////////////////

  /* LR
  // Phone numbers (upper right)
  label = new QLabel( tab1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "kaddressbook", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "kaddressbook", KIcon::Desktop, 0 ) );
//US  layout->addMultiCellWidget( label, 0, 1, 3, 3 );
  layout->addMultiCellWidget( label, 5, 6, 0, 0 );
  */
  mPhoneEditWidget = new PhoneEditWidget( tab1 );
  connect( mPhoneEditWidget, SIGNAL( modified() ), SLOT( emitModified() ) );
//US  layout->addMultiCellWidget( mPhoneEditWidget, 0, 3, 4, 6 ); 
  int iii;
#ifndef DESKTOP_VERSION
  iii = 7;
#else
  iii = 8;
#endif
  layout->addMultiCellWidget( mPhoneEditWidget, 5-maxCol,  5-maxCol+4, 0, maxCol ); 
  iii = 6-maxCol+4;
  /* LR
  bar = new KSeparator( KSeparator::HLine, tab1 );
//US  layout->addMultiCellWidget( bar, 4, 4, 3, 6 );
  layout->addMultiCellWidget( bar, 9, 9, 0, 2 );
  */
/*US  
  //////////////////////////////////////
  // Addresses (lower left)
  label = new QLabel( tab1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "gohome", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "gohome", KIcon::Desktop, 0 ) );
  layout->addMultiCellWidget( label, 5, 6, 0, 0 );
  
  mAddressEditWidget = new AddressEditWidget( tab1 );
  connect( mAddressEditWidget, SIGNAL( modified() ), SLOT( emitModified() ) );
  layout->addMultiCellWidget( mAddressEditWidget, 5, 9, 1, 2 );

  //////////////////////////////////////
  // Email / Web (lower right)
  label = new QLabel( tab1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "email", KIcon::Desktop) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "email", KIcon::Desktop, 0) );
  layout->addMultiCellWidget( label, 5, 6, 3, 3 );

  mEmailWidget = new EmailEditWidget( tab1 );
  connect( mEmailWidget, SIGNAL( modified() ), SLOT( emitModified() ) );
  layout->addMultiCellWidget( mEmailWidget, 5, 6, 4, 6 );

  // add the separator
  bar = new KSeparator( KSeparator::HLine, tab1 );
  layout->addMultiCellWidget( bar, 7, 7, 3, 6 );

  label = new QLabel( tab1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "homepage", KIcon::Desktop) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "homepage", KIcon::Desktop, 0) );
  layout->addMultiCellWidget( label, 8, 9, 3, 3 );

  label = new QLabel( i18n( "URL:" ), tab1 );
  mURLEdit = new KLineEdit( tab1 );
  connect( mURLEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mURLEdit );
  layout->addWidget( label, 8, 4 );
  layout->addMultiCellWidget( mURLEdit, 8, 8, 5, 6 );

  label = new QLabel( i18n( "&IM address:" ), tab1 );
  mIMAddressEdit = new KLineEdit( tab1 );
  connect( mIMAddressEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mIMAddressEdit );
  layout->addWidget( label, 9, 4 );
  layout->addMultiCellWidget( mIMAddressEdit, 9, 9, 5, 6 );
  
  layout->addColSpacing( 6, 50 );
  
  bar = new KSeparator( KSeparator::HLine, tab1 );
  layout->addMultiCellWidget( bar, 10, 10, 0, 6 );
*/  
  ///////////////////////////////////////
      QHBox *categoryBox = new QHBox( tab1 ,"cato");
  categoryBox->setSpacing( KDialogBase::spacingHint() );
  categoryBox->setMargin( KDialogBase::marginHintSmall() );
  
  // Categories
  button = new QPushButton( i18n( "Categories" )+":", categoryBox );
  connect( button, SIGNAL( clicked() ), SLOT( categoryButtonClicked() ) );

  mCategoryEdit = new QPushButton ( categoryBox ); 
  mCategoryEdit->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred  ,QSizePolicy::Fixed  ,FALSE) );
  mCatPopup = new QPopupMenu ( categoryBox );
  mCategoryEdit->setPopup( mCatPopup );
  connect(mCatPopup,SIGNAL(aboutToShow ()  ), this ,SLOT(showCatPopup()));
  connect(mCatPopup,SIGNAL(  activated ( int ) ), this ,SLOT(selectedCatPopup( int )));
  //connect( mCategoryEdit, SIGNAL( textChanged( const QString& ) ), 
  //       SLOT( textChanged( const QString& ) ) );

  mSecrecyWidget = new SecrecyWidget( categoryBox );
  connect( mSecrecyWidget, SIGNAL( changed() ), SLOT( emitModified() ) );

//US  layout->addMultiCellWidget( categoryBox, 11, 11, 0, 6 );
  layout->addMultiCellWidget( categoryBox, iii, iii, 0, maxCol );
  
  // Build the layout and add to the tab widget
  layout->activate(); // required

  mTabWidget->addTab( tab1, i18n( "&General" ) );
}

void AddresseeEditorWidget::showCatPopup()
{
    mCatPopup->clear(); 
    QStringList checkedCategories = QStringList::split (",", mCategoryEdit->text());
    int index = 0;
    for (QStringList::Iterator it = KABPrefs::instance()->mCustomCategories.begin ();
         it != KABPrefs::instance()->mCustomCategories.end ();
         ++it) {
        mCatPopup->insertItem (*it, index );
        //mCategory[index] = *it;
        if (checkedCategories.find (*it) != checkedCategories.end ()) mCatPopup->setItemChecked (index, true);
        ++index;
    }
}
void AddresseeEditorWidget::selectedCatPopup( int index )
{
 QStringList categories =  QStringList::split (",", mCategoryEdit->text());
    QString colcat = categories.first();
    if (categories.find ( KABPrefs::instance()->mCustomCategories[index]) != categories.end ())
      categories.remove (KABPrefs::instance()->mCustomCategories[index]);
    else
        categories.insert (categories.end(), KABPrefs::instance()->mCustomCategories[index]);
    categories.sort ();
    if ( !colcat.isEmpty() ) {
        if ( categories.find ( colcat ) != categories.end () ) {
            categories.remove( colcat );
            categories.prepend( colcat );
        }
    }
    mCategoryEdit->setText( categories.join(",") );
    emitModified();
}
void AddresseeEditorWidget::setRole2FN()
{
    if ( mRoleEdit->text().isEmpty() ) return;
    mFormattedNameType = NameEditDialog::CustomName;
    mAddressee.setFormattedName( mRoleEdit->text() ); 
    mFormattedNameLabel->setText( mRoleEdit->text() );
    emitModified();
}
void AddresseeEditorWidget::setCompany2FN()
{
    if (  mOrgEdit->text().isEmpty() ) return;
    mFormattedNameType = NameEditDialog::CustomName;
    mAddressee.setFormattedName( mOrgEdit->text() );
    mFormattedNameLabel->setText( mOrgEdit->text() );
    emitModified();
}

void AddresseeEditorWidget::setupTab1_1()
{
  // This is the Address tab
  QWidget *tab1_1 = new QWidget( mTabWidget );

//US  QGridLayout *layout = new QGridLayout( tab1_1, 11, 7 );
  QGridLayout *layout = new QGridLayout( tab1_1, 7, 2 );
  layout->setMargin( KDialogBase::marginHintSmall() );
  layout->setSpacing( KDialogBase::spacingHintSmall() );
  
  QLabel *label;
  KSeparator* bar;
  QPushButton *button;

/*US    
  //////////////////////////////////
  // Upper left group (person info)
  
  // Person icon
  label = new QLabel( tab1 );
//US ambiguous call. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop, 0 ) );
  layout->addMultiCellWidget( label, 0, 1, 0, 0 );
  
  // First name
  button = new QPushButton( i18n( "Name..." ), tab1 );
  QToolTip::add( button, i18n( "Edit the contact's name" ) );
  mNameEdit = new KLineEdit( tab1, "mNameEdit" );
  connect( mNameEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( nameTextChanged( const QString& ) ) );
  connect( button, SIGNAL( clicked() ), SLOT( nameButtonClicked() ) );
  
#ifndef KAB_EMBEDDED    
  mNameLabel = new KSqueezedTextLabel( tab1 );
  mNameLabel->hide();
#else //KAB_EMBEDDED    
qDebug("AddresseeEditorWidget::setupTab1 has to be changed");
#endif //KAB_EMBEDDED    
  
  layout->addWidget( button, 0, 1 );
  layout->addWidget( mNameEdit, 0, 2 );
  
#ifndef KAB_EMBEDDED    
  layout->addWidget( mNameLabel, 0, 2 );
#else //KAB_EMBEDDED    
qDebug("AddresseeEditorWidget::setupTab1 has to be changed");
#endif //KAB_EMBEDDED    
  
  label = new QLabel( i18n( "Role:" ), tab1 );
  mRoleEdit = new KLineEdit( tab1 );
  connect( mRoleEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mRoleEdit );
  layout->addWidget( label, 1, 1 );
  layout->addWidget( mRoleEdit, 1, 2 );
  
  // Organization
  label = new QLabel( i18n( "Organization:" ), tab1 );
  mOrgEdit = new KLineEdit( tab1 );
  label->setBuddy( mOrgEdit );
  connect( mOrgEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( textChanged( const QString& ) ) );
  layout->addWidget( label, 2, 1 );
  layout->addWidget( mOrgEdit, 2, 2 );
  
  // File as (formatted name)
  label = new QLabel( i18n( "Formatted name:" ), tab1 );
#ifndef KAB_EMBEDDED    
  mFormattedNameLabel = new KSqueezedTextLabel( tab1 );
#else //KAB_EMBEDDED    
qDebug("AddresseeEditorWidget::setupTab1 has to be changed");
#endif //KAB_EMBEDDED    
  layout->addWidget( label, 3, 1 );
#ifndef KAB_EMBEDDED    
  layout->addWidget( mFormattedNameLabel, 3, 2 );
#else //KAB_EMBEDDED    
qDebug("AddresseeEditorWidget::setupTab1 has to be changed");
#endif //KAB_EMBEDDED    
  
  // Left hand separator. This separator doesn't go all the way
  // across so the dialog still flows from top to bottom
  bar = new KSeparator( KSeparator::HLine, tab1 );
  layout->addMultiCellWidget( bar, 4, 4, 0, 2 );
  
  //////////////////////////////////////
  // Phone numbers (upper right)
  label = new QLabel( tab1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "kaddressbook", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "kaddressbook", KIcon::Desktop, 0 ) );
  layout->addMultiCellWidget( label, 0, 1, 3, 3 );
  
  mPhoneEditWidget = new PhoneEditWidget( tab1 );
  connect( mPhoneEditWidget, SIGNAL( modified() ), SLOT( emitModified() ) );
  layout->addMultiCellWidget( mPhoneEditWidget, 0, 3, 4, 6 ); 

  bar = new KSeparator( KSeparator::HLine, tab1 );
  layout->addMultiCellWidget( bar, 4, 4, 3, 6 );
*/  
  //////////////////////////////////////
  // Addresses (lower left)
  /* LR 
  label = new QLabel( tab1_1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "gohome", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "gohome", KIcon::Desktop, 0 ) );
//US  layout->addMultiCellWidget( label, 5, 6, 0, 0 );
  layout->addMultiCellWidget( label, 0, 1, 0, 0 );
  */
  
  mAddressEditWidget = new AddressEditWidget( tab1_1 );
  connect( mAddressEditWidget, SIGNAL( modified() ), SLOT( emitModified() ) );
//US  layout->addMultiCellWidget( mAddressEditWidget, 5, 9, 1, 2 );
  layout->addMultiCellWidget( mAddressEditWidget, 0, 4, 0, 1 );

//US  
/* LR
  bar = new KSeparator( KSeparator::HLine, tab1_1 );
  layout->addMultiCellWidget( bar, 5, 5, 0, 3 );
*/
  
  //////////////////////////////////////
  // Email / Web (lower right)
  /* LR
  label = new QLabel( tab1_1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "email", KIcon::Desktop) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "email", KIcon::Desktop, 0) );
//US  layout->addMultiCellWidget( label, 5, 6, 3, 3 );
  layout->addMultiCellWidget( label, 6, 7, 0, 0 );
  */
  mEmailWidget = new EmailEditWidget( tab1_1 );
  connect( mEmailWidget, SIGNAL( modified() ), SLOT( emitModified() ) );
//US  layout->addMultiCellWidget( mEmailWidget, 5, 6, 4, 6 );
  layout->addMultiCellWidget( mEmailWidget, 5, 6, 0, 1 );

  /* LR
  // add the separator
  bar = new KSeparator( KSeparator::HLine, tab1_1 );
//US  layout->addMultiCellWidget( bar, 7, 7, 3, 6 );
  layout->addMultiCellWidget( bar, 8, 8, 0, 3 );
  
  label = new QLabel( tab1_1 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "homepage", KIcon::Desktop) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "homepage", KIcon::Desktop, 0) );
//US  layout->addMultiCellWidget( label, 8, 9, 3, 3 );
  layout->addMultiCellWidget( label, 9, 10, 0, 0 );
  */
  label = new QLabel( i18n( "URL:" ), tab1_1 );
  mURLEdit = new KLineEdit( tab1_1 );
  connect( mURLEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mURLEdit );
//US  layout->addWidget( label, 8, 4 );
  layout->addWidget( label, 7,0 );
//US  layout->addMultiCellWidget( mURLEdit, 8, 8, 5, 6 );
  layout->addWidget( mURLEdit, 7, 1);

  label = new QLabel( i18n( "&IM address:" ), tab1_1 );
  mIMAddressEdit = new KLineEdit( tab1_1 );
  connect( mIMAddressEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mIMAddressEdit );
//US  layout->addWidget( label, 9, 4 );
  layout->addWidget( label, 8, 0 );
//US  layout->addMultiCellWidget( mIMAddressEdit, 9, 9, 5, 6 );
  layout->addWidget( mIMAddressEdit, 8,1 );
  
//US  layout->addColSpacing( 6, 50 );
  
//US  bar = new KSeparator( KSeparator::HLine, tab1_1 );
//US  layout->addMultiCellWidget( bar, 10, 10, 0, 6 );

/*US  
  ///////////////////////////////////////
  QHBox *categoryBox = new QHBox( tab1 );
  categoryBox->setSpacing( KDialogBase::spacingHintSmall() );
  
  // Categories
  button = new QPushButton( i18n( "Categories" ), categoryBox );
  connect( button, SIGNAL( clicked() ), SLOT( categoryButtonClicked() ) );

  mCategoryEdit = new KLineEdit( categoryBox );
  mCategoryEdit->setReadOnly( true );
  connect( mCategoryEdit, SIGNAL( textChanged( const QString& ) ), 
           SLOT( textChanged( const QString& ) ) );

  mSecrecyWidget = new SecrecyWidget( categoryBox );
  connect( mSecrecyWidget, SIGNAL( changed() ), SLOT( emitModified() ) );

  layout->addMultiCellWidget( categoryBox, 11, 11, 0, 6 );
*/  
  // Build the layout and add to the tab widget
  layout->activate(); // required

  mTabWidget->addTab( tab1_1, i18n( "&Address" ) );
}



void AddresseeEditorWidget::setupTab2()
{
  // This is the Details tab
  QWidget *tab2 = new QWidget( mTabWidget );

  QGridLayout *layout = new QGridLayout( tab2, 8, 3 );
  layout->setMargin( KDialogBase::marginHintSmall() );
  layout->setSpacing( KDialogBase::spacingHintSmall() );
  
  QLabel *label;
  KSeparator* bar;
  
  ///////////////////////
  // Office info
  
  // Department
  label = new QLabel( tab2 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "folder", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "folder", KIcon::Desktop, 0 ) );
  layout->addMultiCellWidget( label, 0, 1, 0, 0 );
  
  label = new QLabel( i18n( "Department:" ), tab2 );
  layout->addWidget( label, 0, 1 );
  mDepartmentEdit = new KLineEdit( tab2 );
  connect( mDepartmentEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mDepartmentEdit );
  layout->addWidget( mDepartmentEdit, 0, 2 );
  
  label = new QLabel( i18n( "Office:" ), tab2 );
  layout->addWidget( label, 1, 1 );
  mOfficeEdit = new KLineEdit( tab2 );
  connect( mOfficeEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mOfficeEdit );
  layout->addWidget( mOfficeEdit, 1, 2 );
  
  label = new QLabel( i18n( "Profession:" ), tab2 );
  layout->addWidget( label, 2, 1 );
  mProfessionEdit = new KLineEdit( tab2 );
  connect( mProfessionEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mProfessionEdit );
  layout->addWidget( mProfessionEdit, 2, 2 );
  
  label = new QLabel( i18n( "Manager\'s name:" ), tab2 );
//US  layout->addWidget( label, 0, 3 );
  layout->addWidget( label, 3, 1 );
  mManagerEdit = new KLineEdit( tab2 );
  connect( mManagerEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mManagerEdit );
//US  layout->addMultiCellWidget( mManagerEdit, 0, 0, 4, 5 );
  layout->addWidget( mManagerEdit, 3, 2 );
  
  label = new QLabel( i18n( "Assistant's name:" ), tab2 );
//US  layout->addWidget( label, 1, 3 );
  layout->addWidget( label, 4, 1 );
  mAssistantEdit = new KLineEdit( tab2 );
  connect( mAssistantEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mAssistantEdit );
//US  layout->addMultiCellWidget( mAssistantEdit, 1, 1, 4, 5 );
  layout->addWidget( mAssistantEdit, 4, 2 );
  
  bar = new KSeparator( KSeparator::HLine, tab2 );
//US  layout->addMultiCellWidget( bar, 3, 3, 0, 5 );
  layout->addMultiCellWidget( bar, 5, 5, 0, 2 );
  
  /////////////////////////////////////////////////
  // Personal info
  
  //label = new QLabel( tab2 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop ) );
      //label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop, 0 ) );
//US  layout->addMultiCellWidget( label, 4, 5, 0, 0 );
      //layout->addMultiCellWidget( label, 6, 7, 0, 0 );


      int iii = 6;

      if ( QApplication::desktop()->width() == 640 ) {
          QHBox * nbox = new QHBox ( tab2 );
          label = new QLabel( i18n( "Nick name:" )+" ", nbox );
          mNicknameEdit = new KLineEdit(  nbox );
          connect( mNicknameEdit, SIGNAL( textChanged( const QString& ) ),
                   SLOT( textChanged( const QString& ) ) );
          label->setBuddy( mNicknameEdit );

          label = new QLabel( " "+i18n( "Spouse's name:" )+" ",  nbox );
          mSpouseEdit = new KLineEdit(  nbox );
          connect( mSpouseEdit, SIGNAL( textChanged( const QString& ) ),
                   SLOT( textChanged( const QString& ) ) );
          label->setBuddy( mSpouseEdit );
          layout->addMultiCellWidget( nbox, iii, iii, 1, 2 );
          ++iii;

      } else {
          label = new QLabel( i18n( "Nick name:" ), tab2 );
          layout->addWidget( label, iii, 1 );
          mNicknameEdit = new KLineEdit( tab2 );
          connect( mNicknameEdit, SIGNAL( textChanged( const QString& ) ),
                   SLOT( textChanged( const QString& ) ) );
          label->setBuddy( mNicknameEdit );
          layout->addWidget( mNicknameEdit, iii, 2 );
          ++iii;

          label = new QLabel( i18n( "Spouse's name:" ), tab2 );
          layout->addWidget( label, iii, 1 );
          mSpouseEdit = new KLineEdit( tab2 );
          connect( mSpouseEdit, SIGNAL( textChanged( const QString& ) ),
                   SLOT( textChanged( const QString& ) ) );
          label->setBuddy( mSpouseEdit );
          layout->addWidget( mSpouseEdit, iii, 2 );
          ++iii;
      }

      label = new QLabel( i18n( "Children's names:" ), tab2 );
      layout->addWidget( label, iii, 1 );
      mChildEdit = new KLineEdit( tab2 );
      connect( mChildEdit, SIGNAL( textChanged( const QString& ) ),
               SLOT( textChanged( const QString& ) ) );
      label->setBuddy( mChildEdit );
      layout->addWidget( mChildEdit, iii, 2 );
      ++iii;
      if ( QApplication::desktop()->width() == 640 ) {
          QHBox * nbox = new QHBox ( tab2 );
          label = new QLabel( i18n( "Birthday:" )+" ", nbox );
          mBirthdayPicker = new KDateEdit(  nbox  );
          //mBirthdayPicker->toggleDateFormat();
          mBirthdayPicker->setHandleInvalid( true );
          connect( mBirthdayPicker, SIGNAL( dateChanged( QDate ) ),
                   SLOT( dateChanged( QDate ) ) );
           
          label->setBuddy( mBirthdayPicker );
  
          label = new QLabel(  " "+i18n( "Anniversary:" )+" ",  nbox  );
          mAnniversaryPicker = new KDateEdit(  nbox  );
          mAnniversaryPicker->setHandleInvalid( true );
          connect( mAnniversaryPicker, SIGNAL( dateChanged( QDate ) ),
                   SLOT( dateChanged( QDate ) ) );
           
          label->setBuddy( mAnniversaryPicker );
          layout->addMultiCellWidget( nbox, iii, iii, 1, 2 );
          ++iii;

      } else {

          label = new QLabel( i18n( "Birthday:" ), tab2 );
          layout->addWidget( label, iii, 1 );
          mBirthdayPicker = new KDateEdit( tab2 );
          //mBirthdayPicker->toggleDateFormat();
          mBirthdayPicker->setHandleInvalid( true );
          connect( mBirthdayPicker, SIGNAL( dateChanged( QDate ) ),
                   SLOT( dateChanged( QDate ) ) );
           
          label->setBuddy( mBirthdayPicker );
          layout->addWidget( mBirthdayPicker, iii, 2 );
          ++iii;
  
          label = new QLabel( i18n( "Anniversary:" ), tab2 );
          layout->addWidget( label, iii, 1 );
          mAnniversaryPicker = new KDateEdit( tab2 );
          mAnniversaryPicker->setHandleInvalid( true );
          connect( mAnniversaryPicker, SIGNAL( dateChanged( QDate ) ),
                   SLOT( dateChanged( QDate ) ) );
           
          label->setBuddy( mAnniversaryPicker );
          layout->addWidget( mAnniversaryPicker, iii, 2 );
          ++iii;

      }

      label = new QLabel( i18n( "Gender:" ), tab2 );
      layout->addWidget( label, iii, 1 );
      mGenderBox = new QComboBox ( tab2 );
      mGenderBox->insertItem ( i18n( "ALIEN (undefined)" ));
      mGenderBox->insertItem ( i18n( "female" ));
      mGenderBox->insertItem ( i18n( "male" ));
      connect( mGenderBox, SIGNAL(  activated ( const QString & )  ),
                   SLOT( textChanged( const QString& ) ) );
      label->setBuddy( mGenderBox );
      layout->addWidget( mGenderBox, iii, 2 );
      ++iii;
   // Build the layout and add to the tab widget
  layout->activate(); // required

  mTabWidget->addTab( tab2, i18n( "&Details" ) );
}

void AddresseeEditorWidget::setupTab2_1()
{
  // This is the Details tab
  QWidget *tab2_2 = new QWidget( mTabWidget );

  QGridLayout *layout = new QGridLayout( tab2_2, 1, 2 );
  layout->setMargin( KDialogBase::marginHintSmall() );
  layout->setSpacing( KDialogBase::spacingHintSmall() );
  
  QLabel *label;
  KSeparator* bar;
  
/*US  
  ///////////////////////
  // Office info
  
  // Department
  label = new QLabel( tab2 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "folder", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "folder", KIcon::Desktop, 0 ) );
  layout->addMultiCellWidget( label, 0, 1, 0, 0 );
  
  label = new QLabel( i18n( "Department:" ), tab2 );
  layout->addWidget( label, 0, 1 );
  mDepartmentEdit = new KLineEdit( tab2 );
  connect( mDepartmentEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mDepartmentEdit );
  layout->addWidget( mDepartmentEdit, 0, 2 );
  
  label = new QLabel( i18n( "Office:" ), tab2 );
  layout->addWidget( label, 1, 1 );
  mOfficeEdit = new KLineEdit( tab2 );
  connect( mOfficeEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mOfficeEdit );
  layout->addWidget( mOfficeEdit, 1, 2 );
  
  label = new QLabel( i18n( "Profession:" ), tab2 );
  layout->addWidget( label, 2, 1 );
  mProfessionEdit = new KLineEdit( tab2 );
  connect( mProfessionEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mProfessionEdit );
  layout->addWidget( mProfessionEdit, 2, 2 );
  
  label = new QLabel( i18n( "Manager\'s name:" ), tab2 );
  layout->addWidget( label, 0, 3 );
  mManagerEdit = new KLineEdit( tab2 );
  connect( mManagerEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mManagerEdit );
  layout->addMultiCellWidget( mManagerEdit, 0, 0, 4, 5 );
  
  label = new QLabel( i18n( "Assistant's name:" ), tab2 );
  layout->addWidget( label, 1, 3 );
  mAssistantEdit = new KLineEdit( tab2 );
  connect( mAssistantEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mAssistantEdit );
  layout->addMultiCellWidget( mAssistantEdit, 1, 1, 4, 5 );
  
  bar = new KSeparator( KSeparator::HLine, tab2 );
  layout->addMultiCellWidget( bar, 3, 3, 0, 5 );

  /////////////////////////////////////////////////
  // Personal info
  
  label = new QLabel( tab2 );
//US loadIcon call is ambiguous. Add one more parameter
//US  label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop ) );
  label->setPixmap( KGlobal::iconLoader()->loadIcon( "personal", KIcon::Desktop, 0 ) );
  layout->addMultiCellWidget( label, 4, 5, 0, 0 );
  
  label = new QLabel( i18n( "Nick name:" ), tab2 );
  layout->addWidget( label, 4, 1 );
  mNicknameEdit = new KLineEdit( tab2 );
  connect( mNicknameEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mNicknameEdit );
  layout->addWidget( mNicknameEdit, 4, 2 );

  label = new QLabel( i18n( "Spouse's name:" ), tab2 );
  layout->addWidget( label, 5, 1 );
  mSpouseEdit = new KLineEdit( tab2 );
  connect( mSpouseEdit, SIGNAL( textChanged( const QString& ) ),
           SLOT( textChanged( const QString& ) ) );
  label->setBuddy( mSpouseEdit );
  layout->addWidget( mSpouseEdit, 5, 2 );

  label = new QLabel( i18n( "Birthday:" ), tab2 );
  layout->addWidget( label, 4, 3 );
  mBirthdayPicker = new KDateEdit( tab2 );
  mBirthdayPicker->setHandleInvalid( true );
  connect( mBirthdayPicker, SIGNAL( dateChanged( QDate ) ),
           SLOT( dateChanged( QDate ) ) );
  connect( mBirthdayPicker, SIGNAL( invalidDateEntered() ),
           SLOT( invalidDate() ) );
  connect( mBirthdayPicker, SIGNAL( textChanged( const QString& ) ),
           SLOT( emitModified() ) );
  label->setBuddy( mBirthdayPicker );
  layout->addWidget( mBirthdayPicker, 4, 4 );
  
  label = new QLabel( i18n( "Anniversary:" ), tab2 );
  layout->addWidget( label, 5, 3 );
  mAnniversaryPicker = new KDateEdit( tab2 );
  mAnniversaryPicker->setHandleInvalid( true );
  connect( mAnniversaryPicker, SIGNAL( dateChanged( QDate ) ),
           SLOT( dateChanged( QDate ) ) );
  connect( mAnniversaryPicker, SIGNAL( invalidDateEntered() ),
           SLOT( invalidDate() ) );
  connect( mAnniversaryPicker, SIGNAL( textChanged( const QString& ) ),
           SLOT( emitModified() ) );
  label->setBuddy( mAnniversaryPicker );
  layout->addWidget( mAnniversaryPicker, 5, 4 );
  
  bar = new KSeparator( KSeparator::HLine, tab2 );
  layout->addMultiCellWidget( bar, 6, 6, 0, 5 );
*/  
   //////////////////////////////////////
  // Notes
  label = new QLabel( i18n( "Note:" ), tab2_2 );
  label->setAlignment( Qt::AlignTop | Qt::AlignLeft );
//US  layout->addWidget( label, 7, 0 );
  layout->addWidget( label, 0, 0 );
#ifndef KAB_EMBEDDED    
  mNoteEdit = new QTextEdit( tab2_2 );
  mNoteEdit->setWordWrap( QTextEdit::WidgetWidth );
  mNoteEdit->setMinimumSize( mNoteEdit->sizeHint() );
#else //KAB_EMBEDDED    
  mNoteEdit = new QMultiLineEdit( tab2_2 );
  mNoteEdit->setWordWrap( QMultiLineEdit::WidgetWidth );
  mNoteEdit->setMinimumSize( mNoteEdit->sizeHint() );
#endif //KAB_EMBEDDED    
  
  connect( mNoteEdit, SIGNAL( textChanged() ), SLOT( emitModified() ) );
  label->setBuddy( mNoteEdit );
//US  layout->addMultiCellWidget( mNoteEdit, 7, 7, 1, 5 );
  layout->addWidget( mNoteEdit, 1, 0);

   // Build the layout and add to the tab widget
  layout->activate(); // required

  mTabWidget->addTab( tab2_2, i18n( "&Notes" ) );
}



void AddresseeEditorWidget::setupTab3()
{
  // This is the Misc tab
  QWidget *tab3 = new QWidget( mTabWidget );

  QGridLayout *layout = new QGridLayout( tab3, 1, 1 );
  layout->setMargin( KDialogBase::marginHintSmall() );
  layout->setSpacing( KDialogBase::spacingHintSmall() );
//US  layout->setColStretch( 2, 1 );
  
  //////////////////////////////////////
  // Geo
  mGeoWidget = new GeoWidget( tab3 );
  // mGeoWidget->setMinimumSize( mGeoWidget->sizeHint() );
  connect( mGeoWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
  layout->addWidget( mGeoWidget, 0, 0 );
/*US
  //////////////////////////////////////
  // Sound
#ifndef KAB_EMBEDDED    
  mSoundWidget = new SoundWidget( tab3 );
  mSoundWidget->setMinimumSize( mSoundWidget->sizeHint() );
  connect( mSoundWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
  layout->addWidget( mSoundWidget, 0, 1, Qt::AlignTop );
#else //KAB_EMBEDDED    
qDebug("AddresseeEditorWidget::setupTab2 sound part is not supported = has to be changed");
#endif //KAB_EMBEDDED    

  //////////////////////////////////////
  // Images
  mImageWidget = new ImageWidget( tab3 );
  mImageWidget->setMinimumSize( mImageWidget->sizeHint() );
  connect( mImageWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
  layout->addWidget( mImageWidget, 1, 0, Qt::AlignTop );
*/
//US  
/*
  KSeparator* bar = new KSeparator( KSeparator::HLine, tab3 );
  layout->addMultiCellWidget( bar, 1, 1, 0, 0 );
*/ 
  //////////////////////////////////////
  // Keys
  mKeyWidget = new KeyWidget( tab3 );
  //mKeyWidget->setMinimumSize( mKeyWidget->sizeHint() );
  connect( mKeyWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
//US  layout->addWidget( mKeyWidget, 1, 1, Qt::AlignTop );
  layout->addWidget( mKeyWidget, 1, 0 );

  mTabWidget->addTab( tab3, i18n( "&Misc" ) );
}

void AddresseeEditorWidget::setupTab3_1()
{
  // This is the Misc tab
  QWidget *tab3 = new QWidget( mTabWidget );

//US  QGridLayout *layout = new QGridLayout( tab3, 2, 3 );
  QGridLayout *layout = new QGridLayout( tab3, 1, 1 );
  layout->setMargin( KDialogBase::marginHint() );
  layout->setSpacing( KDialogBase::spacingHint() );
//US  layout->setColStretch( 2, 1 );

/*US  
  //////////////////////////////////////
  // Geo
  mGeoWidget = new GeoWidget( tab3 );
  mGeoWidget->setMinimumSize( mGeoWidget->sizeHint() );
  connect( mGeoWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
  layout->addWidget( mGeoWidget, 0, 0, Qt::AlignTop );
*/
  //////////////////////////////////////
  // Sound
#ifndef KAB_EMBEDDED    
  mSoundWidget = new SoundWidget( tab3 );
  mSoundWidget->setMinimumSize( mSoundWidget->sizeHint() );
  connect( mSoundWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
  layout->addWidget( mSoundWidget, 0, 1, Qt::AlignTop );
#else //KAB_EMBEDDED    
//US qDebug("AddresseeEditorWidget::setupTab2 sound part is not supported = has to be changed");
#endif //KAB_EMBEDDED    

  //////////////////////////////////////
  // Images
  mImageWidget = new ImageWidget( tab3 );
  mImageWidget->setMinimumSize( mImageWidget->sizeHint() );
  connect( mImageWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
  layout->addWidget( mImageWidget, 0, 0, Qt::AlignTop );
/*US
  //////////////////////////////////////
  // Keys
  mKeyWidget = new KeyWidget( tab3 );
  mKeyWidget->setMinimumSize( mKeyWidget->sizeHint() );
  connect( mKeyWidget, SIGNAL( changed() ), SLOT( emitModified() ) );
  layout->addWidget( mKeyWidget, 1, 1, Qt::AlignTop );
*/
  mTabWidget->addTab( tab3, i18n( "&Images" ) );
}

    
void AddresseeEditorWidget::load()
{

  // Block signals in case anything tries to emit modified
  // CS: This doesn't seem to work.
  bool block = signalsBlocked();
  blockSignals( true ); 
  mBlockSignals = true; // used for internal signal blocking

  mNameEdit->setText( mAddressee.assembledName() );
  
  if ( mAddressee.formattedName().isEmpty() ) {
//US    KConfig config( "kaddressbookrc" );
    KConfig config( locateLocal("config", "kabcrc") );
    config.setGroup( "General" );
    mFormattedNameType = config.readNumEntry( "FormattedNameType", 1 );
    mAddressee.setFormattedName( NameEditDialog::formattedName( mAddressee, mFormattedNameType ) );
  } else {;
    if ( mAddressee.formattedName() == NameEditDialog::formattedName( mAddressee, NameEditDialog::SimpleName ) )
      mFormattedNameType = NameEditDialog::SimpleName;
    else if ( mAddressee.formattedName() == NameEditDialog::formattedName( mAddressee, NameEditDialog::FullName ) )
      mFormattedNameType = NameEditDialog::FullName;
    else if ( mAddressee.formattedName() == NameEditDialog::formattedName( mAddressee, NameEditDialog::ReverseName ) )
      mFormattedNameType = NameEditDialog::ReverseName;
    else
      mFormattedNameType = NameEditDialog::CustomName;
  }

  mFormattedNameLabel->setText( mAddressee.formattedName() );

  mAConfig->setUid( mAddressee.uid() );
  if ( mAConfig->automaticNameParsing() ) {
      mNameLabel->hide();
      mNameEdit->show();
  } else {
      mNameEdit->hide();
      mNameLabel->setText( mNameEdit->text() );
      mNameLabel->show();
  }

  mRoleEdit->setText( mAddressee.role() );
  mOrgEdit->setText( mAddressee.organization() );
  
//US  mURLEdit->setURL( mAddressee.url().url() );
  mURLEdit->setText( mAddressee.url().prettyURL() );
//US??  mURLEdit->home( false );
  
  // mNoteEdit->setText( mAddressee.note() );
  mNoteEdit->setText( mAddressee.note() );
  mEmailWidget->setEmails( mAddressee.emails() );
  mPhoneEditWidget->setPhoneNumbers( mAddressee.phoneNumbers() );
  mAddressEditWidget->setAddresses( mAddressee, mAddressee.addresses() );
  mBirthdayPicker->setDate( mAddressee.birthday().date() );

//US  mAnniversaryPicker->setDate( QDate::fromString( mAddressee.custom(
//US                               "KADDRESSBOOK", "X-Anniversary" ), Qt::ISODate) );
  QDate dt = KGlobal::locale()->readDate( mAddressee.custom("KADDRESSBOOK", "X-Anniversary" ), 
                                                    "%Y-%m-%d");  // = Qt::ISODate
  mAnniversaryPicker->setDate( dt );

                                                              
  mNicknameEdit->setText( mAddressee.nickName() );
  mCategoryEdit->setText( mAddressee.categories().join( "," ) );

  mGeoWidget->setGeo( mAddressee.geo() );
  mImageWidget->setPhoto( mAddressee.photo() );
  mImageWidget->setLogo( mAddressee.logo() );
  mKeyWidget->setKeys( mAddressee.keys() );
  mSecrecyWidget->setSecrecy( mAddressee.secrecy() );
#ifndef KAB_EMBEDDED    
  mSoundWidget->setSound( mAddressee.sound() );
#else //KAB_EMBEDDED    
//US qDebug("AddresseeEditorWidget::load has to be changed 2");
#endif //KAB_EMBEDDED    

  // Load customs
  mIMAddressEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-IMAddress" ) );
  mSpouseEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-SpousesName" ) );
  mChildEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-Children" ) );
  mManagerEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-ManagersName" ) );
  mAssistantEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-AssistantsName" ) );
  mDepartmentEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-Department" ) );
  mOfficeEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-Office" ) );
  mProfessionEdit->setText( mAddressee.custom( "KADDRESSBOOK", "X-Profession" ) );
  QString gen = mAddressee.custom( "KADDRESSBOOK", "X-Gender" );
  if ( gen == "female" )
      mGenderBox->setCurrentItem ( 1 ); 
  else if ( gen == "male" )
      mGenderBox->setCurrentItem ( 2 ); 
  else 
      mGenderBox->setCurrentItem ( 0 ); 
  blockSignals( block );
  mBlockSignals = false;

  mDirty = false;
}

void AddresseeEditorWidget::save()
{
    if ( !dirty() ) {
            return;
    }
 
    mAddressee.setRevision( QDateTime::currentDateTime() ); 
 
  mAddressee.setRole( mRoleEdit->text() );
  mAddressee.setOrganization( mOrgEdit->text() );
  mAddressee.setUrl( KURL( mURLEdit->text() ) );
  mAddressee.setNote( mNoteEdit->text() );
  if ( mBirthdayPicker->inputIsValid() ) {
      QDate da = mBirthdayPicker->date();
      mAddressee.setBirthday( QDateTime( da ) );
      //qDebug("bday %s ",da.toString().latin1());
  }
  else {
    mAddressee.setBirthday( QDateTime() );
    mBirthdayPicker->clear();
  }
  mAddressee.setNickName( mNicknameEdit->text() );
  mAddressee.setCategories( QStringList::split( ",", mCategoryEdit->text() ) );

  mAddressee.setGeo( mGeoWidget->geo() );
  mAddressee.setPhoto( mImageWidget->photo() );
  mAddressee.setLogo( mImageWidget->logo() );
  mAddressee.setKeys( mKeyWidget->keys() );
#ifndef KAB_EMBEDDED    
  mAddressee.setSound( mSoundWidget->sound() );
#else //KAB_EMBEDDED    
//US qDebug("AddresseeEditorWidget::save sound not supported");
#endif //KAB_EMBEDDED    
  mAddressee.setSecrecy( mSecrecyWidget->secrecy() );

  // save custom fields
  mAddressee.insertCustom( "KADDRESSBOOK", "X-IMAddress", mIMAddressEdit->text() );
  mAddressee.insertCustom( "KADDRESSBOOK", "X-SpousesName", mSpouseEdit->text() );
  mAddressee.insertCustom( "KADDRESSBOOK", "X-Children", mChildEdit->text() );
  mAddressee.insertCustom( "KADDRESSBOOK", "X-ManagersName", mManagerEdit->text() );
  mAddressee.insertCustom( "KADDRESSBOOK", "X-AssistantsName", mAssistantEdit->text() );
  mAddressee.insertCustom( "KADDRESSBOOK", "X-Department", mDepartmentEdit->text() );
  mAddressee.insertCustom( "KADDRESSBOOK", "X-Office", mOfficeEdit->text() );
  mAddressee.insertCustom( "KADDRESSBOOK", "X-Profession", mProfessionEdit->text() );
  if ( mAnniversaryPicker->inputIsValid() ) {
    QString dt = KGlobal::locale()->formatDate(mAnniversaryPicker->date(), true, KLocale::ISODate);
    mAddressee.insertCustom( "KADDRESSBOOK", "X-Anniversary", dt);
  }
  else {
    mAddressee.removeCustom( "KADDRESSBOOK", "X-Anniversary" );
    mAnniversaryPicker->clear();
  }
  int gen = mGenderBox->currentItem ();
  if ( gen == 1 )
    mAddressee.insertCustom( "KADDRESSBOOK", "X-Gender", "female");
  else if ( gen == 2 )
    mAddressee.insertCustom( "KADDRESSBOOK", "X-Gender", "male");
  else 
    mAddressee.removeCustom( "KADDRESSBOOK", "X-Gender" );
                      
  // Save the email addresses
  QStringList emails = mAddressee.emails();
  QStringList::Iterator iter;
  for ( iter = emails.begin(); iter != emails.end(); ++iter )
    mAddressee.removeEmail( *iter );
  
  emails = mEmailWidget->emails();
  bool first = true;
  for ( iter = emails.begin(); iter != emails.end(); ++iter ) {
    mAddressee.insertEmail( *iter, first );
    first = false;
  }
  
  // Save the phone numbers
  KABC::PhoneNumber::List phoneNumbers;
  KABC::PhoneNumber::List::Iterator phoneIter;
  phoneNumbers = mAddressee.phoneNumbers();
  for ( phoneIter = phoneNumbers.begin(); phoneIter != phoneNumbers.end();
        ++phoneIter )
    mAddressee.removePhoneNumber( *phoneIter );
    
  phoneNumbers = mPhoneEditWidget->phoneNumbers();
  for ( phoneIter = phoneNumbers.begin(); phoneIter != phoneNumbers.end();
        ++phoneIter )
    mAddressee.insertPhoneNumber( *phoneIter );
  mAddressee.makePhoneNumbersOLcompatible();
  // Save the addresses
  KABC::Address::List addresses;
  KABC::Address::List::Iterator addressIter;
  addresses = mAddressee.addresses();
  for ( addressIter = addresses.begin(); addressIter != addresses.end();
        ++addressIter )
    mAddressee.removeAddress( *addressIter );
    
  addresses = mAddressEditWidget->addresses();
  for ( addressIter = addresses.begin(); addressIter != addresses.end();
        ++addressIter )
    mAddressee.insertAddress( *addressIter );
  mDirty = false;
}

bool AddresseeEditorWidget::dirty()
{

    if ( ! mDirty ) {
        if ( mBirthdayPicker->inputIsValid() ) {
            QDate da = mBirthdayPicker->date();
            if ( !(da ==  mAddressee.birthday().date()))
                mDirty = true;
        }
        else {
            mBirthdayPicker->clear();
        }
        if ( mAnniversaryPicker->inputIsValid() ) {
            QDate da = mAnniversaryPicker->date();
            if ( da != KGlobal::locale()->readDate( mAddressee.custom("KADDRESSBOOK", "X-Anniversary" ), 
                                                    "%Y-%m-%d"))
                mDirty = true;
        }
        else {
            mAnniversaryPicker->clear();
        }
    }
  return mDirty;
}

void AddresseeEditorWidget::nameTextChanged( const QString &text )
{
    if ( mBlockSignals )
        return;
  // use the addressee class to parse the name for us
    mAConfig->setUid( mAddressee.uid() );
  if ( mAConfig->automaticNameParsing() ) {
    if ( !mAddressee.formattedName().isEmpty() ) {
         QString fn = mAddressee.formattedName();
        mAddressee.setNameFromString( text );
        mAddressee.setFormattedName( fn );
    } else {
      // use extra addressee to avoid a formatted name assignment
      Addressee addr;
      addr.setNameFromString( text );
      mAddressee.setPrefix( addr.prefix() );
      mAddressee.setGivenName( addr.givenName() );
      mAddressee.setAdditionalName( addr.additionalName() );
      mAddressee.setFamilyName( addr.familyName() );
      mAddressee.setSuffix( addr.suffix() );
    }
  }
  nameBoxChanged();

 
  emitModified();
}

void AddresseeEditorWidget::nameBoxChanged()
{
  KABC::Addressee addr;
  mAConfig->setUid( mAddressee.uid() );
  if ( mAConfig->automaticNameParsing() ) {
    addr.setNameFromString( mNameEdit->text() );
    mNameLabel->hide();
    mNameEdit->show();
  } else {
    addr = mAddressee;
    mNameEdit->hide();
    mNameLabel->setText( mNameEdit->text() );
    mNameLabel->show();
  }

  if ( mFormattedNameType != NameEditDialog::CustomName ) {
    mFormattedNameLabel->setText( NameEditDialog::formattedName( mAddressee, mFormattedNameType ) );
    mAddressee.setFormattedName( NameEditDialog::formattedName( mAddressee, mFormattedNameType ) );
  }
}

void AddresseeEditorWidget::nameButtonClicked()
{
  // show the name dialog.
  NameEditDialog dialog( mAddressee, mFormattedNameType, this );
  
  if ( KApplication::execDialog( &dialog) ) {
    if ( dialog.changed() ) {
      mAddressee.setFamilyName( dialog.familyName() );
      mAddressee.setGivenName( dialog.givenName() );
      mAddressee.setPrefix( dialog.prefix() );
      mAddressee.setSuffix( dialog.suffix() );
      mAddressee.setAdditionalName( dialog.additionalName() );
      mFormattedNameType = dialog.formattedNameType();
      if ( mFormattedNameType == NameEditDialog::CustomName ) {
        mFormattedNameLabel->setText( dialog.customFormattedName() );
        mAddressee.setFormattedName( dialog.customFormattedName() );
      }
      // Update the name edit.
      bool block = mNameEdit->signalsBlocked();
      mNameEdit->blockSignals( true );
      mNameEdit->setText( mAddressee.assembledName() );
      mNameEdit->blockSignals( block );

      // Update the combo box.
      nameBoxChanged();
    
      emitModified();
    }
  }
}

void AddresseeEditorWidget::categoryButtonClicked()
{
  // Show the category dialog
  if ( mCategoryDialog == 0 ) {
    mCategoryDialog = new KPIM::CategorySelectDialog( KABPrefs::instance(), this );
    connect( mCategoryDialog, SIGNAL( categoriesSelected( const QStringList& ) ),
             SLOT(categoriesSelected( const QStringList& ) ) );
    connect( mCategoryDialog, SIGNAL( editCategories() ), SLOT( editCategories() ) );
  }

  mCategoryDialog->setCategories();
  mCategoryDialog->setSelected( QStringList::split( ",", mCategoryEdit->text() ) );
  mCategoryDialog->show();
  mCategoryDialog->raise();
}

void AddresseeEditorWidget::categoriesSelected( const QStringList &list )
{
  mCategoryEdit->setText( list.join( "," ) );
}

void AddresseeEditorWidget::editCategories()
{
  if ( mCategoryEditDialog == 0 ) {
    mCategoryEditDialog = new KPIM::CategoryEditDialog( KABPrefs::instance(), this );
    connect( mCategoryEditDialog, SIGNAL( categoryConfigChanged() ),
             SLOT( categoryButtonClicked() ) );
  }
  
  mCategoryEditDialog->show();
  mCategoryEditDialog->raise();
}

void AddresseeEditorWidget::emitModified()
{
  mDirty = true;

  KABC::Addressee::List list;

  if ( mIsExtension && !mBlockSignals ) {
    save();
    list.append( mAddressee );
  }

  emit modified( list );
}

void AddresseeEditorWidget::dateChanged( QDate )
{
  emitModified();
}

//US invalid dates are handdled by the KDateEdit widget itself
void AddresseeEditorWidget::invalidDate()
{
  KMessageBox::sorry( this, i18n( "You must specify a valid date" ) );
}


void AddresseeEditorWidget::pageChanged( QWidget *wdg )
{
#ifndef KAB_EMBEDDED    
  if ( wdg )
    KAcceleratorManager::manage( wdg );
#else //KAB_EMBEDDED    
//US 
#endif //KAB_EMBEDDED    
    
}

QString AddresseeEditorWidget::title() const
{
  return i18n( "Contact Editor" );
}

QString AddresseeEditorWidget::identifier() const
{
  return i18n( "contact_editor" );
}

#ifndef KAB_EMBEDDED    
#include "addresseeeditorwidget.moc"
#endif //KAB_EMBEDDED    
