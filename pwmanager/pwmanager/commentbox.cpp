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
 * $Id: commentbox.cpp,v 1.6 2005/03/03 13:42:15 zautrix Exp $
 **************************************************************************/  

#include "commentbox.h"
#include "pwmexception.h"
#include "htmlgen.h"

#include <klocale.h>

#ifndef PWM_EMBEDDED
#include <khtml_part.h>
#include <khtmlview.h>
#include <qtextedit.h>
#else
#include <qmultilineedit.h>
#endif



#ifndef PWM_EMBEDDED
CommentBox::CommentBox(QWidget *_parentWidget)
{
	PWM_ASSERT(_parentWidget);
	parentWidget = _parentWidget;
	textDta = 0;
	htmlDta = 0;
	mode = mode_notSet;
    setFont( prefs->mViewFont );
}

CommentBox::~CommentBox()
{
	clearText();
	clearHtml();
}

void CommentBox::clear()
{
	clearText();
	clearHtml();
	mode = mode_notSet;
	this->hide();
}

void CommentBox::clearText()
{
	delete_ifnot_null(textDta);
}

void CommentBox::clearHtml()
{
  delete_ifnot_null(htmlDta);
}

void CommentBox::setText(const QString &text)
{
	switchTo(mode_text);
	PWM_ASSERT(textDta);
	textDta->setText( text);
	if (!textDta->isVisible())
		textDta->show();
}

bool CommentBox::getText(QString *text)
{
	if (mode != mode_text)
		return false;
	PWM_ASSERT(text);
	if (!textDta) {
		*text = "";
		return true;
	}
	*text = textDta->text();
	return true;
}

void CommentBox::setHtml(QString code)
{
	switchTo(mode_html);
	PWM_ASSERT(htmlDta);
	if (!HtmlGen::replaceSSDummy(&code))
		printWarn("CommentBox::setHtml(): replaceSSDummy() failed!");
	htmlDta->begin();
	htmlDta->write(code);
	htmlDta->end();
	htmlDta->show();
}

void CommentBox::setContent(const QString &dta)
{
	// if there's no data, hide the comment-box
	if (dta.isEmpty()) {
		clear();
		return;
	}
	if (HtmlGen::isHtml(dta)) {
		setHtml(dta);
		return;
	}
	// we assume it's plain text
	setText(dta);
}

void CommentBox::switchTo(commentBoxMode newMode)
{
	if (newMode == mode)
		return;

	// cleanup old mode
	switch (mode) {
	case mode_text:
		clearText();
		break;
	case mode_html:
		clearHtml();
		break;
	default:
		break;
	}

	// setup new mode
	switch (newMode) {
	case mode_text:
		textDta = new QTextEdit(parentWidget);
		textDta->setTextFormat(Qt::PlainText);
		textDta->setReadOnly(true);
		textDta->show();
		break;
	case mode_html:
		htmlDta = new KHTMLPart(parentWidget, 0,
					parentWidget);
		htmlDta->show();
		break;
	default:
		BUG();
		break;
	}

	mode = newMode;
}

void CommentBox::show()
{
	switch (mode) {
	case mode_text:
		PWM_ASSERT(textDta);
		textDta->show();
		break;
	case mode_html:
		PWM_ASSERT(htmlDta);
		htmlDta->show();
		break;
	default:
		break;
	}

}

void CommentBox::hide()
{
	switch (mode) {
	case mode_text:
		PWM_ASSERT(textDta);
		textDta->hide();
		break;
	case mode_html:
		PWM_ASSERT(htmlDta);
		htmlDta->hide();
		break;
	default:
		break;
	}
}

void CommentBox::resize(const QSize &size)
{
	switch (mode) {
	case mode_text:
		PWM_ASSERT(textDta);
		textDta->resize(size);
		break;
	case mode_html:
 		PWM_ASSERT(htmlDta);
		htmlDta->view()->resize(size);
		break;
	default:
		break;
	}

}

QSize CommentBox::size()
{
	switch (mode) {
	case mode_text:
		PWM_ASSERT(textDta);
		return textDta->size();
		break;
	case mode_html:
		PWM_ASSERT(htmlDta);
		return htmlDta->view()->size();
		break;
	default:
		break;
	}

	return QSize();
}


////////////////////////////////////////////////////////////////////////

#else

CommentBox::CommentBox(QWidget *_parentWidget)
  : QTextBrowser(_parentWidget) 

{
    //this->setReadOnly(true);
  setFocusPolicy( QWidget::ClickFocus );
}

CommentBox::~CommentBox()
{
}

void CommentBox::clear()
{
	this->hide();
}


void CommentBox::setText(const QString &text)
{
	QTextBrowser::setText( text);
	if (!isVisible())
		show();
}

bool CommentBox::getText(QString *text)
{
	*text = this->text();
	return true;
}

void CommentBox::setContent(const QString &dta)
{
	// if there's no data, hide the comment-box
	if (dta.isEmpty()) {
		clear();
		return;
	}

	// we assume it's plain text
	setText(dta);
}

#endif









