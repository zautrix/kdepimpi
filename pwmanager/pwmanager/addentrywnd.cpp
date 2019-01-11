/****************************************************************************
** Form implementation generated from reading ui file 'addentrywnd.ui'
**
** Created: Tue Sep 14 13:42:03 2004
**      by: The User Interface Compiler ($Id: addentrywnd.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "addentrywnd.h"

#include <qvariant.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a addEntryWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addEntryWnd::addEntryWnd( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "addEntryWnd" );

    launcherLineEdit = new QLineEdit( this, "launcherLineEdit" );
    launcherLineEdit->setGeometry( QRect( 310, 90, 250, 21 ) );

    cancelButton = new QPushButton( this, "cancelButton" );
    cancelButton->setGeometry( QRect( 290, 320, 260, 27 ) );

    generateButton = new QPushButton( this, "generateButton" );
    generateButton->setGeometry( QRect( 137, 210, 120, 27 ) );

    descLineEdit = new QLineEdit( this, "descLineEdit" );
    descLineEdit->setGeometry( QRect( 10, 30, 250, 20 ) );

    categoryComboBox = new QComboBox( FALSE, this, "categoryComboBox" );
    categoryComboBox->setGeometry( QRect( 10, 80, 250, 27 ) );
    categoryComboBox->setEditable( TRUE );
    categoryComboBox->setSizeLimit( 100 );
    categoryComboBox->setAutoCompletion( TRUE );
    categoryComboBox->setDuplicatesEnabled( FALSE );

    usernameLineEdit = new QLineEdit( this, "usernameLineEdit" );
    usernameLineEdit->setGeometry( QRect( 10, 140, 250, 20 ) );

    pwLineEdit = new QLineEdit( this, "pwLineEdit" );
    pwLineEdit->setGeometry( QRect( 10, 190, 250, 20 ) );
    pwLineEdit->setEchoMode( QLineEdit::Password );

    urlLineEdit = new QLineEdit( this, "urlLineEdit" );
    urlLineEdit->setGeometry( QRect( 10, 269, 250, 20 ) );

    textLabel1_3 = new QLabel( this, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 276, 10, 20, 280 ) );
    textLabel1_3->setFrameShape( QLabel::VLine );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 10, 250, 20 ) );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 10, 60, 250, 20 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 10, 120, 250, 20 ) );
    textLabel2->setFrameShape( QLabel::NoFrame );
    textLabel2->setFrameShadow( QLabel::Plain );

    textLabel3 = new QLabel( this, "textLabel3" );
    textLabel3->setGeometry( QRect( 10, 170, 250, 20 ) );

    textLabel2_2 = new QLabel( this, "textLabel2_2" );
    textLabel2_2->setGeometry( QRect( 10, 248, 250, 20 ) );

    textLabel3_2 = new QLabel( this, "textLabel3_2" );
    textLabel3_2->setGeometry( QRect( 309, 8, 250, 20 ) );

    textLabel1_4 = new QLabel( this, "textLabel1_4" );
    textLabel1_4->setGeometry( QRect( 310, 30, 250, 16 ) );

    textLabel3_3 = new QLabel( this, "textLabel3_3" );
    textLabel3_3->setGeometry( QRect( 310, 50, 130, 16 ) );

    textLabel2_3 = new QLabel( this, "textLabel2_3" );
    textLabel2_3->setGeometry( QRect( 310, 70, 130, 16 ) );

    textLabel5 = new QLabel( this, "textLabel5" );
    textLabel5->setGeometry( QRect( 440, 70, 120, 16 ) );

    textLabel4_2 = new QLabel( this, "textLabel4_2" );
    textLabel4_2->setGeometry( QRect( 440, 50, 120, 16 ) );

    revealButton = new QPushButton( this, "revealButton" );
    revealButton->setGeometry( QRect( 10, 210, 120, 27 ) );
    revealButton->setToggleButton( TRUE );

    okButton = new QPushButton( this, "okButton" );
    okButton->setGeometry( QRect( 20, 320, 260, 27 ) );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setGeometry( QRect( 300, 140, 260, 150 ) );

    commentDummy = new QLabel( groupBox1, "commentDummy" );
    commentDummy->setGeometry( QRect( 10, 20, 240, 80 ) );

    advancedCommentButton = new QPushButton( groupBox1, "advancedCommentButton" );
    advancedCommentButton->setGeometry( QRect( 10, 110, 240, 30 ) );
    advancedCommentButton->setToggleButton( TRUE );
    languageChange();
    resize( QSize(570, 357).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( generateButton, SIGNAL( clicked() ), this, SLOT( generateButton_slot() ) );
    connect( revealButton, SIGNAL( toggled(bool) ), this, SLOT( revealButton_slot() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButton_slot() ) );
    connect( okButton, SIGNAL( clicked() ), this, SLOT( okButton_slot() ) );
    connect( advancedCommentButton, SIGNAL( toggled(bool) ), this, SLOT( advancedCommentButton_slot(bool) ) );

    // tab order
    setTabOrder( descLineEdit, categoryComboBox );
    setTabOrder( categoryComboBox, usernameLineEdit );
    setTabOrder( usernameLineEdit, pwLineEdit );
    setTabOrder( pwLineEdit, revealButton );
    setTabOrder( revealButton, generateButton );
    setTabOrder( generateButton, urlLineEdit );
    setTabOrder( urlLineEdit, launcherLineEdit );
    setTabOrder( launcherLineEdit, okButton );
    setTabOrder( okButton, cancelButton );

}

/*
 *  Destroys the object and frees any allocated resources
 */
addEntryWnd::~addEntryWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void addEntryWnd::languageChange()
{
    setCaption( tr( "edit/add a password-entry" ) );
    cancelButton->setText( tr( "&Cancel" ) );
    generateButton->setText( tr( "&Generate" ) );
    textLabel1_3->setText( QString::null );
    textLabel1->setText( tr( "Description:" ) );
    textLabel1_2->setText( tr( "Category:" ) );
    textLabel2->setText( tr( "Username:" ) );
    textLabel3->setText( tr( "Password:" ) );
    textLabel2_2->setText( tr( "URL:" ) );
    textLabel3_2->setText( tr( "Launcher:" ) );
    textLabel1_4->setText( tr( "$d = Description" ) );
    textLabel3_3->setText( tr( "$n = Username" ) );
    textLabel2_3->setText( tr( "$c = Comment" ) );
    textLabel5->setText( tr( "$u = URL" ) );
    textLabel4_2->setText( tr( "$p = Password" ) );
    revealButton->setText( tr( "&Reveal" ) );
    okButton->setText( tr( "&OK" ) );
    groupBox1->setTitle( tr( "Comment:" ) );
    commentDummy->setText( QString::null );
    advancedCommentButton->setText( tr( "advanced comment" ) );
}

void addEntryWnd::okButton_slot()
{
    qWarning( "addEntryWnd::okButton_slot(): Not implemented yet" );
}

void addEntryWnd::cancelButton_slot()
{
    qWarning( "addEntryWnd::cancelButton_slot(): Not implemented yet" );
}

void addEntryWnd::revealButton_slot()
{
    qWarning( "addEntryWnd::revealButton_slot(): Not implemented yet" );
}

void addEntryWnd::generateButton_slot()
{
    qWarning( "addEntryWnd::generateButton_slot(): Not implemented yet" );
}

void addEntryWnd::advancedCommentButton_slot(bool)
{
    qWarning( "addEntryWnd::advancedCommentButton_slot(bool): Not implemented yet" );
}

