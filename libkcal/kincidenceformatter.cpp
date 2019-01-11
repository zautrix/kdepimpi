#include "kincidenceformatter.h"
#include <kstaticdeleter.h>
#include <kglobal.h>
#include <klocale.h>
#ifdef DEKTOP_VERSION
#include <kabc/stdaddressbook.h>
#define size count
#endif

KIncidenceFormatter* KIncidenceFormatter::mInstance = 0;
static KStaticDeleter<KIncidenceFormatter> insd;

QString KIncidenceFormatter::getFormattedText( Incidence * inc, bool details, bool created , bool modified  )
{
// #ifndef QT_NO_INPUTDIALOG
//   return QInputDialog::getItem( caption, label, items, current, editable );
// #else
//   return QString::null;
// #endif
    mDetails = details;
    mCreated = created ;
    mModified = modified;
    mText = "";
    if ( inc->typeID() == eventID )
        setEvent((Event *) inc );
    else if ( inc->typeID() == todoID )
        setTodo((Todo *) inc );
    return mText;
}

KIncidenceFormatter* KIncidenceFormatter::instance()
{
    if (!mInstance) {
      mInstance = insd.setObject(new KIncidenceFormatter());
  }
  return mInstance;
}
KIncidenceFormatter::~KIncidenceFormatter()
{
 if (mInstance == this)
      mInstance = insd.setObject(0); 
 //qDebug("KIncidenceFormatter::~KIncidenceFormatter ");
}
KIncidenceFormatter::KIncidenceFormatter()
{
    mColorMode = 0;
}
void KIncidenceFormatter::setEvent(Event *event)
{
    int mode = 0;
    mCurrentIncidence = event;
    bool shortDate = true;
    if ( mode == 0 ) {
        addTag("h3",deTag(event->summary()));
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
    if (event->cancelled ()) {
        mText +="<font color=\"#B00000\">";
        addTag("i",i18n("This event has been cancelled!"));
        mText.append("<br>");
        mText += "</font>";
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
        addTag("b",i18n("Location: "));
        mText.append(deTag(event->location())+"<br>");
    }

    if (event->doesRecur()) {

        QString recurText = event->recurrence()->recurrenceText();
        addTag("p","<em>" + i18n("This is a %1 recurring event.").arg(recurText ) + "</em>");

 bool ok;
        QDate start = QDate::currentDate();
        QDateTime next;
        next = event->getNextOccurence( QDateTime::currentDateTime() , &ok );
        if ( ok ) {
            addTag("p",i18n("<b>Next recurrence is on:</b>")  );
            addTag("p", KGlobal::locale()->formatDate( next.date(), shortDate ));

        } else {
            bool last;
            QDate nextd;
            nextd = event->recurrence()->getPreviousDate( QDate::currentDate() , &last );
            if ( last ) {
                addTag("p",i18n("<b>Last recurrence was on:</b>")  );
                addTag("p", KGlobal::locale()->formatDate( nextd, shortDate ));
            }
        }
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
        addTag("p",i18n("<b>Alarm on:  </b>")  + s  + ": "+KGlobal::locale()->formatDateTime( t, shortDate ));
        //addTag("p", KGlobal::locale()->formatDateTime( t, shortDate ));
        //addTag("p",s);
    }



    addTag("p",i18n("<b>Access: </b>") +event->secrecyStr() );
    // mText.append(event->secrecyStr()+"<br>");
    formatCategories(event);

    formatAttendees(event);

    if ( mCreated ) {
#ifdef DESKTOP_VERSION
        addTag("p",i18n("<b>Created: ")  +" </b>"+KGlobal::locale()->formatDateTime( event->created(), shortDate ));
#else
        addTag("p",i18n("<b>Created: ")  +" </b>");
        addTag("p", KGlobal::locale()->formatDateTime( event->created(), shortDate ));
#endif
        
    }
    if ( mModified ) {
#ifdef DESKTOP_VERSION
        addTag("p",i18n("<b>Last modified: ")  +" </b>"+KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ));
#else
        addTag("p",i18n("<b>Last modified: ")  +" </b>");
        addTag("p", KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ));
#endif
        
    }
    if ( mDetails  ) {
        if (!event->description().isEmpty()) {
            addTag("p",i18n("<b>Details: </b>"));
            addTag("p",deTag(event->description()));
        }
    }
 
}

void KIncidenceFormatter::setTodo(Todo *event )
{
    int mode = 0;
    mCurrentIncidence = event;
    bool shortDate = true;
    if (mode == 0 ) {
        addTag("h3",deTag(event->summary()));
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
    if ( event->percentComplete() == 100 && event->hasCompletedDate() ) {
        mText +="<font color=\"#B00000\">";
        addTag("i",  i18n("<p><i>Completed on %1</i></p>").arg( event->completedStr(shortDate) ) );
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
    }
   
   
    if (event->doesRecur()) {

        QString recurText = event->recurrence()->recurrenceText();
        addTag("p","<em>" + i18n("This is a %1 recurring todo.").arg(recurText ) + "</em>");
    } 
        
    if (event->hasStartDate()) {
        mText.append(i18n("<p><b>Start on:</b> %1</p>").arg(event->dtStartStr(shortDate)));
    }
        

    if (event->hasDueDate()) {
        mText.append(i18n("<p><b>Due on:</b> %1</p>").arg(event->dtDueStr(shortDate)));
    }

    if (!event->location().isEmpty()) {
        addTag("b",i18n("Location: "));
        mText.append(deTag(event->location())+"<br>");
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
        addTag("p",i18n("<b>Alarm on: ") + s  +" </b>");
        addTag("p", KGlobal::locale()->formatDateTime( t, shortDate ));
        //addTag("p",s);
    }

    addTag("p",i18n("<b>Access: </b>") +event->secrecyStr() );
    formatCategories(event);
   
    formatAttendees(event);
    if ( mCreated ) {
#ifdef DESKTOP_VERSION
        addTag("p",i18n("<b>Created: ")  +" </b>"+KGlobal::locale()->formatDateTime( event->created(), shortDate ));
#else
        addTag("p",i18n("<b>Created: ")  +" </b>");
        addTag("p", KGlobal::locale()->formatDateTime( event->created(), shortDate ));
#endif
        
    }
    if ( mModified ) {
#ifdef DESKTOP_VERSION
        addTag("p",i18n("<b>Last modified: ")  +" </b>"+KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ));
#else
        addTag("p",i18n("<b>Last modified: ")  +" </b>");
        addTag("p", KGlobal::locale()->formatDateTime( event->lastModified(), shortDate ));
#endif
        
    }
    if ( mDetails  ) {
        if (!event->description().isEmpty()) {
            addTag("p",i18n("<b>Details: </b>"));
            addTag("p",deTag(event->description()));
        }
    }
}

void KIncidenceFormatter::setJournal(Journal*  )
{

}

void KIncidenceFormatter::formatCategories(Incidence *event)
{
 if (!event->categoriesStr().isEmpty()) {
     addTag("p",i18n("<b>Categories: </b>")+event->categoriesStrWithSpace() );
     //mText.append(event->categoriesStr());
 }
}
void KIncidenceFormatter::addTag(const QString & tag,const QString & text)
{
#ifdef QT_4_COMPAT
    int number=text.count("\n");
#else
  int number=text.contains("\n");
#endif
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

void KIncidenceFormatter::formatAttendees(Incidence *event)
{
 QPtrList<Attendee> attendees = event->attendees();
    if (attendees.count()) {
        QString iconPath = KGlobal::iconLoader()->iconPath("mailappt",KIcon::Small);
        QString NOiconPath = KGlobal::iconLoader()->iconPath("nomailappt",KIcon::Small);
        addTag("h3",i18n("Organizer"));
        mText.append("<ul><li>");
#if 0
        //ndef KORG_NOKABC
       
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
#else
        mText.append(event->organizer());
#endif
        if (!iconPath.isEmpty()) {
            mText += " <a href=\"mailto:" + event->organizer() + "\">";
            mText += "<IMG src=\"" + iconPath + "\">";
            mText += "</a>\n";
        }
        mText.append("</li></ul>");

        addTag("h3",i18n("Attendees"));
        Attendee *a;
        mText.append("<ul>");
        for(a=attendees.first();a;a=attendees.next()) {
#if 0
//ndef KORG_NOKABC
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
#else
            //qDebug("nokabc ");
            mText += "<li><a href=\"uid:" + a->uid() + "\">";
            if (!a->name().isEmpty()) mText += a->name();
            else mText += a->email();
            mText += "</a>\n";
#endif

            if (!a->email().isEmpty()) {
                if (!iconPath.isEmpty()) {
                    mText += "<a href=\"mailto:" + a->name() +" "+ "<" + a->email() + ">" + "\">"; 
                    if ( a->RSVP() )
                        mText += "<IMG src=\"" + iconPath + "\">";
                    else
                        mText += "<IMG src=\"" + NOiconPath + "\">";
                    mText += "</a>\n";
                }
            }
            if (a->status() != Attendee::NeedsAction )
                mText +="[" + a->statusStr() + "] ";
            if (a->role() == Attendee::Chair )
                mText +="(" + a->roleStr().left(1) + ".)";
        }
        mText.append("</li></ul>");
    }
}

void KIncidenceFormatter::formatReadOnly(Incidence *event)
{
 if (event->isReadOnly()) {
        addTag("p","<em>(" + i18n("read-only") + ")</em>");
    }
}
QString KIncidenceFormatter::deTag(QString text)
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
