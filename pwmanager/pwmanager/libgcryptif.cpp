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
 * $Id: libgcryptif.cpp,v 1.6 2004/11/07 02:10:54 ulf69 Exp $
 **************************************************************************/  

#include "libgcryptif.h"

#ifdef CONFIG_PWMANAGER_GCRY

#include "pwmdoc.h"
#include "randomizer.h"

#include <gcrypt.h>

#ifdef PWM_EMBEDDED
#include <pwmprefs.h>
#endif


PwMerror LibGCryptIf::encrypt(unsigned char **outBuf,
			      size_t *outBufLen,
			      unsigned char *inBuf,
			      size_t inBufLen,
			      const unsigned char *key,
			      size_t keylen,
			      char _algo,
			      char _hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
			      )
{
	PwMerror ret = e_success;
	gcry_error_t err;
	gcry_cipher_hd_t handle;
	size_t blklen;
	size_t unpaddedLen = inBufLen;
	size_t cipherKeylen;
	unsigned char *hashedKey;
	unsigned char salt[STRING2KEY_SALTLEN];
	int algo = mapCipherId(_algo);

	if (!inBufLen || !keylen)
		return e_invalidArg;

	// test if algo is ready for encryption
	err = gcry_cipher_algo_info(algo,
				    GCRYCTL_TEST_ALGO,
				    0, 0);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doEncrypt(): GCRYCTL_TEST_ALGO failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out;
	}
	// get the algo block length
	err = gcry_cipher_algo_info(algo,
				    GCRYCTL_GET_BLKLEN,
				    0,
				    &blklen);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doEncrypt(): GCRYCTL_GET_BLKLEN failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out;
	}
	/* double check if we have enough space.
	 * We have only 1024 extra bytes for padding and salt.
	 */
	BUG_ON(blklen > 1024 - STRING2KEY_SALTLEN);
	// get the algo key length
	err = gcry_cipher_algo_info(algo,
				    GCRYCTL_GET_KEYLEN,
				    0,
				    &cipherKeylen);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doEncrypt(): GCRYCTL_GET_KEYLEN failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out;
	}
	// now open the algo and get a handle
	err = gcry_cipher_open(&handle,
			       algo,
			       GCRY_CIPHER_MODE_CBC,
			       0);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doEncrypt(): gcry_cipher_open() failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out;
	}
	// hash the "key" to a fixed size hash matching "cipherKeylen"
	hashedKey = new unsigned char[cipherKeylen];
	hashPassphrase(key, keylen, salt, hashedKey, cipherKeylen, true, _hashalgo);
	// so now set the hashed key
	err = gcry_cipher_setkey(handle, hashedKey, cipherKeylen);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doEncrypt(): gcry_cipher_setkey() failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		delete [] hashedKey;
		goto out_close;
	}
	delete [] hashedKey;
	/* allocate a buffer for the encrypted data.
	 * The size of the buffer is the inBuf length, but blklen
	 * aligned and plus the length of the salt, that is appended.
	 */
	*outBufLen = getBufLen(unpaddedLen, blklen) + STRING2KEY_SALTLEN;
	*outBuf = new unsigned char[*outBufLen];
	padData(inBuf, unpaddedLen, blklen);
	// encrypt the padded data
	err = gcry_cipher_encrypt(handle,
				  *outBuf,
				  *outBufLen - STRING2KEY_SALTLEN,
				  inBuf,
				  *outBufLen - STRING2KEY_SALTLEN);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doEncrypt(): gcry_cipher_encrypt() failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out_delete;
	}
	// append the salt to the encrypted data
	memcpy(*outBuf + *outBufLen - STRING2KEY_SALTLEN, salt, STRING2KEY_SALTLEN);
	goto out_close;
out_delete:
	delete [] *outBuf;
out_close:
	gcry_cipher_close(handle);
out:
	return ret;
}

PwMerror LibGCryptIf::decrypt(unsigned char **outBuf,
			      size_t *outBufLen,
			      const unsigned char *inBuf,
			      size_t inBufLen,
			      const unsigned char *key,
			      size_t keylen,
			      char _algo,
			      char _hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
)
{
	PwMerror ret = e_success;
	gcry_error_t err;
	gcry_cipher_hd_t handle;
	size_t cipherKeylen;
	unsigned char *hashedKey;
	unsigned char salt[STRING2KEY_SALTLEN];
	int algo = mapCipherId(_algo);

	if (!inBufLen || !keylen)
		return e_invalidArg;

	// test if algo is ready for encryption
	err = gcry_cipher_algo_info(algo,
				    GCRYCTL_TEST_ALGO,
				    0, 0);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doDecrypt(): GCRYCTL_TEST_ALGO failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out;
	}
	// get algo key length
	err = gcry_cipher_algo_info(algo,
				    GCRYCTL_GET_KEYLEN,
				    0,
				    &cipherKeylen);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doDecrypt(): GCRYCTL_GET_KEYLEN failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out;
	}
	// extract the salt of the encrypted data buffer
	memcpy(salt, inBuf + inBufLen - STRING2KEY_SALTLEN, STRING2KEY_SALTLEN);
	// open the algo and get a handle
	err = gcry_cipher_open(&handle,
			       algo,
			       GCRY_CIPHER_MODE_CBC,
			       0);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doDecrypt(): gcry_cipher_open() failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out;
	}
	// hash the "key" to a fixed size hash matching "cipherKeylen"
	hashedKey = new unsigned char[cipherKeylen];
	hashPassphrase(key, keylen, salt, hashedKey, cipherKeylen, false, _hashalgo);
	// so now set the hashed key
	err = gcry_cipher_setkey(handle, hashedKey, cipherKeylen);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doDecrypt(): gcry_cipher_setkey() failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		delete [] hashedKey;
		goto out_close;
	}
	delete [] hashedKey;
	*outBufLen = inBufLen - STRING2KEY_SALTLEN;
	*outBuf = new unsigned char[*outBufLen];
	// decrypt the data
	err = gcry_cipher_decrypt(handle,
				  *outBuf,
				  *outBufLen,
				  inBuf,
				  *outBufLen);
	if (err != GPG_ERR_NO_ERROR) {
		printDebug(string("LibGCryptIf::doEncrypt(): gcry_cipher_encrypt() failed: ")
			   + gcry_strerror(err));
		ret = e_cryptNotImpl;
		goto out_delete;
	}
	// remove all random padding
	unpadData(*outBuf, outBufLen);
	goto out_close;
out_delete:
	delete [] *outBuf;
out_close:
	gcry_cipher_close(handle);
out:
	return ret;
}

PwMerror LibGCryptIf::hash(unsigned char **outBuf,
			   size_t *outBufLen,
			   const unsigned char *inBuf,
			   size_t inBufLen,
			   char _algo)
{
	PwMerror ret = e_success;
	unsigned int hashLen;
	int algo = mapHashId(_algo);

	hashLen = gcry_md_get_algo_dlen(algo);
	*outBufLen = hashLen;
	*outBuf = new unsigned char[*outBufLen];
	gcry_md_hash_buffer(algo,
			    *outBuf,
			    inBuf,
			    inBufLen);
	return ret;
}

unsigned int LibGCryptIf::hashLength(char _algo)
{
	unsigned int ret;
	int algo = mapHashId(_algo);
	ret = gcry_md_get_algo_dlen(algo);
	return ret;
}

int LibGCryptIf::mapCipherId(char algo)
{
	switch (algo) {
	case PWM_CRYPT_AES128:
		return GCRY_CIPHER_AES;
	case PWM_CRYPT_AES192:
		return GCRY_CIPHER_AES192;
	case PWM_CRYPT_AES256:
		return GCRY_CIPHER_AES256;
	case PWM_CRYPT_3DES:
		return GCRY_CIPHER_3DES;
	case PWM_CRYPT_TWOFISH:
		return GCRY_CIPHER_TWOFISH;
	case PWM_CRYPT_TWOFISH128:
		return GCRY_CIPHER_TWOFISH128;
	default:
		BUG();
	}
	return GCRY_CIPHER_NONE;
}

int LibGCryptIf::mapHashId(char algo)
{
	switch (algo) {
	case PWM_HASH_SHA1:
		return GCRY_MD_SHA1;
	case PWM_HASH_SHA256:
		return GCRY_MD_SHA256;
	case PWM_HASH_SHA384:
		return GCRY_MD_SHA384;
	case PWM_HASH_SHA512:
		return GCRY_MD_SHA512;
	case PWM_HASH_MD5:
		return GCRY_MD_MD5;
	case PWM_HASH_RMD160:
		return GCRY_MD_RMD160;
	case PWM_HASH_TIGER:
		return GCRY_MD_TIGER;
	default:
		BUG();
	}
	return GCRY_MD_NONE;
}

bool LibGCryptIf::hashPassphrase(const unsigned char *pw,
				 size_t pwlen,
				 unsigned char *salt,
				 unsigned char *key,
				 size_t keylen,
				 bool create,
				 char _hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
)
{
	DEK dek;
	STRING2KEY s2k;
	bool ret;

	dek.keylen = keylen;
	s2k.mode = 1;
	//US bug: do not use the global hash algo here. Use the passed ago instead. The hashalgo stored in the file can
	// be different from the one in the configuration.
	s2k.hash_algo = mapHashId(_hashalgo //conf()->confGlobHashAlgo()
				  );
	s2k.count = 0;
	if (!create)
		memcpy(s2k.salt, salt, STRING2KEY_SALTLEN);
	ret = doHashPassphrase(&dek,
			       pw,
			       pwlen,
			       &s2k,
			       create);
	if (!ret)
		goto out;
	memcpy(key, dek.key, dek.keylen);
	if (create)
		memcpy(salt, s2k.salt, STRING2KEY_SALTLEN);
out:
	return ret;
}


bool LibGCryptIf::doHashPassphrase(DEK *dek,
				   const unsigned char *pw,
				   size_t pwlen,
				   STRING2KEY *s2k,
				   bool create)
{
	// This function is derived from GnuPG-1.2.5-rc2
	gcry_md_hd_t md;
	gcry_error_t err;
	bool ret = true;
	size_t pass, i;
	size_t used = 0;

	PWM_ASSERT(s2k->hash_algo);
	BUG_ON(!(dek->keylen > 0 && dek->keylen <= array_size(dek->key)));

	err = gcry_md_open(&md, s2k->hash_algo, 0);
	if (err != GPG_ERR_NO_ERROR) {
		ret = false;
		goto out;
	}
	for (pass = 0; used < dek->keylen; pass++) {
		if (pass) {
			gcry_md_reset(md);
			for (i = 0; i < pass; i++) // preset the hash context
			gcry_md_putc(md, 0);
		}
		if (s2k->mode == 1 || s2k->mode == 3) {
			size_t len2 = pwlen + 8;
			size_t count = len2;

			if (create && !pass) {
				Randomizer *rnd = Randomizer::obj();
				const unsigned int salt_len = 8;
				string rndBuf(rnd->genRndBuf(salt_len));
				memcpy(s2k->salt, rndBuf.c_str(), salt_len);
				if (s2k->mode == 3)
					s2k->count = 96; // 65536 iterations
			}
			if (s2k->mode == 3) {
				count = (16ul + (s2k->count & 15)) << ((s2k->count >> 4) + 6);
				if (count < len2)
					count = len2;
			}
			// a little bit complicated because we need a ulong for count
			while (count > len2) { // maybe iterated+salted
				gcry_md_write(md, s2k->salt, 8);
				gcry_md_write(md, pw, pwlen);
				count -= len2;
			}
			if (count < 8) {
				gcry_md_write(md, s2k->salt, count);
			} else {
				gcry_md_write(md, s2k->salt, 8);
				count -= 8;
				gcry_md_write(md, pw, count);
			}
		} else
			gcry_md_write(md, pw, pwlen);
		gcry_md_final(md);
		i = gcry_md_get_algo_dlen(s2k->hash_algo);
		if (i > dek->keylen - used)
			i = dek->keylen - used;
		memcpy(dek->key+used, gcry_md_read(md, s2k->hash_algo), i);
		used += i;
	}
	gcry_md_close(md);
out:
	return ret;
}

void LibGCryptIf::padData(unsigned char *buf,
			  size_t bufLen,
			  size_t boundary)
{
	size_t numPadBytes = boundary - ((bufLen + 1) % boundary);
	buf[bufLen] = static_cast<char>(0x01);
	size_t i = 0;
	Randomizer *rnd = Randomizer::obj();
	char c;
	unsigned char *b;
	while (i < numPadBytes) {
		c = rnd->genRndChar();
		if (c == static_cast<char>(0x01))
			continue;
		b = buf + bufLen + 1 + i;
		*b = c;
		++i;
	}
}

void LibGCryptIf::unpadData(const unsigned char *buf,
		 	    size_t *bufLen)
{
	size_t pos;
	BUG_ON(*bufLen % 8);
	pos = *bufLen - 1;
	while (buf[pos] != static_cast<char>(0x01)) {
	  //qDebug("pos %d %d %d", pos, buf[pos],   static_cast<char>(0x01)     );
		BUG_ON(!pos);
        //LR BUG we should terminte the loop if p == 0
        if ( pos == 0 )
            break;
		--pos;
	}
	*bufLen = pos;
	//qDebug("ente ");
}

#endif // CONFIG_PWMANAGER_GCRY

