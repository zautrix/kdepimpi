/*
    This file is part of KOrganizer.
    Copyright (c) 1998 Preston Brown
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <qlayout.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qwhatsthis.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <libkdepim/kdateedit.h>

#include "koglobals.h"
#include "koprefs.h"
#include "klineedit.h"

#include "calendarview.h"
#include "koviewmanager.h"
#include "searchdialog.h"

SearchDialog::SearchDialog(Calendar *calendar,CalendarView *parent)
    : QVBox( 0  )

{
    mCalendar = calendar;
    QFrame *topFrame = new QFrame( this ) ;//plainPage();
    QVBoxLayout *layout = new QVBoxLayout(topFrame,KDialog::marginHint(),KDialog::spacingHint());

    // Search expression
    QHBoxLayout *subLayout = new QHBoxLayout();
    layout->addLayout(subLayout);
    /*
    searchLabel = new QLabel(topFrame);
    searchLabel->setText(i18n("Search for:"));
    subLayout->addWidget(searchLabel);
    */
    QPushButton *OkButton = new QPushButton(  i18n("Search for:"), topFrame );
    //OkButton->setDefault( true );
    connect(OkButton,SIGNAL(clicked()),SLOT(doSearch()));
    subLayout->addWidget(OkButton);
    searchEdit = new KLineEdit(topFrame);
    subLayout->addWidget(searchEdit);

    mAddItems = new QRadioButton( "+ ", topFrame );
    mSubItems = new QRadioButton( "- ", topFrame );
    mRefineItems = new QRadioButton( "< ", topFrame );
    subLayout->addWidget( mAddItems );
    subLayout->addWidget( mSubItems );
    subLayout->addWidget( mRefineItems );
    QFont fo ( mAddItems->font() );
    fo.setBold( true );
    fo.setPointSize( fo.pointSize() + 2 );
    mAddItems->setFont( fo );
    mSubItems->setFont( fo );
    mRefineItems->setFont( fo );
    connect( mAddItems  , SIGNAL( toggled ( bool ) ),this,SLOT(slot_add( bool )));
    connect( mSubItems , SIGNAL( toggled ( bool ) ),this,SLOT(slot_sub( bool )));
    connect( mRefineItems , SIGNAL( toggled ( bool ) ),this,SLOT(slot_refine( bool )));

    QPushButton *togButton = new QPushButton( "", topFrame );
    subLayout->addWidget(togButton);
    connect(togButton,SIGNAL(clicked()),SLOT(toggleCheckboxes()));
    togButton->setPixmap(SmallIcon("1updownarrow"));
    togButton->setMinimumWidth( togButton->sizeHint().height() );

    searchEdit->setText("*"); // Find all events by default
    searchEdit->setFocus();
    connect(searchEdit, SIGNAL(textChanged ( const QString & )),this,SLOT(searchTextChanged( const QString & )));
    connect(searchEdit, SIGNAL( returnPressed () ),this,SLOT(doSearch()));
    // Subjects to search
    // QGroupBox *subjectGroup = new QGroupBox(1,Vertical,i18n("Search In"),
    //                                        topFrame);

    incidenceGroup = new QHBox( topFrame );
    layout->addWidget(incidenceGroup);

    mSearchEvent = new QCheckBox(i18n("Events"),incidenceGroup);
    //mSearchEvent->setChecked(true);
    mSearchTodo = new QCheckBox(i18n("Todos"),incidenceGroup);
    mSearchJournal = new QCheckBox(i18n("Journals"),incidenceGroup);

    subjectGroup = new QHBox( topFrame );
    layout->addWidget(subjectGroup);

    mSummaryCheck = new QCheckBox(i18n("Summary/Loc."),subjectGroup);
    mSummaryCheck->setChecked(true);
    mDescriptionCheck = new QCheckBox(i18n("Details"),subjectGroup);
    mCategoryCheck = new QCheckBox(i18n("Categories"),subjectGroup);

    attendeeGroup = new QHBox( topFrame );
    layout->addWidget(attendeeGroup );
    new QLabel( i18n("Attendee:"),attendeeGroup );
    mSearchAName = new QCheckBox(i18n("Name"),attendeeGroup  );
    mSearchAEmail = new QCheckBox(i18n("Email"), attendeeGroup );
    // Date range
    // QGroupBox *rangeGroup = new QGroupBox(1,Horizontal,i18n("Date Range"),
    //                                    topFrame);
    // layout->addWidget(rangeGroup);

    QWidget *rangeWidget = new QWidget(topFrame);
    int space = KDialog::spacingHint();
    if ( QApplication::desktop()->width() <= 240 ) space = 1;
    QHBoxLayout *rangeLayout = new QHBoxLayout(rangeWidget,0,space);
    rangeLayout->addWidget(new QLabel(i18n("From:"),rangeWidget));
    mStartDate = new KDateEdit(rangeWidget);
    rangeLayout->addWidget(mStartDate);
    rangeLayout->addWidget(new QLabel(i18n("To:"),rangeWidget));
    mEndDate = new KDateEdit(rangeWidget);
    mEndDate->setDate(QDate::currentDate().addDays(365));
    rangeLayout->addWidget(mEndDate);
    QToolButton *wt = QWhatsThis::whatsThisButton (  rangeWidget );
    rangeLayout->addWidget( (QWidget*)wt );
    layout->addWidget(rangeWidget);
    // Results list view
    listView = new KOListView(mCalendar,topFrame);
    layout->addWidget(listView);
    listView->showCompletedTodos();
    //layout->setStretchFactor( listView, 333 );
    //listView->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred  ,QSizePolicy::Expanding) );
    //listView->setMaximumHeight( 50 );
    listView->readSettings(KOGlobals::config(),"SearchListView Layout");
    connect(searchEdit,SIGNAL(scrollDOWN()),SLOT(setFocusToList()));

    setCaption( i18n("KO/Pi Find: "));
#ifdef DESKTOP_VERSION
    OkButton = new QPushButton(  i18n("Close"), this  );
    connect(OkButton,SIGNAL(clicked()),SLOT(hide()));
#endif
}

SearchDialog::~SearchDialog()
{
  
}
void SearchDialog::slot_add( bool b )
{
    if ( b ) {
        if ( mSubItems->isOn() ) mSubItems->toggle();
        if ( mRefineItems->isOn() ) mRefineItems->toggle();
        setCaption( i18n("Matching items will be added to list"));
    } else
        setCaption( i18n("List will be cleared before search"));
}
void SearchDialog::slot_sub( bool b)
{
  if ( b ) {
        if ( mRefineItems->isOn() ) mRefineItems->toggle();
        if ( mAddItems->isOn() ) mAddItems->toggle();
        setCaption( i18n("Matching items will be removed from list"));
    } else
        setCaption( i18n("List will be cleared before search"));
}
void SearchDialog::slot_refine( bool b)
{
  if ( b ) {
        if ( mSubItems->isOn() ) mSubItems->toggle();
        if ( mAddItems->isOn() ) mAddItems->toggle();
        setCaption( i18n("Search on displayed list only"));
    } else
        setCaption( i18n("List will be cleared before search"));
}
void SearchDialog::toggleCheckboxes()
{
    if ( incidenceGroup->isVisible() ) {
        incidenceGroup->hide() ;
        subjectGroup->hide() ;
        attendeeGroup->hide() ;
    } else {
        incidenceGroup->show() ;
        subjectGroup->show() ;
        attendeeGroup->show() ;
    }
}
void SearchDialog::raiseAndSelect()
{

    static int currentState = 0;

    if ( !mSearchJournal->isChecked() && !mSearchTodo->isChecked()  && !mSearchEvent->isChecked() )
        currentState = 0;
    int newState = 0;
    if ( KOPrefs::instance()->mCurrentDisplayedView == VIEW_J_VIEW ) {
        newState = VIEW_J_VIEW;
    }
    else if ( KOPrefs::instance()->mCurrentDisplayedView == VIEW_T_VIEW ) {
        newState = VIEW_T_VIEW;
    }
    else {
        newState = VIEW_A_VIEW;
    }
    if ( newState != currentState ) {
        if ( KOPrefs::instance()->mCurrentDisplayedView == VIEW_J_VIEW ) {
            if ( ! mSearchJournal->isChecked() ) {
                mSearchJournal->setChecked( true );
                mSearchTodo->setChecked( false );
                mSearchEvent->setChecked( false );
                mSummaryCheck->setChecked( true ); 
                mDescriptionCheck->setChecked( true ); 

            }
        }
        else if ( KOPrefs::instance()->mCurrentDisplayedView == VIEW_T_VIEW ) {
            if ( !  mSearchTodo->isChecked()    ) {
                mSearchTodo->setChecked( true );
                mSearchJournal->setChecked( false );
                mSearchEvent->setChecked( false );
            }
        }
        else {
            if ( ! mSearchEvent->isChecked() ) {
                mSearchEvent->setChecked( true );
                mSearchJournal->setChecked( false );
                mSearchTodo->setChecked( false );
            }
        }
    }
    currentState = newState;
    raise();
}
void SearchDialog::setFocusToList()
{
    listView->resetFocus();
}
void SearchDialog::accept()
{
    doSearch();
}
void SearchDialog::updateList()
{
    if ( KOPrefs::instance()->mGlobalUpdateDisabled ) {
        listView->clear();
        return;
    }
    //listView->updateList();
    if ( isVisible() ) {
        updateView();
        //qDebug("SearchDialog::updated ");
    }
    else {
        listView->clear();
        //qDebug("SearchDialog::cleared ");

    }
}
void SearchDialog::searchTextChanged( const QString &_text )
{
#if 0
    enableButton( KDialogBase::User1, !_text.isEmpty() );
#endif
}

void SearchDialog::doSearch()
{
    QRegExp re;
    re.setWildcard(true); // most people understand these better.
    re.setCaseSensitive(false);
    QString st = searchEdit->text();
    if ( st.right(1) != "*")
        st += "*";
    re.setPattern(st);
    if (!mSearchEvent->isChecked() && !mSearchTodo->isChecked() && !mSearchJournal->isChecked() ) {
        KMessageBox::sorry(this,
                           i18n("Please select at least one\nof the types to search for:\n\nEvents\nTodos\nJournals"));
        return;
    }
    if (!re.isValid() ) {
        KMessageBox::sorry(this,
                           i18n("Invalid search expression,\ncannot perform "
                                "the search.\nPlease enter a search expression\n"
                                "using the wildcard characters\n '*' and '?'"
                                "where needed."));
        return;
    }
    search(re);
    listView->setStartDate( mStartDate->date() );
    listView->showEvents(mMatchedEvents);
    listView->addTodos(mMatchedTodos);
    listView->addJournals(mMatchedJournals);
    if (mMatchedEvents.count() + mMatchedJournals.count() + mMatchedTodos.count() == 0) {
        setCaption(i18n("No items found. Use '*' and '?' where needed."));
    } else {
        QString mess; 
        mess = mess.sprintf( i18n("%d item(s) found."), mMatchedEvents.count()+ mMatchedJournals.count() + mMatchedTodos.count() );
        setCaption( i18n("KO/Pi Find: ") + mess);

    }
    searchEdit->setFocus();
}
void SearchDialog::updateConfig()
{
    listView->updateConfig();
}
void SearchDialog::updateView()
{
    //qDebug("SearchDialog::updateView() %d ", isVisible());
    QRegExp re;
    re.setWildcard(true); // most people understand these better.
    re.setCaseSensitive(false); 
    QString st = searchEdit->text();
    if ( st.right(1) != "*")
        st += "*";
    re.setPattern(st);
    mMatchedEvents.clear();
    mMatchedTodos.clear();
    mMatchedJournals.clear();
    if (re.isValid()) {
        search(re);
    }
    listView->setStartDate( mStartDate->date() );
    listView->showEvents(mMatchedEvents);
    listView->addTodos(mMatchedTodos);
    listView->addJournals(mMatchedJournals);
}

void SearchDialog::search(const QRegExp &re)
{
    QPtrList<Event> events;
    if ( !mAddItems->isChecked() && !mSubItems->isChecked() ) {
        if ( mRefineItems->isChecked() ) events = mMatchedEvents;
        mMatchedEvents.clear();
    } 
    if ( mSearchEvent->isChecked() ) {
        if ( !mRefineItems->isChecked() )
            events = mCalendar->events( mStartDate->date(),
                                                mEndDate->date(),
                                                false /*mInclusiveCheck->isChecked()*/ );


        Event *ev;
        for(ev=events.first();ev;ev=events.next()) {
            if (mSummaryCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                if (re.search(ev->summary()) != -1) 
#else
                    if (re.match(ev->summary()) != -1) 
#endif
                        {   
                            if ( mSubItems->isChecked() )
                                mMatchedEvents.remove(ev);
                            else {
                                if ( !mMatchedEvents.contains( ev ) )
                                    mMatchedEvents.append(ev);
                            }
                            continue;
                        }
#if QT_VERSION >= 0x030000
                if (re.search(ev->location()) != -1) 
#else
                    if (re.match(ev->location()) != -1) 
#endif
                        {   
                            if ( mSubItems->isChecked() )
                                mMatchedEvents.remove(ev);
                            else{
                                if ( !mMatchedEvents.contains( ev ) )
                                    mMatchedEvents.append(ev);
                            }
                            continue;
                        }
            }
            if (mDescriptionCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                if (re.search(ev->description()) != -1) 
#else
                    if (re.match(ev->description()) != -1) 
#endif
                        {       
                            if ( mSubItems->isChecked() )
                                mMatchedEvents.remove(ev);
                            else{
                                if ( !mMatchedEvents.contains( ev ) )
                                    mMatchedEvents.append(ev);
                            }
                            continue;
                        }
            }
            if (mCategoryCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                if (re.search(ev->categoriesStr()) != -1) 
#else
                    if (re.match(ev->categoriesStr()) != -1) 
#endif
                        {
                                 
                            if ( mSubItems->isChecked() )
                                mMatchedEvents.remove(ev);
                            else{
                                if ( !mMatchedEvents.contains( ev ) )
                                    mMatchedEvents.append(ev);
                            }
                            continue;
                        }
            }
            if ( mSearchAName->isChecked() || mSearchAEmail->isChecked() ) {
                QPtrList<Attendee> tmpAList = ev->attendees();
                Attendee *a;
                for (a = tmpAList.first(); a; a = tmpAList.next()) {
                    if (mSearchAName->isChecked()) {
#if QT_VERSION >= 0x030000
                        if (re.search(a->name()) != -1) 
#else
                            if (re.match(a->name()) != -1) 
#endif
                                {       
                                    if ( mSubItems->isChecked() )
                                        mMatchedEvents.remove(ev);
                                    else{
                                        if ( !mMatchedEvents.contains( ev ) )
                                            mMatchedEvents.append(ev);
                                    }
                                    break;
                                }
                    }
                    if (mSearchAEmail->isChecked()) {
#if QT_VERSION >= 0x030000
                        if (re.search(a->email()) != -1) 
#else
                            if (re.match(a->email()) != -1) 
#endif
                                {     
                                    if ( mSubItems->isChecked() )
                                        mMatchedEvents.remove(ev);
                                    else{
                                        if ( !mMatchedEvents.contains( ev ) )
                                            mMatchedEvents.append(ev);
                                    }
                                    break;
                                }
                    }
                }
            }
        }
    }
    QPtrList<Todo> todos;

    if ( !mAddItems->isChecked() && !mSubItems->isChecked() ) {
        if ( mRefineItems->isChecked() ) todos = mMatchedTodos ;
        mMatchedTodos.clear();
    } 

    if ( mSearchTodo->isChecked() ) {
        if ( !mRefineItems->isChecked() ) todos = mCalendar->todos( );
        Todo *tod;
        for(tod=todos.first();tod;tod=todos.next()) {
            if (mSummaryCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                if (re.search(tod->summary()) != -1) 
#else
                    if (re.match(tod->summary()) != -1) 
#endif
                        { 
                            if ( mSubItems->isChecked() )
                                mMatchedTodos.remove(tod);
                            else if (!mMatchedTodos.contains( tod )) 
                                mMatchedTodos.append(tod);
                            continue;
                        }
            }
            if (mDescriptionCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                if (re.search(tod->description()) != -1) 
#else
                    if (re.match(tod->description()) != -1) 
#endif
                        { 
                            if ( mSubItems->isChecked() )
                                mMatchedTodos.remove(tod);
                            else if (!mMatchedTodos.contains( tod )) 
                                mMatchedTodos.append(tod);
                            continue;
                        }
            }
            if (mCategoryCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                if (re.search(tod->categoriesStr()) != -1) 
#else
                    if (re.match(tod->categoriesStr()) != -1) 
#endif
                        {    
                            if ( mSubItems->isChecked() )
                                mMatchedTodos.remove(tod);
                            else if (!mMatchedTodos.contains( tod )) 
                                mMatchedTodos.append(tod);                              
                            continue;
                        }
            }
            if ( mSearchAName->isChecked() || mSearchAEmail->isChecked() ) {
                QPtrList<Attendee> tmpAList = tod->attendees();
                Attendee *a;
                for (a = tmpAList.first(); a; a = tmpAList.next()) {
                    if (mSearchAName->isChecked()) {
#if QT_VERSION >= 0x030000
                        if (re.search(a->name()) != -1) 
#else
                            if (re.match(a->name()) != -1) 
#endif
                                {     
                                    if ( mSubItems->isChecked() )
                                        mMatchedTodos.remove(tod);
                                    else if (!mMatchedTodos.contains( tod )) 
                                        mMatchedTodos.append(tod);
                                    break;
                                }
                    }
                    if (mSearchAEmail->isChecked()) {
#if QT_VERSION >= 0x030000
                        if (re.search(a->email()) != -1) 
#else
                            if (re.match(a->email()) != -1) 
#endif
                                {    
                                    if ( mSubItems->isChecked() )
                                        mMatchedTodos.remove(tod);
                                    else if (!mMatchedTodos.contains( tod )) 
                                        mMatchedTodos.append(tod);
                                    break;
                                }
                    }
                }
            }
        }
    }

    QPtrList<Journal> journals;
    if ( !mAddItems->isChecked() && !mSubItems->isChecked() ) {
        if ( mRefineItems->isChecked() ) journals = mMatchedJournals ;
        mMatchedJournals.clear();
    } 
    if (mSearchJournal->isChecked() ) {
        if ( ! mRefineItems->isChecked() ) journals = mCalendar->journals();
        Journal* journ;
        
        for(journ=journals.first();journ;journ=journals.next()) {
            if ( journ->dtStart().date() <= mEndDate->date() 
                 &&journ->dtStart().date() >= mStartDate->date()) {
                if (mDescriptionCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                    if (re.search(journ->description()) != -1) 
#else
                        if (re.match(journ->description()) != -1) 
#endif
                            {
                                if ( mSubItems->isChecked() )
                                    mMatchedJournals.remove(journ);
                                else if (!mMatchedJournals.contains( journ )) 
                                    mMatchedJournals.append(journ);
                                continue;
                            }
                }
                if (mSummaryCheck->isChecked()) {
#if QT_VERSION >= 0x030000
                    if (re.search(journ->summary()) != -1) 
#else
                        if (re.match(journ->summary()) != -1) 
#endif
                            {
                                if ( mSubItems->isChecked() )
                                    mMatchedJournals.remove(journ);
                                else if (!mMatchedJournals.contains( journ )) 
                                    mMatchedJournals.append(journ);
                                continue;
                            }
                }
            }
        }
    }
    if ( mMatchedEvents.count() + mMatchedJournals.count() + mMatchedTodos.count()  < 1 ) {
        qDebug("count %d ", mMatchedEvents.count() + mMatchedJournals.count() + mMatchedTodos.count() );
        if ( mRefineItems->isChecked() )
            mRefineItems->setChecked( false );
        else if ( mSubItems->isChecked() )
            mSubItems->setChecked( false );
    }
}

void SearchDialog::keyPressEvent ( QKeyEvent *e) 
{
    switch ( e->key() ) {
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_F:
        if ( e->state() == Qt::ControlButton ) {

        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        doSearch();
        break;
   
    default:
        e->ignore();
    }
}

//mMatchedJournals;
