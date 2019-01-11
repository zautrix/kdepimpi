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

#include "kdatepicker.h"
#include <kglobal.h>
#include <kapplication.h>
#include <klocale.h>
#include <kiconloader.h>
#include <qframe.h>
#include <qpainter.h>
#include <qdialog.h>
#include <qtoolbutton.h>
#include <qfont.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <kdebug.h>
#include <knotifyclient.h>
#include <kglobalsettings.h>
#include "kdatetbl.h"
#include "kdateedit.h"
//#include "kdatepicker.moc"


KDatePicker::KDatePicker(QWidget *parent, QDate dt, const char *name)
  : QFrame(parent,name),
    yearForward(new QToolButton(this)),
    yearBackward(new QToolButton(this)),
    monthForward(new QToolButton(this)),
    monthBackward(new QToolButton(this)),
    selectMonth(new QToolButton(this)),
    selectYear(new QToolButton(this)),
    todayBut(new QToolButton(this)),
    //line(new QLineEdit(this)),
    val(new KDateValidator(this))
    //table(new KDateTable(this)),
    //fontsize(1)
{
    QFont fo = KGlobalSettings::generalFont();
  int add = 2;
  if ( QApplication::desktop()->width() >= 480 && QApplication::desktop()->width() <= 640 )
      add += 4;
  fo.setPointSize(fo.pointSize()+add );
  setFont( fo );
  table = new KDateTable(this);
  setFontSize(font().pointSize());
  //line->setValidator(val);
  lineDate = new KDateEdit( this, "dateediipicker", true );
  yearForward->setPixmap(SmallIcon("3rightarrowB"));
  yearBackward->setPixmap(SmallIcon("3leftarrowB"));
  monthForward->setPixmap(SmallIcon("2rightarrowB"));
  monthBackward->setPixmap(SmallIcon("2leftarrowB"));
  todayBut->setPixmap(SmallIcon("today"));
  setDate(dt); // set button texts
  connect(table, SIGNAL(dateChanged(QDate)), SLOT(dateChangedSlot(QDate)));
  connect(table, SIGNAL(tableClicked()), SLOT(tableClickedSlot()));
  connect(monthForward, SIGNAL(clicked()), SLOT(monthForwardClicked()));
  connect(monthBackward, SIGNAL(clicked()), SLOT(monthBackwardClicked()));
  connect(yearForward, SIGNAL(clicked()), SLOT(yearForwardClicked()));
  connect(yearBackward, SIGNAL(clicked()), SLOT(yearBackwardClicked()));
  connect(selectMonth, SIGNAL(clicked()), SLOT(selectMonthClicked()));
  connect(selectYear, SIGNAL(clicked()), SLOT(selectYearClicked()));
  connect(todayBut, SIGNAL(clicked()), SLOT(goToday()));
  //connect(line, SIGNAL(returnPressed()), SLOT(lineEnterPressed()));
  connect(lineDate, SIGNAL(dateChanged(QDate)), SLOT(slotSetDate(QDate)));
  connect(lineDate, SIGNAL(returnPressed()), SLOT(lineEnterPressed()));
  table->setFocus();

}

KDatePicker::~KDatePicker()
{
}

void
KDatePicker::resizeEvent(QResizeEvent*)
{
    QWidget *buttons[] = {
	yearBackward,
	    monthBackward,
	    selectMonth,
	    selectYear,
	    monthForward,
	    yearForward };
    const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
    QSize sizes[NoOfButtons];
    int buttonHeight=0;
    int count;
    int w;
    int x=0;
    // ----- calculate button row height:
    for(count=0; count<NoOfButtons; ++count) {
        int xS = buttons[count]->sizeHint().width();
        int yS = buttons[count]->sizeHint().height(); 
        if (  QApplication::desktop()->width() < 320 ) 
            sizes[count]=QSize ( xS+4, yS );
        else
            sizes[count]=QSize ( xS+10, yS );
            
	buttonHeight=QMAX(buttonHeight, sizes[count].height());
    }
    buttonHeight += 10;
    // ----- calculate size of the month button:
    w=0;
    for(count=0; count<NoOfButtons; ++count) {
	if(buttons[count]!=selectMonth)
	{
	    w+=sizes[count].width();
	} else {
	    x=count;
	}
    }
    sizes[x].setWidth(width()-w); // stretch the month button
    // ----- place the buttons:
    x=0;
    for(count=0; count<NoOfButtons; ++count)
    {
	w=sizes[count].width();
	buttons[count]->setGeometry(x, 0, w, buttonHeight);
	x+=w;
    }
    // ----- place the line edit for direct input:
    sizes[0]=lineDate->sizeHint();
    //line->setGeometry(0, height()-sizes[0].height(), width(), sizes[0].height());
    int todaywid = todayBut->sizeHint().width();
todayBut->setGeometry(0, height()-sizes[0].height(),todaywid, sizes[0].height());
    lineDate->setGeometry(0+todaywid, height()-sizes[0].height(), width()-todaywid, sizes[0].height());
    // ----- adjust the table:
    table->setGeometry(0, buttonHeight, width(),
		       height()-buttonHeight-sizes[0].height());
}

void
KDatePicker::dateChangedSlot(QDate date)
{
    lineDate->setDate( date );//(KGlobal::locale()->formatDate(date, true));
    //line->setText(KGlobal::locale()->formatDate(date, true));
    QString temp;
    selectMonth->setText(KGlobal::locale()->monthName(date.month(), false));
    temp.setNum(date.year());
    selectYear->setText(temp);
    emit(dateChanged(date));
}

void
KDatePicker::tableClickedSlot()
{

  emit(dateSelected(table->getDate()));
  emit(tableClicked());
}

const QDate&
KDatePicker::getDate() const
{
  return table->getDate();
}

const QDate &
KDatePicker::date() const
{
    return table->getDate();
}

void KDatePicker::goToday()
{
    slotSetDate( QDate::currentDate() );

}
void KDatePicker::slotSetDate( QDate date )
{
 
 if(date.isValid()) {
        QString temp;
        // -----
        table->setDate(date);
        selectMonth->setText(KGlobal::locale()->monthName(date.month(), false));
        temp.setNum(date.year());
        selectYear->setText(temp);
        //line->setText(KGlobal::locale()->formatDate(date, true));
        lineDate->setDate( date );
    }

}
bool
KDatePicker::setDate(const QDate& date)
{
    table->setFocus();
    if(date.isValid()) {
        QString temp;
        // -----
        table->setDate(date);
        selectMonth->setText(KGlobal::locale()->monthName(date.month(), false));
        temp.setNum(date.year());
        selectYear->setText(temp);
        //line->setText(KGlobal::locale()->formatDate(date, true));
        lineDate->setDate( date );
        return true;
    } else {

        return false;
    }


}

void
KDatePicker::monthForwardClicked()
{
    QDate temp=table->getDate();
    int day=temp.day();
    // -----
    if(temp.month()==12) {
	temp.setYMD(temp.year()+1, 1, 1);
    } else {
	temp.setYMD(temp.year(), temp.month()+1, 1);
    }
    if(temp.daysInMonth()<day) {
	temp.setYMD(temp.year(), temp.month(), temp.daysInMonth());
    } else {
	temp.setYMD(temp.year(), temp.month(), day);
    }
    // assert(temp.isValid());
    setDate(temp);
}

void
KDatePicker::monthBackwardClicked()
{
  QDate temp=table->getDate();
  int day=temp.day();
  // -----
  if(temp.month()==1)
    {
      temp.setYMD(temp.year()-1, 12, 1);
    } else {
      temp.setYMD(temp.year(), temp.month()-1, 1);
    }
  if(temp.daysInMonth()<day)
    {
      temp.setYMD(temp.year(), temp.month(), temp.daysInMonth());
    } else {
      temp.setYMD(temp.year(), temp.month(), day);
    }
  // assert(temp.isValid());
  setDate(temp);
}

void
KDatePicker::yearForwardClicked()
{
  QDate temp=table->getDate();
  int day=temp.day();
  // -----
  temp.setYMD(temp.year()+1, temp.month(), 1);
  if(temp.daysInMonth()<day)
    {
      temp.setYMD(temp.year(), temp.month(), temp.daysInMonth());
    } else {
      temp.setYMD(temp.year(), temp.month(), day);
    }
  // assert(temp.isValid());
  setDate(temp);
}

void
KDatePicker::yearBackwardClicked()
{
  QDate temp=table->getDate();
  int day=temp.day();
  // -----
  temp.setYMD(temp.year()-1, temp.month(), 1);
  if(temp.daysInMonth()<day)
    {
      temp.setYMD(temp.year(), temp.month(), temp.daysInMonth());
    } else {
      temp.setYMD(temp.year(), temp.month(), day);
    }
  // assert(temp.isValid());
  setDate(temp);
}

void
KDatePicker::selectMonthClicked()
{
  int month;
  KPopupFrame* popup = new KPopupFrame(this);
  KDateInternalMonthPicker* picker = new KDateInternalMonthPicker(popup);
  // -----
  picker->resize(picker->sizeHint());
  popup->setMainWidget(picker);
  picker->setFocus();
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  if(popup->exec(selectMonth->mapToGlobal(QPoint(0, selectMonth->height()))))
    {
      QDate date;
      int day;
      // -----
      month=picker->getResult();
      date=table->getDate();
      day=date.day();
      // ----- construct a valid date in this month:
      date.setYMD(date.year(), month, 1);
      date.setYMD(date.year(), month, QMIN(day, date.daysInMonth()));
      // ----- set this month
      setDate(date);
    } else {
      KNotifyClient::beep();
    }
  delete popup;
}

void
KDatePicker::selectYearClicked()
{
  int year;
  KPopupFrame* popup = new KPopupFrame(this);
  KDateInternalYearSelector* picker = new KDateInternalYearSelector(fontsize, popup);
  // -----
  picker->resize(picker->sizeHint());
  popup->setMainWidget(picker);
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  picker->setFocus();
  if(popup->exec(selectYear->mapToGlobal(QPoint(0, selectMonth->height()))))
    {
      QDate date;
      int day;
      // -----
      year=picker->getYear();
      date=table->getDate();
      day=date.day();
      // ----- construct a valid date in this month:
      date.setYMD(year, date.month(), 1);
      date.setYMD(year, date.month(), QMIN(day, date.daysInMonth()));
      // ----- set this month
      setDate(date);
    } else {
      KNotifyClient::beep();
    }
  delete popup;
}

void
KDatePicker::setEnabled(bool enable)
{
  QWidget *widgets[]= {
    yearForward, yearBackward, monthForward, monthBackward,
    selectMonth, selectYear,
    lineDate, table };
  const int Size=sizeof(widgets)/sizeof(widgets[0]);
  int count;
  // -----
  for(count=0; count<Size; ++count)
    {
      widgets[count]->setEnabled(enable);
    }
}

void
KDatePicker::lineEnterPressed()
{
  QDate temp;
  // -----
  temp = lineDate->date();
  //if(val->date(line->text(), temp)==QValidator::Acceptable)
  //{
	emit(dateEntered(temp));
	setDate(temp);
   //  } else {
//       KNotifyClient::beep();
//     }
}

QSize
KDatePicker::sizeHint() const
{
  QSize tableSize=table->sizeHint();
  QWidget *buttons[]={
    yearBackward,
    monthBackward,
    selectMonth,
    selectYear,
    monthForward,
    yearForward };
  const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
  QSize sizes[NoOfButtons];
  int cx=0, cy=0, count;
  // ----- store the size hints:
  for(count=0; count<NoOfButtons; ++count)
    {
      sizes[count]=buttons[count]->sizeHint();
      if(buttons[count]==selectMonth)
	{
	  cx+=maxMonthRect.width()+15;
	} else {
	  cx+=sizes[count].width()+15;
	}
      cy=QMAX(sizes[count].height(), cy);
    }
  // ----- calculate width hint:
  cx=QMAX(cx, tableSize.width()); // line edit ignored
  if ( cx > QApplication::desktop()->width() -5 )
      cx = QApplication::desktop()->width() -5;
  // ----- calculate height hint:
  cy+=tableSize.height()+lineDate->sizeHint().height();
 
  return QSize(cx, cy);
}

void
KDatePicker::setFontSize(int s)
{
  QWidget *buttons[]= {
    // yearBackward,
    // monthBackward,
    selectMonth,
    selectYear,
    // monthForward,
    // yearForward
  };
  const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
  int count;
  QFont font;
  QRect r;
  // -----
  fontsize=s;
  for(count=0; count<NoOfButtons; ++count)
    {
      font=buttons[count]->font();
      font.setPointSize(s);
      buttons[count]->setFont(font);
    }
  QFontMetrics metrics(selectMonth->fontMetrics());
  for(int i=1; i <= 12; ++i)
    { // maxMonthRect is used by sizeHint()
      r=metrics.boundingRect(KGlobal::locale()->monthName(i, false));
      maxMonthRect.setWidth(QMAX(r.width(), maxMonthRect.width()));
      maxMonthRect.setHeight(QMAX(r.height(),  maxMonthRect.height()));
    }
  table->setFontSize(s);
}

void KDatePicker::virtual_hook( int id, void* data )
{ /*BASE::virtual_hook( id, data );*/ }

void  KDatePicker::keyPressEvent ( QKeyEvent * e ) 
{
    switch ( e->key() ) {
    case Qt::Key_Right:
            monthForwardClicked();
          break;
    case Qt::Key_Left:
            monthBackwardClicked();
        break;
        
    case Qt::Key_Down:
        yearForwardClicked();
        
        break;
              
    case Qt::Key_Up:
         yearBackwardClicked();
        break;             
   
    case Qt::Key_T:
        goToday();
        break;             
   
    case Qt::Key_Return:  
    case Qt::Key_Enter:  
    case Qt::Key_Space:  
        tableClickedSlot();
        break;
     case Qt::Key_Escape:
         e->ignore();
        break;
    default:
          break;
    }

}
