/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qdragobject.h>
#include <qdrawutil.h> 
#include <qpainter.h> 

#include <kiconloader.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#define AGENDA_ICON_SIZE 5
#else
#include <qapplication.h>
#define AGENDA_ICON_SIZE 7
#endif
#include <libkcal/icaldrag.h>
#include <libkcal/vcaldrag.h>
#include <libkcal/kincidenceformatter.h>
extern int globalFlagBlockAgenda;
extern int globalFlagBlockAgendaItemPaint;
extern int globalFlagBlockAgendaItemUpdate;

#include "koprefs.h"

#include "koagendaitem.h"
//#include "koagendaitem.moc"


//--------------------------------------------------------------------------

QToolTipGroup *KOAgendaItem::mToolTipGroup = 0;

//--------------------------------------------------------------------------

class KOAgendaItemWhatsThis :public QWhatsThis
{
public:
  KOAgendaItemWhatsThis( KOAgendaItem* view ) : QWhatsThis( view ),_view (view) { };

protected:
  virtual QString text( const QPoint& )
  {
    return _view->getWhatsThisText() ;
  }
private:
  KOAgendaItem * _view;
};

KOAgendaItem::KOAgendaItem(Incidence *incidence, QDate qd, QWidget *parent,bool allday,
                           const char *name,WFlags) :
    QWidget(parent, name), mIncidence(incidence), mDate(qd)
{
#ifndef DESKTOP_VERSION
    //QPEApplication::setStylusOperation( this, QPEApplication::RightOnHold );
#endif
    mKOAgendaItemWhatsThis =  new KOAgendaItemWhatsThis(this);
    int wflags = getWFlags() |WRepaintNoErase;// WResizeNoErase
    setWFlags ( wflags);
    mAllDay = allday;
    init ( incidence, qd );
    //setMouseTracking(true);
    //setAcceptDrops(true);
    xPaintCoord = -1;
    yPaintCoord = -1;
}
QString KOAgendaItem::getWhatsThisText()
{
    if ( mIncidence )
        return KIncidenceFormatter::instance()->getFormattedText( mIncidence, 
                                                                  KOPrefs::instance()->mWTshowDetails,
                                                                  KOPrefs::instance()->mWTshowCreated,
                                                                  KOPrefs::instance()->mWTshowChanged);
    return "KOAgendaItem::getWhatsThisText()::internal error";
}

void KOAgendaItem::initColor ()
{
    if ( (mIncidence->typeID() == todoID ) &&
         ( !((static_cast<Todo*>(mIncidence))->isCompleted()) &&
           ((static_cast<Todo*>(mIncidence))->dtDue().date() <=  QDate::currentDate()) ) ) {
        if (  (static_cast<Todo*>(mIncidence))->dtDue() <  QDateTime::currentDateTime().date()) 
            mBackgroundColor = KOPrefs::instance()->mTodoOverdueColor ;
        else 
            mBackgroundColor = KOPrefs::instance()->mTodoDueTodayColor;
    }
    else {
        QStringList categories = mIncidence->categories();
        QString cat = categories.first();
        if (cat.isEmpty()) {
            if ( (mIncidence->typeID() == todoID ) &&((static_cast<Todo*>(mIncidence))->isCompleted()) )
                mBackgroundColor =KOPrefs::instance()->mTodoDoneColor;
            else
                mBackgroundColor =KOPrefs::instance()->defaultColor( mIncidence->calID() );
        } else {
            mBackgroundColor = *KOPrefs::instance()->categoryColor(cat);
            if ( (mIncidence->typeID() == todoID ) &&((static_cast<Todo*>(mIncidence))->isCompleted()) ) {
                if ( mBackgroundColor == KOPrefs::instance()->mEventColor )
                    mBackgroundColor = KOPrefs::instance()->mTodoDoneColor;
            }
        }
            
    }

    QColor BackgroundColor ( mBackgroundColor );
    if ( mIncidence->calID() > 1 ) {
        //BackgroundColor = KOPrefs::instance()->defaultColor( mIncidence->calID() );
    }
    mColorGroup  = QColorGroup( BackgroundColor.light(), 
                                BackgroundColor.dark(),BackgroundColor.light(),
                                BackgroundColor.dark(),BackgroundColor, black, BackgroundColor) ;
    setBackgroundColor( mBackgroundColor );
    mWhiteText = (mBackgroundColor.red() + mBackgroundColor.green() + mBackgroundColor.blue() < 250);
}
void KOAgendaItem::init ( Incidence *incidence, QDate qd )
{
    mIncidence = incidence;
    mDate = qd;
    mFirstMultiItem = 0;
    mNextMultiItem = 0;
    mLastMultiItem = 0;  
    computeText();
    initColor();
    mConflictItems.clear();
    setCellXY(0,0,1);
    setCellXWidth(0);
    setSubCell(0);
    setSubCells(1);
    setMultiItem(0,0,0);
    startMove();
    mSelected = true;
    select(false);
    QFontMetrics fontinf(KOPrefs::instance()->mAgendaViewFont);
    mFontPixelSize = fontinf.height();;
    hide();
    xPaintCoord = -1;
    yPaintCoord = -1;
}


KOAgendaItem::~KOAgendaItem()
{
#if QT_VERSION >= 0x030000

#else
    delete mKOAgendaItemWhatsThis;
#endif 
  
}

void KOAgendaItem::recreateIncidence()
{
#if 0
    Incidence* newInc = mIncidence->clone();
    newInc->recreate();
    if ( mIncidence->doesRecur() ) {
        mIncidence->addExDate( mDate );
        newInc->recurrence()->unsetRecurs();
        int len = mIncidence->dtStart().secsTo( ((Event*)mIncidence)->dtEnd());
        QTime tim = mIncidence->dtStart().time();
        newInc->setDtStart( QDateTime(mDate, tim) );
        ((Event*)newInc)->setDtEnd( newInc->dtStart().addSecs( len ) );
    }
#endif
    mIncidence = mIncidence->recreateCloneException( mDate );
}
bool KOAgendaItem::updateIcons(QPainter * p, bool horLayout)
{
    int size = AGENDA_ICON_SIZE;

    int yOff = 0;
    int xOff = 0;
    int x = pos().x();

    if ( x < 0 ) x = 0;
    x += 3; 
    int y;
    if ( mAllDay )
        y = pos().y()+3;
    else
        y =  mCellYTop * ( height() / cellHeight() ) +3;


    if ( mIncidence->calID() > 1 ) {
        p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, KOPrefs::instance()->defaultColor( mIncidence->calID() ) );
        p->drawRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x-1, yOff*( 1 +AGENDA_ICON_SIZE)+y-1, AGENDA_ICON_SIZE+2, AGENDA_ICON_SIZE+2 );
        if ( horLayout ){
            ++xOff;
            ++x;
        }
        else {
            ++yOff;
            ++y;
        }
    }
    if (mIncidence->cancelled() &&  height() < 20 ) {
        int xpos = xOff*( 1 +AGENDA_ICON_SIZE )+x;
        int ypos = yOff*( 1 +AGENDA_ICON_SIZE)+y;
        p->drawLine( xpos, ypos, xpos+AGENDA_ICON_SIZE-1, ypos+AGENDA_ICON_SIZE-1  );
        p->drawLine( xpos, ypos+AGENDA_ICON_SIZE-1, xpos+AGENDA_ICON_SIZE-1, ypos );
        if ( horLayout )
            ++xOff;
        else
            ++yOff;
    }
    if (mIncidence->isAlarmEnabled() && mIncidence->alarmEnabled()) {
        p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, red );
        if ( horLayout )
            ++xOff;
        else
            ++yOff;
    }
    if (mIncidence->doesRecur()) {
        p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, blue );
        if ( horLayout )
            ++xOff;
        else
            ++yOff;
    }
    if (mIncidence->description().length() > 0) {
        p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, darkGreen );
        if ( horLayout )
            ++xOff;
        else
            ++yOff;
    }
   if (mIncidence->isReadOnly()) {
        p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, white );
        if ( horLayout )
            ++xOff;
        else
            ++yOff;
    }

    if (mIncidence->attendeeCount()>0) {
        
        if (mIncidence->organizer() == KOPrefs::instance()->email()) {
            p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, black );
            if ( horLayout )
                ++xOff;
            else
                ++yOff;
        } else {
            Attendee *me = mIncidence->attendeeByMails(KOPrefs::instance()->mAdditionalMails,KOPrefs::instance()->email());
            if (me!=0) {


            } else {
                p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, yellow  );
                if ( horLayout )
                    ++xOff;
                else
                    ++yOff;

            }
            p->fillRect ( xOff*( 1 +AGENDA_ICON_SIZE )+x, yOff*( 1 +AGENDA_ICON_SIZE)+y, AGENDA_ICON_SIZE, AGENDA_ICON_SIZE, darkYellow );
            if ( horLayout )
                ++xOff;
            else
                ++yOff;

        }

    }
  return ( yOff || xOff );
}


void KOAgendaItem::select(bool selected)
{
    //qDebug("select %d %d",firstMultiItem(), nextMultiItem() );
    if (mSelected == selected) return;
    mSelected = selected;
    if ( ! isVisible() )
        return;
    if ( firstMultiItem() )
        firstMultiItem()->select( selected );
    if ( !firstMultiItem() && nextMultiItem() ) {
        KOAgendaItem *  placeItem = nextMultiItem();
        while ( placeItem ) { 
            placeItem->select( selected );
            placeItem = placeItem->nextMultiItem();
        }  
    }
    globalFlagBlockAgendaItemUpdate = 0;
    paintMe( selected );
    globalFlagBlockAgendaItemUpdate = 1;
    repaint( false );
}


/*
  The eventFilter has to filter the mouse events of the agenda item childs. The
  events are fed into the event handling method of KOAgendaItem. This allows the
  KOAgenda to handle the KOAgendaItems by using an eventFilter.
*/
bool KOAgendaItem::eventFilter ( QObject *object, QEvent *e )
{
    if (e->type() == QEvent::MouseButtonPress ||
        e->type() == QEvent::MouseButtonDblClick ||
        e->type() == QEvent::MouseButtonRelease ||
        e->type() == QEvent::MouseMove) {
        QMouseEvent *me = (QMouseEvent *)e;
        QPoint itemPos = this->mapFromGlobal(((QWidget *)object)->
                                             mapToGlobal(me->pos()));
        QMouseEvent returnEvent (e->type(),itemPos,me->button(),me->state());
        return event(&returnEvent);
    } else {
        return false;
    }
}
void KOAgendaItem::repaintMe( )
{ 
    paintMe ( mSelected );
}
void KOAgendaItem::paintMe( bool selected, QPainter* paint )
{    
    if ( globalFlagBlockAgendaItemUpdate && ! selected)
      return;
    QPainter pa;
    
    if ( mSelected ) {
        pa.begin( this );
    } else {
        if ( mAllDay )
            pa.begin( paintPixAllday() );
        else
            pa.begin( paintPix() );
    }
    int x, yy, w,  h;
    float nfh = 7.0;
    x = pos().x(); w = width(); h = height ();
    if ( mAllDay )
        yy = y();
    else
        yy = mCellYTop * ( height() / cellHeight() ); 
    if ( mSelected ) {
        pa.translate( -x, -yy );
    } 
    xPaintCoord= x;
    yPaintCoord = yy;
    wPaintCoord = width();
    hPaintCoord = height();
    //qDebug("paintMe %s %d %d %d %d",incidence()->summary().latin1(), x, yy, width(), height());
    if ( paint == 0 )
        paint = &pa;
    bool horLayout = ( w < h );
    int maxhei = mFontPixelSize+4;
    if (  horLayout )
        maxhei +=  AGENDA_ICON_SIZE -4;
    bool small = ( h < maxhei  );
    if ( ! small ) 
        paint->setFont(KOPrefs::instance()->mAgendaViewFont);
    else {
        QFont f =  KOPrefs::instance()->mAgendaViewFont;
        f.setBold( false );
        int fh = f.pointSize();
        nfh = (((float)height())/(float)(mFontPixelSize+4))*fh;
        if ( nfh < 6 )
            nfh = 6;
        f.setPointSize( nfh );
        paint->setFont(f);
    }
    paint->fillRect ( x, yy,  w, h, mBackgroundColor );
    static const  QPixmap completedPxmp = SmallIcon("greenhook16");
    static const QPixmap overduePxmp = SmallIcon("redcross16");
    if ( mIncidence->typeID() == todoID ) {
        Todo* tempTodo = static_cast<Todo*>(mIncidence);
        int xx = pos().x()+(width()-completedPxmp.width()-3 );
        int yyy =  yy+3;
        if ( tempTodo->isCompleted() )
            paint->drawPixmap ( xx, yyy, completedPxmp );
        else {
            paint->drawPixmap ( xx, yyy, overduePxmp );
            
        }
    }
    bool addIcon = false;
    if ( ! small || w > 3 * h || h > 3* w )
       addIcon =  updateIcons(  paint, horLayout );

    //qDrawShadePanel (paint, x,  yy,  w,  h, mColorGroup, selected , 2, 0);
    qDrawWinPanel (paint, x,  yy,  w,  h, mColorGroup, selected ,0);
    //qDebug("draw rect %d %d %d %d ",x,  yy,  w,  h );
    if ( ! small ) {
        x += 3; yy += 3;w -= 6;  h-= 5;
    } else {
        x += 2; yy += 1;w -= 4;  h-= 4; 
        if ( nfh < 6.01 ) {
            yy -= 2;
            h += 4;
        }
        else
            if ( nfh < h -2 )
                ++yy;
    }
    int align;
#ifndef DESKTOP_VERSION
    align = ( AlignLeft|WordBreak|AlignTop);
#else
    align = ( AlignLeft|BreakAnywhere|WordBreak|AlignTop);
#endif
    if (  addIcon  ) {
        if ( ! horLayout ) {
            x += AGENDA_ICON_SIZE+3;
            w -= (AGENDA_ICON_SIZE+3);
        }
        else {
            yy+= AGENDA_ICON_SIZE+2;
            h -=(AGENDA_ICON_SIZE+3);
        }
    } 
    if ( mWhiteText )
        paint->setPen ( white); 
    if ( x < 0 ) {
        w = w+x-3;
        x = 3;
        if ( !horLayout && addIcon )
            x += AGENDA_ICON_SIZE+3;
        if ( w > parentWidget()->width() ){
            w = parentWidget()->width() - 6;
#ifndef DESKTOP_VERSION
    align = ( AlignHCenter|WordBreak|AlignTop);
#else
    align = ( AlignHCenter|BreakAnywhere|WordBreak|AlignTop);
#endif

        }
    }
    QRect dr;
    if ( w + x > parentWidget()->width() )
        w = parentWidget()->width()-x;
     paint->drawText ( x, yy, w, h, align, mDisplayedText, -1, &dr );
     //qDebug("%d %d %d %d ",   x, yy, w, h );
     if ( mIncidence->cancelled() ){


         small = ( height() < 20  );
        
         if ( ! small ) {
             QFontMetrics fm ( paint->font() );
             paint->drawLine(dr.left(), yy+fm.height()/2, dr.right()-2, yy+fm.height()/2);
         }

     }
    pa.end();
 
}

QPixmap * KOAgendaItem::paintPix()
{
    static QPixmap* mPaintPix = 0;
    if ( ! mPaintPix ) {
        int w = QApplication::desktop()->width();
        int h = QApplication::desktop()->height();
        mPaintPix = new QPixmap(w,h);
    }
    return mPaintPix ; 
}
QPixmap * KOAgendaItem::paintPixAllday()
{
    static QPixmap* mPaintPixA = 0;
    if ( ! mPaintPixA ) {
        int w = QApplication::desktop()->width();
        int h = QApplication::desktop()->height()/5;
        mPaintPixA = new QPixmap(w,h);
    }
    return mPaintPixA ; 
}

void KOAgendaItem::repaintItem()
{ 
    globalFlagBlockAgendaItemPaint = 0;
    globalFlagBlockAgenda = 0;
    repaint( false );
}
void KOAgendaItem::paintEvent ( QPaintEvent *e )
{  
  
    if ( globalFlagBlockAgendaItemPaint )
        return; 
    if ( globalFlagBlockAgenda > 0 &&  globalFlagBlockAgenda < 5 )
      return;
    int yy;
    if ( mAllDay )
        yy = y();
    else
        yy = mCellYTop * ( height() / cellHeight() );
    int xx = x();
  
    if ( xPaintCoord != xx || yPaintCoord != yy || 
         wPaintCoord != width() ||  hPaintCoord != height()) {
        xPaintCoord= xx;
        yPaintCoord = yy;
        wPaintCoord = width();
        hPaintCoord = height();
        globalFlagBlockAgendaItemUpdate = 0;
        paintMe( mSelected );
        //qDebug("calling paintMe ");
        globalFlagBlockAgendaItemUpdate = 1;
        if ( mSelected )
            return;
    }
    int rx, ry, rw, rh;
    rx =  e->rect().x();
    ry =  e->rect().y();
    rw =  e->rect().width();
    rh =  e->rect().height();
    //qDebug("   paintevent %s %d %d    %d  %d", mIncidence->summary().latin1(), x(), yy, width(), height()); 

    QPixmap* paintFrom ;
    if ( mSelected ) {
        paintMe( mSelected );
        return;
    } else {
        if ( mAllDay )
            paintFrom =  paintPixAllday();
        else
            paintFrom =  paintPix();
    }
    xx += rx;

    if ( xx < 0 ) {
        rw = rw + xx;
        rx -= xx;
        xx = 0;
        if ( rw <= 1  ) {
            //qDebug("KOAgendaItem::Width1 <= 1 (%d). Returning. %s",rw,mDisplayedText.latin1());
            return;
        }
    }
    if ( paintFrom->width() <   xx+rw ) {
        rw =  paintFrom->width() - xx;
        if ( rw <= 1 ) {
            //qDebug("KOAgendaItem::Width2 <= 1 (%d). Returning.%s ",rw,mDisplayedText.latin1() );
            return;
        }
    }
    //qDebug("%d %d %d %d %d  %d %d",rx, ry, paintFrom, xx ,yPaintCoord+ry, rw, rh);
    bitBlt (this, rx, ry, paintFrom, xx ,yPaintCoord+ry, rw, rh ,CopyROP); 
}
void KOAgendaItem::computeText()
{
    mDisplayedText =  mIncidence->summary();
    if ( (mIncidence->typeID() == todoID ) ) {
        if ( static_cast<Todo*>(mIncidence)->hasDueDate() ) {
            if ( static_cast<Todo*>(mIncidence)->dtDue().date() < QDate::currentDate() )
                mDisplayedText += i18n(" (") +KGlobal::locale()->formatDate((static_cast<Todo*>(mIncidence))->dtDue().date(), true)+")";
            else if ( !(mIncidence->doesFloat()))
                mDisplayedText += i18n(" (") +KGlobal::locale()->formatTime((static_cast<Todo*>(mIncidence))->dtDue().time())+")";
        } 
    } else {
        if ( !(mIncidence->doesFloat()) &&  KOPrefs::instance()->mShowTimeInAgenda)
            mDisplayedText += ": " +KGlobal::locale()->formatTime((static_cast<Event*>(mIncidence))->dtStart().time()) + " - " + KGlobal::locale()->formatTime((static_cast<Event*>(mIncidence))->dtEnd().time()) ;

        if (  mAllDay ) {
            if ( (static_cast<Event*>(mIncidence))->isMultiDay() )  { 
                int dur = 1+ (static_cast<Event*>(mIncidence))->dtStart().date().daysTo( (static_cast<Event*>(mIncidence))->dtEnd().date() );
                mDisplayedText +=  " ("+QString::number( dur )+i18n(" days") +")" ;

                if ( mIncidence->dtStart().date().addDays(3) < mIncidence->dtEnd().date() ) {  
                    if ( mIncidence->doesRecur() ) {
                        mDisplayedText += " (" + mIncidence->recurrence()->recurrenceText() + ")";
                    } else {
                        mDisplayedText += ": " +KGlobal::locale()->formatDate((static_cast<Event*>(mIncidence))->dtStart().date(), true) + " - " + KGlobal::locale()->formatDate((static_cast<Event*>(mIncidence))->dtEnd().date(), true);
                    }
                } 
            } 
        }
    }
    if ( !mIncidence->location().isEmpty() ) {
        if ( mAllDay )
            mDisplayedText += " (";
        else
            mDisplayedText += "\n(";
        mDisplayedText += mIncidence->location() +")";
    }
#ifdef DESKTOP_VERSION
    QString tipText = mIncidence->summary();
    if ( !mIncidence->doesFloat() ) {
        if ( mIncidence->typeID() == eventID ) {
            if ( (static_cast<Event*>(mIncidence))->isMultiDay() ) {
                tipText += "\n"+i18n("From: ")+mIncidence->dtStartStr();
                tipText += "\n"+i18n("To: ")+(static_cast<Event*>(mIncidence))->dtEndStr();
            }
            else {
                tipText += "\n"+i18n("Time: ")+mIncidence->dtStartTimeStr();
                tipText += " - "+(static_cast<Event*>(mIncidence))->dtEndTimeStr();
            }
        }
        else if ( mIncidence->typeID() == todoID ) {
            if (mIncidence->hasStartDate())
                tipText += "\n"+i18n("Start: ")+ (static_cast<Todo*>(mIncidence))->dtStartStr();
            if (((Todo*)mIncidence)->hasDueDate())
                tipText += "\n"+i18n("Due: ")+ (static_cast<Todo*>(mIncidence))->dtDueStr();
        }
    } else if ( mIncidence->typeID() == todoID ) {
            if (mIncidence->hasStartDate())
                tipText += "\n"+i18n("Start: ")+ (static_cast<Todo*>(mIncidence))->dtStartDateStr();
            if (((Todo*)mIncidence)->hasDueDate())
                tipText += "\n"+i18n("Due: ")+ (static_cast<Todo*>(mIncidence))->dtDueDateStr();
        }

    if (!mIncidence->location().isEmpty()) {
        tipText += "\n"+i18n("Location: ")+mIncidence->location();
    }
    QToolTip::add(this,tipText,toolTipGroup(),""); 
#endif
}
void KOAgendaItem::updateItem()
{  
    computeText();
     
    //qDebug("KOAgendaItem:: updateItem() %s %d %d ",incidence()->summary().latin1(), x(), y());
    paintMe( mSelected );
    repaint( false);
}
  
void KOAgendaItem::resizeEvent ( QResizeEvent *ev )
{
    //qDebug("KOAgendaItem::resizeEvent %s ",  mIncidence->summary().latin1());
    paintMe( mSelected );
    repaint( false );
} 

/*
  Return height of item in units of agenda cells
*/
int KOAgendaItem::cellHeight()
{
    int ret = mCellYBottom - mCellYTop + 1;
    if ( ret <= 0 ) {
        ret = 1;
        mCellYBottom = 0;
        mCellYTop = 0;
    }
    return ret;
}
// it may be that allday agenda items have a needed width > 32000
// this code is to fix this problem
int KOAgendaItem::resizeMe( int grid, int wid, int hei, bool invalidWidth )
{
    int diff = 0;
    if ( mCellX < -3  && mAllDay  ) {
        diff = (mCellX + 3) * -grid;
        //qDebug("%s: cellX %d diff %d wid %d grid %d ", mDisplayedText.latin1(),  mCellX, diff, wid, grid); 
        if ( diff >= wid ) {
            // qDebug("KOAgendaItem::resizeMe: diff >= wid: diff %d wid %d ", diff, wid);
            //diff = 0;
        }
    }
    if ( (!invalidWidth && wid == width() ) || diff >= wid  )
        resize( wid, hei );
    else
        resize( wid - diff, hei );
    //qDebug("wid %d x %d ", width(), x());
    return diff;
}
/*
  Return height of item in units of agenda cells
*/
int KOAgendaItem::cellWidth()
{
    return mCellXWidth - mCellX + 1;
}

void KOAgendaItem::setItemDate(QDate qd)
{
  mDate = qd;
}

void KOAgendaItem::setCellXY(int X, int YTop, int YBottom)
{
  mCellX = X;
  mCellYTop = YTop;
  mCellYBottom = YBottom;
}

void KOAgendaItem::setCellXWidth(int xwidth)
{
  mCellXWidth = xwidth;
}

void KOAgendaItem::setCellX(int XLeft, int XRight)
{
  mCellX = XLeft;
  mCellXWidth = XRight;
}

void KOAgendaItem::setCellY(int YTop, int YBottom)
{
  mCellYTop = YTop;
  mCellYBottom = YBottom;
}

void KOAgendaItem::setSubCell(int subCell)
{
  mSubCell = subCell;
}

void KOAgendaItem::setSubCells(int subCells)
{
  mSubCells = subCells;
}

void KOAgendaItem::setMultiItem(KOAgendaItem *first,KOAgendaItem *next,
                                KOAgendaItem *last)
{
  mFirstMultiItem = first;
  mNextMultiItem = next;
  mLastMultiItem = last;
}

void KOAgendaItem::startMove()
{
  mStartCellX = mCellX;
  mStartCellXWidth = mCellXWidth;
  mStartCellYTop = mCellYTop;
  mStartCellYBottom = mCellYBottom;
}

void KOAgendaItem::resetMove()
{
  mCellX = mStartCellX;
  mCellXWidth = mStartCellXWidth;
  mCellYTop = mStartCellYTop;
  mCellYBottom = mStartCellYBottom;
}

void KOAgendaItem::moveRelative(int dx, int dy)
{
  int newX = cellX() + dx;
  int newXWidth = cellXWidth() + dx;
  int newYTop = cellYTop() + dy;
  int newYBottom = cellYBottom() + dy;
  setCellXY(newX,newYTop,newYBottom);
  setCellXWidth(newXWidth);
}

void KOAgendaItem::expandTop(int dy)
{
  int newYTop = cellYTop() + dy;
  int newYBottom = cellYBottom();
  if (newYTop > newYBottom) newYTop = newYBottom;
  setCellY(newYTop, newYBottom);
}

void KOAgendaItem::expandBottom(int dy)
{
  int newYTop = cellYTop();
  int newYBottom = cellYBottom() + dy;
  if (newYBottom < newYTop) newYBottom = newYTop;
  setCellY(newYTop, newYBottom);
}

void KOAgendaItem::expandLeft(int dx)
{
  int newX = cellX() + dx;
  int newXWidth = cellXWidth();
  if (newX > newXWidth) newX = newXWidth;
  setCellX(newX,newXWidth);
}

void KOAgendaItem::expandRight(int dx)
{
  int newX = cellX();
  int newXWidth = cellXWidth() + dx;
  if (newXWidth < newX) newXWidth = newX;
  setCellX(newX,newXWidth);
}

QToolTipGroup *KOAgendaItem::toolTipGroup()
{
  if (!mToolTipGroup) mToolTipGroup = new QToolTipGroup(0);
  return mToolTipGroup;
}

void KOAgendaItem::dragEnterEvent( QDragEnterEvent *e )
{
#ifndef KORG_NODND
  if ( ICalDrag::canDecode( e ) || VCalDrag::canDecode( e ) ||
       !QTextDrag::canDecode( e ) ) {
    e->ignore();
    return;
  }
  e->accept();
#endif
}

void KOAgendaItem::dropEvent( QDropEvent *e )
{
#ifndef KORG_NODND
  QString text;
  if(QTextDrag::decode(e,text))
  {
    kdDebug() << "Dropped : " << text << endl;
    QStringList emails = QStringList::split(",",text);
    for(QStringList::ConstIterator it = emails.begin();it!=emails.end();++it) {
      kdDebug() << " Email: " << (*it) << endl;
      int pos = (*it).find("<");
      QString name = (*it).left(pos);
      QString email = (*it).mid(pos);
      if (!email.isEmpty()) {
        mIncidence->addAttendee(new Attendee(name,email));
      }
    }
  }
#endif
}


QPtrList<KOAgendaItem> KOAgendaItem::conflictItems()
{
  return mConflictItems;
}

void KOAgendaItem::setConflictItems(QPtrList<KOAgendaItem> ci)
{
  mConflictItems = ci;
  KOAgendaItem *item;
  for ( item=mConflictItems.first(); item != 0;
        item=mConflictItems.next() ) {
    item->addConflictItem(this);
  }
}

void KOAgendaItem::addConflictItem(KOAgendaItem *ci)
{
  if (mConflictItems.find(ci)<0)
    mConflictItems.append(ci);
}

bool KOAgendaItem::checkLayout()
{
    if ( !mConflictItems.count() )
        return true;
    int max = 0;
    KOAgendaItem *item;
    for ( item=mConflictItems.first(); item != 0;
          item=mConflictItems.next() ) {
        if ( item->subCells() > max )
            max = item->subCells();
    }
    if ( max > subCells() ) {
        setSubCells( max );
        return false;
    }
    return true;
}
