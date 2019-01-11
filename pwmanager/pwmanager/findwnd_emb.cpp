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

$Id: findwnd_emb.cpp,v 1.2 2004/10/19 22:13:43 zautrix Exp $
*/                                                                      

#include "findwnd_emb.h"

#include <klocale.h>

#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <klineedit.h>
#include <qlabel.h>

/*
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
*/

/*
 *  Constructs a findWnd as a child of 'parent', with the
 *  name 'name'.
 */
findWnd::findWnd( QWidget* parent, const char* name )
  : KDialogBase( KDialogBase::Plain, i18n( "Find" ), 
                 KDialogBase::Ok | KDialogBase::Cancel,
                 KDialogBase::Ok, parent, name, true )
{
    QWidget *page = plainPage();
    QVBoxLayout *layout = new QVBoxLayout( page );
    layout->setMargin( KDialogBase::marginHint() );
    layout->setSpacing( KDialogBase::spacingHint() );

    exactCheckBox = new QCheckBox( i18n( "&Exact match" ), page );
    layout->addWidget( exactCheckBox );

    caseSensCheckBox = new QCheckBox( i18n( "&Case sensitive" ), page );
    layout->addWidget( caseSensCheckBox );


    QButtonGroup* buttonGroup1 = new QButtonGroup(1, Qt::Horizontal, i18n( "Search in Column" ), page );
    layout->addWidget( buttonGroup1 );

    descRadioButton = new QRadioButton( i18n( "&Description" ), buttonGroup1  );
    pwRadioButton = new QRadioButton( i18n( "&Password" ), buttonGroup1 );
    commentRadioButton = new QRadioButton( i18n( "C&omment" ), buttonGroup1 );
    nameRadioButton = new QRadioButton( i18n( "&Username" ), buttonGroup1 );
    urlRadioButton = new QRadioButton( i18n( "U&RL" ), buttonGroup1 );
    launcherRadioButton = new QRadioButton( i18n( "&Launcher" ), buttonGroup1 );
    descRadioButton->setChecked( TRUE );

    findLineEdit = new KLineEdit( page );
    // QLabel* label = new QLabel( findLineEdit, i18n( "Find:" ), page );
    //layout->addWidget( label );
    layout->addWidget( findLineEdit );
    
    setButtonText( KDialogBase::Ok, "&Find" );
    setButtonText( KDialogBase::Cancel, "&Close" );


    resize( sizeHint() );


    connect( buttonGroup1, SIGNAL( clicked(int) ), this, SLOT( selectionChanged_slot() ) );
    connect( findLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( selectionChanged_slot() ) );
    connect( caseSensCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );
    connect( exactCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( selectionChanged_slot() ) );

}

/*
 *  Destroys the object and frees any allocated resources
 */
findWnd::~findWnd()
{
    // no need to delete child widgets, Qt does it all for us
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

