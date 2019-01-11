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

#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvbox.h>

#include <qcheckbox.h>
#include <qscrollview.h>

#include <kbuttonbox.h>
#include <kdialog.h>
#include <klineedit.h>

#include "factory.h"
#include "configwidget.h"
#include "configdialog.h"

using namespace KRES;

ConfigDialog::ConfigDialog( QWidget *parent, const QString& resourceFamily,
    Resource* resource, const char *name )
  : KDialogBase( parent, name, true, i18n( "Resource Configuration" ),
    Ok|Cancel, Ok, true )/*, mConfig( config )*/, mResource( resource ), mPersistentReadOnly(false)
{

  Factory *factory = Factory::self( resourceFamily );

//US  resize( 250, 240 );
  resize( KMIN(KGlobal::getDesktopWidth(), 250), KMIN(KGlobal::getDesktopHeight(), 240));

  QFrame *main;

  main = plainPage();

  QVBoxLayout *mainLayout = new QVBoxLayout( main, 0, spacingHint() );


  QGroupBox *generalGroupBox = new QGroupBox( 2, Qt::Horizontal, main );
  generalGroupBox->layout()->setSpacing( spacingHint() );
  generalGroupBox->setTitle( i18n( "General Settings" ) );

  new QLabel( i18n( "Name:" ), generalGroupBox );

  mName = new KLineEdit( generalGroupBox );

  new QLabel("", generalGroupBox );
  mReadOnly = new QCheckBox( i18n( "Read-only" ), generalGroupBox );
  mReadOnly->setChecked( mResource->readOnly() );

  new QLabel("", generalGroupBox );
  mIncludeInSync = new QCheckBox( i18n( "Include in sync" ), generalGroupBox );
  mIncludeInSync->setChecked( mResource->includeInSync() );

  mName->setText( mResource->resourceName() );

  mainLayout->addWidget( generalGroupBox );

  QGroupBox *resourceGroupBox = new QGroupBox( 2, Qt::Horizontal,  main );
  resourceGroupBox->layout()->setSpacing( spacingHint());
  resourceGroupBox->setTitle( i18n( "%1 Resource Settings" )
                              .arg( factory->typeName( resource->type() ) ) );
  mainLayout->addWidget( resourceGroupBox );

  mainLayout->addStretch();

  mConfigWidget = factory->configWidget( resource->type(), resourceGroupBox );
  if ( mConfigWidget ) {
    connect( mConfigWidget, SIGNAL( setReadOnly( bool ) ),
        SLOT( setReadOnly( bool ) ) );
    connect( mConfigWidget, SIGNAL( setIncludeInSync( bool ) ),
        SLOT( setIncludeInSync( bool ) ) );
    connect( mConfigWidget, SIGNAL( setPersistentReadOnly( bool ) ),
        SLOT( setPersistentReadOnly( bool ) ) );
    mConfigWidget->setInEditMode( false );
    mConfigWidget->loadSettings( mResource );
    mConfigWidget->show();

  }


  connect( mName, SIGNAL( textChanged(const QString &)),
      SLOT( slotNameChanged(const QString &)));

  slotNameChanged( mName->text() );

//US  setMinimumSize( 400, 250 );
  setMinimumSize( KMIN(KGlobal::getDesktopWidth(), 400), KMIN(KGlobal::getDesktopHeight(), 250));

}

void ConfigDialog::setInEditMode( bool value )
{
  if ( mConfigWidget )
    mConfigWidget->setInEditMode( value );
}

void ConfigDialog::slotNameChanged( const QString &text)
{
  enableButtonOK( !text.isEmpty() );
}

void ConfigDialog::setReadOnly( bool value )
{
    if (mPersistentReadOnly == false)
      mReadOnly->setChecked( value );
    else
      mReadOnly->setChecked( true );
}

void ConfigDialog::setIncludeInSync( bool value )
{
    if (mPersistentReadOnly == false)
      mIncludeInSync->setChecked( value );
    else
      mIncludeInSync->setChecked( false );
}

void ConfigDialog::setPersistentReadOnly( bool value )
{
    mPersistentReadOnly = value;

    if (value == true) {
      setReadOnly( true );
      setIncludeInSync( false );
    }

    mReadOnly->setEnabled( !value );
    mIncludeInSync->setEnabled (!value );
}


void ConfigDialog::accept()
{
  if ( mName->text().isEmpty() ) {
    KMessageBox::sorry( this, i18n( "Please enter a resource name" ) );
    return;
  }

  mResource->setResourceName( mName->text() );
  mResource->setReadOnly( mReadOnly->isChecked() );
  mResource->setIncludeInSync( mIncludeInSync->isChecked() );

  if ( mConfigWidget ) {
    // First save generic information
    // Also save setting of specific resource type
    mConfigWidget->saveSettings( mResource );
  }

  KDialog::accept();
}

//US #include "configdialog.moc"
