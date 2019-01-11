/*
    This file is part of KOrganizer.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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

#include <qstring.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qapplication.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include "libkdepim/kdatepicker.h"
#include <knotifyclient.h>
#include "kdatetbl.h"

#include "koglobals.h"
#include <kglobalsettings.h>
#include "koprefs.h"
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif

#include <kcalendarsystem.h>

#include "navigatorbar.h"

NavigatorBar::NavigatorBar( const QDate & date, QWidget *parent, const char *name )
  : QWidget( parent, name )
{
  QBoxLayout *topLayout = new QHBoxLayout( this );

  // Set up the control buttons and date label
  mCtrlFrame = new QFrame( this );
  mCtrlFrame->setFrameStyle(QFrame::Panel|QFrame::Raised);
  mCtrlFrame->setLineWidth(1);

  topLayout->addWidget( mCtrlFrame );

 

  bool isRTL = KOGlobals::self()->reverseLayout();
#ifndef DESKTOP_VERSION
  bool isDesktop = false;
#else
  bool isDesktop = true;
#endif
  if ( QString ( name ) == QString("useBigPixmaps") && QApplication::desktop()->width() > 320 )
      isDesktop = true;
  // Create backward navigation buttons
  mPrevYear = new QPushButton( mCtrlFrame );
  mPrevYear->setPixmap( SmallIcon( isDesktop ? "3leftarrowB" : "3leftarrow" ) );
  QToolTip::add( mPrevYear, i18n("Previous Year") );

  mPrevMonth = new QPushButton( mCtrlFrame );
  mPrevMonth->setPixmap( SmallIcon( isDesktop ? "2leftarrowB" : "2leftarrow") );
  QToolTip::add( mPrevMonth, i18n("Previous Month") );

  // Create forward navigation buttons
  mNextMonth = new QPushButton( mCtrlFrame );
  mNextMonth->setPixmap( SmallIcon( isDesktop ? "2rightarrowB" : "2rightarrow") );
  QToolTip::add( mNextMonth, i18n("Next Month") );

  mPrevWeek = new QPushButton( mCtrlFrame );
  mPrevWeek->setPixmap( SmallIcon( isDesktop ? "1leftarrowB" : "1leftarrow") );
  QToolTip::add( mPrevWeek, i18n("Previous Week") );

  // Create forward navigation buttons
  mNextWeek = new QPushButton( mCtrlFrame );
  mNextWeek->setPixmap( SmallIcon( isDesktop ? "1rightarrowB" : "1rightarrow") );
  QToolTip::add( mNextWeek, i18n("Next Week") );

  mNextYear = new QPushButton( mCtrlFrame );
  mNextYear->setPixmap( SmallIcon( isDesktop ? "3rightarrowB": "3rightarrow") );
  QToolTip::add( mNextYear, i18n("Next Year") );
  mSelectMonth = new QPushButton( mCtrlFrame );
  // Create month name label
  //selectMonth->setFont( tfont );
  // selectMonth->setAlignment( AlignCenter );
  //mDateLabel = new QLabel( selectMonth );
  //mDateLabel->setFont( tfont );
  //mDateLabel->setAlignment( AlignCenter );
  if ( QString ( name ) == QString("useBigPixmaps") ) {
      mNextMonth->setFlat( true);
      mNextWeek->setFlat( true);
      mNextYear->setFlat( true);
      mSelectMonth->setFlat( true);
      mPrevYear->setFlat( true);
      mPrevMonth->setFlat( true);
      mPrevWeek->setFlat( true);

      mPrevYear->setAutoRepeat( true );
      mPrevMonth->setAutoRepeat( true );
      mNextMonth->setAutoRepeat( true );
      mPrevWeek->setAutoRepeat( true );
      mNextWeek->setAutoRepeat( true );
      mNextYear->setAutoRepeat( true );
  } else {
      mPrevWeek->hide();
      mNextWeek->hide();
  }

  resetFont( font() );

  
  // set up control frame layout
  QBoxLayout *ctrlLayout = new QHBoxLayout( mCtrlFrame, 1 );
  ctrlLayout->addWidget( mPrevYear, 3 );
  ctrlLayout->addWidget( mPrevMonth, 3 );
  ctrlLayout->addWidget( mPrevWeek, 3 );
  //ctrlLayout->addStretch( 1 );
  // ctrlLayout->addSpacing( 1 );
  // ctrlLayout->addWidget( mDateLabel );
  ctrlLayout->addWidget( mSelectMonth );
  //  ctrlLayout->addSpacing( 1 );
  // ctrlLayout->addStretch( 1 );
  ctrlLayout->addWidget( mNextWeek, 3 );
  ctrlLayout->addWidget( mNextMonth, 3 );
  ctrlLayout->addWidget( mNextYear, 3 );

  connect( mPrevYear, SIGNAL( clicked() ), SIGNAL( goPrevYear() ) );
  connect( mPrevMonth, SIGNAL( clicked() ), SIGNAL( goPrevMonth() ) );
  connect( mNextMonth, SIGNAL( clicked() ), SIGNAL( goNextMonth() ) );
  connect( mPrevWeek, SIGNAL( clicked() ), SIGNAL( goPrevWeek() ) );
  connect( mNextWeek, SIGNAL( clicked() ), SIGNAL( goNextWeek() ) );
  connect( mNextYear, SIGNAL( clicked() ), SIGNAL( goNextYear() ) );
  connect( mSelectMonth, SIGNAL( clicked() ),SLOT( selectMonth() ) );
  mPrevYear->setFocusPolicy(NoFocus);
  mPrevMonth->setFocusPolicy(NoFocus);
  mNextMonth->setFocusPolicy(NoFocus);
  mPrevWeek->setFocusPolicy(NoFocus);
  mNextWeek->setFocusPolicy(NoFocus);
  mNextYear->setFocusPolicy(NoFocus);
  mSelectMonth->setFocusPolicy(NoFocus);
  setSizePolicy( QSizePolicy (  QSizePolicy::Expanding ,QSizePolicy::Fixed ) );
   
}

NavigatorBar::~NavigatorBar()
{
}
QSize NavigatorBar::sizeHint() const
{
    int wid = mCurrentMinWid ;
    if ( mPrevYear->isVisible() )
        wid += mCurrentButtonMinWid;
    if ( mPrevMonth->isVisible() )
        wid += mCurrentButtonMinWid;
    if ( mPrevWeek->isVisible() )
        wid += mCurrentButtonMinWid;
    if ( mNextMonth->isVisible() )
        wid += mCurrentButtonMinWid;
    if ( mNextWeek->isVisible() )
        wid += mCurrentButtonMinWid;
    if ( mNextYear->isVisible() )
        wid += mCurrentButtonMinWid;
    //qDebug("ret %d %d ", wid, mCurrentHei);
  return   QSize ( wid, mCurrentHei );
}
QSize NavigatorBar::sizeHintTwoButtons( int butNum ) const
{
    return   QSize ( mCurrentMinWid  + butNum * mCurrentButtonMinWid , mCurrentHei );
}
void NavigatorBar::resetFont ( QFont fo )
{

 QFont tfont = fo;
 if ( QApplication::desktop()->width() >= 480 )
     tfont.setPointSize(tfont.pointSize()+2);
 tfont.setBold(true);

 mSelectMonth->setFont( tfont );
  // Set minimum width to width of widest month name label
  int i;
  int maxwidth = 0;
  QFontMetrics fm ( mSelectMonth->font() );
  int width = fm.width("September '00" );
  maxwidth = width+2;
  int size = fm.height()+2;
  if ( QApplication::desktop()->width() >= 480 ) { 
      size += 6;
      maxwidth+= 6;
  }
  mSelectMonth->setMinimumWidth( maxwidth );
  mSelectMonth->setFixedHeight( size );
  mPrevYear->setFixedHeight(  size );
  mPrevMonth->setFixedHeight( size );
  mPrevWeek->setFixedHeight( size );
  mNextMonth->setFixedHeight( size );
  mNextWeek->setFixedHeight( size );
  mNextYear->setFixedHeight ( size );
  mCurrentHei = size +2;
  mCurrentMinWid =  maxwidth+2;
  mCurrentButtonMinWid = mPrevYear->sizeHint().width();
}
  
void NavigatorBar::showButtons( bool left, bool right )
{
  if ( left ) {
    mPrevYear->show();
    mPrevMonth->show();
  } else {
    mPrevYear->hide();
    mPrevMonth->hide();
  }

  if ( right ) {
    mNextYear->show();
    mNextMonth->show();
  } else {
    mNextYear->hide();
    mNextMonth->hide();
  }
  if ( !left && !right ) {
      //mSelectMonth->setMaximumWidth( 1024 );
      mSelectMonth->setSizePolicy( QSizePolicy (  QSizePolicy::Expanding ,QSizePolicy::Fixed ) );
  }
}

void NavigatorBar::selectMonth()
{

    int month;
  KPopupFrame* popup = new KPopupFrame(this);
  KDateInternalMonthPicker* picker = new KDateInternalMonthPicker(popup);
  // -----
  picker->resize(picker->sizeHint());
  popup->setMainWidget(picker);
  picker->setFocus();
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  if(popup->exec(mSelectMonth->mapToGlobal(QPoint(0, mSelectMonth->height()))))
    {
        month = picker->getResult();
        emit monthSelected ( month );
    } else {
      KNotifyClient::beep();
    }
  delete popup;
}
void NavigatorBar::selectDates( const KCal::DateList &dateList )
{
  if (dateList.count() > 0) {
    QDate date = dateList.first();

    const KCalendarSystem *calSys = KOGlobals::self()->calendarSystem();

    // compute the label at the top of the navigator
    QString dtstr = calSys->monthName( date ) + " '" +
                    QString::number( calSys->year( date ) ).right(2);

    mSelectMonth->setText( dtstr );
  }
}

