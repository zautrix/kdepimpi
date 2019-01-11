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
 * $Id: spinforsignal.cpp,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#include "spinforsignal.h"
#include "pwmexception.h"

#ifndef PWM_EMBEDDED
#include <kapp.h>
#endif

#include <time.h>
#include <pthread.h> 

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


SpinForSignal::SpinForSignal()
 : QObject()
{
	doSpin = false;
}

void SpinForSignal::spinSleep()
{
	const struct timespec t = { 0, 100 };
	nanosleep(&t, 0);
}

void SpinForSignal::_spin()
{
	doSpin = true;
	printDebug("spinning for signal.");
	do {
		kapp->processEvents();
		spinSleep();
	} while (doSpin);
	printDebug("spinning stopped.");
}

void SpinForSignal::spin(uint32_t *u32, string *str)
{
	_spin();
	if (pthread_mutex_lock(&mutex)) {
		printError("spin(uint32_t *u32, string *str): pthread_mutex_lock failed");
		return;
	}
	*u32 = u32_storage;
	*str = str_storage;
	pthread_mutex_unlock(&mutex);
}

void SpinForSignal::u32_str_slot(uint32_t u32, const string &str)
{
	if (doSpin) {
		printDebug("ul_str_slot(unsigned long ul, const string &str)");
		u32_storage = u32;
		str_storage = str;
		doSpin = false;
	}
}

void SpinForSignal::cancelSpin()
{
	if (pthread_mutex_lock(&mutex)) {
		printError("cancelSpin(): pthread_mutex_lock failed");
		return;
	}
	printDebug("spinning cancelled.");
	u32_storage = 0;
	str_storage = "";
	doSpin = false;
	pthread_mutex_unlock(&mutex);
}

#include "spinforsignal.moc"
