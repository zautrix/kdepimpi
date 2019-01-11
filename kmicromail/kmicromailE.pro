CONFIG        += qt warn_on 
TEMPLATE	= app
HEADERS         = defines.h \
               editaccounts.h \
               composemail.h \
               accountview.h \
               accountitem.h \
               mainwindow.h \
               viewmail.h \
               viewmailbase.h \
               opiemail.h \
               mailistviewitem.h \
               settingsdialog.h \
               statuswidget.h \
               newmaildir.h \
               selectstore.h \
               selectsmtp.h \
               nntpgroups.h \
               koprefs.h \
               koprefsdialog.h \
               nntpgroupsdlg.h

SOURCES         = main.cpp  \
               opiemail.cpp \
               mainwindow.cpp \
               accountview.cpp \
               accountitem.cpp \
               composemail.cpp \
               editaccounts.cpp \
               viewmail.cpp \
               viewmailbase.cpp \
               mailistviewitem.cpp \
               settingsdialog.cpp \
               statuswidget.cpp \
               newmaildir.cpp \
               selectstore.cpp \
               selectsmtp.cpp \
               nntpgroups.cpp \
               koprefs.cpp\
               koprefsdialog.cpp\
               nntpgroupsdlg.cpp

INTERFACES     = editaccountsui.ui \
               selectmailtypeui.ui \
               imapconfigui.ui \
               pop3configui.ui \
               nntpconfigui.ui \
               smtpconfigui.ui \
               composemailui.ui \
               settingsdialogui.ui \
               statuswidgetui.ui \
               newmaildirui.ui \
               selectstoreui.ui \
               nntpgroupsui.ui


INCLUDEPATH += $(QPEDIR)/include . $(KDEPIMDIR) $(KDEPIMDIR)/libkdepim  $(KDEPIMDIR)/qtcompat $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/libetpan/include $(KDEPIMDIR)/microkde/kdeui 
LIBS        += -L$(QPEDIR)/lib -lmicromailwrapper -lqpe -lmicrolibetpan  -lmicrokde -lmicroqtcompat -lssl -lcrypto -ljpeg -lmicrokdepim -lpthread
LIBS += $(QTOPIALIB)
#LIBS += -lqtopia 
#next line for Zaurus only
#LIBS +=  -luuid

LIBS += $(GCC3EXTRALIB1)
LIBS += $(GCC3EXTRALIB2)
#
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR=$(QPEDIR)/bin
TARGET       = ompi


