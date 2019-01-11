/*
    This file is part of KAddressBook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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
#include <stdlib.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qfile.h>
#include <qvbox.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klistview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kio/kfile/kurlrequester.h>

#include "pwmprefs.h"

#include "pwmconfigwidget.h"
#include "pwmexception.h"

PWMConfigWidget::PWMConfigWidget(PWMPrefs *prefs, QWidget *parent, const char *name )
  : KPrefsWidget(prefs,  parent, name )
{
  QVBoxLayout *topLayout = new QVBoxLayout( this, 0,
                                            KDialog::spacingHint() );

  QTabWidget *tabWidget = new QTabWidget( this );
  topLayout->addWidget( tabWidget );

  // windowsStyle page
  //////////////////////////////////////////////////////
  QWidget *windowStylePage = new QWidget( this );
  QGridLayout *windowStyleLayout = new QGridLayout( windowStylePage, 3, 3);
  
  int i = 0;
  KPrefsWidRadios * windowStyle = addWidRadios(i18n("Window-style:") ,&(prefs->mMainViewStyle), windowStylePage);
  windowStyle->addRadio(i18n("Category on top"));
  windowStyle->addRadio(i18n("Category-list left/top")); 
  windowStyleLayout->addMultiCellWidget( (QWidget*)windowStyle->groupBox(),i,i,0,2);
   ++i;   
   
   QLabel* lab = new QLabel(i18n("Font for Password entries:"), windowStylePage);
   windowStyleLayout->addMultiCellWidget( lab,i,i,0,2);
   ++i;
    KPrefsWidFont *selEntrFont =
      addWidFont(i18n("Password"),i18n("Font:"),
                 &(prefs->mEntryFont),windowStylePage);
  windowStyleLayout->addWidget(selEntrFont->label(),i,0);
  windowStyleLayout->addWidget(selEntrFont->preview(),i,1);
  windowStyleLayout->addWidget(selEntrFont->button(),i,2);
  ++i;

 lab = new QLabel(i18n("Font for Password summary:"), windowStylePage);
   windowStyleLayout->addMultiCellWidget( lab,i,i,0,2);
   ++i;
    selEntrFont =
      addWidFont(i18n("Summary"),i18n("Font:"),
                 &(prefs->mViewFont),windowStylePage);
  windowStyleLayout->addWidget(selEntrFont->label(),i,0);
  windowStyleLayout->addWidget(selEntrFont->preview(),i,1);
  windowStyleLayout->addWidget(selEntrFont->button(),i,2);
  ++i;

  lab = new QLabel(i18n(""), windowStylePage);
   windowStyleLayout->addMultiCellWidget( lab,i,i,0,2);

  // File page
  //////////////////////////////////////////////////////
  QWidget *filePage = new QWidget( this );
  QGridLayout *fileLayout = new QGridLayout( filePage, 3, 2);
  
  i = 0;
  QLabel* kcfg_compression_label = new QLabel(i18n("Compression:"), filePage);
  fileLayout->addWidget(kcfg_compression_label,i,0);
  kcfg_compression = new QComboBox(filePage, "kcfg_compression");
  kcfg_compression->insertItem(i18n("None"));
  kcfg_compression->insertItem(i18n("gzip"));
  //US not yet supported:  kcfg_compression->insertItem(i18n("bzip2"));
  fileLayout->addWidget( kcfg_compression,i,1);
  ++i;   

  QLabel* kcfg_crypt_label = new QLabel(i18n("Encryption:"), filePage);
  fileLayout->addWidget(kcfg_crypt_label,i,0);
  kcfg_cryptAlgo = new QComboBox(filePage, "kcfg_cryptAlgo");
  kcfg_cryptAlgo->insertItem(i18n("Blowfish (128 bit)"));
#ifdef CONFIG_PWMANAGER_GCRY
  kcfg_cryptAlgo->insertItem(i18n("AES-128, Rijndael (128 bit)"));
  kcfg_cryptAlgo->insertItem(i18n("AES-192, Rijndael (192 bit)"));
  kcfg_cryptAlgo->insertItem(i18n("AES-256, Rijndael (256 bit)"));
  kcfg_cryptAlgo->insertItem(i18n("Triple-DES (168 bit)"));
  kcfg_cryptAlgo->insertItem(i18n("Twofish (256 bit)"));
  kcfg_cryptAlgo->insertItem(i18n("Twofish-128 (128 bit)"));
#endif // CONFIG_PWMANAGER_GCRY
  fileLayout->addWidget( kcfg_cryptAlgo,i,1);
  ++i;   

  QLabel* kcfg_hash_label = new QLabel(i18n("Hashing:"), filePage);
  fileLayout->addWidget(kcfg_hash_label,i,0);
  kcfg_hashAlgo = new QComboBox(filePage, "kcfg_hashAlgo");
  kcfg_hashAlgo->insertItem(i18n("SHA-160, SHA1 (160 bit)"));
#ifdef CONFIG_PWMANAGER_GCRY
  kcfg_hashAlgo->insertItem(i18n("SHA-256 (256 bit)"));
  kcfg_hashAlgo->insertItem(i18n("SHA-384 (384 bit)"));
  kcfg_hashAlgo->insertItem(i18n("SHA-512 (512 bit)"));
  kcfg_hashAlgo->insertItem(i18n("MD5 (128 bit)"));
  kcfg_hashAlgo->insertItem(i18n("RIPE-MD-160 (160 bit)"));
  kcfg_hashAlgo->insertItem(i18n("Tiger (192 bit)"));
#endif // CONFIG_PWMANAGER_GCRY
  fileLayout->addWidget( kcfg_hashAlgo,i,1);
  ++i;   

  permissionLineEdit = new QLineEdit(filePage);
  QLabel* permissionLineLabel = new QLabel(permissionLineEdit, i18n("Permissions:"), filePage);
  fileLayout->addWidget(permissionLineLabel,i,0);
  fileLayout->addWidget(permissionLineEdit,i,1);
  ++i;

  KPrefsWidBool *sb = addWidBool(i18n("Make backup before saving"),
                      &(prefs->mMakeFileBackup),filePage);
  fileLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   

  // Timeout page
  //////////////////////////////////////////////////////
  QWidget *timeoutPage = new QWidget( this );
  QGridLayout *timeoutLayout = new QGridLayout( timeoutPage, 3, 2);

  i = 0;
  pwTimeoutSpinBox = new QSpinBox( 0,600,10,timeoutPage, "pwTimeoutSpinBox" );
  QLabel* timeoutLabel = new QLabel(pwTimeoutSpinBox, i18n("<b>Password timeout</b> (timeout to hold password in memory,so you don't have to re-enter it,if you already have entered it) [set to 0 to disable]:"), timeoutPage);
  timeoutLayout->addMultiCellWidget(timeoutLabel, i, i, 0 ,1);
  ++i;
  timeoutLayout->addMultiCellWidget(pwTimeoutSpinBox,i,i,0,1);
  ++i;
  pwTimeoutSpinBox->setSuffix ( i18n(" sec") );
  lockTimeoutSpinBox = new QSpinBox( 0,600,10,timeoutPage, "lockTimeoutSpinBox" );
  QLabel* lockTimeoutLabel = new QLabel(lockTimeoutSpinBox, i18n("<b>Auto-lock timeout</b> (auto lock document after this\namount of seconds) [set to 0 to disable]:"), timeoutPage);
  timeoutLayout->addMultiCellWidget(lockTimeoutLabel,i, i, 0 ,1);
  ++i;
  timeoutLayout->addMultiCellWidget(lockTimeoutSpinBox,i,i,0,1);
  lockTimeoutSpinBox->setSuffix ( i18n(" sec") );
  ++i;

  sb = addWidBool(i18n("deep-lock on autolock"),
                      &(prefs->mAutoDeeplock),timeoutPage);
  timeoutLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   
   sb = addWidBool(i18n("Open document with passwords unlocked"),&(prefs->mUnlockOnOpen),timeoutPage);
  timeoutLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   

  // Autostart page
  //////////////////////////////////////////////////////
  QWidget *autostartPage = new QWidget( this );
  QGridLayout *autostartLayout = new QGridLayout( autostartPage, 3, 2);

  i = 0;

  autostartLineEdit = new KURLRequester(autostartPage, "autoStartLineEdit");
  QLabel* autostartLineLabel = new QLabel(autostartLineEdit, "Open this file automatically on startup:",autostartPage);
  autostartLayout->addMultiCellWidget(autostartLineLabel,i,i,0,1);
  ++i;
  autostartLayout->addMultiCellWidget(autostartLineEdit,i,i,0,1);
  ++i;

  sb = addWidBool(i18n("open deeplocked"),
                      &(prefs->mAutostartDeeplocked),autostartPage);
  autostartLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   


  // external app page
  //////////////////////////////////////////////////////
  QWidget *externalappPage = new QWidget( this );
  QGridLayout *externalappLayout = new QGridLayout( externalappPage, 3, 2);

  i = 0;

  browserLineEdit = new QLineEdit(externalappPage);
  QLabel* browserLineLabel = new QLabel(browserLineEdit, i18n("Favourite browser:"), externalappPage);
  externalappLayout->addWidget(browserLineLabel,i,0);
  externalappLayout->addWidget(browserLineEdit,i,1);
  ++i;

  xtermLineEdit = new QLineEdit(externalappPage);
  QLabel* xtermLineLabel = new QLabel(xtermLineEdit, i18n("Favourite x-terminal:"), externalappPage);
  externalappLayout->addWidget(xtermLineLabel,i,0);
  externalappLayout->addWidget(xtermLineEdit,i,1);
  ++i;

#if 0
  // miscelaneous page
  //////////////////////////////////////////////////////
  QWidget *miscPage = new QWidget( this );
  QGridLayout *miscLayout = new QGridLayout( miscPage, 3, 2);

  i = 0;

  /*US ENH: PWM/Pi has no tray and con be minimized
  sb = addWidBool(i18n("Show icon in system-tray"),&(prefs->mTray),miscPage);
  miscLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   
  */

  sb = addWidBool(i18n("Open document with passwords unlocked"),&(prefs->mUnlockOnOpen),miscPage);
  miscLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   

  /*US ENH: PWM/Pi has no tray and con be minimized
  sb = addWidBool(i18n("auto-minimize to tray on startup"),&(prefs->mAutoMinimizeOnStart),miscPage);
  miscLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   

  KPrefsWidRadios * minimizeRadio = addWidRadios(i18n("auto-lock on minimize:") ,&(prefs->mMinimizeLock), miscPage);
  minimizeRadio->addRadio(i18n("don't lock"));
  minimizeRadio->addRadio(i18n("normal lock")); 
  minimizeRadio->addRadio(i18n("deep-lock")); 
  miscLayout->addMultiCellWidget( (QWidget*)minimizeRadio->groupBox(),i,i,0,2);
   ++i;   

  sb = addWidBool(i18n("KWallet emulation"),&(prefs->mKWalletEmu),miscPage);
  miscLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   

  sb = addWidBool(i18n("Close instead Minimize into tray"),&(prefs->mClose),miscPage);
  miscLayout->addMultiCellWidget(sb->checkBox(), i,i,0,1);
  ++i;   
  */
#endif

  tabWidget->addTab( windowStylePage, i18n( "Look && feel" ) );
  tabWidget->addTab( filePage, i18n( "File" ) );
  tabWidget->addTab( timeoutPage, i18n( "Timeout" ) );
  tabWidget->addTab( autostartPage, i18n( "Autostart" ) );
  tabWidget->addTab( externalappPage, i18n( "External apps" ) );
  //tabWidget->addTab( miscPage, i18n( "Miscellaneous" ) );


  connect( permissionLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( modified() ) );
  connect( pwTimeoutSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( modified() ) );
  connect( lockTimeoutSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( modified() ) );
  connect( autostartLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( modified() ) );
  connect( browserLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( modified() ) );
  connect( xtermLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( modified() ) );

}


void PWMConfigWidget::usrReadConfig()
{
  PWMPrefs* prefs = PWMPrefs::instance();


  setFilePermissions(prefs->mFilePermissions);

  pwTimeoutSpinBox->setValue(prefs->mPwTimeout);
  lockTimeoutSpinBox->setValue(prefs->mLockTimeout);
  autostartLineEdit->setURL(prefs->mAutoStart);
  browserLineEdit->setText(prefs->mBrowserCommand);
  xtermLineEdit->setText(prefs->mXTermCommand);

  kcfg_compression->setCurrentItem(prefs->mCompression);
  kcfg_cryptAlgo->setCurrentItem(prefs->mCryptAlgo);
  kcfg_hashAlgo->setCurrentItem(prefs->mHashAlgo);
}

void PWMConfigWidget::usrWriteConfig()
{
  PWMPrefs* prefs = PWMPrefs::instance();

  prefs->mFilePermissions = getFilePermissions();

  prefs->mPwTimeout = pwTimeoutSpinBox->value();
  prefs->mLockTimeout = lockTimeoutSpinBox->value();
  prefs->mAutoStart  = autostartLineEdit->url();

  prefs->mBrowserCommand = browserLineEdit->text();
  prefs->mXTermCommand = xtermLineEdit->text();

  prefs->mCompression = kcfg_compression->currentItem();
  prefs->mCryptAlgo = kcfg_cryptAlgo->currentItem();
  prefs->mHashAlgo = kcfg_hashAlgo->currentItem();

}

int PWMConfigWidget::getFilePermissions()
{
	char octalDigits[] = "01234567";
	bool isOctal;
	QString permString(permissionLineEdit->text());
	int i, j, length = permString.length();
	if (length != 3) {
		printWarn("Wrong permission string length! Please enter "
			  "the string like the following example: 600");
		return CONF_DEFAULT_FILEPERMISSIONS;
	}
	for (i = 0; i < length; ++i) {
		isOctal = false;
		for (j = 0; j < 8; ++j) {
			if (permString.at(i) == octalDigits[j]) {
				isOctal = true;
				break;
			}
		}
		if (!isOctal) {
			printWarn("CONFIG: File-permissions: This is "
				  "not an octal number ");
			return CONF_DEFAULT_FILEPERMISSIONS;
		}
	}

	int ret = strtol(permString.latin1(), 0, 8);
	if (ret == 0) {
		/* either an error occured, or the user did really type 000 */
		printWarn("CONFIG: File-permissions: Hm, either conversion error, "
			  "or you really typed 000. 8-)");
		return CONF_DEFAULT_FILEPERMISSIONS;
	}
	return ret;
}

void PWMConfigWidget::setFilePermissions(int perm)
{
	char tmpBuf[30];
	sprintf(tmpBuf, "%o", perm);
	permissionLineEdit->setText(tmpBuf);
}



#ifndef PWM_EMBEDDED
#include "pwmconfigwidget.moc"
#endif //PWM_EMBEDDED

