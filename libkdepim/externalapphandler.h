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

$Id: externalapphandler.h,v 1.13 2010/01/25 22:45:26 zautrix Exp $
*/

#ifndef EXTERNALAPPHANDLER_H
#define EXTERNALAPPHANDLER_H

#ifdef QT_4_COMPAT
#include <qt4compat.h>
#include <q3listview.h>
#include <q3ptrlist.h>
#include <q3process.h>
#define QList Q3PtrList
#define QProcess Q3Process
#else

#endif

#include <qobject.h>
#include <qlist.h>
#include <qmap.h>

class QCopEnvelope;


class ExternalAppHandler;
class QCopTransferItem  : public QObject
{
  Q_OBJECT
  public:
    QCopTransferItem(int usedSourceParameters, const QString& sourceMessage, const QString& targetChannel, const QString& targetMessage);
	QCopTransferItem();

    bool sendMessageToTarget(const QString& uid, const QString& param1 = QString::null, const QString& param2 = QString::null, const QString& param3 = QString::null);

    void setSourceChannel(const QString& sourceChannel);

    virtual bool appMessage( const QCString& msg, const QByteArray& data );


 signals:
    void receivedMessageFromSource(const QString& sourceChannel, const QString& uid);
    void receivedMessageFromSource(const QString& sourceChannel, const QString& uid, const QString& param1);
    void receivedMessageFromSource(const QString& sourceChannel, const QString& uid, const QString& param1, const QString& param2);
    void receivedMessageFromSource(const QString& sourceChannel, const QString& uid, const QString& param1, const QString& param2, const QString& param3);

  public:
    int _usedSourceParameters;
    QString _sourceChannel;
    QString _sourceMessage;
    QString _sourceMessageParameters;
    QString _targetChannel;
    QString _targetMessage;
    QString _targetMessageParameters;

};

/*********************************************************************************
 *
 ********************************************************************************/

class QCopMapTransferItem : public QCopTransferItem
{
  Q_OBJECT
  public:
    QCopMapTransferItem(int usedSourceParameters, const QString& sourceMessage, const QString& targetChannel, const QString& targetMessage);

    bool sendMessageToSource(const QString& uid, const QMap<QString,QString>& nameEmailMap);


    virtual bool appMessage( const QCString& msg, const QByteArray& data );


 signals:
    void receivedMessageFromTarget(const QString& uid, const QMap<QString,QString>& nameEmailMap);

};

/*********************************************************************************
 *
 ********************************************************************************/

class QCopListTransferItem : public QCopTransferItem
{
  Q_OBJECT
  public:
    QCopListTransferItem(int usedSourceParameters, const QString& sourceMessage, const QString& targetChannel, const QString& targetMessage);
    ~QCopListTransferItem();
    bool sendMessageToSource(const QString& uid, const QStringList& list1, const QStringList& list2, const QStringList& list3, const QStringList& list4, const QStringList& list5,  const QStringList& list6);


    virtual bool appMessage( const QCString& msg, const QByteArray& data );


 signals:
    void receivedMessageFromTarget(const QString& uid, const QStringList& list1, const QStringList& list2, const QStringList& list3, const QStringList& list4 , const QStringList& list5,  const QStringList& list6);

};

/*********************************************************************************
 *
 ********************************************************************************/


class DefaultAppItem
{
  public:
    DefaultAppItem(int type, int id, const QString& label,  const QString& channel, const QString& message, const QString& parameters, const QString& message2, const QString& parameters2)
      : _type(type), _id(id), _label(label), _channel(channel), _message(message), _parameters(parameters), _message2(message2), _parameters2(parameters2)
      {	}

	DefaultAppItem()
	{  }

  public:
    int _type;
    int _id;
    QString _label;
    QString _channel;
    QString _message;
    QString _parameters;	// a list of parameters in stringrepresentation. Delimiter is ;
    QString _message2;
    QString _parameters2;  // a list of parameters in stringrepresentation. Delimiter is ;

};

/*********************************************************************************
 *
 ********************************************************************************/

class ExternalAppHandler : public QObject
{
  Q_OBJECT
  public:
    virtual ~ExternalAppHandler();

    static ExternalAppHandler *instance();

    enum Types {
		EMAIL = 0,
		PHONE = 1,
		SMS   = 2,
		FAX   = 3,
		PAGER = 4,
		SIP   = 5
	};

    enum Availability {
		UNDEFINED = -1,
		UNAVAILABLE   = 0,
		AVAILABLE = 1
	};

    //calls the emailapplication with a number of attachments that need to be send.
    //either parameter can be left empty.
    bool mailToMultipleContacts( const QString& recipients, const QString& attachmenturls );

    //calls the emailapplication and creates a mail with parameter emailadress as recipients
    bool mailToOneContact( const QString& name, const QString& emailadress );

    //calls the emailapplication and creates a mail with parameter as recipients
    // parameters format is
    //  NAME <EMAIL>:SUBJECT
    bool mailToOneContact( const QString& adressline );

    //calls the phoneapplication with the number
    bool callByPhone( const QString& phonenumber );

    //calls the smsapplication with the number
    bool callBySMS( const QString& phonenumber );

    //calls the pagerapplication with the number
    bool callByPager( const QString& pagernumber );

    //calls the faxapplication with the number
    bool callByFax( const QString& faxnumber );

    //calls the sipapplication with the number
    bool callBySIP( const QString& sipnumber );

    bool isEmailAppAvailable();
    bool isSMSAppAvailable();
    bool isPhoneAppAvailable();
    bool isFaxAppAvailable();
    bool isPagerAppAvailable();
    bool isSIPAppAvailable();


    //Call this method on the source when you want to select names from the addressbook by using QCop
    bool requestNameEmailUidListFromKAPI(const QString& sourceChannel, const QString& sessionuid);
    //Call this method on the target when you want to return the name/email map to the source (client).
    bool returnNameEmailUidListFromKAPI(const QString& sourceChannel, const QString& sessionuid, const QStringList& name, const QStringList& email, const QStringList& uid);



    bool requestFindByEmailFromKAPI(const QString& sourceChannel, const QString& sessionuid, const QString& email);
    bool returnFindByEmailFromKAPI(const QString& sourceChannel, const QString& sessionuid, const QStringList& name, const QStringList& email, const QStringList& uid);

    bool requestDetailsFromKAPI(const QString& name, const QString& email, const QString& uid);



    bool requestBirthdayListFromKAPI(const QString& sourceChannel, const QString& sessionuid);
    bool returnBirthdayListFromKAPI(const QString& sourceChannel, const QString& sessionuid,
				    const QStringList& birthdayList, const QStringList& anniversaryList,
				    const QStringList& realNameList, const QStringList& emailList,
				    const QStringList& assembledNameList, const QStringList& uidList);


    //loadConfig clears the cache and checks again if the applications are available or not
    void loadConfig();

    QList<DefaultAppItem> getAvailableDefaultItems(Types);
    DefaultAppItem* getDefaultItem(Types, int);

 public  slots:
    void appMessage( const QCString& msg, const QByteArray& data );


 signals:
    void callContactdialog();
    void nextView();
    void doRingSync();
    // Emmitted when the target app receives a request from the source app
    void requestForNameEmailUidList(const QString& sourceChannel, const QString& uid);

    // Emitted when the source app recieves a list of name/email pairs (=addresses) from another target app. Usually Ka/Pi
    // The first parameter is a uniqueid. It can be used to identify the event
    void receivedNameEmailUidListEvent(const QString& uid, const QStringList& nameList, const QStringList& emailList, const QStringList& uidList);

    void requestFindByEmail(const QString& sourceChannel, const QString& uid, const QString& email);
    void receivedFindByEmailEvent(const QString& uid, const QStringList& nameList, const QStringList& emailList, const QStringList& uidList);

    void requestForDetails(const QString& sourceChannel, const QString& sessionuid, const QString& name, const QString& email, const QString& uid);


    // Emmitted when the target app receives a request from the source app
    void requestForBirthdayList(const QString& sourceChannel, const QString& uid);

    // Emitted when the source app recieves a list of name/email pairs (=addresses) from another target app. Usually Ka/Pi
    // The first parameter is a uniqueid. It can be used to identify the event
    void receivedBirthdayListEvent(const QString& uid, const QStringList& birthdayList,
				   const QStringList& anniversaryList, const QStringList& realNameList,
				   const QStringList& emailList, const QStringList& assembledNameList,
				   const QStringList& uidList);


  private:
    ExternalAppHandler();
    QList<DefaultAppItem> mDefaultItems;

    Availability mEmailAppAvailable;
    Availability mPhoneAppAvailable;
    Availability mFaxAppAvailable;
    Availability mSMSAppAvailable;
    Availability mPagerAppAvailable;
    Availability mSIPAppAvailable;

    QCopListTransferItem* mNameEmailUidListFromKAPITransfer;
    QCopListTransferItem* mFindByEmailFromKAPITransfer;
    QCopTransferItem* mDisplayDetails;
    QCopListTransferItem* mBirthdayListFromKAPITransfer;


    void addDefaultAppItem(Types type, int id, const QString& label,  const QString& channel, const QString& message, const QString& parameters, const QString& message2, const QString& parameters2);

    QString& translateMessage(QString& message, const QString& param1, const QString& param2) const;
	void passParameters(QCopEnvelope* e, const QString& parameters, const QString& param1, const QString& param2) const;


    static ExternalAppHandler *sInstance;

  private slots:
    void receivedNameEmailUidList_Slot(const QString& uid, const QStringList& nameList, const QStringList& emailList, const QStringList& uidList, const QStringList&, const QStringList&, const QStringList& );

};


#endif
