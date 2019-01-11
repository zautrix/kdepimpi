TEMPLATE	= lib
CONFIG += qt warn_on

TARGET = microkabc_qtopia

INCLUDEPATH += $(KDEPIMDIR) $(KDEPIMDIR)/kabc $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kio/kio $(KDEPIMDIR)/microkde/kresources $(KDEPIMDIR)/qtcompat $(QPEDIR)/include

OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde 
LIBS += -lmicrokabc
LIBS += -L$(QPEDIR)/lib
LIBS += -lqpe

INTERFACES = \

HEADERS = \
 resourceqtopia.h \
 resourceqtopiaconfig.h \
 qtopiaconverter.h

SOURCES = \
 resourceqtopia.cpp \
 resourceqtopiaconfig.cpp \
 qtopiaconverter.cpp 
