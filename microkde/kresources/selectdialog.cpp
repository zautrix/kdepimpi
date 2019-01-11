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

/*US
#include <kbuttonbox.h>
#include <klistbox.h>
#include <klocale.h>
#include <kmessagebox.h>

*/
#include <klocale.h>
#include <kmessagebox.h>

//US
#include <kglobal.h>

#include <qlistbox.h>
#include <qlayout.h>
#include <qgroupbox.h>

#include "resource.h"

#include "selectdialog.h"

using namespace KRES;

//US I am using KBaseDialog instead of KDialog
//US  : KDialog( parent, name, true )
SelectDialog::SelectDialog( QPtrList<Resource> list, QWidget *parent,
                            const char *name )
  : KDialogBase( parent, name, true, i18n( "Resource Selection" ), Help | Ok | Cancel,
                 Ok, true)

{
//US  setCaption( i18n( "Resource Selection" ) );
//US  resize( 300, 200 );
  resize( KMIN(KGlobal::getDesktopWidth(), 300), KMIN(KGlobal::getDesktopHeight(), 200) );

//US  
  QFrame *main = plainPage();
/*US
  QVBoxLayout *layout = new QVBoxLayout( main );
  mConfigPage = new KRES::ConfigPage( main );
  layout->addWidget( mConfigPage );
*/

//US  QVBoxLayout *mainLayout = new QVBoxLayout( this );
  QVBoxLayout *mainLayout = new QVBoxLayout( main );
  mainLayout->setMargin( marginHint() );
    
//US  QGroupBox *groupBox = new QGroupBox( 2, Qt::Horizontal,  this );
  QGroupBox *groupBox = new QGroupBox( 2, Qt::Horizontal,  main );
  groupBox->setTitle( i18n( "Resources" ) );

//US  mResourceId = new KListBox( groupBox );
  mResourceId = new QListBox( groupBox );

  mainLayout->addWidget( groupBox );

  mainLayout->addSpacing( 40 );

/*US 
  KButtonBox *buttonBox = new KButtonBox( this );

  buttonBox->addStretch();    
  buttonBox->addButton( i18n( "&OK" ), this, SLOT( accept() ) );
  buttonBox->addButton( i18n( "&Cancel" ), this, SLOT( reject() ) );
  buttonBox->layout();

  mainLayout->addWidget( buttonBox );
*/
  // setup listbox
  uint counter = 0;
  for ( uint i = 0; i < list.count(); ++i ) {
    Resource *resource = list.at( i );
    if ( resource && !resource->readOnly() ) {
      mResourceMap.insert( counter, resource );
      mResourceId->insertItem( resource->resourceName() );
      counter++;
    }
  }

  mResourceId->setCurrentItem( 0 );
  connect( mResourceId, SIGNAL(returnPressed(QListBoxItem*)), 
           SLOT(accept()) );
}

Resource *SelectDialog::resource()
{
  if ( mResourceId->currentItem() != -1 )
    return mResourceMap[ mResourceId->currentItem() ];
  else
    return 0;
}

Resource *SelectDialog::getResource( QPtrList<Resource> list, QWidget *parent )
{
  if ( list.count() == 0 ) {
    KMessageBox::error( parent, i18n( "There is no resource available!" ) );
    return 0;
  }

  if ( list.count() == 1 ) return list.first();

  // the following lines will return a writeable resource if only _one_ writeable
  // resource exists
  Resource *found = 0;
  Resource *it = list.first();
  while ( it ) {
    if ( !it->readOnly() ) {
      if ( found ) {
        found = 0;
	break;
      } else
        found = it;
    }
    it = list.next();
  }

  if ( found )
    return found;
  
  SelectDialog dlg( list, parent);
//US  if ( dlg.exec() == KDialog::Accepted )
  if ( dlg.exec() )
    return dlg.resource();
  else 
    return 0;
}

/*US
#include "selectdialog.moc"
*/

