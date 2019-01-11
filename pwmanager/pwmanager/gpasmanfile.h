/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   Gpasman, a password manager                                           *
 *   Copyright (C) 1998-1999 Olivier Sessink, olivier@lx.student.wau.nl    *
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
 * $Id: gpasmanfile.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef GPASMANFILE_H
#define GPASMANFILE_H

#include "rc2.h"

#include <stdio.h>

/** gpasman / kpasman file read/write module */
class GpasmanFile
{
public:
	GpasmanFile();
	~GpasmanFile();

	/* 
	 * one entry is a char *entry[4]
	 * this means we have 4 pointers to a char -->
	 * 0 = server
	 * 1 = username
	 * 2 = password
	 * 3 = comment
	 */

	int save_init(const char *filename, const char *password);
	int save_entry(char *entry[4]);
	int save_finalize(void);
	int load_init(const char *filename, const char *password);
	int load_entry(char *entry[4]);
	void load_finalize(void);

protected:
	int check_file(const char *filename);
	int file_exists(const char *tfile);

protected:
	FILE *fd;
	unsigned short iv[4];
	char *buffer;
	int bufferIndex;
	unsigned short plaintext[4];
	int lastcount, size;
	Rc2 rc2;
};

#endif
