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
 * $Id: pwmview.h,v 1.4 2004/10/18 02:57:22 ulf69 Exp $
 **************************************************************************/  

#ifndef PWMVIEW_H
#define PWMVIEW_H

//US ENH: wouldn't it be a good idea if we could use this consts everywhere else.
//US ENH: for examle in listviewpwm.cpp
//US ENH: Because of that I transfer them into the headerfile.
#define COLUMN_DESC		0
#define COLUMN_NAME		1
#define COLUMN_PW		2
#define COLUMN_URL		3
#define COLUMN_LAUNCHER		4


#include "listviewpwm.h"
#include "pwmdoc.h"
#include "pwmviewstyle.h"

#include <kconfig.h>
#include <klocale.h>
#include <kdialogbase.h>

#include <qevent.h>
#include <qfont.h>
#include <qobject.h>
#include <qtextbrowser.h>

#include <vector>
#include <string>

using std::string;
using std::vector;

class PwM;
class ConfFile;
class PwMStatusBar;


/** View class for PwM */
class PwMView : public PwMViewStyle
{
	Q_OBJECT
	friend class PwMViewStyle;
public:
	/** construtor */
	PwMView(PwM *_mainClass, QWidget* parent, PwMDoc *_doc,
		const char *name = 0);
	/** destructor */
	~PwMView();

	/** returns pointer to the document */
	PwMDoc* document()
			{ return doc; }
	/** returns the index of the currently selected entry.
	  * (index as represented in PwMDoc !)
	  */
	bool getCurEntryIndex(unsigned int *index);
	/** returns the position of the given item in the document
	  * Note: This func only serches in the current category.
	  */
	bool getDocEntryIndex(unsigned int *index,
			      const QListViewItem *item);

public slots:
	/** update the view (call if dirty) */
	void updateView()
		{
			updateCategories();
			shiftToView();
		}
	/** (re)sort all items and (re)shift them to listView. */
	void shiftToView();
	/** handle clicking on an item */
	void handleToggle(QListViewItem *item);
	/** handle right-clicking on an item */
	void handleRightClick(QListViewItem *item, const QPoint &point, int);
	/** selects the item at "index" */
	void selAt(int index);
	/** rename category button pressed */
	void renCatButton_slot();
	/** delete category button pressed */
	void delCatButton_slot();

protected:
	/** right-click context-menu */
	QPopupMenu *ctxMenu;

protected:
	/** update the categories from document */
	void updateCategories();
	/** widget resize event */
	void resizeEvent(QResizeEvent *);
	/** initialize context-menu */
	void initCtxMenu();
	/** tempoarly disable auto-sorting and user-sorting */
	void tmpDisableSort()
			{ lv->setSorting(-1); }
	/** re-enable tempoarly disabled sorting */
	void tmpReEnableSort()
		{
			lv->setSorting(lv->columns() + 1,
				       true/*lv->sortOrder() == Qt::Ascending*/);
		}
	/** The user pressed and released a key. */
	void keyReleaseEvent(QKeyEvent *e);

protected slots:
	/** changes the comment text-edit, because a new item has been selected */
	void refreshCommentTextEdit(QListViewItem *curItem);
	/** copy pw to clipboard */
	void copyPwToClip();
	/** copy name to clipboard */
	void copyNameToClip();
	/** copy desc to clipboard */
	void copyDescToClip();
	/** copy url to clipboard */
	void copyUrlToClip();
	/** copy launcher to clipboard */
	void copyLauncherToClip();
	/** copy comment to clipboard */
	void copyCommentToClip();
	/** reorganize the "listViewPos" positions in the document
	  * (for the current category only!)
	  */
	void reorgLp();

private:
	/** document */
	PwMDoc *doc;
	/** pointer to the main class "PwM" */
	PwM *mainClass;
};


//US ENH basic widget to view an password entry. We need it for the sync stuff.
//But might be oif interest for other functionalities as well.
class PwMDataItemView : public QTextBrowser
{
  public:
    PwMDataItemView( QWidget *parent = 0, const char *name = 0 );

    /**
      Sets the PwMDataItem object. It is displayed immediately.

      @param a The PwMDataItem object.
     */
    void setPwMDataItem( const PwMDataItem& a );

    /**
      Returns the current PwMDataItem object.
     */
    PwMDataItem pwmdataitem() const;

  private:
    PwMDataItem mItem;
};


//US ENH we need this chooser when syncing results in a conflict
class PwMDataItemChooser : public KDialogBase
{
    Q_OBJECT

  public:
    PwMDataItemChooser( PwMDataItem loc, PwMDataItem rem, bool takeloc, QWidget *parent = 0, const char *name = 0 );

    int executeD( bool local );

  private:
    int mSyncResult;

  private slots:
    void slot_remote();
    void slot_local();

};


#endif
