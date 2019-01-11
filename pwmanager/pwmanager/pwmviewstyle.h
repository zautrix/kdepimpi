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
 * $Id: pwmviewstyle.h,v 1.3 2005/03/07 16:51:27 zautrix Exp $
 **************************************************************************/  

#ifndef PWMVIEWSTYLE_H
#define PWMVIEWSTYLE_H

#include <qwidget.h>
#include <qsplitter.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#ifndef PWM_EMBEDDED
#include <qtextedit.h>
#else
#include <qmultilineedit.h>
#endif

#include <qlabel.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qsize.h>

class ListViewPwM;
class PwMViewStyle_0;
class PwMViewStyle_1;
class PwMView;
class QListViewItem;
class CommentBox;

class PwMViewStyle : public QWidget
{
	Q_OBJECT

public:
	enum style_t
	{
		style_notset	= -1,
		style_0		= 0,
		style_1		= 1
	};

public:
	PwMViewStyle(QWidget *parent = 0, const char *name = 0);
	~PwMViewStyle();

	/** initialize a new style */
	void initStyle(style_t style);
	void initStyle(int style)
			{ initStyle(static_cast<style_t>(style)); }
	/** returns the currently used style */
	style_t getCurStyle()
			 { return curStyle; }
	void setView(PwMView *view)
			{ v = view; }

	/** returns the currently selected category */
	QString getCurrentCategory();

protected:
	/** add Category to the view */
	void addCategory(const QString &cat);
	/** delete Category from view */
	void delCategory(const QString &cat);
	/** delete all categories from view */
	void delAllCategories();
	/** select the specified category */
	void selectCategory(const QString &cat);
	/** returns the number of categories in this view.
	  * This value dosn't say anything about the number of
	  * categories in the document.
	  */
	int numCategories();
	/** resize the view */
	void resizeView(const QSize &size);
	ListViewPwM *lv;
	CommentBox *commentBox;

protected slots:
    void editPassWord( QListViewItem * );

private:
	/** which style has the view?
	  * KListBox on the left,
	  * or QComboBox on the top?
	  */
	style_t curStyle;

	PwMViewStyle_0 *s0;
	PwMViewStyle_1 *s1;
	PwMView *v;
 signals:
    void editPW();
    void insertPW();
    void deletePW();

};
#endif
