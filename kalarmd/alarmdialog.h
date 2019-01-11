/*
    This file is part of the KDE alarm daemon.
    Copyright (c) 2000 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef ALARMDIALOG_H
#define ALARMDIALOG_H



#include <qdialog.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include "sharp_char.h"


class QSpinBox;
class QLabel;
class QString;

class AlarmDialog : public QDialog {
    Q_OBJECT
  public:
    AlarmDialog( QWidget *parent = 0, const char *name = 0 );
    virtual ~AlarmDialog();

    bool eventNotification(QString m, int replay , QString m2 , bool, int, int );
    int  getSuspendTime( );
    void setSuspendTime( int );
    void setServerNotification( bool b );

  public slots:
    void forceRepaint();
    void spinBoxChanged( int );
    void slotOk();
    void slotSuspend();
    void reject () ;
    void silent () ;
    void accept();
    void suspend();
    void playSound ();
    void setSuspendDuration ();
    void showSuspendedAlarms ();
  signals:
    // void suspendSignal(int duration);
    void addAlarm(const QDateTime &, const QString & );

  private:
    QPushButton* okbut;
    int alarmCounter;
    int mPauseCount;
    int mSuspendCounter;
    int maxAlarmReplay;
    QTimer* playSoundTimer;
    bool mStopAlarm;
    bool mSilent;
    bool mPlayWav;
    bool mServerNotification;
    QLabel* mMessage;
    QLabel* mMissedAlarms;
    QSpinBox *mSuspendSpin;
    QComboBox *mMissedAlarmsCombo;
    QPushButton* mSuspendButton;
    QString mFileName;
    int fd_led;  
    sharp_led_status statusLED;
    QDateTime mSupendAlarmTime;
    QString mSupendAlarmMessage;
    QString mSupendAlarmTitle;
};

#endif
