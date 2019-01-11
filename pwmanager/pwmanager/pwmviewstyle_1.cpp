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
 * $Id: pwmviewstyle_1.cpp,v 1.9 2005/03/03 14:48:41 zautrix Exp $
 **************************************************************************/  

#include "pwmviewstyle_1.h"
#include "pwmview.h"
#include "listviewpwm.h"
#include "commentbox.h"

#include <klocale.h>
#include "pwmprefs.h"

#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif
#define INITIAL_CATEGORIES_WIDTH	100

PwMViewStyle_1::PwMViewStyle_1(PwMView *view)
 : QObject()
{
#ifndef PWM_EMBEDDED
	splitter = new QSplitter(view);
#else
	if ( QApplication::desktop()->width() > 240 ) {
	  splitter = new KDGanttMinimizeSplitter( Qt::Horizontal, view);
	  splitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Left );
	  //      topLayout = new QHBoxLayout( this );
	  //      mMiniSplitter = new KDGanttMinimizeSplitter( Qt::Horizontal, this);
	  //      mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Right );
	} else {
	  splitter = new KDGanttMinimizeSplitter( Qt::Vertical, view);
	  splitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Up );
	  //      topLayout = new QHBoxLayout( this );
	  //      mMiniSplitter = new KDGanttMinimizeSplitter( Qt::Vertical, this);
	  //      mMiniSplitter->setMinimizeDirection ( KDGanttMinimizeSplitter::Down );
	}
	//US  topLayout->addWidget(mMiniSplitter );
#endif

	vbox1 = new QVBox(splitter);
	categoriesTitle = new QLabel(vbox1);
	categoriesList = new QListBox(vbox1);
#ifndef PWM_EMBEDDED
	splitter2 = new QSplitter(splitter);
	splitter2->setOrientation(Qt::Vertical);
#else
	splitter2 = new KDGanttMinimizeSplitter( Qt::Vertical, splitter);
	splitter2->setMinimizeDirection ( KDGanttMinimizeSplitter::Down );

	//US  topLayout->addWidget(mMiniSplitter );
#endif
	lv = new ListViewPwM(splitter2);
	commentBox = new CommentBox(splitter2);
	// set sizes and styles
	//US	commentBox->resize(commentBox->size().width(), 60);
	QValueList<int> sizes;
#ifndef PWM_EMBEDDED
	sizes.push_back(INITIAL_CATEGORIES_WIDTH);
	sizes.push_back(view->height() - INITIAL_CATEGORIES_WIDTH);
#else
	sizes.append(INITIAL_CATEGORIES_WIDTH);
	sizes.append(view->height() - INITIAL_CATEGORIES_WIDTH);
#endif
	//US	splitter->setSizes(sizes);
	categoriesTitle->setAlignment(Qt::AlignHCenter);
#ifndef PWM_EMBEDDED
	categoriesTitle->setFrameShape(QFrame::MenuBarPanel);
#else
	//categoriesTitle->setFrameShape(QFrame::StyledPanel);
#endif
	categoriesTitle->setText(i18n("Category:"));
	catCtxMenu = new QPopupMenu(view);
	catCtxMenu->insertItem(i18n("&Rename"),
			       view, SLOT(renCatButton_slot()));
	catCtxMenu->insertItem(i18n("&Delete"),
			       view, SLOT(delCatButton_slot()));

#ifndef DESKTOP_VERSION
    QPEApplication::setStylusOperation( categoriesList->viewport(), QPEApplication::RightOnHold );
#endif

	// connections
	connect(categoriesList, SIGNAL(highlighted(int)),
		view, SLOT(shiftToView()));
	connect(categoriesList,
		SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		this,
		SLOT(catRightClick(QListBoxItem *, const QPoint &)));

	connect(lv, SIGNAL(toggleOverview()),
		this, SLOT(toggleSplitter()));
}

PwMViewStyle_1::~PwMViewStyle_1()
{
	delete catCtxMenu;
	delete splitter;
}
void PwMViewStyle_1::toggleSplitter()
{
    QValueList<int> si = splitter2->sizes();
    splitter2->toggle();
    QValueList<int> si2 = splitter2->sizes();
    //qDebug("PwMViewStyle_1::toggleSplitter() %d %d %d %d", si[0],si[1],si2[0],si2[1] );
    if ( si[0] == si2[0] && si[1] == si2[1] && si2[1] == 1 ) {
        int diff = si[0]/2;
        if ( diff > 200 )
            diff = 200;
        si[0] -= diff;
        si[1] += diff;
        splitter2->toggle();
        splitter2->setSizes( si );
    }
}

void PwMViewStyle_1::catRightClick(QListBoxItem *item, const QPoint &point)
{
	if (!item)
		return;
	catCtxMenu->move(point);
	catCtxMenu->show();
}

void PwMViewStyle_1::delCategory(const QString &cat)
{
	PWM_ASSERT(categoriesList);
	int i, count = categoriesList->count();
	for (i = 0; i < count; ++i) {
		if (categoriesList->text(i) == cat) {
			categoriesList->removeItem(i);
			return;
		}
	}
	BUG();
}

void PwMViewStyle_1::selectCategory(const QString &cat)
{
	PWM_ASSERT(categoriesList);
	int i, count = categoriesList->count();
	for (i = 0; i < count; ++i) {
		if (categoriesList->text(i) == cat) {
			categoriesList->setCurrentItem(i);
			return;
		}
	}
	// fall back to 0
	categoriesList->setCurrentItem(0);
}

//US ENH: I need a place to load the view dependend settings. Eg. splittersize
void PwMViewStyle_1::restoreSettings(PWMPrefs* prefs)
{
  //load and store the size of the listviewcolumns
  lv->restoreLayout(prefs->getConfig(), "listview");
  splitter->setSizes( prefs->mCategorySplitter );
  splitter2->setSizes( prefs->mCommentSplitter );
  commentBox->setFont( prefs->mViewFont );

}

//US ENH: I need a place to load the view dependend settings. Eg. splittersize
void PwMViewStyle_1::saveSettings(PWMPrefs* prefs)
{
  //store the size of the listviewcolumns
  lv->saveLayout(prefs->getConfig(), "listview");
  prefs->mCategorySplitter = splitter->sizes();
  prefs->mCommentSplitter = splitter2->sizes();

}


#ifndef PWM_EMBEDDED
#include "pwmviewstyle_1.moc"
#endif
