/*
    This file is part of libkdepim.
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

$Id: kpimglobalprefs.h,v 1.12 2005/04/20 10:43:21 zautrix Exp $
*/

#ifndef KPIMGLOBALPREFS_H
#define KPIMGLOBALPREFS_H

#include "kprefs.h"
#include <qdict.h>

class KPimGlobalPrefs : public KPrefs
{
  public:

    void setGlobalConfig();
    static KPimGlobalPrefs *instance();


    virtual ~KPimGlobalPrefs();


    enum EMailClients {
      NONE_EMC = 0,
      OTHER_EMC = 1,
      OMPI_EMC = 2,
      QTOPIA_EMC = 3,
      OPIE_EMC = 4,
      OPIE_MAILIT_EMC = 5
    };

    enum PhoneClients {
      NONE_PHC = 0,
      OTHER_PHC = 1,
      KPPI_PHC = 2
    };

    enum FaxClients {
      NONE_FAC = 0,
      OTHER_FAC = 1
    };

    enum SMSClients {
      NONE_SMC = 0,
      OTHER_SMC = 1
    };

    enum PagerClients {
      NONE_PAC = 0,
      OTHER_PAC = 1
    };

    enum SIPClients {
      NONE_SIC = 0,
      OTHER_SIC = 1,
      KPPI_SIC = 2
    };

  private:
    KPimGlobalPrefs( const QString &name = QString::null );

   static KPimGlobalPrefs *sInstance;
    QDict<QString> *mLocaleDict;


  public:
    //US I copied the following "locale" settings from KOPrefs 
    int mPreferredDate;
    QString mUserDateFormatLong;
    QString mUserDateFormatShort;
    int mPreferredLanguage;
    int mPreferredTime;
    bool mWeekStartsOnSunday;
    QString mTimeZoneId;
    bool mUseDaylightsaving;
    int mDaylightsavingStart;
    int mDaylightsavingEnd;
    bool mTimeZoneAdd30min;
    QFont mApplicationFont;
    QColor mAlternateColor;

    int mEmailClient;
    QString mEmailOtherChannel;
    QString mEmailOtherMessage;
    QString mEmailOtherMessageParameters;
    QString mEmailOtherMessage2;
    QString mEmailOtherMessageParameters2;

    int mPhoneClient;
    QString mPhoneOtherChannel;
    QString mPhoneOtherMessage;
    QString mPhoneOtherMessageParameters;

    int mFaxClient;
    QString mFaxOtherChannel;
    QString mFaxOtherMessage;
    QString mFaxOtherMessageParameters;

    int mSMSClient;
    QString mSMSOtherChannel;
    QString mSMSOtherMessage;
    QString mSMSOtherMessageParameters;

    int mPagerClient;
    QString mPagerOtherChannel;
    QString mPagerOtherMessage;
    QString mPagerOtherMessageParameters;

    int mSipClient;
    QString mSipOtherChannel;
    QString mSipOtherMessage;
    QString mSipOtherMessageParameters;

    QString mEx2PhoneDevice;
    QString mEx2PhoneConnection;
    QString mEx2PhoneModel;


    bool mBackupEnabled;
    QString mBackupDatadir;
    bool mBackupUseDefaultDir;
    int mBackupNumbers;
    int mBackupDayCount;
};

#endif
