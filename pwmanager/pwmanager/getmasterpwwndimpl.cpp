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
 * $Id: getmasterpwwndimpl.cpp,v 1.2 2004/09/24 19:51:43 ulf69 Exp $
 **************************************************************************/  

#include "getmasterpwwndimpl.h"

#include <qlineedit.h>

GetMasterPwWndImpl::GetMasterPwWndImpl(QWidget * parent, const char *name)
: getMasterPwWnd(parent, name)
{
}

GetMasterPwWndImpl::~GetMasterPwWndImpl()
{
}

void GetMasterPwWndImpl::okButton_slot()
{
#ifndef PWM_EMBEDDED 
	if (pwLineEdit->text() != "")
		done(1);
#endif
}

void GetMasterPwWndImpl::cancelButton_slot()
{
#ifndef PWM_EMBEDDED
	done(2);
#endif
}

#ifdef PWM_EMBEDDED
void GetMasterPwWndImpl::slotOk()
{
	if (pwLineEdit->text() != "")
	  getMasterPwWnd::slotOk();
	else
	  return;
}
#endif

#ifndef PWM_EMBEDDED
#include "getmasterpwwndimpl.moc"
#endif
