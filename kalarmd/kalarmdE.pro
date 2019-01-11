TEMPLATE	= lib
CONFIG		+= qt warn_on 
TARGET		= kopialarmapplet
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
INCLUDEPATH += $(QPEDIR)/include 

DESTDIR = $(QPEDIR)/plugins/applets

INTERFACES = \

HEADERS = \
 alarmdialog.h \
 simplealarmdaemonapplet.h \
 simplealarmdaemonimpl.h

SOURCES = \
 alarmdialog.cpp \
 simplealarmdaemonapplet.cpp \
 simplealarmdaemonimpl.cpp

