/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <qlayout.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qapplication.h>

#include <kdebug.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
  
#include <libkdepim/categoryselectdialog.h>

#include "koprefs.h"
#include "filteredit_base.h"

#include "filtereditdialog.h"
//#include "filtereditdialog.moc"

// TODO: Make dialog work on a copy of the filters objects.

class ComboFilterBox: public QComboBox
{
public: 
    ComboFilterBox( QWidget *parent=0, const char *name=0) : 
        QComboBox( parent,name ) { }
    void popupBox() { popup(); }

};

FilterEditDialog::FilterEditDialog(QPtrList<CalFilter> *filters,QWidget *parent,
                                   const char *name) :
  KDialogBase(parent,name,true,i18n("Edit Calendar Filters"),
              Ok|Apply|Cancel)
{
  mFilters = filters;

  QWidget *mainWidget = new QWidget(this);
  setMainWidget(mainWidget);

  mSelectionCombo = new ComboFilterBox(mainWidget);
  connect(mSelectionCombo,SIGNAL(activated(int)),SLOT(filterSelected()));
  // mSelectionCombo->setEditable ( true ); 
  QPushButton *addButton = new QPushButton(i18n("Add Filter"),mainWidget);
  connect(addButton,SIGNAL(clicked()),SLOT(slotAdd()));
  addButton->setMaximumSize( addButton->sizeHint());
  mRemoveButton = new QPushButton( i18n("Remove"), mainWidget );
  connect( mRemoveButton, SIGNAL( clicked() ), SLOT( slotRemove() ) );
  mRemoveButton->setMaximumSize( mRemoveButton->sizeHint());

  QPushButton *upButton = new QPushButton(i18n("Up"),mainWidget);
  upButton->setMaximumSize( upButton->sizeHint());
  connect(upButton,SIGNAL(clicked()),SLOT(slotUp()));

  
  mEditor = new FilterEdit_base(mainWidget);

  QGridLayout *topLayout = new QGridLayout(mainWidget,2,2);
  topLayout->setSpacing(spacingHint());
  topLayout->addWidget(mSelectionCombo,0,0);
  topLayout->addWidget(upButton,0,1);
  topLayout->addWidget(addButton,0,2);
  topLayout->addWidget(mRemoveButton,0,3);
  topLayout->addMultiCellWidget(mEditor,1,1,0,3);
  mSelectionCombo->setMaximumWidth ( QApplication::desktop()->width() -
                                     addButton->maximumWidth() - 
                                     mRemoveButton->maximumWidth() - 
                                     upButton->maximumWidth() - 
                                     spacingHint() * 5 );
  connect(mEditor->mCatEditButton,SIGNAL(clicked()),
          SLOT(editCategorySelection()));

  // Clicking cancel exits the dialog without saving
  connect(this,SIGNAL(cancelClicked()),SLOT(reject()));
  updateFilterList();
}

FilterEditDialog::~FilterEditDialog()
{
}
void FilterEditDialog::slotUp()
{ 
    
    if ( mFilters->count() <= 1 ) return;
    if ( mSelectionCombo->currentItem()  == 0 ) return;
    int num = mSelectionCombo->currentItem();
    CalFilter* f = new CalFilter( );
    *f = *(mFilters->at( num ));
    mFilters->remove( num );
    mFilters->insert( num-1, f );
    updateFilterList();
    mSelectionCombo->setCurrentItem( num-1 );
    readFilter(f);
    //qApp->processEvents();
    //mSelectionCombo->popupBox();
}
void FilterEditDialog::updateFilterList()
{
  mSelectionCombo->clear();

  CalFilter *filter = mFilters->first();

  if (!filter) {
    enableButtonOK(false);
    enableButtonApply(false);
  } else {
    while(filter) {
      mSelectionCombo->insertItem(filter->name());
      filter = mFilters->next();
    }
    CalFilter *f = mFilters->at(mSelectionCombo->currentItem());
    if (f) readFilter(f);

    enableButtonOK(true);
    enableButtonApply(true);
  }

  mRemoveButton->setEnabled( mFilters->count() > 1 );
}

void FilterEditDialog::slotDefault()
{
}

void FilterEditDialog::slotApply()
{
  CalFilter *f = mFilters->at(mSelectionCombo->currentItem());
  writeFilter(f);
  emit filterChanged();
}
void FilterEditDialog::accept()
{
    slotOk();
}
void FilterEditDialog::slotOk()
{
  CalFilter *f = mFilters->at(mSelectionCombo->currentItem());
  writeFilter(f);
  emit filterChanged();
  QDialog::accept();
}

void FilterEditDialog::slotAdd()
{
  KLineEditDlg dlg(i18n("Enter filter name:"), "New Filter", this);
  dlg.setCaption(i18n("Add Filter"));
  if (dlg.exec()) {
      if ( dlg.text().length() > 0  ) {
          mFilters->append(new CalFilter(dlg.text()));
          updateFilterList();
          int num = mSelectionCombo->count() -1 ;
          mSelectionCombo->setCurrentItem( num );
          readFilter(mFilters->at( num ) );
      }
  }
}

void FilterEditDialog::slotRemove()
{
  int currentItem = mSelectionCombo->currentItem();
  if ( currentItem < 0 ) return;

  // We need at least a default filter object.
  if ( mFilters->count() <= 1 ) return;

  int result = KMessageBox::questionYesNo( this,
     i18n("This item will be\npermanently deleted.") );
  
  if ( result != KMessageBox::Yes ) {
    return;
  }
  
  mFilters->remove( currentItem );
  updateFilterList();
  emit filterChanged();
}

void FilterEditDialog::editCategorySelection()
{
  KPIM::CategorySelectDialog *dlg = new KPIM::CategorySelectDialog(
      KOPrefs::instance(), this, "filterCatSelect", true );
  dlg->setSelected(mCategories);

  connect(dlg,SIGNAL(categoriesSelected(const QStringList &)),
          SLOT(updateCategorySelection(const QStringList &)));

  dlg->exec();
}

void FilterEditDialog::updateCategorySelection(const QStringList &categories)
{
  mCategories = categories;

  mEditor->mCatList->clear();
  mEditor->mCatList->insertStringList(mCategories);
}

void FilterEditDialog::filterSelected()
{
  CalFilter *f = mFilters->at(mSelectionCombo->currentItem());
  if (f) readFilter(f);
}

void FilterEditDialog::readFilter(CalFilter *filter)
{
  int c = filter->criteria();

  mEditor->mCompletedCheck->setChecked(c & CalFilter::HideCompleted);
  mEditor->mRecurringCheck->setChecked(c & CalFilter::HideRecurring);
  mEditor->mPublicCheck->setChecked(c & CalFilter::ShowPublic);
  mEditor->mPrivateCheck->setChecked(c & CalFilter::ShowPrivate);
  mEditor->mConfidentialCheck->setChecked(c & CalFilter::ShowConfidential);

  mEditor->mEventCheck->setChecked(c & CalFilter::HideEvents);
  mEditor->mTodoCheck->setChecked(c & CalFilter::HideTodos);
  mEditor->mJournalCheck->setChecked(c & CalFilter::HideJournals);
 
  if (c & CalFilter::ShowCategories) {
    mEditor->mCatShowCheck->setChecked(true);
  } else {
    mEditor->mCatHideCheck->setChecked(true);
  }

  mEditor->mCatList->clear();
  mEditor->mCatList->insertStringList(filter->categoryList());
  mCategories = filter->categoryList();

  setCaption( i18n("Filter position: ") + QString::number ( mSelectionCombo->currentItem()+1 ));
}

void FilterEditDialog::writeFilter(CalFilter *filter)
{
  int c = 0;

  if (mEditor->mCompletedCheck->isChecked()) c |= CalFilter::HideCompleted;
  if (mEditor->mRecurringCheck->isChecked()) c |= CalFilter::HideRecurring;
  if (mEditor->mCatShowCheck->isChecked()) c |= CalFilter::ShowCategories;
  if (mEditor->mPublicCheck->isChecked()) c |= CalFilter::ShowPublic;
  if (mEditor->mPrivateCheck->isChecked()) c |= CalFilter::ShowPrivate;
  if (mEditor->mConfidentialCheck->isChecked()) c |= CalFilter::ShowConfidential;
  if (mEditor->mEventCheck->isChecked()) c |= CalFilter::HideEvents;
  if (mEditor->mTodoCheck->isChecked()) c |= CalFilter::HideTodos;
  if (mEditor->mJournalCheck->isChecked()) c |= CalFilter::HideJournals;

  filter->setCriteria(c);

  QStringList categoryList;
  for(uint i=0;i<mEditor->mCatList->count();++i) {
    categoryList.append(mEditor->mCatList->text(i));
  }
  filter->setCategoryList(categoryList);
}
