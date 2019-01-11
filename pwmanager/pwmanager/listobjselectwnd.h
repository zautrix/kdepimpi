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
 * $Id: listobjselectwnd.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef LISTOBJSELECTWND_H
#define LISTOBJSELECTWND_H

#include <qdialog.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qstringlist.h>


/** Display a list-window for selection of some objects */
class ListObjSelectWnd : public QDialog
{
	Q_OBJECT
public:
	ListObjSelectWnd(const QString &caption, const QString &titleText,
			 QWidget *parent = 0, bool multiSel = false,
			 const char *name = 0,
			 bool modal = FALSE, WFlags f = 0);
	~ListObjSelectWnd();

	void setList(const QStringList &_list);
	void addObj(const QString &name)
			{ list->insertItem(name); }
	void selectObj(const QString &name)
#ifndef PWM_EMBEDDED
			{ list->setCurrentItem(list->findItem(name, Qt::ExactMatch)); }
#else
			{ list->setCurrentItem(list->findItem(name)); }
#endif
	QString getSelected()
			{ return list->currentText(); }
	int getSelectedIndex()
#ifndef PWM_EMBEDDED
			{ return list->index(list->selectedItem()); }
#else
			{ return list->currentItem(); }
#endif
	QStringList getSelectedList();
	void clearObjs()
			{ list->clear(); }

protected:
	QVBox *vbox1;
	QHBox *hbox1;
	QHBox *hbox2;
	QLabel *title;
	QListBox *list;
	QPushButton *okButton;
	QPushButton *cancelButton;
	QPushButton *selAllButton;
	QPushButton *unselAllButton;

protected:
	void resizeEvent(QResizeEvent *);

protected slots:
	void okButton_slot();
	void cancelButton_slot();
	void selAllButton_slot();
	void unselAllButton_slot();
};

#endif
