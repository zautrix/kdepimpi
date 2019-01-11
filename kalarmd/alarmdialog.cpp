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

// $Id: alarmdialog.cpp,v 1.18 2008/04/17 13:37:42 zautrix Exp $

#include <qhbox.h>
#include <qvbox.h>
#include <qapp.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfile.h>
#include <qtimer.h>
#include <qsound.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qregexp.h>
#ifndef DESKTOP_VERSION
#define protected public
#include <qspinbox.h>
#undef protected 
#else
#include <qspinbox.h>
#endif
#include <stdlib.h>
#ifndef _WIN32_
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <fcntl.h>

#ifndef DESKTOP_VERSION
#include <qtopia/alarmserver.h>
#include <qpe/resource.h>
#include <qtopia/sound.h>

#endif     

#include "alarmdialog.h"


AlarmDialog::AlarmDialog(QWidget *parent,const char *name)
    : QDialog (parent, name ,false, Qt::WStyle_StaysOnTop )
{
    setCaption( "KO/Pi Alarm!" );
    QVBoxLayout* layout = new QVBoxLayout( this);
  QLabel* l = new QLabel("The following event triggered alarm:",this);
  layout->addWidget ( l );
  l->setAlignment( AlignCenter);
  mMessage = new QLabel ( " ", this );
  int fs = 18;
  int fs2 = 12;
  int baseSize = 6;
  if ( QApplication::desktop()->width() < 480 ) {
      fs2 = 10;
      fs = 12;
      baseSize = 4;
  }
  layout->setSpacing( 3 );
  layout->setMargin( 3 );
  QFont fo = QApplication::font();
  fo.setBold( true );
  fo.setPointSize( fs2 );
  l->setFont( fo );
  fo.setPointSize( fs );
  mMessage->setFont(fo );
  mMessage->setAlignment( AlignCenter);
  layout->addWidget ( mMessage );
  mMissedAlarms= new QLabel ( "(No missed Alarms)", this );
  mMissedAlarms->setAlignment( AlignCenter);

  playSoundTimer = new QTimer( this );
  connect ( playSoundTimer, SIGNAL( timeout()  ), this, SLOT (playSound() ) );

  playSoundTimer->stop();
  
  layout->addWidget ( mMissedAlarms );
  mMissedAlarmsCombo =  new QComboBox ( this );
  layout->addWidget ( mMissedAlarmsCombo );

  QLabel* labb = new QLabel("Suspend\nduration\n(minutes):",this);
#ifdef DESKTOP_VERSION
  fo = font();
  fo.setPointSize( 12 );
  labb->setFont ( fo );
#endif
  labb->setAlignment(AlignCenter);
  //layout->addWidget ( labb );
  fo = font();
  int pointSize = 36;
  if ( QApplication::desktop()->width() <= 320 )
      pointSize = 18;
  fo.setPointSize( pointSize );
  mSuspendSpin = new QSpinBox(1,1440,1,this);
  mSuspendSpin->setFont( fo );
  mSuspendSpin->setValue(7);  // default suspend duration
  mSuspendSpin->setButtonSymbols( QSpinBox::PlusMinus );
  mSuspendSpin->setButtonSymbols( QSpinBox::PlusMinus );
  
#if QT_VERSION < 0x030000
  mSuspendSpin->upButton ()->setFixedSize( QSize( 8*baseSize, 5*baseSize ));
  mSuspendSpin->downButton ()->setFixedSize( QSize( 8*baseSize, 5*baseSize ));
#endif
  mSuspendSpin->setFixedSize( 18*baseSize, 10*baseSize+2 );
  mSuspendSpin->setButtonSymbols( QSpinBox::PlusMinus ); 
  QHBoxLayout* layoutSpin = new QHBoxLayout( layout );
  layoutSpin->addStretch ();
  layoutSpin->addWidget ( labb );
  layoutSpin->addWidget ( mSuspendSpin );
  layoutSpin->addStretch ();

  QVBox * bbox  = new QVBox ( this );
  layout->addWidget ( bbox );
  bbox->layout()->setSpacing( 2 );
  labb = new QLabel("Press \"Cancel\" or \"Esc\" to suspend!",bbox);
  labb->setAlignment(AlignCenter);
  mSuspendButton = new QPushButton( "Suspend", bbox);
  QPushButton* silen = new QPushButton( " Stop sound ", bbox);
  okbut = new QPushButton( "Ok", bbox); 
  mSuspendButton->setFont( fo );
  silen->setFont( fo );
  okbut->setFont( fo );
  okbut->setDefault( true );
  connect (silen , SIGNAL( clicked() ), this, SLOT (silent() ) ); 
  connect (mSuspendButton, SIGNAL( clicked() ), this, SLOT (slotSuspend() ) );
  connect (okbut , SIGNAL( clicked() ), this, SLOT (slotOk() ) );
  connect (mSuspendSpin , SIGNAL( valueChanged ( int ) ), this, SLOT ( spinBoxChanged( int ) ) );
#ifndef _WIN32_
  if ( QFile::exists ( "/dev/sharp_led" ) )
      fd_led = open ( "/dev/sharp_led", O_RDWR|O_NONBLOCK );
  else
#endif
      fd_led = 0;
  statusLED.which = SHARP_LED_SALARM;
  mSilent = false;
  mSuspendCounter = 0;
  setServerNotification( true );
}
void AlarmDialog::reject () 
{
    QTimer::singleShot ( 3000, this, SLOT (suspend())  );
    slotSuspend();
}
AlarmDialog::~AlarmDialog()
{
}
void AlarmDialog::silent () 
{
    mSilent = true;
}
void AlarmDialog::accept()
{
    slotOk();
}

void AlarmDialog::suspend()
{
#ifdef DESKTOP_VERSION

#else
    Sound::soundAlarm ();
#endif
}
void AlarmDialog::slotOk()
{
    mStopAlarm = true;
    mMissedAlarms->setText("(No missed Alarms)");
    mMessage->setText("");
    mMissedAlarmsCombo->clear();
#ifndef _WIN32_
    if (  fd_led > 0 ) {
        statusLED.status =  LED_SALARM_OFF ;
        ioctl (fd_led, SHARP_LED_SETSTATUS, &statusLED);
    }
#endif
    QDialog::accept();
}

void AlarmDialog::slotSuspend() 
{
    //qDebug("AlarmDialog::suspend() ");  // emit suspendSignal(mSuspendSpin->value());
   mStopAlarm = true;
   QDateTime nextA = QDateTime::currentDateTime().addSecs( mSuspendSpin->value() * 60 );
   QString mess = "suspend_alarm" +mFileName+"+++" ;
   if ( mMessage->text().left( 10 ) !="Suspended:" )
       mess += "Suspended:\n";
   mess +=mMessage->text();
   mSupendAlarmTitle = mMessage->text();
#ifndef DESKTOP_VERSION
   if ( mServerNotification )
       AlarmServer::addAlarm ( nextA,"koalarm",mess.latin1());
#endif 
    emit addAlarm( nextA , mess  );  
    mSupendAlarmTime = nextA;
    mSupendAlarmMessage = mess;;
   slotOk();
}

void AlarmDialog::setServerNotification( bool b )
{
    mServerNotification = b;
}
int  AlarmDialog::getSuspendTime( )
{
    return mSuspendSpin->value();

}
void AlarmDialog::setSuspendTime( int val )
{
    mSuspendSpin->setValue( val );
}

void AlarmDialog::showSuspendedAlarms ()
{

    if ( mSupendAlarmMessage  ==  "" ) {
        QMessageBox::information ( this, "KO/Pi Suspended Alarm", "There is currently\nno suspended alarm!", QMessageBox::Ok );
        return;
    }
    QString mess = "Suspended alarm!\nNew alarm time:\n" + mSupendAlarmTime.toString() + "\n\nTitle:\n"+mSupendAlarmTitle+"\n";
    if (QMessageBox::information ( this, "KO/Pi Suspended Alarm", mess, "OK", "Delete this alarm" )) {
            qDebug("Delete suspended alarm" + mSupendAlarmMessage );
#ifndef DESKTOP_VERSION
            AlarmServer::deleteAlarm ( mSupendAlarmTime,"koalarm",mSupendAlarmMessage.latin1());
#endif 
            mSupendAlarmMessage = "";
        }

}

void AlarmDialog::setSuspendDuration ()
{
    int time = getSuspendTime( );
    QDialog* st = new QDialog (this, " ",true, Qt::WStyle_StaysOnTop );
    st->setCaption( "KO/Pi Alarm" );
    QVBoxLayout* layout = new QVBoxLayout( st);
    QLabel* l = new QLabel("Change suspend\nduration (minutes)",st);
    layout->addWidget ( l );
    l->setAlignment( AlignCenter);
    int fs = 18;
    int fs2 = 12;
    int baseSize = 6;
    if ( QApplication::desktop()->width() < 480 ) {
        fs2 = 10;
        fs = 12;
        baseSize = 4;
    }
    layout->setSpacing( 3 );
    layout->setMargin( 3 );
    QFont fo = QApplication::font();
    fo.setBold( true );
    fo.setPointSize( fs2 );
    l->setFont( fo );
    fo.setPointSize( fs );
    fo = font();
    int pointSize = 36;
    if ( QApplication::desktop()->width() <= 320 )
        pointSize = 18;
    fo.setPointSize( pointSize );
    QSpinBox* mSuspendSpin = new QSpinBox(1,60,1,st);
    mSuspendSpin->setFont( fo );
    mSuspendSpin->setValue( time );  // default suspend duration
    mSuspendSpin->setButtonSymbols( QSpinBox::PlusMinus );
    mSuspendSpin->setButtonSymbols( QSpinBox::PlusMinus );
#if QT_VERSION < 0x030000
    mSuspendSpin->upButton ()->setFixedSize( QSize( 8*baseSize, 5*baseSize ));
    mSuspendSpin->downButton ()->setFixedSize( QSize( 8*baseSize, 5*baseSize ));
#endif
    mSuspendSpin->setFixedSize( 18*baseSize, 10*baseSize+2 );
    mSuspendSpin->setButtonSymbols( QSpinBox::PlusMinus ); 
    QHBoxLayout* layoutSpin = new QHBoxLayout( layout );
    layoutSpin->addStretch ();
    layoutSpin->addWidget ( mSuspendSpin );
    layoutSpin->addStretch ();
    QVBox * bbox  = new QVBox ( st );
    layout->addWidget ( bbox );
    bbox->layout()->setSpacing( 2 );
    QPushButton *okbut = new QPushButton( "Set suspend duration", bbox);
    QPushButton* canc = new QPushButton( " Cancel ", bbox);
    okbut->setFont( fo );
    canc->setFont( fo );
    okbut->setDefault( true );
    connect (okbut , SIGNAL( clicked() ), st, SLOT (accept() ) ); 
    connect (canc, SIGNAL( clicked() ), st, SLOT (reject() ) );
    int w = st->minimumSizeHint().width() ;
    int h = st->minimumSizeHint().height() ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    if ( w < 220 ) w = 220;
    if ( h < 220 ) h = 220;
    st->setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    if ( st->exec() ) {
        setSuspendTime( mSuspendSpin->value());
    }
    delete st;
}
bool AlarmDialog::eventNotification( QString mess, int replay ,  QString fn, bool playwav, int pause , int suspendtimes)
{
    if ( mess.left( 9) != "Suspended" )
        mSuspendCounter = suspendtimes;
    mSupendAlarmMessage = "";
    mPauseCount = pause;
    mFileName = fn;
    mPlayWav = playwav;
    if ( !QFile::exists( fn ) )
        mFileName = "";
    alarmCounter = 0 ;
    maxAlarmReplay = replay ;
    mStopAlarm = false;
    mSilent = false;
    if ( !mMessage->text().stripWhiteSpace().isEmpty() ) {
        mMissedAlarmsCombo->show();
        QString newItem = mMessage->text().stripWhiteSpace();
        newItem.replace( QRegExp("\n"), QString(" ") );
        mMissedAlarmsCombo->insertItem( newItem );
        mMissedAlarms->setText( "Missed alarms:");
    } else
        mMissedAlarmsCombo->hide();
    mMessage->setText(mess);
    int w = minimumSizeHint().width() ;
    int h = minimumSizeHint().height() ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    if ( w < 220 ) w = 220;
    if ( h < 220 ) h = 220;
    //setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    //for raspi
    setGeometry( (dw-w)/2, 0 ,w,h );
    showNormal();
    setActiveWindow();
    raise(); 
    //setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    //for raspi
    setGeometry( (dw-w)/2, 0 ,w,h );

    QString command = "sudo chvt 9 && sudo chvt 7";
    qDebug("KO: turning raspi display on" );
    int ret = system ( command.latin1() );
    qDebug("Raspi display on command 97 on returned: %d ",ret );
                

    //hide();

#ifndef _WIN32_
    if (  fd_led > 0 ) {
        statusLED.status =  LED_SALARM_ON ;
        ioctl (fd_led, SHARP_LED_SETSTATUS, &statusLED);
    }
#endif
    okbut->setDefault( true );
    QTimer::singleShot( 1, this, SLOT ( forceRepaint() ) );
    // playSoundTimer->start( 1000, true );
    return true;
 
}

void AlarmDialog::forceRepaint()
{
    setActiveWindow();
    repaint();
    mSuspendSpin->setFocus();
    playSoundTimer->start( 1000, true );

}
void AlarmDialog::spinBoxChanged( int v )
{
    okbut->setDefault( false );
    mSilent = true;
}

void  AlarmDialog::playSound ()
{
    
    if (mStopAlarm )
        return;
    if ( mSilent )
        return;
    //showNormal();
    setActiveWindow();
    //raise(); 
    mSuspendSpin->setFocus();
    if (  alarmCounter < maxAlarmReplay && ! mSilent) {
        ++alarmCounter;
#ifdef DESKTOP_VERSION
        mPlayWav = true;
#endif
        if ( !mPlayWav || mFileName.length() < 2 ) {

#ifdef DESKTOP_VERSION
            qDebug("Sound play not possible - file not found");
#else
            Sound::soundAlarm ();
#endif
        } else

            {
#ifdef DESKTOP_VERSION
#ifdef _WIN32_
                QSound::play ( mFileName );
#else

                //QString command = "playwave -r 22050 " + mFileName;
		//for raspi

                QString command = "aplay " + mFileName;
                qDebug("KO: Playing file %s with 22kHz",mFileName.latin1() );
                int ret = system ( command.latin1() );
                if ( ret != 0 ) {
                    qDebug("Sound play command failed: %s ",command.latin1() );
                }

#endif

#else
            QSound::play ( mFileName );
#endif
            qDebug("BEEP!");
            }
    } else {
        if ( ! mSilent && mSuspendCounter > 0 ) {
            --mSuspendCounter;
            reject (); 
            hide();
            return;
        } 
    }
    playSoundTimer->start( mPauseCount * 1000, true );
}
