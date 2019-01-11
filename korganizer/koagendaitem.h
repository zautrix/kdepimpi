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
#ifndef KOAGENDAITEM_H
#define KOAGENDAITEM_H

#include <qframe.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qpalette.h> 

#include <libkcal/incidence.h>

class KOAgendaItemWhatsThis;
class QToolTipGroup;
class QDragEnterEvent;
class QDropEvent;

using namespace KCal;

/*
  The KOAgendaItem has to make sure that it receives all mouse events, which are
  to be used for dragging and resizing. That means it has to be installed as
  eventfiler for its children, if it has children, and it has to pass mouse
  events from the cildren to itself. See eventFilter().
*/
class KOAgendaItem : public QWidget
{
    Q_OBJECT
  public:
    KOAgendaItem(Incidence *incidence, QDate qd, QWidget *parent, bool allday, const char *name=0,
                 WFlags f=0 );
    ~KOAgendaItem();
    QString getWhatsThisText();
    void init ( Incidence *incidence, QDate qd );
    int cellX() { return mCellX; }
    int cellXWidth() { return mCellXWidth; }
    int cellYTop() { return mCellYTop; }
    int cellYBottom() { return mCellYBottom; }
    int cellHeight();
    int cellWidth();
    int subCell() { return mSubCell; }
    int subCells() { return mSubCells; }

    void setCellXY(int X, int YTop, int YBottom);
    void setCellY(int YTop, int YBottom);
    void setCellX(int XLeft, int XRight);
    void setCellXWidth(int xwidth);
    void setSubCell(int subCell);
    void setSubCells(int subCells);

    /** Start movement */
    void startMove();
    /** Reset to original values */
    void resetMove();

    void moveRelative(int dx,int dy);
    void expandTop(int dy);
    void expandBottom(int dy);
    void expandLeft(int dx);
    void expandRight(int dx);
    int mLastMoveXPos;

    void setMultiItem(KOAgendaItem *first,KOAgendaItem *next,
                      KOAgendaItem *last);
    KOAgendaItem *firstMultiItem() { return mFirstMultiItem; }
    KOAgendaItem *nextMultiItem() { return mNextMultiItem; }
    KOAgendaItem *lastMultiItem() { return mLastMultiItem; }

    Incidence *incidence() const { return mIncidence; }
    QDate itemDate() { return mDate; }
    
    /** Update the date of this item's occurence (not in the event) */ 
    void setItemDate(QDate qd);
    
    void setText ( const QString & text ) { mDisplayedText = text; }
    QString text () { return mDisplayedText; }

    virtual bool eventFilter ( QObject *, QEvent * );

    static QToolTipGroup *toolTipGroup();
    
    QPtrList<KOAgendaItem> conflictItems();
    void setConflictItems(QPtrList<KOAgendaItem>);
    void addConflictItem(KOAgendaItem *ci);
    void paintMe( bool, QPainter* painter = 0 );
    void repaintMe();
    static QPixmap * paintPix();
    static QPixmap * paintPixAllday();
    void updateItem();
    void computeText();
    void recreateIncidence();
    bool checkLayout();
    void initColor ();
    bool isAllDay() { return mAllDay; }
    int resizeMe( int grid, int wid, int hei, bool invalidWidth = false );
  public slots:
    bool updateIcons( QPainter *, bool );
    void select(bool=true);
    void repaintItem();

 protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void paintEvent ( QPaintEvent * );
    void resizeEvent ( QResizeEvent *ev );

  private:
    KOAgendaItemWhatsThis* mKOAgendaItemWhatsThis;
    bool mAllDay;
    bool mWhiteText;
    int mCellX;
    int mCellXWidth;
    int mCellYTop,mCellYBottom;
    int mSubCell;  // subcell number of this item
    int mSubCells;  // Total number of subcells in cell of this item
    int xPaintCoord;
    int yPaintCoord;
    int wPaintCoord;
    int hPaintCoord;
    // Variables to remember start position
    int mStartCellX;
    int mStartCellXWidth;
    int mStartCellYTop,mStartCellYBottom;
    int mLastMovePos;

    // Multi item pointers
    KOAgendaItem *mFirstMultiItem;
    KOAgendaItem *mNextMultiItem;
    KOAgendaItem *mLastMultiItem;
    
    int mFontPixelSize;
    Incidence *mIncidence; // corresponding event or todo
    QDate mDate; //date this events occurs (for recurrence)
    //void showIcon( QLabel*, int );
    //QLabel *mTodoIconLabel;
    //QLabel *mItemLabel;
    //QWidget *mIconBox;
    //QLabel *mIconAlarm,*mIconRecur,*mIconReadonly;
    //QLabel *mIconReply,*mIconGroup,*mIconOrganizer;
    //QLabel *mIconMoreInfo;
    static QToolTipGroup *mToolTipGroup;

    QColor mBackgroundColor;
    QColorGroup mColorGroup;
    QString mDisplayedText;
    bool mSelected;
    QPtrList<KOAgendaItem> mConflictItems;
};

#endif // KOAGENDAITEM_H
