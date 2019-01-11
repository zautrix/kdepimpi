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

//US#ifdef KAB_EMBEDDED
//#include <qstring.h>
//#endif //KAB_EMBEDDED

#include <qtextstream.h>
#include <qfile.h>
#include <qregexp.h>
#include <stdlib.h>
#include <libkdepim/kpimglobalprefs.h>

#include <kconfig.h>
#include <klocale.h>
#include <kstaticdeleter.h>
#include <kglobalsettings.h>
//US#include <kdebug.h> // defines kdDebug()

#include "kabprefs.h"

#ifdef DESKTOP_VERSION
#include <qapplication.h>
#endif

KABPrefs *KABPrefs::sInstance = 0;
static KStaticDeleter<KABPrefs> staticDeleterAB;

KABPrefs::KABPrefs()
  : KPimPrefs("kaddressbookrc")
{
  KPrefs::setCurrentGroup( "Views" );
  addItemBool( "HonorSingleClick", &mHonorSingleClick, false );

  KPrefs::setCurrentGroup( "General" );
  addItemBool( "AutomaticNameParsing", &mAutomaticNameParsing, true );
  addItemInt( "CurrentIncSearchField", &mCurrentIncSearchField, 0 );
#ifdef KAB_EMBEDDED
  addItemBool("AskForDelete",&mAskForDelete,true);
  addItemBool("AskForQuit",&mAskForQuit,true);
  addItemBool("ToolBarHor",&mToolBarHor, true );
  addItemBool("ToolBarUp",&mToolBarUp, false );
  addItemBool("SearchWithReturn",&mSearchWithReturn, false );
  addItemBool("AutoSearchWithWildcard",&mAutoSearchWithWildcard, false );
  addItemBool("HideSearchOnSwitch",&mHideSearchOnSwitch, false );
  addItemFont("DetailsFont",&mDetailsFont,KGlobalSettings::generalFont());


#endif //KAB_EMBEDDED

  KPrefs::setCurrentGroup( "MainWindow" );
  bool m_visible = false;
#ifdef DESKTOP_VERSION
  m_visible = true;
#endif
  addItemBool( "FullMenuBarVisible", &mFullMenuBarVisible, m_visible );
  addItemBool( "JumpButtonBarVisible", &mJumpButtonBarVisible, false );
  addItemBool( "DetailsPageVisible", &mDetailsPageVisible, true );
  addItemIntList( "ExtensionsSplitter", &mExtensionsSplitter );
  addItemIntList( "DetailsSplitter", &mDetailsSplitter );
  addItemBool( "MultipleViewsAtOnce", &mMultipleViewsAtOnce, true );


  KPrefs::setCurrentGroup( "Extensions_General" );
  QStringList defaultExtensions;
  defaultExtensions << "merge";
  defaultExtensions << "distribution_list_editor";
  addItemInt( "CurrentExtension", &mCurrentExtension, 0 );
  addItemStringList( "ActiveExtensions", &mActiveExtensions, defaultExtensions );

  KPrefs::setCurrentGroup( "Views" );
  QString defaultView = i18n( "Default Table View" );
  addItemString( "CurrentView", &mCurrentView, defaultView );
  addItemStringList( "ViewNames", &mViewNames, defaultView );

  KPrefs::setCurrentGroup( "Filters" );
  addItemInt( "CurrentFilter", &mCurrentFilter, 0 );

}

KABPrefs::~KABPrefs()
{
    //qDebug("KABPrefs::~KABPrefs() ");
 if (sInstance == this)
     sInstance = staticDeleterAB.setObject(0);
}
 
KABPrefs *KABPrefs::instance()
{
  if ( !sInstance ) {
#ifdef KAB_EMBEDDED
    sInstance = staticDeleterAB.setObject( new KABPrefs() );
#else  //KAB_EMBEDDED
    //US the following line has changed ???. Why
    staticDeleterAB.setObject( sInstance, new KABPrefs() );
#endif //KAB_EMBEDDED
    sInstance->readConfig();
  }

  return sInstance;
}

void KABPrefs::setCategoryDefaults()
{
  mCustomCategories.clear();

  mCustomCategories << i18n( "Business" ) << i18n( "Family" ) << i18n( "School" )
                    << i18n( "Customer" ) << i18n( "Friend" );
}

    // US introduce a nonconst way to return the config object.
KConfig* KABPrefs::getConfig()
{
    return config();
}
void KABPrefs::usrReadConfig()
{
  KPimPrefs::usrReadConfig();
}


/*US
void KABPrefs::usrSetDefaults()
{
  KPimPrefs::usrSetDefaults();
}

void KABPrefs::usrReadConfig()
{
  KPimPrefs::usrReadConfig();
}

void KABPrefs::usrWriteConfig()
{
  KPimPrefs::usrWriteConfig();
}
*/
