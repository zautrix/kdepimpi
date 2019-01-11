#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>


#include "klocale.h"
#include "kdebug.h"

#include "kdialogbase.h"

KDialogBase::KDialogBase()
{
}

KDialogBase::KDialogBase( QWidget *parent, const char *name, bool modal,
             const QString &caption,
             int buttonMask, ButtonCode defaultButton,
             bool separator,
             const QString &user1,
             const QString &user2,
             const QString &user3) :
    KDialog( parent, name, modal )
{
  init( caption, buttonMask, user1, user2 );
  if (findButton( defaultButton ) ) {
      (findButton( defaultButton ) )->setFocus();
      (findButton( defaultButton ) )->setDefault( true );
  }

}

KDialogBase::KDialogBase( int dialogFace, const QString &caption,
             int buttonMask, ButtonCode defaultButton,
             QWidget *parent, const char *name, bool modal,
             bool separator,
             const QString &user1,
             const QString &user2,
             const QString &user3) :
    KDialog( parent, name, modal )
{
    init( caption, buttonMask, user1, user2 );
    if (findButton( defaultButton ) ) {
        (findButton( defaultButton ) )->setFocus();
        (findButton( defaultButton ) )->setDefault( true );
    }

}

KDialogBase::~KDialogBase()
{
}

void KDialogBase::init( const QString &caption, int buttonMask,
                        const QString &user1 ,const QString &user2 )
{
  mMainWidget = 0;
  mTabWidget = 0;
  mPlainPage = 0;
  mTopLayout = 0;
  if ( !caption.isEmpty() ) {
    setCaption( caption );
  }

  if ( buttonMask & User1 ) {
    mUser1Button = new QPushButton( user1, this );
    connect( mUser1Button, SIGNAL( clicked() ), SLOT( slotUser1() ) );
  } else {
    mUser1Button = 0;
  }
  if ( buttonMask & User2 ) {
    mUser2Button = new QPushButton( user2, this );
    connect( mUser2Button, SIGNAL( clicked() ), SLOT( slotUser2() ) );
  } else {
    mUser2Button = 0;
  }

  if ( buttonMask & Ok ) {
    mOkButton = new QPushButton( i18n("Ok"), this );
    connect( mOkButton, SIGNAL( clicked() ), SLOT( slotOk() ) );
    //mOkButton->setDefault( true );
  } else {
    mOkButton = 0;
  }
  if ( buttonMask & Default ) {
    mDefaultButton = new QPushButton( i18n("Default"), this );
    connect( mDefaultButton, SIGNAL( clicked() ), SIGNAL( defaultClicked() ) );
  } else {
    mDefaultButton = 0;
  }

  if ( buttonMask & Apply ) {
    mApplyButton = new QPushButton( i18n("Apply"), this );
    connect( mApplyButton, SIGNAL( clicked() ), SLOT( slotApply() ) );
  } else {
    mApplyButton = 0;
  }

  if ( buttonMask & Cancel ) {
    mCancelButton = new QPushButton( i18n("Cancel"), this );
    connect( mCancelButton, SIGNAL( clicked() ), SLOT( slotCancel() ) );
  } else {
    mCancelButton = 0;
  }

  if ( buttonMask & Close ) {
    mCloseButton = new QPushButton( i18n("Close"), this );
    connect( mCloseButton, SIGNAL( clicked() ), SLOT( slotClose() ) );
  } else {
    mCloseButton = 0;
  }
}

QTabWidget *KDialogBase::tabWidget()
{
  if ( !mTabWidget ) {
    mTabWidget = new QTabWidget( this );
    setMainWidget( mTabWidget );
  }
  return mTabWidget;
}

void KDialogBase::hideButtons()
{
    if ( mUser1Button ) mUser1Button->hide() ;
    if ( mUser2Button ) mUser2Button->hide() ;
    if ( mOkButton ) mOkButton->hide()  ;
    if ( mApplyButton )  mApplyButton->hide()  ;
    if ( mDefaultButton ) mDefaultButton->hide();
    if ( mCancelButton ) mCancelButton->hide()  ;
    if ( mCloseButton ) mCloseButton->hide()  ;

}
void KDialogBase::initLayout()
{

  delete mTopLayout;
  mTopLayout = new QVBoxLayout( this );
  mTopLayout->setMargin( marginHintSmall() );
  mTopLayout->setSpacing( spacingHintSmall() );

  mTopLayout->addWidget( mMainWidget );

  QBoxLayout *buttonLayout = new QHBoxLayout;
  mTopLayout->addLayout( buttonLayout );

  if ( mUser1Button ) buttonLayout->addWidget( mUser1Button );
  if ( mUser2Button ) buttonLayout->addWidget( mUser2Button );
  if ( mOkButton ) buttonLayout->addWidget( mOkButton );
  if ( mApplyButton ) buttonLayout->addWidget( mApplyButton );
  if ( mDefaultButton ) buttonLayout->addWidget( mDefaultButton );
  if ( mCancelButton ) buttonLayout->addWidget( mCancelButton );
  if ( mCloseButton ) buttonLayout->addWidget( mCloseButton );
  buttonLayout->setMargin( marginHintSmall()  );
  buttonLayout->setSpacing( spacingHintSmall() );
}

QFrame *KDialogBase::addPage( const QString &name )
{
//  kdDebug() << "KDialogBase::addPage(): " << name << endl;
  QFrame *frame = new QFrame( tabWidget() );
  tabWidget()->addTab( frame, name );
  return frame;
}

QFrame *KDialogBase::addPage( const QString &name, int, const QPixmap & )
{
  return addPage( name );
}


void KDialogBase::setMainWidget( QWidget *widget )
{
  kdDebug() << "KDialogBase::setMainWidget()" << endl;

  mMainWidget = widget;
  initLayout();
}

void KDialogBase::setButtonText( ButtonCode id, const QString &text )
{
  QPushButton *button = findButton( id );
  if ( button ) {
    button->setText( text );
  }
}

void KDialogBase::enableButton( ButtonCode id, bool state )
{
  QPushButton *button = findButton( id );
  if ( button ) {
    button->setEnabled( state );
  }
}

QPushButton *KDialogBase::findButton( ButtonCode id )
{
  QPushButton *button = 0;
  switch ( id ) {
    case Ok:
      button = mOkButton;
      break;
    case Apply:
      button = mApplyButton;
      break;
    case User1:
      button = mUser1Button;
      break;
    case User2:
      button = mUser2Button;
      break;
    case Cancel:
      button = mCancelButton;
      break;
    case Default:
      button = mDefaultButton;
      break;
    case Close:
      button = mCloseButton;
      break;
    default:
      break;
  }
  return button;
}

void KDialogBase::enableButtonOK( bool state )
{
  enableButton( Ok, state );
}

void KDialogBase::enableButtonApply( bool state )
{
  enableButton( Apply, state );
}

void KDialogBase::showButton( ButtonCode id, bool show )
{
  QPushButton *button = findButton( id );
  if ( button ) {
    if ( show ) button->show();
    else button->hide();
  }
}

int KDialogBase::pageIndex( QWidget *widget ) const
{
  return 0;
}


bool KDialogBase::showPage( int index )
{
    tabWidget()->setCurrentPage( index );return false;
}

QFrame *KDialogBase::plainPage()
{
  if ( !mPlainPage ) {
    mPlainPage = new QFrame( this );
    setMainWidget( mPlainPage );
  }
  return mPlainPage;
}

void KDialogBase::slotOk()
{
  emit okClicked();
  accept();
}

void KDialogBase::slotApply()
{
  emit applyClicked();
}

void KDialogBase::slotCancel()
{
  emit cancelClicked();
  reject();
}

void KDialogBase::slotClose()
{
  emit closeClicked();
  reject();
}

void KDialogBase::accept ()
{
    emit acceptClicked();
    KDialog::accept();
}
void KDialogBase::slotUser1()
{
  emit user1Clicked();
}
void KDialogBase::slotUser2()
{
  emit user2Clicked();
}
