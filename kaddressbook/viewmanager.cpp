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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: viewmanager.cpp,v 1.19 2005/08/17 17:09:57 zautrix Exp $
*/


#ifndef KAB_EMBEDDED
#include <libkdepim/kvcarddrag.h>
#include <kabc/vcardconverter.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmultipledrag.h>
#include <ktrader.h>
#include <kurldrag.h>

#include "addresseeutil.h"
#else //KAB_EMBEDDED
#include "views/kaddressbookiconview.h"
#include "views/kaddressbooktableview.h"
#include "views/kaddressbookcardview.h"
#include "kaddressbookview.h"

#include <qaction.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <kconfigbase.h>

#endif //KAB_EMBEDDED


#include <kdebug.h>
#include <kactionclasses.h>

#include <qlayout.h>
#include <qapplication.h>
#include <qwidgetstack.h>

#include <kabc/addressbook.h>
#include "filtereditdialog.h"
#include "addviewdialog.h"
#include "kabcore.h"
#include "kabprefs.h"
#include "viewmanager.h"

ViewManager::ViewManager( KABCore *core, QWidget *parent, const char *name )
  : QWidget( parent, name ), mCore( core ), mActiveView( 0 )
{
  initGUI();
  initActions();

  mViewDict.setAutoDelete( true );

  createViewFactories();
}

ViewManager::~ViewManager()
{
  unloadViews();
  mViewFactoryDict.clear();
}
void ViewManager::scrollUP()
{
    if ( mActiveView ) 
        mActiveView->scrollUP();
}
void ViewManager::scrollDOWN()
{
    if ( mActiveView ) 
        mActiveView->scrollDOWN();
}
void ViewManager::restoreSettings()
{
  mViewNameList = KABPrefs::instance()->mViewNames;
  QString activeViewName = KABPrefs::instance()->mCurrentView;

  mActionSelectView->setItems( mViewNameList );

  // Filter
  mFilterList = Filter::restore( mCore->config(), "Filter" );
  mActionSelectFilter->setItems( filterNames() );
  mActionSelectFilter->setCurrentItem( KABPrefs::instance()->mCurrentFilter );
  int cw = 150;
  if ( QApplication::desktop()->width() >= 800 )
      cw = 200;
  if (QApplication::desktop()->width() == 480 && !KABPrefs::instance()->mHideSearchOnSwitch)
      cw = 0; 
  mActionSelectFilter->setComboWidth( cw );
  // Tell the views to reread their config, since they may have
  // been modified by global settings
  QString _oldgroup = mCore->config()->group();

  QDictIterator<KAddressBookView> it( mViewDict );
  for ( it.toFirst(); it.current(); ++it ) {
    KConfigGroupSaver saver( mCore->config(), it.currentKey() );
    it.current()->readConfig( mCore->config() );
  }
  setActiveView( activeViewName );

  mActionDeleteView->setEnabled( mViewNameList.count() > 1 );
}

void ViewManager::saveSettings()
{
  QString _oldgroup = mCore->config()->group();

  QDictIterator<KAddressBookView> it( mViewDict );
  for ( it.toFirst(); it.current(); ++it ) {
    KConfigGroupSaver saver( mCore->config(), it.currentKey() );
#ifdef DESKTOP_VERSION
    (*it)->writeConfig( mCore->config() );
#else
    (*it).writeConfig( mCore->config() );
#endif
  }

  Filter::save( mCore->config(), "Filter", mFilterList );
  KABPrefs::instance()->mCurrentFilter = mActionSelectFilter->currentItem();

  // write the view name list
  KABPrefs::instance()->mViewNames = mViewNameList;
  KABPrefs::instance()->mCurrentView = mActiveView->caption();

}

QStringList ViewManager::selectedUids() const
{
  if ( mActiveView )
    return mActiveView->selectedUids();
  else
    return QStringList();
}

QStringList ViewManager::selectedEmails() const
{
  if ( mActiveView )
    return mActiveView->selectedEmails();
  else
    return QStringList();
}

KABC::Addressee::List ViewManager::selectedAddressees() const
{
  KABC::Addressee::List list;
  if ( mActiveView ) {
    QStringList uids = mActiveView->selectedUids();
    QStringList::Iterator it;
    for ( it = uids.begin(); it != uids.end(); ++it ) {
      KABC::Addressee addr = mCore->addressBook()->findByUid( *it );
      if ( !addr.isEmpty() )
        list.append( addr );
    }
  }

  return list;
}
//US added another method with no parameter, since my moc compiler does not support default parameters.
void ViewManager::setSelected()
{
  setSelected( QString::null, true );
}

void ViewManager::setSelected( const QString &uid, bool selected )
{
  if ( mActiveView )
    mActiveView->setSelected( uid, selected );
}

void ViewManager::setListSelected(QStringList list)
{
    int i, count = list.count();
    for ( i = 0; i < count;++i )
        setSelected( list[i], true );

}
void ViewManager::unloadViews()
{
  mViewDict.clear();
  mActiveView = 0;
}

void ViewManager::selectView( const QString &name )
{
    setActiveView( name );
    mCore->saveSettings();
}
void ViewManager::setActiveView( const QString &name )
{
  KAddressBookView *view = 0;

  // Check that this isn't the same as the current active view
  if ( mActiveView && ( mActiveView->caption() == name ) )
    return;

  // At this point we know the view that should be active is not
  // currently active. We will try to find the new on in the list. If
  // we can't find it, it means it hasn't been instantiated, so we will
  // create it on demand.

  view = mViewDict.find( name );

  // Check if we found the view. If we didn't, then we need to create it
  if ( view == 0 ) {
    KConfig *config = mCore->config();

    KConfigGroupSaver saver( config, name );

    QString type = config->readEntry( "Type", "Table" );

    kdDebug(5720) << "ViewManager::setActiveView: creating view - " << name << endl;

    ViewFactory *factory = mViewFactoryDict.find( type );
    if ( factory )
      view = factory->view( mCore->addressBook(), mViewWidgetStack );

    if ( view ) {
        if ( !mViewNameList.contains( name ) ) {
            mViewNameList.append( name );
        }
      view->setCaption( name );
      mViewDict.insert( name, view );
//US my version needs an int as second parameter to addWidget
      mViewWidgetStack->addWidget( view, -1 );
      view->readConfig( config );

      // The manager just relays the signals
      connect( view, SIGNAL( selected( const QString& ) ),
               SIGNAL( selected( const QString & ) ) );
        connect( view, SIGNAL( executed( const QString& ) ),
             SIGNAL( executed( const QString& ) ) );

        connect( view, SIGNAL( deleteRequest( ) ),
             SIGNAL( deleteRequest(  ) ) );

      connect( view, SIGNAL( modified() ), SIGNAL( modified() ) );
      connect( view, SIGNAL( dropped( QDropEvent* ) ),
               SLOT( dropped( QDropEvent* ) ) );
      connect( view, SIGNAL( startDrag() ), SLOT( startDrag() ) );
    }
  }

  // If we found or created the view, raise it and refresh it
  if ( view ) {
    mActiveView = view;
    mViewWidgetStack->raiseWidget( view );
    // Set the proper filter in the view. By setting the combo
    // box, the activated slot will be called, which will push
    // the filter to the view and refresh it.

    if ( view->defaultFilterType() == KAddressBookView::None ) {

      mActionSelectFilter->setCurrentItem( 0 );
      setActiveFilter( 0 );
    } else if ( view->defaultFilterType() == KAddressBookView::Active ) {
      setActiveFilter( mActionSelectFilter->currentItem() );
    } else {
      uint pos = filterPosition( view->defaultFilterName() );
      mActionSelectFilter->setCurrentItem( pos );
      setActiveFilter( pos );
    }
//US  qDebug("ViewManager::setActiveView 6" );

    // Update the inc search widget to show the fields in the new active
    // view.
    mCore->setSearchFields( mActiveView->fields() );

//US performance optimization. setActiveFilter calls also mActiveView->refresh()
//US mActiveView->refresh();

    mActionSelectView->setItems( mViewNameList );
    mActionSelectView->setCurrentItem( mViewNameList.findIndex( mActiveView->caption() ) );

  }
  else
  {
  qDebug("ViewManager::setActiveView: unable to find view" );
  }
}

//US added another method with no parameter, since my moc compiler does not support default parameters.
void ViewManager::refreshView()
{
  refreshView( QString::null );
}

void ViewManager::refreshView( const QString &uid )
{
  if ( mActiveView )
    mActiveView->refresh( uid );
}

void ViewManager::setFocusAV()
{
 if ( mActiveView )
     mActiveView->setFocusAV();
}
void ViewManager::editView()
{
  if ( !mActiveView )
    return;

  ViewFactory *factory = mViewFactoryDict.find( mActiveView->type() );
  ViewConfigureWidget *wdg = 0;
  ViewConfigureDialog* dlg = 0;
  if ( factory ) {
    // Save the filters so the dialog has the latest set
    Filter::save( mCore->config(), "Filter", mFilterList );
    dlg = new  ViewConfigureDialog( 0, mActiveView->caption(), this, "conf_dlg" );
    wdg = factory->configureWidget( mCore->addressBook(), dlg,"conf_wid" );
  } else {
      qDebug("ViewManager::editView()::cannot find viewfactory ");
      return;
  }
  if ( wdg ) {
     dlg->setWidget( wdg );

#ifndef DESKTOP_VERSION
    //dlg.setMaximumSize( 640, 480 );
    //dlg->setGeometry( 40,40, 400, 300);
    dlg->showMaximized();
#endif

    KConfigGroupSaver saver( mCore->config(), mActiveView->caption() );

    dlg->restoreSettings( mCore->config() );

    if ( dlg->exec() ) {
      dlg->saveSettings( mCore->config() );
      mActiveView->readConfig( mCore->config() );

      // Set the proper filter in the view. By setting the combo
      // box, the activated slot will be called, which will push
      // the filter to the view and refresh it.
      if ( mActiveView->defaultFilterType() == KAddressBookView::None ) {
        mActionSelectFilter->setCurrentItem( 0 );
        setActiveFilter( 0 );
      } else if ( mActiveView->defaultFilterType() == KAddressBookView::Active ) {
        setActiveFilter( mActionSelectFilter->currentItem() );
      } else {
        uint pos = filterPosition( mActiveView->defaultFilterName() );
        mActionSelectFilter->setCurrentItem( pos );
        setActiveFilter( pos );
      }
      mCore->setSearchFields( mActiveView->fields() );
//US performance optimization. setActiveFilter calls also mActiveView->refresh()
//US      mActiveView->refresh();


      //US this is a bugfix, that we get notified if we change a views configuration
      emit modified();

    }

  }
  delete dlg;
}

void ViewManager::deleteView()
{
  QString text = i18n( "<qt>Are you sure that you want to delete the view <b>%1</b>?</qt>" )
                     .arg( mActiveView->caption() );
  QString caption = i18n( "Confirm Delete" );


   if (QMessageBox::information( this, caption,
                                      text,
                                      i18n("Yes!"), i18n("No"), 0, 0 ) == 0)
   {
    mViewNameList.remove( mActiveView->caption() );

    // remove the view from the config file
    KConfig *config = mCore->config();
    config->deleteGroup( mActiveView->caption() );

    mViewDict.remove( mActiveView->caption() );
    mActiveView = 0;

    // we are in an invalid state now, but that should be fixed after
    // we emit the signal
    mActionSelectView->setItems( mViewNameList );
    if ( mViewNameList.count() > 0 ) {
      mActionSelectView->setCurrentItem( 0 );
      setActiveView( mViewNameList[ 0 ] );
    }
    mActionDeleteView->setEnabled( mViewNameList.count() > 1 );
  }
}

void ViewManager::addView()
{
  AddViewDialog dialog( &mViewFactoryDict, this );

  if ( dialog.exec() ) {
    QString newName = dialog.viewName();
    QString type = dialog.viewType();

    // Check for name conflicts
    bool firstConflict = true;
    int numTries = 1;
    while ( mViewNameList.contains( newName ) > 0 ) {
      if ( !firstConflict ) {
        newName = newName.left( newName.length() - 4 );
        firstConflict = false;
      }

      newName = QString( "%1 <%2>" ).arg( newName ).arg( numTries );
      numTries++;
    }

    // Add the new one to the list
    mViewNameList.append( newName );

    // write the view to the config file,
    KConfig *config = mCore->config();

    config->deleteGroup( newName );

    KConfigGroupSaver saver( config, newName );

    config->writeEntry( "Type", type );

    // try to set the active view
    mActionSelectView->setItems( mViewNameList );
    mActionSelectView->setCurrentItem( mViewNameList.findIndex( newName ) );
    setActiveView( newName );

    editView();

    mActionDeleteView->setEnabled( mViewNameList.count() > 1 );
  }
}

void ViewManager::createViewFactories()
{
#ifndef KAB_EMBEDDED
  KTrader::OfferList plugins = KTrader::self()->query( "KAddressBook/View" );
  KTrader::OfferList::ConstIterator it;
  for ( it = plugins.begin(); it != plugins.end(); ++it ) {
    if ( !(*it)->hasServiceType( "KAddressBook/View" ) )
      continue;

    KLibFactory *factory = KLibLoader::self()->factory( (*it)->library().latin1() );

    if ( !factory ) {
      kdDebug(5720) << "ViewManager::createViewFactories(): Factory creation failed" << endl;
      continue;
    }

    ViewFactory *viewFactory = static_cast<ViewFactory*>( factory );

    if ( !viewFactory ) {
      kdDebug(5720) << "ViewManager::createViewFactories(): Cast failed" << endl;
      continue;
    }

    mViewFactoryDict.insert( viewFactory->type(), viewFactory );
  }

#else //KAB_EMBEDDED
  ViewFactory* viewFactory = new IconViewFactory();
  mViewFactoryDict.insert( viewFactory->type(), viewFactory );
//  qDebug("ViewManager::createViewFactories() Loading factory: %s", viewFactory->type().latin1());

  viewFactory = new TableViewFactory();
  mViewFactoryDict.insert( viewFactory->type(), viewFactory );
//  qDebug("ViewManager::createViewFactories() Loading factory: %s", viewFactory->type().latin1());

  viewFactory = new CardViewFactory();
  mViewFactoryDict.insert( viewFactory->type(), viewFactory );
//  qDebug("ViewManager::createViewFactories() Loading factory: %s", viewFactory->type().latin1());

#endif //KAB_EMBEDDED

}

void ViewManager::dropped( QDropEvent *e )
{

#ifndef KAB_EMBEDDED

  QString clipText, vcards;
  KURL::List urls;

  if ( KURLDrag::decode( e, urls) ) {
    KURL::List::Iterator it = urls.begin();
    int c = urls.count();
    if ( c > 1 ) {
      QString questionString = i18n( "Import one contact into your addressbook?", "Import %n contacts into your addressbook?", c );
      if ( KMessageBox::questionYesNo( this, questionString, i18n( "Import Contacts?" ) ) == KMessageBox::Yes ) {
        for ( ; it != urls.end(); ++it )
          emit urlDropped( *it );
      }
    } else if ( c == 1 )
      emit urlDropped( *it );
  } else if ( KVCardDrag::decode( e, vcards ) ) {
    KABC::Addressee addr;
    KABC::VCardConverter converter;
    QStringList list = QStringList::split( "\r\n\r\n", vcards );
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
      if ( converter.vCardToAddressee( (*it).stripWhiteSpace(), addr ) ) {
        KABC::Addressee a = mCore->addressBook()->findByUid( addr.uid() );
        if ( a.isEmpty() ) {
          mCore->addressBook()->insertAddressee( addr );
          emit modified();
        }
      }
    }

    mActiveView->refresh();
  }
#else //KAB_EMBEDDED
qDebug("ViewManager::dropped() has to be changed!!" );
#endif //KAB_EMBEDDED

}

void ViewManager::startDrag()
{
  kdDebug(5720) << "ViewManager::startDrag: starting to drag" << endl;

#ifndef KAB_EMBEDDED

  // Get the list of all the selected addressees
  KABC::Addressee::List addrList;
  QStringList uidList = selectedUids();
  QStringList::Iterator iter;
  for ( iter = uidList.begin(); iter != uidList.end(); ++iter )
    addrList.append( mCore->addressBook()->findByUid( *iter ) );

  KMultipleDrag *drag = new KMultipleDrag( this );
  drag->addDragObject( new QTextDrag( AddresseeUtil::addresseesToClipboard(addrList), this ) );
  KABC::Addressee::List::Iterator it;
  QStringList vcards;
  for ( it = addrList.begin(); it != addrList.end(); ++it ) {
    QString vcard = QString::null;
    KABC::VCardConverter converter;
    if ( converter.addresseeToVCard( *it, vcard ) )
      vcards.append( vcard );
  }
  drag->addDragObject( new KVCardDrag( vcards.join( "\r\n" ), this ) );

  drag->setPixmap( KGlobal::iconLoader()->loadIcon( "vcard", KIcon::Desktop ) );
  drag->dragCopy();

#else //KAB_EMBEDDED
qDebug("ViewManager::startDrag() has to be changed!!" );
#endif //KAB_EMBEDDED

}
void ViewManager::doSearch( const QString&  s,KABC::Field *field  )
{
    if ( mActiveView ) 
        mActiveView->doSearch( s, field  );

}
void ViewManager::setActiveFilter( int index )
{
  Filter currentFilter;

  if ( ( index - 1 ) < 0 )
    currentFilter = Filter();
  else
    currentFilter = mFilterList[ index - 1 ];

  // Check if we have a view. Since the filter combo is created before
  // the view, this slot could be called before there is a valid view.
  if ( mActiveView ) {
    mActiveView->setFilter( currentFilter );
    mActiveView->refresh();
    emit selected( QString::null );
  }
}

void ViewManager::configureFilters()
{
  FilterDialog dlg( this );

  dlg.setFilters( mFilterList );

  if ( dlg.exec() )
    mFilterList = dlg.filters();

  uint pos = mActionSelectFilter->currentItem();
  mActionSelectFilter->setItems( filterNames() );
  mActionSelectFilter->setCurrentItem( pos );
  setActiveFilter( pos );
  int cw = 150;
  if ( QApplication::desktop()->width() >= 800 )
      cw = 200;
  if (QApplication::desktop()->width() == 480 && !KABPrefs::instance()->mHideSearchOnSwitch)
      cw = 0; 
  mActionSelectFilter->setComboWidth( cw );
  saveSettings();
}

QStringList ViewManager::filterNames() const
{
  QStringList names( i18n( "No Filter" ) );

  Filter::List::ConstIterator it;
  for ( it = mFilterList.begin(); it != mFilterList.end(); ++it )
    names.append( (*it).name() );

  return names;
}
Filter ViewManager::getFilterByName( const QString &name ) const
{
  Filter::List::ConstIterator it;
  for ( it = mFilterList.begin(); it != mFilterList.end(); ++it )
    if ( name == (*it).name() )
      return (*it);

  return Filter();
}

int ViewManager::filterPosition( const QString &name ) const
{
  int pos = 0;

  Filter::List::ConstIterator it;
  for ( it = mFilterList.begin(); it != mFilterList.end(); ++it, ++pos )
    if ( name == (*it).name() )
      return pos + 1;

  return 0;
}

void ViewManager::initActions()
{
//US      <ActionList name="view_loadedviews"/>
//US      <Separator/>

#ifdef KAB_EMBEDDED
  QPopupMenu *viewmenu = (QPopupMenu*)mCore->getViewMenu();
  QPopupMenu *settingsmenu = (QPopupMenu*)mCore->getSettingsMenu();
  QPopupMenu *filtermenu = (QPopupMenu*)mCore->getFilterMenu();
#endif //KAB_EMBEDDED

  mActionSelectView = new KSelectAction( i18n( "Select View" ), 0, mCore->actionCollection(), "select_view" );
#if KDE_VERSION >= 309
  mActionSelectView->setMenuAccelsEnabled( false );
#endif
  connect( mActionSelectView, SIGNAL( activated( const QString& ) ),
           SLOT( selectView( const QString& ) ) );


#ifdef KAB_EMBEDDED
  mActionSelectView->plug(viewmenu);
  viewmenu->insertSeparator();
#endif //KAB_EMBEDDED

  KAction *action;

  action = new KAction( i18n( "Modify View..." ), "configure", 0, this,
               SLOT( editView() ), mCore->actionCollection(), "view_modify" );
#ifndef KAB_EMBEDDED
  action->setWhatsThis( i18n( "By pressing this button a dialog opens that allows you to modify the view of the addressbook. There you can add or remove fields that you want to be shown or hidden in the addressbook like the name for example." ) );
#else //KAB_EMBEDDED
  action->plug(viewmenu);
#endif //KAB_EMBEDDED

  action = new KAction( i18n( "Add View..." ), "window_new", 0, this,
               SLOT( addView() ), mCore->actionCollection(), "view_add" );
#ifndef KAB_EMBEDDED
  action->setWhatsThis( i18n( "You can add a new view by choosing one of the dialog that appears after pressing the button. You have to give the view a name, so that you can distinguish between the different views." ) );
#else //KAB_EMBEDDED
  action->plug(viewmenu);
#endif //KAB_EMBEDDED

  mActionDeleteView = new KAction( i18n( "Delete View" ), "view_remove", 0,
                                   this, SLOT( deleteView() ),
                                   mCore->actionCollection(), "view_delete" );
#ifndef KAB_EMBEDDED
  mActionDeleteView->setWhatsThis( i18n( "By pressing this button you can delete the actual view, which you have added before." ) );
#else //KAB_EMBEDDED
  mActionDeleteView->plug(viewmenu);
  viewmenu->insertSeparator();
#endif //KAB_EMBEDDED

#ifndef KAB_EMBEDDED
  action = new KAction( i18n( "Refresh View" ), "reload", 0, this,
               SLOT( refreshView(const QString &) ), mCore->actionCollection(),
               "view_refresh" );
  action->setWhatsThis( i18n( "The view will be refreshed by pressing this button." ) );
#else //KAB_EMBEDDED
  action = new KAction( i18n( "Refresh View" ), "reload", 0, this,
               SLOT( refreshView()), mCore->actionCollection(),
               "view_refresh" );
  action->plug(viewmenu);
  //viewmenu->insertSeparator();
#endif //KAB_EMBEDDED

  action = new KAction( i18n( "Edit &Filters..." ), "filter", 0, this,
               SLOT( configureFilters() ), mCore->actionCollection(),
               "options_edit_filters" );

  mActionSelectFilter = new KSelectAction( i18n( "Select Filter" ), "filter", mCore->actionCollection(), "select_filter" );
#if KDE_VERSION >= 309
  mActionSelectFilter->setMenuAccelsEnabled( false );
#endif
  connect( mActionSelectFilter, SIGNAL( activated( int ) ),
           SLOT( setActiveFilter( int ) ) );

#ifdef KAB_EMBEDDED
  action->plug(settingsmenu);
  mActionSelectFilter->plug(viewmenu,0);
#endif //KAB_EMBEDDED

}

void ViewManager::initGUI()
{
    QHBoxLayout *layout = new QHBoxLayout( this, 0, 0 );
  mViewWidgetStack = new QWidgetStack( this );
  layout->addWidget( mViewWidgetStack );
}

#ifndef KAB_EMBEDDED
#include "viewmanager.moc"
#endif //KAB_EMBEDDED
