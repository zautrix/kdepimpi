/*
    This file is part of KAddressbook.
    Copyright (c) 1999 Don Sanders <dsanders@kde.org>

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

#ifndef KADDRESSBOOKMAIN_H
#define KADDRESSBOOKMAIN_H

#include <qptrlist.h>

#ifdef KAB_EMBEDDED
class QToolBar;
#include <qaction.h>
//#include <qmainwindow.h>
#include <kmainwindow.h>
#else //KAB_EMBEDDED
#include <kaction.h>
#include <kapplication.h>
#include <kmainwindow.h>
#include "kaddressbookiface.h"
#endif //KAB_EMBEDDED

class KABCore;
class KConfig;

/**
  This class serves as the main window for KAddressBook.  It handles the
  menus, toolbars, and status bars.
 
  @short Main window class
  @author Don Sanders <dsanders@kde.org>
  @version 0.1
 */
#ifdef KAB_EMBEDDED
class KAddressBookMain : public KMainWindow
#else //KAB_EMBEDDED
//MOC_SKIP_BEGIN
class KAddressBookMain : public KMainWindow, virtual public KAddressBookIface
//MOC_SKIP_END
#endif //KAB_EMBEDDED
{
  Q_OBJECT

  public:
    KAddressBookMain();
    virtual ~KAddressBookMain();

#ifdef KAB_EMBEDDED
//    QPEToolBar * getIconToolBar();
    // QToolBar * getIconToolBar();
#endif //KAB_EMBEDDED

    
  public slots:
    void showMinimized () ;
    virtual void addEmail( QString addr );
#ifndef KAB_EMBEDDED
//MOC_SKIP_BEGIN
    virtual ASYNC showContactEditor( QString uid );
//MOC_SKIP_END
#endif //KAB_EMBEDDED
    virtual void newContact();
    virtual QString getNameByPhone( QString phone );
    virtual void save();
    virtual void exit();
  protected:
    void initActions();
#ifdef KAB_EMBEDDED
    //US new method to setup menues and toolbars on embedded systems
    void createGUI();
#endif //KAB_EMBEDDED
    
    /**
      This function is called when it is time for the app to save its
      properties for session management purposes.
     */
    void saveProperties( KConfig* );

    /**
      This function is called when this app is restored.  The KConfig
      object points to the session management config file that was saved
      with @ref saveProperties
     */
    void readProperties( KConfig* );

    void closeEvent( QCloseEvent* ce );
 
  protected slots:
    void configureToolbars();
    void configureKeys();

    void slotNewToolbarConfig();
    
  private:
    KABCore *mCore;

#ifdef KAB_EMBEDDED
    //  QToolBar *iconToolBar;
#endif //KAB_EMBEDDED
        
};

#endif
