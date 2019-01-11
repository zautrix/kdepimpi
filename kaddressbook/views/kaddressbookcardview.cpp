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

#include <qdragobject.h>
#include <qevent.h>
#include <qiconview.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qapplication.h>

#include <kabc/addressbook.h>
#include <kabc/addressee.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>

#include "kabprefs.h"
#include "viewmanager.h"


#include "kaddressbookcardview.h"

#ifndef KAB_EMBEDDED
extern "C" {
  void *init_libkaddrbk_cardview()
  {
    return ( new CardViewFactory );
  }
}
#endif //KAB_EMBEDDED

////////////////////////////////
// AddresseeCardViewItem  (internal class)
class AddresseeCardViewItem : public CardViewItem
{
  public:
    AddresseeCardViewItem(const KABC::Field::List &fields,
                          bool showEmptyFields,
                          KABC::AddressBook *doc, const KABC::Addressee &a,
                          CardView *parent)
      : CardViewItem(parent, a.realName() ),
        mFields( fields ), mShowEmptyFields(showEmptyFields),
        mDocument(doc), mAddressee(a)
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
        {
          clearFields();

          // Try all the selected fields until we find one with text.
          // This will limit the number of unlabeled icons in the view
          KABC::Field::List::Iterator iter;
          for (iter = mFields.begin(); iter != mFields.end(); ++iter)
          {
            // insert empty fields or not? not doing so saves a bit of memory and CPU
            // (during geometry calculations), but prevents having equally
            // wide label columns in all cards, unless CardViewItem/CardView search
            // globally for the widest label. (anders)
            //if (mShowEmptyFields || !(*iter)->value( mAddressee ).isEmpty())
              insertField((*iter)->label(), (*iter)->value( mAddressee ));
          }

          // We might want to make this the first field. hmm... -mpilone
          setCaption( mAddressee.realName() );
        }
    }

  private:
    KABC::Field::List mFields;
    bool mShowEmptyFields;
    KABC::AddressBook *mDocument;
    KABC::Addressee mAddressee;
};

///////////////////////////////
// AddresseeCardView

AddresseeCardView::AddresseeCardView(QWidget *parent, const char *name)
  : CardView(parent, name)
{
  setAcceptDrops(true);
}

AddresseeCardView::~AddresseeCardView()
{
}
void AddresseeCardView::printMe()
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
    drawContents ( &p, 0,0,  contentsWidth (),  contentsHeight () ); 
    p.end();
    repaint();
#endif  
}


void AddresseeCardView::dragEnterEvent(QDragEnterEvent *e)
{
#ifndef KAB_EMBEDDED
  if (QTextDrag::canDecode(e))
    e->accept();
#else //KAB_EMBEDDED
qDebug("AddresseeCardView::dragEnterEvent drag&drop is not implemented");
#endif //KAB_EMBEDDED
}

void AddresseeCardView::dropEvent(QDropEvent *e)
{
  emit addresseeDropped(e);
}

void AddresseeCardView::startDrag()
{
  emit startAddresseeDrag();
}


///////////////////////////////
// KAddressBookCardView

KAddressBookCardView::KAddressBookCardView( KABC::AddressBook *ab,
                                            QWidget *parent, const char *name )
    : KAddressBookView( ab, parent, name )
{
    mShowEmptyFields = false;

    // Init the GUI
    QVBoxLayout *layout = new QVBoxLayout(viewWidget());

    mCardView = new AddresseeCardView(viewWidget(), "mCardView");
    mCardView->setSelectionMode(CardView::Extended);
    layout->addWidget(mCardView);

    // Connect up the signals
    connect(mCardView, SIGNAL(executed(CardViewItem *)),
            this, SLOT(addresseeExecuted(CardViewItem *)));
    connect(mCardView, SIGNAL(selectionChanged()),
            this, SLOT(addresseeSelected()));
    connect(mCardView, SIGNAL(addresseeDropped(QDropEvent*)),
            this, SIGNAL(dropped(QDropEvent*)));
    connect(mCardView, SIGNAL(startAddresseeDrag()),
            this, SIGNAL(startDrag()));
    connect(this, SIGNAL(printView()),
            mCardView , SLOT(printMe()));
}

KAddressBookCardView::~KAddressBookCardView()
{
}
void KAddressBookCardView::setFocusAV()
{
    if ( mCardView )
        mCardView->setFocus();

}
void KAddressBookCardView::scrollUP()
{
    QKeyEvent * ev = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Up, 0,0 );
    QApplication::postEvent( mCardView, ev ); 
    ev = new QKeyEvent ( QEvent::KeyRelease, Qt::Key_Up, 0,0 );
    QApplication::postEvent( mCardView, ev ); 

}
void KAddressBookCardView::scrollDOWN()
{
    QKeyEvent * ev = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Down, 0,0 );
    QApplication::postEvent( mCardView, ev ); 
    ev = new QKeyEvent ( QEvent::KeyRelease, Qt::Key_Down, 0,0 );
    QApplication::postEvent( mCardView, ev ); 
}
void KAddressBookCardView::readConfig(KConfig *config)
{
  KAddressBookView::readConfig(config);
  
  // costum colors?
  if ( config->readBoolEntry( "EnableCustomColors", false ) )
  {
    QPalette p( mCardView->palette() );
    QColor c = p.color(QPalette::Normal, QColorGroup::Base );
    p.setColor( QPalette::Normal, QColorGroup::Base, config->readColorEntry( "BackgroundColor", &c ) );
    c = p.color(QPalette::Normal, QColorGroup::Text );
    p.setColor( QPalette::Normal, QColorGroup::Text, config->readColorEntry( "TextColor", &c ) );
    c = p.color(QPalette::Normal, QColorGroup::Button );
    p.setColor( QPalette::Normal, QColorGroup::Button, config->readColorEntry( "HeaderColor", &c ) );
    c = p.color(QPalette::Normal, QColorGroup::ButtonText );
    p.setColor( QPalette::Normal, QColorGroup::ButtonText, config->readColorEntry( "HeaderTextColor", &c ) );
    c = p.color(QPalette::Normal, QColorGroup::Highlight );
    p.setColor( QPalette::Normal, QColorGroup::Highlight, config->readColorEntry( "HighlightColor", &c ) );
    c = p.color(QPalette::Normal, QColorGroup::HighlightedText );
    p.setColor( QPalette::Normal, QColorGroup::HighlightedText, config->readColorEntry( "HighlightedTextColor", &c ) );
    mCardView->viewport()->setPalette( p );
  }
  else
  {
    // needed if turned off during a session.
    mCardView->viewport()->setPalette( mCardView->palette() );
  }
  
  //custom fonts?
  QFont f( font() );
  if ( config->readBoolEntry( "EnableCustomFonts", false ) )
  {
    mCardView->setFont( config->readFontEntry( "TextFont", &f) );
    f.setBold( true );
    mCardView->setHeaderFont( config->readFontEntry( "HeaderFont", &f ) );
  }
  else
  {
    mCardView->setFont( f );
    f.setBold( true );
    mCardView->setHeaderFont( f );
  }  
  
  mCardView->setDrawCardBorder(config->readBoolEntry("DrawBorder", true));
  mCardView->setDrawColSeparators(config->readBoolEntry("DrawSeparators",
                                                        true));
  mCardView->setDrawFieldLabels(config->readBoolEntry("DrawFieldLabels",false));
  mShowEmptyFields = config->readBoolEntry("ShowEmptyFields", false);
  
  mCardView->setShowEmptyFields( mShowEmptyFields );
  
  mCardView->setItemWidth( config->readNumEntry( "ItemWidth", 200 ) );
  mCardView->setItemMargin( config->readNumEntry( "ItemMargin", 0 ) );
  mCardView->setItemSpacing( config->readNumEntry( "ItemSpacing", 10 ) );
  mCardView->setSeparatorWidth( config->readNumEntry( "SeparatorWidth", 2 ) );
  
#if 0 
  // LR KABPrefs::instance()->mHonorSingleClick is handled and fixed in cardviews contentsMouseDoubleClickEven
  disconnect(mCardView, SIGNAL(executed(CardViewItem *)),
            this, SLOT(addresseeExecuted(CardViewItem *)));

  if (KABPrefs::instance()->mHonorSingleClick)
    connect(mCardView, SIGNAL(executed(CardViewItem *)),
            this, SLOT(addresseeExecuted(CardViewItem *)));
  else
    connect(mCardView, SIGNAL(doubleClicked(CardViewItem *)),
            this, SLOT(addresseeExecuted(CardViewItem *)));
#endif
  
    connect(mCardView, SIGNAL(doubleClicked(CardViewItem *)),
            this, SLOT(addresseeExecuted(CardViewItem *)));
}

void KAddressBookCardView::writeConfig( KConfig *config )
{
  config->writeEntry( "ItemWidth", mCardView->itemWidth() );
  KAddressBookView::writeConfig( config );
}
void KAddressBookCardView::doSearch( const QString&  s,KABC::Field *field  )
{
    mCardView->clear();
    if ( s.isEmpty() || s == "*" ) {
        refresh();
        return;
    }  
    QRegExp re = getRegExp( s );
    if (!re.isValid())
        return; 
    mCardView->viewport()->setUpdatesEnabled( false );
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
                  new AddresseeCardViewItem(fields(), mShowEmptyFields,
                                              addressBook(), *it, mCardView);

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
                            new AddresseeCardViewItem(fields(), mShowEmptyFields,
                                                      addressBook(), *it, mCardView);
                            match = true;
                            break;
                        }
            }
            if ( ! match ) {
                if ( (*it).matchPhoneNumber( &re ) ) { 
                    new AddresseeCardViewItem(fields(), mShowEmptyFields,
                                              addressBook(), *it, mCardView);
                    match = true;
                    break;
                }
            }
            if ( ! match ) {
                if ( (*it).matchAddress( &re ) ) { 
                    new AddresseeCardViewItem(fields(), mShowEmptyFields,
                                              addressBook(), *it, mCardView);
                    match = true;
                    break;
                }
            }
        }
    }
    mCardView->viewport()->setUpdatesEnabled( true );
    mCardView->viewport()->update();
    if ( mCardView->firstItem() ) {
        mCardView->setCurrentItem ( mCardView->firstItem() );
        mCardView->setSelected ( mCardView->firstItem() , true );
    }
    else
        emit selected(QString::null);
}
QStringList KAddressBookCardView::selectedUids()
{
    QStringList uidList;
    CardViewItem *item;
    AddresseeCardViewItem *aItem;

    for (item = mCardView->firstItem(); item; item = item->nextItem())
    {
        if (item->isSelected())
        {
#ifndef KAB_EMBEDDED
            aItem = dynamic_cast<AddresseeCardViewItem*>(item);
#else //KAB_EMBEDDED
            aItem = (AddresseeCardViewItem*)(item);
#endif //KAB_EMBEDDED
            if (aItem)
                uidList << aItem->addressee().uid();
        }
    }

    return uidList;
}

void KAddressBookCardView::refresh(QString uid)
{
    CardViewItem *item;
    AddresseeCardViewItem *aItem;

    if (uid.isNull())
    {
        // Rebuild the view
        mCardView->viewport()->setUpdatesEnabled( false );
        mCardView->clear();

        KABC::Addressee::List addresseeList = addressees();
        KABC::Addressee::List::Iterator iter;
        for (iter = addresseeList.begin(); iter != addresseeList.end(); ++iter)
        {
            if ( (*iter).uid().left(2) == "la" && (*iter).uid().left(19) == QString("last-syncAddressee-") )
                continue;
            aItem = new AddresseeCardViewItem(fields(), mShowEmptyFields,
                                              addressBook(), *iter, mCardView);
        }
        mCardView->viewport()->setUpdatesEnabled( true );
        mCardView->viewport()->update();

        // by default nothing is selected
        emit selected(QString::null);
    }
    else
    {
        // Try to find the one to refresh
        bool found = false;
        for (item = mCardView->firstItem(); item && !found;
             item = item->nextItem())
        {
#ifndef KAB_EMBEDDED
            aItem = dynamic_cast<AddresseeCardViewItem*>(item);
#else //KAB_EMBEDDED
            aItem = (AddresseeCardViewItem*)(item);
#endif //KAB_EMBEDDED
            
            if ((aItem) && (aItem->addressee().uid() == uid))
            {
                aItem->refresh();
                found = true;
            }
        }
    }
}

void KAddressBookCardView::setSelected(QString uid, bool selected)
{
    CardViewItem *item;
    AddresseeCardViewItem *aItem;

    if (uid.isNull())
    {
        mCardView->selectAll(selected);
    }
    else
    {
        bool found = false;
        for (item = mCardView->firstItem(); item && !found;
             item = item->nextItem())
         {
#ifndef KAB_EMBEDDED
             aItem = dynamic_cast<AddresseeCardViewItem*>(item);
#else //KAB_EMBEDDED
            aItem = (AddresseeCardViewItem*)(item);
#endif //KAB_EMBEDDED

             if ((aItem) && (aItem->addressee().uid() == uid))
             {
                 mCardView->setSelected(aItem, selected);
                 mCardView->ensureItemVisible(item);
                 found = true;
             }
         }
    }
}

//US added an additional method without parameter 
void KAddressBookCardView::setSelected()
{
  setSelected(QString::null, true);
}

void KAddressBookCardView::addresseeExecuted(CardViewItem *item)
{
#ifndef KAB_EMBEDDED
    AddresseeCardViewItem *aItem = dynamic_cast<AddresseeCardViewItem*>(item);
#else //KAB_EMBEDDED
    AddresseeCardViewItem *aItem = (AddresseeCardViewItem*)(item);
#endif //KAB_EMBEDDED
    if (aItem)
    {
      //kdDebug()<<"... even has a valid item:)"<<endl;
      emit executed(aItem->addressee().uid());
    }
}

void KAddressBookCardView::addresseeSelected()
{
    CardViewItem *item;
    AddresseeCardViewItem *aItem;

    bool found = false;
    for (item = mCardView->firstItem(); item && !found;
         item = item->nextItem())
    {
        if (item->isSelected())
        {
#ifndef KAB_EMBEDDED
            aItem = dynamic_cast<AddresseeCardViewItem*>(item);
#else //KAB_EMBEDDED
            aItem = (AddresseeCardViewItem*)(item);
#endif //KAB_EMBEDDED
            if ( aItem )
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
#include "kaddressbookcardview.moc"
#endif //KAB_EMBEDDED
