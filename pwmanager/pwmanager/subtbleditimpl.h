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
 * $Id: subtbleditimpl.h,v 1.1 2004/09/15 17:55:58 ulf69 Exp $
 **************************************************************************/  

#ifndef SUBTBLEDITIMPL_H
#define SUBTBLEDITIMPL_H

#include "subtbledit.h"

#include <qlineedit.h>
#include <qlistbox.h>

#include <vector>
#include <utility>

using std::vector;
using std::pair;


class SubTblEditImpl : public subTblEdit
{
	Q_OBJECT
public:
	SubTblEditImpl(QWidget* parent = 0,
		       const char* name = 0,
		       WFlags fl = 0);
	~SubTblEditImpl();

	/** returns the title */
	QString getTitle() const
			{ return titleLineEdit->text(); }
	/** returns a pointer to the data */
	const vector< pair<QString, QString> > * getEntries() const
			{ return &entries; }
	/** sets the content of the subtable editor */
	void setContent(const QString &title,
			const vector< pair<QString, QString> > *_entries);

public slots:
	/** on button pressed */
	void okButton_slot();
	/** cancel button pressed */
	void cancelButton_slot();
	/** add button pressed */
	void addButton_slot();
	/** delete button pressed */
	void delButton_slot();

protected slots:
	/** the user selected another entry */
	void selectedEntry_slot();

protected:
	/** returns the index of the currently selected entry */
	int curIndex()
#ifndef PWM_EMBEDDED
			{ return entryListBox->index(entryListBox->selectedItem()); }
#else
			{ return entryListBox->currentItem(); }
#endif

protected:
	/** internal storage for all entries */
	vector< pair<QString, QString> > entries;
	/** stores the previous selection */
	int prevSelection;
};

#endif
