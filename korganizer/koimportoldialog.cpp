/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <qtooltip.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qprogressbar.h>
#include <qprogressdialog.h>
#include <qwidgetstack.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qapplication.h>
#include <qhbox.h>
#include <qregexp.h>
#include <qheader.h>
#include <qdatetime.h>
#include <qlistview.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfiledialog.h>

#include <libkdepim/categoryselectdialog.h>
#include <libkdepim/kinputdialog.h>

#include <libkcal/calendarlocal.h>
#include <libkcal/icalformat.h>
#include <kabc/stdaddressbook.h>

#include "koprefs.h"
#include "koglobals.h"

#include "koimportoldialog.h"

#include "../outport/msoutl9.h"
#include <ole2.h>
#include <comutil.h>
_Application gOlApp;

QDateTime mDdate2Qdtr( DATE dt)
{
    COleDateTime odt;
    SYSTEMTIME st;
    odt = dt;
    odt.GetAsSystemTime(st);
    QDateTime qdt (QDate(st.wYear, st.wMonth,st.wDay ),QTime( st.wHour, st.wMinute,st.wSecond ) );
    return qdt;
}

class OLEListViewItem : public QCheckListItem
{
  public:
    OLEListViewItem( QListView *parent, QString text ) :
      QCheckListItem( parent, text, QCheckListItem::CheckBox ) { mData = 0; };
    OLEListViewItem(  QListViewItem *after, QString text ) :
      QCheckListItem( after, text, QCheckListItem::CheckBox ) { mData = 0; };
    ~OLEListViewItem() {};
  void setData( DWORD data ) {mData= data; };
  DWORD data() { return mData ;};
  private:
     DWORD  mData;
}; 

KOImportOLdialog::KOImportOLdialog( const QString &caption, 
                                      Calendar *calendar, QWidget *parent ) :
    KDialogBase( Plain, caption, User1 | Close, Ok,
               parent, caption, true, false, i18n("Import!") )
{
  QHBox * mw = new  QHBox( this );
  setMainWidget( mw );
  mListView = new QListView( mw );
  mListView->addColumn(i18n("Select Folder to import"));
  mListView->addColumn(i18n("Content Type"));
  mCalendar = calendar;
  connect( this, SIGNAL( user1Clicked() ),SLOT ( slotApply()));
  setupFolderView();
  resize( sizeHint().width()+50,  sizeHint().height()+50 );
}

KOImportOLdialog::~KOImportOLdialog()
{
   
}


void KOImportOLdialog::setupFolderView()
{
  SCODE sc = ::OleInitialize(NULL);
  if ( FAILED ( sc ) ) {
    KMessageBox::information(this,"OLE initialisation failed");
    return;
  }
  
  if(!gOlApp.CreateDispatch(_T("Outlook.Application"),NULL)){
    KMessageBox::information(this,"Sorry, cannot access Outlook");
    return ;
  }
  MAPIFolder mfInbox;
  MAPIFolder mfRoot;
  CString szName;
  _NameSpace olNS; 
  olNS = gOlApp.GetNamespace(_T("MAPI"));
  mfInbox = olNS.GetDefaultFolder(6);
  mfRoot = mfInbox.GetParent();
  szName = mfRoot.GetName();
  long iType = mfRoot.GetDefaultItemType();
  QString mes;
  mes = QString::fromUcs2( szName.GetBuffer() );
  OLEListViewItem * root = new OLEListViewItem( mListView, mes );
  mfRoot.m_lpDispatch->AddRef();
  addFolder( root, mfRoot.m_lpDispatch );
  root->setOpen( true );
  mListView->setSortColumn( 0 );
  mListView->sort( );
}


void KOImportOLdialog::addFolder(OLEListViewItem* iParent, LPDISPATCH dispParent)
{
  MAPIFolder mfParent(dispParent), mfChild;
  _Folders folders;
  _variant_t fndx((long)0);
  CString szName;
  long iType;
  OLEListViewItem* hChild;

    folders = mfParent.GetFolders();    
    for(int i=1; i <= folders.GetCount(); ++i)
    {
      fndx = (long)i;
      mfChild = folders.Item(fndx.Detach());
      mfChild.m_lpDispatch->AddRef();
      szName = mfChild.GetName();
      iType = mfChild.GetDefaultItemType();
      hChild = new OLEListViewItem( iParent , QString::fromUcs2( szName.GetBuffer() ) );
      if ( iType != 1)
	hChild->setEnabled( false );
      QString ts; 
      switch( iType ) {
      case 0:
	ts = i18n("Mail");
	break;
      case 1:
	ts = i18n("Calendar");
	break;
      case 2:
	ts = i18n("Contacts");
	break;
      case 3:
	ts = i18n("Todos");
	break;
      case 4:
	ts = i18n("Journals");
	break;
      case 5:
	ts = i18n("Notes");
	break;
      default:
	ts = i18n("Unknown");
      }
      hChild->setText( 1,ts);
      hChild->setData( (DWORD) mfChild.m_lpDispatch );
      mfChild.m_lpDispatch->AddRef();
      addFolder(hChild, mfChild.m_lpDispatch);
    }
}

void KOImportOLdialog::slotApply()
{
  importedItems = 0;
  OLEListViewItem* child = (OLEListViewItem*) mListView->firstChild();
  while ( child ) {
    if ( child->isOn()&& child->data() )
      readCalendarData( child->data() );
    child = (OLEListViewItem*) child->itemBelow();
  }
  QString mes = i18n("Importing complete.\n\n%1 items imported.").arg( importedItems);
  KMessageBox::information(this,mes);
}
void KOImportOLdialog::readCalendarData( DWORD folder )
{

  LPDISPATCH dispItem = (LPDISPATCH)folder;
  dispItem->AddRef();
  MAPIFolder mf(dispItem);
  mf.m_lpDispatch->AddRef();
  _Items folderItems;
  _variant_t indx((long)0);
  LPDISPATCH itm;
  int i;
  folderItems = mf.GetItems(); 
  QProgressDialog bar( i18n("Importing calendar data"),i18n("Abort"), folderItems.GetCount(),this );
  bar.setCaption (i18n("Importing!") );
  int h = bar.sizeHint().height() ;
  int w = 300;
  int dw = QApplication::desktop()->width();
  int dh = QApplication::desktop()->height();
  //bar.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
  bar.show();
  for(i=1; i <= folderItems.GetCount(); ++i)
    {
      qApp->processEvents();
      if ( ! bar.isVisible() )
            return ;
      bar.setProgress( i );
      bar.raise();
      indx = (long)i;
      itm = folderItems.Item(indx.Detach());
      _AppointmentItem * pItem = (_AppointmentItem *)&itm;
      ol2kopiCalendar( pItem );
      itm->Release();
    }
}
void KOImportOLdialog::slotOk()
{
   QDialog::accept();
}

void KOImportOLdialog::ol2kopiCalendar( _AppointmentItem * aItem, bool computeRecurrence )
{
  KCal::Event* event = new KCal::Event();
  if ( aItem->GetAllDayEvent() ){
    event->setDtStart( QDateTime( mDdate2Qdtr( aItem->GetStart()).date(),QTime(0,0,0 ) ));
    event->setDtEnd( QDateTime( mDdate2Qdtr( aItem->GetEnd()) .date(),QTime(0,0,0 )).addDays(-1));
    event->setFloats( true );
  } else {
    event->setDtStart( mDdate2Qdtr( aItem->GetStart()) );
    event->setDtEnd( mDdate2Qdtr( aItem->GetEnd()) );
    event->setFloats( false );
  }
  event->setSummary( QString::fromUcs2( aItem->GetSubject().GetBuffer()).stripWhiteSpace() );
  event->setLocation( QString::fromUcs2( aItem->GetLocation().GetBuffer()).stripWhiteSpace() );
  event->setDescription( QString::fromUcs2( aItem->GetBody().GetBuffer()).replace( QRegExp("\\r"), "").stripWhiteSpace()  );
  QString cat = QString::fromUcs2( aItem->GetCategories().GetBuffer()).replace( QRegExp("; "), ";");
  event->setCategories( QStringList::split( ";", cat ) );
  if ( aItem->GetReminderSet() ) {
    event->clearAlarms();
    Alarm* alarm = event->newAlarm();
    alarm->setStartOffset( -aItem->GetReminderMinutesBeforeStart()*60 );
    alarm->setEnabled( true );
    if ( aItem->GetReminderPlaySound() ) {
      alarm->setType( Alarm::Audio );
      alarm->setAudioFile( QString::fromUcs2( aItem->GetReminderSoundFile().GetBuffer()));
      }
    else
      alarm->setType( Alarm::Display );
    alarm->setRepeatCount( aItem->GetReplyTime() );
  }
  // OL :pub 0 - pers 1  -  priv 2 -  conf 3
  // KO : pub 0  -  priv 1 -  conf 2
  int sec =   aItem->GetSensitivity() ;
  if ( sec > 1 )// mapping pers -> private
    --sec;
  event->setSecrecy( sec );
  if ( aItem->GetBusyStatus() == 0 )
    event->setTransparency( Event::Transparent);// OL free
  else
    event->setTransparency( Event::Opaque);//OL all other	

  if ( aItem->GetIsRecurring() && computeRecurrence ) { //recur

    RecurrencePattern recpat = aItem->GetRecurrencePattern();
 
    QDate startDate = mDdate2Qdtr(recpat.GetPatternStartDate()).date();
    int freq = recpat.GetInterval();
    if ( freq == 0 ) freq = 1;
    bool hasEndDate = !recpat.GetNoEndDate();
    QDate endDate = mDdate2Qdtr(recpat.GetPatternEndDate()).date();
    QBitArray weekDays( 7 );
    weekDays.fill(false );
    uint weekDaysNum = recpat.GetDayOfWeekMask();
    int i;
    int bb = 2;
    for( i = 1; i <= 6; ++i ) {
      weekDays.setBit( i - 1, ( bb & weekDaysNum )); 
      bb =  4 << (i-1);
      //qDebug(" %d bit %d ",i-1,weekDays.at(i-1) );
    }
    if ( 1 & weekDaysNum)
      weekDays.setBit( 6 );
    // int pos = 1;// pending

    Recurrence *r = event->recurrence();
    int rtype = recpat.GetRecurrenceType();
    //recurrence types are:
    /*
        olRecursDaily(0)
        olRecursWeekly(1)
        olRecursMonthly(2)
        olRecursMonthNth(3)
        olRecursYearly(5)
        olRecursYearNth(6)
    */

    int duration = recpat.GetOccurrences();
    if ( !hasEndDate )
      duration = -1;
   
    //LPDISPATCH RecurrencePattern::GetExceptions()
    //long RecurrencePattern::GetMonthOfYear()
    if ( rtype == 0 ) {
      if ( hasEndDate ) r->setDaily( freq, endDate );
      else r->setDaily( freq, duration );
    } else if ( rtype == 1 ) {
      if ( hasEndDate ) r->setWeekly( freq, weekDays, endDate );
      else r->setWeekly( freq, weekDays, duration );
    } else if ( rtype == 2 ) {
      if ( hasEndDate )
	r->setMonthly( Recurrence::rMonthlyDay, freq, endDate );
      else
	r->setMonthly( Recurrence::rMonthlyDay, freq, duration );
      //r->addMonthlyDay( startDate.day() );
      r->addMonthlyDay( recpat.GetDayOfMonth() );
    } else if ( rtype == 3 ) {
      if ( hasEndDate )
	r->setMonthly( Recurrence::rMonthlyPos, freq, endDate );
      else
	r->setMonthly( Recurrence::rMonthlyPos, freq, duration );
      QBitArray days( 7 );
      days.fill( false );
      days.setBit( startDate.dayOfWeek() - 1 );
      int pos = (startDate.day()/7)+1;
      r->addMonthlyPos( pos, days );
      //QString mes = i18n("Importing monthlypos.\n\npos: %1 , day: %2").arg( pos).arg( startDate.dayOfWeek() - 1);
      //KMessageBox::information(this,mes);
    } else if ( rtype == 5 ) {
      freq = 1;
      if ( hasEndDate )
	r->setYearly( Recurrence::rYearlyMonth, freq, endDate );
      else
	r->setYearly( Recurrence::rYearlyMonth, freq, duration );
      r->addYearlyNum( startDate.month() );
    } else if ( true /*rtype == 6*/ ) { 
      // KOganizer cannot handle this in the GUI
      // we are mapping this to monthly - every 12. month
      freq = 12;
      if ( hasEndDate )
	r->setMonthly( Recurrence::rMonthlyPos, freq, endDate );
      else
	r->setMonthly( Recurrence::rMonthlyPos, freq, duration );
      QBitArray days( 7 );
      days.fill( false );
      days.setBit( startDate.dayOfWeek() - 1 );
      int pos = (startDate.day()/7)+1;
      r->addMonthlyPos( pos, days );
    }
    // recurrence exceptions
    LPDISPATCH dispItem = recpat.GetExceptions();
    dispItem->AddRef();
    Exceptions ex(dispItem); 
    _variant_t indx((long)0);
    LPDISPATCH itm;
    for(i=1; i <= ex.GetCount(); ++i) {
	indx = (long)i;
	itm = ex.Item( indx.Detach() );
	::Exception * pItem = (::Exception *)&itm;
	event->addExDate( QDateTime( mDdate2Qdtr( pItem->GetOriginalDate())).date() );
	if ( !pItem->GetDeleted() ) {
	  LPDISPATCH appIt = pItem->GetAppointmentItem();
	  _AppointmentItem * paItem = (_AppointmentItem *)&appIt;
	  ol2kopiCalendar( paItem, false  );
	}
	itm->Release();
    }
  }
  // recurrence ENTE
  event->setOrganizer( QString::fromUcs2( aItem->GetOrganizer().GetBuffer()));

  //GetOptionalAttendees()
  //GetRequiredAttendees()
  LPDISPATCH dispItem = aItem->GetRecipients();
  dispItem->AddRef();
  _Folders mf(dispItem);
  mf.m_lpDispatch->AddRef();
  _variant_t indx((long)0);  
  LPDISPATCH itm;
  int i;
  QString optAtt = QString::fromUcs2( aItem->GetOptionalAttendees().GetBuffer());
  QString reqAtt = QString::fromUcs2( aItem->GetRequiredAttendees().GetBuffer());
  //GetRequiredAttendees()
  for(i=1; i <= mf.GetCount(); ++i) {
  	indx = (long)i;
	itm = mf.Item( indx.Detach() );
	Recipient * pItem = (Recipient *)&itm;
	
	//a = new KCal::Attendee( (*it).realName(), (*it).preferredEmail(),false,KCal::Attendee::NeedsAction,KCal::Attendee::ReqParticipant,(*it).uid()) ;
	QString name =  QString::fromUcs2( pItem->GetName().GetBuffer());
	KCal::Attendee::PartStat stat;
	bool rsvp = false;
	switch ( pItem->GetMeetingResponseStatus()  ) {
	case 0: //not answered
	  rsvp = true;
	case 5: //not answered
	  stat = Attendee::NeedsAction;
	  break;
	case 1: //organizer
	  stat = Attendee::Delegated ;
	  break;
	case 2: //tentative 
	  stat = Attendee::Tentative ;
	  break;
	case 3: //accepted
	  stat = Attendee::Accepted;
	  break;
	case 4: //declined
	  stat =Attendee::Declined ;
	  break;
	default: 
	  stat = Attendee::NeedsAction ;

	}
	KCal::Attendee::Role role;
	if ( event->organizer() == name )
	  role = KCal::Attendee::Chair;
	else if ( reqAtt.find( name ) >= 0   )
	  role = KCal::Attendee::ReqParticipant;
	else if ( optAtt.find( name ) >= 0   )
	  role = KCal::Attendee::OptParticipant;
	else  
	  role = KCal::Attendee::NonParticipant;
	QString mail = QString::fromUcs2(pItem->GetAddress().GetBuffer());
	if( mail.isEmpty() && name.find("@") > 0 ) {
	  int kl = name.find("<");
	  int gr = name.find(">");
	  if ( kl >= 0 && gr >= 0) {
	    mail = name.mid (kl+1, gr - kl -1);
	    name = name.left( kl );
	  }
	  else
	    mail = name;
	}
	mail = mail.stripWhiteSpace();
	
	QString uid = getUidByEmail( mail );
	//uid =  QString::fromUcs2( pItem->GetEntryID().GetBuffer());
	KCal::Attendee * a = new KCal::Attendee( name, mail,rsvp,stat,role,uid) ;
	event->addAttendee( a , false );
	itm->Release();
  }


  if ( !mCalendar->addEventNoDup( event ))
    delete event;
  else {
    // QString mes = i18n("Importing %1.\n date: %2 date: %3").arg( event->summary()).arg( event->dtStart().toString()).arg( event->dtEnd().toString());
    //KMessageBox::information(this,mes);
    ++importedItems;
  }
}
void KOImportOLdialog::slotCancel()
{
  reject();
}

QString KOImportOLdialog::getUidByEmail( QString email )
{
  KABC::StdAddressBook* AddressBook = KABC::StdAddressBook::self( true );
  KABC::AddressBook::Iterator it;
  for( it = AddressBook->begin(); it != AddressBook->end(); ++it ) {
    QStringList em = (*it).emails();
    if ( em.contains( email ))
      return (*it).uid();
  }
  return email;
}
