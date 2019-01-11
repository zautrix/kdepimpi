/*
    This file is part of KOrganizer.
    Copyright (c) 1998 Preston Brown
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
#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <qregexp.h>
#include <qradiobutton.h>
#include <kdialogbase.h>
#include <qvbox.h>

#include <libkcal/calendar.h>

#include "kolistview.h"

class KDateEdit;
class QCheckBox;
class QLineEdit;
class KLineEdit;
class QLabel;
class CalendarView;

using namespace KCal;
class SearchDialog : public  QVBox 
{
    Q_OBJECT
  public:
    SearchDialog(Calendar *calendar,CalendarView *parent=0);
    virtual ~SearchDialog();
    KOListView *listview(){ return listView;}
    void updateView();
    void raiseAndSelect();

  public slots:
    void changeEventDisplay(Event *, int) { updateView(); } 
  void updateConfig();
  void updateList();
  protected slots:
  void setFocusToList();
    void  accept();
    void doSearch();
    void searchTextChanged( const QString &_text );	
    void toggleCheckboxes();
    void slot_add( bool );
    void slot_sub( bool );
    void slot_refine( bool );

  signals:
    void showEventSignal(Event *);
    void editEventSignal(Event *);
    void deleteEventSignal(Event *);

  private:

    QHBox *incidenceGroup ,*subjectGroup ,*attendeeGroup;
    void search(const QRegExp &);

    Calendar *mCalendar;
    
    QPtrList<Event> mMatchedEvents;
    QPtrList<Todo> mMatchedTodos;
    QPtrList<Journal> mMatchedJournals;
    
    QLabel *searchLabel;
    KLineEdit *searchEdit;
    KOListView *listView;
    
    KDateEdit *mStartDate;
    KDateEdit *mEndDate;
    QCheckBox *mSummaryCheck;
    QCheckBox *mDescriptionCheck;
    QCheckBox *mCategoryCheck;
    QCheckBox *mSearchEvent;
    QCheckBox *mSearchTodo;
    QCheckBox *mSearchJournal;
    QCheckBox *mSearchAName;
    QCheckBox *mSearchAEmail;
    QRadioButton *mAddItems, *mSubItems, *mRefineItems;
    void keyPressEvent ( QKeyEvent *e) ;
};

#endif
