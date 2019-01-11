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
               storemail.h \
               ../qpe/global.h

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
              storemail.cpp \
               ../qpe/qdialog_hacked.cpp \
               ../qpe/global.cpp

INTERFACES     = logindialogui.ui \
              sendmailprogressui.ui

INCLUDEPATH +=  ../qpe .. ../../microkde ../../microkde/kdecore ../../libetpan/include 
LIBS        +=   -lssl -lcrypto

#-lqpe -letpan

DESTDIR      = ../../bin
TARGET       = micromailwrapper

DEFINES +=  DESKTOP_VERSION
unix : {
OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix
}
win32: {
DEFINES += _WIN32_  
LIBS += mfc71u.lib
QMAKE_LINK += /NODEFAULTLIB:LIBC
#QMAKE_LINK += /NODEFAULTLIB:MSVCRT
#QMAKE_LINK += /NODEFAULTLIB:uafxcw.lib
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
}
