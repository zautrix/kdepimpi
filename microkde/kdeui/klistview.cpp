/* This file is part of the KDE libraries
   Copyright (C) 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Charles Samuels <charles@kde.org>
   Copyright (C) 2000 Peter Putzer

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdragobject.h>
#include <qtimer.h>
#include <qheader.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qstyle.h>
#include <qevent.h>
#include <qpainter.h>

//US #include <kcursor.h>
#include <kapplication.h>
//US #include <kipc.h>
#include <kdebug.h>
#ifdef _WIN32_
#define Q_WS_QWS
#endif
#ifndef _WIN32_
#define private public
#ifdef QT_4_COMPAT
#include <q3listview.h>
#else
#include <qlistview.h>
#endif
#undef private
#endif

#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else


#endif
#include <kglobalsettings.h>
#include <kconfigbase.h>

#include "klistview.h"
//US #include "klistviewlineedit.h"
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#endif

// /*US

#ifndef QT_4_COMPAT
class KListView::Tooltip : public QToolTip
{
public:
  Tooltip (KListView* parent, QToolTipGroup* group = 0L);
  virtual ~Tooltip () {}

protected:
    // */
  /**
   * Reimplemented from QToolTip for internal reasons.
   */
    // /*US   
  virtual void maybeTip (const QPoint&);

private:
  KListView* mParent;
};

KListView::Tooltip::Tooltip (KListView* parent, QToolTipGroup* group)
  : QToolTip (parent, group),
        mParent (parent)
{
}

void KListView::Tooltip::maybeTip (const QPoint&)
{
  // FIXME
}
// */
#endif
class KListView::KListViewPrivate
{
public:
  KListViewPrivate (KListView* listview)
    : pCurrentItem (0L),
      autoSelectDelay(1),
//US      dragDelay (KGlobalSettings::dndEventDelay()),
      
      dragDelay (10),
//US      editor (new KListViewLineEdit (listview)),
      cursorInExecuteArea(false), 
      bUseSingle(false),
      bChangeCursorOverItem(false),
      itemsMovable (true),
      selectedBySimpleMove(false),
      selectedUsingMouse(false),
      itemsRenameable (false),
      validDrag (false),
      dragEnabled (false),
      autoOpen (true),
      dropVisualizer (true),
      dropHighlighter (false),
      createChildren (true),
      pressedOnSelected (false),
      wasShiftEvent (false),
      fullWidth (false),
      sortAscending(true),
	    tabRename(true),
      sortColumn(0),
      selectionDirection(0),
      tooltipColumn (0),
      selectionMode (Single),
      contextMenuKey ( Qt::Key_Menu ),//KGlobalSettings::contextMenuKey()),
      showContextMenusOnPress (true),//showContextMenusOnPress (KGlobalSettings::showContextMenusOnPress()),
      mDropVisualizerWidth (4)
  {
      renameable += 0;
//US      connect(editor, SIGNAL(done(QListViewItem*,int)), listview, SLOT(doneEditing(QListViewItem*,int)));
  }

  ~KListViewPrivate ()
  {
//US    delete editor;
  }

  QListViewItem* pCurrentItem;

  QTimer autoSelect;
  int autoSelectDelay;

  QTimer dragExpand;
  QListViewItem* dragOverItem;
  QPoint dragOverPoint;
  
  QPoint startDragPos;
  int dragDelay;

//US  KListViewLineEdit *editor;
  QValueList<int> renameable;

  bool cursorInExecuteArea:1;
  bool bUseSingle:1;
  bool bChangeCursorOverItem:1;
  bool itemsMovable:1;
  bool selectedBySimpleMove : 1;
  bool selectedUsingMouse:1;
  bool itemsRenameable:1;
  bool validDrag:1;
  bool dragEnabled:1;
  bool autoOpen:1;
  bool dropVisualizer:1;
  bool dropHighlighter:1;
  bool createChildren:1;
  bool pressedOnSelected:1;
  bool wasShiftEvent:1;
  bool fullWidth:1;
  bool sortAscending:1;
  bool tabRename:1;

  int sortColumn;

  //+1 means downwards (y increases, -1 means upwards, 0 means not selected), aleXXX
  int selectionDirection;
  int tooltipColumn;

  SelectionModeExt selectionMode;
  int contextMenuKey;
  bool showContextMenusOnPress;

  QRect mOldDropVisualizer;
  int mDropVisualizerWidth;
  QRect mOldDropHighlighter;
  QListViewItem *afterItemDrop;
  QListViewItem *parentItemDrop;

  QColor alternateBackground;
};

/*US
KListViewLineEdit::KListViewLineEdit(KListView *parent)
        : KLineEdit(parent->viewport()), item(0), col(0), p(parent)
{
        setFrame( false );
        hide();
        connect( parent, SIGNAL( selectionChanged() ), SLOT( slotSelectionChanged() ));
}

KListViewLineEdit::~KListViewLineEdit()
{
}

void KListViewLineEdit::load(QListViewItem *i, int c)
{
        item=i;
        col=c;

        QRect rect(p->itemRect(i));
        setText(item->text(c));

        int fieldX = rect.x() - 1;
        int fieldW = p->columnWidth(col) + 2;

        int pos = p->header()->mapToIndex(col);
        for ( int index = 0; index < pos; index++ )
            fieldX += p->columnWidth( p->header()->mapToSection( index ));

        if ( col == 0 ) {
            int d = i->depth() + (p->rootIsDecorated() ? 1 : 0);
            d *= p->treeStepSize();
            fieldX += d;
            fieldW -= d;
        }

        if ( i->pixmap( col ) ) {// add width of pixmap
            int d = i->pixmap( col )->width();
            fieldX += d;
            fieldW -= d;
        }

        setGeometry(fieldX, rect.y() - 1, fieldW, rect.height() + 2);
        show();
        setFocus();
}
*/
/*	Helper functions to for
 *	tabOrderedRename functionality.
 */

static int nextCol (KListView *pl, QListViewItem *pi, int start, int dir)
{
	if (pi)
	{
		//	Find the next renameable column in the current row
		for (; ((dir == +1) ? (start < pl->columns()) : (start >= 0)); start += dir)
			if (pl->isRenameable(start))
				return start;
	}

	return -1;
}

static QListViewItem *prevItem (QListViewItem *pi)
{
	QListViewItem *pa = pi->itemAbove();

	/*	Does what the QListViewItem::previousSibling()
	 *	of my dreams would do.
	 */
	if (pa && pa->parent() == pi->parent())
		return pa;

	return NULL;
}

static QListViewItem *lastQChild (QListViewItem *pi)
{
	if (pi)
	{
		/*	Since there's no QListViewItem::lastChild().
		 *	This finds the last sibling for the given
		 *	item.
		 */
		for (QListViewItem *pt = pi->nextSibling(); pt; pt = pt->nextSibling())
			pi = pt;
	}

	return pi;
}
/*US
void KListViewLineEdit::selectNextCell (QListViewItem *pitem, int column, bool forward)
{
	const int ncols = p->columns();
	const int dir = forward ? +1 : -1;
	const int restart = forward ? 0 : (ncols - 1);
	QListViewItem *top = (pitem && pitem->parent())
		? pitem->parent()->firstChild()
		: p->firstChild();
	QListViewItem *pi = pitem;

	terminate();		//	Save current changes

	do
	{
*/  
		/*	Check the rest of the current row for an editable column,
		 *	if that fails, check the entire next/previous row. The
		 *	last case goes back to the first item in the current branch
		 *	or the last item in the current branch depending on the
		 *	direction.
		 */
/*US     
		if ((column = nextCol(p, pi, column + dir, dir)) != -1 ||
			(column = nextCol(p, (pi = (forward ? pi->nextSibling() : prevItem(pi))), restart, dir)) != -1 ||
			(column = nextCol(p, (pi = (forward ? top : lastQChild(pitem))), restart, dir)) != -1)
		{
			if (pi)
			{
				p->setCurrentItem(pi);		//	Calls terminate
				p->rename(pi, column);
*/
				/*	Some listviews may override rename() to
				 *	prevent certain items from being renamed,
				 *	if this is done, [m_]item will be NULL
				 *	after the rename() call... try again.
				 */
/*US         
				if (!item)
					continue;

				break;
			}
		}
	}
	while (pi && !item);
}
*/

/*US
#ifdef KeyPress
#undef KeyPress
#endif

bool KListViewLineEdit::event (QEvent *pe)
{
	if (pe->type() == QEvent::KeyPress)
	{
		QKeyEvent *k = (QKeyEvent *) pe;

	    if ((k->key() == Qt::Key_Backtab || k->key() == Qt::Key_Tab) &&
			p->tabOrderedRenaming() && p->itemsRenameable() &&
			!(k->state() & ControlButton || k->state() & AltButton))
		{
			selectNextCell(item, col,
				(k->key() == Key_Tab && !(k->state() & ShiftButton)));
			return true;
	    }
	}

	return KLineEdit::event(pe);
}

void KListViewLineEdit::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
		terminate(true);
	else if(e->key() == Qt::Key_Escape)
		terminate(false);
        else if (e->key() == Qt::Key_Down || e->key() == Qt::Key_Up)
        {
		terminate(true);
                KLineEdit::keyPressEvent(e);
        }
	else
		KLineEdit::keyPressEvent(e);
}

void KListViewLineEdit::terminate()
{
    terminate(true);
}

void KListViewLineEdit::terminate(bool commit)
{
    if ( item )
    {
        //kdDebug() << "KListViewLineEdit::terminate " << commit << endl;
        if (commit)
            item->setText(col, text());
        int c=col;
        QListViewItem *i=item;
        col=0;
        item=0;
        hide(); // will call focusOutEvent, that's why we set item=0 before
        emit done(i,c);
    }
}

void KListViewLineEdit::focusOutEvent(QFocusEvent *ev)
{
    QFocusEvent * focusEv = static_cast<QFocusEvent*>(ev);
    // Don't let a RMB close the editor
    if (focusEv->reason() != QFocusEvent::Popup && focusEv->reason() != QFocusEvent::ActiveWindow)
        terminate(true);
}

void KListViewLineEdit::paintEvent( QPaintEvent *e )
{
    KLineEdit::paintEvent( e );

    if ( !frame() ) {
        QPainter p( this );
        p.setClipRegion( e->region() );
        p.drawRect( rect() );
    }
}

// selection changed -> terminate. As our "item" can be already deleted,
// we can't call terminate(false), because that would emit done() with
// a dangling pointer to "item".
void KListViewLineEdit::slotSelectionChanged()
{
    item = 0;
    col = 0;
    hide();
}
*/

KListView::KListView( QWidget *parent, const char *name ,bool emulateRightMouse  )
  : QListView( parent, name ),
        d (new KListViewPrivate (this))
{
#ifndef DESKTOP_VERSION
    if ( emulateRightMouse )
        QPEApplication::setStylusOperation( viewport(), QPEApplication::RightOnHold );
#endif
//US  setDragAutoScroll(true);

  connect( this, SIGNAL( onViewport() ),
                   this, SLOT( slotOnViewport() ) );
  connect( this, SIGNAL( onItem( QListViewItem * ) ),
                   this, SLOT( slotOnItem( QListViewItem * ) ) );

  connect (this, SIGNAL(contentsMoving(int,int)),
                   this, SLOT(cleanDropVisualizer()));
  connect (this, SIGNAL(contentsMoving(int,int)),
                   this, SLOT(cleanItemHighlighter()));

/*US
  slotSettingsChanged(KApplication::SETTINGS_MOUSE);
  
  if (kapp)
  {
    connect( kapp, SIGNAL( settingsChanged(int) ), SLOT( slotSettingsChanged(int) ) );
    kapp->addKipcEventMask( KIPC::SettingsChanged );
  }
*/
  slotSettingsChanged(1); //US do this to initialize the connections 


  connect(&d->autoSelect, SIGNAL( timeout() ),
                  this, SLOT( slotAutoSelect() ) );
  connect(&d->dragExpand, SIGNAL( timeout() ),
                  this, SLOT( slotDragExpand() ) );

  // context menu handling
  if (d->showContextMenusOnPress)
        {
          connect (this, SIGNAL (rightButtonPressed (QListViewItem*, const QPoint&, int)),
                           this, SLOT (emitContextMenu (QListViewItem*, const QPoint&, int)));
        }
  else
        {
          connect (this, SIGNAL (rightButtonClicked (QListViewItem*, const QPoint&, int)),
                           this, SLOT (emitContextMenu (QListViewItem*, const QPoint&, int)));
        }

  connect (this, SIGNAL (menuShortCutPressed (KListView*, QListViewItem*)),
                   this, SLOT (emitContextMenu (KListView*, QListViewItem*)));
  
 
  //qDebug("KListView::KListView make alternate color configurable");  
  d->alternateBackground = KGlobalSettings::alternateBackgroundColor();
}



KListView::~KListView()
{
  delete d;
}

bool KListView::isExecuteArea( const QPoint& point )
{
  if ( itemAt( point ) )
    return isExecuteArea( point.x() );

  return false;
}
QSize KListView::sizeHint() const
{
    //qDebug("KListView::QSize sizeHint() ");
#ifdef DESKTOP_VERSION
    return QListView::sizeHint();
#else
    return QSize ( 40, 40 );
#endif

}
bool KListView::isExecuteArea( int x )
{
  if( allColumnsShowFocus() )
    return true;
  else {
    int offset = 0;
    int width = columnWidth( 0 );
    int pos = header()->mapToIndex( 0 );

    for ( int index = 0; index < pos; index++ )
      offset += columnWidth( header()->mapToSection( index ) );

    x += contentsX(); // in case of a horizontal scrollbar
    return ( x > offset && x < ( offset + width ) );
  }
}

void KListView::slotOnItem( QListViewItem *item )
{
  QPoint vp = viewport()->mapFromGlobal( QCursor::pos() );
  if ( item && isExecuteArea( vp.x() ) && (d->autoSelectDelay > -1) && d->bUseSingle ) {
    d->autoSelect.start( d->autoSelectDelay, true );
    d->pCurrentItem = item;
  }
}

void KListView::slotOnViewport()
{
  if ( d->bChangeCursorOverItem )
    viewport()->unsetCursor();

  d->autoSelect.stop();
  d->pCurrentItem = 0L;
}

void KListView::slotSettingsChanged(int category)
{
    //qDebug("KListView::slotSettingsChanged has to be verified");


  switch (category)
  {
    //US I create my private category (=1) to set the settings
  case 1:
    d->dragDelay = 2;
    //US set explicitly d->bUseSingle = KGlobalSettings::singleClick();
    //    qDebug("KListView::slotSettingsChanged: single%i", d->bUseSingle);

    disconnect(this, SIGNAL (mouseButtonClicked (int, QListViewItem*, const QPoint &, int)),
               this, SLOT (slotMouseButtonClicked (int, QListViewItem*, const QPoint &, int)));

    if( d->bUseSingle )
      connect (this, SIGNAL (mouseButtonClicked (int, QListViewItem*, const QPoint &, int)),
               this, SLOT (slotMouseButtonClicked( int, QListViewItem*, const QPoint &, int)));

    //US d->bChangeCursorOverItem = KGlobalSettings::changeCursorOverIcon();
    //US d->autoSelectDelay = KGlobalSettings::autoSelectDelay();

    if( !d->bUseSingle || !d->bChangeCursorOverItem )
       viewport()->unsetCursor();

    break;

    /*US
  case KApplication::SETTINGS_MOUSE:
    d->dragDelay =  KGlobalSettings::dndEventDelay();
    d->bUseSingle = KGlobalSettings::singleClick();

    disconnect(this, SIGNAL (mouseButtonClicked (int, QListViewItem*, const QPoint &, int)),
               this, SLOT (slotMouseButtonClicked (int, QListViewItem*, const QPoint &, int)));

    if( d->bUseSingle )
      connect (this, SIGNAL (mouseButtonClicked (int, QListViewItem*, const QPoint &, int)),
               this, SLOT (slotMouseButtonClicked( int, QListViewItem*, const QPoint &, int)));

    d->bChangeCursorOverItem = KGlobalSettings::changeCursorOverIcon();
    d->autoSelectDelay = KGlobalSettings::autoSelectDelay();

    if( !d->bUseSingle || !d->bChangeCursorOverItem )
       viewport()->unsetCursor();

    break;
  case KApplication::SETTINGS_POPUPMENU:
    d->contextMenuKey = KGlobalSettings::contextMenuKey ();
    d->showContextMenusOnPress = KGlobalSettings::showContextMenusOnPress ();

    if (d->showContextMenusOnPress)
    {
      disconnect (0L, 0L, this, SLOT (emitContextMenu (QListViewItem*, const QPoint&, int)));

      connect(this, SIGNAL (rightButtonPressed (QListViewItem*, const QPoint&, int)),
              this, SLOT (emitContextMenu (QListViewItem*, const QPoint&, int)));
    }
    else
    {
      disconnect (0L, 0L, this, SLOT (emitContextMenu (QListViewItem*, const QPoint&, int)));

      connect(this, SIGNAL (rightButtonClicked (QListViewItem*, const QPoint&, int)),
              this, SLOT (emitContextMenu (QListViewItem*, const QPoint&, int)));
    }
    break;
    */

  default:
    break;
  }
    
}

void KListView::slotAutoSelect()
{
  // check that the item still exists
  if( itemIndex( d->pCurrentItem ) == -1 )
    return;

  if (!isActiveWindow())
        {
          d->autoSelect.stop();
          return;
        }

  //Give this widget the keyboard focus.
  if( !hasFocus() )
    setFocus();

  QListViewItem* previousItem = currentItem();
  setCurrentItem( d->pCurrentItem );

#if 0
#ifndef Q_WS_QWS
  // FIXME(E): Implement for Qt Embedded
  if( d->pCurrentItem ) {
    //Shift pressed?
    if( (keybstate & ShiftMask) ) {
      bool block = signalsBlocked();
      blockSignals( true );

      //No Ctrl? Then clear before!
      if( !(keybstate & ControlMask) )
                clearSelection();

      bool select = !d->pCurrentItem->isSelected();
      bool update = viewport()->isUpdatesEnabled();
      viewport()->setUpdatesEnabled( false );

      bool down = previousItem->itemPos() < d->pCurrentItem->itemPos();
      QListViewItemIterator lit( down ? previousItem : d->pCurrentItem );
      for ( ; lit.current(); ++lit ) {
                if ( down && lit.current() == d->pCurrentItem ) {
                  d->pCurrentItem->setSelected( select );
                  break;
                }
                if ( !down && lit.current() == previousItem ) {
                  previousItem->setSelected( select );
                  break;
                }
                lit.current()->setSelected( select );
      }

      blockSignals( block );
      viewport()->setUpdatesEnabled( update );
      triggerUpdate();

      emit selectionChanged();

      if( selectionMode() == QListView::Single )
                emit selectionChanged( d->pCurrentItem );
    }
    else if( (keybstate & ControlMask) )
      setSelected( d->pCurrentItem, !d->pCurrentItem->isSelected() );
    else {
      bool block = signalsBlocked();
      blockSignals( true );

      if( !d->pCurrentItem->isSelected() )
                clearSelection();

      blockSignals( block );

      setSelected( d->pCurrentItem, true );
    }
  }
  else
    kdDebug() << "KListView::slotAutoSelect: Thats not supposed to happen!!!!" << endl;
#endif
#endif
}

void KListView::slotHeaderChanged()
{
  if (d->fullWidth && columns())
  {
    int w = 0;
    for (int i = 0; i < columns() - 1; ++i) w += columnWidth(i);
    setColumnWidth( columns() - 1, viewport()->width() - w - 1 );
  }
}

void KListView::emitExecute( int buttonstate, QListViewItem *item, const QPoint &pos, int c)
{
  //  qDebug("KListView::emitExecute buttonstate=%i", buttonstate);
    if( isExecuteArea( viewport()->mapFromGlobal(pos) ) ) {

        // Double click mode ?
        if ( !d->bUseSingle )
        {
            emit executed( item );
            emit executed( item, pos, c );
        }
        else
        {
	  //US special implementation for embedded systems
            d->autoSelect.stop();

            //Dont emit executed if in SC mode and Shift or Ctrl are pressed
	    if( !( (buttonstate==Qt::ShiftButton) || (buttonstate==Qt::ControlButton) )) {
	      //            if( !( ((keybstate & ShiftMask) || (keybstate & ControlMask)) ) ) {

                emit executed( item );
                emit executed( item, pos, c );
            }
        }
    }
}

void KListView::focusInEvent( QFocusEvent *fe )
{
 //   kdDebug()<<"KListView::focusInEvent()"<<endl;
  QListView::focusInEvent( fe );
  if ((d->selectedBySimpleMove)
      && (d->selectionMode == FileManager)
      && (fe->reason()!=QFocusEvent::Popup)
      && (fe->reason()!=QFocusEvent::ActiveWindow)
      && (currentItem()!=0))
  {
      currentItem()->setSelected(true);
      currentItem()->repaint();
      emit selectionChanged();
  };
}

void KListView::focusOutEvent( QFocusEvent *fe )
{
  cleanDropVisualizer();
  cleanItemHighlighter();

  d->autoSelect.stop();

  if ((d->selectedBySimpleMove)
      && (d->selectionMode == FileManager)
      && (fe->reason()!=QFocusEvent::Popup)
      && (fe->reason()!=QFocusEvent::ActiveWindow)
      && (currentItem()!=0)
/*US      && (!d->editor->isVisible()) */
    )
  {
      currentItem()->setSelected(false);
      currentItem()->repaint();
      emit selectionChanged();
  };

  QListView::focusOutEvent( fe );
}

void KListView::leaveEvent( QEvent *e )
{
  d->autoSelect.stop();

  QListView::leaveEvent( e );
}

bool KListView::event( QEvent *e )
{
  if (e->type() == QEvent::ApplicationPaletteChange) {
    //qDebug("KListView::event make alternate color configurable");  
//US  d->alternateBackground=KGlobalSettings::alternateBackgroundColor();
  d->alternateBackground = QColor(240, 240, 240);
  }

  return QListView::event(e);
}

void KListView::contentsMousePressEvent( QMouseEvent *e )
{
  //qDebug("KListView::contentsMousePressEvent");
  if( (selectionModeExt() == Extended) && (e->state() & Qt::ShiftButton) && !(e->state() & Qt::ControlButton) )
  {
    bool block = signalsBlocked();
    blockSignals( true );

    clearSelection();

    blockSignals( block );
  }
  else if ((selectionModeExt()==FileManager) && (d->selectedBySimpleMove))
  {
     d->selectedBySimpleMove=false;
     d->selectedUsingMouse=true;
     if (currentItem()!=0)
     {
        currentItem()->setSelected(false);
        currentItem()->repaint();
//        emit selectionChanged();
     };
  };

  QPoint p( contentsToViewport( e->pos() ) );
  QListViewItem *at = itemAt (p);

  // true if the root decoration of the item "at" was clicked (i.e. the +/- sign)
  bool rootDecoClicked = at
           && ( p.x() <= header()->cellPos( header()->mapToActual( 0 ) ) +
                treeStepSize() * ( at->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() )
           && ( p.x() >= header()->cellPos( header()->mapToActual( 0 ) ) );

  if (e->button() == Qt::LeftButton && !rootDecoClicked)
  {
    //Start a drag
    d->startDragPos = e->pos();

    if (at)
    {
      d->validDrag = true;
      d->pressedOnSelected = at->isSelected();
    }
  }

  QListView::contentsMousePressEvent( e );
}

void KListView::contentsMouseMoveEvent( QMouseEvent *e )
{
    if (!dragEnabled() || d->startDragPos.isNull() || !d->validDrag) {
      QListView::contentsMouseMoveEvent (e);
      return;
    }

  QPoint vp = contentsToViewport(e->pos());
  QListViewItem *item = itemAt( vp );

  //do we process cursor changes at all?
  if ( item && d->bChangeCursorOverItem && d->bUseSingle )
    {
      //Cursor moved on a new item or in/out the execute area
      if( (item != d->pCurrentItem) ||
          (isExecuteArea(vp) != d->cursorInExecuteArea) )
        {
          d->cursorInExecuteArea = isExecuteArea(vp);
	  //qDebug("KListView::contentsMouseMoveEvent drag&drop not supported yet");  
/*US
          if( d->cursorInExecuteArea ) //cursor moved in execute area
            viewport()->setCursor( KCursor::handCursor() );
          else //cursor moved out of execute area
            viewport()->unsetCursor();
*/            
        }
    }

  bool dragOn = dragEnabled();
  QPoint newPos = e->pos();
  if (dragOn && d->validDrag &&
      (newPos.x() > d->startDragPos.x()+d->dragDelay ||
       newPos.x() < d->startDragPos.x()-d->dragDelay ||
       newPos.y() > d->startDragPos.y()+d->dragDelay ||
       newPos.y() < d->startDragPos.y()-d->dragDelay))
    //(d->startDragPos - e->pos()).manhattanLength() > QApplication::startDragDistance())
    {
      QListView::contentsMouseReleaseEvent( 0 );
      startDrag();
      d->startDragPos = QPoint();
      d->validDrag = false;
    }
}

void KListView::contentsMouseReleaseEvent( QMouseEvent *e )
{
  if (e->button() == Qt::LeftButton)
  {
    // If the row was already selected, maybe we want to start an in-place editing
    if ( d->pressedOnSelected && itemsRenameable() )
    {
      QPoint p( contentsToViewport( e->pos() ) );
      QListViewItem *at = itemAt (p);
      if ( at )
      {
        // true if the root decoration of the item "at" was clicked (i.e. the +/- sign)
        bool rootDecoClicked =
                  ( p.x() <= header()->cellPos( header()->mapToActual( 0 ) ) +
                    treeStepSize() * ( at->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() )
               && ( p.x() >= header()->cellPos( header()->mapToActual( 0 ) ) );

        if (!rootDecoClicked)
        {
          int col = header()->mapToLogical( header()->cellAt( p.x() ) );
          if ( d->renameable.contains(col) )
            rename(at, col);
        }
      }
    }

    d->pressedOnSelected = false;
    d->validDrag = false;
    d->startDragPos = QPoint();
  }
  QListView::contentsMouseReleaseEvent( e );
}

void KListView::contentsMouseDoubleClickEvent ( QMouseEvent *e )
{
  //qDebug("KListView::contentsMouseDoubleClickEvent");
  // We don't want to call the parent method because it does setOpen,
  // whereas we don't do it in single click mode... (David)
  //QListView::contentsMouseDoubleClickEvent( e );

  QPoint vp = contentsToViewport(e->pos());
  QListViewItem *item = itemAt( vp );
  emit QListView::doubleClicked( item ); // we do it now

  int col = item ? header()->mapToLogical( header()->cellAt( vp.x() ) ) : -1;

  if( item ) {
    //qDebug("KListView::contentsMouseDoubleClickEvent: emit doubleClicked");

    emit doubleClicked( item, e->globalPos(), col );

    if( (e->button() == Qt::LeftButton) && !d->bUseSingle ) {
      //qDebug("KListView::contentsMouseDoubleClickEvent: emitExecute");

      emitExecute( e->button(), item, e->globalPos(), col);
    }
  }
}

void KListView::slotMouseButtonClicked( int btn, QListViewItem *item, const QPoint &pos, int c )
{
  //qDebug("KListView::slotMouseButtonClicked");

  if( (btn == Qt::LeftButton) && item ) {
    //qDebug("KListView::slotMouseButtonClicked: emitExecute");

    emitExecute(btn, item, pos, c);
  }
}

void KListView::contentsDropEvent(QDropEvent* e)
{
qDebug("KListView::contentsDropEvent drag&drop not supported yet");  
/*US
  cleanDropVisualizer();
  cleanItemHighlighter();
  d->dragExpand.stop();

  if (acceptDrag (e))
  {
    e->acceptAction();
    QListViewItem *afterme;
    QListViewItem *parent;
    findDrop(e->pos(), parent, afterme);

    if (e->source() == viewport() && itemsMovable())
        movableDropEvent(parent, afterme);
    else
    {
    
        emit dropped(e, afterme);
        emit dropped(this, e, afterme);
        emit dropped(e, parent, afterme);
        emit dropped(this, e, parent, afterme);
        
    }
  }
*/
  
}

void KListView::movableDropEvent (QListViewItem* parent, QListViewItem* afterme)
{
  QPtrList<QListViewItem> items, afterFirsts, afterNows;
  QListViewItem *current=currentItem();
  bool hasMoved=false;
  for (QListViewItem *i = firstChild(), *iNext=0; i != 0; i = iNext)
  {
    iNext=i->itemBelow();
    if (!i->isSelected())
      continue;

    // don't drop an item after itself, or else
    // it moves to the top of the list
    if (i==afterme)
      continue;

    i->setSelected(false);

    QListViewItem *afterFirst = i->itemAbove();

        if (!hasMoved)
        {
                emit aboutToMove();
                hasMoved=true;
        }

    moveItem(i, parent, afterme);

    // ###### This should include the new parent !!! -> KDE 3.0
    // If you need this right now, have a look at keditbookmarks.
    emit moved(i, afterFirst, afterme);

    items.append (i);
    afterFirsts.append (afterFirst);
    afterNows.append (afterme);

    afterme = i;
  }
  clearSelection();
  for (QListViewItem *i=items.first(); i != 0; i=items.next() )
    i->setSelected(true);
  if (current)
    setCurrentItem(current);

  emit moved(items,afterFirsts,afterNows);

  if (firstChild())
    emit moved();
}

void KListView::contentsDragMoveEvent(QDragMoveEvent *event)
{
  qDebug("KListView::contentsDropEvent drag&drop not supported yet");  
/*US  
  if (acceptDrag(event))
  {
    event->acceptAction();
    //Clean up the view

    findDrop(event->pos(), d->parentItemDrop, d->afterItemDrop);
    QPoint vp = contentsToViewport( event->pos() ); 
    QListViewItem *item = isExecuteArea( vp ) ? itemAt( vp ) : 0L;
    
    if ( item != d->dragOverItem )
    {
      d->dragExpand.stop();
      d->dragOverItem = item;
      d->dragOverPoint = vp;
      if ( d->dragOverItem && d->dragOverItem->isExpandable() && !d->dragOverItem->isOpen() )
        d->dragExpand.start( QApplication::startDragTime(), true );
    }
    if (dropVisualizer())
    {
      QRect tmpRect = drawDropVisualizer(0, d->parentItemDrop, d->afterItemDrop);
      if (tmpRect != d->mOldDropVisualizer)
      {
        cleanDropVisualizer();
        d->mOldDropVisualizer=tmpRect;
        viewport()->repaint(tmpRect);
      }
    }
    if (dropHighlighter())
    {
      QRect tmpRect = drawItemHighlighter(0, d->afterItemDrop);
      if (tmpRect != d->mOldDropHighlighter)
      {
        cleanItemHighlighter();
        d->mOldDropHighlighter=tmpRect;
        viewport()->repaint(tmpRect);
      }
    }
  }
  else
      event->ignore();
*/      
}

void KListView::slotDragExpand()
{
  if ( itemAt( d->dragOverPoint ) == d->dragOverItem )
    d->dragOverItem->setOpen( true );
}

void KListView::contentsDragLeaveEvent (QDragLeaveEvent*)
{
  d->dragExpand.stop();
  cleanDropVisualizer();
  cleanItemHighlighter();
}

void KListView::cleanDropVisualizer()
{
  if (d->mOldDropVisualizer.isValid())
  {
    QRect rect=d->mOldDropVisualizer;
    d->mOldDropVisualizer = QRect();
    viewport()->repaint(rect, true);
  }
}

int KListView::depthToPixels( int depth )
{
    return treeStepSize() * ( depth + (rootIsDecorated() ? 1 : 0) ) + itemMargin();
}

void KListView::findDrop(const QPoint &pos, QListViewItem *&parent, QListViewItem *&after)
{
  QPoint p (contentsToViewport(pos));

  // Get the position to put it in
  QListViewItem *atpos = itemAt(p);

  QListViewItem *above;
  if (!atpos) // put it at the end
    above = lastItem();
  else
  {
    // Get the closest item before us ('atpos' or the one above, if any)
      if (p.y() - itemRect(atpos).topLeft().y() < (atpos->height()/2))
          above = atpos->itemAbove();
      else
          above = atpos;
  }

  if (above)
  {
      // Now, we know we want to go after "above". But as a child or as a sibling ?
      // We have to ask the "above" item if it accepts children.
      if (above->isExpandable())
      {
          // The mouse is sufficiently on the right ? - doesn't matter if 'above' has visible children
          if (p.x() >= depthToPixels( above->depth() + 1 ) ||
              (above->isOpen() && above->childCount() > 0) )
          {
              parent = above;
              after = 0L;
              return;
          }
      }

      // Ok, there's one more level of complexity. We may want to become a new
      // sibling, but of an upper-level group, rather than the "above" item
      QListViewItem * betterAbove = above->parent();
      QListViewItem * last = above;
      while ( betterAbove )
      {
          // We are allowed to become a sibling of "betterAbove" only if we are
          // after its last child
          if ( last->nextSibling() == 0 )
          {
              if (p.x() < depthToPixels ( betterAbove->depth() + 1 ))
                  above = betterAbove; // store this one, but don't stop yet, there may be a better one
              else
                  break; // not enough on the left, so stop
              last = betterAbove;
              betterAbove = betterAbove->parent(); // up one level
          } else
              break; // we're among the child of betterAbove, not after the last one
      }
  }
  // set as sibling
  after = above;
  parent = after ? after->parent() : 0L ;
}

QListViewItem* KListView::lastChild () const
{
  QListViewItem* lastchild = firstChild();

  if (lastchild)
        for (; lastchild->nextSibling(); lastchild = lastchild->nextSibling());

  return lastchild;
}

QListViewItem *KListView::lastItem() const
{
  QListViewItem* last = lastChild();

  for (QListViewItemIterator it (last); it.current(); ++it)
    last = it.current();

  return last;
}

KLineEdit *KListView::renameLineEdit() const
{
//US  return d->editor;
qDebug("KListView::renameLineEdit returns 0. Might crash");  
return 0;
}

void KListView::startDrag()
{
qDebug("KListView::startDrag drag&drop not supported yet.");  
/*US
  QDragObject *drag = dragObject();

  if (!drag)
        return;

  if (drag->drag() && drag->target() != viewport())
    emit moved();
*/    
}

QDragObject *KListView::dragObject()
{
  if (!currentItem())
        return 0;

  return new QStoredDrag("application/x-qlistviewitem", viewport());
}

void KListView::setItemsMovable(bool b)
{
  d->itemsMovable=b;
}

bool KListView::itemsMovable() const
{
  return d->itemsMovable;
}

void KListView::setItemsRenameable(bool b)
{
  d->itemsRenameable=b;
}

bool KListView::itemsRenameable() const
{
  return d->itemsRenameable;
}


void KListView::setDragEnabled(bool b)
{
  d->dragEnabled=b;
}

bool KListView::dragEnabled() const
{
  return d->dragEnabled;
}

void KListView::setAutoOpen(bool b)
{
  d->autoOpen=b;
}

bool KListView::autoOpen() const
{
  return d->autoOpen;
}

bool KListView::dropVisualizer() const
{
  return d->dropVisualizer;
}

void KListView::setDropVisualizer(bool b)
{
  d->dropVisualizer=b;
}

QPtrList<QListViewItem> KListView::selectedItems() const
{
  QPtrList<QListViewItem> list;
  for (QListViewItem *i=firstChild(); i!=0; i=i->itemBelow())
        if (i->isSelected()) list.append(i);
  return list;
}


void KListView::moveItem(QListViewItem *item, QListViewItem *parent, QListViewItem *after)
{
  // sanity check - don't move a item into it's own child structure
  QListViewItem *i = parent;
  while(i)
    {
      if(i == item)
        return;
      i = i->parent();
    }

  // Basically reimplementing the QListViewItem(QListViewItem*, QListViewItem*) constructor
  // in here, without ever deleting the item.
  if (item->parent())
        item->parent()->takeItem(item);
  else
        takeItem(item);

  if (parent)
        parent->insertItem(item);
  else
        insertItem(item);

  if (after) 
    ;//item->moveToJustAfter(after);
}

void KListView::contentsDragEnterEvent(QDragEnterEvent *event)
{
qDebug("KListView::contentsDragEnterEvent drag&drop not supported yet.");  
/*US
  if (acceptDrag (event))
    event->accept();
*/    
}

void KListView::setDropVisualizerWidth (int w)
{
  d->mDropVisualizerWidth = w > 0 ? w : 1;
}

QRect KListView::drawDropVisualizer(QPainter *p, QListViewItem *parent,
                                    QListViewItem *after)
{
    QRect insertmarker;

    if (!after && !parent)
        insertmarker = QRect (0, 0, viewport()->width(), d->mDropVisualizerWidth/2);
    else
    {
        int level = 0;
        if (after)
        {
            QListViewItem* it = 0L;
            if (after->isOpen())
            {
                // Look for the last child (recursively)
                it = after->firstChild();
                if (it)
                    while (it->nextSibling() || it->firstChild())
                        if ( it->nextSibling() )
                            it = it->nextSibling();
                        else
                            it = it->firstChild();
            }

            insertmarker = itemRect (it ? it : after);
            level = after->depth();
        }
        else if (parent)
        {
            insertmarker = itemRect (parent);
            level = parent->depth() + 1;
        }
        insertmarker.setLeft( treeStepSize() * ( level + (rootIsDecorated() ? 1 : 0) ) + itemMargin() );
        insertmarker.setRight (viewport()->width());
        insertmarker.setTop (insertmarker.bottom() - d->mDropVisualizerWidth/2 + 1);
        insertmarker.setBottom (insertmarker.bottom() + d->mDropVisualizerWidth/2);
    }

    // This is not used anymore, at least by KListView itself (see viewportPaintEvent)
    // Remove for KDE 3.0.
    if (p)
        p->fillRect(insertmarker, Qt::Dense4Pattern);

    return insertmarker;
}

QRect KListView::drawItemHighlighter(QPainter *painter, QListViewItem *item)
{
  QRect r;

  if (item)
  {
    r = itemRect(item);
    r.setLeft(r.left()+(item->depth()+1)*treeStepSize());
    if (painter) {
//US      style().drawPrimitive(QStyle::PE_FocusRect, painter, r, colorGroup(),
//US                            QStyle::Style_FocusAtBorder, colorGroup().highlight());
      const QColor* pHighl = &(colorGroup().highlight());
      //LR     style().drawFocusRect(painter, r, colorGroup(), pHighl, true);
                                
qDebug("KListView::drawItemHighlighter has to be verified");  
    
    }
                            
  }

  return r;
}

void KListView::cleanItemHighlighter ()
{
  if (d->mOldDropHighlighter.isValid())
  {
    QRect rect=d->mOldDropHighlighter;
    d->mOldDropHighlighter = QRect();
    viewport()->repaint(rect, true);
  }
}

void KListView::rename(QListViewItem *item, int c)
{
  if (d->renameable.contains(c))
  {
    ensureItemVisible(item);
//US    d->editor->load(item,c);
qDebug("KListView::rename has to be verified");  

  }
}

bool KListView::isRenameable (int col) const
{
  return d->renameable.contains(col);
}

void KListView::setRenameable (int col, bool yesno)
{
  if (col>=header()->count()) return;

  d->renameable.remove(col);
  if (yesno && d->renameable.find(col)==d->renameable.end())
    d->renameable+=col;
  else if (!yesno && d->renameable.find(col)!=d->renameable.end())
    d->renameable.remove(col);
}

void KListView::doneEditing(QListViewItem *item, int row)
{
  emit itemRenamed(item, item->text(row), row);
  emit itemRenamed(item);
}

bool KListView::acceptDrag(QDropEvent* e) const
{
qDebug("KListView::acceptDrag drag&drop not supported yet");  
//US  return acceptDrops() && itemsMovable() && (e->source()==viewport());
return false;
}

void KListView::setCreateChildren(bool b)
{
        d->createChildren=b;
}

bool KListView::createChildren() const
{
        return d->createChildren;
}


int KListView::tooltipColumn() const
{
        return d->tooltipColumn;
}

void KListView::setTooltipColumn(int column)
{
        d->tooltipColumn=column;
}

void KListView::setDropHighlighter(bool b)
{
        d->dropHighlighter=b;
}

bool KListView::dropHighlighter() const
{
        return d->dropHighlighter;
}

bool KListView::showTooltip(QListViewItem *item, const QPoint &, int column) const
{
        return ((tooltip(item, column).length()>0) && (column==tooltipColumn()));
}

QString KListView::tooltip(QListViewItem *item, int column) const
{
        return item->text(column);
}

void KListView::setTabOrderedRenaming(bool b)
{
	d->tabRename = b;
}

bool KListView::tabOrderedRenaming() const
{
	return d->tabRename;
}

void KListView::keyPressEvent (QKeyEvent* e)
{
  //don't we need a contextMenuModifier too ? (aleXXX)
  if (e->key() == d->contextMenuKey)
        {
          emit menuShortCutPressed (this, currentItem());
          return;
        }
  if (e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace)
        {
          emit signalDelete ( );
          return;
        }

  if (d->selectionMode != FileManager)
        QListView::keyPressEvent (e);
  else
        fileManagerKeyPressEvent (e);
}

void KListView::activateAutomaticSelection()
{
   d->selectedBySimpleMove=true;
   d->selectedUsingMouse=false;
   if (currentItem()!=0)
   {
      selectAll(false);
      currentItem()->setSelected(true);
      currentItem()->repaint();
      emit selectionChanged();
   };
}

void KListView::deactivateAutomaticSelection()
{
   d->selectedBySimpleMove=false;
}

bool KListView::automaticSelection() const
{
   return d->selectedBySimpleMove;
}

void KListView::fileManagerKeyPressEvent (QKeyEvent* e)
{
   //don't care whether it's on the keypad or not
    int e_state=(e->state() & ~Qt::Keypad);

    int oldSelectionDirection(d->selectionDirection);

    if ((e->key()!=Qt::Key_Shift) && (e->key()!=Qt::Key_Control)
        && (e->key()!=Qt::Key_Meta) && (e->key()!=Qt::Key_Alt))
    {
       if ((e_state==Qt::ShiftButton) && (!d->wasShiftEvent) && (!d->selectedBySimpleMove))
          selectAll(FALSE);
       d->selectionDirection=0;
       d->wasShiftEvent = (e_state == Qt::ShiftButton);
    };

    //d->wasShiftEvent = (e_state == Qt::ShiftButton);


    QListViewItem* item = currentItem();
    if (item==0) return;

    QListViewItem* repaintItem1 = item;
    QListViewItem* repaintItem2 = 0L;
    QListViewItem* visItem = 0L;

    QListViewItem* nextItem = 0L;
    int items = 0;

    bool shiftOrCtrl((e_state==Qt::ControlButton) || (e_state==Qt::ShiftButton));
    int selectedItems(0);
    for (QListViewItem *tmpItem=firstChild(); tmpItem!=0; tmpItem=tmpItem->nextSibling())
       if (tmpItem->isSelected()) selectedItems++;

    if (((selectedItems==0) || ((selectedItems==1) && (d->selectedUsingMouse)))
        && (e_state==Qt::NoButton)
        && ((e->key()==Qt::Key_Down)
        || (e->key()==Qt::Key_Up)
        || (e->key()==Qt::Key_Next)
        || (e->key()==Qt::Key_Prior)
        || (e->key()==Qt::Key_Home)
        || (e->key()==Qt::Key_End)))
    {
       d->selectedBySimpleMove=true;
       d->selectedUsingMouse=false;
    }
    else if (selectedItems>1)
       d->selectedBySimpleMove=false;

    bool emitSelectionChanged(false);

    switch (e->key())
    {
    case Qt::Key_Escape:
       selectAll(FALSE);
       emitSelectionChanged=TRUE;
       break;

    case Qt::Key_Space:
       //toggle selection of current item
       if (d->selectedBySimpleMove)
          d->selectedBySimpleMove=false;
       item->setSelected(!item->isSelected());
       emitSelectionChanged=TRUE;
       break;

    case Qt::Key_Insert:
       //toggle selection of current item and move to the next item
       if (d->selectedBySimpleMove)
       {
          d->selectedBySimpleMove=false;
          if (!item->isSelected()) item->setSelected(TRUE);
       }
       else
       {
          item->setSelected(!item->isSelected());
       };

       nextItem=item->itemBelow();

       if (nextItem!=0)
       {
          repaintItem2=nextItem;
          visItem=nextItem;
          setCurrentItem(nextItem);
       };
       d->selectionDirection=1;
       emitSelectionChanged=TRUE;
       break;

    case Qt::Key_Down:
       nextItem=item->itemBelow();
       //toggle selection of current item and move to the next item
       if (shiftOrCtrl)
       {
          d->selectionDirection=1;
          if (d->selectedBySimpleMove)
             d->selectedBySimpleMove=false;
          else
          {
             if (oldSelectionDirection!=-1)
             {
                item->setSelected(!item->isSelected());
                emitSelectionChanged=TRUE;
             };
          };
       }
       else if ((d->selectedBySimpleMove) && (nextItem!=0))
       {
          item->setSelected(false);
          emitSelectionChanged=TRUE;
       };

       if (nextItem!=0)
       {
          if (d->selectedBySimpleMove)
             nextItem->setSelected(true);
          repaintItem2=nextItem;
          visItem=nextItem;
          setCurrentItem(nextItem);
       };
       break;

    case Qt::Key_Up:
       nextItem=item->itemAbove();
       d->selectionDirection=-1;
       //move to the prev. item and toggle selection of this one
       // => No, can't select the last item, with this. For symmetry, let's
       // toggle selection and THEN move up, just like we do in down (David)
       if (shiftOrCtrl)
       {
          if (d->selectedBySimpleMove)
             d->selectedBySimpleMove=false;
          else
          {
             if (oldSelectionDirection!=1)
             {
                item->setSelected(!item->isSelected());
                emitSelectionChanged=TRUE;
             };
          }
       }
       else if ((d->selectedBySimpleMove) && (nextItem!=0))
       {
          item->setSelected(false);
          emitSelectionChanged=TRUE;
       };

       if (nextItem!=0)
       {
          if (d->selectedBySimpleMove)
             nextItem->setSelected(true);
          repaintItem2=nextItem;
          visItem=nextItem;
          setCurrentItem(nextItem);
       };
       break;

    case Qt::Key_End:
       //move to the last item and toggle selection of all items inbetween
       nextItem=item;
       if (d->selectedBySimpleMove)
          item->setSelected(false);
       if (shiftOrCtrl)
          d->selectedBySimpleMove=false;

       while(nextItem!=0)
       {
          if (shiftOrCtrl)
             nextItem->setSelected(!nextItem->isSelected());
          if (nextItem->itemBelow()==0)
          {
             if (d->selectedBySimpleMove)
                nextItem->setSelected(true);
             repaintItem2=nextItem;
             visItem=nextItem;
             setCurrentItem(nextItem);
          }
          nextItem=nextItem->itemBelow();
       }
       emitSelectionChanged=TRUE;
       break;

    case Qt::Key_Home:
       // move to the first item and toggle selection of all items inbetween
       nextItem = firstChild();
       visItem = nextItem;
       repaintItem2 = visItem;
       if (d->selectedBySimpleMove)
          item->setSelected(false);
       if (shiftOrCtrl)
       {
          d->selectedBySimpleMove=false;
       
          while ( nextItem != item )
          {
             nextItem->setSelected( !nextItem->isSelected() );
             nextItem = nextItem->itemBelow();
          }
          item->setSelected( !item->isSelected() );
       }
       setCurrentItem( firstChild() );
       emitSelectionChanged=TRUE;
       break;

    case Qt::Key_Next:
       items=visibleHeight()/item->height();
       nextItem=item;
       if (d->selectedBySimpleMove)
          item->setSelected(false);
       if (shiftOrCtrl)
       {
          d->selectedBySimpleMove=false;
          d->selectionDirection=1;
       };

       for (int i=0; i<items; i++)
       {
          if (shiftOrCtrl)
             nextItem->setSelected(!nextItem->isSelected());
          //the end
          if ((i==items-1) || (nextItem->itemBelow()==0))

          {
             if (shiftOrCtrl)
                nextItem->setSelected(!nextItem->isSelected());
             if (d->selectedBySimpleMove)
                nextItem->setSelected(true);
             ensureItemVisible(nextItem);
             setCurrentItem(nextItem);
             update();
             if ((shiftOrCtrl) || (d->selectedBySimpleMove))
             {
                emit selectionChanged();
             }
             return;
          }
          nextItem=nextItem->itemBelow();
       }
       break;

    case Qt::Key_Prior:
       items=visibleHeight()/item->height();
       nextItem=item;
       if (d->selectedBySimpleMove)
          item->setSelected(false);
       if (shiftOrCtrl)
       {
          d->selectionDirection=-1;
          d->selectedBySimpleMove=false;
       };

       for (int i=0; i<items; i++)
       {
          if ((nextItem!=item) &&(shiftOrCtrl))
             nextItem->setSelected(!nextItem->isSelected());
          //the end
          if ((i==items-1) || (nextItem->itemAbove()==0))

          {
             if (d->selectedBySimpleMove)
                nextItem->setSelected(true);
             ensureItemVisible(nextItem);
             setCurrentItem(nextItem);
             update();
             if ((shiftOrCtrl) || (d->selectedBySimpleMove))
             {
                emit selectionChanged();
             }
             return;
          }
          nextItem=nextItem->itemAbove();
       }
       break;

    case Qt::Key_Minus:
       if ( item->isOpen() )
          setOpen( item, FALSE );
       break;
    case Qt::Key_Plus:
       if (  !item->isOpen() && (item->isExpandable() || item->childCount()) )
          setOpen( item, TRUE );
       break;
    default:
       bool realKey = ((e->key()!=Qt::Key_Shift) && (e->key()!=Qt::Key_Control)
                        && (e->key()!=Qt::Key_Meta) && (e->key()!=Qt::Key_Alt));

       bool selectCurrentItem = (d->selectedBySimpleMove) && (item->isSelected());
       if (realKey && selectCurrentItem)
          item->setSelected(false);
       //this is mainly for the "goto filename beginning with pressed char" feature (aleXXX)
       QListView::SelectionMode oldSelectionMode = selectionMode();
       setSelectionMode (QListView::Multi);
       QListView::keyPressEvent (e);
       setSelectionMode (oldSelectionMode);
       if (realKey && selectCurrentItem)
       {
          currentItem()->setSelected(true);
          emitSelectionChanged=TRUE;
       }
       repaintItem2=currentItem();
       if (realKey)
          visItem=currentItem();
       break;
    }

    if (visItem)
       ensureItemVisible(visItem);

    QRect ir;
    if (repaintItem1)
       ir = ir.unite( itemRect(repaintItem1) );
    if (repaintItem2)
       ir = ir.unite( itemRect(repaintItem2) );

    if ( !ir.isEmpty() )
    {                 // rectangle to be repainted
       if ( ir.x() < 0 )
          ir.moveBy( -ir.x(), 0 );
       viewport()->repaint( ir, FALSE );
    }
    /*if (repaintItem1)
       repaintItem1->repaint();
    if (repaintItem2)
       repaintItem2->repaint();*/
    update();
    if (emitSelectionChanged)
       emit selectionChanged();
}

void KListView::setSelectionModeExt (SelectionModeExt mode)
{
    d->selectionMode = mode;

    switch (mode)
    {
    case Single:
    case Multi:
    case Extended:
    case NoSelection:
        setSelectionMode (static_cast<QListView::SelectionMode>(static_cast<int>(mode)));
        break;

    case FileManager:
        setSelectionMode (QListView::Extended);
        break;

    default:
        kdWarning () << "Warning: illegal selection mode " << int(mode) << " set!" << endl;
        break;
    }
}

KListView::SelectionModeExt KListView::selectionModeExt () const
{
  return d->selectionMode;
}

int KListView::itemIndex( const QListViewItem *item ) const
{
    if ( !item )
        return -1;

    if ( item == firstChild() )
        return 0;
    else {
        QListViewItemIterator it(firstChild());
        uint j = 0;
        for (; it.current() && it.current() != item; ++it, ++j );

        if( !it.current() )
          return -1;

        return j;
    }
}

QListViewItem* KListView::itemAtIndex(int index)
{
   if (index<0)
      return 0;

   int j(0);
   for (QListViewItemIterator it=firstChild(); it.current(); it++)
   {
      if (j==index)
         return it.current();
      j++;
   };
   return 0;
}


void KListView::emitContextMenu (KListView*, QListViewItem* i)
{
  QPoint p;
  //    qDebug("KListView::emitContextMenu ");

  if (i)
        p = viewport()->mapToGlobal(itemRect(i).center());
  else
        p = mapToGlobal(rect().center());

  emit contextMenu (this, i, p);
}

void KListView::emitContextMenu (QListViewItem* i, const QPoint& p, int col)
{
    QListViewItem* item = i;
    int c = col;
    // do not trust the values for QListViewItem* i and int col;
    //    qDebug("KListView::emitContextMenu col");
    if ( col == -1 ) {
        QPoint pp =  viewport()->mapFromGlobal(p);
        item = itemAt( pp);
        c = header()->sectionAt(pp.x() );
    }
    emit contextRequest( item, p,  c );
    emit contextMenu (this, item, p);
}

void KListView::setAcceptDrops (bool val)
{
  QListView::setAcceptDrops (val);
  viewport()->setAcceptDrops (val);
}

int KListView::dropVisualizerWidth () const
{
        return d->mDropVisualizerWidth;
}


void KListView::viewportPaintEvent(QPaintEvent *e)
{
  QListView::viewportPaintEvent(e);

  if (d->mOldDropVisualizer.isValid() && e->rect().intersects(d->mOldDropVisualizer))
    {
      QPainter painter(viewport());

      // This is where we actually draw the drop-visualizer
      painter.fillRect(d->mOldDropVisualizer, Qt::Dense4Pattern);
    }
  if (d->mOldDropHighlighter.isValid() && e->rect().intersects(d->mOldDropHighlighter))
    {
      QPainter painter(viewport());

qDebug("KListView::viewportPaintEvent has to be verified");  
      
      // This is where we actually draw the drop-highlighter
//US      style().drawPrimitive(QStyle::PE_FocusRect, &painter, d->mOldDropHighlighter, colorGroup(),
//US                            QStyle::Style_FocusAtBorder);
                            
//LR style().drawFocusRect(&painter, d->mOldDropHighlighter, colorGroup(), (const QColor*)0, true);
                                
                            
    }
}

void KListView::setFullWidth()
{
  setFullWidth(true);
}

void KListView::setFullWidth(bool fullWidth)
{
  d->fullWidth = fullWidth;
//US  header()->setStretchEnabled(fullWidth, columns()-1);
}

bool KListView::fullWidth() const
{
  return d->fullWidth;
}

int KListView::addColumn(const QString& label, int width)
{
  int result = QListView::addColumn(label, width);
  if (d->fullWidth) {
//US    header()->setStretchEnabled(false, columns()-2);
//US    header()->setStretchEnabled(true, columns()-1);
  }
  return result;
}

int KListView::addColumn(const QIconSet& iconset, const QString& label, int width)
{
  int result = QListView::addColumn(iconset, label, width);
  if (d->fullWidth) {
//US    header()->setStretchEnabled(false, columns()-2);
//US    header()->setStretchEnabled(true, columns()-1);
  }
  return result;
}

void KListView::removeColumn(int index)
{
  QListView::removeColumn(index);
//US  if (d->fullWidth && index == columns()) header()->setStretchEnabled(true, columns()-1);
}

void KListView::viewportResizeEvent(QResizeEvent* e)
{
  QListView::viewportResizeEvent(e);
}

const QColor &KListView::alternateBackground() const
{
  return d->alternateBackground;
}

void KListView::setAlternateBackground(const QColor &c)
{
  d->alternateBackground = c;
  repaint();
}

void KListView::saveLayout(KConfig *config, const QString &group) const
{
  KConfigGroupSaver saver(config, group);
  QStringList widths, order;
  for (int i = 0; i < columns(); ++i)
  {
    widths << QString::number(columnWidth(i));
    order << QString::number(header()->mapToIndex(i));
  }
  config->writeEntry("ColumnWidths", widths);
  config->writeEntry("ColumnOrder", order);
  config->writeEntry("SortColumn", d->sortColumn);
  config->writeEntry("SortAscending", d->sortAscending);
}

void KListView::restoreLayout(KConfig *config, const QString &group)
{
  KConfigGroupSaver saver(config, group);
  QStringList cols = config->readListEntry("ColumnWidths");
  int i = 0;
  for (QStringList::ConstIterator it = cols.begin(); it != cols.end(); ++it)
    setColumnWidth(i++, (*it).toInt());

  cols = config->readListEntry("ColumnOrder");
  i = 0;
  for (QStringList::ConstIterator it = cols.begin(); it != cols.end(); ++it)
    header()->moveSection(i++, (*it).toInt());

/*US I changed the following code, because hasKey is not available.
!!! check if my version is correct
  if (config->hasKey("SortColumn"))
    setSorting(config->readNumEntry("SortColumn"), config->readBoolEntry("SortAscending", true));
*/
  QStringList langLst = config->readListEntry( "SortColumn" );
  if (!langLst.isEmpty())
    setSorting(config->readNumEntry("SortColumn"), config->readBoolEntry("SortAscending", true));
}

void KListView::setSorting(int column, bool ascending)
{
  d->sortColumn = column;
  d->sortAscending = ascending;
  QListView::setSorting(column, ascending);
}

int KListView::columnSorted(void) const
{
  return d->sortColumn;
}

bool KListView::ascendingSort(void) const
{
  return d->sortAscending;
}

KListViewItem::KListViewItem(QListView *parent)
  : QListViewItem(parent)
{
  init();
}

KListViewItem::KListViewItem(QListViewItem *parent)
  : QListViewItem(parent)
{
  init();
}

KListViewItem::KListViewItem(QListView *parent, QListViewItem *after)
  : QListViewItem(parent, after)
{
  init();
}

KListViewItem::KListViewItem(QListViewItem *parent, QListViewItem *after)
  : QListViewItem(parent, after)
{
  init();
}

KListViewItem::KListViewItem(QListView *parent,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KListViewItem::KListViewItem(QListViewItem *parent,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KListViewItem::KListViewItem(QListView *parent, QListViewItem *after,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KListViewItem::KListViewItem(QListViewItem *parent, QListViewItem *after,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KListViewItem::~KListViewItem()
{
}

void KListViewItem::init()
{
  m_known = false;
}

const QColor &KListViewItem::backgroundColor()
{
  if (isAlternate())
    return static_cast< KListView* >(listView())->alternateBackground();
  return listView()->viewport()->colorGroup().base();
}

bool KListViewItem::isAlternate()
{
  KListView *lv = static_cast<KListView *>(listView());
  if (lv && lv->alternateBackground().isValid())
  {
    KListViewItem *above = 0;
//US    above = dynamic_cast<KListViewItem *>(itemAbove());
    above = (KListViewItem *)(itemAbove());
    m_known = above ? above->m_known : true;
    if (m_known)
    {
       m_odd = above ? !above->m_odd : false;
    }
    else
    {
       KListViewItem *item;
       bool previous = true;
       if (parent())
       {
//US          item = dynamic_cast<KListViewItem *>(parent());
          item = (KListViewItem *)(parent());
          if (item)
             previous = item->m_odd;
//US          item = dynamic_cast<KListViewItem *>(parent()->firstChild());
          item = (KListViewItem *)(parent()->firstChild());
       }
       else
       {
//US          item = dynamic_cast<KListViewItem *>(lv->firstChild());
          item = (KListViewItem *)(lv->firstChild());
       }

       while(item)
       {
          item->m_odd = previous = !previous;
          item->m_known = true;
//US          item = dynamic_cast<KListViewItem *>(item->nextSibling());
          item = (KListViewItem *)(item->nextSibling());
       }
    }
    return m_odd;
  }
  return false;
}

void KListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
  QColorGroup _cg = cg;
  const QPixmap *pm = listView()->viewport()->backgroundPixmap();
  if (pm && !pm->isNull())
  {
        _cg.setBrush(QColorGroup::Base, QBrush(backgroundColor(), *pm));
        QPoint o = p->brushOrigin();
        p->setBrushOrigin( o.x()-listView()->contentsX(), o.y()-listView()->contentsY() );
  }
  else if (isAlternate()) {
//US       if (listView()->viewport()->backgroundMode()==Qt::FixedColor)
#ifdef QT_4_COMPAT
      if (listView()->viewport()->backgroundMode()==Qt::PaletteBackground)
#else
       if (listView()->viewport()->backgroundMode()==QWidget::PaletteBackground)
#endif
            _cg.setColor(QColorGroup::Background, static_cast< KListView* >(listView())->alternateBackground());
       else
        _cg.setColor(QColorGroup::Base, static_cast< KListView* >(listView())->alternateBackground());
  }
  QListViewItem::paintCell(p, _cg, column, width, alignment);
}

//US we do not have a "global KDE" variable to setup singleClick functionality
void KListView::setSingleClick(bool s)
{
  d->bUseSingle = s;
  slotSettingsChanged(1);
  //  qDebug("KListView::setSingleClick: single %i", d->bUseSingle);
}


void KListView::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

//US #include "klistview.moc"
//US #include "klistviewlineedit.moc"

// vim: ts=2 sw=2 et
