/*
  This file is part of KOrganizer.
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

#include <qcursor.h>

#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <libkcal/event.h>

#include "koeventpopupmenu.h"
#include "koprefs.h"

KOEventPopupMenu::KOEventPopupMenu(): QPopupMenu()
{
    mCurrentIncidence = 0;
    mHasAdditionalItems = false;

  
    mSingleOnlyItems.append( insertItem (i18n("&Show"),this,SLOT(popupShow())));
    mEditOnlyItems.append(insertItem (i18n("&Edit..."),this,SLOT(popupEdit())));
    mEditOnlyItems.append(insertItem (i18n("&Delete..."),
                                      this,SLOT(popupDelete())));
    mEditOnlyItems.append(insertItem (i18n("&Clone..."),
                                      this,SLOT(popupClone())));
    mEditOnlyItems.append(insertItem (i18n("&Move..."),
                                      this,SLOT(popupMove())));
#ifndef DESKTOP_VERSION
    mEditOnlyItems.append(insertItem (i18n("&Beam..."),
                                      this,SLOT(popupBeam())));
#endif
    mEditOnlyItems.append(insertItem (i18n("&Toggle Cancel"),
                                      this,SLOT(popupCancel())));
    isDisabled = false;  
    mCatPopup = new QPopupMenu ( this );
    mCatPopup->setCheckable (true);
    connect(mCatPopup,SIGNAL( aboutToShow ()), this ,SLOT( fillCatPopup()));
    connect(mCatPopup,SIGNAL(  activated ( int ) ), this ,SLOT( computeCatPopup( int )));
    mCalPopup = new QPopupMenu ( this );
    mCalPopup->setCheckable (true);
    connect(mCalPopup,SIGNAL( aboutToShow ()), this ,SLOT( fillCalPopup()));
    connect(mCalPopup,SIGNAL(  activated ( int ) ), this ,SLOT( computeCalPopup( int )));
    //mEditOnlyItems.append(insertItem (i18n("Categories"),mCatPopup ));
    //mEditOnlyItems.append(insertItem (i18n("Calendar"),mCalPopup ));
    insertItem (i18n("Categories"),mCatPopup );
    insertItem (i18n("Calendar"),mCalPopup );
    QValueList<int>::Iterator it;
    for( it = mEditOnlyItems.begin(); it != mEditOnlyItems.end(); ++it ) {
        mSingleOnlyItems.append(*it);
    }

}
void KOEventPopupMenu::enableDefault( bool enable )
{
    isDisabled = !enable;
    QValueList<int>::Iterator it;
    for( it = mSingleOnlyItems.begin(); it != mSingleOnlyItems.end(); ++it ) {
        setItemEnabled(*it,enable);
    }
}

void KOEventPopupMenu::fillCalPopup() // CAL
{
    mCalPopup->clear();
    if (!mCurrentIncidence) return;
    bool readO = mCurrentIncidence->isReadOnly()|| isDisabled;
    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    while ( kkf ) {
        int index = mCalPopup->insertItem( kkf->mName, kkf->mCalNumber);
        if ( kkf->mErrorOnLoad || kkf->isReadOnly || readO )
            mCalPopup->setItemEnabled( index, false );
        mCalPopup->setItemChecked (index, kkf->mCalNumber == mCurrentIncidence->calID());
        kkf = KOPrefs::instance()->mCalendars.next();
    }
}
void KOEventPopupMenu::computeCalPopup( int index ) // CAL
{
    if (!mCurrentIncidence) return;
    mCurrentIncidence->setCalID( index );
    emit categoryChanged( mCurrentIncidence );
}
void KOEventPopupMenu::fillCatPopup()
{
    mCatPopup->clear(); 
    if (!mCurrentIncidence) return;
    bool readO = mCurrentIncidence->isReadOnly() || isDisabled;
    QStringList checkedCategories = mCurrentIncidence->categories();
    int index = 0;
    for (QStringList::Iterator it = KOPrefs::instance()->mCustomCategories.begin ();
         it != KOPrefs::instance()->mCustomCategories.end ();
         ++it) {
        mCatPopup->insertItem (*it, index );
        if (checkedCategories.find (*it) != checkedCategories.end ()) {
            mCatPopup->setItemChecked (index, true);
        }
        if ( readO )
            mCatPopup->setItemEnabled( index, false );
        ++index;
    }
}
void KOEventPopupMenu::computeCatPopup( int index )
{ 
    if (!mCurrentIncidence) return;
    QStringList categories =  mCurrentIncidence->categories();
    QString colcat = categories.first();
    if (categories.find (KOPrefs::instance()->mCustomCategories[index]) != categories.end ())
        categories.remove (KOPrefs::instance()->mCustomCategories[index]);
    else
        categories.insert (categories.end(), KOPrefs::instance()->mCustomCategories[index]);
    categories.sort ();
    if ( !colcat.isEmpty() ) {
        if ( categories.find ( colcat ) != categories.end () ) {
            categories.remove( colcat );
            categories.prepend( colcat );
        }
    }
    mCurrentIncidence->setCategories( categories );
    emit categoryChanged( mCurrentIncidence );
}
void KOEventPopupMenu::showIncidencePopup(Incidence *incidence)
{
    if ( !incidence) return;
    mCurrentIncidence = incidence;
  
    if (mCurrentIncidence) {
        // Enable/Disabled menu items only valid for editable events.
        if ( !isDisabled  ) {
            QValueList<int>::Iterator it;
            for( it = mEditOnlyItems.begin(); it != mEditOnlyItems.end(); ++it ) {
                setItemEnabled(*it,!mCurrentIncidence->isReadOnly());
            }
        }
        popup(QCursor::pos());
    }
}

void KOEventPopupMenu::addAdditionalItem(const QIconSet &icon,const QString &text,
                                         const QObject *receiver, const char *member,
                                         bool editOnly)
{
    if (!mHasAdditionalItems) {
        mHasAdditionalItems = true;
        insertSeparator();
    }
    int id = insertItem(icon,text,receiver,member);
    if (editOnly) mEditOnlyItems.append(id);
}

void KOEventPopupMenu::popupShow()
{
    if (mCurrentIncidence) emit showIncidenceSignal(mCurrentIncidence);
}

void KOEventPopupMenu::popupEdit()
{
    if (mCurrentIncidence) emit editIncidenceSignal(mCurrentIncidence);
}

void KOEventPopupMenu::popupDelete()
{
    if (mCurrentIncidence) emit deleteIncidenceSignal(mCurrentIncidence);
}
void KOEventPopupMenu::popupClone()
{
    if (mCurrentIncidence) emit cloneIncidenceSignal(mCurrentIncidence);
}
void KOEventPopupMenu::popupCancel()
{
    if (mCurrentIncidence) emit cancelIncidenceSignal(mCurrentIncidence);
}
void KOEventPopupMenu::popupMove()
{
    if (mCurrentIncidence) emit moveIncidenceSignal(mCurrentIncidence);
}

void KOEventPopupMenu::popupBeam()
{
    if (mCurrentIncidence) emit beamIncidenceSignal(mCurrentIncidence);
}
