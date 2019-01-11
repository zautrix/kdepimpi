#include "kcolordialog.h"
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qslider.h>
#include <qhbox.h>
#include <qapplication.h>
#include <qpushbutton.h>

#include <kglobal.h>
QColor KColorDialog::getColor( ) const
{
     return color();
}

KColorDialog::KColorDialog( QWidget *p ):FNColorDialog( p, "input-dialog" )
{
   
}
