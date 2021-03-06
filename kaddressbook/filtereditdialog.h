/*
    This file is part of KAddressBook.
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: filtereditdialog.h,v 1.2 2005/01/16 14:11:00 zautrix Exp $
*/

#ifndef FILTEREDITDIALOG_H
#define FILTEREDITDIALOG_H

class QButtonGroup;
class QString;
class QToolButton;
class QWidget;
class QListBoxItem;
class QCheckBox;

class KLineEdit;
class KListBox;
class KListView;

#include <kdialogbase.h>
#include <qhbuttongroup.h>

#include "filter.h"

class FilterDialog : public KDialogBase
{
  Q_OBJECT

  public:
    FilterDialog( QWidget *parent, const char *name = 0 );
    ~FilterDialog();

    void setFilters( const Filter::List &list );
    Filter::List filters() const;

  protected slots:
    void add();
    void edit();
    void remove();
    void selectionChanged( QListBoxItem* );

  private:
    void initGUI();
    void refresh();

    Filter::List mFilterList;
    Filter::List mInternalFilterList;

    KListBox *mFilterListBox;
    QPushButton *mAddButton;
    QPushButton *mEditButton;
    QPushButton *mRemoveButton;
};

class FilterEditDialog : public KDialogBase
{
    Q_OBJECT
  public:
    FilterEditDialog( QWidget *parent, const char *name = 0 );
    ~FilterEditDialog();

    void setFilter( const Filter &filter );
    Filter filter();

  protected slots:
    void filterNameTextChanged( const QString& );
    void slotHelp();

  private:
    void initGUI();

    Filter mFilter;

    KLineEdit *mNameEdit;
    KListView *mCategoriesView;
    QHButtonGroup *mMatchRuleGroup;
    QCheckBox *mPrivate;
    QCheckBox *mPublic;
    QCheckBox *mConfidential;
    QPushButton *mEditButton;
    QPushButton *mRemoveButton;
};

#endif
