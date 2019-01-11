TEMPLATE	= lib
CONFIG += qt warn_on release
#release debug

TARGET = microkabc_dir
INCLUDEPATH += $(KDEPIMDIR)/kabc $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kio/kio $(KDEPIMDIR)/microkde/kresources $(KDEPIMDIR)/qtcompat
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde -lmicrokabc
LIBS += -L$(QPEDIR)/lib

INTERFACES = \

HEADERS = \
 resourcedir.h \
 resourcedirconfig.h
 
SOURCES = \
 resourcedir.cpp \
 resourcedirconfig.cpp
