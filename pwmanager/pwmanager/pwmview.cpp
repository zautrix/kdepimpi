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
 * $Id: pwmview.cpp,v 1.12 2005/03/07 16:51:26 zautrix Exp $
 **************************************************************************/  

#include "pwmview.h"
#include "pwmexception.h"
#include "globalstuff.h"
#include "pwm.h"
#include "rencatwnd.h"
#ifndef PWM_EMBEDDED
#include "configuration.h"
#else
#include "pwmprefs.h"
#endif
#include "commentbox.h"

#include <kmessagebox.h>
#include <klocale.h>

#include <qlineedit.h>
#include <qpoint.h>
#include <qapplication.h>
#include <qlayout.h>

//US ENH: wouldn't it be a good idea if we could use this consts everywhere else.
//US ENH: for examle in listviewpwm.cpp
//US ENH: Because of that I transfer them into the headerfile.
/*
#define COLUMN_DESC		0
#define COLUMN_NAME		1
#define COLUMN_PW		2
#define COLUMN_URL		3
#define COLUMN_LAUNCHER		4
*/

PwMView::PwMView(PwM *_mainClass,
		 QWidget *parent, PwMDoc *_doc,
		 const char *name)
 : PwMViewStyle(parent, name)
{
	PWM_ASSERT(_mainClass);
	PWM_ASSERT(parent);
	PWM_ASSERT(_doc);
	setView(this);
	doc = _doc;
	doc->setListViewPointer(this);
	mainClass = _mainClass;
	resize(_mainClass->size());
	initStyle(conf()->confWndMainViewStyle());
	initCtxMenu();
	doc->setCurrentView(this);
	connect(doc, SIGNAL(dataChanged(PwMDoc *)), this, SLOT(updateView()));
	connect(this, SIGNAL(editPW()), mainClass, SLOT(editPwd_slot()));
	connect(this, SIGNAL(insertPW()), mainClass, SLOT(addPwd_slot()));
	connect(this, SIGNAL(deletePW()), mainClass, SLOT(deletePwd_slot()));
}

PwMView::~PwMView()
{
}

void PwMView::initCtxMenu()
{
	ctxMenu = new QPopupMenu(this);
	ctxMenu->insertItem(i18n("&Add password"), mainClass, SLOT(addPwd_slot()));
	ctxMenu->insertSeparator();
	ctxMenu->insertItem(i18n("&Edit"), mainClass, SLOT(editPwd_slot()));
	ctxMenu->insertItem(i18n("&Delete"), mainClass, SLOT(deletePwd_slot()));
	ctxMenu->insertSeparator();
	ctxMenu->insertItem(i18n("copy password to clipboard"),
			    this, SLOT(copyPwToClip()));
	ctxMenu->insertItem(i18n("copy username to clipboard"),
			    this, SLOT(copyNameToClip()));
	ctxMenu->insertItem(i18n("copy description to clipboard"),
			    this, SLOT(copyDescToClip()));
	ctxMenu->insertItem(i18n("copy url to clipboard"),
			    this, SLOT(copyUrlToClip()));
	ctxMenu->insertItem(i18n("copy launcher to clipboard"),
			    this, SLOT(copyLauncherToClip()));
	ctxMenu->insertItem(i18n("copy comment to clipboard"),
			    this, SLOT(copyCommentToClip()));
	ctxMenu->insertSeparator();
	ctxMenu->insertItem(i18n("Execute \"Launcher\""), mainClass,
			    SLOT(execLauncher_slot()));
	ctxMenu->insertItem(i18n("Go to \"URL\""), mainClass,
			    SLOT(goToURL_slot()));
}

void PwMView::resizeEvent(QResizeEvent *)
{
	resizeView(size());
}

void PwMView::refreshCommentTextEdit(QListViewItem *curItem)
{
	PWM_ASSERT(commentBox);
	if (!curItem)
		return;
	string comment;
	PwMerror ret;
	ret = document()->getCommentByLvp_long(getCurrentCategory(),
					  lv->childCount() - lv->itemIndex(curItem) - 1,
					  &comment);
	if (ret == e_binEntry) {
		commentBox->setContent(i18n("This is a binary entry.\n"
					    "It is not a normal password-entry, as it contains "
					    "binary data, which PwManager can't display here."));
	} else if (ret == e_normalEntry) {
		commentBox->setContent(comment.c_str());
	} else {
		BUG();
		return;
	}
	lv->ensureItemVisible(curItem);
}

void PwMView::keyReleaseEvent(QKeyEvent * /*e*/)
{
	refreshCommentTextEdit(lv->currentItem());
}

bool PwMView::getCurEntryIndex(unsigned int *index)
{
	QListViewItem *current = lv->currentItem();
	if (!current)
		return false;
	return getDocEntryIndex(index, current);
}

bool PwMView::getDocEntryIndex(unsigned int *index,
			       const QListViewItem *item)
{
	vector<unsigned int> foundPositions;
	PwMDataItem curItem;
	curItem.desc = item->text(COLUMN_DESC).latin1();
	curItem.name = item->text(COLUMN_NAME).latin1();
	document()->getCommentByLvp(getCurrentCategory(),
				    lv->childCount() - lv->itemIndex(item) - 1,
				    &curItem.comment);
	curItem.url = item->text(COLUMN_URL).latin1();
	curItem.launcher = item->text(COLUMN_LAUNCHER).latin1();
	document()->findEntry(getCurrentCategory(), curItem, SEARCH_IN_DESC |
			      SEARCH_IN_NAME | SEARCH_IN_COMMENT | SEARCH_IN_URL |
			      SEARCH_IN_LAUNCHER,
			      &foundPositions, true);
	if (foundPositions.size()) {
		*index = foundPositions[0];
		return true;
	}

	return false;
}

void PwMView::handleToggle(QListViewItem *item)
{
	PWM_ASSERT(doc);
	if (!item)
		return;
	QCheckListItem *clItem = (QCheckListItem *)item;
	QString curCat(getCurrentCategory());

	// find document position of this entry.
	unsigned int curEntryDocIndex;
	if (!getDocEntryIndex(&curEntryDocIndex, item))
		return;

	// hack to refresh the comment, if only one item is present
	if (lv->childCount() == 1)
		refreshCommentTextEdit(lv->currentItem());

	if (doc->isLocked(curCat, curEntryDocIndex) != clItem->isOn())
		return;		// this is just a click somewhere on the entry
	if (doc->isDeepLocked()) {
		PwMerror ret;
		ret = doc->deepLock(false);
		if (ret != e_success)
			clItem->setOn(false);
		return;
	}
	doc->lockAt(curCat, curEntryDocIndex, !clItem->isOn());
}

void PwMView::handleRightClick(QListViewItem *item, const QPoint &point, int)
{
	if (!item)
		return;
	ctxMenu->move(point);
	/* don't use ctxMenu->exec() here, as it generates race conditions
	 * with the card interface code. Believe it or not. :)
	 */
	ctxMenu->show();
}

void PwMView::updateCategories()
{
    //qDebug("PwMView::updateCategories() ");
	QString oldSel(getCurrentCategory());
	delAllCategories();
	QStringList catList;
	document()->getCategoryList(&catList);
	catList.sort();
#ifndef PWM_EMBEDDED
	QStringList::iterator i = catList.begin(),
			      end = catList.end();
#else
	QStringList::Iterator i = catList.begin(),
			      end = catList.end();
#endif
	while (i != end) {
		addCategory(*i);
		++i;
	}
	selectCategory(oldSel);
}

void PwMView::shiftToView()
{
	int cX = lv->contentsX();
	int cY = lv->contentsY();
	commentBox->clear();

	unsigned int catDocIndex;
	if (unlikely(
	    !(document()->findCategory(getCurrentCategory(),
	    			       &catDocIndex)))) {
		BUG();
	}

	// ensure all listViewPos are set
	doc->ensureLvp();

	// clear all tmp-data vectors
	unsigned int i, entries = doc->numEntries(catDocIndex);
	if (entries) {
		mainClass->setVirgin(false);
	}
	vector<PwMDataItem> tmpSorted;
	PwMDataItem currItem;
	currItem.clear();
	tmpSorted.insert(tmpSorted.begin(), entries, currItem);

	// Sort items and store them in tempoary tmpSorted.
	for (i = 0; i < entries; ++i) {
		doc->getEntry(catDocIndex, i, &currItem);
		//qDebug("PwMView::shiftToView: %s, %i", currItem.desc.c_str(), currItem.listViewPos);
		tmpSorted[currItem.listViewPos] = currItem;
	}

	// shift tempoary data to ListView.
	tmpDisableSort();
	lv->clear();

	//US ENH: adjust the headers of the table according the category texts
	{
	  PwMCategoryItem* catItem = doc->getCategoryEntry(catDocIndex);
	  //	  qDebug("PwMView::ShiftToView CAT: %i, %s", catDocIndex, catItem->name.c_str());
	  lv->setColumnText(COLUMN_DESC, catItem->desc_text.c_str());
	  lv->setColumnText(COLUMN_NAME, catItem->name_text.c_str());
	  lv->setColumnText(COLUMN_PW, catItem->pw_text.c_str());
	}

	QCheckListItem *newItem;
	vector<PwMDataItem>::iterator it = tmpSorted.begin(),
				      end = tmpSorted.end();
	while (it != end) {
		newItem = new ListViewItemPwM(lv);
		newItem->setText(COLUMN_DESC, (*it).desc.c_str());
		if ((*it).binary) {
			newItem->setText(COLUMN_NAME, "");
			newItem->setText(COLUMN_PW, i18n("<BINARY ENTRY>"));
			newItem->setText(COLUMN_URL, "");
			newItem->setText(COLUMN_LAUNCHER, (*it).launcher.c_str());
		} else {
			newItem->setText(COLUMN_NAME, (*it).name.c_str());
			if ((*it).lockStat) {
				newItem->setText(COLUMN_PW, QString((*it).pw.c_str())
						 + " "
						 + i18n("To unlock click the icon on the left."));
			} else {
				newItem->setText(COLUMN_PW, (*it).pw.c_str());
			}
			newItem->setText(COLUMN_URL, (*it).url.c_str());
			newItem->setText(COLUMN_LAUNCHER, (*it).launcher.c_str());
		}
		newItem->setOn(!((*it).lockStat));
		lv->insertItem(newItem);
		++it;
	}
	tmpReEnableSort();

	if (cY || cX)
		lv->setContentsPos(cX, cY);
}

void PwMView::reorgLp()
{
	if (!lv->childCount())
		return;
	PWM_ASSERT(doc);
	PWM_ASSERT(!doc->isDocEmpty());
	QListViewItem *currItem;
	vector<unsigned int> foundPos;
	/* This searchIn _should_ be:
	 *	const unsigned int searchIn = SEARCH_IN_DESC;
	 * But we want backward compatibility (see comment in PwMDoc::addEntry()).
	 * So we need to search again, if we don't find the entry. (see below)
	 */
	const unsigned int searchIn = SEARCH_IN_DESC | SEARCH_IN_NAME |
				      SEARCH_IN_URL | SEARCH_IN_LAUNCHER;
	QString curCat(getCurrentCategory());
	PwMDataItem findThis;
	unsigned int i, cnt = lv->childCount();
	for (i = 0; i < cnt; ++i) {
		currItem = lv->itemAtIndex(i);
		findThis.desc = currItem->text(COLUMN_DESC).latin1();
		findThis.name = currItem->text(COLUMN_NAME).latin1();
		findThis.url = currItem->text(COLUMN_URL).latin1();
		findThis.launcher = currItem->text(COLUMN_LAUNCHER).latin1();
		doc->findEntry(curCat, findThis, searchIn,
			       &foundPos, true);
		if (!foundPos.size()) {
			/* Did not find the entry. We seem to have a binary
			 * entry here (pray for it!). So search again with
			 * the "correct" searchIn flags.
			 */
			const unsigned int searchIn2 = SEARCH_IN_DESC;
			doc->findEntry(curCat, findThis, searchIn2,
				       &foundPos, true);
			if (unlikely(!foundPos.size())) {
				BUG();
				continue;
			}
			/* We assert that it's a binary entry, now.
			 * No chance to efficiently verify it here.
			 */
		}
		doc->setListViewPos(curCat, foundPos[0], cnt - i - 1);
	}
}

void PwMView::selAt(int index)
{
	QListViewItem *item = lv->itemAtIndex(index);
	if (!item)
		return;
	lv->setCurrentItem(item);
	lv->ensureItemVisible(item);
}

void PwMView::renCatButton_slot()
{
	if (doc->isDeepLocked())
		return;
	RenCatWnd wnd(this);
	if (wnd.exec() == 1) {
		QString newName(wnd.getNewName());
		if (newName == "")
			return;
		document()->renameCategory(getCurrentCategory(),
					   newName);
	}
}

void PwMView::delCatButton_slot()
{
	if (doc->isDeepLocked())
		return;
	if (numCategories() <= 1) {
		mainClass->showStatMsg(i18n("Can't remove the last category."));
		return;
	}
	if (KMessageBox::questionYesNo(this,
				       i18n("Do you really want to\n"
					    "delete the selected\n"
					    "category? All password-\n"
					    "entries will be lost in\n"
					    "this category!\n"),
				       i18n("Delete category?"))
	    == KMessageBox::No) {
		return;
	}
	document()->delCategory(getCurrentCategory());
}

void PwMView::copyPwToClip()
{
	if (doc->isDeepLocked())
		return;
	unsigned int curIndex = 0;
	if (!getCurEntryIndex(&curIndex))
		return;
	PwMDataItem d;
	document()->getDataChangedLock();
	document()->getEntry(getCurrentCategory(), curIndex, &d, true);
	document()->putDataChangedLock();
	PwM::copyToClipboard(d.pw.c_str());
}

void PwMView::copyNameToClip()
{
	if (doc->isDeepLocked())
		return;
	unsigned int curIndex = 0;
	if (!getCurEntryIndex(&curIndex))
		return;
	PwMDataItem d;
	document()->getEntry(getCurrentCategory(), curIndex, &d);
	PwM::copyToClipboard(d.name.c_str());
}

void PwMView::copyDescToClip()
{
	if (doc->isDeepLocked())
		return;
	unsigned int curIndex = 0;
	if (!getCurEntryIndex(&curIndex))
		return;
	PwMDataItem d;
	document()->getEntry(getCurrentCategory(), curIndex, &d);
	PwM::copyToClipboard(d.desc.c_str());
}

void PwMView::copyUrlToClip()
{
	if (doc->isDeepLocked())
		return;
	unsigned int curIndex = 0;
	if (!getCurEntryIndex(&curIndex))
		return;
	PwMDataItem d;
	document()->getEntry(getCurrentCategory(), curIndex, &d);
	PwM::copyToClipboard(d.url.c_str());
}

void PwMView::copyLauncherToClip()
{
	if (doc->isDeepLocked())
		return;
	unsigned int curIndex = 0;
	if (!getCurEntryIndex(&curIndex))
		return;
	PwMDataItem d;
	document()->getEntry(getCurrentCategory(), curIndex, &d);
	PwM::copyToClipboard(d.launcher.c_str());
}

void PwMView::copyCommentToClip()
{
	if (doc->isDeepLocked())
		return;
	unsigned int curIndex = 0;
	if (!getCurEntryIndex(&curIndex))
		return;
	PwMDataItem d;
	document()->getEntry(getCurrentCategory(), curIndex, &d);
	PwM::copyToClipboard(d.comment.c_str());
}

/************************************************************************
 *
 *
 *
 ************************************************************************/


PwMDataItemView::PwMDataItemView( QWidget *parent, const char *name )
  : QTextBrowser( parent, name )


{
//US  setWrapPolicy( QTextEdit::AtWordBoundary );
  setLinkUnderline( false );
  // setVScrollBarMode( QScrollView::AlwaysOff );
  //setHScrollBarMode( QScrollView::AlwaysOff );

//US  QStyleSheet *sheet = styleSheet();
//US  QStyleSheetItem *link = sheet->item( "a" );
//US  link->setColor( KGlobalSettings::linkColor() );

}

void PwMDataItemView::setPwMDataItem( const PwMDataItem& a )

{
  mItem = a;
  // clear view
  setText( QString::null );


  QString dynamicPart;
  QString format = "<tr><td align=\"right\"><b>%1</b></td>"
    "<td align=\"left\">%2</td></tr>";

  dynamicPart += format
          .arg( i18n("LastUpdate") )
          .arg( mItem.meta.update.toString().latin1() );

  dynamicPart += format
          .arg( i18n("Description") )
          .arg( mItem.desc.c_str() );

  dynamicPart += format
          .arg( i18n("Name") )
          .arg( mItem.name.c_str() );

  dynamicPart += format
          .arg( i18n("Password") )
          .arg( mItem.pw.c_str() );

  QString comment(mItem.pw.c_str());
  dynamicPart += format
          .arg( i18n("Comment") )
          .arg( comment.replace( QRegExp("\n"), "<br>" ) );

  dynamicPart += format
          .arg( i18n("URL") )
          .arg( mItem.url.c_str() );

  dynamicPart += format
          .arg( i18n("Launcher") )
          .arg( mItem.launcher.c_str() );

  QString mText = "<table><td colspan=\"2\">&nbsp;</td>";

  mText += dynamicPart;
  mText += "</table>";

  // at last display it...
  setText( mText );

}

PwMDataItem PwMDataItemView::pwmdataitem() const
{
  return mItem;
}

/************************************************************************
 *
 *
 *
 ************************************************************************/


PwMDataItemChooser::PwMDataItemChooser( PwMDataItem loc, PwMDataItem rem, bool takeloc, QWidget *parent, const char *name ) 
  : KDialogBase(parent, name, true ,
		i18n("Conflict! Please choose Entry!"),Ok|User1|Close,Close, false)
{
    findButton( Close )->setText( i18n("Cancel Sync"));
    findButton( Ok )->setText( i18n("Remote"));
    findButton( User1 )->setText( i18n("Local"));
    QWidget* topframe = new QWidget( this );
    setMainWidget( topframe );
    QBoxLayout* bl;
    if ( QApplication::desktop()->width() < 640 ) {
        bl = new QVBoxLayout( topframe  );
    } else {
        bl = new QHBoxLayout( topframe  );
    }
    QVBox* subframe = new QVBox( topframe );
    bl->addWidget(subframe  );
    QLabel* lab = new QLabel( i18n("Local Entry"), subframe ); 
    if ( takeloc )
        lab->setBackgroundColor(Qt::green.light() );
    PwMDataItemView * av = new PwMDataItemView( subframe );
    av->setPwMDataItem( loc );
    subframe = new QVBox( topframe );
    bl->addWidget(subframe  );
    lab = new QLabel( i18n("Remote Entry"), subframe  ); 
    if ( !takeloc )
        lab->setBackgroundColor(Qt::green.light() );
    av = new PwMDataItemView( subframe );
    av->setPwMDataItem( rem );
    QObject::connect(findButton( Ok ),SIGNAL(clicked()),this, SLOT(slot_remote())); 
    QObject::connect(this,SIGNAL(user1Clicked()),this, SLOT(slot_local())); 
#ifndef DESKTOP_VERSION
    showMaximized();
#else
    resize ( 640, 400 );
#endif
}

int PwMDataItemChooser::executeD( bool local )
{
    mSyncResult = 3; 
    if ( local ) 
        findButton( User1 )->setFocus();
    else 
        findButton( Ok )->setFocus();
    exec();
    return mSyncResult;  
}
void PwMDataItemChooser::slot_remote()
{
    mSyncResult = 2;
    accept();
}
void PwMDataItemChooser::slot_local()
{
    mSyncResult = 1;
    accept();
}



#ifndef PWM_EMBEDDED
#include "pwmview.moc"
#endif
