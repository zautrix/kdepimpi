TEMPLATE	= lib
CONFIG += qt warn_on
TARGET = microkabc_sharpdtm

INCLUDEPATH += $(KDEPIMDIR) $(KDEPIMDIR)/kabc $(SHARPDTMSDK)/include $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kio/kio $(KDEPIMDIR)/microkde/kresources $(KDEPIMDIR)/qtcompat $(QPEDIR)/include 


OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde 
LIBS += -lmicrokabc
LIBS += -lmicrokdepim
LIBS += -lmicroqtcompat
LIBS += -L$(QPEDIR)/lib
LIBS += -ljpeg
LIBS += -lqpe
LIBS += -lqte
LIBS += -lzdtm
LIBS += -lsl



INTERFACES = \

HEADERS = \
 resourcesharpdtm.h \
 resourcesharpdtmconfig.h \
 sharpdtmconverter.h 
 
SOURCES = \
 resourcesharpdtm.cpp \
 resourcesharpdtmconfig.cpp \
 sharpdtmconverter.cpp

