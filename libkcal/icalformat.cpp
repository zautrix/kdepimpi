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

#include <qapplication.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qclipboard.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <stdlib.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

extern "C" {
  #include <ical.h>
  #include <icalss.h>
  #include <icalparser.h>
  #include <icalrestriction.h>
}

#include "calendar.h"
#include "calendarlocal.h"
#include "journal.h"

#include "icalformat.h"
#include "icalformatimpl.h"

#define _ICAL_VERSION "2.0"

using namespace KCal;

ICalFormat::ICalFormat(  bool pe )
{
    mProcessEvents = pe;
   mImpl = new ICalFormatImpl( this );
   tzOffsetMin = 0;
   //qDebug("new ICalFormat() ");
}

ICalFormat::~ICalFormat()
{
  delete mImpl; 
  //qDebug("delete ICalFormat ");
}

bool ICalFormat::load( Calendar *calendar, const QString &fileName)
{

  clearException();

  QFile file( fileName );  
  if (!file.open( IO_ReadOnly ) ) {
    setException(new ErrorFormat(ErrorFormat::LoadError));
    return false;
  } 
  QTextStream ts( &file );
  QString text; 

  ts.setEncoding( QTextStream::Latin1 );
  text = ts.read(); 
  file.close();

  return fromString( calendar, text );
}

//#include <qdatetime.h>
bool ICalFormat::save( Calendar *calendar, const QString &fileName )
{
    //kdDebug(5800) << "ICalFormat::save(): " << fileName << endl;
    //qDebug("ICalFormat::save ");
  clearException();
  QString text = toString( calendar );
  //return false; 
  // qDebug("to string takes ms: %d ",is.elapsed() );
  if ( text.isNull() ) return false;

  // TODO: write backup file
  //is.restart();
  QFile file( fileName );
  if (!file.open( IO_WriteOnly ) ) {
    setException(new ErrorFormat(ErrorFormat::SaveError,
                 i18n("Could not open file '%1'").arg(fileName)));
    return false;
  }
  QTextStream ts( &file );

  ts.setEncoding( QTextStream::Latin1 );
  ts << text;
  file.close();
  //qDebug("saving file takes ms: %d ", is.elapsed() );
  return true;
}

bool ICalFormat::fromString( Calendar *cal, const QString &text )
{
  setTimeZone( cal->timeZoneId(), !cal->isLocalTime() );
  // qDebug("ICalFormat::fromString tz: %s ", cal->timeZoneId().latin1());
  // Get first VCALENDAR component.
  // TODO: Handle more than one VCALENDAR or non-VCALENDAR top components
  icalcomponent *calendar;

  //calendar = icalcomponent_new_from_string( text.local8Bit().data());
  // good calendar = icalcomponent_new_from_string( text.utf8().data());
  calendar = icalcomponent_new_from_string( (char*)text.latin1());
  if (!calendar) {
    setException(new ErrorFormat(ErrorFormat::ParseErrorIcal));
    return false;
  }

  bool success = true;

  if (icalcomponent_isa(calendar) != ICAL_VCALENDAR_COMPONENT) {
    setException(new ErrorFormat(ErrorFormat::NoCalendar));
    success = false;
  } else {
    // put all objects into their proper places
    if ( !mImpl->populate( cal, calendar ) ) {
      if ( !exception() ) {
        setException(new ErrorFormat(ErrorFormat::ParseErrorKcal));
      } 
      success = false;
    } else
      mLoadedProductId = mImpl->loadedProductId();
  }

  icalcomponent_free( calendar );
  icalmemory_free_ring();

  return success;
}

Incidence *ICalFormat::fromString( const QString &text )
{
  CalendarLocal cal( mTimeZoneId );
  fromString(&cal, text);
  
  Incidence *ical = 0;
  QPtrList<Event> elist = cal.events();
  if ( elist.count() > 0 ) {
    ical = elist.first();
  } else {
    QPtrList<Todo> tlist = cal.todos();
    if ( tlist.count() > 0 ) {
      ical = tlist.first();
    } else {
      QPtrList<Journal> jlist = cal.journals();
      if ( jlist.count() > 0 ) {
        ical = jlist.first();
      }
    }
  }
  return ical;
}


QString ICalFormat::toString( Calendar *cal )
{
 
  setTimeZone( cal->timeZoneId(), !cal->isLocalTime() );
 
  icalcomponent *calendar = mImpl->createCalendarComponent(cal);

  icalcomponent *component;
 
  // todos 
  QPtrList<Todo> todoList = cal->rawTodos();
  QPtrListIterator<Todo> qlt(todoList);
  for (; qlt.current(); ++qlt) {
    component = mImpl->writeTodo(qlt.current());
    icalcomponent_add_component(calendar,component);
    //qDebug(" todos ");
    if ( mProcessEvents ) {
        //qDebug("mProcessEvents ");
        qApp->processEvents();
    }
  }
  // events
  QPtrList<Event> events = cal->rawEvents();
  Event *ev;
  for(ev=events.first();ev;ev=events.next()) {
    component = mImpl->writeEvent(ev);
    icalcomponent_add_component(calendar,component);
    //qDebug("events ");
    if ( mProcessEvents )
        qApp->processEvents();
  }
 
  // journals
  QPtrList<Journal> journals = cal->journals();
  Journal *j;
  for(j=journals.first();j;j=journals.next()) {
    component = mImpl->writeJournal(j);
    icalcomponent_add_component(calendar,component);
    //qDebug("journals ");
    if ( mProcessEvents )
        qApp->processEvents(); 
  }
   const char *text; 
  QString ret =""; 
  text = icalcomponent_as_ical_string( calendar ); 
  if ( mProcessEvents )
        qApp->processEvents(); 

  // text = "BEGIN:VCALENDAR\nPRODID\n :-//K Desktop Environment//NONSGML libkcal 3.1//EN\nVERSION\n :2.0\nBEGIN:VEVENT\nDTSTAMP\n :20031231T213514Z\nORGANIZER\n :MAILTO:lutz@putz.de\nCREATED\n :20031231T213513Z\nUID\n :libkcal-1295166342.120\nSEQUENCE\n :0\nLAST-MODIFIED\n :20031231T213513Z\nSUMMARY\n :test1\nCLASS\n :PUBLIC\nPRIORITY\n :3\nDTSTART\n :20040101T090000Z\nDTEND\n :20040101T110000Z\nTRANSP\n :OPAQUE\nEND:VEVENT\nEND:VCALENDAR\n";


   if ( text ) {
       ret = QString ( text );
   }
   icalcomponent_free( calendar );

  if (!text) { 
    setException(new ErrorFormat(ErrorFormat::SaveError,
                 i18n("libical error")));
    icalmemory_free_ring();
    return QString::null;
  }
  icalmemory_free_ring();
  return ret;
}

QString ICalFormat::toICalString( Incidence *incidence )
{
  CalendarLocal cal( mTimeZoneId );
  cal.addIncidence( incidence->clone() );
  return toString( &cal );
}

QString ICalFormat::toString( Incidence *incidence )
{
  icalcomponent *component;

  component = mImpl->writeIncidence( incidence );

  const char *text = icalcomponent_as_ical_string( component );

  icalcomponent_free( component );

  return QString::fromLocal8Bit( text );
}

QString ICalFormat::toString( Recurrence *recurrence )
{
  icalproperty *property;
  property = mImpl->writeRecurrenceRule( recurrence );
  const char *text = icalproperty_as_ical_string( property );
  icalproperty_free( property );
  return QString::fromLocal8Bit( text );
}
/*
bool ICalFormat::fromString( Recurrence * recurrence, const QString& rrule )
{
  bool success = true;
  icalerror_clear_errno();
  struct icalrecurrencetype recur = icalrecurrencetype_from_string( rrule );
  if ( icalerrno != ICAL_NO_ERROR ) {
    kdDebug() << "Recurrence parsing error: " << icalerror_strerror( icalerrno ) << endl;
    success = false;
  }

  if ( success ) {
    mImpl->readRecurrence( recur, recurrence );
  }

  return success;
}
*/

QString ICalFormat::createScheduleMessage(IncidenceBase *incidence,
                                          Scheduler::Method method)
{
  icalcomponent *message = mImpl->createScheduleComponent(incidence,method);

  QString messageText = icalcomponent_as_ical_string(message);



  return messageText;
}

ScheduleMessage *ICalFormat::parseScheduleMessage( Calendar *cal,
                                                   const QString &messageText )
{
  setTimeZone( cal->timeZoneId(), !cal->isLocalTime() );
  clearException();

  if (messageText.isEmpty()) return 0;

  icalcomponent *message;
  message = icalparser_parse_string(messageText.local8Bit());

  if (!message) return 0;

  icalproperty *m = icalcomponent_get_first_property(message,
                                                     ICAL_METHOD_PROPERTY);

  if (!m) return 0;

  icalcomponent *c;

  IncidenceBase *incidence = 0;
  c = icalcomponent_get_first_component(message,ICAL_VEVENT_COMPONENT);
  if (c) {
    incidence = mImpl->readEvent(c);
  }

  if (!incidence) {
    c = icalcomponent_get_first_component(message,ICAL_VTODO_COMPONENT);
    if (c) {
      incidence = mImpl->readTodo(c);
    }
  }

  if (!incidence) {
    c = icalcomponent_get_first_component(message,ICAL_VFREEBUSY_COMPONENT);
    if (c) {
      incidence = mImpl->readFreeBusy(c);
    }
  }

  if (!incidence) {
    kdDebug() << "ICalFormat:parseScheduleMessage: object is not a freebusy, event or todo" << endl;
    return 0;
  }

  kdDebug(5800) << "ICalFormat::parseScheduleMessage() getting method..." << endl;

  icalproperty_method icalmethod = icalproperty_get_method(m);
  Scheduler::Method method;

  switch (icalmethod) {
    case ICAL_METHOD_PUBLISH:
      method = Scheduler::Publish;
      break;
    case ICAL_METHOD_REQUEST:
      method = Scheduler::Request;
      break;
    case ICAL_METHOD_REFRESH:
      method = Scheduler::Refresh;
      break;
    case ICAL_METHOD_CANCEL:
      method = Scheduler::Cancel;
      break;
    case ICAL_METHOD_ADD:
      method = Scheduler::Add;
      break;
    case ICAL_METHOD_REPLY:
      method = Scheduler::Reply;
      break;
    case ICAL_METHOD_COUNTER:
      method = Scheduler::Counter;
      break;
    case ICAL_METHOD_DECLINECOUNTER:
      method = Scheduler::Declinecounter;
      break;
    default:
      method = Scheduler::NoMethod;
      kdDebug(5800) << "ICalFormat::parseScheduleMessage(): Unknow method" << endl;
      break;
  }


  if (!icalrestriction_check(message)) {
    setException(new ErrorFormat(ErrorFormat::Restriction,
                                   Scheduler::translatedMethodName(method) + ": " +
                                   mImpl->extractErrorProperty(c)));
    return 0;
  }

  icalcomponent *calendarComponent = mImpl->createCalendarComponent(cal);

  Incidence *existingIncidence = cal->event(incidence->uid());
  if (existingIncidence) {
    // TODO: check, if cast is required, or if it can be done by virtual funcs.
    if (existingIncidence->typeID() == todoID ) {
      Todo *todo = static_cast<Todo *>(existingIncidence);
      icalcomponent_add_component(calendarComponent,
                                  mImpl->writeTodo(todo));
    }
    if (existingIncidence->typeID() == eventID ) {
      Event *event = static_cast<Event *>(existingIncidence);
      icalcomponent_add_component(calendarComponent,
                                  mImpl->writeEvent(event));
    }
  } else {
    calendarComponent = 0;
  }
  qDebug("icalclassify commented out ");
  ScheduleMessage::Status status;
#if 0

  icalclass result = icalclassify(message,calendarComponent,(char *)"");



  switch (result) {
    case ICAL_PUBLISH_NEW_CLASS:
      status = ScheduleMessage::PublishNew;
      break;
    case ICAL_OBSOLETE_CLASS:
      status = ScheduleMessage::Obsolete;
      break;
    case ICAL_REQUEST_NEW_CLASS:
      status = ScheduleMessage::RequestNew;
      break;
    case ICAL_REQUEST_UPDATE_CLASS:
      status = ScheduleMessage::RequestUpdate;
      break;
    case ICAL_UNKNOWN_CLASS:
    default:
      status = ScheduleMessage::Unknown;
      break;
  }
#endif
  status = ScheduleMessage::RequestUpdate;
  return new ScheduleMessage(incidence,method,status);
}

void ICalFormat::setTimeZone( const QString &id, bool utc )
{


    mTimeZoneId = id;
    mUtc = utc;

    tzOffsetMin  = KGlobal::locale()->timezoneOffset(mTimeZoneId);
  
    //qDebug("ICalFormat::setTimeZoneOffset %s %d ",mTimeZoneId.latin1(),  tzOffsetMin);
}

QString ICalFormat::timeZoneId() const
{
  return mTimeZoneId;
}

bool ICalFormat::utc() const
{
  return mUtc;
}
int ICalFormat::timeOffset()
{
    return tzOffsetMin;
}
const char *ICalFormat::tzString()
{
    const char*  ret = (const char* ) mTzString;
    return ret;
}
