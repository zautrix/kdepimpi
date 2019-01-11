/*
    This file is part of KAddressbook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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
#include <qapplication.h>
#include <kactionclasses.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>

#ifndef KAB_EMBEDDED
#include <ktrader.h>
#else //KAB_EMBEDDED
#include <features/mergewidget.h>
#include <features/distributionlistwidget.h>
#endif //KAB_EMBEDDED

#include "addresseeeditorwidget.h"
#include "kabcore.h"
#include "kabprefs.h"

#include "extensionmanager.h"

ExtensionManager::ExtensionManager( KABCore *core, QWidget *parent,
                                    const char *name )
  : QScrollView( parent, name ), mCore( core ), mCurrentExtensionWidget( 0 )
{
#ifdef KAB_EMBEDDED
//US  QPopupMenu *settingsmenu = (QPopupMenu*)mCore->getSettingsMenu();
  QWidget *settingsmenu = (QWidget*)mCore->getViewMenu();
#endif //KAB_EMBEDDED

  mActionExtensions = new KSelectAction( i18n( "Show Extension Bar" ), 0,
                                         mCore->actionCollection(),
                                         "options_show_extensions" );

  mActionExtensions->plug( settingsmenu ,0);

  connect( mActionExtensions, SIGNAL( activated( int ) ),
           SLOT( setActiveExtension( int ) ) );
  mWidgetBox = new QWidget( viewport() );
  new QHBoxLayout (mWidgetBox  );
  addChild( mWidgetBox );
  setResizePolicy(AutoOneFit);
  createExtensionWidgets();
  hide();
}

ExtensionManager::~ExtensionManager()
{
}

void ExtensionManager::restoreSettings()
{
  mActionExtensions->setCurrentItem( KABPrefs::instance()->mCurrentExtension );
  int i = 1;
  mCurrentExtensionWidget = mExtensionWidgetList.at( i-1 );
  while ( mCurrentExtensionWidget ) {
      if( i != KABPrefs::instance()->mCurrentExtension )
          mCurrentExtensionWidget->hide();
      mCurrentExtensionWidget  = mExtensionWidgetList.at( ++i-1 );

  }
  setActiveExtension( mActionExtensions->currentItem() );
}

void ExtensionManager::saveSettings()
{
  KABPrefs::instance()->mCurrentExtension = mActionExtensions->currentItem();
}

void ExtensionManager::reconfigure()
{
  saveSettings();
  createExtensionWidgets();
  restoreSettings();
}

bool ExtensionManager::isQuickEditVisible() const
{
  return ( mCurrentExtensionWidget &&
      mCurrentExtensionWidget->identifier() == "contact_editor" );
}

void ExtensionManager::setSelectionChanged()
{
  if ( mCurrentExtensionWidget )
    mCurrentExtensionWidget->contactsSelectionChanged();
}

void ExtensionManager::setActiveExtension( int id )
{
    //qDebug("+++++++++++++++++++ExtensionManager::setActiveExtension %d ", id);
  if ( id == 0 ) {
    hide();
    mCurrentExtensionWidget = 0;
#ifndef DESKTOP_VERSION
//US  our screen is so small, that we better hide the detailscreen, just in case.
//US    mCore->setDetailsToState( );
#endif //KAB_EMBEDDED
  } else if ( id > 0 ) {
    if ( mCurrentExtensionWidget )
      mCurrentExtensionWidget->hide();

    mCurrentExtensionWidget = mExtensionWidgetList.at( id - 1 );

    if ( mCurrentExtensionWidget ) {
#ifndef DESKTOP_VERSION
//US  our screen is so small, that we better hide the detailscreen, just in case.
//US      mCore->setDetailsVisible( false );
#endif //KAB_EMBEDDED
      show();
      mWidgetBox->show();
      mCurrentExtensionWidget->show();
    } else {
      hide();
      mCurrentExtensionWidget = 0;
#ifndef DESKTOP_VERSION
//US  our screen is so small, that we better hide the detailscreen, just in case.
//US    mCore->setDetailsToState( );
#endif //KAB_EMBEDDED
    }
  }

  emit changedActiveExtension( id );
}

void ExtensionManager::createExtensionWidgets()
{
  // clear extension widget list
  mExtensionWidgetList.setAutoDelete( true );
  QPtrListIterator<ExtensionWidget> wdgIt( mExtensionWidgetList );
  ExtensionWidget *wdg = 0;
  while ( ( wdg = wdgIt.current() ) != 0 )
    mExtensionWidgetList.remove( wdg );

  mExtensionWidgetList.setAutoDelete( false );

  QStringList extensionNames( i18n( "None" ) );

  // add addressee editor as default

  QHBoxLayout *hbl = (QHBoxLayout *) mWidgetBox->layout();;

  wdg = new AddresseeEditorWidget( mCore, true, mWidgetBox );
  hbl->addWidget( wdg );
  //wdg->hide();
  connect( wdg, SIGNAL( modified( const KABC::Addressee::List& ) ),
           SIGNAL( modified( const KABC::Addressee::List& ) ) );
  mExtensionWidgetList.append( wdg );
  extensionNames.append( wdg->title() );

  // load the other extensions
  QStringList activeExtensions = KABPrefs::instance()->mActiveExtensions;

#ifndef KAB_EMBEDDED
  KTrader::OfferList plugins = KTrader::self()->query( "KAddressBook/Extension" );
  KTrader::OfferList::ConstIterator it;
  for ( it = plugins.begin(); it != plugins.end(); ++it ) {
    if ( !(*it)->hasServiceType( "KAddressBook/Extension" ) )
      continue;

    KLibFactory *factory = KLibLoader::self()->factory( (*it)->library().latin1() );
    if ( !factory ) {
        //kdDebug(5720) << "ExtensionManager::loadExtensions(): Factory creation failed" << endl;
      continue;
    }

    ExtensionFactory *extensionFactory = static_cast<ExtensionFactory*>( factory );

    if ( !extensionFactory ) {
        //kdDebug(5720) << "ExtensionManager::loadExtensions(): Cast failed" << endl;
      continue;
    }

    if ( !activeExtensions.contains( extensionFactory->identifier() ) )
      continue;

    wdg = extensionFactory->extension( mCore, this );
    if ( wdg ) {
        //wdg->hide();
      connect( wdg, SIGNAL( modified( const KABC::Addressee::List& ) ),
               SIGNAL( modified( const KABC::Addressee::List& ) ) );
      mExtensionWidgetList.append( wdg );
      extensionNames.append( wdg->title() );
    }
  }
#else //KAB_EMBEDDED
    //load Mergefactory/extension
    ExtensionFactory *extensionFactory = new MergeFactory();
    if ( activeExtensions.contains( extensionFactory->identifier() ) )
    {
      wdg = extensionFactory->extension( mCore, mWidgetBox );
      if ( wdg ) {
          hbl->addWidget( wdg );
          //wdg->hide();
        connect( wdg, SIGNAL( modified( const KABC::Addressee::List& ) ),
                 SIGNAL( modified( const KABC::Addressee::List& ) ) );
        mExtensionWidgetList.append( wdg );
        extensionNames.append( wdg->title() );
      }
    }

    //load DistributionListfactory/extension
    extensionFactory = new DistributionListFactory();
    if (activeExtensions.contains( extensionFactory->identifier() ) )
    {
      wdg = extensionFactory->extension( mCore, mWidgetBox );
      if ( wdg ) {
          hbl->addWidget( wdg );
          //wdg->hide();
        connect( wdg, SIGNAL( modified( const KABC::Addressee::List& ) ),
                 SIGNAL( modified( const KABC::Addressee::List& ) ) );
        mExtensionWidgetList.append( wdg );
        extensionNames.append( wdg->title() );
      }
    }

    hbl->addStretch();

#endif //KAB_EMBEDDED


  mActionExtensions->setItems( extensionNames );
  mCurrentExtensionWidget = 0;
}

#ifndef KAB_EMBEDDED
#include "extensionmanager.moc"
#endif //KAB_EMBEDDED
