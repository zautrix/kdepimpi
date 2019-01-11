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

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#ifndef KORG_NOSPLITTER
#include <qsplitter.h>
#endif
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpopupmenu.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include <kapplication.h>
#include <KDGanttMinimizeSplitter.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include "calendarview.h"
#include "koviewmanager.h"

#include <libkcal/calendar.h>
#include <libkcal/icaldrag.h>
#include <libkcal/dndfactory.h>

#include <kcalendarsystem.h>

#include "koglobals.h"
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif
#include "koprefs.h"
#include "koagenda.h"
#include "koagendaitem.h"
#ifndef KORG_NOPRINTER
#include "calprinter.h"
#endif

#include "koagendaview.h"
//#include "koagendaview.moc"

//extern bool globalFlagBlockPainting;
extern int globalFlagBlockAgenda;
extern int globalFlagBlockStartup;
extern int globalFlagBlockAgendaItemPaint;
extern int globalFlagBlockAgendaItemUpdate;
extern int globalFlagBlockLabel;
using namespace KOrg;

#define IDLETIMEOUT 45

TimeLabels::TimeLabels(int rows,QWidget *parent,const char *name,WFlags f) :
    QScrollView(parent,name,f)
{
    myPix.resize( 1, 1 );
    mRows = rows;

    mRedrawNeeded = true;
    setMinimumHeight( 20 );
    mCellHeight = KOPrefs::instance()->mHourSize*4;

    enableClipper(true);

    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);

    resizeContents(50,mRows * mCellHeight);

    viewport()->setBackgroundMode( PaletteBackground );
}

void TimeLabels::setCellHeight(int height)
{
    mCellHeight = height;
}

/*
  Optimization so that only the "dirty" portion of the scroll view
  is redrawn.  Unfortunately, this is not called by default paintEvent() method.
*/
void TimeLabels::drawContents(QPainter *p,int cx, int cy, int cw, int ch)
{

    cx = contentsX() + frameWidth()*2;
    cw = contentsWidth() ;
    // end of workaround

    int cell = ((int)(cy/mCellHeight));
    int y = cell * mCellHeight;
    QFontMetrics fm = fontMetrics();
    QString hour;
    QString suffix = "am";
    int timeHeight =  fm.ascent();
    QFont nFont = font();
    p->setFont( font());
   
    if (!KGlobal::locale()->use12Clock()) {
            suffix = "00";
    } else
        if (cell > 11) suffix = "pm";
       
    if ( timeHeight >  mCellHeight ) {
        timeHeight = mCellHeight-1;
        int pointS = nFont.pointSize();
        while ( pointS > 4 ) {
            nFont.setPointSize( pointS );
            fm = QFontMetrics( nFont );
            if ( fm.ascent() < mCellHeight )
                break;
            -- pointS;
        }
        fm = QFontMetrics( nFont );
        timeHeight = fm.ascent();
    }
    //timeHeight -= (timeHeight/4-2); 
    QFont sFont = nFont;
    sFont.setPointSize( sFont.pointSize()/2 );
    QFontMetrics fmS(  sFont );
    int sHei = fmS.ascent() ;
    //sHei -= (sHei/4-2); 
    int startW = mMiniWidth - frameWidth()-2 ;
    int tw2 = fmS.width(suffix);
    timeHeight = (timeHeight-1) /2 -1; 
    //testline
    //p->drawLine(0,0,0,contentsHeight());
    while (y < cy + ch+mCellHeight) {
        p->drawLine(startW-tw2+1 ,y,cw+2,y);
        hour.setNum(cell);
        // handle 24h and am/pm time formats
        if (KGlobal::locale()->use12Clock()) {
            if (cell == 12) suffix = "pm";
            if (cell == 0) hour.setNum(12);
            if (cell > 12) hour.setNum(cell - 12);
        }

        // center and draw the time label
        int timeWidth = fm.width(hour);
        int offset = startW - timeWidth - tw2 -1 ;
        p->setFont( nFont );
        p->drawText(  offset, y+ timeHeight, hour);
        p->setFont( sFont ); 
        offset = startW - tw2;
        p->drawText( offset, y -1, suffix);

        // increment indices
        y += mCellHeight;
        cell++;
    }




}

/**
   Calculates the minimum width.
*/
int TimeLabels::minimumWidth() const
{
    return mMiniWidth;
}

/** updates widget's internal state */
void TimeLabels::updateConfig()
{
    mRedrawNeeded = true;
    // set the font
    //  config->setGroup("Fonts");
    //  QFont font = config->readFontEntry("TimeBar Font");
    setFont(KOPrefs::instance()->mTimeBarFont);
    QString test = "20";
    if (KGlobal::locale()->use12Clock())
        test = "12";
    mMiniWidth = fontMetrics().width(test);
    if (KGlobal::locale()->use12Clock())
        test = "pm";
    else {
        test = "00";
    }
    QFont sFont = font();
    sFont.setPointSize( sFont.pointSize()/2 );
    QFontMetrics fmS(  sFont );
    mMiniWidth += fmS.width( test ) + frameWidth()*2+4 ;
    // update geometry restrictions based on new settings
    setFixedWidth( mMiniWidth );
  
    // update HourSize
    mCellHeight = KOPrefs::instance()->mHourSize*4;
    resizeContents(mMiniWidth,mRows * mCellHeight+1);
}

/** update time label positions */
void TimeLabels::positionChanged()
{
    int adjustment = mAgenda->contentsY();
    setContentsPos(0, adjustment);
}

/**  */
void TimeLabels::setAgenda(KOAgenda* agenda)
{
    mAgenda = agenda;
}
 
void TimeLabels::contentsMousePressEvent ( QMouseEvent * e) 
{
    mMouseDownY = e->pos().y();
    mOrgCap = topLevelWidget()->caption();
}
  
void TimeLabels::contentsMouseMoveEvent ( QMouseEvent * e ) 
{
  int diff = mMouseDownY -  e->pos().y();
    if ( diff < 10 && diff > -10 )
        return;
    int tSize = KOPrefs::instance()->mHourSize + (diff/10) ;
    if ( tSize  < 4 )
        tSize = 4;
    if ( tSize > 22 )
        tSize = 22;
    tSize = (tSize-2)/2;
    topLevelWidget()->setCaption(i18n("New Agendasize: %1").arg(tSize));

}
void TimeLabels::contentsMouseReleaseEvent ( QMouseEvent * e ) 
{
    topLevelWidget()->setCaption( mOrgCap );
    int diff = mMouseDownY -  e->pos().y();
    if ( diff < 10 && diff > -10 )
        return;
    int tSize = KOPrefs::instance()->mHourSize + (diff/10);
    if ( tSize  < 4 )
        tSize = 4;
    if ( tSize > 22 )
        tSize = 22;
    tSize = (tSize/2)*2;
    if ( tSize == KOPrefs::instance()->mHourSize )
        return;
    KOPrefs::instance()->mHourSize = tSize;
    emit scaleChanged();
}

/** This is called in response to repaint() */
void TimeLabels::paintEvent(QPaintEvent*)
{
  
    //  kdDebug() << "paintevent..." << endl;
    // this is another hack!
    //  QPainter painter(this);
    //QString c
    repaintContents(contentsX(), contentsY(), visibleWidth(), visibleHeight());
}

////////////////////////////////////////////////////////////////////////////

EventIndicator::EventIndicator(Location loc,QWidget *parent,const char *name)
    : QFrame(parent,name)
{
    mColumns = 1;
    mTopBox = 0;
    mLocation = loc;
    mTopLayout = 0;
    mPaintWidget = 0;
    mXOffset = 0;
    if (mLocation == Top) mPixmap = SmallIcon("1uparrow");
    else mPixmap = SmallIcon("1downarrow");
    mEnabled.resize(mColumns);
    mEnabled.fill( false );
    setMinimumHeight(mPixmap.height());
}

EventIndicator::~EventIndicator()
{
}

void EventIndicator::drawContents(QPainter *p)
{
 
    //  kdDebug() << "======== top: " << contentsRect().top() << "  bottom " << //         contentsRect().bottom() << "  left " << contentsRect().left() << "  right " << contentsRect().right() << endl;
    KDGanttSplitterHandle* han = 0;
    if ( mPaintWidget )
        han = mPaintWidget->firstHandle();
    if ( ! han ) {
        int i;
        for(i=0;i<mColumns;++i) {
            if (mEnabled[i]) {
                int cellWidth = contentsRect().right()/mColumns;
                int xOffset = KOGlobals::self()->reverseLayout() ? 
                    (mColumns - 1 - i)*cellWidth + (cellWidth -mPixmap.width())/2 :
                    i*cellWidth + (cellWidth -mPixmap.width()) /2;
                p->drawPixmap(QPoint(1+xOffset,0),mPixmap);
            }
        }
    } else {
        han->repaint();
        //mPaintWidget->setBackgroundColor( red );
      
        QPainter pa( han );
        int i;
        bool setColor = false;
        for(i=0;i<mColumns;++i) {
            if (mEnabled[i]) {
                setColor = true;
                
                int cellWidth = contentsRect().right()/mColumns;
                int xOffset = KOGlobals::self()->reverseLayout() ? 
                    (mColumns - 1 - i)*cellWidth + cellWidth/2 -mPixmap.width()/2 :
                    i*cellWidth + cellWidth/2 -mPixmap.width()/2;
                pa.drawPixmap(QPoint(mXOffset + xOffset,0),mPixmap);
                //qDebug("222draw pix %d ",xOffset );
                
            }

        }
        pa.end();
       
    }
}

void EventIndicator::setXOffset( int x )
{
    mXOffset = x;
}
void  EventIndicator::setPaintWidget( KDGanttMinimizeSplitter * w )
{
    mPaintWidget = w;
    setMaximumHeight(0);
    setMinimumHeight(0);
}
void EventIndicator::changeColumns(int columns)
{
    mColumns = columns;
    mEnabled.resize(mColumns);

    update();
}

void EventIndicator::enableColumn(int column, bool enable)
{
    mEnabled[column] = enable;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

KOAgendaView::KOAgendaView(Calendar *cal,QWidget *parent,const char *name) :
    KOEventView (cal,parent,name)
{
    flag_blockfillAgenda = false;
    mBlockUpdating = true;
    mStartHour = 8;
    mSelectedDates.append(QDate::currentDate());

    mLayoutDayLabels = 0;
    mDayLabelsFrame = 0;
    mDayLabels = 0;
    bool isRTL = KOGlobals::self()->reverseLayout();
    QPixmap expandPix;
    if ( KOPrefs::instance()->mVerticalScreen ) {
         expandPix = SmallIcon( "1updownarrow" );
    } else {
        expandPix = SmallIcon("1leftrightarrow" );
    }

    QBoxLayout *topLayout = new QVBoxLayout(this);

    // Create day name labels for agenda columns
    // Create agenda splitter

    mSplitterAgenda = new KDGanttMinimizeSplitter( Qt::Vertical, this);
    mSplitterAgenda->setMinimizeDirection ( KDGanttMinimizeSplitter::Up );
    topLayout->addWidget( mSplitterAgenda );
    mAllDayFrame = new QHBox(mSplitterAgenda);
    mAllDayFrame->setFocusPolicy(NoFocus);
    QWidget *agendaFrame = new QWidget(mSplitterAgenda);
    agendaFrame->setFocusPolicy(NoFocus);
  
    // Create all-day agenda widget
    mDummyAllDayLeft = new QVBox( mAllDayFrame );

    mExpandButton = new QPushButton(mDummyAllDayLeft);
    mExpandButton->setPixmap( expandPix );
    int widebut = mExpandButton->sizeHint().width()+4;
    int heibut = mExpandButton->sizeHint().height()+4;
    if ( heibut > widebut  )
        widebut = heibut ;
   
    //mExpandButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,
    //                                         QSizePolicy::Fixed ) );
    mExpandButton->setFixedSize( widebut, widebut);
    connect( mExpandButton, SIGNAL( clicked() ), SIGNAL( toggleExpand() ) );
    mExpandButton->setFocusPolicy(NoFocus);
    mAllDayAgenda = new KOAgenda(1,mAllDayFrame);
    mAllDayAgenda->setFocusPolicy(NoFocus);
    QLabel *dummyAllDayRight = new QLabel (mAllDayFrame);

    // Create event context menu for all day agenda
    //mAllDayAgendaPopup = eventPopup();

    // Create agenda frame
    QGridLayout *agendaLayout = new QGridLayout(agendaFrame,4,3);
    //  QHBox *agendaFrame = new QHBox(splitterAgenda);

    // create event indicator bars
    mEventIndicatorTop = new EventIndicator(EventIndicator::Top,agendaFrame);
#ifndef DESKTOP_VERSION
    mEventIndicatorTop->setPaintWidget( mSplitterAgenda );
#endif
    mDayLabelsFrame = new QHBox(agendaFrame);
    //topLayout->addWidget(mDayLabelsFrame);
    mDayLabels = new QFrame (mDayLabelsFrame);
    mLayoutDayLabels = new QHBoxLayout(mDayLabels);
    agendaLayout->addMultiCellWidget(mDayLabelsFrame ,0,0,0,2);
    agendaLayout->addWidget(mEventIndicatorTop,1,1);

    mEventIndicatorBottom = new EventIndicator(EventIndicator::Bottom,
                                               agendaFrame);
    agendaLayout->addWidget(mEventIndicatorBottom,3,1);
    QWidget *dummyAgendaRight = new QWidget(agendaFrame);
    agendaLayout->addWidget(dummyAgendaRight,1,2);

    // Create time labels
    mTimeLabels = new TimeLabels(24,agendaFrame);
    agendaLayout->addWidget(mTimeLabels,2,0);
    connect(mTimeLabels,SIGNAL( scaleChanged()),
            this,SLOT(updateConfig()));

    // Create agenda
    mAgenda = new KOAgenda(1,96,KOPrefs::instance()->mHourSize,agendaFrame);
    agendaLayout->addMultiCellWidget(mAgenda,2,2,1,2);
    agendaLayout->setColStretch(1,1);
    mAgenda->setFocusPolicy(NoFocus);
    // Create event context menu for agenda
    mAllAgendaPopup = eventPopup();
  
#if 0
    mAllAgendaPopup->addAdditionalItem(QIconSet(SmallIcon("bell")),
                                    i18n("Toggle Alarm"),mAgenda,
                                    SLOT(popupAlarm()),true);

#endif
    connect(mAgenda,SIGNAL(showIncidencePopupSignal(Incidence *)),
            mAllAgendaPopup,SLOT(showIncidencePopup(Incidence *)));
  
    connect(mAllDayAgenda,SIGNAL(showIncidencePopupSignal(Incidence *)),
            mAllAgendaPopup,SLOT(showIncidencePopup(Incidence *)));

    connect(mAllAgendaPopup,SIGNAL(categoryChanged(Incidence *)),
            this,SLOT(categoryChanged(Incidence *)));


    mAgenda->setPopup( mAllAgendaPopup );
    mAllDayAgenda->setPopup( mAllAgendaPopup );
    // make connections between dependent widgets
    mTimeLabels->setAgenda(mAgenda);

    // Update widgets to reflect user preferences
    //  updateConfig();

    // createDayLabels();

    // these blank widgets make the All Day Event box line up with the agenda
    dummyAllDayRight->setFixedWidth(mAgenda->verticalScrollBar()->width());
    dummyAgendaRight->setFixedWidth(mAgenda->verticalScrollBar()->width());
    mDummyAllDayLeft->setFixedWidth(mTimeLabels->width());

    // Scrolling
    connect(mAgenda->verticalScrollBar(),SIGNAL(valueChanged(int)),
            mTimeLabels, SLOT(positionChanged()));
    connect(mTimeLabels->verticalScrollBar(),SIGNAL(valueChanged(int)),
            SLOT(setContentsPos(int)));

    connect(mAgenda,SIGNAL(showDateView( int, int)),SLOT(slotShowDateView( int, int )));
    connect(mAllDayAgenda,SIGNAL(showDateView( int, int )), SLOT(slotShowDateView( int, int ) ));

    // Create/Show/Edit/Delete Event
    connect(mAgenda,SIGNAL(newEventSignal(int,int)),
            SLOT(newEvent(int,int)));
    connect(mAgenda,SIGNAL(newTodoSignal(int,int)),
            SLOT(newTodo(int,int)));
    connect(mAgenda,SIGNAL(newEventSignal(int,int,int,int)),
            SLOT(newEvent(int,int,int,int)));
    connect(mAllDayAgenda,SIGNAL(newEventSignal(int,int)),
            SLOT(newEventAllDay(int,int)));
    connect(mAllDayAgenda,SIGNAL(newTodoSignal(int,int)),
            SLOT(newTodoAllDay(int,int)));
    connect(mAllDayAgenda,SIGNAL(newEventSignal(int,int,int,int)),
            SLOT(newEventAllDay(int,int)));
    connect(mAgenda,SIGNAL(newTimeSpanSignal(int,int,int,int)),
            SLOT(newTimeSpanSelected(int,int,int,int)));
    connect(mAllDayAgenda,SIGNAL(newTimeSpanSignal(int,int,int,int)),
            SLOT(newTimeSpanSelectedAllDay(int,int,int,int)));
    connect(mAgenda,SIGNAL(newStartSelectSignal()),SLOT(updateView()));
    connect(mAllDayAgenda,SIGNAL(newStartSelectSignal()),SLOT(updateView()));
  
    connect(mAgenda,SIGNAL(editIncidenceSignal(Incidence *)),
            SIGNAL(editIncidenceSignal(Incidence *)));
    connect(mAllDayAgenda,SIGNAL(editIncidenceSignal(Incidence *)),
            SIGNAL(editIncidenceSignal(Incidence *)));
    connect(mAgenda,SIGNAL(showIncidenceSignal(Incidence *)),
            SIGNAL(showIncidenceSignal(Incidence *)));
    connect(mAllDayAgenda,SIGNAL(showIncidenceSignal(Incidence *)),
            SIGNAL(showIncidenceSignal(Incidence *)));
    connect(mAgenda,SIGNAL(deleteIncidenceSignal(Incidence *)),
            SIGNAL(deleteIncidenceSignal(Incidence *)));
    connect(mAllDayAgenda,SIGNAL(deleteIncidenceSignal(Incidence *)),
            SIGNAL(deleteIncidenceSignal(Incidence *)));

    connect(mAgenda,SIGNAL(itemModified(KOAgendaItem *, int )),
            SLOT(updateEventDates(KOAgendaItem *, int )));
    connect(mAllDayAgenda,SIGNAL(itemModified(KOAgendaItem *, int )),
            SLOT(updateEventDates(KOAgendaItem *, int)));

    // event indicator update
    connect(mAgenda,SIGNAL(lowerYChanged(int)),
            SLOT(updateEventIndicatorTop(int)));
    connect(mAgenda,SIGNAL(upperYChanged(int)),
            SLOT(updateEventIndicatorBottom(int)));
    // drag signals
    /*
      connect(mAgenda,SIGNAL(startDragSignal(Event *)),
      SLOT(startDrag(Event *)));
      connect(mAllDayAgenda,SIGNAL(startDragSignal(Event *)),
      SLOT(startDrag(Event *)));
    */
    // synchronize selections
    connect( mAgenda, SIGNAL( incidenceSelected( Incidence * ) ),
             mAllDayAgenda, SLOT( deselectItem() ) );
    connect( mAllDayAgenda, SIGNAL( incidenceSelected( Incidence * ) ),
             mAgenda, SLOT( deselectItem() ) );
    connect( mAgenda, SIGNAL( incidenceSelected( Incidence * ) ),
             SIGNAL( incidenceSelected( Incidence * ) ) );
    connect( mAllDayAgenda, SIGNAL( incidenceSelected( Incidence * ) ),
             SIGNAL( incidenceSelected( Incidence * ) ) );
    connect( mAgenda, SIGNAL( resizedSignal() ),
             SLOT( updateConfig( ) ) );  
    connect( mAgenda, SIGNAL( addToCalSignal(Incidence *, Incidence *) ),
             SLOT( addToCalSlot(Incidence *, Incidence * ) ) );  
    connect( mAllDayAgenda, SIGNAL( addToCalSignal(Incidence * ,Incidence *) ),
             SLOT( addToCalSlot(Incidence * , Incidence *) ) );  
    // connect( mAgenda, SIGNAL( cloneIncidenceSignal(Incidence *) ), SIGNAL( cloneIncidenceSignal(Incidence *) ) );
    //connect( mAllDayAgenda, SIGNAL( cloneIncidenceSignal(Incidence *) ), SIGNAL( cloneIncidenceSignal(Incidence *) ) );

    connect( mAllDayAgenda, SIGNAL( signalClearSelection() ),mAgenda, SLOT( slotClearSelection()) ); 
    connect( mAgenda, SIGNAL( signalClearSelection() ),mAllDayAgenda, SLOT( slotClearSelection()) );   
#ifndef DESKTOP_VERSION
    connect( mAllDayAgenda, SIGNAL( updateViewSignal() ),this, SLOT( fillAgenda()) ); 
    connect( mAgenda, SIGNAL( updateViewSignal() ), this, SLOT( fillAgenda()) );   
    connect( mAllDayAgenda, SIGNAL( sendPing() ),this, SLOT( startIdleTimeout()) ); 
    connect( mAgenda, SIGNAL( sendPing() ), this, SLOT( startIdleTimeout()) );   
    mIdleTimer = new QTimer ( this );;
    connect(mIdleTimer,SIGNAL(timeout()),SLOT(slotIdleTimeout()));
#endif
}
void KOAgendaView::startIdleTimeout()
{
#ifndef DESKTOP_VERSION
    mIdleStart = QDateTime::currentDateTime();
    mIdleTimer->start( IDLETIMEOUT * 1000 );
#endif
}
void KOAgendaView::slotIdleTimeout()
{
#ifndef DESKTOP_VERSION
    //qDebug("SECS TO %d ",mIdleStart.secsTo( QDateTime::currentDateTime()  ) );
    int secsfromstart = mIdleStart.secsTo( QDateTime::currentDateTime()  );
    mIdleTimer->stop();
    bool isActice = topLevelWidget()->isActiveWindow();
    //qDebug("KO: Active Window %d %d",  isActice, isVisible());
    // we do nothing if we wake up from a suspend
    if ( secsfromstart > IDLETIMEOUT + 30 && isActice ) {
        qDebug("KO: Wakeup from suspend ");
        startIdleTimeout();
        return;
    }
    qDebug("KO: Downsizing Pixmaps %s",QDateTime::currentDateTime().toString().latin1());
    mAgenda->shrinkPixmap();
    mAllDayAgenda->shrinkPixmap();
    KOAgendaItem::paintPix()->resize( 20,20);
    KOAgendaItem::paintPixAllday()->resize( 20,20);
#endif
    
}
void KOAgendaView::toggleAllDay()
{
    if ( mSplitterAgenda->firstHandle() ) 
        mSplitterAgenda->firstHandle()->toggle();
}
void KOAgendaView::addToCalSlot(Incidence * inc, Incidence * incOld )
{
    calendar()->addIncidence( inc ); 
    
    if ( incOld ) {
        if ( incOld->typeID() == todoID )
            emit  todoMoved((Todo*)incOld,  KOGlobals::EVENTEDITED );
        else
            emit incidenceChanged(incOld, KOGlobals::EVENTEDITED);
    }
    
}
void KOAgendaView::categoryChanged(Incidence * inc)
{
    mAgenda->categoryChanged( inc );
    mAllDayAgenda->categoryChanged( inc );
}
KOAgendaView::~KOAgendaView()
{
    delete mAllAgendaPopup;
    //delete mAllDayAgendaPopup;
    delete KOAgendaItem::paintPix();
    delete KOAgendaItem::paintPixAllday();
}
void KOAgendaView::resizeEvent( QResizeEvent* e )
{
    //qDebug("KOAgendaView::resizeEvent( QResizeEvent* e ) %d ", e->size().width());
    bool uc = false;
    int ow =  e->oldSize().width();
    int oh =  e->oldSize().height();
    int w = e->size().width();
    int h =  e->size().height();
    if ( (ow > oh && w< h ) || (ow < oh && w > h ) ) { 
        if ( ! mBlockUpdating  && !globalFlagBlockStartup && !globalFlagBlockAgenda )
            uc = true;
        //qDebug("view changed %d %d %d %d ", ow, oh , w , h);
    }
    mUpcomingWidth = e->size().width() ;
    if ( mBlockUpdating || uc ) {
        mBlockUpdating = false;
        //mAgenda->setMinimumSize(800 , 600 );
        //qDebug("mAgenda->resize+++++++++++++++ ");
        updateConfig();
        //qDebug("KOAgendaView::Updating now possible ");
    } else 
        createDayLabels();
    //qDebug("resizeEvent end "); 
   
}
void KOAgendaView::slotDaylabelClicked( int num )
{
  
    QDate firstDate = mSelectedDates.first();
    if ( num == -1 )
        emit showDateView( 6, firstDate );
    else if (num >= 0 ) {
        if ( mSelectedDates.count() == 1)
            emit showDateView( 9, firstDate.addDays( num ) );
        else
            emit showDateView( 3, firstDate.addDays( num ) );
    }
    else
        showDateView( 10, firstDate.addDays(1) );
}

KOAgendaButton* KOAgendaView::getNewDaylabel()
{

    KOAgendaButton * dayLabel = new KOAgendaButton(mDayLabels);
    connect( dayLabel, SIGNAL( numClicked(int) ), this, SLOT ( slotDaylabelClicked(int) ) );
    mDayLabelsList.append( dayLabel );
    mLayoutDayLabels->addWidget(dayLabel);
    return dayLabel ;
}

void KOAgendaView::createDayLabels()
{

    if ( mBlockUpdating || globalFlagBlockLabel == 1) {
        // qDebug(" KOAgendaView::createDayLabels() blocked ");
        return;

    }
    int newHight;
    if ( !mSelectedDates.count())
        return;

    // ### Before deleting and recreating we could check if mSelectedDates changed...
    // It would remove some flickering and gain speed (since this is called by
    // each updateView() call)
    
    int maxWid = mUpcomingWidth - mTimeLabels->width()- mAgenda->verticalScrollBar()->width() - mAgenda->frameWidth()*2;
    mDayLabelsFrame->setMaximumWidth( mUpcomingWidth );
    if ( maxWid < 20 )
        maxWid = 20;

    QFont dlf = KOPrefs::instance()->mTimeLabelsFont;
    QFontMetrics fm ( dlf );
    dlf.setBold( true );
    int selCount = mSelectedDates.count();
    int widModulo = maxWid - (mAgenda->gridSpacingX() * selCount)+1;
    if ( widModulo < 0 || widModulo > selCount )
        widModulo = 0;
    QString dayTest = "Mon 20";
    //QString dayTest = "Mon 20";
    int wid = fm.width( dayTest );
    //maxWid  -= ( selCount * 3 );  //working for QLabels
    if ( QApplication::desktop()->width() <= 320 )
        maxWid  -= ( selCount * 3 );  //working for QPushButton
    else
        maxWid  -= ( selCount * 4 );  //working for QPushButton
    if ( maxWid < 0 )
        maxWid = 20;
    int needWid = wid * selCount;
    //qDebug("++++++++Needed : %d  MaxWidth: %d", needWid, maxWid );
    //if ( needWid > maxWid ) 
    //  qDebug("DAYLABELS TOOOOOOO BIG ");
    while ( needWid > maxWid   ) {
        dayTest = dayTest.left( dayTest.length() - 1 );
        wid = fm.width( dayTest );
        needWid = wid * selCount;
    }
    int maxLen =  dayTest.length();
    int fontPoint = dlf.pointSize();
    if (  maxLen < 2 ) {
        int fontPoint = dlf.pointSize();
        while ( fontPoint > 4 ) {
            --fontPoint;
            dlf.setPointSize( fontPoint );
            QFontMetrics f( dlf ); 
            wid = f.width( "30" );
            needWid = wid * selCount;
            if ( needWid < maxWid )
                break;
        }
        maxLen = 2;
    }
    //qDebug("Max len %d ", dayTest.length() );
    if ( !KOPrefs::instance()->mTimeLabelsFont.bold() )
        dlf.setBold( false );
    QFontMetrics tempF( dlf );
    newHight = tempF.height();
    mDayLabels->setFont( dlf );
    // mLayoutDayLabels = new QHBoxLayout(mDayLabels);;
    // mLayoutDayLabels->addSpacing(mTimeLabels->width());
    //mLayoutDayLabels->addSpacing( 2 );
    // QFont lFont = dlf;
    bool appendLabels = false;
    KOAgendaButton *dayLabel;
    dayLabel = mDayLabelsList.first();
    if ( !dayLabel  ) {
        appendLabels = true;
        dayLabel = getNewDaylabel();
    }
    dayLabel->setFixedWidth( mTimeLabels->width()+mAgenda->frameWidth() ); 
    dayLabel->setFont( dlf );
    dayLabel->setNum( -1 );
    //dayLabel->setAlignment(QLabel::AlignHCenter);

    dayLabel->setText( KOGlobals::self()->calendarSystem()->monthName( mSelectedDates.first(), true ) );
    dayLabel->show();
    DateList::ConstIterator dit;
    bool oneday = (mSelectedDates.first() == mSelectedDates.last() );
    int counter = -1;
    for( dit = mSelectedDates.begin(); dit != mSelectedDates.end(); ++dit ) {
        ++counter;
        QDate date = *dit;
        // QBoxLayout *dayLayout = new QVBoxLayout(mLayoutDayLabels);
        if (  ! appendLabels ) {
            dayLabel = mDayLabelsList.next();
            if (  !dayLabel  )
                appendLabels = true;
        }
        if ( appendLabels ) {
            dayLabel = getNewDaylabel();
        }
        dayLabel->setMinimumWidth( 1 );
        dayLabel->setMaximumWidth( 10240 );
        dayLabel->setFont( dlf );
        dayLabel->show();
        dayLabel->setAutoRepeat( false );
        dayLabel->setNum( counter );
        QString str;
        int dW = KOGlobals::self()->calendarSystem()->dayOfWeek(date);
        QString dayName = KOGlobals::self()->calendarSystem()->weekDayName( dW, true );
        switch ( maxLen ) {
        case 2:
            str = QString::number( date.day() );
            break;

        case 3:
            str =  dayName.left( 1 )  +QString::number( date.day());

            break;
        case 4:
            str =  dayName.left( 1 ) + " " +QString::number( date.day());

            break;
        case 5:
            str =  dayName.left( 2 ) + " " +QString::number( date.day());

            break;
        case 6:
            str =  dayName.left( 3 ) + " " +QString::number( date.day());
            break;

        default:
            break;
        }
        if ( oneday ) {
            QString addString;
            if ( mSelectedDates.first() == QDateTime::currentDateTime().date() )
                addString = i18n("Today");
            else if ( mSelectedDates.first() == QDateTime::currentDateTime().date().addDays(1) )
                addString = i18n("Tomorrow");
            else if ( mSelectedDates.first() == QDateTime::currentDateTime().date().addDays(-1) )
                addString = i18n("Yesterday");
            else if ( mSelectedDates.first() == QDateTime::currentDateTime().date().addDays(-2) )
                addString = i18n("Day before yesterday");
            else if ( mSelectedDates.first() == QDateTime::currentDateTime().date().addDays(2) )
                addString = i18n("Day after tomorrow");
            if ( !addString.isEmpty() ) {
                if (  QApplication::desktop()->width() < 640 )
                    str = addString+", " + str;
                else
                    str = addString+", "+ KGlobal::locale()->formatDate( date, false);
            } else {
                str = KGlobal::locale()->formatDate( date, KOPrefs::instance()->mShortDateInViewer);
            }
        }
        dayLabel->setText(str);
        //dayLabel->setAlignment(QLabel::AlignHCenter);
        if (date == QDate::currentDate()) {
            QFont bFont = dlf;
            bFont.setBold( true );
            dayLabel->setFont(bFont);
        }
        //dayLayout->addWidget(dayLabel);

#ifndef KORG_NOPLUGINS
        CalendarDecoration::List cds = KOCore::self()->calendarDecorations();
        CalendarDecoration *it;
        for(it = cds.first(); it; it = cds.next()) {
            QString text = it->shortText( date );
            if ( !text.isEmpty() ) {
                QLabel *label = new QLabel(text,mDayLabels);
                label->setAlignment(AlignCenter);
                dayLayout->addWidget(label);
            }
        }

        for(it = cds.first(); it; it = cds.next()) {
            QWidget *wid = it->smallWidget(mDayLabels,date);
            if ( wid ) {
                //      wid->setHeight(20);
                dayLayout->addWidget(wid);
            }
        }
#endif
    }
    if (  ! appendLabels ) {
        dayLabel = mDayLabelsList.next();
        if (  !dayLabel  )
            appendLabels = true;
    }
    if ( appendLabels ) {
        dayLabel = getNewDaylabel();
    }
    //dayLabel->hide();//test only
   
    dayLabel->setText(">");
    dayLabel->setFont( dlf );
    dayLabel->setAutoRepeat( true );
    dayLabel->show();
    dayLabel->setNum( -2 );
    dayLabel->setFixedWidth( mAgenda->verticalScrollBar()->width()+ widModulo  );

    //mLayoutDayLabels->addSpacing(mAgenda->verticalScrollBar()->width()+ offset+2);
    if ( !appendLabels ) {
        dayLabel = mDayLabelsList.next();
        while (  dayLabel  ) {
            //qDebug("!dayLabel %d",dayLabel );
            dayLabel->hide();
            dayLabel = mDayLabelsList.next();
        }
    }
   
    mDayLabelsFrame->setFixedHeight( newHight + 4 );
}

int KOAgendaView::maxDatesHint()
{
    // Not sure about the max number of events, so return 0 for now.
    return 0;
}

int KOAgendaView::currentDateCount()
{
    return mSelectedDates.count();
}

QPtrList<Incidence> KOAgendaView::selectedIncidences()
{
    QPtrList<Incidence> selected;
    Incidence *incidence;

    incidence = mAgenda->selectedIncidence();
    if (incidence) selected.append(incidence);

    incidence = mAllDayAgenda->selectedIncidence();
    if (incidence) selected.append(incidence);

    return selected;
}

DateList KOAgendaView::selectedDates()
{
    DateList selected;
    QDate qd;

    qd = mAgenda->selectedIncidenceDate();
    if (qd.isValid()) selected.append(qd);

    qd = mAllDayAgenda->selectedIncidenceDate();
    if (qd.isValid()) selected.append(qd);

    return selected;
}


void KOAgendaView::updateView()
{
    if ( mBlockUpdating )
        return;
    //  kdDebug() << "KOAgendaView::updateView()" << endl;
    fillAgenda();
 
}


/*
  Update configuration settings for the agenda view. This method is not
  complete.
*/
void KOAgendaView::updateConfig()
{
    if ( mBlockUpdating )
        return;
    if ( mAgenda->height() > 96 * KOPrefs::instance()->mHourSize ) {
        int old = KOPrefs::instance()->mHourSize;
        KOPrefs::instance()->mHourSize = mAgenda->height()/96 +1;
        //qDebug("KOPrefs::instance()->mHourSize adjusted %d to  %d ", old,KOPrefs::instance()->mHourSize );
    }


    // update config for children
    mTimeLabels->updateConfig();
    mAgenda->storePosition();
    mAgenda->updateConfig();
    mAllDayAgenda->updateConfig();
    // widget synchronization
    //TODO: find a better way, maybe signal/slot
    mTimeLabels->positionChanged();

    // for some reason, this needs to be called explicitly
    mTimeLabels->repaint();

    mDummyAllDayLeft->setFixedWidth(mTimeLabels->width());

    // ToolTips displaying summary of events
    KOAgendaItem::toolTipGroup()->setEnabled(KOPrefs::instance()
                                             ->mEnableToolTips);
  
    //setHolidayMasks();

    //createDayLabels(); called by via updateView();
    mEventIndicatorTop->setXOffset(mTimeLabels->width() + mAgenda->frameWidth()); 
    updateView();
    mAgenda->restorePosition();
}


void KOAgendaView::updateEventDates(KOAgendaItem *item, int type)
{
 

    int xxx = item->cellX();
    //qDebug("KOAgendaView::updateEventDates %d %d %d %d %d", xxx, mMinY.at(xxx),mMaxY.at(xxx),item->cellYTop(),item->cellYBottom() );
    if ( xxx >= 0 && xxx < mMinY.count() &&  !item->isAllDay() ) {
        if ( mMinY.at(xxx) > item->cellYTop() )
            mMinY.at(xxx) = item->cellYTop();
        if ( mMaxY.at(xxx) < item->cellYBottom() )
            mMaxY.at(xxx) = item->cellYBottom();
    }
    QDateTime startDt,endDt;
    QDate startDate;
    int lenInSecs;
    //  if ( type == KOAgenda::RESIZETOP )
    //       qDebug("RESIZETOP ");
    //   if ( type ==  KOAgenda::RESIZEBOTTOM )
    //       qDebug("RESIZEBOTTOM ");
    //   if ( type ==  KOAgenda::MOVE )
    //       qDebug("MOVE ");
    if ( item->incidence()->typeID() == eventID ) {
        startDt =item->incidence()->dtStart();
        endDt = item->incidence()->dtEnd();
        lenInSecs = startDt.secsTo( endDt );
    }

    // emit incidenceItemChanged( item->incidence(), KOGlobals::EVENTEDITED );

    if ( item->incidence()->typeID()== todoID && item->mLastMoveXPos > 0 ) {
        startDate = mSelectedDates[item->mLastMoveXPos];
    } else {
        if (item->cellX() < 0) {
            startDate = (mSelectedDates.first()).addDays(item->cellX());
        } else {
            startDate = mSelectedDates[item->cellX()];
        }
    }
    startDt.setDate(startDate);

    if (item->incidence()->doesFloat()) {
        endDt.setDate(startDate.addDays(item->cellWidth() - 1));
    } else {
        if ( type == KOAgenda::RESIZETOP ||  type ==  KOAgenda::MOVE )
            startDt.setTime(mAgenda->gyToTime(item->cellYTop()));
        if ( item->incidence()->typeID() == eventID ) {
            if ( type ==  KOAgenda::MOVE ) {
                endDt = startDt.addSecs(lenInSecs);

            } else if ( type ==  KOAgenda::RESIZEBOTTOM  ) {
                if (item->lastMultiItem()) {
                    endDt.setTime(mAgenda->gyToTime(item->lastMultiItem()->cellYBottom()+1));
                    endDt.setDate(startDate.
                                  addDays(item->lastMultiItem()->cellX() - item->cellX()));
                } else {
                    endDt.setTime(mAgenda->gyToTime(item->cellYBottom()+1));
                    endDt.setDate(startDate);
                }
            }
        } else {
            // todo
            if (item->lastMultiItem()) {
                endDt.setTime(mAgenda->gyToTime(item->lastMultiItem()->cellYBottom()+1));
                endDt.setDate(startDate.
                              addDays(item->lastMultiItem()->cellX() - item->cellX()));
            } else {
                //qDebug("tem->cellYBottom() %d",item->cellYBottom() );
                if ( item->cellYBottom() > 0 )
                    endDt.setTime(mAgenda->gyToTime(item->cellYBottom()+1));
                else
                    endDt.setTime((static_cast<Todo*>(item->incidence()))->dtDue().time());
                endDt.setDate(startDate);
            }
        }
    }
    if ( item->incidence()->typeID() == eventID ) {
        item->incidence()->setDtStart(startDt);
        (static_cast<Event*>(item->incidence()))->setDtEnd(endDt);
    } else if ( item->incidence()->typeID() == todoID ) {
        Todo* to = static_cast<Todo*>(item->incidence());

        to->setDtDue(endDt);
        if ( to->hasStartDate() ) {
            if (to->dtStart() >= to->dtDue() )
                to->setDtStart(to->dtDue().addDays( -2 ));
        }
        
    }
    //qDebug("KOAgendaView::updateEventDates stsart %s end %s ", startDt.toString().latin1(), endDt.toString().latin1() );
    item->incidence()->setRevision(item->incidence()->revision()+1);
    item->setItemDate(startDt.date());
    //item->updateItem();
    if ( item->incidence()->typeID() == todoID ) {
         emit  todoMoved((Todo*)item->incidence(),  KOGlobals::EVENTEDITED );

    }
     else
         emit incidenceChanged(item->incidence(), KOGlobals::EVENTEDITED);
     item->updateItem();
}

void KOAgendaView::showDates( const QDate &start, const QDate &end )
{
    //  kdDebug() << "KOAgendaView::selectDates" << endl;

    mSelectedDates.clear();
    // qDebug("KOAgendaView::showDates ");
    QDate d = start;
    while (d <= end) {
        mSelectedDates.append(d);
        d = d.addDays( 1 );
    }

    // and update the view
    fillAgenda();
}


void KOAgendaView::showEvents(QPtrList<Event>)
{
    kdDebug() << "KOAgendaView::showEvents() is not yet implemented" << endl;
}

void KOAgendaView::changeEventDisplay(Event *, int)
{
    //  qDebug("KOAgendaView::changeEventDisplay ");
    //  kdDebug() << "KOAgendaView::changeEventDisplay" << endl;
    // this should be re-written to be MUCH smarter.  Right now we
    // are just playing dumb.
    fillAgenda();
}

void KOAgendaView::fillAgenda(const QDate &)
{
    // qDebug("KOAgendaView::fillAgenda ");
    fillAgenda();
}

void KOAgendaView::fillAgenda()
{
    if ( globalFlagBlockStartup )
        return;
    if ( globalFlagBlockAgenda == 1 )
        return;
    if ( flag_blockfillAgenda )
        return;
    flag_blockfillAgenda = true;
    //if ( globalFlagBlockAgenda == 2 )
    //globalFlagBlockAgenda = 0;
    // globalFlagBlockPainting = false;
    if ( globalFlagBlockAgenda == 0 )
        globalFlagBlockAgenda = 1;
    //  clearView();
    //qDebug("fillAgenda()++++ ");
    globalFlagBlockAgendaItemPaint = 1;

    mAllDayAgenda->changeColumns(mSelectedDates.count());
    mAgenda->changeColumns(mSelectedDates.count());
    qApp->processEvents(); 
    mEventIndicatorTop->changeColumns(mSelectedDates.count());
    mEventIndicatorBottom->changeColumns(mSelectedDates.count());
    setHolidayMasks();

    mMinY.resize(mSelectedDates.count());
    mMaxY.resize(mSelectedDates.count());

    QPtrList<Event> dayEvents;

    // ToDo items shall be displayed for the day they are due, but only showed today if they are already overdue.
    // Therefore, gtodoset all of them.
    QPtrList<Todo> todos  = calendar()->todos();

    mAgenda->setDateList(mSelectedDates);

    QDate today = QDate::currentDate();

    DateList::ConstIterator dit;
    int curCol = 0;
    int maxCol = mSelectedDates.count()-1;
    for( dit = mSelectedDates.begin(); dit != mSelectedDates.end(); ++dit ) {
        QDate currentDate = *dit;
        //    kdDebug() << "KOAgendaView::fillAgenda(): " << currentDate.toString()
        //              << endl;

        dayEvents = calendar()->events(currentDate,false);

        // Default values, which can never be reached
        mMinY[curCol] = mAgenda->timeToY(QTime(23,59)) + 1;
        mMaxY[curCol] = mAgenda->timeToY(QTime(0,0)) - 1;

        unsigned int numEvent;
        //qDebug("+++++NUMEVENT %d", dayEvents.count());
        for(numEvent=0;numEvent<dayEvents.count();++numEvent) {
            Event *event = dayEvents.at(numEvent);
            if ( !KOPrefs::instance()->mShowSyncEvents && event->uid().left(2) == QString("la") )
                if ( event->uid().left(15) == QString("last-syncEvent-")  )
                    continue;
            //      kdDebug() << " Event: " << event->summary() << endl;

            int beginX = currentDate.daysTo(event->dtStart().date()) + curCol;
            int endX = currentDate.daysTo(event->dtEnd().date()) + curCol;

            //      kdDebug() << "  beginX: " << beginX << "  endX: " << endX << endl;

            if (event->doesFloat()) {
                if (event->doesRecur()) {
                    if (event->isMultiDay() ) {
                        endX = endX - beginX;// endX is now number of days
                        if ( event->recursOn( currentDate ) ) {
                            endX +=  curCol;
                            beginX = curCol;
                            mAllDayAgenda->insertAllDayItem(event,currentDate,beginX,endX);
                        } else {
                            //qDebug("days endX %d curCol %d max Col %d %s",endX ,curCol, maxCol, currentDate.toString().latin1());
                            if ( curCol == maxCol && maxCol+1 < endX ) {
                                int i;
                                for ( i = 1; i< endX; ++i ) {
                                    if ( event->recursOn( currentDate.addDays( -i ) ) )
                                        break;
                                }
                                if ( i > maxCol ) {
                                    mAllDayAgenda->insertAllDayItem(event,currentDate,0,curCol);
                                    //qDebug("BINGO ");
                                }

                            } else {
                                QDate dateit = currentDate.addDays( -endX );
                                if ( event->recursOn( dateit ) ) {
                                    //qDebug("found %d %d %d %s", endX,curCol, curCol-endX ,dateit.toString().latin1()  );
                                    if ( curCol-endX < 0 ) {
                                        mAllDayAgenda->insertAllDayItem(event,currentDate,0,curCol);
                                    }
                                }
                            }
                        }
                    } else {
                        mAllDayAgenda->insertAllDayItem(event,currentDate,curCol,curCol);
                    }
                } else {
                    if (beginX <= 0 && curCol == 0) {
                        mAllDayAgenda->insertAllDayItem(event,currentDate,beginX,endX);
                    } else if (beginX == curCol) {
                        mAllDayAgenda->insertAllDayItem(event,currentDate,beginX,endX);
                    }
                }
            } else if (event->isMultiDay()) {
                if ( event->doesRecur () ) {
                    QDate dateit = currentDate;
                    int count = 0;
                    int max = event->dtStart().daysTo( event->dtEnd() ) +2;
                    while (! event->recursOn( dateit ) && count <= max ) {
                        ++count; 
                        dateit = dateit.addDays( -1 );
                    }
                    bool ok;
                    QDateTime nextOcstart = event->getNextOccurence( QDateTime(dateit) ,&ok );
                    if ( ok )
                         {
                                int secs = event->dtStart().secsTo( event->dtEnd() );
                                QDateTime nextOcend =nextOcstart.addSecs( secs ); ;
                                beginX = currentDate.daysTo(nextOcstart.date()) + curCol;
                                endX = currentDate.daysTo(nextOcend.date()) + curCol;
                            
                        }
                }
                int startY = mAgenda->timeToY(event->dtStart().time());
                int endY = mAgenda->timeToY(event->dtEnd().time()) - 1;
                //qDebug("insert %d %d %d %d   %d ",beginX,endX,startY,endY , curCol );
                if ((beginX <= 0 && curCol == 0) || beginX == curCol) {
                    //qDebug("insert!!! ");
                    mAgenda->insertMultiItem(event,currentDate,beginX,endX,startY,endY);
                }
                if (beginX == curCol) {
                    mMaxY[curCol] = mAgenda->timeToY(QTime(23,59));
                    if (startY < mMinY[curCol]) mMinY[curCol] = startY;
                } else if (endX == curCol) {
                    mMinY[curCol] = mAgenda->timeToY(QTime(0,0));
                    if (endY > mMaxY[curCol]) mMaxY[curCol] = endY;
                } else {
                    mMinY[curCol] = mAgenda->timeToY(QTime(0,0));
                    mMaxY[curCol] = mAgenda->timeToY(QTime(23,59));
                }
            } else {
                int startY = mAgenda->timeToY(event->dtStart().time());
                int endY = mAgenda->timeToY(event->dtEnd().time()) - 1;
                if (endY < startY) endY = startY;
                mAgenda->insertItem(event,currentDate,curCol,startY,endY);
                if (startY < mMinY[curCol]) mMinY[curCol] = startY;
                if (endY > mMaxY[curCol]) mMaxY[curCol] = endY;
            }
        }
        // ---------- [display Todos --------------
        unsigned int numTodo;
        for (numTodo = 0; numTodo < todos.count(); ++numTodo) {
            Todo *todo = todos.at(numTodo);

            if ( ! todo->hasDueDate() && !todo->hasCompletedDate()) continue;  // todo shall not be displayed if it has no date
            if ( todo->hasCompletedDate() && !KOPrefs::instance()->mShowCompletedTodoInAgenda ) continue;
            // ToDo items shall be displayed for the day they are due, but only showed today if they are already overdue.
            // Already completed items can be displayed on their original due date
            //if not KOPrefs::instance()->mShowTodoInAgenda, show overdue in agenda
            bool overdue = (!todo->isCompleted()) && (todo->dtDue() < today) && KOPrefs::instance()->mShowTodoInAgenda;
            bool fillIn = false;
            if ( todo->hasCompletedDate() && todo->completed().date() == currentDate )
                fillIn = true;
            if ( ! fillIn && !todo->hasCompletedDate() ) 
                fillIn = ((todo->dtDue().date() == currentDate) && !overdue) || ((currentDate == today) && overdue);
            if ( fillIn ) {
                if ( (todo->doesFloat() || overdue ) && !todo->hasCompletedDate() ) {  // Todo has no due-time set or is already overdue
                    if ( KOPrefs::instance()->mShowTodoInAgenda )
                        mAllDayAgenda->insertAllDayItem(todo, currentDate, curCol, curCol);
                }
                else {
                    QDateTime dt;
                    if ( todo->hasCompletedDate() )
                        dt = todo->completed();
                    else
                        dt = todo->dtDue();;
                        

                    int endY = mAgenda->timeToY(dt.time()) - 1;
                    int hi = (18/KOPrefs::instance()->mHourSize);
                    //qDebug("hei %d ",KOPrefs::instance()->mHourSize);
                    int startY = endY -hi;

                    mAgenda->insertItem(todo,currentDate,curCol,startY,endY);

                    if (startY < mMinY[curCol]) mMinY[curCol] = startY;
                    if (endY > mMaxY[curCol]) mMaxY[curCol] = endY;
                }
            }
        }
        // ---------- display Todos] --------------

        ++curCol;
    }
    mAgenda->hideUnused();
    mAllDayAgenda->hideUnused();
    mAgenda->checkScrollBoundaries();
    deleteSelectedDateTime();
    createDayLabels();
    emit incidenceSelected( 0 );

    if ( globalFlagBlockAgenda == 2 ) {
        if ( KOPrefs::instance()->mSetTimeToDayStartAt )
            setStartHour( KOPrefs::instance()->mDayBegins  );
        else if ( KOPrefs::instance()->mCenterOnCurrentTime )
            setStartHour( QTime::currentTime ().hour() );
    }
    qApp->processEvents(); 
    globalFlagBlockAgenda = 0; 
    mAllDayAgenda->drawContentsToPainter();
    mAgenda->drawContentsToPainter();
    repaintAgenda();
    startIdleTimeout();
    flag_blockfillAgenda = false;
}
void KOAgendaView::repaintAgenda()
{
    mAgenda->viewport()->repaint( false );
    mAllDayAgenda->viewport()->repaint( false );
    mAgenda->finishUpdate();
    mAllDayAgenda->finishUpdate();
}


void KOAgendaView::clearView()
{
    mAllDayAgenda->clear();
    mAgenda->clear();
}
void KOAgendaView::clearList()
{
    clearView();
    mAllDayAgenda->hideUnused();
    mAgenda->hideUnused();
}

void KOAgendaView::printPreview(CalPrinter *calPrinter, const QDate &fd,
                                const QDate &td)
{
#ifndef KORG_NOPRINTER
    if (fd == td)
        calPrinter->preview(CalPrinter::Day, fd, td);
    else
        calPrinter->preview(CalPrinter::Week, fd, td);
#endif
}

// void KOAgendaView::updateMovedTodo()
// {
//     // updateConfig();
//     // emit updateTodoViews();
// }

void KOAgendaView::slotShowDateView( int mode , int d )
{
    if ( d >= mSelectedDates.count() ) {
        qDebug("KOAgendaView::slotShowDateView datecounterror %d %d ", d, mSelectedDates.count() );

    } else {
        QDate day = mSelectedDates[d];
        emit showDateView(mode , day );
    }

}
void KOAgendaView::newEvent(int gx, int gy)
{
    if (!mSelectedDates.count()) return;
  
    QDate day = mSelectedDates[gx];

    QTime time = mAgenda->gyToTime(gy);
    QDateTime dt(day,time);
    // if ( dt <  QDateTime::currentDateTime () )
    //    dt = QDateTime::currentDateTime ().addSecs( 3600 );
    emit newEventSignal(dt);
}

void KOAgendaView::newEvent(int gxStart, int gyStart, int gxEnd, int gyEnd)
{
    if (!mSelectedDates.count()) return;
  
    QDate dayStart = mSelectedDates[gxStart];
    QDate dayEnd = mSelectedDates[gxEnd];

    QTime timeStart = mAgenda->gyToTime(gyStart);
    QTime timeEnd = mAgenda->gyToTime( gyEnd + 1 );

    QDateTime dtStart(dayStart,timeStart);
    QDateTime dtEnd(dayEnd,timeEnd);

    emit newEventSignal(dtStart,dtEnd);
}

void KOAgendaView::newEventAllDay(int gx, int )
{
    if (!mSelectedDates.count()) return;

    QDate day = mSelectedDates[gx];

    emit newEventSignal(day);
}
void KOAgendaView::newTodoAllDay(int gx, int )
{
    if (!mSelectedDates.count()) return;

    QDateTime day (mSelectedDates[gx] );
    emit newTodoSignal(day, true);
}
void KOAgendaView::newTodo(int gx, int gy )
{
    if (!mSelectedDates.count()) return;
    QDate dayStart = mSelectedDates[gx];
    QTime timeStart = mAgenda->gyToTime(gy);
    QDateTime dt (dayStart,timeStart);
    emit newTodoSignal( dt, false );
}

void KOAgendaView::updateEventIndicatorTop(int newY)
{
    uint i;
    for(i=0;i<mMinY.size();++i) {
        if (newY >= mMinY.at(i)) mEventIndicatorTop->enableColumn(i,true);
        else mEventIndicatorTop->enableColumn(i,false);
    }

    mEventIndicatorTop->update();
}

void KOAgendaView::updateEventIndicatorBottom(int newY)
{
    uint i;
    for(i=0;i<mMaxY.size();++i) {
        if (newY <= mMaxY.at(i)) mEventIndicatorBottom->enableColumn(i,true);
        else mEventIndicatorBottom->enableColumn(i,false);
    }

    mEventIndicatorBottom->update();
}

void KOAgendaView::startDrag(Event *event)
{
#ifndef KORG_NODND
    DndFactory factory( calendar() );
    ICalDrag *vd = factory.createDrag(event,this);
    if (vd->drag()) {
        kdDebug() << "KOAgendaView::startDrag(): Delete drag source" << endl;
    }
#endif
}

void KOAgendaView::readSettings()
{
    readSettings(KOGlobals::config());
}

void KOAgendaView::readSettings(KConfig *config)
{
    //  kdDebug() << "KOAgendaView::readSettings()" << endl;

    config->setGroup("Views");

    //#ifndef KORG_NOSPLITTER
    QValueList<int> sizes = config->readIntListEntry("Separator AgendaView");
    if (sizes.count() == 2) {
        if ( sizes[0] < 20 ) {
            sizes[1] = sizes[1] +20 - sizes[0];
            sizes[0] = 20;
        }
        mSplitterAgenda->setSizes(sizes);
        //  qDebug("read %d %d ",sizes[0],sizes[1]  );
    }
    //#endif

    // updateConfig();
}

void KOAgendaView::writeSettings(KConfig *config)
{
    //  kdDebug() << "KOAgendaView::writeSettings()" << endl;

    config->setGroup("Views");

    //#ifndef KORG_NOSPLITTER
    QValueList<int> list = mSplitterAgenda->sizes();
    config->writeEntry("Separator AgendaView",list);
    //qDebug("write %d %d ", list[0],list[1] );
    //#endif
}

void KOAgendaView::setHolidayMasks()
{
    mHolidayMask.resize(mSelectedDates.count());

    uint i;
    for(i=0;i<mSelectedDates.count();++i) {
        QDate date = mSelectedDates[i];
        bool showSaturday = KOPrefs::instance()->mExcludeSaturdays && (date.dayOfWeek() == 6);
        bool showSunday = KOPrefs::instance()->mExcludeHolidays && (date.dayOfWeek() == 7);
        bool showHoliday = false;
        if ( KOPrefs::instance()->mExcludeHolidays ) {
            QPtrList<Event> events = calendar()->events( date, true );
            Event *event;
            for( event = events.first(); event; event = events.next() ) {
                if ( event->isHoliday()) {
                    showHoliday = true;
                    break;
                }
            }

        }

#ifndef KORG_NOPLUGINS
        bool showHoliday = KOPrefs::instance()->mExcludeHolidays &&
            !KOCore::self()->holiday(date).isEmpty();
#endif
        bool showDay = showSaturday || showSunday || showHoliday;

        if (showDay) {
            mHolidayMask.at(i) = true;
        } else {
            mHolidayMask.at(i) = false;
        }
    }

    mAgenda->setHolidayMask(&mHolidayMask);
    mAllDayAgenda->setHolidayMask(&mHolidayMask);
}

void KOAgendaView::setContentsPos(int y)
{
    mAgenda->setContentsPos(0,y);
}

void KOAgendaView::clearSelection()
{
    mAgenda->deselectItem();
    mAllDayAgenda->deselectItem();
}

void KOAgendaView::newTimeSpanSelectedAllDay(int gxStart, int gyStart,
                                             int gxEnd, int gyEnd)
{
    mTimeSpanInAllDay = true;
    newTimeSpanSelected(gxStart,gyStart,gxEnd,gyEnd);
}




void KOAgendaView::newTimeSpanSelected(int gxStart, int gyStart,
                                       int gxEnd, int gyEnd)
{
    if (!mSelectedDates.count()) return;
 
    QDate dayStart = mSelectedDates[gxStart];
    QDate dayEnd = mSelectedDates[gxEnd];

    QTime timeStart = mAgenda->gyToTime(gyStart);
    QTime timeEnd = mAgenda->gyToTime( gyEnd + 1 );

    QDateTime dtStart(dayStart,timeStart);
    QDateTime dtEnd(dayEnd,timeEnd);

    mTimeSpanBegin = dtStart;
    mTimeSpanEnd = dtEnd;
 
}
                                   
void KOAgendaView::deleteSelectedDateTime()
{
    mTimeSpanBegin.setDate(QDate());
    mTimeSpanEnd.setDate(QDate());
    mTimeSpanInAllDay = false;
}

void KOAgendaView::keyPressEvent ( QKeyEvent * e ) 
{
    e->ignore();
}

void KOAgendaView::scrollOneHourUp()
{
 
    mAgenda->scrollBy ( 0,  -mAgenda->contentsHeight () / 24 );
}
void KOAgendaView::scrollOneHourDown()
{
    mAgenda->scrollBy ( 0,  mAgenda->contentsHeight () / 24 );
}

void  KOAgendaView::setStartHour( int h )
{
    mAgenda->setStartHour( h );

}
void  KOAgendaView::setInitStartHour()
{
 
    if ( KOPrefs::instance()->mCenterOnCurrentTime )
        setStartHour( QTime::currentTime ().hour() );
    else
        setStartHour( KOPrefs::instance()->mDayBegins  );
    
}
 

void  KOAgendaView::updateTodo( Todo * t, int )
{ 
    if ( !isVisible() )
        return;
    bool remove = false;
    bool removeAD = false;
    QDate da;
    if ( t->hasCompletedDate() )
        da = t->completed().date();
    else 
        da = t->dtDue().date();
    if ( ! t->hasDueDate() && !t->hasCompletedDate() ) {
        remove = true;
        removeAD = true;
    }
    else {
        bool overdue = (!t->isCompleted()) && (t->dtDue() < QDate::currentDate()) && KOPrefs::instance()->mShowTodoInAgenda ;
        if ( overdue && 
             QDate::currentDate() >= mSelectedDates.first() &&
             QDate::currentDate() <= mSelectedDates.last()) {
            removeAD = false;
            remove = true;
        }
        else {
            
            if ( da < mSelectedDates.first() ||
                 da > mSelectedDates.last() ) {
                remove = true;
                removeAD = true;
            } else {
                remove = t->doesFloat() && !t->hasCompletedDate();
                removeAD = !remove;
            }
        }
    }
    int days = mSelectedDates.first().daysTo( da  );
    //qDebug("daysto %d  %d %d", days, remove,removeAD );
    mAgenda->updateTodo( t , days, remove); 
    if ( KOPrefs::instance()->mShowTodoInAgenda )
        mAllDayAgenda->updateTodo( t , days, removeAD);
    //qDebug("KOAgendaView::updateTodo( Todo *, int ) ");

}
