#ifndef MICROKDE_KLINEEDITDLG_H
#define MICROKDE_KLINEEDITDLG_H

#include "kdialogbase.h"
#include <klineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qpushbutton.h>

class KLineEditDlg : public QDialog
{
  public:
    KLineEditDlg( const QString & text, const QString & editText, QWidget *parent ) :
      QDialog( parent,"lineedit", true ) {
        QLabel* lab = new QLabel( text, this );
        mEdit = new KLineEdit( this );
        QVBoxLayout* vl = new QVBoxLayout( this );
        vl->setSpacing(5);
        vl->setMargin(7);
        vl->addWidget( lab );
        vl->addWidget( mEdit );
        mEdit->setText( editText );
        QPushButton * p = new QPushButton (" OK ", this );
        vl->addWidget( p );
        connect( p, SIGNAL ( clicked () ), this , SLOT (accept() ) );
    }

    QString text() { return mEdit->text(); }
 private:
    KLineEdit* mEdit;
};

#endif
