/*  This file is part of the KDE Libraries
 *  Copyright (C) 1999-2000 Espen Sand (espensa@online.no)
 *  Copyright (C) 2003 Ravikiran Rajagopal (ravi@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 */


#include <kjanuswidget.h>


#include <qstringlist.h>
/*US 
#include <qbitmap.h>
#include <qgrid.h>
#include <qhbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobjectlist.h>
#include <qpixmap.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qvbox.h>
#include <qwidgetstack.h>
#include <qpainter.h>
#include <qstyle.h>

#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kseparator.h>
#include <kdebug.h>
#include "kjanuswidget.h"
#include <klistview.h>

*/

#include <kdialog.h> // Access to some static members
#include <kdebug.h>

#include <qevent.h>
#include <qpainter.h>
#include <qbitmap.h>

//#include <qobjectlist.h>
#if 0
#include <qpixmap.h>
#include <qbitmap.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgrid.h>
#include <qpainter.h>
#include <qobjectlist.h>


#include <qptrlist.h>
#include <qwidget.h>
#include <qmap.h>
#endif

class KJanusWidget::IconListItem : public QListBoxItem
{
  public:
    IconListItem( QListBox *listbox, const QPixmap &pixmap,
		   const QString &text );
    virtual int height( const QListBox *lb ) const;
    virtual int width( const QListBox *lb ) const;
    int expandMinimumWidth( int width );

  protected:
    const QPixmap &defaultPixmap();
    void paint( QPainter *painter );

  private:
    QPixmap mPixmap;
    int mMinimumWidth;
};

class KJanusWidget::KJanusWidgetPrivate
{
public:
  KJanusWidgetPrivate() : mNextPageIndex(0) { }

  int mNextPageIndex; // The next page index.

  // Dictionary for multipage modes.
  QMap<int,QWidget*> mIntToPage;
  // Reverse dictionary. Used because showPage() may be performance critical.
  QMap<QWidget*,int> mPageToInt;
  // Dictionary of title string associated with page.
  QMap<int, QString> mIntToTitle;
};

template class QPtrList<QListViewItem>;


KJanusWidget::KJanusWidget( QWidget *parent, const char *name, int face )
  : QWidget( parent, name ),
    mValid(false), mPageList(0),
    mTitleList(0), mFace(face), mTitleLabel(0), mActivePageWidget(0),
    mShowIconsInTreeList(false), d(0)
{
  QVBoxLayout *topLayout = new QVBoxLayout( this );
  if( mFace == TreeList || mFace == IconList )
  {
    d = new KJanusWidgetPrivate;

    QFrame *page = 0;
    if( mFace == TreeList )
    {
    //US
    qDebug("KJanusWidget::KJanusWidget TreeList not implemented yet");
/*US    
      QSplitter *splitter = new QSplitter( this );
      topLayout->addWidget( splitter, 10 );
      mTreeListResizeMode = QSplitter::KeepSize;

      mTreeList = new KListView( splitter );
      mTreeList->addColumn( QString::null );
      mTreeList->header()->hide();
      mTreeList->setRootIsDecorated(true);
      mTreeList->setSorting( -1 );
      connect( mTreeList, SIGNAL(selectionChanged()), SLOT(slotShowPage()) );
      connect( mTreeList, SIGNAL(clicked(QListViewItem *)), SLOT(slotItemClicked(QListViewItem *)));

      //
      // Page area. Title at top with a separator below and a pagestack using
      // all available space at bottom.
      //
      QFrame *p = new QFrame( splitter );

      QHBoxLayout *hbox = new QHBoxLayout( p, 0, 0 );
      hbox->addSpacing( KDialog::marginHint() );

      page = new QFrame( p );
      hbox->addWidget( page, 10 );
*/      
    }
    else
    {
      QHBoxLayout *hbox = new QHBoxLayout( topLayout );
      mIconList = new IconListBox( this );

      QFont listFont( mIconList->font() );
      listFont.setBold( true );
      mIconList->setFont( listFont );

      mIconList->verticalScrollBar()->installEventFilter( this );
      hbox->addWidget( mIconList );
      connect( mIconList, SIGNAL(selectionChanged()), SLOT(slotShowPage()));
      hbox->addSpacing( KDialog::marginHint() );
      page = new QFrame( this );
      hbox->addWidget( page, 10 );
    }

    //
    // Rest of page area. Title at top with a separator below and a
    // pagestack using all available space at bottom.
    //

    QVBoxLayout *vbox = new QVBoxLayout( page, 0, KDialog::spacingHint() );

    mTitleLabel = new QLabel( QString::fromLatin1("Empty page"), page, "KJanusWidgetTitleLabel" );
    vbox->addWidget( mTitleLabel );

    QFont titleFont( mTitleLabel->font() );
    titleFont.setBold( true );
    mTitleLabel->setFont( titleFont );

    mTitleSep = new KSeparator( page );
    mTitleSep->setFrameStyle( QFrame::HLine|QFrame::Plain );
    vbox->addWidget( mTitleSep );

    mPageStack = new QWidgetStack( page );
    connect(mPageStack, SIGNAL(aboutToShow(QWidget *)),
            SIGNAL(aboutToShowPage(QWidget *)));
    vbox->addWidget( mPageStack, 10 );
  }
  else if( mFace == Tabbed )
  {
    d = new KJanusWidgetPrivate;

    mTabControl = new QTabWidget( this );
    mTabControl->setMargin (KDialog::marginHint());
    topLayout->addWidget( mTabControl, 10 );
  }
  else if( mFace == Swallow )
  {
    mSwallowPage = new QWidget( this );
    topLayout->addWidget( mSwallowPage, 10 );
  }
  else
  {
    mFace = Plain;
    mPlainPage = new QFrame( this );
    topLayout->addWidget( mPlainPage, 10 );
  }
/*US
  if ( kapp )
    connect(kapp,SIGNAL(kdisplayFontChanged()),SLOT(slotFontChanged()));
*/    
  mValid = true;
  setSwallowedWidget(0); // Set default size if 'mFace' is Swallow.
}


KJanusWidget::~KJanusWidget()
{

/*US the destroyed signal caused a segmentation fault while closing the dialog and destructing
  all pages. Why not just remove all pages in the destructor??
*/
// LR we have all subwidgets with parent-child relation
// LR we do not need to delete here anything by the private class
/*
  if( mFace == Tabbed )
  {
      QMap<QWidget*,int>::Iterator it;
      for (it = d->mPageToInt.begin(); it != d->mPageToInt.end(); ++it) {
        QObject*page = (QObject*)it.key();
        pageGone(page);        
      }
  }
  else
      qDebug("KJanusWidget::~KJanusWidget so far ");
*/
//US end  
  
  delete d;
  
  
}


bool KJanusWidget::isValid() const
{
  return( mValid );
}


QFrame *KJanusWidget::plainPage()
{
  return( mPlainPage );
}


int KJanusWidget::face() const
{
  return( mFace );
}

QWidget *KJanusWidget::FindParent()
{
  if( mFace == Tabbed ) {
    return mTabControl;
  }
  else {
    return this;
  }
}

QFrame *KJanusWidget::addPage( const QStringList &items, const QString &header,
			       const QPixmap &pixmap )
{
  if( mValid == false )
  {
    kdDebug() << "addPage: Invalid object" << endl;
    return( 0 );
  }

  QFrame *page = new QFrame( FindParent(), "page" );
  addPageWidget( page, items, header, pixmap );

  return page;
}

void KJanusWidget::pageGone( QObject *obj )
{
//  QObject* obj = (QObject*)sender();
  removePage( static_cast<QWidget*>( obj ) );
}

void KJanusWidget::slotReopen( QListViewItem * item )
{
  if( item )
    item->setOpen( true );
}

QFrame *KJanusWidget::addPage( const QString &itemName, const QString &header,
			       const QPixmap &pixmap )
{

  QStringList items;
  items << itemName;
  return addPage(items, header, pixmap);
}



QVBox *KJanusWidget::addVBoxPage( const QStringList &items,
				  const QString &header,
				  const QPixmap &pixmap )
{
  if( mValid == false )
  {
      qDebug("addPage: Invalid object ");
    
    return( 0 );
  }

  QVBox *page = new QVBox(FindParent() , "vbox_page" );
  page->setSpacing( KDialog::spacingHintSmall() );
  addPageWidget( page, items, header, pixmap );

  return page;
}

QVBox *KJanusWidget::addVBoxPage( const QString &itemName,
				  const QString &header,
				  const QPixmap &pixmap )
{
  QStringList items;
  items << itemName;
  return addVBoxPage(items, header, pixmap);
}

QHBox *KJanusWidget::addHBoxPage( const QStringList &items,
				  const QString &header,
				  const QPixmap &pixmap )
{
  if( mValid == false ) {
    kdDebug() << "addPage: Invalid object" << endl;
    return( 0 );
  }

  QHBox *page = new QHBox(FindParent(), "hbox_page");
  page->setSpacing( KDialog::spacingHint() );
  addPageWidget( page, items, header, pixmap );

  return page;
}

QHBox *KJanusWidget::addHBoxPage( const QString &itemName,
				  const QString &header,
				  const QPixmap &pixmap )
{
  QStringList items;
  items << itemName;
  return addHBoxPage(items, header, pixmap);
}

QGrid *KJanusWidget::addGridPage( int n, QGrid::Direction dir,
				  const QStringList &items,
				  const QString &header,
				  const QPixmap &pixmap )
{
  if( mValid == false )
  {
    kdDebug() << "addPage: Invalid object" << endl;
    return( 0 );
  }

  QGrid *page = new QGrid( n, dir, FindParent(), "page" );
  page->setSpacing( KDialog::spacingHint() );
  addPageWidget( page, items, header, pixmap );

  return page;
}


QGrid *KJanusWidget::addGridPage( int n, QGrid::Direction dir,
				  const QString &itemName,
				  const QString &header,
				  const QPixmap &pixmap )
{
  QStringList items;
  items << itemName;
  return addGridPage(n, dir, items, header, pixmap);
}

void KJanusWidget::InsertTreeListItem(const QStringList &items, const QPixmap &pixmap, QFrame *page)
{
  bool isTop = true;
  QListViewItem *curTop = 0, *child, *last, *newChild;
  unsigned int index = 1;
  QStringList curPath;

  for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it, index++ ) {
    QString name = (*it);
    bool isPath = ( index != items.count() );

    // Find the first child.
    if (isTop) {
      child = mTreeList->firstChild();
    }
    else {
      child = curTop->firstChild();
    }

    // Now search for a child with the current Name, and if it we doesn't
    // find it, then remember the location of the last child.
    for (last = 0; child && child->text(0) != name ; last = child, child = child->nextSibling());

    if (last == 0 && child == 0) {
      // This node didn't have any children at all, lets just insert the
      // new child.
      if (isTop)
        newChild = new QListViewItem(mTreeList, name);
      else
        newChild = new QListViewItem(curTop, name);

    }
    else if (child != 0) {
      // we found the given name in this child.
      if (!isPath) {
        kdDebug() << "The element inserted was already in the TreeList box!" << endl;
        return;
      }
      else {
        // Ok we found the folder
        newChild  = child;
      }
    }
    else {
      // the node had some children, but we didn't find the given name
      if (isTop)
        newChild = new QListViewItem(mTreeList, last, name);
      else
        newChild = new QListViewItem(curTop, last, name);
    }

    // Now make the element expandable if it is a path component, and make
    // ready for next loop
    if (isPath) {
      newChild->setExpandable(true);
      curTop = newChild;
      isTop = false;
      curPath << name;

      QString key = curPath.join("_/_");
      if (mFolderIconMap.contains(key)) {
        QPixmap p = mFolderIconMap[key];
        newChild->setPixmap(0,p);
      }
    }
    else {
      if (mShowIconsInTreeList) {
        newChild->setPixmap(0, pixmap);
      }
      mTreeListToPageStack.insert(newChild, page);
    }
  }
}

void KJanusWidget::addPageWidget( QFrame *page, const QStringList &items,
				  const QString &header,const QPixmap &pixmap )
{
/*US the following signal causes a segmentation fault while closing the dialog. 
  Why not just remove all pages in the destructor??
*/  
//US  connect(page, SIGNAL(destroyed(QObject*)), this, SLOT(pageGone(QObject*)));
// we have the SIGNAL(destroyed(QObject*) only in Qt3
#ifdef DESKTOP_VERSION
    // connect(page, SIGNAL(destroyed(QObject*)), this, SLOT(pageGone(QObject*)));
#endif
  if( mFace == Tabbed )
  {
    mTabControl->addTab (page, items.last());
    d->mIntToPage[d->mNextPageIndex] = static_cast<QWidget*>(page);
    d->mPageToInt[static_cast<QWidget*>(page)] = d->mNextPageIndex;
    d->mNextPageIndex++;
  }
  else if( mFace == TreeList || mFace == IconList )
  {
    d->mIntToPage[d->mNextPageIndex] = static_cast<QWidget*>(page);
    d->mPageToInt[static_cast<QWidget*>(page)] = d->mNextPageIndex;
    mPageStack->addWidget( page, 0 );

    if (items.count() == 0) {
      kdDebug() << "Invalid QStringList, with zero items" << endl;
      return;
    }

    if( mFace == TreeList )
    {
      InsertTreeListItem(items, pixmap, page);
    }
    else // mFace == IconList
    {
      QString itemName = items.last();
      IconListItem *item = new IconListItem( mIconList, pixmap, itemName );
      mIconListToPageStack.insert(item, page);
      mIconList->invalidateHeight();
      mIconList->invalidateWidth();

      if (mIconList->isVisible())
        mIconList->updateWidth();
    }

    //
    // Make sure the title label is sufficiently wide
    //
    QString lastName = items.last();
    const QString &title = (!header.isNull() ? header : lastName);
    QRect r = mTitleLabel->fontMetrics().boundingRect( title );
    if( mTitleLabel->minimumWidth() < r.width() )
    {
      mTitleLabel->setMinimumWidth( r.width() );
    }
    d->mIntToTitle[d->mNextPageIndex] = title;
    if( d->mIntToTitle.count() == 1 )
    {
      showPage(0);
    }
    d->mNextPageIndex++;
  }
  else
  {
    kdDebug() << "KJanusWidget::addPageWidget: can only add a page in Tabbed, TreeList or IconList modes" << endl;
  }

}

void KJanusWidget::setFolderIcon(const QStringList &path, const QPixmap &pixmap)
{
  QString key = path.join("_/_");
  mFolderIconMap.insert(key,pixmap);
}



bool KJanusWidget::setSwallowedWidget( QWidget *widget )
{
  if( mFace != Swallow || mValid == false )
  {
    return( false );
  }

  //
  // Remove current layout and make a new.
  //
  if( mSwallowPage->layout() != 0 )
  {
    delete mSwallowPage->layout();
  }
  QGridLayout *gbox = new QGridLayout( mSwallowPage, 1, 1, 0 );

  //
  // Hide old children
  //
#ifdef QT_4_COMPAT
  QList<QObject *> l;
 l = mSwallowPage->children(); // silence please
  for( uint i=0; i < l.count(); i++ )
  {
    QObject *o = l.at(i);
    if( o->isWidgetType() )
    {
      ((QWidget*)o)->hide();
    }
  }

#else

  QObjectList *l = (QObjectList*)mSwallowPage->children(); // silence please
  for( uint i=0; i < l->count(); i++ )
  {
    QObject *o = l->at(i);
    if( o->isWidgetType() )
    {
      ((QWidget*)o)->hide();
    }
  }
#endif
  //
  // Add new child or make default size
  //
  if( widget == 0 )
  {
    gbox->addRowSpacing(0,100);
    gbox->addColSpacing(0,100);
    mSwallowPage->setMinimumSize(100,100);
  }
  else
  {
    if( widget->parent() != mSwallowPage )
    {
      widget->reparent( mSwallowPage, 0, QPoint(0,0) );
    }
    gbox->addWidget(widget, 0, 0 );
    gbox->activate();
    mSwallowPage->setMinimumSize( widget->minimumSize() );
  }

  return( true );
}

bool KJanusWidget::slotShowPage()
{
  if( mValid == false )
  {
    return( false );
  }

  if( mFace == TreeList )
  {
    QListViewItem *node = mTreeList->selectedItem();
    if( node == 0 ) { return( false ); }

    QWidget *stackItem = mTreeListToPageStack[node];
    // Make sure to call through the virtual function showPage(int)
    return showPage(d->mPageToInt[stackItem]);
  }
  else if( mFace == IconList )
  {
    QListBoxItem *node = mIconList->item( mIconList->currentItem() );
    if( node == 0 ) { return( false ); }
    QWidget *stackItem = mIconListToPageStack[node];
    // Make sure to call through the virtual function showPage(int)
    return showPage(d->mPageToInt[stackItem]);
  }

  return( false );
}


bool KJanusWidget::showPage( int index )
{
  if( d == 0 || mValid == false )
  {
    return( false );
  }
  else
  {
    return showPage(d->mIntToPage[index]);
  }
}


bool KJanusWidget::showPage( QWidget *w )
{
  if( w == 0 || mValid == false )
  {
    return( false );
  }

  if( mFace == TreeList || mFace == IconList )
  {
    mPageStack->raiseWidget( w );
    mActivePageWidget = w;

    int index = d->mPageToInt[w];
    mTitleLabel->setText( d->mIntToTitle[index] );
    if( mFace == TreeList )
    {
      QMap<QListViewItem *, QWidget *>::Iterator it;
      for (it = mTreeListToPageStack.begin(); it != mTreeListToPageStack.end(); ++it){
        QListViewItem *key = it.key();
        QWidget *val = it.data();
        if (val == w) {
          mTreeList->setSelected(key, true );
          break;
        }
      }
    }
    else
    {
      QMap<QListBoxItem *, QWidget *>::Iterator it;
      for (it = mIconListToPageStack.begin(); it != mIconListToPageStack.end(); ++it){
        QListBoxItem *key = it.key();
        QWidget *val = it.data();
        if (val == w) {
          mIconList->setSelected( key, true );
          break;
        }
      }
    }
  }
  else if( mFace == Tabbed )
  {
    mTabControl->showPage(w);
    mActivePageWidget = w;
  }
  else
  {
    return( false );
  }

  return( true );
}


int KJanusWidget::activePageIndex() const
{
  if( mFace == TreeList) {
    QListViewItem *node = mTreeList->selectedItem();
    if( node == 0 ) { return -1; }
    QWidget *stackItem = mTreeListToPageStack[node];
    return d->mPageToInt[stackItem];
  }
  else if (mFace == IconList) {
    QListBoxItem *node = mIconList->item( mIconList->currentItem() );
    if( node == 0 ) { return( false ); }
    QWidget *stackItem = mIconListToPageStack[node];
    return d->mPageToInt[stackItem];
  }
  else if( mFace == Tabbed ) {
    QWidget *widget = mTabControl->currentPage();
    return( widget == 0 ? -1 : d->mPageToInt[widget] );
  }
  else {
    return( -1 );
  }
}


int KJanusWidget::pageIndex( QWidget *widget ) const
{
  if( widget == 0 )
  {
    return( -1 );
  }
  else if( mFace == TreeList || mFace == IconList )
  {
    return( d->mPageToInt[widget] );
  }
  else if( mFace == Tabbed )
  {
    //
    // The user gets the real page widget with addVBoxPage(), addHBoxPage()
    // and addGridPage() but not with addPage() which returns a child of
    // the toplevel page. addPage() returns a QFrame so I check for that.
    //
    if( widget->isA("QFrame") )
    {
      return( d->mPageToInt[widget->parentWidget()] );
    }
    else
    {
      return( d->mPageToInt[widget] );
    }
  }
  else
  {
    return( -1 );
  }
}
/*US not yet implemented
void KJanusWidget::slotFontChanged()
{
  if( mTitleLabel != 0 )
  {
    mTitleLabel->setFont( KGlobalSettings::generalFont() );
    QFont titleFont( mTitleLabel->font() );
    titleFont.setBold( true );
    mTitleLabel->setFont( titleFont );
  }

  if( mFace == IconList )
  {
    QFont listFont( mIconList->font() );
    listFont.setBold( true );
    mIconList->setFont( listFont );
    mIconList->invalidateHeight();
    mIconList->invalidateWidth();
  }
}
*/

// makes the treelist behave like the list of kcontrol
void KJanusWidget::slotItemClicked(QListViewItem *it)
{
  if(it && (it->childCount()>0))
    it->setOpen(!it->isOpen());
}

void KJanusWidget::setFocus()
{
  if( mValid == false ) { return; }
  if( mFace == TreeList )
  {
    mTreeList->setFocus();
  }
  if( mFace == IconList )
  {
    mIconList->setFocus();
  }
  else if( mFace == Tabbed )
  {
    mTabControl->setFocus();
  }
  else if( mFace == Swallow )
  {
    mSwallowPage->setFocus();
  }
  else if( mFace == Plain )
  {
    mPlainPage->setFocus();
  }
}


QSize KJanusWidget::minimumSizeHint() const
{
  if( mFace == TreeList || mFace == IconList )
  {
    QSize s1( KDialog::spacingHint(), KDialog::spacingHint()*2 );
    QSize s2(0,0);
    QSize s3(0,0);
    QSize s4( mPageStack->sizeHint() );

    if( mFace == TreeList )
    {
/*US    
      s1.rwidth() += style().pixelMetric( QStyle::PM_SplitterWidth );
      s2 = mTreeList->minimumSize();
*/      
    }
    else
    {
      mIconList->updateMinimumHeight();
      mIconList->updateWidth();
      s2 = mIconList->minimumSize();
    }

    if( mTitleLabel->isVisible() == true )
    {
      s3 += mTitleLabel->sizeHint();
      s3.rheight() += mTitleSep->minimumSize().height();
    }

    //
    // Select the tallest item. It has only effect in IconList mode
    //
    int h1 = s1.rheight() + s3.rheight() + s4.height();
    int h2 = QMAX( h1, s2.rheight() );

    return( QSize( s1.width()+s2.width()+QMAX(s3.width(),s4.width()), h2 ) );
  }
  else if( mFace == Tabbed )
  {
    return( mTabControl->sizeHint() );
  }
  else if( mFace == Swallow )
  {
    return( mSwallowPage->minimumSize() );
  }
  else if( mFace == Plain )
  {
    return( mPlainPage->sizeHint() );
  }
  else
  {
    return( QSize( 100, 100 ) ); // Should never happen though.
  }

}


QSize KJanusWidget::sizeHint() const
{
  return( minimumSizeHint() );
}


void KJanusWidget::setTreeListAutoResize( bool state )
{
  if( mFace == TreeList )
  {
/*US  
    mTreeListResizeMode = state == false ?
      QSplitter::KeepSize : QSplitter::Stretch;
    QSplitter *splitter = (QSplitter*)(mTreeList->parentWidget());
    splitter->setResizeMode( mTreeList, mTreeListResizeMode );
*/    
  }
}


void KJanusWidget::setIconListAllVisible( bool state )
{
  if( mFace == IconList )
  {
    mIconList->setShowAll( state );
  }
}

void KJanusWidget::setShowIconsInTreeList( bool state )
{
  mShowIconsInTreeList = state;
}

void KJanusWidget::setRootIsDecorated( bool state )
{
  if( mFace == TreeList ) {
    mTreeList->setRootIsDecorated(state);
  }
}

void KJanusWidget::unfoldTreeList( bool persist )
{
  if( mFace == TreeList )
  {
    if( persist )
      connect( mTreeList, SIGNAL( collapsed( QListViewItem * ) ), this, SLOT( slotReopen( QListViewItem * ) ) );
    else
      disconnect( mTreeList, SIGNAL( collapsed( QListViewItem * ) ), this, SLOT( slotReopen( QListViewItem * ) ) );

    for( QListViewItem * item = mTreeList->firstChild(); item; item = item->itemBelow() )
      item->setOpen( true );
  }
}

void KJanusWidget::showEvent( QShowEvent * )
{
  if( mFace == TreeList )
  {
/*US  
    QSplitter *splitter = (QSplitter*)(mTreeList->parentWidget());
    splitter->setResizeMode( mTreeList, mTreeListResizeMode );
*/    
  }
}


//
// 2000-13-02 Espen Sand
// It should be obvious that this eventfilter must only be
// be installed on the vertical scrollbar of the mIconList.
//
bool KJanusWidget::eventFilter( QObject *o, QEvent *e )
{
  if( e->type() == QEvent::Show )
  {
    IconListItem *item = (IconListItem*)mIconList->item(0);
    if( item != 0 )
    {
      int lw = item->width( mIconList );
      int sw = mIconList->verticalScrollBar()->sizeHint().width();
      mIconList->setFixedWidth( lw+sw+mIconList->frameWidth()*2 );
    }
  }
  else if( e->type() == QEvent::Hide )
  {
    IconListItem *item = (IconListItem*)mIconList->item(0);
    if( item != 0 )
    {
      int lw = item->width( mIconList );
      mIconList->setFixedWidth( lw+mIconList->frameWidth()*2 );
    }
  }
  return QWidget::eventFilter( o, e );
}



//
// Code for the icon list box
//


KJanusWidget::IconListBox::IconListBox( QWidget *parent, const char *name,
                                        Qt::WFlags f )
  :KListBox( parent, name, f ), mShowAll(false), mHeightValid(false),
   mWidthValid(false)
{
}


void KJanusWidget::IconListBox::updateMinimumHeight()
{
  if( mShowAll == true && mHeightValid == false )
  {
    int h = frameWidth()*2;
    for( QListBoxItem *i = item(0); i != 0; i = i->next() )
    {
      h += i->height( this );
    }
    setMinimumHeight( h );
    mHeightValid = true;
  }
}


void KJanusWidget::IconListBox::updateWidth()
{
  if( mWidthValid == false )
  {
    int maxWidth = 10;
    for( QListBoxItem *i = item(0); i != 0; i = i->next() )
    {
      int w = ((IconListItem *)i)->width(this);
      maxWidth = QMAX( w, maxWidth );
    }

    for( QListBoxItem *i = item(0); i != 0; i = i->next() )
    {
      ((IconListItem *)i)->expandMinimumWidth( maxWidth );
    }

    if( verticalScrollBar()->isVisible() )
    {
      maxWidth += verticalScrollBar()->sizeHint().width();
    }

    setFixedWidth( maxWidth + frameWidth()*2 );
    mWidthValid = true;
  }
}


void KJanusWidget::IconListBox::invalidateHeight()
{
  mHeightValid = false;
}


void KJanusWidget::IconListBox::invalidateWidth()
{
  mWidthValid = false;
}


void KJanusWidget::IconListBox::setShowAll( bool showAll )
{
  mShowAll = showAll;
  mHeightValid = false;
}



KJanusWidget::IconListItem::IconListItem( QListBox *listbox, const QPixmap &pixmap,
                                          const QString &text )
  : QListBoxItem( listbox )
{
  mPixmap = pixmap;
  if( mPixmap.isNull() == true )
  {
    mPixmap = defaultPixmap();
  }
  setText( text );
  mMinimumWidth = 0;
}


int KJanusWidget::IconListItem::expandMinimumWidth( int width )
{
  mMinimumWidth = QMAX( mMinimumWidth, width );
  return( mMinimumWidth );
}


const QPixmap &KJanusWidget::IconListItem::defaultPixmap()
{
  static QPixmap *pix=0;
  if( pix == 0 )
  {
    pix = new QPixmap( 32, 32 );
    QPainter p( pix );
    p.eraseRect( 0, 0, pix->width(), pix->height() );
    p.setPen( Qt::red );
    p.drawRect ( 0, 0, pix->width(), pix->height() );
    p.end();

    QBitmap mask( pix->width(), pix->height(), true);
    mask.fill( Qt::black );
    p.begin( &mask );
    p.setPen( Qt::white );
    p.drawRect ( 0, 0, pix->width(), pix->height() );
    p.end();

    pix->setMask( mask );
  }
  return( *pix );
}


void KJanusWidget::IconListItem::paint( QPainter *painter )
{
  QFontMetrics fm = painter->fontMetrics();
  //int wt = fm.boundingRect(text()).width();
  int wp = mPixmap.width();
  int ht = fm.lineSpacing();
  int hp = mPixmap.height();

  painter->drawPixmap( (mMinimumWidth-wp)/2, 5, mPixmap );
  if( text().isEmpty() == false )
  {
    painter->drawText( 0, hp+7, mMinimumWidth, ht, Qt::AlignCenter, text() );
  }
}

int KJanusWidget::IconListItem::height( const QListBox *lb ) const
{
  if( text().isEmpty() == true )
  {
    return( mPixmap.height() );
  }
  else
  {
    return( mPixmap.height() + lb->fontMetrics().lineSpacing()+10 );
  }
}


int KJanusWidget::IconListItem::width( const QListBox *lb ) const
{
  int wt = lb->fontMetrics().boundingRect(text()).width()+10;
  int wp = mPixmap.width() + 10;
  int w  = QMAX( wt, wp );
  return( QMAX( w, mMinimumWidth ) );
}


void KJanusWidget::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

// TODO: In TreeList, if the last child of a node is removed, and there is no corrsponding widget for that node, allow the caller to
// delete the node.
void KJanusWidget::removePage( QWidget *page )
{
//US  qDebug("KJanusWidget::removePage 1 %lu , %lu, %lu", d, page, &(d->mPageToInt));
  if (!d || !(d->mPageToInt.contains(page))) 
  {
    return;
  }
    
  int index = d->mPageToInt[page];
  
  if ( mFace == TreeList )
  {
    QMap<QListViewItem*, QWidget *>::Iterator i;
    for( i = mTreeListToPageStack.begin(); i != mTreeListToPageStack.end(); ++i )
      if (i.data()==page)
      {
        delete i.key();
        mPageStack->removeWidget(page);
        mTreeListToPageStack.remove(i);
        d->mIntToTitle.remove(index);
        d->mPageToInt.remove(page);
        d->mIntToPage.remove(index);
        break;
      }
  }
  else if ( mFace == IconList )
  {
    QMap<QListBoxItem*, QWidget *>::Iterator i;
    for( i = mIconListToPageStack.begin(); i != mIconListToPageStack.end(); ++i )
      if (i.data()==page)
      {
        delete i.key();
        mPageStack->removeWidget(page);
        mIconListToPageStack.remove(i);
        d->mIntToTitle.remove(index);
        d->mPageToInt.remove(page);
        d->mIntToPage.remove(index);
        break;
      }
  }
  else // Tabbed
  {
    mTabControl->removePage(page);
    d->mPageToInt.remove(page);
    d->mIntToPage.remove(index);
  }
}

QString KJanusWidget::pageTitle(int index) const
{
  if (!d || !d->mIntToTitle.contains(index))
    return QString::null;
  else
    return d->mIntToTitle[index];
}

QWidget *KJanusWidget::pageWidget(int index) const
{
  if (!d || !d->mIntToPage.contains(index))
    return 0;
  else
    return d->mIntToPage[index];
}

//US #include "kjanuswidget.moc"
