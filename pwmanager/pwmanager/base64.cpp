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

#include "base64.h"
#include "globalstuff.h"


static const char prtcode[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


string Base64::encode(const string &data)
{
	initCtx();
	string::const_iterator i = data.begin(),
			       end = data.end();
	while (i != end) {
		encodeChar(*i);
		++i;
	}
	encFinalizeCtx();
	return ctx.buf;
}

string Base64::decode(const string &data)
{
	initCtx();
	string::const_iterator i = data.begin(),
			       end = data.end();
	while (i != end) {
		decodeChar(*i);
		++i;
	}
	decFinalizeCtx();
	return ctx.buf;
}

void Base64::initCtx()
{
	ctx.temp = 0;
	ctx.bytes = 0;
	ctx.buf = "";
}

void Base64::encFinalizeCtx()
{
	/* flush the output side of things, by putting out the last characters */
	switch (ctx.bytes) {
	case 0:
		/* nothing in progress */
		break;
	case 2:
		ctx.buf.append(1, static_cast<char>(prtcode[ctx.temp << 2 & 0x3F]));
		ctx.buf.append(1, '=');
		break;
	case 1:
		ctx.buf.append(1, static_cast<char>(prtcode[ctx.temp << 4 & 0x3F]));
		ctx.buf.append(1, '=');
		ctx.buf.append(1, '=');
	}
}

void Base64::decFinalizeCtx()
{
	if (ctx.bytes == 1)
		decodeChar('A');
}

void Base64::encodeChar(unsigned char c)
{
	int result;

	/* Add this 8 bit byte to what we have...*/
	result = ctx.temp;
	result = (result << 8) | (c & 0xFF);

	/* And output all 6 bit base 64 characters now formed */
	switch (ctx.bytes++) {
	case 0:
		ctx.buf.append(1, static_cast<char>(prtcode[result >> 2 & 0x3F]));
		result &= 0x3;
		break;
	case 1:
		ctx.buf.append(1, static_cast<char>(prtcode[result >> 4 & 0x3F]));
		result &= 0xF;
		break;
	case 2:
		ctx.buf.append(1, static_cast<char>(prtcode[result >> 6 & 0x3F]));
		ctx.buf.append(1, static_cast<char>(prtcode[result & 0x3F]));
		result = 0;
		ctx.bytes = 0;
	}
	ctx.temp = result;
}

void Base64::decodeChar(char c)
{
	int result;

	result = ctx.temp;

	/* Convert Base64 character to its 6 bit nibble */
	if (c == '/') {
		result = (result << 6) | 63;
	} else if (c == '+') {
		result = (result << 6) | 62;
	} else if (c >= 'A' && c <= 'Z') {
		result = (result << 6) | (c - 'A');
	} else if (c >= 'a' && c <= 'z') {
		result = (result << 6) | (c - 'a' + 26);
	} else if (c >= '0' && c <= '9') {
		result = (result << 6) | (c - '0' + 52);
	} else if (c == '=') {
		ctx.bytes = 0;
		ctx.temp = 0;
	}

	/* Add that nibble to the output, outputting any complete 8 bit bytes formed */
	switch (ctx.bytes++) {
	case 0:
		break;
	case 1:
		ctx.buf.append(1, static_cast<char>(result >> 4 & 0xFF));
		result &= 0xF;
		break;
	case 2:
		ctx.buf.append(1, static_cast<char>(result >> 2 & 0xFF));
		result &= 0x3;
		break; 
	case 3:
		ctx.buf.append(1, static_cast<char>(result & 0xFF));
		ctx.bytes = 0;
		result = 0;
	}
	if (c == '=') {
		ctx.bytes = 0;
		result = 0;
	}

	ctx.temp = result;
}

bool Base64::selfTest()
{
	string plain1("Base64");
	string plain2("abcdefghijklmnopqrstuvwxyz");
	string enc1("QmFzZTY0");
	string enc2("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo=");
	string buf;

	Base64 base64;

	buf = base64.encode(plain1);
	if (unlikely(buf != enc1))
		return false;
	buf = base64.decode(buf);
	if (unlikely(buf != plain1))
		return false;

	buf = base64.encode(plain2);
	if (unlikely(buf != enc2))
		return false;
	buf = base64.decode(buf);
	if (unlikely(buf != plain2))
		return false;
	return true;
}
