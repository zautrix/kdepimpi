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
 * $Id: findwndimpl.h,v 1.2 2004/09/24 19:51:43 ulf69 Exp $
 **************************************************************************/  

#ifndef FINDWNDIMPL_H
#define FINDWNDIMPL_H

#ifndef PWM_EMBEDDED
#include "findwnd.h"
#else
#include "findwnd_emb.h"
#endif

class PwMView;

/** "add" Window */
class FindWndImpl : public findWnd
{
	Q_OBJECT
public:
	FindWndImpl(PwMView *_parent);
	~FindWndImpl();

signals:
	void foundAt(int index);

public slots:
	/** find button pressed */
	void findButton_slot();
	/** close button pressed */
	void closeButton_slot();
	/** selection of one of the radio buttons changed */
	void selectionChanged_slot();

#ifdef PWM_EMBEDDED
	virtual void slotOk();
#endif

protected:
	/** parent view */
	PwMView *parent;
	/** entry found at */
	unsigned int fAt;
	/** reference value */
	unsigned int refVal;
	/** current position in the found-items-vector */
	int currFoundPos;
	/** the number of entries in the current category */
	unsigned int numEntries;
	/** index number of the current category */
	unsigned int catIndex;
};

#endif
