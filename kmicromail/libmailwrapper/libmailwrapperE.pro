TEMPLATE        = lib
CONFIG        += qt warn_on

HEADERS         = mailwrapper.h \
               imapwrapper.h \
               mailtypes.h \
               pop3wrapper.h \
               abstractmail.h  \
               smtpwrapper.h \
               genericwrapper.h \
               mboxwrapper.h \
               settings.h \
               logindialog.h \
               sendmailprogress.h \
               statusmail.h \
               mhwrapper.h \
               nntpwrapper.h \
               generatemail.h \
               storemail.h

SOURCES         = imapwrapper.cpp \
               mailwrapper.cpp \
               mailtypes.cpp \
               pop3wrapper.cpp \
               abstractmail.cpp \
               smtpwrapper.cpp \
               genericwrapper.cpp \
               mboxwrapper.cpp \
               settings.cpp \
               logindialog.cpp \
               sendmailprogress.cpp \
               statusmail.cpp \
               mhwrapper.cpp \
              nntpwrapper.cpp \
              generatemail.cpp \
              storemail.cpp

INTERFACES     = logindialogui.ui \
              sendmailprogressui.ui

INCLUDEPATH +=  ../../microkde ../../microkde/kdecore $(KDEPIMDIR)/libetpan/include $(QPEDIR)/include
LIBS        +=   -lssl -lcrypto

#-lqpe -letpan

DESTDIR      = $(QPEDIR)/lib
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
TARGET       = micromailwrapper
