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
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>
#include <kemailsettings.h>
#include <kstaticdeleter.h>
#include <kglobalsettings.h>

#include "koprefs.h"
#include "mainwindow.h"


KOPrefs *KOPrefs::mInstance = 0;
static KStaticDeleter<KOPrefs> insd;

KOPrefs::KOPrefs() :
  KPimPrefs("kopiemailrc")
{
    KPrefs::setCurrentGroup("General");
    addItemString("SendCodec",&mSendCodec,i18n ("userdefined") );
    addItemString("SenderName",&mName,i18n ("Please set at") );
    addItemString("SenderEmail",&mEmail,i18n ("Settings@General TAB") );
    addItemBool("ViewMailAsHtml",&mViewAsHtml,false);
    addItemBool("SendMailLater",&mSendLater,true);
    addItemBool("ShowToField",&mShowToField,false);
    addItemBool("UseKapi",&mUseKapi,false);
    addItemInt("CurrentCodec",&mCurrentCodec,0);
    addItemBool("ShowInfoSub",&mShowInfoSub,true);
    addItemBool("ShowInfoFrom",&mShowInfoFrom,true);
    addItemBool("ShowInfoTo",&mShowInfoTo,true);
    addItemBool("ShowInfoStart",&mShowInfoStart,true);
    KPrefs::setCurrentGroup("Fonts");
    addItemFont("Application Font",&mAppFont,KGlobalSettings::generalFont());
    addItemFont("Compose Font",&mComposeFont,KGlobalSettings::generalFont());
    addItemFont("Read Font",&mReadFont,KGlobalSettings::generalFont());
    fillMailDefaults();
    isDirty = false;
}


KOPrefs::~KOPrefs()
{
    if ( isDirty )
        writeConfig();
  if (mInstance == this)
      mInstance = insd.setObject(0); 

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
  if (mName.isEmpty()) mName = i18n ("Please set at");
  if (mEmail.isEmpty()) mEmail = i18n ("Settings@General TAB");
}

void KOPrefs::usrReadConfig()
{

  KPimPrefs::usrReadConfig();
}


void KOPrefs::usrWriteConfig()
{
  KPimPrefs::usrWriteConfig();
}



KConfig* KOPrefs::getConfig()
{
    return config();
}
