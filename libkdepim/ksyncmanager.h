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

  $Id: ksyncmanager.h,v 1.38 2010/03/26 23:25:45 zautrix Exp $
*/
#ifndef _KSYNCMANAGER_H
#define _KSYNCMANAGER_H
#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else

#include <qprogressbar.h>

#endif



#include <qobject.h>
#include <qstring.h>
#include <qpopupmenu.h>
#include <qsocket.h>
#include <qdatetime.h>
#include <qserversocket.h>
#include <qmessagebox.h> 
#include <qtextstream.h>
#include <qregexp.h>
#include <qprogressdialog.h>
#include <kdialog.h>

class QPopupMenu;
class KSyncProfile;
class KPimPrefs;
class QWidget;
class KSyncManager;
class KSyncInterface;
class QProgressBar;


class KServerSocket : public QServerSocket
{
    Q_OBJECT

        public:
    KServerSocket ( QString password, Q_UINT16 port, int backlog = 0, QObject * parent=0, const char * name=0 );

    void newConnection ( int socket ) ;
    void setFileName( QString fn  ) {mFileName = fn;};
    void setNewConnectionBlocked( bool block );
 signals:
    void file_received( bool );
    void request_file();
    void file_received( bool, const QString &);
    void request_file(const QString &);
    void saveFile();
    void endConnect();
    private slots:
    void waitForSocketFinish();
    void discardClient();
    void deleteSocket();
    void readClient();
    void displayErrorMessage();
    void endDisplayErrorMessage();
    void readBackFileFromSocket();
 private :
    bool m_newConnectionBlocked;
    QMessageBox * mErrorBox;
    int mPendingConnect;
    QString mResource;
    int mErrorMessage;
    bool blockRC;
    void send_file();
    void get_file();
    void end_connect();
    void error_connect( QString );
    QDialog* mSyncActionDialog;
    QSocket* mSocket; 
    QString mPassWord;
    QString mFileName; 
    QTime piTime;
    QString piFileString;
};

class KCommandSocket : public QObject
{
    Q_OBJECT
        public:
  enum state { successR, errorR, successW, errorW, errorTO, errorPW, errorCA, errorFI, errorUN, errorED,quiet, cancel };
    KCommandSocket (QString remoteResource, QString password, Q_UINT16 port, QString host, QString hostname, QObject * parent=0, QWidget*  cap = 0, const char * name=0 );
    void readFile( QString );
    void writeFile( QString );
    void sendStop();

 private slots :
    void sendFileRequest();
    void updateConnectDialog();
       
 signals:
    void commandFinished( KCommandSocket*, int );
    private slots:
    void startReadFileFromSocket();
    void readFileFromSocket();
    void deleteSocket();
    void writeFileToSocket();
    void syncCancel();
 private :
    QString mRemoteInfo;
    QString mRemoteResource;
    int mConnectCount;
    int mConnectMax;
    KProgressDialog *mConnectProgress;
    QWidget* tlw;
    QSocket* mSocket; 
    QString mPassWord;
    Q_UINT16 mPort;
    QString mHost;
    QString mFileName;
    QTimer* mTimerSocket;
    int mRetVal;
    QTime mTime;
    QString mFileString;
    bool mFirst;
    bool mFirstLine;
};


class KSyncManager : public QObject
{
    Q_OBJECT

        public:
    enum TargetApp {
        KOPI = 0,
        KAPI = 1,
        PWMPI = 2 };
  
    KSyncManager(QWidget* parent, KSyncInterface* implementation, TargetApp ta, KPimPrefs* prefs, QPopupMenu* syncmenu);
    ~KSyncManager() ;

    void multiSync( bool askforPrefs  );
    bool blockSave() { return mBlockSaveFlag; }
    void setBlockSave(bool sa) { mBlockSaveFlag = sa; }
    void setDefaultFileName( QString s) ;
    QString defaultFileName() { return mDefFileName ;}
    QString syncFileName();
    void enableQuick( bool ask = true);

    bool syncWithDesktop () { return mSyncWithDesktop;}
    QString getCurrentSyncDevice() { return mCurrentSyncDevice; }
    QString getCurrentSyncName() { return mCurrentSyncName; }

    void showProgressBar(int percentage, QString caption = QString::null, int total=100);
    void hideProgressBar();
    bool isProgressBarCanceled();

    // sync stuff
    QString mLocalMachineName;
    QStringList  mExternSyncProfiles;
    QStringList  mSyncProfileNames;
    bool mAskForPreferences;
    bool mShowSyncSummary;
    bool mIsKapiFile;
    bool mWriteBackExistingOnly;
    int mSyncAlgoPrefs;
    bool mWriteBackFile;
    int  mWriteBackInFuture;
    int  mWriteBackInPast;
    QString mPhoneDevice;
    QString mPhoneConnection;
    QString mPhoneModel;
    QString mPassWordPiSync;
    QString mActiveSyncPort;
    QString mActiveSyncIP ;
    QString mFilterInCal;
    QString mFilterOutCal;
    QString mFilterInAB;
    QString mFilterOutAB;
    static QDateTime mRequestedSyncEvent;

 signals:
    void save();
    void request_file();
    void getFile( bool );
    void getFile( bool, const QString &);
    void request_file(const QString &);
    void multiResourceSyncStart( bool );

    public slots:
    void slotSyncMenu( int );
    void slotClearMenu( int action );
    void deleteCommandSocket(KCommandSocket*s, int state);
    void readFileFromSocket();
    void fillSyncMenu();

 private:
    void syncPi();
    KServerSocket * mServerSocket;
    KPimPrefs* mPrefs;
    QString mDefFileName;
    QString mCurrentSyncDevice;
    QString mCurrentSyncName;
    void quickSyncLocalFile();
    bool syncWithFile( QString fn , bool quick );
    void syncLocalFile();
    void syncPhone();
    void syncSharp();
    void syncKDE();
    void syncOL();
    bool syncExternalApplication(QString);
    int mCurrentSyncProfile ;
    void syncRemote( KSyncProfile* prof,  bool ask = true);
    bool edit_sync_options();
    bool edit_pisync_options();
    int ringSync();
    QString  getPassword( );
    bool mPisyncFinished;
    bool mSyncFailed;
    QStringList mSpecificResources;
    QString mCurrentResourceLocal;
    QString mCurrentResourceRemote;
    bool mBlockSaveFlag;
    QWidget* mParent;
    KSyncInterface* mImplementation;
    TargetApp mTargetApp; 
    QPopupMenu* mSyncMenu;
    QProgressBar* bar;
    bool mSyncWithDesktop;

private slots:
    void displayErrorPort();
    void confSync();


};


class KSyncInterface 
{ 
 public :
    virtual void removeSyncInfo( QString syncProfile) = 0;
    virtual bool sync(KSyncManager* manager, QString filename, int mode, QString resource) = 0;
    virtual bool syncExternal(KSyncManager* /*manager*/, QString /*resource*/)
    {
        // empty implementation, because some syncable applications do not 
        // have an external(sharpdtm)  syncmode, like pwmanager.
        return false;
    }
    virtual QString getLastSyncDT4device( QString syncDevice )
    {
      return QString("Never synced before");
    }
};


#endif
