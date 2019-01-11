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
 * $Id: pwgenwndimpl.h,v 1.2 2004/09/23 19:33:55 ulf69 Exp $
 **************************************************************************/  

#ifndef __PWGENWNDIMPL_H
#define __PWGENWNDIMPL_H

#ifndef PWM_EMBEDDED
#include "pwgenwnd.h"
#else
#include "pwgenwnd_emb.h"
#endif

class PwGenWndImpl : public pwGenWnd
{
public:
#ifndef PWM_EMBEDDED
	PwGenWndImpl(QWidget *parent = 0,
		     const char *name = 0,
		     bool modal = FALSE,
		     WFlags fl = 0);
#else
	PwGenWndImpl( QWidget* parent = 0, const char* name = 0);
#endif

	~PwGenWndImpl();

	/** returns the generated password */
	QString getPassword();

protected slots:
	/** generate button pressed */
	void genButton_slot();
#ifdef PWM_EMBEDDED
	virtual void slotOk();
#endif
	/** cancel button pressed */
	void cancelButton_slot();

protected:
	/** start the internal generator */
	bool startIntGen();
	/** check all options of the internal generator */
	bool optionsSanityIntGen();

protected:
	/** the generated password */
	QString password;
};

#endif // __PWGENWNDIMPL_H
