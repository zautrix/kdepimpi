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
 * $Id: pwgenwndimpl.cpp,v 1.2 2004/09/23 19:33:55 ulf69 Exp $
 **************************************************************************/  

#include "pwgenwndimpl.h"
#include "pwmexception.h"
#include "genpasswd.h"

#include <qtabwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>

#include <klocale.h>
#include <kmessagebox.h>


#ifndef PWM_EMBEDDED
PwGenWndImpl::PwGenWndImpl(QWidget *parent,
			   const char *name,
			   bool modal,
			   WFlags fl)
 : pwGenWnd(parent, name, modal, fl)
{
}

#else
PwGenWndImpl::PwGenWndImpl( QWidget* parent, const char* name)
  : pwGenWnd( parent, name)
{
}
#endif

PwGenWndImpl::~PwGenWndImpl()
{
}

#ifdef PWM_EMBEDDED
void PwGenWndImpl::slotOk()
{
  // internal generator
  if (!optionsSanityIntGen())
	return;
  if (!startIntGen())
	return;
 
  KDialogBase::slotOk();
}
#endif

void PwGenWndImpl::genButton_slot()
{
#ifndef PWM_EMBEDDED
	// internal generator
	if (!optionsSanityIntGen())
		return;
	if (startIntGen())
		goto exit_success;
	done(0);
exit_success:
	done(1);
#endif
}

void PwGenWndImpl::cancelButton_slot()
{
#ifndef PWM_EMBEDDED
	done(0);
#endif
}

bool PwGenWndImpl::optionsSanityIntGen()
{
	if (int_charLowerCheckBox->isChecked())
		return true;
	if (int_charUpperCheckBox->isChecked())
		return true;
	if (int_charNumCheckBox->isChecked())
		return true;
	if (int_charSpecCheckBox->isChecked())
		return true;
	if (int_charUserCheckBox->isChecked()) {
		if (int_userDefLineEdit->text().length() >= 2)
			return true;
		if (int_charBlankCheckBox->isChecked())
			return true;
	}
	KMessageBox::error(this,
			   i18n("Incorrect Charset selection!\n"
			        "It's impossible to generate a sane "
			        "password with the selected charset(s).\n"
			        "Please select more charsets."),
			   i18n("Incorrect Charset selection"));
	return false;
}

bool PwGenWndImpl::startIntGen()
{
	GenPasswd gen;
	gen.setLen(int_lenSpinBox->value());
	gen.setUseFilter(int_filterCheckBox->isChecked());
	gen.setCharset(int_charLowerCheckBox->isChecked(),
		       int_charUpperCheckBox->isChecked(),
		       int_charNumCheckBox->isChecked(),
		       int_charSpecCheckBox->isChecked(),
		       int_charBlankCheckBox->isChecked(),
		       int_charUserCheckBox->isChecked() ?
				int_userDefLineEdit->text() :
				QString::null);
	QString pw(gen.gen());
	if (pw.isEmpty())
		return false;
	password = pw;
	return true;
}

QString PwGenWndImpl::getPassword()
{
	QString ret(password);
	password = QString::null;
	return ret;
}
