/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003, 2004 by Michael Buesch                            *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   Original implementation of the tray-tree                              *
 *   (c) by Matt Scifo <mscifo@o1.com>                                     *
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
 * $Id: pwmtray.cpp,v 1.2 2004/09/21 19:44:23 ulf69 Exp $
 **************************************************************************/  

#include "pwmtray.h"
#include "pwmexception.h"
#include "pwm.h"
#include "pwmdoc.h"
#include "pwminit.h"
#ifndef PWM_EMBEDDED
#include "configuration.h"
#else
#include "pwmprefs.h"
#endif

#include <klocale.h>


void ActiveTreeItem::execIt()
{
#ifndef PWM_EMBEDDED
	unsigned int entr = static_cast<unsigned int>(entry);
	unsigned int cat = static_cast<unsigned int>(category);
#else
	unsigned int entr = (unsigned int)(entry);
	unsigned int cat = (unsigned int)(category);
#endif
	switch (task) {
		case pwToClipboard: {
			PwMDataItem d;
			doc->getDataChangedLock();
			bool wasLocked = doc->isLocked(cat, entr);
			doc->getEntry(cat, entr, &d, true);
			if (wasLocked)
				doc->lockAt(cat, entr, true);
			doc->putDataChangedLock();
			PwM::copyToClipboard(d.pw.c_str());
			break;
		} case nameToClipboard: {
			PwMDataItem d;
			doc->getEntry(cat, entr, &d);
			PwM::copyToClipboard(d.name.c_str());
			break;
		} case descToClipboard: {
			PwMDataItem d;
			doc->getEntry(cat, entr, &d);
			PwM::copyToClipboard(d.desc.c_str());
			break;
		} case urlToClipboard: {
			PwMDataItem d;
			doc->getEntry(cat, entr, &d);
			PwM::copyToClipboard(d.url.c_str());
			break;
		} case launcherToClipboard: {
			PwMDataItem d;
			doc->getEntry(cat, entr, &d);
			PwM::copyToClipboard(d.launcher.c_str());
			break;
		} case commentToClipboard: {
			PwMDataItem d;
			doc->getEntry(cat, entr, &d);
			PwM::copyToClipboard(d.comment.c_str());
			break;
		} case execLauncher: {
			doc->execLauncher(cat, entr);
			break;
		} case goToURL: {
			doc->goToURL(cat, entr);
			break;
		} case openMainWnd: {
			// search if there is already an open window.
			const QValueList<PwM *> *wl = tray->init->mainWndList();
#ifndef PWM_EMBEDDED
			QValueList<PwM *>::const_iterator i = wl->begin(),
							  end = wl->end();
#else
			QValueList<PwM *>::ConstIterator i = wl->begin(),
							  end = wl->end();
#endif
			PwM *wnd;
			while (i != end) {
				wnd = *i;
				if (wnd->curDoc() == doc) {
					// now bring this window to the foreground.
					if (!wnd->hasFocus()) {
						wnd->hide();
						wnd->showNormal();
						wnd->setFocus();
					}
					return;
				}
				++i;
			}
			// there is no open window, so open a new one.
			tray->init->createMainWnd(QString::null, false, false, doc);
			break;
		} case closeDoc: {
			doc->tryDelete();
			break;
		} case lock: {
			doc->lockAll(true);
			break;
		} case deepLock: {
			doc->deepLock();
			break;
		} case unlock: {
			doc->lockAll(false);
			break;
		} default: {
			BUG();
		}
	}
}



#ifndef PWM_EMBEDDED
PwMTray::PwMTray(PwMInit *_init, QWidget * parent, const char *name)
 : KSystemTray(parent, name)
#else
PwMTray::PwMTray(PwMInit *_init, QWidget * parent, const char *name)
 : QWidget(parent, name)
#endif
{
	init = _init;
	buildMain();
}

PwMTray::~PwMTray()
{
	emit closed(this);
}

//US ENH for embedded tray class 
KPopupMenu* PwMTray::contextMenu()
{
  if (m_ctxMenu == 0)
    {
      m_ctxMenu = new KPopupMenu();
    }

  return m_ctxMenu;
}


void PwMTray::buildMain()
{
	KPopupMenu *ctxMenu = contextMenu();

	ctxMenu->insertSeparator();
	ctxMenu->insertItem(i18n("&New main window..."), this,
			    SLOT(newMainWnd()));
	ctxMenu->insertItem(i18n("&Open file..."), this,
			    SLOT(openDoc()));
	ctxMenu->insertSeparator();
	ctxMenu->insertItem(i18n("&Remove from tray"), this,
			    SLOT(undock()));
}

void PwMTray::insertActiveTreeItem(KPopupMenu *insertIn, const QString &text,
				   ActiveTreeItem::Task task, PwMDoc *doc,
				   int docCategory, int docEntry,
				   QValueList<int> *activeItemsList)
{
	ActiveTreeItem *activeItem;
	int id;

	activeItem = new ActiveTreeItem(text, insertIn->font(), task,
					doc, docCategory, docEntry,
					this);
	id = insertIn->insertItem(activeItem);
	insertIn->connectItem(id, activeItem, SLOT(execIt()));
#ifndef PWM_EMBEDDED
	activeItemsList->push_back(id);
#else
	activeItemsList->append(id);
#endif
}

void PwMTray::rebuildTree(KPopupMenu *popup, PwMDoc *doc,
			  QValueList<int> *activeItems)
{
	PWM_ASSERT(doc);
	PWM_ASSERT(popup);
	activeItems->clear();
	popup->clear();
#ifndef PWM_EMBEDDED
	popup->insertTitle(i18n("Categories:"));
#endif
	vector<string> catList;
	vector<string> entrList;
	doc->getCategoryList(&catList);
	KPopupMenu *newCatMenu;
	KPopupMenu *newEntrMenu;
	int i, size = catList.size();
	int j, entries;
	for (i = 0; i < size; ++i) {
		newCatMenu = new KPopupMenu(popup);
		popup->insertItem(catList[i].c_str(), newCatMenu);
		doc->getEntryList(i, &entrList);

		entries = entrList.size();
		for (j = 0; j < entries; ++j) {
			newEntrMenu = new KPopupMenu(newCatMenu);
			newCatMenu->insertItem(entrList[j].c_str(), newEntrMenu);

			if (doc->isBinEntry(i, j)) {
				/* This is a binary entry. Don't insert the usual
				 * menu items.
				 */
#ifndef PWM_EMBEDDED
				newEntrMenu->insertTitle(i18n("This is a binary entry.\n"
					    "It is not a normal password-entry, as it contains "
					    "binary data, which PwManager can't display here."));
#endif
				continue;
			}

			insertActiveTreeItem(newEntrMenu,
					     i18n("copy password to clipboard"),
					     ActiveTreeItem::pwToClipboard, doc, i, j,
					     activeItems);

			insertActiveTreeItem(newEntrMenu,
					     i18n("copy username to clipboard"),
					     ActiveTreeItem::nameToClipboard, doc, i, j,
					     activeItems);

			insertActiveTreeItem(newEntrMenu,
					     i18n("copy description to clipboard"),
					     ActiveTreeItem::descToClipboard, doc, i, j,
					     activeItems);

			insertActiveTreeItem(newEntrMenu,
					     i18n("copy url to clipboard"),
					     ActiveTreeItem::urlToClipboard, doc, i, j,
					     activeItems);

			insertActiveTreeItem(newEntrMenu,
					     i18n("copy launcher to clipboard"),
					     ActiveTreeItem::launcherToClipboard, doc, i, j,
					     activeItems);

			insertActiveTreeItem(newEntrMenu,
					     i18n("copy comment to clipboard"),
					     ActiveTreeItem::commentToClipboard, doc, i, j,
					     activeItems);

			newEntrMenu->insertSeparator();

			insertActiveTreeItem(newEntrMenu,
					     i18n("Execute \"Launcher\""),
					     ActiveTreeItem::execLauncher, doc, i, j,
					     activeItems);

			insertActiveTreeItem(newEntrMenu,
					     i18n("Go to \"URL\""),
					     ActiveTreeItem::goToURL, doc, i, j,
					     activeItems);
		}
	}
	insertMainWndCtrl(popup, doc);
}

void PwMTray::insertMainWndCtrl(KPopupMenu *menu, PwMDoc *doc)
{
	ActiveTreeItem *activeItem;
	int id;

#ifndef PWM_EMBEDDED
	menu->insertTitle(i18n("Manager:"));
#endif
	activeItem = new ActiveTreeItem(i18n("&Open main manager window ..."),
					menu->font(), ActiveTreeItem::openMainWnd,
					doc, 0, 0, this);
	id = menu->insertItem(activeItem);
	menu->connectItem(id, activeItem, SLOT(execIt()));

	activeItem = new ActiveTreeItem(i18n("&Close this document ..."),
					menu->font(), ActiveTreeItem::closeDoc,
					doc, 0, 0, this);
	id = menu->insertItem(activeItem);
	menu->connectItem(id, activeItem, SLOT(execIt()));

	menu->insertSeparator();

	activeItem = new ActiveTreeItem(i18n("&Lock all entries"),
					menu->font(), ActiveTreeItem::lock,
					doc, 0, 0, this);
	id = menu->insertItem(activeItem);
	menu->connectItem(id, activeItem, SLOT(execIt()));

	activeItem = new ActiveTreeItem(i18n("&Deep-lock all entries"),
					menu->font(), ActiveTreeItem::deepLock,
					doc, 0, 0, this);
	id = menu->insertItem(activeItem);
	menu->connectItem(id, activeItem, SLOT(execIt()));

	activeItem = new ActiveTreeItem(i18n("&Unlock all entries"),
					menu->font(), ActiveTreeItem::unlock,
					doc, 0, 0, this);
	id = menu->insertItem(activeItem);
	menu->connectItem(id, activeItem, SLOT(execIt()));
}

void PwMTray::updateTree(PwMDoc *document)
{
	PWM_ASSERT(document);
	KPopupMenu *treeEntry;
	int treeItemNum = -1;
	int id = findTreeEntry(document, &treeEntry, &treeItemNum);
	if (id == -1) {
		// tree-entry doesn't exist, yet.
		id = insertTreeEntry(document, &treeEntry);
	}
	if (treeItemNum != -1) {
		// delete all *old* active items
		KPopupMenu *ctxMenu = contextMenu();
		QValueList<int> *oldItems = &tree[treeItemNum].activeItems;
#ifndef PWM_EMBEDDED
		QValueList<int>::iterator i = oldItems->begin();
#else
		QValueList<int>::Iterator i = oldItems->begin();
#endif
		while (i != oldItems->end()) {
			ctxMenu->removeItem(*i);
			++i;
		}
		oldItems->clear();
		ctxMenu->changeItem(id, document->getTitle());
	}

	treeItem newTreeItem;
	if (document->isDeepLocked()) {
		treeEntry->clear();
#ifndef PWM_EMBEDDED		
		treeEntry->insertTitle(i18n("Categories:"));
#endif
		QString msg(IS_DEEPLOCKED_SHORTMSG);
		msg += "  ";
		msg += i18n("(Click here to unlock)");
		treeEntry->insertItem(msg, document,
				      SLOT(_deepUnlock()));
		insertMainWndCtrl(treeEntry, document);
	} else {
		rebuildTree(treeEntry, document, &newTreeItem.activeItems);
	}
	newTreeItem.menuId = id;
	newTreeItem.doc = document;
	newTreeItem.menu = treeEntry;
	if (treeItemNum == -1) {
#ifndef PWM_EMBEDDED
		tree.push_back(newTreeItem);
#else
		tree.append(newTreeItem);
#endif
	} else {
		tree[treeItemNum] = newTreeItem;
	}
}

void PwMTray::closeTreeEntry(PwMDoc *document)
{
	KPopupMenu *menu;
	int treeItem;
	int id = findTreeEntry(document, &menu, &treeItem);
	if (id != -1) {
		removeTreeEntry(id, treeItem, &menu);
	}
}

int PwMTray::findTreeEntry(PwMDoc *doc, KPopupMenu **menu, int *treeItem)
{
	PWM_ASSERT(doc);
#ifndef PWM_EMBEDDED
	int i, count = tree.size();
#else
	int i, count = tree.count();
#endif
	for (i = 0; i < count; ++i) {
		if (tree[i].doc == doc) {
			if (menu)
				*menu = tree[i].menu;
			if (treeItem)
				*treeItem = i;
			return tree[i].menuId;
		}
	}
	if (menu)
		*menu = 0;
	if (treeItem)
		*treeItem = -1;
	return -1;
}

int PwMTray::insertTreeEntry(PwMDoc *doc, KPopupMenu **popup)
{
	PWM_ASSERT(doc);
	PWM_ASSERT(popup);
	KPopupMenu *ctxMenu = contextMenu();
	*popup = new KPopupMenu(ctxMenu);
	return ctxMenu->insertItem(doc->getTitle(), *popup, -1, 1);
}

void PwMTray::removeTreeEntry(int menuId, int treeItem, KPopupMenu **menu)
{
	PWM_ASSERT(menu);
	KPopupMenu *ctxMenu = contextMenu();
	ctxMenu->removeItem(menuId);
	delete_and_null(*menu);
#ifndef PWM_EMBEDDED
	tree.erase(tree.at(treeItem));
#else
	tree.remove(tree.at(treeItem));
#endif
}

void PwMTray::mouseReleaseEvent(QMouseEvent *e)
{
	QWidget *curWnd = init->curWidget();
	KPopupMenu *ctxMenu = contextMenu();
	// give focus to init->curWidget()
	if (curWnd && !curWnd->hasFocus()) {
		curWnd->hide();
		curWnd->showNormal();
		curWnd->setFocus();
	}
	// popup the context menu
	ctxMenu->popup(e->globalPos());
	emit clickedIcon(this);
}

void PwMTray::connectDocToTray(PwMDoc *doc)
{
	connect(doc, SIGNAL(dataChanged(PwMDoc *)),
		this, SLOT(updateTree(PwMDoc *)));
	connect(doc, SIGNAL(docClosed(PwMDoc *)),
		this, SLOT(closeTreeEntry(PwMDoc *)));
	// fake a dataChanged signal for this doc to update the tray tree.
	updateTree(doc);
}

void PwMTray::openDoc()
{
	// open the document in a new window.
	PwM *newInstance = init->createMainWnd();
	PwMDoc *newDoc = newInstance->openDoc("");
	if (!newDoc) {
		newInstance->setForceQuit(true);
		delete_and_null(newInstance);
	}
}

void PwMTray::newMainWnd()
{
	init->createMainWnd();
}

void PwMTray::undock()
{
	conf()->confGlobTray(false);
	init->initTray();
	// Attention! "this" is already deleted here!
}

#ifndef PWM_EMBEDDED
#include "pwmtray.moc"
#endif
