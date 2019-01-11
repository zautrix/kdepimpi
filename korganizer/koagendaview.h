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

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/
#ifndef KOAGENDAVIEW_H
#define KOAGENDAVIEW_H

#include <qscrollview.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qlayout.h>
#ifndef DESKTOP_VERSION
#include <qksplitter.h>
#else
#include <qsplitter.h>
#endif
#include <qmemarray.h>

#include "koeventview.h"

class QHBox;
class QFrame;
class QLabel;
class QPushButton;
class CalendarView;
class KOAgenda;
class KOAgendaItem;
class KConfig;
class KDGanttMinimizeSplitter;

class KOAgendaButton : public QPushButton
{
 Q_OBJECT
  public:
    KOAgendaButton( QWidget *parent=0, const char *name=0 ) :
      QPushButton( parent, name ) 
    {
        mNum = -3;
        setFlat( true );
        setFocusPolicy(NoFocus); 
        setSizePolicy(QSizePolicy( QSizePolicy::Expanding ,QSizePolicy::Expanding ));
        connect( this, SIGNAL( clicked() ), this, SLOT ( bClicked() ) );
    };
     
    QSize sizeHint () const { return QSize( 5,5) ;}
    void setNum( int n) { mNum = n; }
private slots:
    void bClicked() {emit numClicked( mNum);}
signals:
    void numClicked( int );
private:
    int mNum;
};

class TimeLabels : public QScrollView {
    Q_OBJECT
  public:
    TimeLabels(int rows,QWidget *parent=0,const char *name=0,WFlags f=0);

    void setCellHeight(int height);

    /** Calculates the minimum width */
    virtual int minimumWidth() const;

    /** updates widget's internal state */
    void updateConfig();

    /**  */
    void setAgenda(KOAgenda* agenda);

    /**  */
    virtual void paintEvent(QPaintEvent* e);
    void contentsMousePressEvent ( QMouseEvent * ) ;
    void contentsMouseReleaseEvent ( QMouseEvent * ); 
    void contentsMouseMoveEvent ( QMouseEvent * ); 

  public slots:
    /** update time label positions */
    void positionChanged();
 signals:
    void scaleChanged();
  protected:
    void drawContents(QPainter *p,int cx, int cy, int cw, int ch);

  private:
    QPixmap myPix;
    bool mRedrawNeeded;
    int mMiniWidth;
    int mMouseDownY;
    QString mOrgCap;
    int mRows;
    int mCellHeight;

    /**  */
    KOAgenda* mAgenda;
};

class EventIndicator : public QFrame {
    Q_OBJECT
  public:
    enum Location { Top, Bottom };
    EventIndicator(Location loc=Top,QWidget *parent=0,const char *name=0);
    virtual ~EventIndicator();

    void changeColumns(int columns);
    void setPaintWidget( KDGanttMinimizeSplitter* );
    void setXOffset( int );
    void enableColumn(int column, bool enable);

  protected:
    void drawContents(QPainter *);

  private:
    int mXOffset;
    KDGanttMinimizeSplitter* mPaintWidget;
    int mColumns;
    QHBox *mTopBox;
    QBoxLayout *mTopLayout;
    Location mLocation;
    QPixmap mPixmap;
    QMemArray<bool> mEnabled;
};

/**
  KOAgendaView is the agenda-like view used to display events in an one or
  multi-day view.
*/
class KOAgendaView : public KOEventView {
    Q_OBJECT
  public:
    KOAgendaView(Calendar *cal,QWidget *parent = 0,const char *name = 0 );
    virtual ~KOAgendaView();
    void setStartHour( int );
    void toggleAllDay();


    /** Returns maximum number of days supported by the koagendaview */
    virtual int maxDatesHint();

    /** Returns number of currently shown dates. */
    virtual int currentDateCount();

    /** returns the currently selected events */
    virtual QPtrList<Incidence> selectedIncidences();
    
    /** returns the currently selected events */
    virtual DateList selectedDates();

    /** Remove all events from view */
    void clearView();
    void clearList();
    KOAgenda *agenda() { return mAgenda;}
    virtual void printPreview(CalPrinter *calPrinter,
                              const QDate &, const QDate &);
      
    /** start-datetime of selection */                          
    QDateTime selectionStart() {return mTimeSpanBegin;} 
    /** end-datetime of selection */                          
    QDateTime selectionEnd() {return mTimeSpanEnd;}
    /** returns true if selection is for whole day */
    bool selectedIsAllDay() {return mTimeSpanInAllDay;}
    /** make selected start/end invalid */
    void deleteSelectedDateTime();
    void repaintAgenda();
  public slots:
    void setInitStartHour();
    virtual void updateView();
    virtual void updateConfig();
    virtual void showDates(const QDate &start, const QDate &end);
    virtual void showEvents(QPtrList<Event> eventList);

    void updateTodo( Todo *, int );
    void changeEventDisplay(Event *, int);

    void clearSelection();

    void newTodo(int gx,int gy);
    void newEvent(int gx,int gy);
    void newEvent(int gxStart, int gyStart, int gxEnd, int gyEnd);
    void newEventAllDay(int gx, int gy);
    void newTodoAllDay(int gx, int gy);

    void startDrag(Event *);

    void readSettings();
    void readSettings(KConfig *);
    void writeSettings(KConfig *);

    void setContentsPos(int y);

    void scrollOneHourUp();
    void scrollOneHourDown();
    void addToCalSlot(Incidence *, Incidence *);
    void slotShowDateView( int, int );
    void fillAgenda();
    void startIdleTimeout();

  signals:
    void showDateView( int, QDate );
    void newTodoSignal( QDateTime ,bool );
    void toggleExpand();
    void selectWeekNum( int );
    void todoMoved( Todo *, int );
    void incidenceChanged(Incidence * , int );
    // void cloneIncidenceSignal(Incidence *);

  protected:
    KOAgendaButton* getNewDaylabel();
    bool mBlockUpdating;
    int mUpcomingWidth;
    /** Fill agenda beginning with date startDate */
    void fillAgenda(const QDate &startDate);
    void resizeEvent( QResizeEvent* e );
    /** Fill agenda using the current set value for the start date */

    /** Create labels for the selected dates. */
    void createDayLabels();

    /**
      Set the masks on the agenda widgets indicating, which days are holidays.
    */
    void setHolidayMasks();

  protected slots:
    void slotIdleTimeout();
    void categoryChanged( Incidence * );
    void slotDaylabelClicked( int );
    /** Update event belonging to agenda item */
    void updateEventDates(KOAgendaItem *item, int mode = -1);
  //void updateMovedTodo();

    void updateEventIndicatorTop(int newY);
    void updateEventIndicatorBottom(int newY);
    
    /** Updates data for selected timespan */    
    void newTimeSpanSelected(int gxStart, int gyStart, int gxEnd, int gyEnd);
    /** Updates data for selected timespan for all day event*/    
    void newTimeSpanSelectedAllDay(int gxStart, int gyStart, int gxEnd, int gyEnd);

  private:
    bool flag_blockfillAgenda;
    QTimer* mIdleTimer;
    QDateTime mIdleStart;
    // view widgets
    QFrame *mDayLabels;
    QHBox *mDayLabelsFrame;
    QBoxLayout *mLayoutDayLabels;
    QFrame *mAllDayFrame;
    KOAgenda *mAllDayAgenda;
    KOAgenda *mAgenda;
    TimeLabels *mTimeLabels;
    QWidget *mDummyAllDayLeft;

    KDGanttMinimizeSplitter* mSplitterAgenda;
    QPushButton *mExpandButton;

    DateList mSelectedDates;  // List of dates to be displayed
    int mViewType;

    bool mWeekStartsMonday;
    int mStartHour;

    KOEventPopupMenu *mAllAgendaPopup;
    //KOEventPopupMenu *mAllDayAgendaPopup;

    EventIndicator *mEventIndicatorTop;
    EventIndicator *mEventIndicatorBottom;

    QMemArray<int> mMinY;
    QMemArray<int> mMaxY;

    QMemArray<bool> mHolidayMask;
    
    QPtrList<KOAgendaButton> mDayLabelsList;
    QDateTime mTimeSpanBegin;
    QDateTime mTimeSpanEnd;
    bool mTimeSpanInAllDay;
    void keyPressEvent ( QKeyEvent * e ); 
};

#endif  // KOAGENDAVIEW_H
