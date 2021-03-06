/*
    This file is part of libkresources.

    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>
    Copyright (c) 2002 Jan-Pascal van Best <janpascal@vanbest.org>
    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: managerimpl.h,v 1.3 2004/10/13 21:31:20 ulf69 Exp $
*/

#ifndef KRESOURCES_MANAGERIMPL_H
#define KRESOURCES_MANAGERIMPL_H

#include <qstring.h>
#include <qptrlist.h>
#include <qdict.h>
//US
#include <qobject.h>

#include "resource.h"


class KConfig;

namespace KRES {

class Resource;
class Factory;

class ManagerImplListener
{
  public:
    virtual void resourceAdded( Resource *resource ) = 0;
    virtual void resourceModified( Resource *resource ) = 0;
    virtual void resourceDeleted( Resource *resource ) = 0;
};


/**
  @internal

  Do not use this class directly. Use ResourceManager instead
*/
class ManagerImpl : public QObject
{
    Q_OBJECT
  public:
    ManagerImpl( const QString &family);
    ~ManagerImpl();

    void readConfig( KConfig * );
    void writeConfig( KConfig * );

    void add( Resource *resource, bool useDCOP = true );
    void remove( Resource *resource, bool useDCOP = true );

    Resource *standardResource();
    void setStandardResource( Resource *resource );

    void setActive( Resource *resource, bool active );

    Resource::List *resourceList();

    QPtrList<Resource> resources();

    // Get only active or passive resources
    QPtrList<Resource> resources( bool active );

    QStringList resourceNames();

    void setListener( ManagerImplListener *listener );

  public slots:
    void resourceChanged( Resource *resource );

  private:
    // dcop calls

  private:
    void createStandardConfig();

    Resource *readResourceConfig( const QString& identifier, bool checkActive );
    void writeResourceConfig( Resource *resource, bool checkActive );

    void removeResource( Resource *resource );
    Resource *getResource( Resource *resource );
    Resource *getResource( const QString& identifier );

    QString mFamily;
    KConfig *mConfig;
    KConfig *mStdConfig;
    Resource *mStandard;
    Factory *mFactory;
    Resource::List mResources;
    ManagerImplListener *mListener;
};

}

#endif
