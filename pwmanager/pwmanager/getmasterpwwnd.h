/****************************************************************************
** Form interface generated from reading ui file 'getmasterpwwnd.ui'
**
** Created: Tue Sep 14 15:32:37 2004
**      by: The User Interface Compiler ($Id: getmasterpwwnd.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GETMASTERPWWND_H
#define GETMASTERPWWND_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLabel;
class QLineEdit;

class getMasterPwWnd : public QDialog
{
    Q_OBJECT

public:
    getMasterPwWnd( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~getMasterPwWnd();

    QPushButton* okButton;
    QPushButton* cancelButton;
    QLabel* textLabel1;
    QLineEdit* pwLineEdit;

public slots:
    virtual void okButton_slot();
    virtual void cancelButton_slot();

protected:

protected slots:
    virtual void languageChange();

};

#endif // GETMASTERPWWND_H
