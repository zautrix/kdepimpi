/****************************************************************************
** Form implementation generated from reading ui file 'getmasterpwwnd.ui'
**
** Created: Tue Sep 14 15:33:57 2004
**      by: The User Interface Compiler ($Id: getmasterpwwnd.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "getmasterpwwnd.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a getMasterPwWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
getMasterPwWnd::getMasterPwWnd( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "getMasterPwWnd" );

    okButton = new QPushButton( this, "okButton" );
    okButton->setGeometry( QRect( 10, 80, 107, 27 ) );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setGeometry( QRect( 240, 80, 107, 27 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 20, 340, 20 ) );
    textLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );

    pwLineEdit = new QLineEdit( this, "pwLineEdit" );
    pwLineEdit->setGeometry( QRect( 10, 50, 340, 20 ) );
    pwLineEdit->setEchoMode( QLineEdit::Password );
    languageChange();
    resize( QSize(361, 119).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( okButton_slot() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButton_slot() ) );

    // tab order
    setTabOrder( pwLineEdit, okButton );
    setTabOrder( okButton, cancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
getMasterPwWnd::~getMasterPwWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void getMasterPwWnd::languageChange()
{
    setCaption( tr( "Master-password" ) );
    okButton->setText( tr( "&OK" ) );
    cancelButton->setText( tr( "&Cancel" ) );
    textLabel1->setText( tr( "Please enter the master-password:" ) );
}

void getMasterPwWnd::okButton_slot()
{
    qWarning( "getMasterPwWnd::okButton_slot(): Not implemented yet" );
}

void getMasterPwWnd::cancelButton_slot()
{
    qWarning( "getMasterPwWnd::cancelButton_slot(): Not implemented yet" );
}

