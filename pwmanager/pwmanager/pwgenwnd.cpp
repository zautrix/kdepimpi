/****************************************************************************
** Form implementation generated from reading ui file 'pwgenwnd.ui'
**
** Created: Tue Sep 14 15:34:41 2004
**      by: The User Interface Compiler ($Id: pwgenwnd.cpp,v 1.1 2004/09/15 17:55:39 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "pwgenwnd.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a pwGenWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
pwGenWnd::pwGenWnd( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "pwGenWnd" );
    pwGenWndLayout = new QVBoxLayout( this, 11, 6, "pwGenWndLayout"); 

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    groupBox1Layout = new QVBoxLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    int_charLowerCheckBox = new QCheckBox( groupBox1, "int_charLowerCheckBox" );
    int_charLowerCheckBox->setChecked( TRUE );
    layout2->addWidget( int_charLowerCheckBox );
    spacer1 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer1 );

    textLabel4 = new QLabel( groupBox1, "textLabel4" );
    layout2->addWidget( textLabel4 );
    groupBox1Layout->addLayout( layout2 );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    int_charUpperCheckBox = new QCheckBox( groupBox1, "int_charUpperCheckBox" );
    int_charUpperCheckBox->setChecked( TRUE );
    layout3->addWidget( int_charUpperCheckBox );
    spacer2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer2 );

    textLabel5 = new QLabel( groupBox1, "textLabel5" );
    layout3->addWidget( textLabel5 );
    groupBox1Layout->addLayout( layout3 );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    int_charNumCheckBox = new QCheckBox( groupBox1, "int_charNumCheckBox" );
    int_charNumCheckBox->setChecked( TRUE );
    layout4->addWidget( int_charNumCheckBox );
    spacer3 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer3 );

    textLabel6 = new QLabel( groupBox1, "textLabel6" );
    layout4->addWidget( textLabel6 );
    groupBox1Layout->addLayout( layout4 );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    int_charSpecCheckBox = new QCheckBox( groupBox1, "int_charSpecCheckBox" );
    layout6->addWidget( int_charSpecCheckBox );
    spacer4 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout6->addItem( spacer4 );

    textLabel7 = new QLabel( groupBox1, "textLabel7" );
    layout6->addWidget( textLabel7 );
    groupBox1Layout->addLayout( layout6 );

    int_charBlankCheckBox = new QCheckBox( groupBox1, "int_charBlankCheckBox" );
    groupBox1Layout->addWidget( int_charBlankCheckBox );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    int_charUserCheckBox = new QCheckBox( groupBox1, "int_charUserCheckBox" );
    layout7->addWidget( int_charUserCheckBox );
    spacer5 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( spacer5 );

    int_userDefLineEdit = new QLineEdit( groupBox1, "int_userDefLineEdit" );
    int_userDefLineEdit->setEnabled( FALSE );
    layout7->addWidget( int_userDefLineEdit );
    groupBox1Layout->addLayout( layout7 );
    pwGenWndLayout->addWidget( groupBox1 );

    layout8 = new QHBoxLayout( 0, 0, 6, "layout8"); 

    textLabel2 = new QLabel( this, "textLabel2" );
    layout8->addWidget( textLabel2 );

    int_lenSpinBox = new QSpinBox( this, "int_lenSpinBox" );
    int_lenSpinBox->setMaxValue( 9999 );
    int_lenSpinBox->setMinValue( 1 );
    int_lenSpinBox->setValue( 8 );
    layout8->addWidget( int_lenSpinBox );
    pwGenWndLayout->addLayout( layout8 );

    int_filterCheckBox = new QCheckBox( this, "int_filterCheckBox" );
    int_filterCheckBox->setChecked( TRUE );
    pwGenWndLayout->addWidget( int_filterCheckBox );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    genButton = new QPushButton( this, "genButton" );
    layout1->addWidget( genButton );

    cancelButton = new QPushButton( this, "cancelButton" );
    layout1->addWidget( cancelButton );
    pwGenWndLayout->addLayout( layout1 );
    languageChange();
    resize( QSize(450, 349).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( int_charUserCheckBox, SIGNAL( toggled(bool) ), int_userDefLineEdit, SLOT( setEnabled(bool) ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButton_slot() ) );
    connect( genButton, SIGNAL( clicked() ), this, SLOT( genButton_slot() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
pwGenWnd::~pwGenWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void pwGenWnd::languageChange()
{
    setCaption( tr( "Password generator" ) );
    groupBox1->setTitle( tr( "Character set:" ) );
    int_charLowerCheckBox->setText( tr( "Lowercase:" ) );
    textLabel4->setText( tr( "abc" ) );
    int_charUpperCheckBox->setText( tr( "Uppercase:" ) );
    textLabel5->setText( tr( "ABC" ) );
    int_charNumCheckBox->setText( tr( "Numbers:" ) );
    textLabel6->setText( tr( "123" ) );
    int_charSpecCheckBox->setText( tr( "Special characters:" ) );
#ifndef PWM_EMBEDDED
    textLabel7->setText( trUtf8( "\x21\x22\xc2\xa7\x24\x25\x26\x2f\x28\x29\x3d\x3f\x2c\x2e\x2d\x3b\x3a\x5f\x2b" ) );
#else
    QString st("\x21\x22\xc2\xa7\x24\x25\x26\x2f\x28\x29\x3d\x3f\x2c\x2e\x2d\x3b\x3a\x5f\x2b");
    textLabel7->setText( st.utf8() );
#endif
    int_charBlankCheckBox->setText( tr( "Spaces (blank characters)" ) );
    int_charUserCheckBox->setText( tr( "User defined:" ) );
    textLabel2->setText( tr( "Password Length:" ) );
    int_filterCheckBox->setText( tr( "Enable Filtering to get better passwords" ) );
    genButton->setText( tr( "&Generate now" ) );
    cancelButton->setText( tr( "&Cancel" ) );
}

void pwGenWnd::cancelButton_slot()
{
    qWarning( "pwGenWnd::cancelButton_slot(): Not implemented yet" );
}

void pwGenWnd::genButton_slot()
{
    qWarning( "pwGenWnd::genButton_slot(): Not implemented yet" );
}

