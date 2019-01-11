// CHANGED 2004-08-06 Lutz Rogowski
#include <qtextbrowser.h>
#include <qlistview.h>
#include <qaction.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qapplication.h>

#include <qtoolbar.h>
#include <qmenubar.h>
#include <kiconloader.h>
//#include <qpe/resource.h>
#include <klocale.h>

#include "viewmailbase.h"
//#include "opendiag.h"

ViewMailBase::ViewMailBase(QWidget *parent, const char *name, WFlags fl)
	: QMainWindow(parent, name, fl)
{

	setToolBarsMovable(false);

	toolbar = new QToolBar(this);
  menubar = new QMenuBar( toolbar );
  mailmenu = new QPopupMenu( menubar );
  menubar->insertItem( i18n( "Mail" ), mailmenu );

	toolbar->setHorizontalStretchable(true);
	addToolBar(toolbar);

	viewSource = new QAction(i18n("View Source"), 0, 0, this);
 	viewSource->addTo(mailmenu);
    
    downloadMail= new QAction(i18n("Download Mail"),SmallIcon("add"), 0, 0, this);
	downloadMail->addTo(toolbar);
 	downloadMail->addTo(mailmenu);


	reply = new QAction(i18n("Reply"),SmallIcon("reply"), 0, 0, this);
	reply->addTo(toolbar);
 	reply->addTo(mailmenu);

	forward = new QAction(i18n("Forward"),SmallIcon("forward"), 0, 0, this);
	forward->addTo(toolbar);
	forward->addTo(mailmenu);

	attachbutton = new QAction(i18n("Attachments"),SmallIcon("attach"), 0, 0, this, 0, true);
	attachbutton->addTo(toolbar);
	attachbutton->addTo(mailmenu);
	connect(attachbutton, SIGNAL(toggled(bool)), SLOT(slotChangeAttachview(bool)));

    showHtml = new QAction( i18n( "Show Html" ), SmallIcon(  "html" ), 0, 0, this, 0, true );
    showHtml->addTo( toolbar );
    showHtml->addTo( mailmenu );

    deleteMail = new QAction(i18n("Delete Mail"),SmallIcon("trash"), 0, 0, this);
	deleteMail->addTo(toolbar);
	deleteMail->addTo(mailmenu);

    nextMail = new QAction(i18n("Show next mail"),SmallIcon("enter"), 0, 0, this);
	QLabel *spacer = new QLabel(toolbar);
	nextMail->addTo(toolbar);
	nextMail->addTo(mailmenu);

  	closeMail = new QAction(i18n("Close"),SmallIcon("exit"), 0, 0, this);
	//QLabel *spacer = new QLabel(toolbar);
	spacer->setBackgroundMode(QWidget::PaletteButton);
	toolbar->setStretchableWidget(spacer);
	closeMail->addTo(toolbar);
	closeMail->addTo(mailmenu);
	QVBox *view = new QVBox(this);
	setCentralWidget(view);

	attachments = new QListView(view);
    int fixh = 100;
    if ( QApplication::desktop()->width() > 320 )
        fixh = 200;
	attachments->setFixedHeight(fixh);
	attachments->setAllColumnsShowFocus(true);
	attachments->addColumn("Mime Type", fixh-30);
	attachments->addColumn(i18n("Description"), 100);
	attachments->addColumn(i18n("Filename"), 80);
    attachments->addColumn(i18n("Size"), 80);
    attachments->setSorting(-1);
	attachments->hide();

	browser = new QTextBrowser(view);

//	openDiag = new OpenDiag(view);
//	openDiag->hide();

}

void ViewMailBase::slotChangeAttachview(bool state)
{
	if (state) attachments->show();
	else attachments->hide();
}

void ViewMailBase::keyPressEvent ( QKeyEvent * e )
{
    if( e->key()==Qt::Key_Escape ) {
        close();
        e->accept();
        return;
    }
    QWidget::keyPressEvent(e);
}
