TEMPLATE	= lib
CONFIG += qt warn_on
#release debug
TARGET = microkabc_opie

INCLUDEPATH += $(KDEPIMDIR)/kabc $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kio/kio $(KDEPIMDIR)/microkde/kresources $(KDEPIMDIR)/qtcompat $(QPEDIR)/include $(OPIEDIR)/include


OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde 
LIBS += -lmicrokabc
LIBS += -L$(QPEDIR)/lib
LIBS += -L$(OPIEDIR)/lib
LIBS += -lopie
LIBS += -lqpe
LIBS += -lqte

INTERFACES = \

HEADERS = \
 resourceopie.h \
 resourceopieconfig.h \
 opieconverter.h \
 
SOURCES = \
 resourceopie.cpp \
 resourceopieconfig.cpp \
 opieconverter.cpp \

