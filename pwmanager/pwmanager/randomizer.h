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
 * $Id: randomizer.h,v 1.4 2006/02/24 19:41:09 zautrix Exp $
 **************************************************************************/  

#ifndef __RANDOMIZER_H
#define __RANDOMIZER_H

#include "pwmexception.h"
#include "globalstuff.h"

#ifndef PWM_EMBEDDED
#include <qmutex.h>
#endif

#include <string>

using std::string;

class QFile;

/** Randomizer to get random values.
  * This class is thread-safe.
  * You should always use the instance returned by
  * obj() to use it.
  */
class Randomizer
{
public:
	Randomizer();
	~Randomizer();

	static Randomizer * obj()
		{
			PWM_ASSERT(rndObj);
			return rndObj;
		}
	static void init()
		{
			PWM_ASSERT(!rndObj);
			rndObj = new Randomizer;
		}
	static void cleanup()
		{
			delete_ifnot_null(rndObj);
		}

	/** generate random char */
	char genRndChar();
	/** generate random int */
	int genRndInt();
	/** generate a random unsigned int */
	unsigned int genRndUInt();

	/** returns a buffer with random data */
	string genRndBuf(size_t len);
	/** returns a buffer with random data */
	void genRndBuf(unsigned char *buf, size_t len);

protected:
	/** random-device-node (if available. Otherwise NULL) */
	QFile *rndDev;
#ifndef PWM_EMBEDDED
	/** mutex for accessing the public functions thread-save */
	QMutex mutex;
#endif
	/** seed value for fallback - rand_r() */
	unsigned int seed;
	/** static Randomizer object returned by obj() */
	static Randomizer *rndObj;
};

#endif // __RANDOMIZER_H
