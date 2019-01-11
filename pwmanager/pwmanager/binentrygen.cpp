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
 * $Id: binentrygen.cpp,v 1.3 2008/10/02 20:37:12 zautrix Exp $
 **************************************************************************/  
#include <stdlib.h>
#include "binentrygen.h"
#include "base64.h"

#include "pwmexception.h"
#include "globalstuff.h"

void BinEntryGen::encode(const QByteArray &data,
			 PwMDataItem *ret,
			 DataType type)
{
	ret->clear();
	ret->name = tostr(static_cast<int>(type));
	ret->binary = true;
	if (data.size() == 0)
		return;
	Base64 b64;
	string d(data.data(), data.size());
	ret->pw = b64.encode(d);
}

void BinEntryGen::decode(const PwMDataItem &data,
			 QByteArray *ret,
			 DataType *type)
{
	BUG_ON(!data.binary);
	int t =  strtol(data.name.c_str(), 0, 10);
	*type = static_cast<DataType>(t);
	switch (*type) {
		case None:
		case KWalletMap:
		case KWalletStream:
			break;
		default:
			*type = None;
	}
	if (data.pw == "") {
		ret->fill(0);
		ret->resize(0);
		return;
	}
	Base64 b64;
	string d(b64.decode(data.pw));
	ret->duplicate(d.c_str(), d.length());
}

BinEntryGen::DataType BinEntryGen::binType(const PwMDataItem &data)
{
	if (!data.binary)
		return None;
	int type = strtol(data.name.c_str(), 0, 10);
	return (static_cast<DataType>(type));
}

