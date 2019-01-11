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
 * $Id: pwminit.cpp,v 1.11 2005/02/18 09:36:43 zautrix Exp $
 **************************************************************************/  

#include "pwminit.h"
#include "randomizer.h"
#include <qdir.h>

#ifndef PWM_EMBEDDED
#include "selftest.h"
#include "configuration.h"
#else
#include "pwmprefs.h"
#endif

#include "pwm.h"
#include "pwmexception.h"
#include "pwmtray.h"
#include "pwmdoc.h"

#ifdef CONFIG_KWALLETIF
# include "kwalletemu.h"
#endif // CONFIG_KWALLETIF
#ifdef CONFIG_KEYCARD
# include "pwmkeycard.h"
#endif // CONFIG_KEYCARD

#include <qmessagebox.h>

#include <kmessagebox.h>
#ifndef PWM_EMBEDDED
#include <kcmdlineargs.h>
#include <kwin.h>
#include <dcopclient.h>
#endif

#include <kapplication.h>
#include <kiconloader.h>

#include <signal.h>

static PwMInit *sig_init_pointer;
#ifdef _WIN32_
static void sig_handler(int signum)
#else
static NOREGPARM void sig_handler(int signum)
#endif
{
	switch (signum) {
		case SIGINT:
		case SIGTERM:
			sig_init_pointer->shutdownApp(20 + signum);
			break;
		default:
			printDebug(string("unhandled signal ")
				   + tostr(signum));
	}
}



PwMInit::PwMInit(PwMApplication *_app)
 : runStatus (unknown)
 , _curWidget (0)
 , _dcopClient (0)
 , _kwalletEmu (0)
 , _keycard (0)
 , _tray (0)
{
	sig_init_pointer = this;
	app = _app;
}

PwMInit::~PwMInit()
{
    //qDebug("PwMInit::~PwMInit() ");
#ifndef PWM_EMBEDDED
	SelfTest::cancel();
	// close all open mainwnds
	QValueList<PwM *>::iterator i = _mainWndList.begin(),
				    end = _mainWndList.end();

#else
	// close all open mainwnds
	QValueList<PwM *>::Iterator i = _mainWndList.begin(),
				    end = _mainWndList.end();
#endif
	while (i != end) {
		disconnect(*i, SIGNAL(closed(PwM *)),
			   this, SLOT(mainWndClosed(PwM *)));
		delete *i;
		++i;
	}
	_mainWndList.clear();
	// close all remaining open documents
	PwMDocList *_dl = PwMDoc::getOpenDocList();
	vector<PwMDocList::listItem> dl = *(_dl->getList());
	vector<PwMDocList::listItem>::iterator i2 = dl.begin(),
					       end2 = dl.end();
	while (i2 != end2) {
		delete (*i2).doc;
		++i2;
	}

#ifdef CONFIG_KWALLETIF
	delete_ifnot_null(_kwalletEmu);
#endif // CONFIG_KWALLETIF
#ifdef CONFIG_KEYCARD
	delete_ifnot_null(_keycard);
#endif // CONFIG_KEYCARD
	delete_ifnot_null(_tray);

	Randomizer::cleanup();
#ifndef PWM_EMBEDDED
	Configuration::cleanup();
#endif
}

void PwMInit::initializeApp()
{
    //qDebug("PwMInit::initializeApp() ");
	PWM_ASSERT(runStatus == unknown);
	runStatus = init;
	initPosixSignalHandler();
	Randomizer::init();
#ifndef PWM_EMBEDDED
	Configuration::init();
#endif
	initDCOP();
	initKWalletEmu();
	initKeycard();
	initTray();
	handleCmdLineArgs();
	bool openDeeplocked = false;
	if (conf()->confGlobAutostartDeepLocked() ||
	    savedCmd.open_deeplocked)
		openDeeplocked = true;
    if ( false ){
    // LR is not working
	//if (conf()->confWndAutoMinimizeOnStart() ||
    //   savedCmd.minToTray) {
		PwMDoc *newDoc = createDoc();
        //qDebug("  createDoc()");
		if (!newDoc->openDocUi(newDoc,
				       conf()->confGlobAutoStart(),
				       openDeeplocked)) {
			delete newDoc;
        }

			//US ENH for embedded devices: in the case of failure, open a document the default way  
			createMainWnd(conf()->confGlobAutoStart(),
			      openDeeplocked,
			      true,
			      0,
			      savedCmd.minimized);
			



            //	}
	} else {
		createMainWnd(conf()->confGlobAutoStart(),
			      openDeeplocked,
			      true,
			      0,
			      savedCmd.minimized);
	}
	runStatus = running;
}

void PwMInit::shutdownApp(int exitStatus)
{


	PwM * iii;;
	while ( _mainWndList.count() ) {
        iii = _mainWndList.first();
        disconnect(iii, SIGNAL(closed(PwM *)),
                   this, SLOT(mainWndClosed(PwM *)));
        mainWndClosed((iii));
    }

	printDebug(string("PwMInit::shutdownApp(")
		   + tostr(exitStatus) + ") called.");
	PWM_ASSERT((runStatus == running) || (runStatus == init));
	runStatus = shutdown;
	QApplication::exit(exitStatus);
	/* The destructor of PwMInit is called when control
	 * leaves main()
	 */
}

void PwMInit::initPosixSignalHandler()
{
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
}

void PwMInit::initDCOP()
{
#ifndef PWM_EMBEDDED
	_dcopClient = app->dcopClient();
	_dcopClient->setNotifications(true);
#endif

}

void PwMInit::initKWalletEmu(bool forceDisable, bool forceReload)
{
#ifdef CONFIG_KWALLETIF
	if (!conf()->confGlobKwalletEmu() ||
	    forceDisable) {
		delete_ifnot_null(_kwalletEmu);
		return;
	}
	try {
		if (_kwalletEmu && forceReload)
			delete_and_null(_kwalletEmu);
		if (!_kwalletEmu)
			_kwalletEmu = new KWalletEmu(this);
	} catch (PwMException e) {
		string errMsg("initializing KWallet emulation failed.  ID: ");
		errMsg += tostr(static_cast<int>(e.getId()));
		errMsg += "  err-message: ";
		errMsg += e.getMessage();
		printWarn(errMsg);
		return;
	}
#else // CONFIG_KWALLETIF
	PARAM_UNUSED(forceDisable);
	PARAM_UNUSED(forceReload);
#endif // CONFIG_KWALLETIF
}

void PwMInit::initKeycard()
{
#ifdef CONFIG_KEYCARD
	PWM_ASSERT(!_keycard);
	_keycard = new PwMKeyCard(this);
#endif // CONFIG_KEYCARD
}

void PwMInit::initTray()
{
#ifdef PWM_EMBEDDED
  //US ENH : embedded version does not support a tray
  return;
#endif

	if (!conf()->confGlobTray()) {
		if (!_tray)
			return;
		_tray->hide();
		delete_and_null(_tray);
		return;
	}
	if (_tray)
		return;
	_tray = new PwMTray(this);
	connect(_tray, SIGNAL(quitSelected()),
		this, SLOT(removeTrayAndQuit()));
	connect(_tray, SIGNAL(closed(PwMTray *)),
		this, SLOT(trayIconClosed(PwMTray *)));
	KIconLoader icons;
#ifndef PWM_EMBEDDED
	_tray->setPixmap(icons.loadIcon(PACKAGE_NAME, KIcon::Small));
#endif
	_tray->show();
	// connect the signals of all open documents.
	const vector<PwMDocList::listItem> *dl = PwMDoc::getOpenDocList()->getList();
	vector<PwMDocList::listItem>::const_iterator i = dl->begin(),
						     end = dl->end();
	while (i != end) {
		_tray->connectDocToTray((*i).doc);
		++i;
	}
}

void PwMInit::removeTrayAndQuit()
{
	PWM_ASSERT(_tray);
	// _tray is deleted in ~PwMInit
	shutdownApp(0);
}

PwM * PwMInit::createMainWnd(const QString &loadFile,
			     bool loadFileDeepLocked,
			     bool virginity,
			     PwMDoc *doc,
			     bool minimized)
{
	PwM *newWnd;
	if (!doc)
		doc = createDoc();
	newWnd = new PwM(this, doc, virginity);
#ifndef PWM_EMBEDDED
	_mainWndList.push_back(newWnd);
#else
	_mainWndList.append(newWnd);
#endif
	connect(newWnd, SIGNAL(closed(PwM *)),
		this, SLOT(mainWndClosed(PwM *)));
	connect(newWnd, SIGNAL(gotFocus(PwM *)),
		this, SLOT(setCurWidget(PwM *)));
	connect(newWnd, SIGNAL(lostFocus(PwM *)),
		this, SLOT(resetCurWidget()));

	//US ENH
#ifndef PWM_EMBEDDED
	if (minimized)
		newWnd->showMinimized();
	else
		newWnd->show();

#else //PWM_EMBEDDED

#ifndef DESKTOP_VERSION
       app->showMainWidget( newWnd );
#else //DESKTOP_VERSION
       newWnd->resize (640, 480 );
       newWnd->show();
#endif //DESKTOP_VERSION

#endif //PWM_EMBEDDED

	if (loadFile != QString::null &&
	    loadFile != "") {
		newWnd->openDoc(loadFile, loadFileDeepLocked);
	}
	return newWnd;
}

PwMDoc * PwMInit::createDoc()
{
	PwMDoc *doc = new PwMDoc(this);
#ifdef CONFIG_KEYCARD
	doc->setPwMKeyCard(keycard());
#endif
#ifdef CONFIG_KWALLETIF
	if (kwalletEmu())
		kwalletEmu()->connectDocSignals(doc);
#endif

	if (_tray)
		_tray->connectDocToTray(doc);

	return doc;

}

void PwMInit::mainWndClosed(PwM *wnd)
{
	bool doMinimizeToTray = false;
	bool doDeleteDoc = false;
#ifndef PWM_EMBEDDED
	dcopClient()->suspend();
	dcopClient()->setAcceptCalls(false);
#endif
again:

	if (wnd->isForceMinimizeToTray()) {
		if (unlikely(!_tray)) {
			/* This should not happen! If we set forceMinimizeToTray ,
			 * we must be sure that _tray exists.
			 */
			BUG();
			wnd->setForceMinimizeToTray(false);
			goto again;
		}
		doMinimizeToTray = true;
	} else {
	  // Ask to minimize to tray. If not, delete doc.
		if (_tray &&
		    runStatus != shutdown &&
		    !wnd->isForceQuit() &&
		    !wnd->curDoc()->isDeleted()) {
		    	if (conf()->confWndClose())
				doDeleteDoc = true;
			else
				doMinimizeToTray = true;
		} else {
			doDeleteDoc = true;
		}
	}

	if (doMinimizeToTray) {
        //qDebug("doMinimizeToTray ");
		PWM_ASSERT(_tray);
		int mmlock = conf()->confGlobMinimizeLock();
		switch (mmlock) {
		case 0:		// don't lock anything
			break;
		case 1:		// normal lock
			wnd->curDoc()->lockAll(true);
			break;
		case 2:		// deep-lock
			wnd->curDoc()->deepLock();
			break;
		default:
			WARN();
		}
	} else if (doDeleteDoc) {
        //qDebug("doDeleteDoc ");
		if (!wnd->curDoc()->tryDelete()) {
			/* We failed deleting the doc,
			 * so open a new window with it, again.
			 */
			createMainWnd(QString::null, false,
				      false, wnd->curDoc());
		}
	}
#ifndef PWM_EMBEDDED
	// find the closed window in the "mainWndList" and delete it.
	QValueList<PwM *>::iterator i = _mainWndList.begin(),
				    end = _mainWndList.end();
#else
	// find the closed window in the "mainWndList" and delete it.
	QValueList<PwM *>::Iterator i = _mainWndList.begin(),
				    end = _mainWndList.end();
#endif
	while (i != end) {
        //qDebug("iii %x  end %x  wnd %x",  (*i), (*end), wnd);
		if (*i == wnd) {
#ifndef PWM_EMBEDDED
			_mainWndList.erase(i);
#else
			_mainWndList.remove(i);
#endif
         
            if ( app->mainWidget() == wnd ) {
                if (  _mainWndList.count() ) {
#ifndef DESKTOP_VERSION
                    app->showMainWidget(_mainWndList.first() );
#endif //DESKTOP_VERSION
                   
                }
            }
            delete wnd;
			goto out_success;
		}
		++i;
	}
	BUG();
out_success:
#ifndef PWM_EMBEDDED
	if (!_mainWndList.size()) 
#else
	if (!_mainWndList.count()) 
#endif

	  {
		/* If there's no main window and no tray icon
		 * left, we have no user interface, so we can
		 * shut down the application.
		 */
		if (!_tray) {
#ifndef PWM_EMBEDDED
			dcopClient()->setAcceptCalls(true);
			dcopClient()->resume();
#endif
			shutdownApp(0);
			return;
		}
		/* There is no widget left, so set
		 * _curWidget to 0
		 */
		resetCurWidget();
	}
#ifndef PWM_EMBEDDED
	dcopClient()->setAcceptCalls(true);
	dcopClient()->resume();
#endif
}

void PwMInit::trayIconClosed(PwMTray *tray)
{
	if (runStatus != running)
		return;
	PARAM_UNUSED(tray);
	PWM_ASSERT(tray == _tray);
	/* If there's no main wnd left we have to
	 * shutdown the app (same as in mainWndClosed())
	 */
#ifndef PWM_EMBEDDED
	if (!_mainWndList.size())
		shutdownApp(0);
#else
	if (!_mainWndList.count())
		shutdownApp(0);
#endif
}

void PwMInit::handleCmdLineArgs(bool initial)
{
#ifndef PWM_EMBEDDED
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	PWM_ASSERT(args);
	int i, numArgs = args->count();
	const char *curArg;

	// read all cmdline options
	savedCmd.open_deeplocked = args->isSet("open-deeplocked");
	savedCmd.minimized = args->isSet("minimized");
	savedCmd.minToTray = args->isSet("mintray");
	savedCmd.skipSelfTest = args->isSet("skip-self-test");
	if (savedCmd.minimized &&
	    savedCmd.minToTray) {
		printInfo(i18n("Commandline option \"--minimized\" and "
			       "\"--mintray\" selected. These are incompatible. "
			       "\"--mintray\" will be selected.").latin1());
	}
	/* Iterate through all non-option arguments.
	 * Every non-option arg is a filename to open.
	 */
	for (i = 0; i < numArgs; ++i) {
		curArg = args->arg(i);
		PWM_ASSERT(curArg);
		if (savedCmd.minToTray) {
			PwMDoc *newDoc = createDoc();
			if (!newDoc->openDocUi(newDoc,
					       curArg,
					       savedCmd.open_deeplocked)) {
				delete newDoc;
			}
		} else {
			PwM *newInstance = createMainWnd(QString::null,
							 false,
							 true,
							 0,
							 savedCmd.minimized);
			PwMDoc *newDoc = newInstance->openDoc(curArg,
							      savedCmd.open_deeplocked);
			if (!newDoc) {
				newInstance->setForceQuit(true);
				delete_and_null(newInstance);
			}
		}
	}

	if (savedCmd.minToTray) {
		minimizeAllMainWnd(true);
	} else if (savedCmd.minimized) {
		minimizeAllMainWnd(false);
	}
	if (!savedCmd.skipSelfTest && initial) {
		SelfTest::schedule();
	}
	args->clear();
#endif
}

void PwMInit::minimizeAllMainWnd(bool toTray)
{
#ifndef PWM_EMBEDDED
	if (!_mainWndList.size())
		return;
#else
	if (!_mainWndList.count())
		return;
#endif
	const QValueList<PwM *> *ml = mainWndList();
#ifndef PWM_EMBEDDED
	QValueList<PwM *>::const_iterator it = ml->begin(),
					  end = ml->end();
#else
	QValueList<PwM *>::ConstIterator it = ml->begin(),
					  end = ml->end();
#endif
	PwM *wnd;
	if (toTray && _tray) {
		/* minimize to tray.
		 * close all mainWnd.
		 */
		while (it != end) {
			wnd = *it;
			wnd->setForceMinimizeToTray(true);
			wnd->close_slot();
			++it;
		}
	} else {
		// normal minimize
		while (it != end) {
			wnd = *it;
			wnd->hide();
			wnd->showMinimized();
			++it;
		}
	}
}

#ifdef PWM_EMBEDDED

#ifndef DESKTOP_VERSION

PwMApplication::PwMApplication(int & argc, char ** argv) 
    : QPEApplication( argc, argv )
    , init (0)
{
     this->setKeepRunning ();
}
	
PwMApplication::~PwMApplication()
{
  delete_ifnot_null(init);
}
#else //DESKTOP_VERSION

PwMApplication::PwMApplication(int & argc, char ** argv)
  	 : QApplication( argc, argv )
	  , init (0)
{ 
  setStyle( new QPlatinumStyle ());
  QString hdir = QDir::homeDirPath();
  // there is a bug when creating dirs for WIN 98
  // it is difficult to fix, because we have no WIN 98 runnung
  // such that we try it to create the dirs at startup here
  if ( hdir == "C:\\" )
    {
      // win 98 or ME
    QDir app_dir;
    if ( !app_dir.exists("C:\\kdepim") )
      app_dir.mkdir ("C:\\kdepim");
    if ( !app_dir.exists("C:\\kdepim\\apps") )
      app_dir.mkdir ("C:\\kdepim\\apps");
    if ( !app_dir.exists("C:\\kdepim\\config") )
      app_dir.mkdir ("C:\\kdepim\\config");
    if ( !app_dir.exists("C:\\kdepim\\apps\\pwmanager") )
      app_dir.mkdir ("C:\\kdepim\\apps\\pwmanager");
    }
}

PwMApplication::~PwMApplication()
{
  delete_ifnot_null(init);
}

#endif //DESKTOP_VERSION

#endif //PWM_EMBEDDED




#ifndef PWM_EMBEDDED
#include "pwminit.moc"
#endif
