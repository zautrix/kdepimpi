TEMPLATE	= lib
CONFIG		+= qt warn_on
TARGET = microkdepim
INCLUDEPATH += . $(KDEPIMDIR) $(KDEPIMDIR)/microkde $(KDEPIMDIR)/qtcompat $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kdeui $(QPEDIR)/include
LIBS += -lmicrokde
#LIBS += -lmicrokcal
LIBS += -L$(QPEDIR)/lib
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR=$(QPEDIR)/lib


INTERFACES = \

HEADERS = \
 categoryeditdialog.h \
 categoryeditdialog_base.h \
 categoryselectdialog.h \
 categoryselectdialog_base.h \
 externalapphandler.h \
 kdateedit.h \
 kdatepicker.h \
 kinputdialog.h \
 kpimprefs.h \
 kpimglobalprefs.h \
 kprefsdialog.h \
 kprefswidget.h \
 ksyncmanager.h \
 ksyncprofile.h \
 ksyncprefsdialog.h \
 kcmconfigs/kcmkdepimconfig.h \
 kcmconfigs/kdepimconfigwidget.h \
 phoneaccess.h   
 


SOURCES = \
 categoryeditdialog.cpp \
 categoryeditdialog_base.cpp \
 categoryselectdialog.cpp \
 categoryselectdialog_base.cpp \
 externalapphandler.cpp \
 kdateedit.cpp \
 kinputdialog.cpp \
 kdatepicker.cpp \
 kpimprefs.cpp \
 kpimglobalprefs.cpp \
 kprefsdialog.cpp \
 kprefswidget.cpp \
 ksyncmanager.cpp \
 ksyncprofile.cpp \
 ksyncprefsdialog.cpp \
 kcmconfigs/kcmkdepimconfig.cpp \
 kcmconfigs/kdepimconfigwidget.cpp \
 phoneaccess.cpp  


