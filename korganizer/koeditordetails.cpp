/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in thse hope that it will be useful,
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

#include <qtooltip.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qvgroupbox.h>
#include <qwidgetstack.h>
#include <qdatetime.h>
#include <qapp.h>

#include <klocale.h>
#include <kglobal.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#ifndef KORG_NOKABC

#ifdef DESKTOP_VERSION
#include <kabc/addresseedialog.h>
#else //DESKTOP_VERSION
#include <externalapphandler.h>
#endif //DESKTOP_VERSION

#endif

#include <libkcal/incidence.h>

#include "koprefs.h"

#include "koeditordetails.h"

template <>
CustomListViewItem<Attendee *>::~CustomListViewItem()
{
  delete mData;
}

template <>
void CustomListViewItem<Attendee *>::updateItem()
{
  setText(0,mData->name());
  setText(1,mData->email());
  setText(2,mData->roleStr());
  setText(3,mData->statusStr());
  if (mData->RSVP() && !mData->email().isEmpty()) {
    setPixmap(4,SmallIcon("mailappt"));
    setSortKey(4,"j");
  }
  else {
    setPixmap(4,SmallIcon("nomailappt"));
    setSortKey(4,"n");
  }
}


KOEditorDetails::KOEditorDetails (int spacing,QWidget* parent,const char* name)
  : QWidget( parent, name), mDisableItemUpdate( false )
{
  QGridLayout *topLayout = new QGridLayout(this);
  topLayout->setSpacing(spacing);
  topLayout->setMargin(KDialog::marginHint()-2);

  QString organizer = KOPrefs::instance()->email();
  mOrganizerLabel = new QLabel(i18n("Organizer: %1").arg(organizer),this);

  mListView = new KListView(this,"mListView");
  mListView->addColumn(i18n("Name"),180);
  mListView->addColumn(i18n("Email"),180);
  mListView->addColumn(i18n("Role"),60);
  mListView->addColumn(i18n("Status"),100);
  mListView->addColumn(i18n("RSVP"),35);
  if ( QApplication::desktop()->width() <= 320 ||  QApplication::desktop()->height() <= 240) {
      int hei = 80;
      if ( QApplication::desktop()->height() <= 240  )
          hei = 60;
      mListView->setFixedHeight(hei);
  }
  mListView->setAllColumnsShowFocus (true );
  //mListView->setSingleClick( true );
  connect(mListView,SIGNAL(selectionChanged(QListViewItem *)),
          SLOT(updateAttendeeInput()));

  connect(mListView,SIGNAL(executed(QListViewItem * ,const QPoint&, int )),
          SLOT(itemClicked(QListViewItem * ,const QPoint& , int )));

  mRsvpButton = new QCheckBox(this);
  mRsvpButton->setText(i18n("Request response"));
  mAddressBookButton = new QPushButton(i18n("Address &Book..."),this);
  QLabel *attendeeLabel = new QLabel(this);
  attendeeLabel->setText(i18n("Name:"));
  attendeeLabel->setFixedSize( attendeeLabel->sizeHint()  );
  mNameEdit = new QLineEdit(this);
  connect(mNameEdit,SIGNAL(textChanged(const QString &)),
          SLOT(updateAttendeeItem()));


  QLabel *emailLabel = new QLabel(this);
  emailLabel->setText(i18n("Email:"));
  mEmailEdit = new QLineEdit(this);
  connect(mEmailEdit,SIGNAL(textChanged(const QString &)),
          SLOT(updateAttendeeItem()));

  QLabel *attendeeRoleLabel = new QLabel(this);
  attendeeRoleLabel->setText(i18n("Role:"));
  mRoleCombo = new QComboBox(false,this);
  mRoleCombo->insertStringList(Attendee::roleList());
  connect(mRoleCombo,SIGNAL(activated(int)),SLOT(updateAttendeeItem()));

  QLabel *statusLabel = new QLabel(this);
  statusLabel->setText( i18n("Status:") );

  mStatusCombo = new QComboBox(false,this);
  mStatusCombo->insertStringList(Attendee::statusList());
  connect(mStatusCombo,SIGNAL(activated(int)),SLOT(updateAttendeeItem()));


  connect(mRsvpButton,SIGNAL(clicked()),SLOT(updateAttendeeItem()));
  QWidget *buttonBox = new QWidget(this);
  QVBoxLayout *buttonLayout = new QVBoxLayout(buttonBox);

  QPushButton *newButton = new QPushButton(i18n("&New"),buttonBox);
  buttonLayout->addWidget(newButton);
  connect(newButton,SIGNAL(clicked()),SLOT(addNewAttendee()));

  mRemoveButton = new QPushButton(i18n("&Remove"),buttonBox);
  buttonLayout->addWidget(mRemoveButton);
  connect(mRemoveButton, SIGNAL(clicked()),SLOT(removeAttendee()));

  // buttonLayout->addWidget(mAddressBookButton);
  connect(mAddressBookButton,SIGNAL(clicked()),SLOT(openAddressBook()));
  //mRoleCombo->setFixedSize( mRoleCombo->sizeHint () );
  if ( QApplication::desktop()->height() <= 240 ) {
      mRoleCombo->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred  ,QSizePolicy::Minimum ,FALSE) );
      mStatusCombo->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred  ,QSizePolicy::Minimum  ,FALSE) );
      topLayout->addMultiCellWidget(mOrganizerLabel,0,0,0,5);
          topLayout->addMultiCellWidget(mListView,1,1,0,5);
          topLayout->addWidget(attendeeLabel,3,0);
          topLayout->addMultiCellWidget(mNameEdit,3,3,1,4);
          topLayout->addWidget(emailLabel,4,0);
          topLayout->addMultiCellWidget(mEmailEdit,4,4,1,4);
          topLayout->addWidget(attendeeRoleLabel,5,0);
          topLayout->addMultiCellWidget(mRoleCombo,5,5,1,2);
          topLayout->addWidget(statusLabel,5,3);
          topLayout->addMultiCellWidget(mStatusCombo,5,5,4,5);
          topLayout->addMultiCellWidget(mAddressBookButton,2,2,2,5);
          topLayout->addMultiCellWidget(mRsvpButton,2,2,0,1);
          topLayout->addMultiCellWidget(buttonBox,3,4,5,5);
          topLayout->setRowStretch(1,5);
          topLayout->setColStretch(0,0);
  } else {
      if (qApp->desktop()->width() < 640 ) {
          if ( qApp->desktop()->width() < 300 )
              topLayout->setSpacing(1);
          ;//mListView->setFixedHeight(80);
          topLayout->addMultiCellWidget(mOrganizerLabel,0,0,0,3);
          topLayout->addMultiCellWidget(mListView,1,1,0,3);
          topLayout->addWidget(attendeeLabel,3,0);
          topLayout->addMultiCellWidget(mNameEdit,3,3,1,2);
          topLayout->addWidget(emailLabel,4,0);
          topLayout->addMultiCellWidget(mEmailEdit,4,4,1,2);
          topLayout->addWidget(attendeeRoleLabel,5,0);
          topLayout->addMultiCellWidget(mRoleCombo,5,5,1,3);
          topLayout->addWidget(statusLabel,6,0);
          topLayout->addMultiCellWidget(mStatusCombo,6,6,1,3);
          topLayout->addMultiCellWidget(mAddressBookButton,2,2,2,3);
          topLayout->addMultiCellWidget(mRsvpButton,2,2,0,1);
          topLayout->addMultiCellWidget(buttonBox,3,4,3,3);
          topLayout->setRowStretch(1,2);
          topLayout->setColStretch(0,0);
          topLayout->setColStretch(1,2);
          topLayout->setColStretch(2,1);
          topLayout->setColStretch(3,1);
      } else {
          topLayout->addMultiCellWidget(mOrganizerLabel,0,0,0,5);
          topLayout->addMultiCellWidget(mListView,1,1,0,5);
          topLayout->addWidget(attendeeLabel,3,0);
          topLayout->addMultiCellWidget(mNameEdit,3,3,1,4);
          topLayout->addWidget(emailLabel,4,0);
          topLayout->addMultiCellWidget(mEmailEdit,4,4,1,4);
          topLayout->addWidget(attendeeRoleLabel,5,0);
          topLayout->addMultiCellWidget(mRoleCombo,5,5,1,2);
          topLayout->addWidget(statusLabel,5,3);
          topLayout->addMultiCellWidget(mStatusCombo,5,5,4,5);
          topLayout->addMultiCellWidget(mAddressBookButton,2,2,4,5);
          topLayout->addMultiCellWidget(mRsvpButton,2,2,0,1);
          topLayout->addMultiCellWidget(buttonBox,3,4,5,5);
          topLayout->setRowStretch(1,5);
          topLayout->setColStretch(0,0);
      }
  }
// #if 0
//   topLayout->setColStretch(2,1);
//   topLayout->addWidget(statusLabel,3,3);
//   topLayout->addWidget(mStatusCombo,3,4);
// #else
//   topLayout->addWidget(statusLabel,4,3);
//   // topLayout->addWidget(mStatusCombo,4,3);
//   topLayout->addMultiCellWidget(mStatusCombo,4,4,4,5);

// #endif
//   // topLayout->setRowStretch(5,1);
//   topLayout->addMultiCellWidget(mRsvpButton,5,5,0,1);
//   topLayout->addMultiCellWidget(buttonBox,2,3,5,5);
//   topLayout->setRowStretch(1,5);
//   topLayout->setColStretch(0,0);

#ifdef KORG_NOKABC
  mAddressBookButton->hide();
#endif

  updateAttendeeInput();
#ifndef DESKTOP_VERSION
//US listen for arriving address resultsets
  connect(ExternalAppHandler::instance(), SIGNAL(receivedNameEmailUidListEvent(const QString&, const QStringList&, const QStringList&, const QStringList&)),
          this, SLOT(insertAttendees(const QString&, const QStringList&, const QStringList&, const QStringList&)));
#endif

}

KOEditorDetails::~KOEditorDetails()
{
}

void KOEditorDetails::removeAttendee()
{
  AttendeeListItem *aItem = (AttendeeListItem *)mListView->selectedItem();
  if (!aItem) return;

  Attendee *delA = new Attendee(aItem->data()->name(),aItem->data()->email(),
    aItem->data()->RSVP(),aItem->data()->status(),aItem->data()->role(),
    aItem->data()->uid());
  mdelAttendees.append(delA);

  delete aItem;

  updateAttendeeInput();
}


void KOEditorDetails::openAddressBook()
{
#ifndef KORG_NOKABC

#ifdef DESKTOP_VERSION
    KABC::Addressee::List list = KABC::AddresseeDialog::getAddressees(this);
    uint i=0;
    for (i=0; i < list.count(); i++) {
        insertAttendee( new Attendee( list[i].realName(), list[i].preferredEmail(),false,KCal::Attendee::NeedsAction,KCal::Attendee::ReqParticipant,list[i].uid()) );
    }
#else
  bool res = ExternalAppHandler::instance()->requestNameEmailUidListFromKAPI("QPE/Application/kopi", this->name() /* name is here the unique uid*/);
  // the result should now arrive through method insertAttendees
#endif


#if 0
  KABC::Addressee a = KABC::AddresseeDialog::getAddressee(this);
  if (!a.isEmpty()) {
    insertAttendee( new Attendee( a.realName(), a.preferredEmail(),false,KCal::Attendee::NeedsAction,KCal::Attendee::ReqParticipant,a.uid()) );
  }
#endif
#endif
}


void KOEditorDetails::addNewAttendee()
{
#if 0
  // this is cool.  If they didn't enter an email address,
  // try to look it up in the address book and fill it in for them.
  if (QString(mEmailEdit->text()).stripWhiteSpace().isEmpty()) {
    KabAPI addrBook;
    QString name;
    std::list<AddressBook::Entry> entries;
    name = mNameEdit->text();
    if (addrBook.init() == AddressBook::NoError) {
      if (addrBook.getEntryByName(name, entries, 1) == AddressBook::NoError) {
	kdDebug() << "positive match" << endl;
	// take first email address
	if (!entries.front().emails.isEmpty() &&
	    entries.front().emails.first().length()>0)
	  mEmailEdit->setText(entries.front().emails.first());
      }
    }
  }
#endif

  Attendee *a = new Attendee(i18n("(EmptyName)"),i18n("(EmptyEmail)"));
  insertAttendee(a);
}

//the map includes name/email pairs, that comes from Ka/Pi
void KOEditorDetails::insertAttendees(const QString& uid,const QStringList& nameList,const QStringList& emailList,const QStringList& uidList)
{
  if (uid == this->name())
  {
    for ( int i = 0; i < nameList.count(); i++)
    {
      QString _name = nameList[i];
      QString _email = emailList[i];
      QString _uid = uidList[i];

      Attendee *a = new Attendee(_name,_email,false,KCal::Attendee::NeedsAction,KCal::Attendee::ReqParticipant, _uid);
      insertAttendee(a);
    }
  }

}

void KOEditorDetails::insertAttendee(Attendee *a)
{
    AttendeeListItem *first =  (AttendeeListItem*) mListView->firstChild();
    while (first) {
        if ( first->data()->name() == a->name() &&  first->data()->email() == a->email() )
            return;
        first = (AttendeeListItem*) first->nextSibling();
    }

  AttendeeListItem *item = new AttendeeListItem(a,mListView);
  mListView->setSelected( item, true );
}

void KOEditorDetails::setDefaults()
{
  mRsvpButton->setChecked(true);
  mListView->clear();
  mdelAttendees.clear();
  clearAttendeeInput();
  mOrganizerLabel->setText(i18n("Organizer: %1").arg(KOPrefs::instance()->email()));

  mNameEdit->setText("");
  mUidEdit = "";
  mEmailEdit->setText("");
  mRoleCombo->setCurrentItem( 0 );
  mStatusCombo->setCurrentItem( 0 );

}

void KOEditorDetails::readEvent(Incidence *event)
{
    setDefaults();
    //mListView->clear();
    //mdelAttendees.clear();
  QPtrList<Attendee> tmpAList = event->attendees();
  Attendee *a;
  for (a = tmpAList.first(); a; a = tmpAList.next())
    insertAttendee(new Attendee(*a));

  mListView->setSelected( mListView->firstChild(), true );
  mOrganizerLabel->setText(i18n("Organizer: %1").arg(event->organizer()));
}

void KOEditorDetails::writeEvent(Incidence *event)
{
  event->clearAttendees();
  QListViewItem *item;
  AttendeeListItem *a;
  for (item = mListView->firstChild(); item;
       item = item->nextSibling()) {
    a = (AttendeeListItem *)item;
    event->addAttendee(new Attendee(*(a->data())));
  }
  event->setOrganizer(KOPrefs::instance()->email());
}

void KOEditorDetails::cancelAttendeeEvent(Incidence *event)
{
  event->clearAttendees();
  Attendee * att;
  for (att=mdelAttendees.first();att;att=mdelAttendees.next()) {
    event->addAttendee(new Attendee(*att));
  }
  mdelAttendees.clear();
}

bool KOEditorDetails::validateInput()
{
  return true;
}

void KOEditorDetails::updateAttendeeInput()
{
  QListViewItem *item = mListView->selectedItem();
  AttendeeListItem *aItem = static_cast<AttendeeListItem *>( item );
  if (aItem) {
    fillAttendeeInput( aItem );
  } else {
    clearAttendeeInput();
  }
}

void KOEditorDetails::clearAttendeeInput()
{
  mNameEdit->setText("");
  mUidEdit = "";
  mEmailEdit->setText("");
  mRoleCombo->setCurrentItem(0);
  mStatusCombo->setCurrentItem(0);
  mRsvpButton->setChecked(true);
  setEnabledAttendeeInput( false );
}

void KOEditorDetails::fillAttendeeInput( AttendeeListItem *aItem )
{
  Attendee *a = aItem->data();
  mDisableItemUpdate = true;
  mNameEdit->setText(a->name());
  mUidEdit = a->uid();
  mEmailEdit->setText(a->email());
  mRoleCombo->setCurrentItem(a->role());
  mStatusCombo->setCurrentItem(a->status());
  mRsvpButton->setChecked(a->RSVP());

  mDisableItemUpdate = false;

  setEnabledAttendeeInput( true );
}

void KOEditorDetails::setEnabledAttendeeInput( bool enabled )
{
  mNameEdit->setEnabled( enabled );
  mEmailEdit->setEnabled( enabled );
  mRoleCombo->setEnabled( enabled );
  mStatusCombo->setEnabled( enabled );
  mRsvpButton->setEnabled( enabled );

  mRemoveButton->setEnabled( enabled );
}

void KOEditorDetails::itemClicked(QListViewItem * item ,const QPoint & pnt, int c )
{
    if ( item  && c == 4 ) {
        mRsvpButton->setChecked( !mRsvpButton->isChecked() );
        updateAttendeeItem();
    }
}
void KOEditorDetails::updateAttendeeItem()
{
  if (mDisableItemUpdate) return;

  QListViewItem *item = mListView->selectedItem();
  AttendeeListItem *aItem = static_cast<AttendeeListItem *>( item );
  if ( !aItem ) return;

  Attendee *a = aItem->data();

  a->setName( mNameEdit->text() );
  a->setUid( mUidEdit );
  a->setEmail( mEmailEdit->text() );
  if ( mEmailEdit->text().isEmpty() )
      mRsvpButton->setChecked( false );
  a->setRSVP( mRsvpButton->isChecked() );
  a->setRole( Attendee::Role( mRoleCombo->currentItem() ) );
  a->setStatus( Attendee::PartStat( mStatusCombo->currentItem() ) );
  aItem->updateItem();
}
