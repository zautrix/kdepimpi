/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003, 2004 by Michael Buesch                            *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   Many very good improvements and the original implementations of       *
 *   them came from Matt Scifo <mscifo@o1.com>                             *
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
 * $Id: addentrywndimpl.cpp,v 1.6 2005/03/03 14:48:41 zautrix Exp $
 **************************************************************************/  

#include "addentrywndimpl.h"
#include "pwmexception.h"
#include "pwgenwndimpl.h"
#ifndef PWM_EMBEDDED
#include "advcommeditimpl.h"
#endif
#include "htmlgen.h"

#include <kmessagebox.h>
#include <klocale.h>

#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>


#ifndef PWM_EMBEDDED
AddEntryWndImpl::AddEntryWndImpl()
  : addEntryWnd( 0, "AddEntryWndImpl", TRUE)
#else
AddEntryWndImpl::AddEntryWndImpl( PwMDoc* doc, QWidget* parent, const char* name)
  : addEntryWnd( doc, parent, name)
#endif
{
	categoryComboBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,QSizePolicy::Preferred ));
#ifndef PWM_EMBEDDED
	editAdvCommentButton = 0;
	commentTextEdit = 0;
#endif
	switchComment(false);
	pwGen = new PwGenWndImpl(this);
}

AddEntryWndImpl::~AddEntryWndImpl()
{
#ifndef PWM_EMBEDDED
	delete_ifnot_null(editAdvCommentButton);
	delete_ifnot_null(commentTextEdit);
#endif
	delete pwGen;
}

#ifdef PWM_EMBEDDED
void AddEntryWndImpl::slotOk()
{
  slotApply();

  if (pwLineEdit->text().isEmpty()) {
    KMessageBox::error(this,
	i18n("Sorry, you haven't set a password."),
	i18n("no password"));
    return;
  }
  
  if (descLineEdit->text().isEmpty()) {
    KMessageBox::error(this,
	i18n("You haven't set a \"Description\"."),
	i18n("Description not set"));
    return;
  }
 
  KDialogBase::slotOk();
}
#else

void AddEntryWndImpl::okButton_slot()
{
	if (pwLineEdit->text().isEmpty()) {
		KMessageBox::error(this,
				   i18n("Sorry, you haven't set a password."),
				   i18n("no password"));
		return;
	}
	if (descLineEdit->text().isEmpty()) {
		KMessageBox::error(this,
				   i18n
				   ("You haven't set a \"Description\"."),
				   i18n("Description not set"));
		return;
	}
	done(1);
}

void AddEntryWndImpl::cancelButton_slot()
{
	done(2);
}
#endif

void AddEntryWndImpl::setCurrCategory(const QString &cat)
{
	int i, count = categoryComboBox->count();

	for (i = 0; i < count; ++i) {
		if (categoryComboBox->text(i) == cat) {
			categoryComboBox->setCurrentItem(i);
#ifdef PWM_EMBEDDED
			categorySelected(cat);
#endif
			return;
		}
	}
	BUG();
}

void AddEntryWndImpl::revealButton_slot()
{
	if (revealButton->isOn()) {
		pwLineEdit->setEchoMode(QLineEdit::Normal);
	} else {
		pwLineEdit->setEchoMode(QLineEdit::Password);
	}
}

void AddEntryWndImpl::generateButton_slot()
{
	if (!pwGen->exec())
		return;
	setPassword(pwGen->getPassword());
}

QString AddEntryWndImpl::getComment()
{
#ifndef PWM_EMBEDDED
	if (isAdvancedComment()) {
		return advCommentDta;
	}
#endif
	return commentTextEdit->text();
}

void AddEntryWndImpl::setComment(const QString &comm)
{
#ifndef PWM_EMBEDDED
	if (HtmlGen::isHtml(comm)) {
		advancedCommentButton->setOn(true);
		advCommentDta = comm;
	} else {
		advancedCommentButton->setOn(false);
		commentTextEdit->setText(comm);
	}
#else
	commentTextEdit->setText(comm);
#endif
}

void AddEntryWndImpl::advancedCommentButton_slot(bool on)
{
#ifndef PWM_EMBEDDED
	switchComment(on);
#endif
}

void AddEntryWndImpl::switchComment(bool toAdvanced)
{
#ifndef PWM_EMBEDDED

	useAdvComment = toAdvanced;
	if (toAdvanced) {
		if (commentTextEdit) {
			savedCommentText = commentTextEdit->text();
			delete_and_null(commentTextEdit);
		}
		if (editAdvCommentButton)
			return;
		editAdvCommentButton = new QPushButton(i18n("Edit advanced comment..."),
						       commentDummy);
		editAdvCommentButton->resize(commentDummy->size().width(), 50);
		connect(editAdvCommentButton, SIGNAL(clicked()),
			this, SLOT(editAdvCommentButton_slot()));
		editAdvCommentButton->show();
	} else {
		delete_ifnot_null(editAdvCommentButton);
		if (commentTextEdit)
			return;

		commentTextEdit = new QTextEdit(commentDummy);
		commentTextEdit->setTextFormat(Qt::PlainText);
		commentTextEdit->resize(commentDummy->size());
		commentTextEdit->setText(savedCommentText);
		commentTextEdit->show();
	}
#endif
}

void AddEntryWndImpl::editAdvCommentButton_slot()
{
#ifndef PWM_EMBEDDED
	AdvCommEditImpl editor(this);
	editor.setHtmlDta(advCommentDta);
	if (editor.exec())
		return;
	advCommentDta = editor.getHtmlDta();
#endif
}

#ifndef PWM_EMBEDDED
#include "addentrywndimpl.moc"
#endif
