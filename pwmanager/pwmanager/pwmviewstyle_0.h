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
 * $Id: pwmviewstyle_0.h,v 1.3 2005/03/03 13:42:16 zautrix Exp $
 **************************************************************************/  

#ifndef PWMVIEWSTYLE_0_H
#define PWMVIEWSTYLE_0_H

#include <qhbox.h>
#include <qvbox.h>
#include <qpushbutton.h>
#ifndef PWM_EMBEDDED
#include <qtextedit.h>
#include <qsplitter.h>
#else
#include <qmultilineedit.h>
#include <KDGanttMinimizeSplitter.h>
#endif
#include <qlabel.h>
#include <qcombobox.h>

class PwMView;
class ListViewPwM;
class CommentBox;
class PWMPrefs;

class PwMViewStyle_0 : public QObject
{
	Q_OBJECT
public:
	PwMViewStyle_0(PwMView *view);
	~PwMViewStyle_0();

	ListViewPwM * getLv()
			{ return lv; }
	CommentBox * getCommentBox()
			{ return commentBox; }

	/** returns the currently selected category */
	QString getCurrentCategory()
			{ return categoriesCombo->currentText(); }
	/** add Category to the view */
	void addCategory(const QString &cat)
			{ categoriesCombo->insertItem(cat); }
	/** delete Category from view */
	void delCategory(const QString &cat);
	/** delete all categories from view */
	void delAllCategories()
			{ categoriesCombo->clear(); }
	/** select the specified category */
	void selectCategory(const QString &cat);
	/** returns the number of categories in this view.
	  * This value dosn't say anything about the number of
	  * categories in the document.
	  */
	int numCategories()
			{ return categoriesCombo->count(); }
	/** resize the view */
	void resize(const QSize &size)
			{ vbox1->resize(size); }

	//US ENH: I need a place to load the view dependend settings. Eg. splittersize
	void restoreSettings(PWMPrefs* prefs);
	void saveSettings(PWMPrefs* prefs);

public slots:
    void toggleSplitter();
protected:
	/** main list view */
	ListViewPwM *lv;
	/** categories combo-box */
	QComboBox *categoriesCombo;
	/** title string for the categories combo or list box */
	QLabel *categoriesTitle;
	/** hbox1 for widget style */
	QHBox *hbox1;
	/** vbox1 for widget style */
	QVBox *vbox1;
	/** splitter for commentTextEdit */
#ifndef PWM_EMBEDDED	
	QSplitter *splitter1;
#else
	KDGanttMinimizeSplitter * splitter1;
#endif
	/** push button to change the category name */
	QPushButton *renCatButton;
	/** push button to delete the category */
	QPushButton *delCatButton;
	/** comment box */
	CommentBox *commentBox;
};

#endif
