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

$Id: kde2_xxport.h,v 1.1.1.1 2004/06/26 19:02:21 zautrix Exp $
*/

#ifndef KDE2_XXPORT_H
#define KDE2_XXPORT_H

#include <xxportobject.h>

class KDE2XXPort : public XXPortObject
{
  Q_OBJECT

  public:
    KDE2XXPort( KABC::AddressBook *ab, QWidget *parent, const char *name = 0 );

    QString identifier() const { return "kde2"; }

  public slots:
    KABC::AddresseeList importContacts( const QString &data ) const;
};


#ifdef KAB_EMBEDDED
class KDE2XXPortFactory : public XXPortFactory
{
  public:
    XXPortObject *xxportObject( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new KDE2XXPort( ab, parent, name );
    }
};
#endif //KAB_EMBEDDED


#endif
