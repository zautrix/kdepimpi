TEMPLATE	= lib
CONFIG += qt warn_on release
#release debug

include( ../../../variables.pri )

TARGET = microkabc_dir
INCLUDEPATH += ../.. ../../../microkde ../../../microkde/kdecore ../../../microkde/kio/kfile ../../../microkde/kio/kio ../../../microkde/kresources ../../../qtcompat
DESTDIR = ../../../bin
#LIBS += -lmicrokde -lmicrokabc
#LIBS += -L$(QPEDIR)/lib

INTERFACES = \

HEADERS = \
 resourcedir.h \
 resourcedirconfig.h
 
SOURCES = \
 resourcedir.cpp \
 resourcedirconfig.cpp

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
