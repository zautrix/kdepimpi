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

    $Id: pwmprefs.h,v 1.6 2005/03/03 10:32:35 zautrix Exp $
*/

#ifndef PWMPREFS_H
#define PWMPREFS_H

#include <qstringlist.h>
#include <qsize.h>

#include <kpimprefs.h>

class KConfig;

#define conf() PWMPrefs::instance()




#define CONF_DEFAULT_PWTIMEOUT         10/* 10 sec */
#define CONF_DEFAULT_LOCKTIMEOUT       0/* 0 == disable */
#define CONF_DEFAULT_TRAY              false
#define CONF_DEFAULT_UNLOCKONOPEN      true
#define CONF_DEFAULT_MAINVIEWSTYLE     1/* Category List Left */
#define CONF_DEFAULT_COMPRESSION       0x01/* gzip */
#define CONF_DEFAULT_CRYPTALGO         (0x01 - 1)/* blowfish */
#define CONF_DEFAULT_HASHALGO          (0x01 - 1)/* sha1 */
#define CONF_DEFAULT_AUTOMINIMIZE      false
#define CONF_DEFAULT_BROWSERCOMMAND    ""
#define CONF_DEFAULT_XTERMCOMMAND      "konsole -e"
#define CONF_DEFAULT_FILEPERMISSIONS   0600
#define CONF_DEFAULT_MAKEFILEBACKUP    false
#define CONF_DEFAULT_AUTOSTART_DEEPL   true
#define CONF_DEFAULT_AUTODEEPLOCK      true
#define CONF_DEFAULT_KWALLETEMU        false
#define CONF_DEFAULT_MINIMIZELOCK      2/* deep-lock */
#define CONF_DEFAULT_NEWENTRLOCKSTAT   false/* new entries unlocked */
#define CONF_DEFAULT_WNDCLOSE          true/* don't minimize to tray */


class PWMPrefs : public KPimPrefs
{
  public:
    virtual ~PWMPrefs();

    static PWMPrefs *instance();

public:
	/* functions for reading the configuration settings */
	/* GLOBAL */
	QString confGlobAutoStart();
	QString confGlobBrowserCommand();
	QString confGlobXtermCommand();
	QFont confGlobEntryFont();
	QFont confGlobViewFont();
	int confGlobPwTimeout();
	int confGlobLockTimeout();
	int confGlobCompression();
	int confGlobCryptAlgo();
	int confGlobHashAlgo();
	int confGlobFilePermissions();
	int confGlobMinimizeLock();
	bool confGlobUnlockOnOpen();
	bool confGlobTray();
	bool confGlobMakeFileBackup();
	bool confGlobAutostartDeepLocked();
	bool confGlobAutoDeepLock();
	bool confGlobKwalletEmu();
	bool confGlobNewEntrLockStat();
	/* WND */
	QSize confWndMainWndSize();
	int confWndMainViewStyle();
	bool confWndAutoMinimizeOnStart();
	bool confWndClose();

public:
	/* functions for writing the configuration settings */
	/* GLOBAL */
	void confGlobAutoStart(const QString &e);
	void confGlobBrowserCommand(const QString &e);
	void confGlobXtermCommand(const QString &e);
	void confGlobEntryFont(const QFont &e);
	void confGlobViewFont(const QFont &e);
	void confGlobPwTimeout(int e);
	void confGlobLockTimeout(int e);
	void confGlobCompression(int e);
	void confGlobCryptAlgo(int e);
	void confGlobHashAlgo(int e);

	void confGlobFilePermissions(int e);
	void confGlobMinimizeLock(int e);
	void confGlobUnlockOnOpen(bool e);
	void confGlobTray(bool e);
	void confGlobMakeFileBackup(bool e);
	void confGlobAutostartDeepLocked(bool e);
	void confGlobAutoDeepLock(bool e);
	void confGlobKwalletEmu(bool e);
	void confGlobNewEntrLockStat(bool e);
	/* WND */
	void confWndMainWndSize(const QSize &e);
	void confWndMainViewStyle(int e);
	void confWndAutoMinimizeOnStart(bool e);
	void confWndClose(bool e);



  QString mAutoStart;
  QString mBrowserCommand;
  QString mXTermCommand;
  QFont mEntryFont;
  QFont mViewFont;
  int mPwTimeout;
  int mLockTimeout;
  int mCompression;
  int mCryptAlgo;
  int mHashAlgo;
  int mFilePermissions;
  int mMinimizeLock;
  bool mUnlockOnOpen;
  bool mTray;
  bool mMakeFileBackup;
  bool mAutostartDeeplocked;
  bool mAutoDeeplock;
  bool mKWalletEmu;
  bool mNewEntrLockStat;
  QSize mMainWndSize;
  int mMainViewStyle;
  bool mAutoMinimizeOnStart;
  bool mClose;

  //US ENH
  QValueList<int> mCommentSplitter;
  QValueList<int> mCategorySplitter;


    // US introduce a nonconst way to return the config object.
    KConfig* getConfig();

  private:
    PWMPrefs();

    static PWMPrefs *sInstance;
};

#endif
