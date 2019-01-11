#ifndef MINIKDE_KDIALOG_H
#define MINIKDE_KDIALOG_H



#include <qdialog.h>
#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else

#include <qprogressbar.h>
#include <qlayout.h>

#endif

 
class QLabel;
class QPushButton ;

class KDialog : public QDialog
{ 
    Q_OBJECT
  public:
    KDialog( QWidget *parent=0, const char *name=0, bool modal=true );

    static int spacingHint();
    static int marginHint();
    static int spacingHintSmall();
    static int marginHintSmall();
};
class KProgressDialog : public QDialog
{ 
    Q_OBJECT
  public:
    KProgressDialog( QWidget *parent=0, const char *name=0, bool modal=false );
    void setLabelText ( const QString & );
    void setTotalSteps ( int totalSteps );
    void setProgress ( int progress );
    void accept();
    void reject();
 private:
    QLabel * mLabel;
    QProgressBar *mBar;
    QPushButton *mButton;
 signals:
    void cancelled (); 
};

#endif
