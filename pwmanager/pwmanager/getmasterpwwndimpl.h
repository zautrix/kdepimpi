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
 * $Id: getmasterpwwndimpl.h,v 1.2 2004/09/24 19:51:43 ulf69 Exp $
 **************************************************************************/  

#ifndef GETMASTERPWWNDIMPL_H
#define GETMASTERPWWNDIMPL_H

#ifndef PWM_EMBEDDED
#include "getmasterpwwnd.h"
#else
#include "getmasterpwwnd_emb.h"
#endif

/** set master pw wnd */
class GetMasterPwWndImpl : public getMasterPwWnd
{
	Q_OBJECT
public:
	GetMasterPwWndImpl(QWidget* parent = 0, const char *name = 0);
	~GetMasterPwWndImpl();

public slots:
	/** ok button pressed */
	void okButton_slot();
	/** cancel button pressed */
	void cancelButton_slot();
#ifdef PWM_EMBEDDED
	virtual void slotOk();
#endif
};

#endif
