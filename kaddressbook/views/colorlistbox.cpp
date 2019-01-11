/*
 *   kmail: KDE mail client
 *   This file: Copyright (C) 2000 Espen Sand, espen@kde.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <qpainter.h>

#include <kcolordialog.h>

#ifndef KAB_EMBEDDED
#include <kcolordrag.h>
#endif //KAB_EMBEDDED

#include "colorlistbox.h"
#ifdef DESKTOP_VERSION
#include <qcolordialog.h>
#endif

ColorListBox::ColorListBox( QWidget *parent, const char *name, WFlags f )
  :KListBox( parent, name, f ), mCurrentOnDragEnter(-1)
{
  connect( this, SIGNAL(selected(int)), this, SLOT(newColor(int)) );
  connect( this, SIGNAL(clicked(QListBoxItem *)), this, SLOT(slotNewColor(QListBoxItem *)) );
  setAcceptDrops( true);
}


void ColorListBox::setEnabled( bool state )
{
  if( state == isEnabled() )
  {
    return;
  }

  QListBox::setEnabled( state );
  for( uint i=0; i<count(); i++ )
  {
    updateItem( i );
  }
}


void ColorListBox::setColor( uint index, const QColor &color )
{
  if( index < count() )
  {
    ColorListItem *colorItem = (ColorListItem*)item(index);
    colorItem->setColor(color);
    updateItem( colorItem );
  }
}


QColor ColorListBox::color( uint index ) const
{
  if( index < count() )
  {
    ColorListItem *colorItem = (ColorListItem*)item(index);
    return( colorItem->color() );
  }
  else
  {
    return( black );
  }
}
void ColorListBox::slotNewColor(QListBoxItem * i)
{
    if ( i )
        newColor( index( i ) );
}

void ColorListBox::newColor( int index )
{
  if( isEnabled() == false )
  {
    return;
  }

  if( (uint)index < count() )
  {
    QColor c = color( index );
#ifndef KAB_EMBEDDED
    if( KColorDialog::getColor( c, this ) != QDialog::Rejected )
    {
      setColor( index, c );
    }
#else //KAB_EMBEDDED
#ifdef DESKTOP_VERSION
    QColor col = QColorDialog::getColor ( c ); 
    if ( col.isValid () ) {
        setColor( index, col );
    }
#else
    KColorDialog* k =  new KColorDialog( this );
    k->setColor( c );
    int res = k->exec();
    if ( res ) {
      setColor( index, k->getColor() );
    }
    delete k;
#endif
#endif //KAB_EMBEDDED
    
  }
}


void ColorListBox::dragEnterEvent( QDragEnterEvent *e )
{
#ifndef KAB_EMBEDDED
  if( KColorDrag::canDecode(e) && isEnabled() )
  {
    mCurrentOnDragEnter = currentItem();
    e->accept( true );
  }
  else
  {
    mCurrentOnDragEnter = -1;
    e->accept( false );
  }
#else //KAB_EMBEDDED
qDebug("ColorListBox::dragEnterEvent drag&drop currently not supported");
#endif //KAB_EMBEDDED
  
}


void ColorListBox::dragLeaveEvent( QDragLeaveEvent * )
{
#ifndef KAB_EMBEDDED

  if( mCurrentOnDragEnter != -1 )
  {
    setCurrentItem( mCurrentOnDragEnter );
    mCurrentOnDragEnter = -1;
  }
#else //KAB_EMBEDDED
qDebug("ColorListBox::dragLeaveEvent drag&drop currently not supported");
#endif //KAB_EMBEDDED
}


void ColorListBox::dragMoveEvent( QDragMoveEvent *e )
{
#ifndef KAB_EMBEDDED
  if( KColorDrag::canDecode(e) && isEnabled() )
  {
    ColorListItem *item = (ColorListItem*)itemAt( e->pos() );
    if( item != 0 )
    {
      setCurrentItem ( item );
    }
  }
#else //KAB_EMBEDDED
qDebug("ColorListBox::dragMoveEvent drag&drop currently not supported");
#endif //KAB_EMBEDDED
  
}


void ColorListBox::dropEvent( QDropEvent *e )
{
#ifndef KAB_EMBEDDED
  QColor color;
  if( KColorDrag::decode( e, color ) )
  {
    int index = currentItem();
    if( index != -1 )
    {
      ColorListItem *colorItem = (ColorListItem*)item(index);
      colorItem->setColor(color);
      triggerUpdate( false ); // Redraw item
    }
    mCurrentOnDragEnter = -1;
  }
  
#else //KAB_EMBEDDED
qDebug("ColorListBox::dropEvent drag&drop currently not supported");
#endif //KAB_EMBEDDED
  
}



ColorListItem::ColorListItem( const QString &text, const QColor &color )
  : QListBoxItem(), mColor( color ), mBoxWidth( 30 )
{
  setText( text );
}


const QColor &ColorListItem::color( void )
{
  return( mColor );
}


void ColorListItem::setColor( const QColor &color )
{
  mColor = color;
}


void ColorListItem::paint( QPainter *p )
{
  QFontMetrics fm = p->fontMetrics();
  int h = fm.height();

  p->drawText( mBoxWidth+3*2, fm.ascent() + fm.leading()/2, text() );

  p->setPen( Qt::black );
  p->drawRect( 3, 1, mBoxWidth, h-1 );
  p->fillRect( 4, 2, mBoxWidth-2, h-3, mColor );
}


int ColorListItem::height(const QListBox *lb ) const
{
  return( lb->fontMetrics().lineSpacing()+1 );
}


int ColorListItem::width(const QListBox *lb ) const
{
  return( mBoxWidth + lb->fontMetrics().width( text() ) + 6 );
}

#ifndef KAB_EMBEDDED
#include "colorlistbox.moc"
#endif //KAB_EMBEDDED
