
#include <kdialog.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <klocale.h>

KDialog::KDialog( QWidget *parent, const char *name, bool modal ) :
      QDialog( parent, name, modal ) 
{
    ;

}
#ifdef DESKTOP_VERSION
int KDialog::spacingHint() {
  if ( QApplication::desktop()->height() > 480 )
    return 7;
  return 4; 
}
int KDialog::marginHint() { 
  if ( QApplication::desktop()->height() > 480 ) 
    return 7;
  return 4; 
}

int KDialog::spacingHintSmall()  { 
  if ( QApplication::desktop()->height() > 480 ) 
    return 4;
  return 3; 
}
int KDialog::marginHintSmall()  { 
  if ( QApplication::desktop()->height() > 480 ) 
    return 4;
  return 3; 
}

#else
int KDialog::spacingHint() { return 3; }
int KDialog::marginHint() { return 3; }

int KDialog::spacingHintSmall() { return 1; }
int KDialog::marginHintSmall() { return 1; }
#endif
KProgressDialog::KProgressDialog( QWidget *parent, const char *name, bool modal ) :
      QDialog( parent, name, modal ) 
{
    QVBoxLayout* lay= new QVBoxLayout ( this ); 
    mLabel = new QLabel ( "abc",this );
    mBar = new QProgressBar ( this );
    mButton = new QPushButton ( i18n("Cancel"),this );
    lay->addWidget ( mLabel );
    lay->addWidget ( mBar );
    lay->addWidget ( mButton  );
    connect ( mButton , SIGNAL ( clicked () ), this, SIGNAL ( cancelled () ));
    resize ( 220, sizeHint().height() +mLabel->sizeHint().height());

}

void KProgressDialog::setLabelText ( const QString & t)
{
    mLabel->setText( t );
}

void KProgressDialog::setTotalSteps ( int totalSteps )
{
    setActiveWindow();
    setFocus();
    mBar->setTotalSteps ( totalSteps );
}
void KProgressDialog::setProgress ( int progress )
{   
    setActiveWindow();
    setFocus();
    mBar->setProgress (  progress ); 
}
void KProgressDialog::accept()
{
    mLabel->setText( i18n("Eeek, there I am ticklish!") );
    move ( geometry().x()-20,geometry().y()+20);
    // QDialog::accept();
}
void KProgressDialog::reject()
{
    emit cancelled ();
    //QDialog::reject();
}
