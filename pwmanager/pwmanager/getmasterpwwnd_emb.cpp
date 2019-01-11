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

$Id: getmasterpwwnd_emb.cpp,v 1.6 2004/10/25 22:24:48 zautrix Exp $
*/                                                                      

#include "getmasterpwwnd_emb.h"

#include "klocale.h"

/*
#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
*/

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qapplication.h>

/*
 *  Constructs a getMasterPwWnd as a child of 'parent', with the
 *  name 'name' 
 */
getMasterPwWnd::getMasterPwWnd( QWidget* parent, const char* name)
  : KDialogBase( KDialogBase::Plain, i18n( "Master-password" ), 
                 KDialogBase::Ok | KDialogBase::Cancel,
                 KDialogBase::Ok, parent, name, true )
{
    QWidget *page = plainPage();
    QVBoxLayout *pageLayout = new QVBoxLayout( page );

    pwLineEdit = new QLineEdit( page, "pwLineEdit" );
    pwLineEdit->setEchoMode( QLineEdit::Password );

    QLabel* textLabel1 = new QLabel( pwLineEdit, i18n("Please enter the master-password:"), page, "textLabel1" );
    textLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    pageLayout->addWidget(textLabel1);
    pageLayout->addWidget(pwLineEdit);
    
    QWidget* numberBox = new QWidget( page );
#ifndef DESKTOP_VERSION
    if ( QApplication::desktop()->width() > 320 ) {
        numberBox->setFixedHeight(250);
        numberBox->setFixedWidth(200);
    }
    else{
        numberBox->setFixedHeight(150);
        numberBox->setFixedWidth(150);
    }
#endif

    QGridLayout* numberLayout = new QGridLayout( numberBox, 4, 3 );
    numberLayout->setMargin( 0 );
    numberLayout->setSpacing( 0 );

    QPushButton* p1 = new QPushButton( i18n("1"), numberBox );
    numberLayout->addWidget( p1, 0, 0 );
    QPushButton* p2 = new QPushButton( i18n("2"), numberBox );
    numberLayout->addWidget( p2, 0, 1 );
    QPushButton* p3 = new QPushButton( i18n("3"), numberBox );
    numberLayout->addWidget( p3, 0, 2 );
    QPushButton* p4 = new QPushButton( i18n("4"), numberBox );
    numberLayout->addWidget( p4, 1, 0 );
    QPushButton* p5 = new QPushButton( i18n("5"), numberBox );
    numberLayout->addWidget( p5, 1, 1 );
    QPushButton* p6 = new QPushButton( i18n("6"), numberBox );
    numberLayout->addWidget( p6, 1, 2 );
    QPushButton* p7 = new QPushButton( i18n("7"), numberBox );
    numberLayout->addWidget( p7, 2, 0 );
    QPushButton* p8 = new QPushButton( i18n("8"), numberBox );
    numberLayout->addWidget( p8, 2, 1 );
    QPushButton* p9 = new QPushButton( i18n("9"), numberBox );
    numberLayout->addWidget( p9, 2, 2 );
    QPushButton* clear  = new QPushButton( i18n("x"), numberBox );
    numberLayout->addWidget( clear, 3, 0 );
    QPushButton* p0 = new QPushButton( i18n("0"), numberBox );
    numberLayout->addWidget( p0, 3, 1 );
    QPushButton* backspace = new QPushButton( i18n("-"), numberBox );
    numberLayout->addWidget( backspace, 3, 2 );


    pageLayout->addWidget(numberBox);
#ifdef DESKTOP_VERSION
    resize( sizeHint() );
#else
    resize( 200,sizeHint().height() );
#endif

    connect( p0, SIGNAL( clicked() ), this, SLOT( add0() ) );
    connect( p1, SIGNAL( clicked() ), this, SLOT( add1() ) );
    connect( p2, SIGNAL( clicked() ), this, SLOT( add2() ) );
    connect( p3, SIGNAL( clicked() ), this, SLOT( add3() ) );
    connect( p4, SIGNAL( clicked() ), this, SLOT( add4() ) );
    connect( p5, SIGNAL( clicked() ), this, SLOT( add5() ) );
    connect( p6, SIGNAL( clicked() ), this, SLOT( add6() ) );
    connect( p7, SIGNAL( clicked() ), this, SLOT( add7() ) );
    connect( p8, SIGNAL( clicked() ), this, SLOT( add8() ) );
    connect( p9, SIGNAL( clicked() ), this, SLOT( add9() ) );
    connect( backspace, SIGNAL( clicked() ), this, SLOT( backspace() ) );
    connect( clear, SIGNAL( clicked() ), this, SLOT( clear() ) );

    pwLineEdit->setFocus();

}

/*
 *  Destroys the object and frees any allocated resources
 */
getMasterPwWnd::~getMasterPwWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

void getMasterPwWnd::okButton_slot()
{
    qWarning( "getMasterPwWnd::okButton_slot(): Not implemented yet" );
}

void getMasterPwWnd::cancelButton_slot()
{
    qWarning( "getMasterPwWnd::cancelButton_slot(): Not implemented yet" );
}

void getMasterPwWnd::add0()
{
  addCharacter("0");
}
void getMasterPwWnd::add1()
{
  addCharacter("1");
}
void getMasterPwWnd::add2()
{
  addCharacter("2");
}
void getMasterPwWnd::add3()
{
  addCharacter("3");
}
void getMasterPwWnd::add4()
{
  addCharacter("4");
}
void getMasterPwWnd::add5()
{
  addCharacter("5");
}
void getMasterPwWnd::add6()
{
  addCharacter("6");
}
void getMasterPwWnd::add7()
{
  addCharacter("7");
}
void getMasterPwWnd::add8()
{
  addCharacter("8");
}
void getMasterPwWnd::add9()
{
  addCharacter("9");
}
void getMasterPwWnd::backspace()
{
  QString old = pwLineEdit->text();
  old.truncate(old.length()-1);
  pwLineEdit->setText(old);
}

void getMasterPwWnd::clear()
{
  pwLineEdit->setText("");
}

void getMasterPwWnd::addCharacter(const QString& s)
{
  QString old = pwLineEdit->text();
  pwLineEdit->setText(old + s);
}

