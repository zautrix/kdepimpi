/*
    This file is part of KAddressbook.
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

#ifndef KABCORE_H
#define KABCORE_H

#include <kabc/field.h>

#ifndef KAB_EMBEDDED
#endif //KAB_EMBEDDED
#include <qdict.h>
#include <qtimer.h>

#include <qwidget.h>
#include <qpopupmenu.h>
#include <ksyncmanager.h>
#ifndef DESKTOP_VERSION
#include <qcopchannel_qws.h> 
#endif

namespace KABC {
class AddressBook;
}

#ifndef KAB_EMBEDDED
class KAboutData;
class KConfig;

class KAddressBookService;
class LDAPSearchDialog;
#else //KAB_EMBEDDED
class KAddressBookMain;
//US class QAction;
#endif //KAB_EMBEDDED
class KCMultiDialog;
class KXMLGUIClient;
class ExtensionManager;
class XXPortManager;
class JumpButtonBar;
class IncSearchWidget;
class KDGanttMinimizeSplitter;
class KAction;
class KActionCollection;
class KToggleAction;
class KSyncProfile;

class QAction;
class QMenuBar;
class QSplitter;
class ViewContainer;
class ViewManager;
class AddresseeEditorDialog;
class Ir;

class KABCore : public QWidget, public KSyncInterface
{
  Q_OBJECT

  public:
    KABCore( KAddressBookMain *client, bool readWrite, QWidget *parent, const char *name = 0 );


    ~KABCore();


#ifdef KAB_EMBEDDED
  //US added functionality
    QPopupMenu* getViewMenu() {return viewMenu;}
    QPopupMenu* getFilterMenu() {return filterMenu;}
    QPopupMenu* getSettingsMenu() {return settingsMenu;}
    void addActionsManually();
#endif //KAB_EMBEDDED
    /**
      Restores the global settings.
     */
    void restoreSettings();

  

    /**
      Returns a pointer to the StdAddressBook of the application.
     */
    KABC::AddressBook *addressBook() const;

    /**
      Returns a pointer to the KConfig object of the application.
     */
    static KConfig *config();

    /**
      Returns a pointer to the global KActionCollection object. So
      other classes can register their actions easily.
     */
    KActionCollection *actionCollection() const;

    /**
      Returns the current search field of the Incremental Search Widget.
     */
    KABC::Field *currentSearchField() const;

    /**
      Returns the uid list of the currently selected contacts.
     */
    QStringList selectedUIDs() const;

    /**
      Displays the ResourceSelectDialog and returns the selected
      resource or a null pointer if no resource was selected by
      the user.
     */
    KABC::Resource *requestResource( QWidget *parent );

#ifndef KAB_EMBEDDED
    static KAboutData *createAboutData();
#endif //KAB_EMBEDDED

#ifdef KAB_EMBEDDED
    inline QPopupMenu* getImportMenu() { return ImportMenu;}
    inline QPopupMenu* getExportMenu() { return ExportMenu;}
#endif //KAB_EMBEDDED

  public slots:
#ifdef KAB_EMBEDDED
    void createAboutData();
#endif //KAB_EMBEDDED
    void setDetailsToggle();

    void showLicence();
    void faq();
    void whatsnew() ;
    void synchowto() ;
    void storagehowto() ;
    void multisynchowto() ;
    void kdesynchowto() ;
    void writeToPhone();

    /**
      Is called whenever a contact is selected in the view.
     */
    void setContactSelected( const QString &uid );

    /**
      Opens the preferred mail composer with all selected contacts as
      arguments.
     */
    void sendMail();

    /**
      Opens the preferred mail composer with the given contacts as
      arguments.
     */
    void sendMail( const QString& email );


    void mailVCard();
    void mailVCard(const QStringList& uids);

    /**
      Beams the "WhoAmI contact.
     */
    void beamMySelf();

    void beamVCard();
    void export2phone();
    void beamVCard(const QStringList& uids);
    void beamDone( Ir *ir );


    /**
      Starts the preferred web browser with the given URL as argument.
     */
    void browse( const QString& url );

    /**
      Select all contacts in the view.
     */
    void selectAllContacts();

    /**
      Deletes all selected contacts from the address book.
     */
    void deleteContacts();

    /**
      Deletes given contacts from the address book.

      @param uids The uids of the contacts, which shall be deleted.
     */
    void deleteContacts( const QStringList &uids );

    /**
      Copys the selected contacts into clipboard for later pasting.
     */
    void copyContacts();

    /**
      Cuts the selected contacts and stores them for later pasting.
     */
    void cutContacts();

    /**
      Paste contacts from clipboard into the address book.
     */
    void pasteContacts();

    /**
      Paste given contacts into the address book.

      @param list The list of addressee, which shall be pasted.
     */
    void pasteContacts( KABC::Addressee::List &list );

    /**
      Sets the whoAmI contact, that is used by many other programs to
      get personal information about the current user.
     */
    void setWhoAmI();

    /**
      Displays the category dialog and applies the result to all
      selected contacts.
     */
    void setCategories();
    void manageCategories();
    void editCategories();

    /**
      Sets the field list of the Incremental Search Widget.
     */
    void setSearchFields( const KABC::Field::List &fields );

    /**
      Search with the current search field for a contact, that matches
      the given text, and selects it in the view.
     */
    void incrementalSearch( const QString& text );
    void incrementalSearchJump( const QString& text );

    /**
      Marks the address book as modified.
     */
    void setModified();
    /**
      Marks the address book as modified without refreshing the view.
     */
    void setModifiedWOrefresh();

    /**
      Marks the address book as modified concerning the argument.
     */
    void setModified( bool modified );

    /**
      Returns whether the address book is modified.
     */
    bool modified() const;

    /**
      Called whenever an contact is modified in the contact editor
      dialog or the quick edit.
     */
    void contactModified( const KABC::Addressee &addr );
    void addrModified( const KABC::Addressee &addr, bool updateDetails = true );

    /**
      DCOP METHODS.
     */
    void addEmail( QString addr );
    void importVCard( const KURL& url, bool showPreview );
    void importVCard( const QString& vCard, bool showPreview );
    void newContact();
    QString getNameByPhone( const QString& phone );
    /**
      END DCOP METHODS
     */

    /**
      Saves the contents of the AddressBook back to disk.
     */
    void save();

    /**
      Undos the last command using the undo stack.
     */
    void undo();

    /**
      Redos the last command that was undone, using the redo stack.
     */
    void redo();

    /**
      Shows the edit dialog for the given uid. If the uid is QString::null,
      the method will try to find a selected addressee in the view.
     */
    void editContact( const QString &uid /*US = QString::null*/ );
//US added a second method without defaultparameter
    void editContact2();

    /**
      Shows or edits the detail view for the given uid. If the uid is QString::null,
      the method will try to find a selected addressee in the view.
     */
    void executeContact( const QString &uid /*US = QString::null*/ );

    /**
      Launches the configuration dialog.
     */
    void openConfigDialog();
    void openConfigGlobalDialog();

    /**
      Launches the ldap search dialog.
     */
    void openLDAPDialog();

    /**
      Creates a KAddressBookPrinter, which will display the print
      dialog and do the printing.
     */
    void print();

    /**
      Registers a new GUI client, so plugins can register its actions.
     */
   void addGUIClient( KXMLGUIClient *client );

   void requestForNameEmailUidList(const QString& sourceChannel, const QString& sessionuid);
   void requestForDetails(const QString& sourceChannel, const QString& sessionuid, const QString& name, const QString& email, const QString& uid);
   void requestForBirthdayList(const QString& sourceChannel, const QString& sessionuid);


  signals:
    void contactSelected( const QString &name );
    void contactSelected( const QPixmap &pixmap );
  public slots:
    void loadDataAfterStart();
    void recieve(QString cmsg );
    void getFile( bool success,const QString & );
    void syncFileRequest(const QString &);
    void setDetailsVisible( bool visible );
    void setDetailsToState();
   
    void saveSettings();
    void multiResourceSyncStart( bool start );

  private slots:
    void updateToolBar();
    void updateMainWindow();
    void receive( const QCString& cmsg, const QByteArray& data );
    void receiveStart( const QCString& cmsg, const QByteArray& data );
    void toggleBeamReceive( );
    void disableBR(bool);
    void setJumpButtonBarVisible( bool visible );
    void setJumpButtonBar( bool visible );
    void setCaptionBack();
    void resizeAndCallContactdialog();
    void callContactdialog();
    void doRingSync();

  void importFromOL();
    void extensionModified( const KABC::Addressee::List &list );
    void extensionChanged( int id );
    void clipboardDataChanged();
    void updateActionMenu();
    void configureKeyBindings();
    void removeVoice();
    void setFormattedName();
#ifdef KAB_EMBEDDED
    void configureResources();
#endif //KAB_EMBEDDED

    void slotEditorDestroyed( const QString &uid );
    void configurationChanged();
    void addressBookChanged();

  private:
    QCString mCStringMess;
    QByteArray mByteData;
    QString mEmailSourceChannel;
    QString mEmailSourceUID;
    void resizeEvent(QResizeEvent* e );
    bool mBRdisabled;
#ifndef DESKTOP_VERSION
    QCopChannel* infrared;
#endif
    QTimer *mMessageTimer;
    void initGUI();
    void initActions();
    QString getPhoneFile();

    AddresseeEditorDialog *createAddresseeEditorDialog( QWidget *parent,
                                                        const char *name = 0 );

 KXMLGUIClient *mGUIClient;

  KABC::AddressBook *mAddressBook;

  ViewManager *mViewManager;
  // QSplitter *mDetailsSplitter;
  KDGanttMinimizeSplitter *mExtensionBarSplitter;
  ViewContainer *mDetails;
  KDGanttMinimizeSplitter* mMiniSplitter;
  XXPortManager *mXXPortManager;
  JumpButtonBar *mJumpButtonBar;
  IncSearchWidget *mIncSearchWidget;
  ExtensionManager *mExtensionManager;

  KCMultiDialog *mConfigureDialog;

#ifndef KAB_EMBEDDED
    LDAPSearchDialog *mLdapSearchDialog;
#endif //KAB_EMBEDDED
    // QDict<AddresseeEditorDialog> mEditorDict;
    AddresseeEditorDialog *mEditorDialog;
    bool mReadWrite;
    bool mModified;
    bool mIsPart;
    bool mMultipleViewsAtOnce;


    //US file menu
    KAction *mActionMail;
    KAction *mActionBeam;
    KToggleAction *mActionBR;
    KAction *mActionExport2phone;
    KAction* mActionPrint;
    KAction* mActionPrintDetails;
    KAction* mActionNewContact;
    KAction *mActionSave;
    KAction *mActionEditAddressee;
    KAction *mActionMailVCard;
    KAction *mActionBeamVCard;

    KAction *mActionQuit;

    //US edit menu
    KAction *mActionCopy;
    KAction *mActionCut;
    KAction *mActionPaste;
    KAction *mActionSelectAll;
    KAction *mActionUndo;
    KAction *mActionRedo;
    KAction *mActionDelete;

    //US settings menu
    KAction *mActionConfigResources;
    KAction *mActionConfigGlobal;
    KAction *mActionConfigKAddressbook;
    KAction *mActionConfigShortcuts;
    KAction *mActionConfigureToolbars;
    KAction *mActionKeyBindings;
    KToggleAction *mActionJumpBar;
    KToggleAction *mActionDetails;
    KAction *mActionWhoAmI;
    KAction *mActionCategories;
    KAction *mActionEditCategories;
    KAction *mActionManageCategories;
    KAction *mActionAboutKAddressbook;
    KAction *mActionLicence;
    KAction *mActionFaq;
    KAction *mActionWN;
    KAction *mActionSyncHowto;
    KAction *mActionStorageHowto;
    KAction *mActionKdeSyncHowto;
    KAction *mActionMultiSyncHowto;

    KAction *mActionDeleteView;

    QPopupMenu *viewMenu;
    QPopupMenu *filterMenu;
    QPopupMenu *settingsMenu;
    QPopupMenu *changeMenu;
    QPopupMenu *beamMenu;
//US    QAction *mActionSave;
    QPopupMenu *ImportMenu;
    QPopupMenu *ExportMenu;
    //LR additional methods
    KAction *mActionRemoveVoice;
    KAction *mActionSetFormattedName;
    KAction * mActionImportOL;

#ifndef KAB_EMBEDDED
    KAddressBookService *mAddressBookService;
#endif //KAB_EMBEDDED

    class KABCorePrivate;
    KABCorePrivate *d;
    //US    bool mBlockSaveFlag;

#ifdef KAB_EMBEDDED
    KAddressBookMain *mMainWindow;  // should be the same like mGUIClient
#endif //KAB_EMBEDDED

    //this are the overwritten callbackmethods from the syncinterface
    virtual bool sync(KSyncManager* manager, QString filename, int mode,QString resource);
    virtual bool syncExternal(KSyncManager* manager, QString resource);
    virtual void removeSyncInfo( QString syncProfile);
    QString getLastSyncDT4device( QString syncDevice );
    bool readOLdata( KABC::AddressBook* local );
    bool writeOLdata( KABC::AddressBook* local );
    bool syncOL();
    bool syncPhone();
    void message( QString m , bool startTimer = true);

    // LR *******************************
    // sync stuff!
    bool  mMultiResourceSync;
    QString sentSyncFile();
    QPopupMenu *syncMenu;
    KSyncManager* syncManager;
    int mGlobalSyncMode; 
    bool synchronizeAddressbooks( KABC::AddressBook* local, KABC::AddressBook* remote,int mode);
    KABC::Addressee getLastSyncAddressee();
    QDateTime mLastAddressbookSync;
    int  takeAddressee( KABC::Addressee* local,  KABC::Addressee* remote, int mode , bool full );
    // *********************
    //OL sync stuff
    QString mOLsyncFolderID;

};

#endif
