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

#include <qlayout.h>
#include <qtextbrowser.h>
#include <qtextcodec.h>
#include <qfileinfo.h>
#include <qlabel.h>

#include <qapplication.h>
#ifdef DESKTOP_VERSION 
#include <qpaintdevicemetrics.h>
#endif
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include <libkcal/calendar.h>

#ifndef KORG_NOPRINTER
#include "calprinter.h"
#endif
#include "koglobals.h"
#include "koprefs.h"
#include "koeventviewerdialog.h"
#include "koeventviewer.h"
#include <qstylesheet.h> 
#include "kowhatsnextview.h"
using namespace KOrg;

void WhatsNextTextBrowser::setSource(const QString& n)
{

    if (n.startsWith("event:")) {
        emit showIncidence(n);
        return;
    } else if (n.startsWith("todo:")) {
        emit showIncidence(n);
        return;
    } else {
        QTextBrowser::setSource(n);
    }
}
void WhatsNextTextBrowser::printMe()
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

KOWhatsNextView::KOWhatsNextView(Calendar *calendar, QWidget *parent,
                                 const char *name)
    : KOrg::BaseView(calendar, parent, name)
{
    //  mDateLabel =
    //       new QLabel(KGlobal::locale()->formatDate(QDate::currentDate()),this);
    //   mDateLabel->setMargin(2);
    //   mDateLabel->setAlignment(AlignCenter);
    setFont( KOPrefs::instance()->mWhatsNextFont );
    mView = new WhatsNextTextBrowser(this);
    connect(mView,SIGNAL(showIncidence(const QString &)),SLOT(showIncidence(const QString &)));
    QStyleSheet* stsh = mView->styleSheet();
    QStyleSheetItem * style ;
    style   = stsh->item ("h2" );
    if ( style ) {
        style->setMargin(QStyleSheetItem::MarginAll,0);  
    }
    style   = stsh->item ("h3" );
    if ( style ) {
        style->setMargin(QStyleSheetItem::MarginAll,0);  
    }
    mEventViewer = 0;

    QBoxLayout *topLayout = new QVBoxLayout(this);
    //  topLayout->addWidget(mDateLabel);
    topLayout->addWidget(mView);
    mTimer = new QTimer( this );
    connect(mTimer,SIGNAL( timeout() ),this, SLOT(updateView()));

    connect(mView->horizontalScrollBar (),SIGNAL( sliderReleased () ),this, SLOT(restartTimer()));
    connect(mView->verticalScrollBar (),SIGNAL( sliderReleased () ),this, SLOT(restartTimer()));
}

KOWhatsNextView::~KOWhatsNextView()
{
}

int KOWhatsNextView::maxDatesHint()
{
    return 0;
}

int KOWhatsNextView::currentDateCount()
{
    return 0;
}

void KOWhatsNextView::clearList()
{
    mTimer->stop();
    mView->setText(" ");
}
QPtrList<Incidence> KOWhatsNextView::selectedIncidences()
{
    QPtrList<Incidence> eventList;

    return eventList;
}

void KOWhatsNextView::printMe()
{
#ifdef DESKTOP_VERSION 
    mView->printMe();
#endif
}
void KOWhatsNextView::printPreview(CalPrinter *calPrinter, const QDate &fd,
                                   const QDate &td)
{
#ifndef KORG_NOPRINTER
    calPrinter->preview(CalPrinter::Day, fd, td);
#endif
}
void KOWhatsNextView::updateConfig()
{
    setFont( KOPrefs::instance()->mWhatsNextFont );
    updateView();

}
void KOWhatsNextView::showEvent ( QShowEvent * e )
{
    //qDebug("KOWhatsNextView::showEvent ");
    restartTimer();
    QWidget::showEvent ( e );
}
void KOWhatsNextView::hideEvent ( QHideEvent * e)
{
    //qDebug(" KOWhatsNextView::hideEvent");
    mTimer->stop();
    QWidget::hideEvent ( e );
}
void KOWhatsNextView::restartTimer()
{
    //qDebug("KOWhatsNextView::restartTimer() ");
    mTimer->start( 300000  );
    //mTimer->start( 5000  );
}
void KOWhatsNextView::updateView()
{
    //qDebug("KOWhatsNextView::updateView() ");
    if ( mTimer->isActive() )
        restartTimer();
    mCurrentMaxPrio = 5;
    //qDebug("KOWhatsNextView::updateView() ");
    //  mDateLabel->setText(KGlobal::locale()->formatDate(QDate::currentDate()));
    KIconLoader kil("korganizer");
    QString ipath;// = new QString();
    //  kil.loadIcon("korganizer",KIcon::NoGroup,32,KIcon::DefaultState,&ipath);
    //<big><big><strong>" + date + "</strong></big></big>\n";
    mText = "<table width=\"100%\">\n";
    //mText += "<tr bgcolor=\"#3679AD\"><td><h2>"; 
#ifdef DESKTOP_VERSION
    mText += "<tr bgcolor=\"#5699CD\"><td align=\"center\"><h1>";
#else
    mText += "<tr bgcolor=\"#5699CD\"><td align=\"center\"><h2>";
#endif
    //  mText += "<img src=\"";
    //   mText += ipath;
    //   mText += "\">";
    mEventDate = QDate::currentDate();
#ifdef DESKTOP_VERSION
    mText += "<font color=\"#FFFFFF\"> <em>" +  KGlobal::locale()->formatDate( mEventDate , false )+"</em></font></h1>";
#else
    mText += "<font color=\"#FFFFFF\"> <em>" +  KGlobal::locale()->formatDate( mEventDate , false )+"</em></font></h2>";
#endif
    mText += "</td></tr>\n<tr  bgcolor=\"#EAF8FA\"><td>";
    int iii;
    mTodos.clear();
    QPtrList<Event> events;  
    QPtrList<Todo> todos = calendar()->todos(); 
    Todo * todo;
    //mText += "<h2>" + i18n("Events: ") + "</h2>\n";
    int daysToShow = KOPrefs::instance()->mWhatsNextDays ;
    bool itemAdded = false;
    for ( iii = 0; iii <  daysToShow; ++iii ) {
        QString date;
        itemAdded = false;
        events.clear();
        QPtrList<Event> allevents  = calendar()->events( mEventDate, true );
        Event *eve = allevents.first();
        while(eve) {
            bool add = true;
            if ( !KOPrefs::instance()->mShowSyncEvents && eve->uid().left(15) == QString("last-syncEvent-") )
                add = false;
            if (eve->doesFloat() ) {
                int days =  eve->dtStart().date().daysTo (eve->dtEnd().date()  );
                if ( days > 2 ) { // i.e. duration 3 days or more
                    add = false;
                    QDate startDT = eve->dtStart().date();
                    QDate endDT = eve->dtEnd().date();
                    if ( eve->doesRecur() ) {
                        if ( eve->recursOn( mEventDate ) ) // first day of recurrence
                            add = true;
                        if ( eve->recursOn( mEventDate.addDays( -days ) ) ) // last day of recurrence
                            add = true;
                    } else {
                        if ( mEventDate == startDT ) {
                            add = true;
                        }
                        if ( endDT == mEventDate) {
                            add = true;
                        }
                    }
                    if (  mEventDate == QDate::currentDate() )
                        add = true;
                }
            }
            if ( add && ( ! eve->cancelled() || KOPrefs::instance()->mWhatsNextDisplayCancelledItems ) )
                events.append( eve );
            eve = allevents.next();
        }
      
        if ( iii == 0 ) { // today !!!
            todo = todos.first();
            while(todo) { //KOPrefs::instance()->mWhatsNextDisplayCancelledItems
                if ( !todo->isCompleted() &&todo->hasDueDate() && todo->dtDue().date() < mEventDate && ( ! todo->cancelled()|| KOPrefs::instance()->mWhatsNextDisplayCancelledItems ) ) {
                    if ( ! itemAdded ) {
                        appendDay ( iii, mEventDate );
                        //itemAdded = true;
                      
                    } //bool reply=false, bool notRed = true, bool appendTable = false);
                    appendEvent(todo, false, false, !itemAdded );
                    itemAdded = true;
                }
                todo = todos.next();
            }
        }
      
      
        if (events.count() > 0) {
            //  mText += "<p></p>";
            // kil.loadIcon("month",KIcon::NoGroup,22,KIcon::DefaultState,&ipath);
            // mText += "<h2>";
            //mText += "   <img src=\"";
            //mText += ipath;
            //mText += "\">"; 
            if ( ! itemAdded ) {
                appendDay ( iii, mEventDate );
              
            }
            // for first day (iii == 0)
            // we may have syncevents, or events in the past, which maybe should not be diaplayed
            // for that reason we cannot append <table> in appendDay () for iii == 0
            // we must append it in the first successful call of  appendEvent()
            Event *ev = events.first();
            while(ev) {
                //qDebug("+++++event append %s", ev->summary().latin1());
                if ( true /*!ev->recurrence()->doesRecur() || ev->recursOn( mEventDate)*/) {
                    if ( appendEvent(ev, false , iii!= 0,!itemAdded ) ) 
                        itemAdded = true;
                }
                ev = events.next();
            }
         
            //mText += "</table>\n";
        } 
      
        todo = todos.first();
        while(todo) {
            if ( ( (!todo->isCompleted()||  todo->doesRecur()) &&todo->hasDueDate() && todo->dtDue().date() == mEventDate  && ( ! todo->cancelled() || KOPrefs::instance()->mWhatsNextDisplayCancelledItems ) ) )  {
                if ( ! itemAdded ) {
                    appendDay ( iii, mEventDate );
                    //itemAdded = true;
                }
                appendEvent(todo, false , iii!= 0,!itemAdded);
                itemAdded = true;
            }
            todo = todos.next();
        }
        if ( !itemAdded && iii == 0 ) { 
            // appendDay ( iii, mEventDate );
            //mText += "<table>";
            // mText += "<b><font color=\"#000080\"><em>"+i18n("No event, nothing to do.") +"</em></font></b>\n";
            mText += "<h3 align=\"center\"><font color=\"#008000\"><em>"+i18n("No event, nothing to do.") +"</em></font></h3>\n";
            //mText +="</table>";
        }
        if ( itemAdded   )
            mText += "</table>\n";
        mEventDate = mEventDate.addDays( 1 );
    }
  
    int topmostPrios =  KOPrefs::instance()->mWhatsNextPrios;
    if (todos.count() > 0 && topmostPrios > 0 ) {
        //   kil.loadIcon("todo",KIcon::NoGroup,22,KIcon::DefaultState,&ipath);
        // mText += "<h2>";
        //<img src=\"";
        //     mText += ipath;
        //     mText += "\">";
        // mText += i18n("Overdue To-Do:") + "</h2>\n";
  
        //mText += "<ul>\n";
        bool gotone = false;
        int priority = 1; 
        int priosFound = 0;
#ifdef DESKTOP_VERSION
        mText +="<p></p>";
#endif

        mText +="<h2><em><font color=\"#000080\">" + i18n("Incomplete Todo:") + "</font></em></h2>\n";
        mText += "<ul>\n";
        while (!gotone && priority<6) {
            todo = todos.first();
            while(todo) {
                if (!todo->isCompleted() && (todo->priority() == priority)   && (! todo->cancelled() || KOPrefs::instance()->mWhatsNextDisplayCancelledItems ) ) {
                    mCurrentMaxPrio = priority - priosFound + topmostPrios -1;
                    if ( appendTodo(todo) )
                        gotone = true;
                }
                todo = todos.next();
            }
            if ( gotone ) {
                gotone = false;
                ++priosFound;
                if ( priosFound == topmostPrios )
                    break;
		else {
                  mText += "<ul>| ";
                  mText += "</ul>\n";

		}
            }
            priority++;
            // kdDebug() << "adding the todos..." << endl;
        }
        mText += "</ul>\n";
    }

    int replys = 0;
    events = calendar()->events(QDate::currentDate(), QDate(2975,12,6));
    if (events.count() > 0) {
        Event *ev = events.first();
        while(ev) {
            Attendee *me = ev->attendeeByMails(KOPrefs::instance()->mAdditionalMails,KOPrefs::instance()->email());
            if (me!=0) {
                if (me->status()==Attendee::NeedsAction && me->RSVP()) {
                    if (replys == 0) {
                        mText += "<p></p>";
                        //    kil.loadIcon("reply",KIcon::NoGroup,22,KIcon::DefaultState,&ipath);
                        //mText += "<h2>";
                        //<img src=\"";
                        //             mText += ipath;
                        //             mText += "\">";    
                        //mText += i18n("Events and To-Dos that need a reply:") + "</h2>\n";  
                        mText +="<h2><em><font color=\"#000080\">" + i18n("Events and To-Dos that need a reply:") + "</font></em></h2>\n";
                        mText += "<table>\n";
                    }
                    replys++;
                    appendEvent(ev,true);
                }
            }
            ev = events.next();
        }
    }
    todos = calendar()->todos();
    if (todos.count() > 0) {
        Todo *to = todos.first();
        while(to) {
            if ( !to->isCompleted() ){
                Attendee *me = to->attendeeByMails(KOPrefs::instance()->mAdditionalMails,KOPrefs::instance()->email());
                if (me!=0) {
                    if (me->status()==Attendee::NeedsAction && me->RSVP()) {
                        if (replys == 0) {
                            mText +="<h2><em><font color=\"#000080\">" + i18n("Events and To-Dos that need a reply:") + "</font></em></h2>\n";
                            mText += "<table>\n";
                        }
                        replys++;
                        appendEvent(to, true);
                    }
                }
            }
            to = todos.next();
        }
    }
    if (replys > 0 ) mText += "</table>\n";


    mText += "</td></tr>\n</table>\n";

    mView->setText(mText);
    mView->setFocus();

    //  QPixmap bPix = SmallIcon( "back" );
    //   qDebug("xxxxxxxxxxxxxxxxxxxxx ");
    //   QWidget* test = new QWidget();
    //  test->setBackgroundMode(FixedPixmap  );
    //  test->setBackgroundPixmap ( bPix );
    //  test->resize( 300, 400 );
    //  test->show();
    //   mView->setBackgroundMode(FixedPixmap  );
    //   mView->setBackgroundPixmap ( bPix );
    // qDebug("%s ",mText.latin1());
}

void  KOWhatsNextView::appendDay( int i, QDate eventDate )
{
    QString date;
    QString day = KGlobal::locale()->formatDate( eventDate , KOPrefs::instance()->mShortDateInViewer); 

    if ( ! KOPrefs::instance()->mShortDateInViewer && QApplication::desktop()->width() < 320 ) {
        if ( i == 0 ) {
            //mText += "<table>\n";
            return;//date = "<em><font color=\"#008000\"><em>" +  day+"</font></em>";
        }
        else if ( i == 1 )
            date = "<em><font color=\"#000080\">" + day + "</font></em>" ;
        else date = "<em><font color=\"#000080\">" + day + "</font></em>";
        mText += "<h2>" + date + "</h2>\n";
        //mText += "<big><big><strong>" + date + "</strong></big></big>\n";
        mText += "<table>\n";



    } else { 
        if ( i == 0 ) {
            //mText += "<table>\n";
            return;// date = "<font color=\"#008000\"><em>" +i18n("Today: ") +"</em>" + day+"</font>";
        }

#ifdef DESKTOP_VERSION
        else if ( i == 1 ) {
            date = "<h2><em><font color=\"#000080\">" + i18n("Tomorrow: ") + day +"</font></em></h2>" ;
        }
        else date = "<h2><em><font color=\"#000080\">" + i18n("In %1 days: ").arg( i ) +day + "</font></em></h2>";
#else
        else if ( i == 1 ) {
            date = "<em><font color=\"#000080\">" + i18n("Tomorrow: ") + day +"</font></em>" ;
        }
        else date = "<em><font color=\"#000080\">" + i18n("In %1 days: ").arg( i ) +day + "</font></em>";

#endif
        mText += "<h2>" + date + "</h2>\n";
        //mText += "<big><big><strong>" + date + "</strong></big></big>\n";
        mText += "<table>\n";
    }
}


void KOWhatsNextView::showDates(const QDate &, const QDate &)
{
    updateView();
}

void KOWhatsNextView::showEvents(QPtrList<Event>)
{
}

void KOWhatsNextView::changeEventDisplay(Event *, int action)
{
    switch(action) {
    case KOGlobals::EVENTADDED:
        updateView();
        break;
    case KOGlobals::EVENTEDITED:
        updateView();
        break;
    case KOGlobals::EVENTDELETED:
        updateView();
        break;
    default:
        updateView();
        kdDebug() << "KOWhatsNextView::changeEventDisplay(): Illegal action " << action << endl;
    }
}

bool KOWhatsNextView::appendEvent(Incidence *ev, bool reply, bool notRed, bool appendTable )
{
   
    QDateTime cdt =  QDateTime::currentDateTime();
    QDateTime noc;
    QString tempText;
    if ( appendTable && !notRed ) {
            tempText = "<table>";
    }
    bool ok = true; 
    if ( reply ) {
        noc = ev->getNextOccurence( cdt, &ok );
        if (! ok && ev->typeID() == eventID)
            return false;
    }

    QString extraMessageMultiday ("");

    if (ev->typeID() == eventID ) {
      if (ev->doesFloat() ) {
	QDate startDT = ev->dtStart().date();
	QDate endDT = ev->dtEnd().date();
	int days =  ev->dtStart().date().daysTo (ev->dtEnd().date()  );
	if ( days > 2 ) {
	  QString extraMessage = "";
	  bool skipEvent = true;
	  bool displayRemainding = false;

	  if ( ev->doesRecur() ) {
	    if ( ev->recursOn( mEventDate ) ) { // first day of recurrence
	      extraMessage = "<b>" + i18n("Starting day! ") + "</b>" ;
	    } else {
	      if ( ev->recursOn( mEventDate.addDays( -days ) ) )  {// last day of recurrence
		extraMessage = "<b>" + i18n("Last day! ") + "</b>" ;
	      } else {
		if (  mEventDate == QDate::currentDate() ) {
		  startDT = mEventDate;
		  while ( !ev->recursOn( startDT ) )
		    startDT = startDT.addDays(-1);
		  endDT = startDT.addDays( days );
		}
	      }
	    }
	  } else {
	    if ( mEventDate == startDT ) {
	      extraMessage = "<b>" + i18n("Starting day! ") + "</b>" ;
	      skipEvent = false;
	    }
	    if ( endDT == mEventDate) {
	      extraMessage = "<b>" + i18n("Last day! ") + "</b>" ;
	      skipEvent = false;
	    }
	  }
	  if (  mEventDate == QDate::currentDate() ) {
	    if ( skipEvent == true ) { // today we have no start/end day such that we display remainding duration
	      displayRemainding = true;
	      days = QDate::currentDate().daysTo( endDT );
	    }
	    skipEvent = false;
	  }
	  if( skipEvent ) { // NOT NEEDED any more       --- skipevent only for internal error detection
	    // this may not be true here
	    //KMessageBox::error(this,  endDT.toString() + " ---" +  mEventDate.toString(),"Error multiday event adding event");
	    //return false;
	  }
	  if ( displayRemainding ) 
	    extraMessageMultiday += " ["+extraMessage+i18n("Remainding duration: ")+ QString::number(days+1) +i18n(" days") + "]";
	  else
	    extraMessageMultiday += " ["+extraMessage+i18n("Duration: ")+ QString::number(days+1) +i18n(" days") + "]";
	}
      }
    }
    bool bDay = false;
    if ( ev->isBirthday()  || ev->isAnniversary()  )
        bDay = true;
    tempText += "<tr><td><b>";
    if (ev->typeID() == eventID ) {
        if (reply)  {
            if (!ev->doesFloat())
                tempText += KGlobal::locale()->formatDateTime( noc , KOPrefs::instance()->mShortDateInViewer) +": ";
            else
                tempText += KGlobal::locale()->formatDate( noc.date() , KOPrefs::instance()->mShortDateInViewer) +": ";
                
        } else {
            if (!ev->doesFloat()) {
                Event *event = static_cast<Event *>(ev);
                QDateTime st,end;
                if ( event->doesRecur() ) {
                    QDate  recDate= mEventDate;
                    int days =  event->dtStart().date().daysTo (event->dtEnd().date()  );
                    while ( ! event->recursOn(  recDate ) ) {
                        recDate = recDate.addDays( -1 );

                    }
                    st =  QDateTime ( recDate,  event->dtStart().time() );
                    end = QDateTime ( recDate.addDays( days ),  event->dtEnd().time() );
                }
                else {
                    st = event->dtStart();
                    end = event->dtEnd();
                }
      
           
                QString dateText;
                // qDebug("%s %s %s %s ", mEventDate.toString().latin1(),event->summary().latin1(), st .toString().latin1(),end.toString().latin1() );
                if ( st.date() < mEventDate )
                    dateText = "++:++-";
                else
                    dateText =  event->dtStartTimeStr() + "-";
                if ( KOPrefs::instance()->mWhatsNextTime2Lines )
                    dateText += " "; 
                if ( end.date() > mEventDate ) 
                    dateText += "++:++";
                else
                    dateText += event->dtEndTimeStr();
                if ( notRed ) 
                    tempText += dateText;
                else {
                    if ( end < cdt  ) {
                        if ( !KOPrefs::instance()->mWNViewShowsPast )
                            return false;
                        tempText += "<font color=\"#F00000\">" + dateText + "</font>";
                    }
                    else if ( st < cdt )
                        tempText += "<font color=\"#008000\">" + dateText + "</font>";
                    else
                        tempText += dateText;
                
                }
    
            } else {
                if ( bDay ) {
                    
                    if ( ev->isBirthday())
                        tempText += "<font color=\"#00B000\">" + i18n("Birthday") +":</font>";
                    else
                        tempText += "<font color=\"#00B0000\">" + i18n("Anniversary")+":</font>";
                } else {
                    if ( ((Event*)ev)->isMultiDay() ) {
                        QString dfs = KGlobal::locale()->dateFormatShort();
                        KGlobal::locale()->setDateFormatShort("%d.%b");
			QDate startDT = ev->dtStart().date();
			QDate endDT = ev->dtEnd().date();
			int days =  ev->dtStart().date().daysTo (ev->dtEnd().date()  );
			if ( ev->doesRecur() ) {
			  if ( ev->recursOn( mEventDate ) ) // first day of recurrence
			    {
			      startDT = mEventDate;
			      endDT = startDT.addDays( days );
			    } else 
			    if (   ev->recursOn( mEventDate.addDays( -days ) )   ) {
			      endDT = mEventDate;
			      startDT = endDT.addDays( -days );
			    } else
			      if (  mEventDate == QDate::currentDate() ) {
				startDT = mEventDate;
				while ( !ev->recursOn( startDT ) )
				  startDT = startDT.addDays(-1);
				endDT = startDT.addDays( days );
			      }
			}
                        tempText +=KGlobal::locale()->formatDate(startDT, true, KLocale::Userdefined) + "-";
                        if ( KOPrefs::instance()->mWhatsNextTime2Lines )
                            tempText += " ";
                        tempText +=KGlobal::locale()->formatDate(endDT, true, KLocale::Userdefined);
                        KGlobal::locale()->setDateFormatShort(dfs);
                    } else {
		      if ( ((Event*)ev)->isHoliday() ) {
			//KOPrefs::instance()->mHolidayColor
                        tempText += "<font color=\"" + KOPrefs::instance()->mHolidayColor.name() 
			  + "\">" + i18n("Holiday") +":</font>";
		      } 
		      else
                        tempText += " " ;//i18n("Allday:");
                    }
                }

            }
        }
    } else {
        mTodos.append( ev );
        tempText += i18n("ToDo:"); 
        if ( KOPrefs::instance()->mWhatsNextTime2Lines )
            tempText += " "; 
        if (reply)  {
            tempText += " "; 
            if ( noc != cdt ) {
                tempText += KGlobal::locale()->formatDate( noc.date() , KOPrefs::instance()->mShortDateInViewer) +": ";
            }
        } else {
            if ( ((Todo*)ev)->dtDue().date() < QDate::currentDate() ) {
                // tempText +="<font color=\"#F00000\">" + i18n("O-due!") + "</font>";
                QString dfs = KGlobal::locale()->dateFormatShort();
                KGlobal::locale()->setDateFormatShort("%d.%b");
                tempText +="<font color=\"#F00000\">" + KGlobal::locale()->formatDate(((Todo*)ev)->dtDue().date(), true, KLocale::Userdefined) + "</font>";
                KGlobal::locale()->setDateFormatShort(dfs);
		int days = ((Todo*)ev)->dtDue().date().daysTo( QDate::currentDate() );
		if ( days == 1 )
		  extraMessageMultiday = " (1"  +i18n(" day") + " " + i18n("overdue") + ")";
		else
		  extraMessageMultiday = " (" + QString::number(days) +i18n(" days") + " "+ i18n("overdue") + ")";
            } else {
                if (!ev->doesFloat() )
                    if( ( (Todo*)ev)->dtDue()  < cdt  ) {
                        tempText +="<font color=\"#F00000\">" + ((Todo*)ev)->dtDueTimeStr() + "</font>";


                    } else 
                        tempText +=((Todo*)ev)->dtDueTimeStr();
                mTodos.append( ev );
            }
        }
    }
    tempText += "</b></td><td>";
    bool needClose = false;
    if ( ev->cancelled() ) {
        tempText += "<font color=\"#F00000\">[c";
        needClose =true;

    }
    if ( ev->isAlarmEnabled() && ev->alarmEnabled()) { 
        if ( !needClose)
            tempText +="[";
        tempText += "a";
        needClose =true;

    }
    if ( ev->description().length() > 0 ) {
        if ( !needClose)
            tempText +="[";
        tempText += "i";
        needClose =true;
    }
    if ( ev->doesRecur() ) {
        if ( !needClose)
            tempText +="[";
        tempText += "r";
        needClose =true;
    }
    if ( needClose ) {
        tempText += "] ";
    }
    if ( ev->cancelled() ) 
        tempText += "</font>";
    tempText += "<a ";
    if (ev->typeID() == eventID ) tempText += "href=\"event:";
    if (ev->typeID() == todoID ) tempText += "href=\"todo:";
    tempText += ev->uid() + "\">";
    if ( ev->summary().length() > 0 )
        tempText += ev->summary();
    else
        tempText += i18n("-no summary-"); 
    if ( bDay  ) {
        noc = ev->getNextOccurence( cdt.addDays(-1), &ok );
        if ( ok ) {
            int years = 0;
            if ( ev->typeID() == todoID  ) {
                years = noc.date().year() -((Todo*)ev)->dtDue().date().year();
            } else
                years = noc.date().year() - ev->dtStart().date().year();
            tempText += i18n(" (%1 y.)"). arg( years );
        }
    } 
    
    tempText += "</a>";
    if ( KOPrefs::instance()->mWNViewShowLocation )
        if ( !ev->location().isEmpty() )
            tempText += " ("+ev->location() +")";
    if ( ev->relatedTo() && KOPrefs::instance()->mWNViewShowsParents)
            tempText += " ["+ev->relatedTo()->summary() +"]";
    
    tempText += extraMessageMultiday;
    tempText += "</td></tr>\n";
    mText += tempText;
    return true;
}

bool KOWhatsNextView::appendTodo(Incidence *ev, QString ind , bool isSub )
{
    if ( mTodos.find( ev ) != mTodos.end() ) return false;

    mTodos.append( ev );
    if ( !isSub )
        mText += "<p>";
    else
        mText += "<li>";
    mText += "[" +QString::number(ev->priority()) + "/" + QString::number(((Todo*)ev)->percentComplete())+"%] ";


    mText += ind;
    bool needClose = false;
    if ( ev->cancelled() ) {
        mText += "<font color=\"#F00000\">[c";
        needClose =true;

    }
    if ( ev->isAlarmEnabled() && ev->alarmEnabled() ) { 
        if ( !needClose)
            mText +="[";
        mText += "a";
        needClose =true;

    }
   
    if ( ev->description().length() > 0 ) {
        if ( !needClose)
            mText +="[";
        mText += "i";
        needClose =true;
    }  
    if ( ev->doesRecur() ) {
        if ( !needClose)
            mText +="[";
        mText += "r";
        needClose =true;
    }
    //  if ( ev->recurrence()->doesRecur() ) {
    //       if ( !needClose)
    //           mText +="(";
    //       mText += "r";
    //       needClose =true;
    //   }
    if ( needClose )
        mText += "] ";
    if ( ev->cancelled() ) 
        mText += "</font>";
    mText += "<a href=\"todo:" + ev->uid() + "\">";
    if ( ev->summary().length() > 0 )
        mText += ev->summary();
    else
        mText += i18n("-no summary-"); 
    mText += "</a>"; 
    if ( ((Todo*)ev)->hasDueDate () ) {
        QString year = "";
        int ye = ((Todo*)ev)->dtDue().date().year();
        if ( QDateTime::currentDateTime().date().year() != ye )
            year = QString::number( ye );
        QString dfs = KGlobal::locale()->dateFormatShort();
        KGlobal::locale()->setDateFormatShort("%d.%b");
        mText +="<font color=\"#00A000\"> [" + KGlobal::locale()->formatDate(((Todo*)ev)->dtDue().date(), true, KLocale::Userdefined) + "."+ year +"]</font>";
        KGlobal::locale()->setDateFormatShort(dfs);
    }
    if ( KOPrefs::instance()->mWNViewShowLocation )
        if ( !ev->location().isEmpty() )
            mText += " ("+ev->location() +")";
    if ( !isSub ) {
        if ( ((Todo*)ev)->relatedTo()&& KOPrefs::instance()->mWNViewShowsParents)
            mText += " ["+ev->relatedTo()->summary() +"]";
        mText += "</p>\n";
    }
    else {
        ind += "-";
        mText += "</li>\n";
    }
    QPtrList<Incidence> Relations = ev->relations();
    Incidence *to;
    for (to=Relations.first();to;to=Relations.next()) {
        if (!((Todo*)to)->isCompleted() && 
            ((Todo*)to)->priority() <= mCurrentMaxPrio && 
            (! to->cancelled() || KOPrefs::instance()->mWhatsNextDisplayCancelledItems ))
            appendTodo( to, ind , true );
    } 
    
    return true;
}

/*
  void KOWhatsNextView::createEventViewer()
  {
  if (!mEventViewer) {
      
  mEventViewer = new KOEventViewerDialog(this);
  }
  }
*/
void KOWhatsNextView::setEventViewer(KOEventViewerDialog* v )
{
    mEventViewer = v;
}

// TODO: Create this function in CalendarView and remove it from here
void KOWhatsNextView::showIncidence(const QString &uid)
{
 
    if ( !mEventViewer  ) {
        qDebug("KOWhatsNextView::showIncidence::sorry, no event viewer set ");
        return;
    }
    //kdDebug() << "KOWhatsNextView::showIncidence(): " << uid << endl;
    //qDebug("KOWhatsNextView::showIncidence %s ", uid.latin1());
    if (uid.startsWith("event:")) {
#ifdef DESKTOP_VERSION
        Event *event = calendar()->event(uid.mid(8));
#else
        Event *event = calendar()->event(uid.mid(6));
#endif
        //qDebug("event %d uid %s ", event, uid.mid(6).latin1());
        if (!event) return;
        //createEventViewer();
        mEventViewer->setEvent(event);
    } else if (uid.startsWith("todo:")) {
#ifdef DESKTOP_VERSION
        Todo *todo = calendar()->todo(uid.mid(7));
#else
        Todo *todo = calendar()->todo(uid.mid(5));
#endif
        if (!todo) return;
        //createEventViewer();
        mEventViewer->setTodo(todo);
    } else {
        return;

  }
  mEventViewer->showMe();
  mEventViewer->raise();
}
