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

#ifndef HTMLPARSE_H
#define HTMLPARSE_H

#include "htmlgen.h"

#include <dom/dom_node.h>

#include <utility>

using std::pair;

class HtmlParse
{
public:
	HtmlParse();
	~HtmlParse();
	
	/** parse the given HTML data and return the HtmlComment */
	bool parseHtmlComment(const QString &dta, HtmlComment *ret);

protected:
	/** parse a global html comment table */
	bool parseCommentGlobTbl(const DOM::Node &node, HtmlComment *ret);
	/** parse a row of the global table */
	bool parseCommentGlobTblRow(const DOM::Node &node, HtmlComment *ret);
	/** parse a comment sub table */
	bool parseCommentSubTbl(const DOM::Node &node, HtmlComment::SubTable *ret);
	/** parse a row of comment sub table */
	bool parseCommentSubTblRow(const DOM::Node &node, HtmlComment::SubTable *ret);
	/** parse a comment subtable entry */
	bool parseCommentSubTblEntry(const DOM::Node &node, pair<QString, QString> *ret);
};

#endif
