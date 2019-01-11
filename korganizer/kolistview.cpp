/*
    This file is part of KOrganizer.
    Copyright (c) 1999 Preston Brown
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

#include <qlistview.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qprogressbar.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qdialog.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qwhatsthis.h>
#include <qregexp.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>

#include <libkdepim/kpimglobalprefs.h>
#include <libkcal/calendar.h>
#include <libkcal/calendarlocal.h>
#include <libkcal/icalformat.h>
#include <libkcal/vcalformat.h>
#include <libkcal/recurrence.h>
#include <libkcal/filestorage.h>
#include <libkdepim/categoryselectdialog.h>
#include <libkcal/kincidenceformatter.h>
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

#ifndef KORG_NOPRINTER
#include "calprinter.h"
#endif
#include "koglobals.h"
#include "koprefs.h"
#include "kfiledialog.h"

#include "kolistview.h"
#include "koeventviewer.h"

extern QPixmap* sgListViewCompletedPix[6];
extern QPixmap* sgListViewJournalPix;

class KOListViewWhatsThis :public QWhatsThis
{
public:
  KOListViewWhatsThis( QWidget *wid, KOListView* view ) : QWhatsThis( wid ), _wid(wid),_view (view) { };

protected:
  virtual QString text( const QPoint& p)
  {
    return _view->getWhatsThisText(p) ;
  }
private:
  QWidget* _wid;
  KOListView * _view;
};


ListItemVisitor::ListItemVisitor(KOListViewItem *item, QDate date )
{
    mItem = item;
    mDate = date;
}

ListItemVisitor::~ListItemVisitor()
{
}

bool ListItemVisitor::visit(Event *e)
{
    bool ok = false;
    QString start, end;
    QDate ds, de;
    if ( e->doesRecur() ) {
        ds = e->getNextOccurence( QDateTime( mDate, QTime(0,0,0)), &ok ).date();
        if ( ok ) {
            int days = e->dtStart().date().daysTo(e->dtEnd().date() );
            start = KGlobal::locale()->formatDate(ds,true);
            de = ds.addDays( days);
            end = KGlobal::locale()->formatDate(de,true);
        }

    } 
    if ( ! ok ) {
        start =e->dtStartDateStr();
        end = e->dtEndDateStr();
        ds = e->dtStart().date();
        de = e->dtEnd().date();
    }
    mItem->setText(0,e->summary());
    mItem->setText(1,start);
    if ( e->doesFloat() )
        mItem->setText(2,"---");
    else
        mItem->setText(2,e->dtStartTimeStr());
    mItem->setText(3,end);
    if ( e->doesFloat() )
        mItem->setText(4,"---");
    else
        mItem->setText(4,e->dtEndTimeStr());
    mItem->setText(5, e->durationText());
    if ( e->isAlarmEnabled() ) {
        mItem->setText(6,e->alarms().first()->offsetText() );
    } else {
        mItem->setText(6, i18n("No"));
    }
    mItem->setText(7, e->recurrenceText());
    if( ! e->doesRecur() )
        mItem->setSortKey( 7, "-" );
    mItem->setText(8, e->cancelled() ? i18n("Yes") : i18n("No"));
    mItem->setText(9,e->categoriesStr());
    mItem->setText(10, KOPrefs::instance()->calName( e->calID() ));
    mItem->setText(11, KGlobal::locale()->formatDateTime( e->lastModified(), true, true ));
    mItem->setSortKey(11,e->lastModifiedSortKey());
    mItem->setText(12,"---");

    QString key; 
    QTime t;
    t = e->doesFloat() ? QTime(0,0) : e->dtStart().time();
    key.sprintf("%04d%02d%02d%02d%02d",ds.year(),ds.month(),ds.day(),t.hour(),t.minute());
    mItem->setSortKey(1,key);

    t = e->doesFloat() ? QTime(0,0) : e->dtEnd().time();
    key.sprintf("%04d%02d%02d%02d%02d",de.year(),de.month(),de.day(),t.hour(),t.minute());
    mItem->setSortKey(3,key);
    return true;
}

bool ListItemVisitor::visit(Todo *t)
{
    mItem->setText(0,t->summary());
    if ( t->isCompleted() ) {
        mItem->setSortKey(0,"99"+ t->summary().left(10));
    } else 
        mItem->setSortKey(0,QString::number( t->percentComplete()+1 )+ t->summary().left(10));
    mItem->setPixmap( 0, *(sgListViewCompletedPix[t->percentComplete()/20]));
    if (t->hasStartDate()) {
        mItem->setText(1,t->dtStartDateStr());
        if (t->doesFloat()) {
            mItem->setText(2,"---");
        } else {
            mItem->setText(2,t->dtStartTimeStr());
        }
    } else {
        mItem->setText(1,"---");
        mItem->setText(2,"---");
    }
    if ( t->isAlarmEnabled() ) {
        mItem->setText(6,t->alarms().first()->offsetText() );
    } else {
        mItem->setText(6, i18n("No"));
    }
    mItem->setText(7, t->recurrenceText());
    if( ! t->doesRecur() )
        mItem->setSortKey( 7, "-" );
    if (t->hasDueDate()) {
        mItem->setText(3,t->dtDueDateStr());
        if (t->doesFloat()) {
            mItem->setText(4,"---");
        } else {
            mItem->setText(4,t->dtDueTimeStr());
        }
    } else {
        mItem->setText(3,"---");
        mItem->setText(4,"---");
    }
    mItem->setText(5, t->durationText());
    mItem->setText(8, t->cancelled() ? i18n("Yes") : i18n("No"));
    mItem->setText(9,t->categoriesStr());
    mItem->setText(10, KOPrefs::instance()->calName( t->calID() ));
    mItem->setText(11, KGlobal::locale()->formatDateTime( t->lastModified(), true, true ));
    mItem->setSortKey(11,t->lastModifiedSortKey());
    mItem->setText(12,QString::number(t->priority()));


    QString key;
    QDate d;
    QTime tm;
    if (t->hasDueDate()) {
        d = t->dtDue().date();
        tm = t->doesFloat() ? QTime(0,0) : t->dtDue().time();
        key.sprintf("%04d%02d%02d%02d%02d",d.year(),d.month(),d.day(),tm.hour(),tm.minute());
        mItem->setSortKey(3,key);
    }
    if ( t->hasStartDate() ) {
        d = t->dtStart().date();
        tm = t->doesFloat() ? QTime(0,0) : t->dtStart().time();
        key.sprintf("%04d%02d%02d%02d%02d",d.year(),d.month(),d.day(),tm.hour(),tm.minute());
        mItem->setSortKey(1,key);
    }
    return true;
}

bool ListItemVisitor::visit(Journal * j)
{ 
    
    QString des;
    mItem->setPixmap( 0, *sgListViewJournalPix);
    if ( !j->summary().isEmpty() ) {
        des = j->summary();
    } else {
        des = j->description().left(30);
        des = des.simplifyWhiteSpace ();
        des.replace (QRegExp ("\\n"),"" );
        des.replace (QRegExp ("\\r"),"" );
    }
    mItem->setText(0,des.left(25));
    mItem->setSortKey(0,"0"+ des.left(25));
    mItem->setText(1,j->dtStartDateStr());
    mItem->setText(2,"---");
    mItem->setText(3,"---");
    mItem->setText(4,"---");
    mItem->setText(5,"---");
    mItem->setText(6,"---");
    mItem->setText(7,"---");
    mItem->setText(8,"---");
    mItem->setText(9,j->categoriesStr());
    mItem->setText(10, KOPrefs::instance()->calName( j->calID() ));
    mItem->setText(11, KGlobal::locale()->formatDateTime( j->lastModified(), true, true ));
    mItem->setSortKey(11,j->lastModifiedSortKey());
    mItem->setText(12,"---");

    QString key; 
    QDate d;
    d = j->dtStart().date();
    key.sprintf("%04d%02d%02d",d.year(),d.month(),d.day());
    mItem->setSortKey(1,key);
    return true;
}

KOListView::KOListView(Calendar *calendar, QWidget *parent,
                       const char *name)
    : KOEventView(calendar, parent, name)
{
    
    mActiveItem = 0;
    mForceShowCompletedTodos = false;
    mListView = new KOListViewListView(this);
    mListView->addColumn(i18n("Summary"));
    mListView->addColumn(i18n("Start Date"));
    mListView->addColumn(i18n("Start Time"));
    mListView->addColumn(i18n("End/Due Date"));
    mListView->addColumn(i18n("End/Due Time"));
    mListView->addColumn(i18n("Duration"));
    mListView->addColumn(i18n("Alarm")); // alarm set?
    mListView->addColumn(i18n("Recurs")); // recurs?
    mListView->addColumn(i18n("Cancelled"));
    mListView->addColumn(i18n("Categories"));
    mListView->addColumn(i18n("Calendar"));
    mListView->addColumn(i18n("Last Modified"));
    mListView->addColumn(i18n("Prio"));

    mListView->setColumnAlignment(0,AlignLeft);
    mListView->setColumnAlignment(1,AlignLeft);
    mListView->setColumnAlignment(2,AlignHCenter);
    mListView->setColumnAlignment(3,AlignLeft);
    mListView->setColumnAlignment(4,AlignHCenter);
    mListView->setColumnAlignment(5,AlignLeft);
    mListView->setColumnAlignment(6,AlignLeft);
    mListView->setColumnAlignment(7,AlignLeft);
    mListView->setColumnAlignment(8,AlignLeft);
    mListView->setColumnAlignment(9,AlignLeft);
    mListView->setColumnAlignment(10,AlignLeft);
    mListView->setColumnAlignment(11,AlignLeft);
    mListView->setColumnAlignment(12,AlignLeft);
    mKOListViewWhatsThis = new KOListViewWhatsThis(mListView->viewport(),this);

    int iii = 0;
    for ( iii = 0; iii< 13 ; ++iii )
        mListView->setColumnWidthMode( iii, QListView::Manual );

    QBoxLayout *layoutTop = new QVBoxLayout(this);
    layoutTop->addWidget(mListView);
    mListView->setFont ( KOPrefs::instance()->mListViewFont );
    mPopupMenu = eventPopup();
    QPopupMenu* selPopup = new QPopupMenu ( this );
    mPopupMenu->insertSeparator();
    
    selPopup->insertItem(i18n("All"),this,
                                  SLOT(allSelection()));
    selPopup->insertItem(i18n("None"),this,
                                  SLOT(clearSelection()));
    selPopup->insertItem(i18n("Delete selected..."),this,
                                  SLOT(deleteAll()));
    mPopupMenu->insertItem(i18n("Selection"), selPopup );
    mPopupMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("Hide all selected"),this,
                                  SLOT(hideAll()),true);

    selPopup->insertSeparator();
    QPopupMenu * exportPO = new QPopupMenu ( this );
    selPopup->insertItem( i18n("Export"),  exportPO );
#ifdef DESKTOP_VERSION
    mPopupMenu->insertSeparator();
    mPopupMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("Print complete list..."),this,
                                  SLOT(printList()),true);
#endif
    mCalPopup = new QPopupMenu ( this );
    selPopup->insertItem( i18n("Set Calendar"),  mCalPopup );

    selPopup->insertItem(i18n("Set categories")+"...",this,
                         SLOT(setCat()) );
    selPopup->insertItem( i18n("Set alarm..."),this,
                          SLOT(setAlarm()));
#if 0
    mPopupMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("Set categories")+"...",this,
                                  SLOT(setCat()),true);
    mPopupMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("Set alarm..."),this,
                                  SLOT(setAlarm()),true); 
#endif
    QObject::connect(mCalPopup,SIGNAL(aboutToShow()),this,
                     SLOT( populateCalPopup() ));
    QObject::connect(mCalPopup,SIGNAL(activated( int )),this,
                     SLOT( setCalendar( int ) ));
    QObject::connect(mPopupMenu,SIGNAL(categoryChanged( Incidence * )),this,
                     SLOT( catChanged( Incidence * ) ));
    exportPO->insertItem( i18n("As iCal (ics) file..."),this,
                                  SLOT(saveToFile()));
    exportPO->insertItem( i18n("As vCal (vcs) file..."),this,
                                  SLOT(saveToFileVCS()));
    exportPO->insertItem( i18n("Journal/Details..."),this,
                                  SLOT(saveDescriptionToFile()));
    // mPopupMenu->insertSeparator();
    // mPopupMenu->addAdditionalItem(QIconSet(QPixmap()),
    //                            i18n("Add Categ. to selected..."),this,
    //                            SLOT(addCat()),true);
    //mPopupMenu->insertSeparator();
#ifndef DESKTOP_VERSION
    selPopup->insertSeparator();
    selPopup->insertItem( i18n("Beam via IR..."),this,
                          SLOT(beamSelected()));
#if 0
    mPopupMenu->addAdditionalItem(QIconSet(QPixmap()),
                                  i18n("Beam via IR"),this,
                                  SLOT(beamSelected()),true);
#endif
#endif
    /*
      mPopupMenu = new QPopupMenu;
      mPopupMenu->insertItem(i18n("Edit Event"), this,
      SLOT (editEvent()));
      mPopupMenu->insertItem(SmallIcon("delete"), i18n("Delete Event"), this,
      SLOT (deleteEvent()));
      mPopupMenu->insertSeparator();
      mPopupMenu->insertItem(i18n("Show Dates"), this,
      SLOT(showDates()));
      mPopupMenu->insertItem(i18n("Hide Dates"), this,
      SLOT(hideDates()));
    */
    QObject::connect(mListView,SIGNAL( newEvent()),
                     this,SIGNAL(signalNewEvent()));
    QObject::connect(mListView,SIGNAL(doubleClicked(QListViewItem *)),
                     this,SLOT(defaultItemAction(QListViewItem *)));
    QObject::connect(mListView,SIGNAL(rightButtonPressed( QListViewItem *,
                                                           const QPoint &, int )),
                     this,SLOT(popupMenu(QListViewItem *,const QPoint &,int)));
    QObject::connect(mListView,SIGNAL(currentChanged(QListViewItem *)),
                     SLOT(processSelectionChange(QListViewItem *)));
    QObject::connect(mListView,SIGNAL(showIncidence(Incidence *)),
                     SIGNAL(showIncidenceSignal(Incidence *)) );

    readSettings(KOGlobals::config(),"KOListView Layout");
}

KOListView::~KOListView()
{
    delete mPopupMenu;
#if QT_VERSION >= 0x030000

#else
    delete mKOListViewWhatsThis;
#endif 
}

void KOListView::catChanged( Incidence* inc)
{
    KOListViewItem* item = getItemForEvent(inc);
    if (item) { 
        ListItemVisitor v(item, mStartDate );
        inc->accept(v);
    }
}
QString KOListView::getWhatsThisText(QPoint p)
{
    KOListViewItem* item = ( KOListViewItem* ) mListView->itemAt( p );
    if ( item )
        return KIncidenceFormatter::instance()->getFormattedText( item->data(),
                                                                  KOPrefs::instance()->mWTshowDetails,
                                                                  KOPrefs::instance()->mWTshowCreated,
                                                                  KOPrefs::instance()->mWTshowChanged);
    return i18n("That is the list view" );

}

void KOListView::setCalendar( int c )
{
    int result = QMessageBox::warning( this, i18n("KO/Pi: Information!"),
                                       i18n("This adds the selected\nitems to the calendar\n%1\nand removes them from\ntheir current calendar!").arg( KOPrefs::instance()->calName( c ) ),
                                       i18n("Continue"), i18n("Cancel"), 0,
                                       0, 1 );
    if ( result != 0 ) {
        return;
    }
    
    QPtrList<Incidence> delSel = getSelectedIncidences() ;
    int icount = delSel.count();
    if ( icount ) {
        Incidence *incidence = delSel.first();
        while ( incidence ) {
            incidence->setCalID( c );
            KOListViewItem * item = getItemForEvent( incidence );
            if ( item ) {
                ListItemVisitor v(item, mStartDate );
                incidence->accept(v);
            }
            incidence = delSel.next();
        }
    }
    QPtrList<KopiCalendarFile> calendars = KOPrefs::instance()->mCalendars;
    KopiCalendarFile * cal = calendars.first();
    while ( cal ) {
        mCalendar->setCalendarEnabled( cal->mCalNumber,cal->isEnabled );
        mCalendar->setAlarmEnabled( cal->mCalNumber, cal->isAlarmEnabled );
        mCalendar->setReadOnly( cal->mCalNumber, cal->isReadOnly );
        if ( cal->isStandard )
            mCalendar->setDefaultCalendar( cal->mCalNumber );
        cal = calendars.next();
    }
    mCalendar->setSyncEventsReadOnly();
    mCalendar->reInitAlarmSettings();

}
void KOListView::populateCalPopup()
{
    mCalPopup->clear();
    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    while ( kkf ) {
        int index = mCalPopup->insertItem( kkf->mName+"...", kkf->mCalNumber);
        if ( kkf->mErrorOnLoad || kkf->isReadOnly )
        mCalPopup->setItemEnabled( index, false );
        kkf = KOPrefs::instance()->mCalendars.next();
    }
}
void KOListView::updateList()
{
    // qDebug(" KOListView::updateList() ");

}

void KOListView::clearList()
{
    clear ();
}

void KOListView::setCat()
{

    bool set = true;
    int result = KMessageBox::warningYesNoCancel(this, 
                                                 i18n("Do you want to <b>add</b> categories to the selected items or <b>reset</b> the list (i.e. remove current categories)?"),
                                                 i18n("Set categories"),
                                                 i18n("Add"),
                                                 i18n("Reset"));
    if (result == KMessageBox::Cancel) return;
    if (result == KMessageBox::Yes) set = false;
    setCategories( set );
}

void  KOListView::setAlarm()
{
    KOAlarmPrefs kap( this);
    if ( !kap.exec() )
        return;
    QPtrList<Incidence> delSel = getSelectedIncidences( true, true, false, true ); // no journals, only due todos
    Incidence* inc = delSel.first();
    int count = 0;
    while ( inc ) {
        ++count;
        if (kap.mAlarmButton->isChecked()) {
            if (inc->alarms().count() == 0) 
                inc->newAlarm();
            Alarm *alarm = inc->alarms().first();
            alarm->setEnabled(true);
            int j = kap.mAlarmTimeEdit->value()* -60;
            if (kap.mAlarmIncrCombo->currentItem() == 1)
                j = j * 60;
            else if (kap.mAlarmIncrCombo->currentItem() == 2)
                j = j * (60 * 24);
            alarm->setStartOffset( j );

            if (!kap.mAlarmProgram.isEmpty() && kap.mAlarmProgramButton->isOn()) {
                alarm->setProcedureAlarm(kap.mAlarmProgram);
            }
            else if (!kap.mAlarmSound.isEmpty() && kap.mAlarmSoundButton->isOn())
                alarm->setAudioAlarm(kap.mAlarmSound);
            else
                alarm->setType(Alarm::Invalid);
        } else {
            QPtrList<Alarm> alarms = inc->alarms();
            Alarm *alarm;
            for (alarm = alarms.first(); alarm; alarm = alarms.next() ) {
                alarm->setEnabled(false);
                alarm->setType(Alarm::Invalid);
            }
        }
        KOListViewItem* item = getItemForEvent(inc);
        if (item) { 
            ListItemVisitor v(item, mStartDate );
            inc->accept(v);
        }
        inc = delSel.next();   
    }
    topLevelWidget()->setCaption( i18n("Changed alarm for %1 items").arg( count )  );
    qDebug("KO: Set alarm for %d items", count);
    calendar()->reInitAlarmSettings();
    QTimer::singleShot( 1, this, SLOT ( resetFocus() ) );
}
void KOListView::setCategories( bool removeOld )
{
   
    KPIM::CategorySelectDialog* csd = new KPIM::CategorySelectDialog( KOPrefs::instance(), 0 );
    csd->setColorEnabled();
    if (! csd->exec()) { 
        delete csd;
        return;
    }
    QStringList catList = csd->selectedCategories();
    delete csd;
    QPtrList<Incidence> delSel = getSelectedIncidences(); // all inc allowed;
    Incidence* inc = delSel.first();
    while ( inc ) {
        if ( removeOld ) {
            inc->setCategories( catList, false );
        } else {
            inc->addCategories( catList, false );
        } 
        KOListViewItem* item = getItemForEvent(inc);
        if (item) { 
            ListItemVisitor v(item, mStartDate );
            inc->accept(v);
        }
        inc = delSel.next();
    }
    QTimer::singleShot( 1, this, SLOT ( resetFocus() ) );
}

void KOListView::beamSelected()
{
    QPtrList<Incidence> delSel = getSelectedIncidences() ;
    if ( delSel.count() )
        emit beamIncidenceList( delSel );
}

void KOListView::saveDescriptionToFile()
{

    int result = QMessageBox::warning( this, i18n("KO/Pi: Information!"),
                                       i18n("This saves the text/details of selected\nJournals and Events/Todos\nto a text file."),
                                       i18n("Continue"), i18n("Cancel"), 0,
                                       0, 1 );
    if ( result != 0 ) {
        return;
    }
    QPtrList<Incidence> delSel = getSelectedIncidences() ;
    int icount = delSel.count();
    if ( icount ) {
        QString fn =  KOPrefs::instance()->mLastSaveFile;
        fn =  KFileDialog::getSaveFileName( fn, i18n("Save filename"), this );
    
        if ( fn == "" )
            return;
        QFileInfo info;
        info.setFile( fn );
        QString mes;
        bool createbup = true;
        if ( info. exists() ) { 
            mes =  i18n("File already exists!\nOld file from:\n%1\nOverwrite?\n").arg (KGlobal::locale()->formatDateTime(info.lastModified (), true, false )  );
            int result = QMessageBox::warning( this, i18n("KO/Pi: Warning!"),mes,
                                               i18n("Overwrite!"), i18n("Cancel"), 0,
                                               0, 1 );
            if ( result != 0 ) {
                createbup = false;
            }
        }
        if ( createbup ) {
            QString text = i18n("KO/Pi Description/Journal save file.\nSave date: ") + 
                KGlobal::locale()->formatDateTime(QDateTime::currentDateTime(), false);
            Incidence *incidence = delSel.first();
            icount = 0;
            while ( incidence ) {
                if ( incidence->typeID()  == journalID ) {
                    text += "\n************************************\n";
                    if ( !incidence->summary().isEmpty() )
                        text += i18n("Journal: %1 from ").arg( incidence->summary() )  +incidence->dtStartDateStr( false );
                    else
                        text += i18n("Journal from: ") +incidence->dtStartDateStr( false );
                    if ( !incidence->location().isEmpty() )
                        text +="\n(" +  i18n("Location: ") + incidence->location()+ ")";
                    text +="\n" +  i18n("Last modified: ") +KGlobal::locale()->formatDateTime(incidence->lastModified(), false);
                    text +="\n" +  i18n("Description: ") + "\n"+ incidence->description();
                    ++icount;

                } else {
                    if ( !incidence->description().isEmpty() ) {
                        text += "\n************************************\n";
                        if ( incidence->typeID()  == todoID )
                            text += i18n("To-Do: ");
                        text += incidence->summary();
                        if ( !incidence->location().isEmpty() )
                            text +="\n(" +  i18n("Location: ") + incidence->location()+ ")";
                        if ( incidence->hasStartDate() )
                            text +="\n"+ i18n("Start Date: ") + incidence->dtStartStr( false );
                        text +="\n"+ i18n("Last modified: ") +KGlobal::locale()->formatDateTime(incidence->lastModified(), false);
                        text += "\n" + i18n("Description: ") + "\n" + incidence->description();
                        ++icount;

                    }
                }
                incidence = delSel.next();
            }
            QFile file( fn );
            if (!file.open( IO_WriteOnly ) ) {
                topLevelWidget()->setCaption(i18n("File open error - nothing saved!") ); 
                return;
            }  
            QTextStream ts( &file );
            ts << text;
            file.close();
            //qDebug("%s ", text.latin1());
            mes = i18n("KO/Pi:Saved %1 descriptions/journals").arg(icount );
            KOPrefs::instance()->mLastSaveFile = fn;
            topLevelWidget()->setCaption(mes); 
        }
    }
}
void KOListView::saveToFileVCS()
{
    writeToFile( false );
}
void KOListView::saveToFile()
{
    writeToFile( true );
}
QPtrList<Incidence> KOListView::getSelectedIncidences( bool includeEvents, bool includeTodos, bool includeJournals, bool onlyDueTodos )
{
    QPtrList<Incidence> delSel ;
    bool addSubTodos = false;
    bool askSubTodos = true;
    QListViewItem *item  = mListView->firstChild ();
    while ( item ) {
        if ( item->isSelected() ) {
            Incidence* inc = ((KOListViewItem *)item)->data();
            if ( ( addSubTodos && delSel.findRef( inc ) == -1) || !addSubTodos   ) {
                if ( (inc->typeID() == todoID && includeTodos) || 
                     (inc->typeID() == eventID && includeEvents) || 
                     (inc->typeID() == journalID && includeJournals) ) {
                    if ( inc->typeID() == todoID && onlyDueTodos ) {
                        if ( ((Todo*)inc)->hasDueDate() )
                            delSel.append( inc );
                    } else
                        delSel.append( inc );
                    
                }
            }
            if ( inc->typeID() == todoID ) {
                Todo * todo = (Todo*) inc; 
                if ( todo->relations().count() ) {
                    if ( askSubTodos ) {
                        int result = KMessageBox::warningYesNoCancel(this, 
                                                                     i18n("One (or more) selected\ntodo has subtodos!\nDo you want to select\nall subtodos of all\nselected todos as well?"),
                                                                     i18n("Todo has subtodos"),
                                                                     i18n("Yes"),
                                                                     i18n("No"));
                        if ( result == KMessageBox::Cancel ) {
                            delSel.clear();
                            return delSel;
                        }
                        if (result == KMessageBox::Yes) 
                            addSubTodos = true;
                        askSubTodos = false;
                    }
                    if ( addSubTodos ) {
                        QPtrList<Incidence> tempSel ;
                        inc->addRelationsToList( &tempSel );
                        Incidence* tempinc = tempSel.first();
                        while ( tempinc ) {
                            if ( delSel.findRef( tempinc ) == -1 ) {
                                if ( tempinc->typeID() == todoID && onlyDueTodos ) {
                                    if ( ((Todo*)tempinc)->hasDueDate() )
                                        delSel.append( tempinc );
                                } else
                                    delSel.append( tempinc );
                            }
                            tempinc = tempSel.next();
                        }
                    }
                }
            }
        }
        item = item->nextSibling();
    }
    return delSel;
}

void KOListView::writeToFile( bool iCal )
{
    QPtrList<Incidence> delSel = getSelectedIncidences(); // all inc allowed;
    if ( !iCal ) {
        bool journal = false;
        Incidence *incidence = delSel.first();
        while ( incidence ) {
            if ( incidence->typeID() == journalID ) {
                journal = true;
                break;
            }
            incidence = delSel.next();
        }
        if ( journal ) {
            int result = KMessageBox::warningContinueCancel(this,
                                                            i18n("The journal entries can not be\nexported to a vCalendar file."),
                                                            i18n("Data Loss Warning"),i18n("Proceed"),i18n("Cancel"),
                                                            true);
            if (result != KMessageBox::Continue) return;
        }
    }
    if ( delSel.count() ) {
        QString fn =  KOPrefs::instance()->mLastSaveFile;
        QString extension;
        if ( iCal ) {
            if ( fn.right( 4 ).lower() == ".vcs" ) {
                fn = fn.left( fn.length() -3) + "ics";
            }
        } else {
            if ( fn.right( 4 ).lower() == ".ics" ) {
                fn = fn.left( fn.length() -3) + "vcs";
            }
        }
        fn =  KFileDialog::getSaveFileName( fn, i18n("Save filename"), this );
    
        if ( fn == "" )
            return;
        QFileInfo info;
        info.setFile( fn );
        QString mes;
        bool createbup = true;
        if ( info. exists() ) { 
            mes =  i18n("File already exists!\nOld file from:\n%1\nOverwrite?\n").arg (KGlobal::locale()->formatDateTime(info.lastModified (), true, false )  );
            int result = QMessageBox::warning( this, i18n("KO/Pi: Warning!"),mes,
                                               i18n("Overwrite!"), i18n("Cancel"), 0,
                                               0, 1 );
            if ( result != 0 ) {
                createbup = false;
            }
        }
        if ( createbup ) {
            CalendarLocal cal;
            cal.setTimeZoneId(KPimGlobalPrefs::instance()->mTimeZoneId);
            Incidence *incidence = delSel.first();
            while ( incidence ) {
                cal.addIncidence( incidence->clone() );
                incidence = delSel.next();
            }
            if ( iCal ) {
                ICalFormat format;
                format.save( &cal, fn );
            } else {
                
                VCalFormat format;
                format.save( &cal, fn );
            }
            mes = i18n("KO/Pi:Saved %1").arg(fn );
            KOPrefs::instance()->mLastSaveFile = fn;
            topLevelWidget()->setCaption(mes); 
        }
    }
    QTimer::singleShot( 1, this, SLOT ( resetFocus() ) );
}
void  KOListView::hideAll()
{
    QPtrList<QListViewItem> delSel ;
    QListViewItem *item  = mListView->firstChild ();
    while ( item ) {
        if ( item->isSelected() ) {
            delSel.append(item);
        }
        item = item->nextSibling();
    }
    item = delSel.first() ;
    while ( item ) {
        QListViewItem * del = item;
        item = delSel.next();
        delete del;
    }
}
void  KOListView::printList()
{
    mListView->printList();
}
void KOListView::deleteAll()
{
    QPtrList<Incidence> delSel = getSelectedIncidences(); // all inc allowed;;
    if ( delSel.count() ) {
        int icount = delSel.count();
        Incidence *incidence = delSel.first();
        Incidence *toDelete;   
        KOPrefs *p = KOPrefs::instance();
        bool confirm = p->mConfirm;
        QString mess;
        mess = mess.sprintf( i18n("You have %d item(s) selected.\n"), icount );
        if ( KMessageBox::Continue == KMessageBox::warningContinueCancel(this, mess + i18n("All selected items will be\npermanently deleted.\n(Deleting items will take\nsome time on a PDA)\n"), i18n("KO/Pi Confirmation"),i18n("Delete")) ) {
            p->mConfirm = false;
            int delCounter = 0;
            QDialog dia ( this, "p-dialog", true ); 
            QLabel lab (i18n("Close dialog to abort deletion!"), &dia ); 
            QVBoxLayout lay( &dia );
            lay.setMargin(7); 
            lay.setSpacing(7); 
            lay.addWidget( &lab);
            QProgressBar bar( icount, &dia );
            lay.addWidget( &bar);
            int w = 220;
            int h = 50;
            int dw = QApplication::desktop()->width();
            int dh = QApplication::desktop()->height();
            dia.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
            //dia.resize( 240,50 );
            dia.show();
            KOPrefs::instance()->mGlobalUpdateDisabled = true;
            while ( incidence ) {
                bar.setProgress( delCounter );
                mess = mess.sprintf( i18n("Deleting item %d ..."), ++delCounter );
                dia.setCaption( mess );
                qApp->processEvents();
                toDelete =  (incidence);
                incidence = delSel.next();
                emit deleteIncidenceSignal(toDelete ); 
                if (  dia.result() != 0 )
                    break;
               
            }
            KOPrefs::instance()->mGlobalUpdateDisabled = false;
            emit deleteIncidenceSignal( 0 ); 
            mess = mess.sprintf( i18n("%d items remaining in list."), count() );
            topLevelWidget ()->setCaption( mess );
            p->mConfirm = confirm;
        }    
    }  
        

}
int KOListView::maxDatesHint()
{
    return 0;
}

int KOListView::currentDateCount()
{
    return 0;
}

QPtrList<Incidence> KOListView::selectedIncidences()
{
    QPtrList<Incidence> eventList;
    QListViewItem *item  = mListView->firstChild ();
    while ( item ) {
        if ( item->isSelected() ) {
            eventList.append(((KOListViewItem *)item)->data());
        }

        item = item->nextSibling();
    }

    //  // QListViewItem *item = mListView->selectedItem();
    //if (item) eventList.append(((KOListViewItem *)item)->data());

    return eventList;
}

DateList KOListView::selectedDates()
{
    DateList eventList;
    return eventList;
}

void KOListView::showDates(bool show)
{
    // Shouldn't we set it to a value greater 0? When showDates is called with
    // show == true at first, then the columnwidths are set to zero.
    static int oldColWidth1 = 0;
    static int oldColWidth3 = 0;

    if (!show) {
        oldColWidth1 = mListView->columnWidth(1);
        oldColWidth3 = mListView->columnWidth(3);
        mListView->setColumnWidth(1, 0);
        mListView->setColumnWidth(3, 0);
    } else {
        mListView->setColumnWidth(1, oldColWidth1);
        mListView->setColumnWidth(3, oldColWidth3);
    }
    mListView->repaint();
}

void KOListView::printPreview(CalPrinter *calPrinter, const QDate &fd,
                              const QDate &td)
{
#ifndef KORG_NOPRINTER
    calPrinter->preview(CalPrinter::Day, fd, td);
#endif
}

void KOListView::showDates()
{
    showDates(true);
}

void KOListView::hideDates()
{
    showDates(false);
}

void KOListView::resetFocus()
{ 
    topLevelWidget()->setActiveWindow();
    topLevelWidget()->raise();
    mListView->setFocus();
}
void KOListView::updateView()
{
    mListView->setFocus();
    if ( mListView->firstChild () ) {
        mListView->setCurrentItem( mListView->firstChild () );
    }
    processSelectionChange( mListView->firstChild ()  );
}
void KOListView::updateConfig()
{

    mListView->setFont ( KOPrefs::instance()->mListViewFont );
    updateView();

}
void KOListView::setStartDate(const QDate &start)
{
    mStartDate = start;
}

void KOListView::showDates(const QDate &start, const QDate &end)
{
    clear();
    mStartDate = start;
    QDate date = start;
    while( date <= end ) {
        addEvents(calendar()->events(date));
        addTodos(calendar()->todos(date));
        addJournals( calendar()->journals4Date(date)  );
        date = date.addDays( 1 );
    }
    //emit incidenceSelected( 0 );
    updateView();
  
}

void KOListView::addEvents(QPtrList<Event> eventList)
{
 
    Event *ev;
    for(ev = eventList.first(); ev; ev = eventList.next()) {
        addIncidence(ev);
    }
    if ( !mListView->currentItem() ){
        updateView();
    }
}

void KOListView::addTodos(QPtrList<Todo> eventList)
{
    Todo *ev;
    for(ev = eventList.first(); ev; ev = eventList.next()) {
        addIncidence(ev);
    }
    if ( !mListView->currentItem() ){
        updateView();
    }
}
void KOListView::addJournals(QPtrList<Journal> eventList)
{
    Journal *ev;
    for(ev = eventList.first(); ev; ev = eventList.next()) {
        addIncidence(ev);
    }  
    if ( !mListView->currentItem() ){
        updateView();
    }
}

void KOListView::showCompletedTodos()
{
    mForceShowCompletedTodos = true;
}
void KOListView::addIncidence(Incidence *incidence)
{
    if ( mUidDict.find( incidence->uid() ) ) return;
  
    // mListView->setFont ( KOPrefs::instance()->mListViewFont );
    if ( incidence->typeID() == todoID ) {
        if ( ! mForceShowCompletedTodos ) {
            if ( !KOPrefs::instance()->mShowCompletedTodo && ((Todo*)incidence)->isCompleted() )
                return;
        }
    }
    mUidDict.insert( incidence->uid(), incidence );
    KOListViewItem *item = new KOListViewItem( incidence, mListView ); 
    ListItemVisitor v(item, mStartDate );
    if (incidence->accept(v)) {
        return;
    }
    else delete item;
}

void KOListView::showEvents(QPtrList<Event> eventList)
{
    clear();

    addEvents(eventList);

    // After new creation of list view no events are selected.
    emit incidenceSelected( 0 );
}
int KOListView::count()
{
    return mListView->childCount();
}

void KOListView::changeEventDisplay(Event *event, int action)
{
    KOListViewItem *item;

    switch(action) {
    case KOGlobals::EVENTADDED:
        addIncidence( event );
        break;
    case KOGlobals::EVENTEDITED:
        item = getItemForEvent(event);
        if (item) { 
            ListItemVisitor v(item, mStartDate );
            ((Incidence*)event)->accept(v);
        }
        break;
    case KOGlobals::EVENTDELETED:
        item = getItemForEvent(event);
        if (item) {
            mUidDict.remove( event->uid() ); 
            delete item;
        }
        break;
    default:
        ;
    }
}

KOListViewItem *KOListView::getItemForEvent(Incidence *event)
{
    KOListViewItem *item = (KOListViewItem *)mListView->firstChild();
    while (item) {
        if (item->data() == event) return item;
        item = (KOListViewItem *)item->nextSibling();
    }
    return 0;
}

void KOListView::defaultItemAction(QListViewItem *i)
{
    KOListViewItem *item = static_cast<KOListViewItem *>( i );
    if ( item ) defaultAction( item->data() );
      
}

void KOListView::popupMenu(QListViewItem *item,const QPoint &,int)
{
    mActiveItem = (KOListViewItem *)item;
    if (mActiveItem) {
        Incidence *incidence = mActiveItem->data();
        mPopupMenu->enableDefault( !mListView->hasMultiSelection( item ) );
        mPopupMenu->showIncidencePopup(incidence);

        /*
          if ( incidence && incidence->type() == "Event" ) {
          Event *event = static_cast<Event *>( incidence );
          mPopupMenu->showEventPopup(event);
          }
        */
    }
}

void KOListView::readSettings(KConfig *config, QString setting)
{
    //  qDebug("KOListView::readSettings ");
    mListView->restoreLayout(config,setting);
}

void KOListView::writeSettings(KConfig *config, QString setting)
{
    // qDebug("KOListView::writeSettings ");
    mListView->saveLayout(config, setting);
}

void KOListView::processSelectionChange(QListViewItem *)
{
  
    KOListViewItem *item =
        static_cast<KOListViewItem *>( mListView->currentItem() );

    if ( !item ) {
        emit incidenceSelected( 0 );
    } else {
        emit incidenceSelected( item->data() );
    }
}

void KOListView::clearSelection()
{
    mListView->selectAll( false );
}
void KOListView::allSelection()
{
    mListView->selectAll( true );
}

void KOListView::clear()
{
    mListView->clear();
    mUidDict.clear();
}

Incidence* KOListView::currentItem()
{
    if ( mListView->currentItem()  )
        return ((KOListViewItem*) mListView->currentItem())->data();
    return 0;
}
void KOListView::keyPressEvent ( QKeyEvent *e) 
{
   
    if (  e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace ) {
        deleteAll();
        return;
    }
   
    e->ignore();
}
void KOListViewListView::keyPressEvent ( QKeyEvent *e) 
{
  
    switch ( e->key() ) {
    case Qt::Key_Down:
        if ( e->state() == ShiftButton ) {
            QListViewItem* cn = currentItem();
            if ( !cn ) 
                cn = firstChild();
            if ( !cn ) 
                return;
            while (  cn->nextSibling() )
                cn = cn->nextSibling();
            setCurrentItem ( cn );
            ensureItemVisible ( cn );
        
            e->accept();
            return;
        }
        if ( e->state() == ControlButton ) {
            int count = childCount ();
            int jump = count / 5;
            QListViewItem* cn;
            cn = currentItem();
            if ( ! cn )
                return;
            if ( jump  == 0 )
                jump = 1;
            while ( jump && cn->nextSibling() ) {
                cn = cn->nextSibling();
                --jump;
            }
            setCurrentItem ( cn );
            ensureItemVisible ( cn );
            
        } else
            QListView::keyPressEvent ( e ) ;
        e->accept();
        break;
              
    case Qt::Key_Up:
        if ( e->state() == ShiftButton ) {
            QListViewItem* cn = firstChild();
            if ( cn ) {
                setCurrentItem ( cn );
                ensureItemVisible ( cn );
            }
            e->accept();
            return;
        }
        if ( e->state() == ControlButton ) {
            int count = childCount ();
            int jump = count / 5;
            QListViewItem* cn;
            cn = currentItem();
            if ( ! cn )
                return;
            if ( jump  == 0 )
                jump = 1;
            while ( jump && cn->itemAbove ()) {
                cn = cn->itemAbove ();
                --jump;
            }
            setCurrentItem ( cn );
            ensureItemVisible ( cn );
        } else
            QListView::keyPressEvent ( e ) ;
        e->accept();
        break;   
    case Qt::Key_I: {
        QListViewItem* cn;
        cn = currentItem();
        if ( cn ) {
            KOListViewItem* ci = (KOListViewItem*)( cn ); 
            if ( ci ){
                //emit showIncidence( ci->data());
                cn = cn->nextSibling();
                if ( cn ) {
                    setCurrentItem ( cn );
                    ensureItemVisible ( cn );
                }
                emit showIncidence( ci->data());
            }
        }
        e->accept();
    }
        break;          
    case Qt::Key_Return:
    case Qt::Key_Enter:
        {
            QListViewItem* cn;
            cn = currentItem();
            if ( cn ) {
                KOListViewItem* ci = (KOListViewItem*)( cn ); 
                if ( ci ){ 
                    if ( e->state() == ShiftButton )
                        ci->setSelected( false );
                    else
                        ci->setSelected( true );
                    cn = cn->nextSibling();
                    if ( cn ) {
                        setCurrentItem ( cn );
                        ensureItemVisible ( cn );
                    } else {
                        emit currentChanged( ci );
                    }
                }
            }
            e->accept();
        }
    break;             
    default:
        e->ignore();
    }
}
KOListViewListView::KOListViewListView(KOListView * lv ) 
    : KListView( lv, "kolistlistview", false )
{  
    mYMousePos = 0;
    setAllColumnsShowFocus( true ); 
    mPopupTimer = new QTimer(this);
    connect(mPopupTimer , SIGNAL(timeout()), this, SLOT(popupMenu()));
#ifndef DESKTOP_VERSION
    //QPEApplication::setStylusOperation(viewport(), QPEApplication::RightOnHold );
#endif
    setSelectionMode( QListView::Multi );   
    setMultiSelection( true);
}
bool KOListViewListView::hasMultiSelection(QListViewItem* item)
{
    QListViewItem *qitem  = firstChild ();
    while ( qitem ) {
        if ( qitem->isSelected() && item != qitem ) 
            return true;
        qitem = qitem->nextSibling();
    }
    return false;
}
void KOListViewListView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    if (!e) return;
    QPoint vp = contentsToViewport(e->pos());
    QListViewItem *item = itemAt(vp);
    if (!item) {
        emit newEvent();
        return;
    }
    KListView::contentsMouseDoubleClickEvent(e);
}
#if 0
void KOListViewListView::contentsMousePressEvent(QMouseEvent *e)
{
    //qDebug("contentsMousePressEvent++++ ");
    KListView::contentsMousePressEvent( e );
    if (  e->button() == RightButton    ) {
        QListViewItem* ci =  currentItem();
        clearSelection () ;
        if ( ci )
            ci->setSelected( true );
    }
}
void KOListViewListView::contentsMouseReleaseEvent(QMouseEvent *e)
{
    KListView::contentsMouseReleaseEvent(e);
}
void KOListViewListView::contentsMouseMoveEvent(QMouseEvent *e)
{
    KListView::contentsMouseMoveEvent(e);
}
#endif
void KOListViewListView::popupMenu()
{
    mPopupTimer->stop();
    QMouseEvent* e = new QMouseEvent( QEvent::MouseButtonPress, mEventPos ,mEventGlobalPos, RightButton , RightButton );
    QApplication::postEvent( this->viewport(), e ); 

}
void KOListViewListView::contentsMousePressEvent(QMouseEvent *e)
{
    //qDebug("contentsMousePressEvent++++ %d %d", e->pos().y(), e->globalPos().y());
    mYMousePos = mapToGlobal( (e->pos())).y();
    if (  e->button() == LeftButton ) {
        mPopupTimer->start( 600 ); 
        mEventPos = contentsToViewport(e->pos());
        mEventGlobalPos = e->globalPos();
    }
    KListView::contentsMousePressEvent( e );
    if (  e->button() == RightButton    ) {
        QListViewItem* ci =  currentItem();
        //clearSelection();
        if ( ci )
            ci->setSelected( true );
    }
}
void KOListViewListView::contentsMouseReleaseEvent(QMouseEvent *e)
{
    mPopupTimer->stop();
    KListView::contentsMouseReleaseEvent(e);
}
void KOListViewListView::contentsMouseMoveEvent(QMouseEvent *e)
{
    // qDebug("contentsMouseMoveEv....... ");
    //  qDebug("start: %d  current %d ",mYMousePos , mapToGlobal( (e->pos())).y()   );
    int diff = mYMousePos - mapToGlobal( (e->pos())).y();
    if ( diff < 0 ) diff = -diff;
    if ( diff > 15 )
        mPopupTimer->stop();
    else {
        mEventPos = contentsToViewport(e->pos());
        mEventGlobalPos = e->globalPos();
    }
    KListView::contentsMouseMoveEvent(e);
}

#define protected public
#include <qheader.h>
#undef protected 
void  KOListViewListView::printList()
{
#ifdef DESKTOP_VERSION 
    KOPrintPrefs pp ( this );
    if (!pp.exec() )
        return;
    int scaleval = pp.printMode() ;

    QPrinter printer;
    if (!printer.setup() )
        return;
    clearSelection (); 
    QPainter p;
    p.begin ( &printer  );
    p.setFont(font());
    QPaintDeviceMetrics m = QPaintDeviceMetrics ( &printer );
    float dx, dy;
    int wid = (m.width() * 9)/10;
    dx = (float) wid/(float)contentsWidth ();
    dy  = (float)(m.height()) / (float)contentsHeight ();
    float scale;
    // scale to fit the width or height of the paper
    if ( dx < dy )
        scale = dx;
    else
        scale = dy;

    p.translate( m.width()/10,m.width()/10 );
    if ( scaleval == 2 ||  scaleval == 1 && scale < 1.0 ) {
        p.scale( scale, scale );
    }

    int cou = header()->count();
    int iii;
    QRect rect ( 0,0,0, header()->height());
    for ( iii = 0; iii < cou; ++iii ) {
        rect.setLeft ( header()->sectionPos( iii ) ); 
        rect.setRight ( header()->sectionPos( iii ) + header()->sectionSize (iii)); 
        header()->paintSection ( & p,  header()->mapToIndex (iii),  rect );
    }
    p.translate( 0,  header()->height());
    //drawContentsOffset ( &p, 0,0, 0,0, contentsWidth (),  contentsHeight () ); 

    const QColorGroup &cg = colorGroup();
    KOListViewItem* item = (KOListViewItem* )firstChild();
    int yOff = 0;
    while ( item ) {
      p.translate( 0, yOff );
      p.save();
      for ( iii = 0; iii < cou; ++iii ) {
	int align = columnAlignment( iii );
	if ( align == AlignAuto ) align = AlignLeft;
	p.restore();
	p.save();
	p.translate( header()->sectionPos( iii ), 0);
	item->paintCell( &p, cg, iii, header()->sectionSize (iii), align );
      }
      yOff = item->height();
      item = (KOListViewItem* )(item->itemBelow());
      p.restore();
    }
    p.end();
#endif   
}
