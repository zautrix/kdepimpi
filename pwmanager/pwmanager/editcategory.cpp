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

$Id: editcategory.cpp,v 1.2 2005/01/19 14:36:33 zautrix Exp $
*/                                                                      

#include "editcategory.h"
#include "pwmdoc.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <klocale.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <qpushbutton.h>


/*
 *  Constructs a addEntryWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
editCategoryWnd::editCategoryWnd( PwMDoc* d, QWidget* parent, const char* name)
  : KDialogBase( KDialogBase::Plain, i18n( "edit category descriptions" ), 
                 Apply | User2 | Ok,
                 Ok, parent, name, true ),
    doc(d)
{
    findButton( Ok )->setText (i18n("Close" )) ;
    findButton( User2 )->setText (i18n("Cancel" )) ;
    connect(this,SIGNAL(user2Clicked()), SLOT(cancel_slot())); 
    enableButton( KDialogBase::Apply, false );


    QWidget *page = plainPage();
    QGridLayout *layout = new QGridLayout( page, 3, 1 );
    layout->setMargin( KDialogBase::marginHint() );
    layout->setSpacing( KDialogBase::spacingHint() );

    int i = 0;
    categoryComboBox = new KComboBox( page  );
    QLabel* label = new QLabel( categoryComboBox, i18n("Category:"), page );
    layout->addWidget( label, i, 0 );
    layout->addWidget( categoryComboBox, i, 1 );
    i++;
    categoryComboBox->setEditable( FALSE );
    categoryComboBox->setSizeLimit( 100 );
    connect(categoryComboBox,SIGNAL(activated(const QString&)), SLOT(categorySelected(const QString&))); 


    descLineEdit = new KLineEdit( page, "descLineEdit" );
    label = new QLabel( descLineEdit, i18n("Text1 (Description):"), page );
    layout->addWidget( label, i, 0 );
    layout->addWidget( descLineEdit, i, 1 );
    connect( descLineEdit, SIGNAL( textChanged ( const QString & ) ), SLOT( widgetModified(const QString &) ) );
    i++;

    usernameLineEdit = new KLineEdit( page, "usernameLineEdit" );
    label = new QLabel( usernameLineEdit, i18n("Text2 (Username):"), page );
    layout->addWidget( label, i, 0 );
    layout->addWidget( usernameLineEdit, i, 1 );
    connect( usernameLineEdit, SIGNAL( textChanged ( const QString & ) ), SLOT( widgetModified(const QString &) ) );
    i++;

    pwLineEdit = new KLineEdit( page, "pwLineEdit" );
    label = new QLabel( pwLineEdit, i18n("Text3 (Password):"), page );
    layout->addWidget( label, i, 0 );
    layout->addWidget( pwLineEdit, i, 1 );
    connect( pwLineEdit, SIGNAL( textChanged ( const QString & ) ), SLOT( widgetModified(const QString &) ) );
    i++;

    unsigned int count = doc->numCategories();

    for (unsigned int i = 0; i < count; ++i) {
      categoryComboBox->insertItem(doc->getCategory(i)->c_str()); 
    }

    //	PwMCategoryItem* getCategoryEntry(unsigned int index)
    //		{ return &(dti.dta[index]); }



}

/*
 *  Destroys the object and frees any allocated resources
 */
editCategoryWnd::~editCategoryWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

void editCategoryWnd::slotOk()
{
  //    qDebug( "addEntryWnd::slotOk(): Not implemented yet" );
    slotApply();
    accept();
}

void editCategoryWnd::slotApply()
{   
  QString cat = categoryComboBox->currentText();

  unsigned int idx;
  bool found = doc->findCategory(cat, &idx);

  if (found == true)
    {
      PwMCategoryItem* catitem = doc->getCategoryEntry(idx);

      catitem->desc_text = descLineEdit->text().latin1();
      catitem->name_text = usernameLineEdit->text().latin1();
      catitem->pw_text = pwLineEdit->text().latin1();
      enableButton( KDialogBase::Apply, false );
      return;
    }

  BUG();

}

void editCategoryWnd::cancel_slot()
{   
  QString cat = categoryComboBox->currentText();
  categorySelected ( cat );
  reject();
}

void editCategoryWnd::setCurrCategory(const QString &cat)
{
	int i, count = categoryComboBox->count();

	for (i = 0; i < count; ++i) {
		if (categoryComboBox->text(i) == cat) {
			categoryComboBox->setCurrentItem(i);
			categorySelected ( cat );
			return;
		}
	}
	BUG();
}

void editCategoryWnd::categorySelected ( const QString & string )
{
  unsigned int idx;
  bool found = doc->findCategory(string, &idx);

  if (found == true)
    {
      PwMCategoryItem* catitem = doc->getCategoryEntry(idx);

      descLineEdit->setText(catitem->desc_text.c_str());
      usernameLineEdit->setText(catitem->name_text.c_str());
      pwLineEdit->setText(catitem->pw_text.c_str());
      enableButton( KDialogBase::Apply, false );
      return;
    }

  BUG();

}

void editCategoryWnd::widgetModified(const QString &)
{
  enableButton( KDialogBase::Apply, true );
}

