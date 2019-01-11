/***************************************************************************
 *                                                                         *
 *   copyright (C) 2004 by Michael Buesch                                  *
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
 * $Id: binentrygen.h,v 1.2 2006/02/24 19:41:08 zautrix Exp $
 **************************************************************************/  

#ifndef __BINENTRYGEN_H
#define __BINENTRYGEN_H


#include <qcstring.h>

#include "pwmdoc.h"

/** Binary entry generator.
  * This generator generates a normal struct PwMDataItem
  * from binary data (using base64 encoding).
  * This mechanism is used to support the binary interface functions
  * of the KWallet emulation, for example.
  *
  * The format of the encoded binary data as a PwMDataItem is as follows:
  *
  * PwMDataItem::desc    contains the normal description string for
  *                      this entry. Nothing surprising.
  * PwMDataItem::name    contains the "DataType" number in ascii format.
  * PwMDataItem::pw      contains the base64 encoded data stream.
  * PwMDataItem::binary  is always true for binary entries.
  * All other PwMDataItems are currently unused by BinEntryGen.
  */
class BinEntryGen
{
public:
	enum DataType
	{
		None = 0,
		KWalletMap,
		KWalletStream
	};

public:
	BinEntryGen() { }

	/** Encode the binary "data" and return it in "ret" */
	void encode(const QByteArray &data, PwMDataItem *ret, DataType type);
	/** Decode the "data" and return it as binary "ret" */
	void decode(const PwMDataItem &data, QByteArray *ret, DataType *type);

	/** Return the data type for this binary data item */
	DataType binType(const PwMDataItem &data);
};

#endif // __BINENTRYGEN_H
