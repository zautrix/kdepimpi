/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
    (C) 1998-2001 Mirko Boehm (mirko@kde.org)
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

/////////////////// KDateTable widget class //////////////////////
//
// Copyright (C) 1997 Tim D. Gilman
//           (C) 1998-2001 Mirko Boehm
// Written using Qt (http://www.troll.no) for the
// KDE project (http://www.kde.org)
//
// This is a support class for the KDatePicker class.  It just
// draws the calender table without titles, but could theoretically
// be used as a standalone.
//
// When a date is selected by the user, it emits a signal:
//      dateSelected(QDate)

#include <kglobal.h>
#include <kglobalsettings.h>
#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <knotifyclient.h>
#include "kdatetbl.h"
#include <qdatetime.h>
#include <qstring.h>
#include <qpen.h>
#include <qpainter.h>
#include <qdialog.h>
#include <assert.h>
#include <qapplication.h>

KDateValidator::KDateValidator(QWidget* parent, const char* name)
    : QValidator(parent, name)
{
}

QValidator::State
KDateValidator::validate(QString& text, int&) const
{
    QDate temp;
    // ----- everything is tested in date():
    return date(text, temp);
}

QValidator::State
KDateValidator::date(const QString& text, QDate& d) const
{
    QDate tmp = KGlobal::locale()->readDate(text);
    if (!tmp.isNull())
        {
            d = tmp;
            return Acceptable;
        } else
        return Valid;
}

void
KDateValidator::fixup( QString& ) const
{

}

KDateTable::KDateTable(QWidget *parent, QDate date_, const char* name, Qt::WFlags f)
    : QGridView(parent, name, f)
{
    setFont( KGlobalSettings::generalFont() );
    if(!date_.isValid())
        {
            date_=QDate::currentDate();
        }
#ifdef QT_4_COMPAT
    setFocusPolicy( Qt::StrongFocus );
#else
    setFocusPolicy( QWidget::StrongFocus );
#endif
    setNumRows(7); // 6 weeks max + headline
    setNumCols(7); // 7 days a week
    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);
    viewport()->setBackgroundColor(QColor(220,245,255));
#if 0
    viewport()->setEraseColor(lightGray);
#endif
    mMarkCurrent = false;
    setDate(date_); // this initializes firstday, numdays, numDaysPrevMonth
}

void
KDateTable::paintCell(QPainter *painter, int row, int col)
{
    QRect rect;
    QString text;
    QPen pen;
    int w=cellWidth();
    int h=cellHeight();
    int pos;
    QBrush brushBlue(Qt::blue);
    QBrush brushLightblue(QColor(220,245,255));
    QFont _font=font();
    // -----
    if(row==0)
        { // we are drawing the headline
            _font.setBold(true);
            painter->setFont(_font);
            bool normalday = true;
            QString daystr;
            if (KGlobal::locale()->weekStartsMonday())
                {
                    daystr = KGlobal::locale()->weekDayName(col+1, true);
                    if (col == 5 || col == 6)
                        normalday = false;
                } else {
                daystr = KGlobal::locale()->weekDayName(col==0? 7 : col, true);
                if (col == 0 || col == 6)
                    normalday = false;
            }
            if (!normalday)
                {
                    painter->setPen(QColor(220,245,255));
                    painter->setBrush(brushLightblue);
                    painter->drawRect(0, 0, w, h);
                    painter->setPen(Qt::blue);
                } else {
                painter->setPen(Qt::blue);
                painter->setBrush(brushBlue);
                painter->drawRect(0, 0, w, h);
                painter->setPen(Qt::white);
            }
            painter->drawText(0, 0, w, h-1, Qt::AlignCenter,
                              daystr, -1, &rect);
            painter->setPen(Qt::black);
            painter->drawLine(0, h-1,w-1, h-1);
            // ----- draw the weekday:
        } else {
        painter->setFont(_font);
        pos=7*(row-1)+col;
        if (KGlobal::locale()->weekStartsMonday())
            pos++;
        if(pos<firstday || (firstday+numdays<=pos))
            { // we are either
                // ° painting a day of the previous month or
                // ° painting a day of the following month
                if(pos<firstday)
                    { // previous month
                        text.setNum(numDaysPrevMonth+pos-firstday+1);
                    } else { // following month
                    text.setNum(pos-firstday-numdays+1);
                }
                painter->setPen(Qt::gray);
            } else { // paint a day of the current month
            text.setNum(pos-firstday+1);
            painter->setPen(Qt::black);
        }

        pen=painter->pen();
        if(firstday+date.day()-1==pos)
            {
                if(mMarkCurrent && firstday+QDate::currentDate().day()-1==pos)
                    painter->setPen(Qt::green);
                else
                    painter->setPen(Qt::red);
                if(hasFocus())
                    {  
                        painter->setBrush(Qt::darkRed);
                        pen=QPen(Qt::white);
                    } else {
                    painter->setBrush(Qt::darkGray);
                    pen=QPen(Qt::white);
                }
            } else {
            if(mMarkCurrent && firstday+QDate::currentDate().day()-1==pos)
                {
                    painter->setPen(Qt::green);
                    painter->setBrush(Qt::darkGreen);
                    pen=QPen(Qt::white);
                } else {
                painter->setBrush(QColor(220,245,255));
                painter->setPen(QColor(220,245,255));
            }
        }
        painter->drawRect(0, 0, w, h);
        painter->setPen(pen);
        painter->drawText(0, 0, w, h, Qt::AlignCenter, text, -1, &rect);
    }
    /*
    if(rect.width()>maxCell.width()) maxCell.setWidth(rect.width()); 
    if(rect.height()>maxCell.height()) {
        maxCell.setHeight(rect.height());
    }
    */
}

void
KDateTable::keyPressEvent( QKeyEvent *e )
{
    /*
    // not working properly
    if ( e->key() == Qt::Key_Prior ) {
    if ( date.month() == 1 ) {
    KNotifyClient::beep();
    return;
    }
    int day = date.day();
    if ( day > 27 )
    while ( !QDate::isValid( date.year(), date.month()-1, day ) )
    day--;
    setDate(QDate(date.year(), date.month()-1, day));
    return;
    }
    if ( e->key() == Qt::Key_Next ) {
    if ( date.month() == 12 ) {
    KNotifyClient::beep();
    return;
    }
    int day = date.day();
    if ( day > 27 )
    while ( !QDate::isValid( date.year(), date.month()+1, day ) )
    day--;
    setDate(QDate(date.year(), date.month()+1, day));
    return;
    }
    */
    int dayoff = KGlobal::locale()->weekStartsMonday() ? 1 : 0;

    int temp=firstday+date.day()-dayoff;
    int pos = temp;
    bool irgnore = true;
    int add = 0;
    if ( e->state() != Qt::ControlButton ) {
        if ( e->key() == Qt::Key_Up ) {
            add -= 7;
            irgnore = false;
        }
        if ( e->key() == Qt::Key_Down ) {
            add += 7;
            irgnore = false;
        }
        if ( e->key() == Qt::Key_Left ) {
            --add;
            irgnore = false;
        }
        if ( e->key() == Qt::Key_Right ) {
            ++add;
            irgnore = false;
        }
    } 
    if ( irgnore ) {
        e->ignore();
        return;
    }

    pos += add;
    setDate(date.addDays( add ));
    updateCell(temp/7+1, temp%7); // Update the previously selected cell
    updateCell(pos/7+1, pos%7); // Update the selected cell
 
}

void
KDateTable::viewportResizeEvent(QResizeEvent * e)
{
    QGridView::viewportResizeEvent(e);

    setCellWidth(viewport()->width()/7);
    setCellHeight(viewport()->height()/7);
}

void
KDateTable::setFontSize(int size)
{
    int count;
    QRect rect;
    // ----- store rectangles:
    fontsize=size;
    QFont  _font = font();
    _font.setPointSize(fontsize);
    setFont( _font );
    _font.setBold( true );
    QFontMetrics metrics(_font);

    // ----- find largest day name:
    maxCell.setWidth(0);
    maxCell.setHeight(0);
    for(count=0; count<7; ++count)
        {
            rect=metrics.boundingRect(KGlobal::locale()->weekDayName(count+1, true));
            maxCell.setWidth(QMAX(maxCell.width(), rect.width()));
            maxCell.setHeight(QMAX(maxCell.height(), rect.height()));
        }
    // ----- compare with a real wide number and add some space:
    rect=metrics.boundingRect(QString::fromLatin1("88"));
    maxCell.setWidth(QMAX(maxCell.width()+2, rect.width()));
#ifdef DESKTOP_VERSION
    maxCell.setHeight(QMAX(maxCell.height()+8, rect.height()));
#else
    maxCell.setHeight(QMAX(maxCell.height()+4, rect.height()));
#endif
    if ( maxCell.width() * 1000 / maxCell.height()  > 1900 ) {
        maxCell.setHeight(maxCell.width() * 1000 / 1900 );
        //qDebug("setmax ");
    }
}

void
KDateTable::contentsMousePressEvent(QMouseEvent *e)
{
    if(e->type()!=QEvent::MouseButtonPress)
        { // the KDatePicker only reacts on mouse press events:
            return;
        }
    if(!isEnabled())
        {
            KNotifyClient::beep();
            return;
        }

    int dayoff = KGlobal::locale()->weekStartsMonday() ? 1 : 0;
    // -----
    int row, col, pos, temp;
    QPoint mouseCoord;
    // -----
    mouseCoord = e->pos();
    row=rowAt(mouseCoord.y());
    col=columnAt(mouseCoord.x());
    if(row<0 || col<0)
        { // the user clicked on the frame of the table
            return;
        }
    pos=7*(row-1)+col+1;
#if 0
    if(pos+dayoff<=firstday)
        { // this day is in the previous month
            KNotifyClient::beep();
            return;
        }
    if(firstday+numdays<pos+dayoff)
        { // this date is in the next month
            KNotifyClient::beep();
            return;
        }
#endif
    temp=firstday+date.day()-dayoff-1;
    QDate da = QDate(date.year(), date.month(),1);
    setDate(da.addDays( pos-firstday+dayoff-1));
    updateCell(temp/7+1, temp%7); // Update the previously selected cell
    updateCell(row, col); // Update the selected cell
    // assert(QDate(date.year(), date.month(), pos-firstday+dayoff).isValid());
    emit(tableClicked());
}

bool
KDateTable::setDate(const QDate& date_)
{
    bool changed=false;
    QDate temp;
    mMarkCurrent = false;
    // -----
    if(!date_.isValid())
        {
            kdDebug() << "KDateTable::setDate: refusing to set invalid date." << endl;
            return false;
        }
    if(date!=date_)
        {
            date=date_;
            changed=true;
        }
    mMarkCurrent = ( date.month() ==  QDate::currentDate().month() && date.year() ==  QDate::currentDate().year() );
    temp.setYMD(date.year(), date.month(), 1);
    firstday=temp.dayOfWeek();
    if(firstday==1) firstday=8;
    numdays=date.daysInMonth();
    if(date.month()==1)
        { // set to december of previous year
            temp.setYMD(date.year()-1, 12, 1);
        } else { // set to previous month
        temp.setYMD(date.year(), date.month()-1, 1);
    }
    numDaysPrevMonth=temp.daysInMonth();
    if(changed)
        {
            repaintContents(false);
        }
    emit(dateChanged(date));
    return true;
}

const QDate&
KDateTable::getDate() const
{
    return date;
}

void KDateTable::focusInEvent( QFocusEvent *e )
{
    repaintContents(false);
    QGridView::focusInEvent( e );
}

void KDateTable::focusOutEvent( QFocusEvent *e )
{
    repaintContents(false);
    QGridView::focusOutEvent( e );
}

QSize
KDateTable::sizeHint() const
{
    if(maxCell.height()>0 && maxCell.width()>0)
        {
            return QSize((maxCell.width()+2)*numCols()+2*frameWidth(),
                         (maxCell.height()+4)*numRows()+2*frameWidth());
        } else {
        return QSize(-1, -1);
    }
}

KDateInternalMonthPicker::KDateInternalMonthPicker
(QWidget* parent, const char* name)
    : QGridView(parent, name),
      result(0) // invalid
{
    QRect rect;
    QFont font;
    // -----
    activeCol = -1;
    activeRow = -1;
    font=KGlobalSettings::generalFont();
    int fontsize = 10;
    int add = 2;
    if ( QApplication::desktop()->width() >= 480 && QApplication::desktop()->width() <= 640 )
        add += 8;
    if ( QApplication::desktop()->width() > 640 )
        add += 6;
    font.setPointSize(fontsize+add);
    setFont(font);
    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setNumRows(4);
    setNumCols(3);
    // enable to find drawing failures:
    // setTableFlags(Tbl_clipCellPainting);
#if 0
    viewport()->setEraseColor(lightGray); // for consistency with the datepicker
#endif
    // ----- find the preferred size
    //       (this is slow, possibly, but unfortunatly it is needed here):
    QFontMetrics metrics(font);
    for(int i=1; i <= 12; ++i)
        {
            rect=metrics.boundingRect(KGlobal::locale()->monthName(i, false));
            if(max.width()<rect.width()) max.setWidth(rect.width());
            if(max.height()<rect.height()) max.setHeight(rect.height());
        }

}

QSize
KDateInternalMonthPicker::sizeHint() const
{
    return QSize((max.width()+6)*numCols()+2*frameWidth(),
                 (max.height()+6)*numRows()+2*frameWidth());
}

int
KDateInternalMonthPicker::getResult() const
{
    return result;
}

void
KDateInternalMonthPicker::setupPainter(QPainter *p)
{
    p->setPen(Qt::black);
}

void
KDateInternalMonthPicker::viewportResizeEvent(QResizeEvent*)
{
    setCellWidth(width()/3);
    setCellHeight(height()/4);
}

void
KDateInternalMonthPicker::paintCell(QPainter* painter, int row, int col)
{
    int index;
    QString text;
    // ----- find the number of the cell:
    index=3*row+col+1;
    text=KGlobal::locale()->monthName(index, false);
    painter->drawText(0, 0, cellWidth(), cellHeight(), Qt::AlignCenter, text);
    if ( activeCol == col && activeRow == row )
        painter->drawRect( 0, 0, cellWidth(), cellHeight() );
}

void
KDateInternalMonthPicker::contentsMousePressEvent(QMouseEvent *e)
{
    if(!isEnabled() || e->button() != Qt::LeftButton)
        {
            KNotifyClient::beep();
            return;
        }
    // -----
    int row, col;
    QPoint mouseCoord;
    // -----
    mouseCoord = e->pos();
    row=rowAt(mouseCoord.y());
    col=columnAt(mouseCoord.x());

    if(row<0 || col<0)
        { // the user clicked on the frame of the table
            activeCol = -1;
            activeRow = -1;
        } else {
        activeCol = col;
        activeRow = row;
        updateCell( row, col /*, false */ );
    }
}

void
KDateInternalMonthPicker::contentsMouseMoveEvent(QMouseEvent *e)
{
    if (e->state() & Qt::LeftButton)
        {
            int row, col;
            QPoint mouseCoord;
            // -----
            mouseCoord = e->pos();
            row=rowAt(mouseCoord.y());
            col=columnAt(mouseCoord.x());
            int tmpRow = -1, tmpCol = -1;
            if(row<0 || col<0)
                { // the user clicked on the frame of the table
                    if ( activeCol > -1 )
                        {
                            tmpRow = activeRow;
                            tmpCol = activeCol;
                        }
                    activeCol = -1;
                    activeRow = -1;
                } else {
                bool differentCell = (activeRow != row || activeCol != col);
                if ( activeCol > -1 && differentCell)
                    {
                        tmpRow = activeRow;
                        tmpCol = activeCol;
                    }
                if ( differentCell)
                    {
                        activeRow = row;
                        activeCol = col;
                        updateCell( row, col /*, false */ ); // mark the new active cell
                    }
            }
            if ( tmpRow > -1 ) // repaint the former active cell
                updateCell( tmpRow, tmpCol /*, true */ );
        }
}

void
KDateInternalMonthPicker::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if(!isEnabled())
        {
            return;
        }
    // -----
    int row, col, pos;
    QPoint mouseCoord;
    // -----
    mouseCoord = e->pos();
    row=rowAt(mouseCoord.y());
    col=columnAt(mouseCoord.x());
    if(row<0 || col<0)
        { // the user clicked on the frame of the table
            emit(closeMe(0));
        }
    pos=3*row+col+1;
    result=pos;
    emit(closeMe(1));
}



KDateInternalYearSelector::KDateInternalYearSelector
(int fontsize, QWidget* parent, const char* name)
    : QLineEdit(parent, name),
      val(new QIntValidator(this)),
      result(0)
{
    QFont font;
    // -----
    font=KGlobalSettings::generalFont();
    font.setPointSize(fontsize);
    setFont(font);
#if 0
    setFrameStyle(QFrame::NoFrame);
#endif
    // we have to respect the limits of QDate here, I fear:
    val->setRange(0, 8000);
    setValidator(val);
    connect(this, SIGNAL(returnPressed()), SLOT(yearEnteredSlot()));
}

void
KDateInternalYearSelector::yearEnteredSlot()
{
    bool ok;
    int year;
    QDate date;
    // ----- check if this is a valid year:
    year=text().toInt(&ok);
    if(!ok)
        {
            KNotifyClient::beep();
            return;
        }
    date.setYMD(year, 1, 1);
    if(!date.isValid())
        {
            KNotifyClient::beep();
            return;
        }
    result=year;
    emit(closeMe(1));
}

int
KDateInternalYearSelector::getYear()
{
    return result;
}

void
KDateInternalYearSelector::setYear(int year)
{
    QString temp;
    // -----
    temp.setNum(year);
    setText(temp);
}

KPopupFrame::KPopupFrame(QWidget* parent, const char*  name)
    : QFrame(parent, name, Qt::WType_Popup),
      result(0), // rejected
      main(0)
{
    setFrameStyle(QFrame::Box|QFrame::Raised);
    setMidLineWidth(2);
}

void
KPopupFrame::keyPressEvent(QKeyEvent* e)
{
    if(e->key()==Qt::Key_Escape)
        {
            result=0; // rejected
            qApp->exit_loop();
        }
}

void
KPopupFrame::close(int r)
{
    result=r;
    qApp->exit_loop();
}

void
KPopupFrame::setMainWidget(QWidget* m)
{
    main=m;
    if(main!=0)
        {
            resize(main->width()+2*frameWidth(), main->height()+2*frameWidth());
        }
}

void
KPopupFrame::resizeEvent(QResizeEvent*)
{
    if(main!=0)
        {
            main->setGeometry(frameWidth(), frameWidth(),
                              width()-2*frameWidth(), height()-2*frameWidth());
        }
}

void
KPopupFrame::popup(const QPoint &pos)
{
    // Make sure the whole popup is visible.
    QRect d = QApplication::desktop()->frameGeometry();
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();
    if (x+w > d.x()+d.width())
        x = d.width() - w;
    if (y+h > d.y()+d.height())
        y = d.height() - h;
    if (x < d.x())
        x = 0;
    if (y < d.y())
        y = 0;

    // Pop the thingy up.
    move(x, y);
    show();
}

int
KPopupFrame::exec(QPoint pos)
{
    popup(pos);
    repaint();
    qApp->enter_loop();
    hide();
    return result;
}

int
KPopupFrame::exec(int x, int y)
{
    return exec(QPoint(x, y));
}

void KPopupFrame::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void KDateTable::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

//#include "kdatetbl.moc"


KDateInternalWeekPicker::KDateInternalWeekPicker
(QWidget* parent, const char* name)
    : QGridView(parent, name),
      result(0) // invalid
{
    QRect rect;
    QFont font;
    // -----
    activeCol = -1;
    activeRow = -1;
    font=KGlobalSettings::generalFont();
    int fontsize = 10;
    int add = 2;
    if ( QApplication::desktop()->width() >= 480 && QApplication::desktop()->width() <= 640 )
        add += 6;
    font.setPointSize(fontsize+add);
    setFont(font);
    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    if ( QApplication::desktop()->height() > 240 ) {
        setNumRows(13);
        setNumCols(4);
    } else {
        setNumRows(4);
        setNumCols(13);
    }
    // enable to find drawing failures:
    // setTableFlags(Tbl_clipCellPainting);
#if 0
    viewport()->setEraseColor(lightGray); // for consistency with the datepicker
#endif
    // ----- find the preferred size
    //       (this is slow, possibly, but unfortunatly it is needed here):
    QFontMetrics metrics(font);
    for(int i=1; i <= 52; ++i)
        {
            rect=metrics.boundingRect(QString::number( i ));
            if(max.width()<rect.width()) max.setWidth(rect.width());
            if(max.height()<rect.height()) max.setHeight(rect.height());
        }
    if ( QApplication::desktop()->width() > 640 ) {
      
        max.setWidth(max.width()+6);
        max.setHeight(max.height()+8);
    }
}

QSize
KDateInternalWeekPicker::sizeHint() const
{
    return QSize((max.width()+6)*numCols()+2*frameWidth(),
                 (max.height()+6)*numRows()+2*frameWidth());
}

int
KDateInternalWeekPicker::getResult() const
{
    return result;
}

void
KDateInternalWeekPicker::setupPainter(QPainter *p)
{
    p->setPen(Qt::black);
}

void
KDateInternalWeekPicker::viewportResizeEvent(QResizeEvent*)
{
    setCellWidth(width()/ numCols());
    setCellHeight(height()/ numRows());
}

void
KDateInternalWeekPicker::paintCell(QPainter* painter, int row, int col)
{
    int index;
    QString text;
    // ----- find the number of the cell:
    index=numCols()*row+col+1;
    text=QString::number( index );
    painter->drawText(0, 0, cellWidth(), cellHeight(), Qt::AlignCenter, text);
    if ( activeCol == col && activeRow == row )
        painter->drawRect( 0, 0, cellWidth(), cellHeight() );
}

void
KDateInternalWeekPicker::contentsMousePressEvent(QMouseEvent *e)
{
    if(!isEnabled() || e->button() != Qt::LeftButton)
        {
            KNotifyClient::beep();
            return;
        }
    // -----
    int row, col;
    QPoint mouseCoord;
    // -----
    mouseCoord = e->pos();
    row=rowAt(mouseCoord.y());
    col=columnAt(mouseCoord.x());

    if(row<0 || col<0)
        { // the user clicked on the frame of the table
            activeCol = -1;
            activeRow = -1;
        } else {
        activeCol = col;
        activeRow = row;
        updateCell( row, col /*, false */ );
    }
}

void
KDateInternalWeekPicker::contentsMouseMoveEvent(QMouseEvent *e)
{
    if (e->state() & Qt::LeftButton)
        {
            int row, col;
            QPoint mouseCoord;
            // -----
            mouseCoord = e->pos();
            row=rowAt(mouseCoord.y());
            col=columnAt(mouseCoord.x());
            int tmpRow = -1, tmpCol = -1;
            if(row<0 || col<0)
                { // the user clicked on the frame of the table
                    if ( activeCol > -1 )
                        {
                            tmpRow = activeRow;
                            tmpCol = activeCol;
                        }
                    activeCol = -1;
                    activeRow = -1;
                } else {
                bool differentCell = (activeRow != row || activeCol != col);
                if ( activeCol > -1 && differentCell)
                    {
                        tmpRow = activeRow;
                        tmpCol = activeCol;
                    }
                if ( differentCell)
                    {
                        activeRow = row;
                        activeCol = col;
                        updateCell( row, col /*, false */ ); // mark the new active cell
                    }
            }
            if ( tmpRow > -1 ) // repaint the former active cell
                updateCell( tmpRow, tmpCol /*, true */ );
        }
}

void
KDateInternalWeekPicker::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if(!isEnabled())
        {
            return;
        }
    // -----
    int row, col, pos;
    QPoint mouseCoord;
    // -----
    mouseCoord = e->pos();
    row=rowAt(mouseCoord.y());
    col=columnAt(mouseCoord.x());
    if(row<0 || col<0)
        { // the user clicked on the frame of the table
            emit(closeMe(0));
        }
    pos=numCols()*row+col+1;
    result=pos;
    emit(closeMe(1));
}
