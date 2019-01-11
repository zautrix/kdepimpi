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
 * This file is originaly based on version 1.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 * The original file version was 1.2
 * $Id: csv.h,v 1.1 2004/10/22 18:48:35 ulf69 Exp $
 **************************************************************************/  


#ifndef __PWMANAGER_CSV_H
#define __PWMANAGER_CSV_H

#include <qcstring.h>
#include <qfile.h>


class PwMDoc;
class QString;
class QWidget;

/** class for CSV (Comma Separated Value) export and import.
  *
  * http://www.creativyst.com/Doc/Articles/CSV/CSV01.htm
  * http://en.wikipedia.org/wiki/Comma-separated_values
  *
  * There are two types of CSV output we can produce.
  * One with Category support (recommended):
  *     "Category 1",, "Desc 1", "Username 1", "Password 1", "URL 1", "Launcher 1", "Comment 1"
  *     "Category 1",, "Desc 2", "Username 2", "Password 2", "URL 2", "Launcher 2", "Comment 2"
  *     ...
  * The empty "" is neccessary, because in future versions we will
  * support nested Categories. We want to be future compatible, now.
  *
  * And one without Category support:
  *FIXME: it's not implemented, yet. ;)
  *     "Desc 1", "Username 1", "Password 1", "URL 1", "Launcher 1", "Comment 1"
  *     "Desc 2", "Username 2", "Password 2", "URL 2", "Launcher 2", "Comment 2"
  *     ...
  *
  */
class Csv
{
public:
	Csv(QWidget *_parent);
	~Csv();

	/** import data from "filepath" into "doc" */
	bool importData(const QString &filepath,
			PwMDoc *doc);
	/** export data from "doc" to "filepath" */
	bool exportData(const QString &filepath,
			PwMDoc *doc);

protected:
	/** do the import process. */
	bool doImport(const QByteArray &d,
		      PwMDoc *doc);
	/** parse for the next field.
	  * @return Return values are:
	  *         0  -> successfully got the next field.
	  *         1  -> record end.
	  *         2  -> data end.
	  *         -1 -> parser error.
	  */
	int nextField(QCString *ret,
		      const QByteArray &in,
		      bool inRecord,
		      int *_refIndex);
	/** do the export process. */
	bool doExport(QFile &f,
		      PwMDoc *doc);
	/** generate a new data field string. */
	QCString newField(QCString s);

protected:
	/** current parent widget. */
	QWidget *parent;
};

#endif // __PWMANAGER_CSV_H
