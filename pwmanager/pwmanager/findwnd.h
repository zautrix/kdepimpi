/****************************************************************************
** Form interface generated from reading ui file 'findwnd.ui'
**
** Created: Mon Sep 13 13:45:42 2004
**      by: The User Interface Compiler ($Id: findwnd.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FINDWND_H
#define FINDWND_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QButtonGroup;
class QRadioButton;

class findWnd : public QDialog
{
    Q_OBJECT

public:
    findWnd( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~findWnd();

    QLabel* textLabel1;
    QLineEdit* findLineEdit;
    QPushButton* findButton;
    QPushButton* closeButton;
    QCheckBox* exactCheckBox;
    QCheckBox* caseSensCheckBox;
    QButtonGroup* buttonGroup1;
    QRadioButton* descRadioButton;
    QRadioButton* pwRadioButton;
    QRadioButton* commentRadioButton;
    QRadioButton* nameRadioButton;
    QRadioButton* urlRadioButton;
    QRadioButton* launcherRadioButton;

public slots:
    virtual void findButton_slot();
    virtual void selectionChanged_slot();
    virtual void closeButton_slot();

protected:

protected slots:
    virtual void languageChange();

};

#endif // FINDWND_H
