/*
    This file is part of libkdepim.

    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <qapplication.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpushbutton.h>

#include <kdatepicker.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <qpalette.h>

#include "kdateedit.h"
//#include "kdateedit.moc"

KDateEdit::KDateEdit(QWidget *parent, const char *name, bool withoutDP )
  : QHBox(parent, name)
{
  dateFormShort = true;
  withoutDp = withoutDP;
  mDateEdit = new QLineEdit(this);
  mDateEdit->setText(KGlobal::locale()->formatDate(QDate::currentDate(),dateFormShort));
  setFocusProxy(mDateEdit);
  mDateEdit->installEventFilter(this);

  // Highlight Background and Textcolor
  QPalette palette = QWidget::palette();
  unsigned char red, green, blue;
  red = palette.color( QPalette::Normal , QColorGroup::Background ).red() - 10;
  green = palette.color( QPalette::Normal , QColorGroup::Background ).green() - 10;
  blue = palette.color( QPalette::Normal , QColorGroup::Background ).blue() - 10;
  palette.setColor( QColorGroup::Highlight, QColor(red,green,blue) );
  palette.setColor( QColorGroup::HighlightedText, palette.color( QPalette::Normal , QColorGroup::Foreground ) );
  mDateEdit->setPalette( palette );

  if ( withoutDP ) {
      mDateFrame = 0;
      mDateButton = 0;
      mDatePicker = 0;
  } else {
      QPixmap pixmap = SmallIcon("smallcal");
      mDateButton = new QPushButton(this);
      mDateButton->setPixmap(pixmap);
      QPixmap pixmap2 = SmallIcon("today_small");
      QPushButton*  nowButton = new QPushButton(this);
      nowButton->setPixmap(pixmap2);

      mDateFrame = new QVBox(0,0,Qt::WType_Popup);
      // mDateFrame->setFrameStyle(QFrame::PopupPanel | QFrame::Raised);
      mDateFrame->setFrameStyle( QFrame::WinPanel |QFrame::Raised );
      mDateFrame->setLineWidth(3);
      mDateFrame->hide();

      mDatePicker = new KDatePicker(mDateFrame,QDate::currentDate());
      connect(mDatePicker,SIGNAL(dateEntered(QDate)),SLOT(setDate(QDate)));
      connect(mDatePicker,SIGNAL(dateEntered(QDate)),SIGNAL(dateChanged(QDate)));
      connect(mDatePicker,SIGNAL(dateSelected(QDate)),SLOT(setDate(QDate)));
      connect(mDatePicker,SIGNAL(dateSelected(QDate)),SIGNAL(dateChanged(QDate)));
      connect(mDatePicker,SIGNAL(dateSelected(QDate)),mDateFrame,SLOT(hide()));
      connect(mDateButton,SIGNAL(clicked()),SLOT(toggleDatePicker()));
      connect(nowButton,SIGNAL(clicked()),SLOT(goToNow()));
  
#ifdef QT_4_COMPAT
      mDateButton->setFocusPolicy( Qt::NoFocus ); 
#else
      mDateButton->setFocusPolicy( QWidget::NoFocus );  
#endif
      mDateButton->setAutoDefault( false ); 
      //mDateFrame->resize( 400, 300 );
      
  }
      connect(mDateEdit,SIGNAL(returnPressed()),SLOT(lineEnterPressed()));
      connect(mDateEdit,SIGNAL(textChanged(const QString &)),
              SLOT(textChanged(const QString &)));

  // Create the keyword list. This will be used to match against when the user
  // enters information.
  mKeywordMap[i18n("tomorrow")] = 1;
  mKeywordMap[i18n("today")] = 0;
  mKeywordMap[i18n("yesterday")] = -1;

  /*
   * This loop uses some math tricks to figure out the offset in days
   * to the next date the given day of the week occurs. There
   * are two cases, that the new day is >= the current day, which means
   * the new day has not occured yet or that the new day < the current day,
   * which means the new day is already passed (so we need to find the
   * day in the next week).
   */
  QString dayName;
  int currentDay = QDate::currentDate().dayOfWeek();
  for (int i = 1; i <= 7; ++i)
  {
    dayName = KGlobal::locale()->weekDayName(i).lower();
    if (i >= currentDay)
      mKeywordMap[dayName] = i - currentDay;
    else
      mKeywordMap[dayName] = 7 - currentDay + i;
  }

  mTextChanged = false;
  mHandleInvalid = false;
  // QWidget::setTabOrder( mDateEdit, mDateButton );
}

KDateEdit::~KDateEdit()
{
  delete mDateFrame;
}
void KDateEdit::clear()
{
  bool b = mDateEdit->signalsBlocked();
  mDateEdit->blockSignals(true);
  mDateEdit->setText("");
  mDateEdit->blockSignals(b);
}
void KDateEdit::goToNow()
{
    setDate(QDate::currentDate() );
    emit setTimeTo( QTime::currentTime() );
}
void KDateEdit::setDate(QDate newDate)
{
  if (!newDate.isValid() && !mHandleInvalid)
    return;
  if ( readDate() ==  newDate )
      return;
  QString dateString = "";
  if(newDate.isValid())
    dateString = KGlobal::locale()->formatDate( newDate, dateFormShort );

  mTextChanged = false;

  // We do not want to generate a signal here, since we explicity setting
  // the date
  bool b = mDateEdit->signalsBlocked();
  mDateEdit->blockSignals(true);
  mDateEdit->setText(dateString);
  mDateEdit->blockSignals(b);
}

void KDateEdit::setDate( QDate date,int *cpos,const int key ,const bool dateFormShort)
{
  QString dateForm = dateFormShort ?
                     KGlobal::locale()->dateFormatShort() :
                     KGlobal::locale()->dateFormat();

  int begin = dateForm.find("%");
  int space = 0;
  int allStrLength = 0;
  int strLength = 0;
  int repeat = 0;

  // witch? Day, Month or Year switch?
  while(1){
    switch ( dateForm.at(begin + 1).latin1() )
    {
    case 'd':// 16 (month day)
      strLength = 2; //Ok
      break;
    case 'm':// 01 (month)
      strLength = 2; //Ok
      break;
    case 'a':// Mon (Weekday)
      strLength = KGlobal::locale()->weekDayName(date.dayOfWeek(), true).length();
      break;
    case 'A':// Monday (Weekday)
      strLength = KGlobal::locale()->weekDayName(date.dayOfWeek(), false).length();
      break;
    case 'b':// Jan (monthName)
      strLength = KGlobal::locale()->monthName(date.month(), true).length();
      break;
    case 'B':// January (monthName)
      strLength = KGlobal::locale()->monthName(date.month(), false).length();
      break;
    case 'y':// 04 (year short)
      strLength = 2; //Ok
      break;
    case 'Y':// 2004 (year)
      strLength = 4; //Ok
      break;
    default:
      break;
    }
    space = begin - (repeat++ * 2);
    // all select? then dayswitch
    if( (mDateEdit->text().length() == mDateEdit->markedText().length() ) &&
                               ( (dateForm.at(begin + 1).latin1() == 'd') ||
                                 (dateForm.at(begin + 1).latin1() == 'a') ||
                                 (dateForm.at(begin + 1).latin1() == 'A') ) ) {
      break;
    }
    // mDateEdit-StringPos == CursorPosition(cpos) then break and set date
    if( ( (space + allStrLength) <= *cpos && *cpos <= (space + allStrLength + strLength) ) || *cpos < begin ) {
      break;
    }
    allStrLength += strLength;
    begin = dateForm.find("%", begin +1);
  }

  // set date
  switch ( dateForm.at(begin + 1).latin1() ) {
    case 'd':
    case 'a':
    case 'A':
      if(key == Qt::Key_Up) {
        setDate( date.addDays( 1 ) );
      }
      else if(key == Qt::Key_Down) {
        setDate( date.addDays( -1 ) );
      }
      maxDay = readDate().day();
      break;
    case 'm':
    case 'b':
    case 'B':
      if(key == Qt::Key_Up) {
          int year = ((date.month()+1)>12)?date.year()+1:date.year();
          int month = ((date.month()+1)>12)?1:date.month()+1;
          int day = (QDate(year,month,1).daysInMonth()<maxDay)?QDate(year,month,1).daysInMonth():maxDay;
          setDate( QDate( year, month, day ) );
      } else if(key == Qt::Key_Down) {
          int year = ((date.month()-1)<1)?date.year()-1:date.year();
          int month = ((date.month()-1)<1)?12:date.month()-1;
          int day = (QDate(year,month,1).daysInMonth()<maxDay)?QDate(year,month,1).daysInMonth():maxDay;
          setDate( QDate( year, month, day ) );
      }
      break;
    case 'y':
    case 'Y':
      if(key == Qt::Key_Up) {
        setDate( QDate( date.year() + 1, date.month() , date.day()) );
      }
      else if(key == Qt::Key_Down) {
        setDate( QDate( date.year() - 1, date.month() , date.day()) );
      }
      break;
/*    default:
      if(key == Key_Up) {
        setDate( date.addDays( 1 ) );
      } else if(key == Key_Down) {
        setDate( date.addDays( -1 ) );
      }
      break;*/
  }

  date = readDate();
  begin = dateForm.find("%");
  int allSelectStrLength = 0;
  int selectStrLength = 0;

  // set selection do new date an set cursor at end of selection
  for(int i = 0; i < repeat; i++){
    switch ( dateForm.at(begin + 1).latin1() )
    {
    case 'd':// 16 (month day)
      selectStrLength = 2; //Ok
      break;
    case 'm':// 01 (month)
      selectStrLength = 2; //Ok
      break;
    case 'a':// Mon (Weekday short)
      selectStrLength = KGlobal::locale()->weekDayName(date.dayOfWeek(), true).length();
      break;
    case 'A':// Monday (Weekday)
      selectStrLength = KGlobal::locale()->weekDayName(date.dayOfWeek(), false).length();
      break;
    case 'b':// Jan (monthName short)
      selectStrLength = KGlobal::locale()->monthName(date.month(), true).length();
      break;
    case 'B':// January (monthName)
      selectStrLength = KGlobal::locale()->monthName(date.month(), false).length();
      break;
    case 'y':// 04 (year short)
      selectStrLength = 2; //Ok
      break;
    case 'Y':// 2004 (year)
      selectStrLength = 4; //Ok
      break;
    default:
      break;
    }
    space = begin - (i * 2);
    allSelectStrLength += selectStrLength;
    begin = dateForm.find("%", begin +1);
  }
  // set selection from begin of date
  setSelect(  space + allSelectStrLength - selectStrLength , selectStrLength);
  *cpos = space + allSelectStrLength;
  emit(dateChanged(date));

  return;
}

void KDateEdit::setHandleInvalid(bool handleInvalid)
{
  mHandleInvalid = handleInvalid;
}

void KDateEdit::setEnabled(bool on)
{
  mDateEdit->setEnabled(on);
  mDateButton->setEnabled(on);
}

QDate KDateEdit::date() const
{
  QDate date = readDate();

  if (date.isValid() || mHandleInvalid) {
    return date;
  } else {
    KNotifyClient::beep();
    return QDate::currentDate();
  }
}

void KDateEdit::keyPressEvent(QKeyEvent *e)
{
  QDate date = readDate();
  int cpos = mDateEdit->cursorPosition();

  switch(e->key())
  {
  case Qt::Key_Escape:
      mDateEdit->deselect();
  case Qt::Key_Tab:
      QHBox::keyPressEvent(e);
      break;
  case Qt::Key_Up:
      // when date invalid then set to currend and return
      if(!date.isValid()) {
        date = QDate::currentDate();
        setDate(date);
        mDateEdit->setCursorPosition(cpos);
        emit(dateChanged(date));
        QString text = i18n( "You entered an invalid date!\n Date changed  to current date." );
        KMessageBox::information( 0, text );
        return;
      }
      setDate(date, &cpos, Qt::Key_Up, dateFormShort);
      break;
  case Qt::Key_Down:
      // when date invalid then set to current and return
      if(!date.isValid()) {
        date = QDate::currentDate();
        setDate(date);
        mDateEdit->setCursorPosition(cpos);
        emit(dateChanged(date));
        QString text = i18n( "You entered an invalid date!\n Date changed  to current date." );
        KMessageBox::information( 0, text );
        return;
      }
      setDate(date, &cpos, Qt::Key_Down, dateFormShort);
      break;
  default:
      QHBox::keyPressEvent(e);
      break;
  } // switch
  mDateEdit->setCursorPosition(cpos);
}

void KDateEdit::setSelect( int from, int to )
{
//    return;
    mDateEdit->setSelection( from , to );
}

void KDateEdit::toggleDatePicker()
{
  if( mDateFrame->isVisible() ) {
    mDateFrame->hide();
  } else {
    QPoint tmpPoint = mapToGlobal(mDateButton->geometry().bottomRight());
    QSize datepickersize = mDatePicker->sizeHint();

    if ( tmpPoint.x() < 7+datepickersize.width() ) tmpPoint.setX( 7+datepickersize.width() );

    int h = QApplication::desktop()->height();

    if ( tmpPoint.y() + datepickersize.height() > h ) tmpPoint.setY( h - datepickersize.height() );

    mDateFrame->setGeometry(tmpPoint.x()-datepickersize.width()-7, tmpPoint.y(),
     datepickersize.width()+2*mDateFrame->lineWidth(), datepickersize.height()+2*mDateFrame->lineWidth());

    QDate date = readDate();
    if(date.isValid()) {
      mDatePicker->setDate(date);
    } else {
      mDatePicker->setDate(QDate::currentDate());
    }
    mDateFrame->show();
  }
}


void KDateEdit::lineEnterPressed()
{
    QDate date = readDate();

    if(date.isValid())
        {
            // Update the edit. This is needed if the user has entered a
            // word rather than the actual date.
            setDate(date);
            emit(dateChanged(date));
            emit returnPressed();
        }
    else
      {
          if ( withoutDp ) {
              KNotifyClient::beep();
          } else {
              if ( !mDateEdit->text().isEmpty() ) {
                  mTextChanged = false;
                  QString text = i18n( "You entered an invalid date!\n Will use current date instead." );
                  if ( isVisible() )
                      if ( KMessageBox::warningContinueCancel( 0, text ) == KMessageBox::Continue ) {
                          setDate( QDate::currentDate() );
                          emit dateChanged( QDate::currentDate() );
                      }
              }
          }
      }
}

bool KDateEdit::inputIsValid()
{
  return readDate().isValid();
}

QDate KDateEdit::readDate() const
{
  QString text = mDateEdit->text();
  QDate date;

  if (mKeywordMap.contains(text.lower()))
  {
    date = QDate::currentDate().addDays(mKeywordMap[text.lower()]);
  }
  else
  {
    date = KGlobal::locale()->readDate(text);
  }

  return date;
}

bool KDateEdit::eventFilter(QObject *, QEvent *e)
{
  // We only process the focus out event if the text has changed
  // since we got focus
  if ((e->type() == QEvent::FocusOut) )
  {
      if ( mTextChanged ) {
          lineEnterPressed();
          mTextChanged = false;
      }
  }
  // switch dateFormShort by double klick with mouse
  else if (e->type() == QEvent::MouseButtonDblClick)
  {
      toggleDateFormat();
  }
  else if (e->type() == QEvent::FocusIn) 
  {
    maxDay = readDate().day();
  }

  return false;
}
void KDateEdit::toggleDateFormat()
{
    dateFormShort = ! dateFormShort;
    mDateEdit->setText(KGlobal::locale()->formatDate(readDate(),dateFormShort));
    
}

void KDateEdit::textChanged(const QString &)
{
  if(mHandleInvalid && mDateEdit->text().stripWhiteSpace().isEmpty()) {
    QDate date; //invalid date
    emit(dateChanged(date));
  } else  {
    mTextChanged = true;
  }
    maxDay = readDate().day();
}
