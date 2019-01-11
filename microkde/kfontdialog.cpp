#include "kfontdialog.h"
#ifndef DESKTOP_VERSION
#include "ofontselector.h"
#else
#include <qfontdialog.h>
#endif
#include <qdialog.h>
#include <qlayout.h>
#include <qpushbutton.h>
QFont KFontDialog::getFont( const QFont & f, bool & ok )
{
#ifndef DESKTOP_VERSION
    QDialog d( 0, "fd", true );;
    OFontSelector s ( true,  &d, "fontsel");
    QVBoxLayout l ( &d );
    l.addWidget( &s );
    s.setSelectedFont (  f );
    QPushButton b ( "OK",  &d );
    l.addWidget( &b );
    qDebug("size %d ", f.bold());
    QObject::connect( &b, SIGNAL( clicked () ), &d, SLOT ( accept () ) );
    d.show();
    ok = false;
    if ( d.exec () ) {
        ok = true;
        return s.selectedFont ( );
    }
    return f;
#else
    return  QFontDialog::getFont ( &ok, f, 0, "fontdialog" );
#endif
}
