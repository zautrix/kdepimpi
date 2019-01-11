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
 * $Id: serializer.cpp,v 1.15 2008/10/02 20:37:12 zautrix Exp $
 **************************************************************************/  
#include <stdlib.h>
#include "serializer.h"
#include "pwmexception.h"

#ifdef PWM_EMBEDDED
#include <kglobal.h>
#include <klocale.h>
#endif

/* enable/disable serializer debugging (0/1) */
#define SERIALIZER_DEBUG	0
/* use the old xml tags for writing (0/1) */
#define USE_OLD_TAGS		0
/* write a CDATA section (0/1) */
#define WRITE_CDATA_SEC		0


#define META_CREATE_DATE	"c"
#define META_VALID_DATE		"v"
#define META_EXPIRE_DATE	"e"
#define META_UPDATE_DATE	"u"
#define META_UPDATE_INT		"i"
//US ENH : uniqueid and sync information
#define META_UNIQUEID "n" 
#define SYNC_ROOT "s" 
#define SYNC_TARGET_PREFIX "t" 
#define SYNC_TARGET_NAME "n" 


/* This is compatibility stuff.
 * The names of the entries have changed and here are the
 * new and old ones
 */
#define ROOT_MAGIC_OLD		"PwM-xml-dat"
#define VER_STR_OLD		"ver"
#define COMPAT_VER_OLD		"0x02"
#define CAT_ROOT_OLD		"categories"
#define CAT_PREFIX_OLD		"cat_"
#define CAT_NAME_OLD		"name"
//US ENH : optional text for categories
#define CAT_TEXT_OLD		"text"

#define ENTRY_PREFIX_OLD	"entry_"
#define ENTRY_DESC_OLD		"desc"
#define ENTRY_NAME_OLD		"name"
#define ENTRY_PW_OLD		"pw"
#define ENTRY_COMMENT_OLD	"comment"
#define ENTRY_URL_OLD		"url"
#define ENTRY_LAUNCHER_OLD	"launcher"
#define ENTRY_LVP_OLD		"listViewPos"
#define ENTRY_BIN_OLD		"b"
#define ENTRY_META_OLD		"m"

#define ROOT_MAGIC_NEW		"P"
#define VER_STR_NEW		"v"
#define COMPAT_VER_NEW		"2"
#define CAT_ROOT_NEW		"c"
#define CAT_PREFIX_NEW		"c"
#define CAT_NAME_NEW		"n"
//US ENH : optional text for categories
#define CAT_TEXT_NEW		"t"

#define ENTRY_PREFIX_NEW	"e"
#define ENTRY_DESC_NEW		"d"
#define ENTRY_NAME_NEW		"n"
#define ENTRY_PW_NEW		"p"
#define ENTRY_COMMENT_NEW	"c"
#define ENTRY_URL_NEW		"u"
#define ENTRY_LAUNCHER_NEW	"l"
#define ENTRY_LVP_NEW		"v"
#define ENTRY_BIN_NEW		ENTRY_BIN_OLD
#define ENTRY_META_NEW		ENTRY_META_OLD

#if USE_OLD_TAGS != 0
# define ROOT_MAGIC_WR		ROOT_MAGIC_OLD
# define VER_STR_WR		VER_STR_OLD
# define COMPAT_VER_WR		COMPAT_VER_OLD
# define CAT_ROOT_WR		CAT_ROOT_OLD
# define CAT_PREFIX_WR		CAT_PREFIX_OLD
# define CAT_NAME_WR		CAT_NAME_OLD

//US ENH : optional text for categories
# define CAT_TEXT_WR		CAT_TEXT_OLD

# define ENTRY_PREFIX_WR	ENTRY_PREFIX_OLD
# define ENTRY_DESC_WR		ENTRY_DESC_OLD
# define ENTRY_NAME_WR		ENTRY_NAME_OLD
# define ENTRY_PW_WR		ENTRY_PW_OLD
# define ENTRY_COMMENT_WR	ENTRY_COMMENT_OLD
# define ENTRY_URL_WR		ENTRY_URL_OLD
# define ENTRY_LAUNCHER_WR	ENTRY_LAUNCHER_OLD
# define ENTRY_LVP_WR		ENTRY_LVP_OLD
# define ENTRY_BIN_WR		ENTRY_BIN_OLD
# define ENTRY_META_WR		ENTRY_META_OLD
#else
# define ROOT_MAGIC_WR		ROOT_MAGIC_NEW
# define VER_STR_WR		VER_STR_NEW
# define COMPAT_VER_WR		COMPAT_VER_NEW
# define CAT_ROOT_WR		CAT_ROOT_NEW
# define CAT_PREFIX_WR		CAT_PREFIX_NEW
# define CAT_NAME_WR		CAT_NAME_NEW

//US ENH : optional text for categories
# define CAT_TEXT_WR		CAT_TEXT_NEW

# define ENTRY_PREFIX_WR	ENTRY_PREFIX_NEW
# define ENTRY_DESC_WR		ENTRY_DESC_NEW
# define ENTRY_NAME_WR		ENTRY_NAME_NEW
# define ENTRY_PW_WR		ENTRY_PW_NEW
# define ENTRY_COMMENT_WR	ENTRY_COMMENT_NEW
# define ENTRY_URL_WR		ENTRY_URL_NEW
# define ENTRY_LAUNCHER_WR	ENTRY_LAUNCHER_NEW
# define ENTRY_LVP_WR		ENTRY_LVP_NEW
# define ENTRY_BIN_WR		ENTRY_BIN_NEW
# define ENTRY_META_WR		ENTRY_META_NEW
#endif


Serializer::Serializer()
{
	defaultLockStat = true;
//US BUG: I needed to specify a document name. Otherwise impl will not be created for serializing
#ifndef PWM_EMBEDDED
	domDoc = new QDomDocument;
#else
	domDoc = new QDomDocument("mydoc");
#endif
}

Serializer::Serializer(const QCString &buffer)
{
	defaultLockStat = true;
//US BUG: I needed to specify a document name. Otherwise impl will not be created for serializing
#ifndef PWM_EMBEDDED
	domDoc = new QDomDocument;
#else
	domDoc = new QDomDocument("mydoc");
#endif

	if (!parseXml(buffer)) {
		delete domDoc;
#ifndef PWM_EMBEDDED
		throw PwMException(PwMException::EX_PARSE);
#else
		qDebug("Serializer::Serializer : Parse Exception ");
#endif
	}
}

Serializer::~Serializer()
{
	delete_ifnot_null(domDoc);
}

void Serializer::clear()
{
	delete_ifnot_null(domDoc);
	domDoc = new QDomDocument;
}

bool Serializer::parseXml(const QCString &buffer)
{
    //abort();
    //qDebug("parse %s ", buffer.data());
	PWM_ASSERT(domDoc);
#ifndef PWM_EMBEDDED
	if (!domDoc->setContent(buffer, true))
		return false;
#else
#ifdef DESKTOP_VERSION
	if (!domDoc->setContent(buffer, true))
#else
	if (!domDoc->setContent(buffer))
#endif
		return false;
#endif
	if (!checkValid())
		return false;
	return true;
}

QCString Serializer::getXml()
{
	PWM_ASSERT(domDoc);

#ifndef PWM_EMBEDDED
#if defined(PWM_DEBUG) && SERIALIZER_DEBUG != 0
	QCString tmp(domDoc->toCString(8));
	printDebug("<BEGIN Serializer::getXml() dump>\n");
	cout << tmp << endl;
	printDebug("<END Serializer::getXml() dump>");
#endif // DEBUG

	QCString ret(domDoc->toCString(0));
	ret.replace('\n', "");
	return ret;
#else

#if defined(PWM_DEBUG) && SERIALIZER_DEBUG != 0
	QCString tmp("        " + domDoc->toCString());
	printDebug("<BEGIN Serializer::getXml() dump>\n");
	qDebug(tmp);
	cout << tmp << endl;
	printDebug("<END Serializer::getXml() dump>");
#endif // DEBUG

	QCString ret(domDoc->toCString());
	ret.replace(QRegExp("\n"), "");
	return ret;

#endif
}

bool Serializer::serialize(PwMItem &dta)
{
	PWM_ASSERT(domDoc);
	QDomElement root(genNewRoot());
	QDomElement catNode(domDoc->createElement(CAT_ROOT_WR));
	QDomElement syncNode(domDoc->createElement(SYNC_ROOT));
	if (!addSyncData(&syncNode, dta.syncDta))
		return false;
	root.appendChild(syncNode);
	if (!addCategories(&catNode, dta.dta))
		return false;
	root.appendChild(catNode);
	return true;
}

bool Serializer::deSerialize(PwMItem *dta)
{
	PWM_ASSERT(domDoc);
	PWM_ASSERT(dta);
	QDomElement root(domDoc->documentElement());
	QDomNode n;

	dta->clear();
	for (n = root.firstChild(); !n.isNull(); n = n.nextSibling()) {
		// find <categories> ... </categories>
		//      <c>          ... </c>
		if (n.nodeName() == CAT_ROOT_NEW ||
		    n.nodeName() == CAT_ROOT_OLD) {
			if (!readCategories(n, &(dta->dta))) {
				return false;
			}
			continue;
		}
		else if (n.nodeName() == SYNC_ROOT) {
			if (!readSyncData(n, &(dta->syncDta))) {
				return false;
			}
			continue;
		}

			/* NOTE: We can stop processing here, as we
			 *       don't have more nodes in root, yet.
			 */
			return false;

	}
	return true;
}

bool Serializer::readCategories(const QDomNode &n,
				vector<PwMCategoryItem> *dta)
{
	QDomNodeList nl(n.childNodes());
	QDomNode cur;
	QString name;
	QString text;
	unsigned int numCat = nl.count(), i;
	PwMCategoryItem curCat;
	vector<PwMDataItem> curEntr;

	if (!numCat) {
		printDebug("Serializer::readCategories(): empty");
		return false;
	}
	for (i = 0; i < numCat; ++i) {
		cur = nl.item(i);
		if (cur.nodeName().left(1) == CAT_PREFIX_NEW ||
		    cur.nodeName().left(4) == CAT_PREFIX_OLD) {
			name = cur.toElement().attribute(CAT_NAME_NEW);
			if (name == QString::null)
				name = cur.toElement().attribute(CAT_NAME_OLD);
			PWM_ASSERT(name != QString::null);
			PWM_ASSERT(name != "");
			curCat.clear();
			curCat.name = name.latin1();

			//US ENH: new version might include text for description, name and pw
			text = cur.toElement().attribute(CAT_TEXT_NEW);
			if (text == QString::null)
				text = cur.toElement().attribute(CAT_TEXT_OLD);
			if (text != QString::null)
			  {
			    QStringList textlist = QStringList::split(";", text, true);
			    unsigned int num = textlist.count();
			    if (num > 0)
			      curCat.desc_text = textlist[0].latin1();
			    if (num > 1)
			      curCat.name_text = textlist[1].latin1();
			    if (num > 2)
			      curCat.pw_text = textlist[2].latin1();
			  }

			if (!readEntries(cur, &curEntr)) {
				dta->clear();
				return false;
			}
			curCat.d = curEntr;
			dta->push_back(curCat);
		} else {
			printDebug("Serializer::readCategories(): uh? not a category?");
		}
	}
	return true;
}

bool Serializer::readEntries(const QDomNode &n,
			     vector<PwMDataItem> *dta)
{
	QDomNodeList nl(n.childNodes());
	QDomNode cur;
	unsigned int numEntr = nl.count(), i;
	PwMDataItem curEntr;
	//US BUG: to initialize all values of curEntr with meaningfulldata,
	// we call clear on it. Reason: Information in the file we will read might be incomplete.
	// e.g. the metadata is missing.
	curEntr.clear(true);

	dta->clear();
	for (i = 0; i < numEntr; ++i) {
		cur = nl.item(i);
		if (cur.nodeName().left(1) == ENTRY_PREFIX_NEW ||
		    cur.nodeName().left(6) == ENTRY_PREFIX_OLD) {
			if (!extractEntry(cur, &curEntr)) {
				return false;
			}
			dta->push_back(curEntr);
		} else {
			printDebug("Serializer::readEntries(): hm? not an entry?");
		}
	}
	return true;
}

bool Serializer::extractEntry(const QDomNode &n,
			      PwMDataItem *dta)
{
	QDomNodeList nl(n.childNodes());
	QDomNode cur, cdata;
	unsigned int cnt = nl.count(), i;
	QString name, text;

	if (!cnt) {
		printDebug("Serializer::extractEntry(): empty");
		return false;
	}
	dta->clear();
	for (i = 0; i < cnt; ++i) {
		cur = nl.item(i);
		name = cur.nodeName();
		cdata = cur.firstChild();
		if (unlikely(cdata.isCDATASection())) {
			text = cdata.toCDATASection().data();
		} else if (likely(cur.isElement())) {
			text = cur.toElement().text();
		} else {
			printDebug("Serializer::extractEntry(): neither CDATA nor element.");
			return false;
		}
		if (text == " ")
			text = ""; // for backward compatibility.
        //qDebug("entry %s ",unescapeEntryData(text).latin1());
		if (name == ENTRY_DESC_NEW ||
		    name == ENTRY_DESC_OLD) {
			dta->desc = unescapeEntryData(text).latin1();
		} else if (name == ENTRY_NAME_NEW ||
			   name == ENTRY_NAME_OLD) {
			dta->name = unescapeEntryData(text).latin1();
		} else if (name == ENTRY_PW_NEW ||
			   name == ENTRY_PW_OLD) {
			dta->pw = unescapeEntryData(text).latin1();
		} else if (name == ENTRY_COMMENT_NEW ||
			   name == ENTRY_COMMENT_OLD) {
			dta->comment = unescapeEntryData(text).latin1();
		} else if (name == ENTRY_URL_NEW ||
			   name == ENTRY_URL_OLD) {
			dta->url = unescapeEntryData(text).latin1();
		} else if (name == ENTRY_LAUNCHER_NEW ||
			   name == ENTRY_LAUNCHER_OLD) {
			dta->launcher = unescapeEntryData(text).latin1();
		} else if (name == ENTRY_LVP_NEW ||
			   name == ENTRY_LVP_OLD) {
			dta->listViewPos = strtol(text.latin1(), 0, 10);
		} else if (name == ENTRY_BIN_NEW) {
			// ENTRY_BIN_NEW == ENTRY_BIN_OLD
			if (text == "0") {
				dta->binary = false;
			} else {
				dta->binary = true;
			}
		} else if (name == ENTRY_META_NEW) {
			// ENTRY_META_NEW == ENTRY_META_OLD
			if (!extractMeta(cur, &dta->meta))
				return false;
		} else {
			printDebug(string("Serializer::extractEntry(): invalid: ")
				   + name.latin1());
		}
	}
	dta->lockStat = defaultLockStat;
	return true;
}

bool Serializer::extractMeta(const QDomNode &n,
			     PwMMetaData *dta)
{
	QDomNode cur(n.firstChild());
	QString name, val;
	while (!cur.isNull()) {
		name = cur.nodeName();
		val = cur.toElement().text();
		if (val == "") {
			cur = cur.nextSibling();
			continue;
		}

		//US BUG: The transformation of an empty date into an ISO date and back is different on different systems/compilers. 
		//because of that it is possible that here some values are not set, which means they are null.
		//US ENH: at the same moment we need backwardcompatibility. So older versions might have stored invalid dates.

		QDateTime dtval; //dtval should be invalid by definition.

		if ((name == META_CREATE_DATE) ||
		    (name == META_VALID_DATE) ||
		    (name == META_EXPIRE_DATE) ||
		    (name == META_UPDATE_DATE))
		{
		  //qDebug("Serializer::extractMeta:: val:%s, empty:%i, length:%i",val.utf8(), val.isEmpty(), val.length());

#ifndef PWM_EMBEDDED
		  dtval = QDateTime::fromString(val, Qt::ISODate);
#else
		  bool ok;
		  dtval = KGlobal::locale()->readDateTime(val, KLocale::ISODate, &ok);
		    
		  if (ok == false)
		    qDebug("Serializer::extractMeta invalid date or time !!!!!!!!!!!!!");
#endif

		  //if the parsed data is wrong, dtval should be invalid at this time.

		}

		if (name == META_CREATE_DATE) {
			dta->create = dtval;
		} else if (name == META_VALID_DATE) {
			dta->valid = dtval;
		} else if (name == META_EXPIRE_DATE) {
			dta->expire = dtval;
		} else if (name == META_UPDATE_DATE) {
			dta->update = dtval;
		} else if (name == META_UPDATE_INT) {
			dta->updateInt = strtoul(val.latin1(), 0, 10);
		} else if (name == META_UNIQUEID) {
			dta->uniqueid = unescapeEntryData(val).latin1();
		} else {
			printDebug(string("extractMeta(): invalid: ")
				   + name.latin1());
		}

		cur = cur.nextSibling();
	}
	return true;
}

bool Serializer::checkValid()
{
	PWM_ASSERT(domDoc);
	QDomElement root(domDoc->documentElement());
	if (root.nodeName() != ROOT_MAGIC_NEW &&
	    root.nodeName() != ROOT_MAGIC_OLD) {
		printDebug("Serializer: wrong magic");
		return false;
	}
	if (root.attribute(VER_STR_NEW) != COMPAT_VER_NEW &&
	    root.attribute(VER_STR_OLD) != COMPAT_VER_OLD) {
		printDebug("Serializer: wrong version");
		return false;
	}
	return true;
}

QDomElement Serializer::genNewRoot()
{
	PWM_ASSERT(domDoc);
	QDomElement root(domDoc->createElement(ROOT_MAGIC_WR));
	root.setAttribute(VER_STR_WR, COMPAT_VER_WR);
	domDoc->appendChild(root);
	return root;
}

bool Serializer::addCategories(QDomElement *e,
			       const vector<PwMCategoryItem> &dta)
{
	unsigned int numCat = dta.size(), i;
	QString curId, curName;
	QDomElement curCat;

	for (i = 0; i < numCat; ++i) {
		curId = CAT_PREFIX_WR;
		curId += tostr(i).c_str();
		curName = dta[i].name.c_str();
		curCat = domDoc->createElement(curId);
		curCat.setAttribute(CAT_NAME_WR, curName);

		//US ENH: new version includes text for description, name and pw
		QStringList curTextList;
		curTextList << dta[i].desc_text.c_str();
		curTextList << dta[i].name_text.c_str();
		curTextList << dta[i].pw_text.c_str();
		QString text = curTextList.join(";");
		curCat.setAttribute(CAT_TEXT_WR, text);


		if (!addEntries(&curCat, dta[i].d)) {
			return false;
		}
		e->appendChild(curCat);
	}
	return true;
}

bool Serializer::addEntries(QDomElement *e,
			    const vector<PwMDataItem> &dta)
{
	unsigned int numEntr = dta.size(), i;
	QString curId;
	QDomElement curEntr;

	for (i = 0; i < numEntr; ++i) {
		curId = ENTRY_PREFIX_WR;
		curId += tostr(i).c_str();
		curEntr = domDoc->createElement(curId);
		if (!writeEntry(&curEntr, dta[i])) {
			return false;
		}
		e->appendChild(curEntr);
	}
	return true;
}

bool Serializer::writeEntry(QDomElement *e,
			    const PwMDataItem &_dta)
{
#if WRITE_CDATA_SEC != 0
# define new_text(x)	domDoc->createCDATASection(x)
	QDomCDATASection curText;
#else
# define new_text(x)	domDoc->createTextNode(x)
	QDomText curText;
#endif

	QDomText plainText;
	QDomElement tag;

	// begin -- This is for compatibility with the old serializer
	PwMDataItem dta = _dta;
	if (!dta.desc.size())
		dta.desc = " ";
	if (!dta.name.size())
		dta.name = " ";
	if (!dta.pw.size())
		dta.pw = " ";
	if (!dta.comment.size())
		dta.comment = " ";
	if (!dta.url.size())
		dta.url = " ";
	if (!dta.launcher.size())
		dta.launcher = " ";
	// end -- This is for compatibility with the old serializer

	tag = domDoc->createElement(ENTRY_DESC_WR);
	curText = new_text(escapeEntryData(dta.desc.c_str()));
	tag.appendChild(curText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_NAME_WR);
	curText = new_text(escapeEntryData(dta.name.c_str()));
	tag.appendChild(curText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_PW_WR);
	curText = new_text(escapeEntryData(dta.pw.c_str()));
	tag.appendChild(curText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_COMMENT_WR);
	curText = new_text(escapeEntryData(dta.comment.c_str()));
	tag.appendChild(curText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_URL_WR);
	curText = new_text(escapeEntryData(dta.url.c_str()));
	tag.appendChild(curText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_LAUNCHER_WR);
	curText = new_text(escapeEntryData(dta.launcher.c_str()));
	tag.appendChild(curText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_LVP_WR);
	plainText = domDoc->createTextNode(tostr(dta.listViewPos).c_str());
	tag.appendChild(plainText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_BIN_WR);
	if (dta.binary)
		plainText = domDoc->createTextNode("1");
	else
		plainText = domDoc->createTextNode("0");
	tag.appendChild(plainText);
	e->appendChild(tag);

	tag = domDoc->createElement(ENTRY_META_WR);
	if (!writeMeta(&tag, dta.meta))
		return false;
	e->appendChild(tag);

#undef new_text
	return true;
}

bool Serializer::writeMeta(QDomElement *e,
			   const PwMMetaData &dta)
{
	QDomText text;
	QDomElement tag;

	//US BUG!!!: The transformation of an empty date into an ISO date is different on different systems/compilers.
	//So do not transform an empty value at all.
	if (dta.create.isValid())
	{
	  tag = domDoc->createElement(META_CREATE_DATE);
#ifndef PWM_EMBEDDED
	  text = domDoc->createTextNode(dta.create.toString(Qt::ISODate));
#else
	  text = domDoc->createTextNode(KGlobal::locale()->formatDateTime(dta.create,  KLocale::ISODate));
#endif
	  tag.appendChild(text);
	  e->appendChild(tag);
	}

	//US BUG!!!: The transformation of an empty date into an ISO date is different on different systems/compilers. 
	//So do not transform an empty value at all.
	if (dta.valid.isValid())
	{
	  tag = domDoc->createElement(META_VALID_DATE);
#ifndef PWM_EMBEDDED
	  text = domDoc->createTextNode(dta.valid.toString(Qt::ISODate));
#else
	  text = domDoc->createTextNode(KGlobal::locale()->formatDateTime(dta.valid,  KLocale::ISODate));
#endif
	  tag.appendChild(text);
	  e->appendChild(tag);
	}

	//US BUG!!!: The transformation of an empty date into an ISO date is different on different systems/compilers. 
	//So do not transform an empty value at all.
	if (dta.expire.isValid())
	{
	  tag = domDoc->createElement(META_EXPIRE_DATE);
#ifndef PWM_EMBEDDED
	  text = domDoc->createTextNode(dta.expire.toString(Qt::ISODate));
#else
	  text = domDoc->createTextNode(KGlobal::locale()->formatDateTime(dta.expire,  KLocale::ISODate));
#endif
	  tag.appendChild(text);
	  e->appendChild(tag);
	}

	//US BUG!!!: The transformation of an empty date into an ISO date is different on different systems/compilers. 
	//So do not transform an empty value at all.
	if (dta.update.isValid())
	{
	  tag = domDoc->createElement(META_UPDATE_DATE);
#ifndef PWM_EMBEDDED
	  text = domDoc->createTextNode(dta.update.toString(Qt::ISODate));
#else
	  text = domDoc->createTextNode(KGlobal::locale()->formatDateTime(dta.update,  KLocale::ISODate));
#endif
	  tag.appendChild(text);
	  e->appendChild(tag);
	}

	tag = domDoc->createElement(META_UPDATE_INT);
	text = domDoc->createTextNode(tostr(dta.updateInt).c_str());
	tag.appendChild(text);
	e->appendChild(tag);

	tag = domDoc->createElement(META_UNIQUEID);
	text = domDoc->createTextNode(escapeEntryData(dta.uniqueid.c_str()));
	tag.appendChild(text);
	e->appendChild(tag);

#undef new_text
	return true;
}

QString Serializer::escapeEntryData(QString dta)
{
#ifndef PWM_EMBEDDED
	dta.replace('\n', "$>--endl--<$");
	dta.replace("]]>", "||>");
#else
	dta.replace(QRegExp("\n"), "$>--endl--<$");
	dta.replace(QRegExp("]]>"), "||>");
#endif
	return dta;
}

QString Serializer::unescapeEntryData(QString dta)
{
#ifndef PWM_EMBEDDED
	dta.replace("$>--endl--<$", "\n");
	dta.replace("||>", "]]>");
#else
#ifdef DESKTOP_VERSION
	dta.replace("$>--endl--<$", "\n");
	dta.replace("||>", "]]>");
#else
	dta.replace(QRegExp("\\$>--endl--<\\$"), "\n");
	dta.replace(QRegExp("||>"), "]]>");
#endif
#endif
	return dta;
}


//US ENH: the following methods are getting used to write/read sync entries
/** read the syncentries in the node "n" */
bool Serializer::readSyncData(const QDomNode &n, vector<PwMSyncItem> *dta)
{
	QDomNodeList nl(n.childNodes());
	QDomNode cur;
	
	QString devicename, val;
	unsigned int numSync  = nl.count(), i;
	PwMSyncItem curSync;
	bool ok = true;

	if (!numSync) {
	  //no sync entries is a possible result 
		printDebug("Serializer::readSyncData(): empty");
		return true;
	}
	for (i = 0; i < numSync; ++i) {
		cur = nl.item(i);
		if (cur.nodeName().left(1) == SYNC_TARGET_PREFIX) {
			devicename = cur.toElement().attribute(SYNC_TARGET_NAME);
			val = cur.toElement().text();

			if ((val == "") || (devicename == QString::null)) {
			  printDebug("Serializer::readSyncData(): empty synctarget name or syncdate");
			  continue;
			}

			curSync.syncName = devicename.latin1();
#ifndef PWM_EMBEDDED
			curSync.lastSyncDate = QDateTime::fromString(val, Qt::ISODate);
#else
			curSync.lastSyncDate = KGlobal::locale()->readDateTime(val, KLocale::ISODate, &ok);
			if (ok == false)
			  qDebug("Serializer::readSyncData(): could not parse syncdate:%s",val.latin1());

#endif
			dta->push_back(curSync);
		}
	}
	return true;

}



bool Serializer::addSyncData(QDomElement *e,
			       const vector<PwMSyncItem> &dta)
{
	unsigned int numSync = dta.size(), i;
	QString curId, curDeviceName;
	QDomElement curSync;
	QDomText text;

	for (i = 0; i < numSync; ++i) {
		curId = SYNC_TARGET_PREFIX;
		curId += tostr(i).c_str();
		curDeviceName = dta[i].syncName.c_str();
		curSync = domDoc->createElement(curId);
		curSync.setAttribute(SYNC_TARGET_NAME, curDeviceName);

#ifndef PWM_EMBEDDED
		text = domDoc->createTextNode(dta[i].lastSyncDate.toString(Qt::ISODate));
#else
		text = domDoc->createTextNode(KGlobal::locale()->formatDateTime(dta[i].lastSyncDate,  KLocale::ISODate));
#endif
		curSync.appendChild(text);

		e->appendChild(curSync);

	}
	return true;
}

