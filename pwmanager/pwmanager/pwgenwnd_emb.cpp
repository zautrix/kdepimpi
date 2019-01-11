/*                                                                      
    This file is part of PwManager/Platform independent.                                  
    Copyright (c) 2004 Ulf Schenk
                                                                        
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or   
    (at your option) any later version.                                 
                                                                        
    This program is distributed in the hope that it will be useful,     
    but WITHOUT ANY WARRANTY; without even the implied warranty of      
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        
    GNU General Public License for more details.                        
                                                                        
    You should have received a copy of the GNU General Public License   
    along with this program; if not, write to the Free Software         
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           
                                                                        
    As a special exception, permission is given to link this program    
    with any edition of Qt, and distribute the resulting executable,    
    without including the source code for Qt in the source distribution.

$Id: pwgenwnd_emb.cpp,v 1.2 2004/09/24 19:51:43 ulf69 Exp $
*/                                                                      

#include "pwgenwnd_emb.h"

#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlayout.h>

#include "klocale.h"

/*
 *  Constructs a pwGenWnd as a child of 'parent', with the
 *  name 'name'.
 *
 */
pwGenWnd::pwGenWnd( QWidget* parent, const char* name)
  : KDialogBase( KDialogBase::Plain, i18n( "Password generator" ), 
                 KDialogBase::Ok | KDialogBase::Cancel,
                 KDialogBase::Ok, parent, name, true )
{
    QWidget *page = plainPage();
    QVBoxLayout* pwGenWndLayout = new QVBoxLayout( page, 11, 6, "pwGenWndLayout"); 

    QGroupBox* groupBox1 = new QGroupBox( page, "groupBox1" );
    groupBox1->setTitle( i18n( "Character set:" ) );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    QVBoxLayout* groupBox1Layout = new QVBoxLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    int_charLowerCheckBox = new QCheckBox( i18n("Lowercase (abc)"), groupBox1, "int_charLowerCheckBox" );
    int_charLowerCheckBox->setChecked( TRUE );
    groupBox1Layout->addWidget( int_charLowerCheckBox );

    int_charUpperCheckBox = new QCheckBox( i18n("Uppercase (ABC)"), groupBox1, "int_charUpperCheckBox" );
    int_charUpperCheckBox->setChecked( TRUE );
    groupBox1Layout->addWidget( int_charUpperCheckBox );


    int_charNumCheckBox = new QCheckBox( i18n("Numbers (123)"), groupBox1, "int_charNumCheckBox" );
    int_charNumCheckBox->setChecked( TRUE );
    groupBox1Layout->addWidget( int_charNumCheckBox );

    int_charSpecCheckBox = new QCheckBox( i18n("Special characters:"), groupBox1, "int_charSpecCheckBox" );
    groupBox1Layout->addWidget( int_charSpecCheckBox );

    QLabel* textLabel7 = new QLabel( groupBox1, "textLabel7" );
    QString st("\x21\x22\xc2\xa7\x24\x25\x26\x2f\x28\x29\x3d\x3f\x2c\x2e\x2d\x3b\x3a\x5f\x2b");
    textLabel7->setText( st.utf8() );
    groupBox1Layout->addWidget( textLabel7 );

    int_charBlankCheckBox = new QCheckBox( i18n( "Spaces (blank characters)"), groupBox1, "int_charBlankCheckBox" );
    groupBox1Layout->addWidget( int_charBlankCheckBox );

    QHBoxLayout* layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    int_charUserCheckBox = new QCheckBox( i18n("User defined:"), groupBox1, "int_charUserCheckBox" );
    layout7->addWidget( int_charUserCheckBox );

    int_userDefLineEdit = new QLineEdit( groupBox1, "int_userDefLineEdit" );
    int_userDefLineEdit->setEnabled( FALSE );
    layout7->addWidget( int_userDefLineEdit );
    groupBox1Layout->addLayout( layout7 );
    pwGenWndLayout->addWidget( groupBox1 );


    QHBoxLayout* layout8 = new QHBoxLayout( 0, 0, 6, "layout8"); 

    int_lenSpinBox = new QSpinBox( page, "int_lenSpinBox" );
    QLabel* textLabel2 = new QLabel( int_lenSpinBox, i18n("Password Length:"), page, "textLabel2" );
    layout8->addWidget( textLabel2 );

    int_lenSpinBox->setMaxValue( 9999 );
    int_lenSpinBox->setMinValue( 1 );
    int_lenSpinBox->setValue( 8 );
    layout8->addWidget( int_lenSpinBox );
    pwGenWndLayout->addLayout( layout8 );

    int_filterCheckBox = new QCheckBox( i18n( "Enable Filtering to get better passwords" ), page, "int_filterCheckBox" );
    int_filterCheckBox->setChecked( TRUE );
    pwGenWndLayout->addWidget( int_filterCheckBox );

    setButtonText( KDialogBase::Ok, "&Generate now" );

    // signals and slots connections
    connect( int_charUserCheckBox, SIGNAL( toggled(bool) ), int_userDefLineEdit, SLOT( setEnabled(bool) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
pwGenWnd::~pwGenWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

void pwGenWnd::cancelButton_slot()
{
    qWarning( "pwGenWnd::cancelButton_slot(): Not implemented yet" );
}

void pwGenWnd::genButton_slot()
{
    qWarning( "pwGenWnd::genButton_slot(): Not implemented yet" );
}

