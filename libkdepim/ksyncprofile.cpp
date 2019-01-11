/*
    This file is part of KDE-Pim/Pi.
    Copyright (c) 2004 Lutz Rogowski <rogowski@kde.org>

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

// $Id: ksyncprofile.cpp,v 1.16 2005/11/25 18:35:16 zautrix Exp $

#include <qcolor.h>

#include <kconfig.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kdebug.h>

#include "ksyncprofile.h"


KSyncProfile::KSyncProfile(): QObject ()
{
    setDefault();
}
KSyncProfile::~KSyncProfile()
{

}


KSyncProfile* KSyncProfile::clone()
{
    KSyncProfile* myClone = new KSyncProfile();
    myClone->setRemotePw(mRemotePw);
    myClone->setRemoteIP(mRemoteIP);
    myClone->setRemotePort(mRemotePort);
    myClone->setRemotePwAB(mRemotePwAB);
    myClone->setRemoteIPAB(mRemoteIPAB);
    myClone->setRemotePortAB(mRemotePortAB);
    myClone->setRemotePwPWM(mRemotePwPWM);
    myClone->setRemoteIPPWM(mRemoteIPPWM);
    myClone->setRemotePortPWM (mRemotePortPWM);
    myClone->setPreSyncCommand( mPreSyncCommand );
    myClone->setPostSyncCommand( mPostSyncCommand );
    myClone->setLocalTempFile( mLocalTempFile);
    myClone->setRemoteFileName( mRemoteFileName );
    myClone->setPreSyncCommandAB( mPreSyncCommandAB );
    myClone->setPostSyncCommandAB( mPostSyncCommandAB );
    myClone->setLocalTempFileAB( mLocalTempFileAB);
    myClone->setRemoteFileNameAB( mRemoteFileNameAB );
    myClone->setPreSyncCommandPWM( mPreSyncCommandPWM );
    myClone->setPostSyncCommandPWM( mPostSyncCommandPWM );
    myClone->setLocalTempFilePWM( mLocalTempFilePWM);
    myClone->setRemoteFileNamePWM( mRemoteFileNamePWM );
    myClone->setShowSummaryAfterSync( mShowSummaryAfterSync );
    myClone->setAskForPreferences( mAskForPreferences);
    myClone->setWriteBackExisting(mWriteBackExisting );
    myClone->setWriteBackFile( mWriteBackFile);
    myClone->setWriteBackFuture( mWriteBackFuture );
    myClone->setWriteBackFutureWeeks( mWriteBackFutureWeeks );
    myClone->setWriteBackPastWeeks( mWriteBackPastWeeks );
    myClone->setIncludeInRingSync( mIncludeInRingSync );
    myClone->setIncludeInRingSyncAB( mIncludeInRingSyncAB );
    myClone->setIncludeInRingSyncPWM( mIncludeInRingSyncPWM );
    myClone->setSyncPrefs( mSyncPrefs);
    myClone->setIsLocalFileSync( mIsLocalFileSync );
    myClone->setIsPhoneSync( mIsPhoneSync );
    myClone->setIsKapiFile( mIsKapiFile );
    myClone->setIsPiSync( mIsPiSync );
    myClone->setWriteContactToSIM( mWriteContactToSIM );
    myClone->setName( "noName" );
    myClone->setFilterInCal ( mFilterInCal );
    myClone->setFilterOutCal ( mFilterOutCal );
    myClone->setFilterInAB ( mFilterInAB );
    myClone->setFilterOutAB ( mFilterOutAB );
    //myClone->setIdentifier( "noID" );
    return myClone;
}


void KSyncProfile::setDefault()
{
    mPreSyncCommand = i18n("command for downloading remote file to local device");
    mPostSyncCommand = i18n("command for uploading local temp file to remote device");
    mLocalTempFile = "/tmp/mycalendar.ics";
    mRemoteFileName = "/home/polo/kdepim/apps/korganizer/localfile.ics";
    mPreSyncCommandAB = i18n("command for downloading remote file to local device");
    mPostSyncCommandAB = i18n("command for uploading local temp file to remote device");
    mLocalTempFileAB = "/tmp/std.vcf";
    mRemoteFileNameAB = "/home/polo/kdepim/apps/kabc/localfile.vcf";
    mPreSyncCommandPWM = i18n("command for downloading remote file to local device");
    mPostSyncCommandPWM = i18n("command for uploading local temp file to remote device");
    mLocalTempFilePWM = "/tmp/passwords.pwm";
    mRemoteFileNamePWM = "/home/polo/kdepim/apps/pwmanager/localfile.pwm";

    mRemotePw = "abc";
    mRemoteIP = "192.168.0.99";
    mRemotePort = "9197";

    mRemotePwAB = "abc";
    mRemoteIPAB = "192.168.0.99";
    mRemotePortAB = "9198";

    mRemotePwPWM = "abc";
    mRemoteIPPWM = "192.168.0.99";
    mRemotePortPWM = "9199";

    mShowSummaryAfterSync = true;
    mAskForPreferences = true;
    mWriteBackExisting = false;
    mWriteBackFuture = false;
    mWriteBackFutureWeeks = 12;
    mWriteBackPastWeeks = 2;
    mWriteBackFile = true;
    mIncludeInRingSync = false;
    mIncludeInRingSyncAB = false;
    mIncludeInRingSyncPWM = false;
    mSyncPrefs = SYNC_PREF_ASK;
    mIsLocalFileSync = true;
    mName = "noName";
    mIsPhoneSync = false;
    mIsPiSync = false;
    mIsPiSyncSpec = false;
    mIsKapiFile = false;
    mWriteContactToSIM = false;
    mPhoneDevice = "/dev/ircomm";
    mPhoneConnection = "irda";
    mPhoneModel = "6310i";
    mFilterInCal = "no filter";
    mFilterOutCal = "no filter";
    mFilterInAB = "no filter";
    mFilterOutAB = "no filter";
    mResSpecKopi = "";
    mResSpecKapi = "";
}
void KSyncProfile::readConfig(KConfig *config )
{
  if (config)
  {

    config->setGroup( mName );

    mName = config->readEntry( "Name", mName );

    mRemotePw = config->readEntry( "RemotePw",mRemotePw  );
    mRemoteIP = config->readEntry( "RemoteIP",mRemoteIP  );
    mRemotePort = config->readEntry( "RemotePort", mRemotePort );
    mRemotePwAB = config->readEntry( "RemotePwAB", mRemotePwAB );
    mRemoteIPAB = config->readEntry( "RemoteIPAB", mRemoteIPAB );
    mRemotePortAB = config->readEntry( "RemotePortAB", mRemotePortAB );
    mRemotePwPWM = config->readEntry( "RemotePwPWM", mRemotePwPWM  );
    mRemoteIPPWM = config->readEntry( "RemoteIPPWM", mRemoteIPPWM );
    mRemotePortPWM = config->readEntry( "RemotePortPWM", mRemotePortPWM );

    mPreSyncCommand = config->readEntry( "PreSyncCommand",mPreSyncCommand  );
    mPostSyncCommand = config->readEntry( "PostSyncCommand", mPostSyncCommand );
    mLocalTempFile = config->readEntry( "LocalTempFile", mLocalTempFile );
    mRemoteFileName = config->readEntry( "RemoteFileName", mRemoteFileName );

    mPreSyncCommandAB = config->readEntry( "PreSyncCommandAB",mPreSyncCommandAB  );
    mPostSyncCommandAB = config->readEntry( "PostSyncCommandAB", mPostSyncCommandAB );
    mLocalTempFileAB = config->readEntry( "LocalTempFileAB", mLocalTempFileAB );
    mRemoteFileNameAB = config->readEntry( "RemoteFileNameAB", mRemoteFileNameAB );

    mPreSyncCommandPWM = config->readEntry( "PreSyncCommandPWM",mPreSyncCommandPWM  );
    mPostSyncCommandPWM = config->readEntry( "PostSyncCommandPWM", mPostSyncCommandPWM );
    mLocalTempFilePWM = config->readEntry( "LocalTempFilePWM", mLocalTempFilePWM );
    mRemoteFileNamePWM = config->readEntry( "RemoteFileNamePWM", mRemoteFileNamePWM );

    mPhoneDevice = config->readEntry( "PhoneDevice", mPhoneDevice );
    mPhoneConnection = config->readEntry( "PhoneConnection", mPhoneConnection );
    mPhoneModel = config->readEntry( "PhoneModel", mPhoneModel );

    mIncludeInRingSync = config->readBoolEntry( "IncludeInRingSync",mIncludeInRingSync  );
    mIncludeInRingSyncAB = config->readBoolEntry( "IncludeInRingSyncAB",mIncludeInRingSyncAB  );
    mIncludeInRingSyncPWM = config->readBoolEntry( "IncludeInRingSyncPWM",mIncludeInRingSyncPWM  );
    mShowSummaryAfterSync = config->readBoolEntry( "ShowSummaryAfterSync", mShowSummaryAfterSync );
    mAskForPreferences = config->readBoolEntry( "AskForPreferences",mAskForPreferences  );
    mWriteBackExisting = config->readBoolEntry( "WriteBackExisting",mWriteBackExisting  );
    mWriteBackFuture = config->readBoolEntry( "WriteBackFuture",mWriteBackFuture  );
    mWriteBackFile = config->readBoolEntry( "WriteBackFile",mWriteBackFile  );
    mWriteContactToSIM = config->readBoolEntry( "WriteContactToSIM",mWriteContactToSIM  );
    mSyncPrefs = config->readNumEntry( "SyncPrefs", mSyncPrefs );
    mWriteBackFutureWeeks = config->readNumEntry( "WriteBackFutureWeeks", mWriteBackFutureWeeks );
    mWriteBackPastWeeks = config->readNumEntry( "WriteBackPastWeeks", mWriteBackPastWeeks );
    mIsLocalFileSync= config->readBoolEntry( "IsLocalFileSync", mIsLocalFileSync );
    mIsPhoneSync= config->readBoolEntry( "IsPhoneSync", mIsPhoneSync );
    mIsPiSync= config->readBoolEntry( "IsPiSync", mIsPiSync );
    mIsPiSyncSpec= config->readBoolEntry( "IsPiSyncSpec", mIsPiSyncSpec );
    mIsKapiFile = config->readBoolEntry( "IsKapiFile", mIsKapiFile );

    mFilterInCal = config->readEntry( "FilterInCal", mFilterInCal );
    mFilterOutCal = config->readEntry( "FilterOutCal", mFilterOutCal );
    mFilterInAB = config->readEntry( "FilterInAB", mFilterInAB );
    mFilterOutAB = config->readEntry( "FilterOutAB", mFilterOutAB );
    mResSpecKopi = config->readEntry( "ResSpecKopi", mResSpecKopi );
    mResSpecKapi = config->readEntry( "ResSpecKapi", mResSpecKapi );

  }
  else
  {
    setDefault();
  }
}

void KSyncProfile::deleteConfig(KConfig *config )
{
	config->deleteGroup( mName );
}

void KSyncProfile::writeConfig( KConfig * config )
{
    config->setGroup(mName);

    config->writeEntry( "RemotePw", mRemotePw);
    config->writeEntry( "RemoteIP", mRemoteIP);
    config->writeEntry( "RemotePort", mRemotePort);

    config->writeEntry( "RemotePwAB", mRemotePwAB);
    config->writeEntry( "RemoteIPAB", mRemoteIPAB);
    config->writeEntry( "RemotePortAB", mRemotePortAB);

    config->writeEntry( "RemotePwPWM", mRemotePwPWM);
    config->writeEntry( "RemoteIPPWM", mRemoteIPPWM);
    config->writeEntry( "RemotePortPWM", mRemotePortPWM);

    config->writeEntry( "Name", mName );
    config->writeEntry( "PreSyncCommand",mPreSyncCommand  );
    config->writeEntry( "PostSyncCommand", mPostSyncCommand );
    config->writeEntry( "LocalTempFile", mLocalTempFile );
    config->writeEntry( "RemoteFileName", mRemoteFileName );

    config->writeEntry( "PreSyncCommandAB",mPreSyncCommandAB  );
    config->writeEntry( "PostSyncCommandAB", mPostSyncCommandAB );
    config->writeEntry( "LocalTempFileAB", mLocalTempFileAB );
    config->writeEntry( "RemoteFileNameAB", mRemoteFileNameAB );

    config->writeEntry( "PreSyncCommandPWM",mPreSyncCommandPWM  );
    config->writeEntry( "PostSyncCommandPWM", mPostSyncCommandPWM );
    config->writeEntry( "LocalTempFilePWM", mLocalTempFilePWM );
    config->writeEntry( "RemoteFileNamePWM", mRemoteFileNamePWM );

    config->writeEntry( "PhoneDevice", mPhoneDevice );
    config->writeEntry( "PhoneConnection", mPhoneConnection );
    config->writeEntry( "PhoneModel", mPhoneModel );

    config->writeEntry( "IncludeInRingSync",mIncludeInRingSync  );
    config->writeEntry( "IncludeInRingSyncAB",mIncludeInRingSyncAB  );
    config->writeEntry( "IncludeInRingSyncPWM",mIncludeInRingSyncPWM  );
    config->writeEntry( "ShowSummaryAfterSync", mShowSummaryAfterSync );
    config->writeEntry( "AskForPreferences",mAskForPreferences  );
    config->writeEntry( "WriteBackExisting",mWriteBackExisting  );
    config->writeEntry( "WriteBackFuture",mWriteBackFuture );
    config->writeEntry( "WriteBackFile",mWriteBackFile );
    config->writeEntry( "WriteContactToSIM",mWriteContactToSIM );
    config->writeEntry( "SyncPrefs", mSyncPrefs );
    config->writeEntry( "WriteBackFutureWeeks", mWriteBackFutureWeeks);
    config->writeEntry( "WriteBackPastWeeks", mWriteBackPastWeeks);
    config->writeEntry( "IsLocalFileSync", mIsLocalFileSync );
    config->writeEntry( "IsPhoneSync", mIsPhoneSync );
    config->writeEntry( "IsPiSync", mIsPiSync );
    config->writeEntry( "IsPiSyncSpec", mIsPiSyncSpec );
    config->writeEntry( "IsKapiFile", mIsKapiFile );
    config->writeEntry( "FilterInCal", mFilterInCal );
    config->writeEntry( "FilterOutCal", mFilterOutCal );
    config->writeEntry( "FilterInAB", mFilterInAB );
    config->writeEntry( "FilterOutAB", mFilterOutAB );

    config->writeEntry( "ResSpecKopi", mResSpecKopi );
    config->writeEntry( "ResSpecKapi", mResSpecKapi );
}

