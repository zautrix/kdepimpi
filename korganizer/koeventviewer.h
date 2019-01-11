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
*/
#ifndef KOEVENTVIEWER_H
#define KOEVENTVIEWER_H
//
// Viewer widget for events.
//

#include <qtextbrowser.h>

#include <libkcal/event.h>
#include <libkcal/journal.h>


using namespace KCal;

#ifdef DESKTOP_VERSION

#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <kglobal.h>

class KOPrintPrefs : public QDialog
{
  public:
    KOPrintPrefs( QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("KO/Pi Printout") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 9 );
        lay->setMargin( 9 );
        QLabel * lab = new QLabel( i18n("This prints the view as you see it.\n(With the complete content, of course.)\nYou may change the print layout by resizing the view.\nPrint unscaled may print several pages\ndepending on the amount of data.\nPrint scaled down will print all on one page.\nPrint scaled up/down will print all on one page,\nbut will scale up the text to page boundaries,\nif the text is smaller than the page.\nYou can select page geometry setup in the next dialog.\n"), this );
        lay->addWidget( lab ); 
        lab->setAlignment( AlignCenter );
        QButtonGroup* format = new  QButtonGroup( 1, Horizontal, i18n("Printout Mode"), this );
        lay->addWidget( format ); 
        format->setExclusive ( true ) ;
        pmNo = new QRadioButton(i18n("Print unscaled"), format );
        pmScaledDown = new QRadioButton(i18n("Print scaled down to fit one page"), format );
        new QRadioButton(i18n("Print scaled up/down to fit one page"), format );
        pmScaledDown->setChecked( true );
        QPushButton * ok = new QPushButton( i18n("OK"), this );
        lay->addWidget( ok );
        QPushButton * cancel = new QPushButton( i18n("Cancel"), this );
        lay->addWidget( cancel );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
        connect (cancel, SIGNAL(clicked() ), this, SLOT ( reject())  );
    }
      
    int printMode() 
    {
        if ( pmNo->isChecked() )
            return 0;
        if ( pmScaledDown->isChecked() )
            return 1;
        return 2;
    }
private:
    QRadioButton* pmNo;
    QRadioButton* pmScaledDown;
};

#endif

class KOEventViewer : public QTextBrowser {
    Q_OBJECT
  public:
    KOEventViewer(QWidget *parent=0,const char *name=0);
    virtual ~KOEventViewer();

    void setSource(const QString &);
    void setEvent(Event *event);
    void addEvent(Event *event);
    void setTodo(Todo *event, bool clearV = true );
    void setJournal(Journal *jour, bool clearV = true );

    void appendEvent(Event *event, int mode = 0 );
    void appendTodo(Todo *event, int mode = 0 );
    void appendJournal(Journal *jour, int mode = 0 );
    
    void clearEvents(bool now=false);
    
    void addText(QString text);
    void setSyncMode( bool );
    void setColorMode( int );
    void mailToAttendees( bool all );
    void printMe();

  protected:
    int mColorMode;
    void addTag(const QString & tag,const QString & text);

    void formatCategories(Incidence *event);
    void formatAttendees(Incidence *event);
    void formatReadOnly(Incidence *event);
    void keyPressEvent ( QKeyEvent * e );

  private:
    QTextBrowser *mEventTextView;
    bool mSyncMode;
    QString deTag(QString text);

    QString mText;
    QString mMailSubject;
    Incidence* mCurrentIncidence;  
  signals:
    void launchaddressbook(QString uid); 
    void showIncidence(QString uid);
};

#endif
