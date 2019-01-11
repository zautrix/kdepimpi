TEMPLATE	= lib
CONFIG		+= qt warn_on
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
TARGET		= microqtcompat
DESTDIR=$(QPEDIR)/lib


INTERFACES = \

HEADERS = \
 qcombotableitem.h \
 qgridview.h \
 qmemarray.h \
 qptrlist.h \
 qksplitter.h \
 qptrvector.h \
 qinputdialog.h \
 qtooltipcompat.h \
 xml/qdom.h \
 xml/qxml.h

# qsplitter.h 




SOURCES = \
 qinputdialog.cpp \
 qcombotableitem.cpp \
 qgridview.cpp \
 qtooltipcompat.cpp \
 xml/qdom.cpp \
 xml/qxml.cpp

