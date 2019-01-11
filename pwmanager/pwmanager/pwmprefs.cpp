/*
    This file is part of PwManager/Pi
    Copyright (c) 2004 Ulf Schenk

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

    $Id: pwmprefs.cpp,v 1.7 2005/03/03 10:32:35 zautrix Exp $
*/


#include <kconfig.h>
#include <klocale.h>
#include <kstaticdeleter.h>
#include <kglobalsettings.h>

#include "pwmprefs.h"

PWMPrefs *PWMPrefs::sInstance = 0;
static KStaticDeleter<PWMPrefs> staticDeleterPP;

PWMPrefs::PWMPrefs()
  : KPimPrefs("pwmanagerrc")
{
  KPrefs::setCurrentGroup( "Global" );

  addItemString( "autoStart", &mAutoStart, "" );
  addItemString( "browserCommand", &mBrowserCommand, "" );
  addItemString( "xtermCommand", &mXTermCommand, CONF_DEFAULT_XTERMCOMMAND);
  addItemFont( "entryFont", &mEntryFont,KGlobalSettings::generalFont());
  addItemFont( "viewFont", &mViewFont,KGlobalSettings::generalFont());
  addItemInt( "pwTimeout", &mPwTimeout, CONF_DEFAULT_PWTIMEOUT );
  addItemInt( "lockTimeout", &mLockTimeout, CONF_DEFAULT_LOCKTIMEOUT );
  addItemInt( "compression", &mCompression, CONF_DEFAULT_COMPRESSION );
  addItemInt("cryptAlgo", &mCryptAlgo, CONF_DEFAULT_CRYPTALGO);
  addItemInt("hashAlgo", &mHashAlgo, CONF_DEFAULT_HASHALGO);
  addItemInt( "filePermissions", &mFilePermissions, CONF_DEFAULT_FILEPERMISSIONS );
  addItemInt( "minimizeLock", &mMinimizeLock, CONF_DEFAULT_MINIMIZELOCK );
  addItemBool( "unlockOnOpen", &mUnlockOnOpen, CONF_DEFAULT_UNLOCKONOPEN );
  addItemBool( "tray", &mTray, CONF_DEFAULT_TRAY );
  addItemBool( "makeFileBackup", &mMakeFileBackup, CONF_DEFAULT_MAKEFILEBACKUP );
  addItemBool( "autostartDeepLocked", &mAutostartDeeplocked, CONF_DEFAULT_AUTOSTART_DEEPL );
  addItemBool( "autoDeepLock", &mAutoDeeplock, CONF_DEFAULT_AUTODEEPLOCK );
  addItemBool( "kwalletEmu", &mKWalletEmu, CONF_DEFAULT_KWALLETEMU );
  addItemBool( "newEntrLockStat", &mNewEntrLockStat, CONF_DEFAULT_NEWENTRLOCKSTAT );

  KPrefs::setCurrentGroup( "Wnd" );

  addItemSize( "MainWndSize", &mMainWndSize);
  addItemInt( "MainViewStyle", &mMainViewStyle, CONF_DEFAULT_MAINVIEWSTYLE );
  addItemBool( "autoMinimizeOnStart", &mAutoMinimizeOnStart, CONF_DEFAULT_AUTOMINIMIZE );
  addItemBool( "close", &mClose, CONF_DEFAULT_WNDCLOSE );
  addItemIntList( "commentSplitter", &mCommentSplitter );
  addItemIntList( "categorySplitter", &mCategorySplitter );
}

PWMPrefs::~PWMPrefs()
{  
    if (sInstance == this)
        sInstance = staticDeleterPP.setObject(0); 
    else
        qDebug("Whats this? Error in PWMPrefs::~PWMPrefs()?");
}

PWMPrefs *PWMPrefs::instance()
{
  if ( !sInstance ) {
#ifdef PWM_EMBEDDED
    sInstance = staticDeleterPP.setObject( new PWMPrefs() );
#else  //PWM_EMBEDDED
    //US the following line has changed ???. Why
    staticDeleterPP.setObject( sInstance, new PWMPrefs() );
#endif //KAB_EMBEDDED
    sInstance->readConfig();
  }

  return sInstance;
}

    // US introduce a nonconst way to return the config object.
KConfig* PWMPrefs::getConfig()
{
    return config();
}

/*******************************************************************
 *  functions for reading the configuration settings
 *******************************************************************/

QString PWMPrefs::confGlobAutoStart()
{
  return mAutoStart;
}

QString PWMPrefs::confGlobBrowserCommand()
{
  return mBrowserCommand;
}

QString PWMPrefs::confGlobXtermCommand()
{
  return mXTermCommand;
}

QFont PWMPrefs::confGlobEntryFont()
{
  return mEntryFont;
}
QFont PWMPrefs::confGlobViewFont()
{
  return mViewFont;
}

int PWMPrefs::confGlobPwTimeout()
{
  return mPwTimeout;
}

int PWMPrefs::confGlobLockTimeout()
{
  return mLockTimeout;
}

int PWMPrefs::confGlobCompression()
{
  return mCompression;
}

int PWMPrefs::confGlobFilePermissions()
{
  return mFilePermissions;
}

int PWMPrefs::confGlobMinimizeLock()
{
  return mMinimizeLock;
}

bool PWMPrefs::confGlobUnlockOnOpen()
{
  return mUnlockOnOpen;
}

bool PWMPrefs::confGlobTray()
{
  return mTray;
}

bool PWMPrefs::confGlobMakeFileBackup()
{
  return mMakeFileBackup;
}

bool PWMPrefs::confGlobAutostartDeepLocked()
{
  return mAutostartDeeplocked;
}

bool PWMPrefs::confGlobAutoDeepLock()
{
  return mAutoDeeplock;
}

bool PWMPrefs::confGlobKwalletEmu()
{
  return mKWalletEmu;
}

bool PWMPrefs::confGlobNewEntrLockStat()
{
  return mNewEntrLockStat;
}

QSize PWMPrefs::confWndMainWndSize()
{
  return mMainWndSize;
}

int PWMPrefs::confWndMainViewStyle()
{
  return mMainViewStyle;
}

bool PWMPrefs::confWndAutoMinimizeOnStart()
{
  return mAutoMinimizeOnStart;
}

bool PWMPrefs::confWndClose()
{
  return mClose;
}

int PWMPrefs::confGlobCryptAlgo()
{
  return mCryptAlgo + 1;
}

int PWMPrefs::confGlobHashAlgo()
{
  return mHashAlgo + 1;
}

/*******************************************************************
 *  functions for writing the configuration settings
 *******************************************************************/

void PWMPrefs::confGlobAutoStart(const QString &e)
{
  mAutoStart = e;
}

void PWMPrefs::confGlobBrowserCommand(const QString &e)
{
  mBrowserCommand = e;
}

void PWMPrefs::confGlobXtermCommand(const QString &e)
{
  mXTermCommand = e;
}

void PWMPrefs::confGlobEntryFont(const QFont &e)
{
  mEntryFont = e;
}
void PWMPrefs::confGlobViewFont(const QFont &e)
{
  mViewFont = e;
}

void PWMPrefs::confGlobPwTimeout(int e)
{
  mPwTimeout = e;
}

void PWMPrefs::confGlobLockTimeout(int e)
{
  mLockTimeout = e;
}

void PWMPrefs::confGlobCompression(int e)
{
  mCompression = e;
}

void PWMPrefs::confGlobFilePermissions(int e)
{
  mFilePermissions = e;
}

void PWMPrefs::confGlobMinimizeLock(int e)
{
  mMinimizeLock = e;
}

void PWMPrefs::confGlobUnlockOnOpen(bool e)
{
  mUnlockOnOpen = e;
}

void PWMPrefs::confGlobTray(bool e)
{
  mTray = e;
}

void PWMPrefs::confGlobMakeFileBackup(bool e)
{
  mMakeFileBackup = e;
}

void PWMPrefs::confGlobAutostartDeepLocked(bool e)
{
  mAutostartDeeplocked = e;
}

void PWMPrefs::confGlobAutoDeepLock(bool e)
{
  mAutoDeeplock = e;
}

void PWMPrefs::confGlobKwalletEmu(bool e)
{
  mKWalletEmu = e;
}

void PWMPrefs::confGlobNewEntrLockStat(bool e)
{
  mNewEntrLockStat = e;
}

void PWMPrefs::confWndMainWndSize(const QSize &e)
{
  mMainWndSize = e;
}

void PWMPrefs::confWndMainViewStyle(int e)
{
  mMainViewStyle = e;
}

void PWMPrefs::confWndAutoMinimizeOnStart(bool e)
{
  mAutoMinimizeOnStart = e;
}

void PWMPrefs::confWndClose(bool e)
{
  mClose = e;
}

void PWMPrefs::confGlobCryptAlgo(int e)
{
  mCryptAlgo = e - 1;
}

void PWMPrefs::confGlobHashAlgo(int e)
{
  mHashAlgo = e - 1;
}
