/*
    This file is part of KOrganizer.

    Copyright (c) 1998 Preston Brown
    Copyright (c) 2003 Reinhold Kainhofer <reinhold@kainhofer.com>

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

#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qptrlist.h>
#include <qintdict.h>
#include <qfontmetrics.h> 

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kcalendarsystem.h>
#include <kprinter.h>

#include <libkcal/todo.h>
#include <libkcal/event.h>
#include <libkcal/calendar.h>

#include "koprefs.h"
#include "koglobals.h"
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif
#include "cellitem.h"

#include "calprintbase.h"

#ifndef KORG_NOPRINTER
//#include "calprintbase.moc"

int CalPrintBase::mHeaderHeight=90;
int CalPrintBase::mSubHeaderHeight=20;
int CalPrintBase::mMargin=36;



class CalPrintBase::TodoParentStart
{
  public:
    TodoParentStart( QRect pt = QRect(), bool page = true )
      : mRect( pt ), mSamePage( page ) {}

    QRect mRect;
    bool mSamePage;
};

class PrintCellItem : public KOrg::CellItem
{
  public:
    PrintCellItem( Event *event, const QDate &day )
      : mEvent( event ), mDay( day )
    {
    }
  
    Event *event() const { return mEvent; }

    QString label() const { return mEvent->summary(); }
    
    bool overlaps( KOrg::CellItem *o ) const
    {
      PrintCellItem *other = static_cast<PrintCellItem *>( o );

      QDateTime start = event()->dtStart();
      QDateTime end = event()->dtEnd();
      if ( event()->doesRecur() ) {
        start.setDate( mDay );
        end.setDate( mDay );
      }
      QDateTime otherStart = other->event()->dtStart();
      QDateTime otherEnd = other->event()->dtEnd();
      if ( other->event()->doesRecur() ) {
        otherStart.setDate( mDay );
        otherEnd.setDate( mDay );
      }

#if 0
      kdDebug() << "PrintCellItem::overlaps() " << event()->summary()
                << " <-> " << other->event()->summary() << endl;
      kdDebug() << "  start     : " << start.toString() << endl;
      kdDebug() << "  end       : " << end.toString() << endl;
      kdDebug() << "  otherStart: " << otherStart.toString() << endl;
      kdDebug() << "  otherEnd  : " << otherEnd.toString() << endl;
#endif
      
      return !( otherStart >= end || otherEnd <= start );
    }
    
  private:
    Event *mEvent;
    QDate mDay;
};

void setCategoryColors( QPainter &p, Incidence *incidence)
{
  QColor bgColor;
  QStringList categories = incidence->categories();
  QString cat = categories.first();
  if (cat.isEmpty())
    bgColor = KOPrefs::instance()->mEventColor;
  else
    bgColor = *(KOPrefs::instance()->categoryColor(cat));
  QColor textColor = Qt::black;//getTextColor(bgColor);
  p.setPen( textColor );
  p.setBrush( bgColor );
}



CalPrintBase::CalPrintBase( KPrinter *printer, Calendar *cal, KConfig *cfg )
  : QObject(), mPrinter( printer ), mCalendar( cal ), mConfig( cfg )
{
}

CalPrintBase::~CalPrintBase()
{
}



QWidget *CalPrintBase::configWidget( QWidget *w )
{
  QFrame *wdg = new QFrame( w );
  QVBoxLayout *layout = new QVBoxLayout( wdg );

  QLabel *title = new QLabel( description(), wdg );
  QFont titleFont( title->font() );
  titleFont.setPointSize( 20 );
  titleFont.setBold( true );
  title->setFont( titleFont );

  layout->addWidget( title );
  layout->addWidget( new QLabel( longDescription(), wdg ) );
  layout->addSpacing( 20 );
  layout->addWidget( new QLabel( i18n("This printing style does not "
                                      "have any configuration options."),
                                 wdg ) );
  layout->addStretch();
  return wdg;
}
#include <qapplication.h>
void CalPrintBase::doPrint()
{
  QPainter p;

  mPrinter->setColorMode( (mUseColors)?(KPrinter::Color):(KPrinter::GrayScale));
  //#define DUMMY_PRINTER
#ifdef DUMMY_PRINTER

  static QWidget* dummy = 0;
  if ( ! dummy )
      dummy = new QWidget( );
  else {
      delete dummy;
      dummy = new QWidget( );

  }
  dummy->resize( 1024, 1024 );
  dummy->repaint();
  dummy->show();
  dummy->raise();
  dummy->setBackgroundColor( Qt::white);
  qApp->processEvents();
  p.begin(dummy);
#else
  p.begin(mPrinter);
#endif
  // the painter initially begins at 72 dpi per the Qt docs.
  // we want half-inch margins.
  p.setViewport(mMargin, mMargin,
                p.viewport().width()-mMargin,
                p.viewport().height()-mMargin);
  int pageWidth = p.viewport().width();
  int pageHeight = p.viewport().height();

  print(p, pageWidth, pageHeight);

  p.end();
}

void CalPrintBase::doLoadConfig()
{
  if ( mConfig ) {
      mConfig->setGroup(description());
      //KConfigGroupSaver saver( mConfig, description() );
    mConfig->sync();
    QDateTime currDate( QDate::currentDate() );
    mFromDate = mConfig->readDateTimeEntry( "FromDate", &currDate ).date();
    mToDate = mConfig->readDateTimeEntry( "ToDate" ).date();
    mUseColors = mConfig->readBoolEntry( "UseColors", false );
    loadConfig();
  } else {
      qDebug("no config in CalPrintBase::doLoadConfig ");
  }
}

void CalPrintBase::doSaveConfig()
{
  if ( mConfig ) {
      mConfig->setGroup(description());
      // KConfigGroupSaver saver( mConfig, description() );
    saveConfig();
    mConfig->writeEntry( "FromDate", QDateTime( mFromDate ) );
    mConfig->writeEntry( "ToDate", QDateTime( mToDate ) );
    mConfig->writeEntry( "UseColors", mUseColors );
    mConfig->sync();
  } else {
      qDebug("no config in CalPrintBase::doSveConfig ");
  }
}

///////////////////////////////////////////////////////////////////////////////

void CalPrintBase::drawHeader( QPainter &p, QString title,
    const QDate &month1, const QDate &month2,
    int x, int y, int width, int height )
{
  p.drawRect(x, y, width, height);
 //  p.fillRect( x+1, y+1,
//               width-2,height-2,
//               QBrush(Dense7Pattern) );

  QString myOwner(mCalendar->getOwner());

  int right=x+width;

  // print previous month for month view, print current for todo, day and week
  int smallMonthWidth=width/4-10;
  if (smallMonthWidth>100) smallMonthWidth=100; 
  if (month2.isValid()) {
    right -= (10+smallMonthWidth);
    drawSmallMonth(p, QDate(month2.year(), month2.month(), 1),
                   right, y+2, smallMonthWidth, height-4);
    right-=10;
  }
  if (month1.isValid()) {
    right -= (10+smallMonthWidth);
    drawSmallMonth(p, QDate(month1.year(), month1.month(), 1),
                   right, y+2, smallMonthWidth, height-4);
    right-=10;
  }

   // Print the titles...
  QFont font("helvetica", 18, QFont::Bold);
  p.setFont(font);
  QRect textRect( x+5, y+5, right-10-x, height-10 );
  p.drawText( textRect, Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak, title );
}


void CalPrintBase::drawSmallMonth(QPainter &p, const QDate &qd,
    int x, int y, int width, int height)
{
  bool firstCol = true;
  QDate monthDate(QDate(qd.year(), qd.month(), 1));
  QDate monthDate2;
  int month = monthDate.month();

  // draw the title
  p.setFont(QFont("helvetica", 7, QFont::Bold));
  //  int lineSpacing = p.fontMetrics().lineSpacing();
  const KCalendarSystem *calSys = KOGlobals::self()->calendarSystem();
  p.drawText(x, y, width, height/4, AlignCenter, calSys->monthName( qd ) +" "+ QString::number( qd.year() ) );

  int cellWidth = width/7;
  int cellHeight = height/8;
  QString tmpStr;

  // correct begin of week
  int weekdayCol = weekdayColumn( qd.dayOfWeek() );
  monthDate2 = monthDate.addDays(-weekdayCol);

  // draw days of week
   p.setFont(QFont("helvetica", 7, QFont::Bold));
  for (int col = 0; col < 7; col++) {
    // tmpStr.sprintf("%c",(const char*)monthDate2.dayName(monthDate2.dayOfWeek()));
    tmpStr=calSys->weekDayName( monthDate2 )[0].upper();
    p.drawText(x+col*cellWidth, y+height/4, cellWidth, cellHeight,
               AlignCenter, tmpStr);
    monthDate2 = monthDate2.addDays(1);
  }

  // draw separator line
  p.drawLine(x, y+height/4+cellHeight, x+width, y+height/4+cellHeight);

  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 7; col++) {
      if (monthDate.month() != month)
        break;
      if (firstCol) {
        firstCol = true;
        col = weekdayColumn( monthDate.dayOfWeek() );
      }
      p.drawText( x+col*cellWidth,
                  y+height/4+cellHeight+(row*cellHeight),
                  cellWidth, cellHeight, AlignCenter,
                  tmpStr.setNum(monthDate.day()) );
      monthDate = monthDate.addDays(1);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////

/*
 * This routine draws a header box over the main part of the calendar
 * containing the days of the week.
 */
void CalPrintBase::drawDaysOfWeek(QPainter &p,
    const QDate &fromDate, const QDate &toDate,
    int x, int y, int width, int height)
{
  int cellWidth = width/(fromDate.daysTo( toDate )+1);
  int currx=x;
  QDate cellDate(fromDate);

  while (cellDate<=toDate) {
    drawDaysOfWeekBox(p, cellDate, currx, y, cellWidth, height);
    currx+=cellWidth;
    cellDate = cellDate.addDays(1);
  }
}


void CalPrintBase::drawDaysOfWeekBox(QPainter &p, const QDate &qd,
    int x, int y, int width, int height)
{
  const KCalendarSystem *calSys = KOGlobals::self()->calendarSystem();

  p.setFont( QFont( "helvetica", 10, QFont::Bold ) );
  p.drawRect( x, y, width, height );
//   p.fillRect( x+1, y+1,
//               width-2, height-2,
//               QBrush( Dense7Pattern ) );
  p.drawText( x+5, y, width-10, height, AlignCenter | AlignVCenter,
             calSys->weekDayName( qd ) );
}


void CalPrintBase::drawTimeLine(QPainter &p,
    const QTime &fromTime, const QTime &toTime,
    int x, int y, int width, int height)
{
  p.drawRect(x, y, width, height);

  int totalsecs=fromTime.secsTo(toTime);
  float minlen=(float)height*60./(float)totalsecs;
  float cellHeight=(60.*(float)minlen);
  float currY=y;

  QTime curTime( fromTime );
  QTime endTime( toTime );
  if ( fromTime.minute() > 30 )
    curTime = QTime( fromTime.hour()+1, 0, 0 );
  else if ( fromTime.minute() > 0 ) {
    curTime = QTime( fromTime.hour(), 30, 0 );
    float yy = currY + minlen*(float)fromTime.secsTo( curTime )/60.;
    p.drawLine( x+width/2, (int)yy, x+width, (int)yy );
    curTime = QTime( fromTime.hour()+1, 0, 0 );
  }
  currY += ( fromTime.secsTo(curTime)*minlen/60 );

  while ( curTime < endTime ) {
    p.drawLine( x, (int)currY, x+width, (int)currY );
    int newY=(int)(currY+cellHeight/2.);
    QString numStr;
    if (newY < y+height) {
      p.drawLine(x+width/2, (int)newY, x+width, (int)newY);
      // draw the time:
      if ( !KGlobal::locale()->use12Clock() ) {
        numStr.setNum(curTime.hour());
        if (cellHeight > 30) {
          p.setFont(QFont("helvetica", 16, QFont::Bold));
        } else {
          p.setFont(QFont("helvetica", 12, QFont::Bold));
        }
        p.drawText(x+2, (int)currY+2, width/2-2, (int)cellHeight,
                  AlignTop|AlignRight, numStr);
        p.setFont(QFont("helvetica", 10, QFont::Normal));
        p.drawText(x+width/2, (int)currY+2, width/2+2, (int)(cellHeight/2)-3,
                  AlignTop | AlignLeft, "00");
      } else {
        QTime time( curTime.hour(), 0 );
        numStr = KGlobal::locale()->formatTime( time );
        p.setFont(QFont("helvetica", 14, QFont::Bold));
        p.drawText(x+2, (int)currY+2, width-4, (int)cellHeight/2-3,
                  AlignTop|AlignLeft, numStr);
      }
      currY+=cellHeight;
    } // enough space for half-hour line and time
    if (curTime.secsTo(endTime)>3600)
      curTime=curTime.addSecs(3600);
    else curTime=endTime;
  } // currTime<endTime
}


///////////////////////////////////////////////////////////////////////////////

/** prints the all-day box for the agenda print view. if expandable is set,
    height is the cell height of a single cell, and the returned height will
    be the total height used for the all-day events. If !expandable, only one
    cell will be used, and multiple events are concatenated using ", ".
*/
void CalPrintBase::drawAllDayBox(QPainter &p, Event::List &eventList,
    const QDate &qd, bool expandable,
    int x, int y, int width, int &height)
{
  Event::List::Iterator it, itold;

  int offset=y;

  //p.setBrush(QBrush(Dense7Pattern));
  QPen oldPen(p.pen());
  QColor oldBgColor(p.backgroundColor());
  QBrush oldBrush(p.brush());
  QString multiDayStr;

  it = eventList.begin();
#ifndef KORG_NOPLUGINS
  QString hstring(KOCore::self()->holiday(qd));
  if (!hstring.isEmpty()) {
    Event*holiday=new Event();
    holiday->setDtStart(qd);
    holiday->setDtEnd(qd);
    holiday->setFloats(true);
    holiday->setCategories(i18n("Holiday"));
    eventList.prepend(holiday);
  }
#endif
  Event *currEvent = 0;
  // First, print all floating events
  while( it!=eventList.end() ) {
    currEvent=*it;
    itold=it;
    ++it;
    if ( currEvent->doesFloat() ) {
      // set the colors according to the categories
        QString text = currEvent->summary() ;
        if ( ! currEvent->location().isEmpty() )
            text += " ("+currEvent->location()+")";
        if (expandable) {
        if (mUseColors)
          setCategoryColors(p, currEvent);

        p.drawRect( x, offset, width, height );
       
        p.drawText( x+5, offset+5, width-10, height-10, 
                    AlignCenter | AlignVCenter | AlignJustify | WordBreak,
                    text );
        // reset the colors
        p.setBrush( oldBrush );
        p.setPen( oldPen );
        p.setBackgroundColor(oldBgColor);

        offset += height;
      } else {
          //if (!multiDayStr.isEmpty()) multiDayStr+=", ";
        multiDayStr += text+"\n";
      }
      eventList.remove( itold );
    }
  }

  if (!expandable) {
    p.drawRect(x, offset, width, height);
    if (!multiDayStr.isEmpty()) {
        // p.fillRect(x+1, offset+1, width-2, height-2, QBrush(Dense5Pattern) );
      p.drawText( x+5, offset+5, width-10, height-10, 
                  AlignLeft | AlignTop | AlignJustify , 
                  multiDayStr);
    }
  } else {
    height=offset-y;
  }
}


void CalPrintBase::drawAgendaDayBox( QPainter &p, Event::List &events,
                                     const QDate &qd, bool expandable,
                                     QTime &fromTime, QTime &toTime,
                                     int x, int y, int width, int height )
{
  p.drawRect( x, y, width, height );

  Event *event;

  if ( expandable ) {
    // Adapt start/end times to include complete events
    Event::List::ConstIterator it;
    for ( it = events.begin(); it != events.end(); ++it ) {
      event = *it;
      if ( event->dtStart().time() < fromTime )
        fromTime = event->dtStart().time();
      if ( event->dtEnd().time() > toTime )
        toTime = event->dtEnd().time();
    }
  }

  // Show at least one hour
  if ( fromTime.secsTo( toTime ) < 3600 ) {
    fromTime = QTime( fromTime.hour(), 0, 0 );
    toTime = fromTime.addSecs( 3600 );
  }

  // calculate the height of a cell and of a minute
  int totalsecs = fromTime.secsTo( toTime );
  float minlen = height * 60. / totalsecs;
  float cellHeight = 60. * minlen;
  float currY = y;

  // print grid:
  QTime curTime( QTime( fromTime.hour(), 0, 0 ) );
  currY += fromTime.secsTo( curTime ) * minlen / 60;

  while ( curTime < toTime && curTime.isValid() ) {
    if ( currY > y ) p.drawLine( x, int( currY ), x + width, int( currY ) );
    currY += cellHeight / 2;
    if ( ( currY > y ) && ( currY < y + height ) ) {
      QPen oldPen( p.pen() );
      p.setPen( QColor( 192, 192, 192 ) );
      p.drawLine( x, int( currY ), x + width, int( currY ) );
      p.setPen( oldPen );
    } // enough space for half-hour line
    if ( curTime.secsTo( toTime ) > 3600 )
      curTime = curTime.addSecs( 3600 );
    else curTime = toTime;
    currY += cellHeight / 2;
  }

  QDateTime startPrintDate = QDateTime( qd, fromTime );
  QDateTime endPrintDate = QDateTime( qd, toTime );

  // Calculate horizontal positions and widths of events taking into account
  // overlapping events

  QPtrList<KOrg::CellItem> cells;
  cells.setAutoDelete( true );

  Event::List::ConstIterator itEvents;
  for( itEvents = events.begin(); itEvents != events.end(); ++itEvents ) {
    cells.append( new PrintCellItem( *itEvents, qd ) );
  }

  QPtrListIterator<KOrg::CellItem> it1( cells );
  for( it1.toFirst(); it1.current(); ++it1 ) {
    KOrg::CellItem *placeItem = it1.current();

    KOrg::CellItem::placeItem( cells, placeItem );
  }

  QPen oldPen = p.pen();
  QColor oldBgColor = p.backgroundColor();
  QBrush oldBrush = p.brush();

  p.setFont( QFont( "helvetica", 10 ) );
  //p.setBrush( QBrush( Dense7Pattern ) );

  for( it1.toFirst(); it1.current(); ++it1 ) {
    PrintCellItem *placeItem = static_cast<PrintCellItem *>( it1.current() );

    drawAgendaItem( placeItem, p, qd, startPrintDate, endPrintDate, minlen, x,
                    y, width );

    p.setBrush( oldBrush );
    p.setPen( oldPen );
    p.setBackgroundColor( oldBgColor );
  }
  
  p.setBrush( QBrush( NoBrush ) );
}


void CalPrintBase::drawAgendaItem( PrintCellItem *item, QPainter &p,
                                   const QDate &qd,
                                   const QDateTime &startPrintDate,
                                   const QDateTime &endPrintDate,
                                   float minlen, int x, int y, int width )
{
  Event *event = item->event();

  // set the colors according to the categories
  if ( mUseColors ) setCategoryColors( p, event );
  else p.setBrush( Qt::white );
  // start/end of print area for event
  QDateTime startTime = event->dtStart();
  QDateTime endTime = event->dtEnd();
  if ( event->doesRecur() ) {
    startTime.setDate( qd );
    endTime.setDate( qd );
  }
  if ( ( startTime < endPrintDate && endTime > startPrintDate ) ||
       ( endTime > startPrintDate && startTime < endPrintDate ) ) {
    if ( startTime < startPrintDate ) startTime = startPrintDate;
    if ( endTime > endPrintDate ) endTime = endPrintDate;
    int eventLength = int( startTime.secsTo( endTime ) / 60. * minlen );
    int currentyPos = int( y + startPrintDate.secsTo( startTime ) *
                           minlen / 60. );
    int currentWidth = width / item->subCells();
    int currentX = x + item->subCell() * currentWidth;
    QString text = KGlobal::locale()->formatTime(event->dtStart().time())+
                    "-"+KGlobal::locale()->formatTime(event->dtEnd().time())+
    " "+event->summary();
    if ( !event->location().isEmpty() )
        text += " (" +event->location()+")";
    // p.save();
    QPen pe = p.pen();
    pe.setWidth( 2 );
    p.setPen( pe );
    p.drawRect( currentX, currentyPos+1, currentWidth+1, eventLength+1 );
    p.drawText( currentX+3, currentyPos+2, currentWidth-5, eventLength-3,
                AlignLeft | AlignTop | AlignJustify | WordBreak,
                text);
    // p.restore();
  }
}

void CalPrintBase::drawDayBox(QPainter &p, const QDate &qd,
    int x, int y, int width, int height,
    bool fullDate)
{
  QString dayNumStr;
  QString ampm;
  const KLocale*local = KGlobal::locale();


  // This has to be localized
  if (fullDate) {
    dayNumStr = local->formatDate(qd);
  } else {
    dayNumStr = QString::number( qd.day() );
  }

  p.eraseRect( x, y, width, height );
  p.drawRect( x, y, width, height );
  // p.fillRect( x+1, y+1, width-2,height, QBrush(Dense7Pattern) );
  p.drawRect( x, y, width, mSubHeaderHeight );
  //p.fillRect( x+1, y+1, width-2, mSubHeaderHeight-2, QBrush(Dense7Pattern) );
  QString hstring;
#ifndef KORG_NOPLUGINS
  hstring=KOCore::self()->holiday(qd);
#endif

  if (!hstring.isEmpty()) {
    p.setFont( QFont( "helvetica", 8, QFont::Bold, true ) );

    p.drawText( x+5, y, width-25, mSubHeaderHeight, AlignLeft | AlignVCenter,
                hstring );
  }
  p.setFont(QFont("helvetica", 10, QFont::Bold));
  if ( fullDate) {
      // use short date format, if long date is too long
      QFontMetrics fm ( p.font() );
      if ( fm.width( dayNumStr ) > width -10 )
          dayNumStr = local->formatDate(qd, true);
  }
  p.drawText(x+5, y, width-10, mSubHeaderHeight, AlignRight | AlignVCenter,
             dayNumStr);
  Event::List eventList;
  eventList.fill( mCalendar->events( qd, true ));
  Todo::List todos;
  todos.fill( mCalendar->todos( qd ));
  QString outStr;
  p.setFont( QFont( "helvetica", 8 ) );
  int lineSpacing = p.fontMetrics().lineSpacing();

  int textY=mSubHeaderHeight+3; // gives the relative y-coord of the next printed entry
  Event::List::ConstIterator it;
  int entryCount = eventList.count() +todos.count();
  if ( p.fontMetrics().lineSpacing()* entryCount > height-textY ) {
      if ( (p.fontMetrics().lineSpacing()-1) * entryCount > height-textY ) {
          p.setFont( QFont( "helvetica", 7 ) );
          if ( (p.fontMetrics().lineSpacing()-1) * entryCount > height-textY ) 
                p.setFont( QFont( "helvetica", 6 ) );
      }
      lineSpacing = p.fontMetrics().lineSpacing()-1;
  }
  // qDebug("fm %d %d %d ",p.fontMetrics().height(), eventList.count() , height-textY );
  for( it = eventList.begin(); it != eventList.end() && textY<height; ++it ) {
    Event *currEvent = *it;
    if (currEvent->doesFloat() || currEvent->isMultiDay())
      outStr = currEvent->summary();

    else {
        if ( fullDate ) {
            outStr = KGlobal::locale()->formatTime( currEvent->dtStart().time())+
                "-"+KGlobal::locale()->formatTime( currEvent->dtEnd().time())+
                " "+ currEvent->summary();
            if ( ! currEvent->location().isEmpty() )
                outStr += " (" + currEvent->location()+")";
        } else {
            QTime t1 = currEvent->dtStart().time();
            
            outStr = local->formatTime(t1);
            outStr += " " + currEvent->summary();
        }
        
    } // doesFloat

    p.drawText(x+5, y+textY, width-10, lineSpacing,
               AlignLeft|AlignBottom, outStr);
    textY+=lineSpacing;
  }

  if ( textY<height ) {
    
    Todo::List::ConstIterator it2;
    for( it2 = todos.begin(); it2 != todos.end() && textY<height; ++it2 ) {
      Todo *todo = *it2;
      QString text;
      if (todo->hasDueDate()) {
        if (!todo->doesFloat()) {
          text += KGlobal::locale()->formatTime(todo->dtDue().time());
          text += " ";
        }
      }
      text += i18n("To-Do: %1").arg(todo->summary());

      p.drawText(x+5, y+textY, width-10, lineSpacing,
                AlignLeft|AlignBottom, text);
      textY+=lineSpacing;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////

void CalPrintBase::drawWeek(QPainter &p, const QDate &qd,
    int x, int y, int width, int height)
{
  QDate weekDate = qd;
  bool portrait = ( mPrinter->orientation() == KPrinter::Portrait  );
  int cellWidth, cellHeight;
  int vcells;
  if (portrait) {
    cellWidth = width/2;
    vcells=3;
  } else {
    cellWidth = width/6;
    vcells=1;
  }
  cellHeight = height/vcells;

  // correct begin of week
  int weekdayCol = weekdayColumn( qd.dayOfWeek() );
  weekDate = qd.addDays( -weekdayCol );

  for (int i = 0; i < 7; i++, weekDate = weekDate.addDays(1)) {
    if (i<5) {
      drawDayBox(p, weekDate, x+cellWidth*(int)(i/vcells), y+cellHeight*(i%vcells),
        cellWidth, cellHeight, true);
    } else if (i==5) {
      drawDayBox(p, weekDate, x+cellWidth*(int)(i/vcells), y+cellHeight*(i%vcells),
        cellWidth, cellHeight/2, true);
    } else if (i==6) {
      drawDayBox(p, weekDate, x+cellWidth*(int)((i-1)/vcells),
        y+cellHeight*((i-1)%vcells)+cellHeight/2, cellWidth, cellHeight/2, true);
    }
  } // for i through all weekdays
}


void CalPrintBase::drawTimeTable(QPainter &p,
    const QDate &fromDate, const QDate &toDate,
    QTime &fromTime, QTime &toTime,
    int x, int y, int width, int height)
{
  // timeline is 1.5 hours:
  int alldayHeight = (int)( 3600.*height/(fromTime.secsTo(toTime)+3600.) );
  int timelineWidth = 60;
  int cellWidth = (int)( (width-timelineWidth)/(fromDate.daysTo(toDate)+1) );
  int currY=y;
  int currX=x;

  drawDaysOfWeek( p, fromDate, toDate, x+timelineWidth, currY, width-timelineWidth, mSubHeaderHeight);
  currY+=mSubHeaderHeight;
  drawTimeLine( p, fromTime, toTime, x, currY+alldayHeight,
    timelineWidth, height-mSubHeaderHeight-alldayHeight );

  currX=x+timelineWidth;
  // draw each day
  QDate curDate(fromDate);
  while (curDate<=toDate) {Event::List eventList;
    eventList.fill( mCalendar->events(curDate, true));
    drawAllDayBox( p, eventList, curDate, false, currX, currY, cellWidth, alldayHeight);
    drawAgendaDayBox( p, eventList, curDate, false, fromTime, toTime, currX,
      currY+alldayHeight, cellWidth, height-mSubHeaderHeight-alldayHeight );
    currX+=cellWidth;
    curDate=curDate.addDays(1);
  }

}


///////////////////////////////////////////////////////////////////////////////

void CalPrintBase::drawMonth(QPainter &p, const QDate &qd, bool weeknumbers,
    int x, int y, int width, int height)
{
  
  int yoffset = mSubHeaderHeight;
  int xoffset = 0;

  QDate monthDate(QDate(qd.year(), qd.month(), 1));
  QDate monthFirst(monthDate);
  QDate monthLast(monthDate.addMonths(1).addDays(-1));
  

  int weekdayCol = weekdayColumn( monthDate.dayOfWeek() );
  monthDate = monthDate.addDays(-weekdayCol);

  int rows=(weekdayCol + qd.daysInMonth() - 1)/7 +1;
  int cellHeight = (height-yoffset) / rows;

  if (weeknumbers) {
    QFont oldFont(p.font());
    QFont newFont(p.font());
    newFont.setPointSize(7);
    p.setFont(newFont);
    xoffset += 18;
    QDate weekDate(monthDate);
    for (int row = 0; row<rows; row++) {
      int calWeek = weekDate.weekNumber();
      QRect rc(x, y+yoffset+cellHeight*row, xoffset-1, cellHeight);
      p.drawText( rc, AlignRight|AlignVCenter, QString::number(calWeek) );
      weekDate = weekDate.addDays(7);
    }
    p.setFont(oldFont);
  }

  drawDaysOfWeek( p, monthDate, monthDate.addDays(6), x+xoffset, y, width-xoffset, mSubHeaderHeight );
  int cellWidth = (width-xoffset) / 7;

  QColor back = p.backgroundColor();
  bool darkbg = false;
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < 7; col++) {
      // show days from previous/next month with a grayed background
      if ( (monthDate < monthFirst) || (monthDate > monthLast) ) {
        p.setBackgroundColor( QColor( 240, 240, 240) );
        darkbg = true;
      }
      drawDayBox(p, monthDate, x+xoffset+col*cellWidth, y+yoffset+row*cellHeight,  cellWidth, cellHeight);
      if ( darkbg ) {
        p.setBackgroundColor( back );
        darkbg = false;
      }
      monthDate = monthDate.addDays(1);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////

void CalPrintBase::drawTodo( int topmostPrio, bool completed, int &count, Todo * item, QPainter &p, bool connectSubTodos,
    bool desc, int pospriority, int possummary, int posDueDt, int level,
    int x, int &y, int width, int &height, int pageHeight,
    TodoParentStart *stare )
{
    if ( !completed && item->isCompleted() )
        return;
  QString outStr;
//  int fontHeight = 10;
  const KLocale *local = KGlobal::locale();
  int priority=item->priority();
    int posdue=posDueDt;
    if (posdue<0) posdue=x+width;
    QRect rect;
    TodoParentStart startpt;
    // This list keeps all starting points of the parent todos so the connection
    // lines of the tree can easily be drawn (needed if a new page is started)
    static QPtrList<TodoParentStart> startPoints;
    if (level<1) {
      startPoints.clear();
    }
    TodoParentStart* strect = 0;
    int nextLevel = level + 1;
  if ( priority <= topmostPrio ) {
    // size of item
    outStr=item->summary(); 
    if ( ! item->location().isEmpty() )
      outStr += " ("+item->location()+")";
    if ( item->hasDueDate() && posDueDt>=0 ) {
      outStr += " [" +local->formatDate(item->dtDue().date(),true)+"]";
    } 
    if ( topmostPrio < 5 && item->relatedTo() ) {
      Incidence *inc = item->relatedTo();
      outStr += " < ";
      while ( inc ) {
	outStr += inc->summary();
	inc = inc->relatedTo();
	if ( inc )
	  outStr += "/";
      }
      outStr += " > ";
    }
    int left = possummary+(level*10);
    rect = p.boundingRect(left, y, (posdue-left-5),-1, WordBreak, outStr);
    //qDebug("bottom1 %d ", rect.bottom() );
    if ( !item->description().isEmpty() && desc ) {
      outStr = item->description();
      rect = p.boundingRect( left+20, rect.bottom()+5, width-(left+10-x), -1,
			     WordBreak, outStr );
    }
    //qDebug("bottom2 %d y+h %d   y %d  ph %d", rect.bottom(), y+height, y , pageHeight );
    // if too big make new page
    if ( rect.bottom() > y+height) {
      // first draw the connection lines from parent todos:
      if (level > 0 && connectSubTodos) {
	TodoParentStart *rct;
	for ( rct = startPoints.first(); rct; rct = startPoints.next() ) {
	  int start;
	  int center = rct->mRect.left() + (rct->mRect.width()/2);
	  int to = p.viewport().bottom();

	  // draw either from start point of parent or from top of the page
	  if (rct->mSamePage)
	    start = rct->mRect.bottom() + 1;
	  else
	    start = p.viewport().top();
	  p.moveTo( center, start );
	  p.lineTo( center, to );
	  rct->mSamePage=false;
	}
      }
      y=0;
      height=pageHeight-y;
      mPrinter->newPage();
    }

    // If this is a sub-item, r will not be 0, and we want the LH side of the priority line up
    //to the RH side of the parent item's priority
    if (stare && pospriority >= 0 ) {
      pospriority = stare->mRect.right() + 1;
    }

    // Priority
    outStr.setNum(priority);
    rect = p.boundingRect(pospriority, y + 10, 5, -1, AlignCenter, outStr);
    // Make it a more reasonable size
    rect.setWidth(19);
    rect.setHeight(19);
    if ( priority > 0 && pospriority>=0 ) {
      p.drawText(rect, AlignCenter, outStr);
      p.drawRect(rect);
      // cross out the rectangle for completed items
      if ( item->isCompleted() ) {
        p.drawLine( rect.topLeft(), rect.bottomRight() );
        p.drawLine( rect.topRight(), rect.bottomLeft() );
      } else  if (item->cancelled() ) {
        QPen pen = p.pen();
        p.setPen ( QPen ( black, 2) );
        p.drawLine( rect.left()+2,rect.top()+rect.height()/2, rect.right()-2, +rect.top()+rect.height()/2 );
        p.setPen( pen );
      }
    }
    startpt.mRect = rect; //save for later

    // Connect the dots
    if (level > 0 && connectSubTodos) {
      int bottom;
      int center( stare->mRect.left() + (stare->mRect.width()/2)+1 );
      if (stare->mSamePage )
        bottom = stare->mRect.bottom();//lutz + 1;
      else
	bottom = 0;
      int to( rect.top() + (rect.height()/2)+1 );
      int endx( rect.left() );
      p.moveTo(center, bottom);
      p.lineTo(center, to);
      p.lineTo(endx, to);
    }

    // if completed, use strike out font
    // LR does not work - font is underlined, not striked out
    //QFont ft=p.font();
    //ft.setStrikeOut( item->isCompleted() );
    //p.setFont( ft );
    // summary
    outStr=item->summary();  
    if ( ! item->location().isEmpty() )
      outStr += " ("+item->location()+")";
    if ( item->hasDueDate() && posDueDt>=0 ) {
      outStr += " [" +item->dtDueStr(true)+"]";
    } 
    if ( topmostPrio < 5 && item->relatedTo() ) {
      Incidence *inc = item->relatedTo();
      outStr += "  <";
      while ( inc ) {
	outStr += inc->summary();
	inc = inc->relatedTo();
	if ( inc )
	  outStr += "/";
      }
      outStr += ">";
    }
    rect = p.boundingRect( left, rect.top(), (posdue-(left + rect.width() + 5)),
			   -1, WordBreak, outStr);
    QRect newrect;
    p.drawText( rect, WordBreak, outStr, -1, &newrect );
    //ft.setStrikeOut(false);
    // p.setFont(ft);

    // due
    //  if ( item->hasDueDate() && posDueDt>=0 ) {
    //     outStr = local->formatDate(item->dtDue().date(),true);
    //     rect = p.boundingRect(posdue, y, x+width, -1, AlignTop|AlignLeft, outStr);
    //     p.drawText(rect, AlignTop|AlignLeft, outStr);
    //   }

    if ( !item->description().isEmpty() && desc ) {
      y=newrect.bottom() + 5;
      outStr = item->description();
      rect = p.boundingRect( left+20, y, x+width-(left+10), -1,
			     WordBreak, outStr );
      p.drawText( rect, WordBreak, outStr, -1, &newrect );
    }

    // Set the new line position
    y=newrect.bottom() + 10; //set the line position

    strect = &startpt;
  } // end if  ( priority <= topmostPrio )
  else {
    // we have skipped the printout of this todo
      strect = stare;
      --nextLevel;
    
  }
  // If the item has subitems, we need to call ourselves recursively
  Incidence::List l;l.fill( item->relations());
  Incidence::List::ConstIterator it;
  startPoints.append( &startpt );
  if ( topmostPrio == 5 ) {
    for( it = l.begin(); it != l.end(); ++it ) {
      count++;
      drawTodo( topmostPrio, completed, count, static_cast<Todo *>( *it ), p, connectSubTodos,
		desc, pospriority, possummary, posDueDt, nextLevel,
		x, y, width, height, pageHeight, strect );
    }
  }
  startPoints.remove(&startpt);
}

int CalPrintBase::weekdayColumn( int weekday )
{
  return ( weekday + 7 - KGlobal::locale()->weekStartDay() ) % 7;
}

void CalPrintBase::drawSplitWeek( QPainter &p, const QDate &fd,
                                  const QDate &td )
{
  QDate curDay, fromDay, toDay, curWeek, fromWeek, toWeek;

  mPrinter->setOrientation(KPrinter::Portrait);

  int minus = 0;
  if (KGlobal::locale()->weekStartsMonday()) {
    // correct to monday
    fromWeek = fd.addDays(-(fd.dayOfWeek()-1));
    // correct to sunday
    toWeek = td.addDays(7-fd.dayOfWeek());
    minus = 1;
  } else {
    // correct to sunday
    fromWeek = fd.addDays(-(fd.dayOfWeek()%7));
    // correct to saturday
    toWeek = td.addDays(6-td.dayOfWeek());
  }

  fromDay = fd;
  curDay = fd;
  toDay = td;
  p.setFont( QFont("Helvetica") );
  // the painter initially begins at 72 dpi per the Qt docs. 
  int pageWidth = p.viewport().width();
  int pageHeight = p.viewport().height();
  int margin=0;
  mHeaderHeight = 110;
  mSubHeaderHeight = 20;

  p.setViewport(margin, margin, 
       p.viewport().width()-margin, 
       p.viewport().height()-margin);

  curWeek = fromWeek.addDays(6);

  int columnWidth = int( pageWidth / 4.5 );

  do {
       
      //  if ( (curDay.dayOfWeek()-1)%7 < 3 )
    switch((curDay.dayOfWeek()-minus)%7){
      case 0:
        drawSplitTimes( p, pageWidth, columnWidth, pageHeight );
        drawSplitDay( p, curDay, columnWidth, pageHeight,
                      int( columnWidth * 0.5 ) );
        break;
      case 1:
        drawSplitDay( p, curDay, columnWidth, pageHeight,
                      int( columnWidth * 1.5 ) );
        break;
      case 2:
        drawSplitDay( p, curDay, columnWidth, pageHeight,
                      int( columnWidth * 2.5 ) );
        break;
      case 3:
        drawSplitDay( p, curDay, columnWidth, pageHeight,
                      int( columnWidth * 3.5 ) );
        mPrinter->newPage();
        break;
      case 4:
        drawSplitTimes( p, int( pageWidth * ( 3.5/ 4.5 ) ), columnWidth,
                        pageHeight );
        drawSplitDay( p, curDay, columnWidth, pageHeight,
                      int( columnWidth * 0.5 ) );
        drawSplitHeaderRight( p, curWeek.addDays( -6 ), curWeek,
                              curWeek, pageWidth, mHeaderHeight );
        break;
      case 5:
        drawSplitDay( p, curDay, columnWidth, pageHeight,
                      int( columnWidth * 1.5 ) );
        break;
      case 6:
        drawSplitDay( p, curDay, columnWidth, pageHeight,
                      int( columnWidth * 2.5 ) );
        if ( curDay < toDay )
          mPrinter->newPage();
        curWeek = curWeek.addDays( 7 );
        break;
    }
    curDay = curDay.addDays(1);

  } while (curDay <= toDay);
 
}

void CalPrintBase::drawSplitHeaderRight( QPainter &p, const QDate &fd,
                                         const QDate &td,
                                         const QDate &,
                                         int width, int )
{ 
    int tempStore = mSubHeaderHeight;
    mSubHeaderHeight+= mSubHeaderHeight;
  KLocale *local = KGlobal::locale();
  QFont font("helvetica", 18, QFont::Bold);
  QPen penA( black,0);
  QPen penB( black,3);
  p.setFont(font);
  int lineSpacing = p.fontMetrics().lineSpacing();
  QString title;
  QString myOwner(mCalendar->getOwner());
  if ( fd.month() == td.month() ) {
    title = local->monthName(fd.month(), false) + ' ' + QString::number(fd.day()) + ' ' 
       + '-' + ' ' + QString::number(td.day());
  } else {
    title = local->monthName(fd.month(), false) + ' ' + QString::number(fd.day()) + ' ' 
       + '-' + ' ' + local->monthName(td.month(), false) + ' ' + QString::number(td.day());
  }

// Grrrrrrr!  why can't I set the font to a serif font?!?!?
  QFont serifFont("Helvetica", 30);
//  serifFont.setFamily("Serif");
//  serifFont.setWeight(87);
//  serifFont.setItalic(true);
  p.setFont(serifFont);
  QFontInfo info(p.font());

  lineSpacing = p.fontMetrics().lineSpacing();
  p.drawText(0, lineSpacing * 0, width, lineSpacing, AlignRight |AlignTop, title );

  title.truncate(0);

  p.setPen(penB );
  p.drawLine(200, lineSpacing * 1, width, lineSpacing * 1);
  p.setPen(penA );

  p.setFont(QFont("Helvetica", 20, QFont::Bold, TRUE));
  title += QString::number(fd.year());
  p.drawText(0, lineSpacing * 1, width, lineSpacing, AlignRight |AlignTop, title );
  mSubHeaderHeight = tempStore ;
}

void CalPrintBase::drawSplitDay( QPainter &p, const QDate &qd, int width,
                                 int height, int offsetLeft )
{
    int tempStore = mSubHeaderHeight;
    mSubHeaderHeight+= mSubHeaderHeight;
  int startHour = KOPrefs::instance()->mDayBegins;
  int endHour = 20;
  int offset = mHeaderHeight + mSubHeaderHeight + 10;
  Event::List eventList; eventList.fill( mCalendar->events( qd, true ));
  Event::List::Iterator it;
  Event *currEvent;
  KLocale *local = KGlobal::locale();
  QString dayName;
 
  dayName = local->weekDayName(qd.dayOfWeek()) + ' ' + ' ' + QString::number(qd.day());
  //p.setBrush(QBrush(black));
// width+1 to make sure there's a continuous, black bar across the top.
  p.setPen( QPen(Qt::black,2));
  p.drawRect(offsetLeft, mHeaderHeight + 5, width +1, mSubHeaderHeight);
  p.setPen( Qt::black);
  p.setFont(QFont("helvetica", 12, QFont::Bold, true));
  p.drawText(offsetLeft, mHeaderHeight + 5, 
          width, mSubHeaderHeight, AlignHCenter | AlignVCenter,
          dayName);

  p.setPen( QPen(Qt::black,2));
  p.setFont(QFont("helvetica", 12));
  p.setBrush(Qt::white);
  it = eventList.begin();
  int allDays = 0;
  /*
  while( it != eventList.end() ) {
    Event *currEvent = *it;
    if ( currEvent->doesFloat() ) {
      p.drawRect( offsetLeft, offset, width, 35 );
      p.drawText( offsetLeft + 5, offset + 10, width - 10, 30,
                  AlignLeft | AlignTop, currEvent->summary() );
      offset += 40;
      allDays++;
      it = eventList.remove( it );
    } else {
      ++it;
    }
  }
  */
  //p.setBrush(QBrush());
  int tmpEnd;
  for ( it = eventList.begin(); it != eventList.end(); ++it ) {
    Event *currEvent = *it;
    tmpEnd = currEvent->dtEnd().time().hour();
    if (currEvent->dtEnd().time().minute() > 0)
      tmpEnd++;
    if (tmpEnd > endHour) 
      endHour = tmpEnd;
  }
  int hours = endHour - startHour;
  int cellHeight = (height-offset) / hours; // hour increments.

  p.setFont(QFont("helvetica", 12));
  //p.setBrush(QBrush(Dense7Pattern));
  p.setBrush(Qt::white);
  for ( it = eventList.begin(); it != eventList.end(); ++it ) {
    Event *currEvent = *it;
    int startTime = currEvent->dtStart().time().hour();
    int endTime = currEvent->dtEnd().time().hour();
    float minuteInc = cellHeight / 60.0;
    if ((startTime >= startHour)  && (endTime <= (startHour + hours))) {
      startTime -= startHour;
      int startMinuteOff = (int) (minuteInc * 
      currEvent->dtStart().time().minute()); 
      QString text = currEvent->summary() ;
        if ( ! currEvent->location().isEmpty() )
            text += "\n("+currEvent->location()+")";
      int endMinuteOff = (int) (minuteInc * currEvent->dtEnd().time().minute());
      int cheight = (int) (minuteInc * 
                    currEvent->dtStart().secsTo(currEvent->dtEnd()) / 60 );
      p.drawRect(offsetLeft+2, 1+offset+startMinuteOff+startTime*cellHeight, 
                 width-4, cheight);
      p.drawText(offsetLeft+12, offset+startMinuteOff+startTime*cellHeight+5, width-24, 
                 cheight-10, AlignHCenter | AlignTop, text);
    }
  }
  p.setBrush(QBrush(NoBrush));
  mSubHeaderHeight = tempStore ;
}

void CalPrintBase::drawSplitTimes( QPainter &p, int width, int timeWidth,
                                   int height )
{
 int tempStore = mSubHeaderHeight;
    mSubHeaderHeight+= mSubHeaderHeight;
  int startHour = KOPrefs::instance()->mDayBegins;
  int endHour = 20;
  int offset = mHeaderHeight + mSubHeaderHeight + 10;
  int hours = endHour - startHour;
  int cellHeight = (height-offset) / hours; // hour increments.
  
  QString numStr;
  for (int i = 0; i < hours; i++) {
      p.setPen(QPen(black,1));
      p.drawLine(0, offset+i*cellHeight, width, offset+i*cellHeight);
      p.setPen(QPen(gray,0));
      p.drawLine(37, offset+i*cellHeight+(cellHeight/2),
                 width, offset+i*cellHeight+(cellHeight/2));
      p.setPen(QPen(black,0));

    if ( !KGlobal::locale()->use12Clock() ) {
      numStr.setNum(i+startHour);
      if (cellHeight > 40) {
        p.setFont(QFont("helvetica", 16, QFont::Bold));
      } else {
        p.setFont(QFont("helvetica", 14, QFont::Bold));
      }
      p.drawText(0, offset+i*cellHeight, 33, cellHeight/2,
                 AlignTop|AlignRight, numStr);
      p.setFont(QFont("helvetica", 12, QFont::Bold));
      p.drawText(37, offset+i*cellHeight, 45, cellHeight/2,
                 AlignTop | AlignLeft, "00");
    } else {
      QTime time( i + startHour, 0 );
      numStr = KGlobal::locale()->formatTime( time );
      p.setFont(QFont("helvetica", 12, QFont::Bold));
      p.drawText(4, offset+i*cellHeight, 70, cellHeight/2,
                 AlignTop|AlignLeft, numStr);
    }
  } mSubHeaderHeight = tempStore ;
}

#endif
