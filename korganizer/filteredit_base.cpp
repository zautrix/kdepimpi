/****************************************************************************
** Form implementation generated from reading ui file 'filteredit_base.ui'
**
** Created: Fr Jan 21 21:25:30 2005
**      by: The User Interface Compiler ($Id: filteredit_base.cpp,v 1.5 2005/01/29 07:48:33 zautrix Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "filteredit_base.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "tr.h"

/*
 *  Constructs a FilterEdit_base as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
FilterEdit_base::FilterEdit_base( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "FilterEdit_base" );
    FilterEdit_baseLayout = new QVBoxLayout( this, 0, 0, "FilterEdit_baseLayout"); 

    mCriteriaFrame = new QFrame( this, "mCriteriaFrame" );
    mCriteriaFrame->setFrameShape( QFrame::StyledPanel );
    mCriteriaFrame->setFrameShadow( QFrame::Raised );
    mCriteriaFrameLayout = new QVBoxLayout( mCriteriaFrame, 3, 2, "mCriteriaFrameLayout"); 

    ButtonGroup1 = new QButtonGroup( mCriteriaFrame, "ButtonGroup1" );
    ButtonGroup1->setColumnLayout(0, Qt::Vertical );
    ButtonGroup1->layout()->setSpacing( 6 );
    ButtonGroup1->layout()->setMargin( 6 );
    ButtonGroup1Layout = new QGridLayout( ButtonGroup1->layout() );
    ButtonGroup1Layout->setAlignment( Qt::AlignTop );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    mCatShowCheck = new QRadioButton( ButtonGroup1, "mCatShowCheck" );
    layout4->addWidget( mCatShowCheck );

    mCatHideCheck = new QRadioButton( ButtonGroup1, "mCatHideCheck" );
    layout4->addWidget( mCatHideCheck );

    mCatEditButton = new QPushButton( ButtonGroup1, "mCatEditButton" );
    layout4->addWidget( mCatEditButton );

    ButtonGroup1Layout->addLayout( layout4, 0, 0 );

    mCatList = new QListBox( ButtonGroup1, "mCatList" );

    ButtonGroup1Layout->addWidget( mCatList, 1, 0 );
    mCriteriaFrameLayout->addWidget( ButtonGroup1 );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    textLabel1 = new QLabel( mCriteriaFrame, "textLabel1" );
    layout7->addWidget( textLabel1 );

    mRecurringCheck = new QCheckBox( mCriteriaFrame, "mRecurringCheck" );
    layout7->addWidget( mRecurringCheck );

    mCompletedCheck = new QCheckBox( mCriteriaFrame, "mCompletedCheck" );
    layout7->addWidget( mCompletedCheck );
    mCriteriaFrameLayout->addLayout( layout7 );

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

    textLabel1_2 = new QLabel( mCriteriaFrame, "textLabel1_2" );
    layout9->addWidget( textLabel1_2 );

    mEventCheck = new QCheckBox( mCriteriaFrame, "mEventCheck" );
    layout9->addWidget( mEventCheck );

    mTodoCheck = new QCheckBox( mCriteriaFrame, "mTodoCheck" );
    layout9->addWidget( mTodoCheck );

    mJournalCheck = new QCheckBox( mCriteriaFrame, "mJournalCheck" );
    layout9->addWidget( mJournalCheck );
    mCriteriaFrameLayout->addLayout( layout9 );

    layout2 = new QHBoxLayout( 0, 0, 0, "layout2"); 

    textLabel2 = new QLabel( mCriteriaFrame, "textLabel2" );
    layout2->addWidget( textLabel2 );

    mPublicCheck = new QCheckBox( mCriteriaFrame, "mPublicCheck" );
    layout2->addWidget( mPublicCheck );

    mPrivateCheck = new QCheckBox( mCriteriaFrame, "mPrivateCheck" );
    layout2->addWidget( mPrivateCheck );

    mConfidentialCheck = new QCheckBox( mCriteriaFrame, "mConfidentialCheck" );
    mConfidentialCheck->setChecked( FALSE );
    layout2->addWidget( mConfidentialCheck );
    mCriteriaFrameLayout->addLayout( layout2 );
    FilterEdit_baseLayout->addWidget( mCriteriaFrame );
    languageChange();
    resize( QSize(300, 200).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
FilterEdit_base::~FilterEdit_base()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void FilterEdit_base::languageChange()
{
    setCaption( tr( "FilterEditor" ) );
    ButtonGroup1->setTitle( tr( "Categories" ) );
    mCatShowCheck->setText( tr( "Include" ) );
    mCatHideCheck->setText( tr( "Exclude" ) );
    mCatEditButton->setText( tr( "Edit Selection..." ) );
    textLabel1->setText( tr( "Exclude" ) );
    if (QApplication::desktop()->width() > 480 ) {
        mCompletedCheck->setText( tr( "completed to-dos" ) );
        mRecurringCheck->setText( tr( "recurring events" ) );
    }
    else {
        mCompletedCheck->setText( tr( "compl.todos" ) );
        mRecurringCheck->setText( tr( "recurr. events" ) );
    }
    textLabel1_2->setText( tr( "Exclude" ) );
    mEventCheck->setText( tr( "events" ) );
    mTodoCheck->setText( tr( "todos" ) );
    mJournalCheck->setText( tr( "journals" ) );
    textLabel2->setText( tr( "Include" ) );
    mPublicCheck->setText( tr( "public" ) );
    mPrivateCheck->setText( tr( "private" ) );
    mConfidentialCheck->setText( tr( "confidential" ) );
}

