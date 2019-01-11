#ifndef MICROKDE_KCOLORBUTTON_H
#define MICROKDE_KCOLORBUTTON_H

#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else


#endif
#include <qpushbutton.h>
#include <qcolor.h>
#include <kglobal.h>
#include <qpixmap.h>
#include <qlabel.h>


class KColorButton : public QPushButton
{
Q_OBJECT
public:
    KColorButton( QWidget *p );
    void setColor ( const QColor &);
    void setID ( int i) ;
    QColor color() const { return mColor ; }
 signals:
    void changed(const QColor &);
    void changedID(const QColor &, int id);
private slots:
    void   edit();
 private:
 QColor mColor;
 int id;
};

#endif
