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
 * $Id: pwmprint.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef PWMPRINT_H
#define PWMPRINT_H

#include <kconfig.h>

#include <qwidget.h>
#include <qstringlist.h>

class PwMDoc;

class PwMPrint
{
public:
	PwMPrint(PwMDoc *_doc, QWidget *_parent);
	~PwMPrint();

	/** start printing now */
	void printNow();
	/** set document */
	void setDoc(PwMDoc *_doc)
			{ doc = _doc; }

protected:
	/** generate a date-string */
	QString genDateString();
	/** request the user to select all categories to print */
	QStringList selCategories();

protected:
	/** document */
	PwMDoc *doc;
	/** parent widget */
	QWidget *parent;
};

#endif
