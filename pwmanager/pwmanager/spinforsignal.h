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
 * $Id: spinforsignal.h,v 1.2 2004/10/23 17:05:42 zautrix Exp $
 **************************************************************************/  

#ifndef SPINFORSIGNAL_H
#define SPINFORSIGNAL_H

#include <qobject.h>

#ifndef _WIN32_
#include <stdint.h>
#endif
#include <string>
using std::string;

/** non-ui-blocking spin for a QT-signal */
class SpinForSignal : public QObject
{
	Q_OBJECT
public:
	SpinForSignal();
	~SpinForSignal() {}

	/** do spin for signal */
	void spin(uint32_t *u32, string *str);
	/** cancel spinning */
	void cancelSpin();

public slots:
	void u32_str_slot(uint32_t u32, const string &str);

protected:
	volatile bool doSpin;
	uint32_t u32_storage;
	string str_storage;

protected:
	inline void spinSleep();
	void _spin();
};

#endif
