#ifndef MICROKDE_KFILEDIALOG_H
#define MICROKDE_KFILEDIALOG_H

#include <qstring.h>
#include <qwidget.h>

class KFileDialog
{
  public:
  
    static QString getSaveFileName( const QString &,
                                    const QString &, QWidget * );
        

    static QString getOpenFileName( const QString &,
                                    const QString &, QWidget * );
   
    static QString getExistingDirectory( const QString &,
                                    const QString &, QWidget * );
};

#endif
