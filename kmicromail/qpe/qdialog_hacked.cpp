
#include <qdialog.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <klocale.h>
#ifdef QDialog
#undef QDialog
#endif
    QDialog_hacked::QDialog_hacked ( QWidget * parent, const char * name, bool modal, WFlags f ) 
        :  QDialog(  parent,name,modal, f)
        {
            qDebug("New hacked QDialog for ompi desktop");
            //QTimer::singleShot(  1, this,SLOT(addaddbuttons()) );

            setOrientation ( Vertical ); 
            QHBox * hb = new QHBox ( this );
            QPushButton *ok = new QPushButton( i18n("OK"), hb );
            QPushButton *cancel = new QPushButton( i18n("Cancel"), hb );
            setExtension ( hb );
            showExtension ( true );
            connect ( ok, SIGNAL ( clicked()),this, SLOT (accept() ) );
            connect ( cancel, SIGNAL ( clicked()),this, SLOT (reject() ) );
            //setWFlags(WStyle_MinMax );

        }
      

