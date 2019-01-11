/***************************************************************************
 *                                                                         *
 *   copyright (C) 2004 by Michael Buesch                                  *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * copyright (C) 2004 by Ulf Schenk
 * This file is originaly based on version 1.0.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: pwminit.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef __PWMINIT_H
#define __PWMINIT_H

#include <qvaluelist.h>
#include <qobject.h>
#ifndef PWM_EMBEDDED 
#include <kuniqueapplication.h>
#else //PWM_EMBEDDED

#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#else //DESKTOP_VERSION
#include <qapplication.h>
#include <qplatinumstyle.h>
#endif //DESKTOP_VERSION

#include <kapplication.h>
#endif //PWM_EMBEDDED

#include "globalstuff.h"

class DCOPClient;
class KWalletEmu;
class PwMKeyCard;
class KApplication;
class QWidget;
class PwM;
class PwMTray;
class PwMDoc;
class PwMInit;

/** main application class */
#ifndef PWM_EMBEDDED 
//MOC_SKIP_BEGIN
class PwMApplication : public KUniqueApplication
{
public:
	PwMApplication(bool allowStyles=true,
		       bool GUIenabled=true,
		       bool configUnique=false)
	 : KUniqueApplication(allowStyles, GUIenabled, configUnique)
	 , init (0)
		{ }
	~PwMApplication()
		{ delete_ifnot_null(init); }

	int newInstance();

protected:
	PwMInit *init;
};
//MOC_SKIP_END
#else //PWM_EMBEDDED

#ifndef DESKTOP_VERSION
class PwMApplication : public QPEApplication
{
public:
  PwMApplication(int & argc, char ** argv);
  ~PwMApplication();

  int newInstance();

protected:
	PwMInit *init;
};
#else //DESKTOP_VERSION

class PwMApplication : public QApplication
{
public:
  PwMApplication(int & argc, char ** argv);
  ~PwMApplication();
  
  int newInstance();

protected:
	PwMInit *init;
};

#endif
#endif

/** PwManager initialisation class.
  * This class sets up a new instance of PwManager and finally
  * starts the user interface.
  */
class PwMInit : public QObject
{
	Q_OBJECT
	friend class PwMApplication;

protected:
	enum RunStatus
	{
		unknown,
		init,
		running,
		shutdown,
	};

	/* saved command line options. */
	class savedCmd_t
	{
	public:
		bool open_deeplocked;
		bool minimized;
		bool minToTray;
		bool skipSelfTest;
		// initial open paths are not saved here.
	};

public:
	PwMInit(PwMApplication *_app);
	~PwMInit();

	/** Initialize the application. */
	void initializeApp();
	/** shutdown PwManager */
	void shutdownApp(int exitStatus);

	/** returns a pointer to the current widget */
	QWidget * curWidget()
			{ return _curWidget; }
	/** returns a pointer to the dcop client */
	DCOPClient * dcopClient()
			{ return _dcopClient; }
	/** returns a pointer to the KWallet emulation */
	KWalletEmu * kwalletEmu()
			{ return _kwalletEmu; }
	/** returns a pointer to the keycard interface */
	PwMKeyCard * keycard()
			{ return _keycard; }
	/** returns a pointer to the tray icon */
	PwMTray * tray()
			{ return _tray; }
	/** returns a list of all open main windows */
	const QValueList<PwM *> * mainWndList()
			{ return &_mainWndList; }
	/** create a new document */
	PwMDoc * createDoc();
	/** create a new PwM main window */
	PwM * createMainWnd(const QString &loadFile = QString::null,
			    bool loadFileDeepLocked = false,
			    bool virginity = true,
			    PwMDoc *doc = 0,
			    bool minimized = false);

	/** (re)initialize the KWallet emulation */
	void initKWalletEmu(bool forceDisable = false, bool forceReload = false);
	/** (re)initialize the tray icon */
	void initTray();

protected:
	/** initialize the DCOP connection */
	void initDCOP();
	/** initialize the keycard interface */
	void initKeycard();
	/** initialize posix signal handling */
	void initPosixSignalHandler();
	/** handle the commandline args */
	void handleCmdLineArgs(bool initial = true);
	/** minimize all open mainWnds to tray (or if
	  * no tray is present, minimize them the normal way)
	  */
	void minimizeAllMainWnd(bool toTray);

protected slots:
	/** a main window got closed */
	void mainWndClosed(PwM *wnd);
	/** the tray icon got closed */
	void trayIconClosed(PwMTray *tray);
	/** sets _curWidget on behalf of a PwM signal */
	void setCurWidget(PwM *w)
			{ _curWidget = reinterpret_cast<QWidget *>(w); }
	/** resets _curWidget to nothing */
	void resetCurWidget()
			{ _curWidget = 0; }
	/** remove the tray and quit the app */
	void removeTrayAndQuit();

protected:
	/** run-status of the app */
	RunStatus runStatus;
	/** PwMApplication pointer */
	PwMApplication *app;
	/** pointer to the current widget the user is dealing with */
	QWidget *_curWidget;
	/** pointer to the dcop client */
	DCOPClient *_dcopClient;
	/** pointer to the KWallet emulation */
	KWalletEmu *_kwalletEmu;
	/** pointer to the keycard interface */
	PwMKeyCard *_keycard;
	/** pointer to the tray icon */
	PwMTray *_tray;
	/** list of all open main windows */
	QValueList<PwM *> _mainWndList;
	/** saved command line options. */
	savedCmd_t savedCmd;
};

#endif // __PWMINIT_H
