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
 * $Id: genpasswd.cpp,v 1.2 2006/02/24 19:41:08 zautrix Exp $
 **************************************************************************/  

#include "genpasswd.h"
#include "pwmexception.h"
#include "randomizer.h"
#include "globalstuff.h"


/* how often can a char of the same charset be reused in order */
#define FILTER_MAX_CHARSET_REUSE	3
/* re-randomize all charsets on every iteration (0/1) */
#define RERAND_CHARSET			0


struct staticCharsetStruct
{
	const char *lower;
	const char *upper;
	const char *num;
	const char *special;
	const char *blank;
};

static struct staticCharsetStruct staticCharset = {
	"abcdefghijklmnopqrstuvwxyz",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
	"0123456789",
	"!\"§$%&/()=?,.-;:_+",
	" "
};


GenPasswd::GenPasswd()
 : length (8)
 , useFilter (true)
{
	dynCharset.setAutoDelete(true);
}

void GenPasswd::setCharset(bool lower,
			   bool upper,
			   bool num,
			   bool special,
			   bool blank,
			   QString user)
{
	unsigned int sanityCheck = 0;
	dynCharset_element *tmpElement;
	dynCharset.clear();
	if (lower) {
		tmpElement = new dynCharset_element;
		tmpElement->refCnt = 0;
		tmpElement->data = staticCharset.lower;
		dynCharset.append(tmpElement);
		++sanityCheck;
	}
	if (upper) {
		tmpElement = new dynCharset_element;
		tmpElement->refCnt = 0;
		tmpElement->data = staticCharset.upper;
		dynCharset.append(tmpElement);
		++sanityCheck;
	}
	if (num) {
		tmpElement = new dynCharset_element;
		tmpElement->refCnt = 0;
		tmpElement->data = staticCharset.num;
		dynCharset.append(tmpElement);
		++sanityCheck;
	}
	if (special) {
		tmpElement = new dynCharset_element;
		tmpElement->refCnt = 0;
		tmpElement->data = staticCharset.special;
		dynCharset.append(tmpElement);
		++sanityCheck;
	}
	if (blank) {
		tmpElement = new dynCharset_element;
		tmpElement->refCnt = 0;
		tmpElement->data = staticCharset.blank;
		dynCharset.append(tmpElement);
	}
	if (!user.isEmpty()) {
		tmpElement = new dynCharset_element;
		tmpElement->refCnt = 0;
		tmpElement->data = user;
		dynCharset.append(tmpElement);
		if (likely(user.length() >= 2))
			++sanityCheck;
	}
	BUG_ON(!sanityCheck);
	rndDynCharset();
}

void GenPasswd::rndDynCharset()
{
	QString tmpData;
	int pos;
	Randomizer *rnd = Randomizer::obj();
	// QPtrList<dynCharset_element>::iterator is not available in QT-3.1
	unsigned int i, cnt = dynCharset.count();
	dynCharset_element *p;
	for (i = 0; i < cnt; ++i) {
		p = dynCharset.at(i);
		PWM_ASSERT(p);
		tmpData = QString::null;
		while (p->data.length()) {
			pos = rnd->genRndInt() % p->data.length();
			tmpData.append(p->data.at(pos));
			p->data.remove(pos, 1);
		}
		p->data = tmpData;
	}
}

QString GenPasswd::gen()
{
	BUG_ON(dynCharset.count() <= 0);
	BUG_ON(length < 1);
	dynCharset_element *curCharset;
	QString ret;
	int i;
	for (i = 0; i < length; ++i) {
		curCharset = selectNextCharset();
#if RERAND_CHARSET != 0
		rndDynCharset();
#endif // RERAND_CHARSET
		ret += genNewRandom(curCharset);
	}
	return ret;
}

GenPasswd::dynCharset_element * GenPasswd::selectNextCharset()
{
	dynCharset_element *ret;
	int numCharsets = dynCharset.count();
	BUG_ON(numCharsets <= 0);
	if (numCharsets == 1)
		return dynCharset.at(0);
	Randomizer *rnd = Randomizer::obj();
	if (useFilter) {
		// find out which charsets are allowed (filtering)
		QPtrList<dynCharset_element> allowedCharsets;
		// QPtrList<dynCharset_element>::iterator is not available in QT-3.1
		unsigned int i, cnt = dynCharset.count();
		dynCharset_element *p;
		for (i = 0; i < cnt; ++i) {
			p = dynCharset.at(i);
			PWM_ASSERT(p);
			if (p->refCnt < FILTER_MAX_CHARSET_REUSE) {
				allowedCharsets.append(p);
			} else {
				p->refCnt = 0;
			}
		}
		int numAllowedCharsets = allowedCharsets.count();
		BUG_ON(numAllowedCharsets <= 0);
		// now get a random charset out of the allowed
		unsigned int randomPos = rnd->genRndUInt() % numAllowedCharsets;
		ret = allowedCharsets.at(randomPos);
		ret->refCnt++;
		return ret;
	}
	// all charsets are allowed here (no filtering). Get a random.
	unsigned int randomPos = rnd->genRndUInt() % numCharsets;
	ret = dynCharset.at(randomPos);
	return ret;
}

QChar GenPasswd::genNewRandom(const dynCharset_element *charset)
{
	Randomizer *rnd = Randomizer::obj();
	int pos = rnd->genRndInt() % charset->data.length();
	return charset->data.at(pos);
}
