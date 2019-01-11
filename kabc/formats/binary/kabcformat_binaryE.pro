TEMPLATE	= lib
CONFIG += qt warn_on release
#release debug

TARGET = microkabcformat_binary
#INCLUDEPATH += . ./vcard/include ./vcard/include/generated ../microkde ../microkde/kdecore ../microkde/kdeui ../microkde/kio/kfile ../qtcompat
INCLUDEPATH += ../.. ../../../microkde ../../../microkde/kdecore ../../../qtcompat
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde -lmicrokabc
LIBS += -L$(QPEDIR)/lib
DEFINES += KAB_EMBEDDED

INTERFACES = \

HEADERS = \
 binaryformat.h
 
SOURCES = \
 binaryformat.cpp
