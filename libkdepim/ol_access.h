/*
    This file is part of KDE/Pim-Pi
    Copyright (c) 2006 Lutz Rogowski

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
#ifndef OL_ACCESS_H
#define OL_ACCESS_H

#include <qstring.h> 

#include <kdialogbase.h>
#include <kabc/addressee.h>
#include <kabc/addressbook.h>

#include <afxdisp.h>  

#include "../outport/msoutl9.h"

#define OL_CONTACT_DATA 2
#define OL_CALENDAR_DATA 1 //pending is this the right number

class QDateTime;
class QListView;
class OLEListViewItem;
class _ContactItem;
class OL_access : public QObject
{
    Q_OBJECT
        public:
    static OL_access *instance();
    ~OL_access();
    //static converter methods
    static QDateTime mDdate2Qdtr( DATE dt);
    static DATE Qdt2date( QDateTime dt );
    static KABC::Addressee ol2kapiContact( _ContactItem * aItem , bool syncMode);
    static void writeData2OLitem( KABC::Addressee addressee, _ContactItem * aItem );

    KABC::Addressee::List importOLcontacts();
    QStringList getFolderSelection( int type, QString caption );
    void readContactData( LPDISPATCH  folder, KABC::Addressee::List* list, bool syncMode );

    LPDISPATCH getFolderFromID( LPDISPATCH parentFolder, QString selectedFolderID );
    bool setSelectedFolder( QString folderID );
    //call setSelectedFolder() before using the next methods:
    void deleteAddressee( KABC::Addressee a );
    KABC::Addressee changeAddressee( KABC::Addressee a );
    KABC::Addressee addAddressee( KABC::Addressee a );

    


    public slots:
   

 signals:
  
    protected slots:
   

 protected:

 private:
    _Application gOlAppAB;
    bool mErrorInit;
    void addFolder(OLEListViewItem* iParent, LPDISPATCH dispParent, long fType);
    LPDISPATCH getOLcontactItemFromUid( QString uid );

    QString mSelectedFolderID;
    LPDISPATCH mSelectedFolder;
    _Items mFolderItems;

    OL_access();
    static OL_access *sInstance;
};


#if 0
//using namespace KABC;
//class KABC::AddressBook;

/**
  This is the base class for the calendar component editors.
*/
class KAImportOLdialog : public KDialogBase
{
    Q__OBJECT
  public:
    /**
      Construct new IncidenceEditor.
    */
    KAImportOLdialog( const QString &caption, KABC::AddressBook * aBook,
                       QWidget *parent );
     ~KAImportOLdialog();

    /** Initialize editor. This function creates the tab widgets. */
    void init();
    void setSyncMode();
   KABC::Addressee::List getAddressList();
   QString selectedFolderID() {return mSelectedFolderID;}
   static KABC::Addressee ol2kapiContact( _ContactItem * , bool syncMode );
   static bool sOLDispatch;
  pub_lic sl_ots:
   

  sign_als:
  
  prote_cted slo_ts:
    void slotApply();
    void slotOk();
    void slotCancel();

  protected:
    void setupFolderView();
    void addFolder(OLEListViewItem* iParent, LPDISPATCH dispParent);
    void readContactData( DWORD folder );

    KABC::AddressBook * mABook;
    QListView * mListView;
    KABC::Addressee::List mAList;
    bool addAddressee( KABC::Addressee a );
  private:
    int importedItems;
    bool mSyncMode;
    QString mSelectedFolderID;
};
class KAwritebackOL : public QObject
{
    Q__OBJECT
  public:
    KAwritebackOL( const QString &olFolderID );
     ~KAwritebackOL();
     bool init();
     void deleteAddressee( KABC::Addressee a );
     KABC::Addressee changeAddressee( KABC::Addressee a );
     KABC::Addressee addAddressee( KABC::Addressee a );
  public sl_ots:
   

  signals:
  
  protected sl_ots:
   

  protected:
  LPDISPATCH getFolderFromID( LPDISPATCH parentFolder );
  LPDISPATCH getOLitemFromUid( QString uid );
  void writeData2OLitem(  KABC::Addressee a, _ContactItem * olitem );
  private:
  QString mSelectedFolderID;
  LPDISPATCH mSelectedFolder;
  _Items mFolderItems;
};

#endif
#endif


