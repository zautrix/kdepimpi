/*
    This file is part of KABC/Pi.
    Copyright (c) 2004 Ulf Schenk

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

/*
$Id: syncprefwidget.h,v 1.2 2004/08/04 22:45:22 ulf69 Exp $
*/

#ifndef _SYNCPREFWIDGET_H
#define _SYNCPREFWIDGET_H

#include <kresources/syncwidget.h>

class QRadioButton;
class QCheckBox;
class QVBox;
class QComboBox;
class QLineEdit;
class KRES::Resource;

class SyncPrefWidget_Settings : public KRES::SyncWidget
{
    Q_OBJECT
  public:
    SyncPrefWidget_Settings(QWidget *parent=0, const char *name = 0 );
    ~SyncPrefWidget_Settings();

  public slots:
    virtual void loadSettings( KRES::Resource *resource );
    virtual void saveSettings( KRES::Resource *resource );


  protected:
    void init();

  private:
    QCheckBox* mIncludeInRing;
    QCheckBox* mWriteBackFile;
    QCheckBox* mWriteBackExisting;
    QCheckBox* mAskForPreferences;
    QCheckBox* mShowSummaryAfterSync;
};



class SyncPrefWidget_Conflict : public KRES::SyncWidget
{
    Q_OBJECT
  public:
    SyncPrefWidget_Conflict(QWidget *parent=0, const char *name = 0 );
    ~SyncPrefWidget_Conflict();

  public slots:
    virtual void loadSettings( KRES::Resource *resource );
    virtual void saveSettings( KRES::Resource *resource );

  protected:
    void init();

  private:
    QRadioButton* loc, *rem, *newest, *ask, *f_loc,* f_rem, *both;
};




class SyncPrefWidget_Remote : public KRES::SyncWidget
{
    Q_OBJECT
  public:
    SyncPrefWidget_Remote(QWidget *parent=0, const char *name = 0 );
    ~SyncPrefWidget_Remote();

  enum Types {
    Local = 0,
    Remote = 1,
    Mobile =2
  };


  public slots:
    virtual void loadSettings( KRES::Resource *resource );
    virtual void saveSettings( KRES::Resource *resource );

  protected slots:
    void fillSSH();
    void fillFTP();
    void kindChanged( int newKind );

  protected:
    void init();

  private:
    QComboBox* mProfileKind;
    QLineEdit * mRemotePostcommand;
    QLineEdit * mRemotePrecommand;
    QLineEdit * mRemoteFile;
    QLineEdit * mLocalTempFile;
    QVBox* remoteFileWidget;
};





class SyncPrefWidgetContainer : public KRES::SyncWidgetContainer
{
  Q_OBJECT

public:
  SyncPrefWidgetContainer();

	  virtual KRES::SyncWidget* generateSettingsTab(QWidget *parent, const char *name);
	  virtual KRES::SyncWidget* generateConflictsTab(QWidget *parent, const char *name);
	  virtual KRES::SyncWidget* generateRemoteTab(QWidget *parent, const char *name);
};



#endif
