######################################################################
# Automatically generated by qmake (1.07a) Sun Jun 27 23:03:36 2004
######################################################################


TEMPLATE	= lib

TARGET          = icalss
DESTDIR = ../../lib
CONFIG		+= staticlib

include( ../../../variables.pri ) 
INCLUDEPATH     += . ../libical
# Input
win32 {
DEFINES += YY_NO_UNISTD_H

}
HEADERS += icalcalendar.h \
           icalclassify.h \
           icalcluster.h \
           icalclusterimpl.h \
           icaldirset.h \
           icaldirsetimpl.h \
           icalfileset.h \
           icalfilesetimpl.h \
           icalgauge.h \
           icalgaugeimpl.h \
           icalmessage.h \
           icalset.h \
           icalspanlist.h \
           icalss.h \
           icalssyacc.h
SOURCES += icalcalendar.c \
           icalclassify.c \
           icalcluster.c \
           icaldirset.c \
           icalfileset.c \
           icalgauge.c \
           icalmessage.c \
           icalset.c \
           icalspanlist.c \
           icalsslexer.c \
           icalssyacc.c
