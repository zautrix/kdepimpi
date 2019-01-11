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
 * $Id: listviewpwm.cpp,v 1.6 2005/03/07 16:51:26 zautrix Exp $
 **************************************************************************/  

#include "listviewpwm.h"
#include "pwmexception.h"
#include "pwmview.h"

#include <qpainter.h>
#include <qpixmap.h>

#include <kiconloader.h>

#ifdef PWM_EMBEDDED
#include <kglobal.h>
#endif 

ListViewPwM::ListViewPwM(QWidget *parent, const char *name)
 : KListView(parent, name)
{
//	setResizeMode(QListView::AllColumns);
  setAllColumnsShowFocus (true );
}

bool ListViewPwM::event(QEvent *e)
{
	if (e->type() == QEvent::LayoutHint)
		emit layoutChanged();
	if (e->type() == QEvent::KeyPress) {
        QKeyEvent* ke = (QKeyEvent*) e;
        if ( ke->key() == Qt::Key_Space) {
            emit toggleOverview();
            return true;
        }
        if ( ke->key() == Qt::Key_I || ke->key() == Qt::Key_N || ke->key() == Qt::Key_Insert) {
            emit insertPW();
            return true;
        }
        if ( ke->key() == Qt::Key_Delete || ke->key() == Qt::Key_Backspace) {
            emit deletePW();
            return true;
        }

    }
	return KListView::event(e);
}


QPixmap * ListViewItemPwM::onPix = 0;
QPixmap * ListViewItemPwM::offPix = 0;

ListViewItemPwM::ListViewItemPwM(QListView *parent)
 : QCheckListItem(parent, "", QCheckListItem::CheckBox)
{
	if (!onPix) {
		PWM_ASSERT(!offPix);
		KIconLoader* picons;
#ifndef PWM_EMBEDDED
		KIconLoader il;
		picons = &il;
#else
		picons = KGlobal::iconLoader();
#endif

		KIconLoader il;
#ifndef PWM_EMBEDDED
		static QPixmap onP(picons->loadIcon("button_ok", KIcon::Small));
#else
		static QPixmap onP(picons->loadIcon("decrypted", KIcon::Small));
#endif
		onPix = &onP;
		static QPixmap offP(picons->loadIcon("encrypted", KIcon::Small));
		offPix = &offP;
	}
}

void ListViewItemPwM::paintCell(QPainter *p, const QColorGroup &cg,
				int column, int width, int align)
{
  //  qDebug("ListViewItemPwM::paintCell column=%i", column);
	if (!p)
		return;
	//US BUG: 
	if (column != COLUMN_DESC) {
		QCheckListItem::paintCell(p, cg, column, width, align);
		return;
	}
	QPixmap *curPix = isOn() ? onPix : offPix;
	int pixSpace = curPix->width();
	pixSpace += 4;
#ifndef PWM_EMBEDDED
	QRect window(p->viewport());
	// clear the rectangle (we have to clear it first. see QT doc)
	p->eraseRect(0, 0, pixSpace, window.height());
	// now draw the pixmap
	int y = (height() - curPix->height()) / 2;
	p->drawPixmap(1, y, *curPix);
	window.moveLeft(pixSpace);
	p->setViewport(window);
#else
	p->eraseRect(0, 0, pixSpace, height());
	// now draw the pixmap
	int y = (height() - curPix->height()) / 2;
	p->drawPixmap(1, y, *curPix);
	p->translate( pixSpace, 0 );

#endif
	QListViewItem::paintCell(p, cg, column, width - pixSpace, align);

}

#ifndef PWM_EMBEDDED
#include "listviewpwm.moc"
#endif
