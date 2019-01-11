/*
    This file is part of KAddressBook.
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>

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

#include <qheader.h>
#include <qiconset.h>
#include <qimage.h>
#include <qdragobject.h>
#include <qcombobox.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qevent.h>
#include <qapplication.h>

#include <klocale.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kurl.h>

#include "kaddressbooktableview.h"

#include "contactlistview.h"

/////////////////////////////////
// DynamicTip Methods

DynamicTip::DynamicTip( ContactListView *parent)
  : QToolTip( parent )
{
}

void DynamicTip::maybeTip( const QPoint &pos )
{
 static bool ishidden = true;
  if (!parentWidget()->inherits( "ContactListView" ))
    return;

  ContactListView *plv = (ContactListView*)parentWidget();
  if (!plv->tooltips())
    return;

  QPoint posVp = plv->viewport()->pos();

  QListViewItem *lvi = plv->itemAt( pos - posVp );
  if (!lvi)
    return;

#ifndef KAB_EMBEDDED    
  ContactListViewItem *plvi = dynamic_cast< ContactListViewItem* >(lvi);
#else //KAB_EMBEDDED    
  ContactListViewItem *plvi = (ContactListViewItem*)(lvi);
#endif //KAB_EMBEDDED    
  
  if (!plvi)
    return;

	if (ishidden) {
  QString s;
  QRect r = plv->itemRect( lvi );
  r.moveBy( posVp.x(), posVp.y() );

    //kdDebug() << "Tip rec: " << r.x() << "," << r.y() << "," << r.width()
    //          << "," << r.height() << endl;

  KABC::Addressee a = plvi->addressee();
  if (a.isEmpty())
    return;

  s += i18n("label: value", "%1: %2").arg(a.formattedNameLabel())
                                     .arg(a.formattedName());

  s += '\n';
  s += i18n("label: value", "%1: %2").arg(a.organizationLabel())
                                       .arg(a.organization());

  QString notes = a.note().stripWhiteSpace();
  if ( !notes.isEmpty() ) {
    notes += '\n';
    s += '\n' + i18n("label: value", "%1: \n").arg(a.noteLabel());
    QFontMetrics fm( font() );

    // Begin word wrap code based on QMultiLineEdit code
    int i = 0;
    bool doBreak = false;
    int linew = 0;
    int lastSpace = -1;
    int a = 0;
    int lastw = 0;

    while ( i < int(notes.length()) ) {
      doBreak = FALSE;
      if ( notes[i] != '\n' )
        linew += fm.width( notes[i] );

      if ( lastSpace >= a && notes[i] != '\n' )
        if  (linew >= parentWidget()->width()) {
          doBreak = TRUE;
          if ( lastSpace > a ) {
            i = lastSpace;
            linew = lastw;
          }
          else
            i = QMAX( a, i-1 );
        }

      if ( notes[i] == '\n' || doBreak ) {
        s += notes.mid( a, i - a + (doBreak?1:0) ) +"\n";

        a = i + 1;
        lastSpace = a;
        linew = 0;
      }

      if ( notes[i].isSpace() ) {
        lastSpace = i;
        lastw = linew;
      }

      if ( lastSpace <= a ) {
        lastw = linew;
      }

      ++i;
    }
  }

  tip( r, s );
    }
	else
        hide();
	ishidden = !ishidden;
      
}

///////////////////////////
// ContactListViewItem Methods

ContactListViewItem::ContactListViewItem(const KABC::Addressee &a,
                                         ContactListView *parent,
                                         KABC::AddressBook *doc,
                                         const KABC::Field::List &fields )
  : KListViewItem(parent), mAddressee(a), mFields( fields ),
    parentListView( parent ), mDocument(doc)
{
  refresh();
}

QString ContactListViewItem::key(int column, bool ascending) const
{
#ifndef DESKTOP_VERSION
    int lan = KGlobal::locale()->language();
    //qDebug("language %d ", lan);
    if ( lan == 1 ) { //GERMAN
        QString ret = QListViewItem::key(column, ascending).lower().utf8();
        int start = -1;
        while ( (start = ret.find( 'ä', start+1)) > 0 ) {
            ret.at(start-1) = 'a';
        }
        start = -1;
        while ( (start = ret.find( 'ö', start+1)) > 0 ) {
            ret.at(start-1) = 'o';
        }
        start = -1;
        while ( (start = ret.find( 'ü', start+1)) > 0 ) {
            ret.at(start-1) = 'o';
        }
        start = -1;
        while ( (start = ret.find( 'ß', start+1)) > 0 ) {
            ret.at(start-1) = 's';
        }
        //qDebug("conv string %s ", ret.latin1());

        return ret;
        
    }
    else
#endif
        return QListViewItem::key(column, ascending).lower();
}

void ContactListViewItem::paintCell(QPainter * p,
				                    const QColorGroup & cg,
                                    int column,
                                    int width,
                                    int align)
{
  KListViewItem::paintCell(p, cg, column, width, align);

  if ( !p )
    return;

  if (parentListView->singleLine()) {
    p->setPen( parentListView->alternateColor() );
    p->drawLine( 0, height() - 1, width, height() - 1 );
  }
}


ContactListView *ContactListViewItem::parent()
{
  return parentListView;
}


void ContactListViewItem::refresh()
{
  // Update our addressee, since it may have changed else were
  mAddressee = mDocument->findByUid(mAddressee.uid());
  if (mAddressee.isEmpty())
    return;

  int i = 0;
  KABC::Field::List::ConstIterator it;
  for( it = mFields.begin(); it != mFields.end(); ++it ) {
    setText( i++, (*it)->value( mAddressee ) );
  }
}

///////////////////////////////
// ContactListView

ContactListView::ContactListView(KAddressBookTableView *view,
                                 KABC::AddressBook* /* doc */,
                                 QWidget *parent,
                                 const char *name )
  : KListView( parent, name ),
    pabWidget( view ),
    oldColumn( 0 )
{
  mFlagBlockKeyPressed = false;
    mFlagKeyPressed = false;
  mABackground = true;
  mSingleLine = false;
  mToolTips = true;

  mAlternateColor = KGlobalSettings::alternateBackgroundColor();




  setAlternateBackgroundEnabled(mABackground);
  setAcceptDrops( true );
  viewport()->setAcceptDrops( true );
  setAllColumnsShowFocus( true );
  setShowSortIndicator(true);
  
  setSelectionModeExt( KListView::Extended   );
  setDropVisualizer(false);
  // setFrameStyle(QFrame::NoFrame);
  //setLineWidth ( 0 );
  //setMidLineWidth ( 0 );
  //setMargin ( 0 );
#ifndef KAB_EMBEDDED  
  connect(this, SIGNAL(dropped(QDropEvent*)),
          this, SLOT(itemDropped(QDropEvent*)));
#endif //KAB_EMBEDDED  

          
  new DynamicTip( this );
}
void ContactListView::printMe()
{
#ifdef DESKTOP_VERSION 
    QPrinter printer;
    if (!printer.setup() )
        return;
    QPainter p;
    p.begin ( &printer  );
    QPaintDeviceMetrics m = QPaintDeviceMetrics ( &printer );
    float dx, dy;
    int wid = (m.width() * 9)/10;
    dx = (float) wid/(float)contentsWidth ();
    dy  = (float)(m.height()) / (float)contentsHeight ();
    float scale;
    // scale to fit the width or height of the paper
    if ( dx < dy )
        scale = dx;
    else
        scale = dy;
    p.translate( m.width()/10,0 );
    p.scale( scale, scale );
    qDebug("scale %f ", scale);
    drawContents ( &p, 0,0,  contentsWidth (),  contentsHeight () ); 
    p.end();
    qDebug("Why does it not print??? ");
#endif  
}

void ContactListView::setAlternateColor(const QColor &m_AlternateColor)
{
  mAlternateColor = m_AlternateColor;
}

void ContactListView::paintEmptyArea( QPainter * p, const QRect & rect )
{
  QBrush b = palette().brush(QPalette::Active, QColorGroup::Base);

  // Get the brush, which will have the background pixmap if there is one.
  if (b.pixmap())
  {
    p->drawTiledPixmap( rect.left(), rect.top(), rect.width(), rect.height(),
      *(b.pixmap()),
      rect.left() + contentsX(),
      rect.top() + contentsY() );
  }

  else
  {
    // Do a normal paint
    KListView::paintEmptyArea(p, rect);
  }
}

void ContactListView::contentsMousePressEvent(QMouseEvent* e)
{
  presspos = e->pos();
  KListView::contentsMousePressEvent(e);
}


// To initiate a drag operation
void ContactListView::contentsMouseMoveEvent( QMouseEvent *e )
{
  if ((e->state() & LeftButton) && (e->pos() - presspos).manhattanLength() > 4 ) {
    emit startAddresseeDrag();
  }
  else
    KListView::contentsMouseMoveEvent( e );
}

bool ContactListView::acceptDrag(QDropEvent *e) const
{
#ifndef KAB_EMBEDDED  
  return QTextDrag::canDecode(e);
#else //KAB_EMBEDDED  
qDebug("ContactListView::acceptDrag has to be fixed");
  return false;
#endif //KAB_EMBEDDED  
}

void ContactListView::itemDropped(QDropEvent *e)
{
  contentsDropEvent(e);
}

void ContactListView::contentsDropEvent( QDropEvent *e )
{
  emit addresseeDropped(e);
}

void ContactListView::setAlternateBackgroundEnabled(bool enabled)
{
  mABackground = enabled;

  if (mABackground)
  {
    setAlternateBackground(mAlternateColor);
  }
  else
  {
    setAlternateBackground(QColor());
  }
}

void ContactListView::setBackgroundPixmap(const QString &filename)
{
  if (filename.isEmpty())
  {
    unsetPalette();
  }
  else
  {
  qDebug("ContactListView::setBackgroundPixmap has to be verified");
//US    setPaletteBackgroundPixmap(QPixmap(filename));
    KListView::setBackgroundPixmap((const QPixmap&)QPixmap(filename));
  }
  
}

void ContactListView::keyPressEvent ( QKeyEvent * e )
{ 
    if ( mFlagBlockKeyPressed )
        return;
    qApp->processEvents();
    if ( e->isAutoRepeat() && !mFlagKeyPressed ) {
        e->accept();
        return;
    } 
    if (! e->isAutoRepeat() )
        mFlagKeyPressed = true;
    KListView::keyPressEvent ( e );
}
void ContactListView::keyReleaseEvent ( QKeyEvent * e )
{
    if ( mFlagBlockKeyPressed )
        return;
   if ( !e->isAutoRepeat() ) {
        mFlagBlockKeyPressed = true;
        qApp->processEvents();
        mFlagBlockKeyPressed = false;
        mFlagKeyPressed = false;
   }
   KListView::keyReleaseEvent ( e );
}
#ifndef KAB_EMBEDDED    
#include "contactlistview.moc"
#endif //KAB_EMBEDDED    
