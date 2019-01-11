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

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <qpopupmenu.h>
#include <qcursor.h>

#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include <libkcal/calendar.h>
#include "koprefs.h"
#include "koeventview.h"
using namespace KOrg;

//---------------------------------------------------------------------------

KOEventView::KOEventView(Calendar *cal,QWidget *parent,const char *name)
  : KOrg::BaseView(cal,parent,name)
{
}

//---------------------------------------------------------------------------

KOEventView::~KOEventView()
{
}

//---------------------------------------------------------------------------

KOEventPopupMenu *KOEventView::eventPopup()
{
  KOEventPopupMenu *eventPopup = new KOEventPopupMenu;
  
  connect(eventPopup,SIGNAL(editIncidenceSignal(Incidence *)),
                     SIGNAL(editIncidenceSignal(Incidence *)));
  connect(eventPopup,SIGNAL(showIncidenceSignal(Incidence *)),
                     SIGNAL(showIncidenceSignal(Incidence *)));
  connect(eventPopup,SIGNAL(deleteIncidenceSignal(Incidence *)),
                     SIGNAL(deleteIncidenceSignal(Incidence *)));
  connect(eventPopup,SIGNAL(cancelIncidenceSignal(Incidence *)),
                     SIGNAL(cancelIncidenceSignal(Incidence *)));
  connect(eventPopup,SIGNAL(cloneIncidenceSignal(Incidence *)),
                     SIGNAL(cloneIncidenceSignal(Incidence *)));
  connect(eventPopup,SIGNAL(beamIncidenceSignal(Incidence *)),
                     SIGNAL(beamIncidenceSignal(Incidence *)));
  connect(eventPopup,SIGNAL(moveIncidenceSignal(Incidence *)),
                     SIGNAL(moveIncidenceSignal(Incidence *)));

  return eventPopup;
}

//---------------------------------------------------------------------------

void KOEventView::showIncidencePopup(QPopupMenu *popup,Incidence *event)
{
  mCurrentIncidence = event;
  if (event) popup->popup(QCursor::pos());
}

//---------------------------------------------------------------------------

void KOEventView::popupShow()
{
  emit showIncidenceSignal(mCurrentIncidence);
}

//---------------------------------------------------------------------------

void KOEventView::popupEdit()
{
  emit editIncidenceSignal(mCurrentIncidence);
}

//---------------------------------------------------------------------------

void KOEventView::popupDelete()
{
  emit deleteIncidenceSignal(mCurrentIncidence);
}
void KOEventView::popupClone()
{
  emit cloneIncidenceSignal(mCurrentIncidence);
}
void KOEventView::popupCancel()
{
  emit cancelIncidenceSignal(mCurrentIncidence);
}

//---------------------------------------------------------------------------

void KOEventView::defaultAction( Incidence *incidence )
{

  if ( !incidence ) return;

  if ( incidence->isReadOnly() )
    emit showIncidenceSignal(incidence);
  else {
      if ( KOPrefs::instance()->mEditOnDoubleClick )
          emit editIncidenceSignal(incidence);
      else
          emit showIncidenceSignal(incidence);
  }
}

//---------------------------------------------------------------------------

