#ifndef QT4COMPAT_H
#define QT4COMPAT_H
#define QT3_SUPPORT
#if 0


#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else


#endif


#endif
#include "qcstring.h"

#include <qdesktopwidget.h>

#define utf8 toUtf8

#define IO_ReadOnly QIODevice::ReadOnly

#include <q3groupbox.h>
#define QGroupBox Q3GroupBox

#include <q3progressbar.h>
#define QProgressBar Q3ProgressBar

#include <q3buttongroup.h>
#include <q3boxlayout.h>
#define QHBoxLayout Q3HBoxLayout
#define QBoxLayout Q3BoxLayout
#define QVBoxLayout Q3VBoxLayout
#define QButtonGroup Q3ButtonGroup


#define QCheckListItem Q3CheckListItem


#include <q3listview.h>
#define QListViewItem Q3ListViewItem
#define QListViewItemIterator Q3ListViewItemIterator
#define QListView Q3ListView

#include <q3dragobject.h>
#define QDragObject Q3DragObject


#define QIconSet QIcon



#endif
