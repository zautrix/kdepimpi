/*
    This file is part of KABC for platform independent KDE tools.
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
$Id: syncprefwidget.cpp,v 1.2 2004/08/04 22:45:22 ulf69 Exp $
*/


#include <qlayout.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qcombobox.h>


#include <klocale.h>
#include <kdialog.h>

#include "ksyncprofile.h"
#include "resource.h"
#include "syncprefwidget.h"


SyncPrefWidget_Settings::SyncPrefWidget_Settings(QWidget *parent, const char *name) :
  KRES::SyncWidget(parent, name)
{
  init();
}


SyncPrefWidget_Settings::~SyncPrefWidget_Settings()
{
}

void SyncPrefWidget_Settings::init()
{
  QGridLayout *topLayout = new QGridLayout( this, 6,2 );
  topLayout->setMargin( KDialog::marginHint() );
  topLayout->setSpacing( KDialog::spacingHint() );

  int iii = 0;
  mIncludeInRing = new QCheckBox( i18n("Include in multiple sync"), this   );
  topLayout->addMultiCellWidget(mIncludeInRing, iii,iii,0,1);
  ++iii;

  mAskForPreferences = new QCheckBox( i18n("Ask for preferences before sync"), this   );
  topLayout->addMultiCellWidget(mAskForPreferences, iii,iii,0,1);
  ++iii;

  mShowSummaryAfterSync = new QCheckBox( i18n("Show summary after sync"), this   );
  topLayout->addMultiCellWidget(mShowSummaryAfterSync, iii,iii,0,1);
  ++iii;

  mWriteBackExisting= new QCheckBox( i18n("Write back existing entries only"), this   );
  topLayout->addMultiCellWidget(mWriteBackExisting, iii,iii,0,1);
  ++iii;

  mWriteBackFile = new QCheckBox( i18n("Write back synced data"), this   );
  topLayout->addMultiCellWidget(mWriteBackFile, iii,iii,0,1);
  ++iii;

}

void SyncPrefWidget_Settings::loadSettings( KRES::Resource *res )
{
  KABC::Resource* resource = (KABC::Resource*)( res );
  KSyncProfile* prof = resource->getSyncProfile();

  if (prof == 0)
  {
    qDebug("SyncPrefWidget_Settings::loadSettings: something is wrong, because resource does have no syncprofile" );
    return;
  }

  mIncludeInRing->setChecked( prof->getIncludeInRingSync() );
  mAskForPreferences->setChecked( prof->getAskForPreferences());
  mShowSummaryAfterSync->setChecked( prof->getShowSummaryAfterSync());
  mWriteBackExisting->setChecked( prof->getWriteBackExisting() );
  mWriteBackFile->setChecked( prof->getWriteBackFile());
}

void SyncPrefWidget_Settings::saveSettings( KRES::Resource *res )
{
  KABC::Resource* resource = (KABC::Resource*)( res );
  KSyncProfile* prof = resource->getSyncProfile();

  if (prof == 0)
  {
    qDebug("SyncPrefWidget_Settings::saveSettings: something is wrong, because resource does have no syncprofile" );
    return;
  }

  prof->setIncludeInRingSync( mIncludeInRing->isChecked() );
  prof->setAskForPreferences( mAskForPreferences->isChecked());
  prof->setShowSummaryAfterSync( mShowSummaryAfterSync->isChecked() );
  prof->setWriteBackExisting(mWriteBackExisting->isChecked() );
  prof->setWriteBackFile( mWriteBackFile->isChecked());
}


/***********************************************************************
 *
 *
 **********************************************************************/

SyncPrefWidget_Conflict::SyncPrefWidget_Conflict(QWidget *parent, const char *name) :
  KRES::SyncWidget(parent, name)
{
  init();
}


SyncPrefWidget_Conflict::~SyncPrefWidget_Conflict()
{
}

void SyncPrefWidget_Conflict::init()
{
  QGridLayout *topLayout = new QGridLayout( this, 6,2 );
  topLayout->setMargin( KDialog::marginHint() );
  topLayout->setSpacing( KDialog::spacingHint() );

  int iii = 0;

  QButtonGroup* gr = new QButtonGroup ( 1,  Qt::Horizontal, i18n("In case of conflicts"), this);
  topLayout->addMultiCellWidget(gr, iii,iii,0,1);
  ++iii;
  loc = new QRadioButton ( i18n("Take local entry on conflict"), gr );
  rem = new QRadioButton ( i18n("Take remote entry on conflict"), gr );
  newest = new QRadioButton ( i18n("Take newest entry on conflict"), gr );
  ask = new QRadioButton ( i18n("Ask for every entry on conflict"), gr );
  f_loc= new QRadioButton ( i18n("Force: Take local entry always"), gr );
  f_rem = new QRadioButton ( i18n("Force: Take remote entry always"), gr );
   // both = new QRadioButton ( i18n("Take both on conflict"), gr );
}

void SyncPrefWidget_Conflict::loadSettings( KRES::Resource *res )
{
  KABC::Resource* resource = (KABC::Resource*)( res );
  KSyncProfile* prof = resource->getSyncProfile();

  if (prof == 0)
  {
    qDebug("SyncPrefWidget_Conflict::loadSettings: something is wrong, because resource does have no syncprofile" );
    return;
  }

  switch ( prof->getSyncPrefs() ) {
  case 0:
      loc->setChecked( true);
      break;
  case 1:
      rem->setChecked( true );
      break;
  case 2:
      newest->setChecked( true);
      break;
  case 3:
      ask->setChecked( true);
      break;
  case 4:
      f_loc->setChecked( true);
      break;
  case 5:
      f_rem->setChecked( true);
      break;
  case 6:
      //both->setChecked( true);
      break;
  default:
      break;
  }
}

void SyncPrefWidget_Conflict::saveSettings( KRES::Resource *res )
{
  KABC::Resource* resource = (KABC::Resource*)( res );
  KSyncProfile* prof = resource->getSyncProfile();

  if (prof == 0)
  {
    qDebug("SyncPrefWidget_Conftlict::saveSettings: something is wrong, because resource does have no syncprofile" );
    return;
  }

  int syncprefs = rem->isChecked()*1+newest->isChecked()*2+  ask->isChecked()*3+  f_loc->isChecked()*4+  f_rem->isChecked()*5 ;//+  both->isChecked()*6 ;
  prof->setSyncPrefs( syncprefs );

}


/***********************************************************************
 *
 *
 **********************************************************************/

SyncPrefWidget_Remote::SyncPrefWidget_Remote(QWidget *parent, const char *name) :
  KRES::SyncWidget(parent, name)
{
  init();
}


SyncPrefWidget_Remote::~SyncPrefWidget_Remote()
{
}

void SyncPrefWidget_Remote::init()
{
  QLabel * lab;
  QPushButton* button;


  QGridLayout *topLayout = new QGridLayout( this, 6,2 );
  topLayout->setMargin( KDialog::marginHint() );
  topLayout->setSpacing( KDialog::spacingHint() );

  int iii = 0;

  QButtonGroup* gr;

  QButtonGroup *proGr = new QButtonGroup ( 1,  Qt::Horizontal, i18n("Profile kind"), this);
  gr = proGr;
  topLayout->addMultiCellWidget(gr, iii,iii,0,1);
  ++iii;
  mProfileKind = new QComboBox( gr );

  mProfileKind->insertItem( i18n("Local file"), Local );
  mProfileKind->insertItem( i18n("Remote file (w down/upload cmd)"),Remote );
  mProfileKind->insertItem( i18n("Mobile device (cell phone)"), Mobile );

  connect( mProfileKind, SIGNAL( activated( int ) ),
           this, SLOT (kindChanged( int ) ) );


  QHBox* temphb;

  remoteFileWidget = new QVBox( this);
  topLayout->addMultiCellWidget(remoteFileWidget, iii,iii,0,1);
  ++iii;
  lab = new QLabel( i18n("Pre sync (download) command:"), remoteFileWidget);
  mRemotePrecommand = new QLineEdit(remoteFileWidget);

  lab = new QLabel( i18n("Local temp file (=Location):"), remoteFileWidget);
  mLocalTempFile = new QLineEdit(remoteFileWidget);
  mLocalTempFile->setEnabled(false);


  lab = new QLabel( i18n("Post sync (upload) command:"), remoteFileWidget);
  mRemotePostcommand = new QLineEdit(remoteFileWidget);

  lab = new QLabel( i18n("Fill in default values for:"), remoteFileWidget);
  temphb = new QHBox( remoteFileWidget );
  button = new QPushButton( i18n("ssh/scp"), temphb );
  connect ( button, SIGNAL( clicked()), this, SLOT (fillSSH() ) );
  button = new QPushButton( i18n("ftp"), temphb );
  connect ( button, SIGNAL( clicked()), this, SLOT (fillFTP() ) );
  lab = new QLabel( i18n("Hint: Use $PWD$ for placeholder of password!"), remoteFileWidget);

}

void SyncPrefWidget_Remote::loadSettings( KRES::Resource *res )
{
  KABC::Resource* resource = (KABC::Resource*)( res );
  KSyncProfile* prof = resource->getSyncProfile();

  if (prof == 0)
  {
    qDebug("SyncPrefWidget_Remote::loadSettings: something is wrong, because resource does have no syncprofile" );
    return;
  }

  int val;

  if (prof->getIsPhoneSync())
  	val = Mobile;
  else if (!prof->getIsLocalFileSync())
  	val = Remote;
  else // default value
  	val = Local;

  mProfileKind->setCurrentItem( val );
  kindChanged( val );


  mRemotePrecommand->setText(prof->getPreSyncCommand());
  mLocalTempFile->setText(resource->fileName());
  mRemotePostcommand->setText(prof->getPostSyncCommand());
}

void SyncPrefWidget_Remote::saveSettings( KRES::Resource *res )
{
  KABC::Resource* resource = (KABC::Resource*)( res );
  KSyncProfile* prof = resource->getSyncProfile();

  if (prof == 0)
  {
    qDebug("SyncPrefWidget_Remote::saveSettings: something is wrong, because resource does have no syncprofile" );
    return;
  }

  int val = mProfileKind->currentItem();

  if (val == Mobile)
  {
    prof->setIsLocalFileSync(true);
    prof->setIsPhoneSync(true);
  }
  else if (val == Remote)
  {
    prof->setIsLocalFileSync(false);
    prof->setIsPhoneSync(false);
  }
  else // default value is Local
  {
    prof->setIsLocalFileSync(true);
    prof->setIsPhoneSync(false);
  }


  prof->setPreSyncCommand( mRemotePrecommand->text());
//US  prof->setLocalTempFile( mLocalTempFile->text());
  prof->setPostSyncCommand( mRemotePostcommand->text() );
}


void SyncPrefWidget_Remote::fillSSH()
{
  QString location = mLocalTempFile->text();

    mRemotePrecommand->setText("scp zaurus@192.168.0.65:/home/zaurus/kdepim/"+location + " " + location );
//US    mLocalTempFile->setText("/tmp/mycalendar.ics" );
    mRemotePostcommand->setText("scp " + location + " zaurus@192.168.0.65:/home/zaurus/kdepim/"+location );
}

void SyncPrefWidget_Remote::fillFTP()
{
  QString location = mLocalTempFile->text();

    mRemotePrecommand->setText("cd /tmp;ftp ftp://zaurus:a@192.168.0.65/kdepim/" + location );
//US    mLocalTempFile->setText("/tmp/mycalendar.ics" );
    mRemotePostcommand->setText("ftp -u ftp://zaurus:a@192.168.0.65/kdepim/"+location+ " " + location);

}


void SyncPrefWidget_Remote::kindChanged( int newKind )
{

  if (newKind == Mobile)
    remoteFileWidget->setEnabled(false);
  else if (newKind == Remote)
    remoteFileWidget->setEnabled(true);
  else // default value is local
    remoteFileWidget->setEnabled(false);

}

/***********************************************************************
 *
 *
 **********************************************************************/


SyncPrefWidgetContainer::SyncPrefWidgetContainer()
  : SyncWidgetContainer()
{
}

KRES::SyncWidget* SyncPrefWidgetContainer::generateSettingsTab(QWidget *parent = 0, const char *name = 0)
{
  return new SyncPrefWidget_Settings(parent, name);
}

KRES::SyncWidget* SyncPrefWidgetContainer::generateConflictsTab(QWidget *parent = 0, const char *name = 0)
{
  return new SyncPrefWidget_Conflict(parent, name);
}

KRES::SyncWidget* SyncPrefWidgetContainer::generateRemoteTab(QWidget *parent = 0, const char *name = 0)
{
  return new SyncPrefWidget_Remote(parent, name);
}

