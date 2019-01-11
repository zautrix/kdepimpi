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
 * This file is originaly based on version 1.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: serializer.h,v 1.3 2004/10/19 18:11:34 ulf69 Exp $
 **************************************************************************/  

#ifndef __SERIALIZER_H
#define __SERIALIZER_H

#include "pwmdoc.h"

#include <qcstring.h>
#include <qdom.h>

#include <vector>

using std::vector;

/** This serializes its input data into
  * the PwManager-XML-datastream, that becomes
  * encrypted and maybe compressed
  */
class Serializer
{
public:
	/** construct an empty serializer document */
	Serializer();
	/** construct a serializer document and parse "buffer" */
	Serializer(const QCString &buffer);
	/** destructor */
	virtual ~Serializer();

	/** clears all data */
	void clear();
	/** parse the given data buffer */
	bool parseXml(const QCString &buffer);
	/** returns the current XML data */
	QCString getXml();
	/** serialize "dta" and store it as XML data */
	//US ENH: we need to serialize and deserialize not only categories, but also synctargets
	bool serialize(PwMItem &dta);
	/** deserialize the (parsed) XML data and store it in "dta" */
	bool deSerialize(PwMItem *dta);
	/** sets the initial default lockStat we should assign */
	void setDefaultLockStat(bool stat)
			{ defaultLockStat = stat; }

protected:
	/** main data holder */
	QDomDocument *domDoc;
	/** default lockStat to assign */
	bool defaultLockStat;

protected:
	/** check if this is valid PwManager XML data */
	bool checkValid();
	/** read the categories in the node "n" */
	bool readCategories(const QDomNode &n,
			    vector<PwMCategoryItem> *dta);
	/** read the entries in the node "n" */
	bool readEntries(const QDomNode &n,
			 vector<PwMDataItem> *dta);
	/** extract the data out of the given item at "n" */
	bool extractEntry(const QDomNode &n,
			  PwMDataItem *dta);
	/** extract the meta-data */
	bool extractMeta(const QDomNode &n,
			 PwMMetaData *dta);
	/** generates a new root node and sets all initial parameters */
	QDomElement genNewRoot();
	/** add new categories to the XML data stream in e */
	bool addCategories(QDomElement *e,
			   const vector<PwMCategoryItem> &dta);
	/** add the given new entries to the XML data stream in e */
	bool addEntries(QDomElement *e,
			const vector<PwMDataItem> &dta);
	/** do serialize and write the given entry to the XML stream */
	bool writeEntry(QDomElement *e,
			const PwMDataItem &_dta);
	/** write the entry meta data to the xml stream */
	bool writeMeta(QDomElement *e,
		       const PwMMetaData &dta);
	/** escape illegal characters out of the given entry data string */
	QString escapeEntryData(QString dta);
	/** un-escape illegal characters out of the given entry data string */
	QString unescapeEntryData(QString dta);



	//US ENH: the following methods are getting used to write/read sync entries
	/** read the syncentries in the node "n" */
	bool readSyncData(const QDomNode &n,
			    vector<PwMSyncItem> *dta);

	/** add new syncentries to the XML data stream in e */
	bool addSyncData(QDomElement *e,
			   const vector<PwMSyncItem> &dta);

};

#endif // __SERIALIZER_H
