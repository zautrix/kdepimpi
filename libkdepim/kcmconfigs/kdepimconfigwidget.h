/*
    This file is part of KDEPim/Pi.
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

$Id: kdepimconfigwidget.h,v 1.9 2005/07/27 16:04:28 zautrix Exp $
*/

#ifndef KDEPIMCONFIGWIDGET_H
#define KDEPIMCONFIGWIDGET_H

#include <kprefswidget.h>
#include <kio/kfile/kurlrequester.h>
#include <qmap.h>

#include "externalapphandler.h"


class QComboBox;
class QLineEdit;
class KPimGlobalPrefs;
class QGroupBox;
class QTabWidget;
class KDateEdit;

class KDEPIMConfigWidget : public KPrefsWidget
{
  Q_OBJECT

  public:
    KDEPIMConfigWidget(KPimGlobalPrefs *prefs, QWidget *parent, const char *name = 0 );

  public slots:
    void textChanged( const QString& text );
    void showTimeZoneTab();

  protected:
    /** Implement this to read custom configuration widgets. */
  virtual void usrReadConfig();
    /** Implement this to write custom configuration widgets. */
  virtual void usrWriteConfig();


  private slots:
//    void configureExtension();
//    void selectionChanged( QListViewItem* );
//    void itemClicked( QListViewItem* );
    void client_changed( int newClient );
    void externalapp_changed( int newApp );
    void saveStoreSettings();
    void setStandardStore();
    void setLocalStore(); 

  private:
    void setupExternalAppTab();
    void setupLocaleDateTab();
    void setupLocaleTab();
    void setupTimeZoneTab();
    void setupStoreTab();
    void setupBackupTab();
    KURLRequester* mStoreUrl;

    void setCombo(QComboBox *combo,const QString & text, const QStringList *tags = 0);


    void saveEditFieldSettings();
    void updateClientWidgets();

    QTabWidget *tabWidget;


    QLineEdit* mUserDateFormatShort;
    QLineEdit* mUserDateFormatLong;
    QComboBox* mTimeZoneCombo;
    KDateEdit* mStartDateSavingEdit;
    KDateEdit* mEndDateSavingEdit; 

//    void restoreExtensionSettings();
//    void saveExtensionSettings();

//    KListView *mExtensionView;

//    QCheckBox *mNameParsing;
//    QCheckBox *mViewsSingleClickBox;
//    QPushButton *mConfigureButton;
	QComboBox* mExternalApps;
	QGroupBox* mExternalAppGroupBox;


    QComboBox* mClient;
    QLineEdit* mChannel;
    QLineEdit* mMessage;
    QLineEdit* mParameters;
    QLineEdit* mMessage2;
    QLineEdit* mParameters2;

    ExternalAppHandler::Types mCurrentApp;
    int mCurrentClient;


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

    QLabel* mDataStoragePath;
    KURLRequester* mBackupUrl;
    QSpinBox* mBackupDayCountSpin, *mBackupNumbersSpin   ;
    QMap<ExternalAppHandler::Types, QString> mExternalAppsMap;

//    AddresseeWidget *mAddresseeWidget;
};

#endif
