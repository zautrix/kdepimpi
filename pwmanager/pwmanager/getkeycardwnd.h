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
 * $Id: getkeycardwnd.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef GETKEYCARDWND_H
#define GETKEYCARDWND_H

#include <qvbox.h>
#include <qhbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qevent.h>

/** The equivalent to GetMasterPwWndImpl for chipcards */
class GetKeyCardWnd : public QDialog
{
	Q_OBJECT
public:
	GetKeyCardWnd(QWidget *parent = 0, const char *name = 0,
		      bool modal = FALSE, WFlags f = 0);
	~GetKeyCardWnd();

protected slots:
	void okButton_slot();

protected:
	QVBox *vbox1;
	QHBox *hbox1;
	QLabel *text_label;
	QPushButton *okButton;
	QPushButton *cancelButton;

protected:
	void resizeEvent(QResizeEvent *);
};

#endif
