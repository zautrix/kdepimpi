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

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

#ifndef KAB_EMBEDDED
#include <kaccelmanager.h>
#endif //KAB_EMBEDDED


#include <kdebug.h>
#include <klineeditdlg.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>

#include <kabc/addressbook.h>
#include <kabc/addresseedialog.h>
#include <kabc/distributionlist.h>
#include <kabc/vcardconverter.h>

#ifndef KAB_EMBEDDED
#include <libkdepim/kvcarddrag.h>
#endif //KAB_EMBEDDED

#include "kabcore.h"

#include "distributionlistwidget.h"

#ifndef KAB_EMBEDDED

class DistributionListFactory : public ExtensionFactory
{
  public:
    ExtensionWidget *extension( KABCore *core, QWidget *parent, const char *name )
    {
      return new DistributionListWidget( core, parent, name );
    }

    QString identifier() const
    {
      return "distribution_list_editor";
    }
};

extern "C" {
  void *init_libkaddrbk_distributionlist()
  {
    return ( new DistributionListFactory );
  }
}
#endif //KAB_EMBEDDED

class ContactItem : public QListViewItem
{
  public:
    ContactItem( DistributionListView *parent, const KABC::Addressee &addressee,
               const QString &email = QString::null ) :
      QListViewItem( parent ),
      mAddressee( addressee ),
      mEmail( email )
    {
      setText( 0, addressee.realName() );
      if( email.isEmpty() ) {
        setText( 1, addressee.preferredEmail() );
        setText( 2, i18n( "Yes" ) );
      } else {
        setText( 1, email );
        setText( 2, i18n( "No" ) );
      }
    }

    KABC::Addressee addressee() const
    {
      return mAddressee;
    }

    QString email() const
    {
      return mEmail;
    }

  protected:
    bool acceptDrop( const QMimeSource* )
    {
      return true;
    }

  private:
    KABC::Addressee mAddressee;
    QString mEmail;
};

DistributionListWidget::DistributionListWidget( KABCore *core, QWidget *parent,
                                                const char *name )
  : ExtensionWidget( core, parent, name ), mManager( 0 )
{
  QGridLayout *topLayout = new QGridLayout( this, 3, 4, KDialog::marginHint(),
                                            KDialog::spacingHint() );

  if (KGlobal::getOrientation() == KGlobal::Portrait)
  {
    mCreateListButton = new QPushButton( i18n( "New List" ), this );
    mEditListButton = new QPushButton( i18n( "Ren List" ), this );
    mRemoveListButton = new QPushButton( i18n( "Del List" ), this );
    mAddContactButton = new QPushButton( i18n( "Add Cont." ), this );
    mChangeEmailButton = new QPushButton( i18n( "Chge Email" ), this );
    mRemoveContactButton = new QPushButton( i18n( "Del Cont." ), this );
  }
  else
  {
    mCreateListButton = new QPushButton( i18n( "New List..." ), this );
    mEditListButton = new QPushButton( i18n( "Rename List..." ), this );
    mRemoveListButton = new QPushButton( i18n( "Remove List" ), this );
    mAddContactButton = new QPushButton( i18n( "Add Contact" ), this );
    mChangeEmailButton = new QPushButton( i18n( "Change Email..." ), this );
    mRemoveContactButton = new QPushButton( i18n( "Remove Contact" ), this );
  }                                        
  mNameCombo = new QComboBox( this );
  topLayout->addWidget( mNameCombo, 0, 0 );
  connect( mNameCombo, SIGNAL( activated( int ) ), SLOT( updateContactView() ) );

  topLayout->addWidget( mCreateListButton, 0, 1 );
  connect( mCreateListButton, SIGNAL( clicked() ), SLOT( createList() ) );

  topLayout->addWidget( mEditListButton, 0, 2 );
  connect( mEditListButton, SIGNAL( clicked() ), SLOT( editList() ) );

  topLayout->addWidget( mRemoveListButton, 0, 3 );
  connect( mRemoveListButton, SIGNAL( clicked() ), SLOT( removeList() ) );

  mContactView = new DistributionListView( this );
  mContactView->addColumn( i18n( "Name" ) );
  mContactView->addColumn( i18n( "Email" ) );
  mContactView->addColumn( i18n( "Use Preferred" ) );
  mContactView->setEnabled( false );
  mContactView->setAllColumnsShowFocus( true );
  mContactView->setMinimumHeight( 30 );
  
  topLayout->addMultiCellWidget( mContactView, 1, 1, 0, 3 );
  connect( mContactView, SIGNAL( selectionChanged() ),
           SLOT( selectionContactViewChanged() ) );
  connect( mContactView, SIGNAL( dropped( QDropEvent*, QListViewItem* ) ),
           SLOT( dropped( QDropEvent*, QListViewItem* ) ) );

  mAddContactButton->setEnabled( false );
  topLayout->addWidget( mAddContactButton, 2, 0 );
  connect( mAddContactButton, SIGNAL( clicked() ), SLOT( addContact() ) );

  topLayout->addWidget( mChangeEmailButton, 2, 2 );
  connect( mChangeEmailButton, SIGNAL( clicked() ), SLOT( changeEmail() ) );

  topLayout->addWidget( mRemoveContactButton, 2, 3 );
  connect( mRemoveContactButton, SIGNAL( clicked() ), SLOT( removeContact() ) );

  mManager = new KABC::DistributionListManager( core->addressBook() );
  mManager->load();

  updateNameCombo();

#ifdef KAB_EMBEDDED
//  if (KGlobal::getOrientation() == KGlobal::Portrait)
//    parent->setMaximumSize( KGlobal::getDesktopWidth() , 150);
#endif //KAB_EMBEDDED
  
#ifndef KAB_EMBEDDED
  KAcceleratorManager::manage( this );
#endif //KAB_EMBEDDED
}

DistributionListWidget::~DistributionListWidget()
{
  delete mManager;
}

void DistributionListWidget::save()
{
qDebug("DistributionListWidget::save");
  mManager->save();
}

void DistributionListWidget::selectionContactViewChanged()
{
  ContactItem *contactItem =
                  static_cast<ContactItem *>( mContactView->selectedItem() );
  bool state = contactItem;

  mChangeEmailButton->setEnabled( state );
  mRemoveContactButton->setEnabled( state );
}

void DistributionListWidget::createList()
{
  KLineEditDlg dlg( i18n( "Please enter name:" ), QString::null, this );
#ifdef KAB_EMBEDDED  
  dlg.setFixedSize(200, 50);
#endif //KAB_EMBEDDED  
  dlg.setCaption( i18n( "New Distribution List" ) );
  if ( !dlg.exec() )
    return;

  new KABC::DistributionList( mManager, dlg.text() );

  mNameCombo->clear();
  mNameCombo->insertStringList( mManager->listNames() );
  mNameCombo->setCurrentItem( mNameCombo->count() - 1 );

  updateContactView();

  changed();
}

void DistributionListWidget::editList()
{
  QString oldName = mNameCombo->currentText();
  
  KLineEditDlg dlg( i18n( "Please change name:" ), oldName, this );
#ifdef KAB_EMBEDDED  
  dlg.setFixedSize(200, 50);
#endif //KAB_EMBEDDED  
  dlg.setCaption( i18n("Distribution List") );
  if ( !dlg.exec() )
    return;

  KABC::DistributionList *list = mManager->list( oldName );
  list->setName( dlg.text() );

  mNameCombo->clear();
  mNameCombo->insertStringList( mManager->listNames() );
  mNameCombo->setCurrentItem( mNameCombo->count() - 1 );

  updateContactView();

  changed();
}

void DistributionListWidget::removeList()
{
  int result = KMessageBox::warningContinueCancel( this,
      i18n( "<qt>Delete distribution list <b>%1</b>?</qt>" ) .arg( mNameCombo->currentText() ),
      QString::null, i18n( "Delete" ) );

  if ( result != KMessageBox::Continue )
    return;

  delete mManager->list( mNameCombo->currentText() );
  mNameCombo->removeItem( mNameCombo->currentItem() );

  updateContactView();

  changed();
}

void DistributionListWidget::addContact()
{
  KABC::DistributionList *list = mManager->list( mNameCombo->currentText() );
  if ( !list )
    return;

  KABC::Addressee::List addrList = selectedContacts();
  KABC::Addressee::List::Iterator it;
  for ( it = addrList.begin(); it != addrList.end(); ++it )
    list->insertEntry( *it );

  updateContactView();

  changed();
}

void DistributionListWidget::removeContact()
{
  KABC::DistributionList *list = mManager->list( mNameCombo->currentText() );
  if ( !list )
    return;

  ContactItem *contactItem =
                    static_cast<ContactItem *>( mContactView->selectedItem() );
  if ( !contactItem )
    return;

  list->removeEntry( contactItem->addressee(), contactItem->email() );
  delete contactItem;

  changed();
}

void DistributionListWidget::changeEmail()
{
  KABC::DistributionList *list = mManager->list( mNameCombo->currentText() );
  if ( !list )
    return;

  ContactItem *contactItem =
                    static_cast<ContactItem *>( mContactView->selectedItem() );
  if ( !contactItem )
    return;

  QString email = EmailSelector::getEmail( contactItem->addressee().emails(),
                                           contactItem->email(), this );
  list->removeEntry( contactItem->addressee(), contactItem->email() );
  list->insertEntry( contactItem->addressee(), email );

  updateContactView();

  changed();
}

void DistributionListWidget::updateContactView()
{
  mContactView->clear();

  KABC::DistributionList *list = mManager->list( mNameCombo->currentText() );
  if ( !list ) {
    mEditListButton->setEnabled( false );
    mRemoveListButton->setEnabled( false );
    mChangeEmailButton->setEnabled( false );
    mRemoveContactButton->setEnabled( false );
    mContactView->setEnabled( false );
    return;
  } else {
    mEditListButton->setEnabled( true );
    mRemoveListButton->setEnabled( true );
    mContactView->setEnabled( true );
  }

  KABC::DistributionList::Entry::List entries = list->entries();
  KABC::DistributionList::Entry::List::ConstIterator it;
  for( it = entries.begin(); it != entries.end(); ++it )
    new ContactItem( mContactView, (*it).addressee, (*it).email );

  ContactItem *contactItem =
                    static_cast<ContactItem *>( mContactView->selectedItem() );
  bool state = contactItem;

  mChangeEmailButton->setEnabled( state );
  mRemoveContactButton->setEnabled( state );
}

void DistributionListWidget::updateNameCombo()
{
  mNameCombo->insertStringList( mManager->listNames() );

  updateContactView();
}

void DistributionListWidget::dropEvent( QDropEvent *e )
{
  KABC::DistributionList *distributionList = mManager->list( mNameCombo->currentText() );
  if ( !distributionList )
    return;

  QString vcards;
#ifndef KAB_EMBEDDED
  if ( KVCardDrag::decode( e, vcards ) ) {
#endif //KAB_EMBEDDED
    QStringList list = QStringList::split( "\r\n\r\n", vcards );
    QStringList::Iterator it;
    KABC::VCardConverter converter;
    for ( it = list.begin(); it != list.end(); ++it ) {
      KABC::Addressee addr;
      if ( converter.vCardToAddressee( (*it).stripWhiteSpace(), addr ) )
        distributionList->insertEntry( addr );
    }

    changed();
    updateContactView();
#ifndef KAB_EMBEDDED
  }
#endif //KAB_EMBEDDED
}

void DistributionListWidget::contactsSelectionChanged()
{
  mAddContactButton->setEnabled( contactsSelected() && mNameCombo->count() > 0 );
}

QString DistributionListWidget::title() const
{
  return i18n( "Distribution List Editor" );
}

QString DistributionListWidget::identifier() const
{
  return "distribution_list_editor";
}

void DistributionListWidget::dropped( QDropEvent *e, QListViewItem* )
{
  dropEvent( e );
}

void DistributionListWidget::changed()
{
  save();
}


DistributionListView::DistributionListView( QWidget *parent, const char* name )
  : KListView( parent, name )
{
  setDragEnabled( true );
  setAcceptDrops( true );
  setAllColumnsShowFocus( true );
}

void DistributionListView::dragEnterEvent( QDragEnterEvent* e )
{
#ifndef KAB_EMBEDDED
  bool canDecode = QTextDrag::canDecode( e );
  e->accept( canDecode );
#endif //KAB_EMBEDDED
}

void DistributionListView::viewportDragMoveEvent( QDragMoveEvent *e )
{
#ifndef KAB_EMBEDDED
  bool canDecode = QTextDrag::canDecode( e );
  e->accept( canDecode );
#endif //KAB_EMBEDDED
}

void DistributionListView::viewportDropEvent( QDropEvent *e )
{
  emit dropped( e, 0 );
}

void DistributionListView::dropEvent( QDropEvent *e )
{
  emit dropped( e, 0 );
}


EmailSelector::EmailSelector( const QStringList &emails,
                              const QString &current, QWidget *parent )
  : KDialogBase( KDialogBase::Plain, i18n("Select Email Address"), Ok, Ok,
               parent )
{
  QFrame *topFrame = plainPage();
  QBoxLayout *topLayout = new QVBoxLayout( topFrame );

  mButtonGroup = new QButtonGroup( 1, Horizontal, i18n("Email Addresses"),
                                   topFrame );
  topLayout->addWidget( mButtonGroup );

  QStringList::ConstIterator it;
  for( it = emails.begin(); it != emails.end(); ++it ) {
    QRadioButton *button = new QRadioButton( *it, mButtonGroup );
    if ( (*it) == current ) {
      button->setDown( true );
    }
  }
}

QString EmailSelector::selected()
{
  QButton *button = mButtonGroup->selected();
  if ( button )
    return button->text();

  return QString::null;
}

QString EmailSelector::getEmail( const QStringList &emails,
                                 const QString &current, QWidget *parent )
{
  EmailSelector dlg( emails, current, parent );
  dlg.exec();

  return dlg.selected();
}


#ifndef KAB_EMBEDDED
#include "distributionlistwidget.moc"
#endif //KAB_EMBEDDED
