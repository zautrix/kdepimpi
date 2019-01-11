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
 * $Id: compressgzip.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $
 **************************************************************************/  

#include "compressgzip.h"

#include <stdlib.h>
#include <zlib.h>

#define COMPRESSION_LEVEL			Z_BEST_COMPRESSION
#define EXPAND_COMPRESS_DESTBUF_BYTES		1024
#define EXPAND_DECOMPRESS_DESTBUF_BYTES		(1024 * 10)


bool CompressGzip::compress(string *d)
{
	int ret;
	Bytef *dest;
	const Bytef *source;
	unsigned long sourceLen, destLen;

	source = reinterpret_cast<const Bytef *>(d->c_str());
	sourceLen = d->length();
	destLen = calcCompressDestLen(sourceLen);
	dest = static_cast<Bytef *>(malloc(destLen));
	if (!dest)
		return false;
	while (1) {
		ret = compress2(dest,
				static_cast<uLongf *>(&destLen),
				source,
				static_cast<uLong>(sourceLen),
				COMPRESSION_LEVEL);
		switch (ret) {
		case Z_OK:
			goto out;
		case Z_BUF_ERROR:
			/* we don't use realloc(), because it may
			 * (in this case) unneccessarily copy the old block
			 * to the new allocated block.
			 */
			free(dest);
			destLen += EXPAND_COMPRESS_DESTBUF_BYTES;
			dest = static_cast<Bytef *>(malloc(destLen));
			if (!dest)
				return false;
			break;
		default:
			free(dest);
			return false;
		}
	}
out:
	d->assign(reinterpret_cast<char *>(dest), destLen);
	free(dest);
	return true;
}

bool CompressGzip::decompress(string *d)
{
	int ret;
	Bytef *dest;
	const Bytef *source;
	unsigned long sourceLen, destLen;

	source = reinterpret_cast<const Bytef *>(d->c_str());
	sourceLen = d->length();
	destLen = calcDecompressDestLen(sourceLen);
	dest = static_cast<Bytef *>(malloc(destLen));
	if (!dest)
		return false;
	while (1) {
		ret = uncompress(dest,
				 static_cast<uLongf *>(&destLen),
				 source,
				 static_cast<uLong>(sourceLen));
		switch (ret) {
		case Z_OK:
			goto out;
		case Z_BUF_ERROR:
			/* we don't use realloc(), because it may
			 * (in this case) unneccessarily copy the old block
			 * to the new allocated block.
			 */
			free(dest);
			destLen += EXPAND_DECOMPRESS_DESTBUF_BYTES;
			dest = static_cast<Bytef *>(malloc(destLen));
			if (!dest)
				return false;
			break;
		default:
			free(dest);
			return false;
		}
	}
out:
	d->assign(reinterpret_cast<char *>(dest), destLen);
	free(dest);
	return true;
}
