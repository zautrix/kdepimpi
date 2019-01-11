/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003, 2004 by Michael Buesch                            *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * copyright (C) 2004 by Ulf Schenk
 * This file is originaly based on version 1.0.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: pwmtray.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $
 **************************************************************************/  

#ifndef __PWMTRAY_H
#define __PWMTRAY_H

#ifndef PWM_EMBEDDED
#include <ksystemtray.h>
#endif

#include <kpopupmenu.h>

#include <qpainter.h>
#include <qvaluelist.h>

//#include "pwmexception.h"

class PwMInit;
class PwMDoc;
class PwMTray;

/* wrapper to workaround MOC problems */
class __ActiveTreeItem : public QCustomMenuItem
		       , public QObject
{
public:
	__ActiveTreeItem() {}
};

/** implements the "active" part of the tree */
class ActiveTreeItem : public __ActiveTreeItem
{
	Q_OBJECT

public:
	enum Task
	{
		pwToClipboard,
		nameToClipboard,
		descToClipboard,
		urlToClipboard,
		launcherToClipboard,
		commentToClipboard,
		execLauncher,
		goToURL,
		openMainWnd,
		closeDoc,
		lock,
		deepLock,
		unlock
	};

public:
	ActiveTreeItem(const QString &_text, const QFont &_font,
		       Task _task, PwMDoc * _doc,
		       int _category, int _entry,
		       PwMTray *_tray)
	 : text (_text)
	 , font (_font)
	 , task (_task)
	 , doc (_doc)
	 , category (_category)
	 , entry (_entry)
	 , tray (_tray)
	 	{ }

/*	~ActiveTreeItem()
		{ cout << "deleted \"" << text << "\"" << endl; } */

	void paint(QPainter *p, const QColorGroup & /*cg*/, bool /*act*/,
		   bool /*enabled*/, int x, int y, int w, int h)
		{
			p->setFont(font);
			p->drawText(x, y, w, h, AlignLeft | AlignVCenter |
				    DontClip | ShowPrefix, text);
		}

	QSize sizeHint()
		{
			return QFontMetrics(font).size(AlignLeft | AlignVCenter |
						       ShowPrefix | DontClip,  text);
		}

	bool fullSpan() const
			{ return false; }

	bool isSeparator() const
			{ return false; }

	void setFont(const QFont &f)
			{ font = f; }

public slots:
	/** this is the executive part, that is triggered, when the user clicks it */
	void execIt();

protected:
	QString text;
	QFont font;
	Task task;
	PwMDoc *doc;
	int category;
	int entry;
	PwMTray *tray;
};

/** tray icon implementation */
#ifndef PWM_EMBEDDED
//MOC_SKIP_BEGIN
class PwMTray : public KSystemTray
{
	Q_OBJECT

	friend class ActiveTreeItem;

	struct treeItem
	{
		int menuId;
		PwMDoc *doc;
		KPopupMenu *menu;
		QValueList<int> activeItems; // ids of all active items
	};

public: 
	PwMTray(PwMInit *_init, QWidget* parent = 0, const char* name = 0);
	~PwMTray();

	/** connect all signals for this document to the tray */
	void connectDocToTray(PwMDoc *doc);

public slots:
	/** for update-notification from all open documents */
	void updateTree(PwMDoc *document);
	/** when a document has been closed, call this func
	  * to delete its tray-entry
	  */
	void closeTreeEntry(PwMDoc *document);

protected slots:
	/** open a new document */
	void openDoc();
	/** open a new mainwnd */
	void newMainWnd();
	/** undock this tray icon */
	void undock();

signals:
	/** this icon got deleted */
	void closed(PwMTray *);
	/** the user clicked on the tray icon */
	void clickedIcon(PwMTray *);

protected:
	/** holds a list of all open files, its documents and
	  * its menuIDs
	  */
	QValueList<struct treeItem> tree;
	/** pointer to init */
	PwMInit *init;

protected:
	/** search if an entry with for the given document
	  * exists and resturn its ID and a pointer to the main
	  * KPopupMenu. If no item is found,
	  * it returns -1 as ID and a NULL-pointer.
	  */
	int findTreeEntry(PwMDoc *doc, KPopupMenu **menu, int *treeItem);
	/** build the main menu-items */
	void buildMain();
	/** rebuilds the tree for the given KPopupMenu entry */
	void rebuildTree(KPopupMenu *popup, PwMDoc *doc,
			 QValueList<int> *activeItems);
	/** insert a new tree-entry for the given doc and returns the ID.*/
	int insertTreeEntry(PwMDoc *doc, KPopupMenu **popup);
	/** removes a tree entry */
	void removeTreeEntry(int menuId, int treeItem, KPopupMenu **menu);
	/** inserts a new active-tree-item into the given tree entry */
	void insertActiveTreeItem(KPopupMenu *insertIn, const QString &text,
				  ActiveTreeItem::Task task, PwMDoc *doc,
				  int docCategory, int docEntry,
				  QValueList<int> *activeItemsList);
	/** mouse event on icon */
	void mouseReleaseEvent(QMouseEvent *e);
	/** inserts the items for the main window control
	  * into the popup menu.
	  */
	void insertMainWndCtrl(KPopupMenu *menu, PwMDoc *doc);
};
//MOC_SKIP_END
#else
class PwMTray : public QWidget 
{
	Q_OBJECT

	friend class ActiveTreeItem;

	struct treeItem
	{
		int menuId;
		PwMDoc *doc;
		KPopupMenu *menu;
		QValueList<int> activeItems; // ids of all active items
	};

public: 
	PwMTray(PwMInit *_init, QWidget* parent = 0, const char* name = 0);
	~PwMTray();

	//US ENH needed for embedde version.
	KPopupMenu* contextMenu();
	KPopupMenu* m_ctxMenu;

	/** connect all signals for this document to the tray */
	void connectDocToTray(PwMDoc *doc);

public slots:
	/** for update-notification from all open documents */
	void updateTree(PwMDoc *document);
	/** when a document has been closed, call this func
	  * to delete its tray-entry
	  */
	void closeTreeEntry(PwMDoc *document);

protected slots:
	/** open a new document */
	void openDoc();
	/** open a new mainwnd */
	void newMainWnd();
	/** undock this tray icon */
	void undock();

signals:
	/** this icon got deleted */
	void closed(PwMTray *);
	/** the user clicked on the tray icon */
	void clickedIcon(PwMTray *);

protected:
	/** holds a list of all open files, its documents and
	  * its menuIDs
	  */
	QValueList<struct treeItem> tree;
	/** pointer to init */
	PwMInit *init;

protected:
	/** search if an entry with for the given document
	  * exists and resturn its ID and a pointer to the main
	  * KPopupMenu. If no item is found,
	  * it returns -1 as ID and a NULL-pointer.
	  */
	int findTreeEntry(PwMDoc *doc, KPopupMenu **menu, int *treeItem);
	/** build the main menu-items */
	void buildMain();
	/** rebuilds the tree for the given KPopupMenu entry */
	void rebuildTree(KPopupMenu *popup, PwMDoc *doc,
			 QValueList<int> *activeItems);
	/** insert a new tree-entry for the given doc and returns the ID.*/
	int insertTreeEntry(PwMDoc *doc, KPopupMenu **popup);
	/** removes a tree entry */
	void removeTreeEntry(int menuId, int treeItem, KPopupMenu **menu);
	/** inserts a new active-tree-item into the given tree entry */
	void insertActiveTreeItem(KPopupMenu *insertIn, const QString &text,
				  ActiveTreeItem::Task task, PwMDoc *doc,
				  int docCategory, int docEntry,
				  QValueList<int> *activeItemsList);
	/** mouse event on icon */
	void mouseReleaseEvent(QMouseEvent *e);
	/** inserts the items for the main window control
	  * into the popup menu.
	  */
	void insertMainWndCtrl(KPopupMenu *menu, PwMDoc *doc);
};
#endif

#endif
