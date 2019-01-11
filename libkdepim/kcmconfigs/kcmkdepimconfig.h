/*
    This file is part of KdePim/Pi.
    Copyright (c) 2004 Ulf Schenk

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

$Id: kcmkdepimconfig.h,v 1.3 2004/09/21 19:50:28 ulf69 Exp $
*/

#ifndef KCMKDEPIMCONFIG_H
#define KCMKDEPIMCONFIG_H

#include <kcmodule.h>

class KDEPIMConfigWidget;
class KPimGlobalPrefs;

class KCMKdePimConfig : public KCModule
{
  Q_OBJECT

  public:
    KCMKdePimConfig( QWidget *parent = 0, const char *name = 0 );

    virtual void load();
    virtual void save();
    virtual void defaults();

  private:
    KDEPIMConfigWidget *mConfigWidget;
};

#endif
