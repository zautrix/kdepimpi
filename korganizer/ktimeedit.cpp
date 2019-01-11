/*
    This file is part of KOrganizer.
    Copyright (c) 1999 Preston Brown, Ian Dawes

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

#include <qkeycode.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qapplication.h>

#include <kmessagebox.h>
#include <kglobal.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpimglobalprefs.h>

#include "ktimeedit.h"
#include "koprefs.h"
#include <qvalidator.h>

// Validator for a time value with only hours and minutes (no seconds)
// Mostly locale aware. Author: David Faure <faure@kde.org>

// KTimeWidget/QTimeEdit provide nicer editing, but don't provide a combobox.
// Difficult to get all in one...
// But Qt-3.2 will offer QLineEdit::setMask, so a "99:99" mask would help.
KOTimeEdit::KOTimeEdit(QWidget *parent, QTime qt, const char *name)
  : QComboBox(TRUE, parent, name)
{
  setInsertionPolicy(NoInsertion);
    mFlagKeyPressed = false;
    if ( QApplication::desktop()->height() <= 480 ) {
        setSizeLimit ( 6 ); 
    } else {
        setSizeLimit ( 12 ); 
    }
    mTime = qt;

//  mNoTimeString = i18n("No Time");
//  insertItem( mNoTimeString );

  // Fill combo box with selection of times in localized format.
  QTime timeEntry(0,0,0);
  do {
    insertItem(KGlobal::locale()->formatTime(timeEntry));
    timeEntry = timeEntry.addSecs(60*15);
  } while (!timeEntry.isNull());
  // Add end of day.
  insertItem( KGlobal::locale()->formatTime( QTime( 23, 59, 59 ) ) );

  updateText();
  setFocusPolicy(QWidget::StrongFocus);

  connect(this, SIGNAL(activated(int)), this, SLOT(activ(int)));
  connect(this, SIGNAL(highlighted(int)), this, SLOT(hilit(int)));
  connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(changedText()));
  QFontMetrics fm ( font() );
  QString timeString = "24:00";
  if ( KPimGlobalPrefs::instance()->mPreferredTime == 1 )
      timeString = "02:00pm";
  int addSpace = 32;
  if ( QApplication::desktop()->width() > 320 )
      timeString += ":00";
  setFixedWidth(fm.width( timeString ) + 32 );

  // Highlight Background and Textcolor change from default
  QPalette palette = QWidget::palette();
  unsigned char red, green, blue;
  red = palette.color( QPalette::Normal , QColorGroup::Background ).red() - 10;
  green = palette.color( QPalette::Normal , QColorGroup::Background ).green() - 10;
  blue = palette.color( QPalette::Normal , QColorGroup::Background ).blue() - 10;
  palette.setColor( QColorGroup::Highlight, QColor(red,green,blue) );
  palette.setColor( QColorGroup::HighlightedText, palette.color( QPalette::Normal , QColorGroup::Foreground ) );
  setPalette( palette );
}

KOTimeEdit::~KOTimeEdit()
{
}

bool KOTimeEdit::hasTime() const
{
  // Can't happen
  if ( currentText().isEmpty() ) return false;
  //if ( currentText() == mNoTimeString ) return false;

  return true; // always
}

QTime KOTimeEdit::getTime() const
{
  return KGlobal::locale()->readTime(currentText());
}
/*
QSizePolicy  KOTimeEdit::sizePolicy() const
{
  // Set size policy to Fixed, because edit cannot contain more text than the
  // string representing the time. It doesn't make sense to provide more space.
  QSizePolicy sizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  return sizePolicy;
}
*/
void KOTimeEdit::setTime(QTime newTime)
{
  if ( mTime != newTime )
  {
    mTime = newTime;
    updateText();
  }

}

void KOTimeEdit::activ(int i)
{
    // The last entry, 23:59, is a special case
    if( i == count() - 1 )
	mTime = QTime( 23, 59, 0 );
    else
	mTime = QTime(0,0,0).addSecs(i*15*60);
    emit timeChanged(mTime);
}

void KOTimeEdit::hilit(int )
{
  // we don't currently need to do anything here.
}

void KOTimeEdit::addTime(QTime qt, bool update)
{
  // Calculate the new time.
    //qDebug("add h %d min %d  ", qt.hour(),qt.minute() );
  mTime = mTime.addSecs(qt.minute()*60+qt.hour()*3600);
  // if ( update )
      updateText();
  emit timeChanged(mTime);
}

void KOTimeEdit::subTime(QTime qt, bool update)
{
  int h, m;
  //qDebug("sub  h %d min %d  ", qt.hour(),qt.minute() );

  mTime = mTime.addSecs(-(qt.minute()*60+qt.hour()*3600));
  // store the newly calculated time.
  // mTime.setHMS(h, m, 0);
  //if ( update )
      updateText();
  emit timeChanged(mTime);
}

// void KOTimeEdit::mouseReleaseEvent ( QMouseEvent * )
// {
//     qDebug("mouseReleaseEvent ( QMouseEvent * )  ");
// }

// void KOTimeEdit::focusInEvent ( QFocusEvent * )
// {
//     qDebug("focusInEvent ( QFocusEvent * )   ");
// }

void KOTimeEdit::keyReleaseEvent(QKeyEvent *e)
{
  if ( !e->isAutoRepeat() ) {
      mFlagKeyPressed = false;
    }

}
void KOTimeEdit::setSelect( int from, int to )
{
    if ( KOPrefs::instance()->mHightlightDateTimeEdit)
        lineEdit()->setSelection( from , to );
}


void KOTimeEdit::keyPressEvent(QKeyEvent *e)
{

  qApp->processEvents();
  bool hour12Format =  ( KPimGlobalPrefs::instance()->mPreferredTime == 1 );
  int maxpos = hour12Format?7:5;
  if ( e->isAutoRepeat() && !mFlagKeyPressed ) {
    e->ignore();
    // qDebug("  ignore  %d",e->isAutoRepeat()  );
    return;
  }
  if (! e->isAutoRepeat() ) {
    mFlagKeyPressed = true;
  }
  // Tap -> Focus Next Widget
  if ( e->key() == Key_Tab ) {
    QComboBox::keyPressEvent(e);
    return;
  }

  // save Text from QLineEdit and CursorPosition
  QString text = lineEdit()->text();
  int cpos = lineEdit()->cursorPosition();
  // qDebug("cpos %d  ", cpos);

  // Switch for arrows, backspace and escape
  switch(e->key()) {
  case Key_Escape:
      lineEdit()->deselect();
  case Key_Tab:
      QComboBox::keyPressEvent(e);
    break;
  case Key_Up:
      if ( e->state () == Qt::ControlButton ) {
          addTime(QTime(0,15,0), false );
          lineEdit()->setCursorPosition(3);
          setSelect( 3 , 2 );
      }
      else
          if ( e->state () == Qt::ShiftButton ) {
              addTime(QTime(1,0,0), false );
              lineEdit()->setCursorPosition(0);
              setSelect( 0 , 2 );
          }
          else
              // switch time up, cursor location depend
              switch (cpos) {
                  case 7:
                  case 6:
                  case 5:
                      if(!hour12Format) {
                          lineEdit()->setCursorPosition(cpos = 4);
                      } else {
                          addTime(QTime(12,0,0), false );
                          setSelect ( 5 , 2 );
                          break;
                      }
                  case 4:
                      addTime(QTime(0,1,0), false );
                      setSelect ( cpos , 1 );
                      break;
                  case 3:
                      addTime(QTime(0,10,0), false );
                      setSelect ( cpos , 1 );
                      break;
                  case 2:
                      lineEdit()->setCursorPosition(--cpos);
                  case 1:
                  case 0:
                      addTime(QTime(1,0,0), false );
                      setSelect ( 0, 2 );
                      break;
              }
      break;
  case Key_Down:
      if ( e->state () == Qt::ControlButton ) {
          subTime(QTime(0,15,0), false );
          lineEdit()->setCursorPosition(3);
          setSelect( 3 , 2 );
      }
      else
          if ( e->state () == Qt::ShiftButton ) {
              subTime(QTime(1,0,0), false );
              lineEdit()->setCursorPosition(0);
              setSelect( 0 , 2 );
          }
          else
              // switch time down, cursor location depend
              switch (cpos) {
                  case 7:
                  case 6:
                  case 5:
                      if(!hour12Format) {
                          lineEdit()->setCursorPosition(cpos = 4);
                      } else {
                          subTime(QTime(12,0,0), false );
                          setSelect ( 5 , 2 );
                          break;
                      }
                  case 4:
                      subTime(QTime(0,1,0), false );
                      setSelect ( cpos , 1 );
                      break;
                  case 3:
                      subTime(QTime(0,10,0), false );
                      setSelect ( cpos , 1 );
                      break;
                  case 2:
                      lineEdit()->setCursorPosition(--cpos);
                  case 1:
                  case 0:
                      subTime(QTime(1,0,0), false );
                      setSelect ( 0 , 2 );
                      break;
              }
      break;
  // set cursor to correct place
  case Key_Left:
    if ( cpos == 3 )
        --cpos;
    if ( cpos >  0) {
        lineEdit()->setCursorPosition(--cpos);
        setSelect ( cpos , 1 );
    }
    else
        setSelect ( 0 , 1 );
    break;
  // set cursor to correct place
  case Key_Right:
    if ( cpos == 1 )
        ++cpos;
    if ( cpos < maxpos ) {
        lineEdit()->setCursorPosition(++cpos);
        setSelect ( cpos , 1 );
    }
    break;
  // rest
  case Key_Prior:
    subTime(QTime(1,0,0));
    break;
  case Key_Next:
    addTime(QTime(1,0,0));
    break;
  case Key_Backspace:
      if ( cpos >  0) {
          if ( cpos == 3 )
              --cpos;
          if ( cpos > 5)
              cpos = 5;
          text.at( cpos-1 ) = '0';
          lineEdit()->setText( text );
          lineEdit()->setCursorPosition(--cpos);
          setSelect ( cpos , 1 );
      changedText();
      }
    break;
  } // switch arrows

  // if cursor at string end, alltext market and keyEvent don't ArrowLeft -> deselect and cpos
  if( cpos > 4 && lineEdit()->markedText().length() == 5 && e->key() != Key_Left ) {
      lineEdit()->deselect();
      cpos = 0;
      lineEdit()->setCursorPosition(cpos);
      setSelect(cpos , 1);
  }

  if (  cpos == 2 ) {
      lineEdit()->setCursorPosition(++cpos);
  }

  // num keys when cursorPos preEnd
  if (  cpos  < 5  ) {
      // switch another keys
      switch(e->key()) {
      case Key_Delete:
          text.at( cpos ) = '0';
          lineEdit()->setText( text );
          lineEdit()->setCursorPosition(cpos);
          setSelect ( cpos , 1 );
          changedText();
          break;
      case Key_9:
      case Key_8:
      case Key_7:
      case Key_6:
          if ( !(cpos == 1 ||  cpos == 4)  )
              return;
          if ( cpos == 1  && text.at( 0 ) > '1')
              text.at( 0 ) = '1';
      case Key_5:
      case Key_4:
      case Key_3:
          if ( cpos < 1 )
              return;
          if ( hour12Format  && cpos == 1  )
              return;
      case Key_2:
          if ( hour12Format  && cpos == 0 )
              return;
          if ( cpos == 0  && text.at( 1 ) > '3')
              text.at( 1 ) = '3';
      case Key_1:
      case Key_0:
          if ( hour12Format ) {
              if ( e->key() == Key_0  && cpos == 1 && text.at( 0 ) == '0' )
                  return;
              if ( e->key() == Key_0  && cpos == 0 && text.at( 1 ) == '0' )
                 text.at( 1 ) = '1';
          }
          text.at( cpos ) = QChar ( e->key() );
          lineEdit()->setText( text );
          if ( cpos == 1 )
            ++cpos;
        if ( cpos <  5)
            lineEdit()->setCursorPosition(++cpos);
            setSelect( cpos , 1 );
            changedText();
        break;
      case Key_Home:
            lineEdit()->setCursorPosition(0);
            setSelect( cpos , 1 );
        break;
      case Key_End:
            lineEdit()->setCursorPosition(5);
            lineEdit()->deselect();
        break;
      default:
//          QComboBox::keyPressEvent(e);
        break;
      } // switch num keys
  } else if ( cpos == 5 ) {// if cpos < 5
      if ( hour12Format ) {
          if ( e->key() == Key_A ) {
              text.at( 5 ) = 'a';
              lineEdit()->setText( text );
              lineEdit()->setCursorPosition(5);

          } else if ( e->key() == Key_P ) {
              text.at( 5 ) = 'p';
              lineEdit()->setText( text );
              lineEdit()->setCursorPosition(5);

          }
      }
  }


}

void KOTimeEdit::updateText()
{ 
    listBox()->blockSignals( true );
    blockSignals( true );
    QString s = KGlobal::locale()->formatTime(mTime);
    // Set the text but without emitting signals, nor losing the cursor position
    QLineEdit *line = lineEdit();
    line->blockSignals(true);
    int pos = line->cursorPosition();
    setCurrentItem((mTime.hour()*4)+(mTime.minute()/15));
    line->setText(s);
    line->setCursorPosition(pos);
    line->blockSignals(false);
    blockSignals( false );
    listBox()->blockSignals( false );

}

void KOTimeEdit::changedText()
{
    int pos = lineEdit()->cursorPosition();
    mTime = getTime();
    blockSignals( true );
    QString text = lineEdit()->text();
    setCurrentItem((mTime.hour()*4)+(mTime.minute()/15));
    lineEdit()->setText(text);
    blockSignals( false );
    emit timeChanged(mTime);
    lineEdit()->setCursorPosition(pos); 
}
