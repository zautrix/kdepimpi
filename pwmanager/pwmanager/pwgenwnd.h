/****************************************************************************
** Form interface generated from reading ui file 'pwgenwnd.ui'
**
** Created: Tue Sep 14 15:32:37 2004
**      by: The User Interface Compiler ($Id: pwgenwnd.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PWGENWND_H
#define PWGENWND_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QCheckBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;

class pwGenWnd : public QDialog
{
    Q_OBJECT

public:
    pwGenWnd( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~pwGenWnd();

    QGroupBox* groupBox1;
    QCheckBox* int_charLowerCheckBox;
    QLabel* textLabel4;
    QCheckBox* int_charUpperCheckBox;
    QLabel* textLabel5;
    QCheckBox* int_charNumCheckBox;
    QLabel* textLabel6;
    QCheckBox* int_charSpecCheckBox;
    QLabel* textLabel7;
    QCheckBox* int_charBlankCheckBox;
    QCheckBox* int_charUserCheckBox;
    QLineEdit* int_userDefLineEdit;
    QLabel* textLabel2;
    QSpinBox* int_lenSpinBox;
    QCheckBox* int_filterCheckBox;
    QPushButton* genButton;
    QPushButton* cancelButton;

public slots:
    virtual void cancelButton_slot();
    virtual void genButton_slot();

protected:
    QVBoxLayout* pwGenWndLayout;
    QVBoxLayout* groupBox1Layout;
    QHBoxLayout* layout2;
    QSpacerItem* spacer1;
    QHBoxLayout* layout3;
    QSpacerItem* spacer2;
    QHBoxLayout* layout4;
    QSpacerItem* spacer3;
    QHBoxLayout* layout6;
    QSpacerItem* spacer4;
    QHBoxLayout* layout7;
    QSpacerItem* spacer5;
    QHBoxLayout* layout8;
    QHBoxLayout* layout1;

protected slots:
    virtual void languageChange();

};

#endif // PWGENWND_H
