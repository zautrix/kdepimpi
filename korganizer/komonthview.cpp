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

#include <qpopupmenu.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qkeycode.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qwidgetstack.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kiconloader.h>

#include <kcalendarsystem.h>

#ifndef KORG_NOPRINTER
#include "calprinter.h"
#endif
#include "koprefs.h"
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif
#include "koglobals.h"
#include <libkcal/kincidenceformatter.h>

#include "komonthview.h"

#define PIXMAP_SIZE 5
#ifdef DESKTOP_VERSION
QToolTipGroup *MonthViewCell::mToolTipGroup = 0; 
#endif
class KNOWhatsThis :public QWhatsThis
{
public:
    KNOWhatsThis( KNoScrollListBox* sbox ) : QWhatsThis( sbox ), _wid( sbox) { };
    //~KNOWhatsThis( ) {qDebug("~KNOWhatsThis( ) "); };

protected:
    virtual QString text( const QPoint& p)
    {
        return _wid->getWhatsThisText(p) ;
    };
private:
    KNoScrollListBox* _wid;
  
};
 

KNoScrollListBox::KNoScrollListBox(QWidget *parent,const char *name)
    : QListBox(parent, name, WRepaintNoErase)
{
#ifndef DESKTOP_VERSION
    QPEApplication::setStylusOperation( viewport(), QPEApplication::RightOnHold );
#endif 
    mBlockDeselect = false;
    mWT = new KNOWhatsThis(this);
    resetOnFocusIn = true;
    setVScrollBarMode(QScrollView::AlwaysOff);
    setHScrollBarMode(QScrollView::AlwaysOff);
}
KNoScrollListBox::~KNoScrollListBox()
{
#if QT_VERSION >= 0x030000

#else
    delete mWT;
#endif 
}


void KNoScrollListBox::focusInEvent ( QFocusEvent * e )
{
    QListBox::focusInEvent ( e );
    if ( count() ){
        int ci = currentItem();
        if ( ci < 0 ) ci = 0;
    
        setCurrentItem( ci );
        setSelected ( ci, true ); 
        emit highlighted( item ( ci ) );
 
        resetOnFocusIn = true;

        if (  KOPrefs::instance()->mEnableMonthScroll || KOPrefs::instance()->mMonthViewWeek ) {
            QListBoxItem *fi =  firstItem ();
            if (fi ) {
                int ihei = fi->height( this );
                int hei = numRows ()  * ihei;
                if ( hei < height() - horizontalScrollBar()->height () ) {
                    setVScrollBarMode(QScrollView::AlwaysOff);
                }
                else 
                    setVScrollBarMode(QScrollView::Auto);
                if ( ihei *3 > height() ) {
                    setHScrollBarMode(QScrollView::AlwaysOff);
                }
                else {
                    setHScrollBarMode(QScrollView::Auto);
                }
            } else {
                setVScrollBarMode(QScrollView::Auto);
                setHScrollBarMode(QScrollView::Auto);
            }
        } 
    }
}
void KNoScrollListBox::focusOutEvent ( QFocusEvent * e )
{
  if ( ! mBlockDeselect ) {
    int i =  currentItem ();
    if ( i >= 0 ) {
      setSelected ( i, false ); 
    }
    QListBox::focusOutEvent ( e ); 
  }
    setVScrollBarMode(QScrollView::AlwaysOff);
    setHScrollBarMode(QScrollView::AlwaysOff);
    if ( ! mBlockDeselect )
      emit highlightIncidence( 0, (MonthViewCell*)this, 0 );
    mBlockDeselect = false;
}

QString KNoScrollListBox::getWhatsThisText(QPoint p) 
{
    QListBoxItem* item = itemAt ( p );
    if ( ! item ) {
        return i18n("Click in the cell\nto add an event!");
    }
    return KIncidenceFormatter::instance()->getFormattedText(((MonthViewItem*) item)->incidence(),
                                                                  KOPrefs::instance()->mWTshowDetails,
                                                                  KOPrefs::instance()->mWTshowCreated,
                                                                  KOPrefs::instance()->mWTshowChanged);
}
void KNoScrollListBox::keyPressEvent(QKeyEvent *e)
{
    //qDebug("KNoScrollListBox::keyPressEvent ");
    switch(e->key()) {
    case Key_Right:
        if ( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton  )
        {
            e->ignore();
            return;
        }
        scrollBy(10,0);
        break;
    case Key_Left:
        if (e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton )
        {
            e->ignore();
            return;
        }
        scrollBy(-10,0);
        break;
    case Key_Up:
        if( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton)  {
            e->ignore();
            break;
        }
        if ( count() ) { 
            if ( currentItem() == 0 ) {
                emit prevCell();
            } else {
                setCurrentItem((currentItem()+count()-1)%count());
                if(!itemVisible(currentItem())) {
                    if((unsigned int) currentItem() == (count()-1)) {
                        setTopItem(currentItem()-numItemsVisible()+1);
                    } else {
                        setTopItem(topItem()-1);
                    }
                }
            }
        }
        break;
    case Key_Down:
        if(e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton)  {
            e->ignore();
            break;
        }
        if ( count () ) { 
            if ( ((uint)currentItem()+1) == count () ) {
                emit nextCell();
            } else {
                setCurrentItem((currentItem()+1)%count());
                if(!itemVisible(currentItem())) {
                    if(currentItem() == 0) {
                        setTopItem(0);
                    } else {
                        setTopItem(topItem()+1);
                    }
                }
            }
        }
        break;
    case Key_I:
        QTimer::singleShot( 1, this, SLOT ( oneDown() ) );
        e->ignore();
        break; 
    case Key_Return:
    case Key_Enter:
        {
            if ( currentItem() >= 0 ) {
                emit doubleClicked( item( currentItem() ) );
                e->accept();
            } else {
                e->ignore();
            }
        }
        break;
    case Key_Shift:
        emit shiftDown();
        break;
    default:
        e->ignore();
        break;
    }
}

void KNoScrollListBox::oneDown()
{
    if ( count () ) {  
        if ( ((uint)currentItem()+1) == count () ) {
            emit nextCell();
        } else {
            resetOnFocusIn = false;
            setCurrentItem((currentItem()+1)%count());
            if(!itemVisible(currentItem())) {
                if(currentItem() == 0) {
                    setTopItem(0);
                } else {
                    setTopItem(topItem()+1);
                }
            }
        }
    }
}
void KNoScrollListBox::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key()) {
    case Key_Shift:
        emit shiftUp();
        break;
    default:
        break;
    }
}

void KNoScrollListBox::mousePressEvent(QMouseEvent *e)
{
    QListBox::mousePressEvent(e);

    if(e->button() == RightButton) {
        emit rightClick();
    }
}

MonthViewItem::MonthViewItem( Incidence *incidence, const QString & s)
    : QListBoxItem()
{
    mblockRepaint = true;
    isWeekItem = KOPrefs::instance()->mMonthViewWeek;
    recycle( incidence, s );
}
void MonthViewItem::recycle( Incidence *incidence,  const QString & s)
{ 
    mDisplayHighlighted = false;
    setText( s );
    mMultiday = 0; 
    mIncidence = incidence;
    mRecur = false;
    mAlarm = false;
    mReply = false;
    mInfo = false; 
    mdayPos = 0;
}

bool MonthViewItem::setHighlightedFalse()
{
    if ( !mDisplayHighlighted )
        return false;
    mDisplayHighlighted = false;
    return true;
}

bool MonthViewItem::setHighlighted( Incidence * inc )
{
    if ( inc == mIncidence ) {
        if ( mDisplayHighlighted )
            return false;
        mDisplayHighlighted = true;
        return true;
    } else {
        if ( !mDisplayHighlighted )
            return false;
        mDisplayHighlighted = false;
        return true;
    }
    return false;
}
void MonthViewItem::paint(QPainter *p)
{
    if ( mblockRepaint || !mIncidence ) {
        return;
    }
#if QT_VERSION >= 0x030000
    bool sel = isSelected();
#else
    bool sel = selected();
#endif 
    int heihei = height( listBox () );
    int x = 1;
    if (KOPrefs::instance()->mMonthViewUsesCategoryColor || mDisplayHighlighted || sel )
        {
           
            p->setBackgroundColor( palette().color( QPalette::Normal,  \
                                                    sel ||mDisplayHighlighted  ? QColorGroup::Highlight : QColorGroup::Background ) );
            p->eraseRect( 0, 0, listBox()->maxItemWidth(), heihei );
        }
   
    //int y = 3;//(height() - mRecurPixmap.height()) /2; 
    int size = PIXMAP_SIZE;
    if ( QApplication::desktop()->width() < 300 )
        size = 3;
    int y = (heihei - size -1 ) /2; 

    if ( mIncidence->calID() > 1 ) {
        p->fillRect ( x, y-2,size,size+4, KOPrefs::instance()->defaultColor( mIncidence->calID() ) );
        p->drawRect ( x, y-2,size,size+4);
        x += size + 1;
    }
    if ( KOPrefs::instance()->mMonthShowIcons ) {
        if ( mInfo ) { 
            p->fillRect ( x, y,size,size, Qt::darkGreen );
            x += size + 1;
        }
        if ( mRecur ) {
            p->fillRect ( x, y,size,size, Qt::blue );
            x += size + 1;
        }
        if ( mAlarm ) {
            p->fillRect ( x, y,size,size, Qt::red );
            x += size + 1;
        }
        if ( mReply ) {
            p->fillRect ( x, y,size,size, Qt::yellow );
            x += size + 1;
        }
    }


   

    if ( sel ) p->setPen( Qt::white );
    else p->setPen( palette().color( QPalette::Normal,QColorGroup::Foreground ) ); 

#if 0
        p->setPen( palette().color( QPalette::Normal, sel ?                 \
                               QColorGroup::HighlightedText : QColorGroup::Foreground ) ); 
#endif
    QColor textColor = p->pen().color();


    if ( mMultiday ) { 
        int yyy = y+(size/2);
        int sizeM = size+2;
        p->setBrush( QBrush( textColor ) );
        p->drawLine ( x+1, yyy, x +sizeM +sizeM/2-1, yyy ) ;
        if ( mMultiday == 2 || mMultiday == 3 ) { 
            QPointArray pa ( 3 );
            pa.setPoint (0,  x, yyy );
            pa.setPoint (1,  x+sizeM/2, yyy+sizeM/2 );
            pa.setPoint (2,  x+sizeM/2, yyy-sizeM/2 );
            p->drawPolygon( pa );
        }
        if ( mMultiday == 2 || mMultiday == 1 ) { 
            QPointArray pa ( 3 );
            pa.setPoint (0,  x+sizeM +sizeM/2, yyy );
            pa.setPoint (1,  x+sizeM, yyy+sizeM/2 );
            pa.setPoint (2,  x+sizeM, yyy-sizeM/2 );
            p->drawPolygon( pa );
        }
        if ( mMultiday == 1 ) {  
            // p->fillRect (  x, yyy-sizeM/2+1, sizeM/2, size, QBrush ( QBrush::SolidPattern   ) );
            p->drawLine (  x+1, yyy-sizeM/2,  x+1, yyy+sizeM/2 );
        }
        if (  mMultiday == 3 ) { 
            // p->fillRect (  x+sizeM, yyy-sizeM/2+1, sizeM/2, size, QBrush ( QBrush::SolidPattern   ) );
            p->drawLine (  x+sizeM +sizeM/2-1, yyy-sizeM/2,  x+sizeM +sizeM/2-1, yyy+sizeM/2 );
           
        }
        x += sizeM/2 + 1;
        x += sizeM + 1;
    }

    if ( mIncidence->typeID() == todoID ){
        Todo* td = ( Todo* ) mIncidence;
        if ( td->isCompleted() ) {
            int half = size/2;
            p->drawLine ( x, heihei/2, x +half , heihei/2 +half ) ;
            p->drawLine (  x +half , heihei/2 +half ,  x +half+half +2 , heihei/2  -2 ) ;
            x += half+half + 4;

        } else {
            int val = td->percentComplete()/20;
            p->fillRect ( x+1, y-2, val ,size+4,textColor );
            p->drawRect ( x, y-2,7,size+4);
            x += size + 3;
        }
    }
    QFontMetrics fm = p->fontMetrics();
    int yPos;
    int pmheight = size;
    if( pmheight < fm.height() )
        yPos = fm.ascent() + fm.leading()/2;
    else
        yPos = pmheight/2 - fm.height()/2  + fm.ascent();
  
    if ( KOPrefs::instance()->mMonthShowTimes || isWeekItem) {
        p->drawText( x, yPos, text() );
        if ( mIncidence->cancelled() ) {
            int wid = fm.width( text() );
            p->drawLine( x, heihei/2 ,x+wid, heihei/2 );
        }
    } else {
        QString pText = text();
        if( pText.mid(2,1) == ":" )
            pText =  pText.mid( 6 );
        p->drawText( x, yPos,  pText );
        if ( mIncidence->cancelled() ) {
            int wid = fm.width( pText );
            p->drawLine( x, heihei/2 ,x+wid, heihei/2 );
        }
    }  
}

int MonthViewItem::height(const QListBox *lb) const
{
    int ret = 10;
    if ( lb )
        ret = lb->fontMetrics().lineSpacing()+1;
    return ret;
}

int MonthViewItem::width(const QListBox *lb) const
{ 
    if( KOPrefs::instance()->mEnableMonthScroll || isWeekItem ) {
        int size = PIXMAP_SIZE;
        if ( QApplication::desktop()->width() < 300 )
            size = 3;
        int x = 1; 
        if ( KOPrefs::instance()->mMonthShowIcons ) {
            if ( mInfo ) {
                x += size + 1;
            }
            if( mRecur ) {
                x += size+1;
            }
            if( mAlarm ) {
                x += size+1;
            }
            if( mReply ) {
                x += size+1;
            }
        }
        if( mMultiday ) {
            x += size+1+2+size/2;
        }
        return( x + lb->fontMetrics().width( text() ) + 1 );
    }
    if ( ! lb )
        return 10;
    return lb->width();
}


MonthViewCell::MonthViewCell( KOMonthView *parent,QWidget* par )
    : KNoScrollListBox( par ),
      mMonthView( parent )
{
    //QVBoxLayout *topLayout = new QVBoxLayout( this );
    currentPalette = 0; 
    // mLabel = new QLabel( this );QPushButton 
    mLabel = new QPushButton( this );
    //mLabel->setFrameStyle( QFrame::Panel | QFrame::Plain );
    //mLabel->setLineWidth( 1 );
    //mLabel->setAlignment( AlignCenter );
    mLabel->setFlat( true );
    mLabel->setFocusPolicy(NoFocus); 
    //mItemList = new KNoScrollListBox( this );
    setMinimumSize( 10, 10 );
    setFrameStyle( QFrame::Panel | QFrame::Plain );
    setLineWidth( 1 );
    //topLayout->addWidget( mItemList );
    mLabel->raise();
    // QColor( 0,0,255 )    QColor( 160,1600,255 )
    mStandardPalette = palette();
    mStandardPalette.setColor(QColorGroup::Base, mStandardPalette.color( QPalette::Normal, QColorGroup::Background ) );

    enableScrollBars( false );
    updateConfig();
    //connect( mLabel, SIGNAL( clicked( )), SLOT( newEvent() ));
    connect( mLabel, SIGNAL( clicked( )), SLOT( showDay() ));
    connect( this , SIGNAL( doubleClicked( QListBoxItem *) ),
             SLOT( defaultAction( QListBoxItem * ) ) );
    connect( this, SIGNAL( rightButtonPressed( QListBoxItem *,
                                                    const QPoint &) ),
             SLOT( contextMenu( QListBoxItem * ) ) );
    connect( this, SIGNAL( highlighted( QListBoxItem *) ),
             SLOT( selection( QListBoxItem * ) ) );
   
    /* 
    connect( this, SIGNAL( clicked( QListBoxItem * ) ),
             SLOT( selection( QListBoxItem * ) ) ); 
    */
}
#ifdef DESKTOP_VERSION
QToolTipGroup *MonthViewCell::toolTipGroup()
{
    if (!mToolTipGroup) mToolTipGroup = new QToolTipGroup(0);
    return mToolTipGroup;
}
#endif
 
void MonthViewCell::setDate( const QDate &date )
{
    //  kdDebug() << "MonthViewCell::setDate(): " << date.toString() << endl;
    mDate = date;

 

    //resizeEvent( 0 );
}

QDate MonthViewCell::date() const
{
    return mDate;
}

void MonthViewCell::setPrimary( bool primary )
{
    mPrimary = primary;
    //setMyPalette();
}
void MonthViewCell::setMyPalette()
{
   
    if ( mHoliday) {
        if ( currentPalette == 1 ) return;
        mLabel->setPalette( QPalette ( mHolidayPalette.color( QPalette::Normal,QColorGroup::Base),mHolidayPalette.color(QPalette::Normal,QColorGroup::Base )  ));
        setPalette( mHolidayPalette );
        //mLabel->setPalette( mHolidayPalette );
        currentPalette = 1;

    } else {
        if ( mPrimary ) {
            if ( currentPalette == 2 ) return;
            mLabel->setPalette( QPalette ( mPrimaryPalette.color( QPalette::Normal,QColorGroup::Base),mPrimaryPalette.color(QPalette::Normal,QColorGroup::Base )  ));
            //mLabel->setPalette( mPrimaryPalette );
            setPalette( mPrimaryPalette );
            currentPalette = 2;

        } else {
            if ( currentPalette == 3 ) return;
            setPalette( mNonPrimaryPalette );
             mLabel->setPalette( QPalette ( mNonPrimaryPalette.color( QPalette::Normal,QColorGroup::Base),mNonPrimaryPalette.color(QPalette::Normal,QColorGroup::Base )  ));
             //mLabel->setPalette( mNonPrimaryPalette );;
             currentPalette = 3;
        }
    }
    //QPalette pal = palette();
    
    //mLabel->setPalette( QPalette ( pal.color( QPalette::Normal,QColorGroup::Base),pal.color(QPalette::Normal,QColorGroup::Base )  ));
}
QPalette  MonthViewCell::getPalette ()
{
    if ( !KOPrefs::instance()->mMonthViewUsesDayColors )
        return mStandardPalette;
    if ( mHoliday) {
        return mHolidayPalette ;
    } else {
        if ( mPrimary ) {
            return mPrimaryPalette ;
        } 
    }
    return mNonPrimaryPalette; 
}
bool MonthViewCell::isPrimary() const
{
    return mPrimary;
}

void MonthViewCell::setHoliday( bool holiday )
{
    mHoliday = holiday; 
    //setMyPalette();
}

void MonthViewCell::setHoliday( const QString &holiday )
{
    mHolidayString = holiday;

    if ( !holiday.isEmpty() ) {
        setHoliday( true );
    }
}

void MonthViewCell::startUpdateCell()
{
    blockSignals( true );
    mdayCount = 0;
    setFocusPolicy(NoFocus); 
    if ( !mMonthView->isUpdatePossible() )
        return;
    MonthViewItem *mitem = (MonthViewItem*) firstItem ();
    while ( mitem ) {
        mitem->setBlockRepaint( true );
        mitem = (MonthViewItem *)mitem->next();
    }
    if ( mAvailItemList.count() > 20 ) {
        mAvailItemList.setAutoDelete( true );
        mAvailItemList.clear();
        mAvailItemList.setAutoDelete( false );
        clear();
    }
   
    setPrimary( mDate.month()%2 );
    setHoliday( KOGlobals::self()->calendarSystem()->dayOfWeek(mDate) == KOGlobals::self()->calendarSystem()->weekDayOfPray() || ( mDate.dayOfWeek() == 6 ) && KOPrefs::instance()-> mExcludeSaturdays);
    if ( mDate == QDate::currentDate()  ) {
        setLineWidth( 3 );
    } else {
        setLineWidth( 1 );
    }
    MonthViewItem* CurrentAvailItem = (MonthViewItem*) firstItem ();
    //clear(); 
    while ( CurrentAvailItem ) {
            MonthViewItem *item = CurrentAvailItem;
            //item->setHighlightedFalse();
            item->recycle( 0, "");
            CurrentAvailItem = (MonthViewItem *)item->next();
            mAvailItemList.append( item ); 
            takeItem ( item );
    }

#ifdef DESKTOP_VERSION
    QToolTip::remove(this); 
#endif 
    mToolTip.clear(); 
    //qApp->processEvents();
#if 0
    if ( !mHolidayString.isEmpty() ) {
        MonthViewItem *item = new MonthViewItem( 0, mDate, mHolidayString );
        item->setPalette( mHolidayPalette );
        insertItem( item );
        mToolTip.append ( mHolidayString ); 
    }
#endif
}

int MonthViewCell::insertEvent(Event *event)
{
    bool useToolTips = true;
#ifndef DESKTOP_VERSION
    useToolTips = false;
#endif    
    QString mToolTipText;
    setFocusPolicy(WheelFocus); 
        if ( !(event->doesRecur() == Recurrence::rNone) ) {
            if ( !KOPrefs::instance()->mMonthDailyRecur && event->doesRecur() == Recurrence::rDaily )
                return mdayCount;
            else
                if ( !KOPrefs::instance()->mMonthWeeklyRecur && event->doesRecur() == Recurrence::rWeekly )
                    return mdayCount;
        }
        
        if ( event->isHoliday()) {
            setHoliday( true );
            if ( mDate.dayOfWeek() == 7 )
                setLineWidth( 3 );
        }
        QString text;
        int multiday = 0;// 1 = start, 2 = midddle, 3 = end day
        if (event->isMultiDay()) {
            QString prefix = "<->";multiday = 2; 
            QString time;
            if ( event->doesRecur() ) {
                if ( event->recursOn( mDate) ) {
                    prefix ="->" ;multiday = 1;
                }
                else {
                    int days =  event->dtStart().date().daysTo ( event->dtEnd().date() );
                    if ( event->recursOn( mDate.addDays( -days)) ) {
                        prefix ="<-" ;multiday = 3;
                    }
                } 

            } else {
                if (mDate == event->dtStart().date()) {
                    prefix ="->" ;multiday = 1;
                } else if (mDate == event->dtEnd().date()) {
                    prefix ="<-" ;multiday = 3;
                }
            }
            if ( !event->doesFloat() ) {
                if ( mDate == event->dtStart().date () )
                    time = KGlobal::locale()->formatTime(event->dtStart().time())+" ";
                else if ( mDate == event->dtEnd().date () )
                    time = KGlobal::locale()->formatTime(event->dtEnd().time())+" ";

            }
            text = time + event->summary();

            int dur = 0;
            if ( event->doesFloat() ) {
                dur = event->dtStart().daysTo(event->dtEnd())+1;
                
            }
            if ( useToolTips ) {
                mToolTipText += prefix + text;
                if ( dur ) {
                    mToolTipText += i18n(" (Duration: %1 days)" ).arg ( dur );
                }
            }
            if ( dur ) {
                text += " ("+ QString::number( dur ) + i18n(" days" ) + ")";
            }
        } else {
            if (event->doesFloat()) {
                text = event->summary();
                if ( useToolTips )
                    mToolTipText += text;
            }
            else {
                text = KGlobal::locale()->formatTime(event->dtStart().time());
                text += " " + event->summary();
                if ( useToolTips )
                    mToolTipText +=  KGlobal::locale()->formatTime(event->dtStart().time()) +"-"+KGlobal::locale()->formatTime(event->dtEnd().time())+" " + event->summary();
            }
        }
        if ( useToolTips && ! event->location().isEmpty() ) {
            mToolTipText += " (" + event->location() +")";
        }
        MonthViewItem *item ;
        
        if ( mAvailItemList.count() ) {
            item = mAvailItemList.first();
            mAvailItemList.remove( item );
            item->recycle( event, text );
        } else {
            item = new MonthViewItem( event, text );
        }
        
        QPalette pal;
        if (KOPrefs::instance()->mMonthViewUsesCategoryColor) {    
            QStringList categories = event->categories();
            QString cat = categories.first();
            if ( KOPrefs::instance()->mMonthViewUsesForegroundColor ) {
                pal = getPalette();
                if (cat.isEmpty()) {
                    //pal.setColor(QColorGroup::Foreground,KOPrefs::instance()->mEventColor);
                    pal.setColor(QColorGroup::Foreground,KOPrefs::instance()->defaultColor( event->calID() ));
                } else {
                    pal.setColor(QColorGroup::Foreground, *(KOPrefs::instance()->categoryColor(cat)));
                }

            } else {
                if (cat.isEmpty()) {
                    //pal = QPalette(KOPrefs::instance()->mEventColor, KOPrefs::instance()->mEventColor);
                    pal = QPalette( KOPrefs::instance()->defaultColor( event->calID() ), KOPrefs::instance()->defaultColor( event->calID() ));
                } else {
                    pal = QPalette(*(KOPrefs::instance()->categoryColor(cat)), *(KOPrefs::instance()->categoryColor(cat)));
                } 
            }
           
        } else {
            pal = mStandardPalette ;
        }
        pal.setColor(QColorGroup::Highlight, KOPrefs::instance()->mHighlightColor);
        item->setPalette( pal );
        item->setRecur( event->doesRecur() );
        item->setAlarm( event->isAlarmEnabled() && multiday < 2 && event->alarmEnabled() );
        item->setMoreInfo( event->description().length() > 0  );
#ifdef DESKTOP_VERSION
        Attendee *me = event->attendeeByMails(KOPrefs::instance()->mAdditionalMails,
                                              KOPrefs::instance()->email());
        if ( me != 0 ) {
            if ( me->status() == Attendee::NeedsAction && me->RSVP())
                item->setReply(true && multiday < 2);
            else
                item->setReply(false);
        } else
            item->setReply(false);
#endif
       
        item->setMultiDay( multiday );
        if ( multiday ) {
            insertItem( item ,mdayCount);
            ++mdayCount;
        } else {
            uint i = mdayCount;
            uint pos = mdayCount;
            uint itcount = count();
            if ( itcount > 1000 ) {
                qDebug("KO: Bug in MonthViewCell::insertEvent %u ", itcount);
                itcount = 0;
            }
            for ( i = pos; i < itcount;++i ) {
                // qDebug("i %d mday %u count %d ",i,itcount,mdayCount );
                QListBoxItem* it = this->item ( i );
                if ( it && text < it->text() ) {
                    pos = i;
                    break;
                }
                ++pos;
            }
            insertItem( item ,pos);
        }
        if ( useToolTips ) {
            mToolTip.append( mToolTipText );
        }
        return mdayCount;
}
void MonthViewCell::insertTodo(Todo *todo)
{
    setFocusPolicy(WheelFocus); 
      QString text;
        if (todo->hasDueDate()) {
            if (!todo->doesFloat()) {
                text += KGlobal::locale()->formatTime(todo->dtDue().time());
                text += " ";
            }
        }
        text += todo->summary();
        MonthViewItem *item ;
        if ( mAvailItemList.count() ) {
            item = mAvailItemList.first();
            mAvailItemList.remove( item );
            item->recycle( todo, text );
        } else {
            item = new MonthViewItem( todo, text );
        }
        //MonthViewItem *item = new MonthViewItem( todo, mDate, text );
        //item->setPalette( mStandardPalette );
        QPalette pal;
        if (KOPrefs::instance()->mMonthViewUsesCategoryColor) {    
            QStringList categories = todo->categories();
            QString cat = categories.first();
            if ( KOPrefs::instance()->mMonthViewUsesForegroundColor ) {
                pal = getPalette();
                if (cat.isEmpty()) {
                    //pal.setColor(QColorGroup::Foreground,KOPrefs::instance()->mEventColor);
                    pal.setColor(QColorGroup::Foreground,KOPrefs::instance()->defaultColor( todo->calID() ));
                } else {
                    pal.setColor(QColorGroup::Foreground, *(KOPrefs::instance()->categoryColor(cat)));
                }

            } else {
                if (cat.isEmpty()) {
                    //pal = QPalette(KOPrefs::instance()->mEventColor, KOPrefs::instance()->mEventColor);
                    pal = QPalette(KOPrefs::instance()->defaultColor( todo->calID() ), KOPrefs::instance()->defaultColor( todo->calID() ));
                } else {
                    pal = QPalette(*(KOPrefs::instance()->categoryColor(cat)), *(KOPrefs::instance()->categoryColor(cat)));
                } 
            }
           
        } else {
            pal = mStandardPalette ;
        }
        pal.setColor(QColorGroup::Highlight, KOPrefs::instance()->mHighlightColor);
        item->setPalette( pal );
        item->setRecur( todo->doesRecur() );
        item->setAlarm( todo->isAlarmEnabled() && todo->alarmEnabled() );
        item->setMoreInfo( todo->description().length() > 0  );
        insertItem( item , count());
#ifdef DESKTOP_VERSION
        mToolTip.append( text ); 
#endif
}
void MonthViewCell::repaintfinishUpdateCell()
{
    MonthViewItem *mitem = (MonthViewItem*) firstItem ();
    while ( mitem ) {
        mitem->setBlockRepaint( false );
        updateItem ( mitem );
        mitem = (MonthViewItem *)mitem->next();
    }
    blockSignals( false );
}
void MonthViewCell::finishUpdateCell()
{
   
    

#ifdef DESKTOP_VERSION
    if (mToolTip.count() > 0 ) {
        mToolTip.sort();
        QToolTip::add(this,mToolTip.join("\n"),toolTipGroup(),"");
    }
#endif
    //sort();
    //setMyPalette(); 
    setMyPalette();
  
    resizeEvent( 0 ); 

}
void MonthViewCell::updateCell()
{
    if ( !mMonthView->isUpdatePossible() )
        return;
    startUpdateCell();
    //mLabel->setMaximumWidth( width() - mItemList->lineWidth()*2);
    QPtrList<Event> events = mMonthView->calendar()->events( mDate, true );
    Event *event;
    for( event = events.first(); event; event = events.next() ) { // for event
        insertEvent(event);
    }
    // insert due todos
    QPtrList<Todo> todos = mMonthView->calendar()->todos( mDate );
    Todo *todo;
    for(todo = todos.first(); todo; todo = todos.next()) {
        insertTodo( todo );
    }
    finishUpdateCell();
    // if ( isVisible())
    //qApp->processEvents();
}

void MonthViewCell::updateConfig( bool bigFont ) // = false
{

    if ( bigFont ) {
        QFont fo = KOPrefs::instance()->mMonthViewFont;
        int ps = fo.pointSize() + 2;
        if ( ps < 18 )
            ps += 2;
        fo.setPointSize( ps ); 
        setFont( fo );
    } else
        setFont( KOPrefs::instance()->mMonthViewFont );

    QFontMetrics fm( font() );
    mLabelSize = fm.size( 0, "30" ) + QSize( 4, 2 );
    mLabelBigSize = fm.size( 0, "Mag 30" ) + QSize( 2, 2 );
    mHolidayPalette = mStandardPalette;
    mPrimaryPalette = mStandardPalette;
    mNonPrimaryPalette = mStandardPalette;
    if ( KOPrefs::instance()->mMonthViewUsesDayColors ) {
        mHolidayPalette.setColor(QColorGroup::Base, KOPrefs::instance()->mMonthViewHolidayColor  );
        mHolidayPalette.setColor(QColorGroup::Background, KOPrefs::instance()->mMonthViewHolidayColor  );
        mHolidayPalette.setColor(QColorGroup::Foreground,  KOPrefs::instance()->mMonthViewHolidayColor.dark());
        mPrimaryPalette.setColor(QColorGroup::Foreground,KOPrefs::instance()->mMonthViewOddColor.dark());
        mPrimaryPalette.setColor(QColorGroup::Base,KOPrefs::instance()->mMonthViewOddColor);
        mPrimaryPalette.setColor(QColorGroup::Background,KOPrefs::instance()->mMonthViewOddColor);
        mNonPrimaryPalette.setColor(QColorGroup::Foreground,KOPrefs::instance()->mMonthViewEvenColor.dark());
        mNonPrimaryPalette.setColor(QColorGroup::Base,KOPrefs::instance()->mMonthViewEvenColor); 
        mNonPrimaryPalette.setColor(QColorGroup::Background,KOPrefs::instance()->mMonthViewEvenColor); 
    }
    //updateCell(); 
}

void MonthViewCell::enableScrollBars( bool enabled )
{
    
    return;
    if ( enabled ) {
        QListBoxItem *fi =  firstItem ();
        if (fi ) {
            int ihei = fi->height( this );
            int hei = numRows ()  * ihei;
            if ( hei < height() - horizontalScrollBar()->height () ) {
                setVScrollBarMode(QScrollView::AlwaysOff);
            }
            else 
                setVScrollBarMode(QScrollView::Auto);
            if ( ihei *3 > height() ) {
                setHScrollBarMode(QScrollView::AlwaysOff);
            }
            else {
                setHScrollBarMode(QScrollView::Auto);
            }
        } else {
            setVScrollBarMode(QScrollView::Auto);
            setHScrollBarMode(QScrollView::Auto);
        }
    } else {
        setVScrollBarMode(QScrollView::AlwaysOff);
        setHScrollBarMode(QScrollView::AlwaysOff);
    }
}

Incidence *MonthViewCell::selectedIncidence()
{
    int index = currentItem();
    if ( index < 0 ) return 0;
  
    MonthViewItem *mitem =
        static_cast<MonthViewItem *>( item( index ) );

    if ( !mitem ) return 0;
  
    return mitem->incidence();
}

QDate MonthViewCell::selectedIncidenceDate()
{
    QDate qd;
    int index = currentItem();
    if ( index < 0 ) return qd;
    return mDate;
}

void MonthViewCell::deselect()
{
    clearSelection();
    enableScrollBars( false );
    // updateCell();
}
void MonthViewCell::select()
{
    ;// updateCell();
}

void MonthViewCell::resizeEvent ( QResizeEvent * e )
{
    if ( !mMonthView->isUpdatePossible() )
        return;
    //qDebug("++++++++++++++MonthViewCell::resizeEvent %d %d ", width(), height());
    deselect();
    mLabel->setMaximumHeight( height() - lineWidth()*2 );
    
    QString text;
    //mLabel->setText( text );
    bool smallDisplay = QApplication::desktop()->width() < 320 && KOPrefs::instance()->mMonthViewSatSunTog;
    if ( KOPrefs::instance()->mMonthViewWeek || KOGlobals::self()->calendarSystem()->day( mDate ) == 1 || (mDate.dayOfWeek() == 7 && !smallDisplay ) || KOPrefs::instance()->mMonthShowShort) { 
        text = KOGlobals::self()->calendarSystem()->monthName( mDate, true ) + " " + QString::number( mDate.day() );
        mLabel->resize( mLabelBigSize  );
    } else {
        mLabel->resize( mLabelSize );
        text = QString::number( mDate.day() );
    }
    mLabel->setText( text );

    int size = height() - mLabel->height() - lineWidth()-1;
    //qDebug("LW %d ", lineWidth());
    if ( size > 0 )
        verticalScrollBar()->setMaximumHeight( size );
    size = width() - mLabel->width() -lineWidth()-1;
    if ( size > 0 )
        horizontalScrollBar()->setMaximumWidth( size );
    mLabel->move( width()-lineWidth() - mLabel->width(), height()-lineWidth() - mLabel->height() );
    //mLabel->setMaximumWidth( width() - mItemList->lineWidth()*2);
    // mItemList->resize ( width(), height () ); 
    if ( e )
        KNoScrollListBox::resizeEvent (  e );
}

void MonthViewCell::defaultAction( QListBoxItem *item )
{
  
    if ( !item ) {
        QDateTime dt( date(), QTime( KOPrefs::instance()->mStartTime, 0 ) );
        emit newEventSignal( dt );
        return;
    }

    MonthViewItem *eventItem = static_cast<MonthViewItem *>( item );
    Incidence *incidence = eventItem->incidence();
    if ( incidence ) mMonthView->defaultAction( incidence );
}
void MonthViewCell::showDay()
{
    emit showDaySignal( date() );
}
void MonthViewCell::newEvent()
{
    QDateTime dt( date(), QTime( KOPrefs::instance()->mStartTime, 0 ) );
    emit newEventSignal( dt );
}
void MonthViewCell::cellClicked( QListBoxItem *item )
{
    mMonthView->setSelectedCell( this );
    if ( item == 0 ) {
        QDateTime dt( date(), QTime( KOPrefs::instance()->mStartTime, 0 ) );
        emit newEventSignal( dt );
        return;
    }

}

void MonthViewCell::contextMenu( QListBoxItem *item )
{
    mMonthView->setPopupCell( this );
    if ( !item )  {
        mMonthView->showContextMenu( 0 );
        return;
    }
    //selection( item );
    //qApp->processEvents();
    MonthViewItem *eventItem = static_cast<MonthViewItem *>( item );
    Incidence *incidence = eventItem->incidence();
    if ( incidence ) {
      mBlockDeselect = true;
        mMonthView->showContextMenu( incidence );
    }
}

void MonthViewCell::selection( QListBoxItem *item )
{
    if ( !item ) {
        emit highlightIncidence( 0 , this, 0 );
        return;
    }
    MonthViewItem * it = (static_cast<MonthViewItem *>( item ));
    emit highlightIncidence( it->incidence(), this, it->multiDay() );
    mMonthView->setSelectedCell( this );
}

void  MonthViewCell::deHighLight()
{
    MonthViewItem *mitem = (MonthViewItem*) firstItem ();
    while ( mitem ) {
        if ( mitem->setHighlightedFalse() )
            updateItem ( mitem );
        mitem = (MonthViewItem *)mitem->next();
    }
}
// returns true if no inc found
bool MonthViewCell::doHighLight( Incidence * inc )
{
 
    MonthViewItem *mitem = (MonthViewItem*) firstItem ();
    while ( mitem ) {
        if ( mitem->incidence() == inc ) {
            if ( mitem->setHighlighted( inc ) )
                updateItem ( mitem );
            return false;
        }
        mitem = (MonthViewItem *)mitem->next();
    }
    return true;
}    
// *******************************************************************************
// *******************************************************************************
// *******************************************************************************


KOMonthView::KOMonthView(Calendar *calendar, QWidget *parent, const char *name)
    : KOEventView( calendar, parent, name ),
      mDaysPerWeek( 7 ), mNumWeeks( 6 ), mNumCells( mDaysPerWeek * mNumWeeks ),
      mWidthLongDayLabel( 0 ), mSelectedCell( 0 )
{
    mFlagKeyPressed = false;
    mShortDayLabelsM = false;
    mShortDayLabelsW = false;
    skipResize = false;
    clPending = true;
    mPopupCell = 0;
    mNavigatorBar = new NavigatorBar( QDate::currentDate(), this, "useBigPixmaps" );
    mWidStack = new QWidgetStack( this );
    QVBoxLayout* hb = new QVBoxLayout( this );
    mMonthView = new QWidget( mWidStack );
    mWeekView = new QWidget( mWidStack );
#if QT_VERSION >= 0x030000
    mWidStack->addWidget(mMonthView );
    mWidStack->addWidget(mWeekView  );
#else
    mWidStack->addWidget( mMonthView, 1 );
    mWidStack->addWidget( mWeekView , 1 );
#endif
    hb->addWidget( mNavigatorBar );
    hb->addWidget( mWidStack );
    mShowWeekView = KOPrefs::instance()->mMonthViewWeek;
    updatePossible = false;
    //updatePossible = true;
    mCells.setAutoDelete( true );
    mShowSatSunComp = KOPrefs::instance()->mMonthViewSatSunTog ;
    mDayLabels.resize( mDaysPerWeek );
    mDayLabelsW.resize( mDaysPerWeek );
    QFont bfont = font();
    if ( QApplication::desktop()->width() < 650 ) {
        bfont.setPointSize( bfont.pointSize() - 2 );
    }
    bfont.setBold( true );
    int i;
  
    for( i = 0; i < mDaysPerWeek; i++ ) {
        QLabel *label = new QLabel( mMonthView );
        label->setFont(bfont);
        label->setFrameStyle(QFrame::Panel|QFrame::Raised);
        label->setLineWidth(1);
        label->setAlignment(AlignCenter);
        mDayLabels.insert( i, label );
        label = new QLabel( mWeekView );
        label->setFont(bfont);
        label->setFrameStyle(QFrame::Panel|QFrame::Raised);
        label->setLineWidth(1);
        label->setAlignment(AlignCenter);
        mDayLabelsW.insert( i, label );
    }

    bfont.setBold( false );
    mWeekLabels.resize( mNumWeeks+1 );
    mWeekLabelsW.resize( 2 );
    for( i = 0; i < mNumWeeks+1; i++ ) {
        KOWeekButton *label = new KOWeekButton( mMonthView );
        label->setFocusPolicy(NoFocus); 
        label->setFont(bfont);
        connect( label, SIGNAL( selectWeekNum ( int )),this, SLOT( selectInternalWeekNum ( int ))   );  
        label->setFlat(true);
        QWhatsThis::add(label,i18n("Click on the week number to\nshow week zoomed"));
        //label->setFrameStyle(QFrame::Panel|QFrame::Raised);
        //label->setLineWidth(1);
        //label->setAlignment(AlignCenter);
        mWeekLabels.insert( i, label );
    }
    mWeekLabels[mNumWeeks]->setText( i18n("W"));
    mWeekLabels[mNumWeeks]->setFocusPolicy(WheelFocus);
    QWhatsThis::add(mWeekLabels[mNumWeeks],i18n("Click on this to\nselect week number"));
    
    for( i = 0; i < 1+1; i++ ) {
        KOWeekButton *label = new KOWeekButton( mWeekView );
        label->setFocusPolicy(NoFocus); 
        label->setFont(bfont);
        connect( label, SIGNAL( selectWeekNum ( int )),this, SLOT( selectInternalWeekNum ( int ))   );  
        label->setFlat(true);
        QWhatsThis::add(label,i18n("Click on the week number to\nshow week zoomed"));
        //label->setFrameStyle(QFrame::Panel|QFrame::Raised);
        //label->setLineWidth(1);
        //label->setAlignment(AlignCenter);
        mWeekLabelsW.insert( i, label );
    }
    mWeekLabelsW[1]->setText( i18n("W"));
    mWeekLabelsW[1]->setFocusPolicy(WheelFocus); 


    int row, col;
    mCells.resize( mNumCells );
    for( row = 0; row < mNumWeeks; ++row ) {
        for( col = 0; col < mDaysPerWeek; ++col ) {
            MonthViewCell *cell = new MonthViewCell( this, mMonthView );
            mCells.insert( row * mDaysPerWeek + col, cell );

            connect( cell, SIGNAL( defaultAction( Incidence * ) ),
                     SLOT( defaultAction( Incidence * ) ) );
            connect( cell, SIGNAL( newEventSignal( QDateTime ) ),
                     SIGNAL( newEventSignal( QDateTime ) ) );
            connect( cell, SIGNAL( showDaySignal( QDate ) ),
                     SIGNAL( showDaySignal( QDate ) ) );
            connect( cell, SIGNAL( nextCell() ),
                     SLOT( nextCell() ) );
            connect( cell, SIGNAL( prevCell() ),
                     SLOT( prevCell() ) );
            connect( cell, SIGNAL(  highlightIncidence( Incidence * , MonthViewCell *, int ) ),
                     SLOT( incidenceHighlighted( Incidence *, MonthViewCell *, int ) ));
        }
    }
    mCellsW.resize( mDaysPerWeek );
    for( col = 0; col < mDaysPerWeek; ++col ) {
        MonthViewCell *cell = new MonthViewCell( this, mWeekView );
        mCellsW.insert( col, cell );

        connect( cell, SIGNAL( defaultAction( Incidence * ) ),
                 SLOT( defaultAction( Incidence * ) ) );
        connect( cell, SIGNAL( newEventSignal( QDateTime ) ),
                 SIGNAL( newEventSignal( QDateTime ) ) );
        connect( cell, SIGNAL( showDaySignal( QDate ) ),
                 SIGNAL( showDaySignal( QDate ) ) );
        connect( cell, SIGNAL( nextCell() ),
                 SLOT( nextCell() ) );
        connect( cell, SIGNAL( prevCell() ),
                 SLOT( prevCell() ) );
        connect( cell, SIGNAL(  highlightIncidence( Incidence * , MonthViewCell *, int ) ),
                 SLOT( incidenceHighlighted( Incidence *, MonthViewCell *, int ) ));
        cell->updateConfig(KOPrefs::instance()->mMonthViewUsesBigFont );
    }

    //connect( mWeekLabels[mNumWeeks], SIGNAL( clicked() ), SLOT( switchView() ) );
    mContextMenu = eventPopup();
    mContextMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("New Event..."),this,
                                  SLOT(slotNewEvent()),false);
    mContextMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("New Todo..."),this,
                                  SLOT(slotNewTodo()),false);
    mContextMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("Journal"),this,
                                  SLOT(slotEditJournal()),false);

    connect (mContextMenu ,SIGNAL(categoryChanged( Incidence * )),this,
                     SLOT( catChanged( Incidence * ) ));


    QString pathString = "";
    if ( !KOPrefs::instance()->mToolBarMiniIcons ) {
        if ( QApplication::desktop()->width() < 480 )
            pathString += "icons16/";
    } else
        pathString += "iconsmini/";
    mNewItemMenu = new QPopupMenu( this );
    mNewItemMenu->insertItem( SmallIcon( pathString +"newevent" ), i18n("New Event..."),this, SLOT(slotNewEvent()));
    mNewItemMenu->insertItem( SmallIcon( pathString +"newtodo" ),i18n("New Todo..."),this,SLOT(slotNewTodo()),false);
    mNewItemMenu->insertItem( SmallIcon( pathString +"journal" ),i18n("Journal"),this,SLOT(slotEditJournal()),false);

    // updateConfig(); //useless here...
    // ... but we need mWidthLongDayLabel computed
    QFontMetrics fontmetric(mDayLabels[0]->font());
    mWidthLongDayLabel = 0;
    for (int i = 0; i < 7; i++) {
        int width = fontmetric.width(KOGlobals::self()->calendarSystem()->weekDayName(i+1));
        if ( width > mWidthLongDayLabel ) mWidthLongDayLabel = width;
    }

    //mWeekLabels[mNumWeeks]->setText( i18n("W"));

#if 0
    if ( mShowWeekView )
        mWidStack->raiseWidget( mWeekView );
    else
        mWidStack->raiseWidget( mMonthView );
#endif

    emit incidenceSelected( 0 );

    mComputeLayoutTimer = new QTimer( this );
    connect (mComputeLayoutTimer ,SIGNAL(timeout()), this, SLOT ( slotComputeLayout()));


#ifndef DESKTOP_VERSION
    resize( QApplication::desktop()->size() );
#else
    resize(640, 480 );
    updatePossible = true;
#endif
    computeLayout();

    if ( mShowWeekView )
        mWidStack->raiseWidget( mWeekView );
    else
        mWidStack->raiseWidget( mMonthView );
}

KOMonthView::~KOMonthView()
{
    delete mContextMenu;
}

void KOMonthView::catChanged( Incidence * )
{
    updateView();
}
void KOMonthView::incidenceHighlighted( Incidence * inc , MonthViewCell* mc, int mday )
{
    static Incidence * lastInc = 0;
    static MonthViewCell * lastCell = 0;
    
    if ( lastInc == inc && lastCell == mc )
        return;
    lastInc = inc;
    lastCell = mc;
    //qDebug("KOMonthView::incidenceHighlighted %d %d %d", inc, mc, mday );

    bool weekview = false;
    uint index = 0;
    for (uint i = 0; i < mCellsW.count(); ++i) {
        if ( mCellsW[i] == mc ) {
            weekview = true;
            index = i;
            break;
        }
    }
    QPtrVector<MonthViewCell> *cells;
    if ( weekview ) 
        cells = &mCellsW;
    else {
        for (uint i = 0; i < mCells.count(); ++i) {
            if ( mCells[i] == mc ) {
                index = i;
                break;
            }
        }
        cells = &mCells;
    }
    for (uint i = 0; i < (*cells).count(); ++i) {
        (*cells)[i]->deHighLight();
    }
    if ( ! inc ) 
        return;
    
    uint count = (*cells).count();
    bool goLeft = (mday > 1 && index > 0);
    bool goRight = (mday < 3 && mday > 0 && index < count -1);
    for (uint iii = 1; iii < count; ++iii) {
        if ( goLeft ) {
            int left = index - iii;
            if ( left >= 0 ) {
                if ( (*cells)[(uint)left]->doHighLight(inc) )
                    goLeft = false;
            } else 
                goLeft = false;
        } 
        if ( goRight ) {
            uint right = index + iii;
            if ( right < count ) {
                if ( (*cells)[right]->doHighLight(inc) )
                    goRight = false; 

            } else  
                goRight = false; 
        }

    }
#if 0
    if ( mday > 1 && index > 0 )
        for (int i = index-1; i >= 0; --i) {
            //qDebug("index %d iii %d ", index, i);
            if ( (*cells)[(uint)i]->doHighLight(inc) )
                break;
        }
    if ( mday < 3 && mday > 0 && index < (*cells).count()-1)
        for (uint i = index+1; i < (*cells).count(); ++i) {
            if ( (*cells)[i]->doHighLight(inc) )
                break;
        }
#endif
    
}
void KOMonthView::selectInternalWeekNum ( int n )
{
    switchView();  
    if ( !KOPrefs::instance()->mMonthViewWeek )
        emit selectMonth ();
    else
        emit selectWeekNum ( n );
}

int KOMonthView::currentWeek()
{
    if ( mShowWeekView )
        return mWeekLabelsW[0]->getWeekNum();
    return mWeekLabels[0]->getWeekNum();
}
void KOMonthView::switchView()
{
    if ( selectedCell( ) )
        selectedCell()->deselect();
    mShowWeekView = !mShowWeekView;
    KOPrefs::instance()->mMonthViewWeek = mShowWeekView;
    if ( clPending ) {
      computeLayout();
      updateConfig();
    }
    if ( mShowWeekView )
        mWidStack->raiseWidget( mWeekView );
    else
        mWidStack->raiseWidget( mMonthView ); 
    clPending = false;
}

int KOMonthView::maxDatesHint()
{
    return mNumCells;
}

int KOMonthView::currentDateCount()
{
    return mNumCells;
}

QPtrList<Incidence> KOMonthView::selectedIncidences()
{
    QPtrList<Incidence> selected;

    if ( mSelectedCell ) {
        Incidence *incidence = mSelectedCell->selectedIncidence();
        if ( incidence ) selected.append( incidence );
    }

    return selected;
}

DateList KOMonthView::selectedDates()
{
    DateList selected;

    if ( mSelectedCell ) {
        QDate qd = mSelectedCell->selectedIncidenceDate();
        if ( qd.isValid() ) selected.append( qd );
    }

    return selected;
}
#if 0
void KOMonthView::printPreview(CalPrinter *calPrinter, const QDate &fd,
                               const QDate &td)
{
#ifndef KORG_NOPRINTER
    calPrinter->preview(CalPrinter::Month, fd, td);
#endif
}
#endif
void KOMonthView::updateConfig()
{
 
    int mWeekStartsMonday = KGlobal::locale()->weekStartsMonday();

    if ( mShowWeekView || KOPrefs::instance()->mMonthViewSatSunTog ) {
        mWeekStartsMonday = true;
    } 
    QFontMetrics fontmetric(mDayLabels[0]->font());
    mWidthLongDayLabel = 0;

    for (int i = 0; i < 7; i++) {
        int width = fontmetric.width(KOGlobals::self()->calendarSystem()->weekDayName(i+1));
        if ( width > mWidthLongDayLabel ) mWidthLongDayLabel = width;
    }
    bool temp = mShowSatSunComp ;
    mShowSatSunComp = KOPrefs::instance()->mMonthViewSatSunTog ;
    if ( !  mShowWeekView ) {
        if ( temp != KOPrefs::instance()->mMonthViewSatSunTog )
            computeLayout();
    } else 
        doComputeLayoutWeek();
    updateDayLabels();
    //qDebug("KOMonthView::updateConfig() %d %d %d ",height(), mDayLabels[0]->sizeHint().height() ,mNumWeeks);
    //int cellHeight = (height() - mDayLabels[0]->sizeHint().height()) /mNumWeeks;
    //resizeEvent( 0 );
    for (uint i = 0; i < mCells.count(); ++i) {
        mCells[i]->updateConfig();
    }

    for (uint i = 0; i < mCellsW.count(); ++i) {
        mCellsW[i]->updateConfig(KOPrefs::instance()->mMonthViewUsesBigFont);
    }
#ifdef DESKTOP_VERSION
    MonthViewCell::toolTipGroup()->setEnabled(KOPrefs::instance()->mEnableToolTips); 
#endif
    updateView();
}

void KOMonthView::updateDayLabels()
{
  
    QPtrVector<QLabel> *mDayLabelsT;

    mDayLabelsT = &mDayLabelsW;
    for (int i = 0; i < 7; i++) {
       {
            bool show = mShortDayLabelsW;
            if ( i > 4 && mShowSatSunComp && mWidthLongDayLabel > (*mDayLabelsT)[i]->width()  )
                show = true;
            (*mDayLabelsT)[i]->setText(KOGlobals::self()->calendarSystem()->weekDayName(i+1,show));
        }
    }
    mDayLabelsT = &mDayLabels;
    for (int i = 0; i < 7; i++) {
        if (KGlobal::locale()->weekStartsMonday() || KOPrefs::instance()->mMonthViewSatSunTog ) {
            bool show = mShortDayLabelsM;
            if ( i > 4 && mShowSatSunComp && mWidthLongDayLabel > (*mDayLabelsT)[i]->width()  )
                show = true;
            (*mDayLabelsT)[i]->setText(KOGlobals::self()->calendarSystem()->weekDayName(i+1,show));
        } else {
            if (i==0) (*mDayLabelsT)[i]->setText(KOGlobals::self()->calendarSystem()->weekDayName(7,mShortDayLabelsM));
            else (*mDayLabelsT)[i]->setText(KOGlobals::self()->calendarSystem()->weekDayName(i,mShortDayLabelsM));

        }
    }

}

void KOMonthView::clearList()
{
    unsigned int i;
    for( i = 0; i < mCells.size(); ++i ) {
        mCells[i]->clear();
    }  
    for( i = 0; i < mCellsW.size(); ++i ) {
        mCellsW[i]->clear();
    }
}
void KOMonthView::showDates(const QDate &start, const QDate &)
{
    //  kdDebug() << "KOMonthView::showDates(): " << start.toString() << endl;

    QPtrVector<MonthViewCell> *cells;
    QPtrVector<QLabel> *dayLabels;
    QPtrVector<KOWeekButton> *weekLabels;
    uint weekNum = 6;
    mStartDate = start;
    if ( mShowWeekView ) {
        weekNum = 1;
        cells = &mCellsW;
        dayLabels = &mDayLabelsW;
        weekLabels = &mWeekLabelsW;
        if ( !KGlobal::locale()->weekStartsMonday() ) {
            mStartDate = mStartDate.addDays( 1 );
        }
    } else {
        cells = &mCells;
        dayLabels = &mDayLabels;
        weekLabels = &mWeekLabels;
    }

    int mWeekStartsMonday =  KGlobal::locale()->weekStartsMonday();

    if ( mShowWeekView || KOPrefs::instance()->mMonthViewSatSunTog ) {
        mWeekStartsMonday = true;
    } 
    int startWeekDay = mWeekStartsMonday ? 1 : 7;

    while( KOGlobals::self()->calendarSystem()->dayOfWeek(mStartDate) != startWeekDay ) {
        mStartDate = mStartDate.addDays( -1 );
    }
    uint i;
    for( i = 0; i < (*cells).size(); ++i ) {
        QDate date = mStartDate.addDays( i );
        (*cells)[i]->setDate( date );

#ifndef KORG_NOPLUGINS
        // add holiday, if present
        QString hstring(KOCore::self()->holiday(date));
        (*cells)[i]->setHoliday( hstring );
#endif

    }
    QDate date = mStartDate.addDays( mWeekStartsMonday ? 3 : 4 );
    for( i = 0; i < weekNum; ++i ) {
        int wno;
        // remember, according to ISO 8601, the first week of the year is the
        // first week that contains a thursday.  Thus we must subtract off 4,
        // not just 1.
        int dayOfYear = date.dayOfYear();
        if (dayOfYear % 7 != 0)
            wno = dayOfYear / 7 + 1;
        else
            wno =dayOfYear / 7;
        (*weekLabels)[i]->setWeekNum( wno );
        date = date.addDays( 7 );
    }
    updateView();
}

void KOMonthView::showEvents(QPtrList<Event>)
{
    qDebug("KOMonthView::selectEvents is not implemented yet. ");
}

void KOMonthView::changeEventDisplay(Event *, int)
{
    // this should be re-written to be much more efficient, but this
    // quick-and-dirty-hack gets the job done for right now.
    //qDebug("KOMonthView::changeEventDisplay ");
    updateView();
}

void KOMonthView::updateView()
{ 

    if ( !updatePossible )
        return;
    //qDebug("UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU ");
    //QTime ti;
    //ti.start();
    clearSelection();
    QPtrVector<MonthViewCell> *cells;
    if ( mShowWeekView ) {
        cells = &mCellsW;
    } else {
        cells = &mCells;
    }
#if 1
    int i;
    int timeSpan = (*cells).size()-1;
    if ( KOPrefs::instance()->mMonthViewWeek )         
        timeSpan = 6;
    for( i = 0; i < timeSpan + 1; ++i ) {
        (*cells)[i]->startUpdateCell();
    }
 
    QPtrList<Event> events = calendar()->events();
    Event *event;
    QDateTime dt;
    QDate endDate =  mStartDate.addDays( timeSpan );
    for( event = events.first(); event; event = events.next() ) { // for event
        if ( event->doesRecur() ) {
            bool last;
            QDateTime incidenceStart = event->recurrence()->getPreviousDateTime( QDateTime( mStartDate ) , &last );
            QDateTime incidenceEnd;
            int eventlen = event->dtStart().date().daysTo ( event->dtEnd().date()  );
            bool invalid = false;
            while( true ) {
                    if ( incidenceStart.isValid() ) {
                        incidenceEnd = incidenceStart.addDays( eventlen );
                        int st =   incidenceStart.date().daysTo( endDate );
                        if ( st >= 0 ) { // start before timeend
                            int end = mStartDate.daysTo( incidenceEnd.date() );
                            if ( end >= 0 ) { // end after timestart --- got one!
                                //normalize
                                st =  timeSpan - st;
                                if ( st < 0 ) st = 0;
                                if ( end > timeSpan ) end = timeSpan;
                                int iii;
                                //qDebug("found %s %d %d ",event->summary().latin1(), st, end );
                                for ( iii = st;iii<= end;++iii) 
                                    (*cells)[iii]->insertEvent( event );
                            }
                        }
                    } else {
                        if ( invalid )
                            break;
                        invalid = true;
                        //qDebug("invalid %s", event->summary().latin1());
                        incidenceStart = QDateTime( mStartDate ).addSecs( -2 );;
                    }
                    if ( last )
                        break;
                    bool ok;
                    incidenceStart = event->getNextOccurence( incidenceStart.addSecs( 1 ) ,&ok );
                    if ( ! ok )
                        break;
                    if ( incidenceStart.date() > endDate )
                        break;
            }
        } else { // no recur 
            if ( !KOPrefs::instance()->mShowSyncEvents && event->uid().left(2) == QString("la") )
                if ( event->uid().left(15) == QString("last-syncEvent-")  )
                    continue;
            int st =  event->dtStart().date().daysTo( endDate );
            if ( st >= 0 ) { // start before timeend
                int end = mStartDate.daysTo( event->dtEnd().date() );
                if ( end >= 0 ) { // end after timestart --- got one!
                    //normalize
                    st =  timeSpan - st;
                    if ( st < 0 ) st = 0;
                    if ( end > timeSpan ) end = timeSpan;
                    int iii;
                    for ( iii = st;iii<= end;++iii)
                        (*cells)[iii]->insertEvent( event );
                }
            }
        }
    }
    // insert due todos
    QPtrList<Todo> todos = calendar()->todos( );
    Todo *todo;
    for(todo = todos.first(); todo; todo = todos.next()) {
        //insertTodo( todo );
        if ( todo->hasDueDate() ) {
            int day = mStartDate.daysTo( todo->dtDue().date() );
            if ( day >= 0 && day < timeSpan + 1)  {
                (*cells)[day]->insertTodo( todo );
            }
        }
    }

    for( i = 0; i < timeSpan+1; ++i ) {
        (*cells)[i]->finishUpdateCell();
    }
    processSelectionChange();
    //qApp->processEvents();
     for( i = 0; i < timeSpan+1; ++i ) {
         //(*cells)[i]->repaintfinishUpdateCell();
         QTimer::singleShot( 0, (*cells)[i], SLOT ( repaintfinishUpdateCell() ) );
    }
     setKeyBFocus();
#else
    // old code
    //qDebug("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ");
    int i;
    for( i = 0; i < (*cells).count(); ++i ) {
        (*cells)[i]->updateCell();
    }
 
    //qDebug("KOMonthView::updateView() ");
    processSelectionChange();
    // qDebug("---------------------------------------------------------------------+ ");
    (*cells)[0]->setFocus();
#endif

    //qDebug("update time %d ", ti.elapsed());
}

void KOMonthView::setKeyBoardFocus()
{
    //qDebug("KOMonthView::setKeyBoardFocus() ");
    bool shootAgain = false;
    if ( mShowWeekView ) {
        shootAgain = !mWeekLabelsW[1]->hasFocus();
         mWeekLabelsW[1]->setFocus();
    }
    else {
        shootAgain = !mWeekLabels[mNumWeeks]->hasFocus();
         mWeekLabels[mNumWeeks]->setFocus();
    }
    --mKBFcounter;
    if ( shootAgain && mKBFcounter > 0 ) {
      QTimer::singleShot( 50, this, SLOT ( setKeyBoardFocus() ) );
    }
}
void KOMonthView::setKeyBFocus()
{
    //qDebug("KOMonthView::setKeyBFocus() ");
    mKBFcounter = 10;
    QTimer::singleShot( 0, this, SLOT ( setKeyBoardFocus() ) );
}
void KOMonthView::resizeEvent(QResizeEvent * e)
{
    //qDebug("KOMonthView::resizeEvent %d %d -- %d %d ", e->size().width(), e->size().height(), e->oldSize().width(), e->oldSize().height());
    if ( isVisible() ) {
        //qDebug("KOMonthView::isVisible ");
        slotComputeLayout();
    } else
        mComputeLayoutTimer->start( 100 );
    if ( e )
        KOEventView::resizeEvent( e );
}

void KOMonthView::slotComputeLayout()
{
    mComputeLayoutTimer->stop();
    //qDebug("KOMonthView::Post - resizeEvent %d %d ", width(), height() );
    computeLayout();
    clPending = true;
    setKeyBFocus();
}

void KOMonthView::doComputeLayoutWeek()
{

    int daysToShow;
    bool combinedSatSun  = false;
    if (mShowSatSunComp = KOPrefs::instance()->mMonthViewSatSunTog ) {
        daysToShow = 6;
        combinedSatSun  = true;
    }
    int wid = width();//e
    int hei = height()-1-mNavigatorBar->height();
#ifdef DESKTOP_VERSION
    if ( !KOPrefs::instance()->mMonthViewWeekRowlayout ) {
        daysToShow = 2;
    } else 
#endif
        {
        if ( wid < hei + 40 )
            daysToShow = 2;
        else
            daysToShow = 3;
    }
    bool landscape = (daysToShow == 3);
    mShowSatSunComp = true;
    combinedSatSun  = true;
        
    //qDebug("KOMonthView::computeLayout() WWW ------------------------------------ ");
    QFontMetrics fm ( mWeekLabels[0]->font() );
    int weeklabelwid = fm.width( "888" );
    wid -= weeklabelwid;

    int colWid = wid / daysToShow;
    int dayLabelHei = mDayLabelsW[0]->sizeHint().height();
    int cellHei = (hei - (5- daysToShow )*dayLabelHei) /(5- daysToShow ); 
    int colModulo = wid % daysToShow;
    int rowModulo = (hei- (5- daysToShow )*dayLabelHei) % daysToShow-1;
    //qDebug("rowmod %d ", rowModulo);
    int i;
    int x,y,w,h;
    x= 0;
    y= 0;
    w = colWid;
    h = dayLabelHei ;
    for ( i = 0; i < 7; i++) {
        if ( i && !( i % daysToShow) && i < 6) {
            y += hei/(5-daysToShow);
            x = 0;
            w =  colWid;
        }
        if ( ((i) % daysToShow) >= daysToShow-colModulo ) {
            ++w;
        }
        int xC,yC,wC,hC;
        if ( i >=  5 ) { 
            int wi = width() -  x - weeklabelwid;
            if ( i == 5 ) { 
                xC = x+weeklabelwid;
                yC = y;
                wC = wi/2+wi%2;
                hC = h;
            } else {
                xC = x+weeklabelwid;
                yC = y;
                wC = wi;
                hC = h;
            }
            x = x - w +  wi - (wi/2 );
        }
        else {
            int wi = w;  
            if ( !(( i+1) % daysToShow)) {
                wi = width() -  x - weeklabelwid;
            }
            xC = x+weeklabelwid;
            yC = y;
            wC = wi;
            hC = h;
        }
        mDayLabelsW[mapWeekLayout(i,landscape)]->setGeometry( xC,yC,wC,hC);


        x += w;                                   
    }
    x= 0;
    y= dayLabelHei;
    w = colWid;
    h = cellHei;
    int max = 0;
    int w_count = mCellsW.count();
    for ( i = 0; i < w_count; ++i) {
        if ( i > 6 ) {
            mCellsW[i]->hide();
            continue;
        }
        
        w = colWid;
        if ( ((i) % daysToShow) >= daysToShow-colModulo ) {
            ++w;
        }
        if ( i  == (daysToShow-1-rowModulo)*7)
            ++h; 
      
        int xC,yC,wC,hC;
        if ( i >= 5 ) {
            if ( i ==5 ) {
                max = h/2;
                xC = x+weeklabelwid;
                yC = y;
                wC = w;
                hC = max;
                x -= w ;y += h/2;
            } else {
                if ( ((i-1) % daysToShow) >= daysToShow-colModulo ) {
                    ++w;
                }
                max = h-h/2;
                xC = x+weeklabelwid;
                yC = y;
                wC = w;
                hC = max;
                y -= h/2;
            }
        } else {
            max = h;
            xC = x+weeklabelwid;
            yC = y;
            wC = w;
            hC = h;
        }
        mCellsW[mapWeekLayout(i,landscape)]->setGeometry ( xC,yC,wC,hC ); 
                
       
        x += w;
        if ( x + w/2 > wid ) {
            x = 0;
            y += h+dayLabelHei ;
        }
        //mCellsW[i]->dateLabel()->setMaximumHeight( max - mCellsW[i]->lineWidth()*2 );
    }
    y= dayLabelHei;
    h = cellHei ;
    mWeekLabelsW[0]->setGeometry( 0,y,weeklabelwid,hei-dayLabelHei);
    mWeekLabelsW[1]->setGeometry( 0,0,weeklabelwid,dayLabelHei);
    // qDebug("RRRRRRRRRRRRR %d %d  old %d %d", e->size().width(),e->size().height() , e->oldSize().width(),e->oldSize().height());
    //qDebug("parent %d %d ", topLevelWidget()->size().width(), topLevelWidget()->size().height());
    mShortDayLabelsW = mDayLabelsW[0]->width()-2 < mWidthLongDayLabel ;
    updateDayLabels();
    //bool forceUpdate = !updatePossible;
    updatePossible = true;
    //mWeekLabels[mNumWeeks]->setText( i18n("M"));
    //if ( forceUpdate )
    //  updateView();
}
void KOMonthView::computeLayoutWeek()
{
    static int lastWid = 0;
    static int lastHei = 0;
    int tWid = topLevelWidget()->size().width();
    int tHei = topLevelWidget()->size().height();
    int wid = width();//e
    int hei = height()-1-mNavigatorBar->height();
    if ( ((wid *3)/2) < tWid && (( hei *3) /2) < tHei  ) 
        return;

    if ( lastWid == width() && lastHei == height() ) {
        //qDebug("KOListWeekView::No compute layout needed ");
        return;
    }
    lastWid = width();
    lastHei = height();
    doComputeLayoutWeek();
}
int KOMonthView::mapWeekLayout( int  index, bool landscape )
{
    if ( KOPrefs::instance()->mMonthViewWeekRowlayout )
        return index;
    int diff = 0;
    if ( !landscape ) diff = 1;
     switch( index ) {
     case 0: 
     case 5: 
     case 6: 
         return index;
         break;
     case 1: 
         return 2+diff;
         break;
     case 2: 
         return 4-(3*diff);
         break;
     case 3: 
         return 1+(3*diff);
         break;
     case 4:
         return 3-diff; 
         break;
     default:
         qDebug("KO: Error in mapping week layout ");
         return index;
         break;
     }
     return index;
}
void KOMonthView::computeLayout()
{
  

    static int lastWid = 0;
    static int lastHei = 0;

    if ( mShowWeekView ){
            computeLayoutWeek();
            return;
    }
    int daysToShow = 7;
    bool combinedSatSun  = false;
    if (mShowSatSunComp = KOPrefs::instance()->mMonthViewSatSunTog ) {
        daysToShow = 6;
        combinedSatSun  = true;
    }
    int tWid = topLevelWidget()->size().width();
    int tHei = topLevelWidget()->size().height();

    int wid = width();//e
    int hei = height()-1-mNavigatorBar->height();

    if ( ((wid *3)/2) < tWid && (( hei *3) /2) < tHei  ) {
        return;  
    }
    if ( lastWid == width() && lastHei == height() ){
        //qDebug("KOMonthview::No compute layout needed ");
        return;
    }

    lastWid = width();
    lastHei =  height();
    //qDebug("KOMonthView::computeLayout() MMM ------------------- ");
    QFontMetrics fm ( mWeekLabels[0]->font() );
    int weeklabelwid = fm.width( "888" );
    wid -= weeklabelwid;

    int colWid = wid / daysToShow;
    int dayLabelHei = mDayLabels[0]->sizeHint().height();
    int cellHei = (hei - dayLabelHei) /6; 
    int colModulo = wid % daysToShow;
    int rowModulo = (hei- dayLabelHei) % 6;
    //qDebug("rowmod %d ", rowModulo);
    int i;
    int x,y,w,h;
    x= 0;
    y= 0;
    w = colWid;
    h = dayLabelHei ;
    for ( i = 0; i < 7; i++) {
        if ( i == daysToShow-colModulo )
            ++w;
        if ( combinedSatSun ) {
            if ( i >=  daysToShow-1 ) {

                if ( i == 6 )
                    mDayLabels[i]->setGeometry( x+weeklabelwid,y,width()-x-weeklabelwid,h);
                else
                    mDayLabels[i]->setGeometry( x+weeklabelwid,y,w/2,h);
                x -= w/2 ;
            }
            else
                mDayLabels[i]->setGeometry( x+weeklabelwid,y,w,h);
        } else
            mDayLabels[i]->setGeometry( x+weeklabelwid,y,w,h);
        x += w;                                   
    }
    x= 0;
    y= dayLabelHei;
    w = colWid;
    h = cellHei ;
    int max = 0;
    int mc_count = mCells.count();
    for ( i = 0; i < mc_count; ++i) {
        //qDebug("iii %d ", i);
        w = colWid;
        if ( ((i) % 7) >= 7-colModulo ) {
            ++w;
        }
        if ( i  == (6-rowModulo)*7)
            ++h; 
        if ( combinedSatSun ) {
            if ( (i)%7 >=  daysToShow-1 ) {
                if ( (i)%7 ==  daysToShow-1 ) {
                    w = width()-x-weeklabelwid;
                    max = h/2;
                    mCells[i]->setGeometry ( x+weeklabelwid,y,w,max ); 
                    x -= w ;y += h/2;
                } else {
                    w = width()-x-weeklabelwid;
                    max = h-h/2;
                    mCells[i]->setGeometry ( x+weeklabelwid,y,w,max ); 
                    y -= h/2;
                }
            } else {
                max = h;
                mCells[i]->setGeometry ( x+weeklabelwid,y,w,h ); 
            }
                
        }
        else {
            max = h;
            mCells[i]->setGeometry ( x+weeklabelwid,y,w,h );
        }
        x += w;
        if ( x + w/2 > wid ) {
            x = 0;
            y += h;
        } 
        //mCells[i]->dateLabel()->setMaximumHeight( max- mCells[i]->lineWidth()*2 );
    }
    y= dayLabelHei;
    h = cellHei ;
    for ( i = 0; i < 6; i++) {
        if ( i  == (6-rowModulo))
            ++h;
        mWeekLabels[i]->setGeometry( 0,y,weeklabelwid,h);
        y += h;                            
    }
    mWeekLabels[6]->setGeometry( 0,0,weeklabelwid,dayLabelHei);
    // qDebug("RRRRRRRRRRRRR %d %d  old %d %d", e->size().width(),e->size().height() , e->oldSize().width(),e->oldSize().height());
    //qDebug("parent %d %d ", topLevelWidget()->size().width(), topLevelWidget()->size().height());
    mShortDayLabelsM = mDayLabels[0]->width()-2 < mWidthLongDayLabel ;
    updateDayLabels();
    //bool forceUpdate = !updatePossible;
    updatePossible = true;
    //mWeekLabels[mNumWeeks]->setText( i18n("W"));
}

void KOMonthView::showContextMenu( Incidence *incidence )
{
    if( incidence )
        mContextMenu->showIncidencePopup(incidence);
    else {
        //qDebug("KOMonthView::showContextMenu ");
        mNewItemMenu->popup(QCursor::pos());
    }
    /*
      if( incidence && incidence->typeID() == eventID ) {
      Event *event = static_cast<Event *>(incidence);
      mContextMenu->showEventPopup(event);
      } else {
      kdDebug() << "MonthView::showContextMenu(): cast failed." << endl;
      }
    */
}
MonthViewCell * KOMonthView::selectedCell( )
{
    return mSelectedCell;
}
void KOMonthView::setSelectedCell( MonthViewCell *cell )
{
    //qDebug("KOMonthView::setSelectedCell %d", cell);
    if ( mSelectedCell && mSelectedCell != cell ) {
        MonthViewCell * mvc = mSelectedCell;
        mSelectedCell = cell;
        mvc->deselect();
    } else 
        mSelectedCell = cell;
    //  if ( mSelectedCell )
    //         mSelectedCell->select();
    if ( !mSelectedCell )
        emit incidenceSelected( 0 );
    else 
        emit incidenceSelected( mSelectedCell->selectedIncidence() );
}

void KOMonthView::processSelectionChange()
{
    QPtrList<Incidence> incidences = selectedIncidences();
    if (incidences.count() > 0) {
        emit incidenceSelected( incidences.first() );
    } else {
        emit incidenceSelected( 0 );
        clearSelection();
    }
}

void KOMonthView::clearSelection()
{
    if ( mSelectedCell ) {
        mSelectedCell->deselect();
        mSelectedCell = 0;
    }
}

void KOMonthView::keyReleaseEvent ( QKeyEvent * e)
{
    if ( !e->isAutoRepeat() ) {
        mFlagKeyPressed = false;
    }
}

void KOMonthView::keyPressEvent ( QKeyEvent * e )
{
   
    qApp->processEvents();
    if ( e->isAutoRepeat() && !mFlagKeyPressed ) {
        //e->ignore();
        e->accept();
        return;
    }
    if (! e->isAutoRepeat() )
        mFlagKeyPressed = true;
    switch(e->key()) {
    case Key_Up: 
        {
            if ( mShowWeekView ) {
                emit selectWeekNum ( currentWeek() - 1 );
            }
            else {
                emit prevMonth();
            }
        }
        e->accept();
        break;
    case Key_Down:
        {
            if ( mShowWeekView ) {
                emit selectWeekNum ( currentWeek() +1);
            }
            else {
                emit nextMonth();
            }
           
        }
        e->accept();
        break;
    case Key_Return:
    case Key_Enter:
        {
            selectInternalWeekNum ( currentWeek() );
        }
        e->accept();
        break;
    case Key_D:
        if ( mSelectedCell ) {
            mSelectedCell->showDay();
            e->accept();
        } else {
            e->ignore();     
        }
        break;
    default:
        e->ignore();     
        break;
    }
}

void KOMonthView::nextCell()
{
   focusNextPrevChild ( true ); 
}
void KOMonthView::prevCell()
{
    focusNextPrevChild ( false ); 
}

void KOMonthView::slotNewTodo()
{
    //qDebug("KOMonthView::slotNewTodo() ");
    if ( mPopupCell ){
        QDateTime dt( mPopupCell->date(), QTime( KOPrefs::instance()->mStartTime, 0 ) );
        emit newTodoSignal(dt,true);
    }
    mPopupCell = 0;
}
void KOMonthView::slotNewEvent()
{
    if ( mPopupCell ) {
        QDateTime dt( mPopupCell->date(), QTime( KOPrefs::instance()->mStartTime, 0 ) );
        emit newEventSignal( dt );
    }
    //qDebug("KOMonthView::slotNewEvent() ");
    mPopupCell = 0;
}

void KOMonthView::slotEditJournal()
{ 
    if (  mPopupCell )
        emit showJournalSignal( 7, mPopupCell->date() );
    //qDebug("KOMonthView::slotEditJournal() ");
    mPopupCell = 0;
}

void KOMonthView::setPopupCell( MonthViewCell * c)
{
    mPopupCell = c;
}
