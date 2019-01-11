 /* This file is part of the KDE libraries
     Copyright
     (C) 2000 Reginald Stadlbauer (reggie@kde.org)
     (C) 1997 Stephan Kulow (coolo@kde.org)
     (C) 1997-2000 Sven Radej (radej@kde.org)
     (C) 1997-2000 Matthias Ettrich (ettrich@kde.org)
     (C) 1999 Chris Schlaeger (cs@kde.org)
     (C) 2002 Joseph Wenninger (jowenn@kde.org)

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
#include <qobjectlist.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qapplication.h>

 
#include "kdebug.h"
#include "kmainwindow.h"
#include "kglobalsettings.h"
#include "kactioncollection.h"

class KMainWindowPrivate {
public:
//US    bool showHelpMenu:1;

    bool autoSaveSettings:1;
    bool settingsDirty:1;
    bool autoSaveWindowSize:1;
    bool care_about_geometry:1;
    QString autoSaveGroup;
//US    KAccel * kaccel;
//US    KMainWindowInterface *m_interface;
    KDEPrivate::ToolBarHandler *toolBarHandler;
    QTimer* settingsTimer;
    KToggleAction *showStatusBarAction;
    QRect defaultWindowSize;
};

static bool no_query_exit = false;

KMainWindow::KMainWindow( QWidget* parent, const char *name )
    : QMainWindow( parent, name ) /*LR, f ) with the default widget flag we cannot have fastload */ /*US, KXMLGUIBuilder( this ), helpMenu2( 0 ), factory_( 0 )*/
{
    mQToolBar = 0;
    initKMainWindow(name);
}

void KMainWindow::parseGeometry(bool parsewidth)
{
//US the following code is not getting used in the embedded version !! So disable it for now   
/*US    
    
    assert ( !kapp->geometryArgument().isNull() );
    assert ( d->care_about_geometry );

#ifndef Q_WS_QWS
    // FIXME: (E) Implement something similar for Qt Embedded (or decide we don't need it)
    int x, y;
    int w, h;
    int m = XParseGeometry( kapp->geometryArgument().latin1(), &x, &y, (unsigned int*)&w, (unsigned int*)&h);
    if (parsewidth) {
        QSize minSize = minimumSize();
        QSize maxSize = maximumSize();
        if ( (m & WidthValue) == 0 )
            w = width();
        if ( (m & HeightValue) == 0 )
            h = height();
         w = QMIN(w,maxSize.width());
         h = QMIN(h,maxSize.height());
         w = QMAX(w,minSize.width());
         h = QMAX(h,minSize.height());
         resize(w, h);
    } else {
        if ( parsewidth && (m & XValue) == 0 )
            x = geometry().x();
        if ( parsewidth && (m & YValue) == 0 )
            y = geometry().y();
        if ( (m & XNegative) )
            x = KApplication::desktop()->width()  + x - w;
        if ( (m & YNegative) )
            y = KApplication::desktop()->height() + y - h;
        move(x, y);
    }
#endif
*/    
}

KMainWindow::~KMainWindow()
{
    delete d->settingsTimer;
    QMenuBar* mb = internalMenuBar();
    delete mb;
//US    delete d->m_interface;
    
    delete d;
//US    memberList->remove( this );
}

void KMainWindow::initKMainWindow(const char *name)
{
    setDockMenuEnabled( FALSE );
//US     mHelpMenu = 0;
    
//US    kapp->setTopWidget( this );
    actionCollection()->setWidget( this );
//US    connect(kapp, SIGNAL(shutDown()), this, SLOT(shuttingDown()));
//US    if( !memberList )
//US        memberList = new QPtrList<KMainWindow>;
/*US    

    if ( !ksm )
        ksm = ksmd.setObject(new KMWSessionManaged());
    // set a unique object name. Required by session management.
    QCString objname;
    QCString s;
    int unusedNumber;
    if ( !name )
        { // no name given
        objname = kapp->instanceName() + "-mainwindow#";
        s = objname + '1'; // start adding number immediately
        unusedNumber = 1;
        }
    else if( name[ strlen( name ) - 1 ] == '#' )
        { // trailing # - always add a number
        objname = name;
        s = objname + '1'; // start adding number immediately
        unusedNumber = 1;
        }
    else
        {
        objname = name;
        s = objname;
        unusedNumber = 0; // add numbers only when needed
        }
    for(;;) {
        QWidgetList* list = kapp->topLevelWidgets();
        QWidgetListIt it( *list );
        bool found = false;
        for( QWidget* w = it.current();
             w != NULL;
             ++it, w = it.current())
            if( w != this && w->name() == s )
                {
                found = true;
                break;
                }
        delete list;
        if( !found )
            break;
        s.setNum( ++unusedNumber );
        s = objname + s;
    }
    setName( s );
    memberList->append( this );
*/
    
    d = new KMainWindowPrivate;
//US    d->showHelpMenu = true;
    d->settingsDirty = false;
    d->autoSaveSettings = false;
    d->autoSaveWindowSize = true; // for compatibility
//US    d->kaccel = actionCollection()->kaccel();
    d->toolBarHandler = 0;
    d->settingsTimer = 0;
    d->showStatusBarAction = NULL;
/*US    
    if ((d->care_about_geometry == beeing_first)) {
        beeing_first = false;
        if ( kapp->geometryArgument().isNull() ) // if there is no geometry, it doesn't mater
            d->care_about_geometry = false;
        else
            parseGeometry(false);
    }
*/    
    d->care_about_geometry = false;

//US    setCaption( kapp->caption() );
    // attach dcop interface
//US    d->m_interface = new KMainWindowInterface(this);

//US    if (!kapp->authorize("movable_toolbars"))
//US        setDockWindowsMovable(false);
}

KAction *KMainWindow::toolBarMenuAction()
{
  if ( !d->toolBarHandler )
    return 0;

  return d->toolBarHandler->toolBarMenuAction();
}

bool KMainWindow::canBeRestored( int number )
{
/*US we do not have and want to save sessioninformation. Use info from the default 
application config.
*/
//US    if ( !kapp->isRestored() )
//US        return FALSE;
//US    KConfig *config = kapp->sessionConfig();
    KConfig *config = KGlobal::config();
    if ( !config )
        return FALSE;
    config->setGroup( QString::fromLatin1("Number") );
    int n = config->readNumEntry( QString::fromLatin1("NumberOfWindows") , 1 );
    return number >= 1 && number <= n;

}

const QString KMainWindow::classNameOfToplevel( int number )
{
/*US we do not have and want to save sessioninformation. Use info from the default 
application config.
*/
//US    if ( !kapp->isRestored() )
//US        return QString::null;
//US    KConfig *config = kapp->sessionConfig();
    KConfig *config = KGlobal::config();
    if ( !config )
        return QString::null;
    QString s;
    s.setNum( number );
    s.prepend( QString::fromLatin1("WindowProperties") );
    config->setGroup( s );
    if ( !config->hasKey( QString::fromLatin1("ClassName") ) )
        return QString::null;
    else
        return config->readEntry( QString::fromLatin1("ClassName") );
}

bool KMainWindow::restore( int number, bool show )
{
/*US we do not have and want to save sessioninformation. Use info from the default 
application config.
*/
    if ( !canBeRestored( number ) )
        return FALSE;
//US    KConfig *config = kapp->sessionConfig();
    KConfig *config = KGlobal::config();

    if ( readPropertiesInternal( config, number ) ){
        if ( show )
            KMainWindow::show();
        return FALSE;
    }
    return FALSE;
    
}

void KMainWindow::setCaption( const QString &caption )
{
//US    setPlainCaption( kapp->makeStdCaption(caption) );
    setPlainCaption( caption );
}

void KMainWindow::setCaption( const QString &caption, bool modified )
{
//US    setPlainCaption( kapp->makeStdCaption(caption, true, modified) );
    setPlainCaption( caption + "modified:" );
}

void KMainWindow::setPlainCaption( const QString &caption )
{
    QMainWindow::setCaption( caption );
#ifndef Q_WS_QWS
//US the following is disabled for the embedded version
//US    NETWinInfo info( qt_xdisplay(), winId(), qt_xrootwin(), 0 );
//US    info.setName( caption.utf8().data() );
#endif
}

void KMainWindow::slotStateChanged(const QString &newstate)
{
  stateChanged(newstate, KXMLGUIClient::StateNoReverse);
}

/*
 * Get rid of this for KDE 4.0
 */
void KMainWindow::slotStateChanged(const QString &newstate,
                                   KXMLGUIClient::ReverseStateChange reverse)
{
  stateChanged(newstate, reverse);
}

void KMainWindow::closeEvent ( QCloseEvent *e )
{
    //qDebug("MainWindow::closeEvent  ");
    // Save settings if auto-save is enabled, and settings have changed
    if (d->settingsDirty && d->autoSaveSettings)
        saveAutoSaveSettings();

    if (queryClose()) {
        e->accept();

        int not_withdrawn = 0;
/*US        
        QPtrListIterator<KMainWindow> it(*KMainWindow::memberList);
        for (it.toFirst(); it.current(); ++it){
            if ( !it.current()->isHidden() && it.current()->isTopLevel() && it.current() != this )
                not_withdrawn++;
        }
*/
        if ( !no_query_exit && not_withdrawn <= 0 ) { // last window close accepted?
/*US            
            if ( queryExit() && !kapp->sessionSaving()) {            // Yes, Quit app?
                // don't call queryExit() twice
                disconnect(kapp, SIGNAL(shutDown()), this, SLOT(shuttingDown()));
                kapp->deref();             // ...and quit aplication.
            }  else {
                // cancel closing, it's stupid to end up with no windows at all....
                e->ignore();
            }
*/
//US we have no sessionmanagement. Simply close app.
            if ( queryExit() ) {            // Yes, Quit app?
            qDebug("KMainWindow::closeEvent: Exit application ???");
                // don't call queryExit() twice
//US                disconnect(kapp, SIGNAL(shutDown()), this, SLOT(shuttingDown()));
            } 
            
        }
    }
}

bool KMainWindow::queryExit()
{
    return TRUE;
}

bool KMainWindow::queryClose()
{
    return TRUE;
}

void KMainWindow::saveGlobalProperties( KConfig*  )
{
}

void KMainWindow::readGlobalProperties( KConfig*  )
{
}

void KMainWindow::savePropertiesInternal( KConfig *config, int number )
{
    bool oldASWS = d->autoSaveWindowSize;
    d->autoSaveWindowSize = true; // make saveMainWindowSettings save the window size

    QString s;
    s.setNum(number);
    s.prepend(QString::fromLatin1("WindowProperties"));
    config->setGroup(s);

    // store objectName, className, Width and Height  for later restoring
    // (Only useful for session management)
    config->writeEntry(QString::fromLatin1("ObjectName"), name());
    config->writeEntry(QString::fromLatin1("ClassName"), className());

    saveMainWindowSettings(config); // Menubar, statusbar and Toolbar settings.

    s.setNum(number);
    config->setGroup(s);
    saveProperties(config);

    d->autoSaveWindowSize = oldASWS;
}

void KMainWindow::setStandardToolBarMenuEnabled( bool enable )
{
  if ( enable )
  {
    if ( d->toolBarHandler )
      return;

  d->toolBarHandler = new KDEPrivate::ToolBarHandler( this );

/*US  if ( factory() )
    factory()->addClient( d->toolBarHandler );
*/    
  }
  else
  {
    if ( !d->toolBarHandler )
      return;
/*US
    if ( factory() )
      factory()->removeClient( d->toolBarHandler );
*/
    delete d->toolBarHandler;
    d->toolBarHandler = 0;
  }
  
}

bool KMainWindow::isStandardToolBarMenuEnabled() const
{
    return ( d->toolBarHandler != 0 );
}

void KMainWindow::createStandardStatusBarAction(){
  if(!d->showStatusBarAction){
    d->showStatusBarAction = KStdAction::showStatusbar(this, SLOT(setSettingsDirty()), actionCollection());
    connect(d->showStatusBarAction, SIGNAL(toggled(bool)), statusBar(), SLOT(setShown(bool)));
    if(internalStatusBar())
      d->showStatusBarAction->setChecked(!internalStatusBar()->isHidden());
  }
}

QToolBar *KMainWindow::tBar(  )
{
    if  ( ! mQToolBar )
      mQToolBar =   new QToolBar( this );
    return mQToolBar;
}

KToolBar *KMainWindow::toolBar( const char * name )
{
   
    if (!name)
       name = "mainToolBar";
    KToolBar *tb = (KToolBar*)child( name, "KToolBar" );
    if ( tb )
        return tb;
    bool honor_mode = (name == "mainToolBar");

/*US
    if ( builderClient() )
        return new KToolBar(this, name, honor_mode); // XMLGUI constructor
    else
*/    
      return new KToolBar(this, Top, false, name, honor_mode ); // non-XMLGUI
}

QPtrListIterator<KToolBar> KMainWindow::toolBarIterator()
{
    toolbarList.clear();
    QPtrList<QToolBar> lst;
    for ( int i = (int)QMainWindow::Unmanaged; i <= (int)Minimized; ++i ) {
        lst = toolBars( (ToolBarDock)i );
        for ( QToolBar *tb = lst.first(); tb; tb = lst.next() ) {
            if ( !tb->inherits( "KToolBar" ) )
                continue;
            toolbarList.append( (KToolBar*)tb );
        }
    }
    return QPtrListIterator<KToolBar>( toolbarList );
}

void KMainWindow::setAutoSaveSettings( const QString & groupName, bool saveWindowSize )
{
    d->autoSaveSettings = true;
    d->autoSaveGroup = groupName;
    d->autoSaveWindowSize = saveWindowSize;
    // Get notified when the user moves a toolbar around
//US    connect( this, SIGNAL( dockWindowPositionChanged( QDockWindow * ) ),
//US             this, SLOT( setSettingsDirty() ) );
    connect( this, SIGNAL( toolBarPositionChanged(QToolBar *) ),
             this, SLOT( setSettingsDirty() ) );

             
    // Get default values
//US    int scnum = QApplication::desktop()->screenNumber(parentWidget());
//US    QRect desk = QApplication::desktop()->screenGeometry(scnum);
    QRect desk = KGlobalSettings::desktopGeometry(0);
    
    d->defaultWindowSize = QRect(desk.width(), width(), desk.height(), height());
    // Now read the previously saved settings
    applyMainWindowSettings( KGlobal::config(), groupName );
}


void KMainWindow::resetAutoSaveSettings()
{
    d->autoSaveSettings = false;
    if ( d->settingsTimer )
        d->settingsTimer->stop();
}

bool KMainWindow::autoSaveSettings() const
{
    return d->autoSaveSettings;
}

QString KMainWindow::autoSaveGroup() const
{
    return d->autoSaveGroup;
}

void KMainWindow::saveAutoSaveSettings()
{
    ASSERT( d->autoSaveSettings );
    //kdDebug(200) << "KMainWindow::saveAutoSaveSettings -> saving settings" << endl;
    saveMainWindowSettings( KGlobal::config(), d->autoSaveGroup );
    KGlobal::config()->sync();
    d->settingsDirty = false;
    if ( d->settingsTimer )
        d->settingsTimer->stop();
}

void KMainWindow::createGUI( const QString &xmlfile, bool _conserveMemory )
{
    // disabling the updates prevents unnecessary redraws
    setUpdatesEnabled( false );

    // just in case we are rebuilding, let's remove our old client
//US    guiFactory()->removeClient( this );

    // make sure to have an empty GUI
    QMenuBar* mb = internalMenuBar();
    if ( mb )
        mb->clear();

    (void)toolBarIterator(); // make sure toolbarList is most-up-to-date
    toolbarList.setAutoDelete( true );
    toolbarList.clear();
    toolbarList.setAutoDelete( false );
/*US
    // don't build a help menu unless the user ask for it
    if (d->showHelpMenu) {
        // we always want a help menu
        if (helpMenu2 == 0)
            helpMenu2 = new KHelpMenu(this, instance()->aboutData(), true,
                                      actionCollection());
    }

    // we always want to load in our global standards file
    setXMLFile( locate( "config", "ui/ui_standards.rc", instance() ) );

    // now, merge in our local xml file.  if this is null, then that
    // means that we will be only using the global file
    if ( !xmlfile.isNull() ) {
        setXMLFile( xmlfile, true );
    } else {
        QString auto_file(instance()->instanceName() + "ui.rc");
        setXMLFile( auto_file, true );
    }

    // make sure we don't have any state saved already
    setXMLGUIBuildDocument( QDomDocument() );

    // do the actual GUI building
    guiFactory()->addClient( this );

    // try and get back *some* of our memory
    if ( _conserveMemory )
    {
      // before freeing the memory allocated by the DOM document we also
      // free all memory allocated internally in the KXMLGUIFactory for
      // the menubar and the toolbars . This however implies that we
      // have to take care of deleting those widgets ourselves. For
      // destruction this is no problem, but when rebuilding we have
      // to take care of that (and we want to rebuild the GUI when
      // using stuff like the toolbar editor ).
      // In addition we have to take care of not removing containers
      // like popupmenus, defined in the XML document.
      // this code should probably go into a separate method in KMainWindow.
      // there's just one problem: I'm bad in finding names ;-) , so
      // I skipped this ;-)

      QDomDocument doc = domDocument();

      QDomElement e = doc.documentElement().firstChild().toElement();
      for (; !e.isNull(); e = e.nextSibling().toElement() ) {
          if ( e.tagName().lower() == "toolbar" )
              factory_->resetContainer( e.attribute( "name" ) );
          else if ( e.tagName().lower() == "menubar" )
              factory_->resetContainer( e.tagName(), true );
      }

      conserveMemory();
    }
*/
    setUpdatesEnabled( true );
    updateGeometry();
}

void KMainWindow::saveMainWindowSettings(KConfig *config, const QString &configGroup)
{
    kdDebug(200) << "KMainWindow::saveMainWindowSettings " << configGroup << endl;
//US    QStrList entryList;
    QStringList entryList;
    QString oldGroup;

    if (!configGroup.isEmpty())
    {
       oldGroup = config->group();
       config->setGroup(configGroup);
    }

    // Called by session management - or if we want to save the window size anyway
    if ( d->autoSaveWindowSize )
        saveWindowSize( config );

    QStatusBar* sb = internalStatusBar();
    if (sb) {
        entryList.clear();
        if ( sb->isHidden() )
            entryList.append("Disabled");
        else
            entryList.append("Enabled");

	if(sb->isHidden())
//US	  config->writeEntry(QString::fromLatin1("StatusBar"), entryList, ';');
	  config->writeEntry(QString::fromLatin1("StatusBar"), entryList);
	else
	  config->deleteEntry(QString::fromLatin1("StatusBar"));
    }

    QMenuBar* mb = internalMenuBar();
    if (mb) {
        entryList.clear();
        if ( mb->isHidden() )
            entryList.append("Disabled");
        else
            entryList.append("Enabled");

	// By default we don't hide.
	if(mb->isHidden())
//US	  config->writeEntry(QString::fromLatin1("MenuBar"), entryList, ';');
	  config->writeEntry(QString::fromLatin1("MenuBar"), entryList);
	else
	  config->deleteEntry(QString::fromLatin1("MenuBar"));
    }

    int n = 1; // Toolbar counter. toolbars are counted from 1,
    KToolBar *toolbar = 0;
    QPtrListIterator<KToolBar> it( toolBarIterator() );
    while ( ( toolbar = it.current() ) ) {
        ++it;
        QString group;
        if (!configGroup.isEmpty())
        {
           // Give a number to the toolbar, but prefer a name if there is one,
           // because there's no real guarantee on the ordering of toolbars
           group = (!::qstrcmp(toolbar->name(), "unnamed") ? QString::number(n) : QString(" ")+toolbar->name());
           group.prepend(" Toolbar");
           group.prepend(configGroup);
        }
        toolbar->saveSettings(config, group);
        n++;
    }
    if (!configGroup.isEmpty())
       config->setGroup(oldGroup);
}

bool KMainWindow::readPropertiesInternal( KConfig *config, int number )
{
    if ( number == 1 )
        readGlobalProperties( config );

    // in order they are in toolbar list
    QString s;
    s.setNum(number);
    s.prepend(QString::fromLatin1("WindowProperties"));

    config->setGroup(s);

    // restore the object name (window role)
    if ( config->hasKey(QString::fromLatin1("ObjectName" )) )
        setName( config->readEntry(QString::fromLatin1("ObjectName")).latin1()); // latin1 is right here

    applyMainWindowSettings(config); // Menubar, statusbar and toolbar settings.

    s.setNum(number);
    config->setGroup(s);
    readProperties(config);
    return true;
}

void KMainWindow::applyMainWindowSettings(KConfig *config, const QString &configGroup)
{
    kdDebug(200) << "KMainWindow::applyMainWindowSettings" << endl;
    QString entry;
//US    QStrList entryList;
    QStringList entryList;
    int i = 0; // Number of entries in list

    if (!configGroup.isEmpty())
       config->setGroup(configGroup);

    restoreWindowSize(config);

    QStatusBar* sb = internalStatusBar();
    if (sb) {
        entryList.clear();
//US        i = config->readListEntry (QString::fromLatin1("StatusBar"), entryList, ';');
        entryList = config->readListEntry (QString::fromLatin1("StatusBar"));
        entry = entryList.first();
        if (entry == QString::fromLatin1("Disabled"))
           sb->hide();
        else
           sb->show();
	if(d->showStatusBarAction)
	   d->showStatusBarAction->setChecked(!sb->isHidden());
    }

    QMenuBar* mb = internalMenuBar();
    if (mb) {
        entryList.clear();
//US        i = config->readListEntry (QString::fromLatin1("MenuBar"), entryList, ';');
        entryList = config->readListEntry (QString::fromLatin1("MenuBar"));
        entry = entryList.first();
        if (entry==QString::fromLatin1("Disabled"))
        {
            mb->hide(); 
        } else
        {
            mb->show();
        }
    }

        int n = 1; // Toolbar counter. toolbars are counted from 1,
        KToolBar *toolbar;
        QPtrListIterator<KToolBar> it( toolBarIterator() ); // must use own iterator

        for ( ; it.current(); ++it) {
            toolbar= it.current();
            QString group;
            if (!configGroup.isEmpty())
            {
               // Give a number to the toolbar, but prefer a name if there is one,
               // because there's no real guarantee on the ordering of toolbars
               group = (!::qstrcmp(toolbar->name(), "unnamed") ? QString::number(n) : QString(" ")+toolbar->name());
               group.prepend(" Toolbar");
               group.prepend(configGroup);
            }
            toolbar->applySettings(config, group);
            n++;
        }

    finalizeGUI( true );
    }

void KMainWindow::finalizeGUI( bool force )
{
    //kdDebug(200) << "KMainWindow::finalizeGUI force=" << force << endl;
    // The whole reason for this is that moveToolBar relies on the indexes
    // of the other toolbars, so in theory it should be called only once per
    // toolbar, but in increasing order of indexes.
    // Since we can't do that immediately, we move them, and _then_
    // we call positionYourself again for each of them, but this time
    // the toolbariterator should give them in the proper order.
    // Both the XMLGUI and applySettings call this, hence "force" for the latter.
    QPtrListIterator<KToolBar> it( toolBarIterator() );
    for ( ; it.current() ; ++ it )
            it.current()->positionYourself( force );

    d->settingsDirty = false;
}

void KMainWindow::saveWindowSize( KConfig * config ) const
{
/*US  
  int scnum = QApplication::desktop()->screenNumber(parentWidget());
  QRect desk = QApplication::desktop()->screenGeometry(scnum);
*/
  QRect desk = KGlobalSettings::desktopGeometry(0);
  
  QRect size( desk.width(), width(), desk.height(), height() );
  if(size != d->defaultWindowSize){
    config->writeEntry(QString::fromLatin1("Width %1").arg(desk.width()), width() );
    config->writeEntry(QString::fromLatin1("Height %1").arg(desk.height()), height() );
  }
  else{
    config->deleteEntry(QString::fromLatin1("Width %1").arg(desk.width()));
    config->deleteEntry(QString::fromLatin1("Height %1").arg(desk.height()));
  }
}

void KMainWindow::restoreWindowSize( KConfig * config )
{
    if (d->care_about_geometry) {
        parseGeometry(true);
    } else {
        // restore the size
/*US        int scnum = QApplication::desktop()->screenNumber(parentWidget());
        QRect desk = QApplication::desktop()->screenGeometry(scnum);
*/        
        QRect desk = KGlobalSettings::desktopGeometry(0);
        
        QSize size( config->readNumEntry( QString::fromLatin1("Width %1").arg(desk.width()), 0 ),
                    config->readNumEntry( QString::fromLatin1("Height %1").arg(desk.height()), 0 ) );
        if (size.isEmpty()) {
            // try the KDE 2.0 way
            size = QSize( config->readNumEntry( QString::fromLatin1("Width"), 0 ),
                          config->readNumEntry( QString::fromLatin1("Height"), 0 ) );
            if (!size.isEmpty()) {
                // make sure the other resolutions don't get old settings
                config->writeEntry( QString::fromLatin1("Width"), 0 );
                config->writeEntry( QString::fromLatin1("Height"), 0 );
            }
        }
        if ( !size.isEmpty() )
            resize( size );
    }
}

bool KMainWindow::initialGeometrySet() const
{
    return d->care_about_geometry;
}

void KMainWindow::ignoreInitialGeometry()
{
    d->care_about_geometry = false;
}

void KMainWindow::setSettingsDirty()
{
    //kdDebug(200) << "KMainWindow::setSettingsDirty" << endl;
    d->settingsDirty = true;
    if ( d->autoSaveSettings )
    {
        // Use a timer to save "immediately" user-wise, but not too immediately
        // (to compress calls and save only once, in case of multiple changes)
        if ( !d->settingsTimer )
        {
           d->settingsTimer = new QTimer( this );
           connect( d->settingsTimer, SIGNAL( timeout() ), SLOT( saveAutoSaveSettings() ) );
        }
        d->settingsTimer->start( 500, true );
    }
}

bool KMainWindow::settingsDirty() const
{
    return d->settingsDirty;
}

QString KMainWindow::settingsGroup() const
{
    return d->autoSaveGroup;
}

void KMainWindow::resizeEvent( QResizeEvent * e)
{
    if ( d->autoSaveWindowSize )
        setSettingsDirty(); 
    QMainWindow::resizeEvent( e ); 
}

bool KMainWindow::hasMenuBar()
{
	return (internalMenuBar());
}

//US KMenuBar *KMainWindow::menuBar()
QMenuBar *KMainWindow::menuBar()
{
//US    KMenuBar * mb = internalMenuBar();
    QMenuBar * mb = internalMenuBar();
    if ( !mb ) {
//US        mb = new KMenuBar( this );
        mb = new QMenuBar( this );
        // trigger a re-layout and trigger a call to the private
        // setMenuBar method.
        QMainWindow::menuBar();
    }
    return mb;
}

//US KStatusBar *KMainWindow::statusBar()
QStatusBar *KMainWindow::statusBar()
{
//US    KStatusBar * sb = internalStatusBar();
    QStatusBar * sb = internalStatusBar();
    if ( !sb ) {
//US        sb = new KStatusBar( this );
        sb = new QStatusBar( this );
        // trigger a re-layout and trigger a call to the private
        // setStatusBar method.
        QMainWindow::statusBar();
    }
    return sb;
}

void KMainWindow::shuttingDown()
{
    // Needed for Qt <= 3.0.3 at least to prevent reentrancy
    // when queryExit() shows a dialog. Check before removing!
    static bool reentrancy_protection = false;
    if (!reentrancy_protection)
    {
       reentrancy_protection = true;
       // call the virtual queryExit
       queryExit();
       reentrancy_protection = false;
    }

}

//US KMenuBar *KMainWindow::internalMenuBar()
QMenuBar *KMainWindow::internalMenuBar()
{
//US    QObjectList *l = queryList( "KMenuBar", 0, false, false );
    QObjectList *l = queryList( "QMenuBar", 0, false, false );
    if ( !l || !l->first() ) {
        delete l;
        return 0;
    }

//US    KMenuBar *m = (KMenuBar*)l->first();
    QMenuBar *m = (QMenuBar*)l->first();
    delete l;
    return m;
}

//US KStatusBar *KMainWindow::internalStatusBar()
QStatusBar *KMainWindow::internalStatusBar()
{
//US    QObjectList *l = queryList( "KStatusBar", 0, false, false );
    QObjectList *l = queryList( "QStatusBar", 0, false, false );
    if ( !l || !l->first() ) {
        delete l;
        return 0;
    }

//US    KStatusBar *s = (KStatusBar*)l->first();
    QStatusBar *s = (QStatusBar*)l->first();
    delete l;
    return s;
}

void KMainWindow::childEvent( QChildEvent* e)
{
    QMainWindow::childEvent( e );
}

void KMainWindow::paintEvent( QPaintEvent * e)
{
    QMainWindow::paintEvent( e );
}

QSize KMainWindow::sizeForCentralWidgetSize(QSize size)
{
    KToolBar *tb = (KToolBar*)child( "mainToolBar", "KToolBar" );
    if (tb && !tb->isHidden()) {
        switch( tb->barPos() )
        {
          case KToolBar::Top:
          case KToolBar::Bottom:
            size += QSize(0, tb->sizeHint().height());
            break;

          case KToolBar::Left:
          case KToolBar::Right:
            size += QSize(toolBar()->sizeHint().width(), 0);
            break;

          case KToolBar::Flat:
//US            size += QSize(0, 3+kapp->style().pixelMetric( QStyle::PM_DockWindowHandleExtent ));
            size += QSize(0, tb->sizeHint().height());
            break;

          default:
            break;
        }
    }
//US    KMenuBar *mb = menuBar();
    QMenuBar *mb = menuBar();
    if (!mb->isHidden()) {
        size += QSize(0,mb->heightForWidth(size.width()));
/*US        if (style().styleHint(QStyle::SH_MainWindow_SpaceBelowMenuBar, this))
           size += QSize( 0, dockWindowsMovable() ? 1 : 2);
*/           
           size += QSize( 0, 2);
    }
    QStatusBar *sb = internalStatusBar();
    if( sb && !sb->isHidden() )
       size += QSize(0, sb->sizeHint().height());

    return size;
}

// why do we support old gcc versions? using KXMLGUIBuilder::finalizeGUI;
void KMainWindow::finalizeGUI( KXMLGUIClient *client )
{ /*US KXMLGUIBuilder::finalizeGUI( client );*/ }

void KMainWindow::virtual_hook( int id, void* data )
{ /*US KXMLGUIBuilder::virtual_hook( id, data );*/
  KXMLGUIClient::virtual_hook( id, data ); }
