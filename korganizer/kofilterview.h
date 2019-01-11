/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef KOFILTERVIEW_H
#define KOFILTERVIEW_H

#include <qstring.h>
#include <qcheckbox.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qscrollview.h>
#include <qpushbutton.h>
#include <kconfig.h>
#include "kofilterview_base.h"

#include <libkcal/calfilter.h>

#include <kurlrequester.h>
#include <klineedit.h>
#include <kglobal.h>
#include <kmessagebox.h>

class QGridLayout;

using namespace KCal;

class KONewCalPrefs : public QDialog
{
    Q_OBJECT 
  public:
    KONewCalPrefs( QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("Add new Calendar") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 3 );
        lay->setMargin( 3 );
        QLabel * lab = new QLabel( i18n("<b>Name of new calendar:</b>"), this );
        lay->addWidget( lab ); 
        nameE = new KLineEdit( this );
        lay->addWidget( nameE ); 
        lab = new QLabel( i18n("<b>iCal (*.ics) file on disk:</b><br>(will be created, if not existing)"), this );
        lay->addWidget( lab ); 
        url = new KURLRequester ( this );
        lay->addWidget( url ); 
        QPushButton * ok = new QPushButton( i18n("OK"), this );
        lay->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        lay->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( checkValid() )  );
        connect (cancel, SIGNAL( clicked() ), this, SLOT ( reject())  );
        int minwid = 440;
        if ( QApplication::desktop()->width() < 480 ) minwid = 220;
        setMinimumWidth(  minwid );
        resize(sizeHint() );
    }
      
    QString calName() { return nameE->text(); }
    QString calFileName() { return url->url(); }

public slots:
void checkValid() {
    if ( nameE->text().isEmpty() ) {
        KMessageBox::information( this,  i18n("Sorry, the calendar name is empty!") ); 
        nameE->setText( "LPQJ_"+ QString::number( QTime::currentTime().msec () ));
        return;
    } 
    if ( url->url().isEmpty() ) {
        KMessageBox::information( this,  i18n("Sorry, the file name is empty!") ); 
        url->setURL( nameE->text() + ".ics" );
        return;
    }
    accept();
}
           
public:
    KLineEdit* nameE;
    KURLRequester *url;
};

class KOCalButton : public QPushButton
{
    Q_OBJECT
  public:
    KOCalButton( QWidget *parent=0, const char *name=0 ) :
      QPushButton( parent, name) 
    {
      connect( this, SIGNAL( clicked() ),
               SLOT( bottonClicked() )); 
      mNumber = -1;
      setFocusPolicy(NoFocus); 
    }
    void setNum ( int num ) {mNumber = num; }
    signals:
    void selectNum ( int );
private: 
    int mNumber;
    void keyPressEvent ( QKeyEvent * e )
    {
        e->ignore();     
    }

private slots :
    void bottonClicked() { if ( mNumber > 0 ) emit selectNum ( mNumber ); }
};
class KOCalCheckButton : public QCheckBox
{
    Q_OBJECT
  public:
    KOCalCheckButton( QWidget *parent=0, const char *name=0 ) :
      QCheckBox( parent, name) 
    {
      connect( this, SIGNAL( toggled ( bool ) ),
               SLOT( bottonClicked( bool ) )); 
      mNumber = -1;
      setFocusPolicy(NoFocus); 
      //setMaximumWidth( 10 );

    }
    void setNum ( int num ) {mNumber = num; }
    signals:
    void selectNum ( int, bool );
private: 
    int mNumber;
    void keyPressEvent ( QKeyEvent * e )
    {
        e->ignore();     
    }

private slots :
    void bottonClicked( bool b) { if ( mNumber > 0 ) emit selectNum ( mNumber , b); }
};

class KOCalRadioButton : public QRadioButton
{
    Q_OBJECT
  public:
    KOCalRadioButton( QWidget *parent=0, const char *name=0 ) :
      QRadioButton( parent, name) 
    {
      connect( this, SIGNAL( toggled ( bool ) ),
               SLOT( bottonClicked( bool ) )); 
      mNumber = -1;
      setFocusPolicy(NoFocus); 
      //setMaximumWidth( 10 );

    }
        int num() { return mNumber;}
    void setNum ( int num ) {mNumber = num; }
    signals:
    void selectNum ( int );
private: 
    int mNumber;
    void keyPressEvent ( QKeyEvent * e )
    {
        e->ignore();     
    }

private slots :
    void bottonClicked( bool b) { if ( mNumber > 0  ) emit selectNum ( mNumber); }
};



class KOFilterView : public KOFilterView_base
{
    Q_OBJECT
  public:
    KOFilterView(QPtrList<CalFilter> *filterList,QWidget* parent=0,const char* name=0, WFlags fl=0);
    ~KOFilterView();

    void updateFilters();

    bool filtersEnabled();
    void setFiltersEnabled(bool);
    CalFilter *selectedFilter();
    void setSelectedFilter(QString);
    void setSelectedFilter( int );

  signals:
    void filterChanged();
    void editFilters();

  private:
    QPtrList<CalFilter> *mFilters;
};

class KOCalEditView : public QScrollView
{
    Q_OBJECT
  public:
    KOCalEditView( QWidget* parent=0,const char* name=0);
    ~KOCalEditView();
    int addCalendar( QString calName, QString fileName, bool ask = true );
    int getBirtdayID();
    public slots:
    void addCal();
    void enableAll();
    void enableAlarm();
    void disableRO();
    void deleteAll();
    void selectStdCal(int);
    void selectCal(int,bool );
    void selectCalAlarm(int,bool );
    void selectReadOnly(int,bool );
    void setColor(const QColor &,int) ;
    void deleteCal(int) ;
    void infoCal(int) ;
    void readConfig(); 
    void defaultInfo();
    void findNewStandard();
  signals:
    void alarmEnabled ( int cal, bool enable );
    void calendarEnabled ( int cal, bool enable );
    void calendarReadonly ( int cal, bool readonly );
    void setCalendarDefault ( int cal );
    void removeCalendar ( int cal );
    void calendarAdded( int );
    void needsUpdate();
    void checkCalendar();
    void requestCalendarInfo( int id );

  private:
    QWidget *mw;
    void toggleList ( QPtrList<KOCalCheckButton> , bool b = true );
    QPtrList<KOCalRadioButton> mStdandardB;
    QPtrList<KOCalCheckButton> mEnabledB;
    QPtrList<KOCalCheckButton> mAlarmB;
    QPtrList<KOCalCheckButton> mROB;
    QGridLayout* mainLayout; 
};



#endif // KOFILTERVIEW_H
