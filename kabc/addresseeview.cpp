/*
    This file is part of libkdepim.

    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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

#include <kabc/address.h>
#include <kabc/addressee.h>
#include <kabc/phonenumber.h>
#include <kglobal.h>
//US#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klocale.h>
//US #include <kstringhandler.h>
#include <qscrollview.h>
#include <qregexp.h>
#include <qfile.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qpushbutton.h>
#ifdef DESKTOP_VERSION 
#include <qpaintdevicemetrics.h>
#include <qprinter.h>
#include <qpainter.h>
#endif


#include <qstylesheet.h> 
#include "externalapphandler.h"
#include <kabc/addresseeview.h>


//US #ifndef DESKTOP_VERSION
//US #include <qtopia/qcopenvelope_qws.h>
//US #include <qpe/qpeapplication.h>
//US #endif

//US static int kphoneInstalled = 0;

using namespace KABC;
bool AddresseeView::sFullDetailsMode = false;

AddresseeView::AddresseeView( QWidget *parent, const char *name )
  : QTextBrowser( parent, name )


{
//US  setWrapPolicy( QTextEdit::AtWordBoundary );
  setLinkUnderline( false );
  // setVScrollBarMode( QScrollView::AlwaysOff );
  //setHScrollBarMode( QScrollView::AlwaysOff );

//US  QStyleSheet *sheet = styleSheet();
//US  QStyleSheetItem *link = sheet->item( "a" );
//US  link->setColor( KGlobalSettings::linkColor() );


}
void AddresseeView::printMe()
{
#ifdef DESKTOP_VERSION 
    QPrinter printer;
    if (!printer.setup() )
        return;
    QPainter p;
    p.begin ( &printer  );
    QPaintDeviceMetrics m = QPaintDeviceMetrics ( &printer );
    float dx, dy;
    int wid = (m.width() * 9)/10;
    dx = (float) wid/(float)contentsWidth ();
    dy  = (float)(m.height()) / (float)contentsHeight ();
    float scale;
    // scale to fit the width or height of the paper
    if ( dx < dy )
        scale = dx;
    else
        scale = dy;
    p.translate( m.width()/10,0 );
    p.scale( scale, scale );
    drawContents ( &p, 0,0,  contentsWidth (),  contentsHeight () ); 
    p.end();
#endif  
}
void AddresseeView::setSource(const QString& n)
{
    //qDebug("********AddresseeView::setSource %s", n.latin1());

   if ( n == "allDetails" ) {
       sFullDetailsMode = true;
       setAddressee( mCurrentContact );
   } else  if ( n == "notAllDetails" ) {
       sFullDetailsMode = false;
       setAddressee( mCurrentContact );
   } else if ( n.left( 6 ) == "mailto" )
       ExternalAppHandler::instance()->mailToOneContact( n.mid(7) );
   else if ( n.left( 7 ) == "phoneto" )
       ExternalAppHandler::instance()->callByPhone(  n.mid(8) );
   else if ( n.left( 5 ) == "faxto" )
       ExternalAppHandler::instance()->callByFax(  n.mid(6) );
   else if ( n.left( 5 ) == "smsto" )
       ExternalAppHandler::instance()->callBySMS(  n.mid(6) );
   else if ( n.left( 7 ) == "pagerto" )
       ExternalAppHandler::instance()->callByPager(  n.mid(8) );
   else if ( n.left( 5 ) == "sipto" )
       ExternalAppHandler::instance()->callBySIP(  n.mid(6) );

}
void AddresseeView::setAddressee( const KABC::Addressee& mAddressee )
{
  bool kemailAvail = ExternalAppHandler::instance()->isEmailAppAvailable();
  // mAddressee = addr;
  // clear view
  //setText( QString::null );
  mCurrentContact = mAddressee;
  if ( mAddressee.isEmpty() ) {
      setText( QString::null);
      return;
  }
#if 0
  QString name = ( mAddressee.assembledName().isEmpty() ?
                   mAddressee.formattedName() : mAddressee.assembledName() );
#endif

  QString name =  mAddressee.realName();
  QString assName = mAddressee.assembledName();
  if ( assName.isEmpty() )
      assName = name;
  QString dynamicPart;

  dynamicPart += getPhoneNumbers( mAddressee.phoneNumbers(),true );
  QStringList emails = mAddressee.emails();
  QStringList::ConstIterator emailIt;
  QString type = i18n( "Email" );
  emailIt = emails.begin();
  if ( emailIt != emails.end() ) {
    if ( kemailAvail ) {
      dynamicPart += QString(
            "<tr><td align=\"right\"><b>%1</b></td>"
           "<td align=\"left\"><a href=\"mailto:%2 <%3> \">%4</a></td></tr>" )
          .arg( type )
          .arg( assName )
          .arg( *emailIt )
          .arg( *emailIt );
      ++emailIt;
    } else {
      dynamicPart += QString(
            "<tr><td align=\"right\"><b>%1</b></td>"
           "<td align=\"left\">%2</td></tr>" )
          .arg( type )
          .arg( *emailIt );
      ++emailIt;
    }
  }
  if ( mAddressee.birthday().date().isValid() ) {
      dynamicPart += QString(
            "<tr><td align=\"right\"><b>%1</b></td>"
           "<td align=\"left\">%2</td></tr>" )
          .arg( i18n ("Birthday") )
          .arg( KGlobal::locale()->formatDate( mAddressee.birthday().date()  ,true) );
  }
  dynamicPart += getPhoneNumbers( mAddressee.phoneNumbers(), false );

  for ( ; emailIt != emails.end(); ++emailIt ) {
    if ( kemailAvail ) {
      dynamicPart += QString(
        "<tr><td align=\"right\"><b>%1</b></td>"
        "<td align=\"left\"><a href=\"mailto:%2 <%3> \">%4</a></td></tr>" )
        .arg( type )
        .arg( name )
        .arg( *emailIt )
        .arg( *emailIt );
	} else {
      dynamicPart += QString(
        "<tr><td align=\"right\"><b>%1</b></td>"
        "<td align=\"left\">%2</td></tr>" )
        .arg( type )
        .arg( *emailIt );
    }
  }



  KABC::Address::List addresses = mAddressee.addresses();
  KABC::Address::List::ConstIterator addrIt;
  for ( addrIt = addresses.begin(); addrIt != addresses.end(); ++addrIt ) {
      if ( true /*(*addrIt).label().isEmpty()*/ ) {
      QString formattedAddress = (*addrIt).formattedAddress().stripWhiteSpace();
//US      formattedAddress = formattedAddress.replace( '\n', "<br>" );
      //qDebug("adresss %s ",formattedAddress.latin1() );
       formattedAddress = formattedAddress.replace( QRegExp("\n"), "<br>" );
       //qDebug("AddresseeView::setAddressee has to be verified.");

      dynamicPart += QString(
        "<tr><td align=\"right\"><b>%1</b></td>"
        "<td align=\"left\">%2</td></tr>" )
        .arg( KABC::Address::typeLabel( (*addrIt).type() ) )
        .arg( formattedAddress );
    } else {

      dynamicPart += QString(
        "<tr><td align=\"right\"><b>%1</b></td>"
        "<td align=\"left\">%2</td></tr>" )
        .arg( KABC::Address::typeLabel( (*addrIt).type() ) )
//US        .arg( (*addrIt).label().replace( '\n', "<br>" ) );
          .arg( (*addrIt).label() /*replace( QRegExp("\n"), "<br>" )*/ );

    }
  }
 
 
    QString notes;
    if ( sFullDetailsMode ) {
        notes = QString(
                        "<tr><td align=\"right\"><b>%1</b></td>"
                        "<td align=\"left\"><a href=\"notAllDetails\">%4</a></td></tr>" )
            .arg( i18n("Details") )
            .arg( i18n("Hide!") );
     
        QString tempX = mAddressee.custom( "KADDRESSBOOK", "X-Gender" );
        if ( !tempX.isEmpty() ) {
            if ( tempX == "male" || tempX == "female" ) {
                notes += QString(
                                 "<tr><td align=\"right\"><b>%1</b></td>"
                                 "<td align=\"left\">%2</td></tr>" )
                    .arg( i18n( "" ) )
                    .arg( i18n(tempX) );
            }
        } 

        
        tempX =  mAddressee.secrecy().asString();
        
        notes += QString(
                         "<tr><td align=\"right\"><b>%1</b></td>"
                         "<td align=\"left\">%2</td></tr>" )
            .arg(  "" )
            .arg( tempX );
        
       
        tempX = mAddressee.categories().join(" - ");
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Category" ) )
                .arg( tempX );
        } 
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-Profession" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Profession" ) )
                .arg( tempX );
        } 
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-Office" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Office" ) )
                .arg( tempX );
        }
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-Department" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Department" ) )
                .arg( tempX );
        } 
        
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-ManagersName" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Manager" ) )
                .arg( tempX );
        } 
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-AssistantsName" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Assistant" ) )
                .arg( tempX );
        } 
        if ( !mAddressee.url().url().isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Homepage" ) )
                .arg( mAddressee.url().url() );
        }
        tempX = mAddressee.nickName();
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Nickname" ) )
                .arg( tempX );
        }
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-IMAddress" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Messanger" ) )
                .arg( tempX );
        }
       
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-Anniversary" );
        if ( !tempX.isEmpty() ) {
            QDate dt = KGlobal::locale()->readDate( tempX, "%Y-%m-%d");
            if ( dt.isValid () ) {
                tempX = KGlobal::locale()->formatDate(dt, true);
                notes += QString(
                                 "<tr><td align=\"right\"><b>%1</b></td>"
                                 "<td align=\"left\">%2</td></tr>" )
                    .arg( i18n( "Anniversary" ) )
                    .arg( tempX );
            }
        } 
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-SpousesName" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Spouse" ) )
                .arg( tempX );
        } 
        tempX = mAddressee.custom( "KADDRESSBOOK", "X-Children" );
        if ( !tempX.isEmpty() ) {
            notes += QString(
                             "<tr><td align=\"right\"><b>%1</b></td>"
                             "<td align=\"left\">%2</td></tr>" )
                .arg( i18n( "Children" ) )
                .arg( tempX );
        } 
       
      if (  !mAddressee.note().isEmpty() ) {
          notes += QString(
                          "<tr>"
                          "<td align=\"right\" valign=\"top\"><b>%1</b></td>"  // note label
                          "<td align=\"left\">%2</td>"  // note
                          "</tr>" ).arg( i18n( "Notes" ) )
              //US      .arg( mAddressee.note().replace( '\n', "<br>" ) );
              .arg( mAddressee.note().replace( QRegExp("\n"), "<br>" ) );
          //qDebug("AddresseeView::setAddressee has to be verified.");
      }
  } else {
      notes = QString(
            "<tr><td align=\"right\"><b>%1</b></td>"
           "<td align=\"left\"><a href=\"allDetails\">%4</a></td></tr>" )
          .arg( i18n("Details") )
          .arg( i18n("Show!") );


  }

  QString aRole = "";
  QString aOrga = "";
  if ( true /*!mAddressee.role().isEmpty()*/ ) {
      aRole = "<tr>"
          "<td align=\"left\">" + mAddressee.role() + "</td>"
          "</tr>";
  }
  if ( true /*!mAddressee.organization().isEmpty()*/ ) {
      aOrga = "<tr>"
          "<td align=\"left\">" + mAddressee.organization() + "</td>" 
          "</tr>";
  }
  mText = "";
  QString picString = "";
  KABC::Picture picture = mAddressee.photo();
  if (picture.undefined() ) picture = mAddressee.logo();
  bool picAvailintern = false;
  bool picAvailUrl = false;
  if (! picture.undefined() ) {
      picAvailintern = (picture.isIntern() && !picture.data().isNull());
      picAvailUrl = !picture.isIntern() &&  QFile::exists(picture.url() );
  }
  if ( picAvailUrl || picAvailintern || QApplication::desktop()->width() > 320 ) {
      picString =  "<img src=\"myimage\" width=\"50\" height=\"70\">";
      if (  picAvailintern ) {
          QMimeSourceFactory::defaultFactory()->setImage( "myimage", picture.data() );
          int wid = picture.data().width();
          int hei = picture.data().height();  
          if ( wid > 128 || hei > 128 ) {
              if ( wid > hei ) {
                  hei = (hei*128)/wid;
                  wid = 128;
              } else {
                  wid = (wid*128)/hei;
                  hei = 128;
              }
          }
          picString =  QString("<img src=\"myimage\" width=\"%1\" height=\"%2\">").arg(wid).arg(hei);
      } else {
          if ( picAvailUrl ) {
              QPixmap picPix( picture.url() );
              QMimeSourceFactory::defaultFactory()->setPixmap( "myimage", picPix );
              int wid = picPix.width();
              int hei = picPix.height();
              if ( wid > 128 || hei > 128 ) {
                  if ( wid > hei ) {
                      hei = (hei*128)/wid;
                      wid = 128;
                  } else {
                      wid = (wid*128)/hei;
                      hei = 128;
                  }
              }
              picString =  QString("<img src=\"myimage\" width=\"%1\" height=\"%2\">").arg(wid).arg(hei);
          } else {
              if ( !mAddressee.custom( "KADDRESSBOOK", "X-Children" ).isEmpty() ) {
                  static bool setDefaultImageChildren = false;
                  if ( !setDefaultImageChildren ) {
                      QMimeSourceFactory::defaultFactory()->setPixmap( "familyIcon", KGlobal::iconLoader()->loadIcon( "ic_kids", KIcon::Desktop, 128 ) );
                      setDefaultImageChildren = true;
                  }
                  picString =  "<img src=\"familyIcon\" width=\"64\" height=\"64\">";
                  
              } else  if ( !mAddressee.custom( "KADDRESSBOOK", "X-SpousesName" ).isEmpty() ) {
                  static bool setDefaultImagepouses = false;
                  if ( !setDefaultImagepouses ) {
                      QMimeSourceFactory::defaultFactory()->setPixmap( "SpousesIcon", KGlobal::iconLoader()->loadIcon( "ic_family", KIcon::Desktop, 128 ) );
                      setDefaultImagepouses = true;
                  }
                  picString =  "<img src=\"SpousesIcon\" width=\"64\" height=\"64\">";
              } else {
                  QString gen = mAddressee.custom( "KADDRESSBOOK", "X-Gender" );
                  if ( gen == "male" ) {
                      static bool setDefaultImageMale = false;
                      if ( !setDefaultImageMale ) {
                          QMimeSourceFactory::defaultFactory()->setPixmap( "MaleIcon", KGlobal::iconLoader()->loadIcon( "ic_male", KIcon::Desktop, 128 ) );
                          setDefaultImageMale = true;
                  }
                      picString =  "<img src=\"MaleIcon\" width=\"64\" height=\"64\">";
                      
                  } else if ( gen == "female" ) {
                  static bool setDefaultImageFemale = false;
                  if ( !setDefaultImageFemale ) {
                      QMimeSourceFactory::defaultFactory()->setPixmap( "FemaleIcon", KGlobal::iconLoader()->loadIcon( "ic_female", KIcon::Desktop, 128 ) );
                      setDefaultImageFemale = true;
                  }
                  picString =  "<img src=\"FemaleIcon\" width=\"64\" height=\"64\">";

                  } else {
                      static bool setDefaultImage = false;
                      if ( !setDefaultImage ) {
                          //qDebug("Setting default pixmap ");
                          QMimeSourceFactory::defaultFactory()->setPixmap( "defaultIcon", KGlobal::iconLoader()->loadIcon( "ic_penguin", KIcon::Desktop, 128 ) );
                          setDefaultImage = true;
                      }
                      picString =  "<img src=\"defaultIcon\" width=\"64\" height=\"64\">";
                  }
              }
          }
      }
    mText = QString::fromLatin1(
  "<html>"
  "<body text=\"%1\" bgcolor=\"%2\">" // text and background color
  "<table>"
  "<tr>"
  "<td rowspan=\"3\" align=\"right\" valign=\"top\">"
  "%3"
  "</td>"
  "<td align=\"left\"><font size=\"+2\"><b>%4</b></font></td>"  // name
  "</tr>"
  "%5"  // role
  "%6"  // organization
  "<td colspan=\"2\">&nbsp;</td>"
  "%7"  // dynamic part
  "%8"  // notes
  "</table>"
  "</body>"
  "</html>")
//US
  .arg( /*KGlobalSettings::textColor().name()*/ "black" )
//US
  .arg( /*KGlobalSettings::baseColor().name()*/ "white" )
  .arg( picString )
  .arg( name )
  .arg( aRole )
  .arg( aOrga )
  .arg( dynamicPart )
  .arg( notes );

  } else { // no picture!

mText = "<table width=\"100%\">\n";
    //mText += "<tr bgcolor=\"#3679AD\"><td><h2>";
#ifdef DESKTOP_VERSION
    mText += "<tr bgcolor=\"#5699CD\"><td align=\"left\"><h1>";
    mText += "<font color=\"#FFFFFF\">" + name +"</font></h1>";
#else
    mText += "<tr bgcolor=\"#5699CD\"><td align=\"left\"><h3>";
    mText += "<font color=\"#FFFFFF\"> " + name +"</font></h3>";
#endif

    mText += "</td></tr>\n<tr  bgcolor=\"#EAF8FA\"><td>";

    mText += "<table><td colspan=\"2\">&nbsp;</td>";
    /*
      mText += QString("<tr><td align=\"right\"><b2>%1</b2></td>"
        "<td align=\"left\"><b>%2</b></td></tr>" )
          .arg( i18n(" ") )
        .arg( name );
    */
      if ( ! mAddressee.role().isEmpty() )
      mText += QString("<tr><td align=\"right\"><b>%1</b></td>"
        "<td align=\"left\">%2</td></tr>" )
          .arg( i18n(" ") )
        .arg( mAddressee.role());
      if ( ! mAddressee.organization().isEmpty() )
      mText += QString("<tr><td align=\"right\"><b>%1</b></td>"
        "<td align=\"left\">%2</td></tr>" )
          .arg( i18n(" ") )
        .arg( mAddressee.organization());
      mText += dynamicPart;
      mText += notes;
      mText += "</table>";

 }

  // at last display it...
  setText( mText );

}

QString AddresseeView::getPhoneNumbers(  KABC::PhoneNumber::List phones_unsorted ,bool preferred )
{  
  ExternalAppHandler* eah = ExternalAppHandler::instance();
  bool kphoneAvail = eah->isPhoneAppAvailable();
  bool kfaxAvail = eah->isFaxAppAvailable();
  bool ksmsAvail = eah->isSMSAppAvailable();
  bool kpagerAvail = eah->isPagerAppAvailable();
  bool ksipAvail = eah->isSIPAppAvailable();
  QString dynamicPart;
  KABC::PhoneNumber::List::ConstIterator phoneIt;
  QString extension;
  int phonetype;
  QString sms;

  KABC::PhoneNumber::List::Iterator it;
  KABC::PhoneNumber::List phones ;

    PhoneNumber::TypeList  tList = PhoneNumber::supportedTypeList();
    int i = 0;
    int max = tList.count();
    while ( i < max-1 ) {
        for ( it = phones_unsorted.begin(); it != phones_unsorted.end(); ++it ) {
            if ( (*it).type() == tList[i] ) {
                phones.append( (*it ) ); 
                break; 
            }
        }
        ++i;   
    }
    for ( it = phones_unsorted.begin(); it != phones_unsorted.end(); ++it ) {
            if ( (*it).type() == tList[ max-1 ] )
                phones.append( (*it ) );  
    }

  for ( phoneIt = phones.begin(); phoneIt != phones.end(); ++phoneIt ) {
    phonetype = (*phoneIt).type();
    bool con = false;
    if ( ((phonetype & KABC::PhoneNumber::Pref) == 0 ) == preferred ) con = true;
    if ((phonetype & KABC::PhoneNumber::Cell) == KABC::PhoneNumber::Cell ) con = !preferred;;
    if ( con )
        continue;
  
    if (ksmsAvail &&
        (
		 ((phonetype & KABC::PhoneNumber::Car) == KABC::PhoneNumber::Car) ||
         ((phonetype & KABC::PhoneNumber::Cell) == KABC::PhoneNumber::Cell)
        )
       )
    {
      sms = QString("<a href=\"smsto:%1  \">(sms)</a>" )
              .arg( (*phoneIt).number() );

    }
    else
      sms = "";

    extension = QString::null;
    if ((phonetype & KABC::PhoneNumber::Fax) == KABC::PhoneNumber::Fax) {
      if (kfaxAvail) extension = "faxto:";
    }
    else if ((phonetype & KABC::PhoneNumber::Pager) == KABC::PhoneNumber::Pager) {
      if (kpagerAvail) extension = "pagerto:";
    }
#if 0
    else if ((phonetype & KABC::PhoneNumber::Sip) == KABC::PhoneNumber::Sip) {
      if (ksipAvail) extension = "sipto:";
    }
#endif
    else if (kphoneAvail) {
      extension = "phoneto:";
    }
    else
      extension = QString::null;

      if ( !extension.isEmpty() ) {
          dynamicPart += QString(
                                 "<tr><td align=\"right\"><b>%1</b></td>"
           "<td align=\"left\"><a href=\"%2%3  \">%4</a> %5</td></tr>" )
              .arg( KABC::PhoneNumber::typeLabel( phonetype ) )
              .arg( extension )
              .arg( (*phoneIt).number() )
              .arg( (*phoneIt).number() )
              .arg( sms );

      } else {
          dynamicPart += QString(
                                 "<tr><td align=\"right\"><b>%1</b></td>"
                                 "<td align=\"left\">%2 %3</td></tr>" )
              .arg( KABC::PhoneNumber::typeLabel( phonetype ) )
              .arg( (*phoneIt).number() )
              .arg( sms );
      }
  }
  return dynamicPart;
}
/*
KABC::Addressee AddresseeView::addressee() const
{
  return mAddressee;
}
*/
void AddresseeView::addTag(const QString & tag,const QString & text)
{
    if ( text.isEmpty() )
        return;
    int number=text.contains("\n");
    QString str = "<" + tag + ">";
    QString tmpText=text;
    QString tmpStr=str;
    if(number !=-1)
        {
            if (number > 0) {
                int pos=0;
                QString tmp;
                for(int i=0;i<=number;i++) {
                    pos=tmpText.find("\n");
                    tmp=tmpText.left(pos);
                    tmpText=tmpText.right(tmpText.length()-pos-1);
                    tmpStr+=tmp+"<br>";
                }
            }
            else tmpStr += tmpText;
            tmpStr+="</" + tag + ">";
            mText.append(tmpStr);
        }
    else
        {
            str += text + "</" + tag + ">";
            mText.append(str);
        }
}

AddresseeChooser::AddresseeChooser( KABC::Addressee loc, KABC::Addressee rem, bool takeloc, QWidget *parent, const char *name ) : KDialogBase(parent,name,
                true ,i18n("Conflict! Please choose Adressee!"),Ok|User1|Close,Close, false)
{
    findButton( Close )->setText( i18n("Cancel Sync"));
    findButton( Ok )->setText( i18n("Remote"));
    findButton( User1 )->setText( i18n("Local"));
    QWidget* topframe = new QWidget( this );
    setMainWidget( topframe );
    QBoxLayout* bl;
    if ( QApplication::desktop()->width() < 640 ) {
        bl = new QVBoxLayout( topframe  );
    } else {
        bl = new QHBoxLayout( topframe  );
    }
    QVBox* subframe = new QVBox( topframe );
    bl->addWidget(subframe  );
    QLabel* lab = new QLabel( i18n("Local Addressee"), subframe ); 
    if ( takeloc )
        lab->setBackgroundColor(Qt::green.light() );
    AddresseeView * av = new AddresseeView( subframe );
    av->setAddressee( loc );
    subframe = new QVBox( topframe );
    bl->addWidget(subframe  );
    lab = new QLabel( i18n("Remote Addressee"), subframe  ); 
    if ( !takeloc )
        lab->setBackgroundColor(Qt::green.light() );
    av = new AddresseeView( subframe );
    av->setAddressee( rem );
    QObject::connect(findButton( Ok ),SIGNAL(clicked()),this, SLOT(slot_remote())); 
    QObject::connect(this,SIGNAL(user1Clicked()),this, SLOT(slot_local())); 
#ifndef DESKTOP_VERSION
    showMaximized();
#else
    resize ( 640, 400 );
#endif
}

int AddresseeChooser::executeD( bool local )
{
    mSyncResult = 3; 
    if ( local ) 
        findButton( User1 )->setFocus();
    else 
        findButton( Ok )->setFocus();
    exec();
    return mSyncResult;  
}
void AddresseeChooser::slot_remote()
{
    mSyncResult = 2;
    accept();
}
void AddresseeChooser::slot_local()
{
    mSyncResult = 1;
    accept();
}
