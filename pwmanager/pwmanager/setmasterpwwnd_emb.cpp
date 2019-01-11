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

$Id: setmasterpwwnd_emb.cpp,v 1.1 2004/09/24 19:53:20 ulf69 Exp $
*/                                                                      

#include "setmasterpwwnd_emb.h"

#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>

#include <klocale.h>

/*US
#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
*/

/*
 *  Constructs a setMasterPwWnd as a child of 'parent', with the
 *  name 'name'
 *
 */
setMasterPwWnd::setMasterPwWnd( QWidget* parent, const char* name )
  : KDialogBase( KDialogBase::Plain, i18n( "Set master-password" ), 
                 KDialogBase::Ok | KDialogBase::Cancel,
                 KDialogBase::Ok, parent, name, true )
{
    QWidget *page = plainPage();
    QVBoxLayout *pageLayout = new QVBoxLayout( page );

    mainTab = new QTabWidget( page );
    pageLayout->addWidget( mainTab );


    if ( !name )
	setName( "setMasterPwWnd" );

    ////////////////////////////////////////////////////////////////////
    // This is the Password tab1
    tab = new QWidget( mainTab );

    QGridLayout *layout = new QGridLayout( tab, 5, 1 );
    layout->setMargin( KDialogBase::marginHint() );
    layout->setSpacing( KDialogBase::spacingHint() );

    QLabel* label = new QLabel( i18n( "Using a normal password-string to encrypt the data." ), tab );
    label->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    layout->addWidget(label, 0, 0);

    pwEdit_1 = new QLineEdit( tab, "pwEdit_1" );
    pwEdit_1->setEchoMode( QLineEdit::Password );

    label = new QLabel( pwEdit_1, i18n( "Please enter the new master-password:" ), tab );
    layout->addWidget(label, 1, 0);
    layout->addWidget(pwEdit_1, 2, 0);

    pwEdit_2 = new QLineEdit( tab, "pwEdit_2" );
    pwEdit_2->setEchoMode( QLineEdit::Password );

    label = new QLabel( pwEdit_2, i18n( "Please enter the password again:" ), tab );
    layout->addWidget(label, 3, 0);
    layout->addWidget(pwEdit_2, 4, 0);

    mainTab->insertTab( tab, i18n("Normal password") );


    ////////////////////////////////////////////////////////////////////
    // This is the Password tab2
    tab_2 = new QWidget( mainTab );

    layout = new QGridLayout( tab_2, 5, 1 );
    layout->setMargin( KDialogBase::marginHint() );
    layout->setSpacing( KDialogBase::spacingHint() );

    label = new QLabel( i18n( "Using a PwM key-card to encrypt the data." ), tab_2 );
    label->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    layout->addWidget(label, 0, 0);

    label = new QLabel( i18n( "selected card:" ), tab_2 );
    label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout->addWidget(label, 1, 0);

    selCardButton = new QPushButton( i18n( "&Select the currently inserted card as key-card" ), tab_2 );
    layout->addWidget(selCardButton, 2, 0);

    genCardButton = new QPushButton(  i18n( "&generate new key-card") , tab_2  );
    layout->addWidget(genCardButton, 3, 0);

    curCardIdLabel = new QLabel( QString("") , tab_2 );
    layout->addWidget(curCardIdLabel, 4, 0);

    mainTab->insertTab( tab_2, i18n("Key-card (chipcard)" ) );


				      //    resize( QSize(411, 313).expandedTo(minimumSizeHint()) );

    // signals and slots connections
				      //    connect( okButton, SIGNAL( clicked() ), this, SLOT( okButton_slot() ) );
				      //    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButton_slot() ) );
    connect( genCardButton, SIGNAL( clicked() ), this, SLOT( genCardButton_slot() ) );
    connect( selCardButton, SIGNAL( clicked() ), this, SLOT( selCardButton_slot() ) );

}

/*
 *  Destroys the object and frees any allocated resources
 */
setMasterPwWnd::~setMasterPwWnd()
{
    // no need to delete child widgets, Qt does it all for us
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

