/*
    This file is part of PwManager/Pi
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

#include <qlayout.h>

#include <kdebug.h>

#include "pwmconfigwidget.h"

#include "kcmpwmconfig.h"

#include "pwmprefs.h"
#include "kprefs.h"

extern "C"
{
  KCModule *create_pwmconfig(QWidget *parent, const char * ) {
    return new KCMPwmConfig(parent, "kcmpwmconfig" );
  }
}

KCMPwmConfig::KCMPwmConfig(QWidget *parent, const char *name )
  : KCModule( PWMPrefs::instance(), parent, name )
{
  QVBoxLayout *layout = new QVBoxLayout( this );
  mConfigWidget = new PWMConfigWidget( (PWMPrefs*)getPreferences(), this, "mConfigWidget" );
  layout->addWidget( mConfigWidget );
  layout->setSpacing( 0 );
  layout->setMargin( 0 );

  connect( mConfigWidget, SIGNAL( changed( bool ) ), SIGNAL( changed( bool ) ) );
}

void KCMPwmConfig::load()
{
  mConfigWidget->readConfig();
}

void KCMPwmConfig::save()
{
  mConfigWidget->writeConfig();
}

void KCMPwmConfig::defaults()
{
  mConfigWidget->setDefaults();
}


#ifndef PWM_EMBEDDED
#include "kcmkabconfig.moc"
#endif //PWM_EMBEDDED
