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
 * $Id: pwmviewstyle_1.h,v 1.3 2005/03/03 13:42:16 zautrix Exp $
 **************************************************************************/  

#ifndef PWMVIEWSTYLE_1_H
#define PWMVIEWSTYLE_1_H

#include <qvbox.h>

#ifndef PWM_EMBEDDED
#include <qtextedit.h>
#include <qsplitter.h>
#else
#include <qmultilineedit.h>
#include <KDGanttMinimizeSplitter.h>
#endif

#include <qlabel.h>
#include <qlistbox.h>
#include <qpopupmenu.h>

class PwMView;
class ListViewPwM;
class CommentBox;
class PWMPrefs;

class PwMViewStyle_1 : public QObject
{
	Q_OBJECT
public:
	PwMViewStyle_1(PwMView *view);
	~PwMViewStyle_1();

	ListViewPwM * getLv()
			{ return lv; }
	CommentBox * getCommentBox()
			{ return commentBox; }

	/** returns the currently selected category */
	QString getCurrentCategory()
			{ return categoriesList->currentText(); }
	/** add Category to the view */
	void addCategory(const QString &cat)
			{ categoriesList->insertItem(cat); }
	/** delete Category from view */
	void delCategory(const QString &cat);
	/** delete all categories from view */
	void delAllCategories()
			{ categoriesList->clear(); }
	/** select the specified category */
	void selectCategory(const QString &cat);
	/** returns the number of categories in this view.
	  * This value dosn't say anything about the number of
	  * categories in the document.
	  */
	int numCategories()
			{ return categoriesList->count(); }
	/** resize the view */
	void resize(const QSize &size)
			{ splitter->resize(size); }

	//US ENH: I need a place to load the view dependend settings. Eg. splittersize
	void restoreSettings(PWMPrefs* prefs);
	void saveSettings(PWMPrefs* prefs);

public slots:
    void toggleSplitter();
protected slots:
	/** user clicked right button in category list */
	void catRightClick(QListBoxItem *item, const QPoint &point);

protected:
	/** main list view */
	ListViewPwM *lv;
#ifndef PWM_EMBEDDED
	/** main splitter widget */
	QSplitter *splitter;
	/** commentTextEdit splitter */
	QSplitter *splitter2;
#else
	/** main splitter widget */
	KDGanttMinimizeSplitter *splitter;
	/** commentTextEdit splitter */
	KDGanttMinimizeSplitter *splitter2;
#endif

	/** categories list-box */
	QListBox *categoriesList;
	/** title string for the categories combo or list box */
	QLabel *categoriesTitle;
	/** hbox1 for widget style */
	QVBox *vbox1;
	/** text-edit to display the comment */
	CommentBox *commentBox;
	/** category list context menu */
	QPopupMenu *catCtxMenu;
};

#endif
