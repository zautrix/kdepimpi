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

//
// Journal Entry

#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktextedit.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include "koprefs.h"
#include <klineedit.h>
#include <kdialog.h>
#include "kolocationbox.h"

#include <libkcal/journal.h>
#include <libkcal/calendarresources.h>
#include <libkcal/resourcecalendar.h>
#include <kresources/resourceselectdialog.h>

#include "journalentry.h"
//#include "journalentry.moc"
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#endif
JournalEntry::JournalEntry(Calendar *calendar,QWidget *parent) :
  QFrame(parent)
{

    int fac = 5;
    heiHint = QApplication::desktop()->height();
    if ( heiHint > 800 )
        fac += 2;
    heiHint = heiHint / fac;

    showOnlyMode = false;
  mCalendar = calendar;
  mJournal = 0;
  visibleMode = true;
  QHBox * vb = new QHBox ( this );
  QPixmap iconp;
  vb->setMargin ( KDialog::marginHint()-1 );
  QPushButton * toggleJournal = new QPushButton( vb );
  iconp = SmallIcon("1updownarrow");
  toggleJournal->setPixmap (iconp ) ;
  QLabel* textLabel = new QLabel(" "+i18n("Title: "),vb);
  mTitle = new KOLocationBox(TRUE, vb, 30);
  mTitle->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred ,QSizePolicy::Fixed  ,FALSE) );
  mCalendarBox = new QComboBox(vb);
  mCalendarBox->setSizePolicy( QSizePolicy(  QSizePolicy::Preferred ,QSizePolicy::Fixed  ,FALSE) );
#ifndef DESKTOP_VERSION
  mTitle->setSizeLimit( 8 );
  mCalendarBox->setSizeLimit( 8 );
#endif
  vb->setStretchFactor ( mTitle, 8 );
  int limit = 3;
  if ( QApplication::desktop()->width() < 640 )
      limit = 6;
  vb->setStretchFactor ( mCalendarBox, limit ); 
  //mTitleLabel->setMargin(0);
  //mTitleLabel->setAlignment(AlignCenter);
  QPushButton * loadTemplate = new QPushButton( vb );
  QPushButton * saveTemplate = new QPushButton( vb );
  if ( QApplication::desktop()->width() < 321 ) 
      iconp = SmallIcon("fileexport16");
  else
      iconp = SmallIcon("fileexport");
  saveTemplate->setPixmap (iconp ) ;
  int size = saveTemplate->sizeHint().height();
  if ( QApplication::desktop()->width() < 321 ) 
      iconp = SmallIcon("fileimport16");
  else
      iconp = SmallIcon("fileimport");
  loadTemplate->setPixmap (iconp ) ;
  loadTemplate->setFixedSize(  size, size );
  saveTemplate->setFixedSize(  size, size );
  int widwid = size;
  if ( QApplication::desktop()->width() < 320 )
      widwid = size/2+1;
  toggleJournal->setFixedSize( widwid , size );
  mTitle->setFixedHeight( size+4);
  mCalendarBox->setFixedHeight( size+4);
  mEditor = new KTextEdit(this);
#ifndef DESKTOP_VERSION
  QPEApplication::setStylusOperation( mEditor, QPEApplication::RightOnHold );
#endif
  mMaxWidDiff = 3*size - 2*frameWidth() - textLabel->sizeHint().width();
  mDeskWid = QApplication::desktop()->width();
  int maxwid = mDeskWid - mMaxWidDiff;
  if ( QApplication::desktop()->width() < 640 ) {
      mTitle->setMaximumWidth( maxwid/2 +20 );
      mCalendarBox->setMaximumWidth( maxwid/2 -20);
  } else {
      mTitle->setMaximumWidth( (maxwid/4)*3);
      mCalendarBox->setMaximumWidth( maxwid/2 );
  }
  //mCalendarBox->setMaximumWidth( maxwid/2 -20 );
  mEditor->setWordWrap( KTextEdit::WidgetWidth );
  QBoxLayout *topLayout = new QVBoxLayout(this);
  topLayout->addWidget(vb);
  topLayout->addWidget(mEditor);
  mEditor->installEventFilter(this);
  installEventFilter(this);
  mTitle->installEventFilter(this);
  setFocusPolicy (QWidget::NoFocus); 

  connect( saveTemplate, SIGNAL( clicked() ), this , SLOT( slotSaveTemplate() ) );
  connect( loadTemplate, SIGNAL( clicked() ), this , SLOT( slotLoadTemplate() ) );
  connect( toggleJournal, SIGNAL( clicked() ), this , SLOT( toggleShowJournal() ) );
  mTitle->load( KOLocationBox::SUMMARYJOURNAL );
  mTitle->lineEdit ()->setText("");
}

JournalEntry::~JournalEntry()
{
    //qDebug("JournalEntry::~JournalEntry() ");
}
void JournalEntry::resizeEvent(QResizeEvent* e ) 
{
#ifndef DESKTOP_VERSION

    if ( mDeskWid != QApplication::desktop()->width() ) {
        mDeskWid == QApplication::desktop()->width();
        int maxwid = mDeskWid - mMaxWidDiff;
        if ( QApplication::desktop()->width() < 640 ) {
            mTitle->setMaximumWidth( maxwid/2 +20 );
            mCalendarBox->setMaximumWidth( maxwid/2 -20);
        }
        else {
            mTitle->setMaximumWidth( (maxwid/4)*3);
            mCalendarBox->setMaximumWidth( maxwid/2 );
        }
        //mCalendarBox->setMaximumWidth( maxwid/2 -20 );
    }
    //setMaximumWidth( QApplication::desktop()->width() );
    //qDebug("MAXXX %d ", QApplication::desktop()->width());
#endif
    QFrame::resizeEvent( e ); 
}
QSize JournalEntry::sizeHint() const
{
    return QSize ( 240, heiHint );
}
void JournalEntry::slotSaveTemplate()
{
    QString fileName =locateLocal( "templates", "journals" ); 
    QDir t_dir;
    if ( !t_dir.exists(fileName) )
        t_dir.mkdir ( fileName );
    fileName += "/journal";
    fileName = KFileDialog::getSaveFileName(  fileName , i18n("Save as Journal template"), this );
    if ( fileName.length() == 0 )
        return;

    QFile fileIn( fileName );
    if (!fileIn.open( IO_WriteOnly ) ) {
        KMessageBox::error( this, i18n("Error saving template file\n '%1'.")
                            .arg( fileName ) );
        return;
    } 
    // QString text;
    QTextStream tsIn( &fileIn );
    tsIn.setCodec( QTextCodec::codecForName("utf8") );
    tsIn << mEditor->text();
    fileIn.close();  
}
void JournalEntry::slotLoadTemplate()
{
    QString fileName =locateLocal( "templates", "journals" ); 
    QDir t_dir;
    if ( !t_dir.exists(fileName) )
        t_dir.mkdir ( fileName );
    fileName += "/journal";
    fileName = KFileDialog::getOpenFileName(  fileName , i18n("Insert Journal template"), this );
    if ( fileName.length() == 0 )
        return;
    QFile fileIn( fileName );
    if (!fileIn.open( IO_ReadOnly ) ) {
        KMessageBox::error( this, i18n("Error loading template file\n '%1'.")
                            .arg( fileName ) );
        return;
    }
    QTextStream tsIn( &fileIn );
    tsIn.setCodec( QTextCodec::codecForName("utf8") );
    QString text =  tsIn.read();
    fileIn.close();  
    int line, col;
    mEditor->getCursorPosition (& line, & col );
    mEditor-> insertAt ( text, line, col, true ); 
    //mEditor->setIgnoreMark( true );
}
void JournalEntry::setDate(const QDate &date)
{
    showOnlyMode = false;
  writeJournal();
  mDate = date;
  fillCalendar( mCalendar->defaultCalendar() );
}
void JournalEntry::fillCalendar( int setToID )
{
    mCalendarBox->clear();
    KopiCalendarFile * kkf = KOPrefs::instance()->mCalendars.first();
    int std = 0;
    int count = 0;
    while ( kkf ) {
        if ( (!kkf->mErrorOnLoad &&! kkf->isReadOnly) ||  setToID == kkf->mCalNumber ) {
            if ( setToID ) {
                if ( kkf->mCalNumber == setToID ) 
                    std = count;
            } else {
                if ( kkf->isStandard ) {
                    std = count;
                }
            }
            ++count;
            mCalendarBox->insertItem( kkf->mName );
        }
        kkf = KOPrefs::instance()->mCalendars.next();
    }
    mCalendarBox->setCurrentItem( std );
}

void JournalEntry::toggleShowJournal()
{  
    if ( mEditor->text().isEmpty() && mTitle->currentText ().isEmpty() )
        return;
    if (!mEditor->text().isEmpty() || !mTitle->currentText ().isEmpty())
        flushEntry();
    if ( showOnlyMode )
        emit showJournalOnly( 0 );
    else {
        // we have to protect mJournal from deleting if mJournal has empty text
        visibleMode = false; // set to true via :setShowOnly() 
        emit showJournalOnly( mJournal );
        //QTimer::singleShot( 0, this, SLOT( setVisibleOn() ) );
    }
}
void JournalEntry::setVisibleOn() 
{
    visibleMode = true;
}
void JournalEntry::setShowOnly() 
{
    showOnlyMode = true;
    if ( mTitle->currentText().isEmpty() ) 
        mTitle->setFocus();
    else
        mEditor->setFocus();
}
void JournalEntry::setJournal(Journal *journal, bool saveJournal )
{
    if ( saveJournal )
        writeJournal();
  mTitle->load( KOLocationBox::SUMMARYJOURNAL );

  mJournal = journal;
  if ( journal->isReadOnly()  )
      mTitle->lineEdit ()->setText(mJournal->summary()+" ("+i18n("readonly")+")");
  else
      mTitle->lineEdit ()->setText(mJournal->summary());
  mEditor->setText(mJournal->description());
  mTitle->setEnabled (!journal->isReadOnly()   );
  mEditor->setReadOnly ( journal->isReadOnly()  );
  mCalendarBox->setEnabled (!journal->isReadOnly()   );
  fillCalendar( mJournal->calID() );
}

Journal *JournalEntry::journal() const
{
  return mJournal;
}


void JournalEntry::clear()
{
  mJournal = 0;
  mEditor->setText("");
  mTitle->load( KOLocationBox::SUMMARYJOURNAL );
  mTitle->lineEdit ()->setText("");
}

bool JournalEntry::eventFilter( QObject *o, QEvent *e )
{
//  kdDebug() << "JournalEntry::event received " << e->type() << endl;
    if ( isVisible() ) {
        mEditor->setFocusPolicy (QWidget::StrongFocus); 
        mTitle->setFocusPolicy (QWidget::StrongFocus); 
    } else {
        mEditor->setFocusPolicy (QWidget::NoFocus); 
        mTitle->setFocusPolicy (QWidget::NoFocus); 
    }
  if ( e->type() == QEvent::FocusOut ) {
    writeJournal();
  }
  if ( e->type() == QEvent::KeyPress ) {
      QKeyEvent * k = (QKeyEvent *) e; 
      if ( !isVisible() )
          return true;
      if ( k->state() == Qt::ControlButton ) {
          k->ignore();
          //return true;
      }
  }

  return QFrame::eventFilter( o, e );    // standard event processing
}

void JournalEntry::writeJournal()
{
    if ( !visibleMode ) return;
    if ( !mTitle->isEnabled() ) return;
  if (mEditor->text().isEmpty() && mTitle->currentText().isEmpty()) {
      if ( mJournal ) {
          Journal* j = mJournal;
          mJournal = 0;
          bool conf = KOPrefs::instance()->mConfirm;
          KOPrefs::instance()->mConfirm = false;
          emit deleteJournal(j);
          KOPrefs::instance()->mConfirm = conf;
      }
      return;
  }

//  kdDebug() << "JournalEntry::writeJournal()..." << endl;
  
  if (!mJournal) {
    mJournal = new Journal;
    mJournal->setDtStart(QDateTime(mDate,QTime(0,0,0)));
    mCalendar->addJournal(mJournal);
  }
  if ( mJournal->description() != mEditor->text() ) {
      mJournal->setDescription(mEditor->text());
  }
  if ( mJournal->summary() != mTitle->currentText() ) {
      mJournal->setSummary(mTitle->currentText());
      mTitle->save(KOLocationBox::SUMMARYJOURNAL);
  }
  int id = KOPrefs::instance()->getCalendarID( mCalendarBox->currentText() );
  if ( mJournal->calID() != id ) {
      mJournal->setCalID( id );
  }
}

void JournalEntry::flushEntry()
{
   writeJournal();
}
void JournalEntry::keyPressEvent ( QKeyEvent * e ) 
{
    e->ignore();

}
