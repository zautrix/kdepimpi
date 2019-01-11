/****************************************************************************
** Form interface generated from reading ui file 'setmasterpwwnd.ui'
**
** Created: Tue Sep 14 15:32:37 2004
**      by: The User Interface Compiler ($Id: setmasterpwwnd.h,v 1.1 2004/09/15 17:55:58 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SETMASTERPWWND_H
#define SETMASTERPWWND_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QTabWidget;
class QWidget;
class QLabel;
class QLineEdit;

class setMasterPwWnd : public QDialog
{
    Q_OBJECT

public:
    setMasterPwWnd( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~setMasterPwWnd();

    QPushButton* okButton;
    QPushButton* cancelButton;
    QTabWidget* mainTab;
    QWidget* tab;
    QLabel* textLabel2;
    QLabel* textLabel3;
    QLabel* textLabel1;
    QLineEdit* pwEdit_1;
    QLineEdit* pwEdit_2;
    QWidget* tab_2;
    QLabel* textLabel1_2;
    QLabel* textLabel2_2;
    QPushButton* selCardButton;
    QPushButton* genCardButton;
    QLabel* curCardIdLabel;

public slots:
    virtual void okButton_slot();
    virtual void cancelButton_slot();
    virtual void genCardButton_slot();
    virtual void selCardButton_slot();

protected:

protected slots:
    virtual void languageChange();

};

#endif // SETMASTERPWWND_H
