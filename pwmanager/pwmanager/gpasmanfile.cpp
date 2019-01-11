/*  Gpasman, a password manager
    Copyright (C) 1998-1999 Olivier Sessink, olivier@lx.student.wau.nl

    file.c, handles file opening and closing

    Other code contributors:
    Dave Rudder
    Chris Halverson
    Matthew Palmer
    Guide Berning
    Jimmy Mason
	 website at http://www.student.wau.nl/~olivier/gpasman/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* 2003/06/10:
 * modified by Michael Buesch to work together
 * with PwM as import/export module.
 */

/***************************************************************************
 * copyright (C) 2004 by Ulf Schenk
 * This file is originaly based on version 1.0.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: gpasmanfile.cpp,v 1.2 2004/10/23 17:05:41 zautrix Exp $
 **************************************************************************/  

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#ifndef _WIN32_
#include <unistd.h>
#else
#include <io.h>
#define S_IRUSR  _S_IREAD 
#define S_IWUSR  _S_IWRITE 
#define creat  _creat 
#endif
#include <string.h>
#include <errno.h>

#include "gpasmanfile.h"
#include "globalstuff.h"

#define SAVE_BUFFER_LENGTH 1024
#define LOAD_BUFFER_LENGTH 2048

#ifndef S_IAMB
#define S_IAMB 00777
#endif

// enable/disable debug output
//#define GPASMANFILE_DEBUG
#undef GPASMANFILE_DEBUG

#ifndef _WIN32_
#if defined(PWM_DEBUG) && defined(GPASMANFILE_DEBUG)
# define DBG(msg,x...)	do { fprintf(stderr, msg "\n" , ##x); } while (0)
#else
# define DBG(msg,x...)	do { } while (0)
#endif
#else
# define DBG
#endif

#ifdef BIG_ENDIAN_HOST
# define WORDS_BIGENDIAN
#else
# undef WORDS_BIGENDIAN
#endif


GpasmanFile::GpasmanFile()
{
}

GpasmanFile::~GpasmanFile()
{
}

int GpasmanFile::save_init(const char *filename, const char *password)
{

/*
 * returncodes: 
 *  1 = success
 *  0 = can't open filedescriptor / can't create file
 * -1 = permissions are bad
 * -2 = is a symlink
 * -3 = can't get file status
 */

	unsigned char key[128];
	unsigned int j = 0;
	unsigned int keylength;
	int val, count2;

	/* first we should check the permissions of the filename */

	if (file_exists(filename)) {
		val = check_file(filename);
		if (val != 1) {
			DBG("save_init, return %d", val);
			return val;
		}
	} else {
		val = creat(filename, (S_IRUSR | S_IWUSR));
		if (val == -1) {
			DBG("%s", "save_init, return 0");
			return 0;
		} else {
			close(val);
		}
	}

	fd = fopen(filename, "wb");
	if (fd == NULL) {
		return 0;
	}
	buffer = (char*)malloc(SAVE_BUFFER_LENGTH);

	/* make the key ready */
	DBG("save_init, password=%s", password);
	for (j = 0; password[j] != '\0'; j++) {
		key[j] = password[j];
	}
	keylength = j;
	rc2.rc2_expandkey((char*)key, (int)keylength, 128);

	/* First, we make the IV */
	for (count2 = 0; count2 < 4; count2++) {
		iv[count2] = rand();
		putc((unsigned char) (iv[count2] >> 8), fd);
		putc((unsigned char) (iv[count2] & 0xff), fd);
	}

	bufferIndex = 0;
	return 1;
}

int GpasmanFile::save_entry(char *entry[4])
{

	char *text1;
	int count2, count3;
	unsigned short ciphertext[4];

	buffer = (char*)memset(buffer, '\0', SAVE_BUFFER_LENGTH);

	for (count2 = 0; count2 < 4; count2++) {
		text1 = entry[count2];
		if (strlen(text1) == 0) {
			strncpy(text1, " ", strlen(" "));
		}
		strncat(buffer, text1, strlen(text1));
		/* Use 255 as the marker.  \n is too tough to test for */
		buffer[strlen(buffer)] = 255;

	}			/*for (count2 = 0; count2 < 4; count2++) */
	DBG("save_entry, buffer contains %s", buffer);
	count2 = 0;
	/* I'm using CBC mode and encrypting the data straight from top down.
	 *  At the bottom, encrypted, I will append an MD5 hash of the file, eventually.
	 *  PKCS 5 padding (explained at the code section
	 */
	while (count2 < (int)strlen(buffer)) {
#ifndef WORDS_BIGENDIAN
		plaintext[bufferIndex] = buffer[count2 + 1] << 8;
		plaintext[bufferIndex] += buffer[count2] & 0xff;
#else
		plaintext[bufferIndex] = buffer[count2] << 8;
		plaintext[bufferIndex] += buffer[count2 + 1] & 0xff;
#endif
		bufferIndex++;
		if (bufferIndex == 4) {
			rc2.rc2_encrypt(plaintext);

			for (count3 = 0; count3 < 4; count3++) {
				ciphertext[count3] =
				    iv[count3] ^ plaintext[count3];

				/* Now store the ciphertext as the iv */
				iv[count3] = plaintext[count3];

				/* reset the buffer index */
				bufferIndex = 0;
				if (putc
				    ((unsigned char) (ciphertext[count3] >> 8),
				     fd) == EOF)
					return -1;
				if (putc
				    ((unsigned char) (ciphertext[count3] &
						      0xff), fd) == EOF)
					return -1;
			}	/*for (count3 = 0; count3 < 4; count3++) */
		}
		/*if (bufferIndex == 4) */
		/* increment a short, not a byte */
		count2 += 2;
	}			/*while (count2 < strlen (buffer)) */
	return 1;
}

int GpasmanFile::save_finalize(void)
{

	int count1, retval = 1;
	unsigned short ciphertext[4];

	/* Tack on the PKCS 5 padding 
	   How it works is we fill up the last n bytes with the value n

	   So, if we have, say, 13 bytes, 8 of which are used, we have 5 left 
	   over, leaving us 3 short, so we fill it in with 3's.

	   If we come out even, we fill it with 8 8s

	   um, except that in this instance we are using 4 shorts instead of 8 bytes.
	   so, half everything
	 */
	for (count1 = bufferIndex; count1 < 4; count1++) {
		plaintext[count1] = (4 - bufferIndex);
	}
	DBG("save_finalize, 4 - bufferIndex = %d",
	    4 - bufferIndex);
	DBG("save_finalize, plaintext[3]=%c", plaintext[3]);
	rc2.rc2_encrypt(plaintext);
	for (count1 = 0; count1 < 4; count1++) {
		ciphertext[count1] = iv[count1] ^ plaintext[count1];
		if (putc((unsigned char) (ciphertext[count1] >> 8), fd) == EOF)
			retval = -1;
		if (putc((unsigned char) (ciphertext[count1] & 0xff), fd) ==
		    EOF)
			retval = -1;
	}

	fclose(fd);
	DBG("%s", "save_finalize, fd is closed");
	free(buffer);
	return retval;

}

int GpasmanFile::load_init(const char *filename, const char *password)
{
/*
 * returncodes: 
 *  1 = success
 *  0 = can't open filedescriptor / can't create file
 * -1 = permissions are bad
 * -2 = is a symlink
 * -3 = can't get file status
 */
	unsigned int j = 0;
	unsigned int keylength = 0;
	int count = 0, count2 = 0, count3 = 0;
	unsigned char charbuf[8];
	unsigned short ciphertext[4];
	int val = 0;
	unsigned char key[128];

	/* first we should check the file permissions */
	if (file_exists(filename)) {
		val = check_file(filename);
		if (val != 1) {
			return val;
		}
	} else {
		return 0;
	}

	fd = fopen(filename, "rb");
	if (fd == NULL) {
		return 0;
	}

	buffer = (char*)malloc(LOAD_BUFFER_LENGTH);
	DBG("load_init, password=\"%s\"", password);
	for (j = 0; password[j] != '\0'; j++) {
		key[j] = password[j];
	}
	keylength = j;
	rc2.rc2_expandkey((char*)key, (int)keylength, 128);

	size = read(fileno(fd), (unsigned char *) (charbuf + count), 8);
	DBG("load_init, size=%d, keylength=%d", size, keylength);

	if (size < 8) {
		fclose(fd);
		free(buffer);
		return -1;
	}

	for (count = 0; count < 4; count++) {
		count2 = count << 1;
		iv[count] = charbuf[count2] << 8;
		iv[count] += charbuf[count2 + 1];
		DBG("load_init iv[%d]=%d", count, iv[count]);
	}

	size = 0;
	bufferIndex = 0;
	while ((count = read(fileno(fd), (unsigned char *) charbuf, 8)) > 0) {
		DBG("load_init A, count=%d, count2=%d", count, count2);
		while (count < 8) {
			count2 = read(fileno(fd), (unsigned char *) (charbuf +
								     count), 8);
			DBG("load_init B, count=%d, count2=%d",
				count, count2);
			if (count2 == 0) {
				printf("bad EOF\n");
				fclose(fd);
				free(buffer);
				return -1;
			}
			count += count2;
		}		/* while (count < 8) */

		size += 8;
		DBG("load_init charbuf[1]=%c", charbuf[1]);
		for (count2 = 0; count2 < 8; count2 += 2) {
			count3 = count2 >> 1;
			ciphertext[count3] = charbuf[count2] << 8;
			ciphertext[count3] += charbuf[count2 + 1];

			plaintext[count3] = ciphertext[count3] ^ iv[count3];
			iv[count3] = plaintext[count3];
		}

		rc2.rc2_decrypt(plaintext);
		memcpy((unsigned char *) (buffer + bufferIndex), plaintext, 8);
		bufferIndex += 8;
		buffer[bufferIndex + 1] = '\0';
		DBG("bufferIndex=%d, buffer=%s", bufferIndex,
			buffer);
	}			/* while ((count = read (fileno (fd), (unsigned char *) charbuf, 8)) > 0) */
	DBG("load_init, size=%d, buffer[size-1]=%d,", size,
		buffer[size - 1]);
	size -= buffer[size - 1];
	DBG("size=%d", size);
	lastcount = 0;

	/* This will point to the starting index */
	bufferIndex = 0;
	return 1;
}

int GpasmanFile::load_entry(char *entry[4])
{
/* Strip off PKCS 5 padding 
     Should check to make sure it's good here
 */
	int count, count1 = 0;
	DBG("load_entry, lastcount=%d, size=%d, entry=%p",
		lastcount, size, entry);

	for (count = lastcount; count < size; count++) {
		if ((unsigned char) (buffer[count]) == 255) {
			if (buffer[bufferIndex] == '\0') {
				bufferIndex++;
			}
			entry[count1] =
			    (char *) malloc(count - bufferIndex + 1);
			DBG("load_entry, entry[%d]=%p", count1,
				entry[count1]);
			memcpy(entry[count1],
			       (unsigned char *) (buffer + bufferIndex),
			       count - bufferIndex);
			entry[count1][count - bufferIndex] = '\0';
			DBG("load_entry, entry[%d]=%s", count1,
				entry[count1]);
			count++;
			bufferIndex = count;
			count1++;
			if (count1 == 4) {
				lastcount = count;
				DBG("%s", "load_entry, return 1, entry ready");
				return 1;
			}
		}		/* if ((unsigned char) (buffer[count]) == 255) */
	}			/* for (count = 0; count < size; count++) */

	DBG("%s", "load_entry, ended no entry anymore");
	return 2;
}

void GpasmanFile::load_finalize(void)
{
	fclose(fd);
	free(buffer);
}

int GpasmanFile::check_file(const char *filename)
{
	struct stat naamstat;

	if (stat(filename, &naamstat) == -1) {
		return (-3);
	}

	if (((naamstat.st_mode & S_IAMB) | (S_IRUSR | S_IWUSR)) != (S_IRUSR |
								    S_IWUSR)) {
		DBG("%s perms are bad, they are: %ld, should be -rw------",
		    filename, (naamstat.st_mode & (S_IREAD | S_IWRITE)));
		return (-1);
	}
#ifndef _WIN32_
	if (!S_ISREG(naamstat.st_mode)) {
		lstat(filename, &naamstat);
		if (S_ISLNK(naamstat.st_mode)) {
			DBG("%s is a symlink", filename);
			return (-2);
		}
	}
#endif
	return (1);
}

int GpasmanFile::file_exists(const char *tfile)
{
	struct stat naamstat;

	if ((stat(tfile, &naamstat) == -1) && (errno == ENOENT)) {
		DBG("file_exists, %s does NOT exist", tfile);
		return (0);
	} else {
		DBG("file_exists, %s DOES exist", tfile);
		return (1);
	}
}

