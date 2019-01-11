TEMPLATE	= lib
CONFIG		+= qt warn_on 
include( ../variables.pri )
#INCLUDEPATH     += $(QTDIR)/include .
#DEPENDPATH      += $(QTDIR)/include
INCLUDEPATH     += .  ../  ../kabc ./kdecore ./kdeui ./kio/kfile ./kio/kio
#LIBS            += -lqtcompat

TARGET		= microkde
DESTDIR= ../bin
DEFINES += DESKTOP_VERSION KDE_QT_ONLY
unix : {
staticlib: {
INCLUDEPATH += ../kabc/plugins
DEFINES += STATIC_RESOURCES
} 


OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix
}
win32: {
DEFINES += _WIN32_
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
}



HEADERS = \
qlayoutengine_p.h \
KDGanttMinimizeSplitter.h \
 kapplication.h \
 kaudioplayer.h \
 kcalendarsystem.h \
 kcalendarsystemgregorian.h \
 kcolorbutton.h \
 kcombobox.h \
 kconfig.h \
 kdatetbl.h \
 kdebug.h \
 kdialog.h \
 kdialogbase.h \
 keditlistbox.h \
 kemailsettings.h \
 kfiledialog.h \
 kfontdialog.h \
 kglobal.h \
 kglobalsettings.h \
 kiconloader.h \
 klineedit.h \
 klineeditdlg.h \
 kmessagebox.h \
 knotifyclient.h \
 kprinter.h \
 kprocess.h \
 krestrictedline.h \
 krun.h \
 ksimpleconfig.h \
 kstaticdeleter.h \
 ksystemtray.h \
 ktempfile.h \
 ktextedit.h \
 kunload.h \
 kurl.h \
 kdeui/kguiitem.h \
 kdeui/kcmodule.h \
 kdeui/kbuttonbox.h \
 kdeui/klistbox.h \
 kdeui/klistview.h \
 kdeui/kjanuswidget.h \
 kdeui/kseparator.h \
 kdeui/knuminput.h \
 kdeui/knumvalidator.h \
 kdeui/ksqueezedtextlabel.h \
 kio/job.h \
 kio/kio/kdirwatch.h \
 kio/kio/kdirwatch_p.h \
 kio/kfile/kurlrequester.h \
 kresources/resource.h \
 kresources/factory.h \
 kresources/managerimpl.h \
 kresources/manager.h \
 kresources/selectdialog.h \
 kresources/configpage.h \
 kresources/configwidget.h \ 
 kresources/configdialog.h \
 kresources/kcmkresources.h \
 kdecore/kmdcodec.h \
 kdecore/kconfigbase.h \
 kdecore/klocale.h \
 kdecore/kcatalogue.h \
 kdecore/ksharedptr.h \
 kdecore/kshell.h \
 kdecore/kstandarddirs.h \
 kdecore/kstringhandler.h \
 kdecore/kshortcut.h \
 kutils/kcmultidialog.h \
 kdeui/kxmlguiclient.h \
 kdeui/kstdaction.h \
 kdeui/kmainwindow.h \
 kdeui/ktoolbar.h \
 kdeui/ktoolbarbutton.h \
 kdeui/ktoolbarhandler.h \
 kdeui/kaction.h \
 kdeui/kpopupmenu.h \
 kdeui/kactionclasses.h \
 kdeui/kactioncollection.h \
 kdecore/kprefs.h \
 kdecore/klibloader.h \
 kidmanager.h    
 

# kdecore/klibloader.h \


SOURCES = \
KDGanttMinimizeSplitter.cpp \
 kapplication.cpp \
 kcalendarsystem.cpp \
 kcalendarsystemgregorian.cpp \
 kcolorbutton.cpp \
 kconfig.cpp \
 kdatetbl.cpp \
 kdialog.cpp \
 kdialogbase.cpp \
 keditlistbox.cpp \
 kemailsettings.cpp \
 kfontdialog.cpp \
 kfiledialog.cpp \
 kglobal.cpp \
 kglobalsettings.cpp \
 kiconloader.cpp \
 kmessagebox.cpp \
 ktextedit.cpp \
 kprocess.cpp \
 krun.cpp \
 ksystemtray.cpp \
 ktempfile.cpp \
 kurl.cpp \
 kdecore/kcatalogue.cpp \
 kdecore/klocale.cpp \
 kdecore/kmdcodec.cpp \
 kdecore/kshell.cpp \
 kdecore/kstandarddirs.cpp \
 kdecore/kstringhandler.cpp \
 kdeui/kbuttonbox.cpp \
 kdeui/kcmodule.cpp \
 kdeui/kguiitem.cpp \
 kdeui/kjanuswidget.cpp \
 kdeui/klistbox.cpp \
 kdeui/klistview.cpp \
 kdeui/knuminput.cpp \
 kdeui/knumvalidator.cpp \
 kdeui/kseparator.cpp \
 kdeui/ksqueezedtextlabel.cpp \
 kio/kio/kdirwatch.cpp \
 kio/kfile/kurlrequester.cpp \
 kresources/configpage.cpp \
 kresources/configdialog.cpp \
 kresources/configwidget.cpp \
 kresources/factory.cpp \
 kresources/kcmkresources.cpp \
 kresources/managerimpl.cpp \
 kresources/resource.cpp \
 kresources/selectdialog.cpp \
 kutils/kcmultidialog.cpp \
 kdeui/kaction.cpp \
 kdeui/kpopupmenu.cpp \
 kdeui/kactionclasses.cpp \
 kdeui/kactioncollection.cpp \
 kdeui/kmainwindow.cpp \
 kdeui/ktoolbar.cpp \
 kdeui/ktoolbarbutton.cpp \
 kdeui/ktoolbarhandler.cpp \
 kdeui/kstdaction.cpp \
 kdeui/kxmlguiclient.cpp \
 kdecore/kprefs.cpp \
 kdecore/klibloader.cpp \
 kidmanager.cpp 
