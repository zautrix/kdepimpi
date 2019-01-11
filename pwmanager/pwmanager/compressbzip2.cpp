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
 * $Id: compressbzip2.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $
 **************************************************************************/  

#include "compressbzip2.h"

#include <stdlib.h>

#define BZ_NO_STDIO
#include <bzlib.h>

#define BZ_BLOCK_SIZE				9
#define BZ_WORK_FACTOR				30
#define EXPAND_COMPRESS_DESTBUF_BYTES		1024
#define EXPAND_DECOMPRESS_DESTBUF_BYTES		(1024 * 10)


bool CompressBzip2::compress(string *d)
{
	int ret;
	char *dest, *source;
	unsigned int sourceLen, destLen;

	sourceLen = d->length();
	destLen = calcCompressDestLen(sourceLen);
	source = static_cast<char *>(malloc(sourceLen));
	if (!source)
		return false;
	memcpy(source, d->c_str(), sourceLen);
	dest = static_cast<char *>(malloc(destLen));
	if (!dest) {
		free(source);
		return false;
	}
	while (1) {
		ret = BZ2_bzBuffToBuffCompress(dest,
					       &destLen,
					       source,
					       sourceLen,
					       BZ_BLOCK_SIZE,
					       0,
					       BZ_WORK_FACTOR);
		switch (ret) {
		case BZ_OK:
			goto out;
		case BZ_OUTBUFF_FULL:
			/* we don't use realloc(), because it may
			 * (in this case) unneccessarily copy the old block
			 * to the new allocated block.
			 */
			free(dest);
			destLen += EXPAND_COMPRESS_DESTBUF_BYTES;
			dest = static_cast<char *>(malloc(destLen));
			if (!dest) {
				free(source);
				return false;
			}
			break;
		default:
			free(source),
			free(dest);
			return false;
		}
	}
out:
	free(source);
	d->assign(dest, destLen);
	free(dest);
	return true;
}

bool CompressBzip2::decompress(string *d)
{
	int ret;
	char *dest, *source;
	unsigned int sourceLen, destLen;

	sourceLen = d->length();
	destLen = calcDecompressDestLen(sourceLen);
	source = static_cast<char *>(malloc(sourceLen));
	if (!source)
		return false;
	memcpy(source, d->c_str(), sourceLen);
	dest = static_cast<char *>(malloc(destLen));
	if (!dest) {
		free(source);
		return false;
	}
	while (1) {
		ret = BZ2_bzBuffToBuffDecompress(dest,
						 &destLen,
						 source,
						 sourceLen,
						 0,
						 0);
		switch (ret) {
		case BZ_OK:
			goto out;
		case BZ_OUTBUFF_FULL:
			/* we don't use realloc(), because it may
			 * (in this case) unneccessarily copy the old block
			 * to the new allocated block.
			 */
			free(dest);
			destLen += EXPAND_DECOMPRESS_DESTBUF_BYTES;
			dest = static_cast<char *>(malloc(destLen));
			if (!dest) {
				free(source);
				return false;
			}
			break;
		default:
			free(source),
			free(dest);
			return false;
		}
	}
out:
	free(source);
	d->assign(dest, destLen);
	free(dest);
	return true;
}
