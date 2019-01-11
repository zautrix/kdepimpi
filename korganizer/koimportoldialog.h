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
#ifndef KOINCIDENCEEDITOR_H
#define KOINCIDENCEEDITOR_H

#include <kdialogbase.h>

#include <afxdisp.h>  
#include <libkcal/calendar.h>
#include <libkcal/event.h>


class QDateTime;
class QListView;
class OLEListViewItem;
class _AppointmentItem;
namespace KPIM { class CategorySelectDialog; }

using namespace KCal;

/**
  This is the base class for the calendar component editors.
*/
class KOImportOLdialog : public KDialogBase
{
    Q_OBJECT
  public:
    /**
      Construct new IncidenceEditor.
    */
    KOImportOLdialog( const QString &caption, Calendar *calendar,
                       QWidget *parent );
    virtual ~KOImportOLdialog();

    /** Initialize editor. This function creates the tab widgets. */
    void init();
   
  public slots:
   

  signals:
  
  protected slots:
    void slotApply();
    void slotOk();
    void slotCancel();

  protected:
    void setupFolderView();
    void addFolder(OLEListViewItem* iParent, LPDISPATCH dispParent);
    void readCalendarData( DWORD folder );
    void ol2kopiCalendar( _AppointmentItem * , bool computeRecurrence = true );

    Calendar *mCalendar;
    QListView * mListView;
    QString getUidByEmail( QString email );

  private:
    int importedItems;
};

#endif


