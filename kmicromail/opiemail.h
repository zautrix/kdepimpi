// CHANGED 2004-09-31 Lutz Rogowski
#ifndef OPIEMAIL_H
#define OPIEMAIL_H

#include "mainwindow.h"
#include <libmailwrapper/settings.h>

#include <opie2/osmartpointer.h>
#include <libmailwrapper/mailtypes.h>
#include <viewmail.h>
#include <qstringlist.h>

class ComposeMail;

class OpieMail : public MainWindow
{
    Q_OBJECT

public:
    OpieMail( QWidget *parent = 0, const char *name = 0 );
    virtual ~OpieMail();
    static QString appName() { return QString::fromLatin1("kopiemail"); }

public slots:
    virtual void slotwriteMail(const QString&name,const QString&email);
    virtual void slotwriteMail2(const QString&nameemail);
    virtual void slotComposeMail();
    virtual void slotExtAppHandler();
    virtual void appMessage(const QCString &msg, const QByteArray &data);
    virtual void message(const QCString &msg, const QByteArray &data);
    void insertAttendees(const QString&, const QStringList& namelist, const QStringList& emaillist, const QStringList& uidlist);
protected slots:
    virtual void deleteAndDisplayNextMail(ViewMail * vm);
    virtual void displayNextMail(ViewMail * vm);
    virtual void slotSendQueued();
    virtual void slotSearchMails();
    virtual void slotEditSettings();
    virtual void slotEditGlobalSettings();
    virtual void slotEditAccounts();
    virtual void displayMail();
    virtual void replyMail();
    virtual void slotDeleteMail();
    virtual void slotGetMail();
    virtual void slotGetAllMail();
    virtual void slotDeleteAllMail();
    virtual void mailHold(int, QListViewItem *,const QPoint&,int);
    virtual void slotShowFolders( bool show );
    virtual void refreshMailView(const QValueList<RecMailP>&);
    virtual void mailLeftClicked(  QListViewItem * );
    virtual void slotMoveCopyMail();
    virtual void slotMoveCopyAllMail();
    virtual void reEditMail();
    void clearSelection();
    void selectAll();
    void slotDownloadMail();
private:
    ComposeMail* mCurrentComposer; 
    void closeViewMail(ViewMail * vm);
    QString mPendingEmail;
    QString mPendingName;
    QByteArray mPendingData;
    QCString mPendingMessage;
    Settings *settings;
    QTextBrowser * tb;

};

#endif
