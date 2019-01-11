/*
    This file is part of the KOrganizer alarm daemon.
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

#include "simplealarmdaemonimpl.h"

#include "alarmdialog.h"
#include <qpopupmenu.h>
#include <qapp.h>
#include <qdir.h>
#include <qvbox.h>
#include <qfile.h>
#include <qhbox.h>
#include <qtimer.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qdialog.h>
#define protected public
#include <qspinbox.h>
#undef protected 
#include <qtextstream.h> 
#include <qtopia/qcopenvelope_qws.h> 
#include <qtopia/alarmserver.h> 

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


SimpleAlarmDaemonImpl::SimpleAlarmDaemonImpl( QWidget *parent )
    : QLabel( parent )
{
    mAlarmDialog = new AlarmDialog( 0 );
    mPopUp = new QPopupMenu( this );
    mPopUp->insertItem( "What's Next?", this, SLOT ( showWN() ) );
    mPopUp->insertItem( "Next Alarm?", this, SLOT ( showKO() ) );
    //mPopUp->insertSeparator();
    //mPopUp->insertItem( "Todo List", this, SLOT ( showTodo() ) );
    mPopUp->insertSeparator();
    //  mPopUp->insertItem( "Edit Journal", this, SLOT ( writeJournal() ) );
    mPopUp->insertItem( "New Event", this, SLOT ( newEvent() ) );
    mPopUp->insertItem( "New Todo", this, SLOT ( newTodo() ) );
    //  mPopUp->insertItem( "New Mail", this, SLOT ( newMail() ) );
    //mPopUp->insertSeparator();
    //mPopUp->insertItem( "Contacts", this, SLOT ( showAdd() ) );
    mPopUp->insertSeparator();
    mPopUp->insertItem( "Multi Sync", this, SLOT ( ringSync() ) );
    mPopUp->insertSeparator();
    mPopUp->insertItem( "Susp. duration", this, SLOT ( setSuspendDuration() ) );
    mPopUp->insertItem( "Susp. alarm", this, SLOT ( showSuspendedAlarm() ) );
    mTimerPopUp = new QPopupMenu( this );
    
    mBeepPopUp = new QPopupMenu( this );   
    mSoundPopUp = new QPopupMenu( this );   
    mPausePopUp = new QPopupMenu( this ); 
    mFontsizePopup = new QPopupMenu( this ); 
    mFontsizePopup->insertItem( "10", 10 ); 
    mFontsizePopup->insertItem( "12", 12 ); 
    mFontsizePopup->insertItem( "14", 14 ); 
    mFontsizePopup->insertItem( "16", 16 ); 
    mFontsizePopup->insertItem( "18", 18 ); 
    mFontsizePopup->insertItem( "20", 20 ); 
    mFontsizePopup->insertItem( "22", 22 ); 
    mFontsizePopup->insertItem( "24", 24 ); 
    mFontsizePopup->insertItem( "26", 26 ); 
    mFontsizePopup->insertItem( "28", 28 ); 
    mFontsizePopup->insertItem( "30", 30 ); 
    mFontsizePopup->insertItem( "32", 32 ); 
    mFontsizePopup->insertItem( "36", 36 ); 
    mFontsizePopup->setCheckable( true );
    mSoundPopUp->insertItem( "Buzzer", 0 );   
    mSoundPopUp->insertItem( "Wav file", 1 ); 
    mPausePopUp->insertItem( " 1 sec", 1 );
    mPausePopUp->insertItem( " 2 sec", 2 );
    mPausePopUp->insertItem( " 3 sec", 3 );
    mPausePopUp->insertItem( " 5 sec", 5 );
    mPausePopUp->insertItem( "10 sec", 10 );
    mPausePopUp->insertItem( "15 sec", 15 );
    mPausePopUp->insertItem( "30 sec", 30 );
    mPausePopUp->insertItem( " 1 min", 60 );
    mPausePopUp->insertItem( " 5 min", 300 );
    mSuspendPopUp = new QPopupMenu( this );
    QPopupMenu* suspendSettingPopUp = new QPopupMenu( this );
    mSuspendPopUp->insertItem( "Off", 0 ); 
    mSuspendPopUp->insertItem( " 1x", 1 );   
    mSuspendPopUp->insertItem( " 2x", 2 );   
    mSuspendPopUp->insertItem( " 3x", 3 );  
    mSuspendPopUp->insertItem( " 5x", 5 );  
    mSuspendPopUp->insertItem( "10x", 10 );  
    mSuspendPopUp->insertItem( "20x", 20 );  
    mSuspendPopUp->insertItem( "30x", 30 );       
    //mBeepPopUp->insertItem( "Config",savePopUp  ); 
    //mBeepPopUp->insertItem( "Suspend settings",suspendSettingPopUp  );
    //suspendSettingPopUp->insertItem( "Set suspend duration", 0 );   
    //suspendSettingPopUp->insertItem( "Auto  suspend",mSuspendPopUp  );
    //suspendSettingPopUp->insertItem( "Show suspended alarm", 1 );  
    mBeepPopUp->insertItem( "Simulate", 1000 );; 
    mBeepPopUp->insertSeparator();
    mBeepPopUp->insertItem( "100", 100 );         
    mBeepPopUp->insertItem( "50", 50 );       
    mBeepPopUp->insertItem( "25", 25 );    
    mBeepPopUp->insertItem( "17", 17 );         
    mBeepPopUp->insertItem( "10", 10 ); 
    mBeepPopUp->insertItem( "3", 3 );   
    mBeepPopUp->insertItem( "1", 1 );   
    mBeepPopUp->insertItem( "Off", 0 ); 
    mBeepPopUp->setCheckable( true );
    QPopupMenu* configPopUp = new QPopupMenu( this ); 
    configPopUp->insertItem( "Save Config", 0 );   
    configPopUp->insertItem( "Load Config", 1 );  
 
    configPopUp->insertItem( "Replay",mSoundPopUp  );       
    configPopUp->insertItem( "Font Size",mFontsizePopup );
    configPopUp->insertItem( "Auto  suspend",mSuspendPopUp  ); 
    configPopUp->insertItem( "Beep interval",mPausePopUp  );

 
    mPopUp->insertSeparator();
    mPopUp->insertItem( "Play beeps", mBeepPopUp );
    mPopUp->insertSeparator();
    mPopUp->insertItem( "Config", configPopUp );
    mPopUp->insertSeparator();
    mPopUp->insertItem( "Timer", mTimerPopUp );
    //mPopUp->insertSeparator();
    //mPopUp->insertItem( "Simulate", this, SLOT ( simulate() ) );
 
    mPopUp->resize( mPopUp->sizeHint()  );
    mPlayBeeps = 17;
    mBeepPopUp->setItemChecked ( mPlayBeeps, true );
    connect ( mBeepPopUp, SIGNAL( activated ( int )  ), this, SLOT (slotPlayBeep( int ) ) );
    connect ( mTimerPopUp, SIGNAL( activated ( int )  ), this, SLOT (confTimer( int ) ) );
    connect ( mTimerPopUp, SIGNAL(aboutToShow()  ), this, SLOT ( showTimer( ) ) ); 
    connect ( mSoundPopUp, SIGNAL( activated ( int )  ), this, SLOT (confSound( int ) ) );
    connect ( mPausePopUp, SIGNAL( activated ( int )  ), this, SLOT (confPause( int ) ) );
    connect ( mSuspendPopUp, SIGNAL( activated ( int )  ), this, SLOT (confSuspend( int ) ) );  
    connect ( configPopUp, SIGNAL( activated ( int )  ), this, SLOT (saveSlot( int ) ) ); 
    connect ( mFontsizePopup, SIGNAL( activated ( int )  ), this, SLOT (confFontSize( int ) ) );
    connect ( suspendSettingPopUp, SIGNAL( activated ( int )  ), this, SLOT (suspendSettings( int ) ) );
    mTimerTime = 0;
    mCustomText = "Custom Text";
    mCustomMinutes = 7;
    mTimerPopupConf = 1;
    fillTimerPopUp();
    mPausePlay = 0;
    confPause( 1 );
    mSuspend = 0;
    confSuspend( 0 );
    if ( QApplication::desktop()->width() < 480 ) {
        wavAlarm = false;
        mSoundPopUp->setItemChecked ( 0, true );
    }
    else {
        wavAlarm = true;
        mSoundPopUp->setItemChecked ( 1, true );
    }
    mTimerStartLabel = new QLabel( 0, 0, WType_Popup );
    //mTimerStartLabel->setCaption( "Timer started!");  
    mTimerStartLabel->setAlignment ( Qt::AlignCenter ) ;
    saveSlot( 1 );


    
}

SimpleAlarmDaemonImpl::~SimpleAlarmDaemonImpl()
{
    //delete mPopUp;
    delete mAlarmDialog;
    delete mTimerStartLabel;
}

void SimpleAlarmDaemonImpl::saveSlot( int load )
{
    QString fileName = QDir::homeDirPath() +"/.kopialarmrc";
    //qDebug("save %d ", load );
    QFile file( fileName );
    if ( load ) {
        mPopupFontSize  = mTimerPopUp->font().pointSize();
        confFontSize( mPopupFontSize );
        if( !QFile::exists(  fileName) )
            return;
         if (!file.open( IO_ReadOnly ) ) {
            return ;
        } 
         QString line;
         bool ok;
         int val;
         int len;
         while ( file.readLine( line, 1024 ) > 0 ) {
             //qDebug("read %s  ", line.latin1());
             len = line.length();
             if ( line.left(4 ) == "PPAU" ) {
                 val = line.mid( 4,len-5).toInt( &ok );
                 if ( ok ) {
                     confPause( val );
                 }
             }
             if ( line.left(4 ) == "POFO" ) {
                 val = line.mid( 4,len-5).toInt( &ok );
                 if ( ok ) {
                     confFontSize( val );
                 }
             }
             if ( line.left(4 ) == "SUCO" )  {
                 val = line.mid( 4,len-5).toInt( &ok );
                 if ( ok )
                     confSuspend ( val );
             }
             if ( line.left(4 ) == "WAAL" )  {
                 val = line.mid( 4,len-5).toInt( &ok );
                 if ( ok )
                     confSound( val );

             }
             if ( line.left(4 ) == "PLBE" )  {
                 val = line.mid( 4,len-5).toInt( &ok );
                 if ( ok )
                     slotPlayBeep( val );

             }
             if ( line.left(4 ) == "CUTE" ) {
                 mCustomText = line.mid( 5,len-6);
                 // qDebug("text ***%s*** ",mCustomText.latin1() );

             } 
             if ( line.left(4 ) == "CUMI" )  {
                 val = line.mid( 4,len-5).toInt( &ok );
                 if ( ok )
                     mCustomMinutes = val;

             }
             if ( line.left(4 ) == "SUTI" )  {
                 val = line.mid( 4,len-5).toInt( &ok );
                 if ( ok )
                     mAlarmDialog->setSuspendTime( val );;

             }
         }
         file.close();
    } else {
        if (!file.open( IO_WriteOnly ) ) {
            return;
        } 
        QString configString ;
        configString += "PPAU " + QString::number( mPausePlay ) + "\n";
        configString += "SUCO " + QString::number( mSuspend ) + "\n";
        configString += "POFO " + QString::number( mPopupFontSize ) + "\n";
        configString += "WAAL " + QString::number( wavAlarm ) + "\n";
        configString += "PLBE " + QString::number( mPlayBeeps ) + "\n";
        configString += "CUTE " + mCustomText + "\n";
        configString += "CUMI " + QString::number( mCustomMinutes ) + "\n";
        configString += "SUTI " + QString::number( mAlarmDialog->getSuspendTime( )) + "\n";
        QTextStream ts( &file );
        ts << configString ;
        file.close();
    }

}


void SimpleAlarmDaemonImpl::setSuspendDuration()
{
    mAlarmDialog->setSuspendDuration ();
}
void SimpleAlarmDaemonImpl::showSuspendedAlarm()
{
    mAlarmDialog->showSuspendedAlarms ();
}

void  SimpleAlarmDaemonImpl::suspendSettings( int num )
{
    if ( num == 0 ) {
        mAlarmDialog->setSuspendDuration ();
    } else if ( num == 1 ) {
        mAlarmDialog->showSuspendedAlarms ();
    }
}
void  SimpleAlarmDaemonImpl::confSuspend( int num )
{
    mSuspendPopUp->setItemChecked ( mSuspend,false );
    mSuspend = num;
    mSuspendPopUp->setItemChecked ( mSuspend,true );
}
void  SimpleAlarmDaemonImpl::confPause( int num )
{
    mPausePopUp->setItemChecked ( mPausePlay,false );
    mPausePlay = num;
    mPausePopUp->setItemChecked ( mPausePlay,true );
}
void  SimpleAlarmDaemonImpl::confSound( int num )
{
    if ( num == 0 ) {
        wavAlarm = false;
        mSoundPopUp->setItemChecked ( 0, true );
        mSoundPopUp->setItemChecked ( 1, false );
    } else {
        wavAlarm = true;
        mSoundPopUp->setItemChecked ( 0, false );
        mSoundPopUp->setItemChecked ( 1, true );
    }
}
void  SimpleAlarmDaemonImpl::slotPlayBeep( int num )
{
    if ( num == 1000 ) {
        simulate();
        return;
    }
    mBeepPopUp->setItemChecked ( mPlayBeeps,false );
    mPlayBeeps = num;
    mBeepPopUp->setItemChecked ( mPlayBeeps, true );
}

void SimpleAlarmDaemonImpl::recieve( const QCString& msg, const QByteArray&  )
{
    //qDebug("SimpleAlarmDaemonImpl::ALARM RECEIVED! %s", msg.data()); 
    QString mess = QString::fromUtf8(msg.data()); 
    mAlarmMessage = mess.mid( 9 ); 
    QString filename =  getenv("QPEDIR") ;
    filename += "/pics/kdepim/korganizer/koalarm.wav";
    QString tempfilename;
    if ( mess.left( 13 )  == "suspend_alarm") {  
        bool error = false;
        int len = mess.mid( 13 ).find("+++");
        if ( len < 2 )
            error = true;
        else {
            tempfilename = mess.mid( 13, len );
            if ( !QFile::exists( tempfilename  ) )
                error = true;
        }
        if ( ! error ) {
            filename = tempfilename;
        }
        mAlarmMessage = mess.mid( 13+len+3 );
        //qDebug("suspend file %s ",tempfilename.latin1() );
        startAlarm( mAlarmMessage, filename);
        return;
    }
    if ( mess.left( 11 )  == "timer_alarm") {
        mTimerTime = 0;
        startAlarm( mess.mid( 11 ), filename );
        return;
    } 
    if ( mess.left( 10 )  == "proc_alarm") {
        bool error = false;
        int len = mess.mid( 10 ).find("+++");
        if ( len < 2 )
            error = true;
        else {
            tempfilename = mess.mid( 10, len );
            if ( !QFile::exists( tempfilename  ) )
                error = true;
        }
        if ( error ) {
            mAlarmMessage = "Procedure Alarm\nError - File not found\n";
            mAlarmMessage += mess.mid( 10+len+3+9 );
        } else {
            //qDebug("-----system command %s ",tempfilename.latin1() );
            if ( vfork () == 0 ) {
                execl ( tempfilename.latin1(), 0 );
                return;
            }
            QTimer::singleShot( 2000, this, SLOT ( writeFile() ) );
            return;
        }
       
        //qDebug("+++++++system command %s ",tempfilename.latin1() );
    }  
    if ( mess.left( 11 )  == "audio_alarm") {
        bool error = false;
        int len = mess.mid( 11 ).find("+++");
        if ( len < 2 )
            error = true;
        else {
            tempfilename = mess.mid( 11, len );
            if ( !QFile::exists( tempfilename  ) )
                error = true;
        }
        if ( ! error ) {
            filename = tempfilename;
        }
        mAlarmMessage = mess.mid( 11+len+3+9 );
        //qDebug("audio file command %s ",tempfilename.latin1() );
    } 
    if ( mess.left( 9 )  == "cal_alarm") {
        mAlarmMessage = mess.mid( 9 ) ;
    }
    
    QTimer::singleShot( 2000, this, SLOT ( writeFile() ) );
    startAlarm( mAlarmMessage, filename );
     
}

int  SimpleAlarmDaemonImpl::getFileNameLen( QString mess  )
{
    return 0;
}
void SimpleAlarmDaemonImpl::startAlarm( QString mess, QString filename )
{
    //mAlarmDialog->show(); 
    //mAlarmDialog->raise();
    mAlarmDialog->eventNotification( mess, mPlayBeeps, filename, wavAlarm,mPausePlay ,mSuspend );
}


void SimpleAlarmDaemonImpl::fillTimerPopUp()
{
   
    // qDebug(" timer %d %d ",mTimerPopupConf,  mTimerTime  );
    if ( mTimerPopupConf == mTimerTime ) {
        if ( mTimerTime ) {
            int secs = QDateTime::currentDateTime().secsTo ( mRunningTimer );
            QTime t ( secs/3600, (secs/60)%60,  secs%60  ); 
            mTimerPopUp->changeItem ( 1 , t.toString() + " (countdown)");
        }
        else { 
            QString text = mCustomText.stripWhiteSpace ();
            int in = text.find( " " );
            text = text.left ( in );
            mTimerPopUp->changeItem ( 3,  text );
        }
        return; 
    }
    mTimerPopupConf = mTimerTime;
    mTimerPopUp->clear();
    if ( mTimerTime ) {
        int secs = QDateTime::currentDateTime().secsTo ( mRunningTimer );
        QTime t ( secs/3600, (secs/60)%60,  secs%60  ); 
       
        
        mTimerPopUp->insertItem( "Stop timer "+ mRunningTimerText , 0 ); 
        mTimerPopUp->insertItem( t.toString() + " (countdown)",1); 
        mTimerPopUp->insertItem( mRunningTimer.time().toString() + " (alarm)",2); 
    } else {

        QString fileName = QDir::homeDirPath() +"/.kopialarmtimerrc";
        QFile file( fileName );
        if( !QFile::exists(  fileName) ) {
            // write defaults
            if (!file.open( IO_WriteOnly ) ) {
                return;
            } 
            QString configString ;
            configString += "#config file for kopi alarm timer\n";
            configString += "#format:  <Text for popup menu>;<timer countdown in minutes>\n";
            configString += "24 hours; 1440\n";
            configString += " 8 hours; 480\n";
            configString += " 2 hours; 120\n";
            configString += " 1 hour; 60\n";
            configString += "SEPARATOR\n";
            configString += "45 min; 45\n";
            configString += "30 min; 30\n";
            configString += "15 min; 15\n";
            configString += " 5 min; 5\n";
            configString += "SEPARATOR\n";
            QTextStream ts( &file );
            ts << configString ;
            file.close();
        }
         
        if (!file.open( IO_ReadOnly ) ) {
            return ;
        } 
        QString line;
        bool ok;
        while ( file.readLine( line, 1024 ) > 0 ) {
            //qDebug("read %s  ", line.latin1());
            if ( line.left(1 ) != "#" ) {
                // no comment
                if ( line.left(9 ) == "SEPARATOR" )  {
                    mTimerPopUp->insertSeparator(); 
                } else {
                    QStringList li = QStringList::split(";",line);
                    ok = false;
                    if ( li.count() == 2 ) {
                        int val = li[1].toInt( &ok );
                        if ( ok && val > 0 ) {
                            mTimerPopUp->insertItem( li[0], val+10);
                        }
                    }
                }
            }
        }
        file.close();
#if 0
        mTimerPopUp->insertItem( "24 hours", 1440 );
        // mTimerPopUp->insertItem( i18n("12 h"), 720 );
        mTimerPopUp->insertItem( " 8 hours", 480 );
        mTimerPopUp->insertItem( " 5 hours", 300 );
        // mTimerPopUp->insertItem( i18n(" 2 h"), 120 );
        mTimerPopUp->insertItem( " 1 hour", 60 );   
        mTimerPopUp->insertItem( "30 min", 30 );   
        mTimerPopUp->insertItem( "15 min", 15 );   
        mTimerPopUp->insertItem( "10 min", 10 );   
        //mTimerPopUp->insertItem( " 5 min", 5 );  
        mTimerPopUp->insertSeparator();  
        mTimerPopUp->insertItem( "Pizza", 22 );  
        mTimerPopUp->insertItem( "Nap",  45 );  
        mTimerPopUp->insertItem( "Tea", 5 );  
#endif
        QString text = mCustomText.stripWhiteSpace ();
        int in = text.find( " " );
        text = text.left ( in );
        mTimerPopUp->insertItem( text, 3 );
        mTimerPopUp->insertSeparator(); 
        mTimerPopUp->insertItem( "Customize", 2 ); 
    }

}

void SimpleAlarmDaemonImpl::showTimer()
{
    fillTimerPopUp();
}
   
void SimpleAlarmDaemonImpl::confTimer( int time )
{
    //qDebug("impleAlarmDaemonImpl::confTimer() %d ", time );
    int minutes = time;
    if ( minutes == 0 ) {
        if ( ! mTimerTime ) 
            return;

        QDialog dia ( 0,  ("Stop Timer" ), true ); 
        QLabel lab (("Really stop the timer?\n\n"+ mRunningTimerText+"\n"), &dia ); 
        lab.setAlignment( AlignCenter );
        dia.setCaption(("KO/Pi Timer Stop" )); 
        QVBoxLayout lay( &dia ); 
        lay.addWidget( &lab);
        QPushButton ok ( "Stop timer!", &dia); 
        QFont fo = dia.font();
        fo.setPointSize( 36 );
        ok.setFont(  fo );
        lay.addWidget( &ok);
        connect ( &ok, SIGNAL (clicked()), &dia, SLOT ( accept() ) );
        QPushButton con ( "Continue timer!", &dia); 
        fo.setPointSize( 36 );
        con.setFont(  fo );
        lay.addWidget( &con);
        connect ( &con, SIGNAL (clicked()), &dia, SLOT ( reject() ) );
        lay.setMargin(5); 
        lay.setSpacing(5); 
        dia.resize(dia.sizeHint() );
       
        if ( !dia.exec() )
            return;
        
        AlarmServer::deleteAlarm ( mRunningTimer,"koalarm"  , timerMesssage.utf8() );
        mTimerTime = 0;
        return;
    }
    if ( mTimerTime )
        return;
    if ( minutes == 1 ) {
        return;
    }
    QString mess = "timer_alarm";
    QString disp;
    mess += ("Timer Alarm!\n");
    if ( minutes ==  3 ) {
        mess += mCustomText;
        minutes  = mCustomMinutes ; 
        mRunningTimerText = mCustomText.stripWhiteSpace ();
        int in = mRunningTimerText.find( " " );
        mRunningTimerText = mRunningTimerText.left ( in );
        disp = mCustomText;
    }
    else  { 
        if ( minutes == 2 ) {
            // ask time
            QDialog dia ( 0,  ("Customize Timer" ), true ); 
            QLabel lab (("Message Text:"), &dia ); 
            dia.setCaption(("KO/Pi Timer" )); 
            QVBoxLayout lay( &dia );
            lay.setMargin(5); 
            lay.setSpacing(5); 
            lay.addWidget( &lab);
            QLineEdit lEdit( mCustomText, &dia );
            lay.addWidget( &lEdit);
            QLabel lab2 (("Countdown time (1 min - 24 h):"), &dia );
            lay.addWidget( &lab2); 
            QHBox hbox1 ( &dia  );
            lay.addWidget( &hbox1);
            QLabel lab3 (("Hours"), &hbox1 );
            QLabel lab4 (("Minutes"), &hbox1 );
            QHBox hbox ( &dia  );
            QSpinBox spinh( 0, 24, 1,& hbox );
            QFont fo = dia.font();
            fo.setPointSize( 36 );
            QSpinBox spinm( 0, 59, 1,&hbox );
            spinm.setFont( fo );
            spinh.setFont( fo );
            spinh.setButtonSymbols( QSpinBox::PlusMinus );
            spinm.setButtonSymbols( QSpinBox::PlusMinus );
            spinh.upButton ()->setFixedSize( QSize( 48, 30 ));
            spinh.downButton ()->setFixedSize( QSize( 48, 30 ));
            //spinh.editor ()->setFixedSize( QSize( 50, 100 ));
            spinh.setFixedSize( 100,62 );
            spinm.upButton ()->setFixedSize( QSize( 48, 30 ));
            spinm.downButton ()->setFixedSize( QSize( 48, 30 ));
            spinm.downButton ()->setGeometry( 50,50,50,50); 
            // spinm.setSuffix( " m" );  
            //spinh.setSuffix( " h" ); 
            spinm.setWrapping ( true );
            //spinm.editor ()->setFixedSize( QSize( 50, 100 ));
            spinm.setLineStep( 1 );
            spinm.setFixedSize( 110,62 );
            lay.addWidget( &hbox);
            QLabel lab5 ("Timer fires at:", &dia );
            lab5.setAlignment( AlignCenter );
            lay.addWidget( &lab5);
            KODateLabel dl (  &dia );
            dl.setAlignment( AlignCenter );
            dl.setFont( fo  );
            connect ( &spinh, SIGNAL ( valueChanged (int)), &dl, SLOT ( slot_hours( int ) ) );
            connect ( &spinm, SIGNAL ( valueChanged (int)), &dl, SLOT ( slot_minutes( int ) ) );
            lay.addWidget( &dl);
            spinh.setValue( mCustomMinutes/60 );
            spinm.setValue( mCustomMinutes%60 );
            QPushButton ok ( "Start timer", &dia);
            ok.setDefault( true );
            ok.setFont(  fo );
            spinh.setFocus();
            lay.addWidget( &ok);
            connect ( &ok, SIGNAL (clicked()), &dia, SLOT ( accept() ) );
            dia.resize( dia.sizeHint().width(), dia.sizeHint().height() );
            
            if ( !dia.exec() )
                return;
            mCustomText = lEdit.text();
            mCustomMinutes = spinh.value()*60+spinm.value();
            if ( mCustomMinutes == 0 )
                mCustomMinutes = 1;
            if ( mCustomMinutes > 1440 )
                mCustomMinutes = 1440;
            mess += mCustomText;
            disp = mCustomText;
            minutes = mCustomMinutes; 
            mRunningTimerText = mCustomText.stripWhiteSpace ();
            int in = mRunningTimerText.find( " " );
            mRunningTimerText = mRunningTimerText.left ( in );
        }
        else {
            mess += mTimerPopUp->text( minutes );
            disp = mTimerPopUp->text( minutes );
            mRunningTimerText = mTimerPopUp->text( minutes );
            minutes -= 10;
        }
    }
    //minutes = 1;

    mRunningTimer = QDateTime::currentDateTime().addSecs( minutes * 60 );
    timerMesssage = mess;
    QString timerDuration ;
    if ( minutes < 60 ) {
        timerDuration = QString::number( minutes ) + " min";
    } else {
        if ( minutes % 60 ) {
            timerDuration = QString::number( minutes/60 ) +":";
                minutes = minutes%60;
            if ( minutes < 10 ) timerDuration += "0";
               timerDuration += QString::number( minutes ) + " h";
        }
        else
            timerDuration = QString::number( minutes / 60 )+ " hours";
    }
    AlarmServer::addAlarm ( mRunningTimer,"koalarm",timerMesssage.utf8());
    mTimerStartLabel->setText( disp + "\n\n" + timerDuration +"\n\nTimer started!" );  
    int w = mTimerStartLabel->sizeHint().width()+20;
    int h = mTimerStartLabel->sizeHint().height()+40 ;
    int dw = QApplication::desktop()->width();
    int dh = QApplication::desktop()->height();
    mTimerStartLabel->setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
    mTimerStartLabel->show();
    QTimer::singleShot( 3000, mTimerStartLabel, SLOT ( hide() )  );
    mTimerTime = 1;
}

void SimpleAlarmDaemonImpl::confFontSize( int size )
{
    qDebug(QString("conf %1").arg(size));
    mFontsizePopup->setItemChecked( mPopupFontSize, false );
    mPopupFontSize = size;
    mFontsizePopup->setItemChecked( mPopupFontSize, true );
    QFont fon = mTimerPopUp->font();
    fon.setPointSize(  mPopupFontSize );
    mTimerPopUp->setFont( fon );
    mBeepPopUp->setFont( fon );
    mPopUp->setFont( fon );
    fon.setBold( true );
    fon.setPointSize( mPopupFontSize * 2 );
    mTimerStartLabel->setFont( fon );
}
void SimpleAlarmDaemonImpl::writeFile()
{ 
    QCopEnvelope e("QPE/Application/kopi", "-writeFileSilent");
    //QCopEnvelope e("QPE/Application/kopi", "-writeFile");
}
void SimpleAlarmDaemonImpl::showWN()
{
    QCopEnvelope e("QPE/Application/kopi", "-showWN");
}
void SimpleAlarmDaemonImpl::newTodo()
{
    QCopEnvelope e("QPE/Application/kopi", "-newTodo");
}

void SimpleAlarmDaemonImpl::newEvent()
{
    QCopEnvelope e("QPE/Application/kopi", "-newEvent");

}
void SimpleAlarmDaemonImpl::newMail()
{
 QCopEnvelope e("QPE/Application/ompi", "newMail()");
}
void SimpleAlarmDaemonImpl::showAdd()
{
 QCopEnvelope e("QPE/Application/kapi", "raise()");
}
void SimpleAlarmDaemonImpl::ringSync()
{
    QCopEnvelope e("QPE/Application/kopi", "-ringSync");

}
void SimpleAlarmDaemonImpl::newCountdown()
{
    //recieve("cal_alarm", 10 );
}
void SimpleAlarmDaemonImpl::simulate()
{
    QTimer::singleShot( 2000, this, SLOT ( writeFile() ) );
    QString filename =  getenv("QPEDIR") ;
    filename += "/pics/kdepim/korganizer/koalarm.wav";
    startAlarm("Alarm simulation", filename );
}
void SimpleAlarmDaemonImpl::showKO()
{
     QCopEnvelope e("QPE/Application/kopi", "-showKO");
    // testing only
    //QCopEnvelope e("QPE/Application/kopi", "nextView()");

}
void SimpleAlarmDaemonImpl::showTodo()
{
    QCopEnvelope e("QPE/Application/kopi", "-showTodo");

}
void SimpleAlarmDaemonImpl::writeJournal()
{
    QCopEnvelope e("QPE/Application/kopi", "-showJournal");

}

void SimpleAlarmDaemonImpl::mousePressEvent( QMouseEvent * )
{

    mPopUp->popup(mapToGlobal(QPoint (0, -mPopUp->height() )));

}

