
TEMPLATE = app
CONFIG += qt 
include( ../../variables.pri )
DESTDIR= ../../bin
TARGET		= kdecaldump34
INCLUDEPATH += . $(KDEDIR)/include $(KDEDIR)/include/libkcal $(KDE_DEV_DIR)/libkdepim
LIBS += $(KDEDIR)/lib/libkcal.so
LIBS += $(KDEDIR)/lib/libkdepim.so
HEADERS +=

SOURCES += main.cpp
