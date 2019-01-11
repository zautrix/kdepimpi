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

#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>
#include <qregexp.h>
#include <qfileinfo.h>

#include "calendarlocal.h"
#include "icalformat.h"
#include "todo.h"

#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#endif

#define SAVETIMER_TIMEOUT_SECONDS 300
//#define SAVETIMER_TIMEOUT_SECONDS 8
#define SAVETIMER_TIMEOUT_RETRY_SECONDS 5

using namespace KCal;

Todo::Todo():  QObject(), Incidence()
{
//  mStatus = TENTATIVE;

  mHasDueDate = false;
  setHasStartDate( false );
  mCompleted = getEvenTime(QDateTime::currentDateTime());
  mHasCompletedDate = false;
  mPercentComplete = 0;
  mRunning = false;
  mRunSaveTimer = 0;
  setFloats( true );
  mCurrentTimerDelay = 0;
}

Todo::Todo(const Todo &t) : QObject(),Incidence(t)
{
  mDtDue = t.mDtDue;
  mHasDueDate = t.mHasDueDate;
  mCompleted = t.mCompleted;
  mHasCompletedDate = t.mHasCompletedDate;
  mPercentComplete = t.mPercentComplete;
  mRunning = false;
  mRunSaveTimer = 0;
  mCurrentTimerDelay = 0;
}

Todo::~Todo()
{
    setRunning( false );
    //qDebug("Todo::~Todo() ");
}

void Todo::setRunningFalse( QString s )
{
    if ( ! mRunning )
        return;
    mRunning = false;
    if ( mRunSaveTimer )
        mRunSaveTimer->stop();
    saveRunningInfoToFile( s );
}
void Todo::stopRunning()
{
    if ( !mRunning )
        return;
    if ( mRunSaveTimer )
        mRunSaveTimer->stop();
    mRunning = false;
}
void Todo::setRunning( bool run )
{
    if ( run == mRunning )
        return;
    //qDebug("Todo::setRunning %d ", run);
    if ( !mRunSaveTimer ) {
        mRunSaveTimer = new QTimer ( this );
        connect ( mRunSaveTimer, SIGNAL( timeout() ), this , SLOT ( timerSlotSaveRunningInfoToFile() ) );
    }
    mRunning = run;
    mRunLastSave = QDateTime::currentDateTime();
    if ( mRunning ) {
        restartSaveTimer( SAVETIMER_TIMEOUT_SECONDS );
        mRunStart = QDateTime::currentDateTime();
    } else {
        mRunSaveTimer->stop();
        saveRunningInfoToFile();
    }
    mLastSavedFileName = "";
}
void Todo::saveRunningInfo( QString comment, QDateTime start, QDateTime end )
{
    if ( !mRunning) return;
    if ( mRunSaveTimer )
        mRunSaveTimer->stop();
    mRunning = false;
    mRunStart = start;
    mRunEnd = end;
    saveRunningInfoToFile( comment );
}
void Todo::restartSaveTimer( int secs )
{
    mRunSaveTimer->start( secs * 1000 ); 
    mRunLastSave = QDateTime::currentDateTime();
    mCurrentTimerDelay = secs;
}
void Todo::timerSlotSaveRunningInfoToFile()
{
    mRunEnd = QDateTime::currentDateTime();
    int secsTo = mRunLastSave.secsTo( mRunEnd );
    //if( secsTo == 8 ) ++secsTo;
    qDebug("KO Todo::saveTimerTimeout %d %d",  secsTo, mCurrentTimerDelay );
    if ( secsTo > mCurrentTimerDelay  ) {
        qDebug("KO Todo::saveTimerTimeout restart %d ", SAVETIMER_TIMEOUT_RETRY_SECONDS );
        restartSaveTimer( SAVETIMER_TIMEOUT_RETRY_SECONDS );
        return;
    }
    int msecs =  mRunLastSave.time().msecsTo( mRunEnd.time());
    if ( msecs < 0 ) {
        restartSaveTimer( SAVETIMER_TIMEOUT_RETRY_SECONDS );
        return;
    }  
    // qDebug("KO Todo::saveTimerTimeout restarting! millisecs %d",  msecs - ( ( mCurrentTimerDelay * 1000 ) + 50 ) );
    if ( msecs > ( ( mCurrentTimerDelay * 1000 ) + 50 )) {
        qDebug("KO Todo::saveTimerTimeout restarting! millisecs %d",  msecs - ( ( mCurrentTimerDelay * 1000 ) + 50 ) );
        restartSaveTimer( SAVETIMER_TIMEOUT_RETRY_SECONDS );
        return;
    }
    restartSaveTimer( SAVETIMER_TIMEOUT_SECONDS );
    saveRunningInfoToFile( QString::null );
}
void Todo::saveRunningInfoToFile()
{
    mRunEnd = QDateTime::currentDateTime();
    saveRunningInfoToFile( QString::null );
}
void Todo::saveRunningInfoToFile( QString comment )
{
#ifndef DESKTOP_VERSION
    //QPEApplication::setTempScreenSaverMode ( QPEApplication::Disable );
#endif
    //qDebug("Todo::saveRunningInfoToFile() %s", summary().latin1());
    if ( mRunStart.secsTo ( mRunEnd) < 15   )  {
        qDebug("Running time < 15 seconds. Skipped. ");
        return;
    }
    QString dir = KGlobalSettings::timeTrackerDir();
    //qDebug("%s ", dir.latin1());
    QString file = "%1%2%3-%4%5%6-";
    file = file.arg( mRunStart.date().year(), 4).arg( mRunStart.date().month(),2 ).arg( mRunStart.date().day(), 2 ).arg( mRunStart.time().hour(),2  ).arg( mRunStart.time().minute(),2 ).arg( mRunStart.time().second(),2 );
    file.replace ( QRegExp (" "), "0" ); 
    file += uid();
    //qDebug("File %s ",file.latin1() );
    CalendarLocal cal;
    cal.setLocalTime();
    Todo  * to = (Todo*) clone();
    to->setFloats( false );
    to->setDtStart( mRunStart );
    to->setHasStartDate( true );
    to->setDtDue( mRunEnd );
    to->setHasDueDate( true );
    to->setUid( file );
    if ( !comment.isEmpty() ) {
        QString des = to->description();
        if ( des.isEmpty () )
            to->setDescription( "TT-Note: " + comment );
        else
            to->setDescription( "TT-Note: " + comment +"\n" +  des );
    }
    cal.addIncidence( to );
    ICalFormat format( false );
    file = dir +"/" +file +".ics";
    format.save( &cal, file );
    saveParents();
    if ( !mLastSavedFileName.isEmpty() ) {
        if ( mLastSavedFileName != file ) {
            QFile::remove( mLastSavedFileName ); 
        }
    }
    mLastSavedFileName = file;
#ifndef DESKTOP_VERSION
    //QPEApplication::setTempScreenSaverMode ( QPEApplication::Enable );
#endif   
}
void Todo::saveParents()
{
    if (!relatedTo() )
        return;
    Incidence * inc = relatedTo();
    if ( inc->typeID() != todoID )
        return;
    Todo* to = (Todo*)inc;
    bool saveTodo = false;
    QString file = KGlobalSettings::timeTrackerDir() + "/"+ to->uid() + ".ics";
    QFileInfo fi ( file );
    if ( fi.exists() ) {
        if ( fi.lastModified () < to->lastModified ())
            saveTodo = true;
    } else {
        saveTodo = true;
    }
    if ( saveTodo ) {
        CalendarLocal cal;
        cal.setLocalTime();
        Todo  * par = (Todo  *) to->clone();
        cal.addIncidence( par );
        ICalFormat format( false );
        format.save( &cal, file );
    } 
    to->saveParents();
}

int Todo::runTime()
{
    if ( !mRunning )
        return 0;
    return  mRunStart.secsTo( QDateTime::currentDateTime() );
}
bool Todo::hasRunningSub()
{
    if ( mRunning )
        return true;
    Incidence *aTodo;
    for (aTodo = mRelations.first(); aTodo; aTodo = mRelations.next()) {
        if ( ((Todo*)aTodo)->hasRunningSub() )
            return true;
    }
    return false;
}
Incidence *Todo::clone()
{
  return new Todo(*this);
}

bool Todo::contains ( Todo* from )
{ 

    if ( !from->summary().isEmpty() )
        if (  !summary().startsWith(  from->summary() ))
            return false;
    if ( from->hasStartDate() ) {
        if ( !hasStartDate() )
            return false;
        if ( from->dtStart() != dtStart())
            return false;      
    }
    if ( from->hasDueDate() ){
        if ( !hasDueDate() )
            return false;
        if ( from->dtDue() != dtDue())
            return false;  
    }
    if ( !from->location().isEmpty() )
        if (  !location().startsWith( from->location() ) )
            return false;
    if ( !from->description().isEmpty() )
        if (  !description().startsWith(  from->description() ))
            return false;
     if ( from->alarms().count() ) {
        Alarm *a = from->alarms().first();
        if (  a->enabled()  ){
            if ( !alarms().count() )
                return false;
            Alarm *b = alarms().first();
            if( ! b->enabled() )
                return false;
            if ( ! (a->offset() == b->offset() ))
                return false;
        }
    }
   
    QStringList cat = categories();
    QStringList catFrom = from->categories();
    QString nCat;
    unsigned int iii;
    for ( iii = 0; iii < catFrom.count();++iii ) {
        nCat = catFrom[iii];
        if ( !nCat.isEmpty() )
            if ( !cat.contains( nCat )) {
                return false;
            }
    }
    if ( from->isCompleted() ) {
        if ( !isCompleted() )
            return false;
    }
    if( priority() !=  from->priority() )
        return false;


    return true;

}
bool KCal::operator==( const Todo& t1, const Todo& t2 )
{

    bool ret = operator==( (const Incidence&)t1, (const Incidence&)t2 );
    if ( ! ret )
        return false;
    if (  t1.hasDueDate() == t2.hasDueDate() ) {
        if ( t1.hasDueDate() ) {
            if ( t1.doesFloat() == t2.doesFloat() ) {
                if ( t1.doesFloat() ) {
                    if ( t1.dtDue().date() != t2.dtDue().date() )
                        return false;
                } else
                    if ( t1.dtDue() != t2.dtDue() ) 
                        return false;
            } else
                return false;// float !=
        }
        
    } else
        return false;
    if ( t1.percentComplete() != t2.percentComplete() )
        return false;
    if ( t1.isCompleted() ) {
        if (  t1.hasCompletedDate() == t2.hasCompletedDate() ) {
            if ( t1.hasCompletedDate() ) {
                if ( t1.completed() != t2.completed() )
                    return false;
            }
            
        } else
            return false;
    }
    return true;

}

void Todo::setDtDue(const QDateTime &dtDue)
{
  //int diffsecs = mDtDue.secsTo(dtDue);

  /*if (mReadOnly) return;
  const QPtrList<Alarm>& alarms = alarms();
  for (Alarm* alarm = alarms.first(); alarm; alarm = alarms.next()) {
    if (alarm->enabled()) {
      alarm->setTime(alarm->time().addSecs(diffsecs));
    }
  }*/
  mDtDue = getEvenTime(dtDue);

  //kdDebug(5800) << "setDtDue says date is " << mDtDue.toString() << endl;

  /*const QPtrList<Alarm>& alarms = alarms();
  for (Alarm* alarm = alarms.first(); alarm; alarm = alarms.next())
    alarm->setAlarmStart(mDtDue);*/
  updated();
}

QDateTime Todo::dtDue() const
{
  return mDtDue;
}

QString Todo::dtDueTimeStr() const
{
  return KGlobal::locale()->formatTime(mDtDue.time());
}

QString Todo::dtDueDateStr(bool shortfmt) const
{
  return KGlobal::locale()->formatDate(mDtDue.date(),shortfmt);
}

QString Todo::dtDueStr(bool shortfmt) const
{
    if ( doesFloat() )
        return KGlobal::locale()->formatDate(mDtDue.date(),shortfmt);
    return KGlobal::locale()->formatDateTime(mDtDue, shortfmt);
}
// retval 0 : no found
//        1 : due for date found
//        2 : overdue for date found
int Todo::hasDueSubTodoForDate( const QDate & date, bool checkSubtodos  )
{
    int retval = 0;
    if ( isCompleted() )
        return 0;
    if ( cancelled() )
        return 0;
    if ( hasDueDate() ) {
        if ( dtDue().date() < date )
            return 2;
        // we do not return, because we may find an overdue sub todo
        if ( dtDue().date() == date )
            retval = 1;
    }
    if ( checkSubtodos ) {
        Incidence *aTodo;
        for (aTodo = mRelations.first(); aTodo; aTodo = mRelations.next()) {
            int ret = ((Todo*)aTodo)->hasDueSubTodoForDate( date ,checkSubtodos );
            if ( ret == 2 )
                return 2;
            if ( ret == 1)
                retval = 1;
        }
    }
    return retval;
}
int Todo::hasDueSubTodo( bool checkSubtodos  ) //= true
{  
    return hasDueSubTodoForDate(QDate::currentDate(), checkSubtodos );
}
bool Todo::hasDueDate() const
{
  return mHasDueDate;
}

void Todo::setHasDueDate(bool f)
{
  if (mReadOnly) return;
  mHasDueDate = f;
  updated();
}


#if 0
void Todo::setStatus(const QString &statStr)
{
  if (mReadOnly) return;
  QString ss(statStr.upper());

  if (ss == "X-ACTION")
    mStatus = NEEDS_ACTION;
  else if (ss == "NEEDS ACTION")
    mStatus = NEEDS_ACTION;
  else if (ss == "ACCEPTED")
    mStatus = ACCEPTED;
  else if (ss == "SENT")
    mStatus = SENT;
  else if (ss == "TENTATIVE")
    mStatus = TENTATIVE;
  else if (ss == "CONFIRMED")
    mStatus = CONFIRMED;
  else if (ss == "DECLINED")
    mStatus = DECLINED;
  else if (ss == "COMPLETED")
    mStatus = COMPLETED;
  else if (ss == "DELEGATED")
    mStatus = DELEGATED;

  updated();
}

void Todo::setStatus(int status)
{
  if (mReadOnly) return;
  mStatus = status;
  updated();
}

int Todo::status() const
{
  return mStatus;
}

QString Todo::statusStr() const
{
  switch(mStatus) {
  case NEEDS_ACTION:
    return QString("NEEDS ACTION");
    break;
  case ACCEPTED:
    return QString("ACCEPTED");
    break;
  case SENT:
    return QString("SENT");
    break;
  case TENTATIVE:
    return QString("TENTATIVE");
    break;
  case CONFIRMED:
    return QString("CONFIRMED");
    break;
  case DECLINED:
    return QString("DECLINED");
    break;
  case COMPLETED:
    return QString("COMPLETED");
    break;
  case DELEGATED:
    return QString("DELEGATED");
    break;
  }
  return QString("");
}
#endif

bool Todo::isCompleted() const
{
    if (mPercentComplete == 100) {
        return true;
    }
    else return false;
}

void Todo::setCompleted(bool completed)
{
    if ( mHasRecurrenceID && completed && mPercentComplete != 100 ) {
        if ( !setRecurDates() )
            completed = false;
    }
  if (completed) mPercentComplete = 100;
  else {
      mPercentComplete = 0;
      mHasCompletedDate = false;
  }
  updated();
}

QDateTime Todo::completed() const
{
  return mCompleted;
}

QString Todo::completedStr( bool shortF ) const
{
    return KGlobal::locale()->formatDateTime(mCompleted, shortF);
}

void Todo::setCompleted(const QDateTime &completed)
{
    //qDebug("Todo::setCompleted ");
    if ( mHasCompletedDate ) {
        // qDebug("has completed data - return ");
        return;
    }
  mHasCompletedDate = true;
  mPercentComplete = 100;
  mCompleted = getEvenTime(completed);
  updated();
}

bool Todo::hasCompletedDate() const
{
  return mHasCompletedDate;
}

int Todo::percentComplete() const
{
  return mPercentComplete;
}
bool Todo::setRecurDates()
{
    if ( !mHasRecurrenceID )
        return true;
    int secs = mDtStart.secsTo( dtDue() );   
    bool ok = true;
    //qDebug("Todo: setRecurDates() ");
    //qDebug("%s %s %s ",mDtStart.toString().latin1(), dtDue().toString().latin1(),mRecurrenceID.toString().latin1() );
    QDateTime next = getNextOccurence(  mRecurrenceID, &ok );
    if ( ok ) { 
        mRecurrenceID = next;
        mDtStart = next;
        setDtDue( next.addSecs( secs ) );
        if ( QDateTime::currentDateTime() > next) 
            return false;
    } else {
        setHasRecurrenceID( false );
        recurrence()->unsetRecurs();
    }
    return true;
}
void Todo::setPercentComplete(int v)
{ 
    if ( mHasRecurrenceID && v == 100 && mPercentComplete != 100 ) {
        if ( !setRecurDates() )
            v = 0;
    }
  mPercentComplete = v;
  if ( v != 100 )
      mHasCompletedDate = false;
  updated();
}
QDateTime Todo::getNextAlarmDateTime( bool * ok, int * offset, QDateTime start_dt ) const
{
    *ok = false;
    if ( ! hasDueDate() || cancelled() || !alarmEnabled() ) {
         return QDateTime ();
     }
    // if the recurring todo is set to complete and requested time < start time of todo
    // we want to get the alarm.
    bool iscompleted = isCompleted();
    if ( iscompleted && doesRecur() ) {
        Todo * to = (Todo*) this;
        to->checkSetCompletedFalse();
        iscompleted = isCompleted();
        if ( hasStartDate() && start_dt < dtStart() ){
            iscompleted = false;
        }
    }
    if ( iscompleted ) {
        return QDateTime ();
    }
     QDateTime incidenceStart;
     incidenceStart = dtDue(); 
     bool enabled = false;
     Alarm* alarm;
     int off = 0;
     QDateTime alarmStart = QDateTime::currentDateTime().addDays( 3650 );;
    //  if ( QDateTime::currentDateTime() > incidenceStart ){
//          *ok = false;
//          return incidenceStart;
//      }
     for (QPtrListIterator<Alarm> it(mAlarms); (alarm = it.current()) != 0; ++it) {
         if (alarm->enabled()) {
             if ( alarm->hasTime () ) {
                 if ( alarm->time() < alarmStart ) {
                     alarmStart = alarm->time();
                     enabled = true;
                     off = alarmStart.secsTo( incidenceStart );
                 }

             } else {
                 int secs = alarm->startOffset().asSeconds();
                 if ( incidenceStart.addSecs( secs ) < alarmStart ) {
                     alarmStart = incidenceStart.addSecs( secs );
                     enabled = true;
                     off = -secs;
                 }
             }
         }
     }
     if ( enabled ) {
         if ( alarmStart >  start_dt  ) {
             *ok = true;
             * offset = off;
             return alarmStart;
         }
     }
     *ok = false;
     return QDateTime ();

}

void Todo::checkSetCompletedFalse()
{
    if ( !mHasRecurrenceID ) {
        //qDebug("Todo::checkSetCompletedFalse called with mHasRecurrenceID==false");
        return;
    }
    // qDebug("Todo::checkSetCompletedFalse()");
    //qDebug("%s %s %s ",mDtStart.toString().latin1(), dtDue().toString().latin1(),mRecurrenceID.toString().latin1() );
    if ( mPercentComplete == 100 ) {
        QDateTime dt = QDateTime::currentDateTime();
        if ( dt > mDtStart && dt > mRecurrenceID ) {
            //qDebug("start: %s --due: %s --recID: %s ",mDtStart.toString().latin1(), dtDue().toString().latin1(),mRecurrenceID.toString().latin1() );
            //we want to set todo to not completed
            //but we cannot call 
            //setCompleted( false );
            //here because we do not want to call updated() abd change the last modified timestamp

            //For that reason we reimplemet here void Todo::setCompleted(bool completed)

            mPercentComplete = 0;
            mHasCompletedDate = false;

            //qDebug("Todo::checkSetCompletedFalse ");
        }
    }
}
QString Todo::durationText()
{
    if ( mHasDueDate && hasStartDate() ) {
        int sec = dtStart().secsTo( dtDue() );
        if ( doesFloat() )
            sec += 86400;
        return durationText4Time( sec );
    }
    return "---";
}
