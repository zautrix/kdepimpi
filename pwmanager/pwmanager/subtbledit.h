/****************************************************************************
** Form interface generated from reading ui file 'subtbledit.ui'
**
** Created: Tue Sep 14 15:32:37 2004
**      by: The User Interface Compiler ($Id: subtbledit.h,v 1.1 2004/09/15 17:55:58 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SUBTBLEDIT_H
#define SUBTBLEDIT_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class QPushButton;
class QGroupBox;
class QListBox;
class QListBoxItem;

class subTblEdit : public QDialog
{
    Q_OBJECT

public:
    subTblEdit( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~subTblEdit();

    QLabel* textLabel4;
    QLabel* textLabel5;
    QLineEdit* titleLineEdit;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QGroupBox* groupBox3;
    QLabel* textLabel6;
    QLabel* textLabel7;
    QPushButton* addButton;
    QLineEdit* valueLineEdit;
    QLineEdit* nameLineEdit;
    QListBox* entryListBox;
    QPushButton* delButton;

public slots:
    virtual void okButton_slot();
    virtual void cancelButton_slot();
    virtual void addButton_slot();
    virtual void delButton_slot();

protected:

protected slots:
    virtual void languageChange();

};

#endif // SUBTBLEDIT_H
