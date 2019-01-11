/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
 *   email: mbuesch@freenet.de                                           *
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
 * $Id: waitwnd.cpp,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#include "waitwnd.h"
#include "globalstuff.h"
#include "pwmexception.h"

#include <qmessagebox.h>

#include <klocale.h>


WaitWnd::WaitWnd(QString caption, QString _staticText,
		 bool showCancelButton, bool showGenericText,
		 QWidget *parent,
		 const char *name, bool modal, WFlags f)
 : QDialog(parent, name, modal, f)
{
	canClose = false;
	vbox1 = new QVBox(this);
	staticText = new QLabel(vbox1);
	if (showGenericText) {
		genericText = new QLabel(vbox1);
		genericText->setAlignment(Qt::AlignHCenter | Qt::WordBreak);
	} else {
		genericText = 0;
	}
	if (showCancelButton) {
		cancelButton = new QPushButton(vbox1);
		connect(cancelButton, SIGNAL(clicked()),
			this, SLOT(cancel_slot()));
		cancelButton->setText(i18n("&Cancel"));
	} else {
		cancelButton = 0;
	}

	vbox1->setSpacing(10);
	vbox1->setMargin(10);
	resize(300, 150);
	setCaption(caption);
	staticText->setText(_staticText);
	staticText->setAlignment(Qt::AlignHCenter | Qt::WordBreak);
}

WaitWnd::~WaitWnd()
{
}

void WaitWnd::resizeEvent(QResizeEvent *)
{
	vbox1->resize(size());
}

void WaitWnd::closeEvent(QCloseEvent *e)
{
	if (canClose) {
		emit goingToClose();
		e->accept();
	} else {
		e->ignore();
	}
}

void WaitWnd::cancel_slot()
{
	canClose = true;
	close();
}

void WaitWnd::updateGenericText(const QString &text)
{
	if (genericText)
		genericText->setText(text);
}

#ifndef PWM_EMBEDDED
#include "waitwnd.moc"
#endif
