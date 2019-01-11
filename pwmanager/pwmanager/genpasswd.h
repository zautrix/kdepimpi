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
 * $Id: genpasswd.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef __GENPASSWD_H
#define __GENPASSWD_H

#include <qstring.h>
#include <qptrlist.h>

/** internal password generator of PwManager */
class GenPasswd
{
protected:
	struct dynCharset_element
	{
		/** charset data */
		QString data;
		/** reference counter for the filter */
		unsigned int refCnt;
	};

public:
	GenPasswd();

	/** set the charset to use */
	void setCharset(bool lower,
			bool upper,
			bool num,
			bool special,
			bool blank,
			QString user);
	/** set the password length */
	void setLen(int len)
			{ length = len; }
	/** use the filter? */
	void setUseFilter(bool use)
			{ useFilter = use; }
	/** start to generate a new password and return it.
	  * Returns an empty string on error.
	  */
	QString gen();

protected:
	/** randomize the dynamic charset */
	void rndDynCharset();
	/** select the next charset (based on useFilter) */
	dynCharset_element * selectNextCharset();
	/** generate a new random char from the given charset */
	QChar genNewRandom(const dynCharset_element *charset);

protected:
	/** password length to generate */
	int length;
	/** use the filter? */
	bool useFilter;
	/** dynamic charset used for generating the password */
	QPtrList<dynCharset_element> dynCharset;
};

#endif // __GENPASSWD_H
