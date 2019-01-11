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

#include "selftest.h"
#include "pwmexception.h"
#include "sha1.h"
#include "blowfish.h"
#include "base64.h"

#include <pthread.h>


static SelfTest *st = 0;
static pthread_mutex_t st_mutex = PTHREAD_MUTEX_INITIALIZER;


SelfTest::SelfTest()
{
	connect(&schedTimer, SIGNAL(timeout()),
		this, SLOT(doSelfTest()));
}

SelfTest::~SelfTest()
{
}

void SelfTest::run()
{
//	printDebug("running self-test...");
	if (unlikely(!Sha1::selfTest())) {
		failed("SHA1");
		return;
	}
	if (unlikely(!Blowfish::selfTest())) {
		failed("BLOWFISH");
		return;
	}
	if (unlikely(!Base64::selfTest())) {
		failed("BASE64");
		return;
	}
//	printDebug("self-test done.");
	if (!pthread_mutex_trylock(&st_mutex)) {
		/* only cancel (delete) this thread, if there's currently
		 * noone else doing this job. Otherwise we will deadlock.
		 * we use a timer here, to do some kind of context-switch.
		 */
		QTimer::singleShot(0, st, SLOT(doCancel()));
	}
}

void SelfTest::failed(const char *algo)
{
	string msg("\n\n\n"
		   "PwManager FATAL ERROR:\n"
		   "Self-test for algorithm \"");
	msg += algo;
	msg += "\" failed!\n\n";
	msg += "It's likely to be either a PwManager BUG or "
	       "bad hardware in your machine. Please contact the "
	       "maintainer of PwManager for this issue.";
	std::cerr << msg << std::endl;
	::exit(1);
}

void SelfTest::schedule()
{
	if (pthread_mutex_lock(&st_mutex))
		return;
	if (st)
		goto out;
	st = new SelfTest;
	st->startTimer();
out:
	pthread_mutex_unlock(&st_mutex);
}

void SelfTest::cancel()
{
	if (pthread_mutex_lock(&st_mutex))
		return;
	if (!st) {
		pthread_mutex_unlock(&st_mutex);
		return;
	}
	st->doCancel();
}

void SelfTest::doCancel()
{
	st->stopTimer();
	st->wait();
	delete_and_null(st);
	pthread_mutex_unlock(&st_mutex);
}

#include "selftest.moc"
