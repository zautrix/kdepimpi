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
#ifndef _KOJOURNALVIEW_H
#define _KOJOURNALVIEW_H

#include <korganizer/baseview.h>
#include <qlayout.h>

class JournalEntry;
class QLabel;

/**
 * This class provides a journal view.
 
 * @short View for Journal components.
 * @author Cornelius Schumacher <schumacher@kde.org>
 * @see KOBaseView
 */
class KOJournalView : public KOrg::BaseView
{
    Q_OBJECT
  public:
    KOJournalView(Calendar *calendar, QWidget *parent = 0, 
	       const char *name = 0);
    ~KOJournalView();

    virtual int currentDateCount();
    void clearList();
    virtual QPtrList<Incidence> selectedIncidences();
    DateList selectedDates()
      {DateList q;
       return q;};
    void checkModified();
 signals:
     void deleteJournal(Journal *);
  public slots:
     void showOnly ( Journal* );
     void newJournal();
    void updateView();
    void flushView();
    void updateConfig();
    void showDates( const QDate &start, const QDate &end );
    void showEvents(QPtrList<Event> eventList);

    void changeEventDisplay(Event *, int);
  
  private:
    void showList(QPtrList<Journal> jl);
    Calendar *mCalendar;
    JournalEntry* getNewEntry();
    QPtrList<JournalEntry> jEntries; 
    void keyPressEvent ( QKeyEvent * ) ;
    QBoxLayout *mTopLayout;
    QWidget *parWid;
    QLabel * mDateLabel;
    QDate mDate;
    
};

#endif
