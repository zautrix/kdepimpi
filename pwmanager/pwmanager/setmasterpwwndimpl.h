/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
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
 * $Id: setmasterpwwndimpl.h,v 1.3 2004/10/20 08:28:44 zautrix Exp $
 **************************************************************************/  

#ifndef SETMASTERPWWNDIMPL_H
#define SETMASTERPWWNDIMPL_H

#ifndef PWM_EMBEDDED
#include "setmasterpwwnd.h"
#else
#include "setmasterpwwnd_emb.h"
#endif

#include <klocale.h>

#ifdef _WIN32_
#define uint32_t Q_UINT32
#else
#include <stdint.h>
#endif
#include <string>
using std::string;

class PwMKeyCard;

/** set master pw wnd */
class SetMasterPwWndImpl : public setMasterPwWnd
{
	Q_OBJECT
public:
	SetMasterPwWndImpl(QWidget* parent = 0, const char *name = 0);
	~SetMasterPwWndImpl();

	static QString string_cardNone()
			{ return i18n("NONE"); }

	/** returns the selected pw (or the key on the card) */
	string getPw(bool *useCard);
	/** set pointer to the keycard-access object */
	void setPwMKeyCard(PwMKeyCard *_keyCard)
			{ keyCard = _keyCard; }

public slots:
	/** ok button pressed */
	void okButton_slot();
	/** cancel button pressed */
	void cancelButton_slot();
	/** "generate a new card" button pressed */
	void genCardButton_slot();
	/** "select current card" button pressed */
	void selCardButton_slot();

#ifdef PWM_EMBEDDED
	virtual void slotOk();
#endif

protected slots:
	/** key from PwMKeyCard is available */
	void keyAvailable_slot(uint32_t cardId, const string &key);

protected:
	/** key of currently inserted card */
	string curCardKey;
	/** pointer to the keycard-access object */
	PwMKeyCard *keyCard;
};

#endif
