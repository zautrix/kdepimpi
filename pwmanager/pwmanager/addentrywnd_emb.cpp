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

$Id: addentrywnd_emb.cpp,v 1.6 2005/06/25 19:57:07 zautrix Exp $
*/                                                                      

#include "addentrywnd_emb.h"
#include "pwmdoc.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qgroupbox.h>
#include <klocale.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>

/*
 *  Constructs a addEntryWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addEntryWnd::addEntryWnd( PwMDoc* d, QWidget* parent, const char* name)
  : KDialogBase( KDialogBase::Plain, i18n( "edit/add a password entry" ), 
                 KDialogBase::Ok | KDialogBase::Cancel,
                 KDialogBase::Ok, parent, name, true ), doc(d)
{
    QWidget *page = plainPage();
    QVBoxLayout *pageLayout = new QVBoxLayout( page );

    QTabWidget* mTabWidget = new QTabWidget( page );
    pageLayout->addWidget( mTabWidget );

    ////////////////////////////////////////////////////////////////////
    // This is the Password tab
    QWidget *tab1 = new QWidget( mTabWidget );

    QGridLayout *layout = new QGridLayout( tab1, 3, 1 );
    layout->setMargin( KDialogBase::marginHint() );
    layout->setSpacing( KDialogBase::spacingHint() );



    int i = 0;
    descLineEdit = new KLineEdit( tab1, "descLineEdit" );
    descLineLabel = new QLabel( descLineEdit, i18n("Description:"), tab1 );
    layout->addWidget( descLineLabel, i, 0 );
    layout->addWidget( descLineEdit, i, 1 );
    i++;

    categoryComboBox = new KComboBox( tab1  );
    QLabel* label = new QLabel( categoryComboBox, i18n("Category:"), tab1 );
    layout->addWidget( label, i, 0 );
    layout->addWidget( categoryComboBox, i, 1 );
    i++;
    categoryComboBox->setEditable( TRUE );
    categoryComboBox->setSizeLimit( 8 );
    categoryComboBox->setAutoCompletion( TRUE );
    categoryComboBox->setDuplicatesEnabled( FALSE );
    connect(categoryComboBox,SIGNAL(activated(const QString&)), SLOT(categorySelected(const QString&))); 


    usernameLineEdit = new KLineEdit( tab1, "usernameLineEdit" );
    usernameLineLabel = new QLabel( usernameLineEdit, i18n("Username:"), tab1 );
    layout->addWidget( usernameLineLabel, i, 0 );
    layout->addWidget( usernameLineEdit, i, 1 );
    i++;

    pwLineEdit = new KLineEdit( tab1, "pwLineEdit" );
    pwLineEdit->setEchoMode( QLineEdit::Password );
    pwLineLabel = new QLabel( pwLineEdit, i18n("Password:"), tab1 );
    layout->addWidget( pwLineLabel, i, 0 );
    layout->addWidget( pwLineEdit, i, 1 );
    i++;

    revealButton = new QPushButton( i18n("&Reveal"), tab1, "revealButton" );
    revealButton->setToggleButton( TRUE );
    layout->addWidget( revealButton, i, 0 );

    generateButton = new QPushButton( i18n("&Generate"), tab1, "generateButton" );
    layout->addWidget( generateButton, i, 1 );
    i++;

    urlLineEdit = new KLineEdit( tab1, "urlLineEdit" );
    label = new QLabel( urlLineEdit, i18n("URL:"), tab1 );
    layout->addWidget( label, i, 0 );
    layout->addWidget( urlLineEdit, i, 1 );
    i++;

    mTabWidget->addTab( tab1, i18n( "&Password" ) );


    ////////////////////////////////////////////////////////////////////
    // This is the Comment tab
    QWidget *tab2 = new QWidget( mTabWidget );

    layout = new QGridLayout( tab2, 3, 1 );
    layout->setMargin( KDialogBase::marginHintSmall() );
    layout->setSpacing( KDialogBase::spacingHintSmall() );
    i = 0;

    commentTextEdit = new QMultiLineEdit(tab2);
    layout->addMultiCellWidget( commentTextEdit, i, i, 0, 0 );
    i++;


    mTabWidget->addTab( tab2, i18n( "&Comments" ) );


    ////////////////////////////////////////////////////////////////////
    // This is the Launcher tab
    QWidget *tab3 = new QWidget( mTabWidget );

    layout = new QGridLayout( tab3, 3, 1 );
    layout->setMargin( KDialogBase::marginHintSmall() );
    layout->setSpacing( KDialogBase::spacingHintSmall() );
    i = 0;

    launcherLineEdit = new KLineEdit( tab3, "launcherLineEdit" );
    label = new QLabel( launcherLineEdit, i18n("Launcher:"), tab3 );

    QLabel* label1 = new QLabel( launcherLineEdit, i18n("$d = Description"), tab3 );
    QLabel* label2 = new QLabel( launcherLineEdit, i18n("$n = Username"), tab3 );
    QLabel* label3 = new QLabel( launcherLineEdit, i18n("$c = Comment"), tab3 );
    QLabel* label4 = new QLabel( launcherLineEdit, i18n("$u = URL"), tab3 );
    QLabel* label5 = new QLabel( launcherLineEdit, i18n("$p = Password"), tab3 );

    label = new QLabel( launcherLineEdit, i18n("Launcher:"), tab3 );

    layout->addWidget( label1, i, 0 );
    layout->addWidget( label2, i, 1 );
    i++;
    layout->addWidget( label3, i, 0 );
    layout->addWidget( label4, i, 1 );
    i++;
    layout->addWidget( label5, i, 0 );
    i++;

    layout->addWidget( label, i, 0 );
    layout->addWidget( launcherLineEdit, i, 1 );
    i++;


    mTabWidget->addTab( tab3, i18n( "&Launcher" ) );

    // signals and slots connections
    connect( generateButton, SIGNAL( clicked() ), this, SLOT( generateButton_slot() ) );
    connect( revealButton, SIGNAL( toggled(bool) ), this, SLOT( revealButton_slot() ) );

#if 0
    if ( QApplication::desktop()->width() <= 640 && QApplication::desktop()->width() >= 480 ) {
        resize ( 440, 400 );
    }
#endif

}

/*
 *  Destroys the object and frees any allocated resources
 */
addEntryWnd::~addEntryWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

void addEntryWnd::slotOk()
{
    qWarning( "addEntryWnd::slotOk(): Not implemented yet" );
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

void addEntryWnd::categorySelected ( const QString & string )
{
  unsigned int idx;
  bool found = doc->findCategory(string, &idx);

  if (found == true)
    {
      qDebug("addEntryWnd::categorySelected found");
      PwMCategoryItem* catitem = doc->getCategoryEntry(idx);

      descLineLabel->setText(catitem->desc_text.c_str());
      usernameLineLabel->setText(catitem->name_text.c_str());
      pwLineLabel->setText(catitem->pw_text.c_str());
      return;
    }
  else
    {
      qDebug("addEntryWnd::categorySelected NOT found");
  BUG();
    }


}


