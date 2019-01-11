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
 * $Id: pwm.h,v 1.13 2005/11/28 05:53:55 zautrix Exp $
 **************************************************************************/  

#ifndef __PWM_H
#define __PWM_H


#include <kpopupmenu.h>
#include <klistview.h>
#include <kmainwindow.h>

#ifndef PWM_EMBEDDED
#include <kwin.h>
#include <kapp.h>
#include <kdeversion.h>
#else
#include <ksyncmanager.h>
#endif

#include <kaction.h>

#include <qglobal.h>

#include "pwmview.h"
#include "pwmexception.h"


/** timeout for displaying a message on the status-bar (in seconds) */
#define STATUSBAR_MSG_TIMEOUT	5


class PwMInit;
class KSyncManager;

/** PwM is the base class of the project */
#ifndef PWM_EMBEDDED
//MOC_SKIP_BEGIN
class PwM : public KMainWindow
//MOC_SKIP_END
#else
class PwM : public KMainWindow, public KSyncInterface
#endif
{
	Q_OBJECT
public:
	friend class PwMView;
	/** construtor */
	PwM(PwMInit *_init, PwMDoc *doc,
	    bool virginity = true,
	    QWidget* parent = 0, const char *name = 0);
	/** destructor */
	~PwM();

	/** copy some text to the global clipboard */
	static void copyToClipboard(const QString &s);

	/** returns pointer to the view */
	PwMView * curView()
			{ return view; }
	/** returns pointer to the currently using document. */
	PwMDoc * curDoc()
			{ return curView()->document(); }
	/** open a new doc with the given filename */
	PwMDoc * openDoc(QString filename, bool openDeepLocked = false);
	/** show a message on the global status bar.
	  * The message times out after some seconds.
	  */
	void showStatMsg(const QString &msg);
	/** ask the user where to save the doc (if it has not been saved, yet)
	  * and write the data to disk.
	  */
	bool save();
	/** ask the user where to save the doc
	  * and write the data to disk.
	  */
	bool saveAs();
	/** force quit. Quit this window, always! Don't minimize it */
	bool isForceQuit()
			{ return forceQuit; }
	/** set forceQuit */
	void setForceQuit(bool force)
			{ forceQuit = force; }
	/** force minimize this window */
	bool isForceMinimizeToTray()
			{ return forceMinimizeToTray; }
	/** set forceMinimizeToTray */
	void setForceMinimizeToTray(bool force)
			{ forceMinimizeToTray = force; }

public slots:
	/** file/new triggered */
	void new_slot();
	/** file/open triggered */
//US ENH
	void open_slot();
	void open_slot(QString fn);
	/** file/close triggered */
	void close_slot();
	/** file/quit triggered */
	void quitButton_slot();
	/** file/save triggered */
	void save_slot();
	/** file/saveAs triggered */
	void saveAs_slot();
	/** file/export/text triggered */
	void exportToText();
	/** file/export/gpasman triggered */
	void exportToGpasman();
	/** file/export/kwallet triggered */
	void exportToKWallet();
	/** file/export/csv triggered */
	void exportToCsv();
	/** file/import/text triggered */
	bool importFromText();
	/** file/import/gpasman triggered */
	bool importFromGpasman();
	/** file/import/kwallet triggered */
	bool importKWallet();
	/** file/import/csv triggered */
	bool importCsv();
	/** file/print triggered */
	void print_slot();
	/** manage/add triggered */
	//US ENH : changed code to run with older MOC

	void addPwd_slot();
	void addPwd_slot1(QString *pw, PwMDoc *_doc);
	/** manage/edit triggered */
	//US ENH : changed code to run with older MOC
	void editPwd_slot();
	void editPwd_slot1(const QString *category);
	void editPwd_slot3(const QString *category, const int *index ,PwMDoc *_doc );

	/** manage/delete triggered */
	void deletePwd_slot();
	/** execute the "Launcher" entry */
	void execLauncher_slot();
	/** open browser with URL entry */
	void goToURL_slot();
	/** manage/changeMasterPwd triggered */
	void changeMasterPwd_slot();
	/** lock current document */
	void lockWnd_slot();
	/** deeplock current document */
	void deepLockWnd_slot();
	/** window/unlock triggered */
	void unlockWnd_slot();
	/** find item */
	void find_slot();
	/** configure clicked */
	void config_slot();
	void config_global_slot();
	/** (de)activate the "change master pw" button in the menu-bar */
	void activateMpButton(bool activate = true);
	/** generate a new chipcard */
	void genNewCard_slot();
	/** completely erase the current card */
	void eraseCard_slot();
	/** returns the ID number of the current card */
	void readCardId_slot();
	/** make backup image of the current card */
	void makeCardBackup_slot();
	/** write backup image to current card */
	void replayCardBackup_slot();

#ifdef PWM_EMBEDDED
	void category_slot();
	void whatsnew_slot();
	void showLicense_slot();
	void faq_slot();
	void createAboutData_slot();
	void syncHowTo_slot();
#endif

protected:
	/** is this window virgin? */
	bool isVirgin()
			{ return virgin; }
	/** add/remove virginity */
	void setVirgin(bool v);
	/** initialize the menubar */
	void initMenubar();
	/** initialize the toolbar */
	void initToolbar();
	/** initialize the window-metrics */
	void initMetrics();
	/** close-event */
	void closeEvent(QCloseEvent *e);
	/** creates a new PwM-ListView and returns it */
	PwMView * makeNewListView(PwMDoc *doc);
	/** Window hide-event */
	void hideEvent(QHideEvent *);
	/** is this window minimized? */
	bool isMinimized()
		{
#ifndef PWM_EMBEDDED
		  #if KDE_VERSION >= KDE_MAKE_VERSION(3, 2, 0)
			return KWin::windowInfo(winId()).isMinimized();
		  #else // KDE_VERSION
			return KWin::info(winId()).isIconified();
		  #endif // KDE_VERSION
#else
			return false;
#endif
		}
	/** window got the focus */
	void focusInEvent(QFocusEvent *e);
	/** update the caption string */
	void updateCaption();
#ifdef CONFIG_KWALLETIF
	/** check if kwalletemu is enabled and ask the user what to do */
	bool checkAndAskForKWalletEmu();
#endif // CONFIG_KWALLETIF

protected slots:
	/** doc got closed */
	void docClosed(PwMDoc *doc);

signals:
	/** window got closed (by user or someone else) */
	void closed(PwM *wnd);
	/** window got the focus (was brought to foreground) */
	void gotFocus(PwM *wnd);
	/** window lost the focus */
	void lostFocus(PwM *wnd);

protected:
	/** pointer to the view active in this KMainWindow */
	PwMView *view;
	/** pointer to the init class */
	PwMInit *init;
	/** has this window already lost its virginity?
	  * Means is there an open working document
	  */
	bool virgin;
	/** "file" popup-menu */
	KPopupMenu *filePopup;

	/** "manage" popup-menu */
	KPopupMenu *managePopup;
#ifdef CONFIG_KEYCARD
	/** "chipcard" popup-menu */
	KPopupMenu *chipcardPopup;
#endif // CONFIG_KEYCARD
	/** "view" popup-menu */
	KPopupMenu *viewPopup;
	/** "options" popup-menu */
	KPopupMenu *optionsPopup;
	/** "help" popup-menu */
	KPopupMenu *helpPopup;
	/** "export" popup-menu */
	KPopupMenu *exportPopup;
	/** "import" popup-menu */
	KPopupMenu *importPopup;
	/** force quit this window? */
	bool forceQuit;
	/** force minimize this window to the tray */
	bool forceMinimizeToTray;




 private:
#ifdef PWM_EMBEDDED
    //this are the overwritten callbackmethods from the syncinterface
    virtual bool sync(KSyncManager* manager, QString filename, int mode,QString resource); 
    virtual void removeSyncInfo( QString syncProfile);

    // LR *******************************
    // sync stuff!
    QPopupMenu *syncPopup;
    KSyncManager* syncManager;
#endif








};

#endif
