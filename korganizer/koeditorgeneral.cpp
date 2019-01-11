/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <qwidget.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qvgroupbox.h>
#include <qwidgetstack.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qfile.h>
#include <qregexp.h>


#include <kglobal.h>
#include <kdialog.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>

#include <libkcal/todo.h>
#include <libkcal/event.h>
#include <libkdepim/categoryselectdialog.h>
#include <libkdepim/kdateedit.h>

#include "koprefs.h"
#include "koglobals.h"

#include "koeditorgeneral.h"
#include "kolocationbox.h"
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

KOEditorGeneral::KOEditorGeneral(QObject* parent, const char* name) :
    QObject( parent, name)
{
    mNextFocus = 0;
}

KOEditorGeneral::~KOEditorGeneral()
{
}

void KOEditorGeneral::initHeader(QWidget *parent,QBoxLayout *topLayout)
{
    QGridLayout *headerLayout = new QGridLayout(topLayout);

#if 0
    mOwnerLabel = new QLabel(i18n("Owner:"),parent);
    headerLayout->addMultiCellWidget(mOwnerLabel,0,0,0,1);
#endif
    // 1 on pda
    // 11 on desktop
    headerLayout->setSpacing( (KDialog::spacingHint()-3)*2+1 );
    QLabel *summaryLabel = new QLabel(i18n("Summary:"),parent);
    headerLayout->addWidget(summaryLabel,0,0);

    mSummaryEdit = new KOLocationBox(TRUE,parent, 50);
    mSummaryEdit->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred  ,QSizePolicy::MinimumExpanding  ,FALSE) );
    //mSummaryEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5 ,(QSizePolicy::SizeType)3 ,FALSE) );
    //qDebug("h %d %d ", summaryLabel->sizeHint().height(),mSummaryEdit->sizeHint().height() );
    int hei =  (summaryLabel->sizeHint().height() + mSummaryEdit->sizeHint().height())/2;
    if ( QApplication::desktop()->width() > 320 ) 
        mSummaryEdit->setMaximumHeight( hei +6 );
    //qDebug("%d %d %d %d %d %d ", QSizePolicy::Fixed ,  QSizePolicy::Minimum ,   QSizePolicy:: Maximum ,   QSizePolicy:: Preferred  ,    QSizePolicy::  MinimumExpanding , QSizePolicy::Expanding   );
    // SizeType { Fixed = 0, Minimum = MayGrow, Maximum = MayShrink, Preferred = MayGrow|MayShrink, MinimumExpanding = Minimum|ExpMask, Expanding = MinimumExpanding|MayShrink }
    //   mSummaryEdit = new QLineEdit(parent);
    if ( QApplication::desktop()->height() < 320 ) 
        headerLayout->addWidget(mSummaryEdit,0,1);
    else
        headerLayout->addMultiCellWidget(mSummaryEdit,0,0,1,2);
    connect ( mSummaryEdit->lineEdit(), SIGNAL ( returnPressed() ), this, SIGNAL (allAccepted () ) );
  
    QLabel *locationLabel = new QLabel(i18n("Location:"),parent);
    if ( QApplication::desktop()->height() < 320 ) 
        headerLayout->addWidget(locationLabel,0,2);
    else
        headerLayout->addWidget(locationLabel,1,0);

    mLocationEdit = new KOLocationBox(TRUE,parent,30); 
    mLocationEdit->setSizePolicy( QSizePolicy( QSizePolicy::Preferred  ,QSizePolicy::MinimumExpanding ,FALSE) );
    if ( QApplication::desktop()->width() > 320 && QApplication::desktop()->height() > 240 ) 
        mLocationEdit->setMaximumHeight( hei + 6);

    //   mLocationEdit = new QLineEdit(parent);
    connect ( mLocationEdit->lineEdit(), SIGNAL ( returnPressed() ), this, SIGNAL (allAccepted () ) );

    mCalendarBox = new QComboBox ( parent );
    mCalendarBox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred  ,QSizePolicy::MinimumExpanding ,FALSE) );
    if ( QApplication::desktop()->height() < 320 ) {
        headerLayout->addWidget(mLocationEdit,0,3);
        headerLayout->addWidget(mCalendarBox,0,4);
        headerLayout->setColStretch( 1, 10);
        headerLayout->setColStretch( 3, 10);
        mCalendarBox->setMaximumWidth( 64 );
    }
    else {
        headerLayout->addWidget(mLocationEdit,1,1);
        headerLayout->addWidget(mCalendarBox,1,2);
        int str = 3;
        if ( QApplication::desktop()->width() < 640 ) {
            --str;
            if ( QApplication::desktop()->width() < 320 )
                --str;
        }
        headerLayout->setColStretch( 1, str);
        headerLayout->setColStretch( 2, 1);
    }
    
}
void KOEditorGeneral::setFocusOn( int i )
{
    mNextFocus = i;
    QTimer::singleShot( 0, this, SLOT ( slotSetFocusOn() ));
}
void KOEditorGeneral::slotSetFocusOn()
{
    if ( mNextFocus == 1 ) {
        mDescriptionEdit->setFocus();
        mDescriptionEdit->setCursorPosition( mDescriptionEdit->numLines (), 333);
    } 
    if ( mNextFocus == 2 ) {
        mSummaryEdit->setFocus();
    }
}
void KOEditorGeneral::editCategories()
{
    // qDebug("KOEditorGeneral::editCategories() ");
    KPIM::CategorySelectDialog* csd = new KPIM::CategorySelectDialog( KOPrefs::instance(), 0 );
    connect(csd,SIGNAL(categoriesSelected(const QString &)), this ,SLOT(setCategories(const QString &)));
    //KOGlobals::fitDialogToScreen( csd );
    csd->setColorEnabled();
    csd->setSelected( QStringList::split (",", mCategoriesLabel->text()) );
    csd->exec();
    delete csd;
}

void KOEditorGeneral::showCatPopup()
{
    mCatPopup->clear(); 
    QStringList checkedCategories = QStringList::split (",", mCategoriesLabel->text());
    int index = 0;
    for (QStringList::Iterator it = KOPrefs::instance()->mCustomCategories.begin ();
         it != KOPrefs::instance()->mCustomCategories.end ();
         ++it) {
        mCatPopup->insertItem (*it, index );
        //mCategory[index] = *it;
        if (checkedCategories.find (*it) != checkedCategories.end ()) mCatPopup->setItemChecked (index, true);
        ++index;
    }
}
void KOEditorGeneral::selectedCatPopup( int index )
{
    QStringList categories =  QStringList::split (",", mCategoriesLabel->text());
    QString colcat = categories.first();
    if (categories.find (KOPrefs::instance()->mCustomCategories[index]) != categories.end ())
      categories.remove (KOPrefs::instance()->mCustomCategories[index]);
    else
        categories.insert (categories.end(), KOPrefs::instance()->mCustomCategories[index]);
    categories.sort ();
    if ( !colcat.isEmpty() ) {
        if ( categories.find ( colcat ) != categories.end () ) {
            categories.remove( colcat );
            categories.prepend( colcat );
        }
    }
    setCategories( categories.join(",") );
}

void KOEditorGeneral::initCategories(QWidget *parent, QBoxLayout *topLayout)
{
    QBoxLayout *categoriesLayout = new QHBoxLayout( topLayout );
    mCatPopup = new QPopupMenu ( parent );
    mCatPopup->setCheckable (true);
    connect(mCatPopup,SIGNAL(aboutToShow ()  ), this ,SLOT(showCatPopup()));
    connect(mCatPopup,SIGNAL(  activated ( int ) ), this ,SLOT(selectedCatPopup( int )));
    mCategoriesButton = new QPushButton(parent);
    mCategoriesButton->setText(i18n("Categories..."));
    connect(mCategoriesButton,SIGNAL(clicked()),this, SLOT(editCategories() ));
    //connect(mCategoriesButton,SIGNAL(clicked()),this, SLOT(editCategories() ));
    categoriesLayout->addWidget(mCategoriesButton);
    mCategoriesLabel = new QPushButton(parent);//new QLabel(parent);
    mCategoriesLabel->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred  ,QSizePolicy::Fixed  ,FALSE) );
    mCategoriesLabel->setPopup( mCatPopup );
    //mCategoriesLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    categoriesLayout->addWidget(mCategoriesLabel,1);
}

void KOEditorGeneral::initSecrecy(QWidget *parent, QBoxLayout *topLayout)
{
    QBoxLayout *secrecyLayout = new QHBoxLayout( topLayout );

    QLabel *secrecyLabel = new QLabel(i18n("Access:"),parent);
    mCancelBox = new QCheckBox ( i18n("Cancelled"), parent);
    secrecyLayout->addWidget(mCancelBox);
    secrecyLayout->addWidget(secrecyLabel);

    mSecrecyCombo = new QComboBox(parent);
    mSecrecyCombo->insertStringList(Incidence::secrecyList());
    secrecyLayout->addWidget(mSecrecyCombo);
}

void KOEditorGeneral::initDescription(QWidget *parent,QBoxLayout *topLayout)
{
    mDescriptionEdit = new KTextEdit(parent);
    mDescriptionEdit->setFont(KOPrefs::instance()->mEditBoxFont );
    mDescriptionEdit->append("");
    mDescriptionEdit->setReadOnly(false);
    mDescriptionEdit->setOverwriteMode(false);
    mDescriptionEdit->setWordWrap( KTextEdit::WidgetWidth ); 
    topLayout->addWidget(mDescriptionEdit); 
#ifndef DESKTOP_VERSION
    QPEApplication::setStylusOperation( mDescriptionEdit, QPEApplication::RightOnHold );
#endif
 
}

void KOEditorGeneral::initAlarm(QWidget *parent,QBoxLayout *topLayout)
{
    QBoxLayout *alarmLayout = new QHBoxLayout(topLayout);

    //mAlarmBell = new QLabel(parent);
    //mAlarmBell->setPixmap(SmallIcon("bell"));
    //alarmLayout->addWidget(mAlarmBell);
    if ( QApplication::desktop()->width() < 320 ) 
        mAlarmButton = new QCheckBox(i18n("Rem."),parent);
    else
        mAlarmButton = new QCheckBox(i18n("Reminder:"),parent);
      
    connect(mAlarmButton, SIGNAL(toggled(bool)), SLOT(enableAlarmEdit(bool)));
    alarmLayout->addWidget(mAlarmButton);

    mAlarmTimeEdit = new QSpinBox ( 0, 9999, 1, parent, "mAlarmTimeEdit  "  ) ;
    mAlarmTimeEdit->setButtonSymbols( QSpinBox::PlusMinus );
    alarmLayout->addWidget(mAlarmTimeEdit);
    mAlarmIncrCombo = new QComboBox(false, parent);
    if ( QApplication::desktop()->width() < 320 ) {
        mAlarmIncrCombo->insertItem(i18n("min"));
        mAlarmIncrCombo->insertItem(i18n("hou"));
        mAlarmIncrCombo->insertItem(i18n("day"));
        mAlarmTimeEdit->setMaximumWidth( mAlarmTimeEdit->sizeHint().width() );
        mAlarmIncrCombo->setMaximumWidth( mAlarmIncrCombo->sizeHint().width() );
    } else {
        mAlarmIncrCombo->insertItem(i18n("minute(s)"));
        mAlarmIncrCombo->insertItem(i18n("hour(s)"));
        mAlarmIncrCombo->insertItem(i18n("day(s)"));
    }

    //  mAlarmIncrCombo->setMinimumHeight(20);
    alarmLayout->addWidget(mAlarmIncrCombo);
    mAlarmSoundButton = new QPushButton(parent);
    mAlarmSoundButton->setPixmap(SmallIcon("playsound"));
    mAlarmSoundButton->setToggleButton(true);
    QToolTip::add(mAlarmSoundButton, i18n("No sound set"));
    connect(mAlarmSoundButton, SIGNAL(clicked()), SLOT(pickAlarmSound()));
    alarmLayout->addWidget(mAlarmSoundButton);

    mAlarmProgramButton = new QPushButton(parent);
    mAlarmProgramButton->setPixmap(SmallIcon("run"));
    mAlarmProgramButton->setToggleButton(true);
    QToolTip::add(mAlarmProgramButton, i18n("No program set"));
    connect(mAlarmProgramButton, SIGNAL(clicked()), SLOT(pickAlarmProgram()));
    alarmLayout->addWidget(mAlarmProgramButton);
    mAlarmSoundButton->setMaximumWidth( mAlarmSoundButton->sizeHint().width() + 4 );
    mAlarmProgramButton->setMaximumWidth(mAlarmProgramButton->sizeHint().width() + 4 );
    //   if ( KOPrefs::instance()->mCompactDialogs ) {
    //     mAlarmSoundButton->hide();
    //     mAlarmProgramButton->hide();
    //   }
}

void KOEditorGeneral::pickAlarmSound()
{

    //qDebug("KOEditorGeneral::pickAlarmSound() %d",mAlarmSoundButton->isOn() );
    
    bool oldState = mAlarmSoundButton->isOn();
   
    QString fileName(KFileDialog::getOpenFileName(mAlarmSound,
                                                  i18n("*.wav|Wav Files"), 0));
    if (!fileName.isEmpty()) {
        mAlarmSound = fileName;
        QToolTip::remove(mAlarmSoundButton);
        QString dispStr = i18n("Playing '%1'").arg(fileName);
        QToolTip::add(mAlarmSoundButton, dispStr);
        mAlarmProgramButton->setOn(false);
        mAlarmSoundButton->setOn(true);
        QToolTip::add(mAlarmProgramButton, i18n("No program set"));
    } else {
        mAlarmProgramButton->setOn(oldState);
        mAlarmSoundButton->setOn(!oldState);
        
        
    }

    if (mAlarmProgramButton->isOn())
        ((QWidget*)parent())->topLevelWidget()->setCaption(i18n("Proc.Al.: ") + getFittingPath(mAlarmProgram) );
    if ( mAlarmSoundButton->isOn())
        ((QWidget*)parent())->topLevelWidget()->setCaption(i18n("Audio Al.: ") + getFittingPath(mAlarmSound) );

}

void KOEditorGeneral::pickAlarmProgram()
{
    bool oldState = mAlarmProgramButton->isOn();
      
    QString fileName(KFileDialog::getOpenFileName(mAlarmProgram,i18n("Procedure Alarm: ")  , 0));
    if (!fileName.isEmpty()) {
        mAlarmProgram = fileName;
        QToolTip::remove(mAlarmProgramButton);
        QString dispStr = i18n("Running '%1'").arg(fileName);
        QToolTip::add(mAlarmProgramButton, dispStr);
        mAlarmSoundButton->setOn(false);
        mAlarmProgramButton->setOn(true);
        QToolTip::add(mAlarmSoundButton, i18n("No sound set"));
    }  else {
        mAlarmProgramButton->setOn(!oldState);
        mAlarmSoundButton->setOn(oldState);
    }
 
    if (mAlarmProgramButton->isOn())
        ((QWidget*)parent())->topLevelWidget()->setCaption(i18n("Proc.Al.: ") + getFittingPath(mAlarmProgram) );
    if ( mAlarmSoundButton->isOn())
        ((QWidget*)parent())->topLevelWidget()->setCaption(i18n("Audio Al.: ") + getFittingPath(mAlarmSound) );

}


QString KOEditorGeneral::getFittingPath( const QString & s ) 
{
    int maxlen = 50;
    if ( QApplication::desktop()->width() < 640 ) {
        if ( QApplication::desktop()->width() < 320 )
            maxlen = 22;
        else
            maxlen = 35;
    }
    if ( s.length() > maxlen ) {
        return "..."+s.right(maxlen -3);
    }
    return s;
}

void KOEditorGeneral::enableAlarmEdit(bool enable)
{
    if (  enable ) {
        if (!mAlarmProgramButton->isOn() && !mAlarmSoundButton->isOn()) {
            mAlarmSoundButton->setOn( true );
            if ( mAlarmSound.isEmpty() )
                mAlarmSound = KOPrefs::instance()->mDefaultAlarmFile;
            else {
                if ( ! QFile::exists( mAlarmSound ) )
                    mAlarmSound = KOPrefs::instance()->mDefaultAlarmFile;
            }
        }
    }
    ((QWidget*)parent())->topLevelWidget()->setCaption(mAlarmMessage);
    mAlarmTimeEdit->setEnabled(enable);
    mAlarmSoundButton->setEnabled(enable);
    mAlarmProgramButton->setEnabled(enable);
    mAlarmIncrCombo->setEnabled(enable);
}

void KOEditorGeneral::disableAlarmEdit(bool disable)
{
    enableAlarmEdit( !disable );
}

void KOEditorGeneral::enableAlarm( bool enable )
{
    enableAlarmEdit( enable );
}

void KOEditorGeneral::alarmDisable(bool disable)
{
    if (!disable) {
        //mAlarmBell->setEnabled(true);
        mAlarmButton->setEnabled(true);
    } else {
        //mAlarmBell->setEnabled(false);
        mAlarmButton->setEnabled(false);
        mAlarmButton->setChecked(false);
        mAlarmTimeEdit->setEnabled(false);
        mAlarmSoundButton->setEnabled(false);
        mAlarmProgramButton->setEnabled(false);
        mAlarmIncrCombo->setEnabled(false);
    }
}

void KOEditorGeneral::setCategories(const QString &str)
{
    QString tt = str;
    QToolTip::add( mCategoriesLabel, i18n("<b>Click here to edit categories: </b>") +"<em>"+tt.replace( QRegExp(","),", ")+"</em>");
    mCategoriesLabel->setText(str);
}

void KOEditorGeneral::setDefaults(bool allDay)
{
#if 0
    mOwnerLabel->setText(i18n("Owner: ") + KOPrefs::instance()->fullName());
#endif

    mAlarmMessage = i18n("Edit new item");
    enableAlarmEdit( !allDay );

    // TODO: Implement a KPrefsComboItem to solve this in a clean way.
    int alarmTime;
    int a[] = { 1,5,10,15,30,60,180, 1440 };
    int index = KOPrefs::instance()->mAlarmTime;
    if (index < 0 || index > 7) {
        alarmTime = 15;
    } else {
        alarmTime = a[index];
    }
    mAlarmButton ->setChecked( false );
    mAlarmTimeEdit->setValue(alarmTime);
    mAlarmIncrCombo->setCurrentItem(0);
    enableAlarmEdit( false );
    //alarmDisable (false);
    mSecrecyCombo->setCurrentItem(Incidence::SecrecyPublic);
    mCancelBox->setChecked( false ); 
    mSummaryEdit->setEditText("");
    mLocationEdit->setEditText("");
    mDescriptionEdit->setText("");
    mAlarmSound = KOPrefs::instance()->mDefaultAlarmFile;
    setCategories("");
    fillCalCombo();
}
void KOEditorGeneral::setSecrecy( int num )
{
    mSecrecyCombo->setCurrentItem(num);
}
void KOEditorGeneral::fillCalCombo( int setToID )
{
    mCalendarBox->clear();
    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    int std = 0;
    int count = 0;
    while ( kkf ) {
        if ( !kkf->mErrorOnLoad &&! kkf->isReadOnly ) {
            if ( setToID ) {
                if ( kkf->mCalNumber == setToID ) 
                    std = count;
            } else {
                if ( kkf->isStandard ) {
                    std = count;
                }
            }
            ++count;
            mCalendarBox->insertItem( kkf->mName );
        }
        kkf = KOPrefs::instance()->mCalendars.next();
    }
    mCalendarBox->setCurrentItem( std );
    if ( KOPrefs::instance()->mCalendars.count() == 1 )
        mCalendarBox->hide();
    else
        mCalendarBox->show();

}
int KOEditorGeneral::getCalendarID()
{
    return KOPrefs::instance()->getCalendarID( mCalendarBox->currentText() );
}

void KOEditorGeneral::readIncidence(Incidence *event)
{
    fillCalCombo( event->calID() );
    mAlarmMessage = event->summary(); 
    if ( ! event->location().isEmpty() )
        mAlarmMessage += " ("+event->location()+")";
    mAlarmIncrCombo->setCurrentItem(0);
    mSummaryEdit->setEditText(event->summary());
    mLocationEdit->setEditText(event->location());
    mDescriptionEdit->setText(event->description());

#if 0
    // organizer information
    mOwnerLabel->setText(i18n("Owner: ") + event->organizer());
#endif
  
    enableAlarmEdit( event->isAlarmEnabled() );
    //qDebug("KOEditorGeneral::readIncidence(Incidence *event) ");
    if(!event->isAlarmEnabled()) {
        // TODO: Implement a KPrefsComboItem to solve this in a clean way.
        int alarmTime;
        int a[] = { 1,5,10,15,30,60,180, 1440 };
        int index = KOPrefs::instance()->mAlarmTime;
        if (index < 0 || index > 7) {
            alarmTime = 15;
        } else {
            alarmTime = a[index];
        }
        mAlarmTimeEdit->setValue(alarmTime);
    }
    mAlarmButton->setChecked( event->isAlarmEnabled() );
    mSecrecyCombo->setCurrentItem(event->secrecy());
    mCancelBox->setChecked( event->cancelled() ); 
    mAlarmProgramButton->setOn(false);
    mAlarmSoundButton->setOn(false);

    // set up alarm stuff
    QPtrList<Alarm> alarms = event->alarms();
    Alarm* alarm;
    mAlarmIncrCombo->setCurrentItem(0);
    for ( alarm = alarms.first(); alarm; alarm = alarms.next() ) {
        int offset;
        if ( alarm->hasTime() ) {
            QDateTime t = alarm->time();
            offset = event->dtStart().secsTo( t );
        } else {
            offset = alarm->startOffset().asSeconds();
        }
        if ( offset != 0 ) {
            offset = offset / -60; // make minutes
            if (offset % 60 == 0) { // divides evenly into hours?
                offset = offset / 60;
                mAlarmIncrCombo->setCurrentItem(1);
                if (offset % 24 == 0) { // divides evenly into days?
                    offset = offset / 24;
                    mAlarmIncrCombo->setCurrentItem(2);
                }
            }
        }
        mAlarmTimeEdit->setValue( offset );
        if (alarm->type() == Alarm::Procedure) {
      
            mAlarmProgram = alarm->programFile();
            mAlarmProgramButton->setOn(true);
            QString dispStr = i18n("Running '%1'").arg(mAlarmProgram);
            QToolTip::add(mAlarmProgramButton, dispStr);
        }
        else if (alarm->type() == Alarm::Audio) {
            mAlarmSound = alarm->audioFile(); 
            if ( ! QFile::exists( mAlarmSound ) )
                mAlarmSound = KOPrefs::instance()->mDefaultAlarmFile;
            mAlarmSoundButton->setOn(true);
            QString dispStr = i18n("Playing '%1'").arg(mAlarmSound);
            QToolTip::add(mAlarmSoundButton, dispStr);
        }
        mAlarmButton->setChecked(alarm->enabled());
        enableAlarmEdit( alarm->enabled() );
        //qDebug("nableAlarmEdit( alarm->enabled() )********* ");
        // TODO: Deal with multiple alarms
        break; // For now, stop after the first alarm
    }

    setCategories(event->categoriesStr());
}

void KOEditorGeneral::writeIncidence(Incidence *event)
{
    //  kdDebug() << "KOEditorGeneral::writeEvent()" << endl;
    mLocationEdit->save(KOLocationBox::LOCATION);
    event->setSummary(mSummaryEdit->currentText());
    event->setLocation(mLocationEdit->currentText());
    event->setDescription(mDescriptionEdit->text());
    event->setCategories(mCategoriesLabel->text());
    event->setSecrecy(mSecrecyCombo->currentItem());
    event->setCancelled(mCancelBox->isChecked() );; 
    // alarm stuff
    if (mAlarmButton->isChecked()) {
        if (event->alarms().count() == 0) 
            event->newAlarm();
        QPtrList<Alarm> alarms = event->alarms();
        Alarm *alarm;
        for (alarm = alarms.first(); alarm; alarm = alarms.next() ) {
            alarm->setEnabled(true);
            int j = mAlarmTimeEdit->value()* -60;
            if (mAlarmIncrCombo->currentItem() == 1)
                j = j * 60;
            else if (mAlarmIncrCombo->currentItem() == 2)
                j = j * (60 * 24);
            alarm->setStartOffset( j );

            if (!mAlarmProgram.isEmpty() && mAlarmProgramButton->isOn()) {
                alarm->setProcedureAlarm(mAlarmProgram);
            }
            else if (!mAlarmSound.isEmpty() && mAlarmSoundButton->isOn())
                alarm->setAudioAlarm(mAlarmSound);
            else
                alarm->setType(Alarm::Invalid);
            //alarm->setAudioAlarm("default");
            // TODO: Deal with multiple alarms
            break; // For now, stop after the first alarm
        }
    } else {
        Alarm* alarm = event->alarms().first();
        if ( alarm ) {
            alarm->setEnabled(false);
            alarm->setType(Alarm::Invalid);
        }
    }
    int id  = getCalendarID();
    event->setCalID( id );
    event->setAlarmEnabled( KOPrefs::instance()->getCalendar( id )->isAlarmEnabled );
}
