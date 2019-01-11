/*
  This file is part of KOrganizer.

  Copyright (c) 2001
  Cornelius Schumacher <schumacher@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <libkdepim/categoryeditdialog.h>

#include "calendarview.h"
#include "incomingdialog.h"
#include "outgoingdialog.h"
#include "koprefsdialog.h"
#include "koeventeditor.h"
#include "koprefs.h"
#include "datenavigator.h"
#include "kotodoeditor.h"
#include "searchdialog.h"
#include "filtereditdialog.h"
#ifndef KORG_NOPLUGINS
#include "plugindialog.h"
#endif
#ifndef KORG_NOARCHIVE
#include "archivedialog.h"
#endif

#include "kconfig.h"
#include "kodialogmanager.h"
#include <kapplication.h>

KODialogManager::KODialogManager( CalendarView *mainView ) :
  QObject(), mMainView( mainView )
{
  mOutgoingDialog = 0;
  mIncomingDialog = 0;
  mOptionsDialog = 0;
  mSearchDialog = 0;
  mArchiveDialog = 0;
  mFilterEditDialog = 0;
  mPluginDialog = 0;

  // mCategoryEditDialog = new KPIM::CategoryEditDialog(KOPrefs::instance(),mMainView);
  //KOGlobals::fitDialogToScreen( mCategoryEditDialog );
}

KODialogManager::~KODialogManager()
{
  delete mOutgoingDialog;
  delete mIncomingDialog;
  delete mOptionsDialog;
  delete mSearchDialog;
#ifndef KORG_NOARCHIVE
  delete mArchiveDialog;
#endif
  delete mFilterEditDialog;
#ifndef KORG_NOPLUGINS
  delete mPluginDialog;
#endif
}

OutgoingDialog *KODialogManager::outgoingDialog()
{
  createOutgoingDialog();
  return mOutgoingDialog;
}

void KODialogManager::createOutgoingDialog()
{
  if (!mOutgoingDialog) {
    mOutgoingDialog = new OutgoingDialog(mMainView->calendar(),mMainView);
    if (mIncomingDialog) mIncomingDialog->setOutgoingDialog(mOutgoingDialog);
    connect(mOutgoingDialog,SIGNAL(numMessagesChanged(int)),
            mMainView,SIGNAL(numOutgoingChanged(int)));
  }
}

void KODialogManager::showGlobalOptionsDialog( bool showSync )
{
    KPimPrefsGlobalDialog gc ( mMainView );
    if ( showSync )
        gc.showTZconfig();
    gc.exec();
}
void KODialogManager::showOptionsDialog( bool showSync )
{
   
  if (!mOptionsDialog) {
    mOptionsDialog = new KOPrefsDialog(mMainView);
    connect(mOptionsDialog,SIGNAL(configChanged()),
            mMainView,SLOT(updateConfig()));
  }
  mOptionsDialog->readConfig();
#ifndef DESKTOP_VERSION 
  mOptionsDialog->hideButtons();
  mOptionsDialog->showMaximized();
#else
  mOptionsDialog->show();
#endif
 
  mOptionsDialog->exec();
  delete mOptionsDialog;
  mOptionsDialog = 0;
}
void KODialogManager::showSyncOptions()
{
    showGlobalOptionsDialog( true );

}
void KODialogManager::showOutgoingDialog()
{
  createOutgoingDialog();
  mOutgoingDialog->show();
  mOutgoingDialog->raise();
}

IncomingDialog *KODialogManager::incomingDialog()
{
  createOutgoingDialog();
  if (!mIncomingDialog) {
    mIncomingDialog = new IncomingDialog(mMainView->calendar(),mOutgoingDialog,mMainView);
    connect(mIncomingDialog,SIGNAL(numMessagesChanged(int)),
            mMainView,SIGNAL(numIncomingChanged(int)));
    connect(mIncomingDialog,SIGNAL(calendarUpdated()),
            mMainView,SLOT(updateView()));
  }
  return mIncomingDialog;
}

void KODialogManager::createIncomingDialog()
{
  createOutgoingDialog();
  if (!mIncomingDialog) {
    mIncomingDialog = new IncomingDialog(mMainView->calendar(),mOutgoingDialog,mMainView);
    connect(mIncomingDialog,SIGNAL(numMessagesChanged(int)),
            mMainView,SIGNAL(numIncomingChanged(int)));
    connect(mIncomingDialog,SIGNAL(calendarUpdated()),
            mMainView,SLOT(updateView()));
  }
}

void KODialogManager::showIncomingDialog()
{
  createIncomingDialog();
  mIncomingDialog->show();
  mIncomingDialog->raise();
}
/*
void KODialogManager::showCategoryEditDialog()
{
  mCategoryEditDialog->show();
}
*/
void KODialogManager::hideSearchDialog()
{
  if (mSearchDialog) 
      mSearchDialog->hide();
}

void KODialogManager::showSearchDialog()
{
    if (!mSearchDialog) {
        mSearchDialog = new SearchDialog(mMainView->calendar(),mMainView); 
        KOListView * lview = mSearchDialog->listview();

        connect(lview, SIGNAL(showIncidenceSignal(Incidence *)),
                mMainView, SLOT(showIncidence(Incidence *)));
        connect(lview, SIGNAL(editIncidenceSignal(Incidence *)),
                mMainView, SLOT(editIncidence(Incidence *)));
        connect(lview, SIGNAL(deleteIncidenceSignal(Incidence *)),
                mMainView, SLOT(deleteIncidence(Incidence *)));
        connect(lview, SIGNAL(cloneIncidenceSignal(Incidence *)),
                mMainView, SLOT(cloneIncidence(Incidence *)));
        connect(lview, SIGNAL(beamIncidenceSignal(Incidence *)),
                mMainView, SLOT(beamIncidence(Incidence *)));
        connect(lview, SIGNAL(moveIncidenceSignal(Incidence *)),
                mMainView, SLOT(moveIncidence(Incidence *)));
        connect(lview, SIGNAL(beamIncidenceList(QPtrList<Incidence> )),
                mMainView, SLOT(beamIncidenceList(QPtrList<Incidence> )));

        connect(mMainView, SIGNAL(configChanged()), mSearchDialog, SLOT(updateConfig()));
        connect(mMainView, SIGNAL(updateSearchDialog()), mSearchDialog, SLOT(updateList()));
        //  connect( lview, SIGNAL( incidenceSelected( Incidence * ) ),
        //       mMainView, SLOT( processMainViewSelection( Incidence * ) ) );  
#ifndef DESKTOP_VERSION    
        mSearchDialog->setMaximumSize( QApplication::desktop()->size());
        mSearchDialog->showMaximized();
#else
        KConfig *config = KOGlobals::config();
        config->setGroup("WidgetLayout");
        QStringList list;
        list = config->readListEntry("SearchLayout");
        int x,y,w,h;
        if ( ! list.isEmpty() ) {
            x = list[0].toInt();
            y = list[1].toInt();
            w = list[2].toInt();
            h = list[3].toInt();
            KApplication::testCoords( &x,&y,&w,&h );
            mSearchDialog->setGeometry(x,y,w,h);
       
        }

#endif
    }
    // make sure the widget is on top again 
#ifdef DESKTOP_VERSION    
    mSearchDialog->show();
#else 
    mSearchDialog->setMaximumSize( QApplication::desktop()->size());
    mSearchDialog->showMaximized();
#endif
    mSearchDialog->raiseAndSelect();
}

SearchDialog * KODialogManager::getSearchDialog()
{
    return mSearchDialog;
}
void KODialogManager::showArchiveDialog()
{
#ifndef KORG_NOARCHIVE
  if (!mArchiveDialog) {
    mArchiveDialog = new ArchiveDialog(mMainView->calendar(),mMainView);
    connect(mArchiveDialog,SIGNAL(eventsDeleted()),
            mMainView,SLOT(updateView()));
  }
  mArchiveDialog->show();
  mArchiveDialog->raise();

  // Workaround.
  QApplication::restoreOverrideCursor();
#endif
}

void KODialogManager::showFilterEditDialog(QPtrList<CalFilter> *filters)
{
  if (!mFilterEditDialog) {
    mFilterEditDialog = new FilterEditDialog(filters,mMainView);
    connect(mFilterEditDialog,SIGNAL(filterChanged()),
            mMainView,SLOT(filterEdited()));
    
  }

#ifndef DESKTOP_VERSION  
  mFilterEditDialog->showMaximized();
#else
  mFilterEditDialog->show();
#endif
  mFilterEditDialog->raise();
}

void KODialogManager::showPluginDialog()
{
#ifndef KORG_NOPLUGINS
  if (!mPluginDialog) {
    mPluginDialog = new PluginDialog(mMainView);
    connect(mPluginDialog,SIGNAL(configChanged()),
            mMainView,SLOT(updateConfig()));
  }
  mPluginDialog->show();
  mPluginDialog->raise();
#endif
}

KOEventEditor *KODialogManager::getEventEditor()
{
  KOEventEditor *eventEditor = new KOEventEditor( mMainView->calendar(),
                                                  mMainView );

  connect(eventEditor,SIGNAL(eventAdded(Event *)),
          mMainView,SLOT(eventAdded(Event *)));
  connect(eventEditor,SIGNAL(eventChanged(Event *)),
          mMainView,SLOT(eventChanged(Event *)));
  connect(eventEditor,SIGNAL(eventDeleted()),
          mMainView,SLOT(eventDeleted()));
  connect(eventEditor,SIGNAL(deleteAttendee(Incidence *)),
          mMainView,SLOT(schedule_cancel(Incidence *)));
  connect( eventEditor, SIGNAL(jumpToTime( const QDate  &)), 
           mMainView->dateNavigator(), SLOT( selectWeek( const QDate & ) ) );
  connect( eventEditor, SIGNAL( showAgendaView( bool)), 
           mMainView->viewManager(), SLOT(  showAgendaView( bool) ) );

  //  connect(mCategoryEditDialog,SIGNAL(categoryConfigChanged()),
  //      eventEditor,SLOT(updateCategoryConfig()));
  // connect(eventEditor,SIGNAL(editCategories()),
  //      mCategoryEditDialog,SLOT(show()));

  //connect(mMainView,SIGNAL(closingDown()),eventEditor,SLOT(reject()));

#ifndef DESKTOP_VERSION
  eventEditor->resize( QApplication::desktop()->width() -20, 100 );
#endif
  return eventEditor;
}

KOTodoEditor *KODialogManager::getTodoEditor()
{
  KOTodoEditor *todoEditor = new KOTodoEditor( mMainView->calendar(),
                                               mMainView );

  // connect(mCategoryEditDialog,SIGNAL(categoryConfigChanged()),
  //      todoEditor,SLOT(updateCategoryConfig()));
  //  connect(todoEditor,SIGNAL(editCategories()),mCategoryEditDialog,SLOT(show()));

  connect(todoEditor,SIGNAL(todoAdded(Todo *)),
          mMainView,SLOT(todoAdded(Todo *)));
  connect(todoEditor,SIGNAL(todoChanged(Todo *)),
          mMainView,SLOT(todoChanged(Todo *)));
  connect(todoEditor,SIGNAL(todoDeleted()),
          mMainView,SLOT(todoDeleted()));
  connect( todoEditor, SIGNAL(jumpToTime( const QDate &)), 
           mMainView->dateNavigator(), SLOT( selectWeek( const QDate & ) ) );
  connect( todoEditor, SIGNAL( showAgendaView( bool)), 
           mMainView->viewManager(), SLOT(  showAgendaView( bool) ) );
  // connect(todoEditor,SIGNAL(deleteAttendee(Incidence *)),
  //      mMainView,SLOT(schedule_cancel(Incidence *)));
  //connect(mMainView,SIGNAL(closingDown()),todoEditor,SLOT(reject()));
#ifndef DESKTOP_VERSION
  todoEditor->resize( QApplication::desktop()->width() -20, 100 );
#endif
  return todoEditor;
}

void KODialogManager::updateSearchDialog()
{
  if (mSearchDialog) mSearchDialog->updateView();
}

void KODialogManager::setDocumentId( const QString &id )
{
  if (mOutgoingDialog) mOutgoingDialog->setDocumentId( id );
}

void KODialogManager::writeSettings( KConfig *config )
{
    if (mSearchDialog)
        mSearchDialog->listview()->writeSettings(config,"SearchListView Layout");
}
