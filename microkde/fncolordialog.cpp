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

#include <qwidget.h>
#include <qimage.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qlabel.h>
#include "fncolordialog.h"


//-----------------------------------------------------------------------------
// FNPaletteBase
//-----------------------------------------------------------------------------
/* 
 *  Constructs a Example which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FNPaletteBase::FNPaletteBase(QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), bgcolor_(white), _isblock(false)
{
	this->setBackgroundMode(NoBackground);
	wbuf_.resize(32, 32);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FNPaletteBase::~FNPaletteBase()
{
    // no need to delete child widgets, Qt does it all for us
}


/*
 *  Painting event.
 */
void FNPaletteBase::paintEvent(QPaintEvent*)
{
	bitBlt(this, 0, 0, &wbuf_);
}


/*
 *  Painting event.
 */
void FNPaletteBase::resizeEvent(QResizeEvent* evt)
{
	QPixmap save(wbuf_);
	wbuf_.resize(evt->size());
	wbuf_.fill(bgcolor_);
	bitBlt(&wbuf_, 0, 0, &save);
	redraw();
}


/*
 *  Redraw method.
 */
void FNPaletteBase::redraw(bool force)
{
	if (!force) {
		if (_isblock) {
			return;
		}
	}
	_isblock = true;
	wbuf_.fill(bgcolor_);
	QPainter pa;
	pa.begin(&wbuf_);
	drawImpl(pa);
	pa.end();
	repaint();
	_isblock = false;
}


/*
 *  Redraw Implement
 */
void FNPaletteBase::drawImpl(QPainter& pa)
{
	pa.flush();
}

bool FNPaletteBase::pickColor(int x, int y)
{
	if (0 <= x && 0 <= y && x < width() && y < height()) {
		selection_ = QColor(wbuf_.convertToImage().pixel(x, y));
		return true;
	} else {
		return false;
	}
}

void FNPaletteBase::mouseReleaseEvent(QMouseEvent* evt)
{
	//マウスリリースイベント(MouseClicked)
	if (pickColor(evt->x(), evt->y())) {
		emit clicked(this);
	}
}




//-----------------------------------------------------------------------------
// FNColorPalette
//-----------------------------------------------------------------------------
FNColorPalette::FNColorPalette(QColor c, QWidget* parent, const char* name, WFlags fl)
:FNPaletteBase(parent, name, fl)
{
	setBGColor(c);
}

FNColorPalette::~FNColorPalette()
{
}




//-----------------------------------------------------------------------------
// FNHSVPalette
//-----------------------------------------------------------------------------
FNHSVPalette::FNHSVPalette(QWidget* parent, const char* name, WFlags fl)
:FNPaletteBase(parent, name, fl), _hue(0)
{
}

FNHSVPalette::~FNHSVPalette()
{
}

void FNHSVPalette::hueChanged(int v)
{
	_hue = v;
	redraw();
}

void FNHSVPalette::drawImpl(QPainter& pa)
{
	//描画実装
	//HSVカラーパレットを描画
	int wd = width();
	int ht = height();
	if (wd > 0 && ht > 0) {
		QColor c;
		double xs = (double)wd / 255.0;
		double ys = (double)ht / 255.0;
		for (int i = 0; i < 256; i += 32) {
			int x1 = (int)((double)i * xs);
			int x2 = (int)((double)(i + 32) * xs);
			int w = x2 - x1;
			if (1 > w) {
				w = 1;
			}
			for (int j = 0; j <= 256; j += 32) {
				int y1 = (int)((double)j * ys);
				int y2 = (int)((double)(j + 32) * ys);
				int h = y2 - y1;
				if (1 > h) {
					h = 1;
				}
				c.setHsv(_hue, 255 - j, 255 - i);
				pa.fillRect(x1, y1, w, h, QBrush(c));
			}
		}
	}
	pa.flush();
}




//-----------------------------------------------------------------------------
// FNColorDialog
//-----------------------------------------------------------------------------
FNColorDialog::FNColorDialog(QWidget* parent, const char* name, WFlags f)
:QDialog(parent, name, true, f), _isblock(false)
{
	resize(200, 200);
	QVBoxLayout* mainlayout = new QVBoxLayout(this);
	setCaption("Color Selector");
 	mainlayout->setMargin(2);
 	mainlayout->setSpacing(2);
	setSizeGripEnabled(true);
	//基本パレット
	int p = 0;
	_base[p++] = new FNColorPalette(QColor(0, 0, 0), this);
	_base[p++] = new FNColorPalette(QColor(0, 0, 255), this);
	_base[p++] = new FNColorPalette(QColor(255, 0, 0), this);
	_base[p++] = new FNColorPalette(QColor(255, 0, 255), this);
	_base[p++] = new FNColorPalette(QColor(0, 255, 0), this);
	_base[p++] = new FNColorPalette(QColor(0, 255, 255), this);
	_base[p++] = new FNColorPalette(QColor(255, 255, 0), this);
	_base[p++] = new FNColorPalette(QColor(255, 255, 255), this);
	_base[p++] = new FNColorPalette(QColor(128, 128, 128), this);
	_base[p++] = new FNColorPalette(QColor(0, 0, 128), this);
	_base[p++] = new FNColorPalette(QColor(128, 0, 0), this);
	_base[p++] = new FNColorPalette(QColor(128, 0, 128), this);
	_base[p++] = new FNColorPalette(QColor(0, 128, 0), this);
	_base[p++] = new FNColorPalette(QColor(0, 128, 128), this);
	_base[p++] = new FNColorPalette(QColor(128, 128, 0), this);
	_base[p++] = new FNColorPalette(QColor(200, 200, 200), this);

	QGridLayout* baselayout = new QGridLayout(this, 2, 16);
 	baselayout->setMargin(0);
 	baselayout->setSpacing(0);
	p = 0;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 8; ++j) {
			baselayout->addWidget(_base[p], i, j);
			connect(_base[p], SIGNAL(clicked(FNPaletteBase*)), this, SLOT(basePaletteClicked(FNPaletteBase*)));
			_base[p]->resize(24, 24);
			_base[p]->setMaximumSize(24, 24);
			_base[p]->setMinimumSize(24, 24);
			p++;
		}
	}
	mainlayout->addLayout(baselayout);

	//HUE
	_hue = new QSlider(Horizontal, this);
	_hue->setMinValue(0);
	_hue->setMaxValue(360);
	_hue->setLineStep(1);
	_hue->setPageStep(60);
	_hue->setValue(0);
	//sliderReleased
	mainlayout->addWidget(_hue);

	//HSVパレット
	QHBoxLayout* hsvlayout = new QHBoxLayout(this);
 	hsvlayout->setMargin(0);
 	hsvlayout->setSpacing(2);
	_palette = new FNHSVPalette(this);
	_palette->setMinimumSize(90, 100);
	hsvlayout->addWidget(_palette);
	connect(_palette, SIGNAL(clicked(FNPaletteBase*)), this, SLOT(hsvPaletteClicked(FNPaletteBase*)));

	//選択色
	QGridLayout* selectLayout = new QGridLayout(this, 4, 4);
	_select = new FNColorPalette(black, this);
	_select->resize(48, 32);
	_select->setMinimumSize(48, 32);
	_select->setMaximumSize(32767, 32);
	selectLayout->addMultiCellWidget(_select, 0, 0, 1, 3);

	//RGBコントローラ
	selectLayout->addWidget(new QLabel("R:", this), 1, 0);
	_r = new QSpinBox(0, 255, 1, this);
	_r->setValue(0);
	selectLayout->addMultiCellWidget(_r, 1, 1, 1, 3);

	selectLayout->addWidget(new QLabel("G:", this), 2, 0);
	_g = new QSpinBox(0, 255, 1, this);
	_g->setValue(0);
	selectLayout->addMultiCellWidget(_g, 2, 2, 1, 3);

	selectLayout->addWidget(new QLabel("B:", this), 3, 0);
	_b = new QSpinBox(0, 255, 1, this);
	_b->setValue(0);
	selectLayout->addMultiCellWidget(_b, 3, 3, 1, 3);

	hsvlayout->addLayout(selectLayout);
	mainlayout->addLayout(hsvlayout);

	connect(_hue, SIGNAL(valueChanged(int)), _palette, SLOT(hueChanged(int)));
	connect(_r, SIGNAL(valueChanged(int)), this, SLOT(rgbChanged(int)));
	connect(_g, SIGNAL(valueChanged(int)), this, SLOT(rgbChanged(int)));
	connect(_b, SIGNAL(valueChanged(int)), this, SLOT(rgbChanged(int)));
}

FNColorDialog::~FNColorDialog()
{
}

void FNColorDialog::rgbChanged(int)
{
	int r = _r->value();
	int g = _g->value();
	int b = _b->value();
	setColor(QColor(r, g, b));
}

QColor FNColorDialog::color() const
{
	return _select->color();
}
void FNColorDialog::setColor(QColor c)
{
	if (_isblock) {
		return;
	}
	_isblock = true;

	int r;
	int g;
	int b;
	
	c.rgb(&r, &g, &b);
	_r->setValue(r);
	_g->setValue(g);
	_b->setValue(b);
	_select->setBGColor(c);
	_isblock = false;
}

void FNColorDialog::basePaletteClicked(FNPaletteBase* btn)
{
	setColor(btn->color()); 
	accept();
}

void FNColorDialog::hsvPaletteClicked(FNPaletteBase* btn)
{
	setColor(btn->color());
}
