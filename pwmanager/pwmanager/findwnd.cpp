/****************************************************************************
** Form implementation generated from reading ui file 'findwnd.ui'
**
** Created: Tue Sep 14 15:25:08 2004
**      by: The User Interface Compiler ($Id: findwnd.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "findwnd.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a findWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
findWnd::findWnd( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "findWnd" );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 20, 20, 340, 20 ) );
    textLabel1->setFrameShape( QLabel::NoFrame );
    textLabel1->setFrameShadow( QLabel::Plain );

    findLineEdit = new QLineEdit( this, "findLineEdit" );
    findLineEdit->setGeometry( QRect( 20, 40, 340, 20 ) );

    findButton = new QPushButton( this, "findButton" );
    findButton->setGeometry( QRect( 20, 230, 107, 27 ) );

    closeButton = new QPushButton( this, "closeButton" );
    closeButton->setGeometry( QRect( 250, 230, 107, 27 ) );

    exactCheckBox = new QCheckBox( this, "exactCheckBox" );
    exactCheckBox->setGeometry( QRect( 20, 200, 340, 20 ) );

    caseSensCheckBox = new QCheckBox( this, "caseSensCheckBox" );
    caseSensCheckBox->setGeometry( QRect( 20, 180, 340, 20 ) );

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setGeometry( QRect( 20, 70, 340, 90 ) );

    descRadioButton = new QRadioButton( buttonGroup1, "descRadioButton" );
    descRadioButton->setGeometry( QRect( 30, 20, 150, 20 ) );
    descRadioButton->setChecked( TRUE );

    pwRadioButton = new QRadioButton( buttonGroup1, "pwRadioButton" );
    pwRadioButton->setGeometry( QRect( 180, 20, 150, 20 ) );

    commentRadioButton = new QRadioButton( buttonGroup1, "commentRadioButton" );
    commentRadioButton->setGeometry( QRect( 180, 40, 150, 20 ) );

    nameRadioButton = new QRadioButton( buttonGroup1, "nameRadioButton" );
    nameRadioButton->setGeometry( QRect( 30, 40, 150, 20 ) );

    urlRadioButton = new QRadioButton( buttonGroup1, "urlRadioButton" );
    urlRadioButton->setGeometry( QRect( 30, 60, 150, 20 ) );

    launcherRadioButton = new QRadioButton( buttonGroup1, "launcherRadioButton" );
    launcherRadioButton->setGeometry( QRect( 180, 60, 150, 20 ) );
    languageChange();
    resize( QSize(381, 269).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( findButton, SIGNAL( clicked() ), this, SLOT( findButton_slot() ) );
    connect( closeButton, SIGNAL( clicked() ), this, SLOT( closeButton_slot() ) );
    connect( descRadioButton, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );
    connect( nameRadioButton, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );
    connect( pwRadioButton, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );
    connect( commentRadioButton, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );
    connect( findLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( selectionChanged_slot() ) );
    connect( caseSensCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );
    connect( exactCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );

    // tab order
    setTabOrder( findLineEdit, descRadioButton );
    setTabOrder( descRadioButton, caseSensCheckBox );
    setTabOrder( caseSensCheckBox, exactCheckBox );
    setTabOrder( exactCheckBox, findButton );
    setTabOrder( findButton, closeButton );
    setTabOrder( closeButton, nameRadioButton );
    setTabOrder( nameRadioButton, pwRadioButton );
    setTabOrder( pwRadioButton, commentRadioButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
findWnd::~findWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void findWnd::languageChange()
{
    setCaption( tr( "Find" ) );
    textLabel1->setText( tr( "Find:" ) );
    findButton->setText( tr( "&Find" ) );
    //US ENH    findButton->setAccel( QKeySequence( tr( "Alt+F" ) ) );
    closeButton->setText( tr( "&Close" ) );
//US ENH    closeButton->setAccel( QKeySequence( tr( "Alt+C" ) ) );
    exactCheckBox->setText( tr( "&Exact match" ) );
//US ENH    exactCheckBox->setAccel( QKeySequence( tr( "Alt+E" ) ) );
    caseSensCheckBox->setText( tr( "&Case sensitive" ) );
//US ENH    caseSensCheckBox->setAccel( QKeySequence( tr( "Alt+C" ) ) );
    buttonGroup1->setTitle( tr( "Search in Column" ) );
    descRadioButton->setText( tr( "&Description" ) );
    pwRadioButton->setText( tr( "&Password" ) );
    commentRadioButton->setText( tr( "C&omment" ) );
    nameRadioButton->setText( tr( "&Username" ) );
//US ENH    nameRadioButton->setAccel( QKeySequence( tr( "Alt+U" ) ) );
    urlRadioButton->setText( tr( "U&RL" ) );
//US ENH    urlRadioButton->setAccel( QKeySequence( tr( "Alt+R" ) ) );
    launcherRadioButton->setText( tr( "&Launcher" ) );
//US ENH    launcherRadioButton->setAccel( QKeySequence( tr( "Alt+L" ) ) );
}

void findWnd::findButton_slot()
{
    qWarning( "findWnd::findButton_slot(): Not implemented yet" );
}

void findWnd::selectionChanged_slot()
{
    qWarning( "findWnd::selectionChanged_slot(): Not implemented yet" );
}

void findWnd::closeButton_slot()
{
    qWarning( "findWnd::closeButton_slot(): Not implemented yet" );
}

