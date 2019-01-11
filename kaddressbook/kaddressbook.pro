TEMPLATE	= app
CONFIG		= qt warn_on 
TARGET		= kapi
DESTDIR= ../bin

include( ../variables.pri )


INCLUDEPATH += . ./details  ./features  ./xxport ../libkdepim ../microkde ../microkde/kdecore ../microkde/kutils ../microkde/kio/kfile ../microkde/kio/kio ../microkde/kdeui ../microkde/kresources ../kabc  ../ interfaces 
DEFINES += KAB_EMBEDDED KAB_NOSPLITTER DESKTOP_VERSION

unix : {

staticlib: {
TARGET		= kapi_linux
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
LIBS += ../bin/libmicrokde.so
LIBS += ../bin/libmicrokabc.so
LIBS += ../bin/libmicrokcal.so
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
QMAKE_LINK += /NODEFAULTLIB:LIBC
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
#olimport section 
importol {
debug: {
mfc71u:LIBS += mfc71ud.lib
}
release: {
mfc71u:LIBS += mfc71u.lib
}
DEFINES +=  _OL_IMPORT_
HEADERS = ../outport/msoutl9.h \
 kaimportoldialog.h 
SOURCES = ../outport/msoutl9.cpp \
 kaimportoldialog.cpp 
#olimport section end
TARGET		= kapi_xp
} else {
TARGET		= kapi_me
}
}

INTERFACES = \
# filteredit_base.ui \
# kofilterview_base.ui \

HEADERS += \
features/mergewidget.h \
features/distributionlistwidget.h \
kcmconfigs/addresseewidget.h \
kcmconfigs/extensionconfigdialog.h \
kcmconfigs/kcmkabconfig.h \
kcmconfigs/kabconfigwidget.h \
addresseeeditordialog.h \
addresseeeditorwidget.h \
addresseditwidget.h \
addresseeconfig.h \
addresseeutil.h \
emaileditwidget.h \
filtereditdialog.h \
kaddressbookmain.h \
kabprefs.h \
kabcore.h \
viewmanager.h \
extensionmanager.h \
extensionwidget.h \
kaddressbookview.h \
geowidget.h \
imagewidget.h \
incsearchwidget.h \
jumpbuttonbar.h \
phoneeditwidget.h \
secrecywidget.h \
keywidget.h \
nameeditdialog.h \
filter.h \
addviewdialog.h \
configurewidget.h \
viewconfigurewidget.h \
viewconfigurefieldspage.h \
viewconfigurefilterpage.h \
typecombo.h \
undo.h \
undocmds.h \
xxportmanager.h \
xxportobject.h \
xxportselectdialog.h \
details/detailsviewcontainer.h \
details/look_basic.h \
details/look_html.h \
views/kaddressbookiconview.h \
views/kaddressbooktableview.h \
views/kaddressbookcardview.h \
views/configuretableviewdialog.h \
views/configurecardviewdialog.h \
views/cardview.h \
views/colorlistbox.h \
views/contactlistview.h \
xxport/vcard_xxport.h \
xxport/kde2_xxport.h \
xxport/csv_xxport.h \
xxport/csvimportdialog.h \
xxport/opie_xxport.h \
xxport/qtopia_xxport.h \
xxport/sharpdtm_xxport.h \
#details/look_details.h \
#mainwindoiw.h \
# alarmclient.h \
# calendarview.h \
# customlistviewitem.h \
# datenavigator.h

SOURCES += \
addresseeeditordialog.cpp \
addresseeeditorwidget.cpp \
addresseditwidget.cpp \
addresseeconfig.cpp \
addresseeutil.cpp \
extensionmanager.cpp \
features/mergewidget.cpp \
features/distributionlistwidget.cpp \
kcmconfigs/addresseewidget.cpp \
kcmconfigs/extensionconfigdialog.cpp \
kcmconfigs/kcmkabconfig.cpp \
kcmconfigs/kabconfigwidget.cpp \
emaileditwidget.cpp \
filtereditdialog.cpp \
mainembedded.cpp \
kaddressbookmain.cpp \
kabcore.cpp \
kabprefs.cpp \
viewmanager.cpp \
kaddressbookview.cpp \
extensionwidget.cpp \
geowidget.cpp \
imagewidget.cpp \
incsearchwidget.cpp \
jumpbuttonbar.cpp \
phoneeditwidget.cpp \
secrecywidget.cpp \
keywidget.cpp \
nameeditdialog.cpp \
filter.cpp \
addviewdialog.cpp \
configurewidget.cpp \
viewconfigurewidget.cpp \
viewconfigurefieldspage.cpp \
viewconfigurefilterpage.cpp \
undo.cpp \
undocmds.cpp \
xxportmanager.cpp \
xxportobject.cpp \
xxportselectdialog.cpp \
details/detailsviewcontainer.cpp \
details/look_basic.cpp \
details/look_html.cpp \
views/kaddressbookiconview.cpp \
views/kaddressbooktableview.cpp \
views/kaddressbookcardview.cpp \
views/configuretableviewdialog.cpp \
views/configurecardviewdialog.cpp \
views/cardview.cpp \
views/contactlistview.cpp \
views/colorlistbox.cpp \
xxport/vcard_xxport.cpp \
xxport/kde2_xxport.cpp \
xxport/csv_xxport.cpp \
xxport/csvimportdialog.cpp \
xxport/opie_xxport.cpp \
xxport/qtopia_xxport.cpp \
xxport/sharpdtm_xxport.cpp \
#details/look_details.cpp \
#mainwindow.cpp \
# calendarview.cpp \
# timespanview.cpp

