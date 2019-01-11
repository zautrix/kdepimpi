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
//$Id: kactioncollection.h,v 1.1.1.1 2004/06/26 19:02:36 zautrix Exp $

#ifndef __kactioncollection_h__
#define __kactioncollection_h__

#include <kaction.h>

//US #include <qkeysequence.h>
#include <qobject.h>
//US#include <qvaluelist.h>
//US#include <qguardedptr.h>
//US #include <kguiitem.h>
//US#include <kshortcut.h>
//US#include <kstdaction.h>
//US#include <kicontheme.h>

//USclass QMenuBar;
//USclass QPopupMenu;
//USclass QComboBox;
//USclass QPoint;
//USclass QIconSet;
//USclass QString;
//USclass KToolBar;

//USclass KAccel;
//USclass KAccelActions;
//USclass KConfig;
//USclass KConfigBase;
//USclass KURL;
//USclass KInstance;
//USclass KToolBar;
//USclass KActionCollection;
//USclass KPopupMenu;
//USclass KMainWindow;

//US added inclidefiles
class QWidget;


typedef QValueList<KAction *> KActionPtrList;

/**
 * A managed set of KAction objects.
 */
class KActionCollection : public QObject
{
  friend class KAction;
  friend class KXMLGUIClient;

  Q_OBJECT
public:
  KActionCollection( QWidget *parent, const char *name = 0/*US , KInstance *instance = 0 */);
  /**
   * Use this constructor if you want the collection's actions to restrict
   * their accelerator keys to @p watch rather than the @p parent.  If
   * you don't require shortcuts, you can pass a null to the @p watch parameter.
   */
  KActionCollection( QWidget *watch, QObject* parent, const char *name = 0/*US, KInstance *instance = 0 */);
  KActionCollection( const KActionCollection &copy );
  virtual ~KActionCollection();

  /**
   * This sets the widget to which the keyboard shortcuts should be attached.
   * You only need to call this if a null pointer was passed in the constructor.
   */
  virtual void setWidget( QWidget *widget );

  /**
   * This indicates whether new actions which are created in this collection
   * should have their keyboard shortcuts automatically connected on
   * construction.  Set to 'false' if you will be loading XML-based settings.
   * This is automatically done by KParts.  The default is 'true'.
   * @see isAutoConnectShortcuts()
   */
  void setAutoConnectShortcuts( bool );

  /**
   * This indicates whether new actions which are created in this collection
   * have their keyboard shortcuts automatically connected on
   * construction.
   * @see setAutoConnectShortcuts()
   */
  bool isAutoConnectShortcuts();

  /**
   * This sets the default shortcut scope for new actions created in this
   * collection.  The default is ScopeUnspecified.  Ideally the default
   * would have been ScopeWidget, but that would cause some backwards
   * compatibility problems.
   */
  //void setDefaultScope( KAction::Scope );

  /**
   * Doc/View model.  This lets you add the action collection of a document
   * to a view's action collection.
   */
  bool addDocCollection( KActionCollection* pDoc );

  /** Returns the number of widgets which this collection is associated with. */
  //uint widgetCount() const;

  /**
   * Returns true if the collection has its own KAccel object.  This will be
   * the case if it was constructed with a valid widget ptr or if setWidget()
   * was called.
   */
  //bool ownsKAccel() const;

  /** @deprecated  Deprecated because of ambiguous name.  Use kaccel() */
  virtual KAccel* accel();
  /** @deprecated  Deprecated because of ambiguous name.  Use kaccel() */
  virtual const KAccel* accel() const;

  /** Returns the KAccel object of the most recently set widget. */
  KAccel* kaccel();
  /** Returns the KAccel object of the most recently set widget. Const version for convenience. */
  const KAccel* kaccel() const;

  /** @internal, for KAction::kaccelCurrent() */
  KAccel* builderKAccel() const;
  /** Returns the KAccel object associated with widget #. */
  //KAccel* widgetKAccel( uint i );
  //const KAccel* widgetKAccel( uint i ) const;

  /** Returns the number of actions in the collection */
  virtual uint count() const;
  bool isEmpty() const { return count() == 0; }
  /**
   * Return the KAction* at position "index" in the action collection.
   * @see count()
   */
  virtual KAction* action( int index ) const;
  /**
   * Find an action (optionally, of a given subclass of KAction) in the action collection.
   * @param name Name of the KAction.
   * @param classname Name of the KAction subclass.
   * @return A pointer to the first KAction in the collection which matches the parameters or
   * null if nothing matches.
   */
  virtual KAction* action( const char* name, const char* classname = 0 ) const;

  /** Returns a list of all the groups of all the KActions in this action collection.
   * @see KAction::group()
   * @see KAction::setGroup()
   */
  virtual QStringList groups() const;
  /**
   * Returns the list of actions in a particular managed by this action collection.
   * @param group The name of the group.
   */
  virtual KActionPtrList actions( const QString& group ) const;
  /** Returns the list of actions managed by this action collection. */
  virtual KActionPtrList actions() const;

  /**
   * Used for reading shortcut configuration from a non-XML rc file.
   */
//US  bool readShortcutSettings( const QString& sConfigGroup = QString::null, KConfigBase* pConfig = 0 );
  /**
   * Used for writing shortcut configuration to a non-XML rc file.
   */
//US  bool writeShortcutSettings( const QString& sConfigGroup = QString::null, KConfigBase* pConfig = 0 ) const;

//US  void setInstance( KInstance *instance );
  /** The instance with which this class is associated. */
//US  KInstance *instance() const;

  /**
   * Use this to tell the KActionCollection what rc file its configuration
   * is stored in.
   */
  void setXMLFile( const QString& );
  /** The rc file in which the current configuration is stored. */
  const QString& xmlFile() const;

  /**
   * Enable highlighting notification for specific KActions.
   * @see connectHighlight()
   * @see disconnectHighlight()
   * @see actionHighlighted()
   * @see actionHighlighted()
   * @see highlightingEnabled()
   */
  void setHighlightingEnabled( bool enable );
  /**
   * Return whether highlighting notifications are enabled.
   * @see connectHighlight()
   * @see disconnectHighlight()
   * @see actionHighlighted()
   * @see setHighlightingEnabled()
   * @see actionHighlighted()
   */
  bool highlightingEnabled() const;

  /**
   * Call this function if you want to receive a signal whenever a KAction is highlighted in a menu or a toolbar.
   * @param container A container in which the KAction is plugged (must inherit QPopupMenu or KToolBar)
   * @param action The action you are interested in
   * @see disconnectHighlight()
   * @see actionHighlighted()
   * @see setHighlightingEnabled()
   * @see highlightingEnabled()
   * @see actionHighlighted()
   */
  void connectHighlight( QWidget *container, KAction *action );
  /**
   * Disconnect highlight notifications for a particular pair of contianer and action.
   * @param container A container in which the KAction is plugged (must inherit QPopupMenu or KToolBar)
   * @param action The action you are interested in
   * @see connectHighlight()
   * @see actionHighlighted()
   * @see setHighlightingEnabled()
   * @see highlightingEnabled()
   * @see actionHighlighted()
   */
  void disconnectHighlight( QWidget *container, KAction *action );

signals:
  void inserted( KAction* );
  void removed( KAction* );

  /** Emitted when "action" is highlighted.
   * @see connectHighlight()
   * @see disconnectHighlight()
   * @see actionHighlighted()
   * @see setHighlightingEnabled()
   * @see highlightingEnabled()
   */
  void actionHighlighted( KAction *action );
  /** Emitted when "action" is highlighed or loses highlighting.
   * @see connectHighlight()
   * @see disconnectHighlight()
   * @see actionHighlighted()
   * @see setHighlightingEnabled()
   * @see highlightingEnabled()
   */
  void actionHighlighted( KAction *action, bool highlight );

  void actionStatusText( const QString &text );
  void clearStatusText();

private:
  /**
   * @internal Only to be called by KXMLGUIFactory::addClient().
   * When actions are being connected, KAction needs to know what
   * widget it should connect widget-scope actions to, and what
   * main window it should connect
   */
  void beginXMLPlug( QWidget *widget );
  void endXMLPlug();
  /** @internal.  Only to be called by KXMLGUIFactory::removeClient() */
  void prepareXMLUnplug();
  void unplugShortcuts( KAccel* kaccel );

  void _clear();
  void _insert( KAction* );
  void _remove( KAction* );
  KAction* _take( KAction* );

private slots:
   void slotMenuItemHighlighted( int id );
   void slotToolBarButtonHighlighted( int id, bool highlight );
   void slotMenuAboutToHide();
   void slotDestroyed();

private:
   KAction *findAction( QWidget *container, int id );

#ifndef KDE_NO_COMPAT
public:
  KActionCollection( QObject *parent, const char *name = 0 /*US, KInstance *instance = 0 */);

  void insert( KAction* );

  /**
   * @deprecated Removes an action from the collection and deletes it.
   * @param action The KAction to remove.
   */
  void remove( KAction* action );

  /**
   * @deprecated Removes an action from the collection.
   * @return NULL if not found else returns action.
   * @param action the KAction to remove.
   */
  KAction* take( KAction* action );

  KActionCollection operator+ ( const KActionCollection& ) const;
  KActionCollection& operator= ( const KActionCollection& );
  KActionCollection& operator+= ( const KActionCollection& );

public slots:
  /**
   * Clears the entire actionCollection, deleting all actions.
   * @see #remove
   */
  void clear();
#endif // !KDE_NO_COMPAT
protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KActionCollectionPrivate;
    KActionCollectionPrivate *d;
};

#endif
