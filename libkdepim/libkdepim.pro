TEMPLATE	= lib
CONFIG		= qt warn_on
DEFINES +=KORG_NOKABC
TARGET = microkdepim
INCLUDEPATH += ../microkde ../microkde/kdecore ../microkde/kdeui . ..
DESTDIR=../bin

DEFINES +=  DESKTOP_VERSION
include( ../variables.pri )
unix : {
OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix
}
win32: {
DEFINES += _WIN32_
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
}
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
 kdatepicker.cpp \
 kinputdialog.cpp \
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

win32: {

#olimport section 
importol {
debug: {
LIBS += mfc71ud.lib
}
release: {
LIBS += mfc71u.lib
}
DEFINES +=  _OL_IMPORT_
HEADERS += ol_access.h 
SOURCES += ol_access.cpp 
#olimport section end
}
}
