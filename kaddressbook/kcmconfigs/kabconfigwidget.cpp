/*
    This file is part of KAddressBook.
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

#include <qcheckbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfile.h>
#include <qvbox.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klistview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#ifndef KAB_EMBEDDED
#include <ktrader.h>
#else // KAB_EMBEDDED
#include <mergewidget.h>
#include <distributionlistwidget.h>
#endif // KAB_EMBEDDED

#include "addresseewidget.h"
#include "extensionconfigdialog.h"
#include "extensionwidget.h"
#include "kabprefs.h"

#include "kabconfigwidget.h"
#include <kglobalsettings.h>

class ExtensionItem : public QCheckListItem
{
  public:

#ifndef KAB_EMBEDDED
    ExtensionItem( QListView *parent, const QString &text );
    void setService( const KService::Ptr &ptr );
#else //KAB_EMBEDDED
    ExtensionItem( QListView *parent, const QString &text, const QString &name, const QString &comment );
    void setFactory( ExtensionFactory* fac );
#endif //KAB_EMBEDDED

    bool configWidgetAvailable() const;
    ExtensionFactory *factory() const;

    virtual QString text( int column ) const;

  private:
#ifndef KAB_EMBEDDED
    KService::Ptr mPtr;
#else //KAB_EMBEDDED
    ExtensionFactory* mFactory;
    QString mName;
    QString mComment;

#endif //KAB_EMBEDDED

};

KABConfigWidget::KABConfigWidget( KABPrefs* prefs, QWidget *parent, const char *name )
  : KPrefsWidget( prefs, parent, name )
{
  
  QVBoxLayout *topLayout = new QVBoxLayout( this, 0,
                                            KDialog::spacingHintSmall() );

  QTabWidget *tabWidget = new QTabWidget( this );
  topLayout->addWidget( tabWidget );

  // General page
  QWidget *generalPage = new QWidget( this );
  QVBoxLayout *layout = new QVBoxLayout( generalPage, KDialog::marginHintSmall(),
                                            KDialog::spacingHintSmall() );


  QWidget *hBox = new QWidget( generalPage, "qhbox" );
  QHBoxLayout *hboxLayout = new QHBoxLayout( hBox);
  KPrefsWidFont *detailsFont =
      addWidFont(i18n("phone:123"),i18n("Details view font"),
                 &(KABPrefs::instance()->mDetailsFont),hBox);
  hboxLayout->addWidget(detailsFont->label());
  hboxLayout->addWidget(detailsFont->preview());
  hboxLayout->addWidget(detailsFont->button());
  hboxLayout->setMargin(KDialog::marginHintSmall() );
  hboxLayout->setSpacing(KDialog::spacingHintSmall());
  //hBox->setBackgroundColor( black);
  layout->addWidget( hBox );

  //general groupbox
   QWidget *vBox = new QWidget( generalPage, "qvbox" );
  QVBoxLayout *boxLayout = new QVBoxLayout( vBox );
  boxLayout->setAlignment( Qt::AlignTop );
  boxLayout->setMargin(KDialog::marginHintSmall() );
  boxLayout->setSpacing( KDialog::spacingHintSmall() );
  mMenuBarBox = new QCheckBox( i18n( "Full Menu bar (restart)" ), vBox, "mremenuturn" );
  boxLayout->addWidget( mMenuBarBox );
  mSearchReturnBox = new QCheckBox( i18n( "Search only after <return> key pressed" ), vBox, "mreturn" );
  boxLayout->addWidget( mSearchReturnBox );
  mAutoSearchWithWildcardBox = new QCheckBox( i18n( "Search with '*' prefix (wildcard)" ), vBox, "mwildcard" );
  boxLayout->addWidget( mAutoSearchWithWildcardBox);
  mHideSearchOnSwitchBox = new QCheckBox( i18n( "Shrink searchfield in portrait view" ), vBox, "mswitch" );
  boxLayout->addWidget( mHideSearchOnSwitchBox );

#ifdef DESKTOP_VERSION
  mHideSearchOnSwitchBox->hide();
#endif
  mViewsSingleClickBox = new QCheckBox( i18n( "Show edit dialog on single click" ), vBox, "msingle" );
  boxLayout->addWidget( mViewsSingleClickBox );

  mNameParsing = new QCheckBox( i18n( "Automatic name parsing for new contacts" ), vBox, "mparse" );
  boxLayout->addWidget( mNameParsing );

  mMultipleViewsAtOnce = new QCheckBox( i18n( "Display List and Details at once (restart)" ), vBox, "mdisplay" );
  boxLayout->addWidget( mMultipleViewsAtOnce );

  mAskForDelete = new QCheckBox( i18n( "Ask before delete contact" ), vBox, "mdel" );
  boxLayout->addWidget( mAskForDelete );

  mAskForQuit = new QCheckBox( i18n( "Show exit confirmation" ), vBox, "mquit" );
  boxLayout->addWidget( mAskForQuit );

  layout->addWidget( vBox );

  tabWidget->addTab( generalPage, i18n( "General" ) );

  // Extension page
  QWidget *extensionPage = new QWidget( this );
  QVBoxLayout *extensionLayout = new QVBoxLayout( extensionPage, KDialog::marginHintSmall(),
                                            KDialog::spacingHintSmall() );

  //extensions groupbox

  QGroupBox* groupBox = new QGroupBox( 0, Qt::Vertical, i18n( "Extensions (restart)" ), extensionPage );
  boxLayout = new QVBoxLayout( groupBox->layout() );
  boxLayout->setAlignment( Qt::AlignTop );
  boxLayout->setMargin(KDialog::marginHintSmall());
  boxLayout->setSpacing(KDialog::spacingHintSmall());
  groupBox->layout()->setMargin(1) ;
  groupBox->layout()->setSpacing(0);
  mExtensionView = new KListView( groupBox );
  mExtensionView->setAllColumnsShowFocus( true );
  mExtensionView->addColumn( i18n( "Name" ) );
  mExtensionView->addColumn( i18n( "Description" ) );
  //mExtensionView->setMaximumHeight(80);

  boxLayout->addWidget( mExtensionView );

  mConfigureButton = new QPushButton( i18n( "Configure..." ), groupBox );
  mConfigureButton->setEnabled( false );
  boxLayout->addWidget( mConfigureButton );

  extensionLayout->addWidget( groupBox );

  connect( mMenuBarBox, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mNameParsing, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mViewsSingleClickBox, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mSearchReturnBox, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mAutoSearchWithWildcardBox, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mHideSearchOnSwitchBox, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mMultipleViewsAtOnce, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mAskForQuit, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mAskForDelete, SIGNAL( toggled( bool ) ), this, SLOT( modified() ) );
  connect( mExtensionView, SIGNAL( selectionChanged( QListViewItem* ) ),
           SLOT( selectionChanged( QListViewItem* ) ) );
  connect( mExtensionView, SIGNAL( clicked( QListViewItem* ) ),
           SLOT( itemClicked( QListViewItem* ) ) );
  connect( mConfigureButton, SIGNAL( clicked() ),
           SLOT( configureExtension() ) );

  tabWidget->addTab( extensionPage, i18n( "Extensions" ) );

  // Addressee page
  mAddresseeWidget = new AddresseeWidget( this );
  tabWidget->addTab( mAddresseeWidget, i18n( "Contact" ) );
  connect( mAddresseeWidget, SIGNAL( modified() ), SLOT( modified() ) );

}

void KABConfigWidget::usrReadConfig()
{
  KABPrefs* prefs = KABPrefs::instance();

  bool blocked = signalsBlocked();
  blockSignals( true );
  mMenuBarBox->setChecked( prefs->mFullMenuBarVisible);
  mNameParsing->setChecked( prefs->mAutomaticNameParsing );
  mViewsSingleClickBox->setChecked( prefs->mHonorSingleClick );
  mSearchReturnBox->setChecked( prefs->mSearchWithReturn );
  mAutoSearchWithWildcardBox->setChecked( prefs->mAutoSearchWithWildcard );
  mHideSearchOnSwitchBox->setChecked( prefs->mHideSearchOnSwitch );
  mMultipleViewsAtOnce->setChecked( prefs->mMultipleViewsAtOnce );
  mAskForQuit->setChecked( prefs->mAskForQuit );
  mAskForDelete->setChecked( prefs->mAskForDelete );

  mAddresseeWidget->restoreSettings();

  restoreExtensionSettings();

  blockSignals( blocked );

}

void KABConfigWidget::usrWriteConfig()
{
  KABPrefs* prefs = KABPrefs::instance();
  prefs->mFullMenuBarVisible = mMenuBarBox->isChecked();
  prefs->mAutomaticNameParsing = mNameParsing->isChecked();
  prefs->mHonorSingleClick = mViewsSingleClickBox->isChecked();
  prefs->mSearchWithReturn = mSearchReturnBox->isChecked();
  prefs->mAutoSearchWithWildcard = mAutoSearchWithWildcardBox->isChecked();
  prefs->mHideSearchOnSwitch = mHideSearchOnSwitchBox->isChecked();

  prefs->mMultipleViewsAtOnce = mMultipleViewsAtOnce->isChecked();
  prefs->mAskForQuit = mAskForQuit->isChecked();
  prefs->mAskForDelete = mAskForDelete->isChecked();

  mAddresseeWidget->saveSettings();

  saveExtensionSettings();

}

void KABConfigWidget::restoreExtensionSettings()
{
  QStringList activeExtensions = KABPrefs::instance()->mActiveExtensions;

  mExtensionView->clear();

#ifndef KAB_EMBEDDED
  KTrader::OfferList plugins = KTrader::self()->query( "KAddressBook/Extension" );
  KTrader::OfferList::ConstIterator it;
  for ( it = plugins.begin(); it != plugins.end(); ++it ) {
    if ( !(*it)->hasServiceType( "KAddressBook/Extension" ) )
      continue;

    ExtensionItem *item = new ExtensionItem( mExtensionView, (*it)->name() );
    item->setService( *it );
    if ( activeExtensions.contains( item->factory()->identifier() ) )
      item->setOn( true );
  }
#else //KAB_EMBEDDED
    ExtensionFactory *extensionFactory = new MergeFactory();

    ExtensionItem *item = new ExtensionItem( mExtensionView, "Merge",  "Merge", "Merge contacts");

    item->setFactory( extensionFactory );
    if ( activeExtensions.contains( extensionFactory->identifier() ) )
      item->setOn( true );



    extensionFactory = new DistributionListFactory();

    item = new ExtensionItem( mExtensionView, "Distribution List",  "Distribution List", "Manage Distribution Lists");

    item->setFactory( extensionFactory );
    if ( activeExtensions.contains( extensionFactory->identifier() ) )
      item->setOn( true );


#endif //KAB_EMBEDDED

}

void KABConfigWidget::saveExtensionSettings()
{
  QStringList activeExtensions;

  QPtrList<QListViewItem> list;
  QListViewItemIterator it( mExtensionView );
  while ( it.current() ) {
    ExtensionItem *item = static_cast<ExtensionItem*>( it.current() );
    if ( item ) {
      if ( item->isOn() )
        activeExtensions.append( item->factory()->identifier() );
    }
    ++it;
  }

  KABPrefs::instance()->mActiveExtensions = activeExtensions;
}

void KABConfigWidget::configureExtension()
{
  ExtensionItem *item = static_cast<ExtensionItem*>( mExtensionView->currentItem() );
  if ( !item )
    return;

#ifndef KAB_EMBEDDED
  KConfig config( "kaddressbookrc" );
#else //KAB_EMBEDDED
  KConfig config( locateLocal("config", "kaddressbookrc") );
#endif //KAB_EMBEDDED
  config.setGroup( QString( "Extensions_%1" ).arg( item->factory()->identifier() ) );

  ExtensionConfigDialog dlg( item->factory(), &config, this );
  dlg.exec();

  config.sync();
}

void KABConfigWidget::selectionChanged( QListViewItem *i )
{
  ExtensionItem *item = static_cast<ExtensionItem*>( i );
  if ( !item )
    return;

  mConfigureButton->setEnabled( item->configWidgetAvailable() );
}

void KABConfigWidget::itemClicked( QListViewItem *item )
{
  if ( item != 0 )
    modified();
}

#ifndef KAB_EMBEDDED
ExtensionItem::ExtensionItem( QListView *parent, const QString &text )
  : QCheckListItem( parent, text, CheckBox )
{
}

void ExtensionItem::setService( const KService::Ptr &ptr )
{
  mPtr = ptr;
}
#else //KAB_EMBEDDED
ExtensionItem::ExtensionItem( QListView *parent, const QString &text, const QString &name, const QString &comment )
  : QCheckListItem( parent, text, CheckBox )
{
  mName = name;
  mComment = comment;
}


void ExtensionItem::setFactory( ExtensionFactory* fac )
{
  mFactory = fac;
}
#endif //KAB_EMBEDDED

bool ExtensionItem::configWidgetAvailable() const
{
#ifndef KAB_EMBEDDED
  KLibFactory *factory = KLibLoader::self()->factory( mPtr->library().latin1() );
  if ( !factory )
    return false;

  ExtensionFactory *extensionFactory = static_cast<ExtensionFactory*>( factory );
  if ( !extensionFactory )
    return false;

  return extensionFactory->configureWidgetAvailable();
#else //KAB_EMBEDDED
  return mFactory->configureWidgetAvailable();
#endif //KAB_EMBEDDED

}

ExtensionFactory *ExtensionItem::factory() const
{
#ifndef KAB_EMBEDDED
  KLibFactory *factory = KLibLoader::self()->factory( mPtr->library().latin1() );
  if ( !factory )
    return 0;

  return static_cast<ExtensionFactory*>( factory );
#else //KAB_EMBEDDED
  return mFactory;
#endif //KAB_EMBEDDED
}

QString ExtensionItem::text( int column ) const
{
#ifndef KAB_EMBEDDED
  if ( column == 0 )
    return mPtr->name();
  else if ( column == 1 )
    return mPtr->comment();
  else
    return QString::null;
#else //KAB_EMBEDDED
  if ( column == 0 )
    return mName;
  else if ( column == 1 )
    return mComment;
  else
    return QString::null;
#endif //KAB_EMBEDDED
}

#ifndef KAB_EMBEDDED
#include "kabconfigwidget.moc"
#endif //KAB_EMBEDDED

