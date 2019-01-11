/*
    This file is part of KOrganizer.
    Copyright (c) 1997, 1998 Preston Brown
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
#include <qframe.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qtimer.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qtabwidget.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include <libkdepim/categoryselectdialog.h>
#include <libkcal/calendarlocal.h>
#include <libkcal/calendarresources.h>
#include <libkcal/resourcecalendar.h>
#include <libkcal/icalformat.h>
#include <kresources/resourceselectdialog.h>
#include <libkdepim/kdateedit.h>

#include "koprefs.h"
#include "kolocationbox.h"

#include "kotodoeditor.h"
extern int globalFlagBlockAgenda;

KOTodoEditor::KOTodoEditor( Calendar *calendar, QWidget *parent ) :
  KOIncidenceEditor( i18n("Edit To-Do"), calendar, parent )
{
  mTodo = 0;
  mRelatedTodo = 0;
  findButton(User1)->hide();
  init();
  if ( QApplication::desktop()->height() <= 240 )
      hideButtons();
}

KOTodoEditor::~KOTodoEditor()
{
  emit dialogClose( mTodo );
}

void KOTodoEditor::init()
{
  setupGeneral();
  setupAttendeesTab();
  setupRecurrence();
  connect(mGeneral,SIGNAL(datesChecked()),this ,SLOT(checkRecurrence()));
  mRecurrence->setDateTimeStr( i18n("<i>The recurrence is computed from the start datetime!</i>")  );
  connect(mGeneral,SIGNAL(dateTimesChanged(QDateTime,QDateTime)),
          mRecurrence,SLOT(setDefaultsDates(QDateTime,QDateTime)));
}
void KOTodoEditor::setupRecurrence()
{
  QFrame *topFrame = addPage( i18n("Recurrence") );
  QBoxLayout *topLayout = new QVBoxLayout( topFrame );

  mRecurrence = new KOEditorRecurrence( topFrame );
  topLayout->addWidget( mRecurrence );
}

void  KOTodoEditor::setCategories( QString s )
{
   mGeneral->setCategories(s);
}
void KOTodoEditor::setSecrecy( int sec )
{
    mGeneral->setSecrecy( sec );
}
void KOTodoEditor::reload()
{
  if ( mTodo ) readTodo( mTodo );
}

void KOTodoEditor::setupGeneral()
{
  mGeneral = new KOEditorGeneralTodo(this);
  connect ( mGeneral, SIGNAL ( allAccepted() ), this, SLOT ( slotOk () ) );

  // connect(mGeneral,SIGNAL(openCategoryDialog()),mCategoryDialog,SLOT(show()));
  //connect(mCategoryDialog, SIGNAL(categoriesSelected(const QString &)),
  //      mGeneral,SLOT(setCategories(const QString &)));

  if (KOPrefs::instance()->mCompactDialogs) {
    QFrame *topFrame = addPage(i18n("General"));

    QBoxLayout *topLayout = new QVBoxLayout(topFrame);  
    if ( QApplication::desktop()->width() < 480 ) {
        topLayout->setMargin(marginHintSmall());
        topLayout->setSpacing(spacingHintSmall());
    } else {
        topLayout->setMargin(marginHint());
        topLayout->setSpacing(spacingHint());
    }
    topLayout->addStretch( 1 );
   mGeneral->initHeader(topFrame,topLayout);
    topLayout->addStretch( 1 );
    mGeneral->initTime(topFrame,topLayout); 
    topLayout->addStretch( 1 );
   mGeneral->initAlarm(topFrame,topLayout);
    topLayout->addStretch( 1 );
    mGeneral->enableAlarm( false );
    
  
    QBoxLayout *priorityLayout;
    if ( QApplication::desktop()->width() < 500 ) 
        priorityLayout = new QVBoxLayout( topLayout );
    else
        priorityLayout = new QHBoxLayout( topLayout );
    QWidget* prioWidget = new QWidget (topFrame);
    priorityLayout->addWidget(  prioWidget );
    QHBoxLayout* priorityLayout2 = new QHBoxLayout( prioWidget);


    QIconSet icon; 
    if ( QApplication::desktop()->width() < 321 ) 
        icon = SmallIcon("fileimport16");
    else
        icon = SmallIcon("fileimport");
    QPushButton * loadTemplate = new QPushButton(  prioWidget);
    loadTemplate->setIconSet (icon ) ;
    int size = loadTemplate->sizeHint().height();
    loadTemplate->setFixedSize(  size, size );
    if ( QApplication::desktop()->width() < 321 ) 
        icon = SmallIcon("fileexport16");
    else
        icon = SmallIcon("fileexport");
    QPushButton * saveTemplate = new QPushButton( prioWidget);
    saveTemplate->setIconSet (icon ) ;
    saveTemplate->setFixedSize(  size, size );

    priorityLayout2->addWidget(loadTemplate);
    priorityLayout2->addWidget(saveTemplate);
    mGeneral->initPriority(prioWidget,priorityLayout2);
    mGeneral->initCategories( topFrame, priorityLayout );
    topLayout->addStretch(1);

    QFrame *topFrame2 = addPage(i18n("Details"));

    QBoxLayout *topLayout2 = new QVBoxLayout(topFrame2);
    topLayout2->setMargin(marginHint());
    topLayout2->setSpacing(spacingHint());

    QHBoxLayout *completionLayout = new QHBoxLayout( topLayout2 );
    mGeneral->initCompletion(topFrame2,completionLayout);
    
 
    mGeneral->initSecrecy( topFrame2, topLayout2 );
    mGeneral->initDescription(topFrame2,topLayout2); 

    // QHBox * hb = new QHBox ( topFrame2 );
    // topLayout2->addWidget(hb);
    // hb->setSpacing( 3 );
   
    connect( saveTemplate, SIGNAL( clicked() ), this , SLOT( slotSaveTemplate() ) );
    connect( loadTemplate, SIGNAL( clicked() ), this , SLOT( slotLoadTemplate() ) );

  } else {
    QFrame *topFrame = addPage(i18n("General"));
    
    QBoxLayout *topLayout = new QVBoxLayout(topFrame);
    topLayout->setSpacing(spacingHint());

    mGeneral->initHeader(topFrame,topLayout);
    mGeneral->initTime(topFrame,topLayout);
    mGeneral->initStatus(topFrame,topLayout);    
    QBoxLayout *alarmLineLayout = new QHBoxLayout(topLayout);
    mGeneral->initAlarm(topFrame,alarmLineLayout); 
    mGeneral->initDescription(topFrame,topLayout);
    QBoxLayout *detailsLayout = new QHBoxLayout(topLayout);
    mGeneral->initCategories( topFrame, detailsLayout );
    mGeneral->initSecrecy( topFrame, detailsLayout );
  }
  mGeneral->finishSetup();
  
}

void KOTodoEditor::editTodo(Todo *todo, bool editDescription)
{
    //init();

  mTodo = todo;
  readTodo(mTodo);
  if ( editDescription ) {
      showPage( 1 );
      mGeneral->setFocusOn( 1 );
  } else {
      showPage( 0 );
      mGeneral->setFocusOn( 2 );
  }
  checkRecurrence();
}

void KOTodoEditor::newTodo(QDateTime due,Todo *relatedTodo,bool allDay)
{
    //init();

  mTodo = 0;
  setDefaults(due,relatedTodo,allDay);
}

void KOTodoEditor::loadDefaults()
{
  setDefaults(QDateTime::currentDateTime().addDays(7),0,false);
}

bool KOTodoEditor::processInput( bool emitTime )
{
  if (!validateInput()) return false;

  Todo *todo = 0;

  if (mTodo) todo = mTodo;
  else {
    todo = new Todo;
    todo->setOrganizer(KOPrefs::instance()->email());
  }

  writeTodo(todo);
  if ( emitTime ) {
      globalFlagBlockAgenda = 1;
      emit showAgendaView( false );
      if ( todo->hasDueDate() )
          emit jumpToTime( todo->dtDue().date() );
      globalFlagBlockAgenda = 2;
  }
  if (mTodo) {
    todo->setRevision(todo->revision()+1);
    emit todoChanged(todo);
  } else {
    mCalendar->addTodo(todo);
    mTodo = todo;
    emit todoAdded(todo);
    if ( todo->isAlarmEnabled () && !todo->alarmEnabled () ) {
        QTimer::singleShot( 0, this, SLOT ( alarmWarning() ) );
    }
  }
 
  return true;
}

void KOTodoEditor::deleteTodo()
{
  if (mTodo) {
    if (KOPrefs::instance()->mConfirm) {
      switch (msgItemDelete()) {
        case KMessageBox::Continue: // OK
          emit todoToBeDeleted(mTodo);
          emit dialogClose(mTodo);
          mCalendar->deleteTodo(mTodo);
          emit todoDeleted();
          reject();
          break;
      }
    }
    else {
      emit todoToBeDeleted(mTodo);
      emit dialogClose(mTodo);
      mCalendar->deleteTodo(mTodo);
      emit todoDeleted();
      reject();
    }
  } else {
    reject();
  }
}

void KOTodoEditor::setDefaults(QDateTime due,Todo *relatedEvent,bool allDay)
{
  mRelatedTodo = relatedEvent;
      
  mGeneral->setDefaults(due,allDay);
  mDetails->setDefaults(); 
  showPage( 0 );
  if ( mRelatedTodo ) {
      mGeneral->fillCalCombo(mRelatedTodo->calID() );
      mGeneral->setCategories (mRelatedTodo->categoriesStr ());
      mGeneral->setSecrecy (mRelatedTodo->secrecy ());
      if ( mRelatedTodo->priority() < 3 )
          mGeneral->mPriorityCombo->setCurrentItem(mRelatedTodo->priority()-1);
      mGeneral->mSummaryEdit->lineEdit()->setText(mRelatedTodo->summary()+": ");
      int len = mRelatedTodo->summary().length();
      mGeneral->mSummaryEdit->lineEdit()->setFocus();
      mGeneral->mSummaryEdit->lineEdit()->setCursorPosition ( len+2 ); 
      mGeneral->mSummaryEdit->lineEdit()->setSelection ( 0, len+2 );
      
  } else
      mGeneral->setFocusOn( 2 );
  tabWidget()->setTabEnabled ( mRecurrence->parentWidget(), false );
  mRecurrence->setDefaults(QDateTime::currentDateTime(),QDateTime::currentDateTime().addSecs( 3600 ));
}
void KOTodoEditor::checkRecurrence()
{
    if ( mGeneral->mDueCheck->isChecked() && mGeneral->mStartCheck->isChecked()) {
        tabWidget()->setTabEnabled ( mRecurrence->parentWidget(), true );

        if ( mTodo )
            mRecurrence->readEvent( mTodo );
        else {
            bool time = mGeneral->mTimeButton->isChecked();
            QDateTime from,to;
            if ( time ) {
                to = QDateTime( mGeneral->mDueDateEdit->date(), mGeneral->mDueTimeEdit->getTime() ) ;
                from = QDateTime( mGeneral->mStartDateEdit->date(),mGeneral->mStartTimeEdit->getTime( )) ;
            } else {
                to = QDateTime( mGeneral->mDueDateEdit->date(), QTime( 0,0,0) ) ;
                from = QDateTime( mGeneral->mStartDateEdit->date(),QTime( 0,0,0) ) ;
            }
            if ( to < from )
                to = from;
            mRecurrence->setDefaults(from,to);
        }
    } else {
        tabWidget()->setTabEnabled ( mRecurrence->parentWidget(), false );
        mRecurrence->setDefaults(QDateTime::currentDateTime(),QDateTime::currentDateTime().addSecs( 3600 ));
    }
}
void KOTodoEditor::readTodo(Todo *todo)
{
  mGeneral->readTodo(todo);
  mDetails->readEvent(todo);
  mRelatedTodo = 0;//todo->relatedTo();
  // categories
  // mCategoryDialog->setSelected(todo->categories());

  // We should handle read-only events here.
}

void KOTodoEditor::writeTodo(Todo *event)
{
    bool maybeComputeRecurrenceTime = false;
    if( event->hasRecurrenceID() && event->percentComplete() < 100)
        maybeComputeRecurrenceTime = true;
    event->setHasRecurrenceID( false );
    mGeneral->writeTodo(event);
    mDetails->writeEvent(event);

    // set related event, i.e. parent to-do in this case.
    if (mRelatedTodo) {
        event->setRelatedTo(mRelatedTodo);
    }  
    if ( mGeneral->mDueCheck->isChecked() && mGeneral->mStartCheck->isChecked()) {
        mRecurrence->writeEvent(event);
        if ( event->doesRecur() ) {
            int addSec = -1 ;
            if ( maybeComputeRecurrenceTime && event->percentComplete() == 100 )
                addSec = 1;
            event->setRecurrenceID( event->dtStart().addSecs( addSec ) );
            event->setRecurDates();
        } else {
            event->setHasRecurrenceID( false );
        }
    } else {
        event->setHasRecurrenceID( false );
        if ( event->doesRecur() )
            event->recurrence()->unsetRecurs();
    }
}

bool KOTodoEditor::validateInput()
{
  if (!mGeneral->validateInput()) return false;
  if (!mDetails->validateInput()) return false;
  return true;
}

int KOTodoEditor::msgItemDelete()
{
  return KMessageBox::warningContinueCancel(this,
      i18n("This item will be permanently deleted."),
      i18n("KOrganizer Confirmation"),i18n("Delete"));
}

void KOTodoEditor::modified (int modification)
{
  if (modification == KOGlobals::CATEGORY_MODIFIED || 
      KOGlobals::UNKNOWN_MODIFIED == modification )
      //  mCategoryDialog->setSelected (mTodo->categories ());
  mGeneral->modified (mTodo, modification);

}

void KOTodoEditor::slotLoadTemplate()
{

  QString fileName =locateLocal( "templates", "todos" ); 
  QDir t_dir;
  if ( !t_dir.exists(fileName) )
      t_dir.mkdir ( fileName );
  fileName += "/todo";
  fileName = KFileDialog::getSaveFileName(  fileName , "Load Todo template", this );
    if ( fileName.length() == 0 )
        return;
    CalendarLocal cal;
    ICalFormat format;
    if ( !format.load( &cal, fileName ) ) {
        KMessageBox::error( this, i18n("Error loading template file\n '%1'.")
                            .arg( fileName ) );
        return ;
    }
    QPtrList<Todo> todos = cal.todos();
    Todo * todo = todos.first();
    if ( !todo ) {
        KMessageBox::error( this,
                            i18n("Template does not\ncontain a valid Todo."));
    } else {
        readTodo( todo );
    }
  
}

void KOTodoEditor::slotSaveTemplate()
{
    QString fileName =locateLocal( "templates", "todos" ); 
    QDir t_dir;
    if ( !t_dir.exists(fileName) )
        t_dir.mkdir ( fileName );
    fileName += "/todo";
    fileName = KFileDialog::getSaveFileName(  fileName , "Save as Todo template", this );
    if ( fileName.length() > 0 )
        saveTemplate( fileName );
}

void KOTodoEditor::saveTemplate( const QString &templateName )
{
  Todo *todo = new Todo;
  writeTodo( todo );
  saveAsTemplate( todo, templateName );
}
