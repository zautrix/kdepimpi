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
 * $Id: htmlgen.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include <qstring.h>

#include <utility>
#include <vector>

using std::vector;
using std::pair;

/** internal representation of the advanced HTML-comment */
class HtmlComment
{
public:
	class SubTable
	{
	public:
		SubTable() {}
		~SubTable() {}

		void clear()
			{
				title = "";
				entries.clear();
			}
		void setTitle(const QString &_title)
				{ title = _title; }
		QString getTitle() const
				{ return title; }
		void addEntry(const pair<QString, QString> &entry)
				{ entries.push_back(entry); }
		void setEntries(const vector< pair<QString, QString> > *_entries)
				{ entries = *_entries; }
		const vector< pair<QString, QString> > * getEntryList() const
				{ return &entries; }

	protected:
		/** sub-title */
		QString title;
		/** list of entries */
		vector< pair<QString, QString> > entries;
	};

public:
	HtmlComment() {}
	~HtmlComment() {}

	void clear()
		{
			title = "";
			subTables.clear();
		}
	void setTitle(const QString &_title)
			{ title = _title; }
	QString getTitle() const
			{ return title; }
	void addSubtable(const SubTable &subTable)
			{ subTables.push_back(subTable); }
	void eraseSubtable(int index)
			{ subTables.erase(subTables.begin() + index); }
	const SubTable & subtableAt(int index)
			{ return subTables[index]; }
	void setSubtblAt(int index, const SubTable &entry)
			{ subTables[index] = entry; }
	const vector<SubTable> * getSubTableList() const
			{ return &subTables; }

protected:
	/** global title */
	QString title;
	/** list of sub-tables */
	vector<SubTable> subTables;
};

/** HTML generator for the comment-box */
class HtmlGen
{
public:
	HtmlGen();
	~HtmlGen();

	/** replace the @STYLESHEET@ dummy in the "doc" HTML document */
	static bool replaceSSDummy(QString *doc);
	/** check whether "dta" is HTML-code */
	static bool isHtml(const QString &dta);

	/** insert the stylesheet link as dummy? default is true */
	void styleSheetDummy(bool dummy)
			{ useSSDummy = dummy; }
	/** generate a new html-comment */
	QString genHtmlComment(const HtmlComment *dta);

protected:
	/** converts the string "str" into HTML-text and ensures
	  * that there are no collisions with HTML-tags
	  */
	QString escapeHtmlText(const QString &str);
	/** get the "stylesheet-line" to import the CSS style in HTML */
	static QString getStyleSheetHtml();
	/** append the HTML header to the data stream */
	bool appendCommentHeader(QString *str);
	/** append the HTML body for the comment */
	bool appendCommentBody(QString *str, const HtmlComment *dta);
	/** append a global comment table */
	bool appendCommentGlobTbl(QString *str, const HtmlComment *dta);
	/** append a comment subtable */
	bool appendCommentSubTbl(QString *str, const HtmlComment::SubTable *dta);
	/** append a subtable entry */
	bool appendCommentSubTblEntry(QString *str, const pair<QString, QString> *dta);

protected:
	/** use stylesheet dummy */
	bool useSSDummy;
};

#endif
