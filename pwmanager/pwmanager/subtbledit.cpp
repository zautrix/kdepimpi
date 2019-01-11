/****************************************************************************
** Form implementation generated from reading ui file 'subtbledit.ui'
**
** Created: Tue Sep 14 15:44:17 2004
**      by: The User Interface Compiler ($Id: subtbledit.cpp,v 1.1 2004/09/15 17:55:40 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "subtbledit.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a subTblEdit as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
subTblEdit::subTblEdit( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "subTblEdit" );

    textLabel4 = new QLabel( this, "textLabel4" );
    textLabel4->setGeometry( QRect( 20, 10, 310, 20 ) );
    textLabel4->setAlignment( int( QLabel::AlignCenter ) );

    textLabel5 = new QLabel( this, "textLabel5" );
    textLabel5->setGeometry( QRect( 20, 70, 310, 20 ) );
    textLabel5->setAlignment( int( QLabel::AlignCenter ) );

    titleLineEdit = new QLineEdit( this, "titleLineEdit" );
    titleLineEdit->setGeometry( QRect( 20, 30, 310, 26 ) );

    okButton = new QPushButton( this, "okButton" );
    okButton->setGeometry( QRect( 20, 420, 140, 31 ) );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setGeometry( QRect( 190, 420, 140, 31 ) );

    groupBox3 = new QGroupBox( this, "groupBox3" );
    groupBox3->setGeometry( QRect( 20, 210, 310, 200 ) );

    textLabel6 = new QLabel( groupBox3, "textLabel6" );
    textLabel6->setGeometry( QRect( 20, 30, 270, 20 ) );

    textLabel7 = new QLabel( groupBox3, "textLabel7" );
    textLabel7->setGeometry( QRect( 20, 90, 270, 20 ) );

    addButton = new QPushButton( groupBox3, "addButton" );
    addButton->setGeometry( QRect( 30, 150, 250, 31 ) );

    valueLineEdit = new QLineEdit( groupBox3, "valueLineEdit" );
    valueLineEdit->setGeometry( QRect( 20, 110, 270, 26 ) );

    nameLineEdit = new QLineEdit( groupBox3, "nameLineEdit" );
    nameLineEdit->setGeometry( QRect( 20, 50, 270, 26 ) );

    entryListBox = new QListBox( this, "entryListBox" );
    entryListBox->setGeometry( QRect( 20, 90, 310, 80 ) );

    delButton = new QPushButton( this, "delButton" );
    delButton->setGeometry( QRect( 110, 170, 130, 31 ) );
    languageChange();
    resize( QSize(351, 464).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( okButton_slot() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButton_slot() ) );
    connect( addButton, SIGNAL( clicked() ), this, SLOT( addButton_slot() ) );
    connect( delButton, SIGNAL( clicked() ), this, SLOT( delButton_slot() ) );

    // tab order
    setTabOrder( titleLineEdit, entryListBox );
    setTabOrder( entryListBox, delButton );
    setTabOrder( delButton, nameLineEdit );
    setTabOrder( nameLineEdit, valueLineEdit );
    setTabOrder( valueLineEdit, addButton );
    setTabOrder( addButton, okButton );
    setTabOrder( okButton, cancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
subTblEdit::~subTblEdit()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void subTblEdit::languageChange()
{
    setCaption( tr( "HTML-comment subtable editor" ) );
    textLabel4->setText( tr( "Sub-title:" ) );
    textLabel5->setText( tr( "Current subtable-entries:" ) );
    okButton->setText( tr( "&OK" ) );
    cancelButton->setText( tr( "&Cancel" ) );
    groupBox3->setTitle( tr( "New entry" ) );
    textLabel6->setText( tr( "Entry name:" ) );
    textLabel7->setText( tr( "Entry value:" ) );
    addButton->setText( tr( "Add..." ) );
    delButton->setText( tr( "Delete" ) );
}

void subTblEdit::okButton_slot()
{
    qWarning( "subTblEdit::okButton_slot(): Not implemented yet" );
}

void subTblEdit::cancelButton_slot()
{
    qWarning( "subTblEdit::cancelButton_slot(): Not implemented yet" );
}

void subTblEdit::addButton_slot()
{
    qWarning( "subTblEdit::addButton_slot(): Not implemented yet" );
}

void subTblEdit::delButton_slot()
{
    qWarning( "subTblEdit::delButton_slot(): Not implemented yet" );
}

