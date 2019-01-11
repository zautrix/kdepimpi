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
 * $Id: pwmviewstyle_0.cpp,v 1.6 2005/03/03 14:48:41 zautrix Exp $
 **************************************************************************/  

#include "pwmviewstyle_0.h"
#include "pwmview.h"
#include "listviewpwm.h"
#include "commentbox.h"

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include "pwmprefs.h"

PwMViewStyle_0::PwMViewStyle_0(PwMView *view)
 : QObject()
{
	vbox1 = new QVBox(view);
	vbox1->setSpacing(0);
	hbox1 = new QHBox(vbox1);
	hbox1->setSpacing(0);
	categoriesTitle = new QLabel(hbox1);
	categoriesTitle->setText(i18n("Category:"));
	categoriesCombo = new QComboBox(hbox1);
	categoriesCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,QSizePolicy::Preferred ));
	renCatButton = new QPushButton( hbox1);
    renCatButton->setPixmap (KGlobal::iconLoader()->loadIcon("edit", KIcon::Small) );
    int maxsi = renCatButton->sizeHint().height()+4;
    renCatButton->setMinimumSize( maxsi, maxsi );
	delCatButton = new QPushButton( hbox1);
    delCatButton->setPixmap (KGlobal::iconLoader()->loadIcon("editdelete", KIcon::Small) );
    maxsi = delCatButton->sizeHint().height()+4;
    delCatButton->setMinimumSize( maxsi, maxsi );
    delCatButton->setFocusPolicy( QWidget::NoFocus );
    renCatButton->setFocusPolicy( QWidget::NoFocus );
    
#ifndef PWM_EMBEDDED
	splitter1 = new QSplitter(vbox1);
	splitter1->setOrientation(Qt::Vertical);
#else
	splitter1 = new KDGanttMinimizeSplitter( Qt::Vertical, vbox1);
	splitter1->setMinimizeDirection ( KDGanttMinimizeSplitter::Down );

	//US  topLayout->addWidget(mMiniSplitter );
#endif
	lv = new ListViewPwM(splitter1);
	commentBox = new CommentBox(splitter1);
	// set sizes and styles
	commentBox->resize(commentBox->size().width(), 60);
	categoriesTitle->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	// connections
	connect(categoriesCombo, SIGNAL(activated(int)),
		view, SLOT(shiftToView()));
	connect(renCatButton, SIGNAL(clicked()),
		view, SLOT(renCatButton_slot()));
	connect(delCatButton, SIGNAL(clicked()),
		view, SLOT(delCatButton_slot()));
	connect(lv, SIGNAL(toggleOverview()),
		this, SLOT(toggleSplitter()));
}

PwMViewStyle_0::~PwMViewStyle_0()
{
	delete vbox1;
}
void PwMViewStyle_0::toggleSplitter()
{ 
    
    QValueList<int> si = splitter1->sizes();
    splitter1->toggle();
    QValueList<int> si2 = splitter1->sizes();
    //qDebug("PwMViewStyle_0::toggleSplitter() %d %d %d %d", si[0],si[1],si2[0],si2[1] );
    if ( si[0] == si2[0] && si[1] == si2[1] && si2[1] == 1 ) {
        int diff = si[0]/2;
        if ( diff > 200 )
            diff = 200;
        si[0] -= diff;
        si[1] += diff;
        splitter1->toggle();
        splitter1->setSizes( si );
    }

}

void PwMViewStyle_0::delCategory(const QString &cat)
{
	PWM_ASSERT(categoriesCombo);
	int i, count = categoriesCombo->count();
	for (i = 0; i < count; ++i) {
		if (categoriesCombo->text(i) == cat) {
			categoriesCombo->removeItem(i);
			return;
		}
	}
	BUG();
}

void PwMViewStyle_0::selectCategory(const QString &cat)
{
	PWM_ASSERT(categoriesCombo);
	int i, count = categoriesCombo->count();
	for (i = 0; i < count; ++i) {
		if (categoriesCombo->text(i) == cat) {
			categoriesCombo->setCurrentItem(i);
			return;
		}
	}
	// fall back to 0
	categoriesCombo->setCurrentItem(0);
}


//US ENH: I need a place to load the view dependend settings. Eg. splittersize
void PwMViewStyle_0::restoreSettings(PWMPrefs* prefs)
{
  //load and store the size of the listviewcolumns
  lv->restoreLayout(prefs->getConfig(), "listview");
  splitter1->setSizes( prefs->mCommentSplitter );
  commentBox->setFont( prefs->mViewFont );
}

//US ENH: I need a place to load the view dependend settings. Eg. splittersize
void PwMViewStyle_0::saveSettings(PWMPrefs* prefs)
{
  //store the size of the listviewcolumns
  lv->saveLayout(prefs->getConfig(), "listview");
  prefs->mCommentSplitter = splitter1->sizes();

}
