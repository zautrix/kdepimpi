TEMPLATE	= lib
CONFIG += qt warn_on
TARGET = microkabc_olaccess

include( ../../../variables.pri )

INCLUDEPATH += ../../.. ../.. ../../../microkde ../../../microkde/kdecore ../../../microkde/kio/kfile  ../../../microkde/kio/kio ../../../microkde/kresources ../../../qtcompat

INTERFACES = \

DESTDIR = ../../../bin
HEADERS = \
 resourceolaccess.h \
 resourceolaccessconfig.h \
 olaccessconverter.h 
 
SOURCES = \
 resourceolaccess.cpp \
 resourceolaccessconfig.cpp \
 olaccessconverter.cpp

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
