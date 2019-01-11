
TEMPLATE = app
CONFIG += qt 
include( ../../variables.pri )
DESTDIR= ../../bin
TARGET		= kdeabdump34
INCLUDEPATH += . $(KDEDIR)/include  $(KDE_DEV_DIR)/libkdepim
#LIBS += $(KDEDIR)/lib/libkcal.so
LIBS += $(KDEDIR)/lib/libkabc.so
LIBS += $(KDEDIR)/lib/libkdepim.so
HEADERS +=

SOURCES += main.cpp
