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

#include <qlayout.h>

#ifndef KAB_EMBEDDED
#include <kaboutdata.h>
#endif //KAB_EMBEDDED
#include <kdebug.h>
//#include <klocale.h>
//#include <stdlib.h>

#include "kabconfigwidget.h"

#include "kcmkabconfig.h"

#include "kabprefs.h"
#include "kprefs.h"

extern "C"
{
  KCModule *create_kabconfig( QWidget *parent, const char * ) {
    return new KCMKabConfig( parent, "kcmkabconfig" );
  }
}

KCMKabConfig::KCMKabConfig( QWidget *parent, const char *name )
  : KCModule( KABPrefs::instance(), parent, name )
{
    //abort();
  QVBoxLayout *layout = new QVBoxLayout( this );
  mConfigWidget = new KABConfigWidget( (KABPrefs*)getPreferences(), this, "KABConfigWidget" );
  layout->addWidget( mConfigWidget );
  layout->setSpacing( 0 );
  layout->setMargin( 0 );

  connect( mConfigWidget, SIGNAL( changed( bool ) ), SIGNAL( changed( bool ) ) );
}

void KCMKabConfig::load()
{
  mConfigWidget->readConfig();
}

void KCMKabConfig::save()
{
  mConfigWidget->writeConfig();
}

void KCMKabConfig::defaults()
{
  mConfigWidget->setDefaults();
}

#ifndef KAB_EMBEDDED
const KAboutData* KCMKabConfig::aboutData() const
{
  KAboutData *about = new KAboutData( I18N_NOOP( "kcmkabconfig" ),
                                      I18N_NOOP( "KAddressBook Configure Dialog" ),
                                      0, 0, KAboutData::License_GPL,
                                      I18N_NOOP( "(c), 2003 Tobias Koenig" ) );

  about->addAuthor( "Tobias Koenig", 0, "tokoe@kde.org" );

  return about;

}
#endif //KAB_EMBEDDED

#ifndef KAB_EMBEDDED
#include "kcmkabconfig.moc"
#endif //KAB_EMBEDDED
