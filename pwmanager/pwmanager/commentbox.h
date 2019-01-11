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
 * $Id: commentbox.h,v 1.3 2005/03/03 13:42:15 zautrix Exp $
 **************************************************************************/  

#ifndef COMMENTBOX_H
#define COMMENTBOX_H

#include <qstring.h>
#include <qsize.h>
class QWidget;
class QTextEdit;
class KHTMLPart;

#ifndef PWM_EMBEDDED

class CommentBox 
{
protected:
	enum commentBoxMode
	{
		mode_notSet = 0,
		mode_text,
		mode_html
	};

public:
	CommentBox(QWidget *_parentWidget);
	~CommentBox();

	/** clear all data in the comment box */
	void clear();
	/** show the comment box */
	void show();
	/** hide the comment box */
	void hide();
	/** resize the comment box */
	void resize(const QSize &size);
	void resize(int w, int h)
			{ resize(QSize(w, h)); }
	/** get the size of the comment box */
	QSize size();
	/** if neccessary switch to text-mode and
	  * insert this text into the comment box
	  */
	void setText(const QString &text);
	/** get the text of the comment box.
	  * If it's not in text-mode it returns false
	  */
	bool getText(QString *text);
	/** if neccessary switch to HTML-mode and
	  * insert this html code into the comment box
	  */
	void setHtml(QString code);
	/** checks "dta" for its type, sets the correct
	  * mode and writes "dta" to the comment box
	  */
	void setContent(const QString &dta);

protected:
	/** switch the current mode */
	void switchTo(commentBoxMode newMode);
	/** clear all text data */
	void clearText();
	/** clear all HTML data */
	void clearHtml();

protected:
	/** parent widget for this comment box */
	QWidget *parentWidget;
	/** current comment box usage type */
	commentBoxMode mode;
	/** if the comment box is a normal textbox, data is stored here */
	QTextEdit *textDta;
	/** if the comment box is a HTML box, data is stored here */
	KHTMLPart *htmlDta;
};

#else
#include <qtextbrowser.h>
/** Implementation of the advanced HTML comment box */
//US ENH: CommentBox must be derived from QWidget, to allow the splitter to set a initial size
// without conflicting with the two display modes 

class CommentBox : public QTextBrowser 
{
public:
	CommentBox(QWidget *_parentWidget);
	~CommentBox();

	/** clear all data in the comment box */
	void clear();
	/** if neccessary switch to text-mode and
	  * insert this text into the comment box
	  */
	void setText(const QString &text);
	/** get the text of the comment box.
	  * If it's not in text-mode it returns false
	  */
	bool getText(QString *text);
	/** if neccessary switch to HTML-mode and
	  * insert this html code into the comment box
	  */
	void setContent(const QString &dta);

};
#endif


#endif
