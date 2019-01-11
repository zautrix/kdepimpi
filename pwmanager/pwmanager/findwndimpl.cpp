/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
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
 * $Id: findwndimpl.cpp,v 1.2 2004/09/24 19:51:43 ulf69 Exp $
 **************************************************************************/  

#include "findwndimpl.h"
#include "pwmexception.h"
#include "pwmdoc.h"
#include "pwmview.h"

#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <kmessagebox.h>
#include <klocale.h>

FindWndImpl::FindWndImpl(PwMView * _parent)
 : findWnd()
{
	parent = _parent;
	fAt = 0;
	refVal = 0;
	currFoundPos = -1;
	numEntries = parent->document()->numEntries(parent->getCurrentCategory());
	connect(this, SIGNAL(foundAt(int)), parent, SLOT(selAt(int)));
}

FindWndImpl::~FindWndImpl()
{
}

void FindWndImpl::findButton_slot()
{
	if (findLineEdit->text() == "")
		return;
	static vector<unsigned int> foundPositions;
	PwMDoc *doc = parent->document();

	if (currFoundPos < 0) {
		bool unlockedTempoary = false;
		foundPositions.clear();
		PwMDataItem findThis;
		unsigned int searchIn = 0;

		if (descRadioButton->isChecked()) {
			searchIn = SEARCH_IN_DESC;
			findThis.desc = findLineEdit->text().latin1();
		} else if (nameRadioButton->isChecked()) {
			searchIn = SEARCH_IN_NAME;
			findThis.name = findLineEdit->text().latin1();
		} else if (pwRadioButton->isChecked()) {
			searchIn = SEARCH_IN_PW;
			findThis.pw = findLineEdit->text().latin1();
		} else if (commentRadioButton->isChecked()) {
			searchIn = SEARCH_IN_COMMENT;
			findThis.comment = findLineEdit->text().latin1();
		} else if (urlRadioButton->isChecked()) {
			searchIn = SEARCH_IN_URL;
			findThis.url = findLineEdit->text().latin1();
		} else if (launcherRadioButton->isChecked()) {
			searchIn = SEARCH_IN_LAUNCHER;
			findThis.launcher = findLineEdit->text().latin1();
		}

		if (pwRadioButton->isChecked()) {
			if (!doc->unlockAll_tempoary())
				return;
			unlockedTempoary = true;
		}
		doc->findEntry(parent->getCurrentCategory(), findThis,
			       searchIn, &foundPositions, false,
			       caseSensCheckBox->isChecked(),
			       exactCheckBox->isChecked(),
			       true);
		if (unlockedTempoary) {
			doc->unlockAll_tempoary(true);
		}

		if (!foundPositions.size()) {
			KMessageBox::information(this,
						 i18n("No entry found."),
						 i18n("not found"));
			return;
		}
		currFoundPos = 0;
	}

	int lvp = doc->getListViewPos(parent->getCurrentCategory(),
				      foundPositions[currFoundPos++]);
	emit foundAt(numEntries - 1 - lvp);

	if (currFoundPos + 1 > static_cast<int>(foundPositions.size()))
		currFoundPos = 0;
}


#ifdef PWM_EMBEDDED
void FindWndImpl::slotOk()
{
  findButton_slot();

  //do not call  KDialogBase::slotOk() here
  // user should press the close button instead
}
#endif

void FindWndImpl::closeButton_slot()
{
#ifndef PWM_EMBEDDED
	done(0);
#endif
}

void FindWndImpl::selectionChanged_slot()
{
	fAt = 0;
	refVal = 0;
	currFoundPos = -1;
}

#ifndef PWM_EMBEDDED
#include "findwndimpl.moc"
#endif
