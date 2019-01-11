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
 * $Id: listobjselectwnd.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $
 **************************************************************************/  

#include "listobjselectwnd.h"

#include <klocale.h>


ListObjSelectWnd::ListObjSelectWnd(const QString &caption, const QString &titleText,
				   QWidget *parent, bool multiSel, const char *name,
				   bool modal, WFlags f)
 : QDialog(parent, name, modal, f)
{
	vbox1 = new QVBox(this);
	title = new QLabel(vbox1);
	list = new QListBox(vbox1);
	if (multiSel) {
		hbox2 = new QHBox(vbox1);
		selAllButton = new QPushButton(i18n("&Select all"), hbox2);
		unselAllButton = new QPushButton(i18n("&Unselect all"), hbox2);
	}
	hbox1 = new QHBox(vbox1);
	okButton = new QPushButton(i18n("&Ok"), hbox1);
	cancelButton = new QPushButton(i18n("&Cancel"), hbox1);

	vbox1->setSpacing(10);
	vbox1->setMargin(10);
	hbox1->setSpacing(10);
	resize(250, 300);
	setCaption(caption);
	if (multiSel) {
		list->setSelectionMode(QListBox::Multi);
	}

	title->setAlignment(Qt::AlignHCenter | Qt::WordBreak);
	title->setText(titleText);

	connect(okButton, SIGNAL(clicked()),
		this, SLOT(okButton_slot()));
	connect(cancelButton, SIGNAL(clicked()),
		this, SLOT(cancelButton_slot()));
	if (multiSel) {
		connect(selAllButton, SIGNAL(clicked()),
			this, SLOT(selAllButton_slot()));
		connect(unselAllButton, SIGNAL(clicked()),
			this, SLOT(unselAllButton_slot()));
	}
}

ListObjSelectWnd::~ListObjSelectWnd()
{
}

void ListObjSelectWnd::resizeEvent(QResizeEvent *)
{
	vbox1->resize(size());
}

void ListObjSelectWnd::okButton_slot()
{
	unsigned int cnt = list->count(), i;
	for (i = 0; i < cnt; ++i) {
		if (list->isSelected(i)) {
			done(1);
			return;
		}
	}
}

void ListObjSelectWnd::cancelButton_slot()
{
	done(2);
}

void ListObjSelectWnd::selAllButton_slot()
{
	unsigned int cnt = list->count(), i;
	for (i = 0; i < cnt; ++i) {
		list->setSelected(i, true);
	}
}

void ListObjSelectWnd::unselAllButton_slot()
{
	unsigned int cnt = list->count(), i;
	for (i = 0; i < cnt; ++i) {
		list->setSelected(i, false);
	}
}

QStringList ListObjSelectWnd::getSelectedList()
{
	QStringList ret;
	unsigned int cnt = list->count(), i;
	for (i = 0; i < cnt; ++i) {
		if (list->isSelected(i)) {
#ifndef PWM_EMBEDDED
			ret.push_back(list->text(i));
#else
			ret.append(list->text(i));
#endif
		}
	}
	return ret;
}

void ListObjSelectWnd::setList(const QStringList &_list)
{
	list->clear();
	list->insertStringList(_list);
}

#ifndef PWM_EMBEDDED
#include "listobjselectwnd.moc"
#endif
