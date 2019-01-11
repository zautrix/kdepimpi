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

#include <kabc/picture.h>

#ifndef KAB_EMBEDDED
#include <kaccelmanager.h>
#include <kio/netaccess.h>
#include <kimageio.h>
#endif //KAB_EMBEDDED

#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kurlrequester.h>
#include <kurl.h>

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include "imagewidget.h"

ImageWidget::ImageWidget( QWidget *parent, const char *name )
  : QWidget( parent, name )
{
  QGridLayout *topLayout = new QGridLayout( this, 2, 1, KDialog::marginHint(),
                                            KDialog::spacingHint() );

  QGroupBox *photoBox = new QGroupBox( 0, Qt::Vertical, i18n( "Photo" ), this );
  QGridLayout *boxLayout = new QGridLayout( photoBox->layout(), 3, 2,
                                            KDialog::spacingHint() );
  boxLayout->setRowStretch( 2, 1 );

  mPhotoLabel = new QLabel( photoBox );
  int fac = 9;
  if ( QApplication::desktop()->width() > 320 )
      fac = 6;
  mPhotoLabel->setFixedSize( 50*9/fac, 70*9/fac );
  mPhotoLabel->setScaledContents( true );
  mPhotoLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  boxLayout->addMultiCellWidget( mPhotoLabel, 0, 3, 0, 0 );

  mPhotoUrl = new KURLRequester( photoBox );
#ifndef KAB_EMBEDDED  
  mPhotoUrl->setFilter( KImageIO::pattern() );
#else //KAB_EMBEDDED  
//US  qDebug("ImageWidget::ImageWidget KImageIO not defined. Does this harm ???");
#endif //KAB_EMBEDDED  
  
  
  boxLayout->addWidget( mPhotoUrl, 0, 1 );
  QHBox *hb1 = new QHBox ( photoBox );
  mUsePhotoUrl = new QCheckBox( i18n( "Store as URL" ), hb1 );
  mUsePhotoUrl->setEnabled( false );
  boxLayout->addWidget( hb1, 1, 1 );

  QPushButton * pb = new QPushButton( i18n( "Remove" ), hb1 );
  connect( pb, SIGNAL( clicked() ),this, SLOT( removePhoto() ) );
  boxLayout->addWidget( new QLabel(  photoBox ), 2, 1 );

  topLayout->addWidget( photoBox, 0, 0 );

  QGroupBox *logoBox = new QGroupBox( 0, Qt::Vertical, i18n( "Logo" ), this );
  boxLayout = new QGridLayout( logoBox->layout(), 3, 2, KDialog::spacingHint() );
  boxLayout->setRowStretch( 2, 1 );

  mLogoLabel = new QLabel( logoBox );
  mLogoLabel->setFixedSize( 50*9/fac, 70*9/fac );
  mLogoLabel->setScaledContents( true );
  mLogoLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  boxLayout->addMultiCellWidget( mLogoLabel, 0, 3, 0, 0 );

  mLogoUrl = new KURLRequester( logoBox );
#ifndef KAB_EMBEDDED  
  mLogoUrl->setFilter( KImageIO::pattern() );
#else //KAB_EMBEDDED  
//US  qDebug("ImageWidget::ImageWidget KImageIO not defined 2");
#endif //KAB_EMBEDDED  
  boxLayout->addWidget( mLogoUrl, 0, 1 );
  
  QHBox *hb2 = new QHBox ( logoBox );
  mUseLogoUrl = new QCheckBox( i18n( "Store as URL" ), hb2 );
  mUseLogoUrl->setEnabled( false );
  boxLayout->addWidget( hb2, 1, 1 );

   pb = new QPushButton( i18n( "Remove" ), hb2 );
  connect( pb, SIGNAL( clicked() ),this, SLOT( removeLogo() ) );
  boxLayout->addWidget( new QLabel(  logoBox ), 2, 1 );
  topLayout->addWidget( logoBox, 1, 0 );

  connect( mPhotoUrl, SIGNAL( textChanged( const QString& ) ),
           SIGNAL( changed() ) );
  connect( mPhotoUrl, SIGNAL( urlSelected( const QString& ) ),
           SLOT( loadPhoto() ) );
  connect( mPhotoUrl, SIGNAL( urlSelected( const QString& ) ),
           SIGNAL( changed() ) );
  connect( mPhotoUrl, SIGNAL( urlSelected( const QString& ) ),
           SLOT( updateGUI() ) );
  connect( mUsePhotoUrl, SIGNAL( toggled( bool ) ),
           SIGNAL( changed() ) );

  connect( mLogoUrl, SIGNAL( textChanged( const QString& ) ),
           SIGNAL( changed() ) );
  connect( mLogoUrl, SIGNAL( urlSelected( const QString& ) ),
           SLOT( loadLogo() ) );
  connect( mLogoUrl, SIGNAL( urlSelected( const QString& ) ),
           SIGNAL( changed() ) );
  connect( mLogoUrl, SIGNAL( urlSelected( const QString& ) ),
           SLOT( updateGUI() ) );
  connect( mUseLogoUrl, SIGNAL( toggled( bool ) ),
           SIGNAL( changed() ) );

#ifndef KAB_EMBEDDED  
  KAcceleratorManager::manage( this );
#endif //KAB_EMBEDDED  

}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::setPhoto( const KABC::Picture &photo )
{
  bool blocked = signalsBlocked();
  blockSignals( true );

  if ( photo.isIntern() ) {
//US   
//US    mPhotoLabel->setPixmap( photo.data() );
    if (photo.data().isNull() != true)
    {
      QPixmap pm;
      pm.convertFromImage(photo.data()); 

      mPhotoLabel->setPixmap( pm );
    }
    
    mUsePhotoUrl->setChecked( false );
  } else {
    mPhotoUrl->setURL( photo.url() );
    if ( !photo.url().isEmpty() )
      mUsePhotoUrl->setChecked( true );
    loadPhoto();
  }
  
  blockSignals( blocked );
}

KABC::Picture ImageWidget::photo() const
{
  KABC::Picture photo;

  if ( mUsePhotoUrl->isChecked() )
    photo.setUrl( mPhotoUrl->url() );
  else {
    QPixmap *px = mPhotoLabel->pixmap();
    if ( px ) {
#ifndef KAB_EMBEDDED  
      if ( px->height() > px->width() )
        photo.setData( px->convertToImage().scaleHeight( 140 ) );
      else
        photo.setData( px->convertToImage().scaleWidth( 100 ) );
#else //KAB_EMBEDDED  
//US add teh nullcheck      
      if (px->isNull() != true )
        photo.setData( px->convertToImage() );
#endif //KAB_EMBEDDED  

      photo.setType( "PNG" );
    }
  }

  return photo;
}

void ImageWidget::setLogo( const KABC::Picture &logo )
{
  bool blocked = signalsBlocked();
  blockSignals( true );

  if ( logo.isIntern() ) {
//US
//US    mLogoLabel->setPixmap( logo.data() );
    if (logo.data().isNull() != true)
    {
      QPixmap pm;
      pm.convertFromImage(logo.data()); 
      mLogoLabel->setPixmap( pm );
    }
    mUseLogoUrl->setChecked( false );
  } else {
    mLogoUrl->setURL( logo.url() );
    if ( !logo.url().isEmpty() )
      mUseLogoUrl->setChecked( true );
    loadLogo();
  }

  blockSignals( blocked );
}

KABC::Picture ImageWidget::logo() const
{
  KABC::Picture logo;

  if ( mUseLogoUrl->isChecked() )
    logo.setUrl( mLogoUrl->url() );
  else {
    QPixmap *px = mLogoLabel->pixmap();
    if ( px ) {
#ifndef KAB_EMBEDDED  
      if ( px->height() > px->width() )
        logo.setData( px->convertToImage().scaleHeight( 140 ) );
      else
        logo.setData( px->convertToImage().scaleWidth( 100 ) );
#else //KAB_EMBEDDED     
      if (px->isNull() != true ) 
          logo.setData( px->convertToImage() );
#endif //KAB_EMBEDDED  

      logo.setType( "PNG" );
      
    }
  }
  return logo;
}
void ImageWidget::removePhoto()
{
    setPhoto(KABC::Picture() );
}

void ImageWidget::removeLogo()
{
    setLogo(KABC::Picture() );
}

void ImageWidget::loadPhoto()
{
  mPhotoLabel->setPixmap( loadPixmap( mPhotoUrl->url() ) );
}

void ImageWidget::loadLogo()
{
  mLogoLabel->setPixmap( loadPixmap( mLogoUrl->url() ) );
}

void ImageWidget::updateGUI()
{
  KURLRequester *ptr = (KURLRequester*)sender();


  if ( ptr == mPhotoUrl )
    mUsePhotoUrl->setEnabled( true );
  else if ( ptr == mLogoUrl )
    mUseLogoUrl->setEnabled( true );
}

QPixmap ImageWidget::loadPixmap( const QString &url )
{

  QPixmap pixmap;

  if ( url.isEmpty() )
    return pixmap;
  pixmap.load( url );
  return pixmap;
}

#ifndef KAB_EMBEDDED
#include "imagewidget.moc"
#endif //KAB_EMBEDDED
