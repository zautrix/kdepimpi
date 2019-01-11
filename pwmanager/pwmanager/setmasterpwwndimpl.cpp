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
 * $Id: setmasterpwwndimpl.cpp,v 1.2 2004/09/24 19:51:43 ulf69 Exp $
 **************************************************************************/  

#include "setmasterpwwndimpl.h"
#include "pwm.h"
#include "globalstuff.h"

#include <kmessagebox.h>

#include <qlineedit.h>
#include <qlabel.h>
#include <qtabwidget.h>

#include <stdio.h>

#ifdef CONFIG_KEYCARD
# include "pwmkeycard.h"
#endif

#define STRING_CARD_NONE	(SetMasterPwWndImpl::string_cardNone())


SetMasterPwWndImpl::SetMasterPwWndImpl(QWidget * parent, const char *name)
:  setMasterPwWnd(parent, name)
{
#ifdef CONFIG_KEYCARD
	curCardIdLabel->setText(STRING_CARD_NONE);
#else // CONFIG_KEYCARD
#ifndef PWM_EMBEDDED
	mainTab->removePage(mainTab->page(1));
#else
	mainTab->removePage(tab_2);
#endif

#endif // CONFIG_KEYCARD
	keyCard = 0;
}

SetMasterPwWndImpl::~SetMasterPwWndImpl()
{
}

void SetMasterPwWndImpl::okButton_slot()
{
#ifndef PWM_EMBEDDED
	int index = mainTab->currentPageIndex();
	if (index == 0) {
		// normal password
		if (pwEdit_1->text() != pwEdit_2->text()) {
			KMessageBox::error(this,
					   i18n
					   ("The two passwords you have entered don't match.\n"
					    "Please try entering them again."),
					   i18n("Different passwords"));
			return;
		}
		if (pwEdit_1->text() == "") {
			KMessageBox::error(this,
					   i18n("No password entered. "
						"Please type in a password, that "
						"you want to use for the encryption."),
					   i18n("no password"));
			return;
		}
	} else {
		// key-card
		if (curCardIdLabel->text() == STRING_CARD_NONE) {
			KMessageBox::error(this,
					   i18n("You didn't select a card as "
						"PwM-key-card."),
					   i18n("no card"));
			return;
		}
	}
	done(1);
#endif
}

void SetMasterPwWndImpl::cancelButton_slot()
{
#ifndef PWM_EMBEDDED
	done(2);
#endif
}

void SetMasterPwWndImpl::slotOk()
{
	int index = mainTab->currentPageIndex();
	if (index == 0) {
		// normal password
		if (pwEdit_1->text() != pwEdit_2->text()) {
			KMessageBox::error(this,
					   i18n
					   ("The two passwords you have entered\ndon't match.\n"
					    "Please try entering them again."),
					   i18n("Different passwords"));
			return;
		}
		if (pwEdit_1->text() == "") {
			KMessageBox::error(this,
					   i18n("No password entered.\n"
						"Please type in a password,\nthat "
						"you want to use for\nthe encryption."),
					   i18n("no password"));
			return;
		}
	} else {
		// key-card
		if (curCardIdLabel->text() == STRING_CARD_NONE) {
			KMessageBox::error(this,
					   i18n("You didn't select a card as\n"
						"PwM-key-card."),
					   i18n("no card"));
			return;
		}
	}

	setMasterPwWnd::slotOk();
}


void SetMasterPwWndImpl::genCardButton_slot()
{
#ifdef CONFIG_KEYCARD
	PWM_ASSERT(keyCard);
	keyCard->genNewCard();
#endif // CONFIG_KEYCARD
}

void SetMasterPwWndImpl::selCardButton_slot()
{
#ifdef CONFIG_KEYCARD
	PWM_ASSERT(keyCard);
	connect(keyCard, SIGNAL(keyAvailable(uint32_t, const string &)),
		this, SLOT(keyAvailable_slot(uint32_t, const string &)));
	keyCard->getKey();
#endif // CONFIG_KEYCARD
}

void SetMasterPwWndImpl::keyAvailable_slot(uint32_t cardId,
					   const string &key)
{
	if (key == "")
		return;
	curCardKey = key;
	char id_buf[(sizeof(cardId) * 2) + 2 /* "0x" */ + 1 /* NULL */];
	memcpy(id_buf, "0x", 2);
	sprintf(id_buf + 2, "%X", cardId);
	curCardIdLabel->setText(id_buf);
}

string SetMasterPwWndImpl::getPw(bool *useCard)
{
	int index = mainTab->currentPageIndex();
	if (index == 0) {
		// normal password
		if (useCard)
			*useCard = false;
		PWM_ASSERT(pwEdit_1->text() == pwEdit_2->text());
		return pwEdit_1->text().latin1();
	} else {
#ifdef CONFIG_KEYCARD
		// key-card
		if (useCard)
			*useCard = true;
		PWM_ASSERT(curCardKey != "");
		PWM_ASSERT(curCardIdLabel->text() != STRING_CARD_NONE);
		return curCardKey;
#endif // CONFIG_KEYCARD
	}
	return "";
}

#ifndef PWM_EMBEDDED
#include "setmasterpwwndimpl.moc"
#endif
