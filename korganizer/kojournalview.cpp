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

//
// View of Journal entries

#include <qlayout.h>
#include <qscrollview.h>
#include <qpopupmenu.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include <klocale.h>
#include <kdebug.h>
#include "koprefs.h"
#include <kglobal.h>
#include <ktextedit.h>

#include <libkcal/calendar.h>

#include "journalentry.h"

#include "kojournalview.h"
using namespace KOrg;

KOJournalView::KOJournalView(Calendar *calendar, QWidget *parent,
		       const char *name)
  : KOrg::BaseView(calendar, parent, name)
{
    mCalendar = calendar;
    QHBox * vb = new QHBox ( this );
    QPushButton * newJournal = new QPushButton( vb );
    QPixmap icon;
    if ( QApplication::desktop()->width() < 321 ) 
        icon = SmallIcon("ko16old");
    else
        icon = SmallIcon("ko24old");
    newJournal->setPixmap (icon ) ;
    int size = newJournal->sizeHint().height();
    newJournal->setFixedSize(  size, size );
    mDateLabel = new QLabel ( vb );
    mDateLabel->setMargin(1);
    mDateLabel->setAlignment(AlignCenter);
    QScrollView * sv = new QScrollView( this );
    QVBoxLayout * hbl = new QVBoxLayout( this );
    hbl->addWidget( vb );
    hbl->addWidget( sv );
    parWid = new QWidget( sv->viewport() );
    sv->addChild(parWid);
    sv->setResizePolicy( QScrollView:: AutoOneFit );
    mTopLayout = new QVBoxLayout(parWid);
    connect( newJournal, SIGNAL( clicked() ), this , SLOT( newJournal() ) );
    getNewEntry();
}

KOJournalView::~KOJournalView()
{
}

int KOJournalView::currentDateCount()
{
  return 0;
}
JournalEntry* KOJournalView::getNewEntry()
{
    JournalEntry* Entry = new JournalEntry(mCalendar,parWid);
    jEntries.append( Entry );
    mTopLayout->addWidget(Entry); 
    Entry->setFont ( KOPrefs::instance()->mJornalViewFont  );
    connect ( Entry,SIGNAL(deleteJournal(Journal *) ),this ,SIGNAL(deleteJournal(Journal *) ) )  ;
    connect ( Entry,SIGNAL(newJournal() ),this ,SLOT(newJournal() ) )  ;
    connect ( Entry,SIGNAL(showJournalOnly( Journal * ) ),this ,SLOT(showOnly ( Journal* ) ) )  ;
    return Entry;
}

QPtrList<Incidence> KOJournalView::selectedIncidences()
{
  QPtrList<Incidence> eventList;

  return eventList;
}
void KOJournalView::updateConfig()
{
    JournalEntry* mEntry = jEntries.first();
    while ( mEntry ) {
        mEntry->setFont ( KOPrefs::instance()->mJornalViewFont  );
        mEntry = jEntries.next();
    }
}
void KOJournalView::updateView()
{ 
    JournalEntry* mEntry = jEntries.first();
    while ( mEntry ) {
        mEntry->setFont ( KOPrefs::instance()->mJornalViewFont  );
        mEntry = jEntries.next();
    }
    showDates( mDate, QDate() );
}
void KOJournalView::checkModified()
{
    flushView();
}
void KOJournalView::flushView()
{ 
    static bool ff = false;
    if ( ff ) return;
    ff = true;
    JournalEntry* mEntry = jEntries.first();
    while ( mEntry ) {
        mEntry->flushEntry();
        mEntry = jEntries.next();
    }
    ff = false;
}

void KOJournalView::clearList()
{
    JournalEntry* mEntry = jEntries.first();
    while ( mEntry ) {
        mEntry->clear();
        mEntry = jEntries.next();
    }
}
void KOJournalView::newJournal()
{
    //qDebug("  KOJournalView::newJournal()");
    flushView();
    Journal* mJournal = new Journal;
    mJournal->setDtStart(QDateTime(mDate,QTime(0,0,0)));
    mCalendar->addJournal(mJournal);
    showDates( mDate, QDate() );
}

void KOJournalView::showOnly ( Journal* j )
{
    //qDebug("showOnly %x ", j);
    flushView();
    if ( j == 0 ) {
        showDates( mDate, QDate() );
        return;
    }
    QPtrList<Journal> jl;
    jl.append ( j );
    showList( jl );
    JournalEntry* mEntry = jEntries.first();
    mEntry->setShowOnly();
}
void KOJournalView::showList(QPtrList<Journal> jl)
{ 
    static bool ff = false;
    if ( ff ) return;
    ff = true;
    //qDebug("KOJournalView::showList %d",jl.count() );
    JournalEntry* mEntry = jEntries.first();
    JournalEntry* firstEntry =  mEntry;
    int count = jl.count();
    int iii = 0;
    QWidget* fw = qApp->focusWidget (); 
    while ( iii < count ) {
        if ( !mEntry ) {
            mEntry = getNewEntry();
            mEntry->setVisibleMode( true );
            mEntry->setDate(mDate);
            mEntry->setJournal(jl.at(iii), false);
            mEntry->setVisibleMode( true );
            mEntry->show();
            mEntry = 0;
        } else { 
            int xxx = -1, yyy = -1;
            if ( ((QWidget*) mEntry->editor() ) == fw ) {
                mEntry->editor()->getCursorPosition( &xxx,&yyy);
            }
            mEntry->setVisibleMode( true );
            mEntry->setDate(mDate);
            mEntry->setJournal(jl.at(iii), false);
            mEntry->setVisibleMode( true );
            mEntry->show();
            if ( xxx > -1 && yyy > -1 ) {
                mEntry->editor()->setCursorPosition( xxx, yyy );
            } 
            mEntry = jEntries.next();
        }
        ++iii;
    } 
    while ( mEntry ) {
        mEntry->setDate(mDate);
        mEntry->clear();
        if ( mEntry != firstEntry ) {
            mEntry->hide(); 
            mEntry->setVisibleMode( false );
                       }
        else {
            mEntry->setVisibleMode( true );
            mEntry->show();
        }
        mEntry = jEntries.next();
    }
    ff = false;
}

void KOJournalView::showDates(const QDate &start, const QDate &)
{
    mDate = start;
    mDateLabel->setText(KGlobal::locale()->formatDate(mDate));
    QPtrList<Journal> jl = calendar()->journals4Date( start );
    showList( jl );
}

void KOJournalView::showEvents(QPtrList<Event>)
{
  // After new creation of list view no events are selected.
//  emit incidenceSelected( 0 );
}

void KOJournalView::changeEventDisplay(Event *, int /*action*/)
{
  updateView();
}
 
void KOJournalView::keyPressEvent ( QKeyEvent * e ) 
{
    //qDebug("keyPressEven ");
    if ( e->state() == Qt::ControlButton  ) {  
        if ( e->key () == Qt::Key_Right ||  e->key () == Qt::Key_Left )
             e->ignore();
    }
}
