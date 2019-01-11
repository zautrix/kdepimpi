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

#include <time.h>
#ifndef _WIN32_
#include <unistd.h>
#endif
#include <qdir.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qstring.h>
#include <qregexp.h>
#include <qfont.h>
#include <qcolor.h>
#include <qstringlist.h>
#include <stdlib.h>

#include <kglobal.h>
#include <kglobalsettings.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>
#include <kemailsettings.h>
#include <kstaticdeleter.h>
#include <libkdepim/kpimglobalprefs.h>

#include "koprefs.h"
#include "mainwindow.h"

KOPrefs *KOPrefs::mInstance = 0;
static KStaticDeleter<KOPrefs> insd;

KOPrefs::KOPrefs() :
  KPimPrefs("korganizerrc")
{
    mGlobalUpdateDisabled = false;
  mCategoryColors.setAutoDelete(true);
  fillMailDefaults();
  mDefaultCategoryColor           = QColor(175,210,255);//196,196,196);
  QColor defaultHolidayColor      = QColor(255,0,0);
  QColor defaultHighlightColor    = QColor(129,112,255);//64,64,255);
  QColor defaultAgendaBgColor     = QColor(239,241,169);//128,128,128);
  QColor defaultWorkingHoursColor = QColor(170,223,150);//160,160,160);
  QColor defaultTodoDueTodayColor = QColor(255,220,100);
  QColor defaultTodoOverdueColor  = QColor(255,153,125);
  QColor defaultTodoRunColor  = QColor(99,194,30);
  KPrefs::setCurrentGroup("General");
  addItemBool("Enable Group Scheduling",&mEnableGroupScheduling,false);
  addItemBool("ShowIconNewTodo",&mShowIconNewTodo,true);
  addItemBool("ShowIconNewEvent",&mShowIconNewEvent,true);
  addItemBool("ShowIconSearch",&mShowIconSearch,true);
  addItemBool("ShowIconList",&mShowIconList,true);
  addItemBool("ShowIconDay1",&mShowIconDay1,true);
  addItemBool("ShowIconDay5",&mShowIconDay5,true);
  addItemBool("ShowIconDay6",&mShowIconDay6,true);
  addItemBool("ShowIconDay7",&mShowIconDay7,true);
  addItemBool("ShowIconMonth",&mShowIconMonth,true);
  addItemBool("ShowIconTodoview",&mShowIconTodoview,true);
  addItemBool("ShowIconBackFast",&mShowIconBackFast,true);
  addItemBool("ShowIconBack",&mShowIconBack,true);
  addItemBool("ShowIconToday",&mShowIconToday,true);
  addItemBool("ShowIconForward",&mShowIconForward,true);
  addItemBool("ShowIconForwardFast",&mShowIconForwardFast,true);
  addItemBool("ShowIconWhatsThis",&mShowIconWhatsThis,true);
  addItemBool("ShowIconWeekNum",&mShowIconWeekNum,true);
  addItemBool("ShowIconNextDays",&mShowIconNextDays,true);
  addItemBool("ShowIconNext",&mShowIconNext,true);
  addItemBool("ShowIconJournal",&mShowIconJournal,true); 
  addItemBool("ShowIconStretch",&mShowIconStretch,true); 
  addItemBool("ShowIconFilter",&mShowIconFilter,false); 
  addItemBool("ShowIconOnetoolbar",&mShowIconOnetoolbar,true); 

  bool addIcons = false;
#ifdef DESKTOP_VERSION
  addIcons = true;
#endif
  addItemBool("ShowIconNavigator",&mShowIconNavigator,addIcons); 
  addItemBool("ShowIconAllday",&mShowIconAllday,addIcons); 
  addItemBool("ShowIconFilterview",&mShowIconFilterview,addIcons); 
  addItemBool("ShowIconToggleFull",&mShowIconToggleFull,addIcons); 

  addItemInt("LastLoadedLanguage",&mOldLanguage,0);

  addItemBool("AskForQuit",&mAskForQuit,false); 

#ifndef DESKTOP_VERSION
  addItemBool("ShowFullMenu",&mShowFullMenu,false); 
#else
  addItemBool("ShowFullMenu",&mShowFullMenu,true); 
#endif
  addItemBool("ToolBarHor",&mToolBarHor, true );
  addItemBool("ToolBarUp",&mToolBarUp, false );
  addItemBool("ToolBarHorV",&mToolBarHorV, true );
  addItemBool("ToolBarUpV",&mToolBarUpV, false );
  addItemBool("ToolBarHorN",&mToolBarHorN, true );
  addItemBool("ToolBarUpN",&mToolBarUpN, false );
  addItemBool("ToolBarHorF",&mToolBarHorF, true );
  addItemBool("ToolBarUpF",&mToolBarUpF, false );
  addItemBool("ToolBarMiniIcons",&mToolBarMiniIcons, false );
  addItemBool("WhatsNextTime2Lines",&mWhatsNextTime2Lines, false );
  addItemBool("WhatsNextTime2Lines",&mWhatsNextDisplayCancelledItems, false );
  addItemInt("Whats Next Days",&mWhatsNextDays,3);
  addItemInt("Whats Next Prios",&mWhatsNextPrios,1);

  addItemBool("ShowTodoInAgenda",&mShowTodoInAgenda,true);
  addItemBool("ShowCompletedTodoInAgenda",&mShowCompletedTodoInAgenda,true);
  addItemBool("ShowTimeInAgenda",&mShowTimeInAgenda,true);
  addItemBool("HideNonStartedTodos",&mHideNonStartedTodos,false);
  addItemBool("ShowCompletedTodo",&mShowCompletedTodo,true);
  addItemInt("AllDay Size",&mAllDaySize,28);
  QString defAlarm = KGlobal::iconLoader()->iconPath()+"koalarm.wav";
  addItemString("DefaultAlarmFile",&mDefaultAlarmFile,defAlarm );

  addItemStringList("LocationDefaults",&mLocationDefaults );
  addItemStringList("EventSummary User",&mEventSummaryUser);
  addItemStringList("TodoSummary User",&mTodoSummaryUser);
  addItemStringList("JournalSummary User",&mJournalSummaryUser);

  addItemBool("Enable Group Scheduling",&mEnableGroupScheduling,false);
  addItemBool("Enable Project View",&mEnableProjectView,false);
  addItemBool("Auto Save",&mAutoSave,false);
  addItemInt("Auto Save Interval",&mAutoSaveInterval,3);
  addItemBool("Confirm Deletes",&mConfirm,true);
  addItemString("Archive File",&mArchiveFile);
  addItemString("Html Export File",&mHtmlExportFile,
      QDir::homeDirPath() + "/" + i18n("Default export file", "calendar.html"));
  addItemBool("Html With Save",&mHtmlWithSave,false);

  KPrefs::setCurrentGroup("Personal Settings");

  addItemInt("Mail Client",&mMailClient,MailClientKMail);
  addItemBool("Use Control Center Email",&mEmailControlCenter,false);
  addItemBool("Bcc",&mBcc,false);

  KPrefs::setCurrentGroup("Time & Date");


  addItemInt("Default Start Time",&mStartTime,10);
  addItemInt("Default Duration",&mDefaultDuration,2);
  addItemInt("Default Alarm Time",&mAlarmTime,3);
  KPrefs::setCurrentGroup("AlarmSettings");
  addItemInt("AlarmPlayBeeps",&mAlarmPlayBeeps,20);
  addItemInt("AlarmSuspendTime",&mAlarmSuspendTime,7);
  addItemInt("AlarmSuspendCount",&mAlarmSuspendCount,5);
  addItemInt("AlarmBeepInterval",&mAlarmBeepInterval,3);


  KPrefs::setCurrentGroup("Calendar");

  addItemInt("Default Calendar Format",&mDefaultFormat,FormatICalendar);

  KPrefs::setCurrentGroup("Fonts");
  // qDebug(" KPrefs::setCurrentGroup(Fonts); ");
  QFont fon = KGlobalSettings::generalFont();
  addItemFont("TimeBar Font",&mTimeBarFont,fon );
  addItemFont("MonthView Font",&mMonthViewFont,fon);
  addItemFont("AgendaView Font",&mAgendaViewFont,fon);
  addItemFont("MarcusBains Font",&mMarcusBainsFont,fon);
  addItemFont("TimeLabels Font",&mTimeLabelsFont,fon);
  addItemFont("TodoView Font",&mTodoViewFont,fon);
  addItemFont("ListView Font",&mListViewFont,fon);
  addItemFont("DateNavigator Font",&mDateNavigatorFont,fon);
  addItemFont("EditBox Font",&mEditBoxFont,fon);
  addItemFont("JournalView Font",&mJornalViewFont,fon);
  addItemFont("WhatsNextView Font",&mWhatsNextFont,fon);
  addItemFont("EventView Font",&mEventViewFont,fon);

  KPrefs::setCurrentGroup("RemoteSyncing");
  addItemString("ActiveSyncPort",&mActiveSyncPort,"9197" );  
  addItemString("ActiveSyncIP",&mActiveSyncIP,"192.168.0.40" ); 
  addItemBool("ShowSyncEvents",&mShowSyncEvents,false); 
  addItemInt("LastSyncTime",&mLastSyncTime,0); 

#ifdef _WIN32_
  QString hdp= locateLocal("data","korganizer")+"\\\\";
#else
  QString hdp= locateLocal("data","korganizer")+"/";
#endif

  KPrefs::setCurrentGroup("LoadSaveFileNames");

  addItemString("LastImportFile", &mLastImportFile ,hdp +"import.ics" );
  addItemString("LastVcalFile", &mLastVcalFile ,hdp +"export.vcs" );
  addItemString("LastSaveFile", &mLastSaveFile ,hdp +"mybackup.ics" );
  addItemString("LastLoadFile", &mLastLoadFile ,hdp +"mybackup.ics" );
 

  KPrefs::setCurrentGroup("Locale");
  addItemBool("ShortDateInViewer",&mShortDateInViewer,false); 


  KPrefs::setCurrentGroup("Colors");
  addItemColor("Holiday Color",&mHolidayColor,defaultHolidayColor);
  addItemColor("Highlight Color",&mHighlightColor,defaultHighlightColor);
  addItemColor("Event Color",&mEventColor,mDefaultCategoryColor);
  addItemColor("Todo done Color",&mTodoDoneColor,QColor(111,255,115) );
  addItemColor("Agenda Background Color",&mAgendaBgColor,defaultAgendaBgColor);
  addItemColor("WorkingHours Color",&mWorkingHoursColor,defaultWorkingHoursColor);
  addItemColor("Todo due today Color",&mTodoDueTodayColor,defaultTodoDueTodayColor);
  addItemColor("Todo overdue Color",&mTodoOverdueColor,defaultTodoOverdueColor);
  addItemColor("Todo running Color",&mTodoRunColor,defaultTodoRunColor);
  addItemColor("MonthViewEvenColor",&mMonthViewEvenColor,QColor( 160,160,255 ));
  addItemColor("MonthViewOddColor",&mMonthViewOddColor,QColor( 160,255,160 ));
  addItemColor("MonthViewHolidayColor",&mMonthViewHolidayColor,QColor( 255,160,160 ));
  addItemBool("MonthViewUsesDayColors",&mMonthViewUsesDayColors,true); 
  addItemBool("MonthViewSatSunTog",&mMonthViewSatSunTog,true);
  addItemBool("MonthViewWeek",&mMonthViewWeek,false);
  addItemBool("HightlightDateTimeEdit",&mHightlightDateTimeEdit,false); 
  addItemColor("AppColor1",&mAppColor1,QColor( 130,170,255 ));
  addItemColor("AppColor2",&mAppColor2,QColor(  174,216,255 ));
  addItemBool("UseAppColors",&mUseAppColors,false); 

  
  KPrefs::setCurrentGroup("Conflicts");
  addItemBool("EnableAutoDetect",&mDetectConflicts,true);
  addItemBool("CheckConflictsFree",&mIncludeFree,true);
  addItemBool("CheckConflictsAllDayAllDay",&mCheckConflictsAllDayAllDay,false);
  addItemBool("CheckConflictsAllDayNonAD",&mCheckConflictsAllDayNonAD,false);
  addItemBool("CheckConflictsNonADAllDay",&mCheckConflictsNonADAllDay,false);
  addItemBool("CheckConflictsNonADNonAD",&mCheckConflictsNonADNonAD,true);
  addItemString("FilterConflictEditItem", &mFilterConflictEditItem ,"nofilter" );
  addItemString("FilterConflictAllItem", &mFilterConflictAllItem ,"nofilter" );
  
  KPrefs::setCurrentGroup("Views");
  addItemBool("Block Popup Menu",&mBlockPopupMenu,false);
  addItemBool("Show Date Navigator",&mShowDateNavigator,true);
  addItemInt("Hour Size",&mHourSize,8);
  addItemBool("Show Daily Recurrences",&mDailyRecur,true);
  addItemBool("ShowLongAllday",&mLongAllday,true);
  addItemBool("Show Weekly Recurrences",&mWeeklyRecur,true);
  addItemBool("Show Month Daily Recurrences",&mMonthDailyRecur,true);
  addItemBool("Show Month Weekly Recurrences",&mMonthWeeklyRecur,true);
  addItemBool("ShowShortMonthName",&mMonthShowShort,false);
  addItemBool("ShowIconsInMonthCell",&mMonthShowIcons,true);
  addItemBool("ShowTimesInMonthCell",&mMonthShowTimes,true);
#ifdef DESKTOP_VERION
  addItemBool("Enable ToolTips",&mEnableToolTips,true);
#else
  addItemBool("Enable ToolTips",&mEnableToolTips,false);
#endif
  addItemBool("Enable MonthView ScrollBars",&mEnableMonthScroll,false);
  addItemBool("Marcus Bains shows seconds",&mMarcusBainsShowSeconds,false);
  addItemBool("Show Marcus Bains",&mMarcusBainsEnabled,true);
  addItemBool("EditOnDoubleClick",&mEditOnDoubleClick,true);
  addItemBool("ViewChangeHoldFullscreen",&mViewChangeHoldFullscreen,true);
  addItemBool("ViewChangeHoldNonFullscreen",&mViewChangeHoldNonFullscreen,false);
  addItemBool("CenterOnCurrentTime",&mCenterOnCurrentTime,false);
  addItemBool("SetTimeToDayStartAt",&mSetTimeToDayStartAt,true);
  addItemBool("HighlightCurrentDay",&mHighlightCurrentDay,true);
  addItemBool("WNViewShowsParents",&mWNViewShowsParents,true);;
  addItemBool("WNViewShowsPast",&mWNViewShowsPast,true);
  addItemBool("WNViewShowLocation",&mWNViewShowLocation,false);
  addItemBool("UseHighlightLightColor",&mUseHighlightLightColor,false);
  addItemBool("ListViewMonthTimespan",&mListViewMonthTimespan,true);
  addItemBool("TodoViewUsesCatColors",&mTodoViewUsesCatColors,false);
  addItemBool("TodoViewShowsPercentage",&mTodoViewShowsPercentage,false);
  addItemBool("TodoViewUsesSmallFont",&mTodoViewUsesSmallFont,true);
  addItemBool("MonthViewUsesBigFont",&mMonthViewUsesBigFont,true);
  addItemBool("MonthViewWeekRowlayout",&mMonthViewWeekRowlayout,false);
  addItemBool("TodoViewUsesForegroundColor",&mTodoViewUsesForegroundColor,false);
  addItemBool("MonthViewUsesForegroundColor",&mMonthViewUsesForegroundColor,false);
#ifdef DESKTOP_VERSION
  addItemBool("UseInternalAlarmNotification",&mUseInternalAlarmNotification,true);
#else
  addItemBool("UseInternalAlarmNotification",&mUseInternalAlarmNotification,false);
#endif
  addItemInt("Day Begins",&mDayBegins,7);
  addItemInt("Working Hours Start",&mWorkingHoursStart,8);
  addItemInt("Working Hours End",&mWorkingHoursEnd,17);
  addItemBool("Exclude Holidays",&mExcludeHolidays,true);
  addItemBool("Exclude Saturdays",&mExcludeSaturdays,true);

  addItemBool("Month View Uses Category Color",&mMonthViewUsesCategoryColor,false);
  addItemBool("Full View Month",&mFullViewMonth,true);
  addItemBool("Full View Todo",&mFullViewTodo,true);
  addItemBool("Quick Todo",&mEnableQuickTodo,false);

  addItemInt("Next X Days",&mNextXDays,3);

  KPrefs::setCurrentGroup("Printer");

  KPrefs::setCurrentGroup("Layout");

  addItemBool("CompactDialogs",&mCompactDialogs,false);
  addItemBool("VerticalScreen",&mVerticalScreen,true);

  KPrefs::setCurrentGroup("KOrganizer Plugins");

  addItemStringList("SelectedPlugins",&mSelectedPlugins,"holidays");

  KPrefs::setCurrentGroup("Group Scheduling");

  addItemInt("IMIPScheduler",&mIMIPScheduler,IMIPKMail);
  addItemInt("IMIPSend",&mIMIPSend,IMIPdirectsend);
  addItemStringList("AdditionalMails",&mAdditionalMails,"");
  addItemInt("IMIP auto refresh",&mIMIPAutoRefresh,neverAuto);
  addItemInt("IMIP auto insert request",&mIMIPAutoInsertRequest,neverAuto);
  addItemInt("IMIP auto insert reply",&mIMIPAutoInsertReply,neverAuto);
  addItemInt("IMIP auto FreeBusy",&mIMIPAutoFreeBusy,neverAuto);
  addItemInt("IMIP auto save FreeBusy",&mIMIPAutoFreeBusyReply,neverAuto);

  KPrefs::setCurrentGroup( "Editors" );

  addItemStringList( "EventTemplates", &mEventTemplates );
  addItemStringList( "TodoTemplates", &mTodoTemplates );

  addItemInt("DestinationPolicy",&mDestination,standardDestination);

  KPrefs::setCurrentGroup( "ViewOptions" );
  addItemBool("EVshowDetails",&mEVshowDetails,true);
  addItemBool("EVshowCreated",&mEVshowCreated,true);
  addItemBool("EVshowChanged",&mEVshowChanged,true);
  addItemBool("WTshowDetails",&mWTshowDetails,false);
  addItemBool("WTshowCreated",&mWTshowCreated,false);
  addItemBool("WTshowChanged",&mWTshowChanged,false);
  mCalendars.setAutoDelete( true ); 
}


KOPrefs::~KOPrefs()
{
  if (mInstance == this)
      mInstance = insd.setObject(0); 
  mCalendars.setAutoDelete( true );
  mCalendars.clear();
  //qDebug("KOPrefs::~KOPrefs() ");
}


KOPrefs *KOPrefs::instance()
{
  if (!mInstance) {
      mInstance = insd.setObject(new KOPrefs());
      mInstance->readConfig();
  }

  return mInstance;
}

void KOPrefs::usrSetDefaults()
{
   
}

void KOPrefs::fillMailDefaults()
{
  if (mName.isEmpty()) mName = i18n("Anonymous");
  if (mEmail.isEmpty()) mEmail = i18n("nobody@nowhere");
}

void KOPrefs::setTimeZoneIdDefault()
{
    ;
}

void KOPrefs::setAllDefaults()
{
    setCategoryDefaults();
    mEventSummaryUser  = getDefaultList() ;
    mTodoSummaryUser  = getDefaultList() ;
    mJournalSummaryUser  = getDefaultList() ;
    mLocationDefaults  = getLocationDefaultList();
}

void KOPrefs::setCategoryDefaults()
{
  mCustomCategories.clear();
  mCustomCategories = getDefaultList();

  QStringList::Iterator it;
  for (it = mCustomCategories.begin();it != mCustomCategories.end();++it ) {
    setCategoryColor(*it,mDefaultCategoryColor);
  }
}
QStringList KOPrefs::getLocationDefaultList()
{
 QStringList retval ;
 retval << i18n("Home") << i18n("Office") << i18n("Library") << i18n("School") << i18n("Doctor") << i18n("Beach") 
        << i18n("University") << i18n("Restaurant")  << i18n("Bar") << i18n("Conference room") 
        << i18n("Cinema")  << i18n("Lake") << i18n("Kindergarten")
        << i18n("Germany") << i18n("Sweden") << i18n("Forest") << i18n("Desert") << i18n("Kitchen") ;
 // << i18n("") << i18n("") << i18n("") << i18n("") << i18n("") << i18n("") << i18n("") << i18n("")

    retval.sort();
    return retval; 
}
QStringList KOPrefs::getDefaultList()
{
    QStringList retval ;
    retval << i18n("Anniversary") << i18n("Appointment") << i18n("Birthday") << i18n("Business")  << i18n("Customer")
           << i18n("Break")
           << i18n("Family")  << i18n("Favorites") << i18n("Fishing") << i18n("Gifts")  
           << i18n("Holiday") << i18n("Holiday Cards")<< i18n("Hot Contacts")  
                     << i18n("Meeting") << i18n("Miscellaneous") << i18n("Partner") << i18n("Personal") 
           << i18n("PHB") << i18n("Phone Calls")   << i18n("Shopping")
            << i18n("Sports") << i18n("Talk") << i18n("Travel") << i18n("TV") 
           << i18n("Vacation")  ;
    retval.sort();
    //qDebug("cat %s ", retval.join("-").latin1());
    return retval; 
}
// << i18n("Business Travel") << i18n("Education") << i18n("Hiking") << i18n("Hunting")  << i18n("Recurring") << i18n("Personal Travel")  << i18n("Speach")   << i18n("Festival") << i18n("Competition")<< i18n("Party")<< i18n("Projects")<< i18n("Kids") << i18n("Special Occasion")<< i18n("Breakfast")<<  i18n("Dinner") << i18n("Lunch")<< i18n("University")<< i18n("School")<< i18n("Flight")<< i18n("Key Customer") << i18n("VIP") << i18n("SyncEvent")  << i18n("Cinema") 
void KOPrefs::usrReadConfig()
{
    config()->setGroup("General");
    
    //qDebug("KOPrefs::usrReadConfig() ");
    mCustomCategories = config()->readListEntry("Custom Categories");
    mOldLoadedLanguage = mOldLanguage ;
    mOldLanguage = KPimGlobalPrefs::instance()->mPreferredLanguage;
    if (mLocationDefaults.isEmpty()) {
        mLocationDefaults  = getLocationDefaultList();
    }
       
    if (mEventSummaryUser.isEmpty()) {
        mEventSummaryUser  = getDefaultList() ;
    }
    if (mTodoSummaryUser.isEmpty()) {
        mTodoSummaryUser  = getDefaultList() ;
    }
  
    if (mCustomCategories.isEmpty()) setCategoryDefaults();

    config()->setGroup("Personal Settings");
    mName = config()->readEntry("user_name","");
    mEmail = config()->readEntry("user_email","");
    fillMailDefaults();

    config()->setGroup("Category Colors");
    QStringList::Iterator it;
    for (it = mCustomCategories.begin();it != mCustomCategories.end();++it ) {
        setCategoryColor(*it,config()->readColorEntry(*it,&mDefaultCategoryColor));
   
    }
    KConfig fc (locateLocal("config","kopicalendarrc"));
    fc.setGroup("CC");
    int numCals = fc.readNumEntry("NumberCalendars",0 );
    mNextAvailableCalendar = 1;
    if ( numCals == 0 ) {
        KopiCalendarFile *kkf = getNewCalendar();
        kkf->isStandard = true;
        kkf->mName = i18n("Standard");
        kkf->mFileName = QDir::convertSeparators( locateLocal( "data", "korganizer/mycalendar.ics" ) );
        kkf->isReadOnly = false;
    } 
    while ( mNextAvailableCalendar  <= numCals ) {
        //qDebug("Read cal #%d ",  mNextAvailableCalendar );
        QString prefix = "Cal_" +QString::number( mNextAvailableCalendar );
        KopiCalendarFile *kkf = getNewCalendar();
        kkf->isStandard = fc.readBoolEntry( prefix+"_isStandard", false );
        kkf->isEnabled = fc.readBoolEntry( prefix+"_isEnabled", true);
        kkf->isRelative = fc.readBoolEntry( prefix+"_isRelative", false );
        kkf->isAlarmEnabled = fc.readBoolEntry( prefix+"_isAlarmEnabled", true);
        kkf->isReadOnly = fc.readBoolEntry( prefix+"_isReadOnly", false);
        kkf->mName =  fc.readEntry( prefix+"_Name", "Calendar");
        kkf->mFileName = QDir::convertSeparators( fc.readEntry( prefix+"_FileName", kkf->mFileName) );
        kkf->mSavedFileName = QDir::convertSeparators( fc.readEntry( prefix+"_SavedFileName", kkf->mFileName) );
        kkf->mDefaultColor = fc.readColorEntry( prefix+"_Color",&mEventColor);
        if ( kkf->mCalNumber == 1 ) {
            kkf->mFileName = locateLocal( "data", "korganizer/mycalendar.ics" );
        }
        //qDebug("NAME %s %s", kkf->mName.latin1(), i18n("Birthdays").latin1() );
        if ( kkf->mName == i18n("Birthdays") ) {
            kkf->mFileName = locateLocal( "data", "korganizer/birthdays.ics" ); 
        }
        if ( kkf->isRelative )
            kkf->mFileName = QDir::convertSeparators( KGlobalSettings::calendarDir() + kkf->mSavedFileName );
    }

    KPimPrefs::usrReadConfig();
}

KopiCalendarFile * KOPrefs::getCalendar( int num )
{
    return mDefCalColors[num-1];
}

KopiCalendarFile * KOPrefs::getNewCalendar()
{
    KopiCalendarFile * kkf = new KopiCalendarFile();
    kkf->mCalNumber = mNextAvailableCalendar;
    mDefCalColors.resize( mNextAvailableCalendar );
    mDefCalColors[mNextAvailableCalendar-1] = kkf;
    ++mNextAvailableCalendar;
    kkf->mDefaultColor = mEventColor;
    kkf->mName = i18n("New Calendar");
    mCalendars.append( kkf );
    return kkf;
}
void  KOPrefs::deleteCalendar( int num )
{
    KopiCalendarFile * kkf = mCalendars.first();
    while ( kkf ) {
        if ( kkf->mCalNumber == num ) {
            qDebug("KOPrefs::deleteCalendar %d ", num );
            mCalendars.remove( kkf );
            delete kkf;
            return;
        }
        kkf = mCalendars.next();
    }
}
int KOPrefs::getCalendarID( const QString & name )
{
    KopiCalendarFile * kkf = mCalendars.first();
    while ( kkf ) {
        if ( name == kkf->mName)
            return  kkf->mCalNumber;
        kkf = mCalendars.next();
    }
    return 1;
}
int KOPrefs::getFuzzyCalendarID( const QString & name )
{
    KopiCalendarFile * kkf = mCalendars.first();
    while ( kkf ) {
        if ( name.lower() == kkf->mName.lower())
            return  kkf->mCalNumber;
        kkf = mCalendars.next();
    }
    QString name2 = name;
    name2.replace (QRegExp ("_")," " );
    kkf = mCalendars.first();
    while ( kkf ) {
        if ( name2.lower() == kkf->mName.lower())
            return  kkf->mCalNumber;
        kkf = mCalendars.next();
    }
    return 0;
}
QString KOPrefs::calName( int calNum) const
{
    return (mDefCalColors[calNum-1])->mName;
}
QColor  KOPrefs::defaultColor( int calNum ) const
{
    if ( calNum == 1 ) return mEventColor;
    return (mDefCalColors[calNum-1])->mDefaultColor;
}
void KOPrefs::usrWriteConfig()
{
  config()->setGroup("General");
  config()->writeEntry("Custom Categories",mCustomCategories); 
  
  config()->setGroup("Personal Settings");
  config()->writeEntry("user_name",mName);
  config()->writeEntry("user_email",mEmail);

  config()->setGroup("Category Colors");
  QDictIterator<QColor> it(mCategoryColors);
  while (it.current()) {
    config()->writeEntry(it.currentKey(),*(it.current()));
    ++it;
  } 
  KConfig fc (locateLocal("config","kopicalendarrc")); 
  fc.setGroup("CC");
  fc.deleteGroup( "CC");
  fc.setGroup("CC");
  fc.writeEntry("NumberCalendars",mCalendars.count());
    int numCal = 1;
    int writeCal = 0;
    while ( numCal < mNextAvailableCalendar  ) {
        KopiCalendarFile * kkf = mCalendars.first();
        while ( kkf ) {
            //qDebug("cal num %d %d ", kkf->mCalNumber, numCal);
            if ( kkf->mCalNumber == numCal ) {
                ++writeCal;
                //qDebug("Write calendar %d %d ", numCal , writeCal);
                QString prefix = "Cal_" + QString::number( writeCal );
                fc.writeEntry( prefix+"_isStandard", kkf->isStandard );
                fc.writeEntry( prefix+"_isEnabled", kkf->isEnabled );
                fc.writeEntry( prefix+"_isAlarmEnabled", kkf->isAlarmEnabled );
                fc.writeEntry( prefix+"_isReadOnly", kkf->isReadOnly );
                fc.writeEntry( prefix+"_isRelative", kkf->isRelative );
                fc.writeEntry( prefix+"_Name", kkf->mName);
                fc.writeEntry( prefix+"_FileName", kkf->mFileName);
                fc.writeEntry( prefix+"_SavedFileName", kkf->mSavedFileName);
                fc.writeEntry( prefix+"_Color",kkf->mDefaultColor);
            }
            kkf = mCalendars.next();
        }
        ++numCal;
    }
    fc.sync();
  KPimPrefs::usrWriteConfig();
}

void KOPrefs::setCategoryColor(QString cat,const QColor & color)
{
  mCategoryColors.replace(cat,new QColor(color));
}

QColor *KOPrefs::categoryColor(QString cat)
{
  QColor *color = 0;

  if (!cat.isEmpty()) color = mCategoryColors[cat];

  if (color) return color;
  else return &mDefaultCategoryColor;
}

void KOPrefs::setFullName(const QString &name)
{
  mName = name;
}

void KOPrefs::setEmail(const QString &email)
{
    //qDebug(" KOPrefs::setEmai*********** %s",email.latin1() );
  mEmail = email;
}

QString KOPrefs::fullName()
{
  if (mEmailControlCenter) {
    KEMailSettings settings;
    return settings.getSetting(KEMailSettings::RealName);
  } else {
    return mName;
  }
}

QString KOPrefs::email()
{
  if (mEmailControlCenter) {
    KEMailSettings settings;
    return settings.getSetting(KEMailSettings::EmailAddress);
  } else {
    return mEmail;
  }
}
KConfig* KOPrefs::getConfig()
{
    return config();
}
