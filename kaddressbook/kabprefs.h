/*
    This file is part of KAddressBook.
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>

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

#ifndef KABPREFS_H
#define KABPREFS_H

#include <qstringlist.h>
#include <qdict.h>

#include <libkdepim/kpimprefs.h>

class KConfig;

class KABPrefs : public KPimPrefs
{
  public:
    virtual ~KABPrefs();

    static KABPrefs *instance();

    // General
    bool mHonorSingleClick;
    bool mAutomaticNameParsing;
    int mCurrentIncSearchField;

#ifdef KAB_EMBEDDED
    // US introduce a nonconst way to return the config object.
    KConfig* getConfig();

    bool mToolBarHor;
    bool mToolBarUp;
    bool mAskForQuit;


    /** Set preferences to default values */
//    void usrSetDefaults();

    /** Read preferences from config file */
//    void usrReadConfig();

    /** Write preferences to config file */
//    void usrWriteConfig();
#endif //KAB_EMBEDDED
    void usrReadConfig();


    // GUI
    bool mFullMenuBarVisible;
    bool mJumpButtonBarVisible;
    bool mDetailsPageVisible;
    bool mMultipleViewsAtOnce;
    bool mSearchWithReturn;
    bool mAutoSearchWithWildcard;
    bool mHideSearchOnSwitch;
    bool mAskForDelete;
    QValueList<int> mExtensionsSplitter;
    QValueList<int> mDetailsSplitter;

    // Extensions stuff
    int mCurrentExtension;
    QStringList mActiveExtensions;

    // Views stuff
    QString mCurrentView;
    QStringList mViewNames;

    // Filter
    int mCurrentFilter;

    void setCategoryDefaults();
    QFont mDetailsFont;

  private:
    KABPrefs();

    static KABPrefs *sInstance;
};

#endif
