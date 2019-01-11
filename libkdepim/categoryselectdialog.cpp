/*
    This file is part of libkdepim.
    Copyright (c) 2000, 2001, 2002 Cornelius Schumacher <schumacher@kde.org>

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
*/

#include <qlistview.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <kmessagebox.h>


#include "categoryeditdialog.h"
#include "categoryselectdialog.h"

#include "kpimprefs.h"

using namespace KPIM;

CategorySelectDialog::CategorySelectDialog( KPimPrefs *prefs, QWidget* parent,
                                            const char* name, 
                                            bool modal, Qt::WFlags fl )
  : CategorySelectDialog_base( parent, name, true, fl ),
    mPrefs( prefs )
{
    Q_UNUSED( modal );
    mColorItem = 0;
    mColorEnabled = false;
    mCategories->header()->hide();

    setCategories();
 
    connect(mButtonEdit,SIGNAL(clicked()),this, SLOT(editCategoriesDialog()));
    if ( QApplication::desktop()->width() > 460 )
        resize( 300, 360 );
    else
        showMaximized();
    connect( mSetColorCat, SIGNAL( clicked() ), this, SLOT( setColorCat() ) );
    // connect( mCategories, SIGNAL( clicked(QListViewItem *) ), this, SLOT( clicked(QListViewItem *) ) );
#ifndef DESKTOP_VERSION
    mButtonOk->hide();
    mButtonCancel->hide();
#endif
}
void CategorySelectDialog::editCategoriesDialog()
{
    KPIM::CategoryEditDialog* ced = new KPIM::CategoryEditDialog(mPrefs,this );
    
    ced->exec(); 
    delete ced;
    slotApply();
    QStringList temp = mCategoryList;
    setCategories();
    setSelected( temp );
}
void CategorySelectDialog::setCategories()
{
  mColorItem = 0;
  mCategories->clear();
  mCategoryList.clear();

  QStringList::Iterator it;
  for (it = mPrefs->mCustomCategories.begin();
       it != mPrefs->mCustomCategories.end(); ++it ) {
      CategorySelectItem * item = new CategorySelectItem(mCategories,*it,QCheckListItem::CheckBox);
      QObject::connect( item, SIGNAL( stateChanged(QListViewItem *) ), this, SLOT( clicked(QListViewItem *) ) );
      
  }
}

CategorySelectDialog::~CategorySelectDialog()
{
}

void CategorySelectDialog::setSelected(const QStringList &selList)
{
  clear();

  QStringList::ConstIterator it;
  QStringList notFound;
  bool found = false;
  for (it=selList.begin();it!=selList.end();++it) {
      //qDebug(" CategorySelectDialog::setSelected(");
    QCheckListItem *item = (QCheckListItem *)mCategories->firstChild();
    while (item) {
      if (item->text() == *it) {
        item->setOn(true);
        if ( ! found )
            setColorItem( item );
        found = true;
        break;
      }
      item = (QCheckListItem *)item->nextSibling();
    }
//   if ( ! found ) {

//emit updateCategoriesGlobal();
//   QMessageBox::information( this, "KO/E: Information!",
//                                   "Categories found, which were not\n"
//                                   "in list of categories!\n"
//                                   "message",
//                                           "OK", "", 0,
//                                           0, 1 );
//         setSelected(selList);
//         return;
//    }
  }
}

QStringList CategorySelectDialog::selectedCategories() const
{
  return mCategoryList;
}
void CategorySelectDialog::setColorEnabled()
{
    mColorEnabled = true;
    mSetColorCat->show();
}
void CategorySelectDialog::setColorCat()
{
    QCheckListItem * newColorItem = (QCheckListItem * )mCategories->currentItem ();
   if ( !newColorItem ) {
       KMessageBox::error(this,i18n("There is no current item."));
       return;
   }
   if ( !newColorItem->isOn() )
       newColorItem->setOn( true );
   setColorItem( newColorItem );

}
void CategorySelectDialog::clicked ( QListViewItem * it )
{
    if ( ! it )
        return;
    QCheckListItem *i = (QCheckListItem *) it;
    if ( !i->isOn() && i==mColorItem) {
        setColorItem( 0);
        QCheckListItem *item = (QCheckListItem *)mCategories->firstChild();
        while (item) {
            if (item->isOn()) {
                setColorItem( item );
                break;
            }
            item = (QCheckListItem *)item->nextSibling();
        }
    } else if ( i->isOn() && !mColorItem) {
        setColorItem( i);
    }
}
void  CategorySelectDialog::setColorItem( QCheckListItem * newColorItem )
{
    if ( !mColorEnabled )
        return;
    if ( mColorItem == newColorItem) 
        return;
    if ( mColorItem ) {
        mColorItem->setPixmap ( 0, QPixmap() );
        mColorItem = 0;
    }
    if ( newColorItem ) {
        QPixmap pix (newColorItem->height()/2, newColorItem->height()/2 );
        pix.fill(Qt::red );
        newColorItem->setPixmap ( 0, pix );
        mColorItem = newColorItem;
    }
}
void CategorySelectDialog::slotApply()
{
  QStringList categories;
  QCheckListItem *item = (QCheckListItem *)mCategories->firstChild();
  QString colcat;
  while (item) {
    if (item->isOn()) {
        if ( item == mColorItem)
            colcat = item->text();
        else
            categories.append(item->text());
    }
    item = (QCheckListItem *)item->nextSibling();
  }
  categories.sort();
  if ( ! colcat.isEmpty() )
      categories.prepend( colcat );
  // QString categoriesStr = categories.join(",");

  mCategoryList = categories;

}
void CategorySelectDialog::accept()
{
    slotOk();
}

void CategorySelectDialog::slotOk()
{
  slotApply();
  emit categoriesSelected(mCategoryList);
  emit categoriesSelected(mCategoryList.join(","));
  QDialog::accept();
}

void CategorySelectDialog::clear()
{
  QCheckListItem *item = (QCheckListItem *)mCategories->firstChild();
  while (item) {
    item->setOn(false);
    item = (QCheckListItem *)item->nextSibling();
  }  
}

void CategorySelectDialog::updateCategoryConfig()
{
  QStringList selected;
  QCheckListItem *item = (QCheckListItem *)mCategories->firstChild();
  while (item) {
    if (item->isOn()) {
      selected.append(item->text());
    }
    item = (QCheckListItem *)item->nextSibling();
  }

  setCategories();
  
  setSelected(selected);
}

//#include "categoryselectdialog.moc"
