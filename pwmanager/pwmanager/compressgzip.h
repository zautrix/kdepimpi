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
 * $Id: compressgzip.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef COMPRESSGZIP_H
#define COMPRESSGZIP_H

#include <string>
using std::string;

/** gzip compression */
class CompressGzip
{
public:
	CompressGzip() {}

	/** compress the string d */
	bool compress(string *d);
	/** decompress the string d */
	bool decompress(string *d);

protected:
	/** calculates the length of the dest-buffer
	  * for compress() using the size of the source-buffer
	  */
	unsigned int calcCompressDestLen(unsigned int sourceLen)
			{ return (sourceLen + (sourceLen / 100 / 10 + 12)); }
	/** calculates the length of the dest-buffer
	  * for decompress() using the size of the source-buffer
	  */
	unsigned int calcDecompressDestLen(unsigned int sourceLen)
			{ return (sourceLen * 2); }
};

#endif
