/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <qtooltip.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qvgroupbox.h>
#include <qwidgetstack.h>
#include <qdatetime.h>
#include <qapplication.h>

#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <krestrictedline.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kdialog.h>

#include <libkcal/todo.h>

#include <libkdepim/kdateedit.h>

#include "koprefs.h"
#include "ktimeedit.h"

#include "koeditorgeneraltodo.h"
#include "kolocationbox.h"

KOEditorGeneralTodo::KOEditorGeneralTodo(QObject* parent,
                                         const char* name)
  : KOEditorGeneral( parent, name)
{
}

KOEditorGeneralTodo::~KOEditorGeneralTodo()
{
}

void KOEditorGeneralTodo::finishSetup()
{

//   QWidget::setTabOrder(mSummaryEdit, mLocationEdit);
//   QWidget::setTabOrder(mLocationEdit, mDueCheck);
//   QWidget::setTabOrder(mDueCheck, mDueDateEdit);
//   QWidget::setTabOrder(mDueDateEdit, mDueTimeEdit);
//   QWidget::setTabOrder(mDueTimeEdit, mStartCheck);
//   QWidget::setTabOrder(mStartCheck, mStartDateEdit);
//   QWidget::setTabOrder(mStartDateEdit, mStartTimeEdit);
//   QWidget::setTabOrder(mStartTimeEdit, mTimeButton);
//   QWidget::setTabOrder(mTimeButton, mCompletedCombo);
//   QWidget::setTabOrder(mCompletedCombo, mPriorityCombo);
//   QWidget::setTabOrder(mPriorityCombo, mAlarmButton);
//   QWidget::setTabOrder(mAlarmButton, mCategoriesButton);
//   QWidget::setTabOrder(mCategoriesButton, mSecrecyCombo);
//   QWidget::setTabOrder(mSecrecyCombo, mDescriptionEdit);
   mSummaryEdit->load(KOLocationBox::SUMMARYTODO);
  mSummaryEdit->setFocus();
}

void KOEditorGeneralTodo::initTime(QWidget *parent,QBoxLayout *topLayout)
{
  QBoxLayout *timeLayout = new QVBoxLayout(topLayout);

  QGroupBox *timeGroupBox = new QGroupBox(1,QGroupBox::Horizontal,
                                          i18n("Date && Time"),parent);
  timeLayout->addWidget(timeGroupBox);
  timeGroupBox->layout()->setSpacing( KDialog::spacingHint()-2 ); 
  timeGroupBox->layout()->setMargin(  KDialog::marginHint() ); 
  QFrame *timeBoxFrame = new QFrame(timeGroupBox);

  QGridLayout *layoutTimeBox = new QGridLayout(timeBoxFrame,3,3);
  layoutTimeBox->setSpacing(KDialog::spacingHintSmall());
  layoutTimeBox->setColStretch( 1, 1 );

  mStartCheck = new QCheckBox(i18n("Start:"),timeBoxFrame);
  layoutTimeBox->addWidget(mStartCheck,0,0);
  connect(mStartCheck,SIGNAL(toggled(bool)),SLOT(enableStartEdit(bool)));

  mStartDateEdit = new KDateEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mStartDateEdit,0,1);

  mStartTimeEdit = new KOTimeEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mStartTimeEdit,0,2);

  mDueCheck = new QCheckBox(i18n("Due:"),timeBoxFrame);
  layoutTimeBox->addWidget(mDueCheck,1,0);
  connect(mDueCheck,SIGNAL(toggled(bool)),SLOT(enableDueEdit(bool)));
  connect(mDueCheck,SIGNAL(toggled(bool)),SLOT(showAlarm()));
  
  
  mDueDateEdit = new KDateEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mDueDateEdit,1,1);

  mDueTimeEdit = new KOTimeEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mDueTimeEdit,1,2);


  mTimeButton = new QCheckBox(i18n("Time associated"),timeBoxFrame);
  layoutTimeBox->addMultiCellWidget(mTimeButton,2,2,0,1);

  connect(mTimeButton,SIGNAL(toggled(bool)),SLOT(enableTimeEdits(bool)));
  connect(mDueDateEdit,SIGNAL(setTimeTo(QTime)),mDueTimeEdit,SLOT(setTime(QTime)));
  connect(mStartDateEdit,SIGNAL(setTimeTo(QTime)),mStartTimeEdit,SLOT(setTime(QTime)));
  connect(mStartDateEdit, SIGNAL(dateChanged(QDate)),
	  this, SLOT(startDateChanged(QDate)));
  // some more layouting
  //layoutTimeBox->setColStretch(3,1);  
}


void KOEditorGeneralTodo::initCompletion(QWidget *parent, QBoxLayout *topLayout)
{
  mCompletedCombo = new QComboBox(parent);
  // xgettext:no-c-format
  mCompletedCombo->insertItem(i18n("  0 %"));
  // xgettext:no-c-format
  mCompletedCombo->insertItem(i18n(" 20 %"));
  // xgettext:no-c-format
  mCompletedCombo->insertItem(i18n(" 40 %"));
  // xgettext:no-c-format
  mCompletedCombo->insertItem(i18n(" 60 %"));
  // xgettext:no-c-format
  mCompletedCombo->insertItem(i18n(" 80 %"));
  // xgettext:no-c-format
  mCompletedCombo->insertItem(i18n("100 %"));
  connect(mCompletedCombo,SIGNAL(activated(int)),SLOT(completedChanged(int)));
  topLayout->addWidget(mCompletedCombo);

  mCompletedLabel = new QLabel(i18n("completed"),parent);
  topLayout->addWidget(mCompletedLabel); 

  mCompleteDateEdit = new KDateEdit(parent);
  topLayout->addWidget(mCompleteDateEdit ); 

  mCompleteTimeEdit = new KOTimeEdit(parent);
  topLayout->addWidget( mCompleteTimeEdit); 

  mCompletedCombo->setSizePolicy( QSizePolicy( QSizePolicy::Preferred,QSizePolicy::Preferred) );
  mCompletedLabel->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,QSizePolicy::Preferred) );
  connect(mCompleteDateEdit,SIGNAL(setTimeTo(QTime)),mCompleteTimeEdit,SLOT(setTime(QTime)));

  if (  QApplication::desktop()->width() <= 480 ) {
      if (  QApplication::desktop()->width() < 320 )
          mCompleteDateEdit->setMaximumWidth( 85 );
      else 
          mCompleteDateEdit->setMaximumWidth( 140 );
      topLayout->setSpacing( 0 );
  }
}

void KOEditorGeneralTodo::initPriority(QWidget *parent, QBoxLayout *topLayout)
{

    QHBox* h = new QHBox ( parent );
    topLayout->addWidget( h );
  QLabel *priorityLabel = new QLabel(i18n("Priority:"), h);
  // topLayout->addWidget(priorityLabel);
  mPriorityCombo = new QComboBox( h );
  mPriorityCombo->insertItem(i18n("1 (high)"));
  mPriorityCombo->insertItem(i18n("2"));
  mPriorityCombo->insertItem(i18n("3"));
  mPriorityCombo->insertItem(i18n("4"));
  mPriorityCombo->insertItem(i18n("5 (low)"));
  //topLayout->addWidget(mPriorityCombo);
}

void KOEditorGeneralTodo::initStatus(QWidget *parent,QBoxLayout *topLayout)
{
  QBoxLayout *statusLayout = new QHBoxLayout(topLayout);

  initCompletion( parent, statusLayout );

  statusLayout->addStretch( 1 );

  initPriority( parent, statusLayout );
}

void KOEditorGeneralTodo::setDefaults(QDateTime due,bool allDay)
{

  mSummaryEdit->load(KOLocationBox::SUMMARYTODO);
  mLocationEdit->load(KOLocationBox::LOCATION);
  KOEditorGeneral::setDefaults(allDay);

  mTimeButton->setChecked( !allDay );
  mTimeButton->setEnabled( !allDay );
  
  enableTimeEdits( !allDay );
  if ( due.isValid() ) {
      mDueCheck->setChecked(true);
      enableDueEdit(true);
      alarmDisable(false);
  } else {
      mDueCheck->setChecked(false);
      enableDueEdit(false);
      due = QDateTime::currentDateTime().addDays(7);
      alarmDisable(true);
  }

  
  mStartCheck->setChecked(false);
  enableStartEdit(false);

  mDueDateEdit->setDate(due.date());
  mDueTimeEdit->setTime(due.time());
  due = due.addDays(-7);
  mStartDateEdit->setDate(due.date());
  mStartTimeEdit->setTime(due.time());  

  mPriorityCombo->setCurrentItem(2);
  mCompletedLabel->setText(i18n(" completed"));;
  mCompletedCombo->setCurrentItem(0);
  mCompleteDateEdit->hide();
  mCompleteTimeEdit->hide();
}

void KOEditorGeneralTodo::readTodo(Todo *todo)
{

  mSummaryEdit->load(KOLocationBox::SUMMARYTODO);
  mLocationEdit->load(KOLocationBox::LOCATION);
  KOEditorGeneral::readIncidence(todo);

  QDateTime dueDT;
  mTimeButton->setChecked( !todo->doesFloat() );
  mTimeButton->setEnabled( !todo->doesFloat() );
  
  if (todo->hasDueDate()) {
    enableAlarmEdit(true);
    dueDT = todo->dtDue();
    mDueDateEdit->setDate(todo->dtDue().date());
    mDueTimeEdit->setTime(todo->dtDue().time());
    mDueCheck->setChecked(true);
    mTimeButton->setEnabled( true );
  } else {
    alarmDisable(true);
    mDueDateEdit->setEnabled(false);
    mDueTimeEdit->setEnabled(false);
    mDueDateEdit->setDate(QDate::currentDate());
    mDueTimeEdit->setTime(QTime::currentTime());
    mDueCheck->setChecked(false);
  }

  if (todo->hasStartDate()) {
    mStartDateEdit->setDate(todo->dtStart().date());
    mStartTimeEdit->setTime(todo->dtStart().time());
    mStartCheck->setChecked(true);
    mTimeButton->setEnabled( true );
  } else {
    mStartDateEdit->setEnabled(false);
    mStartTimeEdit->setEnabled(false);
    mStartDateEdit->setDate(QDate::currentDate());
    mStartTimeEdit->setTime(QTime::currentTime());
    mStartCheck->setChecked(false);
  }


  mCompletedCombo->setCurrentItem(todo->percentComplete() / 20);
  if (todo->isCompleted() && todo->hasCompletedDate()) {
    mCompleted = todo->completed();
  }
  setCompletedDate();

  mPriorityCombo->setCurrentItem(todo->priority()-1);
}

void KOEditorGeneralTodo::writeTodo(Todo *todo)
{
  KOEditorGeneral::writeIncidence(todo);

  // temp. until something better happens.
  QString tmpStr;
  
  todo->setHasDueDate(mDueCheck->isChecked());
  todo->setHasStartDate(mStartCheck->isChecked());

  QDate tmpDate;
  QTime tmpTime;
  QDateTime tmpDT;
  if ( mTimeButton->isChecked() ) {
    todo->setFloats(false);
    
    // set due date/time
    tmpDate = mDueDateEdit->date();
    tmpTime = mDueTimeEdit->getTime();
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    todo->setDtDue(tmpDT);

    // set start date/time
    tmpDate = mStartDateEdit->date();
    tmpTime = mStartTimeEdit->getTime();
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    todo->setDtStart(tmpDT);
  } else {
    todo->setFloats(true);

    // need to change this.
    tmpDate = mDueDateEdit->date();
    tmpTime.setHMS(0,0,0);
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    todo->setDtDue(tmpDT);
    
    tmpDate = mStartDateEdit->date();
    tmpTime.setHMS(0,0,0);
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    todo->setDtStart(tmpDT);
  }
  todo->setPriority(mPriorityCombo->currentItem()+1);

  // set completion state
  if ( mCompletedCombo->currentItem() == 5 ) {
      QDateTime comp ( mCompleteDateEdit->date(), mCompleteTimeEdit->getTime() );
      if ( comp.isValid () ) {
          todo->setPercentComplete(100);
          todo->setCompleted(comp);
      } else {
          todo->setPercentComplete( 100 );
          if ( mCompleted.isValid() )
              todo->setCompleted(mCompleted);
      }
  } else {
      todo->setPercentComplete(mCompletedCombo->currentItem() * 20);
  }

  mSummaryEdit->save(KOLocationBox::SUMMARYTODO);
}

void KOEditorGeneralTodo::enableDueEdit(bool enable)
{
  mDueDateEdit->setEnabled( enable );

  if(mDueCheck->isChecked() || mStartCheck->isChecked()) {
    mTimeButton->setEnabled(true);
  }
  else {
    mTimeButton->setEnabled(false);
    mTimeButton->setChecked(false);
  }
  
  if (enable) {
    mDueTimeEdit->setEnabled( mTimeButton->isChecked() );
  } else {
    mDueTimeEdit->setEnabled( false );
  }
  emit datesChecked();
}

void KOEditorGeneralTodo::enableStartEdit( bool enable )
{
  mStartDateEdit->setEnabled( enable );

  if(mDueCheck->isChecked() || mStartCheck->isChecked()) {
    mTimeButton->setEnabled(true);
  }
  else {
    mTimeButton->setEnabled(false);
    mTimeButton->setChecked(false);
  }

  if (enable) {
    mStartTimeEdit->setEnabled( mTimeButton->isChecked() );
  } else {
    mStartTimeEdit->setEnabled( false );
  }
  emit datesChecked();
}
void KOEditorGeneralTodo::startDateChanged(QDate newdate)
{
    if ( mDueCheck->isChecked() )
        emit dateTimesChanged(newdate,mDueDateEdit->date());
}
void KOEditorGeneralTodo::enableTimeEdits(bool enable)
{
  if(mStartCheck->isChecked()) {
    mStartTimeEdit->setEnabled( enable );
  }
  if(mDueCheck->isChecked()) {
    mDueTimeEdit->setEnabled( enable );
  }
}

void KOEditorGeneralTodo::showAlarm()
{
  if ( mDueCheck->isChecked() ) {
    alarmDisable(false);
  }
  else {
    alarmDisable(true);
  }
}

bool KOEditorGeneralTodo::validateInput()
{
  if (mDueCheck->isChecked()) {
    if (!mDueDateEdit->inputIsValid()) {
      KMessageBox::sorry(0,i18n("Please specify a valid due date."));
      return false;
    }
  }

  if (mStartCheck->isChecked()) {
    if (!mStartDateEdit->inputIsValid()) {
      KMessageBox::sorry(0,i18n("Please specify a valid start date."));
      return false;
    }
  }

  if (mStartCheck->isChecked() && mDueCheck->isChecked()) {
    QDateTime startDate;
    QDateTime dueDate;
    startDate.setDate(mStartDateEdit->date());
    dueDate.setDate(mDueDateEdit->date());
    if (mTimeButton->isChecked()) {
      startDate.setTime(mStartTimeEdit->getTime());
      dueDate.setTime(mDueTimeEdit->getTime());
    }
    if (startDate > dueDate) {
      KMessageBox::sorry(0,
                         i18n("The start date cannot be after the due date."));
      return false;
    }
  }

  return KOEditorGeneral::validateInput();
}

void KOEditorGeneralTodo::completedChanged(int index)
{
  if (index == 5) {
      //get rid of milli sec
      mCompleted = QDateTime::currentDateTime();
  }
  setCompletedDate();
}

void KOEditorGeneralTodo::setCompletedDate()
{
  if (mCompletedCombo->currentItem() == 5 && mCompleted.isValid()) {
      if (  QApplication::desktop()->width() < 480 ) {
          mCompletedLabel->setText(i18n(" on"));
      }
      else
          mCompletedLabel->setText(i18n(" completed on "));
    mCompleteDateEdit->show();
    mCompleteTimeEdit->show();
    mCompleteTimeEdit->setTime( mCompleted.time() );
    mCompleteDateEdit->setDate( mCompleted.date() );
  } else {
    mCompletedLabel->setText(i18n(" completed"));
    mCompleteDateEdit->hide();
    mCompleteTimeEdit->hide();
  }
}

void KOEditorGeneralTodo::modified (Todo* todo, int modification) 
{
  switch (modification) {
  case KOGlobals::PRIORITY_MODIFIED:
    mPriorityCombo->setCurrentItem(todo->priority()-1);
    break;
  case KOGlobals::COMPLETION_MODIFIED:
    mCompletedCombo->setCurrentItem(todo->percentComplete() / 20);
    if (todo->isCompleted() && todo->hasCompletedDate()) {
      mCompleted = todo->completed();
    }
    setCompletedDate();
    break;
  case KOGlobals::CATEGORY_MODIFIED:
    setCategories (todo->categoriesStr ());
    break;
  case KOGlobals::UNKNOWN_MODIFIED: // fall through
  default:
    readTodo( todo );  
    break;
  }
}
