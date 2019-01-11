TEMPLATE	= lib
CONFIG		+= qt warn_on
TARGET		= microkcal

include( ../variables.pri )

INCLUDEPATH += ../libkdepim ../microkde  versit ../microkde/kdecore 
#../qtcompat
INCLUDEPATH += ../libical/src/libical
INCLUDEPATH += ../libical/src/libicalss
DESTDIR = ../bin
DEFINES +=  DESKTOP_VERSION
unix: {
LIBS += ../libical/lib/libical.a
LIBS += ../libical/lib/libicalss.a
OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix
}
win32: {
DEFINES += _WIN32_

LIBS += ../libical/lib/ical.lib
LIBS += ../libical/lib/icalss.lib
OBJECTS_DIR = obj/win
MOC_DIR = moc/win

}

INTERFACES = \

HEADERS = \
 alarm.h \
 attachment.h \
 attendee.h \
 calendar.h \
 calendarlocal.h \
 calfilter.h \
 calformat.h \
 calstorage.h \
 compat.h \
 customproperties.h \
 dummyscheduler.h \
kincidenceformatter.h \
 duration.h \
 event.h \
 exceptions.h \
 filestorage.h \
 freebusy.h \
 icaldrag.h \
 icalformat.h \
 icalformatimpl.h \
 imipscheduler.h \
 incidence.h \
 incidencebase.h \
 journal.h \
 period.h \
 person.h \
 qtopiaformat.h \
 recurrence.h \
 scheduler.h \
 todo.h \
dndfactory.h \
 vcaldrag.h \
 vcalformat.h \
 versit/port.h \
 versit/vcc.h \
 versit/vobject.h \
 phoneformat.h \



SOURCES = \
 alarm.cpp \
 attachment.cpp \
 attendee.cpp \
 calendar.cpp \
 calendarlocal.cpp \
 calfilter.cpp \
 calformat.cpp \
 compat.cpp \
 customproperties.cpp \
 dummyscheduler.cpp \
 kincidenceformatter.cpp \
 duration.cpp \
 event.cpp \
 exceptions.cpp \
 filestorage.cpp \
 freebusy.cpp \
 icaldrag.cpp \
 icalformat.cpp \
 icalformatimpl.cpp \
 imipscheduler.cpp \
 incidence.cpp \
 incidencebase.cpp \
 journal.cpp \
 period.cpp \
 person.cpp \
 qtopiaformat.cpp \
 recurrence.cpp \
 scheduler.cpp \
 todo.cpp \
dndfactory.cpp \
 vcaldrag.cpp \
 vcalformat.cpp \
 versit/vcc.c \
 versit/vobject.c \
 phoneformat.cpp \

