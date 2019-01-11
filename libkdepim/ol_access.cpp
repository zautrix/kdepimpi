/*
    This file is part of KDE-Pim/Pi.
    Copyright (c) 2006 Lutz Rogowski <rogowski@kde.org>

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
#include <qpushbutton.h>
#include <qregexp.h>
#include <qapplication.h>
#include <qhbox.h>
#include <qheader.h>
#include <qdatetime.h>
#include <qlistview.h>

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstaticdeleter.h>
#include <kdialogbase.h>


#include <ol_access.h>

#include "../outport/msoutl9.h"
#include <ole2.h>
#include <comutil.h>


class OLEListViewItem : public QCheckListItem
{
public:
    OLEListViewItem( QListView *parent, QString text ) :
        QCheckListItem( parent, text, QCheckListItem::CheckBox ) { ; };
    OLEListViewItem(  QListViewItem *after, QString text ) :
        QCheckListItem( after, text, QCheckListItem::CheckBox ) { ; };
    ~OLEListViewItem() {};
    void setData( DWORD data ) {mData= data; };
    void setDataID( QString data ){ mDataID = data ;}
    QString dataID() { return mDataID;}
    DWORD data() { return mData ;};
private:
    DWORD  mData;
    QString mDataID;
}; 

class OLEFolderSelect : public KDialogBase
{
  public:
    OLEFolderSelect() :
      KDialogBase( Plain, "", Ok | Cancel, Ok,
               0, "", true, false, i18n("Import!") )
    {
        QHBox * mw = new  QHBox( this );
        setMainWidget( mw );
        mListView = new QListView( mw );
        mListView->addColumn(i18n("Select Folder"));
        mListView->addColumn(i18n("Content Type"));
    }
    QListView* listView() { return mListView;}
private:
    QListView* mListView;
};

OL_access *OL_access::sInstance = 0;
static KStaticDeleter<OL_access> staticDeleterPim;

OL_access *OL_access::instance()
{
  if ( !sInstance ) {
    sInstance = staticDeleterPim.setObject( new OL_access() );
  }
  return sInstance;
}


OL_access::OL_access()
{
    mErrorInit = true;
    SCODE sc = ::OleInitialize(NULL);
    if ( FAILED ( sc ) ) {
        KMessageBox::information( 0 ,i18n("OLE initialisation failed" ));
        return;
    } 
    if(!gOlAppAB.CreateDispatch(_T("Outlook.Application"),NULL)){
        KMessageBox::information( 0, i18n("Sorry, cannot access Outlook") );
        return ;
    }
    mErrorInit = false;
}
OL_access::~OL_access()
{
    if (sInstance == this)
        sInstance = staticDeleterPim.setObject(0);
}

KABC::Addressee::List OL_access::importOLcontacts()
{
    KABC::Addressee::List retval;
    if ( mErrorInit )
        return retval;
    QStringList folderList = getFolderSelection( OL_CONTACT_DATA , i18n("Select Folders to import"));
    int numFolders = folderList.count()/2;
    int i;
    for ( i = 0; i < numFolders; ++i ) {
        readContactData( getFolderFromID( 0, folderList[(i*2)+1] ) , &retval, false );
    }
    KMessageBox::information( 0, i18n("%1 contacts read").arg( retval.count() ) );
    return retval;
}
void OL_access::readContactData( LPDISPATCH  dispItem, KABC::Addressee::List* list, bool syncMode )
{
    dispItem->AddRef();
    MAPIFolder mf(dispItem);
    mf.m_lpDispatch->AddRef();
    _Items folderItems;
    _variant_t indx((long)0);
    LPDISPATCH itm;
    int i;
    folderItems = mf.GetItems(); 
    QString cap = i18n("Importing contact data");
    if (  syncMode ) {
        cap = i18n("Reading contact data...");
    }
    QProgressDialog bar( cap,i18n("Abort"), folderItems.GetCount(), 0 );
    bar.setCaption (i18n("Accessing OL") );
    int h = bar.sizeHint().height() ;
    int w = 300;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    bar.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    bar.show();
    for(i=1; i <= folderItems.GetCount(); ++i)
        {
            qApp->processEvents();
            if ( ! bar.isVisible() )
                return ;
            bar.setProgress( i );
            indx = (long)i;
            itm = folderItems.Item(indx.Detach());
            _ContactItem * pItem = (_ContactItem *)&itm;
            list->append( ol2kapiContact( pItem, syncMode ) );
            itm->Release();
        }
}
QStringList OL_access::getFolderSelection( int type , QString caption )
{
    OLEFolderSelect folder_dialog;
    QListView * listView = folder_dialog.listView();
    MAPIFolder mfInbox;
    MAPIFolder mfRoot;
    CString szName;
    _NameSpace olNS; 
    olNS = gOlAppAB.GetNamespace(_T("MAPI"));
    mfInbox = olNS.GetDefaultFolder(6);
    mfRoot = mfInbox.GetParent();
    szName = mfRoot.GetName();
    long iType = mfRoot.GetDefaultItemType();
    QString mes;
    mes = QString::fromUcs2( szName.GetBuffer() );
    OLEListViewItem * root = new OLEListViewItem( listView, mes );
    mfRoot.m_lpDispatch->AddRef();
    addFolder( root, mfRoot.m_lpDispatch, type );
    root->setOpen( true );
    listView->setSortColumn( 0 );
    listView->sort( );
    folder_dialog.setCaption( caption );
    QStringList retval;
    folder_dialog.resize( folder_dialog.sizeHint().width()+50,  folder_dialog.sizeHint().height()+50 );
    if ( folder_dialog.exec() ) {
        OLEListViewItem* child = (OLEListViewItem*) listView->firstChild();
        while ( child ) {
            if ( child->isOn() && ! child->firstChild() ) {
                retval << child->text( 0 );
                retval << child->dataID();
            }
            child = (OLEListViewItem*) child->itemBelow();
        }
        //KMessageBox::information(0,retval.join(" \n "));
    }
    return retval;
}
void OL_access::addFolder(OLEListViewItem* iParent, LPDISPATCH dispParent, long fType)
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
            iParent->setEnabled( true );
            fndx = (long)i;
            mfChild = folders.Item(fndx.Detach());
            mfChild.m_lpDispatch->AddRef();
            szName = mfChild.GetName();
            iType = mfChild.GetDefaultItemType();
            hChild = new OLEListViewItem( iParent , QString::fromUcs2( szName.GetBuffer() ) );
            hChild->setDataID( QString::fromUcs2(mfChild.GetEntryID().GetBuffer()) );
            if ( iType != fType)
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
            addFolder(hChild, mfChild.m_lpDispatch, fType);
        }
}

void OL_access::deleteAddressee( KABC::Addressee a )
{
    LPDISPATCH itm = getOLcontactItemFromUid ( a.originalExternalUID() );
    _ContactItem* cItem = (_ContactItem *)&itm;
    if ( cItem != 0 ) {
        //KMessageBox::information(0,"delete: item found " +  a.formattedName() + QString::fromUcs2(cItem->GetLastName().GetBuffer()));
        cItem->Delete();
    }
}
KABC::Addressee OL_access::changeAddressee( KABC::Addressee a )
{
    LPDISPATCH itm = getOLcontactItemFromUid ( a.originalExternalUID() );
    _ContactItem* cItem = (_ContactItem *)&itm;
    if ( cItem != 0 ) {
        //KMessageBox::information(0,"Change: item found " +  a.formattedName());
        writeData2OLitem( a, cItem );
        KABC::Addressee retval = ol2kapiContact( cItem , true );
        retval.setUid( a.uid() );
        retval.setExternalUID( a.originalExternalUID() ); 
        retval.setOriginalExternalUID( a.originalExternalUID() ); 
        return retval;
    }
    return KABC::Addressee();
}
KABC::Addressee OL_access::addAddressee( KABC::Addressee a )
{
    QString newOLid;
    LPDISPATCH c_itm = gOlAppAB.CreateItem(2);
    _ContactItem * aItem = (_ContactItem *)&c_itm ;
    writeData2OLitem( a, aItem );
    newOLid = QString::fromUcs2(aItem->GetEntryID().GetBuffer());
    KABC::Addressee retval = ol2kapiContact( aItem , true );
    retval.setUid( a.uid() );
    retval.setExternalUID( newOLid ); 
    retval.setOriginalExternalUID( newOLid ); 
    return retval;
}

LPDISPATCH OL_access::getOLcontactItemFromUid( QString uid )
{
    _variant_t indx((long)0);
    LPDISPATCH itm;
    int i;
    for(i=1; i <= mFolderItems.GetCount(); ++i)
        {
            qApp->processEvents();
            indx = (long)i;
            itm = mFolderItems.Item(indx.Detach());
            _ContactItem * pItem = (_ContactItem *)&itm;
            if ( QString::fromUcs2(pItem->GetEntryID().GetBuffer()) == uid )
                return itm;
            itm->Release();
        }
    return 0;
}
bool OL_access::setSelectedFolder( QString folderID )
{
    if ( mErrorInit )
        return false;
    mSelectedFolderID = folderID;
    mSelectedFolder = getFolderFromID( 0,mSelectedFolderID  );
    if ( mSelectedFolder == 0 ) {
        KMessageBox::information(0,i18n("OL access: No folder found for id\n%1").arg( mSelectedFolderID ));
        return false;
    }
    LPDISPATCH dispItem = mSelectedFolder;
    dispItem->AddRef();
    MAPIFolder mf(dispItem);
    mf.m_lpDispatch->AddRef();
    mFolderItems = mf.GetItems(); 
    return true;
}

QDateTime OL_access::mDdate2Qdtr( DATE dt)
{
    COleDateTime odt;
    SYSTEMTIME st;
    odt = dt;
    if ( odt.GetStatus() != 0 )
        return QDateTime();
    odt.GetAsSystemTime(st);
    if ( st.wYear > 4000  ) // this program as a year 4000 bug!
        return QDateTime();
    // it seems so, that 1.1.4501 indicates: DATE invalid
    QDateTime qdt (QDate(st.wYear, st.wMonth,st.wDay ),QTime( st.wHour, st.wMinute,st.wSecond ) );
    return qdt;
}
DATE OL_access::Qdt2date( QDateTime dt )
{
    DATE ddd;
    COleDateTime odt;
    odt.SetDateTime(dt.date().year(), dt.date().month(),dt.date().day(),
                    dt.time().hour(), dt.time().minute(), dt.time().second() );
    ddd = odt;
    return ddd;
}

LPDISPATCH OL_access::getFolderFromID( LPDISPATCH parentFolder, QString selectedFolderID )
{
    if ( parentFolder == 0 ) {
        MAPIFolder mfInbox;
        MAPIFolder mfRoot;
        CString szName;
        _NameSpace olNS; 
        olNS = gOlAppAB.GetNamespace(_T("MAPI"));
        mfInbox = olNS.GetDefaultFolder(6);
        mfRoot = mfInbox.GetParent();
        szName = mfRoot.GetName();
        mfRoot.m_lpDispatch->AddRef();
        return getFolderFromID( mfRoot.m_lpDispatch ,selectedFolderID);
    }

    MAPIFolder mfParent(parentFolder), mfChild;
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
            if ( selectedFolderID ==  QString::fromUcs2(mfChild.GetEntryID().GetBuffer()) ) {
                return mfChild.m_lpDispatch;
            }
            LPDISPATCH resultFolder = getFolderFromID(mfChild.m_lpDispatch ,selectedFolderID);
            if ( resultFolder != 0 )
                return resultFolder;
        }
    return 0;
}
void OL_access::writeData2OLitem( KABC::Addressee addressee, _ContactItem * aItem )
{
    // addressee.setUid( QString::fromUcs2(aItem->GetEntryID().GetBuffer()));
    //GetLastModificationTime()
    //addressee.setName( const QString &name );

    aItem->SetFileAs(addressee.formattedName().ucs2());
    aItem->SetLastName(addressee.familyName().ucs2());
    aItem->SetFirstName(addressee.givenName( ).ucs2());
    aItem->SetMiddleName(addressee.additionalName( ).ucs2());
    aItem->SetTitle(addressee.prefix().ucs2());
    aItem->SetSuffix(addressee.suffix( ).ucs2());
    aItem->SetNickName(addressee.nickName( ).ucs2());
    if ( addressee.birthday().isValid() )
        aItem->SetBirthday(Qdt2date(addressee.birthday()));
    else
        aItem->SetBirthday(Qdt2date(QDateTime(QDate( 4501,1,1 ))));
    //).ucs2()(aItem->)
    //addressee.Mailer( const QString &mailer );
    //addressee.TimeZone( const TimeZone &timeZone );
    //addressee.Geo( const Geo &geo );
    //addressee.Title( ).ucs2()(aItem->SetJobTitle()) );// titel is the prefix
    aItem->SetJobTitle(addressee.role( ).ucs2());
    aItem->SetCompanyName(addressee.organization( ).ucs2());
    QString notesStr = addressee.note();
    notesStr.replace( QRegExp("\\n"), "\n\r");
    aItem->SetBody(notesStr.ucs2());
   
    aItem->SetCustomerID(addressee.productId( ).ucs2());
    //addressee.Revision( const QDateTime &revision );
    // addressee.SortString( const QString &sortString );
    aItem->SetWebPage( addressee.url().url().ucs2());
 
    QString tempS;

 
    tempS =  addressee.custom( "KADDRESSBOOK", "X-IMAddress");
    aItem->SetNetMeetingAlias(tempS.ucs2());
 
    tempS =   addressee.custom( "KADDRESSBOOK", "X-SpousesName" );
    aItem->SetSpouse(tempS.ucs2());

    tempS = addressee.custom( "KADDRESSBOOK", "X-ManagersName" );
    aItem->SetManagerName(tempS.ucs2());

    tempS = addressee.custom( "KADDRESSBOOK", "X-AssistantsName" );
    aItem->SetAssistantName(tempS.ucs2());

    tempS =  addressee.custom( "KADDRESSBOOK", "X-Department" );
    aItem->SetDepartment(tempS.ucs2());

    tempS = addressee.custom( "KADDRESSBOOK", "X-Office" );
    aItem->SetOfficeLocation(tempS.ucs2());

    tempS = addressee.custom( "KADDRESSBOOK", "X-Profession");
    aItem->SetProfession(tempS.ucs2());

    tempS = addressee.custom( "KADDRESSBOOK", "X-Anniversary");
    if ( !tempS.isEmpty() ){
        QDateTime dt = QDateTime ( KGlobal::locale()->readDate( tempS,"%Y-%m-%d") );
        aItem->SetAnniversary(Qdt2date( dt ));
    } else {
        aItem->SetAnniversary(Qdt2date(QDateTime(QDate( 4501,1,1 ))));
    }
    int sec =  (int)addressee.secrecy().type();
    if ( sec > 0 )// mapping pers -> private
        ++sec;
    aItem->SetSensitivity( sec ) ;
    //addressee.Logo( const Picture &logo );
    //addressee.Photo( const Picture &photo );
    //addressee.Sound( const Sound &sound );
    //addressee.Agent( const Agent &agent );
 
    QString cat = addressee.categories().join("; ");
    aItem->SetCategories( cat.ucs2());


    KABC::PhoneNumber::List phoneNumbers;
    KABC::PhoneNumber::List::Iterator phoneIter;
    phoneNumbers = addressee.phoneNumbers();
    for ( phoneIter = phoneNumbers.begin(); phoneIter != phoneNumbers.end();
          ++phoneIter ) {

    }
    QString phoneS;

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Work + 
                                          KABC::PhoneNumber::Voice  + 
                                          KABC::PhoneNumber::Msg  );
    aItem->SetAssistantTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Work +
                                          KABC::PhoneNumber::Pref) ;
    aItem->SetBusinessTelephoneNumber(phoneS.ucs2());
 
    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Work ) ;
    aItem->SetBusiness2TelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Work + 
                                          KABC::PhoneNumber::Fax ) ;
    aItem->SetBusinessFaxNumber(phoneS.ucs2());


    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Car ) ;
    aItem->SetCarTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Home +
                                          KABC::PhoneNumber::Pref ) ;
    aItem->SetHomeTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Home ) ;
    aItem->SetHome2TelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Home + 
                                          KABC::PhoneNumber::Fax ) ;
    aItem->SetHomeFaxNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Isdn ) ;
    aItem->SetISDNNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Cell ) ;
    aItem->SetMobileTelephoneNumber(phoneS.ucs2());
  
    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Fax ) ;
    aItem->SetOtherFaxNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Voice ) ;
    aItem->SetOtherTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Pager ) ;
    aItem->SetPagerNumber(phoneS.ucs2());
  
    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Pref ) ;
    aItem->SetPrimaryTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Pcs  +
                                          KABC::PhoneNumber::Voice) ;
    aItem->SetTTYTDDTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Pcs ) ;
    aItem->SetTelexNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Work + 
                                          KABC::PhoneNumber::Msg ) ;
    aItem->SetCompanyMainTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Pcs +
                                          KABC::PhoneNumber::Pref ) ;
    aItem->SetRadioTelephoneNumber(phoneS.ucs2());

    phoneS  = addressee.phoneNumberString(
                                          KABC::PhoneNumber::Msg ) ;
    aItem->SetCallbackTelephoneNumber(phoneS.ucs2());
 
    QStringList emails = addressee.emails();
    emails << "" << "" << "";
    aItem->SetEmail1Address(emails[0].ucs2());
    aItem->SetEmail2Address(emails[1].ucs2());
    aItem->SetEmail3Address(emails[2].ucs2());

    // is this the number of the preferred email?
    //	long GetSelectedMailingAddress();???

    KABC::Address addressHome = addressee.address( KABC::Address::Home );
    KABC::Address* addressAdd = &addressHome;
    aItem->SetHomeAddressCountry(addressAdd->country().ucs2());
    aItem->SetHomeAddressState(addressAdd->region().ucs2());
    aItem->SetHomeAddressCity(addressAdd->locality().ucs2());
    aItem->SetHomeAddressPostalCode(addressAdd->postalCode().ucs2());
    aItem->SetHomeAddressPostOfficeBox(addressAdd->postOfficeBox().ucs2());
    aItem->SetHomeAddressStreet(addressAdd->street().ucs2());

    addressHome = addressee.address( KABC::Address::Work );
    addressAdd = &addressHome;
    aItem->SetBusinessAddressCountry(addressAdd->country().ucs2());
    aItem->SetBusinessAddressState(addressAdd->region().ucs2());
    aItem->SetBusinessAddressCity(addressAdd->locality().ucs2());
    aItem->SetBusinessAddressPostalCode(addressAdd->postalCode().ucs2());
    aItem->SetBusinessAddressPostOfficeBox(addressAdd->postOfficeBox().ucs2());
    aItem->SetBusinessAddressStreet(addressAdd->street().ucs2());

    addressHome = addressee.otherAddress();
    addressAdd = &addressHome;
    aItem->SetOtherAddressCountry(addressAdd->country().ucs2());
    aItem->SetOtherAddressState(addressAdd->region().ucs2());
    aItem->SetOtherAddressCity(addressAdd->locality().ucs2());
    aItem->SetOtherAddressPostalCode(addressAdd->postalCode().ucs2());
    aItem->SetOtherAddressPostOfficeBox(addressAdd->postOfficeBox().ucs2());
    aItem->SetOtherAddressStreet(addressAdd->street().ucs2());

    aItem->SetInternetFreeBusyAddress(addressee.custom( "KADDRESSBOOK", "X-FreeBusyUrl" ).ucs2());
    aItem->SetChildren(addressee.custom( "KADDRESSBOOK", "X-Children").ucs2());

    int gen  = 0;// 0 undef - 1 female - 2 male 
    QString gend = addressee.custom( "KADDRESSBOOK", "X-Gender" );
    if ( gend == "female" )
        gen = 1;
    if ( gend == "male" )
        gen = 2;
    aItem->SetGender(gen);
 
    aItem->Save();
}
KABC::Addressee OL_access::ol2kapiContact( _ContactItem * aItem , bool syncMode)
{
    KABC::Addressee  addressee;
  
    addressee.setUid( QString::fromUcs2(aItem->GetEntryID().GetBuffer()));
    //GetLastModificationTime()
    //addressee.setName( const QString &name );
    addressee.setFormattedName( QString::fromUcs2(aItem->GetFileAs().GetBuffer()) );
    addressee.setFamilyName( QString::fromUcs2(aItem->GetLastName().GetBuffer()) );
    addressee.setGivenName( QString::fromUcs2(aItem->GetFirstName().GetBuffer()) );
    addressee.setAdditionalName( QString::fromUcs2(aItem->GetMiddleName().GetBuffer()) );
    addressee.setPrefix(QString::fromUcs2(aItem->GetTitle().GetBuffer()) );
    addressee.setSuffix( QString::fromUcs2(aItem->GetSuffix().GetBuffer()) );
    addressee.setNickName( QString::fromUcs2(aItem->GetNickName().GetBuffer()) );
    QDateTime dtb = mDdate2Qdtr(aItem->GetBirthday());
    if ( dtb.isValid() )
        addressee.setBirthday( mDdate2Qdtr(aItem->GetBirthday()));

    //QString::fromUcs2(aItem->.GetBuffer())
    //addressee.setMailer( const QString &mailer );
    //addressee.setTimeZone( const TimeZone &timeZone );
    //addressee.setGeo( const Geo &geo );
    //addressee.setTitle( QString::fromUcs2(aItem->GetJobTitle().GetBuffer()) );// titel is the prefix
    addressee.setRole( QString::fromUcs2(aItem->GetJobTitle().GetBuffer()) );
    addressee.setOrganization( QString::fromUcs2(aItem->GetCompanyName().GetBuffer()).replace( QRegExp("\\r"), "") );
    QString notesStr = QString::fromUcs2(aItem->GetBody().GetBuffer());
    notesStr.replace( QRegExp("\\r"), "");
   
    addressee.setProductId( QString::fromUcs2(aItem->GetCustomerID().GetBuffer()) );
    //addressee.setRevision( const QDateTime &revision );
    // addressee.setSortString( const QString &sortString );
    addressee.setUrl( QString::fromUcs2(aItem->GetWebPage().GetBuffer()) );

    QString tempS;
    tempS = QString::fromUcs2(aItem->GetNetMeetingAlias().GetBuffer());//+" AT SERVER: " +QString::fromUcs2(aItem->GetNetMeetingServer().GetBuffer());
    if ( tempS.length() > 12 )
        addressee.insertCustom( "KADDRESSBOOK", "X-IMAddress", tempS );
    tempS = QString::fromUcs2(aItem->GetSpouse().GetBuffer());
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-SpousesName", tempS );
    tempS = QString::fromUcs2(aItem->GetManagerName().GetBuffer());
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-ManagersName", tempS );
    tempS = QString::fromUcs2(aItem->GetAssistantName().GetBuffer());
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-AssistantsName", tempS );
    tempS = QString::fromUcs2(aItem->GetDepartment().GetBuffer());
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-Department", tempS );
    tempS = QString::fromUcs2(aItem->GetOfficeLocation().GetBuffer()).replace( QRegExp("\\r"), "");
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-Office",tempS );
    tempS = QString::fromUcs2(aItem->GetProfession().GetBuffer());
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-Profession", tempS );
    dtb =  mDdate2Qdtr(aItem->GetAnniversary());
    if (dtb.isValid() ) {
        QString dt = KGlobal::locale()->formatDate( dtb.date() , true, KLocale::ISODate);
        addressee.insertCustom( "KADDRESSBOOK", "X-Anniversary", dt);
    }
    int sec =   aItem->GetSensitivity() ;
    if ( sec > 1 )// mapping pers -> private
        --sec;
    addressee.setSecrecy( sec );
    //addressee.setLogo( const Picture &logo );
    //addressee.setPhoto( const Picture &photo );
    //addressee.setSound( const Sound &sound );
    //addressee.setAgent( const Agent &agent );
    QString cat = QString::fromUcs2( aItem->GetCategories().GetBuffer()).replace( QRegExp("\\r"), "");
    cat = cat.replace( QRegExp("; "), ";");
    addressee.setCategories( QStringList::split( ";", cat ));
  
    QString phoneS;

    phoneS = QString::fromUcs2( aItem->GetAssistantTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Work + 
                                                       KABC::PhoneNumber::Voice  + 
                                                       KABC::PhoneNumber::Msg  ) );
    phoneS = QString::fromUcs2( aItem->GetBusinessTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Work +
                                                       KABC::PhoneNumber::Pref) );
    phoneS = QString::fromUcs2( aItem->GetBusiness2TelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Work ) );
    phoneS = QString::fromUcs2( aItem->GetBusinessFaxNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Work + 
                                                       KABC::PhoneNumber::Fax ) );
    phoneS = QString::fromUcs2( aItem->GetCarTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Car ) );
    phoneS = QString::fromUcs2( aItem->GetHomeTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Home +
                                                       KABC::PhoneNumber::Pref ) );
    phoneS = QString::fromUcs2( aItem->GetHome2TelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Home ) );
    phoneS = QString::fromUcs2( aItem->GetHomeFaxNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Home + 
                                                       KABC::PhoneNumber::Fax ) );
    phoneS = QString::fromUcs2( aItem->GetISDNNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Isdn ) );
    phoneS = QString::fromUcs2( aItem->GetMobileTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Cell ) );
    phoneS = QString::fromUcs2( aItem->GetOtherFaxNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Fax ) );
    phoneS = QString::fromUcs2( aItem->GetOtherTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Voice ) );
    phoneS = QString::fromUcs2( aItem->GetPagerNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Pager ) );
    phoneS = QString::fromUcs2( aItem->GetPrimaryTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Pref ) );
    phoneS = QString::fromUcs2( aItem->GetTTYTDDTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Pcs  +
                                                       KABC::PhoneNumber::Voice) );
    phoneS = QString::fromUcs2( aItem->GetTelexNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Pcs ) );
    phoneS = QString::fromUcs2( aItem->GetCompanyMainTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Work + 
                                                       KABC::PhoneNumber::Msg ) );
    phoneS = QString::fromUcs2( aItem->GetRadioTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Pcs +
                                                       KABC::PhoneNumber::Pref ) );
    phoneS = QString::fromUcs2( aItem->GetCallbackTelephoneNumber().GetBuffer());
    if ( ! phoneS.isEmpty()) 
        addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                       KABC::PhoneNumber::Msg ) );
 
    bool preferred = true;
    phoneS = QString::fromUcs2( aItem->GetEmail1Address().GetBuffer());
    if ( ! phoneS.isEmpty()) {
        addressee.insertEmail(phoneS  , preferred );
        preferred = false;
    }
    phoneS = QString::fromUcs2( aItem->GetEmail2Address().GetBuffer());
    if ( ! phoneS.isEmpty()) {
        addressee.insertEmail(phoneS  , preferred );
        preferred = false;
    }
    phoneS = QString::fromUcs2( aItem->GetEmail3Address().GetBuffer());
    if ( ! phoneS.isEmpty()) {
        addressee.insertEmail(phoneS  , preferred );
        preferred = false;
    }
    // is this the number of the preferred email?
    //	long GetSelectedMailingAddress();???
 
    KABC::Address addressHome;
    KABC::Address* addressAdd = &addressHome;
    bool insert = false;
    phoneS = QString::fromUcs2( aItem->GetHomeAddressCountry().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setCountry(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetHomeAddressState().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setRegion(phoneS   );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetHomeAddressCity().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setLocality(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetHomeAddressPostalCode().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostalCode(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetHomeAddressPostOfficeBox().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostOfficeBox(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetHomeAddressStreet().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setStreet(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetHomeAddress().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        // redundant !addressAdd->setExtended(phoneS  );
        // insert = true;
    }
    addressAdd->setType( KABC::Address::Home );
    if ( insert )
        addressee.insertAddress( *addressAdd );
    // ++++++++++++++++++++++ end of address

    KABC::Address addressWork;
    addressAdd = &addressWork;
    insert = false;
    phoneS = QString::fromUcs2( aItem->GetBusinessAddressCountry().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setCountry(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetBusinessAddressState().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setRegion(phoneS   );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetBusinessAddressCity().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setLocality(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetBusinessAddressPostalCode().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostalCode(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetBusinessAddressPostOfficeBox().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostOfficeBox(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetBusinessAddressStreet().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setStreet(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetBusinessAddress().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        // redundant !addressAdd->setExtended(phoneS  );
        // insert = true;
    }
    addressAdd->setType( KABC::Address::Work );
    if ( insert )
        addressee.insertAddress( *addressAdd );
    // ++++++++++++++++++++++ end of address

    KABC::Address addressOther;
    addressAdd = &addressOther;
    insert = false;
    phoneS = QString::fromUcs2( aItem->GetOtherAddressCountry().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setCountry(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetOtherAddressState().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setRegion(phoneS   );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetOtherAddressCity().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setLocality(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetOtherAddressPostalCode().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostalCode(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetOtherAddressPostOfficeBox().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostOfficeBox(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetOtherAddressStreet().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setStreet(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetOtherAddress().GetBuffer());
    if ( ! phoneS.isEmpty()) {
        // redundant !addressAdd->setExtended(phoneS  );
        //insert = true;
    }
    //addressAdd->setId(  );
    if ( insert )
        addressee.insertAddress( *addressAdd );
    // ++++++++++++++++++++++ end of address
#if 0
    KABC::Address addressMail;
    addressAdd = &addressMail;
    insert = false;
    phoneS = QString::fromUcs2( aItem->GetMailingAddressCountry().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setCountry(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetMailingAddressState().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setRegion(phoneS   );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetMailingAddressCity().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setLocality(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetMailingAddressPostalCode().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostalCode(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetMailingAddressPostOfficeBox().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setPostOfficeBox(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetMailingAddressStreet().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        addressAdd->setStreet(phoneS  );
        insert = true;
    }
    phoneS = QString::fromUcs2( aItem->GetMailingAddress().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! phoneS.isEmpty()) {
        // redundant ! addressAdd->setExtended(phoneS  );
        // insert = true;
    }
    addressAdd->setType( KABC::Address::Postal ); 
    if ( insert ) {
        addressee.insertAddress( *addressAdd );
    }
#endif
    // the following code is disabled
    // it does not seem to be useful
#if 0
    if ( insert ) {
        addressAdd->setType( KABC::Address::Home );
        if ( addressMail == addressHome ) {
            addressHome.setType( KABC::Address::Postal+ KABC::Address::Home );
            addressee.insertAddress( addressHome );
        } else { 
            addressAdd->setType( KABC::Address::Work );
            if ( addressMail == addressWork ){
                addressWork.setType( KABC::Address::Postal+ KABC::Address::Work );
                addressee.insertAddress( addressWork );

            } else {
                addressAdd->setType( 0 );
                if ( addressOther == addressMail ){
                    addressOther.setType( KABC::Address::Postal );
                    addressee.insertAddress( addressOther );
                } else {
                    addressee.insertAddress( *addressAdd );
                }
            }
        }
    }
#endif
    // ++++++++++++++++++++++ end of ALL addresses
    //GetUserProperties();
    tempS = QString::fromUcs2(aItem->GetInternetFreeBusyAddress().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-FreeBusyUrl", tempS );
    tempS = QString::fromUcs2(aItem->GetChildren().GetBuffer());
    if ( !tempS.isEmpty() )
        addressee.insertCustom( "KADDRESSBOOK", "X-Children", tempS );
    int gen  = aItem->GetGender();
    if ( gen != 0  ) { // 0 undef - 1 female - 2 male 
        if ( gen == 1 )
            addressee.insertCustom( "KADDRESSBOOK", "X-Gender", "female" );
        else
            addressee.insertCustom( "KADDRESSBOOK", "X-Gender", "male" );
    }
    QString additionalInfo;
    QString tempAdd;
    tempAdd = QString::fromUcs2(aItem->GetLanguage().GetBuffer());
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nLanguage: ");
        additionalInfo += tempAdd;
    }
    tempAdd = QString::fromUcs2(aItem->GetHobby().GetBuffer()); 
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nHobby: ");
        additionalInfo += tempAdd;;
    }
    tempAdd =QString::fromUcs2(aItem->GetPersonalHomePage().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nHomepage: ");
        additionalInfo += tempAdd;;
    }
    tempAdd = QString::fromUcs2(aItem->GetBillingInformation().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nBilling information: ");
        additionalInfo += tempAdd;;
    }
    tempAdd = QString::fromUcs2(aItem->GetCustomerID().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nCustomer ID: ");
        additionalInfo += tempAdd;;
    }
    tempAdd = QString::fromUcs2(aItem->GetUser1().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nUser1: ");
        additionalInfo += tempAdd;;
    }
    tempAdd = QString::fromUcs2(aItem->GetUser2().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nUser2: ");
        additionalInfo += tempAdd;;
    }
    tempAdd = QString::fromUcs2(aItem->GetUser3().GetBuffer());
    phoneS.replace( QRegExp("\\r"), "");
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nUser3: ");
        additionalInfo += tempAdd;;
    }
    tempAdd = QString::fromUcs2(aItem->GetUser4().GetBuffer());
    phoneS.replace( QRegExp("\\r"), ""); 
    if ( ! tempAdd.isEmpty() ) {
        additionalInfo += i18n("\nUser4: ");
        additionalInfo += tempAdd;;
    }
    if (!additionalInfo.isEmpty() && ! syncMode )  {
        tempAdd = notesStr;
        notesStr = "+++++++++++++++++++++++++++\n";
        notesStr += i18n("Additonal fields created\nby KA/Pi Outlook import:");
        notesStr += additionalInfo;
        notesStr += i18n("\nEnd additonal fields created\nby KA/Pi Outlook import!\n");
        notesStr += "+++++++++++++++++++++++++++\n";
        notesStr += tempAdd;
    }
    addressee.setNote( notesStr );
#if 0  
    // pending
    - IM address: no clue where to get info about the helper ID
        -custom fields: difficult to implement - not implemented
        -keys: makes no sense
#endif
        return addressee;
}
#if 0


QDateTime mDdate2Qdtr( DATE dt)
{
    COleDateTime odt;
    SYSTEMTIME st;
    odt = dt;
    if ( odt.GetStatus() != 0 )
      return QDateTime();
    odt.GetAsSystemTime(st);
    if ( st.wYear > 4000  ) // this program as a year 4000 bug!
      return QDateTime();
    // it seems so, that 1.1.4501 indicates: DATE invalid
    QDateTime qdt (QDate(st.wYear, st.wMonth,st.wDay ),QTime( st.wHour, st.wMinute,st.wSecond ) );
    return qdt;
}
DATE Qdt2date( QDateTime dt )
{
    DATE ddd;
    COleDateTime odt;
    odt.SetDateTime(dt.date().year(), dt.date().month(),dt.date().day(),
                    dt.time().hour(), dt.time().minute(), dt.time().second() );
    ddd = odt;
    return ddd;
}

class OLEListViewItem : public QCheckListItem
{
  public:
    OLEListViewItem( QListView *parent, QString text ) :
      QCheckListItem( parent, text, QCheckListItem::CheckBox ) { ; };
    OLEListViewItem(  QListViewItem *after, QString text ) :
      QCheckListItem( after, text, QCheckListItem::CheckBox ) { ; };
    ~OLEListViewItem() {};
  void setData( DWORD data ) {mData= data; };
    void setDataID( QString data ){ mDataID = data ;}
    QString dataID() { return mDataID;}
  DWORD data() { return mData ;};
  private:
     DWORD  mData;
     QString mDataID;
}; 
bool KAImportOLdialog::sOLDispatch = false;

KAImportOLdialog::KAImportOLdialog( const QString &caption, 
                                      KABC::AddressBook * aBook, QWidget *parent ) :
    KDialogBase( Plain, caption, User1 | Close, Ok,
               parent, caption, true, false, i18n("Import!") )
{
  mSyncMode = false;
  QHBox * mw = new  QHBox( this );
  setMainWidget( mw );
  mListView = new QListView( mw );
  mListView->addColumn(i18n("Select Folder"));
  mListView->addColumn(i18n("Content Type"));
  mABook = aBook;
  connect( this, SIGNAL( user1Clicked() ),SLOT ( slotApply()));
  setupFolderView();
  resize( sizeHint().width()+50,  sizeHint().height()+50 );
}

KAImportOLdialog::~KAImportOLdialog()
{
   
}

void KAImportOLdialog::setSyncMode()
{
    mSyncMode = true;
    findButton( User1 )->setText( i18n("Synchronize!") );
}
KABC::Addressee::List KAImportOLdialog::getAddressList()
{
  return mAList;
}
void KAImportOLdialog::setupFolderView()
{
  SCODE sc = ::OleInitialize(NULL);
  if ( FAILED ( sc ) ) {
    KMessageBox::information(this,"OLE initialisation failed");
    return;
  }
  if( ! KAImportOLdialog::sOLDispatch ) {
      if(!gOlAppAB.CreateDispatch(_T("Outlook.Application"),NULL)){
          KMessageBox::information(this,"Sorry, cannot access Outlook");
          return ;
      }
      KAImportOLdialog::sOLDispatch = true;
  }
  MAPIFolder mfInbox;
  MAPIFolder mfRoot;
  CString szName;
  _NameSpace olNS; 
  olNS = gOlAppAB.GetNamespace(_T("MAPI"));
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


void KAImportOLdialog::addFolder(OLEListViewItem* iParent, LPDISPATCH dispParent)
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
      hChild->setDataID( QString::fromUcs2(mfChild.GetEntryID().GetBuffer()) );
      if ( iType != 2)
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

void KAImportOLdialog::slotApply()
{
  importedItems = 0;
  OLEListViewItem* child = (OLEListViewItem*) mListView->firstChild();
  while ( child ) {
      if ( child->isOn() ) {
          readContactData( child->data() );
          if ( mSyncMode ) {
              mSelectedFolderID = child->dataID();
              accept();
              return;
          }
      }
    child = (OLEListViewItem*) child->itemBelow();
  }
  QString mes = i18n("Importing complete.\n\n%1 items imported.").arg( importedItems);
  KMessageBox::information(this,mes);
}
void KAImportOLdialog::readContactData( DWORD folder )
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
  QString cap = i18n("Importing contact data");
  if (  mSyncMode ) {
      cap = i18n("Reading contact data...");
  }
  QProgressDialog bar( cap,i18n("Abort"), folderItems.GetCount(),this);
  bar.setCaption (i18n("Accessing OL") );
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
      indx = (long)i;
      itm = folderItems.Item(indx.Detach());
      _ContactItem * pItem = (_ContactItem *)&itm;
      if ( addAddressee( ol2kapiContact( pItem, mSyncMode ) ))
          ++importedItems;
      itm->Release();
    }
}
void KAImportOLdialog::slotOk()
{
   QDialog::accept();
}

KABC::Addressee KAImportOLdialog::ol2kapiContact( _ContactItem * aItem , bool syncMode)
{
  KABC::Addressee  addressee;
  
  addressee.setUid( QString::fromUcs2(aItem->GetEntryID().GetBuffer()));
  //GetLastModificationTime()
  //addressee.setName( const QString &name );
  addressee.setFormattedName( QString::fromUcs2(aItem->GetFileAs().GetBuffer()) );
  addressee.setFamilyName( QString::fromUcs2(aItem->GetLastName().GetBuffer()) );
  addressee.setGivenName( QString::fromUcs2(aItem->GetFirstName().GetBuffer()) );
  addressee.setAdditionalName( QString::fromUcs2(aItem->GetMiddleName().GetBuffer()) );
  addressee.setPrefix(QString::fromUcs2(aItem->GetTitle().GetBuffer()) );
  addressee.setSuffix( QString::fromUcs2(aItem->GetSuffix().GetBuffer()) );
  addressee.setNickName( QString::fromUcs2(aItem->GetNickName().GetBuffer()) );
  QDateTime dtb = mDdate2Qdtr(aItem->GetBirthday());
  if ( dtb.isValid() )
    addressee.setBirthday( mDdate2Qdtr(aItem->GetBirthday()));

  //QString::fromUcs2(aItem->.GetBuffer())
  //addressee.setMailer( const QString &mailer );
  //addressee.setTimeZone( const TimeZone &timeZone );
  //addressee.setGeo( const Geo &geo );
  //addressee.setTitle( QString::fromUcs2(aItem->GetJobTitle().GetBuffer()) );// titel is the prefix
  addressee.setRole( QString::fromUcs2(aItem->GetJobTitle().GetBuffer()) );
  addressee.setOrganization( QString::fromUcs2(aItem->GetCompanyName().GetBuffer()).replace( QRegExp("\\r"), "") );
  QString notesStr = QString::fromUcs2(aItem->GetBody().GetBuffer());
  notesStr.replace( QRegExp("\\r"), "");
   
  addressee.setProductId( QString::fromUcs2(aItem->GetCustomerID().GetBuffer()) );
  //addressee.setRevision( const QDateTime &revision );
  // addressee.setSortString( const QString &sortString );
  addressee.setUrl( QString::fromUcs2(aItem->GetWebPage().GetBuffer()) );

  QString tempS;
  tempS = QString::fromUcs2(aItem->GetNetMeetingAlias().GetBuffer());//+" AT SERVER: " +QString::fromUcs2(aItem->GetNetMeetingServer().GetBuffer());
  if ( tempS.length() > 12 )
    addressee.insertCustom( "KADDRESSBOOK", "X-IMAddress", tempS );
  tempS = QString::fromUcs2(aItem->GetSpouse().GetBuffer());
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-SpousesName", tempS );
  tempS = QString::fromUcs2(aItem->GetManagerName().GetBuffer());
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-ManagersName", tempS );
  tempS = QString::fromUcs2(aItem->GetAssistantName().GetBuffer());
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-AssistantsName", tempS );
  tempS = QString::fromUcs2(aItem->GetDepartment().GetBuffer());
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-Department", tempS );
  tempS = QString::fromUcs2(aItem->GetOfficeLocation().GetBuffer()).replace( QRegExp("\\r"), "");
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-Office",tempS );
  tempS = QString::fromUcs2(aItem->GetProfession().GetBuffer());
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-Profession", tempS );
  dtb =  mDdate2Qdtr(aItem->GetAnniversary());
  if (dtb.isValid() ) {
    QString dt = KGlobal::locale()->formatDate( dtb.date() , true, KLocale::ISODate);
    addressee.insertCustom( "KADDRESSBOOK", "X-Anniversary", dt);
  }
  int sec =   aItem->GetSensitivity() ;
  if ( sec > 1 )// mapping pers -> private
    --sec;
  addressee.setSecrecy( sec );
  //addressee.setLogo( const Picture &logo );
  //addressee.setPhoto( const Picture &photo );
  //addressee.setSound( const Sound &sound );
  //addressee.setAgent( const Agent &agent );
  QString cat = QString::fromUcs2( aItem->GetCategories().GetBuffer()).replace( QRegExp("\\r"), "");
  cat = cat.replace( QRegExp("; "), ";");
  addressee.setCategories( QStringList::split( ";", cat ));
  
  QString phoneS;

  phoneS = QString::fromUcs2( aItem->GetAssistantTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Work + 
                                                   KABC::PhoneNumber::Voice  + 
                                                   KABC::PhoneNumber::Msg  ) );
  phoneS = QString::fromUcs2( aItem->GetBusinessTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Work +
                                                   KABC::PhoneNumber::Pref) );
  phoneS = QString::fromUcs2( aItem->GetBusiness2TelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Work ) );
  phoneS = QString::fromUcs2( aItem->GetBusinessFaxNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Work + 
                                                   KABC::PhoneNumber::Fax ) );
  phoneS = QString::fromUcs2( aItem->GetCarTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Car ) );
  phoneS = QString::fromUcs2( aItem->GetHomeTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Home +
                                                   KABC::PhoneNumber::Pref ) );
  phoneS = QString::fromUcs2( aItem->GetHome2TelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Home ) );
  phoneS = QString::fromUcs2( aItem->GetHomeFaxNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Home + 
                                                   KABC::PhoneNumber::Fax ) );
  phoneS = QString::fromUcs2( aItem->GetISDNNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Isdn ) );
  phoneS = QString::fromUcs2( aItem->GetMobileTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Cell ) );
  phoneS = QString::fromUcs2( aItem->GetOtherFaxNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Fax ) );
  phoneS = QString::fromUcs2( aItem->GetOtherTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Voice ) );
  phoneS = QString::fromUcs2( aItem->GetPagerNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Pager ) );
  phoneS = QString::fromUcs2( aItem->GetPrimaryTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Pref ) );
  phoneS = QString::fromUcs2( aItem->GetTTYTDDTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Pcs  +
                                                   KABC::PhoneNumber::Voice) );
  phoneS = QString::fromUcs2( aItem->GetTelexNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Pcs ) );
  phoneS = QString::fromUcs2( aItem->GetCompanyMainTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Work + 
                                                   KABC::PhoneNumber::Msg ) );
  phoneS = QString::fromUcs2( aItem->GetRadioTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Pcs +
                                                   KABC::PhoneNumber::Pref ) );
  phoneS = QString::fromUcs2( aItem->GetCallbackTelephoneNumber().GetBuffer());
  if ( ! phoneS.isEmpty()) 
    addressee.insertPhoneNumber( KABC::PhoneNumber(phoneS ,
                                                   KABC::PhoneNumber::Msg ) );
 
  bool preferred = true;
  phoneS = QString::fromUcs2( aItem->GetEmail1Address().GetBuffer());
  if ( ! phoneS.isEmpty()) {
    addressee.insertEmail(phoneS  , preferred );
    preferred = false;
  }
  phoneS = QString::fromUcs2( aItem->GetEmail2Address().GetBuffer());
  if ( ! phoneS.isEmpty()) {
    addressee.insertEmail(phoneS  , preferred );
    preferred = false;
  }
  phoneS = QString::fromUcs2( aItem->GetEmail3Address().GetBuffer());
  if ( ! phoneS.isEmpty()) {
    addressee.insertEmail(phoneS  , preferred );
    preferred = false;
  }
  // is this the number of the preferred email?
  //	long GetSelectedMailingAddress();???
 
  KABC::Address addressHome;
  KABC::Address* addressAdd = &addressHome;
  bool insert = false;
  phoneS = QString::fromUcs2( aItem->GetHomeAddressCountry().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setCountry(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetHomeAddressState().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setRegion(phoneS   );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetHomeAddressCity().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setLocality(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetHomeAddressPostalCode().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostalCode(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetHomeAddressPostOfficeBox().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostOfficeBox(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetHomeAddressStreet().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setStreet(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetHomeAddress().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    // redundant !addressAdd->setExtended(phoneS  );
    // insert = true;
  }
  addressAdd->setType( KABC::Address::Home );
  if ( insert )
    addressee.insertAddress( *addressAdd );
  // ++++++++++++++++++++++ end of address

  KABC::Address addressWork;
  addressAdd = &addressWork;
  insert = false;
  phoneS = QString::fromUcs2( aItem->GetBusinessAddressCountry().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setCountry(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetBusinessAddressState().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setRegion(phoneS   );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetBusinessAddressCity().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setLocality(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetBusinessAddressPostalCode().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostalCode(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetBusinessAddressPostOfficeBox().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostOfficeBox(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetBusinessAddressStreet().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setStreet(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetBusinessAddress().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    // redundant !addressAdd->setExtended(phoneS  );
    // insert = true;
  }
  addressAdd->setType( KABC::Address::Work );
  if ( insert )
    addressee.insertAddress( *addressAdd );
  // ++++++++++++++++++++++ end of address

 KABC::Address addressOther;
  addressAdd = &addressOther;
  insert = false;
  phoneS = QString::fromUcs2( aItem->GetOtherAddressCountry().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setCountry(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetOtherAddressState().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setRegion(phoneS   );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetOtherAddressCity().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setLocality(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetOtherAddressPostalCode().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostalCode(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetOtherAddressPostOfficeBox().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostOfficeBox(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetOtherAddressStreet().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setStreet(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetOtherAddress().GetBuffer());
  if ( ! phoneS.isEmpty()) {
    // redundant !addressAdd->setExtended(phoneS  );
    //insert = true;
  }
  //addressAdd->setId(  );
  if ( insert )
    addressee.insertAddress( *addressAdd );
  // ++++++++++++++++++++++ end of address
#if 0
 KABC::Address addressMail;
  addressAdd = &addressMail;
  insert = false;
  phoneS = QString::fromUcs2( aItem->GetMailingAddressCountry().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setCountry(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetMailingAddressState().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setRegion(phoneS   );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetMailingAddressCity().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setLocality(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetMailingAddressPostalCode().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostalCode(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetMailingAddressPostOfficeBox().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setPostOfficeBox(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetMailingAddressStreet().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    addressAdd->setStreet(phoneS  );
    insert = true;
  }
  phoneS = QString::fromUcs2( aItem->GetMailingAddress().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! phoneS.isEmpty()) {
    // redundant ! addressAdd->setExtended(phoneS  );
    // insert = true;
  }
  addressAdd->setType( KABC::Address::Postal ); 
  if ( insert ) {
    addressee.insertAddress( *addressAdd );
  }
#endif
  // the following code is disabled
  // it does not seem to be useful
#if 0
  if ( insert ) {
    addressAdd->setType( KABC::Address::Home );
    if ( addressMail == addressHome ) {
      addressHome.setType( KABC::Address::Postal+ KABC::Address::Home );
      addressee.insertAddress( addressHome );
    } else { 
      addressAdd->setType( KABC::Address::Work );
      if ( addressMail == addressWork ){
	addressWork.setType( KABC::Address::Postal+ KABC::Address::Work );
	addressee.insertAddress( addressWork );

      } else {
	addressAdd->setType( 0 );
	if ( addressOther == addressMail ){
	  addressOther.setType( KABC::Address::Postal );
	  addressee.insertAddress( addressOther );
	} else {
	  addressee.insertAddress( *addressAdd );
	}
      }
    }
  }
#endif
  // ++++++++++++++++++++++ end of ALL addresses
  //GetUserProperties();
  tempS = QString::fromUcs2(aItem->GetInternetFreeBusyAddress().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-FreeBusyUrl", tempS );
  tempS = QString::fromUcs2(aItem->GetChildren().GetBuffer());
  if ( !tempS.isEmpty() )
    addressee.insertCustom( "KADDRESSBOOK", "X-Children", tempS );
  int gen  = aItem->GetGender();
  if ( gen != 0  ) { // 0 undef - 1 female - 2 male 
    if ( gen == 1 )
      addressee.insertCustom( "KADDRESSBOOK", "X-Gender", "female" );
    else
      addressee.insertCustom( "KADDRESSBOOK", "X-Gender", "male" );
  }
  QString additionalInfo;
  QString tempAdd;
  tempAdd = QString::fromUcs2(aItem->GetLanguage().GetBuffer());
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nLanguage: ");
    additionalInfo += tempAdd;
  }
  tempAdd = QString::fromUcs2(aItem->GetHobby().GetBuffer()); 
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nHobby: ");
    additionalInfo += tempAdd;;
  }
  tempAdd =QString::fromUcs2(aItem->GetPersonalHomePage().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nHomepage: ");
    additionalInfo += tempAdd;;
  }
  tempAdd = QString::fromUcs2(aItem->GetBillingInformation().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nBilling information: ");
    additionalInfo += tempAdd;;
  }
  tempAdd = QString::fromUcs2(aItem->GetCustomerID().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nCustomer ID: ");
    additionalInfo += tempAdd;;
  }
  tempAdd = QString::fromUcs2(aItem->GetUser1().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nUser1: ");
    additionalInfo += tempAdd;;
  }
  tempAdd = QString::fromUcs2(aItem->GetUser2().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nUser2: ");
    additionalInfo += tempAdd;;
  }
  tempAdd = QString::fromUcs2(aItem->GetUser3().GetBuffer());
  phoneS.replace( QRegExp("\\r"), "");
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nUser3: ");
    additionalInfo += tempAdd;;
  }
  tempAdd = QString::fromUcs2(aItem->GetUser4().GetBuffer());
  phoneS.replace( QRegExp("\\r"), ""); 
  if ( ! tempAdd.isEmpty() ) {
    additionalInfo += i18n("\nUser4: ");
    additionalInfo += tempAdd;;
  }
  if (!additionalInfo.isEmpty() && ! syncMode )  {
    tempAdd = notesStr;
    notesStr = "+++++++++++++++++++++++++++\n";
    notesStr += i18n("Additonal fields created\nby KA/Pi Outlook import:");
    notesStr += additionalInfo;
    notesStr += i18n("\nEnd additonal fields created\nby KA/Pi Outlook import!\n");
    notesStr += "+++++++++++++++++++++++++++\n";
    notesStr += tempAdd;
  }
  addressee.setNote( notesStr );
#if 0  
  // pending
  - IM address: no clue where to get info about the helper ID
    -custom fields: difficult to implement - not implemented
    -keys: makes no sense
#endif
      return addressee;
}
void KAImportOLdialog::slotCancel()
{
  reject();
}

bool KAImportOLdialog::addAddressee( KABC::Addressee a )
{
  bool add = true;
  KABC::Addressee::List::Iterator it;
    for ( it = mAList.begin(); it != mAList.end(); ++it ) {
      if ( (*it).uid() == a.uid() ) {
	add = false;
	break;
      }
    }
    if ( add ) {
      if ( mABook->findByUid(a.uid() ).isEmpty())
	mAList.append ( a );
      else 
	add = false;
    }
    return add;
}
// *****************************************************************
// ***************************************************************** 
KAwritebackOL::KAwritebackOL( const QString &olFolderID ): QObject()
{
    mSelectedFolderID = olFolderID;
}


KAwritebackOL::~KAwritebackOL()
{

}

bool KAwritebackOL::init()
{
    mSelectedFolder = getFolderFromID( 0 );
    if ( mSelectedFolder == 0 ) {
            KMessageBox::information(0,"KAwritebackOL: No contact folder found");
            return false;
    }
    LPDISPATCH dispItem = mSelectedFolder;
    dispItem->AddRef();
    MAPIFolder mf(dispItem);
    mf.m_lpDispatch->AddRef();
    mFolderItems = mf.GetItems(); 
    return true;
}
void KAwritebackOL::deleteAddressee( KABC::Addressee a )
{
    LPDISPATCH itm = getOLitemFromUid ( a.originalExternalUID() );
    _ContactItem* cItem = (_ContactItem *)&itm;
    if ( cItem != 0 ) {
        //KMessageBox::information(0,"delete: item found " +  a.formattedName() + QString::fromUcs2(cItem->GetLastName().GetBuffer()));
        cItem->Delete();
    }
}
KABC::Addressee KAwritebackOL::changeAddressee( KABC::Addressee a )
{

    LPDISPATCH itm = getOLitemFromUid ( a.originalExternalUID() );
    _ContactItem* cItem = (_ContactItem *)&itm;
    if ( cItem != 0 ) {
        //KMessageBox::information(0,"Change: item found " +  a.formattedName());
        writeData2OLitem( a, cItem );
        KABC::Addressee retval = KAImportOLdialog::ol2kapiContact( cItem , true );
        retval.setUid( a.uid() );
        retval.setExternalUID( a.originalExternalUID() ); 
        retval.setOriginalExternalUID( a.originalExternalUID() ); 
        return retval;
    }
    return KABC::Addressee();
}
KABC::Addressee KAwritebackOL::addAddressee( KABC::Addressee a )
{
    QString newOLid;
    LPDISPATCH c_itm = gOlAppAB.CreateItem(2);
    _ContactItem * aItem = (_ContactItem *)&c_itm ;
    writeData2OLitem( a, aItem );
    newOLid = QString::fromUcs2(aItem->GetEntryID().GetBuffer());
    KABC::Addressee retval = KAImportOLdialog::ol2kapiContact( aItem , true );
    retval.setUid( a.uid() );
    retval.setExternalUID( newOLid ); 
    retval.setOriginalExternalUID( newOLid ); 
    return retval;
}
void KAwritebackOL::writeData2OLitem(  KABC::Addressee addressee, _ContactItem * aItem )
{
    // addressee.setUid( QString::fromUcs2(aItem->GetEntryID().GetBuffer()));
    //GetLastModificationTime()
    //addressee.setName( const QString &name );

    aItem->SetFileAs(addressee.formattedName().ucs2());
    aItem->SetLastName(addressee.familyName().ucs2());
    aItem->SetFirstName(addressee.givenName( ).ucs2());
    aItem->SetMiddleName(addressee.additionalName( ).ucs2());
    aItem->SetTitle(addressee.prefix().ucs2());
    aItem->SetSuffix(addressee.suffix( ).ucs2());
    aItem->SetNickName(addressee.nickName( ).ucs2());
    if ( addressee.birthday().isValid() )
        aItem->SetBirthday(Qdt2date(addressee.birthday()));
    else
        aItem->SetBirthday(Qdt2date(QDateTime(QDate( 4501,1,1 ))));
  //).ucs2()(aItem->)
  //addressee.Mailer( const QString &mailer );
  //addressee.TimeZone( const TimeZone &timeZone );
  //addressee.Geo( const Geo &geo );
  //addressee.Title( ).ucs2()(aItem->SetJobTitle()) );// titel is the prefix
  aItem->SetJobTitle(addressee.role( ).ucs2());
  aItem->SetCompanyName(addressee.organization( ).ucs2());
  QString notesStr = addressee.note();
  notesStr.replace( QRegExp("\\n"), "\n\r");
  aItem->SetBody(notesStr.ucs2());
   
  aItem->SetCustomerID(addressee.productId( ).ucs2());
  //addressee.Revision( const QDateTime &revision );
  // addressee.SortString( const QString &sortString );
  aItem->SetWebPage( addressee.url().url().ucs2());
 
  QString tempS;

 
  tempS =  addressee.custom( "KADDRESSBOOK", "X-IMAddress");
  aItem->SetNetMeetingAlias(tempS.ucs2());
 
  tempS =   addressee.custom( "KADDRESSBOOK", "X-SpousesName" );
  aItem->SetSpouse(tempS.ucs2());

  tempS = addressee.custom( "KADDRESSBOOK", "X-ManagersName" );
  aItem->SetManagerName(tempS.ucs2());

  tempS = addressee.custom( "KADDRESSBOOK", "X-AssistantsName" );
  aItem->SetAssistantName(tempS.ucs2());

  tempS =  addressee.custom( "KADDRESSBOOK", "X-Department" );
  aItem->SetDepartment(tempS.ucs2());

  tempS = addressee.custom( "KADDRESSBOOK", "X-Office" );
  aItem->SetOfficeLocation(tempS.ucs2());

  tempS = addressee.custom( "KADDRESSBOOK", "X-Profession");
  aItem->SetProfession(tempS.ucs2());

  tempS = addressee.custom( "KADDRESSBOOK", "X-Anniversary");
  if ( !tempS.isEmpty() ){
      QDateTime dt = QDateTime ( KGlobal::locale()->readDate( tempS,"%Y-%m-%d") );
      aItem->SetAnniversary(Qdt2date( dt ));
  } else {
      aItem->SetAnniversary(Qdt2date(QDateTime(QDate( 4501,1,1 ))));
  }
  int sec =  (int)addressee.secrecy().type();
  if ( sec > 0 )// mapping pers -> private
    ++sec;
  aItem->SetSensitivity( sec ) ;
  //addressee.Logo( const Picture &logo );
  //addressee.Photo( const Picture &photo );
  //addressee.Sound( const Sound &sound );
  //addressee.Agent( const Agent &agent );
 
  QString cat = addressee.categories().join("; ");
  aItem->SetCategories( cat.ucs2());


  KABC::PhoneNumber::List phoneNumbers;
  KABC::PhoneNumber::List::Iterator phoneIter;
  phoneNumbers = addressee.phoneNumbers();
  for ( phoneIter = phoneNumbers.begin(); phoneIter != phoneNumbers.end();
        ++phoneIter ) {

  }
  QString phoneS;

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Work + 
                                        KABC::PhoneNumber::Voice  + 
                                        KABC::PhoneNumber::Msg  );
  aItem->SetAssistantTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Work +
                                        KABC::PhoneNumber::Pref) ;
  aItem->SetBusinessTelephoneNumber(phoneS.ucs2());
 
  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Work ) ;
  aItem->SetBusiness2TelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Work + 
                                        KABC::PhoneNumber::Fax ) ;
  aItem->SetBusinessFaxNumber(phoneS.ucs2());


  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Car ) ;
  aItem->SetCarTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Home +
                                        KABC::PhoneNumber::Pref ) ;
  aItem->SetHomeTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Home ) ;
  aItem->SetHome2TelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Home + 
                                        KABC::PhoneNumber::Fax ) ;
  aItem->SetHomeFaxNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Isdn ) ;
  aItem->SetISDNNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Cell ) ;
  aItem->SetMobileTelephoneNumber(phoneS.ucs2());
  
  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Fax ) ;
  aItem->SetOtherFaxNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Voice ) ;
  aItem->SetOtherTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Pager ) ;
  aItem->SetPagerNumber(phoneS.ucs2());
  
  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Pref ) ;
  aItem->SetPrimaryTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Pcs  +
                                        KABC::PhoneNumber::Voice) ;
  aItem->SetTTYTDDTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Pcs ) ;
  aItem->SetTelexNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Work + 
                                        KABC::PhoneNumber::Msg ) ;
  aItem->SetCompanyMainTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Pcs +
                                        KABC::PhoneNumber::Pref ) ;
  aItem->SetRadioTelephoneNumber(phoneS.ucs2());

  phoneS  = addressee.phoneNumberString(
                                        KABC::PhoneNumber::Msg ) ;
  aItem->SetCallbackTelephoneNumber(phoneS.ucs2());
 
  QStringList emails = addressee.emails();
  emails << "" << "" << "";
  aItem->SetEmail1Address(emails[0].ucs2());
  aItem->SetEmail2Address(emails[1].ucs2());
  aItem->SetEmail3Address(emails[2].ucs2());

  // is this the number of the preferred email?
  //	long GetSelectedMailingAddress();???

  KABC::Address addressHome = addressee.address( KABC::Address::Home );
  KABC::Address* addressAdd = &addressHome;
  aItem->SetHomeAddressCountry(addressAdd->country().ucs2());
  aItem->SetHomeAddressState(addressAdd->region().ucs2());
  aItem->SetHomeAddressCity(addressAdd->locality().ucs2());
  aItem->SetHomeAddressPostalCode(addressAdd->postalCode().ucs2());
  aItem->SetHomeAddressPostOfficeBox(addressAdd->postOfficeBox().ucs2());
  aItem->SetHomeAddressStreet(addressAdd->street().ucs2());

  addressHome = addressee.address( KABC::Address::Work );
  addressAdd = &addressHome;
  aItem->SetBusinessAddressCountry(addressAdd->country().ucs2());
  aItem->SetBusinessAddressState(addressAdd->region().ucs2());
  aItem->SetBusinessAddressCity(addressAdd->locality().ucs2());
  aItem->SetBusinessAddressPostalCode(addressAdd->postalCode().ucs2());
  aItem->SetBusinessAddressPostOfficeBox(addressAdd->postOfficeBox().ucs2());
  aItem->SetBusinessAddressStreet(addressAdd->street().ucs2());

  addressHome = addressee.otherAddress();
  addressAdd = &addressHome;
  aItem->SetOtherAddressCountry(addressAdd->country().ucs2());
  aItem->SetOtherAddressState(addressAdd->region().ucs2());
  aItem->SetOtherAddressCity(addressAdd->locality().ucs2());
  aItem->SetOtherAddressPostalCode(addressAdd->postalCode().ucs2());
  aItem->SetOtherAddressPostOfficeBox(addressAdd->postOfficeBox().ucs2());
  aItem->SetOtherAddressStreet(addressAdd->street().ucs2());

  aItem->SetInternetFreeBusyAddress(addressee.custom( "KADDRESSBOOK", "X-FreeBusyUrl" ).ucs2());
  aItem->SetChildren(addressee.custom( "KADDRESSBOOK", "X-Children").ucs2());

  int gen  = 0;// 0 undef - 1 female - 2 male 
  QString gend = addressee.custom( "KADDRESSBOOK", "X-Gender" );
  if ( gend == "female" )
      gen = 1;
  if ( gend == "male" )
      gen = 2;
  aItem->SetGender(gen);
 
  aItem->Save();
}
LPDISPATCH KAwritebackOL::getFolderFromID( LPDISPATCH parentFolder )
{
    if ( parentFolder == 0 ) {
        SCODE sc = ::OleInitialize(NULL);
        if ( FAILED ( sc ) ) {
            KMessageBox::information(0,"OLE initialisation failed");
            return 0;
        }


        if( ! KAImportOLdialog::sOLDispatch ) {
            if(!gOlAppAB.CreateDispatch(_T("Outlook.Application"),NULL)){
                KMessageBox::information(0,"Sorry, cannot access Outlook");
                return 0;
            }
            KAImportOLdialog::sOLDispatch = true;
        }

        MAPIFolder mfInbox;
        MAPIFolder mfRoot;
        CString szName;
        _NameSpace olNS; 
        olNS = gOlAppAB.GetNamespace(_T("MAPI"));
        mfInbox = olNS.GetDefaultFolder(6);
        mfRoot = mfInbox.GetParent();
        szName = mfRoot.GetName();
        mfRoot.m_lpDispatch->AddRef();
        return getFolderFromID( mfRoot.m_lpDispatch );
    }

    MAPIFolder mfParent(parentFolder), mfChild;
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
            if (mSelectedFolderID ==  QString::fromUcs2(mfChild.GetEntryID().GetBuffer()) ) {
                return mfChild.m_lpDispatch;
            }
            LPDISPATCH resultFolder = getFolderFromID(mfChild.m_lpDispatch );
            if ( resultFolder != 0 )
                return resultFolder;
        }
    return 0;
}
LPDISPATCH KAwritebackOL::getOLitemFromUid( QString uid )
{
  _variant_t indx((long)0);
  LPDISPATCH itm;
  int i;
  for(i=1; i <= mFolderItems.GetCount(); ++i)
    {
      qApp->processEvents();
      indx = (long)i;
      itm = mFolderItems.Item(indx.Detach());
      _ContactItem * pItem = (_ContactItem *)&itm;
      if ( QString::fromUcs2(pItem->GetEntryID().GetBuffer()) == uid )
          return itm;
      itm->Release();
    }
  return 0;
}
#endif
