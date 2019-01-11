/****************************************************************************
** Form interface generated from reading ui file 'advcommedit.ui'
**
** Created: Mon Sep 13 14:27:05 2004
**      by: The User Interface Compiler ($Id: advcommedit.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ADVCOMMEDIT_H
#define ADVCOMMEDIT_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QListBox;
class QListBoxItem;

class advCommEdit : public QDialog
{
    Q_OBJECT

public:
    advCommEdit( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~advCommEdit();

    QLabel* textLabel2;
    QGroupBox* groupBox1;
    QLabel* previewDummy;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* refreshPreviewButton;
    QLineEdit* titleLineEdit;
    QLabel* textLabel3;
    QListBox* entryListBox;
    QPushButton* editButton;
    QPushButton* delButton;
    QPushButton* addTblButton;

public slots:
    virtual void okButton_slot();
    virtual void cancelButton_slot();
    virtual void updatePreview();
    virtual void addSubtblButton_slot();
    virtual void editButton_slot();
    virtual void delButton_slot();

protected:

protected slots:
    virtual void languageChange();

};

#endif // ADVCOMMEDIT_H
