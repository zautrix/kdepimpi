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

#ifndef PRINTTEXT_H
#define PRINTTEXT_H

#include <kprinter.h>

#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qrect.h>
#include <qpoint.h>

/** prints text "raw" to a printer */
class PrintText : public KPrinter
{
public:
	PrintText();
	~PrintText();

	/** begin printing. Call this before drawing to the device! */
	void beginPrint();
	/** prints a line */
	void printLine(QString t);
	/** returns a pointer to the paint object */
	QPainter* getPaint()
			{ return paint; }
	/** sets the header-text */
	void setHeader(const QString &left, const QString &middle);
	/** returns the header-text */
	void getHeader(QString *left, QString *middle);

protected:
	/** painter object */
	QPainter *paint;
	/** metrics */
	QPaintDeviceMetrics *metrics;
	/** left field of header */
	QString headerLeft;
	/** middle field of header */
	QString headerMiddle;
	/** right field of header */
	QString headerRight;
	/** current Y-Position */
	int curYPos;
	/** text-body rectangle */
	QRect body;
	/** line-spacing */
	int ls;
	/** top-left point for beginning printing */
	QPoint topLeft;
	/** current page number */
	int page;

protected:
	/** prints the header on the page */
	void printHeader();
};

#endif
