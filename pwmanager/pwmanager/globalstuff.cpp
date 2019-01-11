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
 * $Id: globalstuff.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $
 **************************************************************************/  

#include "globalstuff.h"

#include <kmessagebox.h>
#include <klocale.h>

#ifndef CONFIG_KEYCARD
void no_keycard_support_msg_box(QWidget *parentWidget)
{
	KMessageBox::error(parentWidget,
			   i18n("No Chipcard-support available!\n"
				"If you want to use a chipcard, "
				"you have to compile PwManager with the "
				"configure option --enable-keycard ."),
			   i18n("No Chipcard-support"));
}
#endif // CONFIG_KEYCARD
