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
 * $Id: advcommeditimpl.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef ADVCOMMEDITIMPL_H
#define ADVCOMMEDITIMPL_H

#include <qlistbox.h>

#include "advcommedit.h"

class HtmlComment;
class KHTMLPart;

class AdvCommEditImpl : public advCommEdit
{
	Q_OBJECT
public:
	AdvCommEditImpl(QWidget* parent = 0,
			const char* name = 0,
			WFlags fl = 0);
	~AdvCommEditImpl();

	/** returns the HTML data for the comment */
	QString getHtmlDta();
	/** sets the HTML data, parses and displays it */
	void setHtmlDta(const QString &str);

public slots:
	/** sync preview with dta */
	void updatePreview();
	/** ok button pressed */
	void okButton_slot();
	/** cancel button pressed */
	void cancelButton_slot();
	/** add subtbl button pressed */
	void addSubtblButton_slot();
	/** edit button pressed */
	void editButton_slot();
	/** delete button pressed */
	void delButton_slot();

protected slots:

protected:
	/** read the title from the line-edit and update dta */
	void updateTitle();
	/** returns the index of the currently selected entry */
	int curIndex()
#ifndef PWM_EMBEDDED
			{ return entryListBox->index(entryListBox->selectedItem()); }
#else
			{ return entryListBox->currentItem(); }
#endif
protected:
	/** current internal representation structure
	  * of the comment
	  */
	HtmlComment *dta;
	/** preview browser */
	KHTMLPart *preview;
};

#endif
