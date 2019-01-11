TEMPLATE	= app
CONFIG		= qt warn_on
TARGET		= kopi
OBJECTS_DIR = _obj/
MOC_DIR = _moc
DESTDIR= ../bin

include( ../variables.pri )

INCLUDEPATH += ../microkde  ../ interfaces ../microkde/kdecore ../microkde/kdeui ../microkde/kio/kfile ../microkde/kio/kio ../libkdepim
#../qtcompat
DEFINES += KORG_NOPLUGINS KORG_NOARCHIVE KORG_NOMAIL
DEFINES += KORG_NODCOP KORG_NOKALARMD KORG_NORESOURCEVIEW KORG_NOSPLITTER
#KORG_NOPRINTER KORG_NOKABC KORG_NODND 
DEFINES += KORG_NOLVALTERNATION
DEFINES +=  DESKTOP_VERSION
unix : {
staticlib: { 
TARGET		= kopi_linux
LIBS += ../bin/libmicrokabc_qtopia.a
LIBS += ../bin/libmicrokabc_file.a
LIBS += ../bin/libmicrokabc_dir.a
LIBS += ../bin/libmicrokdepim.a
LIBS += ../bin/libmicrokcal.a
LIBS += ../bin/libmicrokabc.a
LIBS += ../bin/libmicrokde.a
LIBS += ../bin/libmicrokabc_qtopia.a
LIBS += ../bin/libmicrokabc_file.a
LIBS += ../bin/libmicrokabc_dir.a
LIBS += ../bin/libmicrokdepim.a
LIBS += ../bin/libmicrokcal.a
LIBS += ../bin/libmicrokabc.a
LIBS += ../bin/libmicrokde.a
LIBS += ../libical/lib/libical.a
LIBS += ../libical/lib/libicalss.a
} else {
LIBS += ../bin/libmicrokdepim.so
LIBS += ../bin/libmicrokcal.so
LIBS += ../bin/libmicrokde.so
LIBS += ../bin/libmicrokabc.so
#LIBS += -lbluetooth
#LIBS += -lsdp

#LIBS += -lldap
}
OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix
}
win32: {
RC_FILE = winicons.rc
DEFINES += _WIN32_  
LIBS += ../bin/microkdepim.lib
LIBS += ../bin/microkcal.lib
LIBS += ../bin/microkde.lib
LIBS += ../bin/microkabc.lib
LIBS += ../libical/lib/ical.lib
LIBS += ../libical/lib/icalss.lib
#LIBS += atls.lib
QMAKE_LINK += /NODEFAULTLIB:LIBC
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
#olimport section 
importol: {
debug: {
mfc71u:LIBS += mfc71ud.lib
QMAKE_LINK += /NODEFAULTLIB:MSVCRT
QMAKE_LINK += /NODEFAULTLIB:uafxcw.lib
}
release: {
mfc71u:LIBS += mfc71u.lib
}
DEFINES +=  _OL_IMPORT_

HEADERS += ../outport/msoutl9.h \
 koimportoldialog.h 

SOURCES += ../outport/msoutl9.cpp \
 koimportoldialog.cpp
#olimport section end
TARGET		= kopi_xp
} else {
TARGET		= kopi_me
}

}

 
INTERFACES = kofilterview_base.ui 
#filteredit_base.ui

# kdateedit.h \

HEADERS += datenavigatorcontainer.h \
 filteredit_base.h \
 alarmclient.h \
 calendarview.h \
 customlistviewitem.h \
 datenavigator.h \
 docprefs.h \
 filtereditdialog.h \
 incomingdialog.h \
 incomingdialog_base.h \
 interfaces/korganizer/baseview.h \
 interfaces/korganizer/calendarviewbase.h \
 journalentry.h \
 kdatenavigator.h \
 koagenda.h \
 koagendaitem.h \
 koagendaview.h \
 kocounterdialog.h \
 kodaymatrix.h \
 kodialogmanager.h \
 koeditordetails.h \
 koeditorgeneral.h \
 koeditorgeneralevent.h \
 koeditorgeneraltodo.h \
 koeditorrecurrence.h \
 koeventeditor.h \
 koeventpopupmenu.h \
 koeventview.h \
 koeventviewer.h \
 koeventviewerdialog.h \
 kofilterview.h \
 koglobals.h \
 koincidenceeditor.h \
 kojournalview.h \
 kolistview.h \
 kolocationbox.h \
 komonthview.h \
 koprefs.h \
 koprefsdialog.h \
 kotimespanview.h \
 kotodoeditor.h \
 kotodoview.h \
 kotodoviewitem.h \
 koviewmanager.h \
 kowhatsnextview.h \
 ktimeedit.h \
 lineview.h \
 mainwindow.h \
 navigatorbar.h \
 outgoingdialog.h \
 outgoingdialog_base.h \
 publishdialog.h \
 publishdialog_base.h \
 savetemplatedialog.h \
 searchdialog.h \
 statusdialog.h \
 timeline.h \
 timespanview.h \
 version.h \
 ../kalarmd/alarmdialog.h \


SOURCES += datenavigatorcontainer.cpp \ 
filteredit_base.cpp \
 calendarview.cpp \
 datenavigator.cpp \
 docprefs.cpp \
 filtereditdialog.cpp \
 incomingdialog.cpp \
 incomingdialog_base.cpp \
 journalentry.cpp \
 kdatenavigator.cpp \
 koagenda.cpp \
 koagendaitem.cpp \
 koagendaview.cpp \
 kocounterdialog.cpp \
 kodaymatrix.cpp \
 kodialogmanager.cpp \
 koeditordetails.cpp \
 koeditorgeneral.cpp \
 koeditorgeneralevent.cpp \
 koeditorgeneraltodo.cpp \
 koeditorrecurrence.cpp \
 koeventeditor.cpp \
 koeventpopupmenu.cpp \
 koeventview.cpp \
 koeventviewer.cpp \
 koeventviewerdialog.cpp \
 kofilterview.cpp \
 koglobals.cpp \
 koincidenceeditor.cpp \
 kojournalview.cpp \
 kolistview.cpp \
 kolocationbox.cpp \
 komonthview.cpp \
 koprefs.cpp \
 koprefsdialog.cpp \
 kotimespanview.cpp \
 kotodoeditor.cpp \
 kotodoview.cpp \
 kotodoviewitem.cpp \
 koviewmanager.cpp \
 kowhatsnextview.cpp \
 ktimeedit.cpp \
 lineview.cpp \
 main.cpp \
 mainwindow.cpp \
 navigatorbar.cpp \
 outgoingdialog.cpp \
 outgoingdialog_base.cpp \
 publishdialog.cpp \
 publishdialog_base.cpp \
 savetemplatedialog.cpp \
 searchdialog.cpp \
 statusdialog.cpp \
 timeline.cpp \
 timespanview.cpp \
 ../kalarmd/alarmdialog.cpp

HEADERS += calprintbase.h calprinter.h calprintplugins.h cellitem.h
INTERFACES += calprintdayconfig_base.ui \
              calprintmonthconfig_base.ui \
              calprinttodoconfig_base.ui \
              calprintweekconfig_base.ui
SOURCES += calprintbase.cpp calprinter.cpp calprintplugins.cpp cellitem.cpp

