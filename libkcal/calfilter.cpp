/*
    This file is part of libkcal.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include "calfilter.h"

using namespace KCal;

CalFilter::CalFilter()
{
  mEnabled = true;
  mCriteria = ShowPublic | ShowPrivate| ShowConfidential ;
}

CalFilter::CalFilter(const QString &name)
{
  mName = name; 
  mEnabled = true;
  mCriteria = ShowPublic | ShowPrivate| ShowConfidential ;
}

CalFilter::~CalFilter()
{
}

void CalFilter::apply(QPtrList<Event> *eventlist)
{
  if (!mEnabled) return;

//  kdDebug(5800) << "CalFilter::apply()" << endl;

  Event *event = eventlist->first();
  while(event) {
    if (!filterEvent(event)) {
      eventlist->remove();
      event = eventlist->current();
    } else {
      event = eventlist->next();
    }
  }

//  kdDebug(5800) << "CalFilter::apply() done" << endl;
}

// TODO: avoid duplicating apply() code
void CalFilter::apply(QPtrList<Todo> *eventlist)
{
  if (!mEnabled) return;
  Todo *event = eventlist->first();
  while(event) {
    if (!filterTodo(event)) {
      eventlist->remove();
      event = eventlist->current();
    } else {
      event = eventlist->next();
    }
  }

//  kdDebug(5800) << "CalFilter::apply() done" << endl;
}
bool CalFilter::filterCalendarItem(Incidence *in)
{
    if ( !in->calEnabled() )
        return false;
    if ( in->typeID() == eventID )
        return filterEvent( (Event*) in );
    else if ( in->typeID() == todoID )
        return filterTodo( (Todo*) in);
    else if ( in->typeID () == journalID )
        return filterJournal( (Journal*) in );
    return false;
}
bool CalFilter::filterEvent(Event *event)
{
 
    if ( !event->calEnabled() )
        return false;
    if (mCriteria & HideEvents) 
        return false;
  if (mCriteria & HideRecurring) {
    if (event->recurrence()->doesRecur()) return false;
  }

  return filterIncidence(event);
}
bool CalFilter::filterJournal(Journal *j)
{
  if ( !j->calEnabled() )
        return false;
  if (mCriteria & HideJournals) 
        return false;
  return true;
}
bool CalFilter::filterTodo(Todo *todo)
{
  if ( !todo->calEnabled() )
        return false;
  if (mCriteria & HideTodos) 
        return false;
  if (mCriteria & HideCompleted) {
    if (todo->isCompleted()) return false;
  }

  return filterIncidence(todo);
}
bool CalFilter::showCategories()
{
    return mCriteria & ShowCategories;
}
int  CalFilter::getSecrecy()
{
    if ( (mCriteria & ShowPublic ))
        return Incidence::SecrecyPublic;
    if ( (mCriteria & ShowPrivate ))
        return Incidence::SecrecyPrivate;
    if ( (mCriteria & ShowConfidential ))
        return Incidence::SecrecyConfidential;
    return Incidence::SecrecyPublic;
}
bool CalFilter::filterIncidence(Incidence *incidence)
{
    if ( mCriteria > 7 ) {
    switch (incidence->secrecy()) {
    case Incidence::SecrecyPublic: 
        if (! (mCriteria & ShowPublic ))
            return false;
                break;
    case Incidence::SecrecyPrivate:
        if (! (mCriteria & ShowPrivate ))
            return false;
        break;
    case Incidence::SecrecyConfidential:
        if (! (mCriteria & ShowConfidential ))
            return false;
        break;
    default:
        return false;
        break;
    }
    }

    //  kdDebug(5800) << "CalFilter::filterEvent(): " << event->getSummary() << endl;

  if (mCriteria & ShowCategories) {
    for (QStringList::Iterator it = mCategoryList.begin();
         it != mCategoryList.end(); ++it ) {
        //QStringList incidenceCategories = incidence->categories();
      for (QStringList::Iterator it2 = incidence->categoriesP()->begin();
           it2 != incidence->categoriesP()->end(); ++it2 ) {
        if ((*it) == (*it2)) {
          return true;
        }
      }
    }
    return false;
  } else {
    for (QStringList::Iterator it = mCategoryList.begin();
         it != mCategoryList.end(); ++it ) {
        //QStringList incidenceCategories = incidence->categories();
      for (QStringList::Iterator it2 = incidence->categoriesP()->begin();
           it2 != incidence->categoriesP()->end(); ++it2 ) {
        if ((*it) == (*it2)) {
          return false;
        }
      }
    }
    return true;
  }
    
//  kdDebug(5800) << "CalFilter::filterEvent(): passed" << endl;
  
  return true;
}

void CalFilter::setEnabled(bool enabled)
{
  mEnabled = enabled;
}

bool CalFilter::isEnabled()
{
  return mEnabled;
}

void CalFilter::setCriteria(int criteria)
{
  mCriteria = criteria;
}

int CalFilter::criteria()
{
  return mCriteria;
}

void CalFilter::setCategoryList(const QStringList &categoryList)
{
  mCategoryList = categoryList;
}

QStringList CalFilter::categoryList()
{
  return mCategoryList;
}
