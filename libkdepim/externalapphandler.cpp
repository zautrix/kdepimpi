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

$Id: externalapphandler.cpp,v 1.26 2010/01/25 22:45:26 zautrix Exp $
*/
#include <stdlib.h>

#include <qfile.h>
#include <qtimer.h>
#include <qmap.h>
#include <qregexp.h>

#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#include <qtopia/qcopenvelope_qws.h>
#else
#include <qapplication.h>
#include <qprocess.h>
#endif

#include <kstaticdeleter.h>
#include <kmessagebox.h>


#include "externalapphandler.h"

#include "kpimglobalprefs.h"

//uncomment line to get debug output
//#define DEBUG_EXT_APP_HANDLER

/*********************************************************************************
 *
 ********************************************************************************/


QCopTransferItem::QCopTransferItem(int usedSourceParameters, const QString& sourceMessage, const QString& targetChannel, const QString& targetMessage)
  : _usedSourceParameters(usedSourceParameters), _sourceMessage(sourceMessage), _targetChannel(targetChannel), _targetMessage(targetMessage)
{
  //sourceMessage passes later three parameters: sourceChannel, uid, param1
    if (_usedSourceParameters == 0)
      _sourceMessageParameters = "QString,QString)";
    else if (_usedSourceParameters == 1)
      _sourceMessageParameters = "(QString,QString,QString)";
    else if (_usedSourceParameters == 2)
      _sourceMessageParameters = "(QString,QString,QString,QString)";
    else if (_usedSourceParameters == 3)
      _sourceMessageParameters = "(QString,QString,QString,QString,QString)";
}

/*********************************************************************************/

QCopTransferItem::QCopTransferItem()
{
}

/*********************************************************************************/
bool QCopTransferItem::sendMessageToTarget(const QString& uid, const QString& param1, const QString& param2, const QString& param3)
{

#ifndef DESKTOP_VERSION
  //sourceMessage passes two parameters: sourceChannel, uid
  QString sourceMessage = _sourceMessage + _sourceMessageParameters;
#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("1Using QCopEnvelope e(\"%s\",\"%s\")", _targetChannel.latin1(), sourceMessage.latin1());
  qDebug("passing sourcechannel(%s), uid(%s), param1(%s), param2(%s), param3(%s) as parameter to QCopEnvelope", _sourceChannel.latin1(), uid.latin1(), param1.latin1(), param2.latin1(), param3.latin1());
#endif

  QCopEnvelope e(_targetChannel.latin1(), sourceMessage.latin1());

  e << _sourceChannel << uid;

  if (_usedSourceParameters == 1)
    e << param1;
  else if (_usedSourceParameters == 2)
    e << param1 << param2;
  else if (_usedSourceParameters == 3)
    e << param1 << param2 << param3;

  qApp->processEvents();

  return true;

#else
  KMessageBox::sorry( 0, i18n( "This version does not support QCop." ) );
  return false;
#endif

}


/*********************************************************************************/
void QCopTransferItem::setSourceChannel(const QString& sourceChannel)
{

    if ( !sourceChannel.isEmpty())
        _sourceChannel = sourceChannel;
}


/*********************************************************************************/
bool QCopTransferItem::appMessage( const QCString& cmsg, const QByteArray& data )
{

  // copied from old mail2
/*
  static int ii = 0;

    // block second call
    if ( ii < 2 ) {
      ++ii;
      if ( ii > 1 ) {
        qDebug("qcop call blocked ");
        return true;
      }
    }
*/

//  qDebug("QCopTransferItem- QCOP message received: %s ", cmsg.data()  );

  //we are in the target and get a request from the source
  if ( (_sourceMessage + _sourceMessageParameters) == cmsg.data())
  {

#ifdef QT_4_COMPAT
      QByteArray data2 = data;
      QDataStream stream( &data2, IO_ReadOnly );
#else
      QDataStream stream( data, IO_ReadOnly );
#endif

    QString sourceChannel;
    QString uid;
    QString param1;
    QString param2;
    QString param3;

    stream >> sourceChannel >> uid;

    if (_usedSourceParameters == 0)
    {
      emit receivedMessageFromSource(sourceChannel, uid);
    }
    else if (_usedSourceParameters == 1)
    {
      stream >> param1;
      emit receivedMessageFromSource(sourceChannel, uid, param1);
	}
    else if (_usedSourceParameters == 2)
    {
      stream >> param1 >> param2;
      emit receivedMessageFromSource(sourceChannel, uid, param1, param2);
    }
    else if (_usedSourceParameters == 3)
    {
      stream >> param1 >> param2 >> param3;
      emit receivedMessageFromSource(sourceChannel, uid, param1, param2, param3);
    }

    return true;
  }

  return false;
}


/*********************************************************************************
 *
 ********************************************************************************/


QCopMapTransferItem::QCopMapTransferItem(int usedSourceParameters, const QString& sourceMessage, const QString& targetChannel, const QString& targetMessage)
  : QCopTransferItem(usedSourceParameters, sourceMessage, targetChannel,targetMessage)
{
  //targetMessage returns later two parameters: uid, and map<qstring,qstring>
  _targetMessageParameters = "(QString,QMAP<QString,QString>)";
}

/*********************************************************************************/
bool QCopMapTransferItem::sendMessageToSource(const QString& uid, const QMap<QString,QString>& nameEmailMap)
{
#ifndef DESKTOP_VERSION
  //targetMessage passes two parameters: uid, map
  QString targetMessage = _targetMessage + _targetMessageParameters;

#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("2Using QCopEnvelope e(\"%s\",\"%s\")", _sourceChannel.latin1(), targetMessage.latin1());
  qDebug("passing uid(%s) and map as parameter to QCopEnvelope", uid.latin1());
#endif

  QCopEnvelope e(_sourceChannel.latin1(), targetMessage.latin1());
    //US we need no names in the To field. The emailadresses are enough

  e << uid << nameEmailMap;

  qApp->processEvents();

  return true;

#else
  KMessageBox::sorry( 0, i18n( "This version does not support QCop." ) );
  return false;
#endif

}


/*********************************************************************************/
bool QCopMapTransferItem::appMessage( const QCString& cmsg, const QByteArray& data )
{
  bool res = QCopTransferItem::appMessage( cmsg, data );

  if (res == false)
  {
#ifdef QT_4_COMPAT
      QByteArray data2 = data;
      QDataStream stream( &data2, IO_ReadOnly );
#else
      QDataStream stream( data, IO_ReadOnly );
#endif

//    qDebug("QCopMapTransferItem- QCOP message received: %s ", cmsg.data()  );

    //we are in the source and get an answer from the target
    if ((_targetMessage + _targetMessageParameters) == cmsg.data())
    {
      QMap<QString,QString> adrMap;
      QString uid;

      stream >> uid >> adrMap;

      emit receivedMessageFromTarget(uid, adrMap);


      return true;
    }
  }

  return false;
}


/*********************************************************************************
 *
 ********************************************************************************/

QCopListTransferItem::~QCopListTransferItem()
{
    
}

QCopListTransferItem::QCopListTransferItem(int usedSourceParameters, const QString& sourceMessage, const QString& targetChannel, const QString& targetMessage)
  : QCopTransferItem(usedSourceParameters, sourceMessage, targetChannel,targetMessage)
{
  //targetMessage returns later two parameters: uid, and three lists
  _targetMessageParameters = "(QString,QStringList,QStringList,QStringList,QStringList,QStringList,QStringList)";
}

/*********************************************************************************/
bool QCopListTransferItem::sendMessageToSource(const QString& uid, const QStringList& list1, const QStringList& list2, const QStringList& list3, const QStringList& list4, const QStringList& list5, const QStringList& list6)
{
#ifndef DESKTOP_VERSION
  //targetMessage passes two parameters: uid, map
  QString targetMessage = _targetMessage + _targetMessageParameters;

#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("3Using QCopEnvelope e(\"%s\",\"%s\")", _sourceChannel.latin1(), targetMessage.latin1());
  qDebug("passing uid(%s) and list1, list2, list3, list4, list5, list6 as parameter to QCopEnvelope", uid.latin1());

  
    for ( int i = 0; i < list3.count(); i++)
      qDebug("listentry list3: %s",list3[i].latin1());
#endif

  QCopEnvelope e(_sourceChannel.latin1(), targetMessage.latin1());
    //US we need no names in the To field. The emailadresses are enough

  e << uid << list1 << list2 << list3 << list4 << list5 << list6;

  qApp->processEvents();

  return true;

#else
  KMessageBox::sorry( 0, i18n( "This version does not support QCop." ) );
  return false;
#endif

}


/*********************************************************************************/
bool QCopListTransferItem::appMessage( const QCString& cmsg, const QByteArray& data )
{
  bool res = QCopTransferItem::appMessage( cmsg, data );
#ifdef DEBUG_EXT_APP_HANDLER
    qDebug("1QCopListTransferItem- QCOP message received: %s ", cmsg.data()  );
#endif

  if (res == false)
  {

#ifdef QT_4_COMPAT
      QByteArray data2 = data;
      QDataStream stream( &data2, IO_ReadOnly );
#else
      QDataStream stream( data, IO_ReadOnly );
#endif

#ifdef DEBUG_EXT_APP_HANDLER
    qDebug("2QCopListTransferItem- QCOP message received: %s ", cmsg.data()  );
#endif

    //we are in the source and get an answer from the target
    if ((_targetMessage + _targetMessageParameters) == cmsg.data())
    {
      QStringList list1;
      QStringList list2;
      QStringList list3;
      QStringList list4;
      QStringList list5;
      QStringList list6;
      QString uid;

#ifdef DEBUG_EXT_APP_HANDLER
      qDebug("3QCopListTransferItem- QCOP message received: %s ", cmsg.data()  );
#endif

      stream >> uid >> list1 >> list2 >> list3 >> list4 >> list5 >> list6;
      emit receivedMessageFromTarget(uid, list1, list2, list3, list4, list5, list6);


      return true;
    }
  }

  return false;
}



/*********************************************************************************
 *
 ********************************************************************************/


ExternalAppHandler *ExternalAppHandler::sInstance = 0;
static KStaticDeleter<ExternalAppHandler> staticDeleter;

ExternalAppHandler::ExternalAppHandler()
{

  mDefaultItems.setAutoDelete(true);


  mNameEmailUidListFromKAPITransfer = new QCopListTransferItem(0, "requestNameEmailUIDListFromKAPI", "QPE/Application/kapi", "receiveNameEmailUIDList");
  connect(mNameEmailUidListFromKAPITransfer, SIGNAL (receivedMessageFromSource(const QString&, const QString&)), this, SIGNAL (requestForNameEmailUidList(const QString&, const QString&)));
  connect(mNameEmailUidListFromKAPITransfer, SIGNAL (receivedMessageFromTarget(const QString&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&)), this, SLOT (receivedNameEmailUidList_Slot(const QString&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&)));

//US  mFindByEmailFromKAPITransfer = new QCopListTransferItem(1, "requestFindByEmailFromKAPI", "QPE/Application/kapi", "receiveFindByEmailNameEmailUIDList");
//US  connect(mFindByEmailFromKAPITransfer, SIGNAL (receivedMessageFromSource(const QString&, const QString&, const QString&)), this, SIGNAL (requestForFindByEmail(const QString&, const QString&, const QString&)));
//US  connect(mFindByEmailFromKAPITransfer, SIGNAL (receivedMessageFromTarget(const QString&, const QStringList&, const QStringList&, const QStringList&)), this, SIGNAL (receivedFindByEmailEvent(const QString&, const QStringList&, const QStringList&, const QStringList&)));

  mDisplayDetails = new QCopListTransferItem(3, "requestDisplayDetailsFromKAPI", "QPE/Application/kapi", "");
  connect(mDisplayDetails, SIGNAL (receivedMessageFromSource(const QString&, const QString&, const QString&, const QString&, const QString&)), this, SIGNAL (requestForDetails(const QString&, const QString&, const QString&, const QString&, const QString&)));



  mBirthdayListFromKAPITransfer = new QCopListTransferItem(0, "requestBirthdayListFromKAPI", "QPE/Application/kapi", "receiveBirthdayList");
  connect(mBirthdayListFromKAPITransfer, SIGNAL (receivedMessageFromSource(const QString&, const QString&)), this, SIGNAL (requestForBirthdayList(const QString&, const QString&)));
  connect(mBirthdayListFromKAPITransfer, SIGNAL (receivedMessageFromTarget(const QString&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&)), this, SIGNAL (receivedBirthdayListEvent(const QString&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&, const QStringList&)));


}

ExternalAppHandler::~ExternalAppHandler()
{ 
    delete mNameEmailUidListFromKAPITransfer;
    //delete mFindByEmailFromKAPITransfer;
    delete mDisplayDetails;
    delete mBirthdayListFromKAPITransfer;
}

void ExternalAppHandler::receivedNameEmailUidList_Slot(const QString& uid,
						       const QStringList& nameList,
						       const QStringList& emailList,
						       const QStringList& uidList,
						       const QStringList&,
						       const QStringList&,
						       const QStringList& )
{
  // this method is a conevnient way to reduce the number of parameters I have to pass 
  emit receivedNameEmailUidListEvent(uid, nameList, emailList, uidList);
}


void ExternalAppHandler::loadConfig()
{

  mDefaultItems.clear();
  mEmailAppAvailable = UNDEFINED;
  mPhoneAppAvailable = UNDEFINED;
  mFaxAppAvailable = UNDEFINED;
  mSMSAppAvailable = UNDEFINED;
  mPagerAppAvailable = UNDEFINED;
  mSIPAppAvailable = UNDEFINED;

  QString opiepath = QString::fromLatin1( getenv("OPIEDIR") );
  QString qtopiapath = QString::fromLatin1( getenv("QPEDIR") );
  QString qtpath = QString::fromLatin1( getenv("QTDIR") );

  //if qtopiapath is not set, fallback to qt
  if (qtopiapath.isEmpty())
    qtopiapath = qtpath;

  //if opiepath is not set, fallback to qtopia
  if (opiepath.isEmpty())
    opiepath = qtopiapath;



  //mailclients
  QString mailmsg1 = "writeMail(QString,QString)";
  QString mailmsg2 = "writeMail(QMap(QString,QString))";

  QString undefined = "";

  addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::NONE_EMC,   "No email client installed",       undefined,                undefined,        undefined, undefined, undefined);
  addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::OTHER_EMC,  "Userdefined email client",       undefined,                undefined,        undefined, undefined, undefined);

#ifdef DESKTOP_VERSION
  QString appPath;
#ifdef _WIN32_
  appPath = "C:\\Programme\\Mozilla Thunderbird\\thunderbird.exe";
#else
  appPath = "/usr/bin/thunderbird";
#endif
  addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::OMPI_EMC,   "Mozilla Thunderbird",   appPath,    "-compose", "to=%1 <%2>",       ",",   "subject=%1");

#ifdef _WIN32_
  appPath = "C:\\Programme\\Mozilla\\mozilla.exe";
#else
  appPath = "/usr/bin/mozilla";
#endif
  addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::QTOPIA_EMC,   "Mozilla Suite",   appPath,    "-mail -compose", "to=%1 <%2>",       ",",   "subject=%1");



#else
  if (( QFile::exists( qtopiapath + "/bin/ompi" )) ||
     ( QFile::exists( opiepath + "/bin/ompi" )) ||
     ( QFile::exists( qtpath + "/bin/ompi" )))
    addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::OMPI_EMC,   "OM/Pi email client",   "QPE/Application/ompi",    mailmsg1, "%1;%2",        mailmsg2,   "TO=%1;ATTACHMENT=%2");

  if (( QFile::exists( qtopiapath + "/bin/qtmail" )) ||
      ( QFile::exists( qtpath + "/bin/qtmail" )))
    addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::QTOPIA_EMC, "Qtopia email client", "QPE/Application/qtmail",   mailmsg1, "%1;%2",       mailmsg2,    "TO=%1;ATTACHMENT=%2");

  if ( QFile::exists( opiepath + "/bin/opiemail" ))
    addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::OPIE_EMC,   "Opie email client",   "QPE/Application/opiemail", mailmsg1, "%1;%2",       mailmsg2,    "TO=%1;ATTACHMENT=%2");

  if ( QFile::exists( opiepath + "/bin/mailit" ))
    addDefaultAppItem(ExternalAppHandler::EMAIL, KPimGlobalPrefs::OPIE_MAILIT_EMC,   "Opie mailit email client",   "QPE/Application/mailit", mailmsg1, "%1;%2",       mailmsg2,    "TO=%1;ATTACHMENT=%2");
#endif


  //phoneclients

  addDefaultAppItem(ExternalAppHandler::PHONE, KPimGlobalPrefs::NONE_PHC,  "No phone client installed",       undefined,            undefined, undefined, undefined, undefined);
  addDefaultAppItem(ExternalAppHandler::PHONE, KPimGlobalPrefs::OTHER_PHC,  "Other phone client",       undefined,            undefined, undefined, undefined, undefined);
  if (( QFile::exists( qtopiapath + "/bin/kppi" )) ||
     ( QFile::exists( opiepath + "/bin/kppi" )))
    addDefaultAppItem(ExternalAppHandler::PHONE, KPimGlobalPrefs::KPPI_PHC,   "KP/Pi phone client",       "QPE/Application/kppi", "-ring:%1", "", undefined, undefined);

  //faxclients
  addDefaultAppItem(ExternalAppHandler::FAX,   KPimGlobalPrefs::NONE_FAC,  "No fax client installed",       undefined,            undefined, undefined, undefined, undefined);
  addDefaultAppItem(ExternalAppHandler::FAX,   KPimGlobalPrefs::OTHER_FAC,  "Other fax client",       undefined,            undefined, undefined, undefined, undefined);

  //smsclients
  addDefaultAppItem(ExternalAppHandler::SMS,   KPimGlobalPrefs::NONE_SMC,  "No sms client installed",       undefined,            undefined, undefined, undefined, undefined);
  addDefaultAppItem(ExternalAppHandler::SMS,   KPimGlobalPrefs::OTHER_SMC,  "Other sms client",       undefined,            undefined, undefined, undefined, undefined);

  //pagerclients
  addDefaultAppItem(ExternalAppHandler::PAGER, KPimGlobalPrefs::NONE_PAC,  "No pager client installed",       undefined,            undefined, undefined, undefined, undefined);
  addDefaultAppItem(ExternalAppHandler::PAGER, KPimGlobalPrefs::OTHER_PAC,  "Other pager client",       undefined,            undefined, undefined, undefined, undefined);

  //sipclients
  addDefaultAppItem(ExternalAppHandler::SIP, KPimGlobalPrefs::NONE_SIC,  "No SIP client installed",       undefined,            undefined, undefined, undefined, undefined);
  addDefaultAppItem(ExternalAppHandler::SIP, KPimGlobalPrefs::OTHER_SIC,  "Other SIP client",       undefined,            undefined, undefined, undefined, undefined);
  if (( QFile::exists( qtopiapath + "/bin/kppi" )) ||
      ( QFile::exists( opiepath + "/bin/kppi" )))
      addDefaultAppItem(ExternalAppHandler::SIP, KPimGlobalPrefs::KPPI_SIC,   "KP/Pi SIP client",       "QPE/Application/kppi", "-ring:%1", "", undefined, undefined);

}

ExternalAppHandler *ExternalAppHandler::instance()
{
  if ( !sInstance ) {
    sInstance = staticDeleter.setObject( new ExternalAppHandler() );
    sInstance->loadConfig();
  }

  return sInstance;
}

void ExternalAppHandler::addDefaultAppItem(Types type, int id, const QString& label,  const QString& channel, const QString& message, const QString& parameter, const QString& message2, const QString& parameter2)
{
  DefaultAppItem* dai = new DefaultAppItem(type, id, label,  channel, message, parameter, message2, parameter2);
  // qDebug("%d %d %s %s ",   type,  id,  label.latin1(), channel.latin1() );

  mDefaultItems.append(dai);

}


QList<DefaultAppItem> ExternalAppHandler::getAvailableDefaultItems(Types type)
{
  QList<DefaultAppItem> list;

  DefaultAppItem* dai;

  for ( dai=mDefaultItems.first(); dai != 0; dai=mDefaultItems.next() )
  {
    if (dai->_type == type)
      list.append(dai);
  }

  return list;
}

DefaultAppItem* ExternalAppHandler::getDefaultItem(Types type, int clientid)
{
  DefaultAppItem* dai;

  for ( dai=mDefaultItems.first(); dai != 0; dai=mDefaultItems.next() )
  {
    if (dai->_type == type && dai->_id == clientid)
      return dai;
  }

  return 0;
}

bool ExternalAppHandler::isEmailAppAvailable()
{

  if (mEmailAppAvailable == UNDEFINED)
  {
    int client = KPimGlobalPrefs::instance()->mEmailClient;
    if (client == KPimGlobalPrefs::NONE_EMC)
      mEmailAppAvailable = UNAVAILABLE;
    else
      mEmailAppAvailable = AVAILABLE;
  }
  return (mEmailAppAvailable == AVAILABLE);

}

bool ExternalAppHandler::isSMSAppAvailable()
{
#ifndef DESKTOP_VERSION
  if (mSMSAppAvailable == UNDEFINED)
  {
    int client = KPimGlobalPrefs::instance()->mSMSClient;
    if (client == KPimGlobalPrefs::NONE_SMC)
      mSMSAppAvailable = UNAVAILABLE;
    else
      mSMSAppAvailable = AVAILABLE;
  }

  return (mSMSAppAvailable == AVAILABLE);
#else //DESKTOP_VERSION
  return false;
#endif //DESKTOP_VERSION
}

bool ExternalAppHandler::isPhoneAppAvailable()
{
#ifndef DESKTOP_VERSION
  if (mPhoneAppAvailable == UNDEFINED)
  {
    int client = KPimGlobalPrefs::instance()->mPhoneClient;
    if (client == KPimGlobalPrefs::NONE_PHC)
      mPhoneAppAvailable = UNAVAILABLE;
    else
      mPhoneAppAvailable = AVAILABLE;
  }

  return (mPhoneAppAvailable == AVAILABLE);
#else //DESKTOP_VERSION
  return false;
#endif //DESKTOP_VERSION
}

bool ExternalAppHandler::isFaxAppAvailable()
{
#ifndef DESKTOP_VERSION
  if (mFaxAppAvailable == UNDEFINED)
  {
    int client = KPimGlobalPrefs::instance()->mFaxClient;
    if (client == KPimGlobalPrefs::NONE_FAC)
      mFaxAppAvailable = UNAVAILABLE;
    else
      mFaxAppAvailable = AVAILABLE;
  }

  return (mFaxAppAvailable == AVAILABLE);
#else //DESKTOP_VERSION
  return false;
#endif //DESKTOP_VERSION
}

bool ExternalAppHandler::isPagerAppAvailable()
{
#ifndef DESKTOP_VERSION
  if (mPagerAppAvailable == UNDEFINED)
  {
    int client = KPimGlobalPrefs::instance()->mPagerClient;
    if (client == KPimGlobalPrefs::NONE_PAC)
      mPagerAppAvailable = UNAVAILABLE;
    else
      mPagerAppAvailable = AVAILABLE;
  }

  return (mPagerAppAvailable == AVAILABLE);
#else //DESKTOP_VERSION
  return false;
#endif //DESKTOP_VERSION
}


bool ExternalAppHandler::isSIPAppAvailable()
{
#ifndef DESKTOP_VERSION
  if (mSIPAppAvailable == UNDEFINED)
  {
    int client = KPimGlobalPrefs::instance()->mSipClient;
    if (client == KPimGlobalPrefs::NONE_SIC)
      mSIPAppAvailable = UNAVAILABLE;
    else
      mSIPAppAvailable = AVAILABLE;
  }

  return (mSIPAppAvailable == AVAILABLE);
#else //DESKTOP_VERSION
  return false;
#endif //DESKTOP_VERSION
}

/**************************************************************************
 *
 **************************************************************************/


//calls the emailapplication with a number of attachments that need to be send (Seperated by Comma)
bool ExternalAppHandler::mailToMultipleContacts( const QString& emails, const QString& urls )
{

#ifndef DESKTOP_VERSION
  QString channel;
  QString message2;
  QString parameters2;


  int client = KPimGlobalPrefs::instance()->mEmailClient;
  if (client == KPimGlobalPrefs::OTHER_EMC)
  {
    channel = KPimGlobalPrefs::instance()->mEmailOtherChannel;
    message2 = KPimGlobalPrefs::instance()->mEmailOtherMessage;
    parameters2 = KPimGlobalPrefs::instance()->mEmailOtherMessageParameters;
  }
  else
  {
    DefaultAppItem* dai = getDefaultItem(EMAIL, client);
    if (!dai)
    {
      qDebug("could not find configured email application.");
	  return false;
    }
    channel = dai->_channel;
    message2 = dai->_message2;
    parameters2 = dai->_parameters2;
  }

  //first check if one of the mailers need the emails right in the message.
  message2 = translateMessage(message2, emails, urls);


#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("4Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message2.latin1());
  qDebug("passing emailadresses(%s), attachmenturls(%s) as parameters in the form %s to QCopEnvelope", emails.latin1() , urls.latin1(), parameters2.latin1());
#endif


  QCopEnvelope e(channel.latin1(), message2.latin1());
    //US we need no names in the To field. The emailadresses are enough

  passParameters(&e, parameters2, emails, urls);



#else
  //qDebug("mtmc %s %s ", emails.latin1(), urls.latin1());

  QString channel;
  QString message2;
  QString parameters2;
  QString message;
  QString parameters;


  int client = KPimGlobalPrefs::instance()->mEmailClient;
  if (client == KPimGlobalPrefs::OTHER_EMC)
  {
    channel = KPimGlobalPrefs::instance()->mEmailOtherChannel;
    message = KPimGlobalPrefs::instance()->mEmailOtherMessage;
    message2 = KPimGlobalPrefs::instance()->mEmailOtherMessage2;
    parameters = KPimGlobalPrefs::instance()->mEmailOtherMessageParameters;
    parameters2 = KPimGlobalPrefs::instance()->mEmailOtherMessageParameters2;
  }
  else
  {
    DefaultAppItem* dai = getDefaultItem(EMAIL, client);
    if (!dai)
    {
      qDebug("could not find configured email application.");
	  return false;
    }
    channel = dai->_channel;
    message2 = dai->_message2;
    parameters2 = dai->_parameters2;
    message = dai->_message;
    parameters = dai->_parameters;
  }

  //first check if one of the mailers need the emails right in the message.
  message2 = translateMessage(message2, emails, urls);
#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("4Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message2.latin1());
  qDebug("passing emailadresses(%s), attachmenturls(%s) as parameters in the form %s to QCopEnvelope", emails.latin1() , urls.latin1(), parameters2.latin1());
#endif
  qDebug("%s ---  %s   %s --- %s %s", channel.latin1(), message.latin1(),message2.latin1(), parameters.latin1(), parameters2.latin1() );
  //KMessageBox::sorry( 0, message2 );
  QProcess * proc = new QProcess( this );
  int i = 0;
  proc->addArgument( channel );

  if ( message.find (" " ) > 0 ) {
    QStringList list = QStringList::split( " ", message );
    int i = 0;
    while ( i < list.count ( ) ) {
      //qDebug("add%sdd ",list[i].latin1() );
      proc->addArgument( list[i] );
      //KMessageBox::sorry( 0,list[i]);
      ++i;
    }
  } else {
    proc->addArgument(message );
    //KMessageBox::sorry( 0, message );

  }

  parameters2 = translateMessage(parameters2, urls, "" ); 
  QString arg = "to='%1'";
  arg = arg.arg( emails ) + ","+parameters2;;
 
  //KMessageBox::sorry( 0,arg );
  //qDebug("2add%sdd ",arg.latin1() );
  proc->addArgument( arg);
  proc->launch(QString(""));
#endif

  return true;
}

/**************************************************************************
 *
 **************************************************************************/


//calls the emailapplication and creates a mail with parameter emails as recipients
bool ExternalAppHandler::mailToOneContact( const QString& name, const QString& emailadress )
{

  QString channel;
  QString message;
  QString parameters;


  int client = KPimGlobalPrefs::instance()->mEmailClient;
  if (client == KPimGlobalPrefs::OTHER_EMC)
  {
    channel = KPimGlobalPrefs::instance()->mEmailOtherChannel;
    message = KPimGlobalPrefs::instance()->mEmailOtherMessage;
    parameters = KPimGlobalPrefs::instance()->mEmailOtherMessageParameters;
  }
  else
  {
    DefaultAppItem* dai = ExternalAppHandler::getDefaultItem(EMAIL, client);
    if (!dai)
    {
      qDebug("could not find configured email application.");
	  return false;
    }
    channel = dai->_channel;
    message = dai->_message;
    parameters = dai->_parameters;
  }

#ifdef DESKTOP_VERSION
  //message =  channel + " " +message + " \""+ parameters + "\"";
#endif
  //first check if one of the mailers need the emails right in the message.
  message = translateMessage(message, name, emailadress);

#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("5Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message.latin1());
  qDebug("passing name(%s), emailadresses(%s) as parameters in the form %s to QCopEnvelope", name.latin1(), emailadress.latin1(), parameters.latin1());
#endif

#ifndef DESKTOP_VERSION
  QCopEnvelope e(channel.latin1(), message.latin1());
    //US we need no names in the To field. The emailadresses are enough

  passParameters(&e, parameters, name, emailadress);
#else // DESKTOP_VERSION

  //KMessageBox::sorry( 0,channel );
  QProcess * proc = new QProcess( this );
  proc->addArgument( channel );
  if ( message.find (" " ) > 0 ) {
    QStringList list = QStringList::split( " ", message );
    int i = 0;
    while ( i < list.count ( ) ) {
      //qDebug("add%sdd ",list[i].latin1() );
      proc->addArgument( list[i] );
      //KMessageBox::sorry( 0,list[i]);
      ++i;
    }
  } else {
    proc->addArgument(message );

  }
  parameters = translateMessage(parameters, name, emailadress);

  //KMessageBox::information(0,parameters);
  proc->addArgument( parameters );
  proc->launch(QString(""));
#endif

  return true;
}

/**************************************************************************
 *
 **************************************************************************/

//calls the emailapplication and creates a mail with parameter as recipients
// parameters format is
//  NAME <EMAIL>:SUBJECT
bool ExternalAppHandler::mailToOneContact( const QString& adressline )
{
  QString line = adressline;

  int first = line.find( "<");
  int last = line.find( ">");
  QString name = line.left(first);
  QString emailadress = line.mid(first+1, last-first-1);

  //Subject can not be handled right now.
  return mailToOneContact( name, emailadress );

}


/**************************************************************************
 *
 **************************************************************************/

//calls the phoneapplication with the number
bool ExternalAppHandler::callByPhone( const QString& phonenumber )
{
#ifndef DESKTOP_VERSION
  QString channel;
  QString message;
  QString parameters;


  int client = KPimGlobalPrefs::instance()->mPhoneClient;
  if (client == KPimGlobalPrefs::OTHER_PHC)
  {
    channel = KPimGlobalPrefs::instance()->mPhoneOtherChannel;
    message = KPimGlobalPrefs::instance()->mPhoneOtherMessage;
    parameters = KPimGlobalPrefs::instance()->mPhoneOtherMessageParameters;
  }
  else
  {
    DefaultAppItem* dai = ExternalAppHandler::getDefaultItem(PHONE, client);
    if (!dai)
    {
      qDebug("could not find configured phone application.");
	  return false;
    }
    channel = dai->_channel;
    message = dai->_message;
    parameters = dai->_parameters;
  }


  //first check if one of the mailers need the emails right in the message.
  message = translateMessage(message, phonenumber, "");


#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("6Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message.latin1());
  qDebug("passing phonenumber(%s) as parameter in the form %s to QCopEnvelope", phonenumber.latin1(), parameters.latin1());
#endif

  QCopEnvelope e(channel.latin1(), message.latin1());
    //US we need no names in the To field. The emailadresses are enough

  passParameters(&e, parameters, phonenumber, ""); 


#else
  KMessageBox::sorry( 0, i18n( "This version does not support phonecalls." ) );
#endif


  return true;
}

/**************************************************************************
 *
 **************************************************************************/

//calls the smsapplication with the number
bool ExternalAppHandler::callBySMS( const QString& phonenumber )
{
#ifndef DESKTOP_VERSION
  QString channel;
  QString message;
  QString parameters;


  int client = KPimGlobalPrefs::instance()->mSMSClient;
  if (client == KPimGlobalPrefs::OTHER_SMC)
  {
    channel = KPimGlobalPrefs::instance()->mSMSOtherChannel;
    message = KPimGlobalPrefs::instance()->mSMSOtherMessage;
    parameters = KPimGlobalPrefs::instance()->mSMSOtherMessageParameters;
  }
  else
  {
    DefaultAppItem* dai = ExternalAppHandler::getDefaultItem(SMS, client);
    if (!dai)
    {
      qDebug("could not find configured sms application.");
	  return false;
    }
    channel = dai->_channel;
    message = dai->_message;
    parameters = dai->_parameters;
  }


  //first check if one of the mailers need the emails right in the message.
  message = translateMessage(message, phonenumber, "");


#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("7Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message.latin1());
  qDebug("passing phonenumber(%s) as parameter in the form %s to QCopEnvelope", phonenumber.latin1(), parameters.latin1());
#endif

  QCopEnvelope e(channel.latin1(), message.latin1());
    //US we need no names in the To field. The emailadresses are enough

  passParameters(&e, parameters, phonenumber, "");


#else
  KMessageBox::sorry( 0, i18n( "This version does not support the sending of sms." ) );
#endif


  return true;
}

/**************************************************************************
 *
 **************************************************************************/

//calls the pagerapplication with the number
bool ExternalAppHandler::callByPager( const QString& pagernumber )
{
#ifndef DESKTOP_VERSION
  QString channel;
  QString message;
  QString parameters;


  int client = KPimGlobalPrefs::instance()->mPagerClient;
  if (client == KPimGlobalPrefs::OTHER_PAC)
  {
    channel = KPimGlobalPrefs::instance()->mPagerOtherChannel;
    message = KPimGlobalPrefs::instance()->mPagerOtherMessage;
    parameters = KPimGlobalPrefs::instance()->mPagerOtherMessageParameters;
  }
  else
  {
    DefaultAppItem* dai = ExternalAppHandler::getDefaultItem(PAGER, client);
    if (!dai)
    {
      qDebug("could not find configured pager application.");
	  return false;
    }
    channel = dai->_channel;
    message = dai->_message;
    parameters = dai->_parameters;
  }


  //first check if one of the mailers need the emails right in the message.
  message = translateMessage(message, pagernumber, "");


#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("8Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message.latin1());
  qDebug("passing pagernumber(%s) as parameter in the form %s to QCopEnvelope", pagernumber.latin1(), parameters.latin1());
#endif

  QCopEnvelope e(channel.latin1(), message.latin1());
    //US we need no names in the To field. The emailadresses are enough

  passParameters(&e, parameters, pagernumber, "");


#else
  KMessageBox::sorry( 0, i18n( "This version does not support paging." ) );
#endif


  return true;
}

/**************************************************************************
 *
 **************************************************************************/

//calls the faxapplication with the number
bool ExternalAppHandler::callByFax( const QString& faxnumber )
{
#ifndef DESKTOP_VERSION
  QString channel;
  QString message;
  QString parameters;


  int client = KPimGlobalPrefs::instance()->mFaxClient;
  if (client == KPimGlobalPrefs::OTHER_FAC)
  {
    channel = KPimGlobalPrefs::instance()->mFaxOtherChannel;
    message = KPimGlobalPrefs::instance()->mFaxOtherMessage;
    parameters = KPimGlobalPrefs::instance()->mFaxOtherMessageParameters;
  }
  else
  {
    DefaultAppItem* dai = ExternalAppHandler::getDefaultItem(FAX, client);
    if (!dai)
    {
      qDebug("could not find configured fax application.");
	  return false;
    }
    channel = dai->_channel;
    message = dai->_message;
    parameters = dai->_parameters;
  }


  //first check if one of the mailers need the emails right in the message.
  message = translateMessage(message, faxnumber, "");


#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("9Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message.latin1());
  qDebug("passing faxnumber(%s) as parameter in the form %s to QCopEnvelope", faxnumber.latin1(), parameters.latin1());
#endif

  QCopEnvelope e(channel.latin1(), message.latin1());
    //US we need no names in the To field. The emailadresses are enough

  passParameters(&e, parameters, faxnumber, "");


#else
  KMessageBox::sorry( 0, i18n( "This version does not support the sending of faxes." ) );
#endif


  return true;
}

/**************************************************************************
 *
 **************************************************************************/

//calls the sipapplication with the number
bool ExternalAppHandler::callBySIP( const QString& sipnumber )
{
#ifndef DESKTOP_VERSION
  QString channel;
  QString message;
  QString parameters;


  int client = KPimGlobalPrefs::instance()->mSipClient;
  if (client == KPimGlobalPrefs::OTHER_SIC)
  {
    channel = KPimGlobalPrefs::instance()->mSipOtherChannel;
    message = KPimGlobalPrefs::instance()->mSipOtherMessage;
    parameters = KPimGlobalPrefs::instance()->mSipOtherMessageParameters;
  }
  else
  {
    DefaultAppItem* dai = ExternalAppHandler::getDefaultItem(SIP, client);
    if (!dai)
    {
      qDebug("could not find configured sip application.");
	  return false;
    }
    channel = dai->_channel;
    message = dai->_message;
    parameters = dai->_parameters;
  }


  //first check if one of the sip apps need the emails right in the message.
  message = translateMessage(message, sipnumber, "");


#ifdef DEBUG_EXT_APP_HANDLER
  qDebug("10Using QCopEnvelope e(\"%s\",\"%s\")", channel.latin1(), message.latin1());
  qDebug("passing sipnumber(%s) as parameter in the form %s to QCopEnvelope", sipnumber.latin1(), parameters.latin1());
#endif

  QCopEnvelope e(channel.latin1(), message.latin1());
    //US we need no names in the To field. The emailadresses are enough

  passParameters(&e, parameters, sipnumber, "");


#else
  KMessageBox::sorry( 0, i18n( "This version does not support sip." ) );
#endif


  return true;
}


/**************************************************************************
 *
 **************************************************************************/


QString& ExternalAppHandler::translateMessage(QString& message, const QString& param1, const QString& param2 ) const
{
  message = message.replace( QRegExp("%1"), param1 );
  return message.replace( QRegExp("%2"), param2 );
}

/**************************************************************************
 *
 **************************************************************************/

void ExternalAppHandler::passParameters(QCopEnvelope* e, const QString& parameters, const QString& param1 , const QString& param2) const
{
#ifndef DESKTOP_VERSION
  QMap<QString, QString> valmap;
  bool useValMap = false;

  // first extract all parts of the parameters.
  QStringList paramlist = QStringList::split(";", parameters);

  //Now check how many parts we have.
  //=0 :no params to pass
  //>0 :parameters to pass
  for ( QStringList::Iterator it = paramlist.begin(); it != paramlist.end(); ++it )
  {
    QString param = (*it);
    QStringList keyvallist = QStringList::split("=", param);

    //if we have keyvalue pairs, we assume that we pass a map to the envelope
    QStringList::Iterator it2 = keyvallist.begin();
    QString key = (*it2);
    key = key.replace( QRegExp("%1"), param1 );
    key = key.replace( QRegExp("%2"), param2 );
    ++it2;

    if(it2 != keyvallist.end())
    {
      QString value = (*it2);
      value = value.replace( QRegExp("%1"), param1 );
      value = value.replace( QRegExp("%2"), param2 );

	  valmap.insert(key, value);
	  useValMap = true;
    }
    else
    {
      //      qDebug("pass parameter << %s", key.latin1());
      (*e) << key;
    }
  }

  if (useValMap == true)
    (*e) << valmap;

#endif

}



/**************************************************************************
 *
 **************************************************************************/

void ExternalAppHandler::appMessage( const QCString& cmsg, const QByteArray& data )
{
    qDebug("ExternalAppHandler::appMessage %s %x", cmsg.data(), this);
    if ( cmsg == "nextView()" ) {
        qDebug("nextView()");
        QTimer::singleShot( 0, this, SIGNAL ( nextView() ));
        return;
    }
    if ( cmsg == "callContactdialog()" ) {
        qDebug("callContactdialog()");
        QTimer::singleShot( 0, this, SIGNAL ( callContactdialog() ));
        return;
    }
    if ( cmsg == "doRingSync" ) {
        qDebug("doRingSync");
        QTimer::singleShot( 0, this, SIGNAL ( doRingSync() ));
        return;
    }
    
  bool res = mNameEmailUidListFromKAPITransfer->appMessage( cmsg, data );
  if (!res)
    res = mBirthdayListFromKAPITransfer->appMessage( cmsg, data );

  if (!res)
    res = mDisplayDetails->appMessage( cmsg, data );

//  if (!res)
//    res = mNameEmailUidListFromKAPITransfer->appMessage( cmsg, data );
}



bool ExternalAppHandler::requestNameEmailUidListFromKAPI(const QString& sourceChannel, const QString& sessionuid)
{
  mNameEmailUidListFromKAPITransfer->setSourceChannel(sourceChannel);
  // maybe we are sending to KA/Pi fom a different worldd...
  // it may be that the QAplication::desktop()->width() values in KA/Pi are not the same as in our application
  // for that reason we send the current QApplication::desktop()->width() to KA/Pi
  //qDebug("UID %s ", sessionuid.latin1());
  //return mNameEmailUidListFromKAPITransfer->sendMessageToTarget(QString::number ( QApplication::desktop()->width() ));
  return mNameEmailUidListFromKAPITransfer->sendMessageToTarget(sessionuid);
}

bool ExternalAppHandler::returnNameEmailUidListFromKAPI(const QString& sourceChannel, const QString& sessionuid, const QStringList& list1, const QStringList& list2, const QStringList& list3)
{
  QStringList list4, list5, list6;

  mNameEmailUidListFromKAPITransfer->setSourceChannel(sourceChannel);
  return mNameEmailUidListFromKAPITransfer->sendMessageToSource(sessionuid, list1, list2, list3, list4, list5, list6);
}

bool ExternalAppHandler::requestFindByEmailFromKAPI(const QString& sourceChannel, const QString& sessionuid, const QString& email)
{
  mFindByEmailFromKAPITransfer->setSourceChannel(sourceChannel);
  return mFindByEmailFromKAPITransfer->sendMessageToTarget(sessionuid, email);
}

bool ExternalAppHandler::returnFindByEmailFromKAPI(const QString& sourceChannel, const QString& sessionuid, const QStringList& list1, const QStringList& list2, const QStringList& list3)
{
  QStringList list4, list5, list6;

  mFindByEmailFromKAPITransfer->setSourceChannel(sourceChannel);
  return mFindByEmailFromKAPITransfer->sendMessageToSource(sessionuid, list1, list2, list3, list4, list5, list6);
}

bool ExternalAppHandler::requestDetailsFromKAPI(const QString& name, const QString& email, const QString& uid)
{
  mDisplayDetails->setSourceChannel("");
  return mDisplayDetails->sendMessageToTarget("", name, email, uid);
}

bool ExternalAppHandler::requestBirthdayListFromKAPI(const QString& sourceChannel, const QString& sessionuid)
{
  mBirthdayListFromKAPITransfer->setSourceChannel(sourceChannel);
  return mBirthdayListFromKAPITransfer->sendMessageToTarget(sessionuid);
}

bool ExternalAppHandler::returnBirthdayListFromKAPI(const QString& sourceChannel, const QString& sessionuid, const QStringList& list1, const QStringList& list2, const QStringList& list3, const QStringList& list4, const QStringList& list5, const QStringList& list6)
{
  mBirthdayListFromKAPITransfer->setSourceChannel(sourceChannel);
  return mBirthdayListFromKAPITransfer->sendMessageToSource(sessionuid, list1, list2, list3, list4, list5, list6);
}





