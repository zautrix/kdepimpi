/*
    This file is part of libkdepim.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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

$Id: kpimprefs.cpp,v 1.9 2005/04/21 17:43:51 zautrix Exp $
*/

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>

#include "kpimprefs.h"

KPimPrefs::KPimPrefs( const QString &name ) :
  KPrefs( name )
{
#ifdef _WIN32_
  QString hdp= locateLocal("data","korganizer")+"\\\\";
#else
  QString hdp= locateLocal("data","korganizer")+"/";
#endif
  setCurrentGroup("SyncOptions");
  addItemString("PassiveSyncPort",&mPassiveSyncPort,"9197" ); 
  addItemString("PassiveSyncPw",&mPassiveSyncPw,"abc" ); 
  addItemString("LastSyncedLocalFile", &mLastSyncedLocalFile ,hdp +"lastsync.ics" );
  addItemBool("PassiveSyncWithDesktop",&mPassiveSyncWithDesktop,false ); 
  addItemBool("PassiveSyncAutoStart",&mPassiveSyncAutoStart,false );
  addItemInt("RingSyncAlgoPrefs",&mRingSyncAlgoPrefs,3);

  setCurrentGroup("BackupDate");
  addItemInt("LastBackupDate",&mLastBackupDate,0);
}

KPimPrefs::~KPimPrefs()
{
}

void KPimPrefs::usrSetDefaults()
{
  setCategoryDefaults();
}

void KPimPrefs::usrReadConfig()
{
   config()->setGroup("General");
  mCustomCategories = config()->readListEntry("Custom Categories");
  if (mCustomCategories.isEmpty()) setCategoryDefaults();
}


void KPimPrefs::usrWriteConfig()
{
  config()->setGroup("General");
  config()->writeEntry("Custom Categories",mCustomCategories);
}

void KPimPrefs::setCategoryDefaults()
{
  // empty implementation
}

