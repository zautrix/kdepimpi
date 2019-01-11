/****************************************************************************
** Form implementation generated from reading ui file 'setmasterpwwnd.ui'
**
** Created: Tue Sep 14 15:43:34 2004
**      by: The User Interface Compiler ($Id: setmasterpwwnd.cpp,v 1.1 2004/09/15 17:55:40 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "setmasterpwwnd.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a setMasterPwWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
setMasterPwWnd::setMasterPwWnd( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "setMasterPwWnd" );

    okButton = new QPushButton( this, "okButton" );
    okButton->setGeometry( QRect( 10, 280, 107, 27 ) );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setGeometry( QRect( 290, 280, 107, 27 ) );

    mainTab = new QTabWidget( this, "mainTab" );
    mainTab->setGeometry( QRect( 10, 10, 390, 260 ) );

    tab = new QWidget( mainTab, "tab" );

    textLabel2 = new QLabel( tab, "textLabel2" );
    textLabel2->setGeometry( QRect( 20, 90, 340, 20 ) );

    textLabel3 = new QLabel( tab, "textLabel3" );
    textLabel3->setGeometry( QRect( 20, 160, 340, 20 ) );

    textLabel1 = new QLabel( tab, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 20, 370, 40 ) );
    textLabel1->setFrameShape( QLabel::Box );
    textLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );

    pwEdit_1 = new QLineEdit( tab, "pwEdit_1" );
    pwEdit_1->setGeometry( QRect( 20, 120, 340, 20 ) );
    pwEdit_1->setEchoMode( QLineEdit::Password );

    pwEdit_2 = new QLineEdit( tab, "pwEdit_2" );
    pwEdit_2->setGeometry( QRect( 20, 190, 340, 20 ) );
    pwEdit_2->setEchoMode( QLineEdit::Password );
    mainTab->insertTab( tab, QString("") );

    tab_2 = new QWidget( mainTab, "tab_2" );

    textLabel1_2 = new QLabel( tab_2, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 10, 20, 370, 40 ) );
    textLabel1_2->setFrameShape( QLabel::Box );
    textLabel1_2->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );

    textLabel2_2 = new QLabel( tab_2, "textLabel2_2" );
    textLabel2_2->setGeometry( QRect( 20, 190, 170, 20 ) );
    textLabel2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    selCardButton = new QPushButton( tab_2, "selCardButton" );
    selCardButton->setGeometry( QRect( 20, 130, 350, 40 ) );

    genCardButton = new QPushButton( tab_2, "genCardButton" );
    genCardButton->setGeometry( QRect( 80, 90, 230, 28 ) );

    curCardIdLabel = new QLabel( tab_2, "curCardIdLabel" );
    curCardIdLabel->setGeometry( QRect( 200, 190, 170, 20 ) );
    mainTab->insertTab( tab_2, QString("") );
    languageChange();
    resize( QSize(411, 313).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( okButton_slot() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButton_slot() ) );
    connect( genCardButton, SIGNAL( clicked() ), this, SLOT( genCardButton_slot() ) );
    connect( selCardButton, SIGNAL( clicked() ), this, SLOT( selCardButton_slot() ) );

    // tab order
    setTabOrder( pwEdit_1, pwEdit_2 );
    setTabOrder( pwEdit_2, okButton );
    setTabOrder( okButton, cancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
setMasterPwWnd::~setMasterPwWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void setMasterPwWnd::languageChange()
{
    setCaption( tr( "Set master-password" ) );
    okButton->setText( tr( "&OK" ) );
    cancelButton->setText( tr( "&Cancel" ) );
    textLabel2->setText( tr( "Please enter the new master-password:" ) );
    textLabel3->setText( tr( "Please enter the password again:" ) );
    textLabel1->setText( tr( "Using a normal password-string to encrypt the data." ) );
    mainTab->changeTab( tab, tr( "Normal password" ) );
    textLabel1_2->setText( tr( "Using a PwM key-card to encrypt the data." ) );
    textLabel2_2->setText( tr( "selected card:" ) );
    selCardButton->setText( tr( "&Select the currently inserted card as key-card" ) );
    genCardButton->setText( tr( "&generate new key-card" ) );
    curCardIdLabel->setText( QString::null );
    mainTab->changeTab( tab_2, tr( "Key-card (chipcard)" ) );
}

void setMasterPwWnd::okButton_slot()
{
    qWarning( "setMasterPwWnd::okButton_slot(): Not implemented yet" );
}

void setMasterPwWnd::cancelButton_slot()
{
    qWarning( "setMasterPwWnd::cancelButton_slot(): Not implemented yet" );
}

void setMasterPwWnd::genCardButton_slot()
{
    qWarning( "setMasterPwWnd::genCardButton_slot(): Not implemented yet" );
}

void setMasterPwWnd::selCardButton_slot()
{
    qWarning( "setMasterPwWnd::selCardButton_slot(): Not implemented yet" );
}

