TEMPLATE	= lib
CONFIG		+= qt warn_on
TARGET		= microkcal

INCLUDEPATH += ../libkdepim ../microkde ../qtcompat versit ../microkde/kdecore $(QPEDIR)/include
INCLUDEPATH += ../libical/src/libical
INCLUDEPATH += ../libical/src/libicalss
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
#LIBS += ../libical/lib/$(PLATFORM)/libical.a
#LIBS += ../libical/lib/$(PLATFORM)/libicalss.a
LIBS += ../dest$(LIBICAL_PATH)/libical.a
LIBS += ../dest$(LIBICAL_PATH)/libicalss.a

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
kincidenceformatter.h \
 journal.h \
 period.h \
 person.h \
 qtopiaformat.h \
 sharpformat.h \
 phoneformat.h \
 recurrence.h \
 scheduler.h \
 todo.h \
 vcaldrag.h \
 vcalformat.h \
 versit/port.h \
 versit/vcc.h \
 versit/vobject.h \

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
 kincidenceformatter.cpp \
 journal.cpp \
 period.cpp \
 person.cpp \
 qtopiaformat.cpp \
 sharpformat.cpp \
 phoneformat.cpp \
 recurrence.cpp \
 scheduler.cpp \
 todo.cpp \
 vcaldrag.cpp \
 vcalformat.cpp \
 versit/vcc.c \
 versit/vobject.c \

