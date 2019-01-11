#include "kcolorbutton.h"
#include "kcolordialog.h"
#include "qapplication.h"


#include "qlayout.h"
#ifdef DESKTOP_VERSION
#include <qcolordialog.h>
#endif
void KColorButton:: edit()
{

#ifdef DESKTOP_VERSION
    QColor col = QColorDialog::getColor ( mColor ); 
    if ( col.isValid () ) {
        mColor = col;
        setColor ( mColor );
        emit changed ( mColor );
        emit changedID ( mColor, id );
    }
#else
    KColorDialog* k =  new KColorDialog( this );
    k->setColor( mColor );
    int res = k->exec();
    if ( res ) {
        mColor = k->getColor(); 
        setColor ( mColor );
        emit changed ( mColor );
        emit changedID ( mColor, id );
    }
    delete k;
#endif
}
KColorButton::KColorButton( QWidget *p ):QPushButton( p )
{
    int size = 24;
    if(  QApplication::desktop()->width() < 480 || QApplication::desktop()->height() <= 320 )
        size = 18;
    setFixedSize( size,size );
    int id = 0;
    connect ( this, SIGNAL( clicked() ), this ,SLOT (edit() ));
    
}
void KColorButton::setID ( int i) 
{
    id = i;
}
void KColorButton::setColor ( const QColor & c) 
{
 mColor = c; 
 QPixmap pix ( height() - 4, width() - 4 );
 pix.fill( c );
 setPixmap ( pix );
}
