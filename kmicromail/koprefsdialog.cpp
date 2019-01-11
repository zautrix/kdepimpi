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
#include <kdialog.h>
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

#include <klineedit.h>


#include "koprefs.h"

#include "koprefsdialog.h"
//#include <kprefswidget.h>


KOPrefsDialog::KOPrefsDialog(QWidget *parent, char *name, bool modal) :
  KPrefsDialog(KOPrefs::instance(),parent,name,true)
{
  
    setFont( KGlobalSettings::generalMaxFont() );
  setCaption( i18n("Settings - some need a restart (nr)"));
  setupGlobalTab();
  setupMainTab();
  setupMailTab();;
  setupFontsTab();
  readConfig();
#ifndef DESKTOP_VERSION
  if ( QApplication::desktop()->height() == 480  )
      hideButtons();
#endif

#if 0

  setupMainTab();
  setupLocaleTab();
  setupTimeZoneTab();
  setupTimeTab();
  setupLocaleDateTab();
  setupFontsTab();
  setupColorsTab();
  setupViewsTab();
  //setupSyncTab();
  //setupSyncAlgTab();
  //setupPrinterTab();
  //setupGroupSchedulingTab();
  //setupGroupAutomationTab();
#endif
}

#include "kpimglobalprefs.h"

KOPrefsDialog::~KOPrefsDialog()
{
}
void KOPrefsDialog::setupGlobalTab() 
{
   


} 
void KOPrefsDialog::setupMainTab() 
{
    QFrame *topFrame = addPage(i18n("General"),0,0);

  QGridLayout *topLayout = new QGridLayout(topFrame,6,2);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());


  mNameEdit = new QLineEdit(topFrame);
  mNameLabel = new QLabel(mNameEdit, i18n("Full &name:"), topFrame);
  topLayout->addWidget(mNameLabel,0,0);
  topLayout->addWidget(mNameEdit,0,1);

  mEmailEdit = new QLineEdit(topFrame);
  mEmailLabel = new QLabel(mEmailEdit, i18n("E&mail address:"),topFrame);
  topLayout->addWidget(mEmailLabel,1,0);
  topLayout->addWidget(mEmailEdit,1,1); 
  QLabel *lab = new QLabel( i18n("HINT: Separate multiple\neMail addresses by \";\""), topFrame);
  topLayout->addMultiCellWidget(lab,2,2,0,1);
  KPrefsDialogWidBool*  ttt =  addWidBool(i18n("Ignore above settings and\nuse KA/Pi \"Who am I\" instead!"),
                                    &(KOPrefs::instance()->mUseKapi),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),3,3,0,1);
}

void KOPrefsDialog::setupMailTab()
{
 QFrame *topFrame = addPage(i18n("Mail"),0,0);

  QGridLayout *topLayout = new QGridLayout(topFrame,4,2);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());

  KPrefsDialogWidBool*  ttt =  addWidBool(i18n("View mail as html"),
                 &(KOPrefs::instance()->mViewAsHtml),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),0,0,0,1);
  
  
  ttt =  addWidBool(i18n("Send mails later"),
                    &(KOPrefs::instance()->mSendLater),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),1,1,0,1);
  ttt =  addWidBool(i18n("Show \"To\" field in list view"),
                    &(KOPrefs::instance()->mShowToField),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),2,2,0,1);

  int iii =3;
  ttt =  addWidBool(i18n("Show info fields at startup"),
                    &(KOPrefs::instance()->mShowInfoStart),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),iii,iii,0,1);
  ++iii;
  ttt =  addWidBool(i18n("Show \"Subject\" info field"),
                    &(KOPrefs::instance()->mShowInfoSub),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),iii,iii,0,1);
  ++iii;
  ttt =  addWidBool(i18n("Show \"From\" info field"),
                    &(KOPrefs::instance()->mShowInfoFrom),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),iii,iii,0,1);
  ++iii;
  ttt =  addWidBool(i18n("Show \"To\" info field"),
                    &(KOPrefs::instance()->mShowInfoTo),topFrame);
  topLayout->addMultiCellWidget(ttt->checkBox(),iii,iii,0,1);
  ++iii;

  /*
  mCodecEdit = new QLineEdit(topFrame);
  topLayout->addMultiCellWidget( new QLabel(mCodecEdit, i18n("User defined codec for new mails:"), topFrame),2,2,0,1);
  topLayout->addMultiCellWidget(mCodecEdit,3,3,0,1);
  topLayout->addMultiCellWidget( new QLabel(0, i18n("Example: iso-8859-15"), topFrame),4,4,0,1);
  */
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
      addWidFont(i18n("OK"),i18n("Application(nr)"),
                 &(KOPrefs::instance()->mAppFont),topFrame);
  topLayout->addWidget(timeLabelsFont->label(),i,0);
  topLayout->addWidget(timeLabelsFont->preview(),i,1);
  topLayout->addWidget(timeLabelsFont->button(),i,2);
  ++i;


  timeLabelsFont =
      addWidFont(i18n("Dear Mr."),i18n("Compose mail:"),
                 &(KOPrefs::instance()->mComposeFont),topFrame);
  topLayout->addWidget(timeLabelsFont->label(),i,0);
  topLayout->addWidget(timeLabelsFont->preview(),i,1);
  topLayout->addWidget(timeLabelsFont->button(),i,2);
  ++i;

  KPrefsDialogWidFont *timeBarFont =
      addWidFont(i18n("Hello"),i18n("Read mail:"),
                 &(KOPrefs::instance()->mReadFont),topFrame);
  topLayout->addWidget(timeBarFont->label(),i,0);
  topLayout->addWidget(timeBarFont->preview(),i,1);
  topLayout->addWidget(timeBarFont->button(),i,2);
  ++i;

  topLayout->setColStretch(1,1);
  topLayout->setRowStretch(4,1);

}
void KOPrefsDialog::usrReadConfig()
{
 
  mNameEdit->setText(KOPrefs::instance()->mName);
  mEmailEdit->setText(KOPrefs::instance()->mEmail);
  //mCodecEdit->setText(KOPrefs::instance()->mSendCodec);
}
void KOPrefsDialog::usrWriteConfig()
{
    KOPrefs::instance()->mName = mNameEdit->text();
    KOPrefs::instance()->mEmail = mEmailEdit->text();
    //KOPrefs::instance()->mSendCodec = mCodecEdit->text();
 
    
}

#if 0
void KOPrefsDialog::setupLocaleDateTab()
{
QFrame *topFrame = addPage(i18n("Date Format"),0,0);
   QGridLayout *topLayout = new QGridLayout(topFrame,3,2);
   topLayout->setSpacing(spacingHint());
   topLayout->setMargin(marginHint());  
   int iii = 0;

   
   KPrefsWidRadios *syncPrefsGroup =
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

}

void  KOPrefsDialog::setupLocaleTab()
{
 QFrame *topFrame = addPage(i18n("Locale"),0,0);
   QGridLayout *topLayout = new QGridLayout(topFrame,4,2);
   topLayout->setSpacing(spacingHint());
   topLayout->setMargin(marginHint());  
   int iii = 0;
 KPrefsWidRadios *syncPrefsGroup =
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
  //#ifndef DESKTOP_VERSION
#if 0
  ++iii;
  sb =
           addWidBool(i18n("Quick load/save (w/o Unicode)"), 
                      &(KOPrefs::instance()->mUseQuicksave),topFrame);
       topLayout->addMultiCellWidget(sb->checkBox(), iii,iii,0,1);
#endif
}
void KOPrefsDialog::showSyncPage()
{
    showPage ( 2 ) ;

}
void KOPrefsDialog::setupSyncAlgTab()
{
#if 0
  QLabel * lab;
   QFrame *topFrame = addPage(i18n("Sync Prefs"),0,0);
   mSetupSyncAlgTab = topFrame;
   QGridLayout *topLayout = new QGridLayout(topFrame,6,2);
   topLayout->setSpacing(spacingHint());
   topLayout->setMargin(marginHint());  
   int iii = 0;  
   
   KPrefsDialogWidBool *sb =
       addWidBool(i18n("Ask for preferences before syncing"),
                  &(KOPrefs::instance()->mAskForPreferences),topFrame);
   topLayout->addMultiCellWidget(sb->checkBox(), iii,iii,0,1);

   ++iii;

   KPrefsWidRadios *syncPrefsGroup =
      addWidRadios(i18n("Sync preferences:"),&(KOPrefs::instance()->mSyncAlgoPrefs),
                   topFrame);
   syncPrefsGroup->addRadio(i18n("Take local entry on conflict"));
   syncPrefsGroup->addRadio(i18n("Take remote entry on conflict"));
   syncPrefsGroup->addRadio(i18n("Take newest entry on conflict"));
   syncPrefsGroup->addRadio(i18n("Ask for every entry on conflict"));
   syncPrefsGroup->addRadio(i18n("Force take local entry always"));
   syncPrefsGroup->addRadio(i18n("Force take remote entry always"));
   topLayout->addMultiCellWidget( syncPrefsGroup->groupBox(),iii,iii,0,1);
   ++iii;   
  sb =
       addWidBool(i18n("Show summary after syncing"),
                  &(KOPrefs::instance()->mShowSyncSummary),topFrame);
   topLayout->addMultiCellWidget(sb->checkBox(), iii,iii,0,1);

   ++iii;
#endif



}


void KOPrefsDialog::setupSyncTab()
{
#if 0
    QLabel * lab;
   QFrame *topFrame = addPage(i18n("Sync Network"),0,0);
   QGridLayout *topLayout = new QGridLayout(topFrame,6,2);
   topLayout->setSpacing(spacingHint());
   topLayout->setMargin(marginHint());
   lab = new QLabel(i18n("Remote syncing (via ssh/scp)\nnetwork settings "), topFrame);
   int iii = 0;
   topLayout->addMultiCellWidget(lab , iii,iii,0,1);
   ++iii;

   mRemoteIPEdit = new QLineEdit(topFrame);
   lab = new QLabel(mRemoteIPEdit, i18n("Remote IP:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mRemoteIPEdit,iii,1);
   ++iii;
   mRemoteUser = new QLineEdit(topFrame);
   lab = new QLabel(mRemoteUser, i18n("Remote user:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mRemoteUser, iii,1); 
   ++iii;

   mRemoteFile = new QLineEdit(topFrame);
   lab = new QLabel(mRemoteFile, i18n("Remote file:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mRemoteFile,iii,1);
   ++iii;
 
   mLocalTempFile = new QLineEdit(topFrame);
   lab = new QLabel(mLocalTempFile, i18n("Local temp file:"), topFrame);
   topLayout->addWidget(lab ,iii,0);
   topLayout->addWidget(mLocalTempFile,iii,1);
   ++iii;
 
   KPrefsDialogWidBool *wb =
       addWidBool(i18n("Write back synced file"),
                  &(KOPrefs::instance()->mWriteBackFile),topFrame);
   topLayout->addMultiCellWidget(wb->checkBox(), iii,iii,0,1);
   ++iii;
   wb =
       addWidBool(i18n("Write back existing entries only"),
                  &(KOPrefs::instance()->mWriteBackExistingOnly),topFrame);
   topLayout->addMultiCellWidget(wb->checkBox(), iii,iii,0,1);
   ++iii;
 
#endif
}

void KOPrefsDialog::setupMainTab()
{
    QFrame *topFrame = addPage(i18n("General"),0,0);
    // DesktopIcon("identity",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,6,2);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());

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
  QHBox *dummy;
  if ( QApplication::desktop()->width() >  480 ) {
      dummy = new QHBox(topFrame);
  } else {
      dummy = new QVBox(topFrame);
  }

  topLayout->addMultiCellWidget(dummy, 2,2,0,1); 
  addWidBool(i18n("Full menu bar(nr)"),
             &(KOPrefs::instance()->mShowFullMenu),dummy);
  
  
  addWidBool(i18n("Mini icons in toolbar(nr)"),
             &(KOPrefs::instance()->mToolBarMiniIcons),dummy);


  dummy = new QHBox(topFrame);
  new QLabel(i18n("Days in What's Next:"),dummy);
  mWhatsNextSpin = new QSpinBox(1,14,1,dummy);
 
  topLayout->addMultiCellWidget(dummy,3,3,0,1);



  dummy = new QHBox(topFrame);
  new QLabel(i18n("Days in Next-X-Days:"),dummy);
  mNextXDaysSpin = new QSpinBox(2,14,1,dummy);
 
  topLayout->addMultiCellWidget(dummy,4,4,0,1);

  QHBox *prioBox = new QHBox(topFrame);
  // intervalBox->setSpacing(spacingHint());
  topLayout->addMultiCellWidget(prioBox,5,5,0,1);
  QString messa = i18n("Show topmost todo prios in What's Next:");

  if ( QApplication::desktop()->width() <  300 )
      messa = i18n("Show topmost todo prios in What's N.:");
  QLabel *prioLabel = new QLabel(messa, prioBox);
  mPrioSpin = new QSpinBox(0,5,1,prioBox);
  if ( QApplication::desktop()->width() <  300 )
      mPrioSpin->setFixedWidth( 40 );

  // KPrefsDialogWidBool *bcc =
//       addWidBool(i18n("Send copy to owner when mailing events"),
//                  &(KOPrefs::instance()->mBcc),topFrame);
//   topLayout->addMultiCellWidget(bcc->checkBox(),4,4,0,1);


  // QGroupBox *autoSaveGroup = new QGroupBox(1,Horizontal,i18n("Auto-Save"),  topFrame);
  //topLayout->addMultiCellWidget(autoSaveGroup,6,6,0,1);

  // addWidBool(i18n("Enable automatic saving of calendar"),
  //         &(KOPrefs::instance()->mAutoSave),autoSaveGroup);

  QHBox *intervalBox = new QHBox(topFrame);
  // intervalBox->setSpacing(spacingHint());
  topLayout->addMultiCellWidget(intervalBox,6,6,0,1);
  QLabel *autoSaveIntervalLabel = new QLabel(i18n("Auto save delay in minutes:"),intervalBox);
  mAutoSaveIntervalSpin = new QSpinBox(0,500,1,intervalBox);
  autoSaveIntervalLabel->setBuddy(mAutoSaveIntervalSpin); 
  /*
  QHBox * agendasize = new QHBox ( topFrame );
  
  new QLabel (i18n("AllDayAgenda Height:"), agendasize );
  

  mHourSizeSlider = new QSlider(24,47,1,24,Horizontal,agendasize);
  topLayout->addMultiCellWidget(agendasize,7,7,0,1);
  */
  KPrefsDialogWidBool *verticalScreen =
      addWidBool(i18n("Show vertical screen (Needs restart)"),
                 &(KOPrefs::instance()->mVerticalScreen),topFrame);
  //topLayout->addWidget(verticalScreen->checkBox(),ii++,0);
  topLayout->addMultiCellWidget(verticalScreen->checkBox(),7,7,0,1);

  KPrefsDialogWidBool *ask =
    addWidBool(i18n("Ask for quit when closing KO/Pi"),
               &(KOPrefs::instance()->mAskForQuit),topFrame);
  topLayout->addMultiCellWidget(ask->checkBox(),8,8,0,1);
 

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

  KPrefsWidRadios *defaultFormatGroup =
      addWidRadios(i18n("Default Calendar Format"),
                   &(KOPrefs::instance()->mDefaultFormat),topFrame);
  defaultFormatGroup->addRadio(i18n("vCalendar"));
  defaultFormatGroup->addRadio(i18n("iCalendar"));

  topLayout->addMultiCellWidget(defaultFormatGroup->groupBox(),10,10,0,1);

  // Default format unconditionally is iCalendar
  defaultFormatGroup->groupBox()->hide();

  KPrefsWidRadios *mailClientGroup =
      addWidRadios(i18n("Mail Client"),&(KOPrefs::instance()->mMailClient),
                   topFrame);
  mailClientGroup->addRadio(i18n("KMail"));
  mailClientGroup->addRadio(i18n("Sendmail"));
  topLayout->addMultiCellWidget(mailClientGroup->groupBox(),11,11,0,1);

  KPrefsDialogWidBool *htmlsave =
      addWidBool(i18n("Export to HTML with every save"),&(KOPrefs::instance()->mHtmlWithSave),
                 topFrame);
  topLayout->addMultiCellWidget(htmlsave->checkBox(),12,12,0,1);

  KPrefsWidRadios *destinationGroup =
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
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());
 
  QHBox *dummy = new QHBox(topFrame);
  KPrefsWidTime *dayBegins =
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


  QGroupBox *workingHoursGroup = new QGroupBox(1,Horizontal,
                                               i18n("Working Hours"),
                                               topFrame);
  topLayout->addMultiCellWidget(workingHoursGroup,4,4,0,1);
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
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());

//   QBoxLayout *dayBeginsLayout = new QHBoxLayout;
//   topLayout->addLayout(dayBeginsLayout,0,0);

//   KPrefsWidTime *dayBegins =
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

  dummy =
    addWidBool(i18n("Highlight current day in agenda"),
               &(KOPrefs::instance()->mHighlightCurrentDay),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  dummy =
    addWidBool(i18n("Use light color for highlight current day"),
               &(KOPrefs::instance()->mUseHighlightLightColor),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  KPrefsDialogWidBool *dailyRecur =
    addWidBool(i18n("Show events that recur daily in date nav."),
               &(KOPrefs::instance()->mDailyRecur),topFrame);
  topLayout->addWidget(dailyRecur->checkBox(),ii++,0);

  KPrefsDialogWidBool *weeklyRecur =
    addWidBool(i18n("Show ev. that recur weekly in date nav."),
               &(KOPrefs::instance()->mWeeklyRecur),topFrame);
  topLayout->addWidget(weeklyRecur->checkBox(),ii++,0);
  if ( QApplication::desktop()->width() > 640 ) {
      
      KPrefsDialogWidBool *enableToolTips =
          addWidBool(i18n("Enable tooltips displaying summary of ev."),
                     &(KOPrefs::instance()->mEnableToolTips),topFrame);
      topLayout->addWidget(enableToolTips->checkBox(),ii++,0);
  
  }
  KPrefsDialogWidBool *passwdk =
    addWidBool(i18n("Show parent To-Do's in What's Next view"),
               &(KOPrefs::instance()->mWNViewShowsParents),topFrame);
   topLayout->addWidget(passwdk->checkBox(), ii++,0);
  
  passwdk =
    addWidBool(i18n("Show location in What's Next view"),
               &(KOPrefs::instance()->mWNViewShowLocation),topFrame);
   topLayout->addWidget(passwdk->checkBox(), ii++,0);
  
  passwdk =
    addWidBool(i18n("Show Sync Events in WN/Agenda view"),
               &(KOPrefs::instance()->mShowSyncEvents),topFrame);
   topLayout->addWidget(passwdk->checkBox(), ii++,0);
  
   
  KPrefsDialogWidBool *marcusBainsEnabled =
      addWidBool(i18n("Show Marcus Bains line"),
                 &(KOPrefs::instance()->mMarcusBainsEnabled),topFrame);
  topLayout->addWidget(marcusBainsEnabled->checkBox(),ii++,0);
  
 
  // topLayout->addWidget(hourSizeGroup,ii++,0);
  // topLayout->addMultiCellWidget(hourSizeGroup,ii,ii,0,0);
  //topLayout->setRowStretch(11,1);






   topFrame = addPage(i18n("ViewChange"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,6,1);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());
  ii = 0;
 

  dummy =
    addWidBool(i18n("Hold fullscreen on view change"),
               &(KOPrefs::instance()->mViewChangeHoldFullscreen),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  dummy =
    addWidBool(i18n("Hold non-fullscreen on view change"),
               &(KOPrefs::instance()->mViewChangeHoldNonFullscreen),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);


 KPrefsDialogWidBool *fullViewTodo =
      addWidBool(i18n("Event list view uses full window"),
                 &(KOPrefs::instance()->mFullViewTodo),topFrame);
  topLayout->addWidget(fullViewTodo->checkBox(),ii++,0);

  KPrefsDialogWidBool *fullViewMonth =
      addWidBool(i18n("Next days view uses full window"),
                 &(KOPrefs::instance()->mFullViewMonth),topFrame);
  topLayout->addWidget(fullViewMonth->checkBox(),ii++,0);

 dummy =
    addWidBool(i18n("Set agenda to DayBeginsAt on change"),
               &(KOPrefs::instance()->mSetTimeToDayStartAt),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

 dummy =
    addWidBool(i18n("Set agenda to current time on change"),
               &(KOPrefs::instance()->mCenterOnCurrentTime),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

  dummy =
    addWidBool(i18n("Listview uses monthly timespan"),
               &(KOPrefs::instance()->mListViewMonthTimespan),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
 dummy  =
    addWidBool(i18n("Highlight selection in Time Edit"),
               &(KOPrefs::instance()->mHightlightDateTimeEdit),topFrame);
   topLayout->addWidget( dummy->checkBox(), ii++,0);





   topFrame = addPage(i18n("Month View"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,5,1);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());
  ii = 0;
  QLabel *lab;
  QHBox *habo = new QHBox( topFrame );
  if ( QApplication::desktop()->width() < 320 ) {
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
  if ( QApplication::desktop()->width() < 320 ) {
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

  topLayout->addMultiCellWidget(habo,ii, ii,0,1);
  ii++;
#ifdef DESKTOP_VERSION
 KPrefsDialogWidBool *enableMonthScroll =
      addWidBool(i18n("Enable scrollbars in month view cells"),
                 &(KOPrefs::instance()->mEnableMonthScroll),topFrame);
  topLayout->addWidget(enableMonthScroll->checkBox(),ii++,0);
#endif

  dummy =
    addWidBool(i18n("Show Sat/Sun together"),
               &(KOPrefs::instance()->mMonthViewSatSunTog),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

 KPrefsDialogWidBool *coloredCategoriesInMonthView =
       addWidBool(i18n("Month view uses category colors"),
                  &(KOPrefs::instance()->mMonthViewUsesCategoryColor),topFrame);
  topLayout->addWidget(coloredCategoriesInMonthView->checkBox(),ii++,0); 

  dummy =
    addWidBool(i18n("Categorie colors are applied to text"),
               &(KOPrefs::instance()->mMonthViewUsesForegroundColor),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);
 coloredCategoriesInMonthView =
       addWidBool(i18n("Month view uses day colors"),
                  &(KOPrefs::instance()->mMonthViewUsesDayColors),topFrame);
  topLayout->addWidget(coloredCategoriesInMonthView->checkBox(),ii++,0);

 KPrefsWidColor *holidayColor =
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

  // *********************** Todo View

   topFrame = addPage(i18n("Todo View"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,4,1);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());
  ii = 0;

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
  // Todo due today color
  KPrefsWidColor *todoDueTodayColor =
      addWidColor(i18n("Todo due today color:"),
                  &(KOPrefs::instance()->mTodoDueTodayColor),wid);
  QHBoxLayout *widLayout = new QHBoxLayout(wid);
  widLayout->addWidget( todoDueTodayColor->label() );
  widLayout->addWidget( todoDueTodayColor->button() );
  topLayout->addWidget(wid,ii++,0);
  //topLayout->addWidget(todoDueTodayColor->button(),ii++,1);

  // Todo overdue color
  wid = new QWidget( topFrame );
  widLayout = new QHBoxLayout(wid);
  KPrefsWidColor *todoOverdueColor =
      addWidColor(i18n("Todo overdue color:"),
                  &(KOPrefs::instance()->mTodoOverdueColor),wid);
  widLayout->addWidget(todoOverdueColor->label());
  widLayout->addWidget(todoOverdueColor->button());
  topLayout->addWidget(wid,ii++,0);

 dummy =
    addWidBool(i18n("Colors are applied to text"),
               &(KOPrefs::instance()->mTodoViewUsesForegroundColor),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);

 dummy =
    addWidBool(i18n("Allday Agenda view shows todos"),
               &(KOPrefs::instance()->mShowTodoInAgenda),topFrame);
  topLayout->addWidget(dummy->checkBox(),ii++,0);




   topFrame = addPage(i18n("Alarm"),0,0);
    //  DesktopIcon("viewmag",KIcon::SizeMedium));

  topLayout = new QGridLayout(topFrame,2,1);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());
  int iii = 0;

 dummy =
    addWidBool(i18n("Use internal alarm notification"),
               &(KOPrefs::instance()->mUseInternalAlarmNotification),topFrame);
  topLayout->addWidget(dummy->checkBox(),iii++,0);
  lab = new QLabel( i18n("Note: KO/Pi must be running to notify you about an alarm. Recommended for use on Zaurus: Disable this option and install KO/Pi alarm applet.\n"), topFrame);

   topLayout->addWidget(lab ,iii++,0);
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
  // topLayout->setSpacing(spacingHint());
  // topLayout->setMargin(marginHint());

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

  KPrefsWidColor *holidayColor =
      addWidColor(i18n("Holiday color:"),
                  &(KOPrefs::instance()->mHolidayColor),topFrame);
  topLayout->addWidget(holidayColor->label(),ii,0);
  topLayout->addWidget(holidayColor->button(),ii++,1);

  // Highlight Color
  KPrefsWidColor *highlightColor =
      addWidColor(i18n("Highlight color:"),
                  &(KOPrefs::instance()->mHighlightColor),topFrame);
  topLayout->addWidget(highlightColor->label(),ii,0);
  topLayout->addWidget(highlightColor->button(),ii++,1);

  // Event color
  KPrefsWidColor *eventColor =
      addWidColor(i18n("Default event color:"),
                  &(KOPrefs::instance()->mEventColor),topFrame);
  topLayout->addWidget(eventColor->label(),ii,0);
  topLayout->addWidget(eventColor->button(),ii++,1);

  // agenda view background color
  KPrefsWidColor *agendaBgColor =
      addWidColor(i18n("Agenda view background color:"),
                  &(KOPrefs::instance()->mAgendaBgColor),topFrame);
  topLayout->addWidget(agendaBgColor->label(),ii,0);
  topLayout->addWidget(agendaBgColor->button(),ii++,1);

  // working hours color
  KPrefsWidColor *workingHoursColor =
      addWidColor(i18n("Working hours color:"),
                  &(KOPrefs::instance()->mWorkingHoursColor),topFrame);
  topLayout->addWidget(workingHoursColor->label(),ii,0);
  topLayout->addWidget(workingHoursColor->button(),ii++,1);

  KPrefsDialogWidBool *sb =
           addWidBool(i18n("Use colors for application:"),
                      &(KOPrefs::instance()->mUseAppColors),topFrame);
       topLayout->addMultiCellWidget(sb->checkBox(), ii, ii, 0,1 );

       ii++;
    KPrefsWidColor *   workingHoursColor1 =
           addWidColor(i18n("Buttons, menus, etc.:"),
                       &(KOPrefs::instance()->mAppColor1),topFrame);
       topLayout->addWidget(workingHoursColor1->label(),ii,0);
       topLayout->addWidget(workingHoursColor1->button(),ii++,1);
       
      KPrefsWidColor *   workingHoursColor2 =
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
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());

  topLayout->setRowStretch(4,1);
}

void KOPrefsDialog::setupGroupSchedulingTab()
{
#if 0
  QFrame *topFrame  = addPage(i18n("Group Scheduling"),0,
                              DesktopIcon("personal",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,6,2);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());

#if 0
  KPrefsWidRadios *schedulerGroup =
      addWidRadios(i18n("Scheduler Mail Client"),&(KOPrefs::instance()->mIMIPScheduler),
                   topFrame);
  schedulerGroup->addRadio("Dummy"); // Only for debugging
  schedulerGroup->addRadio(i18n("Mail client"));

  topLayout->addMultiCellWidget(schedulerGroup->groupBox(),0,0,0,1);
#endif

  KPrefsWidRadios *sendGroup =
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
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());

  KPrefsWidRadios *autoRefreshGroup =
      addWidRadios(i18n("Auto Send Refresh"),
                   &(KOPrefs::instance()->mIMIPAutoRefresh),topFrame);
  autoRefreshGroup->addRadio(i18n("Never"));
  autoRefreshGroup->addRadio(i18n("If attendee is in addressbook"));
  //autoRefreshGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoRefreshGroup->groupBox(),0,0,0,0);

  KPrefsWidRadios *autoInsertGroup =
      addWidRadios(i18n("Auto Insert IMIP Replies"),
                   &(KOPrefs::instance()->mIMIPAutoInsertReply),topFrame);
  autoInsertGroup->addRadio(i18n("Never"));
  autoInsertGroup->addRadio(i18n("If attendee is in addressbook"));
  //autoInsertGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoInsertGroup->groupBox(),1,1,0,0);

  KPrefsWidRadios *autoRequestGroup =
      addWidRadios(i18n("Auto Insert IMIP Requests"),
                   &(KOPrefs::instance()->mIMIPAutoInsertRequest),topFrame);
  autoRequestGroup->addRadio(i18n("Never"));
  autoRequestGroup->addRadio(i18n("If organizer is in addressbook"));
  //autoInsertGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoRequestGroup->groupBox(),2,2,0,0);

  KPrefsWidRadios *autoFreeBusyGroup =
      addWidRadios(i18n("Auto Send FreeBusy Information"),
                   &(KOPrefs::instance()->mIMIPAutoFreeBusy),topFrame);
  autoFreeBusyGroup->addRadio(i18n("Never"));
  autoFreeBusyGroup->addRadio(i18n("If requested from an email in addressbook"));
  //autoFreeBusyGroup->addRadio(i18n("selected emails"));
  topLayout->addMultiCellWidget(autoFreeBusyGroup->groupBox(),3,3,0,0);

  KPrefsWidRadios *autoFreeBusyReplyGroup =
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
  QDate current ( 2001, 1,1);
  mStartDateSavingEdit->setDate(current.addDays(KOPrefs::instance()->mDaylightsavingStart-1));
  mEndDateSavingEdit->setDate(current.addDays(KOPrefs::instance()->mDaylightsavingEnd-1));
  setCombo(mTimeZoneCombo,i18n(KOPrefs::instance()->mTimeZoneId));
  //mTimezoneOffsetSpin->setValue( KOPrefs::instance()->mTimeZoneOffset);
  mStartTimeSpin->setValue(KOPrefs::instance()->mStartTime);
  mDefaultDurationSpin->setValue(KOPrefs::instance()->mDefaultDuration);
  mAlarmTimeCombo->setCurrentItem(KOPrefs::instance()->mAlarmTime);
  // if  (KOPrefs::instance()->mAllDaySize > 47  )
  //   KOPrefs::instance()->mAllDaySize = KOPrefs::instance()->mAllDaySize/2;
  //mHourSizeSlider->setValue(KOPrefs::instance()->mAllDaySize);

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

  // mRemoteIPEdit->setText(KOPrefs::instance()->mRemoteIP);
  //mRemoteUser->setText(KOPrefs::instance()->mRemoteUser);
  //mRemotePassWd->setText(KOPrefs::instance()->mRemotePassWd);
  //mRemoteFile->setText(KOPrefs::instance()->mRemoteFile);

  //that soundmLocalTempFile->setText(KOPrefs::instance()->mLocalTempFile);
  mDefaultAlarmFile->setText(KOPrefs::instance()->mDefaultAlarmFile);
  QString dummy = KOPrefs::instance()->mUserDateFormatLong;
  mUserDateFormatLong->setText(dummy.replace( QRegExp("K"), QString(",") ));
  dummy = KOPrefs::instance()->mUserDateFormatShort;
  mUserDateFormatShort->setText(dummy.replace( QRegExp("K"), QString(",") ));
  updateCategories();
  mAlarmPlayBeeps->setValue(KOPrefs::instance()->mAlarmPlayBeeps );
  mAlarmSuspendTime->setValue(KOPrefs::instance()->mAlarmSuspendTime );
  mAlarmSuspendCount->setValue(KOPrefs::instance()->mAlarmSuspendCount );
  mAlarmBeepInterval->setValue(KOPrefs::instance()->mAlarmBeepInterval );
}


void KOPrefsDialog::usrWriteConfig()
{

    // KOPrefs::instance()->mRemoteIP = mRemoteIPEdit->text();
    //KOPrefs::instance()->mRemoteUser =  mRemoteUser->text();
    //KOPrefs::instance()->mRemotePassWd = mRemotePassWd->text();
    //KOPrefs::instance()->mRemoteFile= mRemoteFile->text();
    //KOPrefs::instance()->mLocalTempFile =mLocalTempFile->text();
    KOPrefs::instance()->mDefaultAlarmFile =mDefaultAlarmFile->text();

    KOPrefs::instance()->mUserDateFormatShort = mUserDateFormatShort->text().replace( QRegExp(","), QString("K") );
    KOPrefs::instance()->mUserDateFormatLong = mUserDateFormatLong->text().replace( QRegExp(","), QString("K") );
    KOPrefs::instance()->setFullName(mNameEdit->text());
    KOPrefs::instance()->setEmail(mEmailEdit->text());
    
    KOPrefs::instance()->mAutoSaveInterval = mAutoSaveIntervalSpin->value();
    
    KOPrefs::instance()->mTimeZoneId = mTimeZoneCombo->currentText();
    QDate date;
    date = mStartDateSavingEdit->date();
    int sub = 0;
    if ( QDate::leapYear( date.year() ) && date.dayOfYear() > 59 )
        sub = 1;
    KOPrefs::instance()->mDaylightsavingStart = date.dayOfYear()-sub;
    date = mEndDateSavingEdit->date();
    if ( QDate::leapYear( date.year() ) && date.dayOfYear() > 59  )
        sub = 1;
    else 
        sub = 0;
    KOPrefs::instance()->mDaylightsavingEnd = date.dayOfYear()-sub;
    // KOPrefs::instance()->mTimeZoneOffset = mTimezoneOffsetSpin->value();
    
    KOPrefs::instance()->mStartTime = mStartTimeSpin->value();
    KOPrefs::instance()->mDefaultDuration = mDefaultDurationSpin->value();
    KOPrefs::instance()->mAlarmTime = mAlarmTimeCombo->currentItem();
    
    //KOPrefs::instance()->mAllDaySize = mHourSizeSlider->value();
    
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
    
}

void KOPrefsDialog::updateCategories()
{
  mCategoryCombo->clear();
  mCategoryDict.clear();
  mCategoryCombo->insertStringList(KOPrefs::instance()->mCustomCategories);
  updateCategoryColor();
}

void KOPrefsDialog::warningGroupScheduling()
{
  warningExperimental(mEnableGroupScheduling->checkBox()->isChecked());
}

void KOPrefsDialog::warningProjectView()
{
  warningExperimental(mEnableProjectView->checkBox()->isChecked());
}

void KOPrefsDialog::warningExperimental(bool on)
{
  if (on) {
    KMessageBox::information(this,i18n("This is an experimental feature. "
        "It may not work, it may do nothing useful and it may cause data loss. "
        "Use with care.\n"
        "You have to restart KOrganizer for this setting to take effect."));
  } else {
    KMessageBox::information(this,
        i18n("You have to restart KOrganizer for this setting to take effect."));
  }
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
void KOPrefsDialog::updateTimezoneOffset( int index )
{
    /*
    qDebug("updateTimezoneOffset %d ", index);
    if ( index < 24 ) {
        mTimezoneOffsetSpin->setEnabled ( false );
        mTimezoneOffsetSpin->setValue( ( index-11 ) * 60 );


    } else { 
        if  ( index ==  24 ) {
            mTimezoneOffsetSpin->setEnabled ( true );
            mTimezoneOffsetSpin->setValue( KOPrefs::instance()->mTimeZoneOffset);
            
        } else {
            mTimezoneOffsetSpin->setEnabled ( false );
            mTimezoneOffsetSpin->setValue( 0 );
        }
    }
    */
}

void KOPrefsDialog::setupTimeZoneTab()
{
    QFrame *topFrame = addPage(i18n("Time Zone"),0,0);
    // DesktopIcon("clock",KIcon::SizeMedium));

  QGridLayout *topLayout = new QGridLayout(topFrame,5,2);
  topLayout->setSpacing(spacingHint());
  topLayout->setMargin(marginHint());

  QHBox *timeZoneBox = new QHBox( topFrame );
  topLayout->addMultiCellWidget( timeZoneBox, 0, 0, 0, 1 );

  new QLabel( i18n("Timezone:"), timeZoneBox );
  mTimeZoneCombo = new QComboBox( timeZoneBox ); 
  if ( QApplication::desktop()->width() <  300 ) {
       mTimeZoneCombo->setMaximumWidth(150);
    }

  QStringList list;
  list = KGlobal::locale()->timeZoneList();
  mTimeZoneCombo->insertStringList(list);

    // find the currently set time zone and select it
  QString sCurrentlySet = KOPrefs::instance()->mTimeZoneId;
  int nCurrentlySet = 11;
  for (int i = 0; i < mTimeZoneCombo->count(); i++)
    {
      if (mTimeZoneCombo->text(i) == sCurrentlySet)
        {
         nCurrentlySet = i;
         break;
        }
    }
  mTimeZoneCombo->setCurrentItem(nCurrentlySet);
  int iii = 1;
  KPrefsDialogWidBool *sb =
      addWidBool(i18n("Timezone has daylight saving"),
                 &(KOPrefs::instance()->mUseDaylightsaving),topFrame);
  topLayout->addMultiCellWidget(sb->checkBox(), iii,iii,0,1);
  ++iii;  
  QLabel* lab = new QLabel(  i18n("Actual start and end is the\nsunday before this date."), topFrame );
  topLayout->addMultiCellWidget(lab, iii,iii,0,1);
  ++iii;   
  lab = new QLabel(  i18n("The year in the date is ignored."), topFrame );
  topLayout->addMultiCellWidget(lab, iii,iii,0,1);
  ++iii; 
  lab = new QLabel(  i18n("Daylight start:"), topFrame );
  topLayout->addWidget(lab, iii,0);
  mStartDateSavingEdit = new KDateEdit(topFrame);
  topLayout->addWidget(mStartDateSavingEdit, iii,1);
  ++iii;

  lab = new QLabel(  i18n("Daylight end:"), topFrame );
  topLayout->addWidget(lab, iii,0);
  mEndDateSavingEdit = new KDateEdit(topFrame);
  topLayout->addWidget(mEndDateSavingEdit, iii,1);
  ++iii;
  QDate current ( 2001, 1,1);
  mStartDateSavingEdit->setDate(current.addDays(KOPrefs::instance()->mDaylightsavingStart-1));
  mEndDateSavingEdit->setDate(current.addDays(KOPrefs::instance()->mDaylightsavingEnd-1));

 
}
#endif
