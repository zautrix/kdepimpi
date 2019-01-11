/*
    This file is part of KAddressbook.
    Copyright (c) 1999 Don Sanders <dsanders@kde.org>

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

#ifdef KAB_EMBEDDED
#include "kabprefs.h"
#include <kglobal.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qapplication.h>
#else //KAB_EMBEDDED
#include <kedittoolbar.h>
#include <kkeydialog.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#endif //KAB_EMBEDDED
#include <klocale.h>

#include "kabcore.h"
#include "kaddressbookmain.h"
#include "kactioncollection.h"

#ifdef KAB_EMBEDDED
KAddressBookMain::KAddressBookMain() : KMainWindow( 0, "abmain" )
#else //KAB_EMBEDDED
//MOC_SKIP_BEGIN
KAddressBookMain::KAddressBookMain() : DCOPObject( "KAddressBookIface" ), KMainWindow( 0 ) 
//MOC_SKIP_END
#endif //KAB_EMBEDDED
{
    setIcon(SmallIcon( "ka24" )  );
#if 0
  //US for embedded systems, create the toolbar before we initiate KABCore.
  // KABCore will fill the toolbar with menues and icons
  QMainWindow::ToolBarDock tbd;
  tbd = Top;
  iconToolBar = new QToolBar( this );
  addToolBar (iconToolBar , tbd );
  iconToolBar->setHorizontalStretchable(true);  
//US  iconToolBar->setWidth(300);
#endif // 0

  mCore = new KABCore( this, true, this );
  
#ifdef KAB_EMBEDDED
  setCaption( i18n( "KAddressbook/Pi" ) );
#else //KAB_EMBEDDED
  setCaption( i18n( "Address Book Browser" ) );
#endif //KAB_EMBEDDED

  //mCore->restoreSettings();

  initActions();

  setCentralWidget( mCore );

//US  statusBar()->show();

#ifndef KAB_EMBEDDED
  setStandardToolBarMenuEnabled(true);
  
  createGUI( "kaddressbookui.rc", false );


#endif //KAB_EMBEDDED
  setAutoSaveSettings();
  mCore->restoreSettings();
#ifndef DESKTOP_VERSION
      QObject::connect(qApp, SIGNAL (appMessage ( const QCString &, const QByteArray & )), mCore, SLOT (receiveStart ( const QCString &, const QByteArray & )));
#endif
}

KAddressBookMain::~KAddressBookMain()
{
    // mCore->saveSettings();
}

void KAddressBookMain::showMinimized () 
{
    QWidget::showMinimized () ;
}
void KAddressBookMain::addEmail( QString addr )
{
  mCore->addEmail( addr );
}

#ifndef KAB_EMBEDDED
ASYNC KAddressBookMain::showContactEditor( QString uid )
{
  mCore->editContact( uid );
}
#endif //KAB_EMBEDDED
void KAddressBookMain::newContact()
{
  mCore->newContact();
}

QString KAddressBookMain::getNameByPhone( QString phone )
{
  return mCore->getNameByPhone( phone );
}

void KAddressBookMain::save()
{
  mCore->save();
}

void KAddressBookMain::exit()
{
  close( );
}

void KAddressBookMain::saveProperties( KConfig* )
{
}

void KAddressBookMain::readProperties( KConfig* )
{
}

void KAddressBookMain::initActions()
{
#ifndef KAB_EMBEDDED
  KStdAction::quit( this, SLOT( close() ), actionCollection() );
  KStdAction::configureToolbars( this, SLOT( configureToolbars() ), actionCollection() );
#else //KAB_EMBEDDED
  //US: transfered the setup of the actions into KABCore
#endif //KAB_EMBEDDED

  
}

//US new method to setup menues and toolbars on embedded systems
#ifdef KAB_EMBEDDED
/*
QToolBar * KAddressBookMain::getIconToolBar()
{
  return iconToolBar;
}
*/

void KAddressBookMain::createGUI()
{


    
}
#endif //KAB_EMBEDDED

void KAddressBookMain::configureToolbars()
{
#ifndef KAB_EMBEDDED
  saveMainWindowSettings( KGlobal::config(), "MainWindow" );

  KEditToolbar dlg( factory() );
  connect( &dlg, SIGNAL( newToolbarConfig() ), SLOT( slotNewToolbarConfig() ) );

  dlg.exec();
#else  //KAB_EMBEDDED
    qDebug("KAddressBookMain::configureToolbars() not implemented by ulf" );
#endif //KAB_EMBEDDED
}

void KAddressBookMain::slotNewToolbarConfig()
{
#ifndef KAB_EMBEDDED
  applyMainWindowSettings( KGlobal::config(), "MainWindow" );
#else  //KAB_EMBEDDED
    qDebug("KAddressBookMain::slotNewToolbarConfig() not implemented by ulf" );
#endif //KAB_EMBEDDED
}

void KAddressBookMain::configureKeys()
{
#ifndef KAB_EMBEDDED
  KKeyDialog::configureKeys( actionCollection(), xmlFile(), true, this );
#else  //KAB_EMBEDDED
    qDebug("KAddressBookMain::configureKeys() not implemented by ulf" );
#endif //KAB_EMBEDDED
}

void KAddressBookMain::closeEvent( QCloseEvent* ce )
{
  bool mModified = mCore->modified();
  bool mAskForQuit = KABPrefs::instance()->mAskForQuit;

  QString mess = i18n( "Close KA/Pi?");
  if ( mModified == true ) 
      mess += i18n( "\nChanges will be saved!");
  else
      mess += i18n( "\nNo unsaved changes detected!\nNothing will be  saved!");

  bool mQuit = true;


  if (mAskForQuit)
  { 
    
    int res = QMessageBox::information( this, "KA/Pi", mess , i18n("Yes!"), i18n("No"), 0, 0 );
    if (res == 0)
      mQuit = true;
    else
      mQuit = false;
  }

  if (mQuit == false)
    return;

  if (mModified == true)
  {
    save();
    mCore->saveSettings();
    //KABPrefs::instance()->writeConfig();
  }

  ce->accept();
}

#ifndef KAB_EMBEDDED
#include "kaddressbookmain.moc"
#endif //KAB_EMBEDDED

