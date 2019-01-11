/*
    This file is part of KDEPim/Pi.
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

$Id: kcmkdepimconfig.cpp,v 1.5 2005/04/03 19:06:45 zautrix Exp $
*/


#include <qlayout.h>

#include <kdebug.h>
//#include <klocale.h>
//#include <stdlib.h>

#include "kdepimconfigwidget.h"

#include "kcmkdepimconfig.h"
#include "kprefs.h"
#include "kpimglobalprefs.h"

#ifndef _WIN32_
extern "C"
{
  KCModule *create_kdepimconfig(QWidget *parent, const char * ) {
    return new KCMKdePimConfig(parent, "kcmkdepimconfig" );
  }
}
#endif

KCMKdePimConfig::KCMKdePimConfig(QWidget *parent, const char *name )
  : KCModule( KPimGlobalPrefs::instance(), parent, name )
{
    //abort();
  QVBoxLayout *layout = new QVBoxLayout( this );
  mConfigWidget = new KDEPIMConfigWidget( (KPimGlobalPrefs*)getPreferences(), this, "KDEPIMConfigWidget" );
  layout->addWidget( mConfigWidget );
  layout->setSpacing( 0 );
  layout->setMargin( 0 );

  connect( mConfigWidget, SIGNAL( changed( bool ) ), SIGNAL( changed( bool ) ) );
}

void KCMKdePimConfig::load()
{
  mConfigWidget->readConfig();
}

void KCMKdePimConfig::save()
{
  mConfigWidget->writeConfig();
}

void KCMKdePimConfig::defaults()
{
  qDebug("KCMKdePimConfig::defaults()");
  mConfigWidget->setDefaults();
}
