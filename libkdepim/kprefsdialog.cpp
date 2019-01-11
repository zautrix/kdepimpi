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

// $Id: kprefsdialog.cpp,v 1.6 2010/01/25 22:45:26 zautrix Exp $

#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else
#include <qbuttongroup.h>
#endif
#include <qvbox.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qfont.h>
#include <qslider.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qspinbox.h>
#include <qdatetime.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include <kcolorbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kfontdialog.h>
#include <kmessagebox.h>
#include <kcolordialog.h>
#include <kiconloader.h>

#include "kprefs.h"
#include "kpimglobalprefs.h"

#include "kprefsdialog.h"
//#include "kprefsdialog.moc"

KPrefsDialogWidBool::KPrefsDialogWidBool(const QString &text,bool *reference,
                             QWidget *parent)
{
  mReference = reference;

  mCheck = new QCheckBox(text,parent);
}

void KPrefsDialogWidBool::readConfig()
{
  mCheck->setChecked(*mReference);
}

void KPrefsDialogWidBool::writeConfig()
{
  *mReference = mCheck->isChecked();
}

QCheckBox *KPrefsDialogWidBool::checkBox()
{
  return mCheck;
}


KPrefsDialogWidColor::KPrefsDialogWidColor(const QString &text,QColor *reference,
                               QWidget *parent)
{
  mReference = reference;

  mButton = new KColorButton(parent);
  mLabel = new QLabel(mButton, text, parent);
  mButton->setColor( *mReference );
  mButton->setColor( Qt::red );

}

KPrefsDialogWidColor::~KPrefsDialogWidColor()
{
//  kdDebug(5300) << "KPrefsDialogWidColor::~KPrefsDialogWidColor()" << endl;
}

void KPrefsDialogWidColor::readConfig()
{
  mButton->setColor(*mReference);
}

void KPrefsDialogWidColor::writeConfig()
{
  *mReference = mButton->color();
}

QLabel *KPrefsDialogWidColor::label()
{
  return mLabel;
}

KColorButton *KPrefsDialogWidColor::button()
{
  return mButton;
}

KPrefsDialogWidFont::KPrefsDialogWidFont(const QString &sampleText,const QString &labelText,
                             QFont *reference,QWidget *parent)
{
  mReference = reference;

  mLabel = new QLabel(labelText, parent);

  mPreview = new QLabel(sampleText,parent);
  mPreview->setFrameStyle(QFrame::Panel|QFrame::Sunken);

  mButton = new QPushButton(i18n("Choose..."), parent);
  connect(mButton,SIGNAL(clicked()),SLOT(selectFont())); 
  mPreview->setMaximumHeight( QApplication::desktop()->height() / 12 );
  mPreview->setMaximumWidth( (QApplication::desktop()->width() / 2)-10 );
}

KPrefsDialogWidFont::~KPrefsDialogWidFont()
{
}

void KPrefsDialogWidFont::readConfig()
{
  mPreview->setFont(*mReference);
}

void KPrefsDialogWidFont::writeConfig()
{
  *mReference = mPreview->font();
}

QLabel *KPrefsDialogWidFont::label()
{
  return mLabel;
}

QLabel *KPrefsDialogWidFont::preview()
{
  return mPreview;
}

QPushButton *KPrefsDialogWidFont::button()
{
  return mButton;
}

void KPrefsDialogWidFont::selectFont()
{
  QFont myFont(mPreview->font());
  bool ok;
  myFont = KFontDialog::getFont(myFont, ok);
  if ( ok ) {
    mPreview->setFont(myFont);
  }
}


KPrefsDialogWidTime::KPrefsDialogWidTime(const QString &text,int *reference,
                             QWidget *parent)
{
  mReference = reference;

  mLabel = new QLabel(text,parent);
  mSpin = new QSpinBox(0,23,1,parent);
  mSpin->setSuffix(":00");
}

void KPrefsDialogWidTime::readConfig()
{
  mSpin->setValue(*mReference);
}

void KPrefsDialogWidTime::writeConfig()
{
  *mReference = mSpin->value();
}

QLabel *KPrefsDialogWidTime::label()
{
  return mLabel;
}

QSpinBox *KPrefsDialogWidTime::spinBox()
{
  return mSpin;
}


KPrefsDialogWidRadios::KPrefsDialogWidRadios(const QString &text,int *reference,
                QWidget *parent)
{
  mReference = reference;

  mBox = new QButtonGroup(1,Qt::Horizontal,text,parent);
}

KPrefsDialogWidRadios::~KPrefsDialogWidRadios()
{
}

void KPrefsDialogWidRadios::addRadio(const QString &text)
{
  new QRadioButton(text,mBox);
}

QButtonGroup *KPrefsDialogWidRadios::groupBox()
{
  return mBox;
}

void KPrefsDialogWidRadios::readConfig()
{
  mBox->setButton(*mReference);
}

void KPrefsDialogWidRadios::writeConfig()
{
  *mReference = mBox->id(mBox->selected());
}


KPrefsDialogWidString::KPrefsDialogWidString(const QString &text,QString *reference,
                                 QWidget *parent, QLineEdit::EchoMode echomode)
{
  mReference = reference;
  
  mLabel = new QLabel(text,parent);
  mEdit = new QLineEdit(parent);
  mEdit->setEchoMode( echomode );
}

KPrefsDialogWidString::~KPrefsDialogWidString()
{
}

void KPrefsDialogWidString::readConfig()
{
  mEdit->setText(*mReference);
}

void KPrefsDialogWidString::writeConfig()
{
  *mReference = mEdit->text();
}

QLabel *KPrefsDialogWidString::label()
{
  return mLabel;
}

QLineEdit *KPrefsDialogWidString::lineEdit()
{
  return mEdit;
}


KPrefsDialog::KPrefsDialog(KPrefs *prefs,QWidget *parent,char *name,bool modal) :
  KDialogBase(IconList,i18n("Preferences"),Ok|Cancel|Default,Ok,parent,
              name,modal,true)
{
  mPrefs = prefs;

// This seems to cause a crash on exit. Investigate later.
  mPrefsWids.setAutoDelete(true);

  connect(this,SIGNAL(defaultClicked()),SLOT(slotDefault()));
  //connect(this,SIGNAL(cancelClicked()),SLOT(slotDefault()));
  //connect(this,SIGNAL(cancelClicked()),SLOT(reject()));
}

KPrefsDialog::~KPrefsDialog()
{
}

void KPrefsDialog::addWid(KPrefsDialogWid *wid)
{
  mPrefsWids.append(wid);
}

KPrefsDialogWidBool *KPrefsDialog::addWidBool(const QString &text,bool *reference,QWidget *parent)
{
  KPrefsDialogWidBool *w = new KPrefsDialogWidBool(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsDialogWidTime *KPrefsDialog::addWidTime(const QString &text,int *reference,QWidget *parent)
{
  KPrefsDialogWidTime *w = new KPrefsDialogWidTime(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsDialogWidColor *KPrefsDialog::addWidColor(const QString &text,QColor *reference,QWidget *parent)
{
  KPrefsDialogWidColor *w = new KPrefsDialogWidColor(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsDialogWidRadios *KPrefsDialog::addWidRadios(const QString &text,int *reference,QWidget *parent)
{
  KPrefsDialogWidRadios *w = new KPrefsDialogWidRadios(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsDialogWidString *KPrefsDialog::addWidString(const QString &text,QString *reference,QWidget *parent)
{
  KPrefsDialogWidString *w = new KPrefsDialogWidString(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsDialogWidString *KPrefsDialog::addWidPassword(const QString &text,QString *reference,QWidget *parent)
{
  KPrefsDialogWidString *w = new KPrefsDialogWidString(text,reference,parent,QLineEdit::Password);
  addWid(w);
  return w;
}

KPrefsDialogWidFont *KPrefsDialog::addWidFont(const QString &sampleText,const QString &buttonText,
                                        QFont *reference,QWidget *parent)
{
  KPrefsDialogWidFont *w = new KPrefsDialogWidFont(sampleText,buttonText,reference,parent);
  addWid(w);
  return w;
}

void KPrefsDialog::setDefaults()
{
  mPrefs->setDefaults();
  
  readConfig();
}

void KPrefsDialog::readConfig()
{
//  kdDebug(5300) << "KPrefsDialog::readConfig()" << endl;

  KPrefsDialogWid *wid;
  for(wid = mPrefsWids.first();wid;wid=mPrefsWids.next()) {
    wid->readConfig();
  }

  usrReadConfig();
}

void KPrefsDialog::writeConfig()
{
//  kdDebug(5300) << "KPrefsDialog::writeConfig()" << endl;

  KPrefsDialogWid *wid;
  for(wid = mPrefsWids.first();wid;wid=mPrefsWids.next()) {
    wid->writeConfig();
  }

  usrWriteConfig();

//  kdDebug(5300) << "KPrefsDialog::writeConfig() now writing..." << endl;
  
  mPrefs->writeConfig();

//  kdDebug(5300) << "KPrefsDialog::writeConfig() done" << endl;
}


void KPrefsDialog::slotApply()
{
  writeConfig();
  emit configChanged();
}

void KPrefsDialog::slotOk()
{
  slotApply();
  QDialog::accept();
}
void KPrefsDialog::accept()
{
  slotOk();
}

void KPrefsDialog::slotDefault()
{
  if (KMessageBox::warningContinueCancel(this,
      i18n("You are about to set all\npreferences to default values.\nAll "
      "custom modifications will be lost."),i18n("Setting Default Preferences"),
      i18n("Continue"))
    == KMessageBox::Continue) setDefaults(); 
}

KPimPrefsGlobalDialog::KPimPrefsGlobalDialog(QWidget *parent,char *name,bool modal): KPrefsDialog( KPimGlobalPrefs::instance() ,parent, name, modal )
{
    setFont( KGlobalSettings::generalMaxFont() );
    kdelibcfg = new KDEPIMConfigWidget(  KPimGlobalPrefs::instance(), this, "KPrefsGlobalDialog" );
    setMainWidget( kdelibcfg );
    setCaption( i18n("KDE-Pim Global Settings"));
    kdelibcfg->readConfig();
#ifndef DESKTOP_VERSION
    if ( QApplication::desktop()->height() <= 480 )
         hideButtons(); 
    showMaximized();
#endif
}
void KPimPrefsGlobalDialog::showTZconfig()
{
    kdelibcfg->showTimeZoneTab() ;
}    
void KPimPrefsGlobalDialog::usrReadConfig() 
{
    kdelibcfg->readConfig();
}

void KPimPrefsGlobalDialog::usrWriteConfig() 
{
    kdelibcfg->writeConfig();
}

