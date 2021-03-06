/*
    This file is part of KAddressbook.
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

#ifndef EXTENSIONMANAGER_H
#define EXTENSIONMANAGER_H

#include <qhbox.h>
#include <qscrollview.h>
#include <qptrlist.h>

#include <extensionwidget.h>

class KABCore;
class KSelectAction;


class ExtensionManager : public QScrollView
{
  Q_OBJECT

  public:
    ExtensionManager( KABCore *core, QWidget *parent, const char *name = 0 );
    ~ExtensionManager();

    /**
      Restores the extension manager specific settings.
     */
    void restoreSettings();

    /**
      Saves the extension manager specific settings.
     */
    void saveSettings();

    /**
      Rereads the extension manager specific settings with some
      additional initialization stuff.
     */
    void reconfigure();

    /**
      Returns whether the quickedit extension is currently visible.
     */
    bool isQuickEditVisible() const;

  public slots:
    void setSelectionChanged();

  signals:
    void modified( const KABC::Addressee::List& );
    void changedActiveExtension( int id );

  private slots:
    void setActiveExtension( int id );

  private:
    void createExtensionWidgets();

    KABCore *mCore;
    QWidget *mWidgetBox;

    ExtensionWidget *mCurrentExtensionWidget;
    QPtrList<ExtensionWidget> mExtensionWidgetList;

    KSelectAction *mActionExtensions;

};

#endif
