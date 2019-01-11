/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

    Marcus Bains line.
    Copyright (c) 2001 Ali Rahimi

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

#ifndef _WIN32_
#define protected public
#include <qwidget.h>
#undef protected 
#endif
#include <qintdict.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qpainter.h>

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>

#include "koagendaitem.h"
#include "koprefs.h"
#include "koglobals.h"

#include "koagenda.h"

#include <libkcal/event.h>
#include <libkcal/todo.h>

#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#endif

//extern bool globalFlagBlockPainting;
extern int globalFlagBlockAgenda;
extern int globalFlagBlockAgendaItemPaint;
extern int globalFlagBlockAgendaItemUpdate;
extern int globalFlagBlockStartup;


////////////////////////////////////////////////////////////////////////////
MarcusBains::MarcusBains(KOAgenda *_agenda,const char *name)
    : QFrame(_agenda->viewport(),name), agenda(_agenda)
{
    setLineWidth(0);
    setMargin(0);
    setBackgroundColor(Qt::red);
    minutes = new QTimer(this);
    connect(minutes, SIGNAL(timeout()), this, SLOT(updateLoc()));
    minutes->start(0, true);
    mTimeBox = new QLabel(this);
    mTimeBox->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    QPalette pal = mTimeBox->palette();
    pal.setColor(QColorGroup::Foreground, Qt::red);
    mTimeBox->setPalette(pal);
    //mTimeBox->setAutoMask(true);

    agenda->addChild(mTimeBox);

    oldToday = -1;
}

MarcusBains::~MarcusBains()
{
    //delete minutes;
}
void MarcusBains::hideMe()
{
    hide(); mTimeBox->hide();
}
int MarcusBains::todayColumn()
{
    QDate currentDate = QDate::currentDate();

    DateList dateList = agenda->dateList();
    DateList::ConstIterator it;
    int col = 0;
    for(it = dateList.begin(); it != dateList.end(); ++it) {
	if((*it) == currentDate)
	    return KOGlobals::self()->reverseLayout() ?
	                         agenda->columns() - 1 - col : col;
        ++col;
    }

    return -1;
}
void MarcusBains::updateLoc()
{
    if ( !agenda->invalidPixmap() )
        updateLocation();
}
void MarcusBains::updateLocation(bool recalculate)
{
  
    QTime tim = QTime::currentTime();
    //qDebug(" MarcusBains::updateLocation %s ", tim.toString().latin1());
    if((tim.hour() == 0) && (oldTime.hour()==23))
        recalculate = true;

    int mins = tim.hour()*60 + tim.minute();
    int minutesPerCell = 24 * 60 / agenda->rows();
    int y = mins*agenda->gridSpacingY()/minutesPerCell;
    int today = recalculate ? todayColumn() : oldToday;
    int x = agenda->gridSpacingX()*today;
    bool disabled = !(KOPrefs::instance()->mMarcusBainsEnabled);

    oldTime = tim;
    oldToday = today;

    if(disabled || (today<0)) {
        hide(); mTimeBox->hide();
        return;
    } else {
        show(); mTimeBox->show();
    }

    if(recalculate)
        setFixedSize(agenda->gridSpacingX(),1);
    agenda->moveChild(this, x, y);
    raise();

    if(recalculate)
        //mTimeBox->setFont(QFont("helvetica",10));
      mTimeBox->setFont(KOPrefs::instance()->mMarcusBainsFont);

    mTimeBox->setText(KGlobal::locale()->formatTime(tim, KOPrefs::instance()->mMarcusBainsShowSeconds));
    mTimeBox->adjustSize();
    // the -2 below is there because there is a bug in this program
    // somewhere, where the last column of this widget is a few pixels
    // narrower than the other columns.
    int offs = (today==agenda->columns()-1) ? -4 : 0;
    agenda->moveChild(mTimeBox,
               x+agenda->gridSpacingX()-mTimeBox->width()+offs-1,
                  y-mTimeBox->height());
    mTimeBox->raise();
    //mTimeBox->setAutoMask(true);
    int secs = QTime::currentTime().second();
    minutes->start( (60 - secs +1)*1000 ,true);
}


////////////////////////////////////////////////////////////////////////////


/*
  Create an agenda widget with rows rows and columns columns.
*/
KOAgenda::KOAgenda(int columns,int rows,int rowSize,QWidget *parent,
                   const char *name,WFlags f) :
  QScrollView(parent,name,f)
{
   
    mAllAgendaPopup = 0;
  mColumns = columns;
  mRows = rows;
  mGridSpacingY = rowSize;
  mAllDayMode = false; 
#ifndef DESKTOP_VERSION
  //QPEApplication::setStylusOperation( viewport(), QPEApplication::RightOnHold );
#endif
  mHolidayMask = 0;
  init();
  connect ( this, SIGNAL (contentsMoving ( int , int )   ), this, SLOT ( slotContentMove(int,int)) );
}

/*
  Create an agenda widget with columns columns and one row. This is used for
  all-day events.
*/
KOAgenda::KOAgenda(int columns,QWidget *parent,const char *name,WFlags f) :
  QScrollView(parent,name,f)
{
    mAllAgendaPopup = 0;
  blockResize = false;
  mColumns = columns;
  mRows = 1;
  //qDebug("aaaaaaaaaaaaaaaaaaldays %d ", KOPrefs::instance()->mAllDaySize);
  mGridSpacingY = KOPrefs::instance()->mAllDaySize;
  mAllDayMode = true;
#ifndef DESKTOP_VERSION
  //QPEApplication::setStylusOperation( viewport(), QPEApplication::RightOnHold );
#endif
  mHolidayMask = 0;
  init();
}


KOAgenda::~KOAgenda()
{
    if(mMarcusBains) delete mMarcusBains;
    
}

Incidence *KOAgenda::selectedIncidence() const
{
  return (mSelectedItem ? mSelectedItem->incidence() : 0);
}


QDate KOAgenda::selectedIncidenceDate() const
{
  return (mSelectedItem ? mSelectedItem->itemDate() : QDate());
}


void KOAgenda::init()
{  
    mPopupTimer = new QTimer(this);
    connect(mPopupTimer , SIGNAL(timeout()), this, SLOT(popupMenu()));

    mNewItemPopup = new QPopupMenu( this );
    connect ( mNewItemPopup, SIGNAL (activated ( int )  ), this, SLOT ( newItem(int)) );
    QString pathString = "";
    if ( !KOPrefs::instance()->mToolBarMiniIcons ) {
    if ( QApplication::desktop()->width() < 480 )
        pathString += "icons16/";
    } else
        pathString += "iconsmini/";
    
    mNewItemPopup->insertItem ( SmallIcon( pathString +"newevent" ), i18n("New Event..."), 1 ); 
    mNewItemPopup->insertItem ( SmallIcon( pathString +"newtodo" ),  i18n("New Todo..."),2 ); 
    mNewItemPopup->insertSeparator ( ); 
    mNewItemPopup->insertItem ( SmallIcon( pathString +"day" ),  i18n("Day view"),3 );  
    mNewItemPopup->insertItem ( SmallIcon( pathString +"xdays" ),  i18n("Next days"),8 );  
    mNewItemPopup->insertItem ( SmallIcon( pathString +"week" ),  i18n("Next week"),4 );  
    mNewItemPopup->insertItem ( SmallIcon( pathString +"week" ),  i18n("Next two weeks"),5 );   
    mNewItemPopup->insertItem ( SmallIcon( pathString +"month" ),  i18n("This month"),6 );  
    mNewItemPopup->insertItem ( SmallIcon( pathString +"journal" ),  i18n("Journal view"),7 ); 
#ifndef _WIN32_
  int wflags = viewport()-> getWFlags() |WRepaintNoErase;//WResizeNoErase
  viewport()->setWFlags ( wflags);
#endif
  mGridSpacingX = 80;
  mResizeBorderWidth = 8;
  mScrollBorderWidth = 8;
  mScrollDelay = 30;
  mScrollOffset = 10;
  mPaintPixmap.resize( 20,20);
  //enableClipper(true);

  // Grab key strokes for keyboard navigation of agenda. Seems to have no
  // effect. Has to be fixed.
  setFocusPolicy(WheelFocus);

  connect(&mScrollUpTimer,SIGNAL(timeout()),SLOT(scrollUp()));
  connect(&mScrollDownTimer,SIGNAL(timeout()),SLOT(scrollDown()));
  connect(&mResizeTimer,SIGNAL(timeout()),SLOT(finishResize()));

  mStartCellX = 0;
  mStartCellY = 0;
  mCurrentCellX = 0;
  mCurrentCellY = 0;

  mSelectionCellX = 0;
  mSelectionYTop = 0;
  mSelectionHeight = 0;

  mOldLowerScrollValue = -1;
  mOldUpperScrollValue = -1;

  mClickedItem = 0;

  mActionItem = 0;
  mActionType = NOP;
  mItemMoved = false;

  mSelectedItem = 0;

  // mItems.setAutoDelete(true);

  resizeContents( mGridSpacingX * mColumns + 1 , mGridSpacingY * mRows + 1 );

  viewport()->update();

  setMinimumSize(30,  1);
//  setMaximumHeight(mGridSpacingY * mRows + 5);

  // Disable horizontal scrollbar. This is a hack. The geometry should be
  // controlled in a way that the contents horizontally always fits. Then it is
  // not necessary to turn off the scrollbar.
  setHScrollBarMode(AlwaysOff);
  if ( ! mAllDayMode ) 
      setVScrollBarMode(AlwaysOn);
  else
      setVScrollBarMode(AlwaysOff);
      
  setStartHour(KOPrefs::instance()->mDayBegins);

  calculateWorkingHours();

  connect(verticalScrollBar(),SIGNAL(valueChanged(int)),
          SLOT(checkScrollBoundaries(int)));

  // Create the Marcus Bains line.
  if(mAllDayMode)
      mMarcusBains = 0;
  else {
      mMarcusBains = new MarcusBains(this);
      addChild(mMarcusBains);
  }  
  mPopupKind = 0;
  mPopupItem =  0; 
  mInvalidPixmap = false;
  
}

void KOAgenda::shrinkPixmap()
{
    mPaintPixmap.resize( 20,20);
    mInvalidPixmap = true;
}
void KOAgenda::slotContentMove(int,int)
{
    emit sendPing(); 
    if ( mActionType == NOP )
        slotClearSelection(); 
    if ( mSelectedItem && !mActionItem ) {
        deselectItem();
        emit incidenceSelected( 0 );
    }
}
void KOAgenda::clear()
{
  KOAgendaItem *item;
  for ( item=mItems.first(); item != 0; item=mItems.next() ) {
    mUnusedItems.append( item );
    //item->hide();
  }
  mItems.clear();
  mSelectedItem = 0;
  clearSelection();
}

void KOAgenda::clearSelection()
{
  mSelectionCellX = 0;
  mSelectionYTop = 0;
  mSelectionHeight = 0;
}

void KOAgenda::marcus_bains()
{
    if(mMarcusBains) mMarcusBains->updateLocation(true);
}


void KOAgenda::changeColumns(int columns)
{
    if (columns == 0) {
        qDebug("KOAgenda::changeColumns() called with argument 0 ");
        return;
    }
    clear();
    mColumns = columns;
    computeSizes();
    if(mMarcusBains) mMarcusBains->hideMe();
}

/*
  This is the eventFilter function, which gets all events from the KOAgendaItems
  contained in the agenda. It has to handle moving and resizing for all items.
*/
bool KOAgenda::eventFilter ( QObject *object, QEvent *event )
{
    //  kdDebug() << "KOAgenda::eventFilter" << endl;
    switch(event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        return eventFilter_mouse(object, static_cast<QMouseEvent *>(event));

    case (QEvent::Leave):
        if (!mActionItem)
            setCursor(arrowCursor);
        return true;

    default:
        return QScrollView::eventFilter(object,event);
    }
}
void KOAgenda::popupMenu()
{
    mPopupTimer->stop();
    if (  mPopupKind == 1 || mPopupKind == 3 ) {
        if (mActionItem ) {
            endItemAction();
        }
        mLeftMouseDown = false; // no more leftMouse computation
        if (mPopupItem) {
            //mClickedItem = mPopupItem;
            selectItem(mPopupItem);
            if ( mAllAgendaPopup && KOPrefs::instance()->mBlockPopupMenu && mPopupKind == 1 )
                mAllAgendaPopup->installEventFilter( this );
            emit showIncidencePopupSignal(mPopupItem->incidence());
            
        } 
    } else if ( mPopupKind == 2 || mPopupKind == 4 ) {
        if ( mLeftMouseDown ) { // we have a simulated right click - clear left mouse action
            endSelectAction( false ); // do not emit new event signal
            mLeftMouseDown = false; // no more leftMouse computation
        }
        if ( KOPrefs::instance()->mBlockPopupMenu &&  mPopupKind == 2 )
            mNewItemPopup->installEventFilter( this );
        mNewItemPopup->popup( mPopupPos);
        
    }
    mLeftMouseDown = false;
    mPopupItem = 0;
    mPopupKind = 0;
}
void KOAgenda::categoryChanged(Incidence * inc)
{
    KOAgendaItem *item;
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
        if ( item->incidence() == inc ) {
            item->initColor ();
            item->updateItem();
        }
    }
}
bool KOAgenda::invalidPixmap()
{
    return mInvalidPixmap;
}
bool KOAgenda::eventFilter_mouse(QObject *object, QMouseEvent *me)
{

    if ( mInvalidPixmap ) {
        mInvalidPixmap = false;
        qDebug("KO: efm Upsizing Pixmaps %s", QDateTime::currentDateTime().toString().latin1());
        computeSizes();
        emit updateViewSignal();
        return true;
    } 
    emit sendPing(); 
    static int startX = 0;
    static int startY = 0;
    int blockmoveDist = ( QApplication::desktop()->width() < 480 ? 7 : 9 );
    static bool blockMoving = true;

    //qDebug("KOAgenda::eventFilter_mous ");
    if ( object == mNewItemPopup ) {
        //qDebug("mNewItemPopup ");
        if ( me->type() == QEvent::MouseButtonRelease ) {
            mNewItemPopup->removeEventFilter( this ); 
            int dX = me->globalPos().x() - mPopupPos.x();;
            if ( dX < 0   )
                dX = -dX;
            int dY =  me->globalPos().y() - mPopupPos.y();
            if ( dY < 0 )
                dY = -dY;
            if (  dX > blockmoveDist  || dY > blockmoveDist  ) {
                mNewItemPopup->hide();
            }
        }
        return true;
    }
    if ( object == mAllAgendaPopup ) {
        //qDebug(" mAllAgendaPopup ");
        if ( me->type() == QEvent::MouseButtonRelease ) {
            mAllAgendaPopup->removeEventFilter( this );
            int dX = me->globalPos().x() - mPopupPos.x();;
            if ( dX < 0   )
                dX = -dX;
            int dY =  me->globalPos().y() - mPopupPos.y();
            if ( dY < 0 )
                dY = -dY;
            if (  dX > blockmoveDist  || dY > blockmoveDist  ) {
                mAllAgendaPopup->hide();
            } 
        }
        return true;
    }
    QPoint viewportPos;
    if (object != viewport()) {
        blockmoveDist = blockmoveDist*2;
        viewportPos = ((QWidget *)object)->mapToParent(me->pos());
    } else {
        viewportPos = me->pos();
    }
    bool objIsNotViewport = (object != viewport());
    bool leftButt = false;
#ifdef DESKTOP_VERSION
    leftButt  = (me->button() == LeftButton);
#endif
    switch (me->type())  {
    case QEvent::MouseButtonPress:
        if (me->button() == LeftButton) {
            mPopupTimer->start( 600 );
            mLeftMouseDown = true;
        }
        blockMoving = true;
        startX = viewportPos.x();
        startY = viewportPos.y();  
        mPopupPos = me->globalPos();
        if (  objIsNotViewport && !leftButt ) {
            KOAgendaItem * tempItem = (KOAgendaItem *)object;
            if (mAllDayMode) {
                if ( tempItem->height() > 10 ) {
                    int minV = tempItem->height()/4;
                    if ( minV > (blockmoveDist/2)-2 ) {
                        if ( minV > blockmoveDist )
                            minV = blockmoveDist;
                        else
                            minV = (blockmoveDist/2);
                    }
                    bool border = false;
                    int diff = tempItem->y() - viewportPos.y();
                    if ( diff < 0 )
                        diff *= -1;
                    if ( diff < minV ) {
                        border = true;
                        objIsNotViewport = false;
                    }
                    if ( ! border ) {
                        diff = tempItem->y() + tempItem->height()- viewportPos.y();
                        if ( diff < 0 )
                            diff *= -1;
                        if ( diff < minV ) {
                            border = true;
                            objIsNotViewport = false;
                        }
                    }
                }
            } else { // not allday
                if ( tempItem->width() > 10 ) {
                    int minH = tempItem->width()/4;
                    if ( minH > (blockmoveDist/2)-2 ) {
                        if ( minH > blockmoveDist )
                            minH = blockmoveDist;
                        else
                            minH = (blockmoveDist/2);
                    }
                    bool border = false;
                    int diff = tempItem->x() - viewportPos.x();
                    if ( diff < 0 )
                        diff *= -1;
                    if ( diff < minH ) {
                        border = true;
                        objIsNotViewport = false;
                    }
                    if ( ! border ) {
                        diff = tempItem->x() + tempItem->width() - viewportPos.x();
                        if ( diff < 0 )
                            diff *= -1;
                        if ( diff < minH ) {
                            border = true;
                            objIsNotViewport = false;
                        }          
                    }
                }
            }
        }
        if ( objIsNotViewport ) {
            mPopupItem = (KOAgendaItem *)object;
            mPopupKind = 1;
            if (me->button() == RightButton) {
                mPopupKind = 3;
                popupMenu();
            } else if (me->button() == LeftButton) {
                mActionItem = (KOAgendaItem *)object;
                if (mActionItem) {
                    emit signalClearSelection();
                    slotClearSelection();
                    selectItem(mActionItem);
                    Incidence *incidence = mActionItem->incidence();
                    if ( incidence->isReadOnly() /*|| incidence->doesRecur() */) {
                        mActionItem = 0;
                    } else {
                        startItemAction(viewportPos);
                    }
                }
            }
        } else { // ---------- viewport()
            mPopupItem = 0;
            mPopupKind = 2;
            selectItem(0);
            mActionItem = 0;   
            if (me->button() == RightButton) {
                int x,y;
                viewportToContents(viewportPos.x(),viewportPos.y(),x,y);
                int gx,gy;
                contentsToGrid(x,y,gx,gy); 
                mCurrentCellX = gx;
                mCurrentCellY = gy;
                mStartCellX = gx;
                mStartCellY = gy;
                mPopupKind = 4;
                popupMenu();
            } else if (me->button() == LeftButton) {
                setCursor(arrowCursor);
                startSelectAction(viewportPos);
            }
        }
        break;

    case QEvent::MouseButtonRelease:
        if (me->button() == LeftButton ) {  
            mPopupTimer->stop();
        }
        if (object != viewport()) { 
            if (me->button() == LeftButton && mLeftMouseDown) {
                if (mActionItem) {  
                    QPoint clipperPos = clipper()->mapFromGlobal(viewport()->mapToGlobal(viewportPos));
                    //qDebug(" %d %d %d  ",clipperPos.y(),visibleHeight() , 9  );
                    if ( mActionType == MOVE && (clipperPos.y() > visibleHeight()-2 ||clipperPos.y() < 0 ) ) {
                        mScrollUpTimer.stop();
                        mScrollDownTimer.stop();
                        mActionItem->resetMove();
                        placeSubCells( mActionItem );
                        // emit startDragSignal( mActionItem->incidence() );
                        setCursor( arrowCursor );
                        mActionItem = 0;
                        mActionType = NOP;
                        mItemMoved = 0;
                        mLeftMouseDown = false;
                        return true;
                    }
                    endItemAction();
                }
            }
            
        } else { // ---------- viewport()
           if (me->button() == LeftButton && mLeftMouseDown ) { //left click
                endSelectAction( true ); // emit new event signal
            }
        }
        if (me->button() == LeftButton) 
            mLeftMouseDown = false;
       
        break;

    case QEvent::MouseMove:
        //qDebug("mm ");
        if (  !mLeftMouseDown )
            return false;
        if ( blockMoving ) {
            int dX, dY; 
            dX = startX - viewportPos.x();
            if ( dX < 0   )
                dX = -dX;
            dY = viewportPos.y() - startY;
            if ( dY < 0 )
                dY = -dY;
            //qDebug("%d %d %d ", dX, dY , blockmoveDist  );
            if (  dX > blockmoveDist  || dY > blockmoveDist  ) {
                blockMoving = false;
            }
        } 
        if ( ! blockMoving  )
            mPopupTimer->stop();
        if (object != viewport()) {
            KOAgendaItem *moveItem = (KOAgendaItem *)object;
            if (!moveItem->incidence()->isReadOnly()  ) {
                if (!mActionItem)
                    setNoActionCursor(moveItem,viewportPos);
                else {
                    if ( !blockMoving )
                        performItemAction(viewportPos);
                }
            }
        } else { // ---------- viewport() 
            mPopupPos =  viewport()->mapToGlobal( me->pos() );
            if ( mActionType == SELECT ) {
                performSelectAction( viewportPos );
            }
        }
        break;

    case QEvent::MouseButtonDblClick:
        mPopupTimer->stop();
        if (object == viewport()) {
            selectItem(0);
            int x,y;
            viewportToContents(viewportPos.x(),viewportPos.y(),x,y);
            int gx,gy;
            contentsToGrid(x,y,gx,gy);
            emit newEventSignal(gx,gy);
        } else {
            KOAgendaItem *doubleClickedItem = (KOAgendaItem *)object;
            selectItem(doubleClickedItem);
            if ( KOPrefs::instance()->mEditOnDoubleClick )
                emit editIncidenceSignal(doubleClickedItem->incidence());
            else
                emit showIncidenceSignal(doubleClickedItem->incidence());
        }
        break;

    default:
        break;
    }
    return true;

}

void KOAgenda::newItem( int item )
{
    if ( item == 1 ) { //new event
        newEventSignal(mStartCellX ,mStartCellY  );
    } else
        if ( item == 2 ) { //new event
            newTodoSignal(mStartCellX ,mStartCellY  );
        } else 
            {
                emit showDateView( item, mStartCellX );
                // 3Day view
                // 4Week view
                // 5Month view
                // 6Journal view
            }
}
void KOAgenda::slotClearSelection()
{
    if (mSelectionHeight) {
        int selectionX = mSelectionCellX * mGridSpacingX; 
        int top = mSelectionYTop - 2 *mGridSpacingY;
        int hei = mSelectionHeight + 4 *mGridSpacingY;
        clearSelection();
        repaintContents( selectionX, top,
                     mGridSpacingX, hei ,false );
    }

}
void KOAgenda::startSelectAction(QPoint viewportPos)
{
   
    emit signalClearSelection();
    slotClearSelection();

    mActionType = SELECT;

    int x,y;
    viewportToContents(viewportPos.x(),viewportPos.y(),x,y);
    int gx,gy;
    contentsToGrid(x,y,gx,gy);

    mStartCellX = gx;
    mStartCellY = gy;
    mCurrentCellX = gx;
    mCurrentCellY = gy;

    // Store new selection
    mSelectionCellX = gx;
    mSelectionYTop = gy * mGridSpacingY;
    mSelectionHeight = mGridSpacingY;
  
    // Paint new selection
    repaintContents( mSelectionCellX * mGridSpacingX+1, mSelectionYTop,
                     mGridSpacingX-1, mSelectionHeight );
}

void KOAgenda::performSelectAction(QPoint viewportPos)
{
  int x,y;
  viewportToContents(viewportPos.x(),viewportPos.y(),x,y);
  int gx,gy;
  contentsToGrid(x,y,gx,gy);

  QPoint clipperPos = clipper()->
                      mapFromGlobal(viewport()->mapToGlobal(viewportPos));
  
  // Scroll if cursor was moved to upper or lower end of agenda.
  if (clipperPos.y() < mScrollBorderWidth) {
    mScrollUpTimer.start(mScrollDelay);
  } else if (visibleHeight() - clipperPos.y() <
             mScrollBorderWidth) {
    mScrollDownTimer.start(mScrollDelay);
  } else {
    mScrollUpTimer.stop();
    mScrollDownTimer.stop();
  }

  if ( gy > mCurrentCellY ) {
    mSelectionHeight = ( gy + 1 ) * mGridSpacingY - mSelectionYTop;


    repaintContents( (KOGlobals::self()->reverseLayout() ?
                     mColumns - 1 - mSelectionCellX : mSelectionCellX) *
                     mGridSpacingX, mSelectionYTop,
                     mGridSpacingX, mSelectionHeight , false);

    mCurrentCellY = gy;
  } else if ( gy < mCurrentCellY ) {
    if ( gy >= mStartCellY ) {
      int selectionHeight = mSelectionHeight;
      mSelectionHeight = ( gy + 1 ) * mGridSpacingY - mSelectionYTop;

      repaintContents( (KOGlobals::self()->reverseLayout() ?
                       mColumns - 1 - mSelectionCellX : mSelectionCellX) *
                       mGridSpacingX, mSelectionYTop,
                       mGridSpacingX, selectionHeight,false );
    
      mCurrentCellY = gy;
    } else {
    }
  }
}

void KOAgenda::endSelectAction( bool emitNewEvent )
{
  mActionType = NOP;
  mScrollUpTimer.stop();
  mScrollDownTimer.stop();
  
  emit newTimeSpanSignal(mStartCellX,mStartCellY,mCurrentCellX,mCurrentCellY);
  if ( emitNewEvent && mStartCellY < mCurrentCellY ) {
      emit newEventSignal(mStartCellX,mStartCellY,mCurrentCellX,mCurrentCellY);
  }
}

void KOAgenda::startItemAction(QPoint viewportPos)
{
  int x,y;
  viewportToContents(viewportPos.x(),viewportPos.y(),x,y);
  int gx,gy;
  contentsToGrid(x,y,gx,gy);

  mStartCellX = gx;
  mStartCellY = gy;
  mCurrentCellX = gx;
  mCurrentCellY = gy;
  bool allowResize = ( mActionItem->incidence()->typeID() != todoID );

  if (mAllDayMode) {
    int gridDistanceX = (x - gx * mGridSpacingX);
    if ( allowResize && gridDistanceX < mResizeBorderWidth &&
        mActionItem->cellX() == mCurrentCellX) {
      mActionType = RESIZELEFT;
      setCursor(sizeHorCursor);
    } else if ( allowResize && (mGridSpacingX - gridDistanceX) < mResizeBorderWidth &&
               mActionItem->cellXWidth() == mCurrentCellX) {
      mActionType = RESIZERIGHT;
      setCursor(sizeHorCursor);
    } else {
      mActionType = MOVE;
      mActionItem->startMove();
      setCursor(sizeAllCursor);
    }
  } else {
    int gridDistanceY = (y - gy * mGridSpacingY);
    if (allowResize && gridDistanceY < mResizeBorderWidth &&
        mActionItem->cellYTop() == mCurrentCellY &&
        !mActionItem->firstMultiItem()) {
      mActionType = RESIZETOP;
      setCursor(sizeVerCursor);
    } else if (allowResize &&(mGridSpacingY - gridDistanceY) < mResizeBorderWidth &&
               mActionItem->cellYBottom() == mCurrentCellY &&
               !mActionItem->lastMultiItem())  {
      mActionType = RESIZEBOTTOM;
      setCursor(sizeVerCursor);
    } else {
      mActionType = MOVE;
      mActionItem->startMove();
      setCursor(sizeAllCursor);
    }
  }
}

void KOAgenda::performItemAction(QPoint viewportPos)
{
//  kdDebug() << "viewportPos: " << viewportPos.x() << "," << viewportPos.y() << endl;
//  QPoint point = viewport()->mapToGlobal(viewportPos);
//  kdDebug() << "Global: " << point.x() << "," << point.y() << endl;
//  point = clipper()->mapFromGlobal(point);
//  kdDebug() << "clipper: " << point.x() << "," << point.y() << endl;
//  kdDebug() << "visible height: " << visibleHeight() << endl;
  int x,y;
  viewportToContents(viewportPos.x(),viewportPos.y(),x,y);
//  kdDebug() << "contents: " << x << "," << y << "\n" << endl;
  int gx,gy;
  contentsToGrid(x,y,gx,gy);
  QPoint clipperPos = clipper()->
                      mapFromGlobal(viewport()->mapToGlobal(viewportPos));

  // Cursor left active agenda area.
  // This starts a drag.
  if ( /*clipperPos.y() < 0 || clipperPos.y() > visibleHeight() ||*/
       clipperPos.x() < 0 || clipperPos.x() > visibleWidth() ) {
    if ( mActionType == MOVE ) {
      mScrollUpTimer.stop();
      mScrollDownTimer.stop();
      mActionItem->resetMove();
      placeSubCells( mActionItem );
      // emit startDragSignal( mActionItem->incidence() );
      setCursor( arrowCursor );
      mActionItem = 0;
      mActionType = NOP;
      mItemMoved = 0;
      return;
    }
  } else {
    switch ( mActionType ) {
      case MOVE:
        setCursor( sizeAllCursor );
        break;
      case RESIZETOP:
      case RESIZEBOTTOM:
        setCursor( sizeVerCursor );
        break;
      case RESIZELEFT:
      case RESIZERIGHT:
        setCursor( sizeHorCursor );
        break;
      default:
        setCursor( arrowCursor );
    }
  }

  // Scroll if item was moved to upper or lower end of agenda.
  if (clipperPos.y() < mScrollBorderWidth) {
    mScrollUpTimer.start(mScrollDelay);
  } else if (visibleHeight() - clipperPos.y() <
             mScrollBorderWidth) {
    mScrollDownTimer.start(mScrollDelay);
  } else {
    mScrollUpTimer.stop();
    mScrollDownTimer.stop();
  }

  // Move or resize item if necessary
  if (mCurrentCellX != gx || mCurrentCellY != gy) {
    mItemMoved = true;
    mActionItem->raise();
    if (mActionType == MOVE) {
      // Move all items belonging to a multi item
      KOAgendaItem *moveItem = mActionItem->firstMultiItem();
      bool isMultiItem = (moveItem || mActionItem->lastMultiItem());
      if (!moveItem) moveItem = mActionItem;
      while (moveItem) {
        int dy;
        if (isMultiItem) dy = 0;
        else dy = gy - mCurrentCellY;
        moveItem->moveRelative(gx - mCurrentCellX,dy);
        int x,y;
        gridToContents(moveItem->cellX(),moveItem->cellYTop(),x,y);
        int diff = moveItem->resizeMe(mGridSpacingX, mGridSpacingX* moveItem->cellWidth(),
                         mGridSpacingY * moveItem->cellHeight());
        moveItem->raise();
        moveChild(moveItem,x+diff,y);
        moveItem = moveItem->nextMultiItem();
      }
    } else if (mActionType == RESIZETOP) {
      if (mCurrentCellY <= mActionItem->cellYBottom()) {
        mActionItem->expandTop(gy - mCurrentCellY);
        mActionItem->resize(mActionItem->width(),
                            mGridSpacingY * mActionItem->cellHeight());
        int x,y;
        gridToContents(mCurrentCellX,mActionItem->cellYTop(),x,y);
        //moveChild(mActionItem,childX(mActionItem),y);
        QScrollView::moveChild( mActionItem,childX(mActionItem),y );
      }
    } else if (mActionType == RESIZEBOTTOM) {
      if (mCurrentCellY >= mActionItem->cellYTop()) {
        mActionItem->expandBottom(gy - mCurrentCellY);
        mActionItem->resize(mActionItem->width(),
                            mGridSpacingY * mActionItem->cellHeight());
      }
    } else if (mActionType == RESIZELEFT) {
       if (mCurrentCellX <= mActionItem->cellXWidth()) {
         mActionItem->expandLeft(gx - mCurrentCellX);
         int diff =  mActionItem->resizeMe(mGridSpacingX ,
                                           mGridSpacingX * mActionItem->cellWidth(),
                                           mActionItem->height());
        int x,y;
        gridToContents(mActionItem->cellX(),mActionItem->cellYTop(),x,y);
        moveChild(mActionItem,x+diff,childY(mActionItem));
       }
    } else if (mActionType == RESIZERIGHT) {
       if (mCurrentCellX >= mActionItem->cellX()) {
         mActionItem->expandRight(gx - mCurrentCellX);
         mActionItem->resizeMe(mGridSpacingX, mGridSpacingX * mActionItem->cellWidth(),
                             mActionItem->height());
       }
    }
    mCurrentCellX = gx;
    mCurrentCellY = gy;
  }
}

void KOAgenda::endItemAction()
{
 
    if ( mItemMoved ) {
        KOAgendaItem *placeItem = mActionItem->firstMultiItem();
        if ( !placeItem ) {
            placeItem = mActionItem;      
        }
        if ( placeItem->incidence()->doesRecur() ) {
            Incidence* oldInc =  placeItem->incidence();
            placeItem->recreateIncidence();
            emit addToCalSignal(placeItem->incidence(), oldInc );
        }
        int type = mActionType;
        if ( mAllDayMode )
            type = -1;
        KOAgendaItem *modifiedItem = placeItem;
        //emit itemModified( placeItem, mActionType /*KOGlobals::EVENTEDITED */);
        QPtrList<KOAgendaItem> oldconflictItems ;//= placeItem->conflictItems();
        KOAgendaItem *item;
  
        if ( placeItem->incidence()->typeID() == todoID ) {
            mSelectedItem = 0;
            //qDebug("todo %d %d %d ", mCurrentCellX, modifiedItem->cellX() ,modifiedItem->cellXWidth());
            modifiedItem->mLastMoveXPos = mCurrentCellX;
            emit itemModified( modifiedItem, mActionType ); 
        }  
        else {  


            globalFlagBlockAgendaItemPaint = 1;
            for ( item=oldconflictItems.first(); item != 0;
                  item=oldconflictItems.next() ) {
                placeSubCells(item);    
            } 
            while ( placeItem ) { 
                //qDebug("placeItem  %s ", placeItem->incidence()->summary().latin1());  
                oldconflictItems = placeItem->conflictItems();
                for ( item=oldconflictItems.first(); item != 0;
                      item=oldconflictItems.next() ) {
                    placeSubCells(item);    
                } 
                placeSubCells( placeItem );
                placeItem = placeItem->nextMultiItem();
            } 
            globalFlagBlockAgendaItemPaint = 0;
            for ( item=oldconflictItems.first(); item != 0;
                  item=oldconflictItems.next() ) {
                globalFlagBlockAgendaItemUpdate = 0;
                item->repaintMe();
                globalFlagBlockAgendaItemUpdate = 1;
                item->repaint( false );    
            } 
            placeItem = modifiedItem;
        
            while ( placeItem ) { 
                //qDebug("placeItem  %s ", placeItem->incidence()->summary().latin1());  
                globalFlagBlockAgendaItemUpdate = 0;
                placeItem->repaintMe();
                globalFlagBlockAgendaItemUpdate = 1;
                placeItem->repaint(false);
                placeItem = placeItem->nextMultiItem();
            }  
            emit itemModified( modifiedItem, mActionType ); 
         

            placeItem = modifiedItem;
            while ( placeItem ) {
                oldconflictItems = placeItem->conflictItems();
                for ( item=oldconflictItems.first(); item != 0;
                      item=oldconflictItems.next() ) {
                    placeSubCells(item);    
                } 
                placeSubCells( placeItem );
                placeItem = placeItem->nextMultiItem();

            } 
            placeItem = modifiedItem;
            while ( placeItem ) {
                oldconflictItems = placeItem->conflictItems();
                for ( item=oldconflictItems.first(); item != 0;
                      item=oldconflictItems.next() ) {
                    globalFlagBlockAgendaItemUpdate = 0;
                    item->repaintMe();
                    globalFlagBlockAgendaItemUpdate = 1;
                    item->repaint(false);    
                }
                placeItem = placeItem->nextMultiItem();
            } 
            /*

            oldconflictItems = modifiedItem->conflictItems();
            for ( item=oldconflictItems.first(); item != 0;
            item=oldconflictItems.next() ) {
            globalFlagBlockAgendaItemUpdate = 0;
            item->paintMe(false);
            globalFlagBlockAgendaItemUpdate = 1;
            item->repaint(false);    
            }
            */

 
        }
     
    }
    if ( mActionItem )
        emit incidenceSelected( mActionItem->incidence() );
    mScrollUpTimer.stop();
    mScrollDownTimer.stop();
    setCursor( arrowCursor );
    mActionItem = 0;
    mActionType = NOP;
    mItemMoved = 0;

}

void KOAgenda::setNoActionCursor(KOAgendaItem *moveItem,QPoint viewportPos)
{
//  kdDebug() << "viewportPos: " << viewportPos.x() << "," << viewportPos.y() << endl;
//  QPoint point = viewport()->mapToGlobal(viewportPos);
//  kdDebug() << "Global: " << point.x() << "," << point.y() << endl;
//  point = clipper()->mapFromGlobal(point);
//  kdDebug() << "clipper: " << point.x() << "," << point.y() << endl;

  int x,y;
  viewportToContents(viewportPos.x(),viewportPos.y(),x,y);
//  kdDebug() << "contents: " << x << "," << y << "\n" << endl;
  int gx,gy;
  contentsToGrid(x,y,gx,gy);

  // Change cursor to resize cursor if appropriate
  if (mAllDayMode) {
    int gridDistanceX = (x - gx * mGridSpacingX);
    if (gridDistanceX < mResizeBorderWidth &&
        moveItem->cellX() == gx) {
      setCursor(sizeHorCursor);
    } else if ((mGridSpacingX - gridDistanceX) < mResizeBorderWidth &&
             moveItem->cellXWidth() == gx) {
      setCursor(sizeHorCursor);
    } else {
      setCursor(arrowCursor);
    }
  } else {
    int gridDistanceY = (y - gy * mGridSpacingY);
    if (gridDistanceY < mResizeBorderWidth &&
        moveItem->cellYTop() == gy &&
        !moveItem->firstMultiItem()) {
      setCursor(sizeVerCursor);
    } else if ((mGridSpacingY - gridDistanceY) < mResizeBorderWidth &&
               moveItem->cellYBottom() == gy &&
               !moveItem->lastMultiItem()) {
      setCursor(sizeVerCursor);
    } else {
      setCursor(arrowCursor);
    }
  }
}


/*
  Place item in cell and take care that multiple items using the same cell do
  not overlap. This method is not yet optimal. It doesn´t use the maximum space
  it can get in all cases.
  At the moment the method has a bug: When an item is placed only the sub cell
  widths of the items are changed, which are within the Y region the item to
  place spans. When the sub cell width change of one of this items affects a
  cell, where other items are, which do not overlap in Y with the item to place,
  the display gets corrupted, although the corruption looks quite nice.
*/
void KOAgenda::placeSubCells(KOAgendaItem *placeItem)
{

    QPtrList<KOAgendaItem> conflictItems;
    int maxSubCells = 0;
    QIntDict<KOAgendaItem> subCellDict(7);

    KOAgendaItem *item;
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
        if (item != placeItem) {
            if (placeItem->cellX() <= item->cellXWidth() &&
                placeItem->cellXWidth() >= item->cellX()) {
                if ((placeItem->cellYTop() <= item->cellYBottom()) &&
                    (placeItem->cellYBottom() >= item->cellYTop())) {
                    conflictItems.append(item);
                    if (item->subCells() > maxSubCells)
                        maxSubCells = item->subCells();
                    subCellDict.insert(item->subCell(),item);
                }
            }
        }
    }

    if (conflictItems.count() > 0) {
        // Look for unused sub cell and insert item
        int i;
        for(i=0;i<maxSubCells;++i) {
            if (!subCellDict.find(i)) {
                placeItem->setSubCell(i);
                break;
            }
        }
        if (i == maxSubCells) {
            placeItem->setSubCell(maxSubCells);
            maxSubCells++;  // add new item to number of sub cells
        }

        // Prepare for sub cell geometry adjustment
        int newSubCellWidth;
        if (mAllDayMode) newSubCellWidth = mGridSpacingY / maxSubCells;
        else newSubCellWidth = mGridSpacingX / maxSubCells;
        conflictItems.append(placeItem);


        // Adjust sub cell geometry of all direct conflict items
        for ( item=conflictItems.first(); item != 0;
              item=conflictItems.next() ) {
            item->setSubCells(maxSubCells);
            int diff = 0;
            if (mAllDayMode) {
                diff = item->resizeMe( mGridSpacingX, item->cellWidth() * mGridSpacingX, newSubCellWidth);
            } else {
                item->resize(newSubCellWidth, item->cellHeight() * mGridSpacingY);
            }
            int x,y;
            gridToContents(item->cellX(),item->cellYTop(),x,y);
            if (mAllDayMode) {
                y += item->subCell() * newSubCellWidth;
            } else {
                x += item->subCell() * newSubCellWidth;
            }
            moveChild(item,x+diff,y);  
            // qDebug("moveChild %s %d %d ", item->incidence()->summary().latin1() ,x,y);  
            //item->updateItem();
        }
        // Adjust sub cell geometry of all conflict items of all conflict items
        for ( item=conflictItems.first(); item != 0;
              item=conflictItems.next() ) {
            if ( placeItem != item ) {
                KOAgendaItem *item2;
                QPtrList<KOAgendaItem> conflictItems2 = item->conflictItems();
                for ( item2=conflictItems2.first(); item2 != 0;
                      item2=conflictItems2.next() ) {
                    if ( item2->subCells() != maxSubCells) {
                        item2->setSubCells(maxSubCells);
                        int diff = 0;
                        if (mAllDayMode) {
                            diff = item2->resizeMe(mGridSpacingX, item2->cellWidth() * mGridSpacingX, newSubCellWidth);
                        } else {
                            item2->resize(newSubCellWidth, item2->cellHeight() * mGridSpacingY);
                        }
                        int x,y;
                        gridToContents(item2->cellX(),item2->cellYTop(),x,y);
                        if (mAllDayMode) {
                            y += item2->subCell() * newSubCellWidth;
                        } else {
                            x += item2->subCell() * newSubCellWidth;
                        }
                        moveChild(item2,x+diff,y);  
                        //qDebug("setttttt %d %s",maxSubCells, item2->text().latin1() );
                    }
                }
            }
        }
    } else {
        placeItem->setSubCell(0);
        placeItem->setSubCells(1);
        int diff = 0;
        if (mAllDayMode) diff = placeItem->resizeMe( mGridSpacingX, placeItem->width(),mGridSpacingY);
        else placeItem->resize(mGridSpacingX,placeItem->height());
        int x,y;
        gridToContents(placeItem->cellX(),placeItem->cellYTop(),x,y);
        moveChild(placeItem,x+diff,y);
    }
    placeItem->setConflictItems(conflictItems);
  //   for ( item=conflictItems.first(); item != 0;
//           item=conflictItems.next() ) {
//         //item->updateItem();
//         //qDebug("xxx item->updateItem() %s %d %d", item->incidence()->summary().latin1(),item->x(), item->y() );  
//     }
//     placeItem->updateItem();
} 

void KOAgenda::drawContents(QPainter* p, int cx, int cy, int cw, int ch)
{
    if ( globalFlagBlockAgenda )
        return;

    if ( mInvalidPixmap ) {
        mInvalidPixmap = false;
        qDebug("KO: dc Upsizing Pixmaps %s", QDateTime::currentDateTime().toString().latin1());
        computeSizes();
        emit updateViewSignal();
        return;
    } 
    //qDebug("KOAgenda::drawContents %s", QDateTime::currentDateTime().toString().latin1());
    if ( ! mAllDayMode ) {
        // currently not working for
   
        //qDebug("KOAgenda::drawContents ");
#if 0
        if (  mCurPixWid != contentsWidth() || mCurPixHei != contentsHeight() ) {
            qDebug("WAU ");
            drawContentsToPainter();
        }
#endif
        QPaintDevice* pd = p->device();
        p->end();
        int vx, vy; 
        int selectionX = KOGlobals::self()->reverseLayout() ?
            (mColumns - 1 - mSelectionCellX) * mGridSpacingX : 
            mSelectionCellX * mGridSpacingX;
        contentsToViewport ( cx, cy, vx,vy); 
        //qDebug("  %d %d %d %d %d",  cx, cy, cw,ch,mGridSpacingX-1) ;

        if ( !(selectionX ==  cx &&  cy == mSelectionYTop  &&  cw ==mGridSpacingX && ch == mSelectionHeight ) ) {
            if ( mGridSpacingX == cw && mSelectionHeight > 0  &&  ( ( cx + cw ) >= selectionX && cx <= ( selectionX + mGridSpacingX ) &&
                                                                    ( cy + ch ) >= mSelectionYTop && cy <= ( mSelectionYTop + mSelectionHeight ) ) ) {
            
                int vxSel, vySel;
                contentsToViewport ( selectionX, mSelectionYTop, vxSel,vySel); 
                int off = mSelectionHeight;
                if ( vySel < 0 )
                    off += vySel;
                //qDebug("OFF %d %d %d", off,vySel, vy );  
                bitBlt ( pd, vx, vy+off, &mPaintPixmap, cx, cy+off, cw , ch-off  ,CopyROP);
            } else {
                bitBlt ( pd, vx, vy, &mPaintPixmap, cx, cy, cw, ch ,CopyROP);
            }
        }
        if ( mSelectionHeight > 0 ) { 
            //qDebug("----  %d %d %d %d ", selectionX, mSelectionYTop, mGridSpacingX, mSelectionHeight );
            if ( ( cx + cw ) >= selectionX && cx <= ( selectionX + mGridSpacingX ) &&
                 ( cy + ch ) >= mSelectionYTop && cy <= ( mSelectionYTop + mSelectionHeight ) ) {
                contentsToViewport ( selectionX, mSelectionYTop, vx,vy); 
                // bitBlt ( pd, vx+1, vy, &mHighlightPixmap, 0, mSelectionYTop, mGridSpacingX-1, mSelectionHeight ,CopyROP); 
                int hei = mSelectionHeight;
                int offset = 0;
                while ( hei > 0 ) {
                    int p_hei = 5;
                    if ( hei < 5 ) p_hei = hei;
                    hei -= 5;
                    bitBlt ( pd, vx+1, vy+offset, &mHighlightPixmap, 0, 0, mGridSpacingX-1, p_hei ,CopyROP); 
                    offset += 5;
                }
            }     
        } 
        p->begin( pd );
    } else {
#if 0
        qDebug("mCurPixWid  %d %d ",mCurPixWid, contentsWidth() );
        if (  mCurPixWid != contentsWidth() || mCurPixHei != contentsHeight() ) {
            qDebug("WAUWAU ");
            drawContentsToPainter();
        }
#endif
        QPaintDevice* pd = p->device();
        p->end();
        int vx, vy; 
        int selectionX = KOGlobals::self()->reverseLayout() ?
            (mColumns - 1 - mSelectionCellX) * mGridSpacingX : 
            mSelectionCellX * mGridSpacingX;
        contentsToViewport ( cx, cy, vx,vy); 
        // qDebug("  %d %d %d %d ",  cx, cy, cw,ch) ;
        if ( !(selectionX ==  cx &&  cy == mSelectionYTop  &&  cw ==mGridSpacingX && ch == mSelectionHeight ) )
            bitBlt ( pd, vx, vy, &mPaintPixmap, cx, cy, cw, ch ,CopyROP);
    
        if ( mSelectionHeight > 0 ) { 
            //qDebug("----  %d %d %d %d ", selectionX, mSelectionYTop, mGridSpacingX, mSelectionHeight );
            if ( ( cx + cw ) >= selectionX && cx <= ( selectionX + mGridSpacingX ) &&
                 ( cy + ch ) >= mSelectionYTop && cy <= ( mSelectionYTop + mSelectionHeight ) ) {
                contentsToViewport ( selectionX, mSelectionYTop, vx,vy); 
                //bitBlt ( pd, vx+1, vy, &mHighlightPixmap, 0, mSelectionYTop, mGridSpacingX-1, mSelectionHeight ,CopyROP); 
                int hei = mSelectionHeight;
                int offset = 0;
                while ( hei > 0 ) {
                    int p_hei = 5;
                    if ( hei < 5 ) p_hei = hei;
                    hei -= 5;
                    bitBlt ( pd, vx+1, vy+offset, &mHighlightPixmap, 0, 0, mGridSpacingX-1, p_hei ,CopyROP); 
                    offset += 5;
                }
            }     
        } 
        p->begin( pd );
    }

}

void KOAgenda::finishUpdate()
{

    KOAgendaItem *item;
    globalFlagBlockAgendaItemPaint = 1;  
    // Adjust sub cell geometry of all conflict items of all conflict items of all conflict items ... of the conflict item with the max number of conflictitems
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
        if ( !item->checkLayout() ) {
            //qDebug(" conflictitem found ");
            int newSubCellWidth;
            int diff = 0;
            if (mAllDayMode) newSubCellWidth = mGridSpacingY / item->subCells();
            else newSubCellWidth = mGridSpacingX / item->subCells();

            if (mAllDayMode) {
                diff = item->resizeMe(mGridSpacingX, item->cellWidth() * mGridSpacingX, newSubCellWidth);
            } else {
                item->resize(newSubCellWidth, item->cellHeight() * mGridSpacingY);
            }
            int x,y;
            gridToContents(item->cellX(),item->cellYTop(),x,y);
            if (mAllDayMode) {
                y += item->subCell() * newSubCellWidth;
            } else {
                x += item->subCell() * newSubCellWidth;
            }
            moveChild(item,x+diff,y); 
        }
    }
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
        if ( !item->isVisible() )
            item->show();
       
    }
    globalFlagBlockAgendaItemUpdate = 0;
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
            item->repaintMe( );
    }
    globalFlagBlockAgendaItemUpdate = 1;
    qApp->processEvents(); 
    globalFlagBlockAgendaItemPaint = 0;
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
        item->repaint( false );
    }
    marcus_bains();
}

/*
  Draw grid in the background of the agenda.
*/
void KOAgenda::drawContentsToPainter( QPainter* paint, bool backgroundOnly )// int cx, int cy, int cw, int ch)
{

 
    if ( ! mGridSpacingX || ! mGridSpacingY ||! mHolidayMask )
        return;
    if ( globalFlagBlockAgenda > 1 && globalFlagBlockAgenda < 4 )
        return;
    int cx = 0, cy = 0,  cw = contentsWidth(),  ch = contentsHeight();
    if ( ch < 1 )
        ch = 1;
    if (  mPaintPixmap.width() < contentsWidth()+42 || mPaintPixmap.height() < ch ) {
        mPaintPixmap.resize( contentsWidth()+42, ch );
    }
    mCurPixWid = contentsWidth();
    mCurPixHei = ch;
    if ( mHighlightPixmap.width() < mGridSpacingX-1  ) {
        mHighlightPixmap.resize( mGridSpacingX-1, 5 );
        mHighlightPixmap.fill ( KOPrefs::instance()->mHighlightColor );
    }
    mPixPainter.begin( &mPaintPixmap) ;
    //qDebug("wid %d hei %d ",mPaintPixmap.width(),mPaintPixmap.height()  );
    QPainter * p ;
    if (paint == 0) {
        mPaintPixmap.fill(KOPrefs::instance()->mAgendaBgColor);
        p = &mPixPainter;
    }
    else
        p = paint ;
    // qDebug("++++++KOAgenda::drawContentsTo Painter %d %d %d %d ",  cx, cy, cw, ch); 
   
    //--cx;++cw;
    int lGridSpacingY = mGridSpacingY*2;
    uint selDay;
    QDate curDate = QDate::currentDate();
    if ( !backgroundOnly ) {
        for ( selDay = 0; selDay < mSelectedDates.count();  ++selDay)
            {
                if ( mSelectedDates[selDay] == curDate && KOPrefs::instance()->mHighlightCurrentDay) {
                    int x1 = cx;
                    int y1 = 0;
                    if (y1 < cy) y1 = cy;
                    int x2 = cx+cw-1;
                    int y2 = contentsHeight();
                    if (y2 > cy+ch-1) y2=cy+ch-1;
                    if (x2 >= x1 && y2 >= y1) {
                        int gxStart = selDay;
                        //int gxEnd = gxStart ;
                        int xStart = KOGlobals::self()->reverseLayout() ?
                            (mColumns - 1 - gxStart)*mGridSpacingX :
                            gxStart*mGridSpacingX;
                        if (xStart < x1) xStart = x1;
                        int xEnd = KOGlobals::self()->reverseLayout() ?
                            (mColumns - gxStart)*mGridSpacingX-1 :
                            (gxStart+1)*mGridSpacingX-1;
                        if (xEnd > x2) xEnd = x2;
                        if ( KOPrefs::instance()->mUseHighlightLightColor )
                            p->fillRect(xStart,y1,xEnd-xStart+1,y2-y1+1,
                                        KOPrefs::instance()->mAgendaBgColor.light());
                        else
                            p->fillRect(xStart,y1,xEnd-xStart+1,y2-y1+1,
                                        KOPrefs::instance()->mAgendaBgColor.dark());
                      
                    } 
                }
            }
    }
    // Highlight working hours

    if ( !backgroundOnly )
        if (mWorkingHoursEnable) {
            int x1 = cx;
            int y1 = mWorkingHoursYTop;
            if (y1 < cy) y1 = cy;
            int x2 = cx+cw-1;
            //  int x2 = mGridSpacingX * 5 - 1;
            //  if (x2 > cx+cw-1) x2 = cx + cw - 1;
            int y2 = mWorkingHoursYBottom;
            if (y2 > cy+ch-1) y2=cy+ch-1;

            if (x2 >= x1 && y2 >= y1) {
                // qDebug("x1 %d mGridSpacingX %d ", x1, mGridSpacingX );
                int gxStart = x1/mGridSpacingX;
                int gxEnd = x2/mGridSpacingX;
                while(gxStart <= gxEnd) {
                    if (gxStart < int(mHolidayMask->count()) &&
                        !mHolidayMask->at(gxStart)) {
                        int xStart = KOGlobals::self()->reverseLayout() ?
                            (mColumns - 1 - gxStart)*mGridSpacingX :
                            gxStart*mGridSpacingX;
                        if (xStart < x1) xStart = x1;
                        int xEnd = KOGlobals::self()->reverseLayout() ?
                            (mColumns - gxStart)*mGridSpacingX-1 :
                            (gxStart+1)*mGridSpacingX-1;
                        if (xEnd > x2) xEnd = x2;
                        if ( mSelectedDates[gxStart] == curDate && KOPrefs::instance()->mHighlightCurrentDay ) {
                            if ( KOPrefs::instance()->mUseHighlightLightColor )
                                p->fillRect(xStart,y1,xEnd-xStart+1,y2-y1+1,
                                            KOPrefs::instance()->mWorkingHoursColor.light());
                            else
                                p->fillRect(xStart,y1,xEnd-xStart+1,y2-y1+1,
                                            KOPrefs::instance()->mWorkingHoursColor.dark());
                        } else {
                            p->fillRect(xStart,y1,xEnd-xStart+1,y2-y1+1,
                                        KOPrefs::instance()->mWorkingHoursColor);
                        }
                    }
                    ++gxStart;
                }
            }
        }
    /*
      int selectionX = KOGlobals::self()->reverseLayout() ?
      (mColumns - 1 - mSelectionCellX) * mGridSpacingX : 
      mSelectionCellX * mGridSpacingX;

      // Draw selection
      if ( ( cx + cw ) >= selectionX && cx <= ( selectionX + mGridSpacingX ) &&
      ( cy + ch ) >= mSelectionYTop && cy <= ( mSelectionYTop + mSelectionHeight ) ) {
      // TODO: paint only part within cx,cy,cw,ch
      p->fillRect( selectionX, mSelectionYTop, mGridSpacingX,
      mSelectionHeight, KOPrefs::instance()->mHighlightColor );
      }
    */
    // Draw vertical lines of grid
 
    int x = ((int)(cx/mGridSpacingX))*mGridSpacingX;
    if ( mGridSpacingX > 0 ) {
        while (x < cx + cw) {
            p->drawLine(x,cy,x,cy+ch);
            x+=mGridSpacingX;
        }
    }
    // Draw horizontal lines of grid
    int y = ((int)(cy/lGridSpacingY))*lGridSpacingY;
    if ( lGridSpacingY > 0 ) {
        while (y < cy + ch) {
            p->setPen( SolidLine );
            p->drawLine(cx,y,cx+cw,y);
            y+=lGridSpacingY;
            p->setPen( DotLine );
            p->drawLine(cx,y,cx+cw,y);
            y+=lGridSpacingY;
        } 
        p->setPen( SolidLine );
    }
    mPixPainter.end() ;
}

/*
  Convert srcollview contents coordinates to agenda grid coordinates.
*/
void KOAgenda::contentsToGrid (int x, int y, int& gx, int& gy)
{
  gx = KOGlobals::self()->reverseLayout() ? mColumns - 1 - x/mGridSpacingX :
                                                        x/mGridSpacingX;
  gy = y/mGridSpacingY;
}

/*
  Convert agenda grid coordinates to scrollview contents coordinates.
*/
void KOAgenda::gridToContents (int gx, int gy, int& x, int& y)
{
  x = KOGlobals::self()->reverseLayout() ? (mColumns - 1 - gx)*mGridSpacingX:
                                                         gx*mGridSpacingX;
  y = gy*mGridSpacingY;
}


/*
  Return Y coordinate corresponding to time. Coordinates are rounded to fit into
  the grid.
*/
int KOAgenda::timeToY(const QTime &time)
{
  int minutesPerCell = 24 * 60 / mRows;
  int timeMinutes = time.hour() * 60 + time.minute();
  int Y = (timeMinutes + (minutesPerCell / 2)) / minutesPerCell;
  return Y;
}


/*
  Return time corresponding to cell y coordinate. Coordinates are rounded to
  fit into the grid.
*/
QTime KOAgenda::gyToTime(int gy)
{

  int secondsPerCell = 24 * 60 * 60/ mRows;

  int timeSeconds = secondsPerCell * gy;

  QTime time( 0, 0, 0 );
  if ( timeSeconds < 24 * 60 * 60 ) {
    time = time.addSecs(timeSeconds);
  } else {
    time.setHMS( 23, 59, 59 );
  }

  return time;
}

void KOAgenda::setStartHour(int startHour)
{
  int startCell = startHour * mRows / 24;
  setContentsPos(0,startCell * gridSpacingY());
}
QTime KOAgenda::getEndTime()
{
    int tim = (contentsY ()+viewport()->height())*24/contentsHeight ();
    if ( tim > 23 )
        return QTime ( 23,59,59);
    return QTime ( tim,0,0);
}
void KOAgenda::hideUnused()
{
    // experimental only
    // return;
    KOAgendaItem *item;
    for ( item=mUnusedItems.first(); item != 0; item=mUnusedItems.next() ) {
        item->hide();
    }
}


KOAgendaItem *KOAgenda::getNewItem(Incidence * event,QDate qd, QWidget* view)
{

    KOAgendaItem *fi;
    for ( fi=mUnusedItems.first(); fi != 0; fi=mUnusedItems.next() ) {
        if ( fi->incidence() ==  event ) {
            mUnusedItems.remove();
            fi->init( event, qd );
            return fi;
        }
    }
    fi=mUnusedItems.first();
    if ( fi ) {
        mUnusedItems.remove();
        fi->init( event, qd );
        return fi;
    }
    // qDebug("new KOAgendaItem ");

    KOAgendaItem* agendaItem = new KOAgendaItem( event, qd, view, mAllDayMode );
    agendaItem->installEventFilter(this);
    addChild(agendaItem,0,0);
    return  agendaItem;
}
KOAgendaItem * KOAgenda::getItemForTodo ( Todo * todo )
{
    KOAgendaItem *item;
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
        if ( item->incidence() == todo ) {
            mItems.remove();
            return item;
        }
    }
    return 0;
}


void KOAgenda::updateTodo( Todo * todo, int days, bool remove) 
{ 
      // ( todo->hasCompletedDate() && todo->completed().date() == currentDate )||
    KOAgendaItem *item;
    item = getItemForTodo ( todo );
    //qDebug("KOAgenda::updateTodo %d %d %d %d", this, todo, days, remove);
    if ( item ) {
        blockSignals( true );
        //qDebug("item found ");
        item->hide();
        item->setCellX(-2, -1 );
        item->select(false);
        mUnusedItems.append( item );
        mItems.remove( item );
        QPtrList<KOAgendaItem> oldconflictItems = item->conflictItems();
        KOAgendaItem *itemit; 
        //globalFlagBlockAgendaItemPaint = 1;
        for ( itemit=oldconflictItems.first(); itemit != 0;
              itemit=oldconflictItems.next() ) {
            if ( itemit != item )
                placeSubCells(itemit); 
        }  
        qApp->processEvents();
        //globalFlagBlockAgendaItemPaint = 0;
        for ( itemit=oldconflictItems.first(); itemit != 0;
              itemit=oldconflictItems.next() ) {
            globalFlagBlockAgendaItemUpdate = 0;
            if ( itemit != item )
                itemit->repaintMe();
            globalFlagBlockAgendaItemUpdate = 1; 
            //qDebug("sigleshot ");
            QTimer::singleShot( 0, itemit, SLOT ( repaintItem() ));
            //itemit->repaint( false ); repaintItem()
        }
        blockSignals( false );
    }
    if ( remove ) {
        //qDebug("remove****************************************** ");
        return;
    }
    if ( todo->hasCompletedDate() && !KOPrefs::instance()->mShowCompletedTodoInAgenda )
        return;
    //qDebug("updateTodo+++++++++++++++++++++++++++++++++++++ ");
    QDate currentDate = QDate::currentDate();
    bool overdue = (!todo->isCompleted()) && (todo->dtDue() < currentDate)&&  ( KOPrefs::instance()->mShowTodoInAgenda );
    QDateTime dt;
    if ( todo->hasCompletedDate() )
        dt =  todo->completed();
    else
        dt = todo->dtDue();
    if ( overdue ) {
        days += todo->dtDue().date().daysTo( currentDate );
    }
    else
        currentDate = dt.date();
    
    if (( todo->doesFloat() || overdue) && !todo->hasCompletedDate() ) {  
        if ( ! mAllDayMode ) return;
        // aldayagenda
        globalFlagBlockAgendaItemPaint = 1;
        item = insertAllDayItem(todo, currentDate,days, days);
        item->show();
      
    }
    else {
        if (  mAllDayMode ) return;
        // mAgenda
        globalFlagBlockAgendaItemPaint = 1;
        int endY = timeToY(dt.time()) - 1; 
        int hi = 12/KOPrefs::instance()->mHourSize;
        int startY = endY - 1-hi;
        item = insertItem(todo,currentDate,days,startY,endY);
        item->show();
    }
    qApp->processEvents();
    globalFlagBlockAgendaItemPaint = 0; 
    QPtrList<KOAgendaItem> oldconflictItems = item->conflictItems();
    KOAgendaItem *itemit; 
    for ( itemit=oldconflictItems.first(); itemit != 0;
          itemit=oldconflictItems.next() ) {
        globalFlagBlockAgendaItemUpdate = 0;
        itemit->repaintMe();
        globalFlagBlockAgendaItemUpdate = 1;
        itemit->repaint(); 
    } 
    globalFlagBlockAgendaItemUpdate = 0;
    item->repaintMe();
    globalFlagBlockAgendaItemUpdate = 1;
    item->repaint();
}
/*
  Insert KOAgendaItem into agenda.
*/
KOAgendaItem *KOAgenda::insertItem (Incidence *event,QDate qd,int X,int YTop,int YBottom)
{
  if (mAllDayMode) {
      qDebug("KOAgenda: calling insertItem in all-day mode is illegal. ");
    return 0;
  }

  KOAgendaItem *agendaItem = getNewItem(event,qd,viewport());
  //agendaItem->setFrameStyle(WinPanel|Raised);

  int YSize = YBottom - YTop + 1;
  if (YSize < 0) {
    YSize = 1;
  }
  int iheight = mGridSpacingY * YSize;
 
  agendaItem->resize(mGridSpacingX,iheight );
  agendaItem->setCellXY(X,YTop,YBottom);
  agendaItem->setCellXWidth(X);

  //addChild(agendaItem,X*mGridSpacingX,YTop*mGridSpacingY);
  mItems.append(agendaItem);

  placeSubCells(agendaItem);

  //agendaItem->show();


  return agendaItem;
}


/*
  Insert all-day KOAgendaItem into agenda.
*/
KOAgendaItem *KOAgenda::insertAllDayItem (Incidence *event,QDate qd,int XBegin,int XEnd)
{
   if (!mAllDayMode) {
    return 0;
  }
   //qDebug("insertallday %s  -- %d  - %d ",qd.toString().latin1(), XBegin, XEnd );
  KOAgendaItem *agendaItem = getNewItem(event,qd,viewport());

  agendaItem->setCellXY(XBegin,0,0);
  agendaItem->setCellXWidth(XEnd);
  agendaItem->resizeMe(mGridSpacingX, mGridSpacingX * agendaItem->cellWidth(),mGridSpacingY, true );

  //addChild(agendaItem,XBegin*mGridSpacingX,0);
  mItems.append(agendaItem);

  placeSubCells(agendaItem);

  //agendaItem->show();

  return agendaItem;
}


void KOAgenda::insertMultiItem (Event *event,QDate qd,int XBegin,int XEnd,
                                int YTop,int YBottom)
{
    if (mAllDayMode) {
        ;
    return;
  }

  int cellX,cellYTop,cellYBottom;
  QString newtext;
  int width = XEnd - XBegin + 1;
  int count = 0;
  KOAgendaItem *current = 0;
  QPtrList<KOAgendaItem> multiItems;
  for (cellX = XBegin;cellX <= XEnd;++cellX) {
    if (cellX == XBegin) cellYTop = YTop;
    else cellYTop = 0;
    if (cellX == XEnd) cellYBottom = YBottom;
    else cellYBottom = rows() - 1;
    newtext = QString("(%1/%2): ").arg(++count).arg(width);
    newtext.append(event->summary());
    current = insertItem(event,qd,cellX,cellYTop,cellYBottom);
    current->setText(newtext);
    multiItems.append(current);
  }

  KOAgendaItem *next = 0;
  KOAgendaItem *last = multiItems.last();
  KOAgendaItem *first = multiItems.first();
  KOAgendaItem *setFirst,*setLast;
  current = first;
  while (current) {
    next = multiItems.next();
    if (current == first) setFirst = 0;
    else setFirst = first;
    if (current == last) setLast = 0;
    else setLast = last;

    current->setMultiItem(setFirst,next,setLast);
    current = next;
  }
}


//QSizePolicy KOAgenda::sizePolicy() const
//{
  // Thought this would make the all-day event agenda minimum size and the
  // normal agenda take the remaining space. But it doesn´t work. The QSplitter
  // don´t seem to think that an Expanding widget needs more space than a
  // Preferred one.
  // But it doesn´t hurt, so it stays.
//  if (mAllDayMode) {
//    return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
//  } else {
//    return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//  }
//}
void KOAgenda::finishResize ( )
{
    //qDebug("finishResize+++++++++++++++++++++++++++++++ ( ) ");
    if ( globalFlagBlockAgenda ==  0 ) {
        finishUpdate();
        //qDebug("finishUpdate() called ");
    }
}
/*
  Overridden from QScrollView to provide proper resizing of KOAgendaItems.
*/
void KOAgenda::resizeEvent ( QResizeEvent *ev )
{
    mSelectionHeight = 0;
    mResizeTimer.start( 150 , true );
    computeSizes(); 
    QScrollView::resizeEvent( ev );
    return;

}
void KOAgenda::computeSizes()
{   
    if ( globalFlagBlockStartup )
        return;
    int frameOffset = frameWidth() * 2 +1;
    if (mAllDayMode) {
        mGridSpacingX = (width()-frameOffset) / mColumns;
        mGridSpacingY = height() - 2 * frameWidth() - 1;
        resizeContents( mGridSpacingX * mColumns + 1 , mGridSpacingY + 1);
        //    mGridSpacingY = height();
        //    resizeContents( mGridSpacingX * mColumns + 1 , mGridSpacingY * mRows + 1 );

        KOAgendaItem *item;
        int subCellWidth;
        for ( item=mItems.first(); item != 0; item=mItems.next() ) {
            subCellWidth = mGridSpacingY / item->subCells();
            int diff = 0;
            diff = item->resizeMe(mGridSpacingX ,mGridSpacingX * item->cellWidth(),subCellWidth);
            moveChild(item,(KOGlobals::self()->reverseLayout() ?
                      (mColumns - 1 - item->cellX()) * mGridSpacingX :
                            item->cellX() * mGridSpacingX) + diff,
                      item->subCell() * subCellWidth);
        }
        KOPrefs::instance()->mAllDaySize = mGridSpacingY;
    } else {
        mGridSpacingX = (width() - verticalScrollBar()->width()-frameOffset)/mColumns;
        if (height() >   mGridSpacingY * mRows + 1 ) {
            KOPrefs::instance()->mHourSize = ((height())/mRows)+1;
            mGridSpacingY = KOPrefs::instance()->mHourSize ;
            resizeContents( mGridSpacingX * mColumns + 1 , mGridSpacingY * mRows + 1 );
            emit resizedSignal();
        } else
            resizeContents( mGridSpacingX * mColumns + 1 , mGridSpacingY * mRows + 1 );
        KOAgendaItem *item;
        int subCellWidth;
    
        for ( item=mItems.first(); item != 0; item=mItems.next() ) {
            subCellWidth = mGridSpacingX / item->subCells();
            item->resize(subCellWidth,item->height());
            moveChild(item,(KOGlobals::self()->reverseLayout() ?
                            (mColumns - 1 - item->cellX()) * mGridSpacingX :
                            item->cellX() * mGridSpacingX) +
                      item->subCell() * subCellWidth,childY(item));
        }
    }
    int cw = contentsWidth();
    int ch = contentsHeight();
    if ( mAllDayMode ) { 
        QPixmap* paintPixAll = KOAgendaItem::paintPixAllday();
        if ( (paintPixAll->width() < cw || paintPixAll->height() <  ch) && cw > 0  && ch > 0  ) {
            //qDebug("paintPixAll->resize ");
            paintPixAll->resize( cw, ch ); 
        }
    } else {
        QPixmap* paintPix = KOAgendaItem::paintPix();
        if ( paintPix->width() < cw || paintPix->height() <  ch ) {
            //qDebug("paintPix->resize ");
            paintPix->resize( cw , ch );
        }
    }

    checkScrollBoundaries();
    drawContentsToPainter();
    viewport()->repaint(false);
}

void KOAgenda::scrollUp()
{
    scrollBy(0,-mScrollOffset);
}


void KOAgenda::scrollDown()
{
    scrollBy(0,mScrollOffset);
}

void KOAgenda::popupAlarm()
{
    if (!mClickedItem) {
        qDebug("KOAgenda::popupAlarm() called without having a clicked item ");
        return;
    }
    // TODO: deal correctly with multiple alarms
    Alarm* alarm;
    QPtrList<Alarm> list(mClickedItem->incidence()->alarms());
    for(alarm=list.first();alarm;alarm=list.next()) {
        alarm->toggleAlarm();
    }
    emit itemModified( mClickedItem , KOGlobals::EVENTEDITED );
    mClickedItem->paintMe( true );
    mClickedItem->repaint( false );
}

/*
  Calculates the minimum width
*/
int KOAgenda::minimumWidth() const
{
  // TODO:: develop a way to dynamically determine the minimum width
  int min = 100;

  return min;
}

void KOAgenda::updateConfig()
{
   if ( viewport()->backgroundColor() != KOPrefs::instance()->mAgendaBgColor)
       viewport()->setBackgroundColor(KOPrefs::instance()->mAgendaBgColor);
  if ( mAllDayMode ) {
      mGridSpacingY = height() - 1 ;// KOPrefs::instance()->mAllDaySize;
      //mGridSpacingY = KOPrefs::instance()->mAllDaySize;
      resizeContents( mGridSpacingX * mColumns + 1 , mGridSpacingY+1 );
      // setMaximumHeight(  mGridSpacingY+1 );
      viewport()->repaint( false );
      //setFixedHeight( mGridSpacingY+1 );
      //qDebug("KOPrefs:aaaaa:instance()->mAllDaySize %d  ", KOPrefs::instance()->mAllDaySize);
  }
  else {
      mGridSpacingY = KOPrefs::instance()->mHourSize;
      calculateWorkingHours();
  }
}

void KOAgenda::checkScrollBoundaries()
{
  // Invalidate old values to force update
  mOldLowerScrollValue = -1;
  mOldUpperScrollValue = -1;

  checkScrollBoundaries(verticalScrollBar()->value());
}

void KOAgenda::checkScrollBoundaries(int v)
{
    if ( mGridSpacingY ==  0 )
        return;
  int yMin = v/mGridSpacingY;
  int yMax = (v+visibleHeight())/mGridSpacingY;

//  kdDebug() << "--- yMin: " << yMin << "  yMax: " << yMax << endl;

  if (yMin != mOldLowerScrollValue) {
    mOldLowerScrollValue = yMin;
    emit lowerYChanged(yMin);
  }
  if (yMax != mOldUpperScrollValue) {
    mOldUpperScrollValue = yMax;
    emit upperYChanged(yMax);
  }
}

void KOAgenda::deselectItem()
{
  if (mSelectedItem.isNull()) return;
  mSelectedItem->select(false);
  mSelectedItem = 0;
}

void KOAgenda::selectItem(KOAgendaItem *item)
{
  if ((KOAgendaItem *)mSelectedItem == item) return;
  deselectItem();
  if (item == 0) {
    emit incidenceSelected( 0 );
    return;
  }
  mSelectedItem = item;
  mSelectedItem->select();
  emit incidenceSelected( mSelectedItem->incidence() );
}

// This function seems never be called.
void KOAgenda::keyPressEvent( QKeyEvent *kev )
{
  switch(kev->key()) {
    case Key_PageDown:
      verticalScrollBar()->addPage();
      break;
    case Key_PageUp:
      verticalScrollBar()->subtractPage();
      break;
    case Key_Down:
      verticalScrollBar()->addLine();
      break;
    case Key_Up:
      verticalScrollBar()->subtractLine();
      break;
    default:
      ;
  }
}

void KOAgenda::calculateWorkingHours()
{
//  mWorkingHoursEnable = KOPrefs::instance()->mEnableWorkingHours;
  mWorkingHoursEnable = !mAllDayMode;

  mWorkingHoursYTop = mGridSpacingY *
                      KOPrefs::instance()->mWorkingHoursStart * 4;
  mWorkingHoursYBottom = mGridSpacingY *
                         KOPrefs::instance()->mWorkingHoursEnd * 4 - 1;
}


DateList KOAgenda::dateList() const
{
    return mSelectedDates;
}

void KOAgenda::setDateList(const DateList &selectedDates)
{
    mSelectedDates = selectedDates;
}

void KOAgenda::setHolidayMask(QMemArray<bool> *mask)
{
  mHolidayMask = mask;

/*
  kdDebug() << "HolidayMask: ";
  for(uint i=0;i<mask->count();++i) {
    kdDebug() << (mask->at(i) ? "*" : "o");
  }
  kdDebug() << endl;
*/
}

void KOAgenda::contentsMousePressEvent ( QMouseEvent *event )
{
 
  QScrollView::contentsMousePressEvent(event);
}

void KOAgenda::storePosition()
{
    //mContentPosition
    int max = mGridSpacingY*4*24; 
    if ( contentsY() < 5 && max > viewport()->height()*3/2 )
        mContentPosition = 0;
    else if (  contentsY() + viewport()->height() >  max - 5 && max > viewport()->height()*3/2)
        mContentPosition = -1.0;
    else
        mContentPosition = ((float) max)/ ((float)(contentsY()+ ( viewport()->height()/2)));
    //qDebug("mContentPosition %f %d %d %d",mContentPosition , max,   contentsY() ,viewport()->height());

}
void KOAgenda::restorePosition()
{
    int posY;
    int max = mGridSpacingY*4*24; 
    if ( mContentPosition < 0 ) 
        posY = max-viewport()->height();
    else
        if ( mContentPosition == 0 ) 
            posY = 0;
        else
            posY =  (int) ((max/mContentPosition)-(viewport()->height()/2));
    setContentsPos (0, posY );
    //qDebug("posY %d hei %d", posY, max);

}
void KOAgenda::moveChild( QWidget *w, int x , int y )
{
    ++x;
    QScrollView::moveChild( w, x , y );
}
#include <qmessagebox.h>
#ifdef DESKTOP_VERSION
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#endif
void KOAgenda::printSelection()
{
#ifdef DESKTOP_VERSION
    if ( mStartCellY == mCurrentCellY ) {
        QMessageBox::warning( this, i18n("KO/Pi: Warning!"),
                                           i18n("Nothing selected!\n\nThis prints the full width of the Agenda view as you see it!\n\nTo determine the vertical range of the printing, please select\na vertical range (with the left mouse button down) in one column. "),
                                           i18n("OK"), 0, 0,
                                           0, 1 );
        return;
    }
 
    float dx, dy; 
    int x,y,w,h;
    x= 0;  
    w= contentsWidth()+2;
    // h= contentsHeight();
    y = mGridSpacingY*mStartCellY;
    h =  mGridSpacingY*(mCurrentCellY+1)-y+2;

    //return;
    QPrinter* printer = new QPrinter(); 
    if ( !printer->setup()) {
        delete printer;
        return;
    }
    QPainter p( printer );
    QPaintDeviceMetrics m = QPaintDeviceMetrics ( printer ); 
    QString date = i18n("Date range: ")+KGlobal::locale()->formatDate( mSelectedDates.first()  )+" - "+KGlobal::locale()->formatDate( mSelectedDates.last()  );
    //date += " --- printing time: " + KGlobal::locale()->formatDateTime(QDateTime::currentDateTime(), true );
    int hei = p.boundingRect(0,0, 5, 5, Qt::AlignLeft, date ).height();
    // p.drawText( 0, 0, date );
    int offset = m.width()/8;
    // compute the scale
    dx = ((float) m.width()-offset)  / (float)w;
    dy  = (float)(m.height()  - ( 2 * hei )-offset ) / (float)h;
    float scale;
    // scale to fit the width or height of the paper
    if ( dx < dy )
        scale = dx;
    else
        scale = dy;
    // set the scale
    p.drawText( (int) (offset* scale), (int) (offset* scale*3/4), date );

    uint selDay;
    float widOffset = ((float) m.width()-offset) / ((float)(mSelectedDates.count()));
    float startX = 1;
    for ( selDay = 0; selDay < mSelectedDates.count();  ++selDay)
      {
	QString text = KGlobal::locale()->formatDate( mSelectedDates[selDay],true );
	p.setClipRect((int) (offset* scale+startX) , 0, (int) (widOffset-4), (int) (offset* scale+(2*hei* scale)) );
	p.drawText( (int) (offset* scale+startX), (int) ((offset+hei)* scale), text );
	startX += widOffset;
	
      }
    p.translate( (int) (offset* scale),(int) (offset* scale+ (-y * scale)+(2*hei* scale)));
    p.scale( scale, scale );
    p.setClipRect(  (int) (offset* scale), (int) (offset* scale+(2*hei* scale)), (int) (w*scale), (int) (h*scale) );
    // now printing with y offset:  2 hei
    // p.translate( 0, -y*scale);

    drawContentsToPainter(&p, true ); 
    globalFlagBlockAgendaItemUpdate = false;
    KOAgendaItem *item;
    for ( item=mItems.first(); item != 0; item=mItems.next() ) {
        item->select(false);
        item->paintMe( false, &p );
    }
    globalFlagBlockAgendaItemUpdate = true;
    p.end();
    delete printer;
#else
   int result = QMessageBox::warning( this, i18n("KO/Pi: Warning!"),
                                       i18n("Not supported \non  PDA!\n"),
                                       i18n("OK"), 0, 0,
                                       0, 1 );
#endif  
}
