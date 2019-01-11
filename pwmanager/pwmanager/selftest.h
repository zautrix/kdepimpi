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

#ifndef __SELFTEST_H
#define __SELFTEST_H

#include <qthread.h>
#include <qtimer.h>
#include <qobject.h>

// timeout in seconds
#define SCHEDULE_TIMEOUT	3

/* internal wrapper to workaround MOC issues */
class __SelfTest : public QThread
		 , public QObject
{
public:
	__SelfTest() {}
};

/** PwManager algorithm selftest */
class SelfTest : public __SelfTest
{
	Q_OBJECT
public:
	SelfTest();
	~SelfTest();

	/** schedule a new test. Only one test can run at once! */
	static void schedule();
	/** cancel the running test (if there is one) */
	static void cancel();

protected:
	/** start the sched timer */
	void startTimer()
			{ schedTimer.start(SCHEDULE_TIMEOUT * 1000, true); }
	/** stop the sched timer */
	void stopTimer()
			{ schedTimer.stop(); }

protected slots:
	void doSelfTest()
			{ start(); }
	void doCancel();

protected:
	/** print the "failed" message and exit the app */
	void failed(const char *algo);
	/** worker thread */
	void run();

protected:
	/** schedule timer */
	QTimer schedTimer;
};

#endif
