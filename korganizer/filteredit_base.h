/****************************************************************************
** Form interface generated from reading ui file 'filteredit_base.ui'
**
** Created: Fr Jan 21 21:25:30 2005
**      by: The User Interface Compiler ($Id: filteredit_base.h,v 1.4 2005/01/21 21:06:34 zautrix Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FILTEREDIT_BASE_H
#define FILTEREDIT_BASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QButtonGroup;
class QRadioButton;
class QPushButton;
class QListBox;
class QListBoxItem;
class QLabel;
class QCheckBox;

class FilterEdit_base : public QWidget
{
    Q_OBJECT

public:
    FilterEdit_base( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FilterEdit_base();

    QFrame* mCriteriaFrame;
    QButtonGroup* ButtonGroup1;
    QRadioButton* mCatShowCheck;
    QRadioButton* mCatHideCheck;
    QPushButton* mCatEditButton;
    QListBox* mCatList;
    QLabel* textLabel1;
    QCheckBox* mRecurringCheck;
    QCheckBox* mCompletedCheck;
    QLabel* textLabel1_2;
    QCheckBox* mEventCheck;
    QCheckBox* mTodoCheck;
    QCheckBox* mJournalCheck;
    QLabel* textLabel2;
    QCheckBox* mPublicCheck;
    QCheckBox* mPrivateCheck;
    QCheckBox* mConfidentialCheck;

protected:
    QVBoxLayout* FilterEdit_baseLayout;
    QVBoxLayout* mCriteriaFrameLayout;
    QGridLayout* ButtonGroup1Layout;
    QHBoxLayout* layout4;
    QHBoxLayout* layout7;
    QHBoxLayout* layout9;
    QHBoxLayout* layout2;

protected slots:
    virtual void languageChange();

};

#endif // FILTEREDIT_BASE_H
