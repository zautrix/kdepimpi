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
 * $Id: subtbleditimpl.cpp,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#include "subtbleditimpl.h"
#include "pwmexception.h"

#include <qmessagebox.h>

#include <klocale.h>


SubTblEditImpl::SubTblEditImpl(QWidget* parent,
				 const char* name,
				 WFlags fl)
 : subTblEdit(parent, name, fl)
{
	prevSelection = -1;
	connect(entryListBox, SIGNAL(currentChanged(QListBoxItem *)),
		this, SLOT(selectedEntry_slot()));
}

SubTblEditImpl::~SubTblEditImpl()
{
}

void SubTblEditImpl::okButton_slot()
{
	if (getTitle().isEmpty()) {
		QMessageBox::information(this,
					 i18n("no title"),
					 i18n("Please enter a title."));
		return;
	}
	if (!entries.size()) {
		QMessageBox::information(this,
					 i18n("no entries"),
					 i18n("Please add some entries."));
		return;
	}
	int index = curIndex();
	if (index != -1) {
		entries[index].first = nameLineEdit->text();
		entries[index].second = valueLineEdit->text();
	}
	done(0);
}

void SubTblEditImpl::cancelButton_slot()
{
	done(1);
}

void SubTblEditImpl::addButton_slot()
{
	QString name(nameLineEdit->text());
	if (name.isEmpty())
		return;
	QString value(valueLineEdit->text());
	prevSelection = -1;
	nameLineEdit->clear();
	valueLineEdit->clear();
#ifndef PWM_EMBEDDED
	entryListBox->setSelected(entryListBox->index(
				  entryListBox->selectedItem()),
				  false);
#else
	entryListBox->setSelected(entryListBox->currentItem(),
				  false);
#endif
	pair<QString, QString> p;
	p.first = name;
	p.second = value;
	entries.push_back(p);
	entryListBox->insertItem(name);
}

void SubTblEditImpl::delButton_slot()
{
	int index = curIndex();
	if (index == -1)
		return;
	entries.erase(entries.begin() + index);
	entryListBox->removeItem(index);
}

void SubTblEditImpl::selectedEntry_slot()
{
	int index = curIndex();
	if (index == -1)
		return;
	disconnect(entryListBox, SIGNAL(currentChanged(QListBoxItem *)),
		   this, SLOT(selectedEntry_slot()));
	if ((prevSelection != -1) && (prevSelection != index)) {
		entries[prevSelection].first = nameLineEdit->text();
		entries[prevSelection].second = valueLineEdit->text();
		entryListBox->changeItem(nameLineEdit->text(), prevSelection);
	}
	pair<QString, QString> p(entries[index]);
	nameLineEdit->setText(p.first);
	valueLineEdit->setText(p.second);
	prevSelection = index;
	entryListBox->setSelected(index, true);
	connect(entryListBox, SIGNAL(currentChanged(QListBoxItem *)),
		this, SLOT(selectedEntry_slot()));
}

void SubTblEditImpl::setContent(const QString &title,
				const vector< pair<QString, QString> > *_entries)
{
	entries = *_entries;
	titleLineEdit->setText(title);
	vector< pair<QString, QString> >::iterator i = entries.begin(),
						   end = entries.end();
	while (i != end) {
		entryListBox->insertItem(i->first);
		++i;
	}
}

#ifndef PWM_EMBEDDED
#include "subtbleditimpl.moc"
#endif
