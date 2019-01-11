/*
    This file is part of libkcal.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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
#include <qxml.h>
#include <qlabel.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>

#include "calendar.h"
#include "alarm.h"
#include "recurrence.h"
#include "calendarlocal.h"

#include "sharpformat.h"
#include "syncdefines.h"

using namespace KCal;

//CARDID,CATEGORY,DSRP,PLCE,MEM1,TIM1,TIM2,ADAY,ARON,ARMN,ARSD,RTYP,RFRQ,RPOS,RDYS,REND,REDT,ALSD,ALED,MDAY
//  0       1      2     3   4    5     6   7    8     9   10   11   12   13   14   15   16   17   18   19

//ARSD  silentalarm = 0
// 11 RTYP 225 no /0 dialy/ 1 weekly/ 3 month by date/ 2 month by day(pos)/ yearly
// 12 RFRQ
// 13 RPOS pos = 4. monday in month
// 14 RDYS days: 1 mon/ 2 tue .. 64 sun
// 15 REND 0 = no end/ 1 = end
// 16 REDT rec end dt  
//ALSD
//ALED
//MDAY

class SharpParser : public QObject
{
  public:
    SharpParser( Calendar *calendar ) : mCalendar( calendar ) {
    }
  
    bool startElement( Calendar *existingCalendar, const QStringList & attList, QString qName )
    {
        int i = 1;
        bool skip = true;
        int max = attList.count() -2;
        while ( i < max ) {
            if ( !attList[i].isEmpty() ) {
                skip = false;
                break;
            } 
            ++i ;
        }
        if ( skip )
            return false;
        ulong cSum = SharpFormat::getCsum(attList );
        
        if ( qName == "Event" ) {
            Event *event;
            event = existingCalendar->event( "Sharp_DTM",attList[0] );
            if ( event )
                event = (Event*)event->clone();
            else
                event = new Event;
            event->setID("Sharp_DTM", attList[0] );
            event->setCsum( "Sharp_DTM", QString::number( cSum ));
            event->setTempSyncStat(SYNC_TEMPSTATE_NEW_EXTERNAL );

            event->setSummary( attList[2] );
            event->setLocation( attList[3] );
            event->setDescription( attList[4]  );
            if ( attList[7] == "1" ) {
                event->setDtStart( QDateTime(fromString( attList[17]+"T000000", false ).date(),QTime(0,0,0 ) ));
                event->setDtEnd( QDateTime(fromString( attList[18]+"T000000", false ).date(),QTime(0,0,0 )));
                event->setFloats( true );
            } else {
                event->setFloats( false );
                event->setDtStart( fromString( attList[5] ) );
                event->setDtEnd( fromString( attList[6]  ));
            }
        
            QString rtype = attList[11];
            if ( rtype != "255" ) {
                //  qDebug("recurs ");
                QDate startDate = event->dtStart().date();
        
                QString freqStr = attList[12];
                int freq = freqStr.toInt();

                QString hasEndDateStr = attList[15] ;
                bool hasEndDate = hasEndDateStr == "1";
        
                QString endDateStr = attList[16];
                QDate endDate = fromString( endDateStr ).date();

                QString weekDaysStr = attList[14];
                uint weekDaysNum = weekDaysStr.toInt();
        
                QBitArray weekDays( 7 );
                int i;
                int bb = 1;
                for( i = 1; i <= 7; ++i ) {
                    weekDays.setBit( i - 1, ( bb & weekDaysNum )); 
                    bb =  2 << (i-1);
                    //qDebug(" %d bit %d ",i-1,weekDays.at(i-1) );
                }
                // qDebug("next ");
                QString posStr = attList[13];
                int pos = posStr.toInt();
                Recurrence *r = event->recurrence();

                if ( rtype == "0" ) {
                    if ( hasEndDate ) r->setDaily( freq, endDate );
                    else r->setDaily( freq, -1 );
                } else if ( rtype == "1" ) {
                    if ( hasEndDate ) r->setWeekly( freq, weekDays, endDate );
                    else r->setWeekly( freq, weekDays, -1 );
                } else if ( rtype == "3" ) {
                    if ( hasEndDate )
                        r->setMonthly( Recurrence::rMonthlyDay, freq, endDate );
                    else
                        r->setMonthly( Recurrence::rMonthlyDay, freq, -1 );
                    r->addMonthlyDay( startDate.day() );
                } else if ( rtype == "2" ) {
                    if ( hasEndDate )
                        r->setMonthly( Recurrence::rMonthlyPos, freq, endDate );
                    else
                        r->setMonthly( Recurrence::rMonthlyPos, freq, -1 );
                    QBitArray days( 7 );
                    days.fill( false );
                    days.setBit( startDate.dayOfWeek() - 1 );
                    r->addMonthlyPos( pos, days );
                } else if ( rtype == "4" ) {
                    if ( hasEndDate )
                        r->setYearly( Recurrence::rYearlyMonth, freq, endDate );
                    else
                        r->setYearly( Recurrence::rYearlyMonth, freq, -1 );
                    r->addYearlyNum( startDate.month() );
                }
            } else {
                event->recurrence()->unsetRecurs();
            }
        
            QString categoryList = attList[1] ;
            event->setCategories( categoryList );

            // strange 0 semms to mean: alarm enabled
            if ( attList[8] == "0" ) {
                Alarm *alarm;
                if (  event->alarms().count() > 0 )
                    alarm = event->alarms().first();
                else {
                    alarm = new Alarm( event );
                    event->addAlarm( alarm );
                    alarm->setType( Alarm::Audio );
                }
                //alarm->setType( Alarm::Audio );
                alarm->setEnabled( true );
                int alarmOffset = attList[9].toInt();
                alarm->setStartOffset( alarmOffset * -60 );
            } else {
                Alarm *alarm;
                if (  event->alarms().count() > 0 ) {
                    alarm = event->alarms().first();
                    alarm->setType( Alarm::Audio );
                    alarm->setStartOffset( -60*15 );
                    alarm->setEnabled( false );
                }
            }
        
            mCalendar->addEvent( event);
        } else if ( qName == "Todo" ) {
            Todo *todo;

            todo = existingCalendar->todo( "Sharp_DTM", attList[0] );
            if (todo )
                todo = (Todo*)todo->clone();
            else
                todo = new Todo;

//CARDID,CATEGORY,ETDY,LTDY,FNDY,MARK,PRTY,TITL,MEM1
//  0      1       2    3    4    5    6    7    8
//1,,,,,1,4,Loch zumachen,""
//3,Privat,20040317T000000,20040318T000000,20040319T000000,0,5,Call bbb,"notes123 bbb gggg ""bb ""   "
//2,"Familie,Freunde,Holiday",20040318T000000,20040324T000000,20040317T000000,1,2,tod2,notes
     
            todo->setID( "Sharp_DTM", attList[0]);
            todo->setCsum( "Sharp_DTM", QString::number( cSum ));
            todo->setTempSyncStat( SYNC_TEMPSTATE_NEW_EXTERNAL );
      
            todo->setSummary( attList[7] );
            todo->setDescription( attList[8]);
        
            int priority = attList[6].toInt();
            if ( priority == 0 ) priority = 3;
            todo->setPriority( priority );
        
            QString categoryList = attList[1];
            todo->setCategories(  categoryList );
        
       
        
            QString hasDateStr = attList[3]; // due
            if ( !hasDateStr.isEmpty() ) {
                if ( hasDateStr.right(6) == "000000" ) {
                    todo->setDtDue( QDateTime(fromString( hasDateStr, false ).date(), QTime(0,0,0 )) );
                    todo->setFloats( true );
                }
                else {
                    todo->setDtDue( fromString( hasDateStr ) );
                    todo->setFloats( false );
                }
                
                todo->setHasDueDate( true );
            }
            hasDateStr = attList[2];//start
            if ( !hasDateStr.isEmpty() ) {
          
                todo->setDtStart( fromString( hasDateStr ) );
                todo->setHasStartDate( true);
            } else
                todo->setHasStartDate( false );
            hasDateStr = attList[4];//completed
            if ( !hasDateStr.isEmpty() ) {
                todo->setCompleted(fromString( hasDateStr ) );
            }
            QString completedStr = attList[5];
            if ( completedStr == "0" )
                todo->setCompleted( true );
            else {
                // do not change percent complete
                if ( todo->isCompleted() )
                    todo->setCompleted( false );
            }
            mCalendar->addTodo( todo );
        
        } else if ( qName == "Category" ) {
            /*
              QString id = attributes.value( "id" );
              QString name = attributes.value( "name" );
              setCategory( id, name );
            */
        }
        //qDebug("end ");
        return true;
    }

   
   QDateTime fromString ( QString  s, bool useTz = true ) {
       QDateTime dt;
       int y,m,t,h,min,sec;
       y = s.mid(0,4).toInt();
       m = s.mid(4,2).toInt();
       t = s.mid(6,2).toInt();
       h = s.mid(9,2).toInt();
       min = s.mid(11,2).toInt();
       sec = s.mid(13,2).toInt();
       dt = QDateTime(QDate(y,m,t), QTime(h,min,sec));
      int offset =  KGlobal::locale()->localTimeOffset( dt );
      if ( useTz )
          dt =  dt.addSecs ( offset*60);
       return dt;

   }
  protected:
    QDateTime toDateTime( const QString &value )
    {
      QDateTime dt;
      dt.setTime_t( value.toUInt() );
      
      return dt;
    }

  private:
    Calendar *mCalendar;
};


SharpFormat::SharpFormat()
{
    
}

SharpFormat::~SharpFormat()
{
}
ulong SharpFormat::getCsum( const QStringList &  attList)
{
    int max = attList.count() -1;
    ulong cSum = 0;
    int j,k,i;
    int add;
    for ( i = 1; i < max ; ++i ) {
        QString s = attList[i];
        if ( ! s.isEmpty() ){
            j = s.length();
            for ( k = 0; k < j; ++k ) {
                int mul = k +1;
                add = s[k].unicode ();
                if ( k < 16 )
                    mul = mul * mul;
                add = add * mul *i*i*i;
                cSum += add;
            }
        }
    }
    return cSum;

}
#include <stdlib.h>
#define DEBUGMODE false
//#define DEBUGMODE true
bool SharpFormat::load( Calendar *calendar, Calendar *existngCal )
{

   
    bool debug = DEBUGMODE;
    QString text;
    QString codec = "utf8";
    QLabel status ( i18n("Reading events ..."), 0 );
    
    int w = status.sizeHint().width()+20 ;
    if ( w < 200 ) w = 200;
    int h = status.sizeHint().height()+20 ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    status.setCaption(i18n("Reading DTM Data") );
    status.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    status.show();
    status.raise();
    qApp->processEvents();
    QString  fileName; 
    if ( ! debug ) {
        fileName = "/tmp/kopitempout";
        QString command ="db2file datebook -r -c "+ codec + " > " + fileName;
        system (  command.latin1() );
    } else {
        fileName = "/tmp/events.txt";
        
    }  
    QFile file( fileName );
    if (!file.open( IO_ReadOnly ) ) {
        return false;
        
    } 
    QTextStream ts( &file );
    ts.setCodec( QTextCodec::codecForName("utf8") );
    text = ts.read();
    file.close();  
    status.setText(  i18n("Processing events ...") );
    status.raise();
    qApp->processEvents();
    fromString2Cal( calendar, existngCal, text, "Event" ); 
    status.setText(  i18n("Reading todos ...") );
    qApp->processEvents();
    if ( ! debug ) {
        fileName = "/tmp/kopitempout";
        QString command = "db2file todo -r -c " + codec+ " > " + fileName;
        system (  command.latin1() );
    } else {
        fileName = "/tmp/todo.txt";
    } 
    file.setName( fileName );
    if (!file.open( IO_ReadOnly ) ) {
        return false;
        
    } 
    ts.setDevice( &file );
    text = ts.read();
    file.close();  

    status.setText(  i18n("Processing todos ...") );
    status.raise();
    qApp->processEvents();
    fromString2Cal( calendar, existngCal, text, "Todo" ); 
    return true;
}
int  SharpFormat::getNumFromRecord( QString answer, Incidence* inc ) 
{
    int retval = -1;
    QStringList templist; 
    QString tempString;         
    int start = 0;
    int len = answer.length();
    int end = answer.find ("\n",start)+1;
    bool ok = true;
    start = end;
    int ccc = 0;
    while ( start > 0 ) {
        templist.clear();
        ok = true;
        int loopCount = 0;
        while ( ok ) {
            ++loopCount;
            if ( loopCount >  25 ) {
                qDebug("KO: Error in while loop"); 
                ok = false;
                start = 0;
                break;
            }
            if ( ok )
                tempString = getPart(  answer, ok, start );
            if ( start >= len || start == 0 ) {
                start = 0;
                ok = false;
            }
            if ( tempString.right(1) =="\n" )
                tempString = tempString.left( tempString.length()-1);
           
            templist.append( tempString ); 
        }
        ++ccc;
        if ( ccc == 2 && loopCount < 25 ) { 
            start = 0;
            bool ok;
            int newnum = templist[0].toInt( &ok );
            if ( ok && newnum > 0) {
                retval = newnum; 
                inc->setID( "Sharp_DTM",templist[0] );
                inc->setCsum( "Sharp_DTM", QString::number( getCsum( templist ) ));
                inc->setTempSyncStat( SYNC_TEMPSTATE_NEW_ID );
            }
            if ( ok && newnum == -1 ) {
                qDebug("Error writing back %s ", inc->summary().latin1());
            }
        }
    }
    //qDebug("getNumFromRecord returning : %d ", retval);
    return retval;
}
bool SharpFormat::save( Calendar *calendar)
{

    QLabel status ( i18n("Processing/adding events ..."), 0 );
    int w = status.sizeHint().width()+20 ;
    if ( w < 200 ) w = 200;
    int h = status.sizeHint().height()+20 ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    status.setCaption(i18n("Writing DTM Data") );
    status.setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    status.show();
    status.raise();
    qApp->processEvents();
    bool debug = DEBUGMODE;
    QString codec = "utf8";
    QString answer;
    QString ePrefix = "CARDID,CATEGORY,DSRP,PLCE,MEM1,TIM1,TIM2,ADAY,ARON,ARMN,ARSD,RTYP,RFRQ,RPOS,RDYS,REND,REDT,ALSD,ALED,MDAY\n";
    QString tPrefix = "CARDID,CATEGORY,ETDY,LTDY,FNDY,MARK,PRTY,TITL,MEM1\n";
    QString command;
    QPtrList<Event> er = calendar->rawEvents(); 
    Event* ev = er.first();
    QString  fileName = "/tmp/kopitempout";
    int i = 0;
    QString changeString = ePrefix;
    QString  deleteString = ePrefix;
    bool deleteEnt = false;
    bool changeEnt = false;
    QString message = i18n("Processing event # ");
    int procCount = 0;
    while ( ev ) {
        //qDebug("i %d ", ++i);
        if ( ev->tempSyncStat() != SYNC_TEMPSTATE_NEW_EXTERNAL ) {
            status.setText ( message + QString::number ( ++procCount ) );
             qApp->processEvents();
            QString eString = getEventString( ev );
            if ( ev->tempSyncStat() == SYNC_TEMPSTATE_DELETE ) { // delete
                // deleting empty strings does not work.
                // we write first and x  and then delete the record with the x
                eString = eString.replace( QRegExp(",\"\""),",\"x\"" );
                changeString += eString + "\n";
                deleteString += eString + "\n";
                deleteEnt = true;
                changeEnt = true;
                }
            else  if ( ev->getID("Sharp_DTM").isEmpty()  ) {       // add new 
                QString fileNameIn = "/tmp/kopitempin";
                QFile fileIn( fileNameIn );
                if (!fileIn.open( IO_WriteOnly ) ) {
                    return false;
                } 
                QTextStream tsIn( &fileIn );
                tsIn.setCodec( QTextCodec::codecForName("utf8") );
                tsIn << ePrefix << eString ;
                fileIn.close();  
                //command = "(echo \"" + ePrefix + eString + "\" ) | db2file datebook -w -g -c " + codec+ " > "+ fileName;
                command = "(cat /tmp/kopitempin | db2file datebook -w -g -c " + codec+ ") > "+ fileName;
                //qDebug("command ++++++++ ");
                //qDebug("%s ",command.latin1());
                //qDebug("command -------- ");
                system (  command.utf8() );
                QFile file( fileName );
                if (!file.open( IO_ReadOnly ) ) {
                    return false;
                    
                } 
                QTextStream ts( &file );
                ts.setCodec( QTextCodec::codecForName("utf8") );
                answer = ts.read();
                file.close();  
                //qDebug("answer \n%s ", answer.latin1());
                getNumFromRecord( answer, ev  ) ;

            }
            else { // change existing
                //qDebug("canging %d %d",ev->zaurusStat() ,ev->zaurusId() );
                //command = "(echo \"" + ePrefix + eString + "\" ) | db2file datebook -w -g -c " + codec+ " > "+ fileName;
                changeString += eString + "\n";
                changeEnt = true;

            }
        } 
        ev =  er.next();
    }
    status.setText ( i18n("Changing events ...") );
    qApp->processEvents();
    //qDebug("changing... ");
    if ( changeEnt ) {
        QFile file( fileName );
        if (!file.open( IO_WriteOnly ) ) {
            return false;
            
        } 
        QTextStream ts( &file );
        ts.setCodec( QTextCodec::codecForName("utf8") );
        ts << changeString ;
        file.close();
        command = "db2file datebook -w -g -c " + codec+ " < "+ fileName;
        system (  command.latin1() );
        //qDebug("command %s file :\n%s ", command.latin1(), changeString.latin1());
        
    }
    status.setText ( i18n("Deleting events ...") );
    qApp->processEvents();
    //qDebug("deleting... ");
    if ( deleteEnt  ) {
        QFile file( fileName );
        if (!file.open( IO_WriteOnly ) ) {
            return false;
            
        } 
        QTextStream ts( &file );
        ts.setCodec( QTextCodec::codecForName("utf8") );
        ts << deleteString;
        file.close();
        command = "db2file datebook -d -c " + codec+ " < "+ fileName;
        system ( command.latin1() );
        // qDebug("command %s file :\n%s ", command.latin1(), deleteString.latin1());
    }


    changeString = tPrefix;
    deleteString = tPrefix;
    status.setText ( i18n("Processing todos ...") );
    qApp->processEvents();
    QPtrList<Todo> tl = calendar->rawTodos(); 
    Todo* to = tl.first();
    i = 0;
    message = i18n("Processing todo # ");
    procCount = 0;
    while ( to ) {
       if ( to->tempSyncStat() != SYNC_TEMPSTATE_NEW_EXTERNAL ) {
             status.setText ( message + QString::number ( ++procCount ) );
             qApp->processEvents();
            QString eString = getTodoString( to );
            if ( to->tempSyncStat() == SYNC_TEMPSTATE_DELETE ) { // delete
                // deleting empty strings does not work.
                // we write first and x  and then delete the record with the x
                eString = eString.replace( QRegExp(",\"\""),",\"x\"" );
                changeString += eString + "\n";
                deleteString += eString + "\n";
                deleteEnt = true;
                changeEnt = true;
                }
            else  if ( to->getID("Sharp_DTM").isEmpty() ) {       // add new 



                QString fileNameIn = "/tmp/kopitempin";
                QFile fileIn( fileNameIn );
                if (!fileIn.open( IO_WriteOnly ) ) {
                    return false;
                } 
                QTextStream tsIn( &fileIn );
                tsIn.setCodec( QTextCodec::codecForName("utf8") );
                tsIn << tPrefix << eString ;
                fileIn.close();  
                command = "(cat /tmp/kopitempin | db2file todo -w -g -c " + codec+ ") > "+ fileName;
                system (  command.utf8() );
                QFile file( fileName );
                if (!file.open( IO_ReadOnly ) ) {
                    return false;
                } 
                QTextStream ts( &file );
                ts.setCodec( QTextCodec::codecForName("utf8") );
                answer = ts.read();
                file.close();  
                //qDebug("answer \n%s ", answer.latin1());
                getNumFromRecord( answer, to  ) ;

            }
            else { // change existing
                //qDebug("canging %d %d",to->zaurusStat() ,to->zaurusId() );
                //command = "(echo \"" + ePrefix + eString + "\" ) | db2file datebook -w -g -c " + codec+ " > "+ fileName;
                changeString += eString + "\n";
                changeEnt = true;

            }
        } 
        
        to = tl.next();
    }
    status.setText ( i18n("Changing todos ...") );
    qApp->processEvents();
    //qDebug("changing... ");
    if ( changeEnt ) {
        QFile file( fileName );
        if (!file.open( IO_WriteOnly ) ) {
            return false;
            
        } 
        QTextStream ts( &file );
        ts.setCodec( QTextCodec::codecForName("utf8") );
        ts << changeString ;
        file.close();
        command = "db2file todo -w -g -c " + codec+ " < "+ fileName;
        system (  command.latin1() );
        //qDebug("command %s file :\n%s ", command.latin1(), changeString.latin1());
        
    }
    status.setText ( i18n("Deleting todos ...") );
    qApp->processEvents();
    //qDebug("deleting... ");
    if ( deleteEnt  ) {
        QFile file( fileName );
        if (!file.open( IO_WriteOnly ) ) {
            return false;
            
        } 
        QTextStream ts( &file );
        ts.setCodec( QTextCodec::codecForName("utf8") );
        ts << deleteString;
        file.close();
        command = "db2file todo -d -c " + codec+ " < "+ fileName;
        system ( command.latin1() );
        // qDebug("command %s file :\n%s ", command.latin1(), deleteString.latin1());
    }
        
  return true;
}
QString SharpFormat::dtToString( const QDateTime& dti, bool useTZ )
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
QString SharpFormat::getEventString( Event* event )
{
    QStringList list;
    list.append( event->getID("Sharp_DTM")  );
    list.append( event->categories().join(",") );
    if ( !event->summary().isEmpty() )
        list.append( event->summary() );
    else
        list.append("" );
    if ( !event->location().isEmpty() )
        list.append( event->location() );
    else
        list.append("" );
    if ( !event->description().isEmpty() )
        list.append( event->description() );
    else
        list.append( "" );
    if ( event->doesFloat () ) {
        list.append( dtToString( QDateTime(event->dtStart().date(), QTime(0,0,0)), false ));
        list.append( dtToString( QDateTime(event->dtEnd().date(),QTime(23,59,59)), false )); //6
        list.append( "1" );

    }
    else {
        list.append( dtToString( event->dtStart()) );
        list.append( dtToString( event->dtEnd()) ); //6
        list.append( "0" );
    }
    bool noAlarm = true;
    if ( event->alarms().count() > 0 ) {
        Alarm * al = event->alarms().first();
        if ( al->enabled() ) {
            noAlarm = false;
            list.append( "0" ); // yes, 0 == alarm
            list.append( QString::number( al->startOffset().asSeconds()/(-60) ) ); 
            if ( al->type() == Alarm::Audio )
                list.append( "1" ); //  type audio
            else
                list.append( "0" ); //  type silent     
        }
    }
    if ( noAlarm ) {
        list.append( "1" ); // yes, 1 == no alarm
        list.append( "0" ); // no alarm offset
        list.append( "1" ); // type
    }
    // next is: 11
    // next is: 11-16 are recurrence
    Recurrence* rec = event->recurrence();
   
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
            list.append( dtToString( rec->endDate(), false ) );
        } else {
            list.append( "0" );
            list.append( "20991231T000000" );
        }
        
    }
    if ( event->doesFloat () ) {
        list.append( dtToString( event->dtStart(), false ).left( 8 ));
        list.append( dtToString( event->dtEnd(), false ).left( 8 )); //6
        
    }
    else {
        list.append( QString() );
        list.append( QString() );
        
    }
    if (event->dtStart().date() == event->dtEnd().date() )
        list.append( "0" );
    else
        list.append( "1" );


		for(QStringList::Iterator it=list.begin();
				it!=list.end(); ++it){
			QString& s = (*it);
			s.replace(QRegExp("\""), "\"\"");
			if(s.contains(QRegExp("[,\"\r\n]")) || s.stripWhiteSpace() != s){
				s.prepend('\"');
				s.append('\"');
			} else if(s.isEmpty() && !s.isNull()){
				s = "\"\"";
			}
		}
		return list.join(",");


}
QString SharpFormat::getTodoString( Todo* todo )
{
    QStringList list;
    list.append(  todo->getID("Sharp_DTM")  );
    list.append( todo->categories().join(",") );

    if ( todo->hasStartDate() ) {
        list.append( dtToString( todo->dtStart()) );
    } else
        list.append( QString() );

    if ( todo->hasDueDate() ) {
        QTime tim;
        if ( todo->doesFloat()) {
        list.append( dtToString( QDateTime(todo->dtDue().date(),QTime( 0,0,0  )), false)) ;
        } else {
            list.append( dtToString(todo->dtDue() ) );
        }
    } else
        list.append( QString() );

    if ( todo->isCompleted() ) {  
        list.append( dtToString( todo->completed()) );
        list.append( "0" ); // yes 0 == completed
    } else {
        list.append( dtToString( todo->completed()) );
        list.append( "1" );
    }
    list.append( QString::number( todo->priority() ));
    if( ! todo->summary().isEmpty() )
        list.append( todo->summary() );
    else
        list.append( "" );
    if (! todo->description().isEmpty()  )
        list.append( todo->description() );
    else
        list.append( "" );
	for(QStringList::Iterator it=list.begin();
        it!=list.end(); ++it){
        QString& s = (*it);
        s.replace(QRegExp("\""), "\"\"");
        if(s.contains(QRegExp("[,\"\r\n]")) || s.stripWhiteSpace() != s){
            s.prepend('\"');
            s.append('\"');
        } else if(s.isEmpty() && !s.isNull()){
            s = "\"\"";
        }
    }
    return list.join(",");
}
QString SharpFormat::getPart( const QString & text, bool &ok, int &start )
{
    //qDebug("start %d ", start);
  
    QString retval ="";
    if ( text.at(start) == '"' ) {
        if ( text.mid( start,2) == "\"\"" && !( text.mid( start+2,1) == "\"")) {
            start = start +2; 
            if ( text.mid( start,1) == "," ) {
                start += 1;
            }
            retval = "";
            if ( text.mid( start,1) == "\n" ) {
                start += 1;
                ok = false; 
            }
            return retval;
        }
        int hk = start+1;
        hk = text.find ('"',hk);
        while ( text.at(hk+1) == '"' )
            hk = text.find ('"',hk+2);
        retval = text.mid( start+1, hk-start-1);
        start = hk+1;
        retval.replace( QRegExp("\"\""), "\""); 
        if ( text.mid( start,1) == "," ) {
            start += 1;
        }
        if ( text.mid( start,1) == "\n" ) {
            start += 1;
            ok = false; 
        }
        //qDebug("retval***%s*** ",retval.latin1() );
        return retval;

    } else {
        int nl = text.find ("\n",start);
        int kom = text.find (',',start);
        if ( kom < nl ) {
            // qDebug("kom < nl %d ", kom);
            retval = text.mid(start, kom-start);
            start = kom+1;
            return retval;
        } else {
            if ( nl == kom ) {
                // qDebug(" nl == kom ");
                start = 0;
                ok = false;
                return "0";
            }
            // qDebug(" nl < kom ", nl);
            retval = text.mid( start, nl-start);
            ok = false;
            start = nl+1;
            return retval;
        }
    }
}
bool SharpFormat::fromString( Calendar *calendar, const QString & text)
{
    return false;
}
bool SharpFormat::fromString2Cal( Calendar *calendar,Calendar *existingCalendar, const QString & text, const QString & type)
{
    // qDebug("test %s ", text.latin1());
    QStringList templist;
    QString tempString;
    int start = 0;
    int len = text.length();
    int end = text.find ("\n",start)+1;
    bool ok = true;
    start = end;
    SharpParser handler( calendar );
    // handler.setCategoriesList( mCategories );
    while ( start > 0 ) {
        templist.clear();
        ok = true;
        while ( ok ) {
            tempString = getPart(  text, ok, start );
            if ( start >= len || start == 0 ) {
                start = 0;
                ok = false;
            }
            if ( tempString.right(1) =="\n" )
                tempString = tempString.left( tempString.length()-1);
            //if ( ok ) 
                templist.append( tempString ); 
                //qDebug("%d ---%s---", templist.count(),tempString.latin1() );
        }
        handler.startElement( existingCalendar, templist, type );
    }

    return false;
}

QString SharpFormat::toString( Calendar * )
{
  return QString::null;
}
