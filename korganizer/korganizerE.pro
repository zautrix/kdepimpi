TEMPLATE	= app
CONFIG		+= qt warn_on
TARGET		= kopi
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR=$(QPEDIR)/bin

INCLUDEPATH += $(KDEPIMDIR) $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kio/kio $(KDEPIMDIR)/microkde/kdeui $(KDEPIMDIR)/qtcompat $(KDEPIMDIR)/libkdepim interfaces $(KDEPIMDIR)/kabc $(QPEDIR)/include

DEFINES += KORG_NODND KORG_NOPLUGINS  KORG_NOARCHIVE KORG_NOMAIL
DEFINES += KORG_NOPRINTER  KORG_NODCOP KORG_NOKALARMD KORG_NORESOURCEVIEW KORG_NOSPLITTER
DEFINES += KORG_NOLVALTERNATION
#KORG_NOKABC
LIBS += -lmicrokdepim
LIBS += -lmicrokcal
LIBS += -lmicrokde
LIBS += -lmicroqtcompat

LIBS += $(GCC3EXTRALIB1)
LIBS += $(GCC3EXTRALIB2)


LIBS += -lqpe
LIBS += -ljpeg
LIBS += $(QTOPIALIB)
LIBS += -L$(QPEDIR)/lib
 
INTERFACES =  kofilterview_base.ui 
#filteredit_base.ui

HEADERS = datenavigatorcontainer.h \
 wordsgerman.h \
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
 kdateedit.h \
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
 ../kalarmd/alarmdialog.h

SOURCES = datenavigatorcontainer.cpp \
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

