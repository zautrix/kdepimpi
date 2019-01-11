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
               ./qpe/qdialog.h \
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


INCLUDEPATH += ./qpe . .. ../libkdepim   ../microkde ../microkde/kdecore ../libetpan/include ../microkde/kdeui
LIBS        +=  -L../bin -lmicromailwrapper  -lmicrolibetpan  -lmicrokde  -lssl -lcrypto  -lmicrokdepim -lmicrokabc -lpthread
#for raspi font rendering
#LIBS += /usr/lib/x86_64-linux-gnu/libfreetype.so
DESTDIR= ../bin
TARGET       = ompi

DEFINES +=  DESKTOP_VERSION
unix : {
OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix

LIBS += /home/my_libs/libfreetype.so /home/my_libs/libXft.so
#LIBS += /usr/lib/arm-linux-gnueabihf/libfreetype.so /usr/lib/arm-linux-gnueabihf/libXft.so.2.3.2
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

