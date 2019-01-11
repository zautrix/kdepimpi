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
 * $Id: rencatwnd.cpp,v 1.2 2004/10/20 11:31:39 zautrix Exp $
 **************************************************************************/  

#include "rencatwnd.h"

#include <klocale.h>


RenCatWnd::RenCatWnd(QWidget *parent,
		       const char *name, bool modal, WFlags f)
 : QDialog(parent, name, true, f)
{
	vbox1 = new QVBox(this);
	label = new QLabel(vbox1);
	newName = new QLineEdit(vbox1);
	hbox1 = new QHBox(vbox1);
	okButton = new QPushButton(i18n("&Ok"), hbox1);
	cancelButton = new QPushButton(i18n("&Cancel"), hbox1);

	vbox1->setSpacing(10);
	vbox1->setMargin(10);
	hbox1->setSpacing(10);

	resize(220, 100);

	setCaption(i18n("Rename category"));
	label->setText(i18n("New category name:"));

	connect(okButton, SIGNAL(clicked()), this, SLOT(okButton_slot()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButton_slot()));
}

RenCatWnd::~RenCatWnd()
{
}

void RenCatWnd::resizeEvent(QResizeEvent *)
{
	vbox1->resize(size());
}

void RenCatWnd::okButton_slot()
{
	done(1);
}

void RenCatWnd::cancelButton_slot()
{
	done(2);
}

#ifndef PWM_EMBEDDED
#include "rencatwnd.moc"
#endif
