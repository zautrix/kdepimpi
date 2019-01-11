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
 * $Id: waitwnd.h,v 1.1 2004/09/15 17:55:58 ulf69 Exp $
 **************************************************************************/  

#ifndef WAITWND_H
#define WAITWND_H

#include <qvbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qevent.h>

/** window to ask the user to wait */
class WaitWnd : public QDialog
{
	Q_OBJECT
public:
	WaitWnd(QString caption, QString _staticText,
		bool showCancelButton, bool showGenericText,
		QWidget *parent = 0, const char *name = 0,
		bool modal = FALSE, WFlags f = 0);
	~WaitWnd();

	void updateGenericText(const QString &text);

signals:
	void goingToClose();

protected slots:
	void cancel_slot();

protected:
	QVBox *vbox1;
	QLabel *staticText;
	QLabel *genericText;
	QPushButton *cancelButton;
	/** can we safely close the widget? */
	bool canClose;

protected:
	void resizeEvent(QResizeEvent *);
	void closeEvent(QCloseEvent *e);
};

#endif
