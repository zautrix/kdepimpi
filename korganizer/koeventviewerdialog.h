/*
    This file is part of KOrganizer.
    Copyright (c) 2000, 2001 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef KOEVENTVIEWERDIALOG_H
#define KOEVENTVIEWERDIALOG_H
//
// Viewer dialog for events.
//

#include <qtextview.h>

#include <kdialogbase.h>

#include <libkcal/event.h>

using namespace KCal;

class KOEventViewer;

class KOEventViewerDialog : public KDialogBase {
    Q_OBJECT
  public:
    KOEventViewerDialog(QWidget *parent=0,const char *name=0);
    virtual ~KOEventViewerDialog();

    void setEvent(Event *event);
    void addEvent(Event *event);
    void setTodo(Todo *event);
    void setJournal(Journal *journal);
    void setIncidence(Incidence *inc);
    void addIncidence(Incidence *inc);
    void addText(QString text);
    void showMe();
    void setSyncMode( bool );
    void setColorMode( int m );
    int executeS( bool );
  public slots:
     void  updateConfig();
     void print();
     void setMyFocus();
 signals:
     void showIncidence( QString );
    void editIncidence( Incidence* );
    void jumpToTime( const QDate &);
    void showAgendaView( bool );
    void todoCompleted(Todo*); 
    void signalViewerClosed();  
private slots:
    void slotViewerClosed();
    void editIncidence();
    void showIncidence();
 protected:
    void hideEvent ( QHideEvent * e );
  private:
    bool sendSignalViewerClosed;  
    bool mSyncMode;
    int mSyncResult;
    KOEventViewer *mEventViewer;
    Incidence* mIncidence;
    void keyPressEvent ( QKeyEvent * e );
};

#endif
