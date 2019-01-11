/*
    This file is part of KOrganizer.
    Copyright (c) 2000, 2001 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef KOTODOVIEW_H
#define KOTODOVIEW_H

#include <qfont.h>
#include <qfontmetrics.h>
#include <qlineedit.h>
#include <qptrlist.h>
#include <qstrlist.h>
#include <qlistbox.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qmap.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlistview.h>
#include <klistview.h>

#include <libkcal/calendar.h>
#include <libkcal/todo.h>

#include <korganizer/baseview.h>

#include "kotodoviewitem.h"
#include "koprefs.h"
#include "koglobals.h"
#include "datenavigator.h"

class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class KOTodoViewWhatsThis;
class KDateEdit;
class KOTimeEdit;

class DocPrefs;


class KOStopTodoPrefs : public QDialog
{
    Q_OBJECT
  public:
    KOStopTodoPrefs( Todo* todo, QWidget *parent=0, const char *name=0 ) ;
   
       
private slots:
    void doNotSave();
    void accept();
private:
    Todo* mTodo;
    QLineEdit* mComment;
    QDateTime mStop;
    KDateEdit *sde, *ede;
    KOTimeEdit *ste, *ete;

};

class KOStartTodoPrefs : public QDialog
{
    Q_OBJECT
  public:
    KOStartTodoPrefs( QString sum, QWidget *parent=0, const char *name=0 ) ;
 
    bool stopAll() { return mStopAll; }
private slots:
    void doStop();
private:
    bool mStopAll;

};

class KOTodoListView : public KListView
{
    Q_OBJECT
  public:
    KOTodoListView(Calendar *,QWidget *parent=0,const char *name=0);
    virtual ~KOTodoListView() {}

  signals:
    void paintNeeded();
    void todoDropped(Todo *, int);  
    void double_Clicked(QListViewItem *item);
    void reparentTodoSignal( Todo *,Todo * );
    void unparentTodoSignal(Todo *);
    void deleteTodo( Todo * );
  protected:
    void wheelEvent (QWheelEvent *e);
    void contentsDragEnterEvent(QDragEnterEvent *);
    void contentsDragMoveEvent(QDragMoveEvent *);
    void contentsDragLeaveEvent(QDragLeaveEvent *);
    void contentsDropEvent(QDropEvent *);

    void contentsMousePressEvent(QMouseEvent *);
    void contentsMouseMoveEvent(QMouseEvent *);
    void contentsMouseReleaseEvent(QMouseEvent *);
    void contentsMouseDoubleClickEvent(QMouseEvent *);

  private:
    void paintEvent(QPaintEvent * pevent);
    bool internalDrop;
    QString mName;
    Calendar *mCalendar;
    QPoint mPressPos;
    bool mMousePressed;
    QListViewItem *mOldCurrent;
    bool  mFlagKeyPressed;
    void keyPressEvent ( QKeyEvent * ) ;
    void keyReleaseEvent ( QKeyEvent * ) ;
};


/**
  This is the line-edit on top of the todoview for fast addition of new todos
*/
class KOQuickTodo : public QLineEdit
{
  public:
    KOQuickTodo(QWidget *parent=0);
  protected:
    void focusInEvent(QFocusEvent *ev);
    void focusOutEvent(QFocusEvent *ev);
};


/**
  This class provides a multi-column list view of todo events.

  @short multi-column list view of todo events.
  @author Cornelius Schumacher <schumacher@kde.org>
*/
class KOTodoView : public KOrg::BaseView
{
    Q_OBJECT
  public:
    KOTodoView(Calendar *, QWidget* parent=0, const char* name=0 );
    ~KOTodoView();

    QPtrList<Incidence> selectedIncidences();
    QPtrList<Todo> selectedTodos();

    DateList selectedDates()
      {DateList q;
       return q;}

    /** Return number of shown dates. TodoView does not show dates, */
    int currentDateCount() { return 0; }

    void printPreview(CalPrinter *calPrinter, const QDate &fd, const QDate &td);

    void setDocumentId( const QString & );

    void saveLayout(KConfig *config, const QString &group) const;
    void restoreLayout(KConfig *config, const QString &group);
    void setNavigator( DateNavigator* nav ) {mNavigator = nav;}
    QString getWhatsThisText(QPoint p);
    void clearList( bool saveCurrentItem = true );

  public slots:
    void updateView();
    void updateConfig();

    void changeEventDisplay(Event *, int);

    void showDates(const QDate &start, const QDate &end);
    void showEvents(QPtrList<Event> eventList);

    void clearSelection();
    void jumpToDate ();

    void editItem(QListViewItem *item);
    void showItem(QListViewItem *item,const QPoint &,int);
    void popupMenu(QListViewItem *item,const QPoint &,int);
    void newTodo();
    void newSubTodo();
    void unparentTodo();
    void reparentTodo();
    void showTodo();
    void editTodo();
    void cloneTodo();
    void cancelTodo();
    void moveTodo();
    void beamTodo();
    void deleteTodo();

    void setNewPriority(int);
    void setNewPercentage(int);
    void changedCategories(int);

    void setAllOpen();
    void setAllClose();
    void setAllFlat();
    void displayAllFlat();

    void purgeCompleted();
    void toggleCompleted();
    void toggleRunning();
    void toggleQuickTodo();
    void updateTodo( Todo *, int );

    void itemClicked(QListViewItem *);
    void itemStateChanged(QListViewItem *);
    void modified(bool);
    void itemDoubleClicked(QListViewItem *item);
    void resetFocusToList();
    void fillCategories ();
    void fillCal ();
    void changedCal (int);

  signals:
    void newTodoSignal();
    void newSubTodoSignal(Todo *);
    void unparentTodoSignal(Todo *);
    void reparentTodoSignal( Todo *,Todo * );
    void showTodoSignal(Todo *);

    void editTodoSignal(Todo *);
    void deleteTodoSignal(Todo *);
    void todoModifiedSignal (Todo *, int);

    void isModified(bool);
    void cloneTodoSignal( Incidence * );
    void cancelTodoSignal( Incidence * );
    void moveTodoSignal( Incidence * );
    void beamTodoSignal( Incidence * );
    void purgeCompletedSignal();

  protected slots:
    void toggleRunningItem();
    void toggleRunningItemQuick();
    void paintNeeded();
    void processSelectionChange();
    void addQuickTodo();
    void setTodoModified( Todo* );
    void todoModified(Todo *, int );

  private:
    bool mIsActiveWindow;
    void addQuickTodoPar( Todo * parentTodo);
    /*
     * the TodoEditor approach is rather unscaling in the long
     * run.
     * Korganizer keeps it in memory and we need to update
     * 1. make KOTodoViewItem a QObject again?
     * 2. add a public method for setting one todo modified?
     * 3. add a private method for setting a todo modified + friend here?
     *  -- zecke 2002-07-08
     */
    KOTodoViewWhatsThis* mKOTodoViewWhatsThis;
    friend class KOTodoListView;
    void paintEvent(QPaintEvent * pevent);
    bool mPendingUpdateBeforeRepaint;
    friend class KOTodoViewItem;
    QMap<Todo *,KOTodoViewItem *>::ConstIterator insertTodoItem(Todo *todo);
    void restoreItemState( QListViewItem * );

    bool checkTodo( Todo * );
    bool isFlatDisplay;
    void  setOpen( QListViewItem*, bool setOpen);
    KOTodoListView *mTodoListView;
    QPopupMenu *mItemPopupMenu;
    QPopupMenu *mPopupMenu;
    QPopupMenu *mPriorityPopupMenu;
    QPopupMenu *mPercentageCompletedPopupMenu;
    QPopupMenu *mCategoryPopupMenu;
    QPopupMenu *mCalPopupMenu;

    QMap<int, int> mPercentage;
    QMap<int, int> mPriority;
    QMap<int, QString> mCategory;
    KOTodoViewItem *mActiveItem;

    QMap<Todo *,KOTodoViewItem *> mTodoMap;
    QString mName;
    QWidget* mQuickBar;

    DocPrefs *mDocPrefs;
    QString mCurrentDoc;
    KOQuickTodo *mQuickAdd;
    bool mBlockUpdate;
    void keyPressEvent ( QKeyEvent * ) ;
    KOTodoViewItem * pendingSubtodo;
    DateNavigator* mNavigator;
    void storeCurrentItem();
    void resetCurrentItem();
    Incidence * mCurItem, *mCurItemRootParent, *mCurItemParent,*mCurItemAbove; 
    QPushButton * mNewSubBut;
};

#endif
