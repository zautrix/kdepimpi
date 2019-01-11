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
*/

#include <klocale.h>

#include <libkcal/event.h>
#include <qtimer.h>
#include <qpushbutton.h>

#include "koeventviewer.h"
#include <kmessagebox.h>
#include "koprefs.h"
#include <libkcal/todo.h>
#include "qapp.h"

#include "koeventviewerdialog.h"
extern int globalFlagBlockAgenda;

KOEventViewerDialog::KOEventViewerDialog(QWidget *parent,const char *name)
  : KDialogBase(parent,name,
#ifndef DESKTOP_VERSION
                true ,
#else
                false,
#endif
                i18n("Event Viewer"),Ok|User1|Close,Close, false,  i18n("Agenda"))
{
    sendSignalViewerClosed = true;
  mEventViewer = new KOEventViewer(this);
  mEventViewer->setFont( KOPrefs::instance()->mEventViewFont );
  setMainWidget(mEventViewer);
  setButtonText(Ok, i18n("Edit") );

 QObject::connect(findButton( Ok ),SIGNAL(clicked()),
                   SLOT(editIncidence())); 
 QObject::connect(this,SIGNAL(user1Clicked()),
                   SLOT(showIncidence())); 
 connect( mEventViewer, SIGNAL( showIncidence( QString ) ),SIGNAL( showIncidence( QString ) ));
 mIncidence = 0;
  // TODO: Set a sensible size (based on the content?).
  //showMaximized();
  //qDebug("++++++++++++KOEventViewerDialog() ");
 //  if ( KOPrefs::instance()->mCompactDialogs ) {
//     setFixedSize( 240,284 );
//     move( 0, 15 );
//   } else {
//     setMinimumSize(300,200);
//     resize(320,300);
//   }
 mSyncMode = false;
 mSyncResult = 0;

}

KOEventViewerDialog::~KOEventViewerDialog()
{
    //qDebug("-------~KOEventViewerDialog() ");
}
void KOEventViewerDialog::showMe()
{
 
#ifdef DESKTOP_VERSION
    int x,y,w,h;
    x = geometry().x();
    y = geometry().y();
    w = width();
    h = height();
    show();
    setGeometry(x,y,w,h);
    raise();
#else
    showMaximized();
#endif
   QTimer::singleShot( 1, this, SLOT ( setMyFocus() ) );

}
void KOEventViewerDialog::setMyFocus()
{
   
    setActiveWindow();
    mEventViewer->setFocus();  
    
}
void KOEventViewerDialog::print()
{
    mEventViewer->printMe();

}
void KOEventViewerDialog::setSyncMode( bool b )
{
  mSyncMode = b;
  //qDebug("KOEventViewerDialog::setSyncMode %d ",mSyncMode );
  if ( mSyncMode ) {
    findButton( Close )->setText( i18n("Cancel Sync"));
    findButton( Ok )->setText( i18n("Remote"));
    findButton( User1 )->setText( i18n("Local"));
  } else {
    findButton( Close )->setText( i18n("Close"));
    findButton( Ok )->setText( i18n("Edit"));
    findButton( User1 )->setText( i18n("Agenda"));
  }
  mEventViewer->setSyncMode( b );
}
void KOEventViewerDialog::setColorMode( int m )
{
    mEventViewer->setColorMode( m );
}
int KOEventViewerDialog::executeS( bool local )
{
    mSyncResult = 3; 
    if ( local ) 
        findButton( User1 )->setFocus();
    else 
        findButton( Ok )->setFocus();
    exec();
    return mSyncResult;
}

void KOEventViewerDialog::updateConfig()
{
  mEventViewer->setFont( KOPrefs::instance()->mEventViewFont );

}
void KOEventViewerDialog::setEvent(Event *event)
{
  mEventViewer->setEvent(event);
  mIncidence = event; 
  mEventViewer->setFocus(); 
  //findButton( Close )->setFocus();
  if ( !mSyncMode ) {
    findButton( User1 )->setText( i18n("Agenda"));
  }
}
void KOEventViewerDialog::setIncidence(Incidence *in )
{
    if ( in->typeID() == eventID )
        setEvent( (Event*) in );
    else if ( in->typeID() == todoID )
        setTodo( (Todo*) in );
    else if ( in->typeID() == journalID )
        setJournal( (Journal*) in );
}
void KOEventViewerDialog::addIncidence(Incidence *in)
{
    if ( in->typeID() == eventID )
        addEvent( (Event*) in );
    else if ( in->typeID() == todoID )
         mEventViewer->setTodo( (Todo*) in, false );
    else if ( in->typeID() == journalID )
         mEventViewer->setJournal( (Journal*) in, false );
  if ( mSyncMode ) {
      findButton( User1 )->setFocus();
      setCaption(i18n("Conflict! Please choose entry"));
  }   
}

void KOEventViewerDialog::addEvent(Event *event)
{ 
  mEventViewer->addEvent(event);
  mIncidence = event; 
  mEventViewer->setFocus(); 
  //findButton( Close )->setFocus(); 
  if ( !mSyncMode ) {
    findButton( User1 )->setText( i18n("Agenda"));
  }
}

void KOEventViewerDialog::setTodo(Todo *event)
{
  mEventViewer->setTodo(event);
  mIncidence = (Incidence*)event; 
  mEventViewer->setFocus(); 
  //findButton( Close )->setFocus(); 
  if ( !mSyncMode ) {
    findButton( User1 )->setText( i18n("Set complete"));
  }
}
void KOEventViewerDialog::setJournal(Journal *j)
{
  mEventViewer->setJournal(j);
  mIncidence = (Incidence*)j; 
  mEventViewer->setFocus(); 
  //findButton( Close )->setFocus();
  if ( !mSyncMode ) {
    findButton( User1 )->setText( i18n("Agenda"));
  }
}

void KOEventViewerDialog::addText(QString text)
{
  mEventViewer->addText(text); 
  mEventViewer->setFocus(); 
  //findButton( Close )->setFocus();
}
void KOEventViewerDialog::editIncidence()
{   
    sendSignalViewerClosed = false;
    if ( mSyncMode ) {
        mSyncResult = 2;
        accept();
        return;
    }
    if ( mIncidence ){
        close();
        emit editIncidence( mIncidence );
    }
}
void KOEventViewerDialog::showIncidence()
{
    sendSignalViewerClosed = false;
    if ( mSyncMode ) {
        mSyncResult = 1;
        accept();
        return;
    }

    if ( mIncidence ){
        QDate date;
        if ( mIncidence->type() == "Todo" ) {
            /*
            if ( ((Todo*)mIncidence)->hasDueDate() )
                date = ((Todo*)mIncidence)->dtDue().date();
            else {
                globalFlagBlockAgenda = 2;
                emit showAgendaView( false );
                return;
            }
            */
            ((Todo*)mIncidence)->setCompleted( true );
            ((Todo*)mIncidence)->setCompleted(QDateTime::currentDateTime()  );
            emit todoCompleted(((Todo*)mIncidence)); 
            mEventViewer->setTodo((Todo*)mIncidence);
            return;
        } else {
            date = mIncidence->dtStart().date();
            globalFlagBlockAgenda = 1;
            emit showAgendaView( false );
            globalFlagBlockAgenda = 2;
            emit jumpToTime( date );
        }
        close();
    }
}
void KOEventViewerDialog::keyPressEvent ( QKeyEvent * e ) 
{
    switch ( e->key() ) {
   
    case Qt::Key_A :
    case Qt::Key_L :
        showIncidence();
        break;
    case Qt::Key_E :
    case Qt::Key_R :
        editIncidence();
        break;
    case Qt::Key_C:
    case Qt::Key_Escape:
        sendSignalViewerClosed = true;
        close();
        break; 
    case Qt::Key_I:
#ifndef DESKTOP_VERSION
        sendSignalViewerClosed = true;
        close();
#else
        sendSignalViewerClosed = true;
        slotViewerClosed();
        //accept();
#endif
        break; 
    default:
        KDialogBase::keyPressEvent ( e );
        break; 
    }
    
}
void KOEventViewerDialog::hideEvent ( QHideEvent * e )
{ 
    KDialogBase::hideEvent ( e );
    QTimer::singleShot( 1, this, SLOT (slotViewerClosed() ) );
}

void KOEventViewerDialog::slotViewerClosed()
{
    if ( mSyncMode )
        return;
                   
    if ( sendSignalViewerClosed  ) {
        //qDebug("KOEventViewerDialog::hideEvent ");
        emit signalViewerClosed(); 
    }
    sendSignalViewerClosed = true;
}
        
