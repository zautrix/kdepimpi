// CHANGED 2004-08-06 Lutz Rogowski
#ifndef VIEWMAILBASE_H
#define VIEWMAILBASE_H

#include <qmainwindow.h>

#ifndef DESKTOP_VERSION
#include <qpe/qpemenubar.h>
#define QMenuBar QPEMenuBar
#endif

class QAction;
class OpenDiag;
class QListView;
class QToolBar;
class QTextBrowser;
class QMenuBar;
class QPopupMenu;

class ViewMailBase : public QMainWindow
{
	Q_OBJECT

public:
	ViewMailBase(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected:
	QAction *reply, *forward, *attachbutton, *deleteMail, *showHtml, *closeMail, *nextMail, *downloadMail, *viewSource;
	QListView *attachments;
	QToolBar *toolbar;
	QTextBrowser *browser;
	OpenDiag *openDiag;
 	QMenuBar *menubar;
    QPopupMenu *mailmenu;

protected slots:
	void slotChangeAttachview(bool state);
    virtual void keyPressEvent ( QKeyEvent * e );


};

#endif

