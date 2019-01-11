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
*/
#ifndef KOWHATSNEXTVIEW_H
#define KOWHATSNEXTVIEW_H

#include <qtextbrowser.h>
#include <qtimer.h>

#include <korganizer/baseview.h>

class QListView;
class QLabel;

class KOEventViewerDialog;

#include <qpainter.h>
#include <qwidget.h>
class WhatsNextTextBrowser : public QTextBrowser {
    Q_OBJECT
  public:
    WhatsNextTextBrowser(QWidget *parent) : QTextBrowser(parent) {};

    void setSource(const QString &);
    void printMe();

  signals:
    void showIncidence(const QString &uid);
};


/**
  This class provides a view of the next events and todos
*/
class KOWhatsNextView : public KOrg::BaseView
{
    Q_OBJECT
  public:
    KOWhatsNextView(Calendar *calendar, QWidget *parent = 0, 
	            const char *name = 0);
    ~KOWhatsNextView();

    virtual int maxDatesHint();
    virtual int currentDateCount();
    void setEventViewer(KOEventViewerDialog* v );
    virtual QPtrList<Incidence> selectedIncidences();
    void clearList();
    DateList selectedDates()
      {DateList q;
       return q;}    
    virtual void printPreview(CalPrinter *calPrinter,
                              const QDate &, const QDate &);
  
  public slots:
    virtual void updateView();
    void printMe();
    virtual void showDates(const QDate &start, const QDate &end);
    virtual void showEvents(QPtrList<Event> eventList);
    void updateConfig();
    void changeEventDisplay(Event *, int);
  
  protected:
    bool appendEvent(Incidence *, bool reply=false, bool notRed = true, bool appendTable = false);
    bool appendTodo(Incidence *, QString ind = "", bool isSub = false );
    void appendDay( int i, QDate date );
    QDate mEventDate; 
    virtual void showEvent ( QShowEvent * );
    virtual void hideEvent ( QHideEvent * ); 
  
  private slots:
    void showIncidence(const QString &);
    void restartTimer();


  private:
    int mCurrentMaxPrio;
  //void createEventViewer();
    QTimer* mTimer;
    WhatsNextTextBrowser *mView;
    QString mText;
    // QLabel *mDateLabel;
    KOEventViewerDialog *mEventViewer;
    
    QValueList<Incidence *> mTodos;
};

#endif
