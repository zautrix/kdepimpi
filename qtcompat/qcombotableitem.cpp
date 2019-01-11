/****************************************************************************
**
** Implementation of QTable widget class
**
** Created : 000607
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the table module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
/*US
#include "qglobal.h"
#if defined(Q_CC_BOR)
// needed for qsort() because of a std namespace problem on Borland
#include "qplatformdefs.h"
#endif
*/
#include "qcombotableitem.h"

#include "qcombobox.h"
#include "qstyle.h"

/*US

#ifndef QT_NO_TABLE

#include "qpainter.h"
#include "qlineedit.h"
#include "qcursor.h"
#include "qapplication.h"
#include "qtimer.h"
#include "qobjectlist.h"
#include "qiconset.h"
#include "qcombobox.h"
#include "qcheckbox.h"
#include "qdragobject.h"
#include "qevent.h"
#include "qlistbox.h"
#include "qstyle.h"

#include <stdlib.h>
#include <limits.h>

static bool qt_update_cell_widget = TRUE;

class QM_EXPORT_TABLE QTableHeader : public QHeader
{
    friend class QTable;
    Q_OBJECT

public:
    enum SectionState {
	Normal,
	Bold,
	Selected
    };

    QTableHeader( int, QTable *t, QWidget* parent=0, const char* name=0 );
    ~QTableHeader() {};
    void addLabel( const QString &s, int size );
    void setLabel( int section, const QString & s, int size = -1 );
    void setLabel( int section, const QIconSet & iconset, const QString & s,
		   int size = -1 );
    void removeLabel( int section );

    void setSectionState( int s, SectionState state );
    void setSectionStateToAll( SectionState state );
    SectionState sectionState( int s ) const;

    int sectionSize( int section ) const;
    int sectionPos( int section ) const;
    int sectionAt( int section ) const;

    void setSectionStretchable( int s, bool b );
    bool isSectionStretchable( int s ) const;

    void updateCache();

signals:
    void sectionSizeChanged( int s );

protected:
    void paintEvent( QPaintEvent *e );
    void paintSection( QPainter *p, int index, const QRect& fr );
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseDoubleClickEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );

private slots:
    void doAutoScroll();
    void sectionWidthChanged( int col, int os, int ns );
    void indexChanged( int sec, int oldIdx, int newIdx );
    void updateStretches();
    void updateWidgetStretches();

private:
    void updateSelections();
    void saveStates();
    void setCaching( bool b );
    void swapSections( int oldIdx, int newIdx, bool swapTable = TRUE );
    bool doSelection( QMouseEvent *e );
    void sectionLabelChanged( int section );
    void resizeArrays( int n );

private:
    QMemArray<int> states, oldStates;
    QMemArray<bool> stretchable;
    QMemArray<int> sectionSizes, sectionPoses;
    bool mousePressed;
    int pressPos, startPos, endPos;
    QTable *table;
    QTimer *autoScrollTimer;
    QWidget *line1, *line2;
    bool caching;
    int resizedSection;
    bool isResizing;
    int numStretches;
    QTimer *stretchTimer, *widgetStretchTimer;
    QTableHeaderPrivate *d;

};

#ifdef _WS_QWS_
# define NO_LINE_WIDGET
#endif



struct QTablePrivate
{
    QTablePrivate() : hasRowSpan( FALSE ), hasColSpan( FALSE ),
	redirectMouseEvent( FALSE )
    {
	hiddenRows.setAutoDelete( TRUE );
	hiddenCols.setAutoDelete( TRUE );
    }
    uint hasRowSpan : 1;
    uint hasColSpan : 1;
    uint redirectMouseEvent : 1;
    QIntDict<int> hiddenRows, hiddenCols;
    QTimer *geomTimer;
};

struct QTableHeaderPrivate
{
#ifdef NO_LINE_WIDGET
    int oldLinePos;
#endif
};

static bool isRowSelection( QTable::SelectionMode selMode )
{
    return selMode == QTable::SingleRow || selMode == QTable::MultiRow;
}
*/


/*!
    \class QComboTableItem
    \brief The QComboTableItem class provides a means of using
    comboboxes in QTables.

    \ingroup advanced
    \module table

    A QComboTableItem is a table item which looks and behaves like a
    combobox. The advantage of using QComboTableItems rather than real
    comboboxes is that a QComboTableItem uses far less resources than
    real comboboxes in \l{QTable}s. When the cell has the focus it
    displays a real combobox which the user can interact with. When
    the cell does not have the focus the cell \e looks like a
    combobox. Only text items (i.e. no pixmaps) may be used in
    QComboTableItems.

    QComboTableItem items have the edit type \c WhenCurrent (see
    \l{EditType}). The QComboTableItem's list of items is provided by
    a QStringList passed to the constructor.

    The list of items may be changed using setStringList(). The
    current item can be set with setCurrentItem() and retrieved with
    currentItem(). The text of the current item can be obtained with
    currentText(), and the text of a particular item can be retrieved
    with text().

    If isEditable() is TRUE the QComboTableItem will permit the user
    to either choose an existing list item, or create a new list item
    by entering their own text; otherwise the user may only choose one
    of the existing list items.

    To populate a table cell with a QComboTableItem use
    QTable::setItem().

    QComboTableItems may be deleted with QTable::clearCell().

    QComboTableItems can be distinguished from \l{QTableItem}s and
    \l{QCheckTableItem}s using their Run Time Type Identification
    number (see rtti()).

    \img qtableitems.png Table Items

    \sa QCheckTableItem QTableItem QComboBox
*/

QComboBox *QComboTableItem::fakeCombo = 0;
QWidget *fakeComboWidget = 0;

/*!
    Creates a combo table item for the table \a table. The combobox's
    list of items is passed in the \a list argument. If \a editable is
    TRUE the user may type in new list items; if \a editable is FALSE
    the user may only select from the list of items provided.

    By default QComboTableItems cannot be replaced by other table
    items since isReplaceable() returns FALSE by default.

    \sa QTable::clearCell() EditType
*/

QComboTableItem::QComboTableItem( QTable *table, const QStringList &list, bool editable )
    : QTableItem( table, WhenCurrent, "" ), entries( list ), current( 0 ), edit( editable )
{
    setReplaceable( FALSE );
    if ( !fakeCombo ) {
	fakeComboWidget = new QWidget( 0, 0 );
	fakeCombo = new QComboBox( FALSE, fakeComboWidget, 0 );
	fakeCombo->hide();
    }
}

/*!
    Sets the list items of this QComboTableItem to the strings in the
    string list \a l.
*/

void QComboTableItem::setStringList( const QStringList &l )
{
    entries = l;
    current = 0;
    if ( table()->cellWidget( row(), col() ) ) {
	cb->clear();
	cb->insertStringList( entries );
    }
    table()->updateCell( row(), col() );
}

/*! \reimp */

QWidget *QComboTableItem::createEditor() const
{
    // create an editor - a combobox in our case
    ( (QComboTableItem*)this )->cb = new QComboBox( edit, table()->viewport(), "qt_editor_cb" );
    cb->insertStringList( entries );
    cb->setCurrentItem( current );
    QObject::connect( cb, SIGNAL( activated( int ) ), table(), SLOT( doValueChanged() ) );
    return cb;
}

/*! \reimp */

void QComboTableItem::setContentFromEditor( QWidget *w )
{
    if ( w->inherits( "QComboBox" ) ) {
	QComboBox *cb = (QComboBox*)w;
	entries.clear();
	for ( int i = 0; i < cb->count(); ++i )
	    entries << cb->text( i );
	current = cb->currentItem();
	setText( *entries.at( current ) );
    }
}

/*! \reimp */

void QComboTableItem::paint( QPainter *p, const QColorGroup &cg,
			   const QRect &cr, bool selected )
{
    fakeCombo->resize( cr.width(), cr.height() );

    QColorGroup c( cg );
    if ( selected ) {
	c.setBrush( QColorGroup::Base, cg.brush( QColorGroup::Highlight ) );
	c.setColor( QColorGroup::Text, cg.highlightedText() );
    }

/*US    QStyle::SFlags flags = QStyle::Style_Default;
    if(isEnabled() && table()->isEnabled())
	flags |= QStyle::Style_Enabled;
*/    
//US    table()->style().drawComplexControl( QStyle::CC_ComboBox, p, fakeCombo, fakeCombo->rect(), c, flags );

    table()->style().drawComboButton(p, fakeCombo->x(), fakeCombo->y(), fakeCombo->width(), fakeCombo->height(), c, FALSE, FALSE, (/*US isEnabled() && */table()->isEnabled()), 0);
    p->save();
/*US    
    QRect textR = table()->style().querySubControlMetrics(QStyle::CC_ComboBox, fakeCombo,
							 QStyle::SC_ComboBoxEditField);
               
*/               
    int align = alignment(); // alignment() changes entries
//US    p->drawText( textR, wordWrap() ? ( align | WordBreak ) : align, *entries.at( current ) );
    QRect textR = QRect( fakeCombo->x()+2, fakeCombo->y(), fakeCombo->width()- 17, fakeCombo->height()  );
    //qDebug("x %d   --  %d  wid %d  -- %d ", cr.x(),fakeCombo->x(), cr.width(), fakeCombo->width() );
    p->drawText( textR, wordWrap() ? ( align | WordBreak ) : align, *entries.at( current ) );
    p->restore();
}

/*!
    Sets the list item \a i to be the combo table item's current list
    item.

    \sa currentItem()
*/

void QComboTableItem::setCurrentItem( int i )
{
    QWidget *w = table()->cellWidget( row(), col() );
    if ( w && w->inherits( "QComboBox" ) ) {
	( (QComboBox*)w )->setCurrentItem( i );
	current = i;
	setText( ( (QComboBox*)w )->currentText() );
    } else {
	current = i;
	setText( *entries.at( i ) );
	table()->updateCell( row(), col() );
    }
}

/*!
    \overload

    Sets the list item whose text is \a s to be the combo table item's
    current list item. Does nothing if no list item has the text \a s.

    \sa currentItem()
*/

void QComboTableItem::setCurrentItem( const QString &s )
{
    int i = entries.findIndex( s );
    if ( i != -1 )
	setCurrentItem( i );
}

/*!
    Returns the index of the combo table item's current list item.

    \sa setCurrentItem()
*/

int QComboTableItem::currentItem() const
{
    QWidget *w = table()->cellWidget( row(), col() );
    if ( w && w->inherits( "QComboBox" ) )
	return ( (QComboBox*)w )->currentItem();
    return current;
}

/*!
    Returns the text of the combo table item's current list item.

    \sa currentItem() text()
*/

QString QComboTableItem::currentText() const
{
    QWidget *w = table()->cellWidget( row(), col() );
    if ( w && w->inherits( "QComboBox" ) )
	return ( (QComboBox*)w )->currentText();
    return *entries.at( current );
}

/*!
    Returns the total number of list items in the combo table item.
*/

int QComboTableItem::count() const
{
    QWidget *w = table()->cellWidget( row(), col() );
    if ( w && w->inherits( "QComboBox" ) )
	return ( (QComboBox*)w )->count();
    return (int)entries.count();    //### size_t/int cast
}

/*!
    Returns the text of the combo's list item at index \a i.

    \sa currentText()
*/

QString QComboTableItem::text( int i ) const
{
    QWidget *w = table()->cellWidget( row(), col() );
    if ( w && w->inherits( "QComboBox" ) )
	return ( (QComboBox*)w )->text( i );
    return *entries.at( i );
}

/*!
    If \a b is TRUE the combo table item can be edited, i.e. the user
    may enter a new text item themselves. If \a b is FALSE the user may
    may only choose one of the existing items.

    \sa isEditable()
*/

void QComboTableItem::setEditable( bool b )
{
    edit = b;
}

/*!
    Returns TRUE if the user can add their own list items to the
    combobox's list of items; otherwise returns FALSE.

    \sa setEditable()
*/

bool QComboTableItem::isEditable() const
{
    return edit;
}

int QComboTableItem::RTTI = 1;

/*!
    \fn int QComboTableItem::rtti() const

    Returns 1.

    Make your derived classes return their own values for rtti()to
    distinguish between different table item subclasses. You should
    use values greater than 1000, preferably a large random number, to
    allow for extensions to this class.


    \sa QTableItem::rtti()
*/

int QComboTableItem::rtti() const
{
    return RTTI;
}

/*! \reimp */

QSize QComboTableItem::sizeHint() const
{
    fakeCombo->insertItem( currentText() );
    fakeCombo->setCurrentItem( fakeCombo->count() - 1 );
    QSize sh = fakeCombo->sizeHint();
    fakeCombo->removeItem( fakeCombo->count() - 1 );
//US    return sh.expandedTo( QApplication::globalStrut() );
    return sh;
}

