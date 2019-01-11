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
 * $Id: pwmviewstyle.cpp,v 1.6 2005/03/07 16:51:27 zautrix Exp $
 **************************************************************************/  

#include "pwmviewstyle.h"
#include "pwmexception.h"
#include "pwmviewstyle_0.h"
#include "pwmviewstyle_1.h"
#include "listviewpwm.h"
#include "pwmview.h"
#include "commentbox.h"
#ifndef PWM_EMBEDDED
#include "configuration.h"
#else
#include "pwmprefs.h"
#endif

PwMViewStyle::PwMViewStyle(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	curStyle = style_notset;
	s0 = 0;
	s1 = 0;
}

PwMViewStyle::~PwMViewStyle()
{
	//US ENH : store the size of the listviewcolumns
        switch (curStyle)
	  {
	  case style_0:
	    s0->saveSettings(PWMPrefs::instance());
	    break;
	  case style_1:
	    s1->saveSettings(PWMPrefs::instance());
	    break;
	  default:
		BUG();
	  }


	PWMPrefs::instance()->writeConfig();

	delete_ifnot_null(s0);
	delete_ifnot_null(s1);
}

void PwMViewStyle::editPassWord( QListViewItem * i )
{
    if ( !i )
        return;
    emit editPW();
}
void PwMViewStyle::initStyle(style_t style)
{
	printDebug(string("initializing style ") + tostr(style));
	bool wasMaximized = v->isMaximized();
	if (v->isVisible())
		v->hide();
	switch (style) {
	case style_0:
		delete_ifnot_null(s0);
		delete_ifnot_null(s1);
		s0 = new PwMViewStyle_0(v);
		lv = s0->getLv();
		commentBox = s0->getCommentBox();
		break;
	case style_1:
		delete_ifnot_null(s0);
		delete_ifnot_null(s1);
		s1 = new PwMViewStyle_1(v);
		lv = s1->getLv();
		commentBox = s1->getCommentBox();
		break;
	default:
		BUG();
		return;
	}
	curStyle = style;
	connect(lv, SIGNAL(pressed(QListViewItem *)),
		v, SLOT(handleToggle(QListViewItem *)));
	connect(lv, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		v, SLOT(handleRightClick(QListViewItem *, const QPoint &, int)));
	connect(lv, SIGNAL(clicked(QListViewItem *)),
		v, SLOT(refreshCommentTextEdit(QListViewItem *)));
	connect(lv, SIGNAL(returnPressed(QListViewItem *)),
		this, SLOT(editPassWord(QListViewItem *)));
	connect(lv, SIGNAL(doubleClicked(QListViewItem *)),
		this, SLOT(editPassWord(QListViewItem *)));
	connect(lv, SIGNAL(insertPW()),
		this, SIGNAL( insertPW() ));
	connect(lv, SIGNAL(deletePW()),
		this, SIGNAL( deletePW() ));

	lv->addColumn(i18n("Description"), 180);
	lv->addColumn(i18n("Username"), 150);
	lv->addColumn(i18n("Password"), 150);
	lv->addColumn(i18n("URL"), 180);
	lv->addColumn(i18n("Launcher"), 120);
	v->tmpReEnableSort();

	//US ENH : load the size of the listviewcolumns
	switch (curStyle)
	  {
	  case style_0:
	    s0->restoreSettings(PWMPrefs::instance());
	    break;
	  case style_1:
	    s1->restoreSettings(PWMPrefs::instance());
	    break;
	  default:
		BUG();
	  }

	resizeView(v->size());
	v->updateView();
	if (wasMaximized) {
		v->showMaximized();
	} else {
		v->show();
	}
	connect(lv, SIGNAL(layoutChanged()),
		v, SLOT(reorgLp()));
}

void PwMViewStyle::resizeView(const QSize &size)
{
	switch (curStyle) {
	case style_0:
		PWM_ASSERT(s0);
		s0->resize(size);
		return;
	case style_1:
		PWM_ASSERT(s1);
		s1->resize(size);
		return;
	default:
		BUG();
	}
}

QString PwMViewStyle::getCurrentCategory()
{
	switch (curStyle) {
	case style_0:
		PWM_ASSERT(s0);
		return s0->getCurrentCategory();
	case style_1:
		PWM_ASSERT(s1);
		return s1->getCurrentCategory();
	default:
		BUG();
	}
	return "";
}

void PwMViewStyle::addCategory(const QString &cat)
{
	switch (curStyle) {
	case style_0:
		PWM_ASSERT(s0);
		s0->addCategory(cat);
		return;
	case style_1:
		PWM_ASSERT(s1);
		s1->addCategory(cat);
		return;
	default:
		BUG();
	}
}

void PwMViewStyle::delCategory(const QString &cat)
{
	switch (curStyle) {
	case style_0:
		PWM_ASSERT(s0);
		s0->delCategory(cat);
		return;
	case style_1:
		PWM_ASSERT(s1);
		s1->delCategory(cat);
		return;
	default:
		BUG();
	}
}

void PwMViewStyle::delAllCategories()
{
	switch (curStyle) {
	case style_0:
		PWM_ASSERT(s0);
		s0->delAllCategories();
		return;
	case style_1:
		PWM_ASSERT(s1);
		s1->delAllCategories();
		return;
	default:
		BUG();
	}
}

void PwMViewStyle::selectCategory(const QString &cat)
{
	switch (curStyle) {
	case style_0:
		PWM_ASSERT(s0);
		s0->selectCategory(cat);
		return;
	case style_1:
		PWM_ASSERT(s1);
		s1->selectCategory(cat);
		return;
	default:
		BUG();
	}
}

int PwMViewStyle::numCategories()
{
	switch (curStyle) {
	case style_0:
		PWM_ASSERT(s0);
		return s0->numCategories();
	case style_1:
		PWM_ASSERT(s1);
		return s1->numCategories();
	default:
		BUG();
	}
	return 0;
}

#ifndef PWM_EMBEDDED
#include "pwmviewstyle.moc"
#endif
