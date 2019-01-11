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
 * $Id: rencatwnd.h,v 1.1 2004/09/15 17:55:58 ulf69 Exp $
 **************************************************************************/  

#ifndef RENCATWND_H
#define RENCATWND_H

#include <qvbox.h>
#include <qhbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qevent.h>


/** rename category window */
class RenCatWnd : public QDialog
{
	Q_OBJECT
public:
	RenCatWnd(QWidget *parent = 0, const char *name = 0,
		    bool modal = FALSE, WFlags f = 0);
	~RenCatWnd();

	QString getNewName()
			{ return newName->text(); }

protected slots:
	void okButton_slot();
	void cancelButton_slot();

protected:
	QVBox *vbox1;
	QHBox *hbox1;
	QLabel *label;
	QLineEdit *newName;
	QPushButton *okButton;
	QPushButton *cancelButton;

protected:
	void resizeEvent(QResizeEvent *);
};

#endif
