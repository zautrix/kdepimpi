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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: xxportmanager.h,v 1.2 2005/08/17 19:09:13 zautrix Exp $
*/


#ifndef XXPORTMANAGER_H
#define XXPORTMANAGER_H

#include <qdict.h>
#include <qobject.h>

#include <kurl.h>

#include "xxportobject.h"

class KABCore;

class XXPortManager : public QObject
{
  Q_OBJECT

  public:
    XXPortManager( KABCore *core, QObject *parent, const char *name = 0 );
    ~XXPortManager();

    void restoreSettings();
    void saveSettings();

    static KURL importURL;
    static QString importData;

  public slots:
    void importVCard( const KURL &url );
    void importVCard( const KURL &url, bool showPreview );
    void importVCard( const QString &vCard, bool showPreview );

  signals:
    void modified();

  protected slots: 
    void noPreview();
    void slotImport( const QString&, const QString& );
    void slotExport( const QString&, const QString& );

  private:
    void loadPlugins();

    QDict<XXPortObject> mXXPortObjects;

    KABCore *mCore;
    bool mShowPreview;
};

#endif
