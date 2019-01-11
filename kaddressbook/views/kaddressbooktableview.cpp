// $Id: kaddressbooktableview.cpp,v 1.24 2005/08/23 21:48:54 zautrix Exp $

#include <qvbox.h>
#include <qlistbox.h>
#include <qwidget.h>
#include <qfile.h>
#include <qimage.h>
#include <qcombobox.h>
#include <qapplication.h>
#include <qdragobject.h>
#include <qevent.h>
#include <qurl.h>
#include <qpixmap.h>

#include <kabc/addressbook.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kcolorbutton.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kurlrequester.h>

//US#include "configuretableviewdialog.h"
#include "contactlistview.h"
#include "kabprefs.h"
#include "undocmds.h"
#include "viewmanager.h"

#include <qlayout.h>
#include <qheader.h>
#include <qregexp.h>

#include "kaddressbooktableview.h"


KAddressBookTableView::KAddressBookTableView( KABC::AddressBook *ab,
                                              QWidget *parent, const char *name )
  : KAddressBookView( ab, parent, name )
{
  mainLayout = new QVBoxLayout( viewWidget(), 2 );

  // The list view will be created when the config is read.
  mListView = 0;
}

KAddressBookTableView::~KAddressBookTableView()
{
}
void KAddressBookTableView::setFocusAV()
{
    if ( mListView )
        mListView->setFocus();

}
void KAddressBookTableView::scrollUP()
{
    QKeyEvent * ev = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Up, 0,0 );
    QApplication::postEvent( mListView, ev );  
    ev = new QKeyEvent ( QEvent::KeyRelease, Qt::Key_Up, 0,0 );
    QApplication::postEvent( mListView, ev ); 
}
void KAddressBookTableView::scrollDOWN()
{
    QKeyEvent * ev = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Down, 0,0 );
    QApplication::postEvent( mListView, ev );
    ev = new QKeyEvent ( QEvent::KeyRelease, Qt::Key_Down, 0,0 );
    QApplication::postEvent( mListView, ev ); 
}
void KAddressBookTableView::reconstructListView()
{
    if (mListView)
    {
        disconnect(mListView, SIGNAL(selectionChanged()),
                   this, SLOT(addresseeSelected()));
        disconnect(mListView, SIGNAL(executed(QListViewItem*)),
                   this, SLOT(addresseeExecuted(QListViewItem*)));
        disconnect(mListView, SIGNAL(doubleClicked(QListViewItem*)),
                   this, SLOT(addresseeExecuted(QListViewItem*)));
        disconnect(mListView, SIGNAL(startAddresseeDrag()), this,
                   SIGNAL(startDrag()));
        disconnect(mListView, SIGNAL(returnPressed(QListViewItem*)),
          this, SLOT(addresseeExecuted(QListViewItem*)));

        disconnect(mListView, SIGNAL(addresseeDropped(QDropEvent*)), this,
                   SIGNAL(dropped(QDropEvent*)));
        delete mListView;
    }

  mListView = new ContactListView( this, addressBook(), viewWidget() );
  
    connect(this, SIGNAL(printView()),
            mListView , SLOT(printMe()));
  //US set singleClick manually, because it is no global configparameter in embedded space 
  mListView->setSingleClick(KABPrefs::instance()->mHonorSingleClick);

  // Add the columns
  KABC::Field::List fieldList = fields();
  KABC::Field::List::ConstIterator it;

  int c = 0;
  for( it = fieldList.begin(); it != fieldList.end(); ++it ) {
      mListView->addColumn( (*it)->label() );
      mListView->setColumnWidthMode(c++, QListView::Manual);
//US
      //  qDebug("KAddressBookTableView::reconstructListView: field %s", (*it)->label().latin1());
  }

  connect(mListView, SIGNAL(selectionChanged()),
          this, SLOT(addresseeSelected()));
  connect(mListView, SIGNAL(startAddresseeDrag()), this,
          SIGNAL(startDrag()));
  connect(mListView, SIGNAL(addresseeDropped(QDropEvent*)), this,
          SIGNAL(dropped(QDropEvent*)));

  if (KABPrefs::instance()->mHonorSingleClick) {
    //    qDebug("KAddressBookTableView::reconstructListView single");
    connect(mListView, SIGNAL(executed(QListViewItem*)),
          this, SLOT(addresseeExecuted(QListViewItem*)));
  } else {
    //    qDebug("KAddressBookTableView::reconstructListView double");
    connect(mListView, SIGNAL(doubleClicked(QListViewItem*)),
          this, SLOT(addresseeExecuted(QListViewItem*)));
  }
    connect(mListView, SIGNAL(returnPressed(QListViewItem*)),
          this, SLOT(addresseeExecuted(QListViewItem*)));
    connect(mListView, SIGNAL(signalDelete()),
          this, SLOT(addresseeDeleted()));

//US performceimprovement. Refresh is done from the outside
//US refresh();

  mListView->setSorting( 0, true );
  mainLayout->addWidget( mListView );
  mainLayout->activate();
  mListView->show();
}

void KAddressBookTableView::doSearch( const QString&  s, KABC::Field *field  )
{
    mListView->clear();
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
                    ContactListViewItem *item = new ContactListViewItem(*it, mListView, addressBook(), fields());

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
                            //qDebug("match %s %s %s", pattern.latin1(), (*fieldIt)->value( *it ).latin1(), (*fieldIt)->label().latin1()  );
                            ContactListViewItem *item = new ContactListViewItem(*it, mListView, addressBook(), fields());
                            match = true;
                            break;
                        }
            }
            if ( ! match ) {
                if ( (*it).matchPhoneNumber( &re ) ) {
                    ContactListViewItem *item = new ContactListViewItem(*it, mListView, addressBook(), fields());
                    match = true;
                    break;
                }
            } 
            if ( ! match ) {
                if ( (*it).matchAddress( &re ) ) {
                    ContactListViewItem *item = new ContactListViewItem(*it, mListView, addressBook(), fields());
                    match = true;
                    break;
                }
            }
        }
    }
    // Sometimes the background pixmap gets messed up when we add lots
    // of items.
    //mListView->repaint();
    if ( mListView->firstChild() ) {
        mListView->setCurrentItem ( mListView->firstChild() );
        mListView->setSelected ( mListView->firstChild(), true );
    }
    else
        emit selected(QString::null);
   
}
void KAddressBookTableView::writeConfig(KConfig *config)
{
  KAddressBookView::writeConfig(config);

  mListView->saveLayout(config, config->group());
}

void KAddressBookTableView::readConfig(KConfig *config)
{
  KAddressBookView::readConfig( config );
  // The config could have changed the fields, so we need to reconstruct
  // the listview.
  reconstructListView();

  // costum colors?
  if ( config->readBoolEntry( "EnableCustomColors", false ) )
  {
    QPalette p( mListView->palette() );
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
#ifndef KAB_EMBEDDED  
  c = KGlobalSettings::alternateBackgroundColor();
#else //KAB_EMBEDDED  
  c = QColor(240, 240, 240);
#endif //KAB_EMBEDDED  
    c = config->readColorEntry ("AlternatingBackgroundColor", &c);
    mListView->setAlternateColor(c);


    //US    mListView->viewport()->setPalette( p );
    mListView->setPalette( p );
  }
  else
  {
    // needed if turned off during a session.
    //US    mListView->viewport()->setPalette( mListView->palette() );
    mListView->setPalette( mListView->palette() );
  }
  
  //custom fonts?
  QFont f( font() );
  if ( config->readBoolEntry( "EnableCustomFonts", false ) )
  {
    mListView->setFont( config->readFontEntry( "TextFont", &f) );
    f.setBold( true );
    //US    mListView->setHeaderFont( config->readFontEntry( "HeaderFont", &f ) );
    mListView->header()->setFont( config->readFontEntry( "HeaderFont", &f ) );
  }
  else
  {
    mListView->setFont( f );
    f.setBold( true );
    //US    mListView->setHeaderFont( f );
    mListView->header()->setFont( f );
  }  

  // Set the list view options
  mListView->setAlternateBackgroundEnabled(config->readBoolEntry("ABackground",
                                                                 true));
  mListView->setSingleLineEnabled(config->readBoolEntry("SingleLine", false));
  mListView->setToolTipsEnabled(config->readBoolEntry("ToolTips", true));

  if (config->readBoolEntry("Background", false))
    mListView->setBackgroundPixmap(config->readEntry("BackgroundName"));

  // Restore the layout of the listview
  mListView->restoreLayout(config, config->group());
}

void KAddressBookTableView::refresh(QString uid)
{
  // For now just repopulate. In reality this method should
  // check the value of uid, and if valid iterate through
  // the listview to find the entry, then tell it to refresh.

  if (uid.isNull()) {
    // Clear the list view
    QString currentUID, nextUID;
#ifndef KAB_EMBEDDED
    ContactListViewItem *currentItem = dynamic_cast<ContactListViewItem*>( mListView->currentItem() );
#else //KAB_EMBEDDED
    ContactListViewItem *currentItem = (ContactListViewItem*)( mListView->currentItem() );
#endif //KAB_EMBEDDED

    if ( currentItem ) {
#ifndef KAB_EMBEDDED
      ContactListViewItem *nextItem = dynamic_cast<ContactListViewItem*>( currentItem->itemBelow() );
#else //KAB_EMBEDDED
      ContactListViewItem *nextItem = (ContactListViewItem*)( currentItem->itemBelow() );
#endif //KAB_EMBEDDED
      if ( nextItem )
        nextUID = nextItem->addressee().uid();
      currentUID = currentItem->addressee().uid();
    }

    mListView->clear();

    currentItem = 0;
    KABC::Addressee::List addresseeList = addressees();
    KABC::Addressee::List::Iterator it;
    for (it = addresseeList.begin(); it != addresseeList.end(); ++it ) {
        if ( (*it).uid().left(2) == "la" && (*it).uid().left(19) == QString("last-syncAddressee-") )
            continue;
      ContactListViewItem *item = new ContactListViewItem(*it, mListView, addressBook(), fields());
      if ( (*it).uid() == currentUID )
        currentItem = item;
      else if ( (*it).uid() == nextUID && !currentItem )
        currentItem = item;
    }

    // Sometimes the background pixmap gets messed up when we add lots
    // of items.
    mListView->repaint();
    if ( !currentItem ) 
        currentItem =  (ContactListViewItem *)mListView->firstChild();
    if ( currentItem ) {
      mListView->setCurrentItem( currentItem );
      mListView->ensureItemVisible( currentItem );
      mListView->setSelected( currentItem, true );
    }
  } else {
    // Only need to update on entry. Iterate through and try to find it
    ContactListViewItem *ceItem;
    QListViewItemIterator it( mListView );
    while ( it.current() ) {
#ifndef KAB_EMBEDDED
      ceItem = dynamic_cast<ContactListViewItem*>( it.current() );
#else //KAB_EMBEDDED
      ceItem = (ContactListViewItem*)( it.current() );
#endif //KAB_EMBEDDED

      if ( ceItem && ceItem->addressee().uid() == uid ) {
        ceItem->refresh();
        return;
      }
      ++it;
    }

    refresh( QString::null );
  }
}

QStringList KAddressBookTableView::selectedUids()
{
    QStringList uidList;
    QListViewItem *item;
    ContactListViewItem *ceItem;

    for(item = mListView->firstChild(); item; item = item->itemBelow())
    {
        if (mListView->isSelected( item ))
        {
#ifndef KAB_EMBEDDED
            ceItem = dynamic_cast<ContactListViewItem*>(item);
#else //KAB_EMBEDDED
            ceItem = (ContactListViewItem*)(item);
#endif //KAB_EMBEDDED

            if (ceItem != 0L)
                uidList << ceItem->addressee().uid();
        }
    }
    if ( uidList.count() == 0 )
        if ( mListView->currentItem() ) {
            ceItem = (ContactListViewItem*)(mListView->currentItem()) ;
            uidList << ceItem->addressee().uid();
        }

    return uidList;
}

void KAddressBookTableView::setSelected(QString uid, bool selected)
{
    QListViewItem *item;
    ContactListViewItem *ceItem;

    if (uid.isNull())
    {
        mListView->selectAll(selected);
    }
    else
    {
        for(item = mListView->firstChild(); item; item = item->itemBelow())
        {
#ifndef KAB_EMBEDDED
            ceItem = dynamic_cast<ContactListViewItem*>(item);
#else //KAB_EMBEDDED
            ceItem = (ContactListViewItem*)(item);
#endif //KAB_EMBEDDED


            if ((ceItem != 0L) && (ceItem->addressee().uid() == uid))
            {
                mListView->setSelected(item, selected);

                if (selected)
                    mListView->ensureItemVisible(item);
            }
        }
    }
}

void KAddressBookTableView::addresseeSelected()
{
    // We need to try to find the first selected item. This might not be the
    // last selected item, but when QListView is in multiselection mode,
    // there is no way to figure out which one was
    // selected last.
    QListViewItem *item;
    bool found =false;
    for (item = mListView->firstChild(); item && !found;
         item = item->nextSibling())
    {
        if (item->isSelected())
        {
            found = true;
#ifndef KAB_EMBEDDED
            ContactListViewItem *ceItem
                 = dynamic_cast<ContactListViewItem*>(item);
#else //KAB_EMBEDDED
            ContactListViewItem *ceItem
                 = (ContactListViewItem*)(item);
#endif //KAB_EMBEDDED

            if ( ceItem ) emit selected(ceItem->addressee().uid());
        }
    }

    if (!found)
        emit selected(QString::null);
}

void KAddressBookTableView::addresseeExecuted(QListViewItem *item)
{
    if (item)
    {
#ifndef KAB_EMBEDDED
        ContactListViewItem *ceItem
               = dynamic_cast<ContactListViewItem*>(item);
#else //KAB_EMBEDDED
            ContactListViewItem *ceItem
                 = (ContactListViewItem*)(item);
#endif //KAB_EMBEDDED

       if (ceItem)
       {
           emit executed(ceItem->addressee().uid());
       }
    }
    else
    {
        emit executed(QString::null);
    }
}

void KAddressBookTableView::addresseeDeleted()
{

         emit deleteRequest();

}





#ifndef KAB_EMBEDDED
#include "kaddressbooktableview.moc"
#endif //KAB_EMBEDDED
