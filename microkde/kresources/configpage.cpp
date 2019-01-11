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

$Id: configpage.cpp,v 1.9 2005/02/01 12:22:31 zautrix Exp $
*/

#include <qgroupbox.h>

#ifdef DESKTOP_VERSION
#include <qinputdialog.h>
#else
#include <qtcompat/qinputdialog.h>
#endif
#include <qlabel.h>
#include <qlayout.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <klistview.h>
#include <kbuttonbox.h>
//US #include <ktrader.h>

#include "resource.h"
#include "configdialog.h"

#include "configpage.h"

//US
#include <qpushbutton.h>
#include <qfile.h>
#include <kglobal.h>

using namespace KRES;

class ConfigViewItem : public QCheckListItem
{
  public:
    ConfigViewItem( QListView *parent, Resource* resource ) :
      QCheckListItem( parent, resource->resourceName(), CheckBox ),
      mResource( resource ),
      mIsStandard( false )
    {
      setText( 1, mResource->type() );
      setOn( mResource->isActive() );
    }

    void setStandard( bool value )
    {
      setText( 2, ( value ? i18n( "Yes" ) : QString::null ) );
      mIsStandard = value;
    }

    bool standard() const { return mIsStandard; }
    bool readOnly() const { return mResource->readOnly(); }

    Resource *resource() { return mResource; }

  private:
    Resource* mResource;

    bool mIsStandard;
};

ConfigPage::ConfigPage( QWidget *parent, const char *name )
  : QWidget( parent, name ),
    mCurrentManager( 0 ),
    mCurrentConfig( 0 )
{
  setCaption( i18n( "Resource Configuration" ) );

  QVBoxLayout *mainLayout = new QVBoxLayout( this );

  QGroupBox *groupBox = new QGroupBox( i18n( "Resources" ), this );
  groupBox->setColumnLayout(0, Qt::Vertical );
  groupBox->layout()->setSpacing( 3 );
  groupBox->layout()->setMargin( 5 );
  QGridLayout *groupBoxLayout = new QGridLayout( groupBox->layout(), 4, 2 );

//US  mFamilyCombo = new KComboBox( false, groupBox );
  mFamilyCombo = new KComboBox( groupBox );
  groupBoxLayout->addMultiCellWidget( mFamilyCombo, 0, 0, 0, 1 );

  mListView = new KListView( groupBox );
  mListView->setAllColumnsShowFocus( true );
  mListView->addColumn( i18n( "Name" ) );
  mListView->addColumn( i18n( "Type" ) );
  mListView->addColumn( i18n( "Standard" ) ); 

//US  groupBoxLayout->addWidget( mListView, 1, 0 );
  groupBoxLayout->addMultiCellWidget( mListView, 1, 1, 0, 1 );


  mAddButton = new QPushButton( i18n( "&Add..." ), groupBox );
  groupBoxLayout->addWidget( mAddButton, 2, 0 );
  mRemoveButton = new QPushButton( i18n( "&Remove" ), groupBox );
  groupBoxLayout->addWidget( mRemoveButton, 2, 1 );
  mEditButton = new QPushButton( i18n( "&Edit..." ), groupBox );
  groupBoxLayout->addWidget( mEditButton, 3, 0 );
  mStandardButton = new QPushButton( i18n( "&Use as Standard" ), groupBox );
  groupBoxLayout->addWidget( mStandardButton, 3, 1 );

  mRemoveButton->setEnabled( false );
  mEditButton->setEnabled( false );
  mStandardButton->setEnabled( false );


  connect( mAddButton, SIGNAL( clicked() ), SLOT(slotAdd()) );
  connect( mRemoveButton, SIGNAL( clicked() ), SLOT(slotRemove()) );
  connect( mEditButton, SIGNAL( clicked() ), SLOT(slotEdit()) );
  connect( mStandardButton, SIGNAL( clicked() ), SLOT(slotStandard()) );


  mainLayout->addWidget( groupBox );

  connect( mFamilyCombo, SIGNAL( activated( int ) ),
           SLOT( slotFamilyChanged( int ) ) );
  connect( mListView, SIGNAL( selectionChanged() ),
           SLOT( slotSelectionChanged() ) );
  connect( mListView, SIGNAL( clicked( QListViewItem * ) ),
           SLOT( slotItemClicked( QListViewItem * ) ) );

  mLastItem = 0;

//US  mConfig = new KConfig( "kcmkresourcesrc" );
  mConfig = new KConfig( locateLocal( "config", "kcmkresourcesrc")   );
  mConfig->setGroup( "General" );

  load();
}

ConfigPage::~ConfigPage()
{
  QValueList<ResourcePageInfo>::Iterator it;
  for ( it = mInfoMap.begin(); it != mInfoMap.end(); ++it ) {
    (*it).mManager->removeListener( this );
    delete (*it).mManager;
    delete (*it).mConfig;
  }

  mConfig->writeEntry( "CurrentFamily", mFamilyCombo->currentItem() );
  delete mConfig;
  mConfig = 0;
}

void ConfigPage::load()
{
  kdDebug(5650) << "ConfigPage::load()" << endl;

  mListView->clear();

//US we remove the dynamic pluginloader, and set the one family we need (contact) manually.

//US  KTrader::OfferList plugins = KTrader::self()->query( "KResources/Plugin" );
//US  KTrader::OfferList::ConstIterator it;
//US  for ( it = plugins.begin(); it != plugins.end(); ++it ) {
//US    QVariant tmp = (*it)->property( "X-KDE-ResourceFamily" );
//US    QString family = tmp.toString();
  QStringList families;
  families << "contact";


  for ( QStringList::Iterator it = families.begin(); it != families.end(); ++it )
  {
    QString family = (*it);
    if ( !family.isEmpty() ) {
      if ( !mFamilyMap.contains( family ) ) {
        mCurrentManager = new Manager<Resource>( family );
        if ( mCurrentManager ) {
          mFamilyMap.append( family );
          mCurrentManager->addListener( this );

          ResourcePageInfo info;
          info.mManager = mCurrentManager;
          QString configDir = KGlobal::dirs()->saveLocation( "config" );
          //QString configDir = KStandardDirs::appDir() + "/config";
          if ( family == "contact" && QFile::exists( configDir + "/kabcrc" ) ) {
            info.mConfig = new KConfig( locateLocal( "config", "kabcrc" ) );
          } else if ( family == "calendar" && QFile::exists( configDir + "/kcalrc" ) ) {
            info.mConfig = new KConfig( locateLocal( "config", "kcalrc" ) );
          } else {
            QString configFile = locateLocal( "config", QString( "kresources/%1/stdrc" ).arg( family ) );
            info.mConfig = new KConfig( configFile );
          }
          info.mManager->readConfig( info.mConfig );

          mInfoMap.append( info );
        }
      }
    }
  }
  mCurrentManager = 0;

  mFamilyCombo->insertStringList( mFamilyMap );

  int currentFamily = mConfig->readNumEntry( "CurrentFamily", 0 );
  mFamilyCombo->setCurrentItem( currentFamily );
  slotFamilyChanged( currentFamily );
}

void ConfigPage::save()
{
  saveResourceSettings();

  QValueList<ResourcePageInfo>::Iterator it;
  for ( it = mInfoMap.begin(); it != mInfoMap.end(); ++it )
    (*it).mManager->writeConfig( (*it).mConfig );

  emit changed( false );
}

void ConfigPage::defaults()
{
}

void ConfigPage::slotFamilyChanged( int pos )
{
  if ( pos < 0 || pos >= (int)mFamilyMap.count() )
    return;

  saveResourceSettings();

  mFamily = mFamilyMap[ pos ];

//US  qDebug("ConfigPage::slotFamilyChanged 4 family=%s", mFamily.latin1());

  mCurrentManager = mInfoMap[ pos ].mManager;
  mCurrentConfig = mInfoMap[ pos ].mConfig;

  if ( !mCurrentManager )
    kdDebug(5650) << "ERROR: cannot create ResourceManager<Resource>( mFamily )" << endl;

  mListView->clear();

  if ( mCurrentManager->isEmpty() ) {
//US    qDebug("ConfigPage::slotFamilyChanged 4.1 mCurrentManager=%ul", mCurrentManager );

    defaults();
  }

  Resource *standardResource = mCurrentManager->standardResource();

//US    qDebug("ConfigPage::slotFamilyChanged 4.4 resourcename=%s", standardResource->resourceName().latin1());


  Manager<Resource>::Iterator it;
  for ( it = mCurrentManager->begin(); it != mCurrentManager->end(); ++it ) {
    ConfigViewItem *item = new ConfigViewItem( mListView, *it );
    if ( *it == standardResource )
      item->setStandard( true );
  }

  if ( mListView->childCount() == 0 ) {
//US    qDebug("ConfigPage::slotFamilyChanged 4.5 ");

    defaults();
    emit changed( true );
    mCurrentManager->writeConfig( mCurrentConfig );
  } else {
//US    qDebug("ConfigPage::slotFamilyChanged 4.6 ");

    if ( !standardResource ) {
      KMessageBox::sorry( this, i18n( "There is no standard resource!<br> Please select one." ) );

//US    qDebug("ConfigPage::slotFamilyChanged 4.7" );

    }

    emit changed( false );
  }
}

void ConfigPage::slotAdd()
{
  if ( !mCurrentManager )
    return;

  QStringList types = mCurrentManager->resourceTypeNames();
  QStringList descs = mCurrentManager->resourceTypeDescriptions();
  bool ok = false;

  QString desc;

  desc = QInputDialog::getItem( i18n( "Resource Configuration" ),
                    i18n( "Select type of the new resource:" ), descs, 0,
                    false, &ok, this );

  if ( !ok )
    return;

  QString type = types[ descs.findIndex( desc ) ];

  // Create new resource
  Resource *resource = mCurrentManager->createResource( type );
  if ( !resource ) {
    KMessageBox::error( this, i18n("Unable to create resource of type '%1'.")
                              .arg( type ) );
    return;
  }

  resource->setResourceName( type + "-resource" );

  ConfigDialog dlg( this, mFamily, resource, "KRES::ConfigDialog" );

  if ( dlg.exec() ) {
    mCurrentManager->add( resource );

    ConfigViewItem *item = new ConfigViewItem( mListView, resource );

    mLastItem = item;

    // if there are only read-only resources we'll set this resource
    // as standard resource
    if ( !resource->readOnly() ) {
      bool onlyReadOnly = true;
      QListViewItem *it = mListView->firstChild();
      while ( it != 0 ) {
        ConfigViewItem *confIt = static_cast<ConfigViewItem*>( it );
        if ( !confIt->readOnly() && confIt != item )
          onlyReadOnly = false;

        it = it->itemBelow();
      }

      if ( onlyReadOnly )
        item->setStandard( true );
    }

    emit changed( true );
  } else {
    delete resource;
    resource = 0;
  }
}

void ConfigPage::slotRemove()
{
  if ( !mCurrentManager )
    return;

  QListViewItem *item = mListView->currentItem();
  ConfigViewItem *confItem = static_cast<ConfigViewItem*>( item );

  if ( !confItem )
    return;

  if ( confItem->standard() ) {
    KMessageBox::sorry( this, i18n( "You cannot remove your standard resource!\n Please select a new standard resource first." ) );
    return;
  }

  mCurrentManager->remove( confItem->resource() );

  if ( item == mLastItem )
    mLastItem = 0;

  mListView->takeItem( item );
  delete item;

  emit changed( true );
}

void ConfigPage::slotEdit()
{
  if ( !mCurrentManager )
    return;

  QListViewItem *item = mListView->currentItem();
  ConfigViewItem *configItem = static_cast<ConfigViewItem*>( item );
  if ( !configItem )
    return;

  Resource *resource = configItem->resource();

  ConfigDialog dlg( this, mFamily, resource, "KRES::ConfigDialog" );

  if ( dlg.exec() ) {
    configItem->setText( 0, resource->resourceName() );
    configItem->setText( 1, resource->type() );

    if ( configItem->standard() && configItem->readOnly() ) {
      KMessageBox::sorry( this, i18n( "You cannot use a read-only<br> resource as standard!" ) );
      configItem->setStandard( false );
    }

    mCurrentManager->resourceChanged( resource );
    emit changed( true );
  }
}

void ConfigPage::slotStandard()
{
  if ( !mCurrentManager )
    return;

  ConfigViewItem *item = static_cast<ConfigViewItem*>( mListView->currentItem() );
  if ( !item )
    return;

  if ( item->readOnly() ) {
    KMessageBox::sorry( this, i18n( "You cannot use a read-only<br>resource as standard!" ) );
    return;
  }

  if ( !item->isOn() ) {
    KMessageBox::sorry( this, i18n( "You cannot use an inactive<br>resource as standard!" ) );
    return;
  }

  QListViewItem *it = mListView->firstChild();
  while ( it != 0 ) {
    ConfigViewItem *configItem = static_cast<ConfigViewItem*>( it );
    if ( configItem->standard() )
      configItem->setStandard( false );
    it = it->itemBelow();
  }

  item->setStandard( true );
  mCurrentManager->setStandardResource( item->resource() );
   emit changed( true );

}

void ConfigPage::slotSelectionChanged()
{
  bool state = ( mListView->currentItem() != 0 );

  mRemoveButton->setEnabled( state );
  mEditButton->setEnabled( state );
  mStandardButton->setEnabled( state );
}

void ConfigPage::resourceAdded( Resource* resource )
{
  qDebug("ConfigPage::resourceAdded  : %s", resource->resourceName().latin1());
  ConfigViewItem *item = new ConfigViewItem( mListView, resource );

  // FIXME: this sucks. This should be in the config file,
  // or application-dependent, in which case it's always Off
  item->setOn( false );

  mLastItem = item;

  emit changed( true );
}

void ConfigPage::resourceModified( Resource* resource )
{
  qDebug("ConfigPage::resourceModified  : %s", resource->resourceName().latin1());
}

void ConfigPage::resourceDeleted( Resource* resource )
{
  qDebug("ConfigPage::resourceDeleted  : %s", resource->resourceName().latin1());
}

void ConfigPage::slotItemClicked( QListViewItem *item )
{
  ConfigViewItem *configItem = static_cast<ConfigViewItem *>( item );
  if ( !configItem ) return;

  if ( configItem->standard() && !configItem->isOn() ) {
    KMessageBox::sorry( this, i18n( "You cannot deactivate the<br>standard resource. Choose<br>another standard resource first." ) );
    configItem->setOn( true );
    return;
  }

  if ( configItem->isOn() != configItem->resource()->isActive() ) {
    emit changed( true );
  }
}

void ConfigPage::saveResourceSettings()
{
    //qDebug("ConfigPage::saveResourceSettings() begin");

  if ( mCurrentManager ) {

    QListViewItem *item = mListView->firstChild();
    while ( item ) {
      ConfigViewItem *configItem = static_cast<ConfigViewItem*>( item );

      // check if standard resource
      if ( configItem->standard() && !configItem->readOnly() &&
           configItem->isOn() ) {

        mCurrentManager->setStandardResource( configItem->resource() );
      }

      // check if active or passive resource
      configItem->resource()->setActive( configItem->isOn() );

      item = item->nextSibling();
    }
    mCurrentManager->writeConfig( mCurrentConfig );

    if ( !mCurrentManager->standardResource() )
      KMessageBox::sorry( this, i18n( "There is no valid standard resource!<br>Please select one which is neither read-only nor inactive." ) );
  }

  //qDebug("ConfigPage::saveResourceSettings() end");

}

//US #include "configpage.moc"

