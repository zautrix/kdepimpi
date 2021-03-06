/*
    This file is part of libkcal.
    Copyright (c) 1998 Preston Brown
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <kdebug.h>
#include <klocale.h>

#include "incidence.h"
#include "todo.h"

#include "alarm.h"

using namespace KCal;
#include <qwidget.h>
Alarm::Alarm(Incidence *parent)
 : mParent(parent),
   mType(Audio),
   mDescription(""),    // to make operator==() not fail
   mFile(""),           // to make operator==() not fail
   mMailSubject(""),    // to make operator==() not fail
   mAlarmSnoozeTime(5),
   mAlarmRepeatCount(0),
   mEndOffset(false),
   mHasTime(false),
   mAlarmEnabled(false)
{
 
}

Alarm::~Alarm()
{
}

bool Alarm::operator==( const Alarm& rhs ) const
{

  if ( mType != rhs.mType ||
       mAlarmSnoozeTime != rhs.mAlarmSnoozeTime ||
       mAlarmRepeatCount != rhs.mAlarmRepeatCount ||
       mAlarmEnabled != rhs.mAlarmEnabled ||
       mHasTime != rhs.mHasTime)
    return false;

#if 0
    if ( mType != rhs.mType ) {

        qDebug("aaa1 ");
        return false;
    }

    if ( mAlarmSnoozeTime != rhs.mAlarmSnoozeTime  ) {

        qDebug("aaa2 ");
        return false;
    }


    if ( mAlarmRepeatCount != rhs.mAlarmRepeatCount ) {

        qDebug("aaa3 ");
        return false;
    }

    if ( mAlarmEnabled != rhs.mAlarmEnabled  ) {

        qDebug("aaa4 ");
        return false;
    }

    if ( mHasTime != rhs.mHasTime ) {

        qDebug("aaa5 ");
        return false;
    }
#endif
  

  if (mHasTime) {
    if (mAlarmTime != rhs.mAlarmTime)
      return false;
  } else {
    if (mOffset != rhs.mOffset ||
        mEndOffset != rhs.mEndOffset)
      return false;
  }
  switch (mType) {
    case Display:
      return mDescription == rhs.mDescription;

    case Email:
      return mDescription == rhs.mDescription &&
             mMailAttachFiles == rhs.mMailAttachFiles &&
             mMailAddresses == rhs.mMailAddresses &&
             mMailSubject == rhs.mMailSubject;

    case Procedure:
      return mFile == rhs.mFile &&
             mDescription == rhs.mDescription;

    case Audio:
      return mFile == rhs.mFile;

    case Invalid:
      break;
  }
  return false;
}

void Alarm::setType(Alarm::Type type)
{
  if (type == mType)
    return;

  switch (type) {
    case Display:
      mDescription = "";
      break;
    case Procedure:
      mFile = mDescription = "";
      break;
    case Audio:
      mFile = "";
      break;
    case Email:
      mMailSubject = mDescription = "";
      mMailAddresses.clear();
      mMailAttachFiles.clear();
      break;
    case Invalid:
      break;
    default:
      return;
  }
  mType = type;
  mParent->updated();
}

Alarm::Type Alarm::type() const
{
  return mType;
}

void Alarm::setAudioAlarm(const QString &audioFile)
{
  mType = Audio;
  mFile = audioFile;
  mParent->updated();
}

void Alarm::setAudioFile(const QString &audioFile)
{
  if (mType == Audio) {
    mFile = audioFile;
    mParent->updated();
  }
}

QString Alarm::audioFile() const
{
  return (mType == Audio) ? mFile : QString::null;
}

void Alarm::setProcedureAlarm(const QString &programFile, const QString &arguments)
{
  mType = Procedure;
  mFile = programFile;
  mDescription = arguments;
  mParent->updated();
}

void Alarm::setProgramFile(const QString &programFile)
{
  if (mType == Procedure) {
    mFile = programFile;
    mParent->updated();
  }
}

QString Alarm::programFile() const
{
  return (mType == Procedure) ? mFile : QString::null;
}

void Alarm::setProgramArguments(const QString &arguments)
{
  if (mType == Procedure) {
    mDescription = arguments;
    mParent->updated();
  }
}

QString Alarm::programArguments() const
{
  return (mType == Procedure) ? mDescription : QString::null;
}

void Alarm::setEmailAlarm(const QString &subject, const QString &text,
                          const QValueList<Person> &addressees, const QStringList &attachments)
{
  mType = Email;
  mMailSubject = subject;
  mDescription = text;
  mMailAddresses = addressees;
  mMailAttachFiles = attachments;
  mParent->updated();
}

void Alarm::setMailAddress(const Person &mailAddress)
{
  if (mType == Email) {
    mMailAddresses.clear();
    mMailAddresses += mailAddress;
    mParent->updated();
  }
}

void Alarm::setMailAddresses(const QValueList<Person> &mailAddresses)
{
  if (mType == Email) {
    mMailAddresses = mailAddresses;
    mParent->updated();
  }
}

void Alarm::addMailAddress(const Person &mailAddress)
{
  if (mType == Email) {
    mMailAddresses += mailAddress;
    mParent->updated();
  }
}

QValueList<Person> Alarm::mailAddresses() const
{
  return (mType == Email) ? mMailAddresses : QValueList<Person>();
}

void Alarm::setMailSubject(const QString &mailAlarmSubject)
{
  if (mType == Email) {
    mMailSubject = mailAlarmSubject;
    mParent->updated();
  }
}

QString Alarm::mailSubject() const
{
  return (mType == Email) ? mMailSubject : QString::null;
}

void Alarm::setMailAttachment(const QString &mailAttachFile)
{
  if (mType == Email) {
    mMailAttachFiles.clear();
    mMailAttachFiles += mailAttachFile;
    mParent->updated();
  }
}

void Alarm::setMailAttachments(const QStringList &mailAttachFiles)
{
  if (mType == Email) {
    mMailAttachFiles = mailAttachFiles;
    mParent->updated();
  }
}

void Alarm::addMailAttachment(const QString &mailAttachFile)
{
  if (mType == Email) {
    mMailAttachFiles += mailAttachFile;
    mParent->updated();
  }
}

QStringList Alarm::mailAttachments() const
{
  return (mType == Email) ? mMailAttachFiles : QStringList();
}

void Alarm::setMailText(const QString &text)
{
  if (mType == Email) {
    mDescription = text;
    mParent->updated();
  }
}

QString Alarm::mailText() const
{
  return (mType == Email) ? mDescription : QString::null;
}

void Alarm::setDisplayAlarm(const QString &text)
{
  mType = Display;
  mDescription = text;
  mParent->updated();
}

void Alarm::setText(const QString &text)
{
  if (mType == Display) {
    mDescription = text;
    mParent->updated();
  }
}

QString Alarm::text() const
{
  return (mType == Display) ? mDescription : QString::null;
}

void Alarm::setTime(const QDateTime &alarmTime)
{
  mAlarmTime = alarmTime;
  mHasTime = true;

  mParent->updated();
}
int Alarm::offset() 
{
    if ( hasTime() ) {
        if (mParent->typeID() == todoID ) {
            Todo *t = static_cast<Todo*>(mParent);
            return  t->dtDue().secsTo( mAlarmTime ) ;
        } else
            return mParent->dtStart().secsTo( mAlarmTime ) ;
    }
    else
        {
            return mOffset.asSeconds();
        }
    
}
QString Alarm::offsetText()
{
    int min = -offset()/60;
    int hours = min /60;
    min = min % 60;
    int days = hours /24;
    hours = hours % 24;
    QString message;
    //qDebug("%d %d %d ", days, hours, min  );
    if ( days > 0 )
        message += i18n("%1d").arg( days );
    if ( hours > 0 ) {
        if ( !message.isEmpty() ) message += "/";
        message += i18n("%1h").arg( hours );
    }
    if ( min > 0 ) {
        if ( !message.isEmpty() ) message += "/";
        message += i18n("%1min").arg( min );
    }
    if ( message.isEmpty() )
         message = i18n("%1min").arg( 0 );
    if ( !mParent->alarmEnabled() )
        return "!"+message + i18n("(disabled)");
    return message; 
}


QDateTime Alarm::time() const
{
  if ( hasTime() )
    return mAlarmTime;
  else
  {
    if (mParent->typeID() == todoID ) {
      Todo *t = static_cast<Todo*>(mParent);
      return mOffset.end( t->dtDue() );
    } else if (mEndOffset) {
      return mOffset.end( mParent->dtEnd() );
    } else {
      return mOffset.end( mParent->dtStart() );
    }
  }
}

bool Alarm::hasTime() const
{
  return mHasTime;
}

void Alarm::setSnoozeTime(int alarmSnoozeTime)
{
  mAlarmSnoozeTime = alarmSnoozeTime;
  mParent->updated();
}

int Alarm::snoozeTime() const
{
  return mAlarmSnoozeTime;
}

void Alarm::setRepeatCount(int alarmRepeatCount)
{
  kdDebug(5800) << "Alarm::setRepeatCount(): " << alarmRepeatCount << endl;

  mAlarmRepeatCount = alarmRepeatCount;
  mParent->updated();
}

int Alarm::repeatCount() const
{
  kdDebug(5800) << "Alarm::repeatCount(): " << mAlarmRepeatCount << endl;
  return mAlarmRepeatCount;
}

void Alarm::toggleAlarm()
{
  mAlarmEnabled = !mAlarmEnabled;
  mParent->updated();
}

void Alarm::setEnabled(bool enable)
{
  mAlarmEnabled = enable;
  mParent->updated();
}

bool Alarm::enabled() const
{
  return mAlarmEnabled;
}

void Alarm::setStartOffset( const Duration &offset )
{
  mOffset = offset;
  mEndOffset = false;
  mHasTime = false;
  mParent->updated();
}

Duration Alarm::startOffset() const
{
  return (mHasTime || mEndOffset) ? 0 : mOffset;
}

bool Alarm::hasStartOffset() const
{
  return !mHasTime && !mEndOffset;
}

bool Alarm::hasEndOffset() const
{
  return !mHasTime && mEndOffset;
}

void Alarm::setEndOffset( const Duration &offset )
{
  mOffset = offset;
  mEndOffset = true;
  mHasTime = false;
  mParent->updated();
}

Duration Alarm::endOffset() const
{
  return (mHasTime || !mEndOffset) ? 0 : mOffset;
}

void Alarm::setParent( Incidence *parent )
{
  mParent = parent;
}
