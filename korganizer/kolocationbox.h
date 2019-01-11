/*
    File Copyright (c) 2004 Dirk Loesche <dirk.loesche@bigfoot.de>

    This file is part of KOrganizer/PI
    KOrganizer    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
    KOrganizer/PI Copyright (c) 2004 Lutz Rogowski <zautrix@rogowski.info>
    
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

#ifndef KOLOCATIONBOX_H
#define KOLOCATIONBOX_H

#include <qcombobox.h>
#include "koprefs.h"

class KOLocationBox : public QComboBox
{
  public:
    KOLocationBox( bool rw , QWidget *parent , int _maxItems );
    virtual ~KOLocationBox();
    
    enum {LOCATION, SUMMARYEVENT, SUMMARYTODO,SUMMARYJOURNAL};
  
  public slots:
    void        load(int);
    void        save(int);
    void        clearItems(int); // clear listbox and config rc items

  protected:
//    void        focusOutEvent( QFocusEvent * );
//    void        timerEvent( QTimerEvent * );

  private:
    int         maxItems;       // maximal listbox items
    QStringList strlist;        // temporary stringlist
};

#endif
