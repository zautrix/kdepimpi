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
 * $Id: pwmdocui.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef __PWMDOCUI_H
#define __PWMDOCUI_H

#include <qwidget.h>
#include <klocale.h>

/** string to display, if the pw-entry
  * is locked, in place of the password.
  */
#define LOCKED_STRING		PwMDocUi::string_locked()

/** The name of the "default" category */
#define DEFAULT_CATEGORY	PwMDocUi::string_defaultCategory()

/** deep-lock message to display in main-view, if it's locked */
#define IS_DEEPLOCKED_SHORTMSG	PwMDocUi::string_deepLockedShort()
#define IS_DEEPLOCKED_MSG	PwMDocUi::string_deepLockedLong()

/** default title. Display if the doc has no title, yet */
#define DEFAULT_TITLE		PwMDocUi::string_defaultTitle()

class PwMKeyCard;
class PwMDoc;

/** User-interface layer for PwMDoc.
  * This class handles for example all Master-password related things (asking
  * the user for the password od the chipcard) and some
  * other UI-things, that PwMDoc is not allowed to
  * handle. (PwMDoc is _NOT_ allowed to handle anything
  * UI related.)
  */
class PwMDocUi : public QObject
{
	Q_OBJECT
public:
	PwMDocUi(QObject* parent = 0, const char *name = 0);
	~PwMDocUi();

	/** when a new view is activated, call this to
	  * inform the Master-password-layer about this.
	  */
	void setCurrentView(QWidget *view)
			{ currentView = view; }
	/** return the currentView pointer */
	QWidget* getCurrentView()
			{ return currentView; }
	/** set pointer to the keycard-access object */
	void setPwMKeyCard(PwMKeyCard *_keyCard)
			{ keyCard = _keyCard; }
	/** saves the document (with user dialog) */
	bool saveDocUi(PwMDoc *doc);
	/** save as (with user dialog) */
	bool saveAsDocUi(PwMDoc *doc);
	/** opens a new document (with user dialog) */
	bool openDocUi(PwMDoc *doc,
		       QString filename = QString::null,
		       bool openDeepLocked = false);

	static QString string_defaultCategory();
	static QString string_locked();
	static QString string_deepLockedShort();
	static QString string_deepLockedLong();
	static QString string_defaultTitle();

protected:
	/** request the user to enter the master-password or
	  * to insert his chipcard.
	  * If forcePw is true, it will always ask for a pw
	  * and never for a chipcard.
	  */
	QString requestMpw(bool chipcard);
	/** request the user to enter a new master-password of
	  * to create a new chipcard.
	  * It "chipcard" is a NULL pointer, chipcards will not be
	  * allowed.
	  */
	QString requestNewMpw(bool *chipcard);
	/** request a master-pw change.(or chipcard change).
	  * This function asks the user and requests the pw
	  */
	QString requestMpwChange(const QString *currentPw, bool *chipcard);
	/** informs the user, that he entered the wrong mpw.
	  * It opens a message-box
	  */
	void wrongMpwMsgBox(bool chipcard,
			    QString prefix = "",
			    QString postfix = "");
	/** informs the user, that the mpw is needed, but he
	  * didn't enter one
	  */
	void noMpwMsgBox(bool chipcard,
			 QString prefix = "",
			 QString postfix = "");
	/** display "feature not availavle while UID 0" msg box */
	void rootAlertMsgBox();
	/** display "can't deeplock, because not saved yet" msg box */
	void cantDeeplock_notSavedMsgBox();
	/** gpasman-export password length error */
	void gpmPwLenErrMsgBox();
	/** ask for saving */
	int dirtyAskSave(const QString &docTitle);

private:
	/** the currently active view */
	QWidget *currentView;
	/** pointer to the keycard-access object */
	PwMKeyCard *keyCard;
};

#endif
