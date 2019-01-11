/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
//$Id: kactionclasses.h,v 1.1.1.1 2004/06/26 19:02:41 zautrix Exp $

#ifndef __kactionclasses_h__
#define __kactionclasses_h__

#include <kaction.h>

//US#include <qkeysequence.h>
//US#include <qobject.h>
//US#include <qvaluelist.h>
//US#include <qguardedptr.h>
//US#include <kguiitem.h>
#include <kshortcut.h>
//US#include <kstdaction.h>
//US#include <kicontheme.h>

class QMenuBar;
class QPopupMenu;
//USclass QComboBox;
//USclass QPoint;
//USclass QIconSet;
//USclass QString;
//USclass KToolBar;

//USclass KAccel;
//USclass KAccelActions;
class KConfig;
//USclass KConfigBase;
class KURL;
//USclass KInstance;


//US class KToolBar needs to be replaced
class KToolBar;
class KActionCollection;

//US class KPopupMenu needs to be replaced
//US class KPopupMenu;
//USclass KMainWindow;

/**
 *  Checkbox like action.
 *
 *  This action provides two states: checked or not.
 *
 *  @short Checkbox like action.
 */
class KToggleAction : public KAction
{
    Q_OBJECT
    Q_PROPERTY( bool checked READ isChecked WRITE setChecked )
    Q_PROPERTY( QString exclusiveGroup READ exclusiveGroup WRITE setExclusiveGroup )
public:

    /**
     * Constructs a toggle action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param cut The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const KShortcut& cut = KShortcut(), QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const KShortcut& cut,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QIconSet& pix, const KShortcut& cut = KShortcut(),
             QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QString& pix, const KShortcut& cut = KShortcut(),
                   QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( const QString& text, const QString& pix, const KShortcut& cut,
                   const QObject* receiver, const char* slot,
                   QObject* parent, const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KToggleAction( QObject* parent = 0, const char* name = 0 );

    /**
     * Destructor
     */
    virtual ~KToggleAction();

    /**
     *  "Plug" or insert this action into a given widget.
     *
     *  This will typically be a menu or a toolbar.  From this point
     *  on, you will never need to directly manipulate the item in the
     *  menu or toolbar.  You do all enabling/disabling/manipulation
     *  directly with your KToggleAction object.
     *
     *  @param widget The GUI element to display this action.
     *  @param index  The index of the item.
     */
    virtual int plug( QWidget* widget, int index = -1 );

    /**
     *  Returns the actual state of the action.
     */
    bool isChecked() const;

    /**
     * @return which "exclusive group" this action is part of.
     * @see setExclusiveGroup
     */
    QString exclusiveGroup() const;

    /**
     * Defines which "exclusive group" this action is part of.
     * In a given exclusive group, only one toggle action can be checked
     * at a any moment. Checking an action unchecks the other actions
     * of the group.
     */
    virtual void setExclusiveGroup( const QString& name );

public slots:
    /**
     *  Sets the state of the action.
     */
    virtual void setChecked( bool );

protected slots:
    virtual void slotActivated();

protected:
    virtual void updateChecked( int id );

signals:
    void toggled( bool );

protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KToggleActionPrivate;
    KToggleActionPrivate *d;
};

/**
 * An action that operates like a radio button. At any given time
 * only a single action from the group will be active.
 */
class KRadioAction : public KToggleAction
{
  Q_OBJECT
public:
    /**
     * Constructs a radio action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param cut The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const KShortcut& cut = KShortcut(), QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const KShortcut& cut,
                  const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QIconSet& pix, const KShortcut& cut = KShortcut(),
                  QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QString& pix, const KShortcut& cut = KShortcut(),
                  QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                  const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( const QString& text, const QString& pix, const KShortcut& cut,
                  const QObject* receiver, const char* slot,
                  QObject* parent, const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KRadioAction( QObject* parent = 0, const char* name = 0 );

protected:
    virtual void slotActivated();

protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KRadioActionPrivate;
    KRadioActionPrivate *d;
};

/**
 *  Action for selecting one of several items.
 *
 *  This action shows up a submenu with a list of items.
 *  One of them can be checked. If the user clicks on an item
 *  this item will automatically be checked,
 *  the formerly checked item becomes unchecked.
 *  There can be only one item checked at a time.
 *
 *  @short Action for selecting one of several items
 */
class KSelectAction : public KAction
{
    Q_OBJECT
    Q_PROPERTY( int currentItem READ currentItem WRITE setCurrentItem )
    Q_PROPERTY( QStringList items READ items WRITE setItems )
    Q_PROPERTY( bool editable READ isEditable WRITE setEditable )
    Q_PROPERTY( int comboWidth READ comboWidth WRITE setComboWidth )
    Q_PROPERTY( QString currentText READ currentText )
    Q_PROPERTY( bool menuAccelsEnabled READ menuAccelsEnabled WRITE setMenuAccelsEnabled )
public:

    /**
     * Constructs a select action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param cut The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const KShortcut& cut = KShortcut(), QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const KShortcut& cut,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QIconSet& pix, const KShortcut& cut = KShortcut(),
             QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QString& pix, const KShortcut& cut = KShortcut(),
                   QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( const QString& text, const QString& pix, const KShortcut& cut,
                   const QObject* receiver, const char* slot,
                   QObject* parent, const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KSelectAction( QObject* parent = 0, const char* name = 0 );

    /**
     * Destructor
     */
    virtual ~KSelectAction();

    /**
     *  "Plug" or insert this action into a given widget.
     *
     *  This will typically be a menu or a toolbar.
     *  From this point on, you will never need to directly
     *  manipulate the item in the menu or toolbar.
     *  You do all enabling/disabling/manipulation directly with your KSelectAction object.
     *
     *  @param widget The GUI element to display this action.
     *  @param index  The index of the item.
     */
    virtual int plug( QWidget* widget, int index = -1 );

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * @return true if the combo editable.
     */
    virtual bool isEditable() const;

    /**
     * @return the items that can be selected with this action.
     * Use setItems to set them.
     */
    virtual QStringList items() const;

    virtual void changeItem( int index, const QString& text );

    virtual QString currentText() const;

    virtual int currentItem() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This returns the maximum width set by setComboWidth
     */
    virtual int comboWidth() const;

    QPopupMenu* popupMenu() const;

    /**
     * Deprecated. See @ref setMenuAccelsEnabled .
     * @since 3.1
     */
    void setRemoveAmpersandsInCombo( bool b );
    /// @since 3.1
    bool removeAmpersandsInCombo() const;

    /**
     * Sets whether any occurence of the ampersand character ( &amp; ) in items
     * should be interpreted as keyboard accelerator for items displayed in a
     * menu or not.
     * @since 3.1
     */
    void setMenuAccelsEnabled( bool b );
    /// @since 3.1
    bool menuAccelsEnabled() const;

public slots:
    /**
     *  Sets the currently checked item.
     *
     *  @param index Index of the item (remember the first item is zero).
     */
    virtual void setCurrentItem( int index );

    /**
     * Sets the items to be displayed in this action
     * You need to call this.
     */
    virtual void setItems( const QStringList &lst );

    /**
     * Clears up all the items in this action
     */
    virtual void clear();

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This makes the combo editable or read-only.
     */
    virtual void setEditable( bool );

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This gives a _maximum_ size to the combobox.
     * The minimum size is automatically given by the contents (the items).
     */
    virtual void setComboWidth( int width );

protected:
    virtual void changeItem( int id, int index, const QString& text );

    /**
     * Depending on the menuAccelsEnabled property this method will return the
     * actions items in a way for inclusion in a combobox with the ampersand
     * character removed from all items or not.
     * @since 3.1
     */
    QStringList comboItems() const;

protected slots:
    virtual void slotActivated( int id );
    virtual void slotActivated( const QString &text );
    virtual void slotActivated();

signals:
    void activated( int index );
    void activated( const QString& text );

protected:
    virtual void updateCurrentItem( int id );

    virtual void updateComboWidth( int id );

    virtual void updateItems( int id );

    virtual void updateClear( int id );

protected:
    virtual void virtual_hook( int id, void* data );
private:
    void setupMenu() const;
    class KSelectActionPrivate;
    KSelectActionPrivate *d;

};

/// Remove this class in KDE-4.0. It doesn't add _anything_ to KSelectAction
/**
 * @deprecated Use KSelectAction instead.
 */
class KListAction : public KSelectAction
{
    Q_OBJECT
public:
    /**
     * Constructs a list action with text and potential keyboard
     * accelerator but nothing else. Use this only if you really
     * know what you are doing.
     *
     * @param text The text that will be displayed.
     * @param cut The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KListAction( const QString& text, const KShortcut& cut = KShortcut(), QObject* parent = 0,
                  const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const KShortcut& cut, const QObject* receiver,
                  const char* slot, QObject* parent, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QIconSet& pix, const KShortcut& cut = KShortcut(),
                      QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QString& pix, const KShortcut& cut = KShortcut(),
                      QObject* parent = 0, const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The icons that go with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                          const QObject* receiver, const char* slot, QObject* parent,
                  const char* name = 0 );

    /**
     *  @param text The text that will be displayed.
     *  @param pix The dynamically loaded icon that goes with this action.
     *  @param cut The corresponding keyboard accelerator (shortcut).
     *  @param receiver The SLOT's parent.
     *  @param slot The SLOT to invoke to execute this action.
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( const QString& text, const QString& pix, const KShortcut& cut,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );

    /**
     *  @param parent This action's parent.
     *  @param name An internal name for this action.
     */
    KListAction( QObject* parent = 0, const char* name = 0 );

    /**
     * Destructor
     */
    virtual ~KListAction();


    virtual QString currentText() const;
    virtual int currentItem() const;


public slots:
    /**
     *  Sets the currently checked item.
     *
     *  @param index Index of the item (remember the first item is zero).
     */
    virtual void setCurrentItem( int index );

protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KListActionPrivate;
    KListActionPrivate *d;
};

/**
 *  This class is an action to handle a recent files submenu.
 *  The best way to create the action is to use KStdAction::openRecent.
 *  Then you simply need to call @ref loadEntries on startup, @ref saveEntries
 *  on shutdown, @ref addURL when your application loads/saves a file.
 *
 *  @author Michael Koch
 *  @short Recent files action
 */
class KRecentFilesAction : public KListAction  // TODO public KSelectAction
{
  Q_OBJECT
  Q_PROPERTY( uint maxItems READ maxItems WRITE setMaxItems )
public:
  /**
   *  @param text The text that will be displayed.
   *  @param cut The corresponding keyboard accelerator (shortcut).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const KShortcut& cut,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param cut The corresponding keyboard accelerator (shortcut).
   *  @param receiver The SLOT's parent.
   *  @param slot The SLOT to invoke when a URL is selected.
   *  Its signature is of the form slotURLSelected( const KURL & ).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const KShortcut& cut,
                      const QObject* receiver, const char* slot,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The icons that go with this action.
   *  @param cut The corresponding keyboard accelerator (shortcut).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The dynamically loaded icon that goes with this action.
   *  @param cut The corresponding keyboard accelerator (shortcut).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QString& pix, const KShortcut& cut,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The icons that go with this action.
   *  @param cut The corresponding keyboard accelerator (shortcut).
   *  @param receiver The SLOT's parent.
   *  @param slot The SLOT to invoke when a URL is selected.
   *  Its signature is of the form slotURLSelected( const KURL & ).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                      const QObject* receiver, const char* slot,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param text The text that will be displayed.
   *  @param pix The dynamically loaded icon that goes with this action.
   *  @param cut The corresponding keyboard accelerator (shortcut).
   *  @param receiver The SLOT's parent.
   *  @param slot The SLOT to invoke when a URL is selected.
   *  Its signature is of the form slotURLSelected( const KURL & ).
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( const QString& text, const QString& pix, const KShortcut& cut,
                      const QObject* receiver, const char* slot,
                      QObject* parent, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  @param parent This action's parent.
   *  @param name An internal name for this action.
   *  @param maxItems The maximum number of files to display
   */
  KRecentFilesAction( QObject* parent = 0, const char* name = 0,
                      uint maxItems = 10 );

  /**
   *  Destructor.
   */
  virtual ~KRecentFilesAction();

  /**
   *  Returns the maximum of items in the recent files list.
   */
  uint maxItems() const;

public slots:
  /**
   *  Sets the maximum of items in the recent files list.
   *  The default for this value is 10 set in the constructor.
   *
   *  If this value is lesser than the number of items currently
   *  in the recent files list the last items are deleted until
   *  the number of items are equal to the new maximum.
   */
  void setMaxItems( uint maxItems );

  /**
   *  Loads the recent files entries from a given KConfig object.
   *  You can provide the name of the group used to load the entries.
   *  If the groupname is empty, entries are load from a group called 'RecentFiles'
   *
   *  This method does not effect the active group of KConfig.
   */
  void loadEntries( KConfig* config, QString groupname=QString::null );

  /**
   *  Saves the current recent files entries to a given KConfig object.
   *  You can provide the name of the group used to load the entries.
   *  If the groupname is empty, entries are saved to a group called 'RecentFiles'
   *
   *  This method does not effect the active group of KConfig.
   */
  void saveEntries( KConfig* config, QString groupname=QString::null );

public slots:
  /**
   *  Add URL to recent files list.
   *
   *  @param url The URL of the file
   */
  void addURL( const KURL& url );

  /**
   *  Remove an URL from the recent files list.
   *
   *  @param url The URL of the file
   */
  void removeURL( const KURL& url );

  /**
   *  Removes all entries from the recent files list.
   */
  void clearURLList();

signals:

  /**
   *  This signal gets emited when the user selects an URL.
   *
   *  @param url The URL thats the user selected.
   */
  void urlSelected( const KURL& url );

protected slots:
  /**
   *
   */
  void itemSelected( const QString& string );

protected:
  virtual void virtual_hook( int id, void* data );
private:
  void init();

  class KRecentFilesActionPrivate;
  KRecentFilesActionPrivate *d;
};

class KFontAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY( QString font READ font WRITE setFont )
public:
    KFontAction( const QString& text, const KShortcut& cut = KShortcut(), QObject* parent = 0,
                 const char* name = 0 );
    KFontAction( const QString& text, const KShortcut& cut,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );
    KFontAction( const QString& text, const QIconSet& pix, const KShortcut& cut = KShortcut(),
                 QObject* parent = 0, const char* name = 0 );
    KFontAction( const QString& text, const QString& pix, const KShortcut& cut = KShortcut(),
                 QObject* parent = 0, const char* name = 0 );
    KFontAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );
    KFontAction( const QString& text, const QString& pix, const KShortcut& cut,
                 const QObject* receiver, const char* slot, QObject* parent,
                 const char* name = 0 );

    KFontAction( QObject* parent = 0, const char* name = 0 );
    ~KFontAction();

    QString font() const {
        return currentText();
    }

    int plug( QWidget*, int index = -1 );

public slots:
    void setFont( const QString &family );

protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KFontActionPrivate;
    KFontActionPrivate *d;
};

class KFontSizeAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY( int fontSize READ fontSize WRITE setFontSize )
public:
    KFontSizeAction( const QString& text, const KShortcut& cut = KShortcut(), QObject* parent = 0,
                     const char* name = 0 );
    KFontSizeAction( const QString& text, const KShortcut& cut, const QObject* receiver,
                     const char* slot, QObject* parent, const char* name = 0 );
    KFontSizeAction( const QString& text, const QIconSet& pix, const KShortcut& cut = KShortcut(),
                     QObject* parent = 0, const char* name = 0 );
    KFontSizeAction( const QString& text, const QString& pix, const KShortcut& cut = KShortcut(),
                     QObject* parent = 0, const char* name = 0 );
    KFontSizeAction( const QString& text, const QIconSet& pix, const KShortcut& cut,
                     const QObject* receiver, const char* slot,
                     QObject* parent, const char* name = 0 );
    KFontSizeAction( const QString& text, const QString& pix, const KShortcut& cut,
                     const QObject* receiver, const char* slot,
                     QObject* parent, const char* name = 0 );
    KFontSizeAction( QObject* parent = 0, const char* name = 0 );

    virtual ~KFontSizeAction();

    virtual int fontSize() const;

public slots:
    virtual void setFontSize( int size );

protected slots:
    virtual void slotActivated( int );
    virtual void slotActivated( const QString& );
    virtual void slotActivated() { KAction::slotActivated(); }

signals:
    void fontSizeChanged( int );

private:
    void init();


protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KFontSizeActionPrivate;
    KFontSizeActionPrivate *d;
};


/**
 * A KActionMenu is an action that holds a sub-menu of other actions.
 * insert() and remove() allow to insert and remove actions into this action-menu.
 * Plugged in a popupmenu, it will create a submenu.
 * Plugged in a toolbar, it will create a button with a popup menu.
 *
 * This is the action used by the XMLGUI since it holds other actions.
 * If you want a submenu for selecting one tool among many (without icons), see KSelectAction.
 * See also setDelayed about the main action.
 */
class KActionMenu : public KAction
{
  Q_OBJECT
  Q_PROPERTY( bool delayed READ delayed WRITE setDelayed )
  Q_PROPERTY( bool stickyMenu READ stickyMenu WRITE setStickyMenu )

public:
    KActionMenu( const QString& text, QObject* parent = 0,
                 const char* name = 0 );
    KActionMenu( const QString& text, const QIconSet& icon,
                 QObject* parent = 0, const char* name = 0 );
    KActionMenu( const QString& text, const QString& icon,
                 QObject* parent = 0, const char* name = 0 );
    KActionMenu( QObject* parent = 0, const char* name = 0 );
    virtual ~KActionMenu();

    virtual void insert( KAction*, int index = -1 );
    virtual void remove( KAction* );

//US    KPopupMenu* popupMenu() const;
    QPopupMenu* popupMenu() const;
    void popup( const QPoint& global );

    /**
     * Returns true if this action creates a delayed popup menu
     * when plugged in a KToolbar.
     */
    bool delayed() const;
    /**
     * If set to true, this action will create a delayed popup menu
     * when plugged in a KToolbar. Otherwise it creates a normal popup.
     * Default: delayed
     *
     * Remember that if the "main" action (the toolbar button itself)
     * cannot be clicked, then you should call setDelayed(false).
     *
     * On the opposite, if the main action can be clicked, it can only happen
     * in a toolbar: in a menu, the parent of a submenu can't be activated.
     * To get a "normal" menu item when plugged a menu (and no submenu)
     * use KToolBarPopupAction.
     */
    void setDelayed(bool _delayed);

    /**
     * Returns true if this action creates a sticky popup menu.
     * See @ref setStickyMenu.
     */
    bool stickyMenu() const;
    /**
     * If set to true, this action will create a sticky popup menu
     * when plugged in a KToolbar.
     * "Sticky", means it's visible until a selection is made or the mouse is
     * clicked elsewhere. This feature allows you to make a selection without
     * having to press and hold down the mouse while making a selection.
     * Default: sticky.
     */
    void setStickyMenu(bool sticky);

    virtual int plug( QWidget* widget, int index = -1 );

protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KActionMenuPrivate;
    KActionMenuPrivate *d;
};

/**
 * This action is a normal action everywhere, except in a toolbar
 * where it also has a popupmenu (optionnally delayed). This action is designed
 * for history actions (back/forward, undo/redo) and for any other action
 * that has more detail in a toolbar than in a menu (e.g. tool chooser
 * with "Other" leading to a dialog...).
 */
class KToolBarPopupAction : public KAction
{
  Q_OBJECT
  Q_PROPERTY( bool delayed READ delayed WRITE setDelayed )
  Q_PROPERTY( bool stickyMenu READ stickyMenu WRITE setStickyMenu )

public:
    //Not all constructors - because we need an icon, since this action only makes
    // sense when being plugged at least in a toolbar.
    /**
     * Create a KToolBarPopupAction, with a text, an icon, an optionnal accelerator,
     * parent and name.
     *
     * @param text The text that will be displayed.
     * @param icon The icon to display.
     * @param cut The corresponding keyboard accelerator (shortcut).
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KToolBarPopupAction( const QString& text, const QString& icon, const KShortcut& cut = KShortcut(),
                         QObject* parent = 0, const char* name = 0 );

    /**
     * Create a KToolBarPopupAction, with a text, an icon, an accelerator,
     * a slot connected to the action, parent and name.
     *
     * If you do not want or have a keyboard accelerator, set the
     * @p cut param to 0.
     *
     * @param text The text that will be displayed.
     * @param icon The icon to display.
     * @param cut The corresponding keyboard accelerator (shortcut).
     * @param receiver The SLOT's owner.
     * @param slot The SLOT to invoke to execute this action.
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
    KToolBarPopupAction( const QString& text, const QString& icon, const KShortcut& cut,
                         const QObject* receiver, const char* slot,
                         QObject* parent = 0, const char* name = 0 );

    /**
     * Create a KToolBarPopupAction, with a KGuiItem, an accelerator,
     * a slot connected to the action, parent and name. The text and the
     * icon are taken from the KGuiItem.
     *
     * If you do not want or have a keyboard accelerator, set the
     * @p cut param to 0.
     *
     * @param item The text and icon that will be displayed.
     * @param cut The corresponding keyboard accelerator (shortcut).
     * @param receiver The SLOT's owner.
     * @param slot The SLOT to invoke to execute this action.
     * @param parent This action's parent.
     * @param name An internal name for this action.
     */
     
    KToolBarPopupAction( const KGuiItem& item, const KShortcut& cut,
                         const QObject* receiver, const char* slot,
                         KActionCollection* parent, const char* name );
    
    virtual ~KToolBarPopupAction();

    virtual int plug( QWidget *widget, int index = -1 );

    /**
     * The popup menu that is shown when clicking (some time) on the toolbar
     * button. You may want to plug items into it on creation, or connect to
     * aboutToShow for a more dynamic menu.
     */
//US    KPopupMenu *popupMenu() const;
    QPopupMenu *popupMenu() const;

    /**
     * Returns true if this action creates a delayed popup menu
     * when plugged in a KToolbar.
     */
    bool delayed() const;
    /**
     * If set to true, this action will create a delayed popup menu
     * when plugged in a KToolbar. Otherwise it creates a normal popup.
     * Default: delayed.
     */
    void setDelayed(bool delayed);
    /**
     * Returns true if this action creates a sticky popup menu.
     * See @ref setStickyMenu.
     */
    bool stickyMenu() const;
    /**
     * If set to true, this action will create a sticky popup menu
     * when plugged in a KToolbar.
     * "Sticky", means it's visible until a selection is made or the mouse is
     * clicked elsewhere. This feature allows you to make a selection without
     * having to press and hold down the mouse while making a selection.
     * Only available if delayed() is true.
     * Default: sticky.
     */
    void setStickyMenu(bool sticky);

private:
//US    KPopupMenu *m_popup;
    QPopupMenu *m_popup;
    bool m_delayed:1;
    bool m_stickyMenu:1;
protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KToolBarPopupActionPrivate;
    KToolBarPopupActionPrivate *d;
};

/**
 * An action that takes care of everything associated with
 * showing or hiding a toolbar by a menu action. It will
 * show or hide the toolbar with the given name when
 * activated, and check or uncheck itself if the toolbar
 * is manually shown or hidden.
 *
 * If you need to perfom some additional action when the
 * toolbar is shown or hidden, connect to the toggled(bool)
 * signal. It will be emitted after the toolbar's
 * visibility has changed, whenever it changes.
 * @since 3.1
 */
class KToggleToolBarAction : public KToggleAction
{
    Q_OBJECT
public:
    /**
     * Create a KToggleToolbarAction that manages the toolbar
     * named toolBarName. This can be either the name of a
     * toolbar in an xml ui file, or a toolbar programmatically
     * created with that name.
     */
    KToggleToolBarAction( const char* toolBarName, const QString& text,
                          KActionCollection* parent, const char* name );
    KToggleToolBarAction( KToolBar *toolBar, const QString &text,
                          KActionCollection *parent, const char *name );
    virtual ~KToggleToolBarAction();

    virtual int plug( QWidget*, int index = -1 );

    KToolBar *toolBar() {
      return m_toolBar;
    }

public slots:
    virtual void setChecked( bool );

private:
    QCString               m_toolBarName;
    QGuardedPtr<KToolBar>  m_toolBar;
protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KToggleToolBarActionPrivate;
    KToggleToolBarActionPrivate *d;
};

/**
 * An action that automatically embeds a widget into a
 * toolbar.
 */
class KWidgetAction : public KAction
{
    Q_OBJECT
public:
    /**
     * Create an action that will embed widget into a toolbar
     * when plugged. This action may only be plugged into
     * a toolbar.
     */
    KWidgetAction( QWidget* widget, const QString& text,
                   const KShortcut& cut,
                   const QObject* receiver, const char* slot,
                   KActionCollection* parent, const char* name );
    virtual ~KWidgetAction();

    /**
     * Returns the widget associated with this action.
     */
    QWidget* widget() { return m_widget; }

    void setAutoSized( bool );

    /**
     * Plug the action. The widget passed to the constructor
     * will be reparented to w, which must inherit KToolBar.
     */
    virtual int plug( QWidget* w, int index = -1 );
    /**
     * Unplug the action. Ensures that the action is not
     * destroyed. It will be hidden and reparented to 0L instead.
     */
    virtual void unplug( QWidget *w );
protected slots:
    void slotToolbarDestroyed();
private:
    QGuardedPtr<QWidget> m_widget;
    bool                 m_autoSized;
protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KWidgetActionPrivate;
    KWidgetActionPrivate *d;
};

class KActionSeparator : public KAction
{
    Q_OBJECT
public:
    KActionSeparator( QObject* parent = 0, const char* name = 0 );
    virtual ~KActionSeparator();

    virtual int plug( QWidget*, int index = -1 );

protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KActionSeparatorPrivate;
    KActionSeparatorPrivate *d;
};

#endif
