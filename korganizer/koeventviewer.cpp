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
*/

#include <qcstring.h>
#include <qwhatsthis.h>
#include <qdialog.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapplication.h>
#include <libkcal/event.h>
#include <libkcal/todo.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <krun.h>
#include <kglobal.h>
#include <kprocess.h>
#include "koprefs.h"

#include <kabc/stdaddressbook.h>

#ifndef KORG_NODCOP
#include <dcopclient.h>
#include "korganizer.h"
#include "koprefs.h"
#include "actionmanager.h"
#endif

#include "koeventviewer.h"
//#ifndef KORG_NOKABC
//#include <kabc/stdaddressbook.h>
//#define size count
//#endif

#ifdef DESKTOP_VERSION
#include <kabc/addresseedialog.h>
#include <kabc/addresseeview.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#else //DESKTOP_VERSION
#include <qtopia/qcopenvelope_qws.h> 
#endif //DESKTOP_VERSION
#include <externalapphandler.h>

KOEventViewer::KOEventViewer(QWidget *parent,const char *name)
  : QTextBrowser(parent,name)
{
    mSyncMode = false;
    mColorMode = 0;
}

KOEventViewer::~KOEventViewer()
{
}

void KOEventViewer::printMe()
{
#ifdef DESKTOP_VERSION 

    KOPrintPrefs pp ( this );
    if (!pp.exec() )
        return;
    int scaleval = pp.printMode() ;

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
    if ( scaleval == 2 ||  scaleval == 1 && scale < 1.0 ) {
        p.scale( scale, scale );
    }
    drawContents ( &p, 0,0,  contentsWidth (),  contentsHeight () ); 
    p.end();
#endif  

}
void KOEventViewer::setSource(const QString& n)
{

    if ( n.left(8) == "todo_uid" ) {
        int midstr = 9;
#ifdef DESKTOP_VERSION
        midstr = 11;
#endif        
        //qDebug("-%s- ", n.mid(midstr).latin1());
        emit showIncidence(  n.mid(midstr) );
        return;
    }

    if ( n.left(3) == "uid" ) 
#ifdef DESKTOP_VERSION
        {
        KABC::StdAddressBook* AddressBook = KABC::StdAddressBook::self( true );
        KABC::AddressBook::Iterator it;
        for( it = AddressBook->begin(); it != AddressBook->end(); ++it ) {
            // LR I do not understand, why the uid string is different on zaurus and desktop
            QString uid = "uid://"+(*it).uid();

            //qDebug("for *%s*          +%s+ ", n.latin1(), uid.latin1());
            if (n == uid ) {
                //qDebug("found %s ",(*it).mobileHomePhone().latin1() );
                QDialog dia( this,"dia123", true );
                dia.setCaption( i18n("Details of attendee") );
                QVBoxLayout lay ( &dia );
                KABC::AddresseeView av ( &dia );
                av.setAddressee( (*it) );
                lay.addWidget( &av );
                if ( QApplication::desktop()->width() < 480 )
                    dia.resize( 220, 240);
                else {
                    dia.resize( 400,400);
                }
                dia.exec();
                break;
            } 
        }
        return;
    }
#else  
    {
        if (  "uid:organizer" == n ) {
            ExternalAppHandler::instance()->requestDetailsFromKAPI("", mCurrentIncidence->organizer(),"");
            return;
        } 
        QPtrList<Attendee> attendees = mCurrentIncidence->attendees();
        if (attendees.count()) {  
            Attendee *a;
            for(a=attendees.first();a;a=attendees.next()) {
                if ( "uid:"+a->uid() == n ) {
                    bool res = ExternalAppHandler::instance()->requestDetailsFromKAPI(a->name(), a->email(), a->uid());
                    return;
                }
            }
        }
        return;
    }
    //requestNameEmailUidListFromKAPI("QPE/Application/kopi", this->name() /* name is here the unique uid*/);
  // the result should now arrive through method insertAttendees
    //QString uid = "uid:"+(*it).uid();
#endif
    if ( n.left(6) == "mailto" ) {
        // qDebug("KOEventViewer::setSource %s ", n.mid(7).latin1());
        //#ifndef DESKTOP_VERSION
        if ( n.mid(7,3) == "ALL" ) {
            mailToAttendees( true );
        } else if ( n.mid(7,4) == "RSVP"  ) {
            mailToAttendees( false );
        } else {
	  ExternalAppHandler::instance()->mailToOneContact( n.mid(7) );
       //QCopEnvelope e("QPE/Application/ompi", "newMail(QString)" );
       //e << n.mid(7);
        }
        //#endif

    }


#ifndef KORG_NODCOP
    kdDebug() << "KOEventViewer::setSource(): " << n << endl;
    QString tmpStr;
    if (n.startsWith("mailto:")) {
        KApplication::kApplication()->invokeMailer(n.mid(7),QString::null);
        //emit showIncidence(n);
        return;
    } else if (n.startsWith("uid:")) {
        DCOPClient *client = KApplication::kApplication()->dcopClient();
        const QByteArray noParamData;
        const QByteArray paramData;
        QByteArray replyData;
        QCString replyTypeStr;
#define PING_ABBROWSER (client->call("kaddressbook", "KAddressBookIface", "interfaces()",  noParamData, replyTypeStr, replyData))
        bool foundAbbrowser = PING_ABBROWSER;

        if (foundAbbrowser) {
            //KAddressbook is already running, so just DCOP to it to bring up the contact editor
            //client->send("kaddressbook","KAddressBookIface",
            QDataStream arg(paramData, IO_WriteOnly);
            arg << n.mid(6);
            client->send("kaddressbook", "KAddressBookIface", "showContactEditor( QString )",  paramData);
            return;
        } else {
            /*
              KaddressBook is not already running.  Pass it the UID of the contact via the command line while starting it - its neater.
              We start it without its main interface
            */
            KIconLoader* iconLoader = new KIconLoader();
            QString iconPath = iconLoader->iconPath("go",KIcon::Small);
            ActionManager::setStartedKAddressBook(true);
            tmpStr = "kaddressbook --editor-only --uid ";
            tmpStr += KProcess::quote(n.mid(6));
            KRun::runCommand(tmpStr,"KAddressBook",iconPath);
            return;
        }
    } else {
        //QTextBrowser::setSource(n);
    }
#endif
}
void KOEventViewer::mailToAttendees( bool all )
{
    QPtrList<Attendee> attendees = mCurrentIncidence->attendees();
    if (attendees.count() == 0) return;
    QStringList nameList;
    QStringList emailList;
    QStringList uidList;
    Attendee* a;
    for(a=attendees.first();a;a=attendees.next()) {
        if ( !all && !a->RSVP() ) continue;
        if (!a->email().isEmpty()) {
#ifndef DESKTOP_VERSION
            nameList.append (a->realName() );
            emailList.append (a->email() );
            uidList.append (a->uid() );
#else
            emailList.append(a->realName() +" <" + a->email() +">");
#endif
        }
    }
#ifndef DESKTOP_VERSION
    QString uid = "ComposeMailUIpick2"+mMailSubject;
    bool res = ExternalAppHandler::instance()->returnNameEmailUidListFromKAPI("QPE/Application/ompi", uid, nameList, emailList, uidList);

#else
    ExternalAppHandler::instance()->mailToMultipleContacts( emailList.join(","), mMailSubject );
#endif

}
void KOEventViewer::addTag(const QString & tag,const QString & text)
{
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

void KOEventViewer::setColorMode( int m )
{
    mColorMode = m;
}
void KOEventViewer::appendEvent(Event *event, int mode )
{  
    mMailSubject = "";
    mCurrentIncidence = event;
    bool shortDate = KOPrefs::instance()->mShortDateInViewer;
    bool wideScreen = ( QApplication::desktop()->width() >= 640 );
    topLevelWidget()->setCaption(i18n("Event Viewer"));
    if ( mode == 0 ) {
        addTag("h2",deTag(event->summary()));
        formatReadOnly(event);
    }
    else {
        if ( mColorMode == 1 ) {
            mText +="<font color=\"#00A000\">";
        }
        if ( mColorMode == 2 ) {
            mText +="<font color=\"#C00000\">";
        }
        // mText +="<font color=\"#F00000\">" + i18n("O-due!") + "</font>";
        if ( mode == 1 ) {
            addTag("h2",i18n( "Local: " ) +deTag(event->summary()));
        } else {
            addTag("h2",i18n( "Remote: " ) +deTag(event->summary()));
        } 
        formatReadOnly(event);
        addTag("h3",i18n( "Last modified: " ) + KGlobal::locale()->formatDateTime(event->lastModified(),shortDate, true ) );
        if ( mColorMode )
            mText += "</font>";
    } 
    mMailSubject += i18n( "Meeting " )+ event->summary();
    if (event->cancelled ()) {
        mText +="<font color=\"#B00000\">";
        addTag("i",i18n("This event has been cancelled!"));
        mText.append("<br>");
        mText += "</font>";
        mMailSubject += i18n("(cancelled)");
    }
   
    if (event->doesFloat()) {
        if (event->isMultiDay()) {
            mText.append(i18n("<p><b>From:</b> %1 </p><p><b>To:</b> %2</p>")
                         .arg(event->dtStartDateStr(shortDate))
                         .arg(event->dtEndDateStr(shortDate)));
            mText.append(i18n("<p><b>Duration:</b> %1 days</p>")
                         .arg(event->dtStart().daysTo(event->dtEnd())+1));
        } else {
            mText.append(i18n("<p><b>On:</b> %1</p>").arg(event->dtStartDateStr( shortDate )));
        }
    } else {
        if (event->isMultiDay()) {
            mText.append(i18n("<p><b>From:</b> %1</p> ")
                         .arg(event->dtStartStr( shortDate)));
            mText.append(i18n("<p><b>To:</b> %1</p>")
                         .arg(event->dtEndStr(shortDate)));
        } else {
            mText.append(i18n("<p><b>From:</b> %1 <b>To:</b> %2</p>")
                         .arg(event->dtStartTimeStr())
                         .arg(event->dtEndTimeStr()));
            mText.append(i18n("<p><b>On:</b> %1</p> ")
                         .arg(event->dtStartDateStr( shortDate )));
        }
    }
    if (!event->location().isEmpty()) {
        addTag("p","<b>"+i18n("Location: ")+"</b>"+  deTag(event->location() ) );
        mMailSubject += " (" + i18n("Location: ") + event->location() + ")";
    }
    if (event->doesRecur()) {

        QString recurText = event->recurrenceText();
        addTag("p","<em>" + i18n("This is a %1 recurring event.").arg(recurText ) + "</em>");
        bool ok;
        QDate start = QDate::currentDate();
        QDateTime next;
        next = event->getNextOccurence( QDateTime::currentDateTime() , &ok );
        if ( ok ) {
            if ( wideScreen ){
                addTag("p",i18n("<b>Next recurrence is on:</b>") +" " + KGlobal::locale()->formatDate( next.date(), shortDate ) );
            } else {
                addTag("p",i18n("<b>Next recurrence is on:</b>")  );
                addTag("p", KGlobal::locale()->formatDate( next.date(), shortDate ));
            }
            mMailSubject += i18n(" - " )+ KGlobal::locale()->formatDateTime( next, true );

        } else {
            bool last;
            QDate nextd;
            nextd = event->recurrence()->getPreviousDate( QDate::currentDate() , &last );
            if ( last ) {
                if ( wideScreen ){
                    addTag("p",i18n("<b>Last recurrence was on:</b>") +" " + KGlobal::locale()->formatDate( nextd, shortDate ));
                } else{
                    addTag("p",i18n("<b>Last recurrence was on:</b>")  );
                    addTag("p", KGlobal::locale()->formatDate( nextd, shortDate ));
                }
            }
        }
    } else {
        mMailSubject += i18n(" - " )+event->dtStartStr( true );

    }


    if (event->isAlarmEnabled()) {
        Alarm *alarm =event->alarms().first() ;
        QDateTime t = alarm->time();

	if (event->doesRecur()) {
	  bool ok = false;
	  int offset = 0;
	  QDateTime next = event->getNextAlarmDateTime(& ok, &offset, QDateTime::currentDateTime() ) ;
	  if ( ok ) {
	    t = next;
	  }
	}

        QString s =i18n("( %1 before )").arg( alarm->offsetText() );
        if(wideScreen ){
            addTag("p",i18n("<b>Alarm on: ") + s  +" </b>"+ KGlobal::locale()->formatDateTime( t, shortDate ));
        }else{
            addTag("p",i18n("<b>Alarm on: ") + s  +" </b>");
            addTag("p", KGlobal::locale()->formatDateTime( t, shortDate ));
        }
        //addTag("p",s);
        if ( !(event->alarmEnabled() ) ) {
            addTag("p", "<em>("+i18n("Enable alarm in resource settings") + ")</em>");

        }
    }

    addTag("p","<b>"+i18n("Access: ") + "</b>" + event->secrecyStr());
  
    formatCategories(event);

    formatAttendees(event);
  
    if ( KOPrefs::instance()->mEVshowCreated ) {
        if(wideScreen ){
            addTag("p",i18n("<b>Created: ")  +" </b>"+KGlobal::locale()->formatDateTime( event->created(), shortDate ));
        }else{
            addTag("p",i18n("<b>Created: ")  +" </b>");
            addTag("p", KGlobal::locale()->formatDateTime( event->created(), shortDate ));
        }

        
    }
    if ( KOPrefs::instance()->mEVshowChanged ) {
        if(wideScreen ){
            addTag("p",i18n("<b>Last modified: ")  +" </b>" + KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ) );
        }else{
            addTag("p",i18n("<b>Last modified: ")  +" </b>");
            addTag("p", KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ));
        }
        
    }   
    if ( KOPrefs::instance()->mEVshowDetails ) {
        if (!event->description().isEmpty()) {
            addTag("p",i18n("<b>Details: </b>"));
            addTag("p",deTag(event->description()));
        }
    }
    setText(mText);
    //QWhatsThis::add(this,mText);

}

void KOEventViewer::appendTodo(Todo *event, int mode )
{
    mMailSubject = "";
    mCurrentIncidence = event;
    topLevelWidget()->setCaption(i18n("Todo Viewer"));
    bool shortDate = KOPrefs::instance()->mShortDateInViewer;
    bool wideScreen = ( QApplication::desktop()->width() >= 640 );
    if (mode == 0 ) {
        addTag("h2",deTag(event->summary()));
        formatReadOnly(event);
    }
    else { 
        if ( mColorMode == 1 ) {
            mText +="<font color=\"#00A000\">";
        }
        if ( mColorMode == 2 ) {
            mText +="<font color=\"#B00000\">";
        }
        if ( mode == 1 ) {
            addTag("h2",i18n( "Local: " ) +deTag(event->summary()));
        } else {
            addTag("h2",i18n( "Remote: " ) +deTag(event->summary()));
        }
        formatReadOnly(event);
        addTag("h3",i18n( "Last modified: " ) + KGlobal::locale()->formatDateTime(event->lastModified(),shortDate, true ) );
        if ( mColorMode )
            mText += "</font>";
    } 
    mMailSubject += i18n( "Todo " )+ event->summary();

     if ( event->percentComplete() == 100 && event->hasCompletedDate() ) {
        mText +="<font color=\"#B00000\">";
        addTag("i",  i18n("<p><i>Completed on %1</i></p>").arg( event->completedStr(KOPrefs::instance()->mShortDateInViewer) ) );
        mText += "</font>";
    } else {
        mText.append(i18n("<p><i>%1 % completed</i></p>")
                     .arg(event->percentComplete()));
    }

    if (event->cancelled ()) {
        mText +="<font color=\"#B00000\">";
        addTag("i",i18n("This todo has been cancelled!"));
        mText.append("<br>");
        mText += "</font>";
        mMailSubject += i18n("(cancelled)");
    }
   
   

    if (event->doesRecur()) {

        QString recurText = event->recurrence()->recurrenceText();
        addTag("p","<em>" + i18n("This is a %1 recurring todo.").arg(recurText ) + "</em>");
            
    }
    if (event->hasStartDate()) {
        mText.append(i18n("<p><b>Start on:</b> %1</p>").arg(event->dtStartStr(KOPrefs::instance()->mShortDateInViewer)));
    }
    if (!event->location().isEmpty()) { 
        mMailSubject += " (" + i18n("Location: ") + event->location() + ")";
    }
    if (event->hasDueDate()) {
        mText.append(i18n("<p><b>Due on:</b> %1</p>").arg(event->dtDueStr(KOPrefs::instance()->mShortDateInViewer)));
    }
    if (!event->location().isEmpty()) { 
        addTag("p","<b>"+i18n("Location: ")+"</b>"+  deTag(event->location() ) );
    }
    mText.append(i18n("<p><b>Priority:</b> %2</p>")
                 .arg(QString::number(event->priority())));

    if (event->isAlarmEnabled()) {
        Alarm *alarm =event->alarms().first() ;
        QDateTime t = alarm->time();
	if (event->doesRecur()) {
	  bool ok = false;
	  int offset = 0;
	  QDateTime next = event->getNextAlarmDateTime(& ok, &offset, QDateTime::currentDateTime() ) ;
	  if ( ok ) {
	    t = next;
	  }
	}
	QString s =i18n("( %1 before )").arg( alarm->offsetText() ); 
        if ( wideScreen ) {
            addTag("p",i18n("<b>Alarm on: ") + s  +" </b>"+ KGlobal::locale()->formatDateTime( t, shortDate ));
        } else {
            addTag("p",i18n("<b>Alarm on: ") + s  +" </b>");
            addTag("p", KGlobal::locale()->formatDateTime( t, shortDate ));
        }
        if ( !(event->alarmEnabled() ) ) {
            addTag("p", "<em>("+i18n("Enable alarm in resource settings") + ")</em>");

        }
    }

    addTag("p","<b>"+i18n("Access: ") + "</b>" + event->secrecyStr());

    formatCategories(event);

    formatAttendees(event);
    
    if ( KOPrefs::instance()->mEVshowCreated ) {
        if(wideScreen ){

            addTag("p",i18n("<b>Created: ")  +" </b>"+KGlobal::locale()->formatDateTime( event->created(), shortDate ));

        } else {
            addTag("p",i18n("<b>Created: ")  +" </b>");
            addTag("p", KGlobal::locale()->formatDateTime( event->created(), shortDate ));
        }  
    }
    if ( KOPrefs::instance()->mEVshowChanged ) {
        if(wideScreen ){
            addTag("p",i18n("<b>Last modified: ")  +" </b>" +KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ) );

        } else {
            addTag("p",i18n("<b>Last modified: ")  +" </b>");
            addTag("p", KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ));
        }
    } 
    if ( event->relatedTo() ) {
        addTag("b",i18n("Parent todo:<br>"));

        QString t_name = "[" +QString::number(event->relatedTo()->priority()) + "/" + QString::number(((Todo*)event->relatedTo())->percentComplete())+"%] ";// +event->relatedTo()->summary());

        mText += t_name;
        mText += "<a href=\"todo_uid:" + event->relatedTo()->uid() + "\">";
        mText += deTag(event->relatedTo()->summary());
        mText += "</a><br>";

        //        mText.append(deTag("[" +QString::number(event->relatedTo()->priority()) + "/" + QString::number(((Todo*)event->relatedTo())->percentComplete())+"%] " +event->relatedTo()->summary()) +"<br>");
    }
    QPtrList<Incidence> Relations = event->relations();
    Incidence *to;
    if ( Relations.first() )
        addTag("b",i18n("Sub todos:<br>"));
    for (to=Relations.first();to;to=Relations.next()) {
        QString t_name = "[" +QString::number(((Todo*)to)->priority()) + "/" + QString::number(((Todo*)to)->percentComplete())+"%] ";// +to->relatedTo()->summary());
        mText += t_name;
        mText += "<a href=\"todo_uid:" + to->uid() + "\">";
        mText += deTag(to->summary());
        mText += "</a><br>";
        
    }  

    if ( KOPrefs::instance()->mEVshowDetails ) {
        if (!event->description().isEmpty()) {
            addTag("p",i18n("<b>Details: </b>"));
            addTag("p",deTag(event->description()));
        }
    }
    setText(mText);
}

void KOEventViewer::formatCategories(Incidence *event)
{
    if (!event->categoriesStr().isEmpty()) {
        if (event->categories().count() == 1) {
            addTag("p","<b>"+i18n("Category") + ":</b> " + event->categoriesStrWithSpace());
        } else {
            addTag("p","<b>"+i18n("Categories")+":</b> " + event->categoriesStrWithSpace() ) ;
        }
    }
}
void KOEventViewer::formatAttendees(Incidence *event)
{
    QPtrList<Attendee> attendees = event->attendees();
    if (attendees.count()) {
        
  
        QString iconPath = KGlobal::iconLoader()->iconPath("mailappt",KIcon::Small);
        QString NOiconPath = KGlobal::iconLoader()->iconPath("nomailappt",KIcon::Small);
        addTag("h3",i18n("Organizer"));
        mText.append("<ul><li>");
#ifndef KORG_NOKABC
       
#ifdef DESKTOP_VERSION
        KABC::AddressBook *add_book = KABC::StdAddressBook::self();
        KABC::Addressee::List addressList;
        addressList = add_book->findByEmail(event->organizer());
        KABC::Addressee o = addressList.first();
        if (!o.isEmpty() && addressList.size()<2) {
            mText += "<a href=\"uid:" + o.uid() + "\">";
            mText += o.formattedName();
            mText += "</a>\n";
        } else {
            mText.append(event->organizer());
        }
#else //DESKTOP_VERSION
                    mText += "<a href=\"uid:organizer\">";
                    mText += event->organizer();
                    mText += "</a>\n";
#endif //DESKTOP_VERSION


#else
        mText.append(event->organizer());
#endif
       
        if (iconPath) {
            mText += " <a href=\"mailto:" + event->organizer() + "\">";
            mText += "<IMG src=\"" + iconPath + "\">";
            mText += "</a>\n";
        }
        mText.append("</li></ul>");

        addTag("h3",i18n("Attendees"));
        Attendee *a;
        mText.append("<ul>");
        int a_count = 0;
        int a_count_nr = 0;

        for(a=attendees.first();a;a=attendees.next()) {
#ifndef KORG_NOKABC
#ifdef DESKTOP_VERSION
            if (a->name().isEmpty()) {
                addressList = add_book->findByEmail(a->email());
                KABC::Addressee o = addressList.first();
                if (!o.isEmpty() && addressList.size()<2) {
                    mText += "<a href=\"uid:" + o.uid() + "\">";
                    mText += o.formattedName();
                    mText += "</a>\n";
                } else {
                    mText += "<li>";
                    mText.append(a->email());
                    mText += "\n";
                }
            } else {
                mText += "<li><a href=\"uid:" + a->uid() + "\">";
                if (!a->name().isEmpty()) mText += a->name();
                else mText += a->email();
                mText += "</a>\n";
            }
#else //DESKTOP_VERSION
                mText += "<li><a href=\"uid:" + a->uid() + "\">";
                if (!a->name().isEmpty()) mText += a->name();
                else mText += a->email();
                mText += "</a>\n";
#endif //DESKTOP_VERSION
#else
            //qDebug("nokabc ");
            mText += "<li><a href=\"uid:" + a->uid() + "\">";
            if (!a->name().isEmpty()) mText += a->name();
            else mText += a->email();
            mText += "</a>\n";
#endif

          
            if (!a->email().isEmpty()) {
                if (iconPath) {
                    mText += "<a href=\"mailto:" + a->realName() +" <" + a->email() + ">:" + mMailSubject + "\">";
                    if ( a->RSVP() ) {
                        ++a_count_nr;
                        mText += "<IMG src=\"" + iconPath + "\">";
                    }
                    else {
                        ++a_count;
                        mText += "<IMG src=\"" + NOiconPath + "\">";
                    }
                    mText += "</a>\n";
                }
            }
            if (a->status() != Attendee::NeedsAction )
                mText +="[" + a->statusStr() + "] ";
            if (a->role() == Attendee::Chair )
                mText +="(" + a->roleStr().left(1) + ".)";
        }
        mText.append("</li></ul>");
        if ( (a_count+a_count_nr) > 1 ) {
                mText += "<a href=\"mailto:ALL\">";
                mText += i18n( "Mail to all" );
                mText += "</a> ( ";
                mText += "<IMG src=\"" + iconPath + "\">";
                mText += i18n( " and " );
                mText += "<IMG src=\"" + NOiconPath + "\"> )";
                mText += "<br>\n";


        }
        if ( a_count_nr > 1 ) {
                mText += "<a href=\"mailto:RSVP\">";
                mText += i18n( "Mail to selected" );
                mText += "</a> ( ";
                mText += i18n( "<IMG src=\"%1\"> only )").arg ( iconPath );
                mText += "<br>\n";
        }
    }

}
void KOEventViewer::appendJournal(Journal *jour, int mode )
{ 
    bool shortDate = KOPrefs::instance()->mShortDateInViewer;
    QString text_d = i18n("Journal from: ");
    if ( !jour->summary().isEmpty() )
        text_d = jour->summary();
    if (mode == 0 ) {
        addTag("h2", text_d );
    }
    else {
        if ( mode == 1 ) {
            addTag("h3",i18n( "Local: " ) + text_d );
        } else {
            addTag("h3",i18n( "Remote: " ) + text_d ); 
        } 
    }
    topLevelWidget()->setCaption(i18n("Journal viewer"));
    mText.append(i18n("<h3> %1 </h3> ").arg(jour->dtStartDateStr(KOPrefs::instance()->mShortDateInViewer))); 
    formatReadOnly(jour);
    addTag("p","<b>"+i18n( "Last modified: " ) + "</b>"+KGlobal::locale()->formatDateTime(jour->lastModified(),shortDate ) );
   
    if (!jour->description().isEmpty()) {
        addTag("p",deTag(jour->description()));
    }
    setText(mText);
}

void KOEventViewer::formatReadOnly(Incidence *event)
{
    int id = event->calID();
    if ( id > 1 ) {
        addTag("p", "<em>("+i18n("Calendar:")+" " + KOPrefs::instance()->getCalendar( id )->mName + ")</em>");
    }
    if (event->isReadOnly()) {
        addTag("p","<em>(" + i18n("read-only") + ")</em>");
    }
}
void KOEventViewer::setSyncMode( bool b )
{
    mSyncMode = b;
}

void KOEventViewer::setTodo(Todo *event, bool clearV )
{
    if ( clearV )
        clearEvents();
    if ( mSyncMode ) {
        if ( clearV )
            appendTodo(event,1 );
        else
            appendTodo(event,2);
    } else
        appendTodo(event);
}
void KOEventViewer::setJournal(Journal *event, bool clearV )
{
    if ( clearV )
        clearEvents();
    if ( mSyncMode ) {
        if ( clearV )
            appendJournal(event, 1); 
        else
            appendJournal(event, 2); 
    } else
        appendJournal(event);
}

void KOEventViewer::setEvent(Event *event)
{
    clearEvents();
    if ( mSyncMode )
        appendEvent(event, 1);
    else
        appendEvent(event);
}

void KOEventViewer::addEvent(Event *event)
{  
    if ( mSyncMode )
        appendEvent(event, 2);
    else
        appendEvent(event);
}

void KOEventViewer::clearEvents(bool now)
{
    mText = "";
    if (now) setText(mText);
}

void KOEventViewer::addText(QString text)
{
    mText.append(text);
    setText(mText);
}
QString KOEventViewer::deTag(QString text)
{
#if QT_VERSION >= 0x030000
    text.replace( '<' , "&lt;" );
    text.replace( '>' , "&gt;" );
#else
    if ( text.find ('<') >= 0 ) {
        text.replace( QRegExp("<") , "&lt;" );
    }
    if ( text.find ('>') >= 0 ) {
        text.replace( QRegExp(">") , "&gt;" );
    }
#endif
    return text;
}
void KOEventViewer::keyPressEvent ( QKeyEvent * e )
{ 
    switch ( e->key() ) {
    case Qt::Key_Return:
    case Qt::Key_Enter :
        e->ignore();
        break;
    default:
        QTextBrowser::keyPressEvent ( e );
        break; 
    }
}
