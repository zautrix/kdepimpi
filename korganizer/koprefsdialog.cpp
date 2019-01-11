/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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

#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qfont.h>
#include <qslider.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qdatetime.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qstrlist.h>
#include <qapplication.h>

#include <kcolorbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kfontdialog.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kcolordialog.h>
#include <kiconloader.h>
#include <kemailsettings.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>

#include <kurlrequester.h>
#include <klineedit.h>

#if defined(USE_SOLARIS)
#include <sys/param.h>

#define ZONEINFODIR    "/usr/share/lib/zoneinfo"
#define INITFILE       "/etc/default/init"
#endif

#include "koprefs.h"

#include "koprefsdialog.h"
#include "kpimglobalprefs.h"


KOPrefsDialog::KOPrefsDialog(QWidget *parent, char *name, bool modal) :
  KPrefsDialog(KOPrefs::instance(),parent,name,true)
{

    setFont( KGlobalSettings::generalMaxFont() );
  setCaption( i18n("Preferences - some settings need a restart (nr)"));
  mCategoryDict.setAutoDelete(true);

  KGlobal::locale()->insertCatalogue("timezones");
  mSpacingHint = spacingHintSmall();
   mMarginHint = marginHintSmall();
#ifndef DESKTOP_VERSION
  if ( QApplication::desktop()->height() == 480  )
      hideButtons();
#endif
 
  setupMainTab();
  // setupLocaleTab();
  //setupTimeZoneTab();
  setupTimeTab();
  //setupLocaleDateTab();
  setupFontsTab();
  setupColorsTab();
  setupViewsTab();
  //setupSyncTab();
  //setupSyncAlgTab();
  //setupPrinterTab();
  //setupGroupSchedulingTab();
  //setupGroupAutomationTab();
  

}


KOPrefsDialog::~KOPrefsDialog()
{
}
void KOPrefsDialog::setupGlobalTab() 
{
   

} 
void KOPrefsDialog::setupLocaleDateTab()
{
#if 0
QFrame *topFrame = addPage(i18n("Date Format"),0,0);
   QGridLayout *topLayout = new QGridLayout(topFrame,3,2);
   topLayout->setSpacing(mSpacingHint);
   topLayout->setMargin(mMarginHint);  
   int iii = 0;

   
   KPrefsDialogWidRadios *syncPrefsGroup =
       addWidRadios(i18n("Date Format:"),&(KOPrefs::instance()->mPreferredDate),topFrame);
   QString format;
   if ( QApplication::desktop()->width() < 480 )
       format = "(%d.%m.%Y)";
   else 
       format = "(%d.%m.%Y|%A %d %B %Y)";
   syncPrefsGroup->addRadio(i18n("24.03.2004 "+format));
   if ( QApplication::desktop()->width() < 480 )
       format = "(%m.%d.%Y)";
   else 
       format = "(%m.%d.%Y|%A %B %d %Y)";
   syncPrefsGroup->addRadio(i18n("03.24.2004 "+format));
   if ( QApplication::desktop()->width() < 480 )
       format = "(%Y-%m-%d)";
   else 
       format = "(%Y-%m-%d|%A %Y %B %d)";
   syncPrefsGroup->addRadio(i18n("2004-03-24 "+format));
   syncPrefsGroup->addRadio(i18n("User defined"));
   topLayout->addMultiCellWidget( syncPrefsGroup->groupBox(),iii,iii,0,1);
   ++iii;   
   ++iii;
   QLabel * lab;
   mUserDateFormatLong = new QLineEdit(topFrame);
   lab = new QLabel(mUserDateFormatLong, i18n("User long date:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mUserDateFormatLong,iii,1);
   ++iii;
   mUserDateFormatShort = new QLineEdit(topFrame);
   lab = new QLabel(mUserDateFormatShort, i18n("User short date:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mUserDateFormatShort,iii,1);
   ++iii;
   lab = new QLabel( i18n("Monday 19 April 2004: %A %d %B %Y"), topFrame);
   topLayout->addMultiCellWidget(lab ,iii,iii,0,1);
   ++iii;
   lab = new QLabel( i18n("Mon 19.04.04: %a %d.%m.%y"), topFrame);
   topLayout->addMultiCellWidget(lab ,iii,iii,0,1);
   ++iii;  
   lab = new QLabel( i18n("Mon, 19.Apr.04: %a, %d.%b.%y"), topFrame);
   topLayout->addMultiCellWidget(lab ,iii,iii,0,1);
   ++iii;
#endif

}

void  KOPrefsDialog::setupLocaleTab()
{
#if 0
 QFrame *topFrame = addPage(i18n("Locale"),0,0);
   QGridLayout *topLayout = new QGridLayout(topFrame,4,2);
   topLayout->setSpacing(mSpacingHint);
   topLayout->setMargin(mMarginHint);  
   int iii = 0;
 KPrefsDialogWidRadios *syncPrefsGroup =
      addWidRadios(i18n("Language:(needs restart)"),&(KOPrefs::instance()->mPreferredLanguage),topFrame);
   syncPrefsGroup->addRadio(i18n("English"));
   syncPrefsGroup->addRadio(i18n("German")); 
   syncPrefsGroup->addRadio(i18n("French")); 
   syncPrefsGroup->addRadio(i18n("User defined (usertranslation.txt)")); 
   if (  QApplication::desktop()->width() <  300   ) 
       ;//   syncPrefsGroup->groupBox()-> setOrientation (Qt::Vertical);
   topLayout->addMultiCellWidget( syncPrefsGroup->groupBox(),iii,iii,0,1);
   ++iii;   
 
   syncPrefsGroup =
       addWidRadios(i18n("Time Format(nr):"),&(KOPrefs::instance()->mPreferredTime),topFrame);
   if (  QApplication::desktop()->width() >  300   )
       syncPrefsGroup->groupBox()-> setOrientation (Qt::Vertical);
   syncPrefsGroup->addRadio(i18n("24:00"));
   syncPrefsGroup->addRadio(i18n("12:00am"));
   syncPrefsGroup->groupBox()-> setOrientation (Qt::Vertical);
   topLayout->addMultiCellWidget( syncPrefsGroup->groupBox(),iii,iii,0,1);
   ++iii;   
   KPrefsDialogWidBool *sb;
  if ( QApplication::desktop()->width() <  300 ) {
       sb =
           addWidBool(i18n("Week starts on Sunday"),
                      &(KOPrefs::instance()->mWeekStartsOnSunday),topFrame);
       topLayout->addMultiCellWidget(sb->checkBox(), iii,iii,0,1);
       ++iii;   
       sb =
           addWidBool(i18n("Use short date in (WN/E) view"),
                      &(KOPrefs::instance()->mShortDateInViewer),topFrame);
       topLayout->addMultiCellWidget(sb->checkBox(), iii,iii,0,1);
   }
   else {
       QWidget * hb = new QWidget( topFrame );
       QHBoxLayout  *hbLayout = new QHBoxLayout(hb);
       sb =
           addWidBool(i18n("Week starts on Sunday"),
                      &(KOPrefs::instance()->mWeekStartsOnSunday),hb);
       hbLayout->addWidget(sb->checkBox()  );
       sb =
           addWidBool(i18n("Use short date in (WN/E) view"),
                      &(KOPrefs::instance()->mShortDateInViewer),hb);
       hbLayout->addWidget(sb->checkBox()  );
       topLayout->addMultiCellWidget(hb, iii,iii,0,1);

   }
  //   KPrefsDialogWidBool *sb;  //#ifndef DESKTOP_VERSION
#if 0
  ++iii;
  sb =
           addWidBool(i18n("Quick load/save (w/o Unicode)"), 
                      &(KOPrefs::instance()->mUseQuicksave),topFrame);
       topLayout->addMultiCellWidget(sb->checkBox(), iii,iii,0,1);
#endif
#endif
}

void KOPrefsDialog::setupMainTab()
{
    QFrame *topFrame = addPage(i18n("General"),0,0);
    // DesktopIcon("identity",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,5,2);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);

 //  KPrefsDialogWidBool *emailControlCenter =
//       addWidBool(i18n("&Use email settings from Control Center"),
//                  &(KOPrefs::instance()->mEmailControlCenter),topFrame);
//   topLayout->addMultiCellWidget(emailControlCenter->checkBox(),0,0,0,1);
  // connect(emailControlCenter->checkBox(),SIGNAL(toggled(bool)),
  //      SLOT(toggleEmailSettings(bool)));

  mNameEdit = new QLineEdit(topFrame);
  mNameLabel = new QLabel(mNameEdit, i18n("Full &name:"), topFrame);
  topLayout->addWidget(mNameLabel,0,0);
  topLayout->addWidget(mNameEdit,0,1);

  mEmailEdit = new QLineEdit(topFrame);
  mEmailLabel = new QLabel(mEmailEdit, i18n("E&mail address:"),topFrame);
  topLayout->addWidget(mEmailLabel,1,0);
  topLayout->addWidget(mEmailEdit,1,1);
  KPrefsDialogWidBool *wb;
  

  
  KPrefsDialogWidBool *widbool = addWidBool(i18n("Full menu bar(nr)"),
             &(KOPrefs::instance()->mShowFullMenu),topFrame);
  topLayout->addMultiCellWidget(  widbool->checkBox(), 2,2,0,1); 
  
  
  widbool = addWidBool(i18n("Mini icons in toolbar(nr)"),
             &(KOPrefs::instance()->mToolBarMiniIcons),topFrame);
  topLayout->addMultiCellWidget(  widbool->checkBox(), 3,3,0,1); 


 KPrefsDialogWidBool *verticalScreen =
      addWidBool(i18n("Vertical screen layout(Needs restart)"),
                 &(KOPrefs::instance()->mVerticalScreen),topFrame);
  //topLayout->addWidget(verticalScreen->checkBox(),ii++,0);
  topLayout->addMultiCellWidget(verticalScreen->checkBox(),4,4,0,1);

  
  int iii = 5;
  widbool = addWidBool(i18n("Block popup until mouse button release"),
             &(KOPrefs::instance()->mBlockPopupMenu),topFrame);
  topLayout->addMultiCellWidget(  widbool->checkBox(), iii,iii,0,1); 
  ++iii;
  if (  QApplication::desktop()->height() <= 240 ) {
      topFrame = addPage(i18n("General") +" 2",0,0);
      topLayout = new QGridLayout(topFrame,4,2);
      topLayout->setSpacing(2);
      topLayout->setMargin(3);
      iii = 0;
  }
  QHBox *dummy = new QHBox(topFrame);
  new QLabel(i18n("Days in Next-X-Days:"),dummy);
  mNextXDaysSpin = new QSpinBox(2,14,1,dummy);
 
  topLayout->addMultiCellWidget(dummy,iii,iii,0,1);

  ++iii;
 

  // KPrefsDialogWidBool *bcc =
//       addWidBool(i18n("Send copy to owner when mailing events"),
//                  &(KOPrefs::instance()->mBcc),topFrame);
//   topLayout->addMultiCellWidget(bcc->checkBox(),4,4,0,1);


  // QGroupBox *autoSaveGroup = new QGroupBox(1,Horizontal,i18n("Auto-Save"),  topFrame);
  //topLayout->addMultiCellWidget(autoSaveGroup,6,6,0,1);

  // addWidBool(i18n("Enable automatic saving of calendar"),
  //         &(KOPrefs::instance()->mAutoSave),autoSaveGroup);

  QHBox *intervalBox = new QHBox(topFrame);
  // intervalBox->setSpacing(mSpacingHint);
  topLayout->addMultiCellWidget(intervalBox,iii,iii,0,1);
  ++iii;
  QLabel *autoSaveIntervalLabel = new QLabel(i18n("Auto save delay in minutes:"),intervalBox);
  mAutoSaveIntervalSpin = new QSpinBox(0,500,1,intervalBox);
  autoSaveIntervalLabel->setBuddy(mAutoSaveIntervalSpin); 
  /*
  QHBox * agendasize = new QHBox ( topFrame );
  
  new QLabel (i18n("AllDayAgenda Height:"), agendasize );
  

  mHourSizeSlider = new QSlider(24,47,1,24,Horizontal,agendasize);
  topLayout->addMultiCellWidget(agendasize,7,7,0,1);
  */
  

  KPrefsDialogWidBool *ask =
    addWidBool(i18n("Ask for quit when closing KO/Pi"),
               &(KOPrefs::instance()->mAskForQuit),topFrame);
  topLayout->addMultiCellWidget(ask->checkBox(),iii,iii,0,1);
  ++iii;
 

  /*
  KPrefsDialogWidBool *confirmCheck =
      addWidBool(i18n("Confirm &deletes"),&(KOPrefs::instance()->mConfirm),
                 topFrame);
  topLayout->addMultiCellWidget(confirmCheck->checkBox(),7,7,0,1);


  mEnableGroupScheduling =
      addWidBool(i18n("Enable group scheduling"),
                 &(KOPrefs::instance()->mEnableGroupScheduling),topFrame);
  topLayout->addWidget(mEnableGroupScheduling->checkBox(),8,0);
  connect(mEnableGroupScheduling->checkBox(),SIGNAL(clicked()),
          SLOT(warningGroupScheduling()));

  mEnableProjectView =
      addWidBool(i18n("Enable project view"),
                 &(KOPrefs::instance()->mEnableProjectView),topFrame);
  topLayout->addWidget(mEnableProjectView->checkBox(),9,0);
  connect(mEnableProjectView->checkBox(),SIGNAL(clicked()),
          SLOT(warningProjectView()));

  // Can't be disabled anymore
  mEnableGroupScheduling->checkBox()->hide();

  // Disable setting, because this feature now becomes stable
  mEnableProjectView->checkBox()->hide();

  KPrefsDialogWidRadios *defaultFormatGroup =
      addWidRadios(i18n("Default Calendar Format"),
                   &(KOPrefs::instance()->mDefaultFormat),topFrame);
  defaultFormatGroup->addRadio(i18n("vCalendar"));
  defaultFormatGroup->addRadio(i18n("iCalendar"));

  topLayout->addMultiCellWidget(defaultFormatGroup->groupBox(),10,10,0,1);

  // Default format unconditionally is iCalendar
  defaultFormatGroup->groupBox()->hide();

  KPrefsDialogWidRadios *mailClientGroup =
      addWidRadios(i18n("Mail Client"),&(KOPrefs::instance()->mMailClient),
                   topFrame);
  mailClientGroup->addRadio(i18n("KMail"));
  mailClientGroup->addRadio(i18n("Sendmail"));
  topLayout->addMultiCellWidget(mailClientGroup->groupBox(),11,11,0,1);

  KPrefsDialogWidBool *htmlsave =
      addWidBool(i18n("Export to HTML with every save"),&(KOPrefs::instance()->mHtmlWithSave),
                 topFrame);
  topLayout->addMultiCellWidget(htmlsave->checkBox(),12,12,0,1);

  KPrefsDialogWidRadios *destinationGroup =
      addWidRadios(i18n("New Events/Todos should"),&(KOPrefs::instance()->mDestination),
                   topFrame);
  destinationGroup->addRadio(i18n("be added to the standard resource"));
  destinationGroup->addRadio(i18n("be asked which resource to use"));
  topLayout->addMultiCellWidget(destinationGroup->groupBox(),13,13,0,1);

  topLayout->setRowStretch(14,1);
  */
}


void KOPrefsDialog::setupTimeTab()
{
    QFrame *topFrame = addPage(i18n("Time"),0,0);
    // DesktopIcon("clock",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,4,2);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
 
  QHBox *dummy = new QHBox(topFrame);
  KPrefsDialogWidTime *dayBegins =
      addWidTime(i18n("Day begins at:"),&(KOPrefs::instance()->mDayBegins),
               dummy);
  //topLayout->addWidget(dayBegins->label(),2,0);
  
  //topLayout->addWidget(dayBegins->spinBox(),2,1);
  topLayout->addMultiCellWidget(dummy,0,0,0,1);

  topLayout->addWidget(new QLabel(i18n("Default appointment time:"),
                       topFrame),1,0);
  mStartTimeSpin = new QSpinBox(0,23,1,topFrame);
  mStartTimeSpin->setSuffix(":00");
  topLayout->addWidget(mStartTimeSpin,1,1);

  topLayout->addWidget(new QLabel(i18n("Def. duration of new app.:"),
                       topFrame),2,0);
  mDefaultDurationSpin = new QSpinBox(0,23,1,topFrame);
  mDefaultDurationSpin->setSuffix(":00");
  topLayout->addWidget(mDefaultDurationSpin,2,1);

  QStringList alarmList;
  alarmList << i18n("1 minute") << i18n("5 minutes") << i18n("10 minutes")
            << i18n("15 minutes") << i18n("30 minutes")<< i18n("1 hour")<< i18n("3 hours")  << i18n("24 hours") ;
  topLayout->addWidget(new QLabel(i18n("Default alarm time:"),topFrame),
                       3,0);
  mAlarmTimeCombo = new QComboBox(topFrame);
  mAlarmTimeCombo->insertStringList(alarmList);
  topLayout->addWidget(mAlarmTimeCombo,3,1);
  int iii = 4;
  if (  QApplication::desktop()->height() <= 240 ) {
      topFrame = addPage(i18n("Time") +" 2",0,0);
      topLayout = new QGridLayout(topFrame,1,2);
      topLayout->setSpacing(2);
      topLayout->setMargin(3);
      iii = 0;
  }

  QGroupBox *workingHoursGroup = new QGroupBox(1,Horizontal,
                                               i18n("Working Hours"),
                                               topFrame);
  topLayout->addMultiCellWidget(workingHoursGroup,iii,iii,0,1);
  workingHoursGroup->layout()->setSpacing( 0 );
  workingHoursGroup->layout()->setMargin( 4 );
  QHBox *workStartBox = new QHBox(workingHoursGroup);
  // workStartBox->setMargin( 0 );
  addWidTime(i18n("Daily starting hour:"),
             &(KOPrefs::instance()->mWorkingHoursStart),workStartBox);

  QHBox *workEndBox = new QHBox(workingHoursGroup);
  //workEndBox->setMargin( 0 );
  addWidTime(i18n("Daily ending hour:"),
             &(KOPrefs::instance()->mWorkingHoursEnd),workEndBox);
  QVBox *excludeBox  = new QVBox(workingHoursGroup);
  //excludeBox->setMargin( 0 );
  addWidBool(i18n("Exclude holidays"),
             &(KOPrefs::instance()->mExcludeHolidays),excludeBox);

  addWidBool(i18n("Exclude Saturdays"),
             &(KOPrefs::instance()->mExcludeSaturdays),excludeBox);

//   KPrefsDialogWidBool *marcusBainsShowSeconds = addWidBool(i18n("Show seconds on Marcus Bains line"),
// 	     &(KOPrefs::instance()->mMarcusBainsShowSeconds),
// 	     topFrame);
//   topLayout->addWidget(marcusBainsShowSeconds->checkBox(),5,0);

  // topLayout->setRowStretch(6,1);
}


void KOPrefsDialog::setupViewsTab()
{

    QFrame *topFrame = addPage(i18n("Views"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,6,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);

//   QBoxLayout *dayBeginsLayout = new QHBoxLayout;
//   topLayout->addLayout(dayBeginsLayout,0,0);

//   KPrefsDialogWidTime *dayBegins =
//     addWidTime(i18n("Day begins at:"),&(KOPrefs::instance()->mDayBegins),
//                topFrame);
//   dayBeginsLayout->addWidget(dayBegins->label());
//   dayBeginsLayout->addStretch(1);
//   dayBeginsLayout->addWidget(dayBegins->spinBox());

//   QBoxLayout *nextDaysLayout = new QHBoxLayout;
//   topLayout->addLayout(nextDaysLayout,1,0);
//   nextDaysLayout->addWidget(new QLabel(i18n("Days to show in Next-X-Days view:"),topFrame));
//   mNextXDaysSpin = new QSpinBox(2,14,1,topFrame);
//   nextDaysLayout->addStretch(1);
//   nextDaysLayout->addWidget(mNextXDaysSpin);


  int ii = 0; 
  KPrefsDialogWidBool *dummy =
    addWidBool(i18n("Edit item on doubleclick (if not, show)"),
               &(KOPrefs::instance()->mEditOnDoubleClick),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
  
 


 
 
  // topLayout->addWidget(hourSizeGroup,ii++,0);
  // topLayout->addMultiCellWidget(hourSizeGroup,ii,ii,0,0);
  //topLayout->setRowStretch(11,1);




#if 0

   topFrame = addPage(i18n("ViewChange"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,6,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  ii = 0;

#endif

  dummy =
    addWidBool(i18n("Hold fullscreen on view change"),
               &(KOPrefs::instance()->mViewChangeHoldFullscreen),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  dummy =
    addWidBool(i18n("Hold non-fullscreen on view change"),
               &(KOPrefs::instance()->mViewChangeHoldNonFullscreen),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);



  KPrefsDialogWidBool *fullViewMonth =
      addWidBool(i18n("Next days view uses full window"),
                 &(KOPrefs::instance()->mFullViewMonth),topFrame);
  topLayout->addWidget(fullViewMonth->checkBox(),ii++,0);


 KPrefsDialogWidBool *fullViewTodo =
      addWidBool(i18n("Event list view uses full window"),
                 &(KOPrefs::instance()->mFullViewTodo),topFrame);
  topLayout->addWidget(fullViewTodo->checkBox(),ii++,0);
  dummy =
    addWidBool(i18n("Listview uses monthly timespan"),
               &(KOPrefs::instance()->mListViewMonthTimespan),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
 dummy  =
    addWidBool(i18n("Highlight selection in Time Edit"),
               &(KOPrefs::instance()->mHightlightDateTimeEdit),topFrame);
   topLayout->addWidget( dummy->checkBox(), ii++,0);

  KPrefsDialogWidBool *dailyRecur =
    addWidBool(i18n("Show events that recur daily in date nav."),
               &(KOPrefs::instance()->mDailyRecur),topFrame);
  topLayout->addWidget(dailyRecur->checkBox(),ii++,0);

  KPrefsDialogWidBool *weeklyRecur =
    addWidBool(i18n("Show ev. that recur weekly in date nav."),
               &(KOPrefs::instance()->mWeeklyRecur),topFrame);
  topLayout->addWidget(weeklyRecur->checkBox(),ii++,0);

  weeklyRecur =
    addWidBool(i18n("Show multiday allday ev. in date nav."),
               &(KOPrefs::instance()->mLongAllday),topFrame);
  topLayout->addWidget(weeklyRecur->checkBox(),ii++,0);

#ifdef DESKTOP_VERSION
  KPrefsDialogWidBool *enableToolTips =
      addWidBool(i18n("Enable tooltips displaying summary of ev."),
                 &(KOPrefs::instance()->mEnableToolTips),topFrame);
  topLayout->addWidget(enableToolTips->checkBox(),ii++,0);
#endif
  //  *********************************************************

  topFrame = addPage(i18n("Agenda View"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,5,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  ii = 0;


 dummy =
    addWidBool(i18n("Show time in agenda items"),
               &(KOPrefs::instance()->mShowTimeInAgenda),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  dummy =
    addWidBool(i18n("Highlight current day in agenda"),
               &(KOPrefs::instance()->mHighlightCurrentDay),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  dummy =
    addWidBool(i18n("Use light color for highlight current day"),
               &(KOPrefs::instance()->mUseHighlightLightColor),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);


 KPrefsDialogWidBool *marcusBainsEnabled =
      addWidBool(i18n("Show current time"),
                 &(KOPrefs::instance()->mMarcusBainsEnabled),topFrame);
  topLayout->addWidget(marcusBainsEnabled->checkBox(),ii++,0);
  

 dummy =
    addWidBool(i18n("Set agenda to DayBeginsAt on change"),
               &(KOPrefs::instance()->mSetTimeToDayStartAt),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

 dummy =
    addWidBool(i18n("Set agenda to current time on change"),
               &(KOPrefs::instance()->mCenterOnCurrentTime),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);


 dummy =
    addWidBool(i18n("Allday Agenda view shows todos"),
               &(KOPrefs::instance()->mShowTodoInAgenda),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);



 dummy =
    addWidBool(i18n("Agenda view shows completed todos"),
               &(KOPrefs::instance()->mShowCompletedTodoInAgenda),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);





   topFrame = addPage(i18n("Month View"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,5,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  ii = 0;
  QLabel *lab;
  QHBox *habo = new QHBox( topFrame );
  if ( QApplication::desktop()->width() <= 480 ) {
      lab = new QLabel ( i18n("Show events that recur "), topFrame );
      topLayout->addMultiCellWidget(lab,ii, ii,0,1);
      ii++;
  } else {
      new QLabel ( i18n("Show events that recur "), habo );
  }
  dailyRecur =
    addWidBool(i18n("daily"),
               &(KOPrefs::instance()->mMonthDailyRecur),habo);
  // topLayout->addWidget(dailyRecur->checkBox(),ii++,0);

  weeklyRecur =
    addWidBool(i18n("weekly"),
               &(KOPrefs::instance()->mMonthWeeklyRecur),habo);
  topLayout->addMultiCellWidget(habo,ii, ii,0,1);
  ii++;


  habo = new QHBox( topFrame ); 
  if ( QApplication::desktop()->width() <= 480 ) {
      lab = new QLabel (i18n("Show in every cell ") , topFrame );
      topLayout->addMultiCellWidget(lab,ii, ii,0,1);
      ii++;
      
  } else {
      new QLabel ( i18n("Show in every cell "), habo );
  }
  weeklyRecur = 
    addWidBool(i18n("short month"),
               &(KOPrefs::instance()->mMonthShowShort),habo);
 weeklyRecur =
    addWidBool(i18n("icons"),
               &(KOPrefs::instance()->mMonthShowIcons),habo);
 weeklyRecur =
     addWidBool(i18n("times"),
                &(KOPrefs::instance()->mMonthShowTimes),habo);
  topLayout->addMultiCellWidget(habo,ii, ii,0,1);
  ii++;
#ifdef DESKTOP_VERSION
 KPrefsDialogWidBool *enableMonthScroll =
      addWidBool(i18n("Enable scrollbars in month view cells"),
                 &(KOPrefs::instance()->mEnableMonthScroll),topFrame);
  topLayout->addWidget(enableMonthScroll->checkBox(),ii++,0);
#endif
  dummy =
    addWidBool(i18n("Week view mode uses bigger font"),
               &(KOPrefs::instance()->mMonthViewUsesBigFont),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
  dummy =
    addWidBool(i18n("Week view mode uses row layout"),
               &(KOPrefs::instance()->mMonthViewWeekRowlayout),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
  dummy =
    addWidBool(i18n("Show Sat/Sun together"),
               &(KOPrefs::instance()->mMonthViewSatSunTog),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

 KPrefsDialogWidBool *coloredCategoriesInMonthView =
       addWidBool(i18n("Month view uses category colors"),
                  &(KOPrefs::instance()->mMonthViewUsesCategoryColor),topFrame);
  topLayout->addWidget(coloredCategoriesInMonthView->checkBox(),ii++,0); 

  dummy =
    addWidBool(i18n("Category colors are applied to text"),
               &(KOPrefs::instance()->mMonthViewUsesForegroundColor),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);



  if (  QApplication::desktop()->height() <= 240 ) {
      topFrame = addPage(i18n("Month View") +" 2",0,0);
      topLayout = new QGridLayout(topFrame,4,1);
      topLayout->setSpacing(2);
      topLayout->setMargin(1);
      ii = 0;
  }


 coloredCategoriesInMonthView =
       addWidBool(i18n("Month view uses day colors"),
                  &(KOPrefs::instance()->mMonthViewUsesDayColors),topFrame);
  topLayout->addWidget(coloredCategoriesInMonthView->checkBox(),ii++,0);

 KPrefsDialogWidColor *holidayColor =
      addWidColor(i18n("Day color odd months"),
                  &(KOPrefs::instance()->mMonthViewOddColor),topFrame);
  topLayout->addWidget(holidayColor->label(),ii,0);
  topLayout->addWidget(holidayColor->button(),ii++,1);

 holidayColor =
      addWidColor(i18n("Day color even months"),
                  &(KOPrefs::instance()->mMonthViewEvenColor),topFrame);
  topLayout->addWidget(holidayColor->label(),ii,0);
  topLayout->addWidget(holidayColor->button(),ii++,1);

 
  holidayColor =
      addWidColor(i18n("Color for Sundays + category \"Holiday\""),
                  &(KOPrefs::instance()->mMonthViewHolidayColor),topFrame);
  topLayout->addWidget(holidayColor->label(),ii,0);
  topLayout->addWidget(holidayColor->button(),ii++,1);
  // *********************** What'sNext View
  topFrame = addPage(i18n("What's Next View"),0,0);
  //  DesktopIcon("viewmag",KIcon::SizeMedium));
  
  topLayout = new QGridLayout(topFrame,4,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  ii = 0;


  QHBox* hdummy = new QHBox(topFrame);
  new QLabel(i18n("Days in What's Next:"),hdummy);
  mWhatsNextSpin = new QSpinBox(1,99,1,hdummy);
  
  topLayout->addWidget(hdummy,ii++,0);

  QHBox *prioBox = new QHBox(topFrame);
  // intervalBox->setSpacing(mSpacingHint);
  topLayout->addWidget(prioBox,ii++,0);
 
  QLabel *prioLabel = new QLabel(i18n("Number of max.displayed todo prios:"), prioBox);
  mPrioSpin = new QSpinBox(0,5,1,prioBox);
  if ( QApplication::desktop()->width() <  300 )
      mPrioSpin->setFixedWidth( 40 );
  KPrefsDialogWidBool *passwdk =

      addWidBool(i18n("Show times on two lines"),
                 &(KOPrefs::instance()->mWhatsNextTime2Lines),topFrame);
  topLayout->addWidget(passwdk->checkBox(), ii++,0);
  passwdk =

      addWidBool(i18n("Show events that are done"),
                 &(KOPrefs::instance()->mWNViewShowsPast),topFrame);
  topLayout->addWidget(passwdk->checkBox(), ii++,0);
  passwdk =
      addWidBool(i18n("Show cancelled events/todos"),
                 &(KOPrefs::instance()->mWhatsNextDisplayCancelledItems),topFrame);
  topLayout->addWidget(passwdk->checkBox(), ii++,0);
  passwdk =
      addWidBool(i18n("Show parent To-Do's"),
                 &(KOPrefs::instance()->mWNViewShowsParents),topFrame);
  topLayout->addWidget(passwdk->checkBox(), ii++,0);
  
  passwdk =
      addWidBool(i18n("Show location"),
                 &(KOPrefs::instance()->mWNViewShowLocation),topFrame);
  topLayout->addWidget(passwdk->checkBox(), ii++,0);
  

  passwdk =
      addWidBool(i18n("Use short date in WN+Event view"),
                 &(KOPrefs::instance()->mShortDateInViewer),topFrame);
  topLayout->addWidget(passwdk->checkBox(), ii++,0);
   

 

  // *********************** Todo View

   topFrame = addPage(i18n("Todo View"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,4,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  ii = 0;
dummy =
    addWidBool(i18n("Hide not running Todos in To-do view"),
               &(KOPrefs::instance()->mHideNonStartedTodos),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);


 KPrefsDialogWidBool *showCompletedTodo =
      addWidBool(i18n("To-do view shows completed Todos"),
                 &(KOPrefs::instance()->mShowCompletedTodo),topFrame);
  topLayout->addWidget(showCompletedTodo->checkBox(),ii++,0);
 dummy =
    addWidBool(i18n("To-do view shows complete as 'xx %'"),
               &(KOPrefs::instance()->mTodoViewShowsPercentage),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

 dummy =
    addWidBool(i18n("Small To-do view uses smaller font"),
               &(KOPrefs::instance()->mTodoViewUsesSmallFont),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);



 dummy =
    addWidBool(i18n("Todo view uses category colors"),
               &(KOPrefs::instance()->mTodoViewUsesCatColors),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);


 QWidget* wid = new QWidget( topFrame );
  // Todo run today color
  KPrefsDialogWidColor *todoRunColor =
      addWidColor(i18n("Color for running todos:"),
                  &(KOPrefs::instance()->mTodoRunColor),wid);
  QHBoxLayout *widLayout = new QHBoxLayout(wid);
  widLayout->addWidget( todoRunColor->label() );
  widLayout->addWidget( todoRunColor->button() );
  topLayout->addWidget(wid,ii++,0);

  wid = new QWidget( topFrame );
  // Todo due today color
  KPrefsDialogWidColor *todoDueTodayColor =
      addWidColor(i18n("Todo due today color:"),
                  &(KOPrefs::instance()->mTodoDueTodayColor),wid);
  widLayout = new QHBoxLayout(wid);
  widLayout->addWidget( todoDueTodayColor->label() );
  widLayout->addWidget( todoDueTodayColor->button() );
  topLayout->addWidget(wid,ii++,0);
  //topLayout->addWidget(todoDueTodayColor->button(),ii++,1);

  // Todo overdue color
  wid = new QWidget( topFrame );
  widLayout = new QHBoxLayout(wid);
  KPrefsDialogWidColor *todoOverdueColor =
      addWidColor(i18n("Todo overdue color:"),
                  &(KOPrefs::instance()->mTodoOverdueColor),wid);
  widLayout->addWidget(todoOverdueColor->label());
  widLayout->addWidget(todoOverdueColor->button());
  topLayout->addWidget(wid,ii++,0);

 dummy =
    addWidBool(i18n("Colors are applied to text"),
               &(KOPrefs::instance()->mTodoViewUsesForegroundColor),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);



   topFrame = addPage(i18n("View Options"),0,0);

  topLayout = new QGridLayout(topFrame,4,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  ii = 0;  

   dummy  =
      addWidBool(i18n("Show Sync Events"),
                 &(KOPrefs::instance()->mShowSyncEvents),topFrame);
  topLayout->addWidget(dummy->checkBox(), ii++,0);

  lab = new QLabel( i18n("Show in todo/event viewer:"), topFrame);
  topLayout->addWidget(lab ,ii++,0);

  dummy = addWidBool(i18n("Details"),
               &(KOPrefs::instance()->mEVshowDetails),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
  dummy = addWidBool(i18n("Created time"),
               &(KOPrefs::instance()->mEVshowCreated),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
  dummy = addWidBool(i18n("Last modified time"),
               &(KOPrefs::instance()->mEVshowChanged),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);


  lab = new QLabel( i18n("Show in What'sThis quick overview:"), topFrame);
  topLayout->addWidget(lab ,ii++,0);

  dummy = addWidBool(i18n("Details"),
               &(KOPrefs::instance()->mWTshowDetails),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
  dummy = addWidBool(i18n("Created time"),
               &(KOPrefs::instance()->mWTshowCreated),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
  dummy = addWidBool(i18n("Last modified time"),
               &(KOPrefs::instance()->mWTshowChanged),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  topFrame = addPage(i18n("Conflict detection"),0,0);

  topLayout = new QGridLayout(topFrame,2,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  ii = 0;  
   dummy  = addWidBool(i18n("Enable conflict detection"),
                       &(KOPrefs::instance()->mDetectConflicts),topFrame);
  topLayout->addWidget(dummy->checkBox(), ii++,0);
  topFrame = new QFrame( topFrame );
  topLayout->addWidget(topFrame ,ii++,0);
  topLayout = new QGridLayout(topFrame,4,1); 
  connect ( dummy->checkBox(), SIGNAL( toggled( bool ) ), topFrame, SLOT ( setEnabled( bool ) ) );
  dummy = addWidBool(i18n("Include events which \"show as free\""),
                           &(KOPrefs::instance()->mIncludeFree),topFrame);
  topLayout->addWidget(dummy->checkBox(), ii++,0);
  topLayout->addWidget( new QLabel ( i18n("Conflict detection checks an <b>edited event</b> with <b>other events</b> for overlapping."), topFrame ) , ii++,0); 
 
  topLayout->addWidget( new QLabel ( i18n("Filter for the edited event"), topFrame ) , ii++,0);
  mFilterEditItem = new QComboBox(  topFrame );
  topLayout->addWidget(mFilterEditItem,ii++,0);
  topLayout->addWidget( new QLabel ( i18n("Filter for other events"), topFrame ) , ii++,0);
  mFilterAllItem = new QComboBox( topFrame );
  topLayout->addWidget(mFilterAllItem,ii++,0);
  dummy = addWidBool(i18n("Check Allday with Allday"),
                           &(KOPrefs::instance()->mCheckConflictsAllDayAllDay),topFrame);
  topLayout->addWidget(dummy->checkBox(), ii++,0);
  dummy = addWidBool(i18n("Check Allday with NonAllday"),
                           &(KOPrefs::instance()->mCheckConflictsAllDayNonAD),topFrame);
  topLayout->addWidget(dummy->checkBox(), ii++,0);
  dummy = addWidBool(i18n("Check NonAllday with Allday"),
                           &(KOPrefs::instance()->mCheckConflictsNonADAllDay),topFrame);
  topLayout->addWidget(dummy->checkBox(), ii++,0);
  dummy = addWidBool(i18n("Check NonAllday with NonAllday"),
                           &(KOPrefs::instance()->mCheckConflictsNonADNonAD),topFrame);
  topLayout->addWidget(dummy->checkBox(), ii++,0);




   topFrame = addPage(i18n("Alarm"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,2,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);
  int iii = 0;

 dummy =
    addWidBool(i18n("Use internal alarm notification"),
               &(KOPrefs::instance()->mUseInternalAlarmNotification),topFrame);
  topLayout->addWidget(dummy->checkBox(),iii++,0);

  if (  QApplication::desktop()->height() > 240 ) {
      
      lab = new QLabel( i18n("Note: KO/Pi must be running to notify you about an alarm. Recommended for use on Zaurus: Disable this option and install KO/Pi alarm applet.\n"), topFrame);
      
      topLayout->addWidget(lab ,iii++,0);
  }
#ifndef DESKTOP_VERSION
    lab->setAlignment( AlignLeft|WordBreak|AlignTop);
#else
    lab->setAlignment( AlignLeft|BreakAnywhere|WordBreak|AlignTop);
    lab->setSizePolicy( QSizePolicy(  QSizePolicy::Ignored , QSizePolicy::Ignored,true) );
#endif
 
    QHBox* dummyBox = new QHBox(topFrame);
    new QLabel(i18n("Play beeps count:"),dummyBox);
    mAlarmPlayBeeps = new QSpinBox(0,500,1,dummyBox);
    topLayout->addWidget(dummyBox,iii++,0);

    dummyBox = new QHBox(topFrame);
    new QLabel(i18n("Beeps interval in sec:"),dummyBox);
    mAlarmBeepInterval = new QSpinBox(1,600,1,dummyBox);
    topLayout->addWidget(dummyBox,iii++,0);

    dummyBox = new QHBox(topFrame);
    new QLabel(i18n("Default suspend time in min:"),dummyBox);
    mAlarmSuspendTime = new QSpinBox(1,600,1,dummyBox);
    topLayout->addWidget(dummyBox,iii++,0);

    dummyBox = new QHBox(topFrame);
    new QLabel(i18n("Auto suspend count:"),dummyBox);
    mAlarmSuspendCount = new QSpinBox(0,60,1,dummyBox);
    topLayout->addWidget(dummyBox,iii++,0);



    QHBox* hbo = new QHBox ( topFrame );
    mDefaultAlarmFile = new QLineEdit(hbo);
    QPushButton * loadTemplate = new QPushButton(hbo); 
    QPixmap icon;
    if ( QApplication::desktop()->width() < 321 ) 
        icon = SmallIcon("fileimport16");
    else
        icon = SmallIcon("fileimport");
    loadTemplate->setIconSet (icon ) ; 
    connect( loadTemplate, SIGNAL( clicked() ), this , SLOT( selectSoundFile() ) );
    int size = loadTemplate->sizeHint().height();
    loadTemplate->setFixedSize(  size, size );
  //lab = new QLabel( i18n("This setting is useless for 5500 user!"), topFrame);
  // topLayout->addWidget(lab ,iii++,0);
   lab = new QLabel( i18n("Alarm *.wav file for newly created alarm:"), topFrame);
   topLayout->addWidget(lab ,iii++,0);
   topLayout->addWidget(hbo,iii++,0);
  //  lab = new QLabel( i18n("Note: This does not mean, that for every alarm this file is replayed. This file here is associated with a newly created alarm."), topFrame);

//    topLayout->addWidget(lab ,iii++,0);
// #ifndef DESKTOP_VERSION
//     lab->setAlignment( AlignLeft|WordBreak|AlignTop);
// #else
//     lab->setAlignment( AlignLeft|BreakAnywhere|WordBreak|AlignTop);
//     lab->setSizePolicy( QSizePolicy(  QSizePolicy::Ignored , QSizePolicy::Ignored,true) );
// #endif
 

}

void KOPrefsDialog::selectSoundFile()
{
    QString fileName = mDefaultAlarmFile->text();
    fileName = KFileDialog::getSaveFileName(  mDefaultAlarmFile->text() , "Choose default alarm file", this );
     if ( fileName.length() > 0 )
         mDefaultAlarmFile->setText( fileName );
}
void KOPrefsDialog::setupFontsTab()
{
 
    QFrame *topFrame = addPage(i18n("Fonts"),0,0);
    //  DesktopIcon("fonts",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,7,3);
  topLayout->setSpacing(1);
  topLayout->setMargin(3);
  KPrefsDialogWidFont * tVFont;
  int i = 0;
  KPrefsDialogWidFont *timeLabelsFont =
      addWidFont(i18n("23"),i18n("DateNavigator:(nr)"),
                 &(KOPrefs::instance()->mDateNavigatorFont),topFrame);
  topLayout->addWidget(timeLabelsFont->label(),i,0);
  topLayout->addWidget(timeLabelsFont->preview(),i,1);
  topLayout->addWidget(timeLabelsFont->button(),i,2);
  ++i;


  timeLabelsFont =
      addWidFont(i18n("Mon 15"),i18n("Date Labels:"),
                 &(KOPrefs::instance()->mTimeLabelsFont),topFrame);
  topLayout->addWidget(timeLabelsFont->label(),i,0);
  topLayout->addWidget(timeLabelsFont->preview(),i,1);
  topLayout->addWidget(timeLabelsFont->button(),i,2);
  ++i;

  KPrefsDialogWidFont *timeBarFont =
      addWidFont(KGlobal::locale()->formatTime(QTime(12,34)),i18n("Time bar:"),
                 &(KOPrefs::instance()->mTimeBarFont),topFrame);
  topLayout->addWidget(timeBarFont->label(),i,0);
  topLayout->addWidget(timeBarFont->preview(),i,1);
  topLayout->addWidget(timeBarFont->button(),i,2);
  ++i;


  KPrefsDialogWidFont *marcusBainsFont =
      addWidFont(KGlobal::locale()->formatTime(QTime(12,34,23)),i18n("M. Bains line:"),
                 &(KOPrefs::instance()->mMarcusBainsFont),topFrame);
  topLayout->addWidget(marcusBainsFont->label(),i,0);
  topLayout->addWidget(marcusBainsFont->preview(),i,1);
  topLayout->addWidget(marcusBainsFont->button(),i,2);
  ++i;

  tVFont =
      addWidFont(i18n("Summary"),i18n("Event Viewer:"),
                 &(KOPrefs::instance()->mEventViewFont),topFrame);
  topLayout->addWidget(tVFont->label(),i,0);
  topLayout->addWidget(tVFont->preview(),i,1);
  topLayout->addWidget(tVFont->button(),i,2);
  ++i;



  tVFont =
      addWidFont(i18n("Details"),i18n("EditorBox:"),
                 &(KOPrefs::instance()->mEditBoxFont),topFrame);
  topLayout->addWidget(tVFont->label(),i,0);
  topLayout->addWidget(tVFont->preview(),i,1);
  topLayout->addWidget(tVFont->button(),i,2);
  ++i;


 
  topLayout->setColStretch(1,1);
  topLayout->setRowStretch(4,1);


  i = 0;
  topFrame = addPage(i18n("View Fonts"),0,
                             DesktopIcon("fonts",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,7,3);
  topLayout->setSpacing(1);
  topLayout->setMargin(3);

  tVFont =
      addWidFont(i18n("Configure KO"),i18n("What's Next View:"),
                 &(KOPrefs::instance()->mWhatsNextFont),topFrame);
  topLayout->addWidget(tVFont->label(),i,0);
  topLayout->addWidget(tVFont->preview(),i,1);
  topLayout->addWidget(tVFont->button(),i,2);
  ++i;
  KPrefsDialogWidFont *agendaViewFont =
      addWidFont(i18n("Event text"),i18n("Agenda view:"),
                 &(KOPrefs::instance()->mAgendaViewFont),topFrame);
  topLayout->addWidget(agendaViewFont->label(),i,0);
  topLayout->addWidget(agendaViewFont->preview(),i,1);
  topLayout->addWidget(agendaViewFont->button(),i,2);
  ++i;


  KPrefsDialogWidFont *monthViewFont =
      addWidFont(KGlobal::locale()->formatTime(QTime(12,34)) + " " + i18n("Event"),
                 i18n("Month view:"),&(KOPrefs::instance()->mMonthViewFont),topFrame);
  topLayout->addWidget(monthViewFont->label(),i,0);
  topLayout->addWidget(monthViewFont->preview(),i,1);
  topLayout->addWidget(monthViewFont->button(),i,2);
  ++i;


 KPrefsDialogWidFont *lVFont =
      addWidFont(i18n("Event"),i18n("List View:"),
                 &(KOPrefs::instance()->mListViewFont),topFrame);
  topLayout->addWidget(lVFont->label(),i,0);
  topLayout->addWidget(lVFont->preview(),i,1);
  topLayout->addWidget(lVFont->button(),i,2);
  ++i;


  tVFont =
      addWidFont(i18n("ToDo"),i18n("ToDoView:"),
                 &(KOPrefs::instance()->mTodoViewFont),topFrame);
  topLayout->addWidget(tVFont->label(),i,0);
  topLayout->addWidget(tVFont->preview(),i,1);
  topLayout->addWidget(tVFont->button(),i,2);
  ++i;


  tVFont =
      addWidFont(i18n("Today"),i18n("JournalView:"),
                 &(KOPrefs::instance()->mJornalViewFont),topFrame);
  topLayout->addWidget(tVFont->label(),i,0);
  topLayout->addWidget(tVFont->preview(),i,1);
  topLayout->addWidget(tVFont->button(),i,2);
  ++i;




  topLayout->setColStretch(1,1);
  topLayout->setRowStretch(4,1);




}

void KOPrefsDialog::setupColorsTab()
{
    QFrame *topFrame = addPage(i18n("Colors"),0,0);
    //  DesktopIcon("colorize",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,5,2);
  // topLayout->setSpacing(mSpacingHint);
  // topLayout->setMargin(mMarginHint);

  topLayout->setSpacing(2);
  topLayout->setMargin(3);

  int ii = 1;
  QGroupBox *categoryGroup ;

  categoryGroup = new QGroupBox(1,Vertical,i18n("Categories"),
                                           topFrame);
  topLayout->addMultiCellWidget(categoryGroup,0,0,0,1);

  mCategoryCombo = new QComboBox(categoryGroup);
  mCategoryCombo->insertStringList(KOPrefs::instance()->mCustomCategories);
  connect(mCategoryCombo,SIGNAL(activated(int)),SLOT(updateCategoryColor()));
   
  mCategoryButton = new KColorButton(categoryGroup);
  connect(mCategoryButton,SIGNAL(changed(const QColor &)),SLOT(setCategoryColor()));
  updateCategoryColor();


  // Holiday Color

  KPrefsDialogWidColor *holidayColor =
      addWidColor(i18n("Holiday color:"),
                  &(KOPrefs::instance()->mHolidayColor),topFrame);
  topLayout->addWidget(holidayColor->label(),ii,0);
  topLayout->addWidget(holidayColor->button(),ii++,1);

  // Highlight Color
  KPrefsDialogWidColor *highlightColor =
      addWidColor(i18n("Highlight color:"),
                  &(KOPrefs::instance()->mHighlightColor),topFrame);
  topLayout->addWidget(highlightColor->label(),ii,0);
  topLayout->addWidget(highlightColor->button(),ii++,1);

  // Event color
  KPrefsDialogWidColor *eventColor =
      addWidColor(i18n("Default event color:"),
                  &(KOPrefs::instance()->mEventColor),topFrame);
  topLayout->addWidget(eventColor->label(),ii,0);
  topLayout->addWidget(eventColor->button(),ii++,1);
  eventColor =
      addWidColor(i18n("Default todo done color:"),
                  &(KOPrefs::instance()->mTodoDoneColor),topFrame);
  topLayout->addWidget(eventColor->label(),ii,0);
  topLayout->addWidget(eventColor->button(),ii++,1);

  if (  QApplication::desktop()->height() <= 240 ) {
      topFrame = addPage(i18n("Colors") +" 2",0,0);
      //  DesktopIcon("colorize",KIcon::SizeMedium));
      
      topLayout = new QGridLayout(topFrame,5,2);
      // topLayout->setSpacing(mSpacingHint);
      // topLayout->setMargin(mMarginHint);
      
      topLayout->setSpacing(2);
      topLayout->setMargin(3);
  }
  

  // agenda view background color
  KPrefsDialogWidColor *agendaBgColor =
      addWidColor(i18n("Agenda view background color:"),
                  &(KOPrefs::instance()->mAgendaBgColor),topFrame);
  topLayout->addWidget(agendaBgColor->label(),ii,0);
  topLayout->addWidget(agendaBgColor->button(),ii++,1);

  // working hours color
  KPrefsDialogWidColor *workingHoursColor =
      addWidColor(i18n("Working hours color:"),
                  &(KOPrefs::instance()->mWorkingHoursColor),topFrame);
  topLayout->addWidget(workingHoursColor->label(),ii,0);
  topLayout->addWidget(workingHoursColor->button(),ii++,1);

  KPrefsDialogWidBool *sb =
           addWidBool(i18n("Use colors for application:"),
                      &(KOPrefs::instance()->mUseAppColors),topFrame);
       topLayout->addMultiCellWidget(sb->checkBox(), ii, ii, 0,1 );

       ii++;
    KPrefsDialogWidColor *   workingHoursColor1 =
           addWidColor(i18n("Buttons, menus, etc.:"),
                       &(KOPrefs::instance()->mAppColor1),topFrame);
       topLayout->addWidget(workingHoursColor1->label(),ii,0);
       topLayout->addWidget(workingHoursColor1->button(),ii++,1);
       
      KPrefsDialogWidColor *   workingHoursColor2 =
           addWidColor(i18n("Frames, labels, etc.:"),
                       &(KOPrefs::instance()->mAppColor2),topFrame);
       topLayout->addWidget(workingHoursColor2->label(),ii,0);
       topLayout->addWidget(workingHoursColor2->button(),ii++,1);


  
}

void KOPrefsDialog::setCategoryColor()
{
  mCategoryDict.replace(mCategoryCombo->currentText(), new QColor(mCategoryButton->color()));
}

void KOPrefsDialog::updateCategoryColor()
{
  QString cat = mCategoryCombo->currentText();
  QColor *color = mCategoryDict.find(cat);
  if (!color) {
    color = KOPrefs::instance()->categoryColor(cat);
  }
  if (color) {
    mCategoryButton->setColor(*color);
  }
}

void KOPrefsDialog::setupPrinterTab()
{
  mPrinterTab = addPage(i18n("Printing"),0,
                        DesktopIcon("fileprint",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(mPrinterTab,5,2);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);

  topLayout->setRowStretch(4,1);
}

void KOPrefsDialog::setupGroupSchedulingTab()
{
#if 0
  QFrame *topFrame  = addPage(i18n("Group Scheduling"),0,
                              DesktopIcon("personal",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,6,2);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);

#if 0
  KPrefsDialogWidRadios *schedulerGroup =
      addWidRadios(i18n("Scheduler Mail Client"),&(KOPrefs::instance()->mIMIPScheduler),
                   topFrame);
  schedulerGroup->addRadio("Dummy"); // Only for debugging
  schedulerGroup->addRadio(i18n("Mail client"));

  topLayout->addMultiCellWidget(schedulerGroup->groupBox(),0,0,0,1);
#endif

  KPrefsDialogWidRadios *sendGroup =
      addWidRadios(i18n("Scheduler Mails Should Be"),&(KOPrefs::instance()->mIMIPSend),
                   topFrame);
  sendGroup->addRadio(i18n("Send to outbox"));
  sendGroup->addRadio(i18n("Send directly"));

  topLayout->addMultiCellWidget(sendGroup->groupBox(),1,1,0,1);

  topLayout->addMultiCellWidget(new QLabel(i18n("Additional email addresses:"),topFrame),2,2,0,1);
  mAMails = new QListView(topFrame);
  mAMails->addColumn(i18n("Email"),300);
  topLayout->addMultiCellWidget(mAMails,3,3,0,1);

  topLayout->addWidget(new QLabel(i18n("Additional email address:"),topFrame),4,0);
  aEmailsEdit = new QLineEdit(topFrame);
  aEmailsEdit->setEnabled(false);
  topLayout->addWidget(aEmailsEdit,4,1);

  QPushButton *add = new QPushButton(i18n("New"),topFrame,"new");
  topLayout->addWidget(add,5,0);
  QPushButton *del = new QPushButton(i18n("Remove"),topFrame,"remove");
  topLayout->addWidget(del,5,1);

  //topLayout->setRowStretch(2,1);
  connect(add, SIGNAL( clicked() ), this, SLOT(addItem()) );
  connect(del, SIGNAL( clicked() ), this, SLOT(removeItem()) );
  connect(aEmailsEdit,SIGNAL( textChanged(const QString&) ), this,SLOT(updateItem()));
  connect(mAMails,SIGNAL(selectionChanged(QListViewItem *)),SLOT(updateInput()));
#endif
}

void KOPrefsDialog::setupGroupAutomationTab()
{
    return;
  QFrame *topFrame  = addPage(i18n("Group Automation"),0,
                              DesktopIcon("personal",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,5,1);
  topLayout->setSpacing(mSpacingHint);
  topLayout->setMargin(mMarginHint);

  KPrefsDialogWidRadios *autoRefreshGroup =
      addWidRadios(i18n("Auto Send Refresh"),
                   &(KOPrefs::instance()->mIMIPAutoRefresh),topFrame);
  autoRefreshGroup->addRadio(i18n("Never"));
  autoRefreshGroup->addRadio(i18n("If attendee is in addressbook"));
  //autoRefreshGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoRefreshGroup->groupBox(),0,0,0,0);

  KPrefsDialogWidRadios *autoInsertGroup =
      addWidRadios(i18n("Auto Insert IMIP Replies"),
                   &(KOPrefs::instance()->mIMIPAutoInsertReply),topFrame);
  autoInsertGroup->addRadio(i18n("Never"));
  autoInsertGroup->addRadio(i18n("If attendee is in addressbook"));
  //autoInsertGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoInsertGroup->groupBox(),1,1,0,0);

  KPrefsDialogWidRadios *autoRequestGroup =
      addWidRadios(i18n("Auto Insert IMIP Requests"),
                   &(KOPrefs::instance()->mIMIPAutoInsertRequest),topFrame);
  autoRequestGroup->addRadio(i18n("Never"));
  autoRequestGroup->addRadio(i18n("If organizer is in addressbook"));
  //autoInsertGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoRequestGroup->groupBox(),2,2,0,0);

  KPrefsDialogWidRadios *autoFreeBusyGroup =
      addWidRadios(i18n("Auto Send FreeBusy Information"),
                   &(KOPrefs::instance()->mIMIPAutoFreeBusy),topFrame);
  autoFreeBusyGroup->addRadio(i18n("Never"));
  autoFreeBusyGroup->addRadio(i18n("If requested from an email in addressbook"));
  //autoFreeBusyGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoFreeBusyGroup->groupBox(),3,3,0,0);

  KPrefsDialogWidRadios *autoFreeBusyReplyGroup =
      addWidRadios(i18n("Auto Save FreeBusy Replies"),
                   &(KOPrefs::instance()->mIMIPAutoFreeBusyReply),topFrame);
  autoFreeBusyReplyGroup->addRadio(i18n("Never"));
  autoFreeBusyReplyGroup->addRadio(i18n("If attendee is in addressbook"));
  //autoFreeBusyGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoFreeBusyReplyGroup->groupBox(),4,4,0,0);
}

void KOPrefsDialog::showPrinterTab()
{
  showPage(pageIndex(mPrinterTab));
}


void KOPrefsDialog::setCombo(QComboBox *combo, const QString & text,
                               const QStringList *tags)
{
  if (tags) {
    int i = tags->findIndex(text);
    if (i > 0) combo->setCurrentItem(i);
  } else {
    for(int i=0;i<combo->count();++i) {
      if (combo->text(i) == text) {
        combo->setCurrentItem(i);
        break;
      }
    }
  }
}

void KOPrefsDialog::usrReadConfig()
{
  
  mNameEdit->setText(KOPrefs::instance()->fullName());
  mEmailEdit->setText(KOPrefs::instance()->email());

  mAutoSaveIntervalSpin->setValue(KOPrefs::instance()->mAutoSaveInterval);
 
  mStartTimeSpin->setValue(KOPrefs::instance()->mStartTime);
  mDefaultDurationSpin->setValue(KOPrefs::instance()->mDefaultDuration);
  mAlarmTimeCombo->setCurrentItem(KOPrefs::instance()->mAlarmTime);

  mNextXDaysSpin->setValue(KOPrefs::instance()->mNextXDays);
  mWhatsNextSpin->setValue(KOPrefs::instance()->mWhatsNextDays);
  mPrioSpin->setValue(KOPrefs::instance()->mWhatsNextPrios);
  // mAMails->clear();
 //  for ( QStringList::Iterator it = KOPrefs::instance()->mAdditionalMails.begin();
//             it != KOPrefs::instance()->mAdditionalMails.end(); ++it ) {
//     QListViewItem *item = new QListViewItem(mAMails);
//     item->setText(0,*it);
//     mAMails->insertItem(item);
//   }

  //that soundmLocalTempFile->setText(KOPrefs::instance()->mLocalTempFile);
  mDefaultAlarmFile->setText(KOPrefs::instance()->mDefaultAlarmFile);
  updateCategories();
  mAlarmPlayBeeps->setValue(KOPrefs::instance()->mAlarmPlayBeeps );
  mAlarmSuspendTime->setValue(KOPrefs::instance()->mAlarmSuspendTime );
  mAlarmSuspendCount->setValue(KOPrefs::instance()->mAlarmSuspendCount );
  mAlarmBeepInterval->setValue(KOPrefs::instance()->mAlarmBeepInterval );
  mCatDefaultColor = KOPrefs::instance()->mEventColor;


    KConfig cfgko(locateLocal("config","korganizerrc"));
    cfgko.setGroup("General");
    QStringList temp = cfgko.readListEntry("CalendarFilters");
    temp.prepend(i18n("No Filter") );
    mFilterEditItem->insertStringList( temp );
    mFilterAllItem->insertStringList( temp );
    int index = temp.findIndex( KOPrefs::instance()->mFilterConflictEditItem );
    if ( index >= 0 )
        mFilterEditItem->setCurrentItem( index );
    index = temp.findIndex( KOPrefs::instance()->mFilterConflictAllItem );
    if ( index >= 0 )
        mFilterAllItem->setCurrentItem( index );
}


void KOPrefsDialog::usrWriteConfig()
{
    KOPrefs::instance()->mDefaultAlarmFile =mDefaultAlarmFile->text();
    KOPrefs::instance()->setFullName(mNameEdit->text());
    KOPrefs::instance()->setEmail(mEmailEdit->text());
    
    KOPrefs::instance()->mAutoSaveInterval = mAutoSaveIntervalSpin->value();
    KOPrefs::instance()->mStartTime = mStartTimeSpin->value();
    KOPrefs::instance()->mDefaultDuration = mDefaultDurationSpin->value();
    KOPrefs::instance()->mAlarmTime = mAlarmTimeCombo->currentItem();
    if ( mCatDefaultColor != KOPrefs::instance()->mEventColor ) {
        QStringList cat = KOPrefs::instance()->mCustomCategories;
        int iii = 0;
        while ( iii < cat.count() ) {
            if ( *KOPrefs::instance()->categoryColor( cat[ iii ] ) == mCatDefaultColor )
                KOPrefs::instance()->setCategoryColor( cat[ iii ], KOPrefs::instance()->mEventColor );
            ++iii;
        }
    }
    QDictIterator<QColor> it(mCategoryDict);
    while (it.current()) {
        KOPrefs::instance()->setCategoryColor(it.currentKey(),*it.current());
        ++it;
    }
    
    KOPrefs::instance()->mNextXDays = mNextXDaysSpin->value();
    KOPrefs::instance()->mWhatsNextDays = mWhatsNextSpin->value();
    KOPrefs::instance()->mWhatsNextPrios = mPrioSpin->value();
    
    KOPrefs::instance()->mAdditionalMails.clear();
    //  QListViewItem *item;
    //  item = mAMails->firstChild();
    //   while (item)
    //   {
    //     KOPrefs::instance()->mAdditionalMails.append( item->text(0) );
    //     item = item->nextSibling();
    //   }
    KOPrefs::instance()->mAlarmPlayBeeps  = mAlarmPlayBeeps->value();
    KOPrefs::instance()->mAlarmSuspendTime = mAlarmSuspendTime->value() ;
    KOPrefs::instance()->mAlarmSuspendCount= mAlarmSuspendCount->value() ;
    KOPrefs::instance()->mAlarmBeepInterval= mAlarmBeepInterval->value() ;

    KOPrefs::instance()->mFilterConflictEditItem = mFilterEditItem->currentText();
    KOPrefs::instance()->mFilterConflictAllItem = mFilterAllItem->currentText();
}

void KOPrefsDialog::updateCategories()
{
  mCategoryCombo->clear();
  mCategoryDict.clear();
  mCategoryCombo->insertStringList(KOPrefs::instance()->mCustomCategories);
  updateCategoryColor();
}

void KOPrefsDialog::toggleEmailSettings(bool on)
{
  if (on) {
    mEmailEdit->setEnabled(false);
    mNameEdit->setEnabled(false);
    mEmailLabel->setEnabled(false);
    mNameLabel->setEnabled(false);

    KEMailSettings settings;
    mNameEdit->setText(settings.getSetting(KEMailSettings::RealName));
    mEmailEdit->setText(settings.getSetting(KEMailSettings::EmailAddress));
  } else {
    mEmailEdit->setEnabled(true);
    mNameEdit->setEnabled(true);
    mEmailLabel->setEnabled(true);
    mNameLabel->setEnabled(true);
  }
}

void KOPrefsDialog::addItem()
{
 //  aEmailsEdit->setEnabled(true);
//   QListViewItem *item = new QListViewItem(mAMails);
//   mAMails->insertItem(item);
//   mAMails->setSelected(item,true);
//   aEmailsEdit->setText(i18n("(EmptyEmail)"));
}

void KOPrefsDialog::removeItem()
{
//   QListViewItem *item;
//   item = mAMails->selectedItem();
//   if (!item) return;
//   mAMails->takeItem(item);
//   item = mAMails->selectedItem();
//   if (!item) {
//     aEmailsEdit->setText("");
//     aEmailsEdit->setEnabled(false);
//   }
//   if (mAMails->childCount() == 0) {
//     aEmailsEdit->setEnabled(false);
//   }
}

void KOPrefsDialog::updateItem()
{
 //  QListViewItem *item;
//   item = mAMails->selectedItem();
//   if (!item) return;
//   item->setText(0,aEmailsEdit->text());
}

void KOPrefsDialog::updateInput()
{
//   QListViewItem *item;
//   item = mAMails->selectedItem();
//   if (!item) return;
//   aEmailsEdit->setEnabled(true);
//   aEmailsEdit->setText(item->text(0));
}

