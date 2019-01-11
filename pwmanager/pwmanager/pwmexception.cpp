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
 * $Id: pwmexception.cpp,v 1.2 2004/09/24 22:21:25 ulf69 Exp $
 **************************************************************************/  


#include "pwmexception.h"

#include <kmessagebox.h>


void pwmFatal(const char *id,
	      const char *file,
	      int line)
{
	cerr << PROG_NAME " " << id << " at "
	     << file << ":" << line
	     << endl;
}

void __printError(const string &msg)
{
	QString __msg(PROG_NAME " generated a fatal fault:\n");
	__msg += msg.c_str();
	cerr << "\n\n" << __msg.latin1() << endl;
	KMessageBox::error(0, __msg, PROG_NAME " fatal ERROR!");
}

void __printInfo(const string &msg)
{
	cout << PROG_NAME " INFO: " << msg << endl;
#ifdef PWM_DEBUG
	qDebug("%s INFO: %s", PROG_NAME, msg.c_str()) ;
#endif
}

void __printWarn(const string &msg)
{
	cerr << PROG_NAME " WARNING: " << msg << endl;
#ifdef PWM_DEBUG
	qDebug("%s WARNING: %s", PROG_NAME, msg.c_str()) ;
#endif
}

#ifdef PWM_DEBUG
void __printDebug(const string &msg)
{
	cout << PROG_NAME " DEBUG: " << msg << endl;
	qDebug("%s DEBUG: %s", PROG_NAME, msg.c_str()) ;
}
#endif // PWM_DEBUG
