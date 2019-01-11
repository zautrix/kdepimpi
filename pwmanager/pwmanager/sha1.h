/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
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
 * $Id: sha1.h,v 1.2 2004/10/20 08:28:44 zautrix Exp $
 **************************************************************************/  

#ifndef SHA1_H
#define SHA1_H
#ifdef _WIN32_
#define uint32_t unsigned int
#define uint8_t	unsigned char
#define byte	unsigned char
#else
#include <stdint.h>
typedef uint8_t		byte;
#endif
#include <string>
using std::string;


#define SHA1_HASH_LEN_BIT	160
#define SHA1_HASH_LEN_BYTE	(SHA1_HASH_LEN_BIT / 8)

/** sha1 hash algorithm.
  * Derived from libgcrypt-1.1.12
  */
class Sha1
{
	struct SHA1_CONTEXT
	{
		uint32_t  h0,h1,h2,h3,h4;
		uint32_t  nblocks;
		byte buf[64];
		int  count;
	};

public:
	Sha1() { sha1_init(); }
	static bool selfTest();

	void sha1_write(const byte *inbuf, uint32_t inlen);
	string sha1_read();

protected:
	void sha1_init();
	void sha1_final();
	void burn_stack (int bytes);
	void transform(const byte *data);

	/** Rotate a 32 bit integer by n bytes */
	uint32_t rol(uint32_t x, int n)
	{
#if defined(__GNUC__) && defined(__i386__)
		__asm__("roll %%cl,%0"
			:"=r" (x)
			:"0" (x),"c" (n));
		return x;
#else
		return ((x) << (n)) | ((x) >> (32-(n)));
#endif
	}

protected:
	struct SHA1_CONTEXT ctx;
};

#endif
