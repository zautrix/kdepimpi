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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: xxportmanager.cpp,v 1.13 2005/08/17 19:23:08 zautrix Exp $
*/

#include <qlayout.h>
#include <qlist.h>

#include <kabc/addressbook.h>
#include <kabc/resource.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <kmessagebox.h>

#ifndef KAB_EMBEDDED
#include <ktrader.h>
#else //KAB_EMBEDDED
extern "C"
{
  void* init_microkaddrbk_csv_xxport();
  void* init_microkaddrbk_kde2_xxport();
  void* init_microkaddrbk_vcard_xxport();
  //  void* init_microkaddrbk_opie_xxport();
  void* init_microkaddrbk_qtopia_xxport();
  void* init_microkaddrbk_sharpdtm_xxport();
}
#endif //KAB_EMBEDDED

#include <addresseeview.h>

#include "kabcore.h"
#include "undocmds.h"
#include "xxportselectdialog.h"

#include "xxportmanager.h"
#include "nameeditdialog.h"

KURL XXPortManager::importURL = KURL();
QString XXPortManager::importData = QString::null;

class PreviewDialog : public KDialogBase
{
  public:
    PreviewDialog( const KABC::Addressee &addr,
                   QWidget *parent, const char *name = 0 );
};

XXPortManager::XXPortManager( KABCore *core, QObject *parent, const char *name )
  : QObject( parent, name ), mCore( core ), mShowPreview( false )
{
  loadPlugins();
}

XXPortManager::~XXPortManager()
{
}

void XXPortManager::restoreSettings()
{
}

void XXPortManager::saveSettings()
{
}

void XXPortManager::importVCard( const KURL &url )
{
  importVCard( url, false );
}

void XXPortManager::importVCard( const KURL &url, bool showPreview )
{
  importURL = url;
  mShowPreview = showPreview;
  slotImport( "vcard", "<empty>" );
  mShowPreview = false;
  importURL = KURL();
}

void XXPortManager::importVCard( const QString &vCard, bool showPreview )
{
  importData = vCard;
  mShowPreview = showPreview;
  slotImport( "vcard", "<empty>" );
  mShowPreview = false;
  importData = "";
}

void XXPortManager::slotImport( const QString &identifier, const QString &data )
{
  XXPortObject *obj = mXXPortObjects[ identifier ];
  if ( !obj ) {
    KMessageBox::error( mCore, i18n( "<qt>No import plugin available for <b>%1</b>.</qt>" ).arg( identifier ) );
    return;
  }

  KABC::Resource *resource = mCore->requestResource( mCore );
  if ( !resource )
    return;

  KABC::AddresseeList list = obj->importContacts( data );
  bool useUndo = ( list.count() < 25 );
#ifdef  DESKTOP_VERSION
  useUndo = true;
#endif  
  mShowPreview = true;
  KABC::AddresseeList::Iterator it;
  bool imported = false;
  int count = 0;
  
  KConfig config( locateLocal("config", "kabcrc") );
  config.setGroup( "General" );
  int FormattedNameType = config.readNumEntry( "FormattedNameType", 1 );


  for ( it = list.begin(); it != list.end(); ++it ) {
    if ( mShowPreview ) {
      PreviewDialog dlg( *it, mCore );
      connect( &dlg, SIGNAL( closeClicked() ), this, SLOT ( noPreview()) );
      if ( !dlg.exec() ) {
          if ( mShowPreview )
              continue;
      }
    }
    (*it).setResource( resource );
    if ( (*it).formattedName().isEmpty() )
        (*it).setFormattedName( NameEditDialog::formattedName( (*it), FormattedNameType ) );
    if ( useUndo ) {
        ++count; 
        // We use a PwNewCommand so the user can undo it.
        PwNewCommand *command = new PwNewCommand( mCore->addressBook(), *it );
        UndoStack::instance()->push( command );
        RedoStack::instance()->clear();
    } else {
        ++count;
        mCore->addressBook()->insertAddressee( (*it), true, true );
    }
    imported = true;
  }
  if ( imported ) {
      KMessageBox::information( mCore, i18n( "%1 contacts\nsuccessfully\nimported.").arg( count) );
    emit modified();
  }
}
void XXPortManager::noPreview()
{
    mShowPreview = false;
}
void XXPortManager::slotExport( const QString &identifier, const QString &data )
{
  XXPortObject *obj = mXXPortObjects[ identifier ];
  if ( !obj ) {
    KMessageBox::error( mCore, i18n( "<qt>No export plugin available for <b>%1</b>.</qt>" ).arg( identifier ) );
    return;
  }

  KABC::AddresseeList addrList;
  XXPortSelectDialog dlg( mCore, obj->requiresSorting(), mCore );
  if ( dlg.exec() )
    addrList = dlg.contacts();
  else
    return;
  if ( !obj->exportContacts( addrList, data ) )
    KMessageBox::error( mCore, i18n( "Unable to export contacts." ) );
  else
    KMessageBox::information( mCore, i18n( "contacts successfully exported." ) );
}

void XXPortManager::loadPlugins()
{
  mXXPortObjects.clear();

#ifndef KAB_EMBEDDED
  KTrader::OfferList plugins = KTrader::self()->query( "KAddressBook/XXPort" );
  KTrader::OfferList::ConstIterator it;
  for ( it = plugins.begin(); it != plugins.end(); ++it ) {
    if ( !(*it)->hasServiceType( "KAddressBook/XXPort" ) )
      continue;

    KLibFactory *factory = KLibLoader::self()->factory( (*it)->library().latin1() );
    if ( !factory ) {
      kdDebug(5720) << "XXPortManager::loadExtensions(): Factory creation failed" << endl;
      continue;
    }

    XXPortFactory *xxportFactory = static_cast<XXPortFactory*>( factory );

    if ( !xxportFactory ) {
      kdDebug(5720) << "XXPortManager::loadExtensions(): Cast failed" << endl;
      continue;
    }

#else //KAB_EMBEDDED
  QList<XXPortFactory> factorylist;
  factorylist.append(static_cast<XXPortFactory*>(init_microkaddrbk_csv_xxport()));
  factorylist.append(static_cast<XXPortFactory*>(init_microkaddrbk_kde2_xxport()));
  factorylist.append(static_cast<XXPortFactory*>(init_microkaddrbk_vcard_xxport()));
  //US  factorylist.append(static_cast<XXPortFactory*>(init_microkaddrbk_opie_xxport()));
  factorylist.append(static_cast<XXPortFactory*>(init_microkaddrbk_qtopia_xxport()));
  factorylist.append(static_cast<XXPortFactory*>(init_microkaddrbk_sharpdtm_xxport()));

  QListIterator<XXPortFactory> it(factorylist);
  for ( ; it.current(); ++it )
  {
    XXPortFactory *xxportFactory = it.current();
#endif //KAB_EMBEDDED

    XXPortObject *obj = xxportFactory->xxportObject( mCore->addressBook(), mCore );
    if ( obj ) {
      mCore->addGUIClient( obj );
      mXXPortObjects.insert( obj->identifier(), obj );
      connect( obj, SIGNAL( exportActivated( const QString&, const QString& ) ),
               this, SLOT( slotExport( const QString&, const QString& ) ) );
      connect( obj, SIGNAL( importActivated( const QString&, const QString& ) ),
               this, SLOT( slotImport( const QString&, const QString& ) ) );
    }
  }
}


PreviewDialog::PreviewDialog( const KABC::Addressee &addr, QWidget *parent,
                              const char *name )
  : KDialogBase( Plain, i18n( "Import this contact?" ), Ok | User1| User2, Ok, parent,
                 name, true, true ,i18n( "Import all!" ),i18n( "No" ) )
{
  QWidget *page = plainPage();
  QVBoxLayout *layout = new QVBoxLayout( page, marginHint(), spacingHint() );
  connect( this, SIGNAL( user1Clicked() ), this, SLOT ( slotClose() ) );
  connect( this, SIGNAL( user2Clicked() ), this, SLOT ( slotCancel() ) );

  KABC::AddresseeView *view = new KABC::AddresseeView( page );
  view->setAddressee( addr );
  layout->addWidget( view );
#ifdef DESKTOP_VERSION
  resize( 640, 480 );
#else
  showMaximized();
#endif
}

#ifndef KAB_EMBEDDED
#include "xxportmanager.moc"
#endif //KAB_EMBEDDED

