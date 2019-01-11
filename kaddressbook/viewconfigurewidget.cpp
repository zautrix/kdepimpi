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


#ifndef KAB_EMBEDDED

#include <qvbox.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "viewmanager.h"

#endif //KAB_EMBEDDED


#include <qlayout.h>

#include <kjanuswidget.h>
#include <kglobal.h>
#include <klocale.h>

#include "viewconfigurewidget.h"
#include "viewconfigurefieldspage.h"
#include "viewconfigurefilterpage.h"

ViewConfigureWidget::ViewConfigureWidget( KABC::AddressBook *ab, QWidget *parent,
                                          const char *name )
  : ConfigureWidget( ab, parent, name )
{
  QVBoxLayout *topLayout = new QVBoxLayout( this );
  
  mMainWidget = new KJanusWidget( this, "JanusWidget", KJanusWidget::Tabbed );
  
  topLayout->addWidget( mMainWidget );

  // Add the first page, the attributes
  QVBox *page = addPage( i18n( "Fields" ), QString::null,
                         KGlobal::iconLoader()->loadIcon( "view_detailed",
                         KIcon::Panel, 16 ) );

 
  // Add the select fields page 
  mFieldsPage = new ViewConfigureFieldsPage( addressBook(), (QWidget*)page );

//US  mFieldsPage->setMinimumSize(300,300);                         
  
  // Add the second page, the filter selection
  page = addPage( i18n( "Default Filter" ), QString::null, 
                  KGlobal::iconLoader()->loadIcon( "filter", 
                  KIcon::Panel, 16 ) );
                  
  mFilterPage = new ViewConfigureFilterPage( (QWidget*)page );
  //qDebug("+++++ViewConfigureWidget::ViewConfigureWidget ");
}

ViewConfigureWidget::~ViewConfigureWidget()
{
    //qDebug("-----ViewConfigureWidget::~ViewConfigureWidget() ");
}

void ViewConfigureWidget::restoreSettings( KConfig *config )
{
  mFieldsPage->restoreSettings( config );
  mFilterPage->restoreSettings( config );
}

void ViewConfigureWidget::saveSettings( KConfig *config )
{
  mFieldsPage->saveSettings( config );
  mFilterPage->saveSettings( config );
}
    
QVBox *ViewConfigureWidget::addPage( const QString &item, const QString &header,
                                   const QPixmap &pixmap )
{
  return mMainWidget->addVBoxPage( item, header, pixmap );
}

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <stdlib.h>

ViewConfigureDialog::ViewConfigureDialog( ViewConfigureWidget *wdg, const QString &viewName,
                                          QWidget *parent, const char *name )
  : KDialogBase( Swallow, i18n( "Modify View: " ) + viewName, Help | Ok | Cancel,
                 Ok, parent, name, true, true ), mConfigWidget( wdg )
{
//US, I have to change the way, how to connect the ViewConfigureWidget. It looks
// like it has changed.
    //QFrame *topFrame = plainPage();
    if ( mConfigWidget ) {
    mConfigWidget->reparent(this, QPoint(0,0), true);
  // QVBoxLayout * layout = new QVBoxLayout( this );
  //layout->addWidget(mConfigWidget );
  //mConfigWidget->reparent(topFrame, QPoint(1,1), true);
  
  setMainWidget( mConfigWidget );
  }

}

ViewConfigureDialog::~ViewConfigureDialog()
{
}
void ViewConfigureDialog::setWidget( ViewConfigureWidget *wdg )
{
    mConfigWidget = wdg ;
    setMainWidget( mConfigWidget );

}

void ViewConfigureDialog::restoreSettings( KConfig *config )
{
  mConfigWidget->restoreSettings( config );
}

void ViewConfigureDialog::saveSettings( KConfig *config )
{
  mConfigWidget->saveSettings( config );
}

void ViewConfigureDialog::slotHelp()
{
#ifndef KAB_EMBEDDED
  kapp->invokeHelp( "using-views" );
#else //KAB_EMBEDDED
  qDebug("ViewConfigureDialog::slotHelp not yet implemented");
#endif //KAB_EMBEDDED
}

#ifndef KAB_EMBEDDED
#include "viewconfigurewidget.moc"
#endif //KAB_EMBEDDED


