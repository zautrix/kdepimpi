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

// $Id: kprefswidget.cpp,v 1.2 2004/11/01 14:16:10 zautrix Exp $

#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qfont.h>
#include <qslider.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qvbox.h>
#include <qhbox.h>
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
#include <kfontdialog.h>
#include <kmessagebox.h>
#include <kcolordialog.h>
#include <kiconloader.h>

#include "kprefs.h"

#include "kprefswidget.h"
//#include "kprefsdialog.moc"

KPrefsWidBool::KPrefsWidBool(const QString &text,bool *reference,
                             QWidget *parent)
{
  mReference = reference;

  mCheck = new QCheckBox(text,parent);

  connect( mCheck, SIGNAL( toggled( bool ) ), SIGNAL( modified() ) );

}

void KPrefsWidBool::readConfig()
{
  mCheck->setChecked(*mReference);
}

void KPrefsWidBool::writeConfig()
{
  *mReference = mCheck->isChecked();
}

QCheckBox *KPrefsWidBool::checkBox()
{
  return mCheck;
}


KPrefsWidColor::KPrefsWidColor(const QString &text,QColor *reference,
                               QWidget *parent)
{
  mReference = reference;

  mButton = new KColorButton(parent);
  mLabel = new QLabel(mButton, text, parent);
  mButton->setColor( *mReference );
  mButton->setColor( Qt::red );

  connect( mButton, SIGNAL( changed(const QColor &)), SIGNAL( modified() ) );

}

KPrefsWidColor::~KPrefsWidColor()
{
//  kdDebug(5300) << "KPrefsWidColor::~KPrefsWidColor()" << endl;
}

void KPrefsWidColor::readConfig()
{
  mButton->setColor(*mReference);
}

void KPrefsWidColor::writeConfig()
{
  *mReference = mButton->color();
}

QLabel *KPrefsWidColor::label()
{
  return mLabel;
}

KColorButton *KPrefsWidColor::button()
{
  return mButton;
}

KPrefsWidFont::KPrefsWidFont(const QString &sampleText,const QString &labelText,
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

KPrefsWidFont::~KPrefsWidFont()
{
}

void KPrefsWidFont::readConfig()
{
  mPreview->setFont(*mReference);
}

void KPrefsWidFont::writeConfig()
{
  *mReference = mPreview->font();
}

QLabel *KPrefsWidFont::label()
{
  return mLabel;
}

QLabel *KPrefsWidFont::preview()
{
  return mPreview;
}

QPushButton *KPrefsWidFont::button()
{
  return mButton;
}

void KPrefsWidFont::selectFont()
{
  QFont myFont(mPreview->font());
  bool ok;
  myFont = KFontDialog::getFont(myFont, ok);
  if ( ok ) {
    mPreview->setFont(myFont);
    emit modified();
  }
}


KPrefsWidTime::KPrefsWidTime(const QString &text,int *reference,
                             QWidget *parent)
{
  mReference = reference;

  mLabel = new QLabel(text,parent);
  mSpin = new QSpinBox(0,23,1,parent);
  mSpin->setSuffix(":00");
  connect( mSpin, SIGNAL( valueChanged(int)), SIGNAL( modified() ) );

}

void KPrefsWidTime::readConfig()
{
  mSpin->setValue(*mReference);
}

void KPrefsWidTime::writeConfig()
{
  *mReference = mSpin->value();
}

QLabel *KPrefsWidTime::label()
{
  return mLabel;
}

QSpinBox *KPrefsWidTime::spinBox()
{
  return mSpin;
}


KPrefsWidRadios::KPrefsWidRadios(const QString &text,int *reference,
                QWidget *parent)
{
  mReference = reference;

  mBox = new QButtonGroup(1,Qt::Horizontal,text,parent);
  connect( mBox, SIGNAL( clicked(int)), SIGNAL( modified() ) );
}

KPrefsWidRadios::~KPrefsWidRadios()
{
}

void KPrefsWidRadios::addRadio(const QString &text)
{
  new QRadioButton(text,mBox);
}

QButtonGroup *KPrefsWidRadios::groupBox()
{
  return mBox;
}

void KPrefsWidRadios::readConfig()
{
  mBox->setButton(*mReference);
}

void KPrefsWidRadios::writeConfig()
{
  *mReference = mBox->id(mBox->selected());
}


KPrefsWidString::KPrefsWidString(const QString &text,QString *reference,
                                 QWidget *parent, QLineEdit::EchoMode echomode)
{
  mReference = reference;
  
  mLabel = new QLabel(text,parent);
  mEdit = new QLineEdit(parent);
  mEdit->setEchoMode( echomode );
  connect( mEdit, SIGNAL( textChanged(const QString&) ), SIGNAL( modified() ) );

}

KPrefsWidString::~KPrefsWidString()
{
}

void KPrefsWidString::readConfig()
{
  mEdit->setText(*mReference);
}

void KPrefsWidString::writeConfig()
{
  *mReference = mEdit->text();
}

QLabel *KPrefsWidString::label()
{
  return mLabel;
}

QLineEdit *KPrefsWidString::lineEdit()
{
  return mEdit;
}


KPrefsWidget::KPrefsWidget(KPrefs *prefs,QWidget *parent,const char *name) :
  QWidget(parent, name )
{
  mPrefs = prefs;
  mPrefsWids.setAutoDelete( true );
}

KPrefsWidget::~KPrefsWidget()
{
}

void KPrefsWidget::addWid(KPrefsWid *wid)
{
  mPrefsWids.append(wid);
  connect( wid, SIGNAL( modified() ), this, SLOT( modified() ) );

}

KPrefsWidBool *KPrefsWidget::addWidBool(const QString &text,bool *reference,QWidget *parent)
{
  KPrefsWidBool *w = new KPrefsWidBool(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsWidTime *KPrefsWidget::addWidTime(const QString &text,int *reference,QWidget *parent)
{
  KPrefsWidTime *w = new KPrefsWidTime(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsWidColor *KPrefsWidget::addWidColor(const QString &text,QColor *reference,QWidget *parent)
{
  KPrefsWidColor *w = new KPrefsWidColor(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsWidRadios *KPrefsWidget::addWidRadios(const QString &text,int *reference,QWidget *parent)
{
  KPrefsWidRadios *w = new KPrefsWidRadios(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsWidString *KPrefsWidget::addWidString(const QString &text,QString *reference,QWidget *parent)
{
  KPrefsWidString *w = new KPrefsWidString(text,reference,parent);
  addWid(w);
  return w;
}

KPrefsWidString *KPrefsWidget::addWidPassword(const QString &text,QString *reference,QWidget *parent)
{
  KPrefsWidString *w = new KPrefsWidString(text,reference,parent,QLineEdit::Password);
  addWid(w);
  return w;
}

KPrefsWidFont *KPrefsWidget::addWidFont(const QString &sampleText,const QString &buttonText,
                                        QFont *reference,QWidget *parent)
{
  KPrefsWidFont *w = new KPrefsWidFont(sampleText,buttonText,reference,parent);
  addWid(w);
  return w;
}

void KPrefsWidget::setDefaults()
{
  mPrefs->setDefaults();
  
  readConfig();
}

void KPrefsWidget::readConfig()
{
//  kdDebug(5300) << "KPrefsDialog::readConfig()" << endl;

  KPrefsWid *wid;
  for(wid = mPrefsWids.first();wid;wid=mPrefsWids.next()) {
    wid->readConfig();
  }

  usrReadConfig();

  emit changed( false );

}

void KPrefsWidget::writeConfig()
{
//  kdDebug(5300) << "KPrefsDialog::writeConfig()" << endl;

  KPrefsWid *wid;
  for(wid = mPrefsWids.first();wid;wid=mPrefsWids.next()) {
    wid->writeConfig();
  }

  usrWriteConfig();

//  kdDebug(5300) << "KPrefsDialog::writeConfig() now writing..." << endl;
  
  mPrefs->writeConfig();

  emit changed( false );

//  kdDebug(5300) << "KPrefsDialog::writeConfig() done" << endl;
}

/*US
void KPrefsWidget::slotApply()
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
*/

void KPrefsWidget::modified()
{
  emit changed( true );
}

