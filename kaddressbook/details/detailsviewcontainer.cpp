/*                                                                      
    This file is part of KAddressBook.
    Copyright (c) 1996-2002 Mirko Boehm <mirko@kde.org>
                                                                        
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

#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwidgetstack.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>

#include "look_basic.h"
//#include "look_details.h"
#include "look_html.h"

#ifdef KAB_EMBEDDED
#include "kabprefs.h"
#endif //KAB_EMBEDDED


#include "detailsviewcontainer.h"

ViewContainer::ViewContainer( QWidget *parent, const char* name )
  : QWidget( parent, name ), mCurrentLook( 0 )
{
  QBoxLayout *topLayout = new QVBoxLayout( this );
  //topLayout->setMargin( KDialog::marginHint() );
  //topLayout->setSpacing( KDialog::spacingHint() );
  topLayout->setMargin( 0 );
  topLayout->setSpacing( 0 );

  QBoxLayout *styleLayout = new QHBoxLayout( topLayout );    

  QLabel *label = new QLabel( i18n("Style:"), this );
  styleLayout->addWidget( label );

  mStyleCombo = new QComboBox( this );
  styleLayout->addWidget( mStyleCombo );

  QFrame *frameRuler = new QFrame( this );
//US  frameRuler->setFrameShape( QFrame::HLine );
//US  frameRuler->setFrameShadow( QFrame::Sunken );
//US  topLayout->addWidget( frameRuler );

  mDetailsStack = new QWidgetStack( this );
  topLayout->addWidget( mDetailsStack, 1 );

  registerLooks();

#if 1
    // Hide detailed view selection combo box, because we currently have
    // only one. Reenable it when there are more detailed views.
    label->hide();
    mStyleCombo->hide();
    frameRuler->hide();
#endif
}

void ViewContainer::printView()
{
    mCurrentLook->printView();
}
KABBasicLook *ViewContainer::currentLook()
{
  return mCurrentLook;
}

void ViewContainer::registerLooks()
{
  mLookFactories.append( new KABHtmlViewFactory( mDetailsStack ) );
//  mLookFactories.append( new KABDetailedViewFactory( mDetailsStack ) );
  mStyleCombo->clear();

  for ( uint i = 0; i < mLookFactories.count(); ++i )
    mStyleCombo->insertItem( mLookFactories.at( i )->description() );

  if ( !mLookFactories.isEmpty() )
    slotStyleSelected( 0 );
}

void ViewContainer::slotStyleSelected( int index )
{
#ifndef KAB_EMBEDDED
  KConfig *config = kapp->config();
#else //KAB_EMBEDDED
  //US I hope I got the same config object as above expected.
  KConfig *config = KABPrefs::instance()->getConfig();
#endif //KAB_EMBEDDED
  KABC::Addressee addr;

  if ( index >= 0 && index < mStyleCombo->count() ) {
    if ( mCurrentLook != 0 ) {
      mCurrentLook->saveSettings( config );
      addr = mCurrentLook->addressee();

      delete mCurrentLook;
      mCurrentLook = 0;
    }

    KABLookFactory *factory = mLookFactories.at( index );

    mCurrentLook = factory->create();
    mDetailsStack->raiseWidget( mCurrentLook );

    connect( mCurrentLook, SIGNAL( sendEmail( const QString& ) ), this,
             SIGNAL( sendEmail( const QString& ) ) );
    connect( mCurrentLook, SIGNAL( browse( const QString& ) ), this,
             SIGNAL( browse( const QString& ) ) );
  }

  mCurrentLook->restoreSettings( config );
  mCurrentLook->setAddressee( addr );
}

void ViewContainer::setAddressee( const KABC::Addressee& addressee )
{
  if ( mCurrentLook != 0 ) {
      mCurrentAddressee = addressee;
      mCurrentLook->setAddressee( mCurrentAddressee );
    
  }
}

KABC::Addressee ViewContainer::addressee()
{
  static KABC::Addressee empty; // do not use!

  if ( !mCurrentLook )
    return empty;
  else
    return mCurrentLook->addressee();
}

void ViewContainer::setReadOnly( bool state )
{
  if ( mCurrentLook )
    mCurrentLook->setReadOnly( state );
}

#ifndef KAB_EMBEDDED
#include "detailsviewcontainer.moc"
#endif //KAB_EMBEDDED
