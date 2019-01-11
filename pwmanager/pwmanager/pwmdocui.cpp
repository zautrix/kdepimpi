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
 * $Id: pwmdocui.cpp,v 1.11 2004/10/25 22:24:48 zautrix Exp $
 **************************************************************************/  

#include "pwmdocui.h"
#include "setmasterpwwndimpl.h"
#include "getmasterpwwndimpl.h"
#include "pwmexception.h"
#include "getkeycardwnd.h"
#include "pwm.h"
#include "globalstuff.h"
#include "spinforsignal.h"

#include <qlineedit.h>
#include <qtabwidget.h>

#include <kmessagebox.h>
#include <kfiledialog.h>

#ifndef PWM_EMBEDDED
#include <kwin.h>
#else
#include <qdir.h>
#include "pwmprefs.h"
#endif


#ifdef CONFIG_KEYCARD
# include "pwmkeycard.h"
#endif


PwMDocUi::PwMDocUi(QObject *parent, const char *name)
 : QObject(parent, name)
{
	currentView = 0;
	keyCard = 0;
}

PwMDocUi::~PwMDocUi()
{
}

QString PwMDocUi::requestMpw(bool chipcard)
{
	QString pw;

	if (chipcard) {
#ifdef CONFIG_KEYCARD
		PWM_ASSERT(keyCard);
		uint32_t id;
		string ret;
		SpinForSignal *spinner = keyCard->getSpinner();
		connect(keyCard, SIGNAL(keyAvailable(uint32_t, const string &)),
			spinner, SLOT(u32_str_slot(uint32_t, const string &)));
		keyCard->getKey();
		spinner->spin(&id, &ret);
		disconnect(keyCard, SIGNAL(keyAvailable(uint32_t, const string &)),
			   spinner, SLOT(u32_str_slot(uint32_t, const string &)));
		if (ret == "")
			return "";
		pw = ret.c_str();
#else // CONFIG_KEYCARD
		no_keycard_support_msg_box(currentView);
#endif // CONFIG_KEYCARD
	} else {
#ifndef PWM_EMBEDDED
		GetMasterPwWndImpl pwWnd;
		KWin::setState(pwWnd.winId(), NET::StaysOnTop);
#else
		GetMasterPwWndImpl pwWnd;
#endif
		if (pwWnd.exec() != 1)
			return "";
		pw = pwWnd.pwLineEdit->text();
	}

	return pw;
}

QString PwMDocUi::requestNewMpw(bool *chipcard)
{
	QString pw;
	SetMasterPwWndImpl pwWnd(currentView);
	pwWnd.setPwMKeyCard(keyCard);
	if (!chipcard) {
#ifndef PWM_EMBEDDED
		pwWnd.mainTab->removePage(pwWnd.mainTab->page(1));
#else
		pwWnd.mainTab->removePage(pwWnd.tab_2);
#endif
	}

	if (pwWnd.exec() != 1)
		return "";
	pw = pwWnd.getPw(chipcard).c_str();

	return pw;
}

QString PwMDocUi::requestMpwChange(const QString *currentPw, bool *chipcard)
{
	QString pw(requestMpw(*chipcard));
	if (pw == "")
		return "";
	if (pw != *currentPw) {
		wrongMpwMsgBox(*chipcard);
		return "";
	}

	pw = requestNewMpw(chipcard);
	if (pw == "")
		return "";
	return pw;
}

void PwMDocUi::wrongMpwMsgBox(bool chipcard, QString prefix, QString postfix)
{
	QString msg;
	if (prefix != "") {
		msg += prefix;
		msg += "\n";
	}

	if (chipcard) {
		msg += i18n("Wrong key-card!\n"
			    "Please try again with the\n"
			    "correct key-card.");
	} else {
		msg += i18n("Wrong master-password!\n"
			    "Please try again.");
	}

	if (postfix != "") {
		msg += "\n";
		msg += postfix;
	}
	KMessageBox::error(currentView, msg,
			   (chipcard) ? (i18n("wrong chipcard"))
			   	      : (i18n("password error")));
}

void PwMDocUi::noMpwMsgBox(bool chipcard, QString prefix, QString postfix)
{
	QString msg;
	if (prefix != "") {
		msg += prefix;
		msg += "\n";
	}

	if (chipcard) {
		msg += i18n("No key-card found!\n"
			    "Please insert the\n"
			    "correct key-card.");
	} else {
		msg += i18n("No master-password given!");
	}

	if (postfix != "") {
		msg += "\n";
		msg += postfix;
	}
	KMessageBox::error(currentView, msg,
			   (chipcard) ? (i18n("no chipcard"))
			   	      : (i18n("password error")));
}

void PwMDocUi::rootAlertMsgBox()
{
	KMessageBox::error(currentView,
			   i18n("This feature is not available,n"
				"if you execute PwM with \"root\" \n"
				"UID 0 privileges, for security reasons!"),
			   i18n("not allowed as root!"));
}

void PwMDocUi::cantDeeplock_notSavedMsgBox()
{
	KMessageBox::error(currentView,
			   i18n("Can't deep-lock, because the document\n"
				"hasn't been saved, yet. Please save\n"
				"to a file and try again."),
			   i18n("not saved, yet"));
}

void PwMDocUi::gpmPwLenErrMsgBox()
{
	KMessageBox::error(currentView,
			   i18n("GPasman does not support passwords\n"
				"shorter than 4 characters! Please try\n"
				"again with a longer password."),
			   i18n("Password too short"));
}

int PwMDocUi::dirtyAskSave(const QString &docTitle)
{
	int ret;
#ifndef PWM_EMBEDDED
	ret = KMessageBox::questionYesNoCancel(currentView,
					       i18n("The list\n\"") +
					       docTitle +
					       i18n
					       ("\" has been modified.\n"
					        "Do you want to save it?"),
					       i18n("Save?"));
	if (ret == KMessageBox::Yes) {
		return 0;
	} else if (ret == KMessageBox::No) {
		return 1;
	}
#else
    QString doc = docTitle;
    if ( doc.length() > 33 )
        doc = "..." + doc.right(30);
	ret = KMessageBox::warningYesNoCancel(currentView,
					       i18n("The list\n \"") +
					       doc +
					       i18n
					       ("\"\nhas been modified.\n"
					        "Do you want to save it?"),
					       i18n("Save?"));
	if (ret == KMessageBox::Yes) {
		return 0;
	} else if (ret == KMessageBox::No) {
		return 1;
	}

#endif

	// cancel
	return -1;
}

bool PwMDocUi::saveDocUi(PwMDoc *doc)
{
	PWM_ASSERT(doc);
	doc->timer()->getLock(DocTimer::id_autoLockTimer);
	if (doc->isDocEmpty()) {
		KMessageBox::information(currentView,
					 i18n
					 ("Sorry, there's nothing to save.\n"
					  "Please first add some passwords."),
					 i18n("nothing to do"));
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return true;
	}
	PwMerror ret = doc->saveDoc(conf()->confGlobCompression());
	if (ret == e_filename) {
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return saveAsDocUi(doc);
	} else if (ret == e_weakPw) {
		KMessageBox::error(currentView,
				   i18n("Error: This is a weak password.\n"
					"Please select another password."),
				   i18n("weak password"));
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return false;
	} else if (ret == e_fileBackup) {
		KMessageBox::error(currentView,
				   i18n("Error: Couldn't make backup-file!"),
				   i18n("backup failed"));
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return false;
	} else if (ret == e_noPw ||
		   ret == e_wrongPw ||
		   ret == e_openFile) {
	            doc->timer()->putLock(DocTimer::id_autoLockTimer);
		    return false;
	} else if (ret != e_success) {
		KMessageBox::error(currentView,
				   i18n("Error: Couldn't write to file.\n"
					"Please check if you have permission to\n"
					"write to the file in that directory."),
				   i18n("error while writing"));
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return false;
	}
	doc->timer()->putLock(DocTimer::id_autoLockTimer);
	return true;
}

bool PwMDocUi::saveAsDocUi(PwMDoc *doc)
{
	PWM_ASSERT(doc);
	doc->timer()->getLock(DocTimer::id_autoLockTimer);
	if (doc->isDocEmpty()) {
		KMessageBox::information(currentView,
					 i18n
					 ("Sorry, there's nothing to save.\n"
					  "Please first add some passwords."),
					 i18n("nothing to do"));
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return true;
	}
#ifndef PWM_EMBEDDED
	QString fn(KFileDialog::getSaveFileName(QString::null,
						i18n("*.pwm|PwManager Password file"),
						currentView));
#else
        QString fn = locateLocal( "data", KGlobal::getAppName() + "/*.pwm" );
	fn = KFileDialog::getSaveFileName(fn,
						i18n("password filename(*.pwm)"),
						currentView);

#endif
	if (fn == "") {
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return false;
	}
	if (fn.right(4) != ".pwm")
		fn += ".pwm";

	PwMerror ret = doc->saveDoc(conf()->confGlobCompression(), &fn);
	if (ret == e_noPw ||
	    ret == e_wrongPw ||
	    ret == e_openFile) {
	  doc->timer()->putLock(DocTimer::id_autoLockTimer);
	  return false;
	} else if (ret != e_success) {
		KMessageBox::error(currentView,
				   i18n("Error: Couldn't write to file.\n"
					"Please check if you have permission to\n"
					"write to the file in that directory."),
				   i18n("error while writing"));
		doc->timer()->putLock(DocTimer::id_autoLockTimer);
		return false;
	}
	doc->timer()->putLock(DocTimer::id_autoLockTimer);
	return true;
}

bool PwMDocUi::openDocUi(PwMDoc *doc,
			 QString filename,
			 bool openDeepLocked)
{
	if (filename.isEmpty())
	  {
#ifndef PWM_EMBEDDED
		filename = KFileDialog::getOpenFileName(QString::null,
							i18n("*.pwm|PwManager Password file\n"
							     "*|All files"), getCurrentView());
#else
		filename = locateLocal( "data", KGlobal::getAppName() + "/*.pwm");
		filename = KFileDialog::getOpenFileName(filename,
							i18n("password filename(*.pwm)"), getCurrentView());
#endif
	  }
	if (filename.isEmpty())
		goto cancelOpen;
	PwMerror ret;
	while (true) {
		int lockStat = -1;
		if (openDeepLocked) {
			lockStat = 2;
		} else {
			if (conf()->confGlobUnlockOnOpen()) {
				lockStat = 0;
			} else {
				lockStat = 1;
			}
		}
		ret = doc->openDoc(&filename, lockStat);
		//qDebug("pwmdocui::OpenDocui %i", ret);		
		if (ret != e_success) {
			if (ret == e_readFile || ret == e_openFile) {
				KMessageBox::error(getCurrentView(),
						   i18n("Could not read file!")
						   + "\n"
						   + filename,
						   i18n("File error"));
				goto cancelOpen;
			}
			if (ret == e_alreadyOpen) {
				KMessageBox::error(getCurrentView(),
						   i18n("This file is already open."),
						   i18n("Already open"));
				goto cancelOpen;
			}
			if (ret == e_fileVer) {
				KMessageBox::error(getCurrentView(),
						   i18n
						   ("File-version is not supported!\n"
						    "Did you create this file with an\nolder or newer version of PwM?"),
						   i18n
						   ("Incompatible version"));
				goto cancelOpen;
			}
			if (ret == e_wrongPw) {
				continue;
			}
			if (ret == e_noPw) {
				goto cancelOpen;
			}
			if (ret == e_fileFormat) {
				KMessageBox::error(getCurrentView(),
						   i18n
						   ("Sorry, this file has not been recognized\n"
						    "as a PwM Password file.\n"
						    "Probably you have selected the wrong file."),
						   i18n
						   ("No PwM password-file"));
				goto cancelOpen;
			}
			if (ret == e_fileCorrupt) {
				KMessageBox::error(getCurrentView(),
						   i18n
						   ("File corrupt!\n"
						    "Maybe the media, you stored this file on,\n"
						    "had bad sectors?"),
						   i18n
						   ("Checksum error"));
				goto cancelOpen;
			}
		}
		break;
	}
	return true;

      cancelOpen:
	return false;
}

QString PwMDocUi::string_defaultCategory()
{
	return i18n("Default");
}

QString PwMDocUi::string_locked()
{
	return i18n("<LOCKED>");
}

QString PwMDocUi::string_deepLockedShort()
{
	return i18n("DEEP-LOCKED");
}

QString PwMDocUi::string_deepLockedLong()
{
	return i18n("This file is DEEP-LOCKED!\n"
		    "That means all data has been encrypted\n"
		    "and written out to the file. If you want\n"
		    "to see the entries, please UNLOCK the file.\n"
		    "While unlocking, you will be prompted for the\n"
		    "master-password or the key-card.");
}

QString PwMDocUi::string_defaultTitle()
{
	return i18n("Untitled");
}

#ifndef PWM_EMBEDDED
#include "pwmdocui.moc"
#endif
