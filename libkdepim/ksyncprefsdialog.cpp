/*
    This file is part of KOrganizer.
    Copyright (c) 2004 Lutz Rogowski <rogowski@kde.org>

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
#include <qlabel.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qfont.h>
#include <qslider.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qspinbox.h>
#include <qdatetime.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qstrlist.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
 
#include <kcolorbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kfontdialog.h>
#include <kmessagebox.h>
#include <kcolordialog.h>
#include <kiconloader.h>
#include <kemailsettings.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

//#include <kurlrequester.h>
#include <klineedit.h>
#include "ksyncprofile.h"


//#include "koprefs.h"

#include "ksyncprefsdialog.h"
//#include "koglobals.h"


KSyncPrefsDialog::KSyncPrefsDialog(QWidget *parent, char *name, bool modal) :
  KDialog(parent,name,true)
{

  setCaption( i18n("Synchronization Preferences"));

  mSyncProfiles.setAutoDelete( true );
   setupSyncAlgTab();
}


KSyncPrefsDialog::~KSyncPrefsDialog()
{
}
  
void KSyncPrefsDialog::setupSyncAlgTab()
{
  QLabel * lab;
  //QFrame *page = addPage(i18n("Sync Prefs"),0,0);
  QVBox * mainbox = new QVBox( this );
   QScrollView* sv = new QScrollView( mainbox );
   QHBoxLayout * lay = new QHBoxLayout( this );
   lay->addWidget( mainbox );
   QHBox * b_box = new QHBox( mainbox );

   QPushButton* button = new QPushButton( i18n("Ok"), b_box );
   connect ( button, SIGNAL( clicked()), this, SLOT (slotOK() ) );
   button = new QPushButton( i18n("Cancel"), b_box );
   connect ( button, SIGNAL( clicked()), this, SLOT (reject() ) );
   //QBoxLayout * sl = new QVBoxLayout(this );
   //sl->addWidget ( sv );  
   sv->setResizePolicy ( QScrollView::AutoOneFit );
   QFrame *topFrame = new QFrame ( sv );
   sv->addChild( topFrame );
   mSetupSyncAlgTab = topFrame;
   QGridLayout *topLayout = new QGridLayout(topFrame,6,2);
   topLayout->setSpacing(spacingHint());
   topLayout->setMargin(marginHint());  

   //lab = new QLabel(i18n("Sync settings not yet implemented. DO NOT USE!"), topFrame);
   int iii = 0;
   //topLayout->addMultiCellWidget(lab , iii,iii,0,1);
   //++iii;

   mMyMachineName = new QLineEdit(topFrame);
   lab = new QLabel(mMyMachineName, i18n("Local device name:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mMyMachineName,iii,1);
   ++iii;
  
   QHBox* buttonbox = new QHBox( topFrame);
   topLayout->addMultiCellWidget(buttonbox, iii,iii,0,1);
   ++iii;
   button = new QPushButton( i18n("New profile"), buttonbox );
   connect ( button, SIGNAL( clicked()), this, SLOT (newProfile() ) ); 

   button = new QPushButton( i18n("Clone profile"), buttonbox );
   connect ( button, SIGNAL( clicked()), this, SLOT ( cloneProfile() ) );

   button = new QPushButton( i18n("Delete profile"), buttonbox );
   connect ( button, SIGNAL( clicked()), this, SLOT (deleteProfile() ) );

   mProfileBox = new QComboBox(topFrame);
   mProfileBox->setEditable ( true ); 
   mProfileBox->setInsertionPolicy(QComboBox::NoInsertion);
   connect ( mProfileBox, SIGNAL(activated ( int ) ), this, SLOT (profileChanged( int ) ) );
   connect ( mProfileBox, SIGNAL(  textChanged ( const QString & ) ), this, SLOT (textChanged( const QString & ) ) );
  
   lab = new QLabel(mProfileBox, i18n("Profile:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mProfileBox, iii,1); 
   ++iii;



   QHGroupBox *iims = new QHGroupBox( i18n("Multiple Sync options"), topFrame);
   new QLabel( i18n("Include in multiple "), iims );
   mIncludeInRing = new QCheckBox( i18n("calendar  "), iims  );
   mIncludeInRingAB = new QCheckBox( i18n("addressbook  "), iims  );
   mIncludeInRingPWM = new QCheckBox( i18n("pwmanager"), iims   );
   new QLabel( i18n(" sync"), iims );
   topLayout->addMultiCellWidget(iims, iii,iii,0,1);
   ++iii;
   QVGroupBox* gb0 = new QVGroupBox( i18n("Sync algo options"), topFrame);
   topLayout->addMultiCellWidget(gb0, iii,iii,0,1);
   ++iii;
   QButtonGroup* gr;
   {
       QVGroupBox* topFrame = gb0;

       

       mAskForPreferences = new QCheckBox( i18n("Ask for preferences before sync"), topFrame   );
       //topLayout->addMultiCellWidget(mAskForPreferences, iii,iii,0,1);
       //++iii;
       gr = new QButtonGroup ( 1,  Qt::Horizontal, i18n("Sync preferences"), topFrame);
       //topLayout->addMultiCellWidget(gr, iii,iii,0,1);
       //++iii;
       loc = new QRadioButton ( i18n("Take local entry on conflict"), gr );
       rem = new QRadioButton ( i18n("Take remote entry on conflict"), gr );
       newest = new QRadioButton ( i18n("Take newest entry on conflict"), gr );
       ask = new QRadioButton ( i18n("Ask for every entry on conflict"), gr );
       f_loc= new QRadioButton ( i18n("Force: Take local entry always"), gr );
       f_rem = new QRadioButton ( i18n("Force: Take remote entry always"), gr );
       // both = new QRadioButton ( i18n("Take both on conflict"), gr );

       mShowSummaryAfterSync = new QCheckBox( i18n("Show summary after sync"), topFrame   );
       //topLayout->addMultiCellWidget(mShowSummaryAfterSync, iii,iii,0,1);
       //++iii;  

#if 0
       QHGroupBox* gb51 = new QHGroupBox( i18n("Local resources to sync with:"), topFrame); 
       {
           QVBox * fibo2 = new QVBox ( gb51 );
           new QLabel ( i18n("Calendar resources:"), fibo2 );
           //mFilterInCal = new QComboBox( fibo2 );
           fibo2 = new QVBox ( gb51 );
           new QLabel ( i18n("Addressbook resources:"), fibo2 );
           //mFilterInAB = new QComboBox( fibo2 ); 
       }
#endif
       QHGroupBox* gb5 = new QHGroupBox( i18n("Apply filter when adding data to local:"), topFrame);
       QVBox * fibo2 = new QVBox ( gb5 );
       new QLabel ( i18n("Incoming calendar filter:"), fibo2 );
       mFilterInCal = new QComboBox( fibo2 );
       fibo2 = new QVBox ( gb5 );
       new QLabel ( i18n("Incoming addressbook filter:"), fibo2 );
       mFilterInAB = new QComboBox( fibo2 );

       mWriteBackFile = new QCheckBox( i18n("Write back synced data"), topFrame   );
       // topLayout->addMultiCellWidget(mWriteBackFile, iii,iii,0,1);
       // ++iii;
   
       QVGroupBox* gb2 = new QVGroupBox( i18n("Write back options"), topFrame);
       //topLayout->addMultiCellWidget(gb2, iii,iii,0,1);
       //++iii;
       {
           QVGroupBox*topFrame = gb2; 
           mWriteBackExisting= new QCheckBox( i18n("Write back (on remote) existing entries only"), topFrame   );
           QHGroupBox* gb4 = new QHGroupBox( i18n("Apply filter when adding data to remote:"), topFrame);
           QVBox * fibo = new QVBox ( gb4 );
           new QLabel ( i18n("Outgoing calendar filter:"), fibo );
           mFilterOutCal = new QComboBox( fibo );
           fibo = new QVBox ( gb4 );
           new QLabel ( i18n("Outgoing addressbook filter:"), fibo );
           mFilterOutAB = new QComboBox( fibo );
           //topLayout->addMultiCellWidget(mWriteBackExisting, iii,iii,0,1);
           //++iii;
           mWriteBackFuture= new QCheckBox( i18n("Write back (calendar) entries for time period only"), topFrame   );
           //topLayout->addMultiCellWidget(mWriteBackFuture, iii,iii,0,1);
           //++iii;
           QVGroupBox* gb3 = new QVGroupBox( i18n("Time period"), topFrame);
           connect ( mWriteBackFuture, SIGNAL( toggled ( bool ) ), gb3, SLOT ( setEnabled ( bool )  ) );

           new QLabel( i18n("From ") , gb3 );
           QHBox * tf1 = new QHBox ( gb3 );
           mWriteBackPastWeeks= new QSpinBox(1,104, 1,  tf1 );
           mWriteBackPastWeeks->setMaximumWidth( 100 );
           new QLabel( i18n(" weeks in the past to ") ,tf1 );
           QHBox * tf2 = new QHBox ( gb3 );
           mWriteBackFutureWeeks= new QSpinBox(1,104, 1,  tf2);
           mWriteBackFutureWeeks->setMaximumWidth( 100 );
           new QLabel( i18n(" weeks in the future ") , tf2 );
           //topLayout->addMultiCellWidget(mWriteBackFutureWeeks, iii,iii,1,1);
           //++iii;
           gb3->setEnabled( false ); 
           connect ( mWriteBackExisting, SIGNAL( toggled ( bool ) ), gb4, SLOT ( setDisabled ( bool )  ) );
       }
       connect ( mWriteBackFile, SIGNAL( toggled ( bool ) ), gb2, SLOT ( setEnabled ( bool )  ) );

   }
   proGr = new QButtonGroup ( 1,  Qt::Horizontal, i18n("Profile kind"), topFrame);
   gr = proGr;
   topLayout->addMultiCellWidget(gr, iii,iii,0,1);
   ++iii;
   mIsLocal = new QRadioButton ( i18n("Local file"), gr );
   mIsPi = new QRadioButton ( i18n("Pi-Sync - all resources ( direct Kx/Pi to Kx/Pi sync )"), gr );
   connect (mIsPi, SIGNAL( toggled(bool)), this, SLOT (kindChanged(bool) ) ); 
   mIsPiSpecific = new QRadioButton ( i18n("Pi-Sync - selected resources ( direct Kx/Pi to Kx/Pi sync )"), gr );
   connect (mIsPiSpecific, SIGNAL( toggled(bool)), this, SLOT (kindChanged(bool) ) ); 
   mIsNotLocal = new QRadioButton ( i18n("Remote file (w down/upload command)"), gr );
   connect (mIsLocal, SIGNAL( toggled(bool)), this, SLOT (kindChanged(bool) ) ); 
   mIsPhone = new QRadioButton ( i18n("Mobile device (cell phone)"), gr );
   connect (mIsPhone, SIGNAL( toggled(bool)), this, SLOT (kindChanged(bool) ) );  

   QVGroupBox* gb1 = new QVGroupBox( i18n("Profile kind specific settings"), topFrame);
   topLayout->addMultiCellWidget(gb1, iii,iii,0,1);
   ++iii;

   // ******************************************
   // Profile kind specific settings
   {
       // *** phone *******************************
       QVGroupBox* topFrame = gb1;
       phoneWidget = new QVBox( topFrame);
       //topLayout->addMultiCellWidget(phoneWidget, iii,iii,0,1);
       //++iii; 
       mWriteContactToSIM = 0;//new QCheckBox( i18n("Sync contacts with phone SIM card (If not, sync with phone memory)"), phoneWidget );
       QHBox* temphb = new QHBox( phoneWidget );
       new QLabel( i18n("I/O device: "), temphb ); 
       mPhoneDevice = new QLineEdit( temphb);
       button = new QPushButton( i18n("Help..."), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT ( helpDevice() ) );


       temphb = new QHBox( phoneWidget );
       new QLabel( i18n("Connection: "), temphb ); 
       mPhoneConnection = new QLineEdit( temphb);
       button = new QPushButton( i18n("Help..."), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT ( helpConnection() ) );


       temphb = new QHBox( phoneWidget );
       new QLabel( i18n("Model(opt.): "), temphb ); 
       mPhoneModel = new QLineEdit( temphb);
       button = new QPushButton( i18n("Help..."), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT ( helpModel() ) );

       // *** local*******************************
       localFileWidget = new QVBox( topFrame);
       //topLayout->addMultiCellWidget(localFileWidget, iii,iii,0,1);
       //++iii;
       temphb = new QHBox( localFileWidget );

       lab = new QLabel( i18n("Local file Cal:"), temphb );
       lab = new QLabel( i18n("Local file ABook:"), temphb );
       lab = new QLabel( i18n("Local file PWMgr:"), temphb );
       temphb = new QHBox( localFileWidget );
       button = new QPushButton( i18n("Choose..."), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT ( chooseFile() ) );
       button = new QPushButton( i18n("Choose..."), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT ( chooseFileAB() ) );
       button = new QPushButton( i18n("Choose..."), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT ( chooseFilePWM() ) );
       temphb = new QHBox( localFileWidget );

       mRemoteFile = new QLineEdit( temphb);
       mRemoteFileAB = new QLineEdit( temphb);
       mRemoteFilePWM = new QLineEdit( temphb);
       mIsKapiFileL = new QCheckBox( i18n("Addressbook file (*.vcf) is used by KA/Pi"), localFileWidget );


       // *** remote*******************************
       remoteFileWidget = new QVBox( topFrame);
       //topLayout->addMultiCellWidget(remoteFileWidget, iii,iii,0,1);
       //++iii;
       temphb = new QHBox( remoteFileWidget );
       new QLabel( i18n("Calendar:"), temphb);
       new QLabel( i18n("AddressBook:"), temphb);
       new QLabel( i18n("PWManager:"), temphb);

       lab = new QLabel( i18n("Pre sync (download) command:"), remoteFileWidget);
       temphb = new QHBox( remoteFileWidget );
       mRemotePrecommand = new QLineEdit(temphb);
       mRemotePrecommandAB = new QLineEdit(temphb);
       mRemotePrecommandPWM = new QLineEdit(temphb);

       lab = new QLabel( i18n("Local temp file:"), remoteFileWidget);
       temphb = new QHBox( remoteFileWidget );
       mLocalTempFile = new QLineEdit(temphb);
       mLocalTempFileAB = new QLineEdit(temphb);
       mLocalTempFilePWM = new QLineEdit(temphb);

       lab = new QLabel( i18n("Post sync (upload) command:"), remoteFileWidget);
       temphb = new QHBox( remoteFileWidget );
       mRemotePostcommand = new QLineEdit(temphb );
       mRemotePostcommandAB = new QLineEdit(temphb );
       mRemotePostcommandPWM = new QLineEdit(temphb );
       mIsKapiFileR = new QCheckBox( i18n("Addressbook file is used by KA/Pi"), remoteFileWidget );
       lab = new QLabel( i18n("Fill in default values for:"), remoteFileWidget);
       temphb = new QHBox( remoteFileWidget );
       button = new QPushButton( i18n("ssh/scp"), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT (fillSSH() ) );
       button = new QPushButton( i18n("ftp"), temphb );
       connect ( button, SIGNAL( clicked()), this, SLOT (fillFTP() ) );
       lab = new QLabel( i18n("Hint: Use $PWD$ for placeholder of password!"), remoteFileWidget);

       // *** pi-sync*******************************
       piWidget = new QVBox( topFrame);
       //topLayout->addMultiCellWidget(piWidget, iii,iii,0,1);
       //++iii;
       temphb = new QHBox( piWidget );
       new QLabel( i18n("Calendar:"), temphb);
       new QLabel( i18n("AddressBook:"), temphb);
       new QLabel( i18n("PWManager:"), temphb);

       lab = new QLabel( i18n("Password for remote access: (could be the same for each)"), piWidget);
       temphb = new QHBox( piWidget );
       mRemotePw = new QLineEdit(temphb);
       mRemotePwAB = new QLineEdit(temphb);
       mRemotePwPWM = new QLineEdit(temphb);

       lab = new QLabel( i18n("Remote IP address: (could be the same for each)"), piWidget);
       temphb = new QHBox( piWidget );
       mRemoteIP = new QLineEdit(temphb);
       mRemoteIPAB = new QLineEdit(temphb);
       mRemoteIPPWM = new QLineEdit(temphb);

       lab = new QLabel( i18n("Remote port number: (should be different for each)"), piWidget);
       temphb = new QHBox( piWidget );
       mRemotePort = new QLineEdit(temphb);
       mRemotePortAB = new QLineEdit(temphb);
       mRemotePortPWM = new QLineEdit(temphb);

       lab = new QLabel( i18n("Local/remote Resource sync partners (Leave empty to not sync)"), piWidget);
       mTableBox = new QHBox( piWidget );
       mResTableKopi = new QTable( 1, 1, mTableBox );
       mResTableKapi = new QTable( 1, 1, mTableBox );
       mResTablePwmpi = new QTable( 1, 1, mTableBox );
       mResTableKopi->horizontalHeader()->setLabel( 0, i18n("Remote") );
       mResTableKapi->horizontalHeader()->setLabel( 0, i18n("Remote") );
       mResTablePwmpi->horizontalHeader()->setLabel( 0, i18n("Remote") );
       mResTableKopi->setLeftMargin( 80 );
       mResTableKapi->setLeftMargin( 80 );
   } 
   // ******************************************
   // Profile kind specific settings END
 
}

void KSyncPrefsDialog::readResources()
{
    mResourcesKopi.clear();
    KConfig fc(locateLocal("config","kopicalendarrc"));
    fc.setGroup("CC");
    int numCals = fc.readNumEntry("NumberCalendars",0 );
    int curCal = 1;
    while ( curCal <= numCals ) {
        QString prefix = "Cal_" +QString::number( curCal );
        QString name =  fc.readEntry( prefix+"_Name", "Calendar");
        mResourcesKopi.append( name );
        ++curCal;
    }
    mResTableKopi->setNumRows( mResourcesKopi.count() );
    int i;
    for ( i = 0;i < mResourcesKopi.count(); ++i ) {
        mResTableKopi->verticalHeader ()->setLabel( i, mResourcesKopi[i] );
        mResTableKopi->setText( i,0, mResourcesKopi[i] );
    }

    mResourcesKapi.clear();

    KConfig conf ( locateLocal("config", "kabcrc") );

    conf.setGroup( "General" );
    
    QStringList keys = conf.readListEntry( "ResourceKeys" );
    //keys += conf->readListEntry( "PassiveResourceKeys" );

    //QString standardKey = mConfig->readEntry( "Standard" );
    
    for ( QStringList::Iterator it = keys.begin(); it != keys.end(); ++it ) {
        conf.setGroup("Resource_"+ (*it) );
        QString name = conf.readEntry( "ResourceName" );
        mResourcesKapi.append( name );
    }
    mResTableKapi->setNumRows( mResourcesKapi.count() );
    for ( i = 0;i < mResourcesKopi.count(); ++i ) {
        mResTableKapi->verticalHeader ()->setLabel( i, mResourcesKapi[i] );
        mResTableKapi->setText( i, 0, mResourcesKapi[i] );
    }


}
void KSyncPrefsDialog::readFilter()
{
    mFilterKapi.clear();
    mFilterKopi.clear();
    KConfig cfgko(locateLocal("config","korganizerrc"));
    KConfig cfgka(locateLocal("config","kaddressbookrc"));
    cfgko.setGroup("General");
    mFilterKopi =  cfgko.readListEntry("CalendarFilters");
    cfgka.setGroup("Filter");
    int count = cfgka.readNumEntry( "Count", 0 );
    for ( int i = 0; i < count; i++ ) {
        cfgka.setGroup("Filter_"+QString::number( i ) );  
        mFilterKapi.append( cfgka.readEntry("Name", "internal error") );  
    }
    mFilterOutCal->clear();
    mFilterInCal->clear();
    mFilterOutAB->clear();
    mFilterInAB->clear();
    QStringList temp = mFilterKopi;
    temp.prepend(i18n("No Filter") );
    mFilterOutCal->insertStringList( temp );
    mFilterInCal->insertStringList( temp );
    temp = mFilterKapi;
    temp.prepend(i18n("No Filter") );
    mFilterOutAB->insertStringList( temp );
    mFilterInAB->insertStringList( temp );
}

void KSyncPrefsDialog::slotOK()
{
    if ( mMyMachineName->text() == "undefined" ) {
        KMessageBox::error(this,i18n("Local device name undefined!\nPlease define device name!"),i18n("KO/Pi config error"));
        return;
    }
    int i;
    for (i = 0; i < mSyncProfileNames.count(); ++ i) {
#ifdef QT_4_COMPAT
        if ( mSyncProfileNames.count( mSyncProfileNames.at(i)) > 1  ) {
#else

        if ( mSyncProfileNames.contains( mSyncProfileNames[i]) > 1  ) {
#endif
            KMessageBox::error(this,i18n("Multiple profiles with same name!\nPlease use unique profile names!"),i18n("KO/Pi config error"));
            return;
        }
    }
    usrWriteConfig();
    QDialog::accept();
}
void KSyncPrefsDialog::accept()
{
    slotOK();
}
void KSyncPrefsDialog::chooseFile()
{
    QString fn = QDir::homeDirPath();

    fn =KFileDialog:: getOpenFileName( fn, i18n("Sync filename(*.ics/*.vcs)"), this );
    if ( fn == "" )
        return;
    mRemoteFile->setText( fn );
}

void KSyncPrefsDialog::chooseFileAB()
{
    QString fn = QDir::homeDirPath();

    fn =KFileDialog:: getOpenFileName( fn, i18n("Sync filename(*.vcf)"), this );
    if ( fn == "" )
        return;
    mRemoteFileAB->setText( fn );
}

void KSyncPrefsDialog::chooseFilePWM()
{
    QString fn = QDir::homeDirPath();
    fn =KFileDialog:: getOpenFileName( fn, i18n("Sync filename(*.pwm)"), this );
    if ( fn == "" )
        return;
    mRemoteFilePWM->setText( fn );
}
void KSyncPrefsDialog::updateMyCaption()
{
    setCaption( mProfileBox->currentText() + " : " +i18n("Synchronization Preferences"));
}
void KSyncPrefsDialog::textChanged( const QString &  s )
{
    if ( mProfileBox->count() == 0 )
        return;
    if ( currentSelection < 3 ) {
        //KMessageBox::error(this,i18n("This profil name\ncannot be edited!\n"),i18n("KO/Pi config error"));
        mProfileBox->blockSignals( true );
        mProfileBox->setCurrentItem(mProfileBox-> currentItem ());
        mProfileBox->blockSignals( false );
        return;
    }
    //qDebug("cur i %d ",mProfileBox-> currentItem () );
    mProfileBox->changeItem ( s, mProfileBox-> currentItem () ) ;
    KSyncProfile* prof = mSyncProfiles.at(mProfileBox-> currentItem ()) ;
    prof->setName( s );
    mSyncProfileNames[mProfileBox-> currentItem ()] = s;
    updateMyCaption();
}
void KSyncPrefsDialog::profileChanged( int item )
{
  //qDebug("KSyncPrefsDialog::profileChanged before %d, count %d ", item, mProfileBox->count() ); 
    KSyncProfile* prof;
    saveProfile();
    readFilter();
    readResources();
    currentSelection = item;
    prof = mSyncProfiles.at(item) ;

    mRemotePw->setText(prof->getRemotePw());
    mRemoteIP->setText(prof->getRemoteIP());
    mRemotePort->setText(prof->getRemotePort());
    
    mRemotePwAB->setText(prof->getRemotePwAB());
    mRemoteIPAB->setText(prof->getRemoteIPAB());
    mRemotePortAB->setText(prof->getRemotePortAB());
    
    mRemotePwPWM->setText(prof->getRemotePwPWM());
    mRemoteIPPWM->setText(prof->getRemoteIPPWM());
    mRemotePortPWM->setText(prof->getRemotePortPWM());

    mRemotePrecommand->setText(prof->getPreSyncCommand());
    mRemotePostcommand->setText(prof->getPostSyncCommand());
    mLocalTempFile->setText(prof->getLocalTempFile());
    mRemoteFile->setText(prof->getRemoteFileName()) ;

    mRemotePrecommandAB->setText(prof->getPreSyncCommandAB());
    mRemotePostcommandAB->setText(prof->getPostSyncCommandAB());
    mLocalTempFileAB->setText(prof->getLocalTempFileAB());
    mRemoteFileAB->setText(prof->getRemoteFileNameAB()) ;

    mRemotePrecommandPWM->setText(prof->getPreSyncCommandPWM());
    mRemotePostcommandPWM->setText(prof->getPostSyncCommandPWM());
    mLocalTempFilePWM->setText(prof->getLocalTempFilePWM());
    mRemoteFilePWM->setText(prof->getRemoteFileNamePWM()) ;

    if ( mWriteContactToSIM )
        mWriteContactToSIM->setChecked( prof->getWriteContactToSIM());
    mPhoneDevice->setText(prof->getPhoneDevice());
    mPhoneConnection->setText(prof->getPhoneConnection());
    mPhoneModel->setText(prof->getPhoneModel());

    mShowSummaryAfterSync->setChecked( prof->getShowSummaryAfterSync());
    mAskForPreferences->setChecked( prof->getAskForPreferences());
    mWriteBackExisting->setChecked( prof->getWriteBackExisting() );
    mWriteBackFile->setChecked( prof->getWriteBackFile());
    mIncludeInRing->setChecked( prof->getIncludeInRingSync() );
    mIncludeInRingAB->setChecked( prof->getIncludeInRingSyncAB() );
    mIncludeInRingPWM->setChecked( prof->getIncludeInRingSyncPWM() );
    mWriteBackFuture->setChecked( prof->getWriteBackFuture());
    mWriteBackFutureWeeks->setValue( prof->getWriteBackFutureWeeks() );
    mWriteBackPastWeeks->setValue( prof->getWriteBackPastWeeks() );

    mFilterInCal->setCurrentItem( mFilterKopi.findIndex(prof->getFilterInCal () ) + 1 );
    mFilterOutCal->setCurrentItem( mFilterKopi.findIndex(prof->getFilterOutCal () ) + 1 );
    mFilterInAB->setCurrentItem( mFilterKapi.findIndex(prof->getFilterInAB () ) + 1 );
    mFilterOutAB->setCurrentItem( mFilterKapi.findIndex(prof->getFilterOutAB () ) + 1 );
    
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
    mIsLocal->setChecked(prof->getIsLocalFileSync()) ;
    mIsPhone->setChecked(prof->getIsPhoneSync()) ;
    mIsPi->setChecked(prof->getIsPiSync()) ;
    mIsPiSpecific->setChecked(prof->getIsPiSyncSpec()) ;
    mIsKapiFileL->setChecked(prof->getIsKapiFile()) ;
    mIsKapiFileR->setChecked(prof->getIsKapiFile()) ;


    QStringList res = QStringList::split( ":",prof->getResSpecKopi(), true );
    int add = res.count()/2;
    int i;
    for ( i = 0;i < add ; ++i ) {
        mResTableKopi->setText( i, 0, res[i+add] );
    } 
    res = QStringList::split( ":",prof->getResSpecKapi(), true);
    add = res.count()/2;
    for ( i = 0;i <  add; ++i ) {
        mResTableKapi->setText( i, 0, res[i+add] );
    }
    mIsNotLocal->setChecked(!prof->getIsLocalFileSync() && !prof->getIsPhoneSync() &&!prof->getIsPiSync() &&!prof->getIsPiSyncSpec());
    proGr->setEnabled( item > 2 );
    if ( item < 3 ) {
        localFileWidget->hide();
        remoteFileWidget->hide();
        phoneWidget->hide();
        piWidget->hide();

    } else
        kindChanged( prof->getIsLocalFileSync() );
    updateMyCaption();
}

void KSyncPrefsDialog::fillSSH()
{
    mRemotePrecommand->setText("scp zaurus@192.168.0.65:/home/zaurus/kdepim/apps/korganizer/mycalendar.ics /tmp/mycalendar.ics" );
    mLocalTempFile->setText("/tmp/mycalendar.ics" );
    mRemotePostcommand->setText("scp /tmp/mycalendar.ics zaurus@192.168.0.65:/home/zaurus/kdepim/apps/korganizer/mycalendar.ics" );
    mRemotePrecommandAB->setText("scp zaurus@192.168.0.65:/home/zaurus/kdepim/apps/kabc/std.vcf /tmp/std.vcf" );
    mLocalTempFileAB->setText("/tmp/std.vcf" );
    mRemotePostcommandAB->setText("scp /tmp/std.vcf zaurus@192.168.0.65:/home/zaurus/kdepim/apps/kabc/std.vcf" );
    mRemotePrecommandPWM->setText("scp zaurus@192.168.0.65:/home/zaurus/kdepim/apps/pwmanager/passwords.pwm /tmp/passwords.pwm" );
    mLocalTempFilePWM->setText("/tmp/passwords.pwm" );
    mRemotePostcommandPWM->setText("scp /tmp/passwords.pwm zaurus@192.168.0.65:/home/zaurus/kdepim/apps/pwmanager/pwmanager.pwm" );
}
void KSyncPrefsDialog::fillFTP()
{
    mRemotePrecommand->setText("cd /tmp;ftp ftp://zaurus:a@192.168.0.65/kdepim/apps/korganizer/mycalendar.ics" );
    mLocalTempFile->setText("/tmp/mycalendar.ics" );
    mRemotePostcommand->setText("ftp -u ftp://zaurus:a@192.168.0.65/kdepim/apps/korganizer/mycalendar.ics /tmp/mycalendar.ics" );
    mRemotePrecommandAB->setText("cd /tmp;ftp ftp://zaurus:a@192.168.0.65/kdepim/apps/kabc/std.vcf" );
    mLocalTempFileAB->setText("/tmp/std.vcf" );
    mRemotePostcommandAB->setText("ftp -u ftp://zaurus:a@192.168.0.65/kdepim/apps/kabc/std.vcf /tmp/std.vcf" );

    mRemotePrecommandPWM->setText("cd /tmp;ftp ftp://zaurus:a@192.168.0.65/kdepim/apps/pwmanager/passwords.pwm" );
    mLocalTempFilePWM->setText("/tmp/passwords.pwm" );
    mRemotePostcommandPWM->setText("ftp -u ftp://zaurus:a@192.168.0.65/kdepim/apps/pwmanager/passwords.pwm /tmp/passwords.pwm" );
   
}
void KSyncPrefsDialog::kindChanged( bool b )
{

    if ( mIsLocal->isChecked () ) {
        mIsKapiFileL->setChecked( mIsKapiFileR->isChecked() );
    } else {
        mIsKapiFileR->setChecked( mIsKapiFileL->isChecked() );
    }
  
    if ( mIsLocal->isChecked () )
        localFileWidget->show();
    else
        localFileWidget->hide();

    if ( mIsNotLocal->isChecked () )
        remoteFileWidget->show();
    else
        remoteFileWidget->hide();

    if ( mIsPhone->isChecked () ) {
        phoneWidget->show();
    }
    else {
        phoneWidget->hide();
    } 
    
    if ( mIsPi->isChecked () || mIsPiSpecific->isChecked () ) {
        piWidget->show();
        if (mIsPiSpecific->isChecked () ) {
            mTableBox->show();
        }
        else {
            mTableBox->hide();
        } 
    }
    else {
        piWidget->hide();
    } 
   
   
}
void KSyncPrefsDialog::deleteProfile()
{
    //qDebug("KSyncPrefsDialog::deleteProfile() "); 
    if ( currentSelection >= 0 ) {
        if ( currentSelection < 3 ) {
            KMessageBox::error(this,i18n("This profil cannot be deleted!\n"),i18n("KO/Pi config error"));
            return;
        }
        KSyncProfile* temp = mSyncProfiles.at(currentSelection);
        mSyncProfiles.remove( temp );
        mSyncProfileNames.remove( mSyncProfileNames.at( currentSelection ));
        insertProfiles();
    }
}

void KSyncPrefsDialog::saveProfile()
{
    KSyncProfile* prof;
    if ( currentSelection >= 0 ) {
        prof = mSyncProfiles.at(currentSelection) ;

        prof->setRemotePw( mRemotePw->text());
        prof->setRemoteIP( mRemoteIP->text());
        prof->setRemotePort( mRemotePort->text());
        
        prof->setRemotePwAB( mRemotePwAB->text());
        prof->setRemoteIPAB( mRemoteIPAB->text());
        prof->setRemotePortAB( mRemotePortAB->text());
        
        prof->setRemotePwPWM( mRemotePwPWM->text());
        prof->setRemoteIPPWM( mRemoteIPPWM->text());
        prof->setRemotePortPWM( mRemotePortPWM->text());

        prof->setPreSyncCommand( mRemotePrecommand->text());
        prof->setPostSyncCommand( mRemotePostcommand->text() );
        prof->setLocalTempFile( mLocalTempFile->text());
        prof->setRemoteFileName( mRemoteFile->text() );
        prof->setPreSyncCommandAB( mRemotePrecommandAB->text());
        prof->setPostSyncCommandAB( mRemotePostcommandAB->text() );
        prof->setLocalTempFileAB( mLocalTempFileAB->text());
        prof->setRemoteFileNameAB( mRemoteFileAB->text() );
        prof->setPreSyncCommandPWM( mRemotePrecommandPWM->text());
        prof->setPostSyncCommandPWM( mRemotePostcommandPWM->text() );
        prof->setLocalTempFilePWM( mLocalTempFilePWM->text());
        prof->setRemoteFileNamePWM( mRemoteFilePWM->text() );
        prof->setShowSummaryAfterSync( mShowSummaryAfterSync->isChecked() );
        prof->setAskForPreferences( mAskForPreferences->isChecked());
        prof->setWriteBackExisting(mWriteBackExisting->isChecked() );
        prof->setWriteBackFile( mWriteBackFile->isChecked());
        prof->setIncludeInRingSync( mIncludeInRing->isChecked() );
        prof->setIncludeInRingSyncAB( mIncludeInRingAB->isChecked() );
        prof->setIncludeInRingSyncPWM( mIncludeInRingPWM->isChecked() );
        int syncprefs = rem->isChecked()*1+newest->isChecked()*2+  ask->isChecked()*3+  f_loc->isChecked()*4+  f_rem->isChecked()*5 ;//+  both->isChecked()*6 ;
        prof->setSyncPrefs( syncprefs);
        prof->setIsLocalFileSync( mIsLocal->isChecked() );
        prof->setIsPhoneSync( mIsPhone->isChecked() );
        prof->setIsPiSync( mIsPi->isChecked() );
        prof->setIsPiSyncSpec( mIsPiSpecific->isChecked() );
        prof->setIsKapiFile( mIsKapiFileL->isChecked() );
        prof->setWriteBackFuture(mWriteBackFuture->isChecked());
        prof->setWriteBackFutureWeeks(mWriteBackFutureWeeks->value());
        prof->setWriteBackPastWeeks(mWriteBackPastWeeks->value());
        if ( mWriteContactToSIM )
            prof->setWriteContactToSIM(mWriteContactToSIM->isChecked());
        prof->setPhoneDevice(  mPhoneDevice->text() );
        prof->setPhoneConnection(  mPhoneConnection->text() );
        prof->setPhoneModel(  mPhoneModel->text() );
        prof->setFilterInCal ( mFilterInCal->currentText ());
        prof->setFilterOutCal ( mFilterOutCal ->currentText ());
        prof->setFilterInAB ( mFilterInAB ->currentText ());
        prof->setFilterOutAB ( mFilterOutAB ->currentText ());
        mProfileBox->setFocus();
        if ( mIsPiSpecific->isChecked() ) {

            QStringList res = mResourcesKopi;
            int i;
            for ( i = 0;i < mResourcesKopi.count(); ++i ) {
                res.append( mResTableKopi->text( i, 0 ));
            }
            prof->setResSpecKopi( res.join(":"));
            res = mResourcesKapi;
            for ( i = 0;i < mResourcesKapi.count(); ++i ) {
                res.append( mResTableKapi->text( i, 0 ));
            }
            prof->setResSpecKapi( res.join(":"));
        }
    }
}

void KSyncPrefsDialog::insertProfiles()
{
    int curItem = mProfileBox->currentItem();
    mProfileBox->blockSignals( true );
    mProfileBox->clear();
    mProfileBox->insertStringList (mSyncProfileNames ); 
    int item = mSyncProfileNames.count() -1; 
    if ( curItem >= 0 &&  mSyncProfileNames.count() > 0 && curItem < mSyncProfileNames.count() )
        mProfileBox->setCurrentItem( curItem );  
    else if ( item >= 0 ) {
        mProfileBox->setCurrentItem( item );
    }
    currentSelection = -1;
    if ( mSyncProfileNames.count() > 0 ) {
        //qDebug(" profileChanged( mProfileBox->currentItem()  ");
        profileChanged( mProfileBox->currentItem() );
        currentSelection = mProfileBox->currentItem();
    }
    mProfileBox->blockSignals( false );
}
   
void KSyncPrefsDialog::addProfile ( KSyncProfile* temp )
{
    saveProfile();
    mSyncProfiles.append( temp );
    mSyncProfileNames << temp->getName();
    insertProfiles();
    int last = mProfileBox->count() -1;
    mProfileBox->blockSignals( true );
    mProfileBox->setCurrentItem( last );
    mProfileBox->blockSignals( false );
    profileChanged(last);
}
void KSyncPrefsDialog::newProfile()
{
    addProfile ( new KSyncProfile () );
}

void KSyncPrefsDialog::cloneProfile()
{ 
    if ( currentSelection >= 0 ) 
        addProfile (mSyncProfiles.at(currentSelection)->clone()) ;
    else
        newProfile();
}

void KSyncPrefsDialog::setLocalMachineName ( const QString& name )
{
    mMyMachineName->setText( name );

}
QString KSyncPrefsDialog::getLocalMachineName ( )
{
    return mMyMachineName->text();
}

QStringList KSyncPrefsDialog::getSyncProfileNames()
{
    return mSyncProfileNames;
}
void KSyncPrefsDialog::usrReadConfig()
{ 
    //KConfig *config = KOGlobals::config();
    KConfig config ( locateLocal( "config","ksyncprofilesrc"  ) ); 
    config.setGroup("General");
    mSyncProfileNames =config.readListEntry("SyncProfileNames");
    mMyMachineName->setText(config.readEntry("LocalMachineName","undefined"));
    int i;
    KSyncProfile* temp ;
    mSyncProfiles.clear();
    for ( i = 0; i < mSyncProfileNames.count();++i ) {
        temp = new KSyncProfile ();
        temp->setName( mSyncProfileNames[i] );
        temp->readConfig( &config );
        mSyncProfiles.append( temp );
    }
    insertProfiles();
    //mMyMachineName->setText(KOPrefs::instance()->mLocalMachineName );
}


void KSyncPrefsDialog::usrWriteConfig()
{
    saveProfile();
    if ( currentSelection >= 0 ) 
        profileChanged(currentSelection);
    //KConfig *config = KOGlobals::config();
    KConfig config ( locateLocal( "config","ksyncprofilesrc"  ) ); 
    KSyncProfile* prof = mSyncProfiles.first();
    QStringList externalSyncProfileNames;
    externalSyncProfileNames.append("Sharp_DTM");;
    while ( prof ) {
        prof->writeConfig(&config);
        if ( prof->getIsPhoneSync( ) )
            externalSyncProfileNames.append(prof->getName( ) );
        prof = mSyncProfiles.next();
    }
    //KOPrefs::instance()->mSyncProfileNames = mSyncProfileNames;
    //KOPrefs::instance()->mLocalMachineName = mMyMachineName->text();
    config.setGroup("General");
    config.writeEntry("SyncProfileNames",mSyncProfileNames);
    config.writeEntry("ExternSyncProfiles",externalSyncProfileNames);
    config.writeEntry("LocalMachineName",mMyMachineName->text());
    config.sync();
}

void  KSyncPrefsDialog::helpDevice()
{
    QString hint = i18n("Insert device where\nphone is connected. E.g.:\n");
#ifdef _WIN32_
    hint += "Leave empty for Irda.\n"
        "com1:\n(first serial port)\n"
        "usb not supported\n"
        "???\n(bluetooth device address)\n";

#else
    hint += "/dev/ircomm\n(Irda)\n"
        "/dev/ttyS0\n(first serial port)\n"
        "/dev/ttyUSB0\n(first device usb port)\n"
        "???\n(bluetooth device address)\n";
#endif
    KMessageBox::information(this,hint,i18n("KDE-Pim sync config"));
}
void  KSyncPrefsDialog::helpModel()
{
    QString hint = i18n("Recommended: Leave empty!\n(Such that model can\nbe auto detected)\nOr insert name of model:\n");
    hint += "E.g. for Nokia 6310i:\n6310i\nAlso possible:\nobex\nfor Obex connection";
    KMessageBox::information(this,hint,i18n("KDE-Pim sync config"));

}
void  KSyncPrefsDialog::helpConnection()
{
    QString hint = i18n("Insert kind of connection,e.g.:\n");
    hint += "irda | Nokia FBUS over infrared\n"
        "irdaat | AT commands infrared\n(for Siemens/Sony-Erricsson)\n"
        "irdaobex | set also model as obex\n"
        "fbus | Nokia FBUS2 serial\n";
    KMessageBox::information(this,hint,i18n("KDE-Pim sync config"));
}
