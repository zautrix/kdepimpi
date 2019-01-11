/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003, 2004 by Michael Buesch                            *
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
 * $Id: pwm.cpp,v 1.31 2005/11/28 05:53:55 zautrix Exp $
 **************************************************************************/  

#include <klocale.h>
#include <klistview.h>
#include <ktoolbar.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include <qstatusbar.h>

#ifndef PWM_EMBEDDED
#include <kmenubar.h>
#include <kstatusbar.h>
#include <dcopclient.h>
#include "configwndimpl.h"
#include "configuration.h"
#else
#include <qmenubar.h>
#include <qmessagebox.h>
#include <pwmprefs.h>
#include <kpimglobalprefs.h>
#include <kprefsdialog.h>
#include <kcmconfigs/kcmpwmconfig.h>
#include <kcmconfigs/kcmkdepimconfig.h>
#include <kcmultidialog.h>
#include "editcategory.h"
#endif


#ifndef DESKTOP_VERSION
#include <qpe/global.h>
#endif

#include <qpixmap.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qfileinfo.h>
#include <qclipboard.h>


#include <stdio.h>

#include "pwm.h"
#include "pwminit.h"
#include "pwmprint.h"
#include "addentrywndimpl.h"
#include "globalstuff.h"
#include "findwndimpl.h"
#include "csv.h"

#ifdef CONFIG_KWALLETIF
# include "kwalletif.h"
# include "kwalletemu.h"
#endif
#ifdef CONFIG_KEYCARD
# include "pwmkeycard.h"
#endif

#define DEFAULT_SIZE		(QSize(700, 400))

// Button IDs for "file" popup menu
enum {
	BUTTON_POPUP_FILE_NEW = 0,
	BUTTON_POPUP_FILE_OPEN,
	BUTTON_POPUP_FILE_CLOSE,
	BUTTON_POPUP_FILE_SAVE,
	BUTTON_POPUP_FILE_SAVEAS,
	BUTTON_POPUP_FILE_EXPORT,
	BUTTON_POPUP_FILE_IMPORT,
	BUTTON_POPUP_FILE_PRINT,
	BUTTON_POPUP_FILE_QUIT
};
// Button IDs for "manage" popup menu
enum {
	BUTTON_POPUP_MANAGE_ADD = 0,
	BUTTON_POPUP_MANAGE_EDIT,
	BUTTON_POPUP_MANAGE_DEL,
	BUTTON_POPUP_MANAGE_CHANGEMP
};
// Button IDs for chipcard popup menu
enum {
#ifdef CONFIG_KEYCARD
	BUTTON_POPUP_CHIPCARD_GENNEW = 0,
	BUTTON_POPUP_CHIPCARD_DEL,
	BUTTON_POPUP_CHIPCARD_READID,
	BUTTON_POPUP_CHIPCARD_SAVEBACKUP,
	BUTTON_POPUP_CHIPCARD_REPLAYBACKUP
#else // CONFIG_KEYCARD
	BUTTON_POPUP_CHIPCARD_NO = 0
#endif // CONFIG_KEYCARD
};
// Button IDs for "view" popup menu
enum {
	BUTTON_POPUP_VIEW_FIND = 0,
	BUTTON_POPUP_VIEW_LOCK,
	BUTTON_POPUP_VIEW_DEEPLOCK,
	BUTTON_POPUP_VIEW_UNLOCK
};
// Button IDs for "options" popup menu
enum {
	BUTTON_POPUP_OPTIONS_CONFIG = 0,
	BUTTON_POPUP_OPTIONS_GLOBAL_CONFIG
#ifdef PWM_EMBEDDED
	,BUTTON_POPUP_OPTIONS_CATEGORY
#endif
};
// Button IDs for "export" popup menu (in "file" popup menu)
enum {
	BUTTON_POPUP_EXPORT_TEXT = 0,
	BUTTON_POPUP_EXPORT_GPASMAN,
	BUTTON_POPUP_EXPORT_CSV
#ifdef CONFIG_KWALLETIF
	,BUTTON_POPUP_EXPORT_KWALLET
#endif
};
// Button IDs for "import" popup menu (in "file" popup menu)
enum {
	BUTTON_POPUP_IMPORT_TEXT = 0,
	BUTTON_POPUP_IMPORT_GPASMAN,
	BUTTON_POPUP_IMPORT_CSV
#ifdef CONFIG_KWALLETIF
	,BUTTON_POPUP_IMPORT_KWALLET
#endif
};

#ifdef PWM_EMBEDDED
// Button IDs for "help" popup menu
enum {
	BUTTON_POPUP_HELP_LICENSE = 0,
	BUTTON_POPUP_HELP_FAQ,
	BUTTON_POPUP_HELP_ABOUT,
	BUTTON_POPUP_HELP_SYNC,
	BUTTON_POPUP_HELP_WHATSNEW
};
#endif

// Button IDs for toolbar
enum {
	BUTTON_TOOL_NEW = 0,
	BUTTON_TOOL_OPEN,
	BUTTON_TOOL_SAVE,
	BUTTON_TOOL_SAVEAS,
	BUTTON_TOOL_PRINT,
	BUTTON_TOOL_ADD,
	BUTTON_TOOL_EDIT,
	BUTTON_TOOL_DEL,
	BUTTON_TOOL_FIND,
	BUTTON_TOOL_LOCK,
	BUTTON_TOOL_DEEPLOCK,
	BUTTON_TOOL_UNLOCK
};


PwM::PwM(PwMInit *_init, PwMDoc *doc,
	 bool virginity,
	 QWidget *parent, const char *name)
 : KMainWindow(parent, "HALLO")
 , forceQuit (false)
 , forceMinimizeToTray (false)
{
    syncManager = 0;
    virgin = !virginity;
	init = _init;
	connect(doc, SIGNAL(docClosed(PwMDoc *)),
		this, SLOT(docClosed(PwMDoc *)));
	initMenubar();
	initToolbar();
	initMetrics();
	setVirgin(virginity);
	setFocusPolicy(QWidget::WheelFocus);
#ifndef PWM_EMBEDDED
	statusBar()->show();
#endif
	view = makeNewListView(doc);
	setCentralWidget(view);
	updateCaption();
	showStatMsg(i18n("Ready."));
}

PwM::~PwM()
{
    //qDebug("PwM::~PwM() %x", this);
	disconnect(curDoc(), SIGNAL(docClosed(PwMDoc *)),
		   this, SLOT(docClosed(PwMDoc *)));
	conf()->confWndMainWndSize(size());
    //LR closing of windows changed
    //needed for fastload option on PDA
	//emit closed(this);
	//qDebug("PwM::~PwM() emited closed(this)");
	delete view;
    delete syncManager;
}

void PwM::initMenubar()
{
  KIconLoader* picons;
#ifndef PWM_EMBEDDED
	KIconLoader icons;
	picons = &icons;
#else
	picons = KGlobal::iconLoader();


	syncPopup = new KPopupMenu(this);
	
	syncManager = new KSyncManager((QWidget*)this, (KSyncInterface*)this, KSyncManager::PWMPI, PWMPrefs::instance(), syncPopup);
	syncManager->setBlockSave(false);

	connect ( syncPopup, SIGNAL( activated ( int )  ), syncManager, SLOT (slotSyncMenu( int ) ) );
	syncManager->fillSyncMenu();

#endif
	filePopup = new KPopupMenu(this);
	importPopup = new KPopupMenu(filePopup);
	exportPopup = new KPopupMenu(filePopup);
	managePopup = new KPopupMenu(this);
#ifdef CONFIG_KEYCARD
	chipcardPopup = new KPopupMenu(this);
#endif // CONFIG_KEYCARD
	viewPopup = new KPopupMenu(this);
	optionsPopup = new KPopupMenu(this);

// "file" popup menu
	filePopup->insertItem(QIconSet(picons->loadIcon("filenew", KIcon::Small)),
			      i18n("&New"), this,
			      SLOT(new_slot()), 0, BUTTON_POPUP_FILE_NEW);
	filePopup->insertItem(QIconSet(picons->loadIcon("fileopen", KIcon::Small)),
			      i18n("&Open"), this,
			      SLOT(open_slot()), 0, BUTTON_POPUP_FILE_OPEN);
	filePopup->insertItem(QIconSet(picons->loadIcon("fileclose", KIcon::Small)),
			      i18n("&Close"), this,
			      SLOT(close_slot()), 0, BUTTON_POPUP_FILE_CLOSE);
	filePopup->insertSeparator();
	filePopup->insertItem(QIconSet(picons->loadIcon("filesave", KIcon::Small)),
			      i18n("&Save"), this,
			      SLOT(save_slot()), 0, BUTTON_POPUP_FILE_SAVE);
	filePopup->insertItem(QIconSet(picons->loadIcon("filesaveas", KIcon::Small)),
			      i18n("Save &as..."),
			      this, SLOT(saveAs_slot()), 0,
			      BUTTON_POPUP_FILE_SAVEAS);
	filePopup->insertSeparator();
	// "file/export" popup menu
	exportPopup->insertItem(i18n("&Text-file..."), this,
				SLOT(exportToText()), 0, BUTTON_POPUP_EXPORT_TEXT);
	exportPopup->insertItem(i18n("&Gpasman / Kpasman ..."), this,
				SLOT(exportToGpasman()), 0, BUTTON_POPUP_EXPORT_GPASMAN);
	exportPopup->insertItem(i18n("&CSV (Comma Separated Value) ..."), this,
				SLOT(exportToCsv()), 0, BUTTON_POPUP_EXPORT_CSV);
#ifdef CONFIG_KWALLETIF
	exportPopup->insertItem(i18n("&KWallet..."), this,
				SLOT(exportToKWallet()), 0, BUTTON_POPUP_EXPORT_KWALLET);
#endif
	filePopup->insertItem(QIconSet(picons->loadIcon("fileexport", KIcon::Small)),
			      i18n("E&xport"), exportPopup,
			      BUTTON_POPUP_FILE_EXPORT);
	// "file/import" popup menu
	importPopup->insertItem(i18n("&Text-file..."), this,
				SLOT(importFromText()), 0, BUTTON_POPUP_IMPORT_TEXT);
	importPopup->insertItem(i18n("&Gpasman / Kpasman ..."), this,
				SLOT(importFromGpasman()), 0, BUTTON_POPUP_IMPORT_GPASMAN);
	importPopup->insertItem(i18n("&CSV (Comma Separated Value) ..."), this,
				SLOT(importCsv()), 0, BUTTON_POPUP_IMPORT_CSV);
#ifdef CONFIG_KWALLETIF
	importPopup->insertItem(i18n("&KWallet..."), this,
				SLOT(importKWallet()), 0, BUTTON_POPUP_IMPORT_KWALLET);
#endif
	filePopup->insertItem(QIconSet(picons->loadIcon("fileimport", KIcon::Small)),
			      i18n("I&mport"), importPopup,
			      BUTTON_POPUP_FILE_IMPORT);
	filePopup->insertSeparator();
#if 0
	filePopup->insertItem(QIconSet(picons->loadIcon("fileprint", KIcon::Small)),
			      i18n("&Print..."), this,
			      SLOT(print_slot()), 0, BUTTON_POPUP_FILE_PRINT);
	filePopup->insertSeparator();
#endif
	filePopup->insertItem(QIconSet(picons->loadIcon("exit", KIcon::Small)),
			      i18n("&Quit"), this,
			      SLOT(quitButton_slot()), 0, BUTTON_POPUP_FILE_QUIT);
	menuBar()->insertItem(i18n("&File"), filePopup);
// "manage" popup menu
	managePopup->insertItem(QIconSet(picons->loadIcon("pencil", KIcon::Small)),
				i18n("&Add password"), this,
				SLOT(addPwd_slot()), 0,
				BUTTON_POPUP_MANAGE_ADD);
	managePopup->insertSeparator();
	managePopup->insertItem(QIconSet(picons->loadIcon("edit", KIcon::Small)),
				i18n("&Edit"), this, SLOT(editPwd_slot()), 0,
				BUTTON_POPUP_MANAGE_EDIT);
	managePopup->insertItem(QIconSet(picons->loadIcon("editdelete", KIcon::Small)),
				i18n("&Delete"), this, SLOT(deletePwd_slot()),
				0, BUTTON_POPUP_MANAGE_DEL);
	managePopup->insertSeparator();
	managePopup->insertItem(QIconSet(picons->loadIcon("rotate", KIcon::Small)),
				i18n("Change &Master Password"), this,
				SLOT(changeMasterPwd_slot()), 0,
				BUTTON_POPUP_MANAGE_CHANGEMP);
	menuBar()->insertItem(i18n("&Manage"), managePopup);
// "chipcard" popup menu
#ifdef CONFIG_KEYCARD
	chipcardPopup->insertItem(QIconSet(picons->loadIcon("filenew", KIcon::Small)),
				  i18n("&Generate new key-card"), this,
				  SLOT(genNewCard_slot()), 0,
				  BUTTON_POPUP_CHIPCARD_GENNEW);
	chipcardPopup->insertItem(QIconSet(picons->loadIcon("editdelete", KIcon::Small)),
				  i18n("&Erase key-card"), this,
				  SLOT(eraseCard_slot()), 0,
				  BUTTON_POPUP_CHIPCARD_DEL);
	chipcardPopup->insertItem(QIconSet(picons->loadIcon("", KIcon::Small)),
				  i18n("Read card-&ID"), this,
				  SLOT(readCardId_slot()), 0,
				  BUTTON_POPUP_CHIPCARD_READID);
	chipcardPopup->insertSeparator();
	chipcardPopup->insertItem(QIconSet(picons->loadIcon("2rightarrow", KIcon::Small)),
				  i18n("&Make card backup-image"), this,
				  SLOT(makeCardBackup_slot()), 0,
				  BUTTON_POPUP_CHIPCARD_SAVEBACKUP);
	chipcardPopup->insertItem(QIconSet(picons->loadIcon("2leftarrow", KIcon::Small)),
				  i18n("&Replay card backup-image"), this,
				  SLOT(replayCardBackup_slot()), 0,
				  BUTTON_POPUP_CHIPCARD_REPLAYBACKUP);
	menuBar()->insertItem(i18n("&Chipcard manager"), chipcardPopup);
#endif // CONFIG_KEYCARD
// "view" popup menu
	viewPopup->insertItem(QIconSet(picons->loadIcon("find", KIcon::Small)),
				i18n("&Find"), this,
				SLOT(find_slot()), 0, BUTTON_POPUP_VIEW_FIND);
	viewPopup->insertSeparator();
	viewPopup->insertItem(QIconSet(picons->loadIcon("halfencrypted", KIcon::Small)),
				i18n("&Lock all entries"), this,
				SLOT(lockWnd_slot()), 0,
				BUTTON_POPUP_VIEW_LOCK);
	viewPopup->insertItem(QIconSet(picons->loadIcon("encrypted", KIcon::Small)),
				i18n("&Deep-lock all entries"), this,
				SLOT(deepLockWnd_slot()), 0,
				BUTTON_POPUP_VIEW_DEEPLOCK);
	viewPopup->insertItem(QIconSet(picons->loadIcon("decrypted", KIcon::Small)),
				i18n("&Unlock all entries"), this,
				SLOT(unlockWnd_slot()), 0,
				BUTTON_POPUP_VIEW_UNLOCK);
	menuBar()->insertItem(i18n("&View"), viewPopup);
// "options" popup menu
	optionsPopup->insertItem(QIconSet(picons->loadIcon("configure", KIcon::Small)),
				 i18n("&Configure PwM/Pi..."), this,
                 SLOT(config_slot()),0,
				 BUTTON_POPUP_OPTIONS_CONFIG);
	optionsPopup->insertItem(QIconSet(picons->loadIcon("configure", KIcon::Small)),
				 i18n("Global Settings..."), this,
                 SLOT(config_global_slot()),0,
				 BUTTON_POPUP_OPTIONS_GLOBAL_CONFIG);
    if ( QApplication::desktop()->width() <= 240 ) {
        managePopup->insertSeparator();
        managePopup->insertItem(i18n("&Options"), optionsPopup);
    } else
        menuBar()->insertItem(i18n("&Options"), optionsPopup);
// "help" popup menu
#ifndef PWM_EMBEDDED
	helpPopup = helpMenu(QString::null, false);
#else
	optionsPopup->insertItem(QIconSet(picons->loadIcon("configure", KIcon::Small)),
				 i18n("C&ategories..."), this,
                 SLOT(category_slot()),0,
				 BUTTON_POPUP_OPTIONS_CATEGORY);


	menuBar()->insertItem(i18n("&Sync"), syncPopup);





	helpPopup = new KPopupMenu(this);


	helpPopup->insertItem(i18n("&License"), this,
				SLOT(showLicense_slot()), 0,
				BUTTON_POPUP_HELP_LICENSE);

	helpPopup->insertItem(i18n("&Faq"), this,
				SLOT(faq_slot()), 0,
				BUTTON_POPUP_HELP_FAQ);

	helpPopup->insertItem(i18n("&About PwManager"), this,
				SLOT(createAboutData_slot()), 0,
				BUTTON_POPUP_HELP_ABOUT);

	helpPopup->insertItem(i18n("&Sync HowTo"), this,
				SLOT(syncHowTo_slot()), 0,
				BUTTON_POPUP_HELP_SYNC);

	helpPopup->insertItem(i18n("&What's New"), this,
				SLOT(whatsnew_slot()), 0,
				BUTTON_POPUP_HELP_WHATSNEW);

#endif
	menuBar()->insertItem(i18n("&Help"), helpPopup);

}

void PwM::initToolbar()
{
  KIconLoader* picons;
#ifndef PWM_EMBEDDED
	KIconLoader icons;
	picons = &icons;
#else
	picons = KGlobal::iconLoader();
#endif

#if 0
	toolBar()->insertButton(picons->loadIcon("filenew", KIcon::Toolbar),
				BUTTON_TOOL_NEW, SIGNAL(clicked(int)), this,
				SLOT(new_slot()), true, i18n("New"));
#endif
	toolBar()->insertButton(picons->loadIcon("fileopen", KIcon::Toolbar),
				BUTTON_TOOL_OPEN, SIGNAL(clicked(int)), this,
				SLOT(open_slot()), true, i18n("Open"));
	toolBar()->insertButton(picons->loadIcon("pencil", KIcon::Toolbar),
				BUTTON_TOOL_ADD, SIGNAL(clicked(int)), this,
				SLOT(addPwd_slot()), true,
				i18n("Add password"));
	toolBar()->insertSeparator();
    
	toolBar()->insertButton(picons->loadIcon("find", KIcon::Toolbar),
				BUTTON_TOOL_FIND, SIGNAL(clicked(int)), this,
				SLOT(find_slot()), true, i18n("Find entry"));	
	toolBar()->insertSeparator();
	toolBar()->insertButton(picons->loadIcon("filesave", KIcon::Toolbar),
				BUTTON_TOOL_SAVE, SIGNAL(clicked(int)), this,
				SLOT(save_slot()), true, i18n("Save"));
#if 0
	toolBar()->insertButton(picons->loadIcon("filesaveas", KIcon::Toolbar),
				BUTTON_TOOL_SAVEAS, SIGNAL(clicked(int)), this,
				SLOT(saveAs_slot()), true, i18n("Save as"));
#endif


#if 0
	toolBar()->insertButton(picons->loadIcon("fileprint", KIcon::Toolbar),
				BUTTON_TOOL_PRINT, SIGNAL(clicked(int)), this,
				SLOT(print_slot()), true, i18n("Print..."));
#endif
	toolBar()->insertSeparator();
	toolBar()->insertButton(picons->loadIcon("edit", KIcon::Toolbar),
				BUTTON_TOOL_EDIT, SIGNAL(clicked(int)), this,
				SLOT(editPwd_slot()), true,
				i18n("Edit password"));
	toolBar()->insertButton(picons->loadIcon("editdelete", KIcon::Toolbar),
				BUTTON_TOOL_DEL, SIGNAL(clicked(int)), this,
				SLOT(deletePwd_slot()), true,
				i18n("Delete password"));
	toolBar()->insertSeparator();
	toolBar()->insertButton(picons->loadIcon("halfencrypted", KIcon::Toolbar),
				BUTTON_TOOL_LOCK, SIGNAL(clicked(int)), this,
				SLOT(lockWnd_slot()), true,
				i18n("Lock all entries"));
	toolBar()->insertButton(picons->loadIcon("encrypted", KIcon::Toolbar),
				BUTTON_TOOL_DEEPLOCK, SIGNAL(clicked(int)), this,
				SLOT(deepLockWnd_slot()), true,
				i18n("Deep-Lock all entries"));
	toolBar()->insertButton(picons->loadIcon("decrypted", KIcon::Toolbar),
				BUTTON_TOOL_UNLOCK, SIGNAL(clicked(int)), this,
				SLOT(unlockWnd_slot()), true,
				i18n("Unlock all entries"));
}

void PwM::initMetrics()
{
	QSize s = conf()->confWndMainWndSize();
	if (s.isValid())
		resize(s);
	else
		resize(DEFAULT_SIZE);
}

void PwM::updateCaption()
{
    QString  s = curDoc()->getTitle();
   int maxlen = 50;
    if ( QApplication::desktop()->width() < 640 ) {
        if ( QApplication::desktop()->width() < 320 )
            maxlen = 22;
        else
            maxlen = 35;
    }
    if ( s.length() > maxlen ) {
        s = "..."+s.right(maxlen -3);
    }
	setPlainCaption( s );
}

void PwM::hideEvent(QHideEvent *)
{
	if (isMinimized()) {
		if (init->tray()) {
			forceMinimizeToTray = true;
			close();
		}
		int mmlock = conf()->confGlobMinimizeLock();
		switch (mmlock) {
		case 0:		// don't lock anything
			break;
		case 1: {	// normal lock
			curDoc()->lockAll(true);
			break;
		} case 2: {	// deep-lock
			curDoc()->deepLock();
			break;
		} default:
			WARN();
		}
	}
}

void PwM::setVirgin(bool v)
{
	if (virgin == v)
		return;
	virgin = v;
	filePopup->setItemEnabled(BUTTON_POPUP_FILE_SAVE, !v);
	filePopup->setItemEnabled(BUTTON_POPUP_FILE_SAVEAS, !v);
	filePopup->setItemEnabled(BUTTON_POPUP_FILE_EXPORT, !v);
	filePopup->setItemEnabled(BUTTON_POPUP_FILE_PRINT, !v);
	managePopup->setItemEnabled(BUTTON_POPUP_MANAGE_EDIT, !v);
	managePopup->setItemEnabled(BUTTON_POPUP_MANAGE_DEL, !v);
	managePopup->setItemEnabled(BUTTON_POPUP_MANAGE_CHANGEMP, !v);
	viewPopup->setItemEnabled(BUTTON_POPUP_VIEW_LOCK, !v);
	viewPopup->setItemEnabled(BUTTON_POPUP_VIEW_DEEPLOCK, !v);
	viewPopup->setItemEnabled(BUTTON_POPUP_VIEW_UNLOCK, !v);
	viewPopup->setItemEnabled(BUTTON_POPUP_VIEW_FIND, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_SAVE, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_SAVEAS, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_PRINT, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_EDIT, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_DEL, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_LOCK, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_DEEPLOCK, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_UNLOCK, !v);
	toolBar()->setItemEnabled(BUTTON_TOOL_FIND, !v);
}

void PwM::new_slot()
{
	init->createMainWnd();
}

//US ENH
void PwM::open_slot()
{
	open_slot("");
}

void PwM::open_slot(QString fn)
{
	openDoc(fn);
}

PwMDoc * PwM::openDoc(QString filename, bool openDeepLocked)
{
	if (!isVirgin()) {
		// open the document in a new window.
		PwM *newInstance = init->createMainWnd();
		PwMDoc *newDoc = newInstance->openDoc(filename, openDeepLocked);
		if (!newDoc) {
			newInstance->setForceQuit(true);
			delete_and_null(newInstance);
		}
		return newDoc;
	}

	if (!curDoc()->openDocUi(curDoc(), filename, openDeepLocked))
		return 0;
	showStatMsg(i18n("Successfully opened file."));
	updateCaption();
	setVirgin(false);
	return curDoc();
}

PwMView * PwM::makeNewListView(PwMDoc *doc)
{
	PwMView *ret = new PwMView(this, this, doc);
	ret->setFont(conf()->confGlobEntryFont());
	ret->show();
	return ret;
}

void PwM::close_slot()
{
	close();
}

void PwM::quitButton_slot()
{
	init->shutdownApp(0);
}

void PwM::save_slot()
{
	save();
}

bool PwM::save()
{
	if (!curDoc()->saveDocUi(curDoc()))
		return false;
	showStatMsg(i18n("Successfully saved data."));
	updateCaption();
	return true;
}

void PwM::saveAs_slot()
{
	saveAs();
}

bool PwM::saveAs()
{
	if (!curDoc()->saveAsDocUi(curDoc()))
		return false;
	showStatMsg(i18n("Successfully saved data."));
	updateCaption();
	return true;
}

//US ENH : changed code to run with older MOC
void PwM::addPwd_slot()
{
  addPwd_slot1(0, 0);
}

void PwM::addPwd_slot1(QString *pw, PwMDoc *_doc)
{
	PwMDoc *doc;
	if (_doc) {
		doc = _doc;
	} else {
		doc = curDoc();
	}
	PWM_ASSERT(doc);
	doc->timer()->getLock(DocTimer::id_autoLockTimer);
#ifndef PWM_EMBEDDED
	AddEntryWndImpl w;
#else
	AddEntryWndImpl w(doc, this, "addentrywndimpl");
#endif
    w.setCaption( i18n ("Add new password") );
	vector<string> catList;
	doc->getCategoryList(&catList);
	unsigned i, size = catList.size();
	for (i = 0; i < size; ++i) {
		w.addCategory(catList[i].c_str());
	}
	w.setCurrCategory(view->getCurrentCategory());
	if (pw)
		w.pwLineEdit->setText(*pw);

      tryAgain:
    w.descLineEdit->setFocus();
	if (w.exec() == 1) 
	  {
		PwMDataItem d;

		//US BUG: to initialize all values of curEntr with meaningfulldata,
		// we call clear on it. Reason: Metadata will be uninitialized otherwise.
		// another option would be to create a constructor for PwMDataItem
		d.clear(true);

		d.desc = w.getDescription().latin1();
		d.name = w.getUsername().latin1();
		d.pw = w.getPassword().latin1();
		d.comment = w.getComment().latin1();
		d.url = w.getUrl().latin1();
		d.launcher = w.getLauncher().latin1();
		PwMerror ret = doc->addEntry(w.getCategory(), &d);
		if (ret == e_entryExists) {
			KMessageBox::error(this,
					   i18n
					   ("An entry with this \"Description\",\n"
					    "does already exist.\n"
					    "Please select another description."),
					   i18n("entry already exists."));
			goto tryAgain;
		} else if (ret == e_maxAllowedEntr) {
			KMessageBox::error(this, i18n("The maximum possible number of\nentries"
						      "has been reached.\nYou can't add more entries."),
						 i18n("maximum number of entries"));
			doc->timer()->putLock(DocTimer::id_autoLockTimer);
			return;
		}
	}
	setVirgin(false);
	doc->timer()->putLock(DocTimer::id_autoLockTimer);
}

//US ENH : changed code to run with older MOC
void PwM::editPwd_slot()
{
  editPwd_slot3(0,0,0);
}

void PwM::editPwd_slot1(const QString *category)
{
  editPwd_slot3(category, 0, 0);
}

void PwM::editPwd_slot3(const QString *category, const int *index,
		       PwMDoc *_doc)
{
	PwMDoc *doc;
	if (_doc) {
		doc = _doc;
	} else {
		doc = curDoc();
	}
	PWM_ASSERT(doc);
	if (doc->isDocEmpty())
		return;
	if (doc->isDeepLocked())
		return;
	doc->timer()->getLock(DocTimer::id_autoLockTimer);
	unsigned int curEntryIndex;
	if (index) {
		curEntryIndex = *index;
	} else {
		if (!(view->getCurEntryIndex(&curEntryIndex))) {
            qDebug("couldn't get index. Maybe we have a binary entry here. ");
			printDebug("couldn't get index. Maybe we have a binary entry here.");
			doc->timer()->putLock(DocTimer::id_autoLockTimer);
			return;
		}
	}
	QString curCategory;
	if (category) {
		curCategory = *category;
	} else {
		curCategory = view->getCurrentCategory();
	}
	PwMDataItem currItem;
	if (!doc->getEntry(curCategory, curEntryIndex, &currItem, true)) {
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return;
	}
	BUG_ON(currItem.binary);
	AddEntryWndImpl w(doc);
    w.setCaption( i18n ("Edit password") );
	vector<string> catList;
	doc->getCategoryList(&catList);
	unsigned i, size = catList.size();
	for (i = 0; i < size; ++i) {
		w.addCategory(catList[i].c_str());
	}
	w.setCurrCategory(curCategory);
	w.setDescription(currItem.desc.c_str());
	w.setUsername(currItem.name.c_str());
	w.setPassword(currItem.pw.c_str());
	w.setUrl(currItem.url.c_str());
	w.setLauncher(currItem.launcher.c_str());
	w.setComment(currItem.comment.c_str());
    w.descLineEdit->setFocus();
	if (w.exec() == 1) {
		currItem.desc = w.getDescription().latin1();
		currItem.name = w.getUsername().latin1();
		currItem.pw = w.getPassword().latin1();
		currItem.comment = w.getComment().latin1();
		currItem.url = w.getUrl().latin1();
		currItem.launcher = w.getLauncher().latin1();
		if (!doc->editEntry(curCategory, w.getCategory(),
					 curEntryIndex, &currItem)) {
			KMessageBox::error(this,
					   i18n("Couldn't edit the entry.\n"
						"Maybe you changed the category and\n"
						"this entry is already present\nin the new "
						"category?"),
					   i18n("couldn't edit entry."));
			doc->timer()->putLock(DocTimer::id_autoLockTimer);
			return;
		}
	}
	doc->timer()->putLock(DocTimer::id_autoLockTimer);
}

void PwM::deletePwd_slot()
{
	PWM_ASSERT(curDoc());
	if (curDoc()->isDocEmpty())
		return;
	if (curDoc()->isDeepLocked())
		return;
	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	unsigned int curEntryIndex = 0;
	if (!(view->getCurEntryIndex(&curEntryIndex))) {
		printDebug("couldn't get index");
		curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
		return;
	}

	PwMDataItem currItem;
	QString curCategory = view->getCurrentCategory();
	if (!curDoc()->getEntry(curCategory, curEntryIndex, &currItem)) {
		printDebug("couldn't get entry");
		curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
		return;
	}
	if (KMessageBox::
	    questionYesNo(this,
			  i18n
			  ("Do you really want to delete\nthe selected entry") +
			  " \n\"" + QString(currItem.desc.c_str())
			  + "\" ?", i18n("delete?"))
	    == KMessageBox::Yes) {

		curDoc()->delEntry(curCategory, curEntryIndex);
	}
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
}

void PwM::changeMasterPwd_slot()
{
	PWM_ASSERT(curDoc());
	curDoc()->changeCurrentPw();
}

void PwM::lockWnd_slot()
{
	PWM_ASSERT(curDoc());
	curDoc()->lockAll(true);
}

void PwM::deepLockWnd_slot()
{
	PWM_ASSERT(curDoc());
	curDoc()->deepLock();
}

void PwM::unlockWnd_slot()
{
	PWM_ASSERT(curDoc());
	curDoc()->lockAll(false);
}
void PwM::config_global_slot()
{
  KPimPrefsGlobalDialog gc ( this );
    gc.exec();
}
void PwM::config_slot()
{
  int oldStyle = conf()->confWndMainViewStyle();

#if 0 
    KCMultiDialog*  ConfigureDialog = new KCMultiDialog( "PIM", this ,"pwmconfigdialog", true );

    KCMPwmConfig* pwmcfg = new KCMPwmConfig( ConfigureDialog->getNewVBoxPage(i18n( "PwManager")) , "KCMPwmConfig" );
    ConfigureDialog->addModule(pwmcfg );

    KCMKdePimConfig* kdelibcfg = new KCMKdePimConfig( ConfigureDialog->getNewVBoxPage(i18n( "Global")) , "KCMKdeLibConfig" );
    ConfigureDialog->addModule(kdelibcfg );
#endif



    KDialogBase * ConfigureDialog = new KDialogBase (  KDialogBase::Plain , i18n("Configure KA/Pi"), KDialogBase::Default |KDialogBase::Cancel | KDialogBase::Apply | KDialogBase::Ok, KDialogBase::Ok,0, "name", true, true);

    KCMPwmConfig* kabcfg = new KCMPwmConfig( ConfigureDialog , "KCMpwmConfig" );
    ConfigureDialog->setMainWidget( kabcfg );
    connect( ConfigureDialog, SIGNAL( applyClicked() ),
            kabcfg, SLOT( save() ) );
    connect( ConfigureDialog, SIGNAL( acceptClicked() ),
             kabcfg, SLOT( save() ) );
    connect( ConfigureDialog, SIGNAL( defaultClicked() ),
             kabcfg, SLOT( defaults() ) );
    //saveSettings();
    kabcfg->load();
#ifndef DESKTOP_VERSION
    if ( QApplication::desktop()->height() <= 480 )
        ;// ConfigureDialog->hideButtons(); 
    ConfigureDialog->showMaximized();
#endif


    if ( ConfigureDialog->exec() )
        KMessageBox::information( this,  i18n("Some changes are only\neffective after a restart!\n") );
    delete ConfigureDialog;



	int newStyle = conf()->confWndMainViewStyle();
	// reinitialize tray
	init->initTray();
	// reinitialize KWallet emulation
	init->initKWalletEmu();

	PwMDocList *_dl = PwMDoc::getOpenDocList();
	const vector<PwMDocList::listItem> *dl = _dl->getList();
	vector<PwMDocList::listItem>::const_iterator i = dl->begin(),
						     end = dl->end();
	PwMDoc *doc;
	while (i != end) {
		doc = (*i).doc;
		// unlock-without-mpw timeout
		doc->timer()->start(DocTimer::id_mpwTimer);
		// auto-lock timeout
		doc->timer()->start(DocTimer::id_autoLockTimer);
		++i;
	}

	const QValueList<PwM *> *ml = init->mainWndList();
#ifndef PWM_EMBEDDED
	QValueList<PwM *>::const_iterator i2 = ml->begin(),
					  end2 = ml->end();
#else
	QValueList<PwM *>::ConstIterator i2 = ml->begin(),
					  end2 = ml->end();
#endif
	PwM *pwm;
	while (i2 != end2) {
		pwm = *i2;
		// reinitialize the window style.
		if (oldStyle != newStyle)
			pwm->curView()->initStyle(newStyle);
		// set the new font
		pwm->curView()->setFont(conf()->confGlobEntryFont());
		++i2;
	}
}

void PwM::activateMpButton(bool activate)
{
	managePopup->setItemEnabled(BUTTON_POPUP_MANAGE_CHANGEMP, activate);
}

void PwM::closeEvent(QCloseEvent *e)
{
    //qDebug("PwM::closeEvent ");
    emit closed( this );
    return;
	e->accept();
}

void PwM::docClosed(PwMDoc *doc)
{
    //qDebug("PwM::docClosed ");
	PARAM_UNUSED(doc);
	PWM_ASSERT(doc == curDoc());
	close();
}

void PwM::find_slot()
{
	PWM_ASSERT(curDoc());
	if (curDoc()->isDocEmpty())
		return;
	if (curDoc()->isDeepLocked())
		return;
	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	FindWndImpl findWnd(view);
	findWnd.exec();
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
}

void PwM::exportToText()
{
	PWM_ASSERT(curDoc());
	if (curDoc()->isDocEmpty()) {
		KMessageBox::information(this,
					 i18n
					 ("Sorry, there's nothing to export.\n"
					  "Please first add some passwords."),
					 i18n("nothing to do"));
		return;
	}
	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	QString fn(KFileDialog::getSaveFileName(QString::null,
						i18n("*|plain-text file"),
						this));
	if (fn == "") {
		curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
		return;
	}

	PwMerror ret = curDoc()->exportToText(&fn);
	if (ret != e_success) {
		KMessageBox::error(this,
				   i18n("Error: Couldn't write to file.\n"
					"Please check if you have permission to write\n"
					"to the file in that directory."),
				   i18n("error while writing"));
	} else
		showStatMsg(i18n("Successfully exported data."));
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
}

bool PwM::importFromText()
{
	if (!isVirgin()) {
		if (KMessageBox::questionYesNo(this,
					       i18n("Do you want to import the data\n"
					            "into the current document? (If you\n"
					            "select \"no\", a new document will be\n"
					            "opened.)"),
					       i18n("import into this document?"))
		    == KMessageBox::No) {
			// import the data to a new window.
			PwM *newInstance = init->createMainWnd();
			bool ok = newInstance->importFromText();
			if (!ok) {
				newInstance->setForceQuit(true);
				delete_and_null(newInstance);
			}
			return ok;
		}
	}

	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	PwMerror ret;
	QString path(KFileDialog::getOpenFileName(QString::null,
						  i18n("*|PWM-exported text file"),
						  this));
	if (path == "")
		goto cancelImport;

	ret = curDoc()->importFromText(&path, 0);
	if (ret == e_fileFormat) {
		KMessageBox::error(this,
				   i18n("Could not read file-format.\n"
					"This seems to be _not_ a valid file\n"
					"exported by PwM."),
				   i18n("invalid file-format"));
		goto cancelImport;
	} else if (ret == e_invalidArg) {
		BUG();
		goto cancelImport;
	} else if (ret != e_success) {
		KMessageBox::error(this,
				   i18n("Could not import file!\n"
					"Do you have permission to read this file?\n"
					"Do you have enough free memory?"),
				   i18n("import failed"));
		goto cancelImport;
	}
	setVirgin(false);
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
	return true;

cancelImport:
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
	return false;
}

void PwM::exportToGpasman()
{
	PWM_ASSERT(curDoc());
	if (curDoc()->isDocEmpty()) {
		KMessageBox::information(this,
					 i18n
					 ("Sorry, there's nothing to export.\n"
					  "Please first add some passwords."),
					 i18n("nothing to do"));
		return;
	}
	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	QString fn(KFileDialog::getSaveFileName(QString::null,
						i18n("*|Gpasman or Kpasman file"),
						this));
	if (fn == "") {
		curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
		return;
	}

	PwMerror ret = curDoc()->exportToGpasman(&fn);
	if (ret != e_success) {
		if (ret == e_noPw) {
			curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
			return;
		}
		KMessageBox::error(this,
				   i18n("Error: Couldn't write to file.\n"
					"Please check if you have permission to write "
					"to the file in that directory."),
				   i18n("error while writing"));
	} else
		showStatMsg(i18n("Successfully exported data."));
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
}



void PwM::exportToCsv()
{
  PWM_ASSERT(curDoc());
  if (curDoc()->isDocEmpty()) {
    KMessageBox::information(this,
			     i18n
			     ("Sorry, there is nothing to export;\n"
			      "please add some passwords first."),
			     i18n("Nothing to Do"));
    return;
  }

  curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
  QString fn(KFileDialog::getSaveFileName("*.csv", i18n("*|CSV Text File"), this));
  if (fn.isEmpty()) {
    curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
    return;
  }

  Csv csv(this);
  if (!csv.exportData(fn, curDoc())) {
    curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
    showStatMsg(i18n("CSV file export failed."));
    return;
  }
  showStatMsg(i18n("Successfully exported data."));
  curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
}

bool PwM::importCsv()
{
  Csv csv(this);
  if (!isVirgin()) {
    if (KMessageBox::questionYesNo(this,
       i18n("Do you want to import the data\n"
            "into the current document? (If you\n"
            "select \"no\", a new document will be\n"
            "opened.)"),
       i18n("Import into This Document?"))
    == KMessageBox::No) {
      // import the data to a new window.
      PwM *newInstance = init->createMainWnd();
      bool ok = newInstance->importCsv();
      if (!ok) {
	newInstance->setForceQuit(true);
	delete_and_null(newInstance);
      }
      return ok;
    }
  }

  QString filename = KFileDialog::getOpenFileName("*.csv", i18n("*|CSV Text File"), this);
  if (filename.isEmpty())
    return false;
  curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
  if (!csv.importData(filename, curDoc())) {
    curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
    showStatMsg(i18n("CSV file import failed."));
    return false;
  }
  curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
  KMessageBox::information(this,
			   i18n("Successfully imported the CSV data\n"
      "into the current document."), i18n("Successfully Imported"));
  showStatMsg(i18n("Successfully imported"));
  setVirgin(false);
  return true;
}


void PwM::exportToKWallet()
{
#ifdef CONFIG_KWALLETIF
	if (!checkAndAskForKWalletEmu())
		return;
	PWM_ASSERT(curDoc());
	if (curDoc()->isDocEmpty()) {
		KMessageBox::information(this,
					 i18n
					 ("Sorry, there's nothing to export.\n"
					  "Please first add some passwords."),
					 i18n("nothing to do"));
		init->initKWalletEmu();
		return;
	}
	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	KWalletIf walletIf(this);
	if (walletIf.kwalletExport(curDoc())) {
		KMessageBox::information(this,
					 i18n("Successfully exported the data of the current "
					      "document to KWallet."),
					 i18n("Successfully exported data."));
		showStatMsg(i18n("Successfully exported data."));
	}
	init->initKWalletEmu();
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
#endif // CONFIG_KWALLETIF
}

bool PwM::importFromGpasman()
{
	if (!isVirgin()) {
		if (KMessageBox::questionYesNo(this,
					       i18n("Do you want to import the data\n"
					            "into the current document? (If you\n"
					            "select \"no\", a new document will be\n"
					            "opened.)"),
					       i18n("import into this document?"))
		    == KMessageBox::No) {
			// import the data to a new window.
			PwM *newInstance = init->createMainWnd();
			bool ok = newInstance->importFromGpasman();
			if (!ok) {
				newInstance->setForceQuit(true);
				delete_and_null(newInstance);
			}
			return ok;
		}
	}

	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	PwMerror ret;
	QString path(KFileDialog::getOpenFileName(QString::null,
						  i18n("*|Gpasman or Kpasman file"), this));
	if (path == "")
		goto cancelImport;
	ret = curDoc()->importFromGpasman(&path);
	if (ret == e_wrongPw) {
		if (KMessageBox::questionYesNo(this,
				 	       i18n
					       ("This is probably the wrong master-password\n"
					        "you have typed in.\n"
					        "There is no real way to determine the\n"
					        "correctness of the password in the Gpasman\n"
					        "file-format. But I think this\n"
					        "password ist wrong.\n"
					        "Do you want to continue nevertheless?"),
					       i18n("password error"))
		    == KMessageBox::No) {
			goto cancelImport;
		}
	} else if (ret != e_success) {
		KMessageBox::error(this,
				   i18n("Could not import file!\n"
					"Do you have permission to read this file?"),
				   i18n("import failed"));
		goto cancelImport;
	}
	setVirgin(false);
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
	return true;

cancelImport:
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
	return false;
}

#ifdef CONFIG_KWALLETIF
bool PwM::checkAndAskForKWalletEmu()
{
	if (init->kwalletEmu()) {
		/* KWallet emulation is enabled. We can't import/export
		 * data from/to it, while emulation is active.
		 */
		if (KMessageBox::questionYesNo(this,
					       i18n("KWallet emulation is enabled.\n"
						    "You can't import or export data from/to "
						    "the original KWallet, while the emulation "
						    "is active.\n"
						    "Do you want to tempoarly disable the KWallet emulation?"),
					       i18n("Tempoarly disable KWallet emulation?"))
		    == KMessageBox::Yes) {
			init->initKWalletEmu(true);
			PWM_ASSERT(!init->kwalletEmu());
			return true;
		}
		return false;
	}
	return true;
}
#endif // CONFIG_KWALLETIF

bool PwM::importKWallet()
{
#ifdef CONFIG_KWALLETIF
	if (!checkAndAskForKWalletEmu())
		return false;
	KWalletIf walletIf(this);
	if (!isVirgin()) {
		if (KMessageBox::questionYesNo(this,
					       i18n("Do you want to import the data "
					            "into the current document? (If you "
					            "select \"no\", a new document will be "
					            "opened.)"),
					       i18n("import into this document?"))
		    == KMessageBox::No) {
			// import the data to a new window.
			PwM *newInstance = init->createMainWnd();
			bool ok = newInstance->importKWallet();
			if (!ok) {
				newInstance->setForceQuit(true);
				delete_and_null(newInstance);
				goto exit_fail;
			} else {
				goto exit_ok;
			}
		}
	}
	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
	if (!walletIf.kwalletImport(curDoc())) {
		curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
		showStatMsg(i18n("KWallet import failed"));
		goto exit_fail;
	}
	KMessageBox::information(this,
				 i18n("Successfully imported the KWallet data "
				      "into the current document."),
				 i18n("successfully imported"));
	showStatMsg(i18n("successfully imported"));
	setVirgin(false);
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);

exit_ok:
	init->initKWalletEmu();
	return true;

exit_fail:
	init->initKWalletEmu();
#endif // CONFIG_KWALLETIF
	return false;
}

void PwM::print_slot()
{
	curDoc()->timer()->getLock(DocTimer::id_autoLockTimer);
#ifndef PWM_EMBEDDED
	PwMPrint p(curDoc(), this);
	p.printNow();
#else
	qDebug("PwM::print_slot , PRINTING IS NOT IMPLEMENTED");
#endif
	curDoc()->timer()->putLock(DocTimer::id_autoLockTimer);
}

void PwM::genNewCard_slot()
{
#ifdef CONFIG_KEYCARD
	init->keycard()->genNewCard();
#endif
}

void PwM::eraseCard_slot()
{
#ifdef CONFIG_KEYCARD
	init->keycard()->eraseCard();
#endif
}

void PwM::readCardId_slot()
{
#ifdef CONFIG_KEYCARD
	init->keycard()->displayKey();
#endif
}

void PwM::makeCardBackup_slot()
{
#ifdef CONFIG_KEYCARD
	init->keycard()->makeBackupImage();
#endif
}

void PwM::replayCardBackup_slot()
{
#ifdef CONFIG_KEYCARD
	init->keycard()->replayBackupImage();
#endif
}

void PwM::execLauncher_slot()
{
	PWM_ASSERT(curDoc());
	if (curDoc()->isDeepLocked())
		return;
	unsigned int curEntryIndex;
	if (!view->getCurEntryIndex(&curEntryIndex))
		return;
	bool ret = curDoc()->execLauncher(view->getCurrentCategory(),
					  curEntryIndex);
	if (ret)
		showStatMsg(i18n("Executed the \"Launcher\"."));
	else
		showStatMsg(i18n("ERROR: Couldn't execute the \"Launcher\"!"));
}

void PwM::goToURL_slot()
{
	PWM_ASSERT(curDoc());
	if (curDoc()->isDeepLocked())
		return;
	unsigned int curEntryIndex;
	if (!view->getCurEntryIndex(&curEntryIndex))
		return;
	bool ret = curDoc()->goToURL(view->getCurrentCategory(),
				     curEntryIndex);
	if (ret)
		showStatMsg(i18n("started browser with current URL."));
	else
		showStatMsg(i18n("ERROR: Couldn't start browser! Maybe invalid URL?"));
}

void PwM::copyToClipboard(const QString &s)
{
	QClipboard *cb = QApplication::clipboard();
#ifndef PWM_EMBEDDED
	if (cb->supportsSelection())
		cb->setText(s, QClipboard::Selection);
	cb->setText(s, QClipboard::Clipboard);
#else
	cb->setText(s);

#endif

}


void PwM::showStatMsg(const QString &msg)
{
#ifdef DESKTOP_VERSION
	statusBar()->message(msg, STATUSBAR_MSG_TIMEOUT * 1000);
#else
	//qDebug("Statusbar : %s",msg.latin1());
    Global::statusMessage(msg);
#endif
}

void PwM::focusInEvent(QFocusEvent *e)
{
	if (e->gotFocus()) {
		emit gotFocus(this);
	} else if (e->lostFocus()) {
		emit lostFocus(this);
	}
}


#ifdef PWM_EMBEDDED

void PwM::category_slot()
{
	PwMDoc *doc = curDoc();
	PWM_ASSERT(doc);
	doc->timer()->getLock(DocTimer::id_autoLockTimer);

	editCategoryWnd w(doc, this, "editcategory");
/*
	vector<string> catList;
	doc->getCategoryList(&catList);
	unsigned i, size = catList.size();
	for (i = 0; i < size; ++i) {
		w.addCategory(catList[i].c_str());
	}
	w.setCurrCategory(view->getCurrentCategory());
	if (pw)
		w.pwLineEdit->setText(*pw);
*/
	w.setCurrCategory(view->getCurrentCategory());

      tryAgain:
	if (w.exec() == 1) 
	  {
		PwMDataItem d;

		//US BUG: to initialize all values of curEntr with meaningfulldata,
		// we call clear on it. Reason: Metadata will be uninitialized otherwise.
		// another option would be to create a constructor for PwMDataItem
		d.clear(true);
		/*
		d.desc = w.getDescription().latin1();
		d.name = w.getUsername().latin1();
		d.pw = w.getPassword().latin1();
		d.comment = w.getComment().latin1();
		d.url = w.getUrl().latin1();
		d.launcher = w.getLauncher().latin1();
		PwMerror ret = doc->addEntry(w.getCategory(), &d);
		if (ret == e_entryExists) {
			KMessageBox::error(this,
					   i18n
					   ("An entry with this \"Description\",\n"
					    "does already exist.\n"
					    "Please select another description."),
					   i18n("entry already exists."));
			goto tryAgain;
		} else if (ret == e_maxAllowedEntr) {
			KMessageBox::error(this, i18n("The maximum possible number of\nentries"
						      "has been reached.\nYou can't add more entries."),
						 i18n("maximum number of entries"));
			doc->timer()->putLock(DocTimer::id_autoLockTimer);
			return;
		}
		*/
	}
	setVirgin(false);
	doc->timer()->putLock(DocTimer::id_autoLockTimer);
}


void PwM::whatsnew_slot()
{
    KApplication::showFile( "KDE-Pim/Pi Version Info", "kdepim/WhatsNew.txt" );
}

void PwM::showLicense_slot()
{
    KApplication::showLicence();
}

void PwM::faq_slot()
{
  KApplication::showFile( "PWM/Pi FAQ", "kdepim/pwmanager/pwmanagerFAQ.txt" );
}

void PwM::syncHowTo_slot()
{
    KApplication::showFile( "KDE-Pim/Pi Synchronization HowTo", "kdepim/SyncHowto.txt" );
}


void PwM::createAboutData_slot()
{
  QString version;
#include <../version>
;
    QMessageBox::about( this, "About PwManager/Pi",
                        "PwManager/Platform-independent\n"
                        "(PWM/Pi) " +version + " - " +
#ifdef DESKTOP_VERSION
                        "Desktop Edition\n"
#else
                        "PDA-Edition\n"
                        "for: Zaurus 5500 / 7x0 / 8x0\n"
#endif

                        "(c) 2004 Ulf Schenk\n"
                        "(c) 2004 Lutz Rogowski\n"
                        "(c) 1997-2004, The KDE PIM Team\n"

	"(c) Michael Buesch - main programming\nand current maintainer\nmbuesch@freenet.de\n"
	"Matt Scifo - mscifo@o1.com\n"
	"Elias Probst - elias.probst@gmx.de\n"
	"George Staikos - staikos@kde.org\n"
	"Matthew Palmer - mjp16@uow.edu.au\n"
        "Olivier Sessink - gpasman@nl.linux.org\n"
	"The libgcrypt developers -\nBlowfish and SHA1 algorithms\nftp://ftp.gnupg.org/gcrypt/alpha/libgcrypt/\n"
	"Troy Engel - tengel@sonic.net\n"
	"Wickey - wickey@gmx.at\n"
	"Ian MacGregor - original documentation author.\n"
 );
}


//this are the overwritten callbackmethods from the syncinterface
bool PwM::sync(KSyncManager* manager, QString filename, int mode,QString resource)
{
  PWM_ASSERT(curDoc());

  bool ret =  curDoc()->sync(manager, filename, mode,resource);

  qDebug("PwM::sync save now: ret=%i", ret);
 
  if (ret == true) {
    //US BUG: what can we call here to update the view of the current doc?
    //mViewManager->refreshView();

    //US curDoc()->sync sets the dirtyFlag in case the sync was successfull.
    save();
  }

  return ret;
}
 
void PwM::removeSyncInfo( QString syncProfile)
{
    qDebug("PWM:removeSyncInfo for profile %s ", syncProfile.latin1());
    curDoc()->removeSyncInfo( syncProfile );
    //US curDoc()->removeSyncInfo sets the dirtyFlag.
}

#endif


#ifndef PWM_EMBEDDED
#include "pwm.moc"
#endif
