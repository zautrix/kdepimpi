/*
    This file is part of libkresources.

    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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

#include <qlayout.h>

//US #include <kaboutdata.h>
//US #include <kgenericfactory.h>
#include <klocale.h>

#include "configpage.h"

#include "kcmkresources.h"

using namespace KRES;

//US typedef KGenericFactory<KCMKResources, QWidget> ResourcesFactory;
//US K_EXPORT_COMPONENT_FACTORY( kcm_kresources, ResourcesFactory( "kcmkresources" ) );

//US KCMKResources::KCMKResources( QWidget *parent, const char *name, const QStringList& )
//US  : KCModule( ResourcesFactory::instance(), parent, name )
KCMKResources::KCMKResources( QWidget *parent, const char *name, const QStringList& )
  : KDialogBase( parent, name, true, i18n( "Configure Resources" ),
    Ok|Cancel, Ok, true )
{
  QFrame *main = plainPage();

  QVBoxLayout *layout = new QVBoxLayout( main );
  mConfigPage = new KRES::ConfigPage( main );
  layout->addWidget( mConfigPage );


  connect( mConfigPage, SIGNAL( changed( bool ) ), SLOT( changed( bool ) ) );
#ifndef DESKTOP_VERSION
    showMaximized();
#endif
}

void KCMKResources::changed( bool changed)
{
  modified = changed;
}

void KCMKResources::slotOk()
{
  if (modified) {
    mConfigPage->save();
    modified = false;
  }
  
  KDialogBase::slotOk();
}

void KCMKResources::load()
{
  qDebug("KCMKResources::load" );
  mConfigPage->load();
}

void KCMKResources::save()
{
  qDebug("KCMKResources::save" );
  mConfigPage->save();
}

void KCMKResources::defaults()
{
  qDebug("KCMKResources::defaults" );
  mConfigPage->defaults();
}

//US #include "kcmkresources.moc"
