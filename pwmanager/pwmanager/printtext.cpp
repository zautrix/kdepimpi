/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include "printtext.h"
#include "pwmexception.h"

#include <klocale.h>

#include <qpen.h>


PrintText::PrintText()
{
	curYPos = 0;
	paint = new QPainter;
	metrics = new QPaintDeviceMetrics(this);
}

PrintText::~PrintText()
{
	delete paint;
	delete metrics;
}

void PrintText::beginPrint()
{
	PWM_ASSERT(paint && metrics);
	curYPos = 0;
	page = 1;
	paint->begin(this);
	ls = paint->fontMetrics().lineSpacing();

	// set printing border
	int border = (metrics->width() / metrics->widthMM()) * 15; // 15 mm border
	topLeft.setX(border);
	topLeft.setY(border);

	// set body
	body.setRect(topLeft.x(), ls * 2 + topLeft.y(),
		     metrics->width() - topLeft.x() * 2,
		     metrics->height() - ls * 2 - topLeft.y() * 2);
	paint->setTabStops(paint->fontMetrics().width("M") * 8);

	printHeader();
}

void PrintText::printLine(QString t)
{
	if (curYPos == 0)
		curYPos = ls * 2 + topLeft.y(); // skip header
	if (t == "")
		t = " ";
	QRect r = paint->boundingRect(topLeft.x(), curYPos, body.width(), body.height(),
				      QPainter::ExpandTabs | QPainter::WordBreak, t);
	int height = r.height();
	if (height + curYPos > metrics->height()) {
		// next page
		newPage();
		++page;
		headerRight = "";
		printHeader();
		curYPos = ls * 2 + topLeft.y();
	}
	paint->drawText(topLeft.x(), curYPos, metrics->width(), metrics->height() - curYPos,
			QPainter::ExpandTabs | QPainter::WordBreak, t);
	curYPos += ls;
}

void PrintText::printHeader()
{
	if (headerRight == "")
		headerRight = i18n("Page #") + QString::number(page);

	paint->drawText(topLeft.x(), topLeft.y(), metrics->width() - topLeft.x() * 2,
			ls, Qt::AlignLeft, headerLeft);
	paint->drawText(topLeft.x(), topLeft.y(), metrics->width() - topLeft.x() * 2,
			ls, Qt::AlignHCenter, headerMiddle);
	paint->drawText(topLeft.x(), topLeft.y(), metrics->width() - topLeft.x() * 2,
			ls, Qt::AlignRight, headerRight);

	QPen pen;
	pen.setWidth(3);
	paint->setPen(pen);
	paint->drawLine(topLeft.x(), (ls + ls / 2) + topLeft.y(),
			metrics->width() - topLeft.x(),
			(ls + ls / 2) + topLeft.y());
}

void PrintText::setHeader(const QString &left, const QString &middle)
{
	headerLeft = left;
	headerMiddle = middle;
}

void PrintText::getHeader(QString *left, QString *middle)
{
	*left = headerLeft;
	*middle = headerMiddle;
}
