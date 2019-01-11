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
#ifndef KOPREFS_H
#define KOPREFS_H


#include <libkdepim/kpimprefs.h>
#include <qdict.h>
#include <qdir.h>
#include <qobject.h>

class KConfig;
class QFont;
class QColor;
class QStringList;

#define VIEW_WN_VIEW 1
#define VIEW_NX_VIEW 2
#define VIEW_J_VIEW 3
#define VIEW_A_VIEW 4
#define VIEW_ML_VIEW 5
#define VIEW_M_VIEW 6
#define VIEW_L_VIEW 7
#define VIEW_T_VIEW 8

class KopiCalendarFile : public QObject
{
 public:
    KopiCalendarFile(  ) : QObject( )
        {
            isStandard = false;
            isEnabled = true;
            isAlarmEnabled = true;
            isReadOnly = false;
            mErrorOnLoad = false;
            isRelative = false;
            mCalNumber = 0;
            mDefaultColor = Qt::red;
            mName = "Calendar";
            mFileName = QDir::homeDirPath() + "/icalfile.ics";
            mSavedFileName = "icalfile.ics";
        }
    bool isStandard;
    bool isEnabled;
    bool isAlarmEnabled;
    bool isReadOnly;
    bool mErrorOnLoad;
    bool isRelative;
    int mCalNumber;
    QString mName;
    QString mFileName;
    QString mSavedFileName;
    QColor mDefaultColor;
    QDateTime mLoadDt;
};
class KOPrefs : public KPimPrefs
{
  public:
    enum { FormatVCalendar, FormatICalendar };
    enum { MailClientKMail, MailClientSendmail };
    enum { IMIPDummy, IMIPKMail };
    enum { IMIPOutbox, IMIPdirectsend };
    enum { neverAuto, addressbookAuto, selectedAuto };
    enum { standardDestination, askDestination };

    virtual ~KOPrefs();

    /** Get instance of KOPrefs. It is made sure that there is only one
    instance. */
    static KOPrefs *instance();

    /** Set preferences to default values */
    void usrSetDefaults();

    /** Read preferences from config file */
    void usrReadConfig();

    /** Write preferences to config file */
    void usrWriteConfig();
    void setCategoryDefaults();
    void setAllDefaults();
    KopiCalendarFile * getNewCalendar();
    KopiCalendarFile * getCalendar( int );
    void  deleteCalendar( int );
    QColor defaultColor( int ) const;
    QString calName( int ) const;
    int getCalendarID( const QString & name );
    int getFuzzyCalendarID( const QString & name );
  protected:
    void setTimeZoneIdDefault();

    /** Fill empty mail fields with default values. */
    void fillMailDefaults();

  private:
    /** Constructor disabled for public. Use instance() to create a KOPrefs
    object. */
    KOPrefs();

    static KOPrefs *mInstance;
    QStringList getDefaultList();
    QStringList getLocationDefaultList();
  public:
    // preferences data
    KConfig* getConfig();
    void setFullName(const QString &);
    QString fullName();
    void setEmail(const QString &);
    QString email();

    QString mAdditional;

    bool    mEmailControlCenter;

    bool    mBcc;
    bool    mAutoSave;
    int     mAutoSaveInterval;
    bool    mConfirm;

    bool    mEnableGroupScheduling;
    bool    mEnableProjectView;

    int     mDefaultFormat;
    int     mMailClient;

    int     mStartTime;
    int     mDefaultDuration;
    int     mAlarmTime;

    int     mWorkingHoursStart;
    int     mWorkingHoursEnd;
    bool    mExcludeHolidays;
    bool    mExcludeSaturdays;
    bool    mMarcusBainsShowSeconds;

    QFont   mTimeBarFont;
    QFont   mMonthViewFont;
    QFont   mAgendaViewFont;
    QFont   mMarcusBainsFont;
    QFont   mTimeLabelsFont;
    QFont   mTodoViewFont;
    QFont   mListViewFont;
    QFont   mDateNavigatorFont;
    QFont   mEditBoxFont;
    QFont   mJornalViewFont;
    QFont   mWhatsNextFont;
    QFont   mEventViewFont;




    QColor  mHolidayColor;
    QColor  mHighlightColor;
    QColor  mEventColor;
    QColor  mTodoDoneColor;
    QColor  mAgendaBgColor;
    QColor  mWorkingHoursColor;
    QColor  mTodoDueTodayColor;
    QColor  mTodoOverdueColor;
    QColor  mTodoRunColor;
    QColor  mMonthViewEvenColor;
    QColor  mMonthViewOddColor;
    QColor  mMonthViewHolidayColor;
    bool mMonthViewUsesDayColors;
    bool mMonthViewSatSunTog;
    bool mMonthViewWeek;
    bool mMonthViewWeekRowlayout;
    QColor  mAppColor1;
    QColor  mAppColor2;
    bool mUseAppColors;

    int     mDayBegins;
    int     mHourSize;
    int     mAllDaySize;
    bool    mShowFullMenu;
    bool    mDailyRecur;
    bool    mWeeklyRecur;
    bool    mLongAllday;
    bool    mMonthDailyRecur;
    bool    mMonthWeeklyRecur;
    bool    mMonthShowIcons;
    bool    mMonthShowTimes;
    bool    mMonthShowShort;
    bool    mEnableToolTips;
    bool    mEnableMonthScroll;
    bool    mFullViewMonth;
    bool    mMonthViewUsesCategoryColor;
    bool    mFullViewTodo;
    bool    mShowCompletedTodo;
    bool    mMarcusBainsEnabled;
    int     mNextXDays;
    int     mWhatsNextDays;
    bool    mWhatsNextTime2Lines;
    bool    mWhatsNextDisplayCancelledItems;
    int     mWhatsNextPrios;
    bool    mEnableQuickTodo;

    bool    mCompactDialogs;
    bool    mVerticalScreen;

    bool mShowIconNewTodo;
    bool mShowIconNewEvent;
    bool mShowIconSearch;
    bool mShowIconList;
    bool mShowIconDay1;
    bool mShowIconDay5;
    bool mShowIconDay6;
    bool mShowIconDay7;
    bool mShowIconMonth;
    bool mShowIconTodoview;
    bool mShowIconBackFast;
    bool mShowIconBack;
    bool mShowIconToday;
    bool mShowIconForward;
    bool mShowIconForwardFast;
    bool mShowIconWhatsThis;
    bool mShowIconWeekNum;
    bool mShowIconNextDays;
    bool mShowIconNext;
    bool mShowIconJournal;
    bool mShowIconFilter;
    bool mShowIconOnetoolbar;
    bool mShowIconNavigator;
    bool mShowIconAllday;
    bool mShowIconFilterview;
    bool mShowIconToggleFull;
    
    bool mShowIconStretch;

    bool mToolBarHor;
    bool mToolBarUp;
    bool mToolBarHorV;
    bool mToolBarUpV;
    bool mToolBarHorN;
    bool mToolBarUpN;
    bool mToolBarHorF;
    bool mToolBarUpF;
    bool mToolBarMiniIcons;

    bool mAskForQuit;
    bool mUsePassWd;
    bool mShowSyncEvents;
    bool mShowTodoInAgenda;
    bool mShowCompletedTodoInAgenda;
    bool mShowTimeInAgenda;
    bool mHideNonStartedTodos;

    bool mBlockPopupMenu;

    int mLastSyncTime;
    void setCategoryColor(QString cat,const QColor & color);
    QColor *categoryColor(QString cat);

    QString mArchiveFile;
    QString mHtmlExportFile;
    bool    mHtmlWithSave;

    QStringList mSelectedPlugins;
  
    QString mLastImportFile;
    QString mLastVcalFile;
    QString mLastSaveFile;
    QString mLastLoadFile;


    QString mDefaultAlarmFile;
    int mIMIPScheduler;
    int mIMIPSend;
    QStringList mAdditionalMails;
    int mIMIPAutoRefresh;
    int mIMIPAutoInsertReply;
    int mIMIPAutoInsertRequest;
    int mIMIPAutoFreeBusy;
    int mIMIPAutoFreeBusyReply;

    QStringList mTodoTemplates;
    QStringList mEventTemplates;

    int mDestination;


    bool mEditOnDoubleClick;
    bool mViewChangeHoldFullscreen;
    bool mViewChangeHoldNonFullscreen;
    bool mCenterOnCurrentTime;
    bool mSetTimeToDayStartAt;
    bool mHighlightCurrentDay;
    bool mUseHighlightLightColor;
    bool mListViewMonthTimespan;
    bool mWNViewShowsParents;
    bool mWNViewShowsPast;
    bool mWNViewShowLocation;
    bool mTodoViewShowsPercentage;
    bool mTodoViewUsesCatColors;
    bool mMonthViewUsesBigFont;
    bool mTodoViewUsesSmallFont;
    bool mTodoViewUsesForegroundColor;
    bool mMonthViewUsesForegroundColor;

    bool mHightlightDateTimeEdit;
    bool mShortDateInViewer;

    bool mShowDateNavigator;

    QStringList mLocationDefaults;
    QStringList mEventSummaryUser;
    QStringList mTodoSummaryUser;
    QStringList mJournalSummaryUser;

    bool mUseInternalAlarmNotification;
    int mAlarmPlayBeeps;
    int mAlarmSuspendTime;
    int mAlarmSuspendCount;
    int mAlarmBeepInterval;
    int mOldLanguage;
    int mOldLoadedLanguage;

  
    QString mActiveSyncPort;
    QString mActiveSyncIP;

    // settings for eventviewer
    bool mEVshowDetails;
    bool mEVshowCreated;
    bool mEVshowChanged;
    bool mWTshowDetails;
    bool mWTshowCreated;
    bool mWTshowChanged;

    int mCurrentDisplayedView;
    QPtrList<KopiCalendarFile> mCalendars;
    int mNextAvailableCalendar;
    bool mGlobalUpdateDisabled;


    bool mDetectConflicts;
    bool mIncludeFree;
    bool mCheckConflictsAllDayAllDay;
    bool mCheckConflictsAllDayNonAD;
    bool mCheckConflictsNonADAllDay;
    bool mCheckConflictsNonADNonAD;
    QString mFilterConflictEditItem;
    QString mFilterConflictAllItem;

  private:
    QDict<QColor> mCategoryColors;
    QArray<KopiCalendarFile*> mDefCalColors;
    QColor mDefaultCategoryColor;

    QFont mDefaultTimeBarFont;
    QFont mDefaultViewFont;
    QFont mDefaultMonthViewFont;

    QString mName;
    QString mEmail;
};

#endif
