/*******************************************************************
 *  base64.h
 *   © Copyright 1995 John Halleck
 *   All Rights Reserved
 *
 *  ported to c++ by Michael Buesch <mbuesch@freenet.de>
 *
 * --ABSTRACT--  base64.h
 * Do the base 64 encoding as used by PEM and MIME.
 *
 * --KEYWORDS--  base64.h
 *
 * --CONTENTS--  base64.h
 * Date, Department, Author
 *    23nov1994, John Halleck
 * Revision history
 *    For each revision: Date, change summary, authorizing document,
 *    change department, section, author
 *    23nov1994, Initial Creation, John Halleck
 *    8apr1995, split library into hex and base64 libraries, John Halleck
 * Unit purpose
 *    (What does this do?)
 *    [Nothing]
 * External Units accessed
 *    Name, purpose, access summary
 *    [None]
 * Exceptions propagated by this unit
 *    [None]
 * Machine-dependencies
 *    Access type, purpose, and justification
 *    [None]
 * Compiler-dependencies
 *    [None]
 *********************************************************************/

/***************************************************************************
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
 * $Id: base64.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  


#ifndef __BASE64_H
#define __BASE64_H

#include <string>

using std::string;

class Base64
{
protected:
	struct Base64Ctx
	{
		int temp;	// Working value for input
		int bytes;	// which input byte we are working on
		string buf;	// Data buffer
	};

public:
	Base64() {}
	/** run algorithm self test */
	static bool selfTest();

	/** encode "data" */
	string encode(const string &data);
	/** decode "data" */
	string decode(const string &data);

protected:
	/** initialize the context */
	void initCtx();
	/** finalize the context */
	void encFinalizeCtx();
	/** finalize the context */
	void decFinalizeCtx();
	/** encode a character */
	void encodeChar(unsigned char c);
	/** decode a character */
	void decodeChar(char c);

protected:
	/** Base64 context */
	Base64Ctx ctx;
};

#endif // __BASE64_H
