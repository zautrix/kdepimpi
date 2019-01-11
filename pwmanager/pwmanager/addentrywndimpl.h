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
 * $Id: addentrywndimpl.h,v 1.4 2004/10/29 05:20:48 ulf69 Exp $
 **************************************************************************/  

#ifndef __ADDENTRYWNDIMPL_H
#define __ADDENTRYWNDIMPL_H


#ifndef PWM_EMBEDDED
#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include "addentrywnd.h"
#else
#include <klineedit.h>
#include <kcombobox.h>
#include <qmultilineedit.h>
#include "addentrywnd_emb.h"
#endif

#include <qpushbutton.h>

class PwGenWndImpl;

/** "add/edit" Window */
class AddEntryWndImpl : public addEntryWnd
{
	Q_OBJECT
public:
#ifndef PWM_EMBEDDED
	AddEntryWndImpl();
#else
	AddEntryWndImpl( PwMDoc* doc, QWidget* parent = 0, const char* name = 0);
#endif
	~AddEntryWndImpl();

	/* get... functions */
	QString getDescription()
			{ return descLineEdit->text(); }
	QString getCategory()
			{ return categoryComboBox->currentText(); }
	QString getUsername()
			{ return usernameLineEdit->text(); }
	QString getPassword()
			{ return pwLineEdit->text(); }
	QString getUrl()
			{ return urlLineEdit->text(); }
	QString getLauncher()
			{ return launcherLineEdit->text(); }
	QString getComment();

	/* set... functions */
	void setDescription(const QString &desc)
			{ descLineEdit->setText(desc); }
	void setCurrCategory(const QString &cat);
	void addCategory(const QString &cat)
			{ categoryComboBox->insertItem(cat); }
	void setUsername(const QString &name)
			{ usernameLineEdit->setText(name); }
	void setPassword(const QString &pw)
			{ pwLineEdit->setText(pw); }
	void setUrl(const QString &url)
			{ urlLineEdit->setText(url); }
	void setLauncher(const QString launcher)
			{ launcherLineEdit->setText(launcher); }
	void setComment(const QString &comm);

	/** are we using an advanced comment */
	bool isAdvancedComment()
			{ return useAdvComment; }

public slots:
#ifndef PWM_EMBEDDED 
//MOC_SKIP_BEGIN
	/** OK button pressed */
	void okButton_slot();
	/** cancel button pressed */
	void cancelButton_slot();
//MOC_SKIP_END
#else
	virtual void slotOk();
#endif
	/** Reveal button pressed */
	void revealButton_slot();
	/** Generate button pressed */
	void generateButton_slot();
	/** advanced comment button pressed */
	void advancedCommentButton_slot(bool on);
	/** edit advanced comment button pressed */
	void editAdvCommentButton_slot();

protected:
	void switchComment(bool toAdvanced);

protected:
	QPushButton *editAdvCommentButton;
#ifndef PWM_EMBEDDED
	QTextEdit *commentTextEdit;
#else
	//nothing here 
#endif
	/** saved data from normal comment text edit box */
	QString savedCommentText;
	/** use an advanced comment? */
	bool useAdvComment;
	/** data of advanced comment (if available) */
	QString advCommentDta;
	/** password generation object */
	PwGenWndImpl *pwGen;
};

#endif
