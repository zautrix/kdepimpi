TEMPLATE	= lib
CONFIG += qt warn_on
INCLUDEPATH     += . .. $(KDEPIMDIR)/qtcompat $(KDEPIMDIR)/kabc kdecore kdeui kio/kfile kio/kio $(QPEDIR)/include

DEPENDPATH      += $(QPEDIR)/include
LIBS            += -lmicroqtcompat -L$(QPEDIR)/lib

DEFINES += KDE_QT_ONLY

TARGET		= microkde
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR=$(QPEDIR)/lib


INTERFACES = \

HEADERS = \
qlayoutengine_p.h fncolordialog.h\
KDGanttMinimizeSplitter.h \
 kapplication.h \
 kaudioplayer.h \
 kcalendarsystem.h \
 kcalendarsystemgregorian.h \
 kcolorbutton.h \
 kcolordialog.h \
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
 ofileselector_p.h \
 ofontselector.h \
 kdeui/kguiitem.h \
 kdeui/kaction.h \
 kdeui/kpopupmenu.h \
 kdeui/kactionclasses.h \
 kdeui/kactioncollection.h \
 kdeui/kcmodule.h \
 kdeui/kstdaction.h \
 kdeui/kbuttonbox.h \
 kdeui/klistbox.h \
 kdeui/klistview.h \
 kdeui/kjanuswidget.h \
 kdeui/kseparator.h \
 kdeui/kmainwindow.h \
 kdeui/knuminput.h \
 kdeui/knumvalidator.h \
 kdeui/ksqueezedtextlabel.h \
 kdeui/ktoolbar.h \
 kdeui/ktoolbarbutton.h \
 kdeui/ktoolbarhandler.h \
 kdeui/kxmlguiclient.h \
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
 kdecore/klibloader.h \
 kdecore/kcatalogue.h \
 kdecore/kprefs.h \
 kdecore/ksharedptr.h \
 kdecore/kshell.h \
 kdecore/kstandarddirs.h \
 kdecore/kstringhandler.h \
 kdecore/kshortcut.h \
 kutils/kcmultidialog.h \
 kidmanager.h    

 
 

SOURCES = \
KDGanttMinimizeSplitter.cpp fncolordialog.cpp \
 kapplication.cpp \
 kcalendarsystem.cpp \
 kcalendarsystemgregorian.cpp \
 kcolorbutton.cpp \
 kcolordialog.cpp \
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
 kprocess.cpp \
 krun.cpp \
 ksystemtray.cpp \
 ktempfile.cpp \
 kurl.cpp \
 ktextedit.cpp \
 ofileselector_p.cpp \
 ofontselector.cpp \
 kdecore/kcatalogue.cpp \
 kdecore/klibloader.cpp \
 kdecore/klocale.cpp \
 kdecore/kmdcodec.cpp \
 kdecore/kprefs.cpp \
 kdecore/kshell.cpp \
 kdecore/kstandarddirs.cpp \
 kdecore/kstringhandler.cpp \
 kdeui/kaction.cpp \
 kdeui/kpopupmenu.cpp \
 kdeui/kactionclasses.cpp \
 kdeui/kactioncollection.cpp \
 kdeui/kbuttonbox.cpp \
 kdeui/kcmodule.cpp \
 kdeui/kguiitem.cpp \
 kdeui/kjanuswidget.cpp \
 kdeui/klistbox.cpp \
 kdeui/klistview.cpp \
 kdeui/kmainwindow.cpp \
 kdeui/knuminput.cpp \
 kdeui/knumvalidator.cpp \
 kdeui/kseparator.cpp \
 kdeui/kstdaction.cpp \
 kdeui/ksqueezedtextlabel.cpp \
 kdeui/ktoolbar.cpp \
 kdeui/ktoolbarbutton.cpp \
 kdeui/ktoolbarhandler.cpp \
 kdeui/kxmlguiclient.cpp \
 kio/kfile/kurlrequester.cpp \
 kio/kio/kdirwatch.cpp \
 kresources/configpage.cpp \
 kresources/configdialog.cpp \
 kresources/configwidget.cpp \
 kresources/factory.cpp \
 kresources/kcmkresources.cpp \
 kresources/managerimpl.cpp \
 kresources/resource.cpp \
 kresources/selectdialog.cpp \
 kutils/kcmultidialog.cpp  \
 kidmanager.cpp   
 
