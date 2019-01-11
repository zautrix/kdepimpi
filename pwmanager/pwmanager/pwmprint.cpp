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
 * $Id: pwmprint.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $
 **************************************************************************/  

#include "pwmprint.h"
#include "pwmexception.h"
#include "pwm.h"
#include "pwmdoc.h"
#include "configuration.h"
#include "printtext.h"
#include "globalstuff.h"
#include "listobjselectwnd.h"

#include <kprinter.h>
#include <kmessagebox.h>
#include <kdialogbase.h>
#include <qdatetime.h>


PwMPrint::PwMPrint(PwMDoc *_doc, QWidget *_parent)
{
	PWM_ASSERT(_doc);
	PWM_ASSERT(_parent);
	doc = _doc;
	parent = _parent;
}

PwMPrint::~PwMPrint()
{
}

void PwMPrint::printNow()
{
	QString docTitle(doc->getTitle());
	if (!doc->unlockAll_tempoary())
		return;
	if (doc->isDocEmpty()) {
		KMessageBox::information(parent,
					 i18n("Sorry, there are no entries to print "
					      "in the document \"") +
					 docTitle + "\".",
					 i18n("nothing to do"));
		doc->unlockAll_tempoary(true);
		return;
	}
	QStringList printCategories(selCategories());
	if (printCategories.isEmpty()) {
		doc->unlockAll_tempoary(true);
		return;
	}
	doc->ensureLvp();

	PrintText prn;
	if (!prn.setup(parent)) {
		doc->unlockAll_tempoary(true);
		return;
	}
	prn.setFullPage(false);
	prn.setCreator(PROG_NAME);
	prn.setDocName(docTitle);

	prn.setHeader(genDateString(), docTitle);
	prn.beginPrint();

	prn.printLine("");
	prn.printLine(QString(PROG_NAME " v" PACKAGE_VER " ") + i18n("password table:"));
	prn.printLine("");

	vector<PwMCategoryItem> tmp;
	PwMCategoryItem catItem;
	catItem.clear();
	PwMDataItem item;
	item.clear();
	unsigned int numEntr, j, i = 0;
	QStringList::iterator catI = printCategories.begin(),
			      catEnd = printCategories.end();
	// Sort items on lvp and store them in tempoary "tmp".
	while (catI != catEnd) {
		catItem.clear();
		catItem.name = (*catI).latin1();
		tmp.push_back(catItem);
		item.clear();
		numEntr = doc->numEntries(*catI);
		tmp[i].d.insert(tmp[i].d.begin(), numEntr, item);
		for (j = 0; j < numEntr; ++j) {
			doc->getEntry(*catI, j, &item);
			tmp[i].d[numEntr - item.listViewPos - 1] = item;
		}
		++catI;
		++i;
	}
	doc->unlockAll_tempoary(true);

	QString currLine;
	vector<PwMCategoryItem>::iterator cat_it = tmp.begin(),
					  cat_end = tmp.end();
	unsigned int size;
	while (cat_it != cat_end) {
		i = 0;
		size = cat_it->d.size();
		prn.printLine("");
		currLine = "== ";
		currLine += i18n("Category: ");
		currLine += cat_it->name.c_str();
		currLine += " ==";
		prn.printLine(currLine);
		prn.printLine("");
		while (true) {
			item = cat_it->d[i];
			currLine = "-- ";
			currLine += item.desc.c_str();
			currLine += " --";
			prn.printLine(currLine);
			if (item.name.size()) {
				currLine = i18n("Username");
				currLine += ": ";
				currLine += item.name.c_str();
				prn.printLine(currLine);
			}
			if (item.pw.size()) {
				currLine = i18n("Password");
				currLine += ": ";
				currLine += item.pw.c_str();
				prn.printLine(currLine);
			}
			if (item.comment.size()) {
				currLine = i18n("Comment");
				currLine += ": ";
				currLine += item.comment.c_str();
				prn.printLine(currLine);
			}
			if (item.url.size()) {
				currLine = i18n("URL");
				currLine += ": ";
				currLine += item.url.c_str();
				prn.printLine(currLine);
			}
			if (item.launcher.size()) {
				currLine = i18n("Launcher");
				currLine += ": ";
				currLine += item.launcher.c_str();
				prn.printLine(currLine);
			}

			++i;
			if (i >= size)
				break;
			prn.printLine("");
		}
		++cat_it;
	}
}

QString PwMPrint::genDateString()
{
	QDateTime dt = QDateTime::currentDateTime();
	QString ret(dt.toString(Qt::LocalDate));
	return ret;
}

QStringList PwMPrint::selCategories()
{
	ListObjSelectWnd selWnd(i18n("Print categories"),
				i18n("Please select the categories "
				     "you want to print."),
				parent, true);
	QStringList catList;
	doc->getCategoryList(&catList);
	selWnd.setList(catList);
	if (selWnd.exec() != 1)
		return QStringList();
	return selWnd.getSelectedList();
}
