TEMPLATE	= lib
CONFIG += qt warn_on release
#release debug

TARGET = microkabc_ldap
INCLUDEPATH += $(KDEPIMDIR)/kabc $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kresources $(KDEPIMDIR)/qtcompat $(QPEDIR)/include
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde -lmicrokabc
LIBS += -L$(QPEDIR)/lib

INTERFACES = \

HEADERS = \
 resourceldap.h \
 resourceldapconfig.h
 
SOURCES = \
 resourceldap.cpp \
 resourceldapconfig.cpp
