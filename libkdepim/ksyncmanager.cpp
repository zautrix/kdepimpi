/*
  This file is part of KDE-Pim/Pi.
  Copyright (c) 2004 Ulf Schenk

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

// $Id: ksyncmanager.cpp,v 1.78 2010/03/26 23:25:45 zautrix Exp $

#include "ksyncmanager.h"

#include <stdlib.h>

#ifndef _WIN32_
#include <unistd.h>
#endif


#include "ksyncprofile.h"
#include "ksyncprefsdialog.h"
#include "kpimprefs.h"

#include <qdir.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qtextcodec.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qapplication.h>

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kmessagebox.h> 

QDateTime KSyncManager::mRequestedSyncEvent;


KSyncManager::KSyncManager(QWidget* parent, KSyncInterface* implementation, TargetApp ta, KPimPrefs* prefs, QPopupMenu* syncmenu)
    : QObject(),  mPrefs(prefs ), mParent(parent),mImplementation(implementation), mTargetApp(ta), mSyncMenu(syncmenu)
{
    mServerSocket = 0;
    bar = new QProgressBar ( 1, 0 );
    bar->setCaption ("");
    mWriteBackInPast = 2;
    
    mSyncFailed = false;

}

KSyncManager::~KSyncManager()
{
    delete bar;
}
 
void KSyncManager::setDefaultFileName( QString s)
{
    mDefFileName = s ;
    if ( mPrefs->mPassiveSyncAutoStart )
      enableQuick( false );
}

void KSyncManager::fillSyncMenu()
{
    if ( mSyncMenu->count() )
        mSyncMenu->clear();
    
    mSyncMenu->insertItem( i18n("Configure..."), 0 );
    mSyncMenu->insertSeparator();
    QPopupMenu *clearMenu = new QPopupMenu ( mSyncMenu );
    mSyncMenu->insertItem( i18n("Remove sync info"),clearMenu, 5000 );
    clearMenu->insertItem( i18n("For all profiles"), 1 );
    clearMenu->insertSeparator();
    connect ( clearMenu, SIGNAL( activated ( int )  ), this, SLOT (slotClearMenu( int ) ) );
    mSyncMenu->insertSeparator();
    if ( mServerSocket == 0 ) {
        mSyncMenu->insertItem( i18n("Enable Pi-Sync"), 2 );
    } else {
        mSyncMenu->insertItem( i18n("Disable Pi-Sync"), 3 );
    }
    mSyncMenu->insertSeparator();
    mSyncMenu->insertItem( i18n("Multiple sync"), 1 );
    mSyncMenu->insertSeparator();
    KConfig config ( locateLocal( "config","ksyncprofilesrc"  ) );
    config.setGroup("General");
    QStringList prof = config.readListEntry("SyncProfileNames");
    mLocalMachineName = config.readEntry("LocalMachineName","undefined");
    if ( prof.count() < 2 ) {
        prof.clear();
        QString externalName;
#ifdef DESKTOP_VERSION
#ifdef _WIN32_
        externalName = "OutLook";
#else
        externalName = "KDE_Desktop";
#endif
#else
        externalName = "Sharp_DTM";
#endif
        prof << externalName;
        prof << i18n("Local_file");
        prof << i18n("Last_file");
        KSyncProfile* temp = new KSyncProfile ();
        temp->setName( prof[0] );
        temp->writeConfig(&config);
        temp->setName( prof[1] );
        temp->writeConfig(&config);
        temp->setName( prof[2] );
        temp->writeConfig(&config);
        config.setGroup("General");
        config.writeEntry("SyncProfileNames",prof);
        config.writeEntry("ExternSyncProfiles",externalName);
        config.sync();
        delete temp;
    }
    mExternSyncProfiles = config.readListEntry("ExternSyncProfiles");
    mSyncProfileNames = prof;
    unsigned int i;
    for ( i = 0; i < prof.count(); ++i ) {
        QString insertText = prof[i];
        if ( i == 0 ) {
#ifdef DESKTOP_VERSION
#ifdef _WIN32_
        insertText = "OutLook";
#else
        insertText = "KDE_Desktop";
#endif
#else
        insertText = "Sharp_DTM";
#endif
        }
        mSyncMenu->insertItem(  insertText, 1000+i ); 
        clearMenu->insertItem(  insertText, 1000+i ); 
        if ( i == 2 )
            mSyncMenu->insertSeparator();
    }
    QDir app_dir;
    //US do not display SharpDTM if app is pwmpi, or no sharpfiles available
    if ( mTargetApp == PWMPI) {
        mSyncMenu->removeItem( 1000 );
        clearMenu->removeItem( 1000 );
    }
#ifndef DESKTOP_VERSION
    else if (!app_dir.exists(QDir::homeDirPath()+"/Applications/dtm" ) ) {
        mSyncMenu->removeItem( 1000 );
        clearMenu->removeItem( 1000 );
    }
#endif
    mSyncMenu->removeItem( 1002 );
    clearMenu->removeItem( 1002 );
}
void KSyncManager::slotClearMenu( int action )
{
    QString syncDevice;
    if ( action > 999 ) {
        syncDevice = mSyncProfileNames[action - 1000] ;
    }



    int result = 0;
    QString sd;
    if ( syncDevice.isEmpty() )
        sd = i18n("Do you want to\nclear all sync info\nof all profiles?");
    else
        sd = i18n("Do you want to\nclear the sync\ninfo of profile\n%1?\n"). arg( syncDevice );

    result = QMessageBox::warning( mParent, i18n("Warning!"),sd,i18n("OK"), i18n("Cancel"), 0,
                                   0, 1 );
    if ( result )
        return;
    mImplementation->removeSyncInfo( syncDevice );
}
void KSyncManager::slotSyncMenu( int action )
{
    qDebug("KSM::syncaction %d ", action);
    mCurrentResourceLocal = "";  
    emit multiResourceSyncStart( false );
    if ( action == 5000 )
        return;
    mSyncWithDesktop = false;
    if ( action == 0 ) {

        // seems to be a Qt2 event handling bug
        // syncmenu.clear causes a segfault at first time
        // when we call it after the main event loop, it is ok
        // same behaviour when calling OM/Pi via QCOP for the first time
        QTimer::singleShot ( 1, this, SLOT ( confSync() ) ); 
        //confSync();

        return;
    }
    if ( action == 1 ) {
        multiSync( true );
        return;
    }
    if ( action == 2 ) {
        enableQuick();
        QTimer::singleShot ( 1, this, SLOT ( fillSyncMenu() ) ); 
        return;
    }
    if ( action == 3 ) {
        delete mServerSocket;
        mServerSocket = 0;
        QTimer::singleShot ( 1, this, SLOT ( fillSyncMenu() ) ); 
        return;
    }

    if (blockSave())
        return;

    setBlockSave(true);
    bool silent = false;
    if ( action == 999 ) {
        //special mode for silent syncing
        action = 1000;
        silent = true;
    }

    mCurrentSyncProfile = action - 1000 ;
    mCurrentSyncDevice = mSyncProfileNames[mCurrentSyncProfile] ;
    mCurrentSyncName =  mLocalMachineName ;
    KConfig config ( locateLocal( "config","ksyncprofilesrc"  ) );
    KSyncProfile* temp = new KSyncProfile ();
    temp->setName(mSyncProfileNames[mCurrentSyncProfile]);
    temp->readConfig(&config);
    if (silent) {
        mAskForPreferences = false;
        mShowSyncSummary = false;
        mWriteBackFile = true;
        mSyncAlgoPrefs = 2;// take newest
    }
    else {
        mAskForPreferences = temp->getAskForPreferences();
        mShowSyncSummary = temp->getShowSummaryAfterSync();
        mWriteBackFile = temp->getWriteBackFile();
        mSyncAlgoPrefs = temp->getSyncPrefs();
    }
    mWriteBackExistingOnly = temp->getWriteBackExisting();
    mIsKapiFile = temp->getIsKapiFile();
    mWriteBackInFuture = 0;
    if ( temp->getWriteBackFuture() ) {
        mWriteBackInFuture =  temp->getWriteBackFutureWeeks( );
        mWriteBackInPast =  temp->getWriteBackPastWeeks( );
    }
    mFilterInCal = temp->getFilterInCal();
    mFilterOutCal = temp->getFilterOutCal();
    mFilterInAB = temp->getFilterInAB();
    mFilterOutAB = temp->getFilterOutAB();

    if ( action == 1000  ) {
        mIsKapiFile = false; 
#ifdef DESKTOP_VERSION
        syncKDE();
#else
        syncSharp();
#endif
    
    } else if ( action == 1001 ) {
        syncLocalFile();

    } else if ( action == 1002 ) {
        mWriteBackFile = false;
        mAskForPreferences = false;
        mShowSyncSummary = false; 
        mSyncAlgoPrefs = 3; 
        quickSyncLocalFile();

    } else if ( action >= 1003  ) {
        if ( temp->getIsLocalFileSync() ) {
            switch(mTargetApp)
                {
                case (KAPI):
                    if ( syncWithFile( temp->getRemoteFileNameAB( ), false ) )
                        mPrefs->mLastSyncedLocalFile = temp->getRemoteFileNameAB();
                    break;
                case (KOPI):
                    if ( syncWithFile( temp->getRemoteFileName( ), false ) )
                        mPrefs->mLastSyncedLocalFile = temp->getRemoteFileName();
                    break;
                case (PWMPI):
                    if ( syncWithFile( temp->getRemoteFileNamePWM( ), false ) )
                        mPrefs->mLastSyncedLocalFile = temp->getRemoteFileNamePWM();
                    break;
                default:
                    qDebug("KSM::slotSyncMenu: invalid apptype selected");
                    break;
	      
                }
        } else {
            if (  temp->getIsPhoneSync() ) {
                mPhoneDevice = temp->getPhoneDevice( ) ;
                mPhoneConnection = temp->getPhoneConnection( );
                mPhoneModel = temp->getPhoneModel( );
                syncPhone();
            } else if (  temp->getIsPiSync()|| temp->getIsPiSyncSpec()) {
                mSpecificResources.clear();
                if ( mTargetApp == KAPI ) {
                    if ( temp->getIsPiSyncSpec() )
                        mSpecificResources = QStringList::split( ":", temp->getResSpecKapi(),true );
                    mPassWordPiSync = temp->getRemotePwAB();
                    mActiveSyncPort = temp->getRemotePortAB();
                    mActiveSyncIP = temp->getRemoteIPAB();
                } else if ( mTargetApp == KOPI ) {
                    if ( temp->getIsPiSyncSpec() )
                        mSpecificResources = QStringList::split( ":", temp->getResSpecKopi(),true );
                    mPassWordPiSync = temp->getRemotePw();
                    mActiveSyncPort = temp->getRemotePort();
                    mActiveSyncIP = temp->getRemoteIP();
                } else  {
                    mPassWordPiSync = temp->getRemotePwPWM();
                    mActiveSyncPort = temp->getRemotePortPWM();
                    mActiveSyncIP = temp->getRemoteIPPWM();
                } 
                syncPi();
                while ( !mPisyncFinished ) {
                    //qDebug("waiting ");
                    qApp->processEvents();
                }
            } else
                syncRemote( temp );

        }
    }
    delete temp;
    setBlockSave(false);
}

void KSyncManager::enableQuick( bool ask )
{
    bool autoStart;
    bool changed = false;
    if ( ask ) {
        QDialog dia ( 0, "input-dialog", true );
        QLineEdit lab ( &dia );
        QVBoxLayout lay( &dia );
        lab.setText( mPrefs->mPassiveSyncPort );
        lay.setMargin(7);
        lay.setSpacing(7);
        int po = 9197+mTargetApp;
        QLabel label ( i18n("Port number (Default: %1)\nValid range from 1 to 65535").arg(po), &dia );
        lay.addWidget( &label);
        lay.addWidget( &lab);

        QLineEdit lepw ( &dia );
        lepw.setText( mPrefs->mPassiveSyncPw );
        QLabel label2 ( i18n("Password to enable\naccess from remote:"), &dia );
        lay.addWidget( &label2);
        lay.addWidget( &lepw);
        QCheckBox autostart(i18n("Automatically start\nat application startup"), &dia );
        lay.addWidget( &autostart);
        autostart.setChecked( mPrefs->mPassiveSyncAutoStart );
#ifdef DESKTOP_VERSION
#ifdef _WIN32_
        QCheckBox syncdesktop( i18n("Automatically sync with Outlook\nwhen receiving sync request"),&dia );
        syncdesktop.hide();// not implemented!
#else
        QCheckBox syncdesktop( i18n("Automatically sync with KDE-Desktop\nwhen receiving sync request"),&dia );
#endif
        lay.addWidget( &syncdesktop);
#else
        mPrefs->mPassiveSyncWithDesktop = false;
        QCheckBox syncdesktop( i18n("Automatically sync\nwith KDE-Desktop"),&dia );
        syncdesktop.hide();
#endif
        syncdesktop.setChecked( mPrefs->mPassiveSyncWithDesktop );

        QPushButton pb ( "OK",  &dia);
        lay.addWidget( &pb );
        connect(&pb, SIGNAL( clicked() ), &dia, SLOT ( accept() ) );
        dia.resize( 230,120 );
        dia.setCaption( i18n("Enter port for Pi-Sync") );
        dia.show();
#ifndef DESKTOP_VERSION
        int dw = QApplication::desktop()->width();
        int dh = QApplication::desktop()->height();
        dia.move( (dw-dia.width())/2, (dh - dia.height() )/2 );
#endif
        if ( ! dia.exec() )
            return;
        dia.hide();
        qApp->processEvents();
        if ( mPrefs->mPassiveSyncPw != lepw.text() ) {
            changed = true;
            mPrefs->mPassiveSyncPw = lepw.text();
        }
        if ( mPrefs->mPassiveSyncPort != lab.text() ) {
            mPrefs->mPassiveSyncPort = lab.text();
            changed = true;
        }
        autoStart = autostart.isChecked();
        if (mPrefs->mPassiveSyncWithDesktop != syncdesktop.isChecked() ) {
            changed = true;
            mPrefs->mPassiveSyncWithDesktop = syncdesktop.isChecked();
        }
    }
    else
        autoStart = mPrefs->mPassiveSyncAutoStart;
    if ( autoStart != mPrefs->mPassiveSyncAutoStart )
        changed =  true;
    bool ok;
    mPrefs->mPassiveSyncAutoStart = false;
    Q_UINT32 port_t = mPrefs->mPassiveSyncPort.toUInt(&ok);
    if ( ! ok || port_t > 65535 ) {
        KMessageBox::information( 0, i18n("No valid port number:\n%1").arg ( mPrefs->mPassiveSyncPort ), i18n("Pi-Sync Port Error"));
        return;
    }
    Q_UINT16 port = port_t;
    //qDebug("port %d ", port);
    mServerSocket = new KServerSocket ( mPrefs->mPassiveSyncPw, port ,1 );
    mServerSocket->setFileName( defaultFileName() );//bbb
    if ( !mServerSocket->ok() ) {
        QTimer::singleShot( 2000, this, SLOT ( displayErrorPort() ) );
        delete mServerSocket;
        mServerSocket = 0;
        return;
    }
    mPrefs->mPassiveSyncAutoStart = autoStart;
    if ( changed ) {
        mPrefs->writeConfig();
    }
    //connect( mServerSocket, SIGNAL ( request_file() ),this, SIGNAL  ( request_file() ) );
    //connect( mServerSocket, SIGNAL ( file_received( bool ) ), this,  SIGNAL  ( getFile( bool ) ) );
    connect( mServerSocket, SIGNAL ( request_file(const QString &) ),this, SIGNAL  ( request_file(const QString &) ) );
    connect( mServerSocket, SIGNAL ( file_received( bool ,const QString &) ), this,  SIGNAL  ( getFile( bool,const QString & ) ) );
}
void KSyncManager::displayErrorPort()
{
    KMessageBox::information( 0, i18n("<b>Enabling Pi-Sync failed!</b> Failed to bind or listen to the port %1! Is another instance already listening to that port?").arg( mPrefs->mPassiveSyncPort) , i18n("Pi-Sync Port Error"));
}
void KSyncManager::syncLocalFile()
{

    QString fn =mPrefs->mLastSyncedLocalFile;
    QString ext;

    switch(mTargetApp)
        {
	    case (KAPI):
            ext = "(*.vcf)";
            break;
	    case (KOPI):
            ext = "(*.ics/*.vcs)";
            break;
	    case (PWMPI):
            ext = "(*.pwm)";
            break;
	    default:
            qDebug("KSM::syncLocalFile: invalid apptype selected");
            break;
	      
        }

    fn =KFileDialog:: getOpenFileName( fn, i18n("Sync filename"+ext), mParent );
    if ( fn == "" )
        return;
    if (  syncWithFile(  fn, false ) ) {
        qDebug("KSM::syncLocalFile() successful ");
    }

}

bool  KSyncManager::syncWithFile( QString fn , bool quick )
{
    bool ret = false;
    QFileInfo info;
    info.setFile( fn );
    QString mess;
    if ( !info. exists() ) {
        mess =  i18n( "Sync file \n...%1\ndoes not exist!\nNothing synced!\n").arg(fn.right( 30) );
        QMessageBox::warning( mParent, i18n("Warning!"),
                                           mess );
        return ret;
    }
    int result = 0;
    if ( !quick ) {
        mess =  i18n("Sync with file \n...%1\nfrom:\n%2\n").arg(fn.right( 25)).arg(KGlobal::locale()->formatDateTime(info.lastModified (), true, false ));
        result = QMessageBox::warning( mParent, i18n("Warning!"),
                                       mess,
                                       i18n("Sync"), i18n("Cancel"), 0,
                                       0, 1 );
        if ( result )
            return false;
    }
    if ( mAskForPreferences )
        if ( !edit_sync_options()) {
            mParent->topLevelWidget()->setCaption( i18n("Syncing aborted. Nothing synced.") );
            return false;
        }
    if ( result == 0 ) {
        //qDebug("Now sycing ... ");
        if ( ret = mImplementation->sync( this, fn, mSyncAlgoPrefs ,mCurrentResourceLocal ) )
            mParent->topLevelWidget()->setCaption( i18n("Synchronization successful") );
        else
            mParent->topLevelWidget()->setCaption( i18n("Sync cancelled or failed.") );
        if ( ! quick )
            mPrefs->mLastSyncedLocalFile = fn;
    }
    return ret;
}

void KSyncManager::quickSyncLocalFile()
{
    
    if ( syncWithFile( mPrefs->mLastSyncedLocalFile, true ) ) {
        qDebug("KSM::quick syncLocalFile() successful ");
      
    }
}

void KSyncManager::multiSync( bool askforPrefs  )
{
    if (blockSave())
        return;
    setBlockSave(true);
    mCurrentResourceLocal = "";
    if ( askforPrefs ) {
        QString question = i18n("Do you really want\nto multiple sync\nwith all checked profiles?\nSyncing takes some\ntime - all profiles\nare synced twice!");
        if ( QMessageBox::information( mParent, i18n("KDE-Pim Sync"),
                                       question,
                                       i18n("Yes"), i18n("No"),
                                       0, 0 ) != 0 ) {
            setBlockSave(false);
            mParent->topLevelWidget()->setCaption(i18n("Aborted! Nothing synced!"));
            return;
        }
    }
    mCurrentSyncDevice = i18n("Multiple profiles") ;
    mSyncAlgoPrefs = mPrefs->mRingSyncAlgoPrefs;
    if ( askforPrefs ) {
        if ( !edit_sync_options()) {
            mParent->topLevelWidget()->setCaption( i18n("Syncing aborted.") );
            return;
        }
        mPrefs->mRingSyncAlgoPrefs = mSyncAlgoPrefs;
    }
    mParent->topLevelWidget()->setCaption(i18n("Multiple sync started.") );
    qApp->processEvents();
    int num = ringSync() ;
    if (  num > 1 )
        ringSync();
    setBlockSave(false);
    if ( num )
        emit save();
    if ( num )
        mParent->topLevelWidget()->setCaption(i18n("%1 profiles synced. Multiple sync complete!").arg(num) );
    else
        mParent->topLevelWidget()->setCaption(i18n("Nothing synced! No profiles defined for multisync!"));
    return;
}

int KSyncManager::ringSync()
{
    emit multiResourceSyncStart( false );
    int syncedProfiles = 0;
    unsigned int i;
    QTime timer;
    KConfig config ( locateLocal( "config","ksyncprofilesrc"  ) );
    QStringList syncProfileNames = mSyncProfileNames;
    KSyncProfile* temp = new KSyncProfile ();
    mAskForPreferences = false;
    mCurrentResourceLocal = "";
    for ( i = 0; i < syncProfileNames.count(); ++i ) {
        mCurrentSyncProfile = i;
        temp->setName(syncProfileNames[mCurrentSyncProfile]);
        temp->readConfig(&config);

        bool includeInRingSync = false;
        switch(mTargetApp)
            {
            case (KAPI):
                includeInRingSync = temp->getIncludeInRingSyncAB();
                break;
            case (KOPI):
                includeInRingSync = temp->getIncludeInRingSync();
                break;
            case (PWMPI):
                includeInRingSync = temp->getIncludeInRingSyncPWM();
                break;
            default:
                qDebug("KSM::ringSync: invalid apptype selected");
                break;
	      
            }
        
	
        if ( includeInRingSync && ( i < 1 || i > 2  )) {
            mParent->topLevelWidget()->setCaption(i18n("Profile ")+syncProfileNames[mCurrentSyncProfile]+ i18n(" is synced ... "));
            ++syncedProfiles; 
            mSyncWithDesktop = false; 
            // mAskForPreferences = temp->getAskForPreferences();
            mWriteBackFile = temp->getWriteBackFile();
            mWriteBackExistingOnly = temp->getWriteBackExisting();
            mIsKapiFile = temp->getIsKapiFile();
            mWriteBackInFuture = 0;
            if ( temp->getWriteBackFuture() ) {
                mWriteBackInFuture =  temp->getWriteBackFutureWeeks( );
                mWriteBackInPast =  temp->getWriteBackPastWeeks( );
            }
            mFilterInCal = temp->getFilterInCal();
            mFilterOutCal = temp->getFilterOutCal();
            mFilterInAB = temp->getFilterInAB();
            mFilterOutAB = temp->getFilterOutAB();
            mShowSyncSummary = false;
            mCurrentSyncDevice = syncProfileNames[i] ;
            mCurrentSyncName = mLocalMachineName;
            if ( i == 0 ) {
                mIsKapiFile = false; 
#ifdef DESKTOP_VERSION
                syncKDE();
#else
                syncSharp();
#endif
            } else {
                if ( temp->getIsLocalFileSync() ) {
                    switch(mTargetApp)
                        {
                        case (KAPI):
                            if ( syncWithFile( temp->getRemoteFileNameAB( ), false ) )
                                mPrefs->mLastSyncedLocalFile = temp->getRemoteFileNameAB();
                            break;
                        case (KOPI):
                            if ( syncWithFile( temp->getRemoteFileName( ), false ) )
                                mPrefs->mLastSyncedLocalFile = temp->getRemoteFileName();
                            break;
                        case (PWMPI):
                            if ( syncWithFile( temp->getRemoteFileNamePWM( ), false ) )
                                mPrefs->mLastSyncedLocalFile = temp->getRemoteFileNamePWM();
                            break;
                        default:
                            qDebug("KSM: invalid apptype selected");
                            break;
                        }
                } else {
                    if (  temp->getIsPhoneSync() ) {
                        mPhoneDevice = temp->getPhoneDevice( ) ;
                        mPhoneConnection = temp->getPhoneConnection( );
                        mPhoneModel = temp->getPhoneModel( );
                        syncPhone();
                    } else if (  temp->getIsPiSync() || temp->getIsPiSyncSpec()) {
                        mSpecificResources.clear();
                        if ( mTargetApp == KAPI ) {
                            if ( temp->getIsPiSyncSpec() )
                                mSpecificResources = QStringList::split( ":", temp->getResSpecKapi(),true );
                            mPassWordPiSync = temp->getRemotePwAB();
                            mActiveSyncPort = temp->getRemotePortAB();
                            mActiveSyncIP = temp->getRemoteIPAB();
                        } else if ( mTargetApp == KOPI ) {
                            if ( temp->getIsPiSyncSpec() )
                                mSpecificResources = QStringList::split( ":", temp->getResSpecKopi(),true );
                            mPassWordPiSync = temp->getRemotePw();
                            mActiveSyncPort = temp->getRemotePort();
                            mActiveSyncIP = temp->getRemoteIP();
                        } else  {
                            mPassWordPiSync = temp->getRemotePwPWM();
                            mActiveSyncPort = temp->getRemotePortPWM();
                            mActiveSyncIP = temp->getRemoteIPPWM();
                        } 
                        syncPi();
                        while ( !mPisyncFinished ) {
                            //qDebug("waiting ");
                            qApp->processEvents();
                        }
                        timer.start(); 
                        while ( timer.elapsed () < 2000 ) {
                            qApp->processEvents();
                        }
                    } else
                        syncRemote( temp, false );

                }
            }
            timer.start();
            mParent->topLevelWidget()->setCaption(i18n("Multiple sync in progress ... please wait!") );
            while ( timer.elapsed () < 2000 ) {
                qApp->processEvents();
#ifndef _WIN32_
                sleep (1);
#endif
            }

        }

    }
    delete temp;
    return syncedProfiles;
}

void KSyncManager::syncRemote( KSyncProfile* prof,  bool ask)
{
    QString question;
    if ( ask ) {
        question = i18n("Do you really want\nto remote sync\nwith profile \n")+ prof->getName()+" ?\n";
        if ( QMessageBox::information( mParent, i18n("Sync"),
                                       question,
                                       i18n("Yes"), i18n("No"),
                                       0, 0 ) != 0 )
            return;
    }

    QString preCommand;
    QString localTempFile;
    QString postCommand;

    switch(mTargetApp)
        {
        case (KAPI):
            preCommand = prof->getPreSyncCommandAB();
            postCommand = prof->getPostSyncCommandAB();
            localTempFile = prof->getLocalTempFileAB();
            break;
        case (KOPI):
            preCommand = prof->getPreSyncCommand();
            postCommand = prof->getPostSyncCommand();
            localTempFile = prof->getLocalTempFile();
            break;
        case (PWMPI):
            preCommand = prof->getPreSyncCommandPWM();
            postCommand = prof->getPostSyncCommandPWM();
            localTempFile = prof->getLocalTempFilePWM();
            break;
        default:
            qDebug("KSM::syncRemote: invalid apptype selected");
            break;
        }


    int fi;
    if ( (fi = preCommand.find("$PWD$")) > 0 ) {
        QString pwd = getPassword();
        preCommand = preCommand.left( fi )+ pwd + preCommand.mid( fi+5 );

    }
    int maxlen = 30;
    if ( QApplication::desktop()->width() > 320 )
        maxlen += 25;
    mParent->topLevelWidget()->setCaption ( i18n( "Copy remote file to local machine..." ) );
    int result = system ( preCommand );
    // 0 : okay
    // 256: no such file or dir
    //
    qDebug("KSM::Sync: Remote copy result(0 = okay): %d ",result );
    if ( result != 0 ) {
        unsigned int len = maxlen;
        while ( len <  preCommand.length() ) {
            preCommand.insert( len , "\n" );
            len += maxlen +2;
        }
        question = i18n("Sorry, the copy command failed!\nCommand was:\n%1\n \nTry command on console to get more\ndetailed info about the reason.\n").arg (preCommand) ;
        QMessageBox::information( mParent, i18n("Sync - ERROR"),
                                  question,
                                  i18n("Okay!")) ;
        mParent->topLevelWidget()->setCaption ("KDE-Pim");
        return;
    }
    mParent->topLevelWidget()->setCaption ( i18n( "Copying succeed." ) );
    //qDebug(" file **%s** ",prof->getLocalTempFile().latin1() );

    if ( syncWithFile( localTempFile, true ) ) {

        if ( mWriteBackFile ) {
            int fi;
            if ( (fi = postCommand.find("$PWD$")) > 0 ) {
                QString pwd = getPassword();
                postCommand = postCommand.left( fi )+ pwd + postCommand.mid( fi+5 );
                
            }
            mParent->topLevelWidget()->setCaption ( i18n( "Writing back file ..." ) );
            result = system ( postCommand );
            qDebug("KSM::Sync:Writing back file result: %d ", result);
            if ( result != 0 ) {
                mParent->topLevelWidget()->setCaption ( i18n( "Writing back file result: " )+QString::number( result ) );
                return;
            } else {
                mParent->topLevelWidget()->setCaption ( i18n( "Syncronization sucessfully completed" ) );
            }
        }
    }
    return;
}
bool KSyncManager::edit_pisync_options()
{
    QDialog dia( mParent, "dia", true );
    dia.setCaption( i18n("Pi-Sync options" ) );
    QVBoxLayout lay ( &dia );
    lay.setSpacing( 5 );
    lay.setMargin( 3 );
    QLabel lab0 ( i18n("Remote sync host:"), &dia);  
    lab0.setAlignment( Qt::AlignCenter );
    lay.addWidget( &lab0 );
    QLabel lab01 ( QString("<b>%1</b>").arg ( mCurrentSyncDevice ), &dia);  
    lab01.setAlignment( Qt::AlignCenter );
    lay.addWidget( &lab01 );
    QString lastsynctime = mImplementation->getLastSyncDT4device( mCurrentSyncDevice );
    QLabel lab02 ( i18n("Last sync on:\n%1").arg ( lastsynctime ), &dia);  
    lab02.setAlignment( Qt::AlignCenter );
    lay.addWidget( &lab02 );
    QLabel lab1 ( i18n("Password for remote access:"), &dia); 
    lay.addWidget( &lab1 );
    QLineEdit le1 (&dia );
    lay.addWidget( &le1 );
    QLabel lab2 ( i18n("Remote IP address:"), &dia);
    lay.addWidget( &lab2 );
    QLineEdit le2 (&dia );
    lay.addWidget( &le2 );
    QLabel lab3 ( i18n("Remote port number:\n(May be: 1 - 65535)"), &dia);
    lay.addWidget( &lab3 );
    QLineEdit le3 (&dia );
    lay.addWidget( &le3 );
    QPushButton pb ( i18n("Start syncing"),  &dia); 
    lay.addWidget( &pb );
    connect(&pb, SIGNAL( clicked() ), &dia, SLOT ( accept() ) ); 
    le1.setText( mPassWordPiSync );
    le2.setText( mActiveSyncIP  );
    le3.setText( mActiveSyncPort );
    if ( dia.exec() ) {
        mPassWordPiSync = le1.text();
        mActiveSyncPort = le3.text();
        mActiveSyncIP = le2.text();
        return true;
    }
    return false;
}
bool KSyncManager::edit_sync_options()
{

    QDialog dia( mParent, "dia", true );
    dia.setCaption( i18n("Remote Host: " ) + mCurrentSyncDevice );
    QButtonGroup gr ( 1,  Qt::Horizontal, i18n("Sync preferences"), &dia);
    QVBoxLayout lay ( &dia );
    lay.setSpacing( 2 );
    lay.setMargin( 3 );
    lay.addWidget(&gr);
    QRadioButton loc ( i18n("Take local entry on conflict"), &gr );
    QRadioButton rem ( i18n("Take remote entry on conflict"), &gr );
    QRadioButton newest( i18n("Take newest entry on conflict"), &gr );
    QRadioButton ask( i18n("Ask for every entry on conflict"), &gr );
    QRadioButton f_loc( i18n("Force: Take local entry always"), &gr );
    QRadioButton f_rem( i18n("Force: Take remote entry always"), &gr );
    //QRadioButton both( i18n("Take both on conflict"), &gr );
    QPushButton pb ( i18n("Sync data now!"),  &dia); 
    lay.addWidget( &pb );
    connect(&pb, SIGNAL( clicked() ), &dia, SLOT ( accept() ) ); 
    QPushButton pb2 ( i18n("Cancel syncing\nNo data will be changed"),  &dia); 
    lay.addWidget( &pb2 );
    connect(&pb2, SIGNAL( clicked() ), &dia, SLOT ( reject() ) ); 
    switch ( mSyncAlgoPrefs ) {
    case 0:
        loc.setChecked( true);
        break;
    case 1:
        rem.setChecked( true );
        break;
    case 2:
        newest.setChecked( true);
        break;
    case 3:
        ask.setChecked( true);
        break;
    case 4:
        f_loc.setChecked( true);
        break;
    case 5:
        f_rem.setChecked( true);
        break;
    case 6:
        // both.setChecked( true);
        break; 
    default:
        break;
    }
    if ( dia.exec() ) {
        mSyncAlgoPrefs = rem.isChecked()*1+newest.isChecked()*2+  ask.isChecked()*3+  f_loc.isChecked()*4+  f_rem.isChecked()*5;//+  both.isChecked()*6 ;
        return true;
    }
    return false;
}

QString  KSyncManager::getPassword( )
{
    QString retfile = "";
    QDialog dia ( mParent, "input-dialog", true );
    QLineEdit lab ( &dia );
    lab.setEchoMode( QLineEdit::Password );
    QVBoxLayout lay( &dia );
    lay.setMargin(7);
    lay.setSpacing(7);
    lay.addWidget( &lab);
    dia.setFixedSize( 230,50 );
    dia.setCaption( i18n("Enter password") );
    QPushButton pb ( "OK",  &dia);
    lay.addWidget( &pb );
    connect(&pb, SIGNAL( clicked() ), &dia, SLOT ( accept() ) );
    dia.show();
    int res = dia.exec();
    if ( res )
        retfile = lab.text();
    dia.hide();
    qApp->processEvents();
    return retfile;

}


void KSyncManager::confSync()
{ 
    static KSyncPrefsDialog* sp = 0;
    if ( ! sp ) {
        sp = new KSyncPrefsDialog( mParent, "syncprefs", true );
    }
    sp->usrReadConfig();
#ifndef DESKTOP_VERSION
    sp->showMaximized();
#else
    sp->show();
#endif
    sp->exec();
    QStringList oldSyncProfileNames = mSyncProfileNames;
    mSyncProfileNames = sp->getSyncProfileNames();
    mLocalMachineName = sp->getLocalMachineName ();
    uint ii;
    for ( ii = 0; ii < oldSyncProfileNames.count(); ++ii ) {
        if ( ! mSyncProfileNames.contains( oldSyncProfileNames[ii] ) )
            mImplementation->removeSyncInfo( oldSyncProfileNames[ii]  );
    }
    QTimer::singleShot ( 1, this, SLOT ( fillSyncMenu() ) ); 
}
void KSyncManager::syncOL()
{
  mSyncWithDesktop = true;
    emit save();
    switch(mTargetApp)
        {
        case (KAPI):
            {
                syncExternalApplication("ol");
            }
            break;
        case (KOPI):
            {
#ifdef DESKTOP_VERSION
                QString command = "kdecaldump33";  
                QString commandfile = "kdecaldump33";  
                QString commandpath = qApp->applicationDirPath () + "/";
#else
                QString command = "kdecaldump33";  
                QString commandfile = "kdecaldump33";  
                QString commandpath =  QDir::homeDirPath ()+"/";
#endif
                if ( ! QFile::exists ( commandpath+commandfile ) )
                    command = commandfile;
                else
                    command  = commandpath+commandfile;

                QString fileName = QDir::homeDirPath ()+"/.kdecalendardump.ics";
                int result = system (  command.latin1());
                qDebug("Cal dump 33 command call result result: %d ", result);
                if ( result != 0 ) {
                    qDebug("Calling CAL dump version 33 failed. Trying 34... ");
                    commandfile = "kdecaldump34";  
                    if ( ! QFile::exists ( commandpath+commandfile ) )
                        command = commandfile;
                    else
                        command  = commandpath+commandfile;
                    result = system (  command.latin1());
                    qDebug("Cal dump 34 command call result result: %d ", result);
                    if ( result != 0 ) {
                        KMessageBox::error( 0, i18n("Error accessing KDE calendar data.\nMake sure the file\n%1kdecaldump3x\nexists ( x = 3 or 4 ).\nSupported KDE versions are 3.3 and 3.4.\nUsed version should be auto detected.\n").arg( commandpath ));
                        return;
                    }
                }
                if ( syncWithFile( fileName,true ) ) {
                    if ( mWriteBackFile ) {
                        command += " --read";
                        system (  command.latin1());
                    }
                }
                
            }
            break;
        case (PWMPI):

            break;
        default:
            qDebug("KSM::slotSyncMenu: invalid apptype selected");
            break;
	      
        }
}
void KSyncManager::syncKDE()
{
#ifdef _WIN32_
        syncOL();
#else

    mSyncWithDesktop = true;
    emit save();
    switch(mTargetApp)
        {
        case (KAPI):
            {
#ifdef DESKTOP_VERSION
                QString command = "kdeabdump33";  
                QString commandfile = "kdeabdump33";  
                QString commandpath = qApp->applicationDirPath () + "/";
#else
                QString command = "kdeabdump33";  
                QString commandfile = "kdeabdump33";  
                QString commandpath =  QDir::homeDirPath ()+"/";
#endif
                if ( ! QFile::exists ( commandpath+commandfile ) )
                    command = commandfile;
                else
                    command  = commandpath+commandfile;
               
                QString fileName = QDir::homeDirPath ()+"/.kdeaddressbookdump.vcf";
                int result = system (  command.latin1());
                qDebug("AB dump 33 command call result: %d ", result);
                if ( result != 0 ) {
                    qDebug("Calling AB dump version 33 failed. Trying 34... ");
                    commandfile = "kdeabdump34";  
                    if ( ! QFile::exists ( commandpath+commandfile ) )
                        command = commandfile;
                    else
                        command  = commandpath+commandfile;
                    result = system (  command.latin1());
                    qDebug("AB dump 34 command call result: %d ", result);
                    if ( result != 0 ) {
                        KMessageBox::error( 0, i18n("Error accessing KDE addressbook data.\nMake sure the file\n%1kdeabdump3x\nexists ( x = 3 or 4 ).\nSupported KDE versions are 3.3 and 3.4.\nUsed version should be auto detected.\n").arg( commandpath ));
                        return;
                    }
                }
                if ( syncWithFile( fileName,true ) ) {
                    if ( mWriteBackFile ) {
                        command += " --read";
                        system (  command.latin1());
                    }
                }
                
            }
            break;
        case (KOPI):
            {
#ifdef DESKTOP_VERSION
                QString command = "kdecaldump33";  
                QString commandfile = "kdecaldump33";  
                QString commandpath = qApp->applicationDirPath () + "/";
#else
                QString command = "kdecaldump33";  
                QString commandfile = "kdecaldump33";  
                QString commandpath =  QDir::homeDirPath ()+"/";
#endif
                if ( ! QFile::exists ( commandpath+commandfile ) )
                    command = commandfile;
                else
                    command  = commandpath+commandfile;

                QString fileName = QDir::homeDirPath ()+"/.kdecalendardump.ics";
                int result = system (  command.latin1());
                qDebug("Cal dump 33 command call result result: %d ", result);
                if ( result != 0 ) {
                    qDebug("Calling CAL dump version 33 failed. Trying 34... ");
                    commandfile = "kdecaldump34";  
                    if ( ! QFile::exists ( commandpath+commandfile ) )
                        command = commandfile;
                    else
                        command  = commandpath+commandfile;
                    result = system (  command.latin1());
                    qDebug("Cal dump 34 command call result result: %d ", result);
                    if ( result != 0 ) {
                        KMessageBox::error( 0, i18n("Error accessing KDE calendar data.\nMake sure the file\n%1kdecaldump3x\nexists ( x = 3 or 4 ).\nSupported KDE versions are 3.3 and 3.4.\nUsed version should be auto detected.\n").arg( commandpath ));
                        return;
                    }
                }
                if ( syncWithFile( fileName,true ) ) {
                    if ( mWriteBackFile ) {
                        command += " --read";
                        system (  command.latin1());
                    }
                }
                
            }
            break;
        case (PWMPI):

            break;
        default:
            qDebug("KSM::slotSyncMenu: invalid apptype selected");
            break;
	      
        }
#endif
}

void KSyncManager::syncSharp()
{

    if ( ! syncExternalApplication("sharp") )
        qDebug("KSM::ERROR sync sharp  ");
}

bool KSyncManager::syncExternalApplication(QString  resource)
{
   
    emit save();

    if ( mAskForPreferences )  
        if ( !edit_sync_options()) {
            mParent->topLevelWidget()->setCaption( i18n("Syncing aborted. Nothing synced.") );
            return false;
        }

    qDebug("KSM::Sync extern %s", resource.latin1());

    bool syncOK = mImplementation->syncExternal(this, resource);

    return syncOK;

}

void KSyncManager::syncPhone()
{

    syncExternalApplication("phone");

}

void KSyncManager::showProgressBar(int percentage, QString caption, int total)
{
    if (!bar->isVisible())
        {
            int w = 300;
            if ( QApplication::desktop()->width() < 320 )
                w = 220;
            int h = bar->sizeHint().height() ;
            int dw = QApplication::desktop()->width();
            int dh = QApplication::desktop()->height();
            bar->setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
            bar->setCaption (caption); 
            bar->setTotalSteps (  total  ) ;
            bar->show();
        }
    bar->raise();
    bar->setProgress( percentage );
    qApp->processEvents();
}

void KSyncManager::hideProgressBar()
{
    bar->hide();
    qApp->processEvents();
}

bool KSyncManager::isProgressBarCanceled()
{
    return !bar->isVisible();
}

QString KSyncManager::syncFileName()
{

    QString fn = "tempfile";
    switch(mTargetApp)
        {
        case (KAPI):
            fn = "tempsyncab.vcf";
            break;
        case (KOPI):
            fn = "tempsynccal.ics";
            break;
        case (PWMPI):
            fn = "tempsyncpw.pwm";
            break;
        default:
            break;
        }
#ifdef DESKTOP_VERSION
    return  locateLocal( "tmp", fn );
#else
    return (QString( "/tmp/" )+ fn );
#endif
}

void KSyncManager::syncPi()
{
    mIsKapiFile = true; 
    mPisyncFinished = false;
    qApp->processEvents();
    if ( mAskForPreferences )
        if ( !edit_pisync_options()) {
            mParent->topLevelWidget()->setCaption( i18n("Syncing aborted. Nothing synced.") );
            mPisyncFinished = true;
            return;
        }
    bool ok;
    Q_UINT16 port = mActiveSyncPort.toUInt(&ok);
    if ( ! ok ) {
        mParent->topLevelWidget()->setCaption( i18n("Sorry, no valid port.Syncing cancelled.") );
        mPisyncFinished = true;
        return;
    }
    mCurrentResourceLocal = "";
    mCurrentResourceRemote = "";
    mSyncFailed = false;
    if ( mSpecificResources.count() ) {
        uint lastSyncRes = mSpecificResources.count()/2;
        int ccc = mSpecificResources.count()-1;
        while ( lastSyncRes > 0 && ccc > 0 && mSpecificResources[ ccc ].isEmpty() ) {
            --ccc;
            --lastSyncRes;
            //qDebug ( "KSM: sync pi %d",ccc  );
        }
        uint startLocal = 0;
        uint startRemote = mSpecificResources.count()/2;
        emit multiResourceSyncStart( true );
	int resource_count_synced = 0;
        while ( startLocal < mSpecificResources.count()/2 ) {
            if (  startLocal+1 >= lastSyncRes )
                emit multiResourceSyncStart( false );
            mPisyncFinished = false;
            mCurrentResourceLocal = mSpecificResources[ startLocal ];
            mCurrentResourceRemote = mSpecificResources[ startRemote ]; 
            //qDebug ( "KSM: AAASyncing resources: Local: %s --- Remote: %s ",mCurrentResourceLocal.latin1(),  mCurrentResourceRemote.latin1() );
            if ( !mCurrentResourceRemote.isEmpty() ) {
                qDebug ( "KSM: Syncing resources: Local: %s --- Remote: %s ",mCurrentResourceLocal.latin1(),  mCurrentResourceRemote.latin1() );
               
                mParent->topLevelWidget()->setCaption( i18n("Syncing resource %1 <-> (remote) %2/%3").arg( mCurrentResourceLocal ).arg( mCurrentResourceRemote ).arg( mCurrentSyncDevice )  );
		qApp->processEvents();
                KCommandSocket* commandSocket = new KCommandSocket( mCurrentResourceRemote, mPassWordPiSync, port, mActiveSyncIP, mCurrentSyncDevice, this, mParent->topLevelWidget() );
                connect( commandSocket, SIGNAL(commandFinished( KCommandSocket*, int )), this, SLOT(deleteCommandSocket(KCommandSocket*, int)) );
                commandSocket->readFile(  syncFileName() ); 
                while ( !mPisyncFinished ) {
                    //qDebug("waiting ");
                    qApp->processEvents();
                }
                if (  startLocal+1 < mSpecificResources.count()/2  ) { 
                    mParent->topLevelWidget()->setCaption( i18n("Waiting a second before syncing next resource...") );
                    QTime timer;
                    timer.start(); 
                    while ( timer.elapsed () < 1000 ) {
                        qApp->processEvents();
                    }
                }
		++resource_count_synced; 
            }
            ++startRemote;
            ++startLocal; 
            mAskForPreferences = false;

	    if ( mSyncFailed ) {
	      mPisyncFinished = true;
	      mParent->topLevelWidget()->setCaption( i18n("Multi-resource Pi-sync cancelled or failed") );
	      return;
	    }
        }
        mPisyncFinished = true;
        mParent->topLevelWidget()->setCaption( QString("(%1) ").arg( resource_count_synced ) + i18n("Multi-resource Pi-sync finished") );
    } else {
        mParent->topLevelWidget()->setCaption( i18n("Syncing with %1").arg( mCurrentSyncDevice ) );
        KCommandSocket* commandSocket = new KCommandSocket( "", mPassWordPiSync, port, mActiveSyncIP,mCurrentSyncDevice, this, mParent->topLevelWidget() );
        connect( commandSocket, SIGNAL(commandFinished( KCommandSocket*, int )), this, SLOT(deleteCommandSocket(KCommandSocket*, int)) );
        commandSocket->readFile(  syncFileName() );
    }
}

void KSyncManager::deleteCommandSocket(KCommandSocket*s, int state)
{
    //enum { success, errorW, errorR, quiet };
    if ( state == KCommandSocket::errorR ||
	 state == KCommandSocket::errorTO ||
	 state == KCommandSocket::errorPW ||
         state == KCommandSocket::errorCA ||
	 state == KCommandSocket::errorFI ||
	 state == KCommandSocket::errorUN||
	 state == KCommandSocket::errorED ) {

      QString errorText;
        if ( state == KCommandSocket::errorPW ) {
            errorText = i18n("Wrong password for remote host\nSyncing blocked for 15 secs on remote.\nReceiving of remote data failed.");
            mParent->topLevelWidget()->setCaption( i18n("Wrong password!" ));
	}
        else if ( state == KCommandSocket::errorR ||state == KCommandSocket::errorTO )
            mParent->topLevelWidget()->setCaption( i18n("ERROR: Receiving remote file failed.") );
        else if ( state == KCommandSocket::errorCA )
            mParent->topLevelWidget()->setCaption( i18n("Sync cancelled from remote.") );
        else if ( state == KCommandSocket::errorFI )
            mParent->topLevelWidget()->setCaption( i18n("File error on remote.") );
        else if ( state == KCommandSocket::errorED ) {
            errorText = i18n("Syncing still blocked on remote.");
            mParent->topLevelWidget()->setCaption( errorText );
	}
        else if ( state == KCommandSocket::errorUN )
            mParent->topLevelWidget()->setCaption( i18n("Unknown error on remote.") );
	if ( !errorText.isEmpty() )
	  KMessageBox::error( 0, errorText, i18n("Pi-Sync Password Error"));
        delete s;
        if ( state == KCommandSocket::errorR ) {
	  KCommandSocket* commandSocket = new KCommandSocket( "",mPassWordPiSync, mActiveSyncPort.toUInt(), mActiveSyncIP, mCurrentSyncDevice, this, mParent->topLevelWidget());
            connect( commandSocket, SIGNAL(commandFinished( KCommandSocket*, int)), this, SLOT(deleteCommandSocket(KCommandSocket*, int )) );
            commandSocket->sendStop();
        }
        mPisyncFinished = true;
        mSyncFailed = true;
        return;
        
    } else  if ( state == KCommandSocket::errorW ) {
        mParent->topLevelWidget()->setCaption( i18n("ERROR:Writing back file failed.") );
        mPisyncFinished = true;
        mSyncFailed = true;

    } else  if ( state == KCommandSocket::successR ) {
        QTimer::singleShot( 1, this , SLOT ( readFileFromSocket()));

    } else  if ( state == KCommandSocket::successW ) {
        mParent->topLevelWidget()->setCaption( i18n("Pi-Sync successful!") );
        mPisyncFinished = true;
    } else  if ( state == KCommandSocket::quiet ){
        qDebug("KCS: quiet ");
        mPisyncFinished = true;  
    } else  if ( state == KCommandSocket::cancel ){
        mSyncFailed = true;
        mPisyncFinished = true;
    } else {
        qDebug("KSS: Error: unknown state: %d ", state);
        mPisyncFinished = true;
        mSyncFailed = true;
    }

    delete s;
}

void KSyncManager::readFileFromSocket()
{
    QString fileName = syncFileName();
    bool syncOK = true;
    //mParent->topLevelWidget()->setCaption( i18n("Remote file saved to temp file.") );
    if ( ! syncWithFile( fileName , true ) ) {
        mParent->topLevelWidget()->setCaption( i18n("Sync cancelled or failed.") );
        syncOK = false;
    }
    KCommandSocket* commandSocket = new KCommandSocket( mCurrentResourceRemote,mPassWordPiSync, mActiveSyncPort.toUInt(), mActiveSyncIP, mCurrentSyncDevice, this, mParent->topLevelWidget() );
    connect( commandSocket, SIGNAL(commandFinished( KCommandSocket*, int)), this, SLOT(deleteCommandSocket(KCommandSocket*, int )) );
    if ( mWriteBackFile && syncOK  ) {
        mParent->topLevelWidget()->setCaption( i18n("Sending back file ...") );
        commandSocket->writeFile(  fileName );
    }
    else {
        commandSocket->sendStop();
        if ( syncOK ) 
            mParent->topLevelWidget()->setCaption( i18n("Pi-Sync succesful!") );
	else
	  mSyncFailed = true;
        mPisyncFinished = true;
    }
}

KServerSocket:: KServerSocket ( QString pw, Q_UINT16 port, int backlog, QObject * parent, const char * name ) : QServerSocket( port, backlog, parent, name )
{
    mPendingConnect = 0;
    mPassWord = pw;
    mSocket = 0;
    mSyncActionDialog = 0;
    blockRC = false;
    mErrorMessage = 0; 
    mErrorBox = 0;
    m_newConnectionBlocked = false;
}
void KServerSocket::setNewConnectionBlocked( bool block )
{
    m_newConnectionBlocked = block;
}
void KServerSocket::waitForSocketFinish()
{
    if (  mSocket ) {
        //qDebug("KSS:: waiting for finish operation");
      //www
      //KMessageBox::error( 0, i18n("KSS:: waiting for finish operation"), i18n("Pi-Sync Error"));
        QTimer::singleShot( 250, this , SLOT ( waitForSocketFinish()));
        return;
    }
    mSocket = new QSocket( this );
    connect( mSocket , SIGNAL(readyRead()), this, SLOT(readClient()) );
    connect( mSocket , SIGNAL(delayedCloseFinished()), this, SLOT(discardClient()) );
    mSocket->setSocket( mPendingConnect );
    mPendingConnect = 0;
}
void KServerSocket::newConnection ( int socket ) 
{
    // qDebug("KServerSocket:New connection %d ", socket); 
    if ( mPendingConnect ) {
        qDebug("KSS::Error : new Connection");
        return;
    }
    if ( mSocket ) {
        mPendingConnect = socket;
        QTimer::singleShot( 250, this , SLOT ( waitForSocketFinish()));
        return;
        qDebug("KSS::newConnection Socket deleted! ");
        delete mSocket;
        mSocket = 0;
    }
    mPendingConnect = 0;
    mSocket = new QSocket( this );
    connect( mSocket , SIGNAL(readyRead()), this, SLOT(readClient()) );
    connect( mSocket , SIGNAL(delayedCloseFinished()), this, SLOT(discardClient()) );
    mSocket->setSocket( socket );
}

void KServerSocket::discardClient()
{
    QTimer::singleShot( 10, this , SLOT ( deleteSocket()));
}
void KServerSocket::deleteSocket()
{
    //qDebug("KSS::deleteSocket");
    if ( mSocket ) {
        delete mSocket;
        mSocket = 0;
    }
    if ( mErrorMessage )
        QTimer::singleShot( 10, this , SLOT ( displayErrorMessage()));
}
void KServerSocket::readClient()
{
  if ( blockRC ) {
    //www
    //KMessageBox::error( 0, i18n("BLOCKRC"), i18n("Pi-Sync Error"));
  
        return;
  }
    if ( mSocket == 0 ) {
        qDebug("ERROR::KSS::readClient(): mSocket == 0  ");
	//www
        //KMessageBox::error( 0, i18n("KSS::readClient(): mSocket == 0 "), i18n("Pi-Sync Error"));
        return;
    }
    if ( mErrorMessage ) {
        mErrorMessage = 999;
        error_connect("ERROR_ED\r\n\r\n");
        return;
    }
    mResource = "";
    mErrorMessage = 0;
    //qDebug("KServerSocket::readClient()");
    if ( mSocket->canReadLine() ) {
        QString line = mSocket->readLine();
        //qDebug("KServerSocket readline: %s ", line.latin1());
        QStringList tokens = QStringList::split( QRegExp("[ \r\n][ \r\n]*"), line );
        if ( tokens[0] == "GET" ) {
            if (  tokens[1] == mPassWord ) {
                //emit sendFile( mSocket );
                bool ok = false;
                QDateTime dt = KGlobal::locale()->readDateTime( tokens[2], KLocale::ISODate, &ok);
                if ( ok ) {
                    KSyncManager::mRequestedSyncEvent = dt; 
                }
                else
                    KSyncManager::mRequestedSyncEvent = QDateTime();
                mResource =tokens[3];
                send_file();
            }
            else {
                mErrorMessage = 1;
                error_connect("ERROR_PW\r\n\r\n");
            }
        } 
        if ( tokens[0] == "PUT" ) {
            if (  tokens[1] == mPassWord ) {
                //emit getFile( mSocket );
                blockRC = true;
                mResource =tokens[2];
                get_file();
            }
            else {
                mErrorMessage = 2;
                error_connect("ERROR_PW\r\n\r\n");
                end_connect();
            }
        } 
        if ( tokens[0] == "STOP" ) {
            //emit endConnect(); 
	  mSocket->close();
	  if ( mSocket->state() == QSocket::Idle ) {
	    QTimer::singleShot( 0, this , SLOT ( discardClient()));
	  }
	  end_connect();
        }
    }
}
void KServerSocket::displayErrorMessage()
{
    end_connect();
    if ( mErrorBox ) {
      if ( mErrorBox->isVisible()  )
	   return;
    }

    if ( mErrorMessage == 1 ) { 
      mErrorBox = new QMessageBox( i18n("Pi-Sync Error"),
			       i18n("Got send file request\nwith invalid password") + 
			       i18n(".\nSyncing blocked for 15 secs\nfor security reasons.\nDialog will be closed\nafter 15 secs automatically."),
			       QMessageBox::Critical, 
			       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    }
    else if ( mErrorMessage == 2 ) {
      mErrorBox = new QMessageBox( i18n("Pi-Sync Error"),
			       i18n("Got receive file request\nwith invalid password") + 
			       i18n(".\nSyncing blocked for 15 secs\nfor security reasons.\nDialog will be closed\nafter 15 secs automatically."),
			       QMessageBox::Critical,
			       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    }
    if ( mErrorBox ) {
        QTimer::singleShot( 15000, this , SLOT ( endDisplayErrorMessage()));
	mErrorBox->show();
	mErrorBox->raise();
	mErrorBox->exec();
    }
}
void KServerSocket::endDisplayErrorMessage()
{
  if ( mErrorBox ) {
    mErrorBox->close();
    delete mErrorBox;
    mErrorBox = 0;
    mErrorMessage = 0;
  }
}
void KServerSocket::error_connect( QString errmess )
{

  
    QTextStream os( mSocket );
    os.setEncoding( QTextStream::Latin1 );
    os << errmess ;
    mSocket->close();
    if ( mSocket->state() == QSocket::Idle ) {
        QTimer::singleShot( 0, this , SLOT ( discardClient()));
    }
}
void KServerSocket::end_connect()
{
  if ( mSyncActionDialog )
    delete mSyncActionDialog;
  mSyncActionDialog = 0;
}
void KServerSocket::send_file()
{
    //qDebug("MainWindow::sendFile(QSocket* s) ");
    end_connect();
    mSyncActionDialog = new QDialog ( 0, "input-dialog", true, Qt::WStyle_StaysOnTop );
    mSyncActionDialog->setCaption(i18n("Received sync request"));
    QLabel* label = new QLabel( QString("Pi-Sync\n") + i18n("Received sync request") + i18n("\n\nSyncing resource: %1\n\n").arg( i18n(mResource) ) + i18n("Synchronizing from remote ...\nDo not use this application!\nIf syncing fails\nyou can close this dialog."), mSyncActionDialog ); 
    label->setAlignment ( Qt::AlignHCenter );
    QVBoxLayout* lay = new QVBoxLayout( mSyncActionDialog );
    lay->addWidget( label);
    lay->setMargin(7);
    lay->setSpacing(7);
    if ( KSyncManager::mRequestedSyncEvent.isValid() ) {
        int secs = QDateTime::currentDateTime().secsTo( KSyncManager::mRequestedSyncEvent );
        //secs = 333;
        if ( secs < 0 )
             secs = secs * (-1);
        if ( secs > 30 ) 
            //if ( true ) 
            {
                QString warning = i18n("Clock skew of\nsyncing devices\nis %1 seconds!").arg( secs );
                QLabel* label = new QLabel( warning, mSyncActionDialog ); 
                label->setAlignment ( Qt::AlignHCenter );
                lay->addWidget( label);
                if ( secs > 180 )
                    {
                        if ( secs > 300 ) {
                            if ( KMessageBox::Cancel == KMessageBox::warningContinueCancel(0, i18n("The clocks of the syncing\ndevices have a difference\nof more than 5 minutes.\nPlease adjust your clocks.\nYou may get wrong syncing results!\nPlease confirm synchronization!"), i18n("High clock skew!"),i18n("Synchronize!"))) {
                                qDebug("KSS::Sync cancelled ,cs"); 
                                mErrorMessage = 0;
                                end_connect();
                                error_connect("ERROR_CA\r\n\r\n");
                                return ;
                            }
                        }
                        QFont f = label->font();
                        f.setPointSize ( f.pointSize() *2 );
                        f. setBold (true ); 
                        QLabel* label = new QLabel( warning, mSyncActionDialog );
                        label->setFont( f ); 
                        warning = i18n("ADJUST\nYOUR\nCLOCKS!");
                        label->setText( warning ); 
                        label->setAlignment ( Qt::AlignHCenter );
                        lay->addWidget( label);
                        mSyncActionDialog->setFixedSize( 230, 300);
                    } else {
                    mSyncActionDialog->setFixedSize( 230, 200);
                }
            } else {
            mSyncActionDialog->setFixedSize( 230, 120);
        }
    } else
        mSyncActionDialog->setFixedSize( 230, 120);
    mSyncActionDialog->show();
    mSyncActionDialog->raise();
    emit request_file(mResource);
    //emit request_file();
    qApp->processEvents();
    QString fileName = mFileName;
    QFile file( fileName );
    if (!file.open( IO_ReadOnly ) ) {
        mErrorMessage = 0;
        end_connect();
        error_connect("ERROR_FI\r\n\r\n");
        return ;   
    } 
    mSyncActionDialog->setCaption( i18n("Sending file...") );
    QTextStream ts( &file );
    ts.setEncoding( QTextStream::Latin1 );

    QTextStream os( mSocket );
    os.setEncoding( QTextStream::Latin1 );
    while ( ! ts.atEnd() ) {
        os << ts.readLine() << "\r\n";
    }
    os  << "\r\n";
    //os << ts.read();
    file.close();
    mSyncActionDialog->setCaption( i18n("Waiting for synced file...") );
    mSocket->close();
    if ( mSocket->state() == QSocket::Idle )
        QTimer::singleShot( 10, this , SLOT ( discardClient()));
}
void KServerSocket::get_file()
{
    mSyncActionDialog->setCaption( i18n("Receiving synced file...") );
   
    piTime.start();
    piFileString = "";
    QTimer::singleShot( 1, this , SLOT (readBackFileFromSocket( ) ));
    if ( mSocket->state() == QSocket::Idle )
        QTimer::singleShot( 10, this , SLOT ( discardClient()));
}


void KServerSocket::readBackFileFromSocket()
{
    //qDebug("readBackFileFromSocket() %d ", piTime.elapsed ());
    while ( mSocket->canReadLine ()  ) {
        piTime.restart();
        QString line = mSocket->readLine ();
        piFileString += line;
        //qDebug("readline: %s ", line.latin1());
        mSyncActionDialog->setCaption( i18n("Received %1 bytes").arg( piFileString.length()  ) );

    }
    if ( piTime.elapsed () < 3000 ) {
        // wait for more 
        //qDebug("waitformore ");
        QTimer::singleShot( 100, this , SLOT (readBackFileFromSocket( ) ));
        return;
    }
    QString fileName = mFileName;
    QFile file ( fileName );
    if (!file.open( IO_WriteOnly ) ) {
        end_connect();
        qDebug("KSS:Error open read back file ");
        piFileString = "";
        emit file_received( false, mResource);
        emit file_received( false);
        blockRC = false;
        return ;
                    
    } 

    // mView->setLoadedFileVersion(QDateTime::currentDateTime().addSecs( -1));
    QTextStream ts ( &file );
    ts.setEncoding( QTextStream::Latin1 );
    mSyncActionDialog->setCaption( i18n("Writing file to disk...") );
    ts << piFileString;
    mSocket->close();
    if ( mSocket->state() == QSocket::Idle )
        QTimer::singleShot( 10, this , SLOT ( discardClient()));
    file.close(); 
    piFileString = "";
    emit file_received( true, mResource );
    emit file_received( true);
    end_connect();
    blockRC = false;

}

KCommandSocket::KCommandSocket ( QString remres, QString password, Q_UINT16 port, QString host,QString hostname , QObject * parent, QWidget * cap,  const char * name ): QObject( parent, name )
{
    mConnectProgress = new KProgressDialog ( cap );
    mConnectProgress->setCaption( i18n("Pi-Sync remote access"));
    mRemoteResource = remres;
    if ( mRemoteResource.isEmpty() )
        mRemoteResource = "ALL";
    else
        mRemoteResource.replace (QRegExp (" "),"_" );
    mPassWord = password;
    mSocket = 0;
    mFirst = false;
    mFirstLine = true;
    mPort = port;
    mHost = host;
    tlw = cap;
    mRetVal = quiet;
    mTimerSocket = new QTimer ( this );
    connect( mTimerSocket, SIGNAL ( timeout () ), this, SLOT ( updateConnectDialog() ) );
    connect( mConnectProgress, SIGNAL ( cancelled () ), this, SLOT ( syncCancel() ) );
    mConnectCount = -1;
    mRemoteInfo = i18n("\n\nHost:\t%1").arg(hostname) + i18n("\nIP:\t%1").arg(mHost) + i18n("\nPort:\t%1").arg(mPort) + i18n("\nRemote resource name:\n%1").arg(i18n(mRemoteResource));
}
void KCommandSocket::sendFileRequest()
{
    mConnectProgress->hide();
    mConnectCount = 300;mConnectMax = 300;
    mConnectProgress->setLabelText( i18n("Waiting for remote data...") + mRemoteInfo );
    mTimerSocket->start( 100, true );
    QTextStream os( mSocket );
    os.setEncoding( QTextStream::Latin1 );
    QString curDt = " " +KGlobal::locale()->formatDateTime(QDateTime::currentDateTime().addSecs(-1),true, true,KLocale::ISODate );
    os << "GET " << mPassWord << curDt << " " << mRemoteResource  << "\r\n\r\n";
}

void KCommandSocket::readFile( QString fn )
{
    if ( !mSocket ) {
        mSocket = new QSocket( this ); 
        //qDebug("KCS: read file - new socket");
        connect( mSocket, SIGNAL(readyRead()), this, SLOT(startReadFileFromSocket()) );
        connect( mSocket, SIGNAL(delayedCloseFinished ()), this, SLOT(deleteSocket()) );
        connect( mSocket, SIGNAL(connected ()), this, SLOT(sendFileRequest() ));
    }
    mFileString = "";
    mFileName = fn;
    mFirst = true;
    mConnectCount = 30;mConnectMax = 30;
    mTimerSocket->start( 1000, true );
    mSocket->connectToHost( mHost, mPort ); 
    //qDebug("KCS: Waiting for connection");
}
void KCommandSocket::updateConnectDialog()
{
    
    if ( mConnectCount == mConnectMax  ) {
        //qDebug("MAXX %d", mConnectMax);
        mConnectProgress->setTotalSteps ( 30 );
        mConnectProgress->show();
	mConnectProgress->setLabelText( i18n("Trying to connect to remote...") + mRemoteInfo );
    }
    //qDebug("updateConnectDialog() %d", mConnectCount);
    mConnectProgress->raise();
    mConnectProgress->setProgress( (mConnectMax - mConnectCount)%30 );
    --mConnectCount;
    if ( mConnectCount > 0 )
        mTimerSocket->start( 1000, true );
    else
        deleteSocket(); 
    
}
void KCommandSocket::writeFile( QString fileName  )
{
    if ( !mSocket ) {
        mSocket = new QSocket( this );
        connect( mSocket, SIGNAL(delayedCloseFinished ()), this, SLOT(deleteSocket()) );
        connect( mSocket, SIGNAL(connected ()), this, SLOT(writeFileToSocket()) );
    }
    mFileName = fileName ;
    mConnectCount = 30;mConnectMax = 30;
    mTimerSocket->start( 1000, true );
    mSocket->connectToHost( mHost, mPort ); 
}
void KCommandSocket::writeFileToSocket()
{
    mTimerSocket->stop();
    QFile file2( mFileName );
    if (!file2.open( IO_ReadOnly ) ) {
        mConnectProgress->hide();
        mConnectCount = -1;
        mRetVal= errorW;
        mSocket->close();
        if ( mSocket->state() == QSocket::Idle )
            QTimer::singleShot( 10, this , SLOT ( deleteSocket()));
        return ;
    }  
    mConnectProgress->setTotalSteps ( file2.size() );
    mConnectProgress->show();
    int count = 0;
    mConnectProgress->setLabelText( i18n("Sending back synced data...") + mRemoteInfo );
    mConnectProgress->setProgress( count );
    mConnectProgress->blockSignals( true );
    QTextStream ts2( &file2 );
    ts2.setEncoding( QTextStream::Latin1 );
    QTextStream os2( mSocket );
    os2.setEncoding( QTextStream::Latin1 );
    os2 << "PUT " << mPassWord << " " << mRemoteResource << "\r\n\r\n";;
    int byteCount = 0;
    int byteMax = file2.size()/53;
    while ( ! ts2.atEnd() ) {
        qApp->processEvents();
        if ( byteCount > byteMax ) {
            byteCount = 0;
            mConnectProgress->setProgress( count );
        }
        QString temp = ts2.readLine();
        count += temp.length();
        byteCount += temp.length();
        os2 << temp << "\r\n";
    }
    file2.close();
    mConnectProgress->hide();
    mConnectCount = -1;
    os2 << "\r\n";
    mRetVal= successW;
    mSocket->close();
    if ( mSocket->state() == QSocket::Idle )
        QTimer::singleShot( 10, this , SLOT ( deleteSocket()));
    mConnectProgress->blockSignals( false );
}
void KCommandSocket::sendStop()
{
    if ( !mSocket ) {
        mSocket = new QSocket( this );
        connect( mSocket, SIGNAL(delayedCloseFinished ()), this, SLOT(deleteSocket()) );
    }
    mSocket->connectToHost( mHost, mPort ); 
    QTextStream os2( mSocket );
    os2.setEncoding( QTextStream::Latin1 );
    os2 << "STOP\r\n\r\n";
    mSocket->close();
    if ( mSocket->state() == QSocket::Idle )
        QTimer::singleShot( 10, this , SLOT ( deleteSocket()));
}

void KCommandSocket::startReadFileFromSocket()
{
    if ( ! mFirst )
        return;
    mConnectProgress->setLabelText( i18n("Receiving data from remote...") +  mRemoteInfo );
    mFirst = false;
    mFileString = "";
    mTime.start();
    mFirstLine = true;
    QTimer::singleShot( 1, this , SLOT (readFileFromSocket( ) ));

}
void KCommandSocket::readFileFromSocket()
{
    //qDebug("readBackFileFromSocket() %d ", mTime.elapsed ());
    while ( mSocket->canReadLine ()  ) {
        mTime.restart();
        QString line = mSocket->readLine ();
        if ( mFirstLine ) {
            mFirstLine = false;
            if (  line.left( 6 ) == "ERROR_" ) {
                mTimerSocket->stop();
                mConnectCount = -1;
                if (  line.left( 8 ) == "ERROR_PW" ) {
                    mRetVal = errorPW;
                    deleteSocket();
                    return ;
                }
                if (  line.left( 8 ) == "ERROR_CA" ) {
                    mRetVal = errorCA;
                    deleteSocket();
                    return ;
                }
                if (  line.left( 8 ) == "ERROR_FI" ) {
                    mRetVal = errorFI;
                    deleteSocket();
                    return ;
                }
                if (  line.left( 8 ) == "ERROR_ED" ) {
                    mRetVal = errorED;
                    deleteSocket();
                    return ;
                }
                mRetVal = errorUN;
                deleteSocket();
                return ;
            }
        }
        mFileString += line;
        //qDebug("readline: %s ", line.latin1());
    }
    if ( mTime.elapsed () < 3000 ) {
        // wait for more 
        //qDebug("waitformore ");
        QTimer::singleShot( 100, this , SLOT (readFileFromSocket( ) ));
        return;
    }
    mTimerSocket->stop();
    mConnectCount = -1;
    mConnectProgress->hide();
    QString fileName = mFileName;
    QFile file ( fileName );
    if (!file.open( IO_WriteOnly ) ) {
        mFileString = "";
        mRetVal = errorR;
        qDebug("KSS:Error open temp sync file for writing: %s",fileName.latin1() );
        deleteSocket();
        return ;
                    
    } 
    // mView->setLoadedFileVersion(QDateTime::currentDateTime().addSecs( -1));
    QTextStream ts ( &file );
    ts.setEncoding( QTextStream::Latin1 );
    ts << mFileString;
    file.close();  
    mFileString = "";
    mRetVal = successR;
    mSocket->close();
    // if state is not idle, deleteSocket(); is called via 
    //  connect( mSocket, SIGNAL(delayedCloseFinished ()), this, SLOT(deleteSocket()) );
    if ( mSocket->state() == QSocket::Idle )
        deleteSocket();
}

void KCommandSocket::syncCancel()
{
  mRetVal = cancel;
  deleteSocket();
}
void KCommandSocket::deleteSocket()
{
    //qDebug("KCommandSocket::deleteSocket() ");
    mConnectProgress->hide();
   
    if ( mConnectCount >= 0  ) {
        mTimerSocket->stop();
        if ( mRetVal != cancel ) mRetVal = errorTO;
        qDebug("KCS::Connection to remote host timed out or cancelled");
        if ( mSocket ) {
            mSocket->close();
            //if ( mSocket->state() == QSocket::Idle )
            //   deleteSocket();
            delete mSocket;
            mSocket = 0;
        }
        if ( mConnectCount == 0  )
        KMessageBox::error( 0, i18n("Connection to remote\nhost timed out!\nDid you forget to enable\nPi-Sync on remote host?"));
        else if ( tlw )
            tlw->setCaption( i18n("Connection to remote host cancelled!") );
        emit commandFinished( this, mRetVal );
        return;      
    }
    //qDebug("KCommandSocket::deleteSocket() %d", mRetVal ); 
    if ( mSocket)
        delete mSocket;
    delete mConnectProgress;
    mSocket = 0; 
    //qDebug("commandFinished ");
    emit commandFinished( this, mRetVal );
}
