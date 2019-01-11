/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef _KSYNCPREFSDIALOG_H
#define _KSYNCPREFSDIALOG_H


#include <kdialogbase.h>
#include <qptrlist.h>
#include <qtable.h>
#include <qhbox.h>

//#include <libkdepim/kprefsdialog.h>

class KColorButton;
class QSpinBox;;
class QRadioButton;
class QCheckBox;
class QSlider;
class KURLRequester;
class QComboBox;
class QButtonGroup;
class QLineEdit;
class QVBox;
class QStringList;
class KSyncProfile;
 
/** Dialog to change the korganizer configuration.
  */
class KSyncPrefsDialog : public KDialog
{
    Q_OBJECT
  public:
    /** Initialize dialog and pages */
    KSyncPrefsDialog(QWidget *parent=0,char *name=0,bool modal=false);
    ~KSyncPrefsDialog();
    void usrReadConfig();
    void setLocalMachineName ( const QString& name );
    QString getLocalMachineName ( );
    
    QStringList getSyncProfileNames();

  public slots:
  protected slots:
      void accept();
      void deleteProfile();
      void newProfile();
      void cloneProfile();
      void kindChanged(bool);
      void fillSSH();
      void fillFTP();
      void textChanged( const QString & );
      void profileChanged( int );
      void chooseFile();
      void chooseFileAB();
      void chooseFilePWM();
      void slotOK();
      void helpDevice();
      void helpModel();
      void helpConnection();

  protected:
    void usrWriteConfig();
    void setupSyncAlgTab();
    void readFilter();
    void readResources();
  private:
    void updateMyCaption();
    int currentSelection;
    QPtrList<KSyncProfile> mSyncProfiles;
    QStringList mSyncProfileNames;
    QStringList mFilterKapi;
    QStringList mFilterKopi;
    QStringList mResourcesKopi;
    QStringList mResourcesKapi;
    QLineEdit * mMyMachineName;
    QComboBox * mProfileBox;
    QRadioButton* mIsLocal;
    QRadioButton* mIsNotLocal;
    QRadioButton*  mIsPhone;
    QRadioButton*  mIsPi;
    QRadioButton*  mIsPiSpecific;
    QCheckBox* mIncludeInRing;
    QCheckBox* mIncludeInRingAB;
    QCheckBox* mIncludeInRingPWM;
    void addProfile ( KSyncProfile* );
    void insertProfiles();
    void saveProfile();
    QButtonGroup* proGr;
    QHBox * mTableBox;
    QTable* mResTableKopi;
    QTable* mResTableKapi;
    QTable* mResTablePwmpi;

    QComboBox * mFilterOutCal;
    QComboBox * mFilterInCal;
    QComboBox * mFilterOutAB;
    QComboBox * mFilterInAB;

    QRadioButton* loc, *rem, *newest, *ask, *f_loc,* f_rem, *both;


    QLineEdit * mRemotePostcommand;
    QLineEdit * mRemotePrecommand;
    QLineEdit * mRemoteFile;
    QLineEdit * mLocalTempFile;

    QLineEdit * mRemotePostcommandAB;
    QLineEdit * mRemotePrecommandAB;
    QLineEdit * mRemoteFileAB;
    QLineEdit * mLocalTempFileAB;

    QLineEdit * mRemotePostcommandPWM;
    QLineEdit * mRemotePrecommandPWM;
    QLineEdit * mRemoteFilePWM;
    QLineEdit * mLocalTempFilePWM;


    QLineEdit * mRemotePw;
    QLineEdit * mRemoteIP;
    QLineEdit * mRemotePort;

    QLineEdit * mRemotePwAB;
    QLineEdit * mRemoteIPAB;
    QLineEdit * mRemotePortAB;

    QLineEdit * mRemotePwPWM;
    QLineEdit * mRemoteIPPWM;
    QLineEdit * mRemotePortPWM;

    QLineEdit * mPhoneDevice;
    QLineEdit * mPhoneConnection;
    QLineEdit * mPhoneModel;

    QWidget* mSetupSyncAlgTab;
    QVBox* localFileWidget;
    QVBox* remoteFileWidget;
    QVBox* phoneWidget;
    QVBox* piWidget;
    QCheckBox* mWriteBackFile;
    QCheckBox* mWriteBackFuture;
    QSpinBox* mWriteBackFutureWeeks;
    QSpinBox* mWriteBackPastWeeks;
    QCheckBox* mWriteBackExisting;
    QCheckBox* mAskForPreferences;
    QCheckBox* mShowSummaryAfterSync;
    QCheckBox* mWriteContactToSIM;
    QCheckBox* mIsKapiFileL;
    QCheckBox* mIsKapiFileR;
};

#endif
