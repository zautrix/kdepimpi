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

#include <qtooltip.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qvgroupbox.h>
#include <qwidgetstack.h>
#include <qdatetime.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>

#include <libkcal/event.h>

#include <libkdepim/kdateedit.h>

#include "koprefs.h"

#include "koeditorgeneralevent.h"
#include "kolocationbox.h"

KOEditorGeneralEvent::KOEditorGeneralEvent(QObject* parent,
                                           const char* name) :
  KOEditorGeneral( parent, name)
{
    mTemplate = false;
  connect(this,SIGNAL(dateTimesChanged(QDateTime,QDateTime)),
          SLOT(setDuration()));
  connect(this,SIGNAL(dateTimesChanged(QDateTime,QDateTime)),
          SLOT(emitDateTimeStr()));
}

KOEditorGeneralEvent::~KOEditorGeneralEvent()
{
}

void KOEditorGeneralEvent::finishSetup()
{
 
  //disabled
//   QWidget::setTabOrder( mSummaryEdit, mLocationEdit );
//   QWidget::setTabOrder( mLocationEdit, mStartDateEdit );
//   QWidget::setTabOrder( mStartDateEdit, mStartTimeEdit );
//   QWidget::setTabOrder( mStartTimeEdit, mEndDateEdit );
//   QWidget::setTabOrder( mEndDateEdit, mEndTimeEdit );
//   QWidget::setTabOrder( mEndTimeEdit, mNoTimeButton );
//   QWidget::setTabOrder( mNoTimeButton, mAlarmButton );
//   QWidget::setTabOrder( mAlarmButton, mAlarmTimeEdit );
//   QWidget::setTabOrder( mFreeTimeCombo, mCategoriesButton );
//   QWidget::setTabOrder( mCategoriesButton, mSecrecyCombo );
//   QWidget::setTabOrder( mSecrecyCombo, mDescriptionEdit );
    
 

  mSummaryEdit->load(KOLocationBox::SUMMARYEVENT);
  mSummaryEdit->setFocus();
}

void KOEditorGeneralEvent::initTime(QWidget *parent,QBoxLayout *topLayout)
{
  QBoxLayout *timeLayout = new QVBoxLayout(topLayout);

  QGroupBox *timeGroupBox = new QGroupBox(1,QGroupBox::Horizontal,
                                          i18n("Date && Time"),parent);
  timeLayout->addWidget(timeGroupBox);

  timeGroupBox->layout()->setSpacing( KDialog::spacingHintSmall() ); 
  timeGroupBox->layout()->setMargin( KDialog::marginHint() ); 
  QFrame *timeBoxFrame = new QFrame(timeGroupBox);

  QGridLayout *layoutTimeBox = new QGridLayout(timeBoxFrame,2,3);
  layoutTimeBox->setSpacing(topLayout->spacing());

  mStartDateLabel = new QLabel(i18n("Start:"),timeBoxFrame);
  layoutTimeBox->addWidget(mStartDateLabel,0,0);

  mStartDateEdit = new KDateEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mStartDateEdit,0,1);

  mStartTimeEdit = new KOTimeEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mStartTimeEdit,0,2);


  mEndDateLabel = new QLabel(i18n("End:"),timeBoxFrame);
  layoutTimeBox->addWidget(mEndDateLabel,1,0);

  mEndDateEdit = new KDateEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mEndDateEdit,1,1);

  mEndTimeEdit = new KOTimeEdit(timeBoxFrame);
  layoutTimeBox->addWidget(mEndTimeEdit,1,2);
  QWidget* duration = new QWidget( timeBoxFrame );
  QHBoxLayout *flagsBox = new QHBoxLayout( duration );
  mNoTimeButton = new QCheckBox(i18n("All day event"),duration);
  flagsBox->addWidget(mNoTimeButton);
  connect(mNoTimeButton, SIGNAL(toggled(bool)),SLOT(dontAssociateTime(bool)));
  mDurationLabel = new QLabel( duration );
  //  if ( KOPrefs::instance()->mCompactDialogs ) {
  //layoutTimeBox->addMultiCellWidget( mDurationLabel, 3, 3, 0, 3 );
  //} else {
    flagsBox->addWidget( mDurationLabel );
    //}
    flagsBox->setStretchFactor(mDurationLabel, 10 );
    mDurationLabel->setAlignment( AlignRight | AlignVCenter);
  layoutTimeBox->addMultiCellWidget( duration, 2, 2, 0, 3 );

  // time widgets are checked if they contain a valid time
  connect(mStartTimeEdit, SIGNAL(timeChanged(QTime)),
	  this, SLOT(startTimeChanged(QTime)));
  connect(mEndTimeEdit, SIGNAL(timeChanged(QTime)),
	  this, SLOT(endTimeChanged(QTime)));

  // date widgets are checked if they contain a valid date
  connect(mStartDateEdit, SIGNAL(dateChanged(QDate)),
	  this, SLOT(startDateChanged(QDate)));
  connect(mEndDateEdit, SIGNAL(dateChanged(QDate)),
	  this, SLOT(endDateChanged(QDate)));
  connect(mStartDateEdit,SIGNAL(setTimeTo(QTime)),this,SLOT(sTimeChanged(QTime)));
  connect(mEndDateEdit,SIGNAL(setTimeTo(QTime)),this,SLOT(eTimeChanged(QTime)));
}

void KOEditorGeneralEvent::initClass(QWidget *parent,QBoxLayout *topLayout)
{
  QBoxLayout *classLayout = new QHBoxLayout(topLayout);

  QLabel *freeTimeLabel = new QLabel(i18n("Show time as:"),parent);
  classLayout->addWidget(freeTimeLabel);

  mFreeTimeCombo = new QComboBox(false, parent);
  mFreeTimeCombo->insertItem(i18n("Busy"));
  mFreeTimeCombo->insertItem(i18n("Free"));
  classLayout->addWidget(mFreeTimeCombo);
}

void KOEditorGeneralEvent::timeStuffDisable(bool disable)
{
  mStartTimeEdit->setEnabled( !disable );
  mEndTimeEdit->setEnabled( !disable );

  setDuration();
  emitDateTimeStr();
}

void KOEditorGeneralEvent::dontAssociateTime(bool noTime)
{
  timeStuffDisable(noTime);
  //if(alarmButton->isChecked()) alarmStuffDisable(noTime);
  allDayChanged(noTime);
}

void KOEditorGeneralEvent::setDateTimes(QDateTime start, QDateTime end)
{
//  kdDebug() << "KOEditorGeneralEvent::setDateTimes(): Start DateTime: " << start.toString() << endl;
    if ( !mTemplate )
        mStartDateEdit->setDate(start.date());
  // KTimeEdit seems to emit some signals when setTime() is called.
  mStartTimeEdit->blockSignals( true );
  mStartTimeEdit->setTime(start.time());
  mStartTimeEdit->blockSignals( false );
    if ( !mTemplate )
        mEndDateEdit->setDate(end.date());
  mEndTimeEdit->setTime(end.time());

  mCurrStartDateTime = start;
  mCurrEndDateTime = end;

  setDuration();
  emitDateTimeStr();
}

void KOEditorGeneralEvent::sTimeChanged(QTime ti)
{
    mStartTimeEdit->setTime( ti );
    startTimeChanged ( ti);
}
void KOEditorGeneralEvent::eTimeChanged(QTime ti )
{
    mEndTimeEdit->setTime( ti );
    endTimeChanged( ti );
}
void KOEditorGeneralEvent::startTimeChanged(QTime newtime)
{

  int secsep = mCurrStartDateTime.secsTo(mCurrEndDateTime);

  mCurrStartDateTime = QDateTime( mStartDateEdit->date(), QTime(newtime.hour(),newtime.minute(),0) );
  // adjust end time so that the event has the same duration as before.
  if( secsep > 0 )
      mCurrEndDateTime = mCurrStartDateTime.addSecs(secsep);
  mEndTimeEdit->setTime(mCurrEndDateTime.time());
  mEndDateEdit->setDate(mCurrEndDateTime.date());

  emit dateTimesChanged(mCurrStartDateTime,mCurrEndDateTime);
}

void KOEditorGeneralEvent::endTimeChanged(QTime newtime)
{

  mCurrEndDateTime =  QDateTime ( mEndDateEdit->date(), QTime(newtime.hour(),newtime.minute(),0));

  emit dateTimesChanged(mCurrStartDateTime,mCurrEndDateTime);
}

void KOEditorGeneralEvent::startDateChanged(QDate newdate)
{
  int daysep = mCurrStartDateTime.daysTo(mCurrEndDateTime);

  mCurrStartDateTime.setDate(newdate);

  // adjust end date so that the event has the same duration as before
  mCurrEndDateTime.setDate(mCurrStartDateTime.date().addDays(daysep));
  mEndDateEdit->setDate(mCurrEndDateTime.date());
  qDebug("KOEditorGeneralEvent::startDateChanged ");
  emit dateTimesChanged(mCurrStartDateTime,mCurrEndDateTime);
}

void KOEditorGeneralEvent::endDateChanged(QDate newdate)
{
  QDateTime newdt(newdate, mCurrEndDateTime.time());

  if(newdt < mCurrStartDateTime) {
    // oops, we can't let that happen.
    newdt = mCurrStartDateTime;
    mEndDateEdit->setDate(newdt.date());
    mEndTimeEdit->setTime(newdt.time());
  }
  mCurrEndDateTime = newdt;

  emit dateTimesChanged(mCurrStartDateTime,mCurrEndDateTime);
}

void KOEditorGeneralEvent::setDefaults(QDateTime from,QDateTime to,bool allDay)
{ 
  mSummaryEdit->load(KOLocationBox::SUMMARYEVENT);
  mLocationEdit->load(KOLocationBox::LOCATION);
  KOEditorGeneral::setDefaults(allDay);

  mNoTimeButton->setChecked(allDay);
  timeStuffDisable(allDay);
  mFreeTimeCombo->setCurrentItem( 0 );
  setDateTimes(from,to); 
}

void KOEditorGeneralEvent::readEvent( Event *event, bool tmpl )
{
  QString tmpStr;

  mTemplate = tmpl;
  // the rest is for the events only
  mNoTimeButton->setChecked(event->doesFloat());
  timeStuffDisable(event->doesFloat());
  
  setDateTimes(event->dtStart(),event->dtEnd());
  
  mTemplate = false;
  switch( event->transparency() ) {
  case Event::Transparent:
    mFreeTimeCombo->setCurrentItem(1);
    break;
  case Event::Opaque:
    mFreeTimeCombo->setCurrentItem(0);
    break;
  }

  mSummaryEdit->load(KOLocationBox::SUMMARYEVENT);
  mLocationEdit->load(KOLocationBox::LOCATION);
  readIncidence(event);
}

void KOEditorGeneralEvent::writeEvent(Event *event)
{
//  kdDebug() << "KOEditorGeneralEvent::writeEvent()" << endl;

  writeIncidence(event);

  QDate tmpDate;
  QTime tmpTime;
  QDateTime tmpDT;

  // temp. until something better happens.
  QString tmpStr;

  if (mNoTimeButton->isChecked()) {
    event->setFloats(true);
    // need to change this.
    tmpDate = mStartDateEdit->date();
    tmpTime.setHMS(0,0,0);
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    event->setDtStart(tmpDT);

    tmpDate = mEndDateEdit->date();
    tmpTime.setHMS(0,0,0);
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    event->setDtEnd(tmpDT);
  } else {
    event->setFloats(false);

    // set date/time end
    tmpDate = mEndDateEdit->date();
    tmpTime = mEndTimeEdit->getTime();
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    event->setDtEnd(tmpDT);

    // set date/time start
    tmpDate = mStartDateEdit->date();
    tmpTime = mStartTimeEdit->getTime();
    tmpDT.setDate(tmpDate);
    tmpDT.setTime(tmpTime);
    event->setDtStart(tmpDT);
  } // check for float
  mSummaryEdit->save(KOLocationBox::SUMMARYEVENT);

  event->setTransparency(mFreeTimeCombo->currentItem() > 0
			 ? KCal::Event::Transparent
			 : KCal::Event::Opaque);

//  kdDebug() << "KOEditorGeneralEvent::writeEvent() done" << endl;
}

void KOEditorGeneralEvent::setDuration()
{
  QString tmpStr = "", catStr;
  int hourdiff, minutediff;
  // end<date is an accepted temporary state while typing, but don't show
  // any duration if this happens
  if(mCurrEndDateTime >= mCurrStartDateTime) {

    if (mNoTimeButton->isChecked()) {
        int daydiff = mCurrStartDateTime.date().daysTo(mCurrEndDateTime.date()) + 1;
      tmpStr = i18n("Duration: ");
      tmpStr.append(i18n("1 Day","%n Days",daydiff));
    } else {
        int secto =  mCurrStartDateTime.secsTo( mCurrEndDateTime  );
        hourdiff = secto / 3600;
        minutediff = (secto/60 ) % 60;
      if (hourdiff || minutediff){
        tmpStr = i18n("Duration: ");
        if (hourdiff){
          catStr = i18n("1 h","%n h",hourdiff);
          tmpStr.append(catStr);
        }
        if (hourdiff && minutediff){
          tmpStr += i18n(", ");
        }
        if (minutediff){
          catStr = i18n("1 min","%n min",minutediff);
          tmpStr += catStr;
        }
      } else tmpStr = "";
    }
  }
  mDurationLabel->setText(tmpStr);
}

void KOEditorGeneralEvent::emitDateTimeStr()
{
  KLocale *l = KGlobal::locale();
  QString startString = "";
  if ( mCurrStartDateTime.date() < mCurrEndDateTime.date()  ) {
      if ( mNoTimeButton->isChecked() ) {
          startString = i18n("From: ") + l->formatDate(mCurrStartDateTime.date() );
          startString += "\n"+i18n("To: ")+ l->formatDate(mCurrEndDateTime.date());
          
      } else {
          startString = i18n("From: ") +l->formatDateTime(mCurrStartDateTime, false);
          startString += "\n"+i18n("To: ")+l->formatDateTime(mCurrEndDateTime, false);
      }
  } else {
      if ( mNoTimeButton->isChecked() ) {
          startString = i18n("On: ") + l->formatDate(mCurrStartDateTime.date() );
      } else {
          startString = i18n("From: ") + KGlobal::locale()->formatTime(mCurrStartDateTime.time())+
              "-"+KGlobal::locale()->formatTime(mCurrEndDateTime.time());
          startString += i18n(" on ")+KGlobal::locale()->formatDate( mCurrStartDateTime.date(), true);
      }
  }
  startString +="\n"+mDurationLabel->text();
  emit dateTimeStrChanged(startString);
}

bool KOEditorGeneralEvent::validateInput()
{

  if (!mStartDateEdit->inputIsValid()) {
    KMessageBox::sorry( 0,
        i18n("Please specify a valid start date,\nfor example '%1'.")
                        .arg( KGlobal::locale()->formatDate( QDate::currentDate(), true ) ) );
    return false;
  }

  if (!mEndDateEdit->inputIsValid()) {
    KMessageBox::sorry( 0,
        i18n("Please specify a valid end date,\nfor example '%1'.")
        .arg( KGlobal::locale()->formatDate( QDate::currentDate(), true ) ) );
    return false;
  }

  QDateTime startDt,endDt;
  startDt.setDate(mStartDateEdit->date());
  endDt.setDate(mEndDateEdit->date());
  if (!mNoTimeButton->isChecked()) {
    startDt.setTime(mStartTimeEdit->getTime());
    endDt.setTime(mEndTimeEdit->getTime());
  }

  if (startDt > endDt) {
    KMessageBox::sorry(0,i18n("The event ends before it starts.\n"
                                 "Please correct dates and times."));
    return false;
  }

  return KOEditorGeneral::validateInput();
}
