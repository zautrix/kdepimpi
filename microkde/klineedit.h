#ifndef MINIKDE_KLINEEDIT_H
#define MINIKDE_KLINEEDIT_H

#ifdef QT_4_COMPAT
#include <qt4compat.h>
#include <QKeyEvent>
#else
#endif    
#include <qlineedit.h>

#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#endif


class KLineEdit : public QLineEdit
{
    
  Q_OBJECT

  public:
    KLineEdit( QWidget *parent=0, const char *name=0 ) :
      QLineEdit( parent, name ) 
        {
#ifndef DESKTOP_VERSION
    QPEApplication::setStylusOperation( this, QPEApplication::RightOnHold );
#endif
        }
        void keyPressEvent ( QKeyEvent * e) 
        {
            switch ( e->key() ) {
            case Qt::Key_Down:
                emit scrollDOWN();
                e->accept();
                break; 
            case Qt::Key_Up:
                emit scrollUP();
                e->accept();
                break; 
            default:
                QLineEdit::keyPressEvent ( e );
                break;
            }
            
        }
    void setTrapReturnKey( bool ) {}
 signals:
    void scrollUP();
    void scrollDOWN();
};

#endif
