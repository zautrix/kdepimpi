/*
    This file is part of KOrganizer.

    Copyright (c) 2001,2002,2003 Cornelius Schumacher <schumacher@kde.org>
    Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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

#include <kdebug.h>
#include <klocale.h>

//#include "koglobals.h"
#include "navigatorbar.h"
#include "kdatenavigator.h"

#include <kcalendarsystem.h>

#include "datenavigatorcontainer.h"
#include "koprefs.h"

DateNavigatorContainer::DateNavigatorContainer( QWidget *parent,
                                                const char *name )
  : QWidget( parent, name ), mCalendar( 0 ),
    mHorizontalCount( 1 ), mVerticalCount( 1 )
{
    mResizeEnabled = false;
  mExtraViews.setAutoDelete( true );

  mNavigatorView = new KDateNavigator( this, name );
  mNavigatorView->hide();
  connectNavigatorView( mNavigatorView );
  //setSizePolicy( QSizePolicy (QSizePolicy::Expanding,QSizePolicy::Expanding)   ); 
  mLastDisplayedDN = 0;
  mUpdateTimer = new QTimer( this );
  connect (mUpdateTimer ,SIGNAL(timeout()), this, SLOT ( checkUpdateDayMatrixDates() ));
  mFirstSelectedDate = QDate::currentDate();
  mSelectedDateCount = 1;
}

DateNavigatorContainer::~DateNavigatorContainer()
{
}

void DateNavigatorContainer::connectNavigatorView( KDateNavigator *v )
{
  connect( v, SIGNAL( datesSelected( const KCal::DateList & ) ),
           SIGNAL( datesSelected( const KCal::DateList & ) ) );
#if 0
  connect( v, SIGNAL( incidenceDropped( Incidence *, const QDate & ) ),
           SIGNAL( incidenceDropped( Incidence *, const QDate & ) ) );
  connect( v, SIGNAL( incidenceDroppedMove( Incidence *, const QDate & ) ),
           SIGNAL( incidenceDroppedMove( Incidence *, const QDate & ) ) );
#endif
  connect( v, SIGNAL( weekClicked( const QDate & ) ),
           SIGNAL( weekClicked( const QDate & ) ) );
  connect( v, SIGNAL( showMonth( const QDate & ) ),
           SIGNAL( showMonth( const QDate & ) ) );

  connect( v, SIGNAL( goPrevious() ), SIGNAL( goPrevious() ) );
  connect( v, SIGNAL( goNext() ), SIGNAL( goNext() ) );

  connect( v, SIGNAL( goNextMonth() ), SLOT( slotgoNextMonth() ) );
  connect( v, SIGNAL( goPrevMonth() ), SLOT( slotgoPrevMonth() ) );
  connect( v, SIGNAL( goNextYear() ), SLOT( slotgoNextYear() ) );
  connect( v, SIGNAL( goPrevYear() ), SLOT( slotgoPrevYear() ) );

  connect( v, SIGNAL( monthSelected( int ) ), SLOT( slotMonthSelected( int ) ) );
}
void DateNavigatorContainer::slotgoNextYear()
{
    jumpMonth( 12 );
    emit goNextYear();

}
void DateNavigatorContainer::slotgoPrevYear()
{
    jumpMonth( -12 );
    emit goPrevYear();

}
void DateNavigatorContainer::slotgoPrevMonth()
{
    jumpMonth( -1 );
    emit goPrevMonth();

}
void DateNavigatorContainer::slotgoNextMonth()
{ 
    jumpMonth( 1 );
    emit goNextMonth();
}
void DateNavigatorContainer::jumpMonth( int month )
{

    QDate baseDate = mNavigatorView->baseDate();
    computeMonthSelected( baseDate.month() + month, false );
}
void DateNavigatorContainer::slotMonthSelected( int month )
{
    computeMonthSelected( month, true );
}
void DateNavigatorContainer::computeMonthSelected( int month , bool forceEmit )
{
    //qDebug("slotMonthSelected %d ", month);
    QDate baseDate = mNavigatorView->baseDate();
    if ( baseDate.month() == month )
        return;
    //qDebug("month %d %d  ",baseDate.month(),month);
    QDate date = QDate (  baseDate.year(), baseDate.month() , 15 );
    date = date.addDays( -(baseDate.month()-month ) *30 );
    QDate newBase = QDate (  date.year(), date.month() ,1 ); 
    
    //qDebug("NEW BASE %s", newBase.toString().latin1());
    mNavigatorView->setBaseDate( newBase );
    QDate last = lastAvailableDate();
    QDate first = firstAvailableDate();
    QDate selFirst = mFirstSelectedDate;
    QDate selLast = selFirst.addDays( mSelectedDateCount-1 );
    if (  selFirst >= first && selLast <= last ) {
        setBaseDates();
        if ( forceEmit ) 
            updateDayMatrixDates();
    }
    else {
        setBaseDates();
        if ( forceEmit ) 
            updateDayMatrixDates();
        if ( forceEmit ) 
            emit monthSelected( month );
    }
}
void DateNavigatorContainer::setCalendar( Calendar *cal )
{
  mCalendar = cal;
  mNavigatorView->setCalendar( cal );
  for( int i = 0; i < mLastDisplayedDN; ++i ) {
      KDateNavigator *n = mExtraViews.at( i );
      n->setCalendar( cal );
  }
}
void DateNavigatorContainer::checkUpdateDayMatrixDates()
{ 
    //qDebug("KODNC: wid %d  hei %d ", width(), height());
    mUpdateTimer->stop();
    //return;
    if ( width() < 3 || height() < 3 )
        return;
     static int lastWid = 0;
    static int lastHei = 0;
    if ( lastWid == width() && height() == lastHei ) {
        qDebug("KODNC: No layout computing needed. ");
    } else {
        lastWid = width();
        lastHei = height();

        QSize minSize = mNavigatorView->yourSizeHint();

        int verticalCount = size().height() / minSize.height();
        int horizontalCount = size().width() / minSize.width();
        //qDebug(" wattdatt %d new %d %d ", size().width() ,e->size().width() , minSize.width() );  
        //qDebug("COUNT %d %d %d %d ", verticalCount, horizontalCount , mVerticalCount, mHorizontalCount );
        bool fontchange = false;  
        if ( horizontalCount == 1)
            horizontalCount = size().width() / mNavigatorView->yourFullSizeHint().width();  
        QFont fo;
        if ( horizontalCount != mHorizontalCount ||
             verticalCount != mVerticalCount ) {
            uint count = horizontalCount * verticalCount;
            if ( count == 0 ) {
                bool ok;
                fo = mNavigatorView->yourFontHint( size() , &ok );
                //mNavigatorView->resize( size() );
                //if ( ! ok )
                //  return;
                horizontalCount =  size().width() / mNavigatorView->sizeHintTwoButtons( ).width();
                if (  horizontalCount <= 1 )
                    minSize = mNavigatorView->sizeHintTwoButtons( 4 ); 
                else 
                    minSize = mNavigatorView->sizeHintTwoButtons(); 
                verticalCount = size().height() / minSize.height();
                horizontalCount = size().width() / minSize.width();
                if ( horizontalCount == 0 )
                    horizontalCount = 1;
                if ( verticalCount == 0 )
                    verticalCount = 1;
                fontchange = true;
                count = horizontalCount * verticalCount;
            } else {
                if ( mNavigatorView->fontChanged() ) {
                    fontchange = true;
                    fo = KOPrefs::instance()->mDateNavigatorFont;
                    mNavigatorView->changeFont( fo );
                    mNavigatorView->unsetFontChanged();
                }
            }
            mLastDisplayedDN =  horizontalCount*verticalCount-1;
            while ( count > ( mExtraViews.count() + 1 ) ) {
                KDateNavigator *n = new KDateNavigator( this );
                n->setMonthSignalOffset ( mExtraViews.count()+1 );
                mExtraViews.append( n );
                n->setCalendar( mCalendar );
                connectNavigatorView( n );
                // n->show();
            }
            
            setBaseDates();
            if ( fontchange ) {
                //mNavigatorView->changeFont( fo );
                uint i;
                for( i = 0; i < mExtraViews.count(); ++i ) {
                    KDateNavigator *view = mExtraViews.at( i );
                    view->changeFont( fo );
                }
            }
            mHorizontalCount = horizontalCount;
            mVerticalCount = verticalCount;
    
        }
        if ( mNavigatorView->fontChanged() && ! fontchange  ) {
            qDebug("KDNC: Resetting all fonts ");
            fo = KOPrefs::instance()->mDateNavigatorFont;
            mNavigatorView->changeFont( fo );
            mNavigatorView->unsetFontChanged();
            uint i;
            for( i = 0; i < mExtraViews.count(); ++i ) {
                KDateNavigator *view = mExtraViews.at( i );
                view->changeFont( fo );
            }
        }
        int theight = height() / mVerticalCount;
        int twidth = width() / mHorizontalCount;

        NavigatorBar *bar = mNavigatorView->navigatorBar();
        if ( mHorizontalCount > 1 ) bar->showButtons( true, false );
        else bar->showButtons( true, true );

        mNavigatorView->setGeometry(0,
                                    0, twidth, theight );
        for( int i = 0; i < mLastDisplayedDN; ++i ) {
            int x = ( i + 1 ) % mHorizontalCount;
            int y = ( i + 1 ) / mHorizontalCount;

            KDateNavigator *view = mExtraViews.at( i );
            bar = view->navigatorBar();
            if ( y > 0 ) bar->showButtons( false, false );
            else {
                if ( x + 1 == mHorizontalCount ) bar->showButtons( false, true );
                else bar->showButtons( false, false );
            }
            view->setGeometry( x * twidth,
                               y * theight, twidth, theight );
        }
        uint iii = 0;
        uint ccc = mHorizontalCount * mVerticalCount;
        mNavigatorView->show();
        while ( iii < ( mExtraViews.count()  ) ) {
            if ( iii < ccc-1 )
                mExtraViews.at( iii )->show();
            else
                mExtraViews.at( iii )->hide();
            ++iii;
        }
    }

    QDate last = lastAvailableDate();
    QDate first = firstAvailableDate();

    QDate selFirst = mFirstSelectedDate;
    QDate selLast = selFirst.addDays( mSelectedDateCount-1 );
    if ( selFirst >= first && selLast <= last ) {
        updateDayMatrixDates();
    }
    else {
        updateDayMatrixDates();
        emit monthSelected( mFirstSelectedDate.month() );
    }
}
void DateNavigatorContainer::updateDayMatrixDates()
{ 
    QDate fDate =  mFirstSelectedDate;
    QDate lDate =  fDate.addDays( mSelectedDateCount - 1 );
    mNavigatorView->dayMatrix()->setSelectedDaysFrom( fDate , lDate );
    mNavigatorView->dayMatrix()->repaint( false );
    for( int i = 0; i < mLastDisplayedDN; ++i ) {
        KDateNavigator *n = mExtraViews.at( i );
        if ( n->dayMatrix()->setSelectedDaysFrom( fDate , lDate ) ) {
            n->dayMatrix()->repaint( false );
        }
    }
}

void DateNavigatorContainer::updateDayMatrix()
{
  mNavigatorView->updateDayMatrix();
  for( int i = 0; i < mLastDisplayedDN; ++i ) {
      KDateNavigator *n = mExtraViews.at( i );
      n->updateDayMatrix();
  }
}

void DateNavigatorContainer::updateToday()
{
    qDebug("DateNavigatorContainer::updateToday() NOT IMPL ");
#if 0
  mNavigatorView->updateToday();
  KDateNavigator *n;
  for( n = mExtraViews.first(); n; n = mExtraViews.next() ) {
    n->updateToday();
  }
#endif 
}

void DateNavigatorContainer::updateView()
{
  mNavigatorView->updateView();
  for( int i = 0; i < mLastDisplayedDN; ++i ) {
      KDateNavigator *n = mExtraViews.at( i );
      n->updateView();
  }
}

void DateNavigatorContainer::updateConfig()
{
  mNavigatorView->updateConfig();
  for( int i = 0; i < mLastDisplayedDN; ++i ) {
      KDateNavigator *n = mExtraViews.at( i );
      n->updateConfig();
  }
}
QDate DateNavigatorContainer::lastAvailableDate() const 
{
    QDate date = mNavigatorView->baseDate();
    QDate last = QDate ( date.year(), date.month(), date.daysInMonth() );
    int iii = mLastDisplayedDN;
    if ( mLastDisplayedDN )
        last = last.addDays( 1);
    while ( iii ) {
        last = last.addDays( last.daysInMonth ());
        //qDebug("DATE %s   ", last.toString().latin1() );
        --iii;
    }
    if ( mLastDisplayedDN )
        last = last.addDays( -1);
    return last;
}
QDate DateNavigatorContainer::firstAvailableDate() const 
{
    return QDate ( mNavigatorView->baseDate().year(), mNavigatorView->baseDate().month(), 1 );
}
void DateNavigatorContainer::selectDates( const DateList &dateList )
{
    mFirstSelectedDate = dateList.first() ;
    mSelectedDateCount = dateList.count() ;
    if ( !mLastDisplayedDN ) {
        mNavigatorView->selectDates( dateList );
        return;
    } 
    QDate fDate =  dateList.first();
    QDate lDate =  dateList.last();
    //qDebug("%s %s  ", lastAvailableDate().toString().latin1(), firstAvailableDate().toString().latin1()  );
    //qDebug("End %s %s ",lDate.toString().latin1(),curEnd.toString().latin1()  );
    if ( lDate <= lastAvailableDate() && firstAvailableDate() <=  fDate) {
        updateDayMatrixDates();
        return;
    }
    mNavigatorView->selectDates( dateList );
    setBaseDates();
  if ( mLastDisplayedDN ) {
      KDateNavigator *view = mExtraViews.at( 0 );
      view->dayMatrix()->setSelectedDaysFrom(*(dateList.begin()), *(--dateList.end()));
      view->dayMatrix()->repaint( false ); 
      if ( mLastDisplayedDN > 1 ) {
          KDateNavigator *view = mExtraViews.at( 1 );
          view->dayMatrix()->setSelectedDaysFrom(*(dateList.begin()), *(--dateList.end()));
          view->dayMatrix()->repaint( false );
      }
  }
}

void DateNavigatorContainer::setBaseDates()
{
  QDate baseDate = mNavigatorView->baseDate();
  bool doRepaint = true;
  for( int i = 0; i < mLastDisplayedDN; ++i ) {
      KDateNavigator *n = mExtraViews.at( i );
      baseDate = baseDate.addDays( baseDate.daysInMonth () - baseDate.day() +1 );
      n->setBaseDate( baseDate, doRepaint );
  }
}
void DateNavigatorContainer::setResizeEnabled()
{
    mResizeEnabled = true;
    //qDebug("DateNavigatorContainer::setResizeEnabled ");
    checkUpdateDayMatrixDates();
}
void DateNavigatorContainer::resizeEvent( QResizeEvent *  )
{

    //qDebug("DateNavigatorContainer::resizeEvent %d %d ", width(), height());
  //qDebug("COUNT %d ", mExtraViews.count());
    if ( ! mResizeEnabled  ) {
        //qDebug("NOT ResizeEnabled");
        return;
    }
    else
#ifdef DESKTOP_VERSION
        mUpdateTimer->start( 100 );
#else
        mUpdateTimer->start( 50 );
#endif
        
  //updateDayMatrixDates();
}

QSize DateNavigatorContainer::minimumSizeHint() const
{
  return mNavigatorView->minimumSizeHint();
}

QSize DateNavigatorContainer::sizeHint() const
{
    return mNavigatorView->yourSizeHint();
}
