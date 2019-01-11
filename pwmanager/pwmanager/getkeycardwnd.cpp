/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
 *   email: fsdeveloper@yahoo.de                                           *
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
 * $Id: getkeycardwnd.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $
 **************************************************************************/  

#include "getkeycardwnd.h"

#include <qmessagebox.h>

#include <klocale.h>


GetKeyCardWnd::GetKeyCardWnd(QWidget *parent,
			 const char *name, bool modal, WFlags f)
 : QDialog(parent, name, modal, f)
{
	vbox1 = new QVBox(this);
	text_label = new QLabel(vbox1);
	hbox1 = new QHBox(vbox1);
	okButton = new QPushButton(i18n("&Ok"), hbox1);
	cancelButton = new QPushButton(i18n("&Cancel"), hbox1);

	vbox1->setSpacing(10);
	vbox1->setMargin(10);
	hbox1->setSpacing(10);

	resize(500, 100);

	setCaption(i18n("Insert key-card"));
	text_label->setText(i18n("Please insert the key-card "
				 "and press the OK button."));

	connect(okButton, SIGNAL(clicked()), this, SLOT(okButton_slot()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

GetKeyCardWnd::~GetKeyCardWnd()
{
}

void GetKeyCardWnd::resizeEvent(QResizeEvent *)
{
	vbox1->resize(size());
}

void GetKeyCardWnd::okButton_slot()
{
	done(1);
}

#ifndef PWM_EMBEDDED
#include "getkeycardwnd.moc"
#endif
