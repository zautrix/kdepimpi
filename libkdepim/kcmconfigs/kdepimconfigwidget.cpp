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

$Id: kdepimconfigwidget.cpp,v 1.31 2010/01/25 22:45:27 zautrix Exp $
*/
#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else


#include <qgroupbox.h>
#endif


#include <qlayout.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qvbox.h>
#include <qdir.h>
#include <qregexp.h>
#include <qspinbox.h>

#include <kdialog.h>
#include <kprefsdialog.h>
#include <klocale.h>
#include <kglobalsettings.h>
#include <kdateedit.h>
#include <kglobal.h>
#include <stdlib.h>

/*US
#include <qcheckbox.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfile.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klistview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#ifndef KAB_EMBEDDED
#include <ktrader.h>
#else // KAB_EMBEDDED
#include <mergewidget.h>
#include <distributionlistwidget.h>
#endif // KAB_EMBEDDED

#include "addresseewidget.h"
#include "extensionconfigdialog.h"
#include "extensionwidget.h"
*/

#include "qapplication.h"

#include "kpimglobalprefs.h"

#include "kdepimconfigwidget.h"
#include <kprefs.h>
#include <kmessagebox.h>



KDEPIMConfigWidget::KDEPIMConfigWidget(KPimGlobalPrefs *prefs, QWidget *parent, const char *name )
  : KPrefsWidget(prefs, parent, name )
{
  mExternalAppsMap.insert(ExternalAppHandler::EMAIL, i18n("Email"));
  mExternalAppsMap.insert(ExternalAppHandler::PHONE, i18n("Phone"));
  mExternalAppsMap.insert(ExternalAppHandler::SMS, i18n("SMS"));
  mExternalAppsMap.insert(ExternalAppHandler::FAX, i18n("Fax"));
  mExternalAppsMap.insert(ExternalAppHandler::PAGER, i18n("Pager"));
  mExternalAppsMap.insert(ExternalAppHandler::SIP, i18n("SIP"));


  QVBoxLayout *topLayout = new QVBoxLayout( this, 0,
                                            KDialog::spacingHint() );

  tabWidget = new QTabWidget( this );
  topLayout->addWidget( tabWidget );


  setupLocaleTab();
  setupLocaleDateTab();
  setupTimeZoneTab();
  setupExternalAppTab();
  setupStoreTab();
  setupBackupTab();
}
void KDEPIMConfigWidget::showTimeZoneTab()
{
    tabWidget->setCurrentPage ( 3 ) ;
}
void KDEPIMConfigWidget::setupBackupTab()
{
    QVBox *colorPage = new QVBox( this );
    tabWidget->addTab( colorPage, i18n( "Backup" ) );
    QWidget* topFrame = new QWidget( colorPage ); 
    QVBoxLayout *topLayout = new QVBoxLayout(topFrame);
    KPrefsWidBool *sb = addWidBool(i18n("Backup enabled"),
                      &(KPimGlobalPrefs::instance()->mBackupEnabled),topFrame);
    topLayout->addWidget((QWidget*)sb->checkBox());
    QWidget* bupFrame = new QWidget( topFrame ); 
    topLayout->addWidget((bupFrame));
    QObject::connect ( sb->checkBox(), SIGNAL (toggled ( bool ) ), bupFrame, SLOT ( setEnabled( bool ) ) );
    QVBoxLayout *bupLayout = new QVBoxLayout(bupFrame);
    sb = addWidBool(i18n("Use standard backup dir"),
                      &(KPimGlobalPrefs::instance()->mBackupUseDefaultDir),bupFrame);
    bupLayout->addWidget((QWidget*)sb->checkBox());
    mBackupUrl = new KURLRequester( bupFrame );
    mBackupUrl->setPathIsDir();
    mBackupUrl->setURL( KGlobalSettings::backupDataDir() );
    QObject::connect ( sb->checkBox(), SIGNAL (toggled ( bool ) ), mBackupUrl ,SLOT ( setDisabled( bool ) ) );
    bupLayout->addWidget( mBackupUrl );

    mBackupUrl->setEnabled( !KPimGlobalPrefs::instance()->mBackupUseDefaultDir );
    bupFrame->setEnabled( KPimGlobalPrefs::instance()->mBackupEnabled );
    QHBox *dummy = new QHBox(bupFrame);
    new QLabel(i18n("Number of Backups:"),dummy);
    mBackupNumbersSpin = new QSpinBox(1,21,1,dummy);
    new QLabel(i18n(" "),dummy);
    bupLayout->addWidget( dummy );

    dummy = new QHBox(bupFrame);
    new QLabel(i18n("Make backup every "),dummy);
    mBackupDayCountSpin = new QSpinBox(1,28,1,dummy);
    new QLabel(i18n(" days"),dummy);
    new QLabel(i18n(" "),dummy);
    bupLayout->addWidget( dummy ); 
    QString localKdeDir;
    localKdeDir = readEnvPath("LOCALMICROKDEHOME");
    if ( ! localKdeDir.isEmpty() ) {
        sb->checkBox()->setEnabled( false );
        sb->checkBox()->setChecked( true );
        mBackupUrl->setEnabled( false );
        KPimGlobalPrefs::instance()->mBackupUseDefaultDir = true;
    }

}
void KDEPIMConfigWidget::setupStoreTab()
{
    QVBox *colorPage = new QVBox( this );
    tabWidget->addTab( colorPage, i18n( "Colors" ) );
    QWidget* cw = new QWidget( colorPage ); 
    KPrefsWidColor *holidayColor =
      addWidColor(i18n("Alternating background of list views"),
                  &(KPimGlobalPrefs::instance()->mAlternateColor),cw);
    QHBoxLayout *topLayout = new QHBoxLayout(cw);
    topLayout->addWidget(holidayColor->label());
    topLayout->addWidget( (QWidget* )holidayColor->button());


    QVBox *storePage = new QVBox( this ); 
    if (  QApplication::desktop()->height() > 240 )
    new QLabel( i18n("Your current storage dir is:\n%1\nYour mail is stored in:\n(storagedir)/apps/kopiemail/localmail").arg(KGlobal::dirs()->localkdedir()), storePage  );
    new QLabel( i18n("<b>New data storage dir:</b>"), storePage );
    mStoreUrl = new KURLRequester( storePage );
    mStoreUrl->setPathIsDir();
    mStoreUrl->setURL( KGlobal::dirs()->localkdedir() );
#ifdef DESKTOP_VERSION     
    QString confFile = qApp->applicationDirPath ()+ "/.microkdehome" ;
    QFileInfo fi ( confFile );
    if ( fi.exists() ) {
        KConfig cfg (   confFile );
        cfg.setGroup("Global");
        QString localKdeDir = cfg.readEntry( "MICROKDEHOME", "x_x_x" );
        if ( localKdeDir != "x_x_x" ) {
            mStoreUrl->setURL( localKdeDir );
            qDebug("Reading config from %s ", confFile.latin1());
        }
    }
               
#endif 
    new QLabel( i18n("New dirs are created automatically"), storePage  );
    QHBox *bb = new QHBox( storePage );
    QPushButton * pb;
    if ( QApplication::desktop()->width() < 640 )
        pb = new QPushButton ( i18n("Save"), bb  );
    else
        pb = new QPushButton ( i18n("Save settings"), bb  );
    connect(pb, SIGNAL( clicked() ), this, SLOT ( saveStoreSettings() ) );
    pb = new QPushButton ( i18n("Save standard"), bb  );
    connect(pb, SIGNAL( clicked() ), this, SLOT ( setStandardStore() ) );
#ifdef DESKTOP_VERSION
    pb = new QPushButton ( i18n("Save using LOCAL storage"), bb  );
    connect(pb, SIGNAL( clicked() ), this, SLOT ( setLocalStore() ) );
#endif
    new QLabel( i18n("<b>New settings are used\nafter a restart</b>"), storePage );
    mDataStoragePath = new QLabel( i18n("Settings are stored in\n%1").arg(QDir::homeDirPath() + "/.microkdehome" ), storePage  );
    tabWidget->addTab( storePage, i18n( "Data storage path" ) );

#ifdef DESKTOP_VERSION  
      if ( mStoreUrl->url().startsWith( "LOCAL:" ) ) {
	mDataStoragePath->setText( i18n("Settings are stored in\n%1").arg( qApp->applicationDirPath ()+"/.microkdehome" ));
      }
#endif
}
void KDEPIMConfigWidget::setLocalStore()
{
    mStoreUrl->setURL(  "LOCAL:kdepimpi"  );
    saveStoreSettings();
    QString message = i18n("'LOCAL' mode makes is possible to run\nKA/Pi and KO/Pi from a USB memory stick.\nIn LOCAL mode the data is stored\nin a path relative to the executable.\nNote, that in LOCAL mode only addressbook\nresource files in\n <path of the executable>/<dirname after LOCAL:>/apps/kabc/*.vcf\n are supported.\nIf you use the standard addressbook settings\nyou do not have to reconfigure any path,\njust restart the application and import\nyour addressbook and calendar data.");
    KMessageBox::information( this, message);
}
void KDEPIMConfigWidget::setStandardStore()
{
    mStoreUrl->setURL(  QDir::homeDirPath() + "/kdepim"  );
    saveStoreSettings();
}
void KDEPIMConfigWidget::saveStoreSettings()
{
#ifdef DESKTOP_VERSION
        if ( !mStoreUrl->url().startsWith( "LOCAL:" ) ) {
            QString file = qApp->applicationDirPath ()+"/.microkdehome";
            QFileInfo fi ( file );
            if ( fi.exists() ) {
                bool res = QFile::remove( file );
                if ( ! res )
                    KMessageBox::information( this, i18n("ERROR: Cannot remove file\n%1\nPlease remove it manually.").arg( file ));
            }
        }
#endif
    if ( !mStoreUrl->url().isEmpty() ) {
        QString path = QDir::homeDirPath();
        QString url = mStoreUrl->url();
#ifdef DESKTOP_VERSION
        if ( url.startsWith( "LOCAL:" ) ) {
            path = qApp->applicationDirPath () ;
        }
#endif
        KConfig cfg (   path + "/.microkdehome" );
        cfg.setGroup("Global");
        cfg.writeEntry( "MICROKDEHOME", url ); 
        qDebug("cfg.writeEntry( MICROKDEHOME, %s  ", url.latin1());
        cfg.sync();
	mDataStoragePath->setText( i18n("Settings are stored in\n%1").arg( path+"/.microkdehome" ));
    } else {
        mStoreUrl->setURL(  QDir::homeDirPath() + "/kdepim"  );
        saveStoreSettings();
    }
}
void KDEPIMConfigWidget::setupExternalAppTab()
{
  QWidget *externalAppsPage = new QWidget( this );
  QVBoxLayout* layout = new QVBoxLayout( externalAppsPage, KDialog::marginHintSmall(),
                                            KDialog::spacingHintSmall() );

  mExternalApps = new QComboBox( externalAppsPage );

  QMap<ExternalAppHandler::Types, QString>::Iterator it;
  for( it = mExternalAppsMap.begin(); it != mExternalAppsMap.end(); ++it )
    mExternalApps->insertItem( it.data(), it.key() );

  layout->addWidget( mExternalApps );

  connect( mExternalApps, SIGNAL( activated( int ) ),
           this, SLOT (externalapp_changed( int ) ) );


  mExternalAppGroupBox = new QGroupBox( 0, Qt::Vertical, i18n( "Used Mail Client" ), externalAppsPage );
  QGridLayout *boxLayout = new QGridLayout( mExternalAppGroupBox->layout(), 4, 2,    -1, "gridlayout" );
  mExternalAppGroupBox->layout()->setMargin(4);

  mClient = new QComboBox( mExternalAppGroupBox );
  boxLayout->addMultiCellWidget( mClient, 0, 0, 0, 1 );

  connect( mClient, SIGNAL( activated( int ) ),
           this, SLOT (client_changed( int ) ) );

  QLabel* lab = new QLabel( i18n("Channel:"), mExternalAppGroupBox);
  boxLayout->addWidget( lab, 1, 0 );
  mChannel = new QLineEdit(mExternalAppGroupBox);
  mChannel->setReadOnly(true);
  boxLayout->addMultiCellWidget( mChannel, 2 , 2, 0, 1 );

  lab = new QLabel( i18n("Message:"), mExternalAppGroupBox);
  boxLayout->addWidget( lab, 3, 0 );
  mMessage = new QLineEdit(mExternalAppGroupBox);
  mMessage->setReadOnly(true);
  boxLayout->addWidget( mMessage , 4, 0);

  lab = new QLabel( i18n("Parameters:"), mExternalAppGroupBox);
  boxLayout->addWidget( lab, 3, 1 );
  mParameters = new QLineEdit(mExternalAppGroupBox);
  mParameters->setReadOnly(true);
  boxLayout->addWidget( mParameters, 4, 1 );
      lab = new QLabel( i18n("HINT: Delimiter=; Name=%1,Email=%2"), mExternalAppGroupBox);
      boxLayout->addMultiCellWidget( lab, 5, 5, 0, 1 );


  if (  QApplication::desktop()->height() > 240 ) {
      lab = new QLabel( i18n("extra Message:"), mExternalAppGroupBox);
      boxLayout->addWidget( lab, 6, 0 );
      mMessage2 = new QLineEdit(mExternalAppGroupBox);
      mMessage2->setReadOnly(true);
      boxLayout->addWidget( mMessage2 , 7, 0);

      lab = new QLabel( i18n("extra Parameters:"), mExternalAppGroupBox);
      boxLayout->addWidget( lab, 6, 1 );
      mParameters2 = new QLineEdit(mExternalAppGroupBox);
      mParameters2->setReadOnly(true);
      boxLayout->addWidget( mParameters2, 7, 1 );

      lab = new QLabel( i18n("HINT: Emails=%1,Attachments=%2"), mExternalAppGroupBox);
      boxLayout->addMultiCellWidget( lab, 8, 8, 0, 1 );
      connect( mMessage2, SIGNAL( textChanged ( const QString & )), this, SLOT( textChanged ( const QString & ))  );
      connect( mParameters2, SIGNAL( textChanged ( const QString & )), this, SLOT( textChanged ( const QString & ))  );
  } else {
      mMessage2 = 0;
      mParameters2 = 0;
  }

  connect( mChannel, SIGNAL( textChanged ( const QString & )), this, SLOT( textChanged ( const QString & ))  );
  connect( mMessage, SIGNAL( textChanged ( const QString & )), this, SLOT( textChanged ( const QString & ))  );
  connect( mParameters, SIGNAL( textChanged ( const QString & )), this, SLOT( textChanged ( const QString & ))  );


  layout->addWidget( mExternalAppGroupBox );
  tabWidget->addTab( externalAppsPage, i18n( "External Apps." ) );

}

  
void KDEPIMConfigWidget::setupLocaleDateTab()
{
  QWidget *topFrame = new QWidget( this );
  QGridLayout *topLayout = new QGridLayout( topFrame, 3, 2);

   topLayout->setSpacing(KDialog::spacingHintSmall());
   topLayout->setMargin(KDialog::marginHintSmall());  
   int iii = 0;

   
   KPrefsWidRadios *syncPrefsGroup =
       addWidRadios(i18n("Date Format:"),&(KPimGlobalPrefs::instance()->mPreferredDate),topFrame);
   QString format;
   if ( QApplication::desktop()->width() < 480 )
       format = "(%d.%m.%Y)";
   else 
       format = "(%d.%m.%Y|%A %d %B %Y)";
   syncPrefsGroup->addRadio(i18n("24.03.2004 "+format));
   if ( QApplication::desktop()->width() < 480 )
       format = "(%m.%d.%Y)";
   else 
       format = "(%m.%d.%Y|%A %B %d %Y)";
   syncPrefsGroup->addRadio(i18n("03.24.2004 "+format));
   if ( QApplication::desktop()->width() < 480 )
       format = "(%Y-%m-%d)";
   else 
       format = "(%Y-%m-%d|%A %Y %B %d)";
   syncPrefsGroup->addRadio(i18n("2004-03-24 "+format));
   syncPrefsGroup->addRadio(i18n("User defined"));
   if ( QApplication::desktop()->width() < 480 ) {
       syncPrefsGroup->groupBox()->layout()->setMargin( 5 );
       syncPrefsGroup->groupBox()->layout()->setSpacing( 0 );
   }  
   topLayout->addMultiCellWidget( (QWidget*)syncPrefsGroup->groupBox(),iii,iii,0,1);
   ++iii;   
   ++iii;
   QLabel * lab;
   mUserDateFormatLong = new QLineEdit(topFrame);
   lab = new QLabel(mUserDateFormatLong, i18n("User long date:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mUserDateFormatLong,iii,1);
   ++iii;
   mUserDateFormatShort = new QLineEdit(topFrame);
   lab = new QLabel(mUserDateFormatShort, i18n("User short date:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mUserDateFormatShort,iii,1);
   ++iii;
   lab = new QLabel( i18n("Monday 19 April 2004: %A %d %B %Y"), topFrame);
   topLayout->addMultiCellWidget(lab ,iii,iii,0,1);
   ++iii;
   //qDebug(" QApplication::desktop()->height()xx %d ",  QApplication::desktop()->height() ); 
   if (  QApplication::desktop()->height() > 240 ) {
       lab = new QLabel( i18n("Mon 19.04.04: %a %d.%m.%y"), topFrame);
       topLayout->addMultiCellWidget(lab ,iii,iii,0,1);
       ++iii;  
       lab = new QLabel( i18n("Mon, 19.Apr.04: %a, %d.%b.%y"), topFrame);
       topLayout->addMultiCellWidget(lab ,iii,iii,0,1);
       ++iii;
   }

  connect( mUserDateFormatLong, SIGNAL( textChanged ( const QString & )), this, SLOT( textChanged ( const QString & ))  );
  connect( mUserDateFormatShort, SIGNAL( textChanged ( const QString & )), this, SLOT( textChanged ( const QString & ))  );


   tabWidget->addTab( topFrame, i18n( "Date Format" ) );
}

void KDEPIMConfigWidget::setupLocaleTab()
{

  QWidget *topFrame = new QWidget( this );
   QGridLayout *topLayout = new QGridLayout(topFrame,4,2);

   topLayout->setSpacing(KDialog::spacingHint());
   topLayout->setMargin(KDialog::marginHint());  
   int iii = 0;
 KPrefsWidRadios *syncPrefsGroup =
      addWidRadios(i18n("Language:(needs restart)"),&(KPimGlobalPrefs::instance()->mPreferredLanguage),topFrame);
   syncPrefsGroup->addRadio(i18n("English"));
   syncPrefsGroup->addRadio(i18n("German")); 
   syncPrefsGroup->addRadio(i18n("French")); 
   syncPrefsGroup->addRadio(i18n("Italian")); 
   syncPrefsGroup->addRadio(i18n("User defined (usertranslation.txt)")); 
   if (  QApplication::desktop()->width() <  300   ) {
       syncPrefsGroup->groupBox()->layout()->setMargin( 5 );
       syncPrefsGroup->groupBox()->layout()->setSpacing( 0 );
   }
   topLayout->addMultiCellWidget( (QWidget*)syncPrefsGroup->groupBox(),iii,iii,0,1);
   ++iii;   
 

   tabWidget->addTab( topFrame, i18n( "Language" ) );
   topFrame = new QWidget( this );
   topLayout = new QGridLayout(topFrame,4,2);
   
   topLayout->setSpacing(KDialog::spacingHint());
   topLayout->setMargin(KDialog::marginHint());  
   iii = 0;
   syncPrefsGroup =
       addWidRadios(i18n("Time Format(nr):"),&(KPimGlobalPrefs::instance()->mPreferredTime),topFrame);
   if (  QApplication::desktop()->width() >  300   )
       syncPrefsGroup->groupBox()->setOrientation (Qt::Vertical);
   syncPrefsGroup->addRadio(i18n("24:00"));
   syncPrefsGroup->addRadio(i18n("12:00am"));
   syncPrefsGroup->groupBox()->setOrientation (Qt::Vertical);
   topLayout->addMultiCellWidget( syncPrefsGroup->groupBox(),iii,iii,0,1);
   ++iii;   

   KPrefsWidBool *sb = addWidBool(i18n("Week starts on Sunday"),
                      &(KPimGlobalPrefs::instance()->mWeekStartsOnSunday),topFrame);
   topLayout->addMultiCellWidget((QWidget*)sb->checkBox(), iii,iii,0,1);
   ++iii;   


   tabWidget->addTab( topFrame, i18n( "Time Format" ) );

}


void KDEPIMConfigWidget::setupTimeZoneTab()
{
    QWidget *topFrame;
    QGridLayout *topLayout ;

 




  topFrame = new QWidget( this );
  topLayout = new QGridLayout( topFrame, 5, 2);
  topLayout->setSpacing(KDialog::spacingHintSmall());
  topLayout->setMargin(KDialog::marginHintSmall());

  QHBox *timeZoneBox = new QHBox( topFrame );
  topLayout->addMultiCellWidget( timeZoneBox, 0, 0, 0, 1 );

  new QLabel( i18n("Timezone:"), timeZoneBox );
  mTimeZoneCombo = new QComboBox( timeZoneBox ); 
  if ( QApplication::desktop()->width() <  300 ) {
       mTimeZoneCombo->setMaximumWidth(150);
    }

  QStringList list;
  list = KGlobal::locale()->timeZoneList();
  mTimeZoneCombo->insertStringList(list);

    // find the currently set time zone and select it
  QString sCurrentlySet = KPimGlobalPrefs::instance()->mTimeZoneId;
  int nCurrentlySet = 11;
  for (int i = 0; i < mTimeZoneCombo->count(); i++)
    {
      if (mTimeZoneCombo->text(i) == sCurrentlySet)
        {
         nCurrentlySet = i;
         break;
        }
    }
  mTimeZoneCombo->setCurrentItem(nCurrentlySet);
  int iii = 1;
  KPrefsWidBool *sb =
      addWidBool(i18n("Add 30 min to selected Timezone"),
                 &(KPimGlobalPrefs::instance()->mTimeZoneAdd30min),topFrame);
  topLayout->addMultiCellWidget((QWidget*)sb->checkBox(), iii,iii,0,1);
  ++iii;  
  sb =
      addWidBool(i18n("Timezone has daylight saving"),
                 &(KPimGlobalPrefs::instance()->mUseDaylightsaving),topFrame);
  topLayout->addMultiCellWidget((QWidget*)sb->checkBox(), iii,iii,0,1);
  ++iii; 
  QLabel* lab; 

  lab = new QLabel(  i18n("Actual start and end is the\nsunday before this date."), topFrame );
  topLayout->addMultiCellWidget(lab, iii,iii,0,1);
  ++iii;
 
  lab = new QLabel(  i18n("The year in the date is ignored."), topFrame );
  topLayout->addMultiCellWidget(lab, iii,iii,0,1);
  ++iii; 
  lab = new QLabel(  i18n("Daylight start:"), topFrame );
  topLayout->addWidget(lab, iii,0);
  mStartDateSavingEdit = new KDateEdit(topFrame);
  topLayout->addWidget(mStartDateSavingEdit, iii,1);
  ++iii;

  lab = new QLabel(  i18n("Daylight end:"), topFrame );
  topLayout->addWidget(lab, iii,0);
  mEndDateSavingEdit = new KDateEdit(topFrame);
  topLayout->addWidget(mEndDateSavingEdit, iii,1);
  ++iii;
  QDate current ( 2001, 1,1);
  mStartDateSavingEdit->setDate(current.addDays(KPimGlobalPrefs::instance()->mDaylightsavingStart-1));
  mEndDateSavingEdit->setDate(current.addDays(KPimGlobalPrefs::instance()->mDaylightsavingEnd-1));

  connect( mStartDateSavingEdit, SIGNAL(  dateChanged(QDate)), this, SLOT( modified())  );
  connect( mEndDateSavingEdit, SIGNAL(  dateChanged(QDate)), this, SLOT( modified())  );
  connect( mTimeZoneCombo, SIGNAL( activated( int ) ), this, SLOT (modified() ) );
  tabWidget->addTab( topFrame, i18n( "Time Zone" ) );
 

  topFrame = new QWidget( this );
  topLayout = new QGridLayout( topFrame, 3, 2);
  topLayout->setSpacing(KDialog::spacingHintSmall());
  topLayout->setMargin(KDialog::marginHintSmall());
  tabWidget->addTab( topFrame, i18n( "Fonts" ) );

  QLabel* labb = new QLabel( i18n("Global application font for all apps:"),  topFrame );
  topLayout->addMultiCellWidget(labb,0,0,0,2);
  int i = 1;
  KPrefsWidFont *timeLabelsFont =
      addWidFont(i18n("Kx/Pi"),i18n("Application Font"),
                 &(KPimGlobalPrefs::instance()->mApplicationFont),topFrame);
  topLayout->addWidget(timeLabelsFont->label(),i,0);
  topLayout->addWidget(timeLabelsFont->preview(),i,1);
  topLayout->addWidget(timeLabelsFont->button(),i,2);
}

void KDEPIMConfigWidget::externalapp_changed( int newApp )
{
  // first store the current data
  saveEditFieldSettings();

  // set mCurrentApp
  mCurrentApp = (ExternalAppHandler::Types)newApp;

  // set mCurrentClient
  switch(mCurrentApp)
  {
    case(ExternalAppHandler::EMAIL):
      mCurrentClient = mEmailClient;
      break;
    case(ExternalAppHandler::PHONE):
      mCurrentClient = mPhoneClient;
      break;
    case(ExternalAppHandler::SMS):
      mCurrentClient = mSMSClient;
      break;
    case(ExternalAppHandler::FAX):
      mCurrentClient = mFaxClient;
      break;
    case(ExternalAppHandler::PAGER):
      mCurrentClient = mPagerClient;
      break;
    case(ExternalAppHandler::SIP):
      mCurrentClient = mSipClient;
      break;
    default:
      return;
  }

  // and at last update the widgets
  updateClientWidgets();
}



void KDEPIMConfigWidget::client_changed( int newClient )
{
  if (newClient == mCurrentClient)
    return;

  // first store the current data
  saveEditFieldSettings();


  //then reset the clientvariable
  mCurrentClient = newClient;

  // and at last update the widgets
  updateClientWidgets();

  KPrefsWidget::modified();
}

void KDEPIMConfigWidget::saveEditFieldSettings()
{

  switch(mCurrentApp)
  {
    case(ExternalAppHandler::EMAIL):
      mEmailClient = mClient->currentItem();
      break;
    case(ExternalAppHandler::PHONE):
      mPhoneClient= mClient->currentItem();
      break;
    case(ExternalAppHandler::SMS):
      mSMSClient = mClient->currentItem();
      break;
    case(ExternalAppHandler::FAX):
      mFaxClient = mClient->currentItem();
      break;
    case(ExternalAppHandler::PAGER):
      mPagerClient = mClient->currentItem();
      break;
    case(ExternalAppHandler::SIP):
      mSipClient = mClient->currentItem();
      break;
    default:
      return;
  }

  //store the current data back to the apropriate membervariables if we had set it to "other"
  if ((mCurrentApp == ExternalAppHandler::EMAIL) && (mCurrentClient == KPimGlobalPrefs::OTHER_EMC))
  {
    mEmailOtherChannel = mChannel->text();
    mEmailOtherMessage = mMessage->text();
    mEmailOtherMessageParameters = mParameters->text();
    if ( mMessage2 )
        mEmailOtherMessage2 = mMessage2->text();   
    if ( mParameters2 )
        mEmailOtherMessageParameters2 = mParameters2->text();
  }
  else if ((mCurrentApp == ExternalAppHandler::PHONE) && (mCurrentClient == KPimGlobalPrefs::OTHER_PHC))
  {
    mPhoneOtherChannel = mChannel->text();
    mPhoneOtherMessage = mMessage->text();
    mPhoneOtherMessageParameters = mParameters->text();
  }
  else if ((mCurrentApp == ExternalAppHandler::SMS) && (mCurrentClient == KPimGlobalPrefs::OTHER_SMC))
  {
    mSMSOtherChannel = mChannel->text();
    mSMSOtherMessage = mMessage->text();
    mSMSOtherMessageParameters = mParameters->text();
  }
  else if ((mCurrentApp == ExternalAppHandler::FAX) && (mCurrentClient == KPimGlobalPrefs::OTHER_FAC))
  {
    mFaxOtherChannel = mChannel->text();
    mFaxOtherMessage = mMessage->text();
    mFaxOtherMessageParameters = mParameters->text();
  }
  else if ((mCurrentApp == ExternalAppHandler::PAGER) && (mCurrentClient == KPimGlobalPrefs::OTHER_PAC))
  {
    mPagerOtherChannel = mChannel->text();
    mPagerOtherMessage = mMessage->text();
    mPagerOtherMessageParameters = mParameters->text();
  }
  else if ((mCurrentApp == ExternalAppHandler::SIP) && (mCurrentClient == KPimGlobalPrefs::OTHER_SIC))
  {
    mSipOtherChannel = mChannel->text();
    mSipOtherMessage = mMessage->text();
    mSipOtherMessageParameters = mParameters->text();
  }


}

void KDEPIMConfigWidget::updateClientWidgets()
{
  bool blocked = signalsBlocked();
  blockSignals( true );

  // at this point we assume, that mCurrentApp and mCurrentClient are set to the values that we want to display
  QMap<ExternalAppHandler::Types, QString>::Iterator it = mExternalAppsMap.find ( mCurrentApp );
  if (it == mExternalAppsMap.end())
    return;

  // update group box
  mExternalAppGroupBox->setTitle(i18n( "Used %1 Client" ).arg(it.data()));

  //update the entries in the client combobox
  mClient->clear();

  QList<DefaultAppItem> items = ExternalAppHandler::instance()->getAvailableDefaultItems(mCurrentApp);
  DefaultAppItem* dai;
  for ( dai=items.first(); dai != 0; dai=items.next() )
  {
    mClient->insertItem( i18n(dai->_label), dai->_id );

    if (dai->_id == mCurrentClient)
    {
      //restore the edit fields with the data of the local membervariables if we had set it to "other".
      //Otherwise take the default data from externalapphandler.
      mChannel->setText(dai->_channel);
      mMessage->setText(dai->_message);
      mParameters->setText(dai->_parameters);  
      if ( mMessage2 )
          mMessage2->setText(dai->_message2); 
    if ( mParameters2 )
      mParameters2->setText(dai->_parameters2);


      if ((mCurrentApp == ExternalAppHandler::EMAIL) && (mCurrentClient == KPimGlobalPrefs::OTHER_EMC))
      {
        mChannel->setText(mEmailOtherChannel);
        mMessage->setText(mEmailOtherMessage);
        mParameters->setText(mEmailOtherMessageParameters); 
        if ( mMessage2 )
        mMessage2->setText(mEmailOtherMessage2);
    if ( mParameters2 )
        mParameters2->setText(mEmailOtherMessageParameters2);
      }
      else if ((mCurrentApp == ExternalAppHandler::PHONE) && (mCurrentClient == KPimGlobalPrefs::OTHER_PHC))
      {
        mChannel->setText(mPhoneOtherChannel);
        mMessage->setText(mPhoneOtherMessage);
        mParameters->setText(mPhoneOtherMessageParameters);
      }
      else if ((mCurrentApp == ExternalAppHandler::SMS) && (mCurrentClient == KPimGlobalPrefs::OTHER_SMC))
      {
        mChannel->setText(mSMSOtherChannel);
        mMessage->setText(mSMSOtherMessage);
        mParameters->setText(mSMSOtherMessageParameters);
      }
      else if ((mCurrentApp == ExternalAppHandler::FAX) && (mCurrentClient == KPimGlobalPrefs::OTHER_FAC))
      {
        mChannel->setText(mFaxOtherChannel);
        mMessage->setText(mFaxOtherMessage);
        mParameters->setText(mFaxOtherMessageParameters);
      }
      else if ((mCurrentApp == ExternalAppHandler::PAGER) && (mCurrentClient == KPimGlobalPrefs::OTHER_PAC))
      {
        mChannel->setText(mPagerOtherChannel);
        mMessage->setText(mPagerOtherMessage);
        mParameters->setText(mPagerOtherMessageParameters);
      }
      else if ((mCurrentApp == ExternalAppHandler::SIP) && (mCurrentClient == KPimGlobalPrefs::OTHER_SIC))
      {
        mChannel->setText(mSipOtherChannel);
        mMessage->setText(mSipOtherMessage);
        mParameters->setText(mSipOtherMessageParameters);
      }
    }

  }

  bool readonly;
  bool enabled;
  if ( ((mCurrentApp == ExternalAppHandler::EMAIL) && (mCurrentClient == KPimGlobalPrefs::OTHER_EMC))
     ||((mCurrentApp == ExternalAppHandler::PHONE) && (mCurrentClient == KPimGlobalPrefs::OTHER_PHC))
     ||((mCurrentApp == ExternalAppHandler::SMS) && (mCurrentClient == KPimGlobalPrefs::OTHER_SMC))
     ||((mCurrentApp == ExternalAppHandler::FAX) && (mCurrentClient == KPimGlobalPrefs::OTHER_FAC))
     ||((mCurrentApp == ExternalAppHandler::PAGER) && (mCurrentClient == KPimGlobalPrefs::OTHER_PAC))
     ||((mCurrentApp == ExternalAppHandler::SIP) && (mCurrentClient == KPimGlobalPrefs::OTHER_SIC)))
     {
       readonly = false;
	 }
	 else
	 {
       readonly = true;
	 }

  if ( ((mCurrentApp == ExternalAppHandler::EMAIL) && (mCurrentClient == KPimGlobalPrefs::NONE_EMC))
     ||((mCurrentApp == ExternalAppHandler::PHONE) && (mCurrentClient == KPimGlobalPrefs::NONE_PHC))
     ||((mCurrentApp == ExternalAppHandler::SMS) && (mCurrentClient == KPimGlobalPrefs::NONE_SMC))
     ||((mCurrentApp == ExternalAppHandler::FAX) && (mCurrentClient == KPimGlobalPrefs::NONE_FAC))
     ||((mCurrentApp == ExternalAppHandler::PAGER) && (mCurrentClient == KPimGlobalPrefs::NONE_PAC))
     ||((mCurrentApp == ExternalAppHandler::SIP) && (mCurrentClient == KPimGlobalPrefs::NONE_SIC)))
     {
       enabled = false;
	 }
	 else
	 {
       enabled = true;
	 }


  mChannel->setReadOnly(readonly);
  mMessage->setReadOnly(readonly);
  mParameters->setReadOnly(readonly); 
  if ( mMessage2 )
  mMessage2->setReadOnly(readonly);
    if ( mParameters2 )
  mParameters2->setReadOnly(readonly);

  mChannel->setEnabled(enabled);
  mMessage->setEnabled(enabled);
  mParameters->setEnabled(enabled);
  if ( mMessage2 )
  mMessage2->setEnabled(enabled);
    if ( mParameters2 )
  mParameters2->setEnabled(enabled);



  mClient->setCurrentItem(mCurrentClient);


  // enable/disable the extra message/parameter field
  if (mCurrentApp == ExternalAppHandler::EMAIL)
  {
  }
  else
  {
  if ( mMessage2 )
    mMessage2->setText( "" );
    if ( mParameters2 )
    mParameters2->setText( "" );
  }

  if (enabled == true) {
  if ( mMessage2 )
    mMessage2->setEnabled(mCurrentApp == ExternalAppHandler::EMAIL);
    if ( mParameters2 )
    mParameters2->setEnabled(mCurrentApp == ExternalAppHandler::EMAIL);
  }


  blockSignals( blocked );

}

void KDEPIMConfigWidget::usrReadConfig()
{
  KPimGlobalPrefs* prefs = KPimGlobalPrefs::instance();

  bool blocked = signalsBlocked();
  blockSignals( true );

  if (KPimGlobalPrefs::instance()->mBackupUseDefaultDir )
      mBackupUrl->setURL( KGlobalSettings::backupDataDir() );
  else {
      mBackupUrl->setURL(prefs->mBackupDatadir);

  }
  mBackupNumbersSpin->setValue( prefs->mBackupNumbers );
  mBackupDayCountSpin->setValue( prefs->mBackupDayCount);

  QString dummy = prefs->mUserDateFormatLong;
  mUserDateFormatLong->setText(dummy.replace( QRegExp("K"), QString(",") ));
  dummy = prefs->mUserDateFormatShort;
  mUserDateFormatShort->setText(dummy.replace( QRegExp("K"), QString(",") ));

  QDate current ( 2001, 1,1);
  mStartDateSavingEdit->setDate(current.addDays(prefs->mDaylightsavingStart-1));
  mEndDateSavingEdit->setDate(current.addDays(prefs->mDaylightsavingEnd-1));
  setCombo(mTimeZoneCombo,i18n(prefs->mTimeZoneId));

  mEmailClient = prefs->mEmailClient;
  mEmailOtherChannel = prefs->mEmailOtherChannel;
  mEmailOtherMessage = prefs->mEmailOtherMessage;
  mEmailOtherMessageParameters = prefs->mEmailOtherMessageParameters;
  mEmailOtherMessage2 = prefs->mEmailOtherMessage2;
  mEmailOtherMessageParameters2 = prefs->mEmailOtherMessageParameters2;

  mPhoneClient = prefs->mPhoneClient;
  mPhoneOtherChannel = prefs->mPhoneOtherChannel;
  mPhoneOtherMessage = prefs->mPhoneOtherMessage;
  mPhoneOtherMessageParameters = prefs->mPhoneOtherMessageParameters;

  mFaxClient = prefs->mFaxClient;
  mFaxOtherChannel = prefs->mFaxOtherChannel;
  mFaxOtherMessage = prefs->mFaxOtherMessage;
  mFaxOtherMessageParameters = prefs->mFaxOtherMessageParameters;

  mSMSClient = prefs->mSMSClient;
  mSMSOtherChannel = prefs->mSMSOtherChannel;
  mSMSOtherMessage = prefs->mSMSOtherMessage;
  mSMSOtherMessageParameters = prefs->mSMSOtherMessageParameters;

  mPagerClient = prefs->mPagerClient;
  mPagerOtherChannel = prefs->mPagerOtherChannel;
  mPagerOtherMessage = prefs->mPagerOtherMessage;
  mPagerOtherMessageParameters = prefs->mPagerOtherMessageParameters;

  mSipClient = prefs->mSipClient;
  mSipOtherChannel = prefs->mSipOtherChannel;
  mSipOtherMessage = prefs->mSipOtherMessage;
  mSipOtherMessageParameters = prefs->mSipOtherMessageParameters;

  mCurrentApp = ExternalAppHandler::EMAIL;
  mCurrentClient = mEmailClient;

  updateClientWidgets();

  blockSignals( blocked );

}

void KDEPIMConfigWidget::usrWriteConfig()
{
  KPimGlobalPrefs* prefs = KPimGlobalPrefs::instance();

  saveEditFieldSettings();

  prefs->mBackupNumbers  = mBackupNumbersSpin->value();
  prefs->mBackupDayCount = mBackupDayCountSpin->value();
  QString bup_url = mBackupUrl->url();
  if ( bup_url.right(1) != "/" && bup_url.right(1) != "\\" )
    bup_url += "/";

  prefs->mBackupDatadir = bup_url;

  prefs->mUserDateFormatShort = mUserDateFormatShort->text().replace( QRegExp(","), QString("K") );
  prefs->mUserDateFormatLong = mUserDateFormatLong->text().replace( QRegExp(","), QString("K") );

  prefs->mTimeZoneId = mTimeZoneCombo->currentText();
  QDate date;
  date = mStartDateSavingEdit->date();
  int sub = 0;
  if ( QDate::leapYear( date.year() ) && date.dayOfYear() > 59 )
      sub = 1;
  prefs->mDaylightsavingStart = date.dayOfYear()-sub;
  date = mEndDateSavingEdit->date();
  if ( QDate::leapYear( date.year() ) && date.dayOfYear() > 59  )
      sub = 1;
  else 
      sub = 0;
  prefs->mDaylightsavingEnd = date.dayOfYear()-sub;


  prefs->mEmailClient = mEmailClient;
  prefs->mEmailOtherChannel = mEmailOtherChannel;
  prefs->mEmailOtherMessage = mEmailOtherMessage;
  prefs->mEmailOtherMessageParameters = mEmailOtherMessageParameters;
  prefs->mEmailOtherMessage2 = mEmailOtherMessage2;
  prefs->mEmailOtherMessageParameters2 = mEmailOtherMessageParameters2;

  prefs->mPhoneClient = mPhoneClient;
  prefs->mPhoneOtherChannel = mPhoneOtherChannel;
  prefs->mPhoneOtherMessage = mPhoneOtherMessage;
  prefs->mPhoneOtherMessageParameters = mPhoneOtherMessageParameters;

  prefs->mFaxClient = mFaxClient;
  prefs->mFaxOtherChannel = mFaxOtherChannel;
  prefs->mFaxOtherMessage = mFaxOtherMessage;
  prefs->mFaxOtherMessageParameters = mFaxOtherMessageParameters;

  prefs->mSMSClient = mSMSClient;
  prefs->mSMSOtherChannel = mSMSOtherChannel;
  prefs->mSMSOtherMessage = mSMSOtherMessage;
  prefs->mSMSOtherMessageParameters = mSMSOtherMessageParameters;

  prefs->mPagerClient = mPagerClient;
  prefs->mPagerOtherChannel = mPagerOtherChannel;
  prefs->mPagerOtherMessage = mPagerOtherMessage;
  prefs->mPagerOtherMessageParameters = mPagerOtherMessageParameters;


  prefs->mSipClient = mSipClient;
  prefs->mSipOtherChannel = mSipOtherChannel;
  prefs->mSipOtherMessage = mSipOtherMessage;
  prefs->mSipOtherMessageParameters = mSipOtherMessageParameters;

  //release the cache that other views can access the changed values instantanious
  ExternalAppHandler::instance()->loadConfig();
  KPimGlobalPrefs::instance()->setGlobalConfig();
}


void KDEPIMConfigWidget::setCombo(QComboBox *combo, const QString & text,
                               const QStringList *tags)
{
  if (tags) {
    int i = tags->findIndex(text);
    if (i > 0) combo->setCurrentItem(i);
  } else {
    for(int i=0;i<combo->count();++i) {
      if (combo->text(i) == text) {
        combo->setCurrentItem(i);
        break;
      }
    }
  }
}


void KDEPIMConfigWidget::textChanged( const QString& text )
{
  emit changed( true );
}
