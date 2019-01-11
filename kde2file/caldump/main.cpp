/*******************************************************************************
 * main.cpp                                                                    *
 *                                                                             *
 * 
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify        *
 * it under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with this program; if not, write to the Free Software                 *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
 *                                                                             *
 * As a special exception, permission is given to link this program            *
 * with any edition of Qt, and distribute the resulting executable,            *
 * without including the source code for Qt in the source distribution.        *
 *                                                                             *
 ******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <libkcal/calformat.h>
#include <libkcal/calendarresources.h>
#include <libkcal/resourcelocal.h>
#include <libkcal/filestorage.h>
#include <libkcal/icalformat.h>

#include <qdatetime.h>
#include <qfile.h>
#include <qdir.h>
#include <qapplication.h>

#include <stdlib.h>
#include <iostream>

using namespace KCal;
using namespace std;

static const char progName[] = "kdecalendar";
static const char progDisplay[] = "KDE_Calendar";
static const char progVersion[] = "33.1/3";
static const char progDesc[] = "A command line interface to KDE calendars";


static KCmdLineOptions options[] =
    {
        { "dump",
          I18N_NOOP( "Dumps calendar" ), 0 },
        { "read",
          I18N_NOOP( "Reads calendar" ), 0 },
        KCmdLineLastOption
    };

int main( int argc, char *argv[] )
{
    KAboutData aboutData(
                         progName,                        // internal program name
                         I18N_NOOP( progDisplay ),        // displayable program name.
                         progVersion,                     // version string
                         I18N_NOOP( progDesc ),           // short porgram description
                         KAboutData::License_GPL,         // license type
                         "(c) 2004, Lutz Rogowski", // copyright statement
                         0,                               // any free form text
                         "",                         // program home page address
                         "bugs.kde.org"                   // bug report email address
                         );


    // KCmdLineArgs::init() final 'true' argument indicates no commandline options
    // for QApplication/KApplication (no KDE or Qt options)
    KCmdLineArgs::init( argc, argv, &aboutData, true );
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

    KInstance ins  ( progName );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    bool read = false;
    if ( args->isSet( "read" ) ) {
        read = true;
        qDebug("read ");
    }
    QString fileName = QDir::homeDirPath ()+"/.kdecalendardump.ics";
    CalendarResources *calendarResource = 0;
    CalendarLocal *localCalendar = 0;
    KConfig c( locate( "config", "korganizerrc" ) );
    c.setGroup( "Time & Date" );
    QString tz = c.readEntry( "TimeZoneId" );
    calendarResource = new CalendarResources( tz );
    calendarResource->readConfig();
    calendarResource->load();
    qDebug("************************************* ");
    qDebug("**************kdecaldump************* ");
    qDebug("************************************* ");
    qDebug("Using timezone ID: %s", calendarResource->timeZoneId().latin1());

    if ( !read ) {
        localCalendar = new CalendarLocal();
        localCalendar->setTimeZoneId( calendarResource->timeZoneId());
        KCal::Incidence::List  allInc = calendarResource->rawIncidences();
        Incidence::List::ConstIterator it;
        int num = 0;
        for( it = allInc.begin(); it != allInc.end(); ++it ) {
            ResourceCalendar * re = calendarResource->resource( (*it) );
            //if ( re ) 
                {
                ++num;
                Incidence* cl = (*it)->clone();
                cl->setLastModified( (*it)->lastModified() );
                if ( cl->type() == "Journal" )
                    localCalendar->addJournal( (Journal *) cl  );
                else if ( cl->type() == "Todo" )
                    localCalendar->addTodo( (Todo *) cl );
                else if ( cl->type() == "Event" )
                    localCalendar->addEvent( (Event *) cl );
            }
        }
  
        FileStorage* storage = new FileStorage( calendarResource );
        storage->setFileName( fileName );
        storage->setSaveFormat( new ICalFormat() );
        storage->save();
        delete storage;
        qDebug("************************************* ");
        qDebug("************kdecaldump*************** ");
        qDebug("************************************* ");
        qDebug("%d calendar entries dumped to file %s", num, fileName.latin1());

    } else {
        qDebug("************load");
        localCalendar = new CalendarLocal();
        localCalendar->setTimeZoneId( calendarResource->timeZoneId());
        FileStorage* storage = new FileStorage( localCalendar );
        storage->setFileName( fileName );
        int num = 0;
        int del = 0;
        int add = 0;
        if ( storage->load() ) {
            qDebug("***********loaded!");
            KCal::Incidence::List  newInc = localCalendar->rawIncidences();
            Incidence::List::ConstIterator it;
            for( it = newInc.begin(); it != newInc.end(); ++it ) {
                if ( (*it)->pilotId() > 1 ) { //changed
                    qDebug("*********pilot id %d %s ",(*it)->pilotId() ,(*it)->summary().latin1());
                    Incidence *incOld = calendarResource->incidence( (*it)->uid() );
                    ResourceCalendar * res = 0;
                    if ( incOld )
                        res = calendarResource->resource( incOld );
                    if ( res ) {
                        Incidence* cl = (*it)->clone();
                        cl->setPilotId( incOld->pilotId() );
                        ++num;
                        if ( incOld->type() == "Journal" )
                            calendarResource->deleteJournal( (Journal *) incOld  );
                        else if ( incOld->type() == "Todo" )
                            calendarResource->deleteTodo( (Todo *) incOld );
                        else if ( incOld->type() == "Event" )
                            calendarResource->deleteEvent( (Event *) incOld );
                        qDebug("*********change incidence %s ",cl->summary().latin1());
                        if ( cl->type() == "Journal" )
                            calendarResource->addJournal( (Journal *) cl, res  );
                        else if ( cl->type() == "Todo" )
                            calendarResource->addTodo( (Todo *) cl, res  );
                        else if ( cl->type() == "Event" )
                            calendarResource->addEvent( (Event *) cl, res  );

                    } else {
                        Incidence* cl = (*it)->clone();
                        qDebug("*********add incidence %s ",cl->summary().latin1());
                        calendarResource->addIncidence( cl );
                        ++add;
                    }
                } else { // maybe added
                    Incidence *incOld = calendarResource->incidence( (*it)->uid() );
                    if ( !incOld ) { //added
                        Incidence* cl = (*it)->clone();
                        qDebug("*********add incidence %s ",cl->summary().latin1());
                        calendarResource->addIncidence( cl );
                        ++add;
                    }
                }
            }
            KCal::Incidence::List  allInc = calendarResource->rawIncidences();
                
            for( it = allInc.begin(); it != allInc.end(); ++it ) {
                ResourceCalendar * re = calendarResource->resource( (*it) );
                if ( re && !re->readOnly() ) {
                    Incidence* cl = localCalendar->incidence( (*it)->uid() );
                    if ( !cl ) {
                        ++del;  
                        cl = (*it);
                        if ( cl->type() == "Journal" )
                            calendarResource->deleteJournal( (Journal *) cl  );
                        else if ( cl->type() == "Todo" )
                            calendarResource->deleteTodo( (Todo *) cl );
                        else if ( cl->type() == "Event" )
                            calendarResource->deleteEvent( (Event *) cl );
                        //QDateTime lm = cl->lastModified();
                        //cl->setResources( (*it)->resources() );
                        //cl->setLastModified(lm);
                    }  
                }
            }
            
            calendarResource->save();
            qDebug("************************************* ");
            qDebug("************kdecaldump*************** ");
            qDebug("************************************* ");
            qDebug("Calendar entries\nchanged %d\ndeleted %d\nadded %d\nfrom file %s", num,del, add, fileName.latin1());
            
        } else
            qDebug("ERROR loading file %s",fileName.latin1() );

    }

    if ( localCalendar ) {
        localCalendar->close();
        delete localCalendar;
    }
    if ( calendarResource ) {
        calendarResource->close();
        delete calendarResource;
    }

    qDebug("ente ");
    return 0;
}
