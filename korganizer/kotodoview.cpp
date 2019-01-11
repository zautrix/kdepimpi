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

#include <qlayout.h>
#include <qheader.h>
#include <qcursor.h>
#include <qwhatsthis.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <qinputdialog.h>

#include <qvbox.h>
#include <kdebug.h>
#include "koprefs.h"
#include <klocale.h>
#include <kglobal.h>
#include <kdateedit.h>
#include "ktimeedit.h"
#include <kiconloader.h>
#include <kmessagebox.h>

#include <libkcal/icaldrag.h>
#include <libkcal/vcaldrag.h>
#include <libkcal/calfilter.h>
#include <libkcal/dndfactory.h>
#include <libkcal/calendarresources.h>
#include <libkcal/resourcecalendar.h>
#include <kresources/resourceselectdialog.h>
#include <libkcal/kincidenceformatter.h>
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif
#ifndef KORG_NOPRINTER
#include "calprinter.h"
#endif
#include "docprefs.h"

#include "kotodoview.h"
using namespace KOrg;


KOStartTodoPrefs::KOStartTodoPrefs( QString sum, QWidget *parent, const char *name ) :
    QDialog( parent, name, true ) 
{
    mStopAll = true;
    setCaption( i18n("Start todo") );
    QVBoxLayout* lay = new QVBoxLayout( this );
    lay->setSpacing( 3 );
    lay->setMargin( 3 );
    QLabel * lab = new QLabel( i18n("<b>%1\n</b>").arg( sum ), this );
    lay->addWidget( lab ); 
    lab->setAlignment( AlignCenter );

    QPushButton * ok = new QPushButton( i18n("Start this todo\nand stop all running"), this );
    lay->addWidget( ok );
    ok->setDefault( true );
    QPushButton * start = new QPushButton( i18n("Start todo"), this );
    lay->addWidget( start );
    QPushButton * cancel = new QPushButton( i18n("Cancel - do not start"), this );
    lay->addWidget( cancel );
    connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
    connect ( start,SIGNAL(clicked() ),this , SLOT ( doStop() )  );
    connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
    resize( sizeHint() );
        
}
void KOStartTodoPrefs::doStop()
{
    mStopAll = false;
    accept();
}      
KOStopTodoPrefs::KOStopTodoPrefs( Todo* todo, QWidget *parent, const char *name ) :
    QDialog( parent, name, true ) 
{
    mTodo = todo;
    setCaption( i18n("Stop todo") );
    QVBoxLayout* lay = new QVBoxLayout( this );
    lay->setSpacing( 3 );
    lay->setMargin( 3 );
    QLabel * lab = new QLabel( i18n("<b>%1\n</b>").arg( todo->summary() ), this );
    lay->addWidget( lab ); 
    lab->setAlignment( AlignHCenter );
    lab = new QLabel( i18n("Additional Comment:"), this );
    lay->addWidget( lab ); 
    mComment = new QLineEdit( this );
    lay->addWidget( mComment );
    QHBox * start = new QHBox ( this );
    lay->addWidget( start ); 
    lab = new QLabel( i18n("Start:"), start );
    QHBox * end = new QHBox ( this );
    lay->addWidget( end ); 
    lab = new QLabel( i18n("End:"), end );
    sde = new KDateEdit( start );
    ste = new KOTimeEdit( start );
    connect ( sde,SIGNAL(setTimeTo( QTime ) ),ste , SLOT ( setTime(QTime  ) )  );
    ede = new KDateEdit( end );
    ete = new KOTimeEdit(end );
    connect ( ede,SIGNAL(setTimeTo( QTime ) ),ete , SLOT ( setTime(QTime  ) )  );
    sde->setDate( mTodo->runStart().date() );
    ste->setTime( mTodo->runStart().time() );
    mStop = QDateTime::currentDateTime();
    ede->setDate( mStop.date());
    ete->setTime( mStop.time() );
    QPushButton * ok = new QPushButton( i18n("Stop and save"), this );
    lay->addWidget( ok );
    ok->setDefault( true );
    QPushButton * cancel = new QPushButton( i18n("Continue running"), this );
    lay->addWidget( cancel );
    connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
    connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
    ok = new QPushButton( i18n("Stop - do not save"), this );
    connect ( ok,SIGNAL(clicked() ),this , SLOT ( doNotSave() )  );
    lay->addWidget( ok );
    if (QApplication::desktop()->width() < 320 )
        resize( 240, sizeHint().height() );
    else
        resize( 320, sizeHint().height() );
        
}
       
void KOStopTodoPrefs::accept()
{
    QDateTime start = QDateTime( sde->date(), ste->getTime() );
    QDateTime stop = QDateTime( ede->date(), ete->getTime() );
    if ( start > stop ) {
        KMessageBox::sorry(this,
                           i18n("The start time is\nafter the end time!"),
                           i18n("Time mismatch!"));
        return;
    }
    // restoring the seconds
    if ( start == QDateTime( mTodo->runStart().date(), 
                             QTime(mTodo->runStart().time().hour() , mTodo->runStart().time().minute() ) ) )
        start = mTodo->runStart();
    if ( stop == QDateTime( mStop.date(), 
                            QTime( mStop.time().hour() , mStop.time().minute() ) ) )
        stop = mStop;
    
    mTodo->saveRunningInfo( mComment->text(), start, stop );
    QDialog::accept();
}
void KOStopTodoPrefs::doNotSave() 
{
    int result = KMessageBox::warningContinueCancel(this,
                                                    i18n("Do you really want to set\nthe state to stopped\nwithout saving the data?"),mTodo->summary(),i18n("Yes, stop todo") );
    if (result != KMessageBox::Continue) return;
    mTodo->stopRunning();
    QDialog::accept();
}


class KOTodoViewWhatsThis :public QWhatsThis
{
public:
  KOTodoViewWhatsThis( QWidget *wid, KOTodoView* view ) : QWhatsThis( wid ), _wid(wid),_view (view) { };

protected:
  virtual QString text( const QPoint& p)
  {
    return _view->getWhatsThisText(p) ;
  }
private:
  QWidget* _wid;
  KOTodoView * _view;
};

KOTodoListView::KOTodoListView(Calendar *calendar,QWidget *parent,
                               const char *name) :
  KListView(parent,name)
{
  mName = QString ( name );
  mCalendar = calendar;
#ifndef DESKTOP_VERSION
  QPEApplication::setStylusOperation(viewport(), QPEApplication::RightOnHold );
#endif
  mOldCurrent = 0;
  mMousePressed = false;

  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
  int size = 16;
  if (qApp->desktop()->width() < 300 )
      size = 12;
  setTreeStepSize( size + 6 );

}

void KOTodoListView::contentsDragEnterEvent(QDragEnterEvent *e)
{
#ifndef KORG_NODND
//  kdDebug() << "KOTodoListView::contentsDragEnterEvent" << endl;
  if ( !ICalDrag::canDecode( e ) && !VCalDrag::canDecode( e ) &&
       !QTextDrag::canDecode( e ) ) {
    e->ignore();
    return;
  }

  mOldCurrent = currentItem();
#endif
}


void KOTodoListView::contentsDragMoveEvent(QDragMoveEvent *e)
{
#ifndef KORG_NODND
//  kdDebug() << "KOTodoListView::contentsDragMoveEvent" << endl;

  if ( !ICalDrag::canDecode( e ) && !VCalDrag::canDecode( e ) &&
       !QTextDrag::canDecode( e ) ) {
    e->ignore();
    return;
  }

  e->accept();
#endif
}

void KOTodoListView::contentsDragLeaveEvent(QDragLeaveEvent *)
{
#ifndef KORG_NODND
//  kdDebug() << "KOTodoListView::contentsDragLeaveEvent" << endl;

  setCurrentItem(mOldCurrent);
  setSelected(mOldCurrent,true);
#endif
}

void KOTodoListView::contentsDropEvent(QDropEvent *e)
{
#ifndef KORG_NODND
//  kdDebug() << "KOTodoListView::contentsDropEvent" << endl;

  if ( !ICalDrag::canDecode( e ) && !VCalDrag::canDecode( e ) &&
       !QTextDrag::canDecode( e ) ) {
    e->ignore();
    return;
  }

  DndFactory factory( mCalendar );
  Todo *todo = factory.createDropTodo(e);

  if (todo) {
    e->acceptAction();

    KOTodoViewItem *destination =
        (KOTodoViewItem *)itemAt(contentsToViewport(e->pos()));
    Todo *destinationEvent = 0;
    if (destination) destinationEvent = destination->todo();

    Todo *existingTodo = mCalendar->todo(todo->uid());

    if(existingTodo) {
      Incidence *to = destinationEvent;
      while(to) {
        if (to->uid() == todo->uid()) {
          KMessageBox::sorry(this,
              i18n("Cannot move Todo to itself\nor a child of itself"),
              i18n("Drop Todo"));
          delete todo;
          return;
        }
        to = to->relatedTo();
      }
      internalDrop = true;
      if ( destinationEvent )
          reparentTodoSignal( destinationEvent, existingTodo );
      else 
          unparentTodoSignal(existingTodo);
      delete todo;
    } else {
      mCalendar->addTodo(todo);
      emit todoDropped(todo, KOGlobals::EVENTADDED);
      if ( destinationEvent )
          reparentTodoSignal( destinationEvent, todo );
    }
  }
  else {
    QString text;
    if (QTextDrag::decode(e,text)) {
      //QListViewItem *qlvi = itemAt( contentsToViewport(e->pos()) );
      KOTodoViewItem *todoi = static_cast<KOTodoViewItem *>(itemAt( contentsToViewport(e->pos()) ));
      qDebug("Dropped : " + text);
      QStringList emails = QStringList::split(",",text);
      for(QStringList::ConstIterator it = emails.begin();it!=emails.end();++it) {
        int pos = (*it).find("<");
        QString name = (*it).left(pos);
        QString email = (*it).mid(pos);
        if (!email.isEmpty() && todoi) {
          todoi->todo()->addAttendee(new Attendee(name,email));
        }
      }
    }
    else {
        qDebug("KOTodoListView::contentsDropEvent(): Todo from drop not decodable ");
      e->ignore();
    }
  }
#endif
}
void KOTodoListView::wheelEvent (QWheelEvent *e)
{
    QListView::wheelEvent (e);
}

void KOTodoListView::contentsMousePressEvent(QMouseEvent* e)
{

    QPoint p(contentsToViewport(e->pos()));
    QListViewItem *i = itemAt(p);
    bool rootClicked = true;
    if (i) {
        // if the user clicked into the root decoration of the item, don't
        // try to start a drag!
        int X = p.x();
        //qDebug("%d %d %d", X, header()->sectionPos(0), treeStepSize() );
        if (X > header()->sectionPos(0) +
            treeStepSize() * (i->depth() + (rootIsDecorated() ? 1 : 0)) +
            itemMargin() +i->height()||
            X < header()->sectionPos(0)) {
            rootClicked = false;
        }
    } else {
        rootClicked = false;
    }
#ifndef KORG_NODND
    mMousePressed = false; 
    if (! rootClicked && !( e->button() == RightButton)  ) {
        mPressPos = e->pos();
        mMousePressed = true;
    } else {
        mMousePressed = false;
    }
#endif
    //qDebug("KOTodoListView::contentsMousePressEvent %d", rootClicked);
#ifndef DESKTOP_VERSION
    if (!( e->button() == RightButton && rootClicked) )
        QListView::contentsMousePressEvent(e);   
#else
    QListView::contentsMousePressEvent(e);   
#endif
}
void KOTodoListView::paintEvent(QPaintEvent* e)
{
    emit paintNeeded();
    QListView::paintEvent( e);
}
void KOTodoListView::contentsMouseMoveEvent(QMouseEvent* e)
{

#ifndef KORG_NODND
    //QListView::contentsMouseMoveEvent(e);
  if (mMousePressed && (mPressPos - e->pos()).manhattanLength() >
      QApplication::startDragDistance()*3) {
    mMousePressed = false;
    QListViewItem *item = itemAt(contentsToViewport(mPressPos));
    if (item) {
      DndFactory factory( mCalendar );
      ICalDrag *vd = factory.createDrag(
                          ((KOTodoViewItem *)item)->todo(),viewport());
      internalDrop = false;
      // we cannot do any senseful here, because the DnD is still broken in Qt
      if (vd->drag()) {
          if ( !internalDrop ) {
              //emit deleteTodo( ((KOTodoViewItem *)item)->todo() );
              qDebug("Dnd: External move: Delete drag source "); 
          } else
              qDebug("Dnd: Internal move ");
          
      } else {
          if ( !internalDrop ) {
              qDebug("Dnd: External Copy");
          } else
              qDebug("DnD: Internal copy: Copy pending");
      }
    }
  } 
#endif
}
void KOTodoListView::keyReleaseEvent ( QKeyEvent *e ) 
{
    if ( !e->isAutoRepeat() ) {
        mFlagKeyPressed = false;
    }
}


void KOTodoListView::keyPressEvent ( QKeyEvent * e )
{
    qApp->processEvents();
    if ( !isVisible() ) {
        e->ignore();
        return;
    }
    if ( e->isAutoRepeat() && !mFlagKeyPressed ) {
        e->ignore();
        // qDebug("  ignore  %d",e->isAutoRepeat()  );
        return;
    }
    if (! e->isAutoRepeat() )
        mFlagKeyPressed = true;
    QListViewItem* cn;
    if ( (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) && mName != "todolistsmall") {
        cn = currentItem();
        if ( cn ) {
            KOTodoViewItem* ci = (KOTodoViewItem*)( cn ); 
            if ( ci ){
                if ( e->state() == ShiftButton )
                    ci->setOn( false );
                else
                    ci->setOn( true );
                cn = cn->itemBelow();
                if ( cn ) {
                    setCurrentItem ( cn );
                    ensureItemVisible ( cn );
                }
                
            }
        }
        
        e->accept();
        return;
    }
   
    if ( e->state() == Qt::ControlButton || e->state() == Qt::ShiftButton || mName != "todolistsmall"  ) {
        switch ( e->key() ) {
        case Qt::Key_Down:
        case Qt::Key_Up:
            QListView::keyPressEvent (  e );
            e->accept();
            break; 
        case Qt::Key_Left:
        case Qt::Key_Right:
            QListView::keyPressEvent (  e );
            e->accept();
            return;
            break; 
        default:
            e->ignore();
            break;
        }
        return;
    }
    e->ignore();
}
void KOTodoListView::contentsMouseReleaseEvent(QMouseEvent *e)
{
  QListView::contentsMouseReleaseEvent(e);
  mMousePressed = false;
}

void KOTodoListView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
  if (!e) return;

  QPoint vp = contentsToViewport(e->pos());

  QListViewItem *item = itemAt(vp);

  emit double_Clicked(item);
  if (!item) return;

  emit doubleClicked(item,vp,0);
}

/////////////////////////////////////////////////////////////////////////////

KOQuickTodo::KOQuickTodo(QWidget *parent) :
  QLineEdit(parent)
{
  setText(i18n("Click to add new Todo"));
  setFocusPolicy ( QWidget::ClickFocus );
}

void KOQuickTodo::focusInEvent(QFocusEvent *ev)
{
  if ( text()==i18n("Click to add new Todo") )
    setText("");
  QLineEdit::focusInEvent(ev);
}

void KOQuickTodo::focusOutEvent(QFocusEvent *ev)
{
  setText(i18n("Click to add new Todo"));
  QLineEdit::focusOutEvent(ev);
}

/////////////////////////////////////////////////////////////////////////////

KOTodoView::KOTodoView(Calendar *calendar,QWidget* parent,const char* name) :
  KOrg::BaseView(calendar,parent,name)
{ 
    mIsActiveWindow = false;
    mCurItem = 0;
    mCurItemRootParent = 0;
    mCurItemParent = 0;
    mCurItemAbove = 0; 
    mActiveItem = 0;
    mCategoryPopupMenu = 0;
    mPendingUpdateBeforeRepaint = false;
    isFlatDisplay = false;
  mNavigator = 0;
  QBoxLayout *topLayout = new QVBoxLayout(this);
  mName = QString ( name );
  mBlockUpdate = false;
  mQuickBar = new QWidget( this );
  topLayout->addWidget(mQuickBar);
  
  mQuickAdd = new KOQuickTodo(mQuickBar); 
  QBoxLayout *quickLayout = new QHBoxLayout(mQuickBar);
  quickLayout->addWidget( mQuickAdd );
  mNewSubBut =  new QPushButton( "sub",mQuickBar );
  QPushButton * s_done = new QPushButton( "D",mQuickBar );
  QPushButton * s_run = new QPushButton( "R",mQuickBar );
  QPushButton * allopen = new QPushButton( "O",mQuickBar );
  QPushButton * allclose = new QPushButton( "C",mQuickBar );
  QPushButton * flat = new QPushButton( "F",mQuickBar );

  int fixwid = mQuickAdd->sizeHint().height();
  int fixhei = fixwid;
  if ( QApplication::desktop()->width() > 800 )
      fixwid = (fixwid*3)/2;
  connect ( flat, SIGNAL ( clicked()), SLOT ( setAllFlat()));
  connect ( allopen, SIGNAL ( clicked()), SLOT ( setAllOpen()));
  connect ( allclose, SIGNAL ( clicked()), SLOT ( setAllClose()));
  s_done->setPixmap( SmallIcon("greenhook16"));
  connect ( s_done, SIGNAL ( clicked()), SLOT ( toggleCompleted()));
  s_run->setPixmap( SmallIcon("ko16old"));
  connect ( s_run, SIGNAL ( clicked()), SLOT ( toggleRunning()));

  connect ( mNewSubBut, SIGNAL ( clicked()), SLOT ( newSubTodo()));

  mNewSubBut->setFixedWidth(mNewSubBut->sizeHint().width() );
  mNewSubBut->setEnabled( false );
  flat->setFixedWidth( fixwid );
  s_done->setFixedWidth( fixwid );
  allopen->setFixedWidth( fixwid );
  allclose->setFixedWidth( fixwid );
  s_run->setFixedWidth( fixwid );

  flat->setFixedHeight(fixhei );
  s_done->setFixedHeight(fixhei );
  allopen->setFixedHeight(fixhei );
  allclose->setFixedHeight(fixhei );
  s_run->setFixedHeight(fixhei );
  mNewSubBut->setFixedHeight(fixhei );

  flat->setFocusPolicy( NoFocus );
  s_done->setFocusPolicy( NoFocus );
  allopen->setFocusPolicy( NoFocus );
  allclose->setFocusPolicy( NoFocus );
  s_run->setFocusPolicy( NoFocus );
  mNewSubBut->setFocusPolicy( NoFocus );
  
  QWhatsThis::add( flat, i18n("Click this button to display all todos in a <b>flat</b> hierarchy" ) );
  QWhatsThis::add( allopen, i18n("Click this button to display all todos <b>openend</b>" ) );
  QWhatsThis::add( allclose, i18n("Click this button to display all todos <b>closed</b>" ) );
  QWhatsThis::add( s_run, i18n("Click this button to toggle show/hide <b>running</b> todos" ) );
  QWhatsThis::add( mNewSubBut, i18n("Click this button to add a new subtodo to the currently selected todo" ) );
  QWhatsThis::add( s_done, i18n("Click this button to toggle show/hide <b>completed</b> todos" ) );

  quickLayout->addWidget( mNewSubBut );
  quickLayout->addWidget( s_done );
  quickLayout->addWidget( s_run );
  quickLayout->addWidget( allopen );
  quickLayout->addWidget( allclose );
  quickLayout->addWidget( flat );

  if ( !KOPrefs::instance()->mEnableQuickTodo ) mQuickBar->hide();

  mTodoListView = new KOTodoListView(calendar,this, name );
  topLayout->addWidget(mTodoListView);
  //mTodoListView->header()->setMaximumHeight(30);
  mTodoListView->setRootIsDecorated(true);
  mTodoListView->setAllColumnsShowFocus(true);

  mTodoListView->setShowSortIndicator(true);

  mTodoListView->addColumn(i18n("Todo"));
  mTodoListView->addColumn(i18n("Prio"));
  mTodoListView->setColumnAlignment(1,AlignHCenter);
  mTodoListView->addColumn(i18n("Complete"));
  mTodoListView->setColumnAlignment(2,AlignCenter);

  mTodoListView->addColumn(i18n("Due Date"));
  mTodoListView->setColumnAlignment(3,AlignLeft);
  mTodoListView->addColumn(i18n("Due Time"));
  mTodoListView->setColumnAlignment(4,AlignHCenter);

  mTodoListView->addColumn(i18n("Start Date"));
  mTodoListView->setColumnAlignment(5,AlignLeft);
  mTodoListView->addColumn(i18n("Start Time"));
  mTodoListView->setColumnAlignment(6,AlignHCenter);

  //mTodoListView->addColumn(i18n("Cancelled"));
  mTodoListView->addColumn(i18n("Categories"));
  mTodoListView->addColumn(i18n("Calendar"));
  mTodoListView->addColumn(i18n("Last Modified"));
  mTodoListView->addColumn(i18n("Created"));
  mTodoListView->addColumn(i18n("Last Modified Sub"));
#if 0
  mTodoListView->addColumn(i18n("Sort Id"));
  mTodoListView->setColumnAlignment(4,AlignHCenter);
#endif

  mTodoListView->setMinimumHeight( 60 );
  mTodoListView->setItemsRenameable( true );
  mTodoListView->setRenameable( 0 );
  mTodoListView->setColumnWidth( 0, 120 );
  int iii = 0;
  for ( iii = 0; iii< 12 ; ++iii )
      mTodoListView->setColumnWidthMode( iii, QListView::Manual );


  mKOTodoViewWhatsThis = new KOTodoViewWhatsThis(mTodoListView->viewport(),this);

  mPriorityPopupMenu = new QPopupMenu(this);
  for (int i = 1; i <= 5; i++) {
    QString label = QString ("%1").arg (i);
    mPriority[mPriorityPopupMenu->insertItem (label)] = i;
  }
  connect (mPriorityPopupMenu, SIGNAL(activated (int)), SLOT (setNewPriority(int)));

  mPercentageCompletedPopupMenu = new QPopupMenu(this);
  for (int i = 0; i <= 100; i+=20) {
    QString label = QString ("%1 %").arg (i);
    mPercentage[mPercentageCompletedPopupMenu->insertItem (label)] = i;
  }
  connect (mPercentageCompletedPopupMenu, SIGNAL (activated (int)), SLOT (setNewPercentage (int)));


  mCategoryPopupMenu = new QPopupMenu (this);
  mCategoryPopupMenu->setCheckable (true);
  connect (mCategoryPopupMenu, SIGNAL (activated (int)), SLOT (changedCategories (int)));
  connect (mCategoryPopupMenu, SIGNAL (aboutToShow ()), SLOT (fillCategories ()));

  mCalPopupMenu = new QPopupMenu (this);
  mCalPopupMenu->setCheckable (true);
  connect (mCalPopupMenu, SIGNAL (activated (int)), SLOT (changedCal (int)));
  connect (mCalPopupMenu, SIGNAL (aboutToShow ()), SLOT (fillCal ()));



  
  mItemPopupMenu = new QPopupMenu(this); 
  mItemPopupMenu->insertItem(i18n("Show"), this,
                             SLOT (showTodo()));
  mItemPopupMenu->insertItem(i18n("Edit..."), this,
                             SLOT (editTodo()));
  mItemPopupMenu->insertItem( i18n("Delete..."), this,
                             SLOT (deleteTodo()));
  mItemPopupMenu->insertItem( i18n("Clone..."), this,
                             SLOT (cloneTodo()));
  mItemPopupMenu->insertItem( i18n("Move..."), this,
                             SLOT (moveTodo()));
#ifndef DESKTOP_VERSION
  mItemPopupMenu->insertItem( i18n("Beam..."), this,
                             SLOT (beamTodo()));
#endif
  mItemPopupMenu->insertItem( i18n("Toggle Cancel"), this,
                             SLOT (cancelTodo()));
  mItemPopupMenu->insertItem( i18n("Categories"), mCategoryPopupMenu);
  mItemPopupMenu->insertItem( i18n("Calendar"), mCalPopupMenu);
  mItemPopupMenu->insertSeparator();
  mItemPopupMenu->insertItem( i18n("Start/Stop todo..."), this,
                             SLOT (toggleRunningItem()));
  mItemPopupMenu->insertSeparator();
  /*
  mItemPopupMenu->insertItem( i18n("New Todo..."), this,
                             SLOT (newTodo()));
  */
  mItemPopupMenu->insertItem(i18n("New Sub-Todo..."), this,
                             SLOT (newSubTodo())); 
  mItemPopupMenu->insertItem(i18n("Unparent Todo"), this,
                             SLOT (unparentTodo()),0,21);
  mItemPopupMenu->insertItem(i18n("Reparent Todo"), this,
                             SLOT (reparentTodo()),0,22);
  mItemPopupMenu->insertSeparator();
#if 0
  mItemPopupMenu->insertItem(i18n("Delete completed To-Dos","Purge Completed..."),
                             this, SLOT( purgeCompleted() ) );
  mItemPopupMenu->insertItem(i18n("toggle completed To-Dos","Show Completed"),
                             this, SLOT( toggleCompleted() ),0, 33 );
  mItemPopupMenu->insertItem(i18n("toggle quick todo","Show Quick Todo"),
                             this, SLOT( toggleQuickTodo() ),0, 34 );
  mItemPopupMenu->insertItem(i18n("toggle running todo","Hide not Running"),
                             this, SLOT( toggleRunning() ),0, 35 );
  
#endif
  mPopupMenu = new QPopupMenu(this);
  mPopupMenu->insertItem(SmallIconSet("todo"), i18n("New Todo..."), this,
                         SLOT (newTodo()),0,1);
  mPopupMenu->insertItem(i18n("delete completed To-Dos","Purge Completed..."),
                         this, SLOT(purgeCompleted()),0,2);
  mPopupMenu->insertItem(i18n("Show Completed"),
                             this, SLOT( toggleCompleted() ),0,3 );
  mPopupMenu->insertItem(i18n("toggle running todo","Hide not Running"),
                             this, SLOT( toggleRunning() ),0,5 );
  mPopupMenu->insertItem(i18n(" set all open","Display all opened"),
                             this, SLOT( setAllOpen() ),0,6 );
  mPopupMenu->insertItem(i18n(" set all close","Display all closed"),
                             this, SLOT( setAllClose() ),0,7 );
  mPopupMenu->insertItem(i18n(" set all flat","Display all flat"),
                             this, SLOT( setAllFlat() ),0,8 );
  mPopupMenu->insertSeparator();
  mPopupMenu->insertItem(i18n("toggle quick todo","Show Quick Todo"),
                             this, SLOT( toggleQuickTodo() ),0,4 );
  mDocPrefs = new DocPrefs( name );
 
  mItemPopupMenu->insertItem(i18n("Todo View"),mPopupMenu );
  mPopupMenu->setCheckable( true );
  mItemPopupMenu->setCheckable( true );


  mPopupMenu->setItemChecked( 3,KOPrefs::instance()->mShowCompletedTodo  );
  mItemPopupMenu->setItemChecked( 33 , KOPrefs::instance()->mShowCompletedTodo   );

  mPopupMenu->setItemChecked(4,KOPrefs::instance()->mEnableQuickTodo);
  mItemPopupMenu->setItemChecked( 34 , KOPrefs::instance()->mEnableQuickTodo );

  mPopupMenu->setItemChecked(5,KOPrefs::instance()->mHideNonStartedTodos);
  mItemPopupMenu->setItemChecked( 35 , KOPrefs::instance()->mHideNonStartedTodos );


  // Double clicking conflicts with opening/closing the subtree
  connect( mTodoListView, SIGNAL( doubleClicked( QListViewItem *) ),
           SLOT( editItem( QListViewItem *) ) );
  /*
  connect( mTodoListView, SIGNAL(  rightButtonClicked ( QListViewItem *,
                                                        const QPoint &,int ) ),
           SLOT( popupMenu( QListViewItem *, const QPoint & ,int) ) );
  */ 
  connect( mTodoListView, SIGNAL(  contextRequest ( QListViewItem *,
                                                        const QPoint &,int ) ),
           SLOT( popupMenu( QListViewItem *, const QPoint & ,int) ) );
  connect( mTodoListView, SIGNAL( clicked( QListViewItem * ) ),
           SLOT( itemClicked( QListViewItem * ) ) );
  connect( mTodoListView, SIGNAL( double_Clicked( QListViewItem * ) ),
           SLOT( itemDoubleClicked( QListViewItem * ) ) );
  connect( mTodoListView, SIGNAL( todoDropped( Todo *, int ) ),
           SLOT( updateView() ) );
  connect( mTodoListView, SIGNAL( todoDropped( Todo *, int ) ),
           SLOT( todoModified(Todo *, int) ) );
  connect( mTodoListView, SIGNAL( expanded( QListViewItem * ) ),
           SLOT( itemStateChanged( QListViewItem * ) ) );
  connect( mTodoListView, SIGNAL( collapsed( QListViewItem * ) ),
           SLOT( itemStateChanged( QListViewItem * ) ) );
  connect( mTodoListView, SIGNAL( paintNeeded() ),
           SLOT( paintNeeded()) );

#if 0
  connect(mTodoListView,SIGNAL(selectionChanged(QListViewItem *)),
          SLOT(selectionChanged(QListViewItem *)));
  connect(mTodoListView,SIGNAL(clicked(QListViewItem *)),
          SLOT(selectionChanged(QListViewItem *)));
  connect(mTodoListView,SIGNAL(pressed(QListViewItem *)),
          SLOT(selectionChanged(QListViewItem *)));
#endif

  connect( mTodoListView, SIGNAL(reparentTodoSignal( Todo *,Todo * ) ), SIGNAL(reparentTodoSignal( Todo *,Todo * ) ));
  connect( mTodoListView, SIGNAL(unparentTodoSignal(Todo *)  ), SIGNAL(unparentTodoSignal(Todo *)  ));
  connect( mTodoListView, SIGNAL( deleteTodo(Todo *)  ), SIGNAL(deleteTodoSignal(Todo *)  ));

  connect( mTodoListView, SIGNAL(selectionChanged() ),
           SLOT( processSelectionChange() ) );
  connect( mQuickAdd, SIGNAL( returnPressed () ),
           SLOT( addQuickTodo() ) );
 
}

KOTodoView::~KOTodoView()
{

#if QT_VERSION >= 0x030000

#else
    delete mKOTodoViewWhatsThis;
#endif 

  delete mDocPrefs;
}
QString KOTodoView::getWhatsThisText(QPoint p)
{
    KOTodoViewItem* item = ( KOTodoViewItem* ) mTodoListView->itemAt( p );
    if ( item )
        return KIncidenceFormatter::instance()->getFormattedText( item->todo(),  
                                                                  KOPrefs::instance()->mWTshowDetails,
                                                                  KOPrefs::instance()->mWTshowCreated,
                                                                  KOPrefs::instance()->mWTshowChanged);
    return i18n("That is the todo view" );

}

void KOTodoView::jumpToDate ()
{
  //   if (mActiveItem) {
//     mActiveItem->todo());
//    if ( mActiveItem->todo()->hasDueDate() )
//       emit mActiveItem->todo()jumpToTime( mTodo->dtDue().date() );
}
void  KOTodoView::paintNeeded()
{
    if ( mPendingUpdateBeforeRepaint ) {
        updateView();
       mPendingUpdateBeforeRepaint = false;
    }
}
void  KOTodoView::paintEvent(QPaintEvent * pevent)
{
    if ( mPendingUpdateBeforeRepaint ) {
        updateView();
       mPendingUpdateBeforeRepaint = false;
    }
    KOrg::BaseView::paintEvent( pevent);
}

void KOTodoView::clearList( bool saveCurrentItem ) // default true 
{
   if ( mTodoListView->childCount() ) {
       if ( saveCurrentItem )
           storeCurrentItem();
       mTodoListView->clear();
       mTodoMap.clear();
    }
}
void KOTodoView::updateView()
{
    mActiveItem = 0;
    pendingSubtodo = 0;
    if ( mBlockUpdate ) {
        return;
    }
    if ( !isVisible() ) {
        clearList ();
        mPendingUpdateBeforeRepaint = true;
        return;
    }
    //qDebug("KOTodoView::updateView() %x", this);
    if ( isFlatDisplay ) {
        displayAllFlat();
        return;
    }
    //qDebug("update ");
//  kdDebug() << "KOTodoView::updateView()" << endl;
    QFont fo = KOPrefs::instance()->mTodoViewFont;
   
    clearList ();
   
    if ( mName == "todolistsmall" ) {
        if ( KOPrefs::instance()->mTodoViewUsesSmallFont ) {
            int ps = fo.pointSize() -2;
            if ( ps > 12 )
                ps -= 2;
            fo.setPointSize( ps );
        } 
    }
    
  mTodoListView->setFont( fo );
  // QFontMetrics fm ( KOPrefs::instance()->mTodoViewFont );
  //mTodoListView->header()->setMaximumHeight(fm.height());
  QPtrList<Todo> todoList = calendar()->todos();

/*
  kdDebug() << "KOTodoView::updateView(): Todo List:" << endl;
  Event *t;
  for(t = todoList.first(); t; t = todoList.next()) {
    kdDebug() << "  " << t->getSummary() << endl;

    if (t->getRelatedTo()) {
      kdDebug() << "      (related to " << t->getRelatedTo()->getSummary() << ")" << endl;
    }

    QPtrList<Event> l = t->getRelations();
    Event *c;
    for(c=l.first();c;c=l.next()) {
      kdDebug() << "    - relation: " << c->getSummary() << endl;
    }
  }
*/

  // Put for each Event a KOTodoViewItem in the list view. Don't rely on a
  // specific order of events. That means that we have to generate parent items
  // recursively for proper hierarchical display of Todos.
  Todo *todo;
  todo = todoList.first();// todo; todo = todoList.next()) {
  while ( todo ) {
      bool next = true;
      // qDebug("todo %s ", todo->summary().latin1());
      Incidence *incidence = todo->relatedTo();
      while  ( incidence  ) {
          if ( incidence->typeID() == todoID ) {
              //qDebug("related %s ",incidence->summary().latin1() );
              if ( !(todoList.contains ( ((Todo* )incidence ) ) )  && incidence->calEnabled() ) {
                  //qDebug("related not found ");
                  todoList.remove( );
                  todo = todoList.current();
                  next = false;
                  incidence = 0;

              } else {
                  //qDebug("related found  ");
                  incidence = incidence->relatedTo();
              }
          } else 
              incidence = 0;
      }
      if ( next )
          todo = todoList.next();
  }

  for(todo = todoList.first(); todo; todo = todoList.next()) {
      if (!mTodoMap.contains(todo)  && checkTodo( todo ) )
      {
      insertTodoItem(todo);
    }
  }
  // Restore opened/closed state
  mTodoListView->blockSignals( true );
  if( mDocPrefs ) restoreItemState( mTodoListView->firstChild() );
  mTodoListView->blockSignals( false );
  resetCurrentItem();
}

void KOTodoView::storeCurrentItem()
{
    mCurItem = 0;
    mCurItemRootParent = 0;
    mCurItemParent = 0;
    mCurItemAbove = 0; 
    mIsActiveWindow = topLevelWidget()->isActiveWindow();
    mActiveItem = (KOTodoViewItem*)mTodoListView->currentItem();
    if (mActiveItem) {
        mCurItem = mActiveItem->todo();
        KOTodoViewItem* activeItemAbove =  (KOTodoViewItem*)mActiveItem->itemAbove ();
        if ( activeItemAbove )
            mCurItemAbove = activeItemAbove->todo();
        mCurItemRootParent = mCurItem;
        mCurItemParent = mCurItemRootParent->relatedTo();
        while ( mCurItemRootParent->relatedTo() != 0 )
            mCurItemRootParent = mCurItemRootParent->relatedTo();
    }
    mActiveItem = 0;
}

void KOTodoView::resetCurrentItem()
{
    //mTodoListView->setFocus();
    KOTodoViewItem* foundItem = 0; 
    KOTodoViewItem* foundItemRoot = 0; 
    KOTodoViewItem* foundItemParent = 0; 
    KOTodoViewItem* foundItemAbove = 0; 
    if ( mTodoListView->firstChild ()  ) {
        if ( mCurItem ) {
            KOTodoViewItem* item = (KOTodoViewItem*)mTodoListView->firstChild ();
            while ( item  ) {
                if ( item->todo() ==  mCurItem ) {
                    foundItem = item; 
                    break;
                } else if ( item->todo() ==  mCurItemAbove ) {
                    foundItemAbove = item;

                } 
                if ( item->todo() ==  mCurItemRootParent ) {
                    foundItemRoot = item;
                }
                if ( item->todo() ==  mCurItemParent ) {
                    foundItemParent = item;
                }
                item = (KOTodoViewItem*)item->itemBelow();
            }
            if ( ! foundItem ) {
                if ( foundItemParent ) {
                    foundItem = foundItemParent;
                } else {
                    if ( foundItemRoot )
                        foundItem = foundItemRoot;
                    else
                        foundItem = foundItemAbove;
                }
            }  
        } 
        if ( foundItem ) {
            mTodoListView->setSelected (  foundItem, true  ); 
            mTodoListView->setCurrentItem( foundItem ); 
            mTodoListView->ensureItemVisible(  foundItem );
        } else { 
            if ( mTodoListView->firstChild () ) {
                mTodoListView->setSelected ( mTodoListView->firstChild (), true  ); 
                mTodoListView->setCurrentItem( mTodoListView->firstChild () );
            }
        }
    }
    processSelectionChange(); 
    if ( mName != "todolistsmall" )
        QTimer::singleShot( 100, this, SLOT ( resetFocusToList() ));
}
void KOTodoView::resetFocusToList()
{
    if ( mIsActiveWindow ) 
        topLevelWidget()->setActiveWindow();
    mTodoListView->setFocus();
}
//Incidence * mCurItem, *mCurItemRootParent,*mCurItemAbove; 
bool KOTodoView::checkTodo( Todo * todo )
{
   
    if (  !KOPrefs::instance()->mShowCompletedTodo &&  todo->isCompleted() )
        return false;
    if ( !todo->isCompleted()  ) {
        if ( todo->hasDueDate() && todo->dtDue().date() <= QDate::currentDate() )
            return true;
    }
    if ( KOPrefs::instance()->mHideNonStartedTodos && mNavigator ) {
        if ( todo->hasStartDate() ) 
            if ( mNavigator->selectedDates().last() < todo->dtStart().date() )
                return false;
        if ( todo->hasDueDate() ) 
            if ( mNavigator->selectedDates().first() > todo->dtDue().date() )
                return false;
    }
    return true;
}

void KOTodoView::restoreItemState( QListViewItem *item )
{
    pendingSubtodo = 0;
  while( item ) {
    KOTodoViewItem *todoItem = (KOTodoViewItem *)item;
    todoItem->setOpen( mDocPrefs->readBoolEntry( todoItem->todo()->uid() ) );
    if( item->childCount() > 0 ) restoreItemState( item->firstChild() );
    item = item->nextSibling();
  }
}


QMap<Todo *,KOTodoViewItem *>::ConstIterator
KOTodoView::insertTodoItem(Todo *todo)
{

    //  kdDebug() << "KOTodoView::insertTodoItem(): " << todo->getSummary() << endl;
    // TODO: Check, if dynmaic cast is necessary

    pendingSubtodo = 0;
    Incidence *incidence = todo->relatedTo();
    while ( incidence && !incidence->calEnabled() )
        incidence = incidence->relatedTo();
    if (incidence && incidence->typeID() == todoID ) {
        Todo *relatedTodo = static_cast<Todo *>(incidence);

        //    kdDebug() << "  has Related" << endl;
        QMap<Todo *,KOTodoViewItem *>::ConstIterator itemIterator;
        itemIterator = mTodoMap.find(relatedTodo);
        if (itemIterator == mTodoMap.end()) {
            //      kdDebug() << "    related not yet in list" << endl;
            itemIterator = insertTodoItem (relatedTodo);
        }
        // isn't this pretty stupid? We give one Todo  to the KOTodoViewItem
        // and one into the map. Sure finding is more easy but why? -zecke
        KOTodoViewItem *todoItem = new KOTodoViewItem(*itemIterator,todo,this);
        return mTodoMap.insert(todo,todoItem);
    } else {
        //    kdDebug() << "  no Related" << endl;
        // see above -zecke
        KOTodoViewItem *todoItem = new KOTodoViewItem(mTodoListView,todo,this);
        return mTodoMap.insert(todo,todoItem);
    }
}


void KOTodoView::updateConfig()
{
    updateView();
  mTodoListView->repaintContents();
}

QPtrList<Incidence> KOTodoView::selectedIncidences()
{
  QPtrList<Incidence> selected;

  KOTodoViewItem *item = (KOTodoViewItem *)(mTodoListView->selectedItem());
//  if (!item) item = mActiveItem;
  if (item) selected.append(item->todo());

  return selected;
}

QPtrList<Todo> KOTodoView::selectedTodos()
{
  QPtrList<Todo> selected;

  KOTodoViewItem *item = (KOTodoViewItem *)(mTodoListView->selectedItem());
//  if (!item) item = mActiveItem;
  if (item) selected.append(item->todo());

  return selected;
}

void KOTodoView::changeEventDisplay(Event *, int)
{
  updateView();
}

void KOTodoView::showDates(const QDate &, const QDate &)
{
}

void KOTodoView::showEvents(QPtrList<Event>)
{
  kdDebug() << "KOTodoView::selectEvents(): not yet implemented" << endl;
}

void KOTodoView::printPreview(CalPrinter *calPrinter, const QDate &fd,
                              const QDate &td)
{
#ifndef KORG_NOPRINTER
  calPrinter->preview(CalPrinter::Todolist, fd, td);
#endif
}

void KOTodoView::editItem(QListViewItem *item )
{
    if ( item ) 
        emit editTodoSignal(((KOTodoViewItem *)item)->todo());
}

void KOTodoView::showItem(QListViewItem *item,const QPoint &,int)
{
    if ( item ) 
        emit showTodoSignal(((KOTodoViewItem *)item)->todo());
}

void KOTodoView::popupMenu(QListViewItem *item,const QPoint &p,int column)
{
    pendingSubtodo = 0;
  mActiveItem = (KOTodoViewItem *)item;
  if (item) {
    switch (column){
    case 1:
      mPriorityPopupMenu->popup(QCursor::pos ()); break;
    case 2:
      mPercentageCompletedPopupMenu->popup(QCursor::pos ()); break;
    case 3:
         moveTodo();
        break;
    case 7:
      mCategoryPopupMenu->popup(QCursor::pos ()); break;
    case 8:
      mCalPopupMenu->popup(QCursor::pos ()); break;
    default:
            mItemPopupMenu->popup(QCursor::pos());
    }
 } else mPopupMenu->popup(QCursor::pos());
}
void KOTodoView::newTodo()
{
  emit newTodoSignal();
}

void KOTodoView::newSubTodo()
{
    mActiveItem = (KOTodoViewItem*)mTodoListView->currentItem();
  if (mActiveItem) {
      if ( mQuickAdd->isVisible() && !mQuickAdd->text().isEmpty() && mQuickAdd->text() != i18n("Click to add new Todo") ) {
          addQuickTodoPar( mActiveItem->todo());
      } else
          emit newSubTodoSignal(mActiveItem->todo());
  }
}
void KOTodoView::unparentTodo()
{
 if (mActiveItem) {
    emit unparentTodoSignal(mActiveItem->todo());
  }
}

void KOTodoView::reparentTodo()
{ 
    if (mActiveItem) {
        topLevelWidget()->setCaption(i18n("Click on new parent item"));
        pendingSubtodo = mActiveItem;
    }
}
void KOTodoView::editTodo()
{
  if (mActiveItem) {
    emit editTodoSignal(mActiveItem->todo());
  }
}
void KOTodoView::cloneTodo()
{
  if (mActiveItem) {
    emit cloneTodoSignal((Incidence*)mActiveItem->todo());
  }
}
void KOTodoView::cancelTodo()
{
  if (mActiveItem) {
    emit cancelTodoSignal((Incidence*)mActiveItem->todo());
  }
}
void KOTodoView::moveTodo()
{
  if (mActiveItem) {
    emit moveTodoSignal((Incidence*)mActiveItem->todo());
  }
}
void KOTodoView::beamTodo()
{
  if (mActiveItem) {
    emit beamTodoSignal((Incidence*)mActiveItem->todo());
  }
}


void KOTodoView::showTodo()
{
  if (mActiveItem) {
    emit showTodoSignal(mActiveItem->todo());
  }
}

void KOTodoView::deleteTodo()
{
  if (mActiveItem) {
      emit deleteTodoSignal(mActiveItem->todo());
  }
}

void KOTodoView::setNewPriority(int index)
{
  if (mActiveItem && !mActiveItem->todo()->isReadOnly ()) {
    mActiveItem->todo()->setPriority(mPriority[index]);
    mActiveItem->construct();
   todoModified (mActiveItem->todo(), KOGlobals::PRIORITY_MODIFIED);
   mActiveItem->todo()->setRevision( mActiveItem->todo()->revision()+1  );
  }
  processSelectionChange();
}

void KOTodoView::setNewPercentage(int index)
{
    if (mActiveItem && !mActiveItem->todo()->isReadOnly ()) {

        if ( mPercentage[index] == 100 && !mActiveItem->isOn() ) {
            mActiveItem->setOn( true );
            processSelectionChange();
            return;
        } else   if ( mPercentage[index] != 100 && mActiveItem->isOn() ) {
            KOTodoViewItem* par = (static_cast<KOTodoViewItem*>(mActiveItem->parent()));
            if ( par && par->isOn()  )
                par->setOn( false );
        }
        if (mPercentage[index] == 100) {
            mActiveItem->todo()->setCompleted(QDateTime::currentDateTime());
        } else {
            mActiveItem->todo()->setCompleted(false);
        }
        mActiveItem->todo()->setPercentComplete(mPercentage[index]);
        mActiveItem->construct();
        todoModified (mActiveItem->todo (), KOGlobals::COMPLETION_MODIFIED);
        mActiveItem->todo()->setRevision( mActiveItem->todo()->revision()+1  );
    }
  processSelectionChange();
}

void KOTodoView::fillCategories ()
{
    mCategoryPopupMenu->clear();
    if ( ! mActiveItem ) return;
    QStringList checkedCategories = mActiveItem->todo()->categories ();
    for (QStringList::Iterator it = KOPrefs::instance()->mCustomCategories.begin ();
         it != KOPrefs::instance()->mCustomCategories.end ();
         ++it) {
        int index = mCategoryPopupMenu->insertItem (*it);
        mCategory[index] = *it;
        if (checkedCategories.find (*it) != checkedCategories.end ()) mCategoryPopupMenu->setItemChecked (index, true);
    }
}
void KOTodoView::fillCal ()
{
    mCalPopupMenu->clear();
    if (!mActiveItem) return;
    bool readO = mActiveItem->todo()->isReadOnly();
    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    while ( kkf ) {
        int index = mCalPopupMenu->insertItem( kkf->mName, kkf->mCalNumber);
        if ( kkf->mErrorOnLoad || kkf->isReadOnly || readO )
            mCalPopupMenu->setItemEnabled( index, false );
        mCalPopupMenu->setItemChecked (index, kkf->mCalNumber == mActiveItem->todo()->calID());
        kkf = KOPrefs::instance()->mCalendars.next();
    }
}
void KOTodoView::changedCal (int index )
{
    if (!mActiveItem) return;
    mActiveItem->todo()->setCalID( index );
    mActiveItem->construct();
}
void KOTodoView::changedCategories(int index)
{
  if (mActiveItem && !mActiveItem->todo()->isReadOnly ()) {
    QStringList categories = mActiveItem->todo()->categories ();
    QString colcat = categories.first();
    if (categories.find (mCategory[index]) != categories.end ())
      categories.remove (mCategory[index]);
    else
      categories.insert (categories.end(), mCategory[index]);
    categories.sort ();
    if ( !colcat.isEmpty() ) {
        if ( categories.find ( colcat ) != categories.end () ) {
            categories.remove( colcat );
            categories.prepend( colcat );
        }
    }
    mActiveItem->todo()->setCategories (categories);
    mActiveItem->construct();
    mActiveItem->todo()->setRevision( mActiveItem->todo()->revision()+1  );
    todoModified (mActiveItem->todo (), KOGlobals::CATEGORY_MODIFIED);
  }
}
void KOTodoView::toggleRunningItemQuick()
{
    if ( !mActiveItem ) return;
    Todo * t = mActiveItem->todo();
    if ( t->isRunning() ) {
        if (  t->runTime()  < 15) {
            t->stopRunning();
            mActiveItem->construct();
            topLevelWidget()->setCaption(i18n("Todo stopped - no data saved because runtime was < 15 sec!"));
            return;
        }
        else
            toggleRunningItem();
        return;
    } else {
        t->setRunning( true );
        mActiveItem->construct();
        topLevelWidget()->setCaption(i18n("Todo started! Double click again to stop!"));
    }
}
void KOTodoView::itemDoubleClicked(QListViewItem *item)
{
    if ( pendingSubtodo != 0 ) {
        topLevelWidget()->setCaption(i18n("Reparenting aborted!"));
    }
    pendingSubtodo = 0;
    //int row = mTodoListView->header()->sectionAt ( mTodoListView->header()->mapFromGlobal( QCursor::pos()).x() );
    int row = mTodoListView->header()->sectionAt ( mTodoListView->viewportToContents(mTodoListView->viewport()->mapFromGlobal( QCursor::pos())) .x() );
    //qDebug("ROW %d ", row);
    if (!item) {
        newTodo();
        return;
    } else {
        if ( row == 1 ) {
            mActiveItem = (KOTodoViewItem *) item;
            newSubTodo();
            return;
        }
        if ( row == 5 || row == 6 || row == 2) {
            mActiveItem = (KOTodoViewItem *) item;
            toggleRunningItemQuick();
            return; 
        }
    }
    if ( KOPrefs::instance()->mEditOnDoubleClick )
        editItem( item );
    else
        showItem( item , QPoint(), 0 );
}
void KOTodoView::toggleRunningItem()
{
    // qDebug("KOTodoView::toggleRunning() ");
    if ( ! mActiveItem )
        return;
    Todo * t = mActiveItem->todo();
    if ( t->isRunning() ) {
        KOStopTodoPrefs tp ( t, this );
        if (QApplication::desktop()->width() <= 800 ){
            int wid = tp.width();
            int hei = tp.height();
            int xx = (QApplication::desktop()->width()-wid)/2;
            int yy = (QApplication::desktop()->height()-hei)/2;
            tp.setGeometry( xx,yy,wid,hei );
        }
        tp.exec();
        updateTodo ( t, KOGlobals::EVENTEDITED );
    } else {
        KOStartTodoPrefs tp ( t->summary(), this );
        if (QApplication::desktop()->width() <= 800 ){
            int wid = tp.width();
            int hei = tp.height();
            int xx = (QApplication::desktop()->width()-wid)/2;
            int yy = (QApplication::desktop()->height()-hei)/2;
            tp.setGeometry( xx,yy,wid,hei );
        }
        if ( !tp.exec() ) return;
        if ( tp.stopAll() ) {
            mCalendar->stopAllTodos();
            t->setRunning( true );
            updateView();
        } else {
            t->setRunning( true );
            updateTodo ( t, KOGlobals::EVENTEDITED );
        }
    }
}

void KOTodoView::itemClicked(QListViewItem *item)
{
    //qDebug("KOTodoView::itemClicked %d", item);
  if (!item) {
      if ( pendingSubtodo != 0 ) {
          topLevelWidget()->setCaption(i18n("Reparenting aborted!"));
      }
      pendingSubtodo = 0;
      return;
  }
  KOTodoViewItem *todoItem = (KOTodoViewItem *)item;
  if ( pendingSubtodo != 0 ) {
      bool allowReparent = true;
      QListViewItem *par = item;
      while ( par ) {
          if ( par ==  pendingSubtodo ) {
              allowReparent = false;
              break;
          }
          par = par->parent();
      }
      if ( !allowReparent  ) {
          topLevelWidget()->setCaption(i18n("Recursive reparenting not possible!"));
          pendingSubtodo = 0;
      } else {
          Todo* newParent = todoItem->todo();
          Todo* newSub = pendingSubtodo->todo();
          pendingSubtodo = 0;
          emit reparentTodoSignal( newParent,newSub  );
          return;
      }
  }

}

void KOTodoView::setDocumentId( const QString &id )
{

  mDocPrefs->setDoc( id );
}

void KOTodoView::itemStateChanged( QListViewItem *item )
{
  if (!item) return;

  KOTodoViewItem *todoItem = (KOTodoViewItem *)item;

//  kdDebug() << "KOTodoView::itemStateChanged(): " << todoItem->todo()->summary() << endl;

  if( mDocPrefs ) mDocPrefs->writeEntry( todoItem->todo()->uid(), todoItem->isOpen() );
}

void KOTodoView::saveLayout(KConfig *config, const QString &group) const
{
  mTodoListView->saveLayout(config,group);
}

void KOTodoView::restoreLayout(KConfig *config, const QString &group)
{
  mTodoListView->restoreLayout(config,group);
}

void KOTodoView::processSelectionChange()
{
//  kdDebug() << "KOTodoView::processSelectionChange()" << endl;

  KOTodoViewItem *item =
    static_cast<KOTodoViewItem *>( mTodoListView->selectedItem() );

  if ( !item ) {
    emit incidenceSelected( 0 );
    mNewSubBut->setEnabled( false );
  } else {
    emit incidenceSelected( item->todo() );
    mNewSubBut->setEnabled( true );
  }
}

void KOTodoView::modified(bool b)
{
  emit isModified(b);
}
void KOTodoView::setTodoModified( Todo* todo )
{
  todoModified( todo, KOGlobals::UNKNOWN_MODIFIED );
}
void KOTodoView::clearSelection()
{
  mTodoListView->selectAll( false );
}
void  KOTodoView::setAllOpen()
{
    if ( isFlatDisplay ) {
        isFlatDisplay = false;
        mPopupMenu->setItemChecked( 8,false );
        updateView();
    } else {
        storeCurrentItem();
    }
    setOpen(mTodoListView->firstChild(), true);
    resetCurrentItem();
}
void  KOTodoView::setAllClose()
{
    if ( isFlatDisplay ) {
        isFlatDisplay = false;
        mPopupMenu->setItemChecked( 8,false );
        updateView();
    } else {
        storeCurrentItem();
    }
    setOpen(mTodoListView->firstChild(), false);
    resetCurrentItem();
}
void  KOTodoView::setOpen( QListViewItem* item, bool setOpenI)
{

    while ( item ) {
        setOpen( item->firstChild(), setOpenI );
        item->setOpen( setOpenI );
        item = item->nextSibling();
    }
}

void KOTodoView::displayAllFlat()
{
  
    mActiveItem = 0;
    pendingSubtodo = 0;
    if ( mBlockUpdate ) {
        return;
    }
    clearList ();
    mPopupMenu->setItemChecked( 8,true );
    isFlatDisplay = true;
    QPtrList<Todo> todoList = calendar()->todos();
    Todo *todo;   
    for(todo = todoList.first(); todo; todo = todoList.next()) {
        if ( checkTodo( todo ) ) {
            KOTodoViewItem *todoItem = new KOTodoViewItem(mTodoListView,todo,this);
            mTodoMap.insert(todo,todoItem);
        }
    }
    resetCurrentItem();
}

void  KOTodoView::setAllFlat()
{
    if ( isFlatDisplay ) {
        isFlatDisplay = false;
        mPopupMenu->setItemChecked( 8,false );
        updateView();
        return;
    } 
    displayAllFlat();
}

void KOTodoView::purgeCompleted()
{
  emit purgeCompletedSignal();
 
}
void KOTodoView::toggleQuickTodo()
{
    if ( mQuickBar->isVisible() ) {
        mQuickBar->hide();
        KOPrefs::instance()->mEnableQuickTodo = false;
    }
    else {
        mQuickBar->show();
        KOPrefs::instance()->mEnableQuickTodo = true;
    }
    mPopupMenu->setItemChecked(4,KOPrefs::instance()->mEnableQuickTodo);
    mItemPopupMenu->setItemChecked( 34 , KOPrefs::instance()->mEnableQuickTodo );
}

void KOTodoView::toggleRunning()
{
    KOPrefs::instance()->mHideNonStartedTodos = !KOPrefs::instance()->mHideNonStartedTodos;
    mPopupMenu->setItemChecked(5,KOPrefs::instance()->mHideNonStartedTodos);
    mItemPopupMenu->setItemChecked( 35 , KOPrefs::instance()->mHideNonStartedTodos );
    updateView();
    if ( KOPrefs::instance()->mHideNonStartedTodos )
        topLevelWidget()->setCaption(i18n("Hide not Running"));
    else
        topLevelWidget()->setCaption(i18n("Show not Running"));
}

void KOTodoView::toggleCompleted()
{
    KOPrefs::instance()->mShowCompletedTodo = !KOPrefs::instance()->mShowCompletedTodo;
    mPopupMenu->setItemChecked( 3,KOPrefs::instance()->mShowCompletedTodo  );
    mItemPopupMenu->setItemChecked( 33 , KOPrefs::instance()->mShowCompletedTodo   );
    updateView();
    if ( KOPrefs::instance()->mShowCompletedTodo )
        topLevelWidget()->setCaption(i18n("Show Completed"));
    else
        topLevelWidget()->setCaption(i18n("Hide Completed"));
}

void KOTodoView::addQuickTodo()
{
    addQuickTodoPar( 0 );
}
void KOTodoView::addQuickTodoPar( Todo * parentTodo)
{
    Todo *todo = new Todo();
    todo->setSummary(mQuickAdd->text());
    todo->setOrganizer(KOPrefs::instance()->email());
    if ( parentTodo ) {
        todo->setRelatedTo(parentTodo);

        todo->setCategories (parentTodo->categoriesStr ());
        todo->setSecrecy (parentTodo->secrecy ());
        if ( parentTodo->priority() < 3 )
            todo->setPriority( parentTodo->priority() );
        todo->setCalID( parentTodo->calID() );
    } else {
        CalFilter * cf = mCalendar->filter();
        if ( cf ) {
            if ( cf->isEnabled()&& cf->showCategories()) {
                todo->setCategories(cf->categoryList());
            }
            if ( cf->isEnabled() )
                todo->setSecrecy( cf->getSecrecy());
        }
    }
    mCalendar->addTodo(todo);
    mQuickAdd->setText("");
    todoModified (todo,  KOGlobals::EVENTADDED );
    updateView();

}
void KOTodoView::keyPressEvent ( QKeyEvent * e )
{  
    //    e->ignore();
    //return;
    if ( !isVisible() ) {
        e->ignore();
        return;
    }
    switch ( e->key() ) {
    case Qt::Key_Down:
    case Qt::Key_Up:
        // KOrg::BaseView::keyPressEvent (  e );
        e->ignore();
        break; 
   
    case Qt::Key_Q:


        if ( e->state() == Qt::ControlButton || e->state() == Qt::ShiftButton ) {
            e->ignore();
            break; 
        }
        toggleQuickTodo();
        break; 
    case Qt::Key_U:
        if ( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton ) {
            mActiveItem = (KOTodoViewItem*)mTodoListView->currentItem();
            unparentTodo();
            e->accept();
        } else
            e->ignore();
        break; 
    case Qt::Key_S:
        if ( e->state() == Qt::ControlButton ) {
            e->ignore();
            break; 
        }
        if ( e->state() == Qt::ShiftButton ) {
            mActiveItem = (KOTodoViewItem*)mTodoListView->currentItem();
            reparentTodo();
            e->accept();
        } else
            e->ignore();
        break; 
    case Qt::Key_P:
        if ( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton ) {
            mActiveItem = (KOTodoViewItem*)mTodoListView->currentItem();
            if ( pendingSubtodo )
                itemClicked(mActiveItem);
            e->accept();
        } else
            e->ignore();
        break;   
    case Qt::Key_R:
        if (!( e->state() == Qt::ControlButton||  e->state() == Qt::ShiftButton) ) {
            mActiveItem = (KOTodoViewItem*)mTodoListView->currentItem();
            toggleRunningItemQuick();
            e->accept();
        } else
            e->ignore();
        break;  
    case Qt::Key_Escape:
        if ( pendingSubtodo ) {
            itemClicked(0);
            e->accept();
        } else
            e->ignore();
        break; 
    default:
        e->ignore();
    }
    
    if ( true ) {
        if (  e->key() == Qt::Key_I ) {
            KOTodoViewItem*cn = (KOTodoViewItem*)mTodoListView->currentItem();
            if ( cn ) {
                mActiveItem = cn;
                KOTodoViewItem* ci = (KOTodoViewItem*)( cn ); 
                if ( ci ){
                    showTodo();
                    cn = (KOTodoViewItem*)cn->itemBelow();
                    if ( cn ) {
                        mTodoListView->setCurrentItem ( cn );
                        mTodoListView->ensureItemVisible ( cn );
                    }
                    
                }
            }
            e->accept();   
        }
    }
}

void KOTodoView::updateTodo( Todo * t, int type )
{
    if  ( mBlockUpdate )
        return;

    QMap<Todo *,KOTodoViewItem *>::ConstIterator itemIterator;
    itemIterator = mTodoMap.find(t);
    if (itemIterator != mTodoMap.end()) {
        (*itemIterator)->construct();
    } else {
        if ( type == KOGlobals::EVENTADDED ) {
            insertTodoItem( t );
        }
    }

}

void KOTodoView::todoModified(Todo * t , int p )
{
    mBlockUpdate = true;
    emit todoModifiedSignal ( t, p );
    mBlockUpdate = false;
}
