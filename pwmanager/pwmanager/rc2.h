/***************************************************************************
 *                                                                         *
 * Header file for rc2 implementation by Matthew Palmer <mjp16@uow.edu.au> *
 * Modified to run with PwM by Michael Buesch <mbuesch@freenet.de>         *
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
 * $Id: rc2.h,v 1.1 2004/09/15 17:55:58 ulf69 Exp $
 **************************************************************************/  

#ifndef RC2_H
#define RC2_H

/** implementation of the RC2 encryption algorithm. */
class Rc2
{
public:
	Rc2();
	~Rc2();

	void rc2_expandkey(char key[], int length, int ekl);
	void rc2_encrypt(unsigned short input[4]);
	void rc2_decrypt(unsigned short input[4]);

protected:
	void _rc2_mix(unsigned short *);
	void _rc2_mash(unsigned short *);
	void _rc2_rmix(unsigned short *);
	void _rc2_rmash(unsigned short *);
	int _rc2_pow(int, int);
	unsigned short _rc2_ror(unsigned short, int);
	unsigned short _rc2_rol(unsigned short, int);

protected:
	/** Expanded Key */
	unsigned char _rc2_expkey[128];
	/** global integer variable used in mixing */
	int _rc2_counter;
};

#endif
