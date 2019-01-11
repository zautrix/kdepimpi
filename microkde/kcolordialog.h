#ifndef MINIKDE_KCOLORDIALOG_H
#define MINIKDE_KCOLORDIALOG_H

#include <qcolor.h>

#include <qdialog.h>
#include <qslider.h>
#include <qlabel.h>
#include <fncolordialog.h>

class KColorDialog : public FNColorDialog 
{
Q_OBJECT
  public:
    KColorDialog( QWidget *p );
    QColor getColor( ) const;
};


#endif
