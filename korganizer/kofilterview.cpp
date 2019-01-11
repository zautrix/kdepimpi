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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qwhatsthis.h>
#include <qdir.h>


#include <libkcal/calfilter.h>

#include "kofilterview.h"
#include "koprefs.h"
#include <kiconloader.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kcolorbutton.h>
#include <kmessagebox.h>



 

KOFilterView::KOFilterView(QPtrList<CalFilter> *filterList,QWidget* parent,
                           const char* name,WFlags fl )
  : KOFilterView_base(parent,name,fl)
{
  mFilters = filterList;

  connect(mSelectionCombo,SIGNAL(activated(int)),SIGNAL(filterChanged()));
  connect(mEnabledCheck,SIGNAL(clicked()),SIGNAL(filterChanged()));
  connect(mEditButton,SIGNAL(clicked()),SIGNAL(editFilters()));
}

KOFilterView::~KOFilterView()
{
    // no need to delete child widgets, Qt does it all for us
}

bool KOFilterView::filtersEnabled()
{
  return mEnabledCheck->isChecked();
}

void KOFilterView::setFiltersEnabled(bool set)
{
  mEnabledCheck->setChecked(set);
  emit filterChanged();
}


void KOFilterView::updateFilters()
{
  mSelectionCombo->clear();

  CalFilter *filter = mFilters->first();
  while(filter) {
    mSelectionCombo->insertItem(filter->name());
    filter = mFilters->next();
  }
}

CalFilter *KOFilterView::selectedFilter()
{
  CalFilter *f = mFilters->at(mSelectionCombo->currentItem());
  return f;
}

void KOFilterView::setSelectedFilter(QString filterName)
{
  int filter_num = mSelectionCombo->count();
  int i;
  for (i=0;i<filter_num;i++) {
    if (mSelectionCombo->text(i)==filterName)
      mSelectionCombo->setCurrentItem(i);
  }
  emit filterChanged();
}
void KOFilterView::setSelectedFilter( int fil )
{ 
  if ( fil >= mSelectionCombo->count() )
      return;
  mSelectionCombo->setCurrentItem( fil );
  emit filterChanged();
}



KOCalEditView::KOCalEditView(QWidget* parent,
                           const char* name )
  : QScrollView(parent,name)
{
    mw = 0; 
    setResizePolicy( AutoOneFit );
    setFrameStyle ( QFrame::Panel | QFrame::Plain );
    setLineWidth ( 1 ); 
    setMidLineWidth ( 1 ); 
    setFocusPolicy(NoFocus); 
}

KOCalEditView::~KOCalEditView()
{
    // no need to delete child widgets, Qt does it all for us
}
void KOCalEditView::selectCal(int id ,bool b)
{
    KOPrefs::instance()->getCalendar( id )->isEnabled = b;
    emit calendarEnabled ( id, b );
    emit needsUpdate();

}
void KOCalEditView::selectStdCal( int id  )
{
    KOCalRadioButton* it = mStdandardB.first();
    while ( it ) {
        it->blockSignals( true );
        it->setChecked( it->num() == id );
        it->blockSignals( false );
        it = mStdandardB.next();
    }
   KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    while ( kkf ) {
        kkf->isStandard = (kkf->mCalNumber == id );
        kkf = KOPrefs::instance()->mCalendars.next();
    }
    emit setCalendarDefault ( id );
}

void KOCalEditView::selectCalAlarm(int id ,bool b )
{
    KOPrefs::instance()->getCalendar( id )->isAlarmEnabled = b;
    emit alarmEnabled ( id , b );
    emit needsUpdate();
}
void KOCalEditView::selectReadOnly(int id ,bool b )
{

    emit calendarReadonly ( id , b );
    KOPrefs::instance()->getCalendar( id )->isReadOnly = b;
    if ( KOPrefs::instance()->getCalendar( id )->isStandard && b ) {
        findNewStandard(); 
    }
    if ( !b ){
        KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
        while ( kkf ) {
            if (kkf->isReadOnly && kkf->isStandard ) {
                selectStdCal( id );
                break;
            }
            kkf = KOPrefs::instance()->mCalendars.next();
        }
    }
    
    mStdandardB.at(id-1)->setEnabled( !b );
    emit needsUpdate();
    
}
void KOCalEditView::findNewStandard()
{ 
    bool found = false;
    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    while ( kkf ) {
        if (!kkf->isReadOnly && !kkf->mErrorOnLoad ) {
            found = true;
            selectStdCal( kkf->mCalNumber );
            break;
        }
        kkf = KOPrefs::instance()->mCalendars.next();
    }
    if ( !found ) {
        KMessageBox::error( this,i18n("\nNO\n WRITEABLE\n   CALENDAR\n      FOUND!\n\nPlease fix your calendar settings!\n"),
                    i18n("Houston, we have a problem!") );

    }
}

void KOCalEditView::setColor( const QColor& c, int id )
{
    KOPrefs::instance()->getCalendar( id )->mDefaultColor = c;
    emit needsUpdate();
}
void KOCalEditView::deleteCal( int id )
{
    KopiCalendarFile * kkf = KOPrefs::instance()->getCalendar( id );
    QString name = kkf->mName;
    QString file = KGlobal::formatMessage ( kkf->mFileName ,0 );
    if ( KMessageBox::warningContinueCancel( this,  i18n("The calendar <b>%1</b> is displaying file <b>%2</b>. Do you want to remove this calendar from KO/Pi? (The file is not removed!)").arg(name).arg(file) ) != KMessageBox::Continue ) return;
    bool findnewstd =  kkf->isStandard;
    emit removeCalendar ( id );
    KOPrefs::instance()->mCalendars.remove ( kkf );
    if ( findnewstd  ) findNewStandard();
    emit needsUpdate();
    QTimer::singleShot( 0, this, SLOT ( readConfig() ) ); 
}
void KOCalEditView::infoCal( int id )
{
    QString name = KOPrefs::instance()->getCalendar( id )->mName;
    QString file = KGlobal::formatMessage ( KOPrefs::instance()->getCalendar( id )->mFileName, 0 );
    if ( KOPrefs::instance()->getCalendar( id )->mErrorOnLoad ) {
        if (  KMessageBox::Yes == KMessageBox::questionYesNo( this,  i18n("The calendar <b>%1</b> is not loaded! Loading of file <b>%2</b> failed! <b>Try again to load the calendar?</b>").arg(name).arg(file) ) ) {
            emit calendarAdded( id );
            emit needsUpdate();
            QTimer::singleShot( 0, this, SLOT ( readConfig() ) ); 
            QTimer::singleShot( 100, this, SIGNAL ( checkCalendar() ) ); 
        }
    }
    else {
        emit requestCalendarInfo( id );
     
    }
}
void KOCalEditView::readConfig() 
{
    
    mStdandardB.clear();
    mEnabledB.clear();
    mAlarmB.clear();
    mROB.clear();

    if ( mw ) delete mw;
    mw = new QWidget ( viewport() );
    addChild(mw);
    int ii = 0;
    mainLayout = new QGridLayout ( mw , 2, 8 );
    mainLayout->setMargin( 2 );
    mainLayout->setSpacing( 2 );
    QPushButton * addButT = new QPushButton ( mw );
    addButT->setFocusPolicy(NoFocus); 
    mainLayout->addWidget( addButT,0,0 );
    addButT->setText( "D");
    connect(addButT,SIGNAL(clicked()),SLOT(defaultInfo()));
    QWhatsThis::add( addButT, i18n("Please choose the <b>default calendar</b> in this column. Newly created or imported items are added to the default calendar.") );
    //addBut->setPixmap ( SmallIcon("greenhook16"));
    QPushButton *addBut = new QPushButton ( mw );
    addBut->setFocusPolicy(NoFocus); 
    mainLayout->addWidget( addBut,0,++ii );
    addBut->setPixmap ( SmallIcon("eye"));
    QWhatsThis::add( addBut, i18n("In this column you can <b>set a calendar to be visible</b>. If a calendar is not visible its entries are not displayed in the views. You can add items to it and it is loaded/saved as usual.") );
    connect(addBut,SIGNAL(clicked()),SLOT(enableAll()));
    int max = addBut->sizeHint().height();
    addBut->setMaximumWidth( max );
    addButT->setFixedSize( QSize( max, max ) );
    QLabel* lab = new QLabel (i18n(" Calendar \n Resource "), mw );
    mainLayout->addWidget( lab,0,++ii ); 
    QWhatsThis::add( lab, i18n("In this column you can see the <b>name of the calendar</b>. If you click on the name button you will get an information box about the loaded calendar file. If the file was not loaded at startup you can try to load it here again.") );
    //lab = new QLabel ( i18n(" "), mw );
    //mainLayout->addWidget( lab,0,++ii );
    //lab->setFixedWidth( 1 );
    addBut = new QPushButton ( mw );
    addBut->setFocusPolicy(NoFocus); 
    mainLayout->addWidget( addBut,0,++ii );
    addBut->setPixmap ( SmallIcon("bell"));
    QWhatsThis::add( addBut, i18n("In this column you can <b>disable the alarms of a calendar all together</b>. The alarm data in the calendar itself is not changed, the alarms are marked internally as \"do not use\". Useful if you load a calendar of another person but do not want to get notified about alarms of that person.") );
    connect(addBut,SIGNAL(clicked()),SLOT(enableAlarm()));
    addBut->setMaximumWidth( addBut->sizeHint().height() );

    addBut = new QPushButton ( mw );
    addBut->setFocusPolicy(NoFocus); 
    mainLayout->addWidget( addBut,0,++ii );
    addBut->setPixmap ( SmallIcon("pencil"));
    QWhatsThis::add( addBut, i18n("In this column you can <b>set a calendar and all entries of the calendar to read only</b>. If a calendar is readonly the entries cannot be edited and no items can be added to the calendar. If you change a setting of a calendar to readonly in this column all data will be saved because the data of a readonly calendar is not saved later.") );
    connect(addBut,SIGNAL(clicked()),SLOT(disableRO()));
    addBut->setMaximumWidth( addBut->sizeHint().height() );
    lab = new QLabel ( "", mw );
    mainLayout->addWidget( lab,0,++ii );

    addBut = new QPushButton ( mw );
    addBut->setFocusPolicy(NoFocus); 
    mainLayout->addWidget( addBut,0,++ii );
    addBut->setPixmap ( SmallIcon("plus"));
    connect(addBut,SIGNAL(clicked()),SLOT(addCal()));
    QWhatsThis::add( addBut, i18n("Click this button to <b>add a calendar</b>. You can add an existing calendar file or you can add a new calendar and KO/Pi creates a new empty calendar file for you.") );
    lab = new QLabel ( " ", mw );
    mainLayout->addWidget( lab,0,++ii );


    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    int row = 1;
    bool errorLoadStandard = false;
    while ( kkf ) {
        int iii = 0;
        KOCalRadioButton* rb = new KOCalRadioButton( mw );
        mainLayout->addWidget( rb,row,0 );mStdandardB.append( rb );
        rb->setChecked( kkf->isStandard );
        if ( kkf->isStandard && ( kkf->mErrorOnLoad || kkf->isReadOnly ) )
            errorLoadStandard = true;
        rb->setNum( kkf->mCalNumber );
        connect (rb, SIGNAL (selectNum(int)), SLOT ( selectStdCal(int) ) );
        if ( kkf->mErrorOnLoad ||  kkf->isReadOnly )
            rb->setEnabled( false );
        KOCalCheckButton* cb = new KOCalCheckButton( mw );
        mainLayout->addWidget( cb,row,++iii );mEnabledB.append( cb );
        cb->setChecked( kkf->isEnabled && !kkf->mErrorOnLoad );
        cb->setNum( kkf->mCalNumber );
        if ( kkf->mErrorOnLoad )
            cb->setEnabled( false );
        connect (cb, SIGNAL (selectNum(int,bool)), SLOT ( selectCal(int,bool) ) );
        KOCalButton* name = new KOCalButton( mw );
        name->setNum( kkf->mCalNumber );
        name->setText(  kkf->mName );
        mainLayout->addWidget( name,row,++iii );
        connect (name, SIGNAL (selectNum(int)), SLOT ( infoCal(int) ) );
        //lab = new QLabel (" ", mw );
        //mainLayout->addWidget( lab,row,++iii );
        cb = new KOCalCheckButton( mw );
        mainLayout->addWidget( cb,row,++iii );mAlarmB.append( cb );
        cb->setChecked( kkf->isAlarmEnabled && !kkf->mErrorOnLoad);
        cb->setNum( kkf->mCalNumber );
        connect (cb, SIGNAL (selectNum(int,bool)), SLOT ( selectCalAlarm(int,bool) ) );
        if ( kkf->mErrorOnLoad )
            cb->setEnabled( false );
        cb = new KOCalCheckButton( mw );
        mainLayout->addWidget( cb,row,++iii );mROB.append( cb );
        cb->setChecked( kkf->isReadOnly );
        cb->setNum( kkf->mCalNumber );
        connect (cb, SIGNAL (selectNum(int,bool)), SLOT ( selectReadOnly(int,bool) ) );
        if ( kkf->mErrorOnLoad )
            cb->setEnabled( false );
        if ( row > 1) {
            KColorButton *colb = new KColorButton( mw );
            mainLayout->addWidget( colb,row,++iii );
            colb->setID( kkf->mCalNumber );
            colb->setColor( kkf->mDefaultColor );
            connect (colb, SIGNAL (changedID(const QColor&, int )), SLOT ( setColor(const QColor&,int) ) );
            KOCalButton* calb = new KOCalButton( mw );
            mainLayout->addWidget( calb,row,++iii );
            calb->setNum( kkf->mCalNumber );
            calb->setPixmap ( SmallIcon("minus"));
            connect (calb, SIGNAL (selectNum(int)), SLOT ( deleteCal(int) ) );
            int hei = calb->sizeHint().height();
            //calb->setMaximumSize( hei*9/10, hei*9/10 );
        }
        ++row;
        kkf = KOPrefs::instance()->mCalendars.next();
    }
    if ( errorLoadStandard ) 
        findNewStandard();
    lab = new QLabel (  "", mw );
    mainLayout->addWidget( lab,row,0 );
    mw->show();

}

void KOCalEditView::defaultInfo()
{
    KMessageBox::information( this,  i18n("Please choose the <b>default calendar</b> in this column. Newly created or imported items are added to the default calendar.") ); 
}
void KOCalEditView::addCal()
{
    bool tryagain = true;
    QString name, file = KGlobalSettings::calendarDir()+"newCal.ics";
    while ( tryagain ) {
        KONewCalPrefs prefs ( this ); 
        prefs.nameE->setText( name );
        prefs.url->setURL( file );
        if ( !  prefs.exec() )
            return;
        name = prefs.calName();
        file = prefs.calFileName();
        tryagain = false;
        KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
        while ( kkf ) {
            if ( kkf->mName == name ) {
                KMessageBox::information( this,  i18n("Sorry, the calendar name \n%1\nalready exists!\nPlease choose another name!").arg( name ) ); 
                name = "";
                tryagain = true;  
                break;
            }
            if ( kkf->mFileName == file ) {
                KMessageBox::information( this,  i18n("Sorry, the file \n%1\nis already loaded!\nPlease choose another file!").arg(  KGlobal::formatMessage (file, 0 )) ); 
                tryagain = true;  
                file = KGlobalSettings::calendarDir()+"newCal.ics";
                break;
            }
            kkf = KOPrefs::instance()->mCalendars.next();
        } 
        QFileInfo fi ( file );
        if ( fi.isDir() ) {
            tryagain = true;  
        }
    }
    addCalendar ( name, file );
    QTimer::singleShot( 100, this, SIGNAL ( checkCalendar() ) );
}
int KOCalEditView::addCalendar( QString name, QString file, bool ask )
{
    file = QDir::convertSeparators( file ); 
    QFileInfo fi ( file );
    QString absFile = file;
    bool isRelative = false;
    if ( fi.isRelative() ) {
        isRelative = true; 
        absFile = QDir::convertSeparators( KGlobalSettings::calendarDir()+file );
        fi.setFile( absFile );
    } else {
        QString cd = QDir::convertSeparators( KGlobalSettings::calendarDir() );
        if (  file.left( cd.length() ) ==  cd ) {
            isRelative = true; 
            file = fi.fileName ();
            fi.setFile( absFile );
        }
    }
    if (!fi.exists() ) {
        if ( ask )
            if ( KMessageBox::questionYesNo(this, i18n("The file\n%1\ndoes not exist!\nShall I create it for you?").arg( KGlobal::formatMessage (absFile,0) ) )== KMessageBox::No )
                return 0;
        QFile fileIn( absFile );
        if (!fileIn.open( IO_WriteOnly ) ) {
            KMessageBox::sorry( this,  i18n("Sorry, cannot create the file\n%1!\nNo calendar added!").arg( file ) ); 
            return 0;
        } 
        QTextStream tsIn( &fileIn );
        tsIn.setCodec( QTextCodec::codecForName("utf8") );
        tsIn << "BEGIN:VCALENDAR\nPRODID:-//KDE-Pim//Platform-independent 2.1.0\nVERSION:2.0\nEND:VCALENDAR\n";
        fileIn.close();       
    }
    KopiCalendarFile * kkf = KOPrefs::instance()->getNewCalendar();
    kkf->mName = name;
    kkf->mFileName =  absFile;
    kkf->mSavedFileName = file;
    kkf->isRelative = isRelative;
    emit calendarAdded( kkf->mCalNumber );
    if ( ask )
        emit needsUpdate();
    QTimer::singleShot( 0, this, SLOT ( readConfig() ) );   
    return kkf->mCalNumber;
}
int KOCalEditView::getBirtdayID()
{
    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    while ( kkf ) {
        if ( kkf->mName == i18n("Birthdays") )
            return kkf->mCalNumber;
        kkf = KOPrefs::instance()->mCalendars.next();
    }
    QString file = locateLocal( "data", "korganizer/birthdays.ics" ); 
    return addCalendar( i18n("Birthdays"), file, false );
}

void KOCalEditView::enableAll()
{
    toggleList( mEnabledB );
}
void KOCalEditView::enableAlarm()
{
    toggleList( mAlarmB );
}
void KOCalEditView::disableRO()
{
    toggleList( mROB, false );
}
void KOCalEditView::toggleList ( QPtrList<KOCalCheckButton> list , bool enable )
{
    bool dis = !enable;
    KOCalCheckButton* it = list.first();
    while ( it ) {
        if ( !it->isChecked() == enable && it->isEnabled() ) {
            dis = !dis;
            break;
        }
        it = list.next();
    }
    it = list.first();
    while ( it ) {
        if ( it->isEnabled() )
            it->setChecked(dis);
        it = list.next();
    }  
}
void KOCalEditView::deleteAll()
{
    qDebug("delteAll");
}
