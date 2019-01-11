/*
    This file is part of libkdepim.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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

$Id: kpimglobalprefs.cpp,v 1.21 2005/04/21 17:43:51 zautrix Exp $
*/

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kstaticdeleter.h>

#include <qregexp.h>
#include <qfile.h>
#include <stdlib.h>
#include <qtextstream.h>
#include <qapplication.h>
#include "kpimglobalprefs.h"

KPimGlobalPrefs *KPimGlobalPrefs::sInstance = 0;
static KStaticDeleter<KPimGlobalPrefs> staticDeleterGP;


KPimGlobalPrefs::KPimGlobalPrefs( const QString &name )
  : KPrefs("microkdeglobalrc")
{
    mLocaleDict = 0;
  KPrefs::setCurrentGroup("Fonts");
  addItemFont("ApplicationFont",&mApplicationFont,KGlobalSettings::generalFont() );
  KPrefs::setCurrentGroup("Locale");
  addItemInt("PreferredLanguage",&mPreferredLanguage,0);
  addItemInt("PreferredTime",&mPreferredTime,0);
  addItemInt("PreferredDate",&mPreferredDate,0);
  addItemBool("WeekStartsOnSunday",&mWeekStartsOnSunday,false);
  addItemString("UserDateFormatLong", &mUserDateFormatLong, "%AK %d. %b %y");
  addItemString("UserDateFormatShort", &mUserDateFormatShort, "%aK %d.%m.%y");

  KPrefs::setCurrentGroup("Colors");
  addItemColor("AlternateBGcolor",&mAlternateColor,KGlobalSettings::alternateBackgroundColor());


  KPrefs::setCurrentGroup("Time & Date");

  addItemString("TimeZoneName",&mTimeZoneId, ("+01:00 Europe/Oslo(CET)") );
  addItemBool("UseDaylightsaving",&mUseDaylightsaving,true);
  addItemBool("TimeZoneAdd30min",&mTimeZoneAdd30min,false);
  addItemInt("DaylightsavingStart",&mDaylightsavingStart,90);
  addItemInt("DaylightsavingEnd",&mDaylightsavingEnd,304);

  KPrefs::setCurrentGroup( "ExternalApplications" );

  addItemInt( "EmailChannelType", &mEmailClient, OMPI_EMC );
  addItemString( "EmailChannel", &mEmailOtherChannel, "" );
  addItemString( "EmailChannelMessage", &mEmailOtherMessage, "" );
  addItemString( "EmailChannelParameters", &mEmailOtherMessageParameters, "" );
  addItemString( "EmailChannelMessage2", &mEmailOtherMessage2, "" );
  addItemString( "EmailChannelParameters2", &mEmailOtherMessageParameters2, "" );

  addItemInt( "PhoneChannelType", &mPhoneClient, KPPI_PHC );
  addItemString( "PhoneChannel", &mPhoneOtherChannel, "" );
  addItemString( "PhoneChannelMessage", &mPhoneOtherMessage, "" );
  addItemString( "PhoneChannelParameters", &mPhoneOtherMessageParameters, "" );

  addItemInt( "FaxChannelType", &mFaxClient, NONE_FAC );
  addItemString( "FaxChannel", &mFaxOtherChannel, "" );
  addItemString( "FaxChannelMessage", &mFaxOtherMessage, "" );
  addItemString( "FaxChannelParameters", &mFaxOtherMessageParameters, "" );

  addItemInt( "SMSChannelType", &mSMSClient, NONE_SMC );
  addItemString( "SMSChannel", &mSMSOtherChannel, "" );
  addItemString( "SMSChannelMessage", &mSMSOtherMessage, "" );
  addItemString( "SMSChannelParameters", &mSMSOtherMessageParameters, "" );

  addItemInt( "PagerChannelType", &mPagerClient, NONE_PAC );
  addItemString( "PagerChannel", &mPagerOtherChannel, "" );
  addItemString( "PagerChannelMessage", &mPagerOtherMessage, "" );
  addItemString( "PagerChannelParameters", &mPagerOtherMessageParameters, "" );

  addItemInt( "SIPChannelType", &mSipClient, KPPI_SIC );
  addItemString( "SIPChannel", &mSipOtherChannel, "" );
  addItemString( "SIPChannelMessage", &mSipOtherMessage, "" );
  addItemString( "SIPChannelParameters", &mSipOtherMessageParameters, "" );

  KPrefs::setCurrentGroup( "PhoneAccess" );
  addItemString("Ex2PhoneDevice",&mEx2PhoneDevice,"/dev/ircomm");
  addItemString("Ex2PhoneConnection",&mEx2PhoneConnection,"irda");
  addItemString("Ex2PhoneModel",&mEx2PhoneModel,"6310i");

  KPrefs::setCurrentGroup( "BackupSettings" );
  addItemString("BackupDatadir",&mBackupDatadir,KGlobalSettings::backupDataDir());
  addItemInt( "BackupNumbers", &mBackupNumbers, 3 );
  addItemInt( "BackupDayCount", &mBackupDayCount, 2 );
  addItemBool( "BackupUseDefaultDir",&mBackupUseDefaultDir, true );
  addItemBool( "BackupEnabled",&mBackupEnabled, false );

}

void KPimGlobalPrefs::setGlobalConfig()
{
    if ( mLocaleDict == 0 ) {
        QString fileName ; 
        QString name = KGlobal::getAppName() +"/";
#ifndef DESKTOP_VERSION
        fileName= QString(getenv("QPEDIR"))+"/pics/kdepim/"+name;
#else
        fileName  = qApp->applicationDirPath () + "/kdepim/"+ name;
#endif
        mLocaleDict = 0;
        if ( mPreferredLanguage > 0 && mPreferredLanguage < 5 ) {
         
            if ( mPreferredLanguage == 1 ) 
                fileName = fileName+"germantranslation.txt"; 
            else if ( mPreferredLanguage == 4 ) 
                fileName = fileName+"usertranslation.txt"; 
            else if ( mPreferredLanguage == 2 ) 
                fileName = fileName+"frenchtranslation.txt"; 
            else if ( mPreferredLanguage == 3 ) 
                fileName = fileName+"italiantranslation.txt";
            QFile file( fileName );
            if (file.open( IO_ReadOnly ) ) {
                QTextStream ts( &file );
                ts.setEncoding( QTextStream::Latin1 );
                //ts.setCodec( QTextCodec::latin1 );
                QString  text = ts.read();
                file.close();  
                text.replace( QRegExp("\\\\n"), "\n" );
                QString line;
                QString we;
                QString wt;
                int br = 0;
                int nbr;
                nbr = text.find ( "},", br );
                line = text.mid( br, nbr - br );
                br = nbr+1;
                int se, ee, st, et;
                mLocaleDict = new QDict<QString>;
                mLocaleDict->setAutoDelete( true );
                QString end = "{ \"\",\"\" }";
                while ( (line != end) && (br > 1)  ) {
                    //qDebug("%d *%s* ", br, line.latin1());
                    se = line.find("\"")+1;
                    et = line.findRev("\"",-1);
                    ee = line.find("\",\"");
                    st = ee+3;
                    we = line.mid( se, ee-se );
                    if (  mPreferredLanguage == 4  )
                        wt = QString::fromUtf8(line.mid( st, et-st ).latin1());
                    else
                        wt = line.mid( st, et-st );
                    //qDebug("*%s* *%s* ", we.latin1(), wt.latin1());
                    mLocaleDict->insert( we, new QString (wt) );
                    nbr = text.find ( "}", br );
                    line = text.mid( br, nbr - br );
                    br = nbr+1;
                }
                //qDebug("end *%s* ", end.latin1());

                setLocaleDict( mLocaleDict );
            } else {
                qDebug("KO: Cannot find translation file %s",fileName.latin1() );
            }
        }
    }

    KGlobal::locale()->setHore24Format( !mPreferredTime );
    KGlobal::locale()->setWeekStartMonday( !mWeekStartsOnSunday );
    KGlobal::locale()->setIntDateFormat( (KLocale::IntDateFormat)mPreferredDate );
    KGlobal::locale()->setLanguage( mPreferredLanguage );
    QString dummy = mUserDateFormatLong;
    KGlobal::locale()->setDateFormat(dummy.replace( QRegExp("K"), QString(",") ));
    dummy = mUserDateFormatShort;
    KGlobal::locale()->setDateFormatShort(dummy.replace( QRegExp("K"), QString(",") ));
    KGlobal::locale()->setDaylightSaving( mUseDaylightsaving,
                                          mDaylightsavingStart,
                                          mDaylightsavingEnd );
    KGlobal::locale()->setTimezone( mTimeZoneId, mTimeZoneAdd30min );
    KGlobalSettings::setAlternateBackgroundColor(mAlternateColor);
    QString localKdeDir;
    localKdeDir = readEnvPath("LOCALMICROKDEHOME");
    if ( ! localKdeDir.isEmpty() ) {
        mBackupUseDefaultDir = true; 
        mBackupDatadir = KGlobalSettings::backupDataDir(); 
    }

}
KPimGlobalPrefs::~KPimGlobalPrefs()
{ 
    if (sInstance == this)
      sInstance = staticDeleterGP.setObject(0); 
    else
        qDebug("Whats this? Error in KPimGlobalPrefs::~KPimGlobalPrefs() ?");
    if ( mLocaleDict )
        delete mLocaleDict;
}

KPimGlobalPrefs *KPimGlobalPrefs::instance()
{
  if ( !sInstance ) {
    sInstance = staticDeleterGP.setObject( new KPimGlobalPrefs() );
    sInstance->readConfig();
  }

  return sInstance;
}
