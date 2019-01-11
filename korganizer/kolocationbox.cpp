/*
    Copyright (c) 2004 Dirk Loesche <dirk.loesche@bigfoot.de>

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

#include "kolocationbox.h"
#include <qstring.h>
#include <qlineedit.h>
#include <qapplication.h>


KOLocationBox::KOLocationBox( bool rw , QWidget *parent , int _maxItems) :
  QComboBox( rw , parent )
{
  maxItems = _maxItems;
  //maxItems = 50; // sorry - hack from me to set maxitems globally to 30
  setInsertionPolicy(AtTop);
  setDuplicatesEnabled( FALSE );
  setMaxCount( maxItems );
  setAutoCompletion( TRUE );  

}

KOLocationBox::~KOLocationBox()
{
}

void KOLocationBox::load(int what)
{
  clear();
  // qDebug("load %d ",what );
  switch(what) {
    case LOCATION:
        insertStringList( KOPrefs::instance()->mLocationDefaults, 0 );
//        insertStringList( KOPrefs::instance()->mLocationUserDefaults, 0 );
        break;  // don't disable
    case SUMMARYEVENT:
        insertStringList( KOPrefs::instance()->mEventSummaryUser, 0 );
        break;  // don't disable
    case SUMMARYTODO:
        insertStringList( KOPrefs::instance()->mTodoSummaryUser, 0 );
        break;  // don't disable
    case SUMMARYJOURNAL:
        insertStringList( KOPrefs::instance()->mJournalSummaryUser, 0 );
        break;  // don't disable
  }
}

void KOLocationBox::save(int what)
{
  strlist.clear();
  for( int l = 0; l < count() ; l++ ) {
    strlist << text( l );
  }
  // strlist.sort();
  QString currentLine = lineEdit()->text();
  if ( !strlist.contains( currentLine  ) )
      strlist.prepend( currentLine );
  // qDebug("save %d ", what);
  switch(what) {
    case LOCATION:
        KOPrefs::instance()->mLocationDefaults = strlist;
//        KOPrefs::instance()->mLocationUserDefaults = strlist;
        break; // don't disable
    case SUMMARYEVENT:
        KOPrefs::instance()->mEventSummaryUser = strlist;
        break;  // don't disable
    case SUMMARYTODO:
        KOPrefs::instance()->mTodoSummaryUser = strlist;
        break;  // don't disable
    case SUMMARYJOURNAL:
        KOPrefs::instance()->mJournalSummaryUser = strlist;
        break;  // don't disable
  }        
}

void KOLocationBox::clearItems(int what)
{
  clear();
  save(what);
}
