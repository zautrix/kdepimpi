/*  this program for Sharp SLA300, B500, C7x0, C860 Linux PDA
	Copyright (C) 2003-2005 Joe Kanemori.<kanemori@ymg.urban.ne.jp>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundatibannwaon; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*
2005/02/27 FreeNote 1.11.10pre
・PDFの出力形式を一部変更
・インポート時のバグfix

*/
#ifndef  FNPALETTEDIALOG_H
#define  FNPALETTEDIALOG_H
#include <qwidget.h> 
#include <qpixmap.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qslider.h>
#include <qspinbox.h>

//-----------------------------------------------------------------------------
// FNPaletteBase
//-----------------------------------------------------------------------------
class FNPaletteBase : public QWidget
{ 
    Q_OBJECT
public:
	
    FNPaletteBase(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0);

	
    virtual ~FNPaletteBase();

	
    void setBGColor(QColor c) {
		bgcolor_ = c;
		selection_ = c;
		redraw();
	};
	QColor color() const {
		return selection_;
	};

protected:
	virtual void paintEvent(QPaintEvent*);		
	virtual void resizeEvent(QResizeEvent* evt);
	virtual void redraw(bool force=false);		
	virtual void drawImpl(QPainter& pa);
	virtual void mouseReleaseEvent(QMouseEvent* evt); 
	virtual bool pickColor(int x, int y);
	QColor selection_;
private:
    QPixmap wbuf_;	
    QColor bgcolor_;
	bool _isblock;

signals:
	void clicked(FNPaletteBase* sender);
};




//-----------------------------------------------------------------------------
// FNColorPalette
//-----------------------------------------------------------------------------
class  FNColorPalette : public FNPaletteBase
{ 
    Q_OBJECT
public:
    FNColorPalette(QColor c, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0);
    virtual ~FNColorPalette();
};




//-----------------------------------------------------------------------------
// FNHSVPalette
//-----------------------------------------------------------------------------
class FNHSVPalette : public FNPaletteBase
{
    Q_OBJECT
public:
    FNHSVPalette(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0);
    virtual ~FNHSVPalette();
public slots:
    void hueChanged(int v);
protected:
	virtual void drawImpl(QPainter& pa);
	int _hue;
};




//-----------------------------------------------------------------------------
// FNColorDialog
//-----------------------------------------------------------------------------
class  FNColorDialog : public QDialog
{
	Q_OBJECT
public:
	FNColorDialog(QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
	virtual ~FNColorDialog();
	void setColor(QColor c);
	QColor color() const;
public slots:
	virtual void basePaletteClicked(FNPaletteBase*);
	virtual void hsvPaletteClicked(FNPaletteBase*);
	void rgbChanged(int);
private:
	FNHSVPalette*   _palette;
	FNColorPalette* _base[16];
	QSlider* _hue;
	FNColorPalette* _select;
	QSpinBox* _r;
	QSpinBox* _g;
	QSpinBox* _b;
    bool _isblock;

};
#endif //FNPALETTEDIALOG_H
