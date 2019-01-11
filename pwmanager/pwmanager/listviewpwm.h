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
 * $Id: listviewpwm.h,v 1.3 2005/03/07 16:51:26 zautrix Exp $
 **************************************************************************/  

#ifndef __LISTVIEW_H
#define __LISTVIEW_H

#include <klistview.h>

/** PwManager implementation of the list view.
  * Derived from KListView.
  */
class ListViewPwM : public KListView
{
	Q_OBJECT
public:
	ListViewPwM(QWidget *parent = 0, const char *name = 0);

signals:
	void layoutChanged();
    void toggleOverview();
    void insertPW();
    void deletePW(   );

protected:
	virtual bool event(QEvent *e);
};

class ListViewItemPwM : public QCheckListItem
{
public:
	ListViewItemPwM(QListView *parent);

protected:
	void paintCell(QPainter *p, const QColorGroup &cg,
		       int column, int width, int align);

protected:
	/** pixmap to display for an item with state "on" */
	static QPixmap *onPix;
	/** pixmap to display for an item with state "off" */
	static QPixmap *offPix;
};

#endif
