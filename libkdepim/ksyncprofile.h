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
#ifndef _KSYNCPROFILE_H
#define _KSYNCPROFILE_H

#include <qptrlist.h>
#include <qcolor.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qobject.h>
#include <qstring.h>

#include <libkcal/syncdefines.h>



class KConfig;


class KSyncProfile : public QObject {
  public:
    KSyncProfile();
    ~KSyncProfile() ;

    KSyncProfile* clone();
    void setDefault();
    void readConfig(KConfig *);
    void writeConfig(KConfig *);
	void deleteConfig(KConfig *);

    void setRemotePw( const QString& n ) {mRemotePw = n;}
    QString getRemotePw( ) { return mRemotePw; }
    void setRemotePwAB( const QString& n ) {mRemotePwAB = n;}
    QString getRemotePwAB( ) { return mRemotePwAB; }
    void setRemotePwPWM( const QString& n ) {mRemotePwPWM = n;}
    QString getRemotePwPWM( ) { return mRemotePwPWM; }

    void setRemoteIP( const QString& n ) {mRemoteIP = n;}
    QString getRemoteIP( ) { return mRemoteIP; }
    void setRemoteIPAB( const QString& n ) {mRemoteIPAB = n;}
    QString getRemoteIPAB( ) { return mRemoteIPAB; }
    void setRemoteIPPWM( const QString& n ) {mRemoteIPPWM = n;}
    QString getRemoteIPPWM( ) { return mRemoteIPPWM; }

    void setRemotePort( const QString& n ) {mRemotePort = n;}
    QString getRemotePort( ) { return mRemotePort; }
    void setRemotePortAB( const QString& n ) {mRemotePortAB = n;}
    QString getRemotePortAB( ) { return mRemotePortAB; }
    void setRemotePortPWM( const QString& n ) {mRemotePortPWM = n;}
    QString getRemotePortPWM( ) { return mRemotePortPWM; }

    void setPreSyncCommand( const QString& n ) {mPreSyncCommand = n;}
    QString getPreSyncCommand( ) { return mPreSyncCommand; }
    void setPostSyncCommand( const QString& n ) {mPostSyncCommand = n;}
    QString getPostSyncCommand( ) { return mPostSyncCommand;}
    void setLocalTempFile( const QString& n ) { mLocalTempFile= n;}
    QString getLocalTempFile( ) { return mLocalTempFile;}
    void setRemoteFileName( const QString& n ) { mRemoteFileName = n;}
    QString getRemoteFileName( ) { return mRemoteFileName;}

    void setPreSyncCommandAB( const QString& n ) {mPreSyncCommandAB = n;}
    QString getPreSyncCommandAB( ) { return mPreSyncCommandAB; }
    void setPostSyncCommandAB( const QString& n ) {mPostSyncCommandAB = n;}
    QString getPostSyncCommandAB( ) { return mPostSyncCommandAB;}
    void setLocalTempFileAB( const QString& n ) { mLocalTempFileAB= n;}
    QString getLocalTempFileAB( ) { return mLocalTempFileAB;}
    void setRemoteFileNameAB( const QString& n ) { mRemoteFileNameAB = n;}
    QString getRemoteFileNameAB( ) { return mRemoteFileNameAB;}

    void setPreSyncCommandPWM( const QString& n ) {mPreSyncCommandPWM = n;}
    QString getPreSyncCommandPWM( ) { return mPreSyncCommandPWM; }
    void setPostSyncCommandPWM( const QString& n ) {mPostSyncCommandPWM = n;}
    QString getPostSyncCommandPWM( ) { return mPostSyncCommandPWM;}
    void setLocalTempFilePWM( const QString& n ) { mLocalTempFilePWM= n;}
    QString getLocalTempFilePWM( ) { return mLocalTempFilePWM;}
    void setRemoteFileNamePWM( const QString& n ) { mRemoteFileNamePWM = n;}
    QString getRemoteFileNamePWM( ) { return mRemoteFileNamePWM;}

    void setPhoneDevice( const QString& n ) { mPhoneDevice = n;}
    QString getPhoneDevice( ) { return mPhoneDevice;}
    void setPhoneConnection( const QString& n ) { mPhoneConnection = n;}
    QString getPhoneConnection( ) { return mPhoneConnection;}
    void setPhoneModel( const QString& n ) { mPhoneModel = n;}
    QString getPhoneModel( ) { return mPhoneModel;}
    /*
    void set( const QString& n ) { = n;}
    QString get( ) { return ;}
    */

    void setFilterInCal (const QString& n  ){ mFilterInCal  = n;}
    void setFilterOutCal (const QString& n  ){ mFilterOutCal  = n;}
    void setFilterInAB (const QString& n  ){ mFilterInAB  = n;}
    void setFilterOutAB (const QString& n  ){ mFilterOutAB  = n;}
    QString getFilterInCal () { return mFilterInCal  ;}
    QString getFilterOutCal () { return mFilterOutCal  ;}
    QString getFilterInAB () { return mFilterInAB  ;}
    QString getFilterOutAB () { return mFilterOutAB  ;}

    void setName( const QString& n ) {mName = n;}
    QString getName( ) { return mName;}
    void setShowSummaryAfterSync( bool b ) { mShowSummaryAfterSync = b;}
    bool getShowSummaryAfterSync( ) { return mShowSummaryAfterSync ;}
    void setAskForPreferences( bool b ) { mAskForPreferences= b;}
    bool getAskForPreferences( ) { return mAskForPreferences;}
    void setWriteBackExisting( bool b ) { mWriteBackExisting = b;}
    bool getWriteBackExisting( ) { return mWriteBackExisting;}
    void setWriteBackFuture( bool b ) { mWriteBackFuture = b;}
    bool getWriteBackFuture( ) { return mWriteBackFuture;}
    void setWriteBackFile( bool b ) { mWriteBackFile= b;}
    bool getWriteBackFile( ) { return mWriteBackFile;}
    void setWriteContactToSIM( bool b ) { mWriteContactToSIM= b;}
    bool getWriteContactToSIM( ) { return mWriteContactToSIM;}
    void setIncludeInRingSync( bool b ) {mIncludeInRingSync = b;}
    bool getIncludeInRingSync( ) { return mIncludeInRingSync;}
    void setIncludeInRingSyncAB( bool b ) {mIncludeInRingSyncAB = b;}
    bool getIncludeInRingSyncAB( ) { return mIncludeInRingSyncAB;}
    void setIncludeInRingSyncPWM( bool b ) {mIncludeInRingSyncPWM = b;}
    bool getIncludeInRingSyncPWM( ) { return mIncludeInRingSyncPWM;}
    void setSyncPrefs( int n ) { mSyncPrefs= n;}
    int getSyncPrefs( ) { return mSyncPrefs;}
    void setWriteBackFutureWeeks( int n ) { mWriteBackFutureWeeks= n;}
    int getWriteBackFutureWeeks( ) { return mWriteBackFutureWeeks;}
    void setWriteBackPastWeeks( int n ) { mWriteBackPastWeeks= n;}
    int getWriteBackPastWeeks( ) { return mWriteBackPastWeeks;}
    void setIsLocalFileSync( bool b ) { mIsLocalFileSync= b;}
    bool getIsLocalFileSync( ) { return mIsLocalFileSync;}
    void setIsPhoneSync( bool b ) { mIsPhoneSync= b;}
    bool getIsPhoneSync( ) { return mIsPhoneSync;}
    void setIsPiSync( bool b ) { mIsPiSync= b;}
    void setIsPiSyncSpec( bool b ) { mIsPiSyncSpec= b;}
    bool getIsPiSync( ) { return mIsPiSync;}
    bool getIsPiSyncSpec( ) { return mIsPiSyncSpec;}
    void setIsKapiFile( bool b ) { mIsKapiFile= b;}
    bool getIsKapiFile( ) { return mIsKapiFile;}


    QString getResSpecKopi() { return  mResSpecKopi;}
    QString getResSpecKapi() { return  mResSpecKapi;}
    void setResSpecKopi( const QString& n ) {mResSpecKopi = n;}
    void setResSpecKapi( const QString& n ) {mResSpecKapi = n;}
  private:
    QString mName;
    QString mPreSyncCommand;
    QString mPostSyncCommand;
    QString mLocalTempFile;
    QString mRemoteFileName;
    QString mPreSyncCommandAB;
    QString mPostSyncCommandAB;
    QString mLocalTempFileAB;
    QString mRemoteFileNameAB;
    QString mPreSyncCommandPWM;
    QString mPostSyncCommandPWM;
    QString mLocalTempFilePWM;
    QString mRemoteFileNamePWM;

    QString mRemotePw;
    QString mRemoteIP;
    QString mRemotePort;

    QString mRemotePwAB;
    QString mRemoteIPAB;
    QString mRemotePortAB;

    QString mRemotePwPWM;
    QString mRemoteIPPWM;
    QString mRemotePortPWM;

    QString mPhoneDevice;
    QString mPhoneConnection;
    QString mPhoneModel;

    QString mFilterInCal;
    QString mFilterOutCal;
    QString mFilterInAB;
    QString mFilterOutAB;

    bool mIncludeInRingSync;
    bool mIncludeInRingSyncAB;
    bool mIncludeInRingSyncPWM;
    int mSyncPrefs;
    bool mWriteBackFile;
    bool mWriteBackExisting;
    bool mWriteBackFuture;
    int mWriteBackFutureWeeks;
    int mWriteBackPastWeeks;
    bool mAskForPreferences;
    bool mShowSummaryAfterSync;
    bool mIsLocalFileSync;
    bool mIsPhoneSync;
    bool mWriteContactToSIM;

    bool mIsPiSync;
    bool mIsPiSyncSpec;
    bool mIsKapiFile;
    QString mResSpecKopi;
    QString mResSpecKapi;
};

#endif
