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
 * This file is originaly based on version 1.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: randomizer.cpp,v 1.4 2004/10/20 08:28:44 zautrix Exp $
 **************************************************************************/  

#include "randomizer.h"
#include "pwmexception.h"

#include <qfile.h>

#include <kapplication.h>

#include <stdlib.h>
#include <time.h>


#ifdef PWM_EMBEDDED

#ifndef Q_LONG
#define Q_LONG long
#endif

#endif //PWM_EMBEDDED
#ifdef _WIN32_
int rand_r( uint * seed ) {
  srand(*seed);
  return rand();
}
#endif
Randomizer * Randomizer::rndObj (0);

Randomizer::Randomizer()
{
	rndDev = new QFile;
	seed = time(0);

#if 1 // set to 0 to test rand_r() fallback

	// probe for /dev/urandom
	rndDev->setName("/dev/urandom");
	if (rndDev->exists() &&
	    rndDev->open(IO_ReadOnly)) {
		printDebug("Randomizer: using /dev/urandom");
		return;
	}

	// probe for /dev/random
	rndDev->setName("/dev/random");
	if (rndDev->exists() &&
	    rndDev->open(IO_ReadOnly)) {
		printDebug("Randomizer: using /dev/random");
		return;
	}

	// probe for EGD
	char *fn = getenv("RANDFILE");
	if (fn) {
		rndDev->setName(fn);
		if (rndDev->exists() &&
		    rndDev->open(IO_ReadOnly)) {
			printDebug(string("Randomizer: using $RANDFILE \"")
				   + fn
				   + "\" (aka EGD)");
			return;
		}
	}
#endif

	/* no secure randomizer found.
	 * Fall back to stdlib randomizer.
	 */
	delete_and_null(rndDev);
	printWarn("neither /dev/*random nor EGD found! "
		  "Falling back to insecure rand_r()!");
}

Randomizer::~Randomizer()
{
#ifndef PWM_EMBEDDED
	while (mutex.locked()) {
		/* wait for the mutex to unlock.
		 * Don't block the GUI here, so processEvents()
		 */
		kapp->processEvents();
	}
#endif
	if (rndDev) {
		rndDev->close();
		delete rndDev;
	}
}

char Randomizer::genRndChar()
{
	char ret;
#ifndef PWM_EMBEDDED 
	mutex.lock();
#endif
	if (rndDev) {
		/* we have a file which provides random data.
		 * Simply read it.
		 */
		ret = rndDev->getch();
	} else {
		/* fall back to rand_r() */
		ret = rand_r(&seed) % 0xFF;
	}
#ifndef PWM_EMBEDDED
	mutex->unlock();
#endif
	return ret;
}

int Randomizer::genRndInt()
{
	int ret;
#ifndef PWM_EMBEDDED
	mutex->lock();
#endif
	if (rndDev) {
		if (sizeof(int) == 4) {
			(reinterpret_cast<char *>(&ret))[0] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[1] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[2] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[3] = rndDev->getch();
		} else if (sizeof(int) == 8) {
			(reinterpret_cast<char *>(&ret))[0] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[1] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[2] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[3] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[4] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[5] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[6] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[7] = rndDev->getch();
		} else {
			printWarn(string(__FILE__) + ":" + tostr(__LINE__)
				  + ":  sizeof(int) != 4 && sizeof(int) != 8");
			ret = rand_r(&seed);
		}
	} else {
		ret = rand_r(&seed);
	}
#ifndef PWM_EMBEDDED 
	mutex->unlock();
#endif
	return ret;
}

unsigned int Randomizer::genRndUInt()
{
	unsigned int ret;
#ifndef PWM_EMBEDDED
	mutex->lock();
#endif
	if (rndDev) {
		if (sizeof(unsigned int) == 4) {
			(reinterpret_cast<char *>(&ret))[0] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[1] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[2] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[3] = rndDev->getch();
		} else if (sizeof(unsigned int) == 8) {
			(reinterpret_cast<char *>(&ret))[0] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[1] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[2] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[3] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[4] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[5] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[6] = rndDev->getch();
			(reinterpret_cast<char *>(&ret))[7] = rndDev->getch();
		} else {
			printWarn(string(__FILE__) + ":" + tostr(__LINE__)
				  + ":  sizeof(unsigned int) != 4 && sizeof(unsigned int) != 8");
			ret = rand_r(&seed);
		}
	} else {
		ret = rand_r(&seed);
	}
#ifndef PWM_EMBEDDED
	mutex->unlock();
#endif
	return ret;
}

void Randomizer::genRndBuf(unsigned char *buf, size_t len)
{
#ifndef PWM_EMBEDDED
	mutex->lock();
#endif
	if (rndDev) {
		Q_LONG n;
		n = rndDev->readBlock(reinterpret_cast<char *>(buf), len);
		WARN_ON(n != static_cast<Q_LONG>(len));
	} else {
		size_t i;
		for (i = 0; i < len; ++i)
			buf[i] = rand_r(&seed) % 0xFF;
	}
#ifndef PWM_EMBEDDED 
	mutex->unlock();
#endif
}

string Randomizer::genRndBuf(size_t len)
{
	string ret;
	unsigned char *buf;
	buf = new unsigned char[len];
	genRndBuf(buf, len);
	ret.assign(reinterpret_cast<const char *>(buf), len);
	return ret;
}
