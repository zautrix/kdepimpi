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

#include "htmlparse.h"
#include "pwmexception.h"

#include <dom/html_document.h>
#include <dom/html_element.h>
#include <dom/html_misc.h>


/** enable/disable HTML-parser debugging (0/1) */
#define HTMLPARSE_DEBUG		1


#if !defined(PWM_DEBUG) || HTMLPARSE_DEBUG == 0
# undef printDebug
# define printDebug(x)	do {} while (0)
#endif

HtmlParse::HtmlParse()
{
}

HtmlParse::~HtmlParse()
{
}

bool HtmlParse::parseHtmlComment(const QString &dta,
				  HtmlComment *ret)
{
	PWM_ASSERT(ret);
	ret->clear();
	DOM::HTMLDocument curDoc(static_cast<KHTMLView *>(0));
	curDoc.open();
	curDoc.write(dta);
	curDoc.close();
	DOM::HTMLElement body(curDoc.body());
	DOM::HTMLCollection children(body.children());

	unsigned long i, numCh = children.length();
	if (numCh != 1) {
		/* we currently support only one global table */
		printDebug("HtmlParse::parseHtmlComment(): global children cnt != 1");
		return false;
	}
	DOM::DOMString nodeName;
	DOM::Node curNode;
	for (i = 0; i < numCh; ++i) {
		curNode = children.item(i);
		nodeName = curNode.nodeName();
		if (nodeName == "table") {
			if (!parseCommentGlobTbl(curNode, ret))
				return false;
		} else {
			// We don't support something else than tables, yet.
			printDebug("HtmlParse::parseHtmlComment(): unknown node");
		}
	}
	return true;
}

bool HtmlParse::parseCommentGlobTbl(const DOM::Node &node,
				    HtmlComment *ret)
{
	PWM_ASSERT(node.nodeName() == "table");
	DOM::Node bodyNode(node.firstChild());
	PWM_ASSERT(bodyNode.nodeName() == "tbody");
	DOM::Node curNode(bodyNode.firstChild());
	DOM::DOMString nodeName;
	ret->clear();
	while (!curNode.isNull()) {
		nodeName = curNode.nodeName();
		if (nodeName == "tr") {
			if (!parseCommentGlobTblRow(curNode, ret))
				return false;
		} else {
			printDebug("HtmlParse::parseCommentGlobTbl(): node unknown");
		}
		curNode = curNode.nextSibling();
	}
	return true;
}

bool HtmlParse::parseCommentGlobTblRow(const DOM::Node &node,
				       HtmlComment *ret)
{
	DOM::Node curNode(node.firstChild()), child;
	DOM::DOMString nodeName;
	HtmlComment::SubTable subTbl;
	while (!curNode.isNull()) {
		nodeName = curNode.nodeName();
		if (nodeName == "th") {
			// global title
			ret->setTitle(curNode.firstChild().nodeValue().string());
		} else if (nodeName == "td") {
			child = curNode.firstChild();
			if (child.nodeName() == "table" &&
			    child.firstChild().nodeName() == "tbody") {
				// we have a sub-table
				if (!parseCommentSubTbl(child.firstChild(), &subTbl))
					return false;
				ret->addSubtable(subTbl);
			} else {
				printDebug("HtmlParse::parseCommentGlobTblRow(): subelement unknown");
			}
		} else {
			printDebug("HtmlParse::parseCommentGlobTblRow(): node unknown");
		}
		curNode = curNode.nextSibling();
	}
	return true;
}

bool HtmlParse::parseCommentSubTbl(const DOM::Node &node,
				   HtmlComment::SubTable *ret)
{
	PWM_ASSERT(node.nodeName() == "tbody");
	DOM::Node curNode(node.firstChild());
	DOM::DOMString nodeName;
	ret->clear();
	while (!curNode.isNull()) {
		nodeName = curNode.nodeName();
		if (nodeName == "tr") {
			if (!parseCommentSubTblRow(curNode, ret))
				return false;
		} else {
			printDebug("HtmlParse::parseCommentSubTbl(): node unknown");
		}
		curNode = curNode.nextSibling();
	}
	return true;
}

bool HtmlParse::parseCommentSubTblRow(const DOM::Node &node,
				      HtmlComment::SubTable *ret)
{
	DOM::Node curNode(node.firstChild()), child;
	DOM::DOMString nodeName;
	pair<QString, QString> curEntr;
	while (!curNode.isNull()) {
		nodeName = curNode.nodeName();
		if (nodeName == "th") {
			// sub title
			ret->setTitle(curNode.firstChild().nodeValue().string());
		} else if (nodeName == "td") {
			child = curNode.firstChild();
			if (child.nodeName() == "#text") {
				if (!parseCommentSubTblEntry(curNode, &curEntr))
					return false;
				ret->addEntry(curEntr);
				return true;
			} else {
				printDebug("HtmlParse::parseCommentSubTblRow(): subelement unknown");
			}
		} else {
			printDebug("HtmlParse::parseCommentGlobTblRow(): node unknown");
		}
		curNode = curNode.nextSibling();
	}
	return true;
}

bool HtmlParse::parseCommentSubTblEntry(const DOM::Node &node,
					pair<QString, QString> *ret)
{
	DOM::Node curChild(node);
	if (curChild.isNull())
		return false;
	ret->first = curChild.firstChild().nodeValue().string();
	curChild = curChild.nextSibling();
	if (curChild.isNull())
		return false;
	ret->second = curChild.firstChild().nodeValue().string();
	return true;
}
