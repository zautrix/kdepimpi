TEMPLATE	= lib
CONFIG += qt warn_on release

include( ../../../variables.pri )

TARGET = microkabc_qtopia

INCLUDEPATH += ../.. ../../.. ../../../kabc ../../../microkde ../../../microkde/kdecore ../../../microkde/kio/kfile ../../../microkde/kio/kio ../../../microkde/kresources 

DESTDIR = ../../../bin
#LIBS += -lmicrokde 
#LIBS += -lkamicrokabc

DEFINES += KAB_EMBEDDED  DESKTOP_VERSION

INTERFACES = \

HEADERS = \
 resourceqtopia.h \
 resourceqtopiaconfig.h \
 qtopiaconverter.h

SOURCES = \
 resourceqtopia.cpp \
 resourceqtopiaconfig.cpp \
 qtopiaconverter.cpp 



unix : {
OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix
}
win32: {
CONFIG += dll
DEFINES += _WIN32_
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
LIBS += ../../../bin/microkdepim.lib
LIBS += ../../../bin/microkcal.lib
LIBS += ../../../bin/microkde.lib
LIBS += ../../../bin/microkabc.lib
}
