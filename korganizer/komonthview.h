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

#ifndef _KOMONTHVIEW_H
#define _KOMONTHVIEW_H

#include <qlabel.h>
#include <qframe.h>
#include <qdatetime.h>
#include <qlistbox.h>
#include <qpoint.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qintdict.h>
#include <qpushbutton.h>
#include <qvaluelist.h>
#include <qptrvector.h>

#include <libkcal/calendar.h>
#include <libkcal/event.h>

#include "koeventview.h"
#include "navigatorbar.h"

#ifdef DESKTOP_VERSION
class QToolTipGroup; 
#endif

class KNOWhatsThis; 
class MonthViewCell; 
class KOWeekButton : public QPushButton
{
    Q_OBJECT
  public:
    KOWeekButton( QWidget *parent=0, const char *name=0 ) :
      QPushButton( parent, name) 
    {
      connect( this, SIGNAL( clicked() ),
               SLOT( bottonClicked() )); 
      mNumber = -1;
    }
    void setWeekNum ( int num ) {mNumber = num; setText( QString::number ( num ));}
    int getWeekNum() { return mNumber;}
    signals:
    void selectWeekNum ( int );
private: 
    void focusInEvent ( QFocusEvent * ){;}
    int mNumber;
    void keyPressEvent ( QKeyEvent * e )
    {
        e->ignore();     
    }

private slots :
    void bottonClicked() { if ( mNumber > 0 ) emit selectWeekNum ( mNumber ); }
};

class KNoScrollListBox: public QListBox
{
    Q_OBJECT
  public:
    KNoScrollListBox(QWidget *parent=0, const char *name=0);
    ~KNoScrollListBox();
    QString getWhatsThisText(QPoint p) ;

  signals:
    void shiftDown();
    void shiftUp();
    void rightClick();
    void nextCell();
    void prevCell();
    void highlightIncidence( Incidence * , MonthViewCell*, int );
 protected:
    bool mBlockDeselect;
   protected slots:
    void oneDown();
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *); 
    void focusInEvent ( QFocusEvent * );
    void focusOutEvent ( QFocusEvent * ); 

 private:
    bool resetOnFocusIn;
    KNOWhatsThis * mWT;
};


class MonthViewItem: public QListBoxItem
{
  public:
    MonthViewItem( Incidence *,const QString & title );
    void recycle( Incidence *incidence, const QString & s);
    void setRecur(bool on) { mRecur = on; }
    void setAlarm(bool on) { mAlarm = on; }
    void setReply(bool on) { mReply = on; }
    void setMoreInfo(bool on) { mInfo = on; }
    void setMultiDay(int type) { mMultiday = type; }
    int multiDay() { return mMultiday; }
    void setMultiDayPos(int type) { mdayPos = type; }
    int gettMultiDayPos() { return mdayPos; }
    void setBlockRepaint(bool on) { mblockRepaint = on; }
    bool setHighlighted( Incidence * );

    void setPalette(const QPalette &p) { mPalette = p; }
    QPalette palette() const { return mPalette; }
    bool setHighlightedFalse();
    Incidence *incidence() const { return mIncidence; }

  protected:
    virtual void paint(QPainter *);
    virtual int height(const QListBox *) const;
    virtual int width(const QListBox *) const;

  private:
    int mdayPos;
    bool isWeekItem;
    bool mblockRepaint;
    int mMultiday;
    bool mRecur;
    bool mAlarm;
    bool mReply;
    bool mInfo;
    bool mDisplayHighlighted;

    QPalette mPalette;
    QDate mDate;

    Incidence *mIncidence;
};


class KOMonthView;

class MonthViewCell : public KNoScrollListBox
{
    Q_OBJECT
  public:
    MonthViewCell(KOMonthView *,QWidget* );
    ~MonthViewCell() {mAvailItemList.setAutoDelete( true );}

    void setDate( const QDate & );
    QDate date() const;

    void setPrimary( bool );
    bool isPrimary() const;

    void setHoliday( bool );
    void setHoliday( const QString & );

    void updateCell();
    void startUpdateCell();
    void finishUpdateCell();
    int insertEvent(Event *);
    void insertTodo(Todo *);

    void updateConfig( bool bigFont = false );

    void enableScrollBars( bool );

    Incidence *selectedIncidence();
    QDate selectedIncidenceDate();
    QPushButton * dateLabel() { return mLabel; }
    bool doHighLight( Incidence *);
    void deselect();
    void select();
#ifdef DESKTOP_VERSION
    static QToolTipGroup *toolTipGroup(); 
#endif
  signals:
    void defaultAction( Incidence * );
    void newEventSignal( QDateTime );
    void showDaySignal( QDate );
    
  protected:
    QStringList mToolTip;
    void resizeEvent( QResizeEvent * );

public slots:
    void showDay();
    void deHighLight();
    void repaintfinishUpdateCell();
  protected slots: 
    void defaultAction( QListBoxItem * );
    void contextMenu( QListBoxItem * );
    void selection( QListBoxItem * );
    void cellClicked( QListBoxItem * );
    void newEvent();
    
  private:
    int mdayCount;
    QPtrList <MonthViewItem> mAvailItemList; 
    KOMonthView *mMonthView;
    int currentPalette; 
  
    QDate mDate;
    bool mPrimary;
    bool mHoliday;
    QString mHolidayString;
    
    //QLabel *mLabel;
    QPushButton *mLabel;
    //QListBox *mItemList;
#ifdef DESKTOP_VERSION
    static QToolTipGroup *mToolTipGroup; 
#endif 
    QSize mLabelSize;
    QSize mLabelBigSize;
    QPalette mHolidayPalette;
    QPalette mStandardPalette;
    QPalette mPrimaryPalette;
    QPalette mNonPrimaryPalette;
    void setMyPalette();
    QPalette  getPalette ();
    
};


class KOMonthView: public KOEventView
{
    Q_OBJECT
  public:
    KOMonthView(Calendar *cal, QWidget *parent = 0, const char *name = 0 );
    ~KOMonthView();

    /** Returns maximum number of days supported by the komonthview */
    virtual int maxDatesHint();

    /** Returns number of currently shown dates. */
    virtual int currentDateCount();

    /** returns the currently selected events */
    virtual QPtrList<Incidence> selectedIncidences();

    /** returns dates of the currently selected events */
    virtual DateList selectedDates();
#if 0
    virtual void printPreview(CalPrinter *calPrinter,
                              const QDate &, const QDate &);
#endif
    bool isMonthView() { return !mShowWeekView; }
    bool isUpdatePossible() { return updatePossible; }

    MonthViewCell * selectedCell();
    bool skipResize; 
    NavigatorBar* navigatorBar() { return mNavigatorBar ;}
    void clearList();
  public slots:
    void incidenceHighlighted( Incidence *, MonthViewCell*, int );
    void nextCell();
    void prevCell();
    virtual void updateView();
    virtual void updateConfig();
    virtual void showDates(const QDate &start, const QDate &end);
    virtual void showEvents(QPtrList<Event> eventList);

    void changeEventDisplay(Event *, int);

    void clearSelection();

    void showContextMenu( Incidence * );

    void setSelectedCell( MonthViewCell * );
    void setPopupCell( MonthViewCell * );
    void switchView();
    void setKeyBFocus();

  protected slots:
    void catChanged( Incidence * );
    void setKeyBoardFocus();
    void slotNewTodo();
    void slotNewEvent();
    void slotEditJournal();
    void slotComputeLayout();
    void selectInternalWeekNum ( int );
    void processSelectionChange();
 signals:
    void nextMonth();
    void prevMonth();
    void selectWeekNum ( int );
    void selectMonth ();
    void showDaySignal( QDate );
    void newTodoSignal( QDateTime, bool );
    void showJournalSignal( int,QDate );
  protected:
    void resizeEvent(QResizeEvent *);
    void viewChanged();
    void updateDayLabels();
    int mapWeekLayout( int, bool );

  private:
    int mKBFcounter;
    QTimer* mComputeLayoutTimer;
    NavigatorBar* mNavigatorBar;
    int currentWeek();
    bool clPending;
    QWidgetStack * mWidStack;
    QWidget* mMonthView;
    QWidget* mWeekView;
    bool mShowWeekView;
    bool updatePossible;
    int mDaysPerWeek;
    int mNumWeeks;
    int mNumCells;
    //bool mWeekStartsMonday;
    bool mShowSatSunComp;
    void computeLayout();
    void computeLayoutWeek();
    void doComputeLayoutWeek();

    QPtrVector<MonthViewCell> mCells;
    QPtrVector<QLabel> mDayLabels;
    QPtrVector<KOWeekButton> mWeekLabels;
    QPtrVector<MonthViewCell> mCellsW;
    QPtrVector<QLabel> mDayLabelsW;
    QPtrVector<KOWeekButton> mWeekLabelsW;

    bool mShortDayLabelsM;
    bool mShortDayLabelsW;
    int mWidthLongDayLabel;

    QDate mStartDate;

    MonthViewCell *mSelectedCell;
    MonthViewCell *mPopupCell;
    bool mFlagKeyPressed;
    KOEventPopupMenu *mContextMenu;
    QPopupMenu *mNewItemMenu;
    void keyPressEvent ( QKeyEvent * ) ;
    void keyReleaseEvent ( QKeyEvent * ) ;

};

#endif
