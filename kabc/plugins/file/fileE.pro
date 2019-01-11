TEMPLATE	= lib
CONFIG += qt warn_on release
#release debug

TARGET = microkabc_file
INCLUDEPATH += $(KDEPIMDIR)/kabc $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kio/kio $(KDEPIMDIR)/microkde/kresources $(KDEPIMDIR)/qtcompat
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde -lmicrokabc
LIBS += -L$(QPEDIR)/lib

INTERFACES = \

HEADERS = \
 resourcefile.h \
 resourcefileconfig.h
 
SOURCES = \
 resourcefile.cpp \
 resourcefileconfig.cpp
