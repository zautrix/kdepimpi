/****************************************************************************
** Form interface generated from reading ui file 'addentrywnd.ui'
**
** Created: Tue Sep 14 13:42:03 2004
**      by: The User Interface Compiler ($Id: addentrywnd.h,v 1.1 2004/09/15 17:55:40 ulf69 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ADDENTRYWND_H
#define ADDENTRYWND_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QGroupBox;

class addEntryWnd : public QDialog
{
    Q_OBJECT

public:
    addEntryWnd( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~addEntryWnd();

    QLineEdit* launcherLineEdit;
    QPushButton* cancelButton;
    QPushButton* generateButton;
    QLineEdit* descLineEdit;
    QComboBox* categoryComboBox;
    QLineEdit* usernameLineEdit;
    QLineEdit* pwLineEdit;
    QLineEdit* urlLineEdit;
    QLabel* textLabel1_3;
    QLabel* textLabel1;
    QLabel* textLabel1_2;
    QLabel* textLabel2;
    QLabel* textLabel3;
    QLabel* textLabel2_2;
    QLabel* textLabel3_2;
    QLabel* textLabel1_4;
    QLabel* textLabel3_3;
    QLabel* textLabel2_3;
    QLabel* textLabel5;
    QLabel* textLabel4_2;
    QPushButton* revealButton;
    QPushButton* okButton;
    QGroupBox* groupBox1;
    QLabel* commentDummy;
    QPushButton* advancedCommentButton;

public slots:
    virtual void okButton_slot();
    virtual void cancelButton_slot();
    virtual void revealButton_slot();
    virtual void generateButton_slot();
    virtual void advancedCommentButton_slot(bool on);

protected:

protected slots:
    virtual void languageChange();

};

#endif // ADDENTRYWND_H
