/***************************************************************************
 *                                                                         *
 *   copyright (C) 2004 by Michael Buesch                                  *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   hashPassphrase() is derived from GnuPG and is                         *
 *   Copyright (C) 1998, 1999, 2000, 2001, 2003                            *
 *   Free Software Foundation, Inc.                                        *
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
 * $Id: libgcryptif.h,v 1.7 2006/02/24 19:41:09 zautrix Exp $
 **************************************************************************/  

#ifndef __LIBGCRYPTIF_H
#define __LIBGCRYPTIF_H


//#undef CONFIG_PWMANAGER_GCRY // for debugging only.
#ifdef CONFIG_PWMANAGER_GCRY

#include <stddef.h>
#include <sys/types.h>
#ifndef _WIN32_
#include <stdint.h>
#else
#define  uint8_t Q_UINT8
#define  byte Q_UINT8
#define  uint32_t Q_UINT32
#endif
#define STRING2KEY_SALTLEN	8

#include "pwmexception.h"


/** interface class for the libgcrypt cipher and hash algorithms
  * NOTE: Always allocate 1024 extra bytes for the inBuf (for padding)
  */
class LibGCryptIf
{
protected:
	struct STRING2KEY
	{
		int  mode;
		int hash_algo;
		uint8_t salt[STRING2KEY_SALTLEN];
		uint32_t  count;
	};
	struct DEK
	{
		size_t keylen;
		uint8_t key[32]; // this is the largest used keylen (256 bit)
	};

public:
	LibGCryptIf() { }
	/** is libgcrypt available? */
	static bool available()
			{ return true; }
	/** encrypt data. _algo is the PWM_CRYPT_* ID
	  * of the algorithm.
	  */
	PwMerror encrypt(unsigned char **outBuf,
			 size_t *outBufLen,
			 unsigned char *inBuf,
			 size_t inBufLen,
			 const unsigned char *key,
			 size_t keylen,
			 char _algo,
			 char _hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
);
	/** decrypt data. _algo is the PWM_CRYPT_* ID
	  * of the algorithm.
	  */
	PwMerror decrypt(unsigned char **outBuf,
			 size_t *outBufLen,
			 const unsigned char *inBuf,
			 size_t inBufLen,
			 const unsigned char *key,
			 size_t keylen,
			 char _algo,
			 char _hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
);
	/** hash data. _algo is the PWM_HASH_* ID of the hash */
	PwMerror hash(unsigned char **outBuf,
		      size_t *outBufLen,
		      const unsigned char *inBuf,
		      size_t inBufLen,
		      char _algo);
	/** returns the length of the hash. _algo is the PWM_HASH_*
	  * id of the hash. returns 0 on error.
	  */
	unsigned int hashLength(char _algo);

protected:
	/** returns the total buffer length */
	size_t getBufLen(size_t inBufLen, size_t boundary)
		{
			return ((boundary - (inBufLen % boundary)) + inBufLen);
		}
	/** pad the data up to the given boundary.
	  * "buf" has to be big enough!
	  */
	void padData(unsigned char *buf,
		     size_t bufLen,
		     size_t boundary);
	/** unpad the data */
	void unpadData(const unsigned char *buf,
		       size_t *bufLen);
	/** maps the PWM_CRYPT_* ID of an algorithm
	  * to the libgcrypt GCRY_CIPHER_* ID
	  */
	int mapCipherId(char algo);
	/** maps the PWM_HASH_* ID of an algorithm
	  * to the libgcrypt GCRY_MD_* ID
	  */
	int mapHashId(char algo);
	/** hash a passphrase to a cipher key */
	bool hashPassphrase(const unsigned char *pw,
			    size_t pwlen,
			    unsigned char *salt,
			    unsigned char *key,
			    size_t keylen,
			    bool create, 
			 char _hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
);
	/** hash a passphrase to a cipher key */
	bool doHashPassphrase(DEK *dek,
			      const unsigned char *pw,
			      size_t pwlen,
			      STRING2KEY *s2k,
			      bool create);
};


#else // CONFIG_PWMANAGER_GCRY
/** libgcrypt is not installed. This is a NOP wrapper. */
class LibGCryptIf
{
public:
	LibGCryptIf() { }
	static bool available()
			{ return false; }
	PwMerror encrypt(unsigned char **,
			 size_t *,
			 unsigned char *,
			 size_t,
			 const unsigned char *,
			 size_t,
			 char)
			{ return e_cryptNotImpl; }
	PwMerror decrypt(unsigned char **,
			 size_t *,
			 const unsigned char *,
			 size_t,
			 const unsigned char *,
			 size_t,
			 char)
			{ return e_cryptNotImpl; }
	PwMerror hash(unsigned char **,
		      size_t *,
		      const unsigned char *,
		      size_t,
		      char)
			{ return e_hashNotImpl; }
	unsigned int hashLength(char)
			{ return 0; }
};
#endif // CONFIG_PWMANAGER_GCRY
#endif // __LIBGCRYPTIF_H
