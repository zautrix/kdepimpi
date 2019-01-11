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
#ifndef JOURNALENTRY_H
#define JOURNALENTRY_H
//
// Widget showing one Journal entry

#include <qframe.h>

#include <libkcal/calendar.h>

class QLabel;
class KTextEdit;
class QComboBox;
class KLineEdit;
class KOLocationBox;

using namespace KCal;

class JournalEntry : public QFrame {
    Q_OBJECT
  public:
    JournalEntry(Calendar *,QWidget *parent);
    virtual ~JournalEntry();
    
    void setJournal(Journal *, bool saveJournal = true );
    Journal *journal() const;

    void setDate(const QDate &);

    void clear();

    void flushEntry();
    void setShowOnly();
    QSize sizeHint() const;
    void setVisibleMode( bool b ) { visibleMode = b;}
    void fillCalendar( int id = 0 );
    void resizeEvent(QResizeEvent* e ) ;
    KTextEdit * editor() {return mEditor;};
  protected slots:
    void slotSaveTemplate();
    void slotLoadTemplate();
    void toggleShowJournal();
    void setVisibleOn(); 
 signals:
     void deleteJournal(Journal *);
     void newJournal();
     void showJournalOnly( Journal * );

  protected:    
    bool eventFilter( QObject *o, QEvent *e );

    void writeJournal();
    
  private:
    int mMaxWidDiff;
    int mDeskWid;
    bool visibleMode;
    bool showOnlyMode;
    Calendar *mCalendar;
    Journal *mJournal;
    QDate mDate;
    void keyPressEvent ( QKeyEvent * ) ;
    QComboBox *mCalendarBox;
    KOLocationBox * mTitle;
    KTextEdit *mEditor;
    int heiHint;
};

#endif
