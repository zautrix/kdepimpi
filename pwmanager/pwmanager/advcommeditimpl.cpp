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
 * $Id: advcommeditimpl.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $
 **************************************************************************/  

#include "advcommeditimpl.h"
#include "htmlgen.h"
#include "htmlparse.h"
#include "pwmexception.h"
#include "subtbleditimpl.h"

#include <qlineedit.h>
#include <qlabel.h>

#include <khtml_part.h>
#include <khtmlview.h>


AdvCommEditImpl::AdvCommEditImpl(QWidget* parent,
				 const char* name,
				 WFlags fl)
 : advCommEdit(parent, name, fl)
{
	dta = new HtmlComment;
	preview = new KHTMLPart(previewDummy);
	preview->view()->resize(previewDummy->size());
	preview->show();
}

AdvCommEditImpl::~AdvCommEditImpl()
{
	delete dta;
	delete preview;
}

void AdvCommEditImpl::okButton_slot()
{
	updateTitle();
	done(0);
}

void AdvCommEditImpl::cancelButton_slot()
{
	done(1);
}

void AdvCommEditImpl::addSubtblButton_slot()
{
	SubTblEditImpl editor(this);
	if (editor.exec())
		return;
	QString subTitle(editor.getTitle());
	HtmlComment::SubTable subTbl;
	subTbl.setTitle(subTitle);
	subTbl.setEntries(editor.getEntries());
	dta->addSubtable(subTbl);
	entryListBox->insertItem(subTitle);
	updatePreview();
}

void AdvCommEditImpl::editButton_slot()
{
	int index = curIndex();
	if (index == -1)
		return;
	SubTblEditImpl editor(this);
	HtmlComment::SubTable subTbl = dta->subtableAt(index);
	editor.setContent(subTbl.getTitle(), subTbl.getEntryList());
	if (editor.exec())
		return;
	QString subTitle(editor.getTitle());
	subTbl.setTitle(subTitle);
	subTbl.setEntries(editor.getEntries());
	dta->setSubtblAt(index, subTbl);
	entryListBox->changeItem(subTitle, index);
	updatePreview();
}

void AdvCommEditImpl::delButton_slot()
{
	int index = curIndex();
	if (index == -1)
		return;
	dta->eraseSubtable(index);
	entryListBox->removeItem(index);
	updatePreview();
}

void AdvCommEditImpl::updatePreview()
{
	updateTitle();
	HtmlGen htmlGen;
	htmlGen.styleSheetDummy(false);
	QString code(htmlGen.genHtmlComment(dta));
	if (code.isEmpty())
		return;
	preview->begin();
	preview->write(code);
	preview->end();
}

void AdvCommEditImpl::updateTitle()
{
	dta->setTitle(titleLineEdit->text());
}

QString AdvCommEditImpl::getHtmlDta()
{
	HtmlGen htmlGen;
	return htmlGen.genHtmlComment(dta);
}

void AdvCommEditImpl::setHtmlDta(const QString &str)
{
	if (str.isEmpty())
		return;
	PWM_ASSERT(HtmlGen::isHtml(str));
	HtmlParse htmlParse;
	if (!htmlParse.parseHtmlComment(str, dta)) {
		printWarn("AdvCommEditImpl::setHtmlDta(): parseHtmlComment() failed!");
		return;
	}
	titleLineEdit->setText(dta->getTitle());
	const vector<HtmlComment::SubTable> *subTbls = dta->getSubTableList();
	vector<HtmlComment::SubTable>::const_iterator i = subTbls->begin(),
						      end = subTbls->end();
	while (i != end) {
		entryListBox->insertItem(i->getTitle());
		++i;
	}
	updatePreview();
}

#include "advcommeditimpl.moc"
