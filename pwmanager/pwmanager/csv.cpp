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
 * $Id: csv.cpp,v 1.1 2004/10/22 18:48:35 ulf69 Exp $
 **************************************************************************/  

#include "csv.h"
#include "pwmdoc.h"
#include "pwmexception.h"

#include <kmessagebox.h>
#include <klocale.h>

#define MAX_CSV_FILE_SIZE	(50 * 1024 * 1024) // bytes


Csv::Csv(QWidget *_parent)
 : parent (_parent)
{
}

Csv::~Csv()
{
}

bool Csv::importData(const QString &filepath,
		     PwMDoc *doc)
{
	bool ret = true;
	QByteArray d;
	QFile f(filepath);
	if (!f.open(IO_ReadOnly)) {
		KMessageBox::error(parent,
				   i18n("Could not open file.\n"
					"Does the file exist?"),
				   i18n("Open error."));
		ret = false;
		goto out;
	}
	if (f.size() > MAX_CSV_FILE_SIZE) {
		KMessageBox::error(parent,
				   i18n("File too big.\nMaximum file size is 1 Byte.", "File too big.\nMaximum file size is %n Bytes.", MAX_CSV_FILE_SIZE),
				   i18n("File too big."));
		ret = false;
		goto out_close;
	}
	d = f.readAll();
	if (d.isEmpty()) {
		KMessageBox::error(parent,
				   i18n("Could not read file or file empty."),
				   i18n("Reading failed."));
		ret = false;
		goto out_close;
	}
	if (!doImport(d, doc)) {
		KMessageBox::error(parent,
				   i18n("Import failed.\n"
					"Corrupt CSV data format."),
				   i18n("File corrupt."));
		ret = false;
		goto out_close;
	}

out_close:
	f.close();
out:
	return ret;
}

bool Csv::doImport(const QByteArray &d,
		   PwMDoc *doc)
{
	PwMDataItem di;
	//US ENH: initialize all members:
	di.clear();

	int refIndex = 0;
	int ret;
	QCString s, curCat;
	int fieldIndex = 0;
	bool inRecord = false;
	/* fieldIndex is a reference count to see which
	 * value we are attaching to di.
	 * Valid counts are:
	 * 0 -> category
	 * 1 -> desc
	 * 2 -> name
	 * 3 -> pw
	 * 4 -> url
	 * 5 -> launcher
	 * 6 -> comment
	 */

	while (1) {
		ret = nextField(&s, d, inRecord, &refIndex);
		switch (ret) {
		case 0:
			// successfully got next field.
			inRecord = true;
			switch (fieldIndex) {
			case 0: // category
				if (s.isEmpty()) {
					/* This is special case. It's the category
					 * list terminating empty field.
					 */
					++fieldIndex;
				} else
					curCat = s;
				break;
			case 1:	// desc
				di.desc = s;
				++fieldIndex;
				break;
			case 2: // name
				di.name = s;
				++fieldIndex;
				break;
			case 3: // pw
				di.pw = s;
				++fieldIndex;
				break;
			case 4: // url
				di.url = s;
				++fieldIndex;
				break;
			case 5: // launcher
				di.launcher = s;
				++fieldIndex;
				break;
			case 6: // comment
				di.comment = s;
				++fieldIndex;
				break;
			default:
				/* Too many fields in a record.
				 * We simply throw it away.
				 */
				break;
			}
			break;
		case 1:
			// record complete.
			if (fieldIndex == 6)
				di.comment = s;
			inRecord = false;
			fieldIndex = 0;
			doc->addEntry(curCat, &di, true);
			//US ENH: clear di for the next row
			di.clear();
			break;
		case 2:
			// data completely parsed.
			doc->flagDirty();
			return true;
		case -1:
			// parse error
			doc->flagDirty();
			return false;
		}
	}
	BUG();
	return false;
}

int Csv::nextField(QCString *ret,
		   const QByteArray &in,
		   bool inRecord,
		   int *_refIndex)
{
	int rv = -2;
	char c;
	bool inField = false;
	bool isQuoted = false;
	bool searchingTerminator = false;
	int refIndex;
	int inSize = static_cast<int>(in.size());
	ret->truncate(0);

	for (refIndex = *_refIndex; refIndex < inSize; ++refIndex) {
		c = in.at(refIndex);
		if (!inField) {
			// we have to search the field beginning, now.
			switch (c) {
			case ' ': // space
			case '	': // tab
				// hm, still not the beginning. Go on..
				break;
			case '\r':
			case '\n':
				if (inRecord) {
					/* This is the end of the last field in
					 * the record.
					 */
					PWM_ASSERT(ret->isEmpty());
					rv = 1; // record end
					goto out;
				} else {
					// hm, still not the beginning. Go on..
					break;
				}
			case ',':
				// Oh, an empty field. How sad.
				PWM_ASSERT(ret->isEmpty());
				rv = 0; // field end
				goto out;
			case '\"':
				// this is the quoted beginning.
				inField = true;
				isQuoted = true;
				if (refIndex + 1 >= inSize)
					goto unexp_eof;
				break;
			default:
				// this is the unquoted beginning.
				inField = true;
				isQuoted = false;
				*ret += c;
				break;
			}
		} else {
			// we are in the field now. Search the end.
			if (isQuoted) {
				if (searchingTerminator) {
					switch (c) {
					case '\r':
					case '\n':
						rv = 1; // record end
						goto out;
					case ',':
						// found it.
						rv = 0; // field end
						goto out;
					default:
						// go on.
						continue;
					}
				}
				switch (c) {
				case '\"':
					/* check if this is the end of the
					 * entry, or just an inline escaped quote.
					 */
					char next;
					if (refIndex + 1 >= inSize) {
						// This is the last char, so it's the end.
						rv = 2; // data end
						goto out;
					}
					next = in.at(refIndex + 1);
					switch (next) {
					case '\"':
						// This is an escaped double quote.
						// So skip next iteration.
						refIndex += 1;
						*ret += c;
						break;
					default:
						/* end of field.
						 * We have to search the comma (or newline...),
						 * which officially terminates the entry.
						 */
						searchingTerminator = true;
						break;
					}
					break;
				default:
					// nothing special about the char. Go on!
					*ret += c;
					break;
				}
			} else {
				switch (c) {
				case '\"':
					// This is not allowed here.
					return -1; // parser error
				case '\r':
				case '\n':
					rv = 1; // record end
					goto out;
				case ',':
					rv = 0; // field end
					goto out;
				default:
					// nothing special about the char. Go on!
					*ret += c;
					break;
				}
			}
		}
	}
	// we are at the end of the stream, now!
	if (searchingTerminator) {
		/* Ok, there's no terminating comma (or newline...),
		 * because we are at the end. That's perfectly fine.
		 */
		PWM_ASSERT(inField);
		rv = 2; // data end
		goto out;
	}
	if (!isQuoted && inField) {
		// That's the end of the last unquoted field.
		rv = 2; // data end
		goto out;
	}
	if (!inField) {
		// This is expected EOF
		rv = 2; // data end
		goto out;
	}

unexp_eof:
	printDebug("unexpected EOF :(");
	return -1; // parser error

out:
	if (!isQuoted)
		*ret = ret->stripWhiteSpace();
	*_refIndex = refIndex + 1;
	return rv;
}

bool Csv::exportData(const QString &filepath,
		     PwMDoc *doc)
{
	PWM_ASSERT(!doc->isDocEmpty());
	bool ret = true;
	if (QFile::exists(filepath)) {
		int ret;
		ret = KMessageBox::questionYesNo(parent,
						 i18n("This file does already exist.\n"
						      "Do you want to overwrite it?"),
						 i18n("Overwrite file?"));
		if (ret == KMessageBox::No)
			return false;
		if (!QFile::remove(filepath)) {
			KMessageBox::error(parent,
					   i18n("Could not delete the old file."),
					   i18n("Delete error."));
			return false;
		}
	}
	QFile f(filepath);
	if (!f.open(IO_ReadWrite)) {
		KMessageBox::error(parent,
				   i18n("Could not open file for writing."),
				   i18n("Open error."));
		ret = false;
		goto out;
	}
	doc->unlockAll_tempoary();
	if (!doExport(f, doc))
		ret = false;
	doc->unlockAll_tempoary(true);
	f.close();
out:
	return ret;
}

bool Csv::doExport(QFile &f,
		   PwMDoc *doc)
{
	unsigned int numCat = doc->numCategories();
	unsigned int numEntr;
	unsigned int i, j;
	PwMDataItem d;
	QCString s, catName;
	QByteArray b;

	for (i = 0; i < numCat; ++i) {
		numEntr = doc->numEntries(i);
		catName = newField(doc->getCategory(i)->c_str());
		for (j = 0; j < numEntr; ++j) {
			doc->getEntry(i, j, &d);
			s = catName;
			s += ",,";
			s += newField(d.desc.c_str());
			s += ",";
			s += newField(d.name.c_str());
			s += ",";
			s += newField(d.pw.c_str());
			s += ",";
			s += newField(d.url.c_str());
			s += ",";
			s += newField(d.launcher.c_str());
			s += ",";
			s += newField(d.comment.c_str());
			s += "\r\n";
			b = s;
			// remove \0 termination
#ifndef PWM_EMBEDDED
			b.resize(b.size() - 1, QGArray::SpeedOptim);
#else
			b.resize(b.size() - 1);
#endif
			if (!f.writeBlock(b))
				return false;
		}
	}
	return true;
}

QCString Csv::newField(QCString s)
{
	if (s.isEmpty())
		return QCString();
	QCString ret("\"");
#ifndef PWM_EMBEDDED
	s.replace('\"', "\"\"");
#else
	s.replace(QRegExp("\""), "\"\"");
#endif
	ret += s;
	ret += "\"";
	return ret;
}
