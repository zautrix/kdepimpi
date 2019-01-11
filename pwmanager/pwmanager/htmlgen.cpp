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
 * $Id: htmlgen.cpp,v 1.2 2004/10/23 17:05:41 zautrix Exp $
 **************************************************************************/  

#include "htmlgen.h"
#include "pwmexception.h"

#include <kstandarddirs.h>

/** enable/disable HTML-generator debugging (0/1) */
#define HTMLGEN_DEBUG	0


#define HTML_DOCTYPE_HDR	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
#define HTML_PWM_HDR		"<!-- PwManager generated HTML -->"
#define HTML_COMMENT_HDR	"<!-- type: comment -->"
#define HTML_COMMENTVER_HDR	"<!-- ver: 0.1 -->"
#define HTML_STYLESHEET_DUMMY	"@STYLESHEET@"
#define HTML_GLOBTBL_CLASS	"\"globtable\""
#define HTML_GLOBTITLE_CLASS	"\"globtitle\""
#define HTML_SUBTBL_CLASS	"\"subtable\""
#define HTML_SUBTITLE_CLASS	"\"subtitle\""
#define HTML_ENTRY_CLASS	"\"entry\""
#define HTML_VALUE_CLASS	"\"value\""
#define PATH_COMMENTSTYLE_CSS	"pwmanager/html/htmlcomment_style.css"
#if defined(PWM_DEBUG) && HTMLGEN_DEBUG != 0
#define HTML_ENDL		"\n"
#else // defined(PWM_DEBUG) && ...
#define HTML_ENDL		""
#endif // defined(PWM_DEBUG) && ...


HtmlGen::HtmlGen()
{
	useSSDummy = true;
}

HtmlGen::~HtmlGen()
{
}

QString HtmlGen::escapeHtmlText(const QString &str)
{
	QString ret;
	unsigned int len = str.length(), i;
	char c;
	for (i = 0; i < len; ++i) {
		c = str.at(i);
		switch (c) {
		case '<':
			ret.append("&lt;");
			break;
		case '>':
			ret.append("&gt;");
			break;
		case '&':
			ret.append("&amp;");
			break;
		case '\"':
			ret.append("&quot;");
			break;
		case 'ä':
			ret.append("&auml;");
			break;
		case 'Ä':
			ret.append("&Auml;");
			break;
		case 'ü':
			ret.append("&uuml;");
			break;
		case 'Ü':
			ret.append("&Uuml;");
			break;
		case 'ö':
			ret.append("&ouml;");
			break;
		case 'Ö':
			ret.append("&Ouml;");
			break;
		case 'ß':
			ret.append("&szlig;");
			break;
		case '¿':
			ret.append("&euro;");
			break;
		default:
			ret.append(c);
		}
	}
	return ret;
}

bool HtmlGen::isHtml(const QString &dta)
{
	int ret;
	ret = dta.find("<html>", 0, false);
	if (ret == -1)
		return false;
	ret = dta.find("<head>", ret, false);
	if (ret == -1)
		return false;
	return true;
}

QString HtmlGen::getStyleSheetHtml()
{
	QString ret;
	ret = "<link rel=\"stylesheet\" href=\"";
	QString cssPath(::locate("data", PATH_COMMENTSTYLE_CSS));
	if ((cssPath == QString::null) || (cssPath == "")) {
		printDebug("HtmlGen::getStyleSheetHtml(): not found");
		return "";
	}
	ret += cssPath;
	ret += "\" type=\"text/css\">" HTML_ENDL;
	return ret;
}

bool HtmlGen::replaceSSDummy(QString *doc)
{
	int beginPos = doc->find(HTML_STYLESHEET_DUMMY);
	if (beginPos == -1) {
		printDebug("HtmlGen::replaceSSDummy(): not found");
		return false;
	}
	*doc = doc->replace(beginPos, strlen(HTML_STYLESHEET_DUMMY),
			    getStyleSheetHtml());
	return true;
}

QString HtmlGen::genHtmlComment(const HtmlComment *dta)
{
	QString ret(HTML_DOCTYPE_HDR
		    HTML_PWM_HDR	HTML_ENDL
		    HTML_COMMENT_HDR	HTML_ENDL
		    HTML_COMMENTVER_HDR	HTML_ENDL);
	ret += "<html>" HTML_ENDL;
	if (!appendCommentHeader(&ret))
		return "";
	if (!appendCommentBody(&ret, dta))
		return "";
	ret += "</html>" HTML_ENDL;

#if defined(PWM_DEBUG) && HTMLGEN_DEBUG != 0
	printDebug("<BEGIN HtmlGen::genHtmlComment() dump>");
	cout << ret << endl;
	printDebug("<END HtmlGen::genHtmlComment() dump>");
#endif // DEBUG
	return ret;
}

bool HtmlGen::appendCommentHeader(QString *str)
{
	*str += "<head>" HTML_ENDL;
	if (useSSDummy) {
		*str += HTML_STYLESHEET_DUMMY	HTML_ENDL;
	} else {
		QString ssLine(getStyleSheetHtml());
		if (ssLine.isEmpty())
			return false;
		*str += ssLine;
	}
	*str += "</head>" HTML_ENDL;
	return true;
}

bool HtmlGen::appendCommentBody(QString *str,
				const HtmlComment *dta)
{
	*str += "<body>" HTML_ENDL;
	if (!appendCommentGlobTbl(str, dta))
		return false;
	*str += "</body>" HTML_ENDL;
	return true;
}

bool HtmlGen::appendCommentGlobTbl(QString *str,
				   const HtmlComment *dta)
{
	*str += "<table class=" HTML_GLOBTBL_CLASS ">" HTML_ENDL;
	*str += "<tr><th class=" HTML_GLOBTITLE_CLASS ">";
	*str += escapeHtmlText(dta->getTitle());
	*str += "</th></tr>" HTML_ENDL;

	const vector<HtmlComment::SubTable> *subTbls = dta->getSubTableList();
	vector<HtmlComment::SubTable>::const_iterator i = subTbls->begin(),
						      end = subTbls->end();
	while (i != end) {
		*str += "<tr><td>" HTML_ENDL;
		if (!appendCommentSubTbl(str, &(*i)))
			return false;
		++i;
		*str += "</td></tr>" HTML_ENDL;
	}

	*str += "</table>" HTML_ENDL;
	return true;
}

bool HtmlGen::appendCommentSubTbl(QString *str,
				  const HtmlComment::SubTable *dta)
{
	*str += "<table class=" HTML_SUBTBL_CLASS ">" HTML_ENDL;
	*str += "<tr><th colspan=\"2\" class=" HTML_SUBTITLE_CLASS ">";
	*str += escapeHtmlText(dta->getTitle());
	*str += "</th></tr>" HTML_ENDL;

	const vector< pair<QString, QString> > *entries = dta->getEntryList();
	vector< pair<QString, QString> >::const_iterator i = entries->begin(),
							 end = entries->end();
	while (i != end) {
		*str += "<tr>" HTML_ENDL;
		if (!appendCommentSubTblEntry(str, &(*i)))
			return false;
		*str += "</tr>" HTML_ENDL;
		++i;
	}

	*str += "</table>" HTML_ENDL;
	return true;
}

bool HtmlGen::appendCommentSubTblEntry(QString *str,
				       const pair<QString, QString> *dta)
{
	*str += "<td class=" HTML_ENTRY_CLASS ">";
	*str += escapeHtmlText(dta->first);
	*str += "</td>" HTML_ENDL;
	*str += "<td class=" HTML_VALUE_CLASS ">";
	*str += escapeHtmlText(dta->second);
	*str += "</td>" HTML_ENDL;
	return true;
}
