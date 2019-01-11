/*
    This file is part of libkcal.

    Copyright (c) 2004 Lutz Rogowski <rogowski@kde.org>

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
#ifdef DESKTOP_VERSION
#include <qdesktopwidget.h>
#endif
#include <qdatetime.h>
#include <qstring.h>
#include <qapplication.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qdir.h>
#include <qlabel.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <phoneaccess.h>

#include "calendar.h"
#include "alarm.h"
#include "recurrence.h"
#include "calendarlocal.h"

#include "phoneformat.h"
#include "syncdefines.h"

using namespace KCal;
class PhoneParser : public QObject
{
public:
    PhoneParser(   )  {
        ;
    }
   
    static QString dtToString( const QDateTime& dti, bool useTZ = false )
    {
        QString datestr;
        QString timestr;
        int offset =  KGlobal::locale()->localTimeOffset( dti );
        QDateTime   dt;
        if (useTZ)
            dt =  dti.addSecs ( -(offset*60));
        else
            dt =  dti;
        if(dt.date().isValid()){
            const QDate& date = dt.date();
            datestr.sprintf("%04d%02d%02d",
                            date.year(), date.month(), date.day());
        }
        if(dt.time().isValid()){
            const QTime& time = dt.time();
            timestr.sprintf("T%02d%02d%02d",
                            time.hour(), time.minute(), time.second());
        }
        return datestr + timestr;
    }


};

  

PhoneFormat::PhoneFormat(QString profileName, QString device,QString connection, QString model )
{
    mProfileName = profileName;
    PhoneAccess::writeConfig( device, connection, model );
}

PhoneFormat::~PhoneFormat()
{
}

#if 0
int PhoneFormat::initDevice(GSM_StateMachine *s)
{
    GSM_ReadConfig(NULL, &s->Config[0], 0);
	s->ConfigNum 	= 1;
    GSM_Config *cfg = &s->Config[0];
    if ( ! mConnection.isEmpty() ) {
        cfg->Connection = strdup(mConnection.latin1());
		cfg->DefaultConnection = false;
        qDebug("Connection set %s ", cfg->Connection );

    }
    if ( ! mDevice.isEmpty() ) {
        cfg->Device = strdup(mDevice.latin1());
		cfg->DefaultDevice = false;
        qDebug("Device  set %s ", cfg->Device);

    }
    if ( ! mModel.isEmpty() ) {
		strcpy(cfg->Model,mModel.latin1() );
		cfg->DefaultModel = false;
        qDebug("Model  set %s ",cfg->Model );
    }
    int error=GSM_InitConnection(s,3);
    return error;
}
#endif
ulong PhoneFormat::getCsumTodo( Todo* todo )
{
    QStringList  attList;
    if ( todo->hasDueDate() )
        attList << PhoneParser::dtToString ( todo->dtDue() );
    attList << todo->summary();
    QString completedString = "no";
    if ( todo->isCompleted() )
        completedString = "yes";
    attList << completedString; 
    int prio = todo->priority();
    if( prio == 2 ) prio = 1;
    if (prio == 4 ) prio = 5 ;
    attList << QString::number( prio ); 
    QString alarmString = "na";
    Alarm *alarm;
    if (  todo->alarms().count() > 0 ) {
        alarm = todo->alarms().first();
        if ( alarm->enabled() ) {
            alarmString = QString::number(alarm->offset() );
        }
    }
    attList << alarmString;
    attList << todo->categoriesStr();
    attList << todo->secrecyStr(); 
    return PhoneFormat::getCsum(attList );  

}
ulong PhoneFormat::getCsumEvent( Event* event )
{
    QStringList  attList;
    attList << PhoneParser::dtToString ( event->dtStart() );
    attList << PhoneParser::dtToString ( event->dtEnd() );
    attList <<  event->summary();
    attList <<  event->location();
    QString alarmString = "na";
    Alarm *alarm;
    if (  event->alarms().count() > 0 ) {
        alarm = event->alarms().first();
        if ( alarm->enabled() ) {
            alarmString = QString::number( alarm->offset() );
        }
    }
    attList <<  alarmString;
    Recurrence* rec = event->recurrence();
    QStringList  list;
    bool writeEndDate = false;
    switch ( rec->doesRecur() )
        {
        case Recurrence::rDaily: // 0
            list.append( "0" );
            list.append( QString::number( rec->frequency() ));//12
            list.append( "0" );
            list.append( "0" );
            writeEndDate = true;
            break;
        case Recurrence::rWeekly:// 1
            list.append( "1" );
            list.append( QString::number( rec->frequency()) );//12
            list.append( "0" );
            {
                int days = 0;
                QBitArray weekDays = rec->days(); 
                int i;
                for( i = 1; i <= 7; ++i ) {
                    if ( weekDays[i-1] ) {
                        days += 1 << (i-1);
                    }
                }
                list.append( QString::number( days ) );
            }
            //pending weekdays
            writeEndDate = true;
           
            break;
        case Recurrence::rMonthlyPos:// 2
            list.append( "2" );
            list.append( QString::number( rec->frequency()) );//12
           
            writeEndDate = true; 
            {
                int count = 1;
                QPtrList<Recurrence::rMonthPos> rmp;
                rmp = rec->monthPositions();
                if ( rmp.first()->negative )
                    count = 5 - rmp.first()->rPos - 1;
                else
                    count = rmp.first()->rPos - 1;
                list.append( QString::number( count ) );
           
            }

            list.append( "0" );
            break;
        case Recurrence::rMonthlyDay:// 3
            list.append( "3" );
            list.append( QString::number( rec->frequency()) );//12
            list.append( "0" );
            list.append( "0" );
            writeEndDate = true;
            break;
        case Recurrence::rYearlyMonth://4
            list.append( "4" );
            list.append( QString::number( rec->frequency()) );//12
            list.append( "0" );
            list.append( "0" );
            writeEndDate = true;
            break;
       
        default:
            list.append( "255" );
            list.append( QString() );
            list.append( "0" );
            list.append( QString() );
            list.append( "0" );
            list.append( "20991231T000000" );
            break;
        }
    if  ( writeEndDate ) {
        
        if ( rec->endDate().isValid() ) { // 15 + 16
            list.append( "1" );
            list.append( PhoneParser::dtToString( QDateTime(rec->endDate()) ));
        } else {
            list.append( "0" );
            list.append( "20991231T000000" );
        }
        
    }
    attList << list.join("");
    attList << event->categoriesStr();
    //qDebug("csum cat %s", event->categoriesStr().latin1()); 

    attList << event->secrecyStr();
    return PhoneFormat::getCsum(attList );  
}
ulong PhoneFormat::getCsum( const QStringList &  attList)
{
    int max = attList.count();
    ulong cSum = 0;
    int j,k,i;
    int add;
    for ( i = 0; i < max ; ++i ) {
        QString s = attList[i];
        if ( ! s.isEmpty() ){
            j = s.length();
            for ( k = 0; k < j; ++k ) {
                int mul = k +1;
                add = s[k].unicode ();
                if ( k < 16 )
                    mul = mul * mul;
                int ii = i+1;
                add = add * mul *ii*ii*ii;
                cSum += add;
            }
        }
            
    }
    //QString dump = attList.join(",");
    //qDebug("csum: %d %s", cSum,dump.latin1()); 
    
    return cSum;

}
//extern "C" GSM_Error GSM_InitConnection(GSM_StateMachine *s, int ReplyNum);
#include <stdlib.h>
#define DEBUGMODE false
bool PhoneFormat::load( Calendar *calendar, Calendar *existingCal)
{

    QString fileName;
#ifdef DESKTOP_VERSION
    fileName = locateLocal("tmp", "phonefile.vcs");
#else
    fileName = "/tmp/phonefile.vcs";
#endif
    QString command;
    if ( ! PhoneAccess::readFromPhone( fileName )) {
        return false;
    }
    VCalFormat vfload;
    vfload.setLocalTime ( true ); 
    qDebug("loading file ..."); 

    if ( ! vfload.load( calendar, fileName ) )
        return false;
    QPtrList<Event> er = calendar->rawEvents();
    Event* ev = er.first();  
    qDebug("reading events... "); 
    while ( ev ) {
        QStringList cat = ev->categories();
        if ( cat.contains( "MeetingDEF" )) {
            ev->setCategories( QStringList() );
        } else 
        if ( cat.contains( "Birthday" )) {
            ev->setFloats( true );
            QDate da = ev->dtStart().date();
            ev->setDtStart( QDateTime( da)  );
            ev->setDtEnd( QDateTime( da.addDays(1))  );
            
        }
        uint cSum;
        cSum = PhoneFormat::getCsumEvent( ev );
        int id = ev->pilotId();
        Event *event;
        event = existingCal->event( mProfileName ,QString::number( id ) );
        if ( event ) {
            event = (Event*)event->clone();
            copyEvent( event,  ev );
            calendar->deleteEvent( ev );
            calendar->addEvent( event);
        }
        else
            event = ev;
        event->setCsum( mProfileName, QString::number( cSum ));
        event->setTempSyncStat( SYNC_TEMPSTATE_NEW_EXTERNAL );
        event->setID( mProfileName,QString::number( id ) );
        ev =  er.next();
    }
    {
        qDebug("reading todos... "); 
        QPtrList<Todo> tr = calendar->rawTodos();
        Todo* ev = tr.first();  
        while ( ev ) {

            QStringList cat = ev->categories();
            if ( cat.contains( "MeetingDEF" )) {
                ev->setCategories( QStringList() );
            }
            int id = ev->pilotId();
            uint cSum;
            cSum = PhoneFormat::getCsumTodo( ev );
            Todo *event;
            event = existingCal->todo( mProfileName ,QString::number( id ) );
            if ( event ) {
                //qDebug("copy todo %s ", event->summary().latin1()); 

                event = (Todo*)event->clone();
                copyTodo( event,  ev );
                calendar->deleteTodo( ev );
                calendar->addTodo( event);
            }
            else
                event = ev;
            event->setCsum( mProfileName, QString::number( cSum ));
            event->setTempSyncStat( SYNC_TEMPSTATE_NEW_EXTERNAL );
            event->setID( mProfileName,QString::number( id ) );
            ev =  tr.next();
        }
    }
    return true;
}
void PhoneFormat::copyEvent( Event* to,  Event* from )
{
    if ( from->dtStart().isValid() )
        to->setDtStart(  from->dtStart() );
    if ( from->dtEnd().isValid() )
        to->setDtEnd(  from->dtEnd() );
    if ( !from->location().isEmpty() )
        to->setLocation(  from->location() );
    if ( !from->description().isEmpty() )
        to->setDescription(  from->description() );
    if ( !from->summary().isEmpty() )
        to->setSummary(  from->summary() );

    if ( from->alarms().count() ) {
        to->clearAlarms();
        Alarm *a = from->alarms().first();
        Alarm *b = to->newAlarm( );
        b->setEnabled( a->enabled()  );
        b->setStartOffset(Duration( a->offset() ) );
    
    }
    QStringList cat = to->categories();
    QStringList catFrom = from->categories();
    QString nCat;
    int iii;
    for ( iii = 0; iii < catFrom.count();++iii ) {
        nCat = catFrom[iii];
        if ( !nCat.isEmpty() )
            if ( !cat.contains( nCat )) {
                cat << nCat;
            }
    }
    to->setCategories( cat );
    if ( from->doesRecur() ) {
        Recurrence * r = new Recurrence( *from->recurrence(),to);
        to->setRecurrence( r ) ;
    }


}
void PhoneFormat::copyTodo( Todo* to,  Todo* from )
{
    if ( from->hasStartDate() ) {
        to->setHasStartDate( true );
        to->setDtStart(  from->dtStart() );
    }
    if ( from->hasDueDate() ){
        to->setHasDueDate( true );
        to->setDtDue(  from->dtDue() );
    }
    if ( !from->location().isEmpty() )
        to->setLocation(  from->location() );
    if ( !from->description().isEmpty() )
        to->setDescription(  from->description() );
    if ( !from->summary().isEmpty() )
        to->setSummary(  from->summary() );

    if ( from->alarms().count() ) {
        to->clearAlarms();
        Alarm *a = from->alarms().first();
        Alarm *b = to->newAlarm( );
        b->setEnabled( a->enabled()  );
        b->setStartOffset(Duration( a->offset() ) );
    }
   
    QStringList cat = to->categories();
    QStringList catFrom = from->categories();
    QString nCat;
    int iii;
    for ( iii = 0; iii < catFrom.count();++iii ) {
        nCat = catFrom[iii];
        if ( !nCat.isEmpty() )
            if ( !cat.contains( nCat )) {
                cat << nCat;
            }
    }
    to->setCategories( cat );
    if ( from->isCompleted() ) {
        to->setCompleted( true );
        if( from->completed().isValid() )
            to->setCompleted( from->completed() );
    } else {
        // set percentcomplete only, if to->isCompleted()
        if ( to->isCompleted() )
            to->setPercentComplete(from->percentComplete()); 
    }
    if( to->priority() == 2 && from->priority() == 1 )
        ; //skip
    else if (to->priority() == 4 && from->priority() == 5 )
        ;
    else
        to->setPriority(from->priority());

}
#include <qcstring.h>

void PhoneFormat::afterSave( Incidence* inc,const QString& id ,const QString& csum)
{
    inc->setID( mProfileName, id );
    inc->setCsum( mProfileName, csum);
    inc->setTempSyncStat( SYNC_TEMPSTATE_NEW_ID );

}

bool PhoneFormat::writeToPhone( Calendar *  calendar)
{
#ifdef DESKTOP_VERSION
    QString  fileName = locateLocal("tmp", "phonefile.vcs");
#else
    QString  fileName = "/tmp/phonefile.vcs";
#endif

    VCalFormat vfsave;
    vfsave.setLocalTime ( true ); 
    QString id = calendar->timeZoneId();
    calendar->setLocalTime();
    if ( ! vfsave.save( calendar, fileName ) )
        return false;
    calendar->setTimeZoneId( id );
    return PhoneAccess::writeToPhone( fileName );
}
bool PhoneFormat::save( Calendar *calendar)
{
   

    // 1 remove events which should be deleted
    QPtrList<Event> er = calendar->rawEvents();
    Event* ev = er.first();  
    while ( ev ) {
        if ( ev->tempSyncStat() == SYNC_TEMPSTATE_DELETE ) { 
            calendar->deleteEvent( ev );
        } else {

        }
        ev =  er.next();
    }
    //  2 remove todos which should be deleted
    QPtrList<Todo> tl = calendar->rawTodos(); 
    Todo* to = tl.first();
    while ( to ) {
        if ( to->tempSyncStat() == SYNC_TEMPSTATE_DELETE ) {
            calendar->deleteTodo( to );   
        } else {
            if ( to->isCompleted()) {
                calendar->deleteTodo( to );   
            }
        }
        to = tl.next();
    } 
    //  3 save file 
    if ( !writeToPhone( calendar ) )
        return false;
    QLabel status ( i18n(" Opening device ..."), 0 );
    int w = status.sizeHint().width()+20 ;
    if ( w < 200 ) w = 230;
    int h = status.sizeHint().height()+20 ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    status.setCaption(i18n("Writing to phone...") );
    status.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    QString message;
    status.show();
    status.raise();
    qApp->processEvents();
    // 5 reread data
    message = i18n(" Rereading all data ... ");
    status.setText ( message  );
    qApp->processEvents();
    CalendarLocal* calendarTemp = new CalendarLocal(); 
    calendarTemp->setTimeZoneId( calendar->timeZoneId());
    if ( ! load( calendarTemp,calendar) ){
        qDebug("error reloading calendar ");
        delete calendarTemp;
        return false;
    }
    // 6 compare data

//algo 6 compare event
    er = calendar->rawEvents();
    ev = er.first(); 
    message = i18n(" Comparing event # ");
    QPtrList<Event> er1 = calendarTemp->rawEvents();
    Event* ev1; 
    int procCount = 0;
    while ( ev ) {
        //qDebug("event new ID %s",ev->summary().latin1()); 
            status.setText ( message + QString::number ( ++procCount ) );
            qApp->processEvents();
            ev1 = er1.first(); 
            while ( ev1 ) {
                if (  ev->contains( ev1 ) ) {
                    afterSave( ev ,ev1->getID(mProfileName),ev1->getCsum(mProfileName));
                    er1.remove( ev1 );
                    break;
                }
                ev1 =  er1.next();
            } 
            if ( ! ev1 ) {
                // ev->removeID(mProfileName);
                qDebug("ERROR: No event found  on phone for %s ", ev->summary().latin1());
            }
            
        
        ev =  er.next();
    }
    //algo 6 compare todo
    tl = calendar->rawTodos(); 
    to = tl.first();
    procCount = 0;
    QPtrList<Todo> tl1 = calendarTemp->rawTodos(); 
    Todo* to1 ;
    message = i18n(" Comparing todo # ");
    while ( to  ) {
            status.setText ( message + QString::number ( ++procCount ) );
            qApp->processEvents();
            Todo* to1 = tl1.first(); 
            while ( to1 ) {
                if (  to->contains( to1 ) ) {
                    afterSave( to ,to1->getID(mProfileName),to1->getCsum(mProfileName));
                    tl1.remove( to1 );
                    break;
                }
                to1 =  tl1.next();
            }
            if ( ! to1 ) {
                //to->removeID(mProfileName);
                qDebug("ERROR: No todo found on phone for %s ", to->summary().latin1());
            }
        
        to = tl.next();
    }
    delete calendarTemp;
    return true;



}


QString PhoneFormat::toString( Calendar * )
{
    return QString::null;
}
bool PhoneFormat::fromString( Calendar *calendar, const QString & text)
{
    return false;
}
