TEMPLATE	= app
CONFIG		+= qt warn_off
DESTDIR= ../../bin
TARGET		= pwmpi
include( ../../variables.pri )

INCLUDEPATH += . ../../  ../../libkdepim ../../microkde ../../microkde/kdecore ../../microkde/kdeui ../../microkde/kutils ../libcrypt/crypt ../libcrypt/error ../libcrypt/zlib
DEFINES += PWM_EMBEDDED CONFIG_PWMANAGER_GCRY DESKTOP_VERSION

#enable this setting if you want debugoutput for pwmanager
#DEFINES += CONFIG_DEBUG
LIBS += -L../libcrypt/
LIBS += -L../../bin/
LIBS += -lmicrokde
LIBS += -lmicrokdepim
LIBS += -lzlib
LIBS += -lkpmicrocipher
LIBS += -lkpmicroerror
LIBS += -lkpmicrompi
LIBS += -lstdc++

unix:{
OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix

}
win32:{

DEFINES += _WIN32_
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
QMAKE_LINK += /NODEFAULTLIB:LIBC
QMAKE_CXXFLAGS += /TP /GX /GR /Ehsc
}

#INTERFACES = \
#addentrywnd.ui \
#configwnd.ui \
#findwnd.ui \
#getmasterpwwnd.ui \
#pwgenwnd.ui \
#setmasterpwwnd.ui \
#subtbledit.ui

#INTERFACES = \
#subtbledit.ui \



#HEADERS = \
#configuration_31compat.h \
#configuration.h \
#configwnd.h \
#configwndimpl.h \
#selftest.h
#subtbledit.h \
#subtbleditimpl.h \
#compressbzip2.h \

HEADERS = \
addentrywnd_emb.h \
addentrywndimpl.h \
base64.h \
binentrygen.h \
blowfish.h \
commentbox.h \
compiler.h \
compressgzip.h \
csv.h \
editcategory.h \
findwnd_emb.h \
findwndimpl.h \
genpasswd.h \
getkeycardwnd.h \
getmasterpwwnd_emb.h \
getmasterpwwndimpl.h \
globalstuff.h \
gpasmanfile.h \
htmlgen.h \
htmlparse.h \
ipc.h \
libgcryptif.h \
listobjselectwnd.h \
listviewpwm.h \
printtext.h \
pwgenwnd_emb.h \
pwgenwndimpl.h \
pwmdoc.h \
pwmdocui.h \
pwmexception.h \
pwm.h \
pwminit.h \
pwmprefs.h \
pwmprint.h \
pwmtray.h \
pwmview.h \
pwmviewstyle_0.h \
pwmviewstyle_1.h \
pwmviewstyle.h \
randomizer.h \
rc2.h \
rencatwnd.h \
serializer.h \
setmasterpwwnd_emb.h \
setmasterpwwndimpl.h \
sha1.h \
waitwnd.h \
kcmconfigs/kcmpwmconfig.h \ 
kcmconfigs/pwmconfigwidget.h

#sources that need not be build
#SOURCES = \
#advcommeditimpl.cpp \
#configuration.cpp \
#configwnd.cpp \
#configwndimpl.cpp \
#configuration_31compat.cpp \
#htmlparse.cpp \
#printtext.cpp \
#selftest.cpp \
#pwmprint.cpp \
#spinforsignal.cpp
#subtbledit.cpp \
#subtbleditimpl.cpp \
#compressbzip2.cpp
 

SOURCES = \
addentrywnd_emb.cpp \
addentrywndimpl.cpp \
base64.cpp \
binentrygen.cpp \
blowfish.cpp \
commentbox.cpp \
compressgzip.cpp \
csv.cpp \
editcategory.cpp \
findwnd_emb.cpp \
findwndimpl.cpp \
genpasswd.cpp \
getkeycardwnd.cpp \
getmasterpwwnd_emb.cpp \
getmasterpwwndimpl.cpp \
globalstuff.cpp \
gpasmanfile.cpp \
htmlgen.cpp \
ipc.cpp \
libgcryptif.cpp \
listobjselectwnd.cpp \
listviewpwm.cpp \
main.cpp \
pwgenwnd_emb.cpp \
pwgenwndimpl.cpp \
pwm.cpp \
pwmdoc.cpp \
pwmdocui.cpp \
pwmexception.cpp \
pwminit.cpp \
pwmprefs.cpp \
pwmtray.cpp \
pwmview.cpp \
pwmviewstyle_0.cpp \
pwmviewstyle_1.cpp \
pwmviewstyle.cpp \
randomizer.cpp \
rc2.cpp \
rencatwnd.cpp \
serializer.cpp \
setmasterpwwnd_emb.cpp \
setmasterpwwndimpl.cpp \
sha1.cpp \
waitwnd.cpp \
kcmconfigs/kcmpwmconfig.cpp \ 
kcmconfigs/pwmconfigwidget.cpp



