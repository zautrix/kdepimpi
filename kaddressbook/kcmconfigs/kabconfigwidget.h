/*
    This file is part of KAddressBook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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

#ifndef KABCONFIGWIDGET_H
#define KABCONFIGWIDGET_H

#include <kprefswidget.h>

class QCheckBox;
class QListViewItem;
class QPushButton;
class QComboBox;
class QLineEdit;
class KListView;
class KABPrefs;

class AddresseeWidget;

class KABConfigWidget : public KPrefsWidget
{
  Q_OBJECT

  public:
    KABConfigWidget(KABPrefs *prefs, QWidget *parent, const char *name = 0 );

  protected:
    /** Implement this to read custom configuration widgets. */
  virtual void usrReadConfig();
    /** Implement this to write custom configuration widgets. */
  virtual void usrWriteConfig();



  private slots:
    void configureExtension();
    void selectionChanged( QListViewItem* );
    void itemClicked( QListViewItem* );

  private:
    void restoreExtensionSettings();
    void saveExtensionSettings();

    KListView *mExtensionView;
    QCheckBox *mSearchReturnBox;
    QCheckBox *mNameParsing;
    QCheckBox *mViewsSingleClickBox;
    QCheckBox *mAutoSearchWithWildcardBox;
    QCheckBox *mHideSearchOnSwitchBox;
    QCheckBox *mMultipleViewsAtOnce;
    QCheckBox *mAskForQuit;
    QCheckBox *mMenuBarBox;
    QCheckBox *mAskForDelete;
    QPushButton *mConfigureButton;

    AddresseeWidget *mAddresseeWidget;
};

#endif
