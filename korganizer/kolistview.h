/*
    This file is part of KOrganizer.
    Copyright (c) 1999 Preston Brown
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
#ifndef _KOLISTVIEW_H
#define _KOLISTVIEW_H

#include <qlistview.h>
#include <qmap.h>
#include <qdict.h>

#include <klistview.h>

#include <libkcal/incidence.h>

#include "koeventview.h"
#include "customlistviewitem.h"

using namespace KCal;

class KOListViewWhatsThis;

#include <qpushbutton.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qtimer.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qtooltip.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <kiconloader.h>
#include "kfiledialog.h"
#include "koprefs.h"
class KOAlarmPrefs : public QDialog
{
 Q_OBJECT
  public:
    KOAlarmPrefs( QWidget *par=0, const char *name=0 ) :
      QDialog( par, name, true ) 
    {
        setCaption( i18n("Alarm Options") );
        QVBoxLayout* alarmLayout = new QVBoxLayout( this );
        alarmLayout->setSpacing( 3 );
        alarmLayout->setMargin( 3 );
        QWidget *parent = this;
        mAlarmButton = new QCheckBox(i18n("Set reminder ON with offset to:"),parent);
        alarmLayout->addWidget(mAlarmButton);
        mAlarmTimeEdit = new QSpinBox ( 0, 9999, 1, parent, "mAlarmTimeEdit  "  ) ;
        mAlarmTimeEdit->setValue( 15 );
        alarmLayout->addWidget(mAlarmTimeEdit);
        mAlarmIncrCombo = new QComboBox(false, parent);
        mAlarmIncrCombo->insertItem(i18n("minute(s)"));
        mAlarmIncrCombo->insertItem(i18n("hour(s)"));
        mAlarmIncrCombo->insertItem(i18n("day(s)"));
        alarmLayout->addWidget(mAlarmIncrCombo);
        QHBox * hb = new QHBox ( parent );
        alarmLayout->addWidget(hb);
        mAlarmSoundButton = new QPushButton(hb);
        mAlarmSoundButton->setPixmap(SmallIcon("playsound"));
        mAlarmSoundButton->setToggleButton(true);
        connect(mAlarmSoundButton, SIGNAL(clicked()), SLOT(pickAlarmSound()));
        mAlarmProgramButton = new QPushButton(hb);
        mAlarmProgramButton->setPixmap(SmallIcon("run"));
        mAlarmProgramButton->setToggleButton(true);
        connect(mAlarmProgramButton, SIGNAL(clicked()), SLOT(pickAlarmProgram()));
        mAlarmSoundButton->setMaximumWidth( mAlarmSoundButton->sizeHint().width() + 4 );
        mAlarmProgramButton->setMaximumWidth(mAlarmProgramButton->sizeHint().width() + 4 ); 
        mAlarmLabel = new QLabel( this );
        alarmLayout->addWidget( mAlarmLabel );
        mAlarmLabel->setText( "..."+KOPrefs::instance()->mDefaultAlarmFile.right( 30 )  );
        mAlarmSound = KOPrefs::instance()->mDefaultAlarmFile;
        mAlarmSoundButton->setOn( true );
        QPushButton * ok = new QPushButton( i18n("Set Alarm!"), this );
        alarmLayout->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        alarmLayout->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
        connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
        resize( 200, 200 );
     
    }   



    QString mAlarmSound, mAlarmProgram ;
    QCheckBox* mAlarmButton;
    QSpinBox* mAlarmTimeEdit;
    QLabel* mAlarmLabel;
    QComboBox* mAlarmIncrCombo ;
    QPushButton* mAlarmSoundButton ,*mAlarmProgramButton;
private slots:

void pickAlarmSound()
{
    //QString prefix = mAlarmSound;
    if (!mAlarmSoundButton->isOn()) {
        //mAlarmSound = "";
        QToolTip::remove(mAlarmSoundButton);
        QToolTip::add(mAlarmSoundButton, i18n("No sound set"));
        mAlarmProgramButton->setOn(true);
        mAlarmSoundButton->setOn(false);
    } else {
        QString fileName(KFileDialog::getOpenFileName(mAlarmSound,
                                                      i18n("*.wav|Wav Files"), 0));
        if (!fileName.isEmpty()) {
            mAlarmSound = fileName;
            mAlarmLabel->setText( "..."+fileName.right( 30 )  ); 
            QToolTip::remove(mAlarmSoundButton);
            QString dispStr = i18n("Playing '%1'").arg(fileName);
            QToolTip::add(mAlarmSoundButton, dispStr);
            mAlarmProgramButton->setOn(false);
            mAlarmSoundButton->setOn(true);
        } else {
            mAlarmProgramButton->setOn(true);
            mAlarmSoundButton->setOn(false);
           
        }
    }
};

void pickAlarmProgram()
{
    if (!mAlarmProgramButton->isOn()) {
        //mAlarmProgram = "";
        QToolTip::remove(mAlarmProgramButton);
        QToolTip::add(mAlarmProgramButton, i18n("No program set"));
        mAlarmProgramButton->setOn(false);
        mAlarmSoundButton->setOn(true);
    } else {
        QString fileName(KFileDialog::getOpenFileName(mAlarmProgram,i18n("Procedure Alarm.: ")  , 0));
        if (!fileName.isEmpty()) {
            mAlarmProgram = fileName;
            mAlarmLabel->setText( "..."+fileName.right( 30 )  ); 
            QToolTip::remove(mAlarmProgramButton);
            QString dispStr = i18n("Running '%1'").arg(fileName);
            QToolTip::add(mAlarmProgramButton, dispStr);
            mAlarmSoundButton->setOn(false);
            mAlarmProgramButton->setOn(true);
        }  else {
            mAlarmProgramButton->setOn(false);
            mAlarmSoundButton->setOn(true);
        }
    }
};
  
};







typedef CustomListViewItem<Incidence *> KOListViewItem;

/**
  This class provides the initialisation of a KOListViewItem for calendar
  components using the Incidence::Visitor.
*/
class ListItemVisitor : public Incidence::Visitor
{
  public:
    ListItemVisitor(KOListViewItem *, QDate d);
    ~ListItemVisitor();
    
    bool visit(Event *);
    bool visit(Todo *);
    bool visit(Journal *);

  private:
    KOListViewItem *mItem;
    QDate mDate;
};

/**
  This class provides a multi-column list view of events.  It can
  display events from one particular day or several days, it doesn't 
  matter.  To use a view that only handles one day at a time, use
  KODayListView.

  @short multi-column list view of various events.
  @author Preston Brown <pbrown@kde.org>
  @see KOBaseView, KODayListView
*/
class KOListView;

class KOListViewListView : public KListView
{  
   Q_OBJECT
 public:
    KOListViewListView(KOListView * lv );
    bool hasMultiSelection(QListViewItem*);
    void printList();
 signals:
    void newEvent();
    void showIncidence( Incidence* );
  public slots:
    void popupMenu();
 private: 
    QPoint mEventPos;
    QPoint mEventGlobalPos;
    QTimer* mPopupTimer;
    int mYMousePos;
    void keyPressEvent ( QKeyEvent * ) ;
    void contentsMouseDoubleClickEvent(QMouseEvent *e);
    void contentsMousePressEvent(QMouseEvent *e);
    void contentsMouseReleaseEvent(QMouseEvent *e);
    void contentsMouseMoveEvent(QMouseEvent *e);
    bool mMouseDown;
};

class KOListView : public KOEventView
{
    Q_OBJECT
  public:
    KOListView(Calendar *calendar, QWidget *parent = 0, 
	       const char *name = 0);
    ~KOListView();

    virtual int maxDatesHint();
    virtual int currentDateCount();
    virtual QPtrList<Incidence> selectedIncidences();
    virtual DateList selectedDates();
    
    void showDates(bool show);
    Incidence* currentItem();
    void addTodos(QPtrList<Todo> eventList);
    void addJournals(QPtrList<Journal> eventList);
    virtual void printPreview(CalPrinter *calPrinter,
                              const QDate &, const QDate &);
    
    void readSettings(KConfig *config, QString setting = "KOListView Layout");
    void writeSettings(KConfig *config, QString setting = "KOListView Layout");
    void updateList();
    void clearList();
    void setStartDate(const QDate &start);
    int count();
    QString getWhatsThisText(QPoint p);
    QPtrList<Incidence> getSelectedIncidences( bool includeEvents = true, bool includeTodos = true , bool includeJournals = true, bool onlyDueTodos  = false );
    void showCompletedTodos();
 signals:
    void signalNewEvent();
    void beamIncidenceList(QPtrList<Incidence>);

  public slots:
    void hideAll();
    void printList();
    void resetFocus();
    virtual void updateView();
    virtual void showDates(const QDate &start, const QDate &end);
    virtual void showEvents(QPtrList<Event> eventList);
    void clearSelection();
    void allSelection();

    void clear();
    void showDates();
    void hideDates();
    void deleteAll();
    void saveToFile();
    void saveToFileVCS();
    void saveDescriptionToFile();
    void beamSelected();
    void updateConfig();
    void setCat();
    void setAlarm();
    void setCategories( bool removeOld );
    void changeEventDisplay(Event *, int);
  
    void defaultItemAction(QListViewItem *item);
    void popupMenu(QListViewItem *item,const QPoint &,int);
    void setCalendar( int c );
    void populateCalPopup();

  protected slots:
    void processSelectionChange(QListViewItem *);
    void catChanged( Incidence* );

  protected:
    void writeToFile( bool iCal );
    void addEvents(QPtrList<Event> eventList);
    void addIncidence(Incidence *);
    KOListViewItem *getItemForEvent(Incidence *event);

  private:
    bool mForceShowCompletedTodos;
    QPopupMenu* mCalPopup;
    KOListViewWhatsThis *mKOListViewWhatsThis;
    KOListViewListView *mListView;
    KOEventPopupMenu *mPopupMenu;
    KOListViewItem *mActiveItem;
    QDict<Incidence> mUidDict;
    QDate mStartDate;
    void keyPressEvent ( QKeyEvent * ) ;
};

#endif
