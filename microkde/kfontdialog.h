#ifndef MINIKDE_KFONTDIALOG_H
#define MINIKDE_KFONTDIALOG_H

#include <qfont.h>
#include <qdialog.h>

class KFontDialog
{
  public:
    enum { Accepted };
    
    static QFont getFont( const QFont & f, bool & ok);
};

#endif
