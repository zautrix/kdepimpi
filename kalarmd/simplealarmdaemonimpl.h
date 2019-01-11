/*
    This file is part of the KOrganizer alarm daemon.
    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef SIMPLEALARMDAEMONIMPL_H
#define SIMPLEALARMDAEMONIMPL_H

//#include "simplealarmdaemon.h"
#include <qdatetime.h>
#include <qlabel.h>
#include <qtimer.h>

class QLabel;
class QTimer;
class QPopupMenu;
class AlarmDialog;
class SimpleAlarmDaemonImpl : public QLabel
{
    Q_OBJECT
  public:
    SimpleAlarmDaemonImpl( QWidget *parent = 0 );
    
    ~SimpleAlarmDaemonImpl();

  protected slots:
      void recieve( const QCString& msg, const QByteArray& data );
    void newTodo();
    void newEvent();
    void newCountdown();
    void simulate();
    void showKO();
    void showWN();
    void showAdd();
    void newMail();
    void ringSync();
    void showTodo();
    void writeFile();
    void writeJournal();
    void slotPlayBeep( int );
    void showTimer( );
    void confPause( int );
    void confFontSize( int );
    void confTimer( int );
    void saveSlot( int );
    void confSuspend( int );
    void suspendSettings( int );
    void confSound( int num );
    void startAlarm(QString mess, QString fn );
    void setSuspendDuration();
    void showSuspendedAlarm();

  protected:
    void mousePressEvent( QMouseEvent * );

  private:
    AlarmDialog *mAlarmDialog;
    QLabel * mTimerStartLabel;
    int mPlayBeeps;
    int mPausePlay;
    int mSuspend;
    QString mAlarmMessage;
    int mTimerTime;
    int getFileNameLen( QString );
    QPopupMenu* mPopUp, *mBeepPopUp, *mTimerPopUp, *mSoundPopUp,*mPausePopUp,*mSuspendPopUp, *mFontsizePopup;
    QDateTime mRunningTimer;
    void fillTimerPopUp();
    QString timerMesssage;
    QString mCustomText;
    QString mRunningTimerText;
    int mCustomMinutes;
    int mTimerPopupConf;
    int mPopupFontSize;
    bool wavAlarm;
};
class KODateLabel : public QLabel
{
    Q_OBJECT
  public:
    KODateLabel( QWidget *parent=0, const char *name=0 ) :
      QLabel( parent, name ) 
    {
        hour = 0;
        minutes = 0;
        QTimer * ti = new QTimer( this );
        connect ( ti, SIGNAL ( timeout () ), this, SLOT ( updateText() ));
        ti->start( 1000 );
        
    }
public slots:      
    void slot_minutes( int m )
    { 
        minutes = m; updateText();
    }    
    void slot_hours( int h )
    { 
        hour = h; updateText();
    }
private slots:
    void updateText() 
    {
        QDateTime dt = QDateTime::currentDateTime();
        dt = dt.addSecs( minutes * 60 + hour * 3600 );
        setText(  dt.time().toString()  );
    }
    int hour, minutes;
};
#endif
