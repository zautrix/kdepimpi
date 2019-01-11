#ifndef MICROKDE_KSYSTEMTRAY_H
#define MICROKDE_KSYSTEMTRAY_H

#include <qlabel.h>

class KSystemTray : public QLabel
{ 
    Q_OBJECT
  public:
    KSystemTray( QWidget *parent = 0 );
    void mousePressEvent( QMouseEvent *);
};

#endif
