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

#ifndef KAB_EMBEDDED
#include <qiconview.h>
#include <qstringlist.h>

#include <kabc/addressee.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#else //KAB_EMBEDDED
#endif //KAB_EMBEDDED

#include <kabc/addressbook.h>
#include "kabprefs.h"
#include "viewmanager.h"
#include "kaddressbookiconview.h"
#include <qlayout.h>
#include <qregexp.h>
#include <qapplication.h>
#include <kglobal.h>
/*US transfered to the headerfile
class IconViewFactory : public ViewFactory
{
  public:
    KAddressBookView *view( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new KAddressBookIconView( ab, parent, name );
    }

    QString type() const { return "Icon"; }
    
    QString description() const { return i18n( "Icons represent contacts. Very simple view." ); }
};

*/

extern "C" {
  void *init_libkaddrbk_iconview()
  {
    return ( new IconViewFactory );
  }
}

////////////////////////////////
// AddresseeIconView (internal class)
#ifndef KAB_EMBEDDED
AddresseeIconView::AddresseeIconView(QWidget *parent, const char *name)
  : KIconView(parent, name)
#else //KAB_EMBEDDED
AddresseeIconView::AddresseeIconView(QWidget *parent, const char *name)
  : QIconView(parent, name)
#endif //KAB_EMBEDDED

{
  setSelectionMode( QIconView::Extended );
  setResizeMode( QIconView::Adjust );
  setWordWrapIconText( true );
  setGridX( 100 );
  setItemsMovable(false);
  setSorting(true, true);

  
//US ???  setMode( KIconView::Select );
  
#ifndef KAB_EMBEDDED

  connect(this, SIGNAL(dropped(QDropEvent*, const QValueList<QIconDragItem>&)),
          this, SLOT(itemDropped(QDropEvent*, const QValueList<QIconDragItem>&)));
#endif //KAB_EMBEDDED
}

AddresseeIconView::~AddresseeIconView()
{
}


void AddresseeIconView::itemDropped(QDropEvent *e, 
                                    const QValueList<QIconDragItem> &)
{
  emit addresseeDropped(e);
}

QDragObject *AddresseeIconView::dragObject()
{
  emit startAddresseeDrag();
  
  // We never want IconView to start the drag
  return 0;
}
////////////////////////////////
// AddresseeIconViewItem  (internal class)
#ifndef KAB_EMBEDDED
class AddresseeIconViewItem : public KIconViewItem
#else //KAB_EMBEDDED
class AddresseeIconViewItem : public QIconViewItem
#endif //KAB_EMBEDDED
{
  public:
#ifndef KAB_EMBEDDED
    AddresseeIconViewItem(const KABC::Field::List &fields,
                          KABC::AddressBook *doc, const KABC::Addressee &a, 
                          QIconView *parent)
      : KIconViewItem(parent), mFields( fields ), mDocument(doc), mAddressee(a)
#else //KAB_EMBEDDED
    AddresseeIconViewItem(const KABC::Field::List &fields,
                          KABC::AddressBook *doc, const KABC::Addressee &a,
                          QIconView *parent)
      : QIconViewItem(parent), mFields( fields ), mDocument(doc), mAddressee(a)
#endif //KAB_EMBEDDED
      {
          if ( mFields.isEmpty() ) {
            mFields = KABC::Field::defaultFields();
          }
          refresh();
      }
      
    const KABC::Addressee &addressee() const { return mAddressee; }
    
    void refresh()
    {
        // Update our addressee, since it may have changed elsewhere
        mAddressee = mDocument->findByUid(mAddressee.uid());
        
        if (!mAddressee.isEmpty())
          setText( mAddressee.givenName() + " " + mAddressee.familyName() );

        QPixmap icon;
      
        KABC::Picture pic = mAddressee.photo();
        if ( pic.data().isNull() )
          pic = mAddressee.logo();

        if ( pic.isIntern() && !pic.data().isNull() ) {
          QImage img = pic.data();
#ifndef KAB_EMBEDDED
        if ( img.width() > img.height() )
          icon = img.scaleWidth( 32 );
        else
          icon = img.scaleHeight( 32 );
#else //KAB_EMBEDDED

        int wid = pic.data().width();
        int hei = pic.data().height(); 
        int max = 48; 
        if ( wid > max || hei > max ) {
            if ( wid > hei ) {
                hei = (hei*max)/wid;
                wid = max;
            } else {
                wid = (wid*max)/hei;
                hei = max;
            }
        }
        qDebug("AddresseeIconViewItem::refresh - scale here dependend of the displaysize and the right factor");
        icon.convertFromImage(img.smoothScale(wid, hei));
#endif //KAB_EMBEDDED
          
        } else {
            icon = KGlobal::iconLoader()->loadIcon( "vcard", KIcon::Desktop, 128 );
        }
      setPixmap( icon );
    }
    
  private:
    KABC::Field::List mFields;
    KABC::AddressBook *mDocument;
    KABC::Addressee mAddressee;
};

///////////////////////////////
// KAddressBookView

KAddressBookIconView::KAddressBookIconView( KABC::AddressBook *ab,
                                            QWidget *parent, const char *name)
    : KAddressBookView( ab, parent, name )
{
    // Init the GUI
    QVBoxLayout *layout = new QVBoxLayout(viewWidget());
    
    mIconView = new AddresseeIconView(viewWidget(), "mIconView");
    layout->addWidget(mIconView);
    
    // Connect up the signals
    
//US method executed is part of KIconView
//US    connect(mIconView, SIGNAL(executed(QIconViewItem *)),
//US            this, SLOT(addresseeExecuted(QIconViewItem *)));
    connect(mIconView, SIGNAL(selectionChanged(QIconViewItem *)),
            this, SLOT(addresseeExecuted(QIconViewItem *)));
    
    connect(mIconView, SIGNAL(selectionChanged()),
            this, SLOT(addresseeSelected()));
    connect(mIconView, SIGNAL(addresseeDropped(QDropEvent*)),
            this, SIGNAL(dropped(QDropEvent*)));
    connect(mIconView, SIGNAL(startAddresseeDrag()),
            this, SIGNAL(startDrag()));
}

KAddressBookIconView::~KAddressBookIconView()
{
}
void KAddressBookIconView::setFocusAV()
{
    if ( mIconView )
        mIconView->setFocus();
}


void KAddressBookIconView::scrollUP()
{
    QKeyEvent * ev = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Up, 0,0 );
    QApplication::postEvent( mIconView, ev ); 
}
void KAddressBookIconView::scrollDOWN()
{
    QKeyEvent * ev = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Down, 0,0 );
    QApplication::postEvent( mIconView, ev );
}
void KAddressBookIconView::readConfig(KConfig *config)
{
  KAddressBookView::readConfig(config);
    
//US method executed is part of KIconView
//US  disconnect(mIconView, SIGNAL(executed(QIconViewItem *)),
//US             this, SLOT(addresseeExecuted(QIconViewItem *)));
  disconnect(mIconView, SIGNAL(selectionChanged(QIconViewItem *)),
             this, SLOT(addresseeExecuted(QIconViewItem *)));

//US method executed is part of KIconView. Use selectionChanged instead
/*US
  if (KABPrefs::instance()->mHonorSingleClick)
    connect(mIconView, SIGNAL(executed(QIconViewItem *)),
            this, SLOT(addresseeExecuted(QIconViewItem *)));
  else
    connect(mIconView, SIGNAL(doubleClicked(QIconViewItem *)),
            this, SLOT(addresseeExecuted(QIconViewItem *)));
*/
    connect(mIconView, SIGNAL(selectionChanged(QIconViewItem *)),
            this, SLOT(addresseeExecuted(QIconViewItem *)));
            
}
void KAddressBookIconView::doSearch( const QString&  s ,KABC::Field *field )
{
    mIconView->clear();
    mIconList.clear();
    if ( s.isEmpty() || s == "*" ) {
        refresh();
        return;
    }  
    QRegExp re = getRegExp( s );
    if (!re.isValid())
        return; 
    KABC::Addressee::List addresseeList = addressees();
    KABC::Addressee::List::Iterator it;  
    if (  field ) {
        for (it = addresseeList.begin(); it != addresseeList.end(); ++it ) {
            if ( (*it).uid().left(2) == "la" && (*it).uid().left(19) == QString("last-syncAddressee-") )
                continue;
#if QT_VERSION >= 0x030000
            if (re.search(field->value( *it ).lower()) == 0)
#else
                if (re.match(field->value( *it ).lower()) == 0)
#endif
                    mIconList.append(new AddresseeIconViewItem( fields(), addressBook(), *it, mIconView ));
                 

        }
    } else {
        KABC::Field::List fieldList = allFields();
        KABC::Field::List::ConstIterator fieldIt;
        for (it = addresseeList.begin(); it != addresseeList.end(); ++it ) {
            if ( (*it).uid().left(2) == "la" && (*it).uid().left(19) == QString("last-syncAddressee-") )
                continue;
            bool match = false;
            for ( fieldIt = fieldList.begin(); fieldIt != fieldList.end(); ++fieldIt ) {
#if QT_VERSION >= 0x030000
                if (re.search((*fieldIt)->value( *it ).lower()) == 0)
#else
                    if (re.match((*fieldIt)->value( *it ).lower()) == 0)
#endif   
                        { 
                            mIconList.append( new AddresseeIconViewItem( fields(), addressBook(), *it, mIconView ));
                            match = true;
                            break;
                        }
            } 
            if ( ! match ) {
                if ( (*it).matchPhoneNumber( &re ) ) { 
                    mIconList.append( new AddresseeIconViewItem( fields(), addressBook(), *it, mIconView ));
                    match = true;
                    break;
                }
            }
            if ( ! match ) {
                if ( (*it).matchAddress( &re ) ) { 
                    mIconList.append( new AddresseeIconViewItem( fields(), addressBook(), *it, mIconView ));
                    match = true;
                    break;
                }
            }
        }
    }
    mIconView->arrangeItemsInGrid( true ); 
    if ( mIconView->firstItem() ) {
        mIconView->setCurrentItem ( mIconView->firstItem() );
        mIconView->setSelected ( mIconView->firstItem() , true );
    }
    else
        emit selected(QString::null);
}
QStringList KAddressBookIconView::selectedUids()
{
    QStringList uidList;
    QIconViewItem *item;
    AddresseeIconViewItem *aItem;
    
    for (item = mIconView->firstItem(); item; item = item->nextItem())
    {
        if (item->isSelected())
        {
#ifndef KAB_EMBEDDED
            aItem = dynamic_cast<AddresseeIconViewItem*>(item);
#else //KAB_EMBEDDED
            aItem = (AddresseeIconViewItem*)(item);
#endif //KAB_EMBEDDED
            if (aItem)
                uidList << aItem->addressee().uid();
        }
    }
    
    return uidList;
}
    
void KAddressBookIconView::refresh(QString uid)
{
  QIconViewItem *item;
  AddresseeIconViewItem *aItem;
    
  if ( uid.isNull() ) {
    // Rebuild the view
    mIconView->clear();
    mIconList.clear();
        
    KABC::Addressee::List addresseeList = addressees();
    KABC::Addressee::List::Iterator iter;
    for ( iter = addresseeList.begin(); iter != addresseeList.end(); ++iter ) {
        if ( (*iter).uid().left(2) == "la" && (*iter).uid().left(19) == QString("last-syncAddressee-") )
                continue;
        aItem = new AddresseeIconViewItem( fields(), addressBook(), *iter, mIconView );
    }

    mIconView->arrangeItemsInGrid( true );

    for ( item = mIconView->firstItem(); item; item = item->nextItem() )
	{
#ifndef KAB_EMBEDDED
	  AddresseeIconViewItem* aivi = dynamic_cast<AddresseeIconViewItem*>( item );
#else //KAB_EMBEDDED
	  AddresseeIconViewItem* aivi = (AddresseeIconViewItem*)( item );
#endif //KAB_EMBEDDED
      mIconList.append( aivi );
	}

  } else {
    // Try to find the one to refresh
    for ( item = mIconView->firstItem(); item; item = item->nextItem() ) {
#ifndef KAB_EMBEDDED
      aItem = dynamic_cast<AddresseeIconViewItem*>(item);
#else //KAB_EMBEDDED
      aItem = (AddresseeIconViewItem*)(item);
#endif //KAB_EMBEDDED
      if ((aItem) && (aItem->addressee().uid() == uid)) {
        aItem->refresh();
        mIconView->arrangeItemsInGrid( true );
        return;
      }
    }
    refresh( QString::null );
  }
}

void KAddressBookIconView::setSelected(QString uid, bool selected)
{
    QIconViewItem *item;
    AddresseeIconViewItem *aItem;
    
    if (uid.isNull())
    {
        mIconView->selectAll(selected);
    }
    else
    {
        bool found = false;
        for (item = mIconView->firstItem(); item && !found; 
             item = item->nextItem())
         {
#ifndef KAB_EMBEDDED
             aItem = dynamic_cast<AddresseeIconViewItem*>(item);
#else //KAB_EMBEDDED
             aItem = (AddresseeIconViewItem*)(item);
#endif //KAB_EMBEDDED

             if ((aItem) && (aItem->addressee().uid() == uid))
             {
                 mIconView->setSelected(aItem, selected);
                 mIconView->ensureItemVisible( aItem );
                 found = true;
             }
         }
    }
}
   
void KAddressBookIconView::addresseeExecuted(QIconViewItem *item)
{
#ifndef KAB_EMBEDDED
    AddresseeIconViewItem *aItem = dynamic_cast<AddresseeIconViewItem*>(item);
#else //KAB_EMBEDDED
    AddresseeIconViewItem *aItem = (AddresseeIconViewItem*)(item);
#endif //KAB_EMBEDDED

    if (aItem) {
        emit executed(aItem->addressee().uid());
    }
}

void KAddressBookIconView::addresseeSelected()
{
    QIconViewItem *item;
    AddresseeIconViewItem *aItem;
    
    bool found = false;
    for (item = mIconView->firstItem(); item && !found; 
         item = item->nextItem())
    {
      if (item->isSelected())
      {
#ifndef KAB_EMBEDDED
        aItem = dynamic_cast<AddresseeIconViewItem*>(item);
#else //KAB_EMBEDDED
        aItem = (AddresseeIconViewItem*)(item);
#endif //KAB_EMBEDDED
        if (aItem)
        {
          emit selected(aItem->addressee().uid());
          found = true;
        }
      }
    }
    
    if (!found)
        emit selected(QString::null);
}

#ifndef KAB_EMBEDDED
#include "kaddressbookiconview.moc"
#endif //KAB_EMBEDDED
