// CHANGED 2004-09-31 Lutz Rogowski
// CHANGED 2004-08-06 Lutz Rogowski


#define protected public
#include <qwidget.h>
#undef protected
#include "koprefsdialog.h"
#include <kapplication.h>
#include <libkdepim/externalapphandler.h>
#include <libkdepim/kpimglobalprefs.h>
#ifdef  MINIKDE_KDIALOG_H
#undef MINIKDE_KDIALOG_H
#endif
#include "settingsdialog.h"
#include "opiemail.h"
#include "editaccounts.h"
#include "composemail.h"
#include "mailistviewitem.h"
#include "viewmail.h"
#include "selectstore.h"
#include "selectsmtp.h"
#include "accountitem.h"
#include "accountview.h"
#include "klocale.h"

#include <qmessagebox.h>
#include <qtimer.h>
#include <qcursor.h>
#include <qtextbrowser.h>
#include <qregexp.h>
#include <qpe/global.h>

#ifdef DESKTOP_VERSION
#include <qapplication.h>
#else
#include <qpe/qpeapplication.h>
#endif
#include <libmailwrapper/smtpwrapper.h>
#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/abstractmail.h>
#include "koprefs.h"

//using namespace Opie::Core;

OpieMail::OpieMail( QWidget *parent, const char *name )
    : MainWindow( parent, name) //, WStyle_ContextHelp )
{
    mCurrentComposer = 0;
    settings = new Settings();
    tb = 0;
    setIcon(SmallIcon( "kmicromail" )  );
    folderView->populate( settings->getAccounts() );
   connect(ExternalAppHandler::instance(), SIGNAL(receivedNameEmailUidListEvent(const QString&, const QStringList&, const QStringList&, const QStringList&)),
       this, SLOT(insertAttendees(const QString&, const QStringList&, const QStringList&, const QStringList&)));
   folderView->setFocus(); 
}

OpieMail::~OpieMail()
{
    if (settings) delete settings;
    if ( tb )
        delete tb;
}

void OpieMail::appMessage(const QCString &msg, const QByteArray &data)
{
    
}
#include <stdlib.h>
void OpieMail::message(const QCString &msg, const QByteArray &data)
{
    // copied from old mail2
    static int ii = 0;
    //qDebug("QCOP CALL ############################# %d ", ii);
    //QString mess ( msg  );
    //qDebug("Message = %s ",mess.latin1());
    ++ii;
    //qDebug("KM:appMessage %d *%s* %x", ii, msg.data(), this);

    mPendingEmail = QString::null;
    mPendingName = QString::null;
    if (msg == "writeMail(QString,QString)")
    {
        //qDebug("writeMail(QString,QString) ");
        QDataStream stream(data,IO_ReadOnly);
        stream >> mPendingName >> mPendingEmail;
        // removing the whitespaces at beginning and end is needed!
        QTimer::singleShot ( 50, this, SLOT(slotComposeMail() ) ); 
    }
    else if (msg == "newMail()")
    {
        //qDebug("slotComposeMail() ");
        // we cannot call slotComposeMail(); directly, because may be executing a QCOP call
        // and a QCOP call does not like a processevents in his execution
        // with the Qtimer we call slotComposeMail() after we reached the main event loop
        QTimer::singleShot ( 50, this, SLOT(slotComposeMail() ) ); 
        // slotComposeMail();
    }
    else if (msg == "newMail(QString)")
    {
        //qDebug(" newMail(QString)");
        QDataStream stream(data,IO_ReadOnly);
        stream >> mPendingName;
        // the format is 
        //  NAME <EMAIL>:SUBJECT
        QTimer::singleShot ( 50, this, SLOT(slotComposeMail() ) ); 
    } else {
        mPendingData = data;
        mPendingMessage = msg;
        QTimer::singleShot ( 50, this, SLOT(slotExtAppHandler() ) ); 
    }

    //qDebug("END OpieMail::message ");
}
void OpieMail::slotExtAppHandler()
{
    ExternalAppHandler::instance()->appMessage ( mPendingMessage, mPendingData );
}
void OpieMail::slotwriteMail2(const QString& namemail )
{
    //qDebug("OpieMail::slotwriteMail2 ");
    //qApp->processEvents();
     ComposeMail compose( settings, this, 0, true );
    if ( !namemail.isEmpty() ) {
        QString to = namemail;
        if ( namemail.find( " <") > 1 ) {
            to = "\"" +to.replace( QRegExp( " <"), "\" <") ;
        } else 
        if ( namemail.find( "<") > 1 ) {
            to = "\"" +to.replace( QRegExp( "<"), "\" <") ;
        }
        int sub = to.find( ">:");
        if ( sub > 0 ) {
            compose.setTo( to.left(sub+1)  );
            compose.setSubject( to.mid(sub+2) );
        } else
            compose.setTo( to );
     }
    compose.slotAdjustColumns();
#ifndef DESKTOP_VERSION
    compose.showMaximized();
#endif
    mCurrentComposer = &compose;
    compose.exec();
    mCurrentComposer = 0;
    folderView->refreshOutgoing();
    raise();
    //qDebug("retttich ");
}
void OpieMail::slotwriteMail(const QString&name,const QString&email)
{
    // qDebug("OpieMail::slotwriteMail ");
    ComposeMail compose( settings, this, 0, true );
    if (!email.isEmpty())
    {
        if (!name.isEmpty())
        {
            compose.setTo("\"" + name + "\"" + " " + "<"+ email + ">");
        }
        else
        {
            compose.setTo(email);
        }
    }
    compose.slotAdjustColumns();
#ifndef DESKTOP_VERSION
    compose.showMaximized();
#endif

    mCurrentComposer = &compose;
    compose.exec();
    mCurrentComposer = 0;
    folderView->refreshOutgoing();
    raise();
}

void OpieMail::slotComposeMail()
{
    if ( mPendingEmail == QString::null && mPendingName == QString::null)
        slotwriteMail2( QString () );
    else {
        if ( mPendingEmail == QString::null )
            slotwriteMail2( mPendingName );
        else
            slotwriteMail( mPendingName, mPendingEmail );   
    }
    //slotwriteMail(0l,0l);
}

void OpieMail::slotSendQueued()
{
    SMTPaccount *smtp = 0;

    QList<Account> list = settings->getAccounts();
    QList<SMTPaccount> smtpList;
    smtpList.setAutoDelete(false);
    Account *it;
    for ( it = list.first(); it; it = list.next() )
    {
        if ( it->getType() == MAILLIB::A_SMTP )
        {
            smtp = static_cast<SMTPaccount *>(it);
            smtpList.append(smtp);
        }
    }
    if (smtpList.count()==0)
    {
        QMessageBox::information(0,i18n("Info"),i18n("Define a smtp\n account first!\n"));
        return;
    }
    if ( QMessageBox::warning(this, i18n("Sending all mails"), i18n("Do you really want to\nsend all queued mails?" )  , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
        return;
    if (smtpList.count()==1)
    {
        smtp = smtpList.at(0);
    }
    else
    {
        smtp = 0;
        selectsmtp selsmtp;
        selsmtp.setSelectionlist(&smtpList);
        selsmtp.resize( selsmtp.sizeHint() );
        if (  selsmtp.exec() == QDialog::Accepted )
        {
            smtp = selsmtp.selected_smtp();
        }
    }
    if (smtp)
    {
     
        Global::statusMessage("Sending mails...!");
        SMTPwrapper * wrap = new SMTPwrapper(smtp);
        if ( wrap->flushOutbox() )
        {
            Global::statusMessage("Mails sent!");
        } 
        delete wrap;
    }
    folderView->refreshOutgoing();
}

void OpieMail::slotSearchMails()
{
    qDebug("OpieMail::slotSearchMails():not implemented ");
}
void OpieMail::slotEditGlobalSettings()
{
    KPimPrefsGlobalDialog gc ( this );
    gc.exec();
}

void OpieMail::slotEditSettings()
{

    KOPrefsDialog settingsDialog( this,  "koprefs", true );
#ifndef DESKTOP_VERSION
    settingsDialog.showMaximized();
#endif
    settingsDialog.exec();

   slotSetCodec( KOPrefs::instance()->mCurrentCodec );
    //    KApplication::execDialog(settingsDialog);
}

void OpieMail::slotEditAccounts()
{
    EditAccounts eaDialog( settings, this, 0, true );
    eaDialog.slotAdjustColumns();
#ifndef DESKTOP_VERSION
    eaDialog.showMaximized();
#endif
    eaDialog.exec();
    if ( settings ) delete settings;
    settings = new Settings();

    folderView->populate( settings->getAccounts() );
}
void OpieMail::replyMail()
{

    QListViewItem*item = mailView->currentItem();
    if (!item) return;
    RecMailP mail = ((MailListViewItem*)item)->data();
    RecBodyP body = folderView->fetchBody(mail);

    QString rtext;
    rtext += QString("* %1 wrote on %2:\n")        // no i18n on purpose
        .arg(  mail->getFrom())
             .arg( mail->getDate());

    QString text = body->Bodytext();
    QStringList lines = QStringList::split(QRegExp("\\n"), text);
    QStringList::Iterator it;
    for (it = lines.begin(); it != lines.end(); it++)
    {
        rtext += "> " + *it + "\n";
    }
    rtext += "\n";

    QString prefix;
    if ( mail->getSubject().find(QRegExp("^Re: .*$")) != -1) prefix = "";
    else prefix = "Re: ";                // no i18n on purpose

    Settings *settings = new Settings();
    ComposeMail composer( settings ,this, 0, true);
    if (mail->Replyto().isEmpty()) {
        composer.setTo( mail->getFrom());
    } else {
        composer.setTo( mail->Replyto());
    }
    composer.setSubject( prefix + mail->getSubject());
    composer.setMessage( rtext );
    composer.setInReplyTo( mail->Msgid());
    composer.setCharset( body->getCharset() );

    mCurrentComposer = &composer;
    if ( QDialog::Accepted == KApplication::execDialog( &composer ) )
    {
        mail->Wrapper()->answeredMail(mail);
    }
    mCurrentComposer = 0;
    folderView->refreshOutgoing();
    delete settings;

}
void OpieMail::closeViewMail(ViewMail * vm)
{
    vm->hide();
}

void  OpieMail::slotDownloadMail( )
{
    QListViewItem*item = mailView->currentItem();
    if (!item ) {
        Global::statusMessage("Error: No item slected!");
        return;
    }
    RecMailP mail = ((MailListViewItem*)item)->data();
    Account * acc = mail->Wrapper()->getAccount();
    if ( !acc ) {
        Global::statusMessage("Mail is already stored locally!");
        return;
    }
    QString lfName = acc->getLocalFolder();
    //qDebug("local folder " + lfName );
    if ( lfName.isEmpty() )
        lfName = acc->getAccountName();
    AbstractMail* targetMail = folderView->allAccounts()[0]->getWrapper();
    //qDebug("target %d %d ",targetMail,mail->Wrapper() );
    if ( targetMail == mail->Wrapper() ) {
        Global::statusMessage("Mail is already locally stored!");
        return;
    }
    if ( !targetMail->createMbox(lfName)) {
        Global::statusMessage("Error creating folder!");
        return;
    }
    Global::statusMessage("Fetching mail...please wait!");
    qApp->processEvents();
    encodedString*st = 0;
    st = mail->Wrapper()->fetchRawBody(mail);
    if ( st ) {
        targetMail->storeMessage(st->Content(),st->Length(),lfName);
        Global::statusMessage("Mail stored in "+ lfName);
        delete st;
    } else {
        Global::statusMessage("Error: Cannot fetch mail!");
    }
}


void OpieMail::deleteAndDisplayNextMail(ViewMail * vm)
{
    QListViewItem*item = mailView->currentItem();
    if (!item ) {
        closeViewMail(vm);
        return;
    }
    RecMailP mail = ((MailListViewItem*)item)->data();
    mail->Wrapper()->deleteMail( mail );
    item = item->itemBelow();
    if (!item ) {
        closeViewMail(vm);
        return;
    }
    mailView->setCurrentItem(item);
    mail = ((MailListViewItem*)item)->data();
    RecBodyP body = folderView->fetchBody(mail);
    vm->setBody( body );
    vm->setMail( mail );
}
void OpieMail::displayNextMail(ViewMail * vm)
{
    QListViewItem*item = mailView->currentItem();
    if (!item) return;
    ( (MailListViewItem*)item )->setPixmap( 0, QPixmap() );
    item = item->itemBelow();
    if (!item) {
        vm->setCaption(i18n("End of List" ));
        return;
    }
    mailView->setCurrentItem(item);
    RecMailP mail = ((MailListViewItem*)item)->data();
    RecBodyP body = folderView->fetchBody(mail);
    vm->setBody( body );
    vm->setMail( mail );
}
void OpieMail::displayMail()
{
    QListViewItem*item = mailView->currentItem();
    if (!item) return;
    RecMailP mail = ((MailListViewItem*)item)->data();
    RecBodyP body = folderView->fetchBody(mail);
    ViewMail readMail( this,"", Qt::WType_Modal   );
    readMail.setBody( body );
    readMail.setMail( mail );
#ifndef DESKTOP_VERSION
    readMail.showMaximized();
#else
    readMail.resize( 640, 480);
#endif
    connect(  &readMail,SIGNAL( showNextMail(ViewMail *)  ), this, SLOT( displayNextMail(ViewMail *) ) );
    connect(  &readMail,SIGNAL( deleteAndDisplayNextMail(ViewMail *) ), this, SLOT(deleteAndDisplayNextMail(ViewMail *) ) );
    connect(  &readMail,SIGNAL( signalDownloadMail()  ), this, SLOT( slotDownloadMail() ) );

    readMail.exec();

    if (  readMail.deleted )
    {
        folderView->refreshCurrent();
    }
    else
    {
        QListViewItem*item = mailView->currentItem();
        if (item)
            ( (MailListViewItem*)item )->setPixmap( 0, QPixmap() );
    }
}
void OpieMail::slotGetAllMail()
{
    QListViewItem * item = folderView->firstChild();
    while ( item ){
        ((AccountViewItem *)item)->contextMenuSelected( 101 );
        item = item->nextSibling ();
    }
}
void OpieMail::slotGetMail()
{
    QListViewItem * item = folderView->currentItem();
    if ( ! item ) return;
    ((AccountViewItem *)item)->contextMenuSelected( 101 );
}
void OpieMail::slotDeleteMail()
{
    if (!mailView->currentItem()) return;
    RecMailP mail = ((MailListViewItem*)mailView->currentItem() )->data();
    if ( QMessageBox::warning(this, i18n("Delete Mail"), QString( i18n("<p>Do you really want to delete this mail? <br><br>" ) + mail->getFrom() + " - " + mail->getSubject() ) , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
    {
        mail->Wrapper()->deleteMail( mail );
        folderView->refreshCurrent();
    }
}
void OpieMail::slotDeleteAllMail()
{

    QValueList<RecMailP> t;
    if ( QMessageBox::warning(this, i18n("Delete All Mails"), i18n("Do you really want to delete\nall selected mails?" )  , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
        {
            MailListViewItem* item = (MailListViewItem*)mailView->firstChild ();
            while ( item ) {
                if ( item->isSelected() ) {
                    t.append( item->data() );
                }
                item = (MailListViewItem*)item->nextSibling();
            }
        }
    else
        return;
    if ( t.count() == 0 )
        return;
    RecMailP mail = t.first();
    mail->Wrapper()->deleteMailList(t);
    folderView->refreshCurrent();


}
void OpieMail::clearSelection()
{
    mailView->clearSelection();
  
}
void OpieMail::selectAll()
{
    QListViewItem* item = mailView->firstChild ();
    while ( item ) {
        mailView->setSelected ( item, true ); 
        item = item->nextSibling();
    }
}

void OpieMail::mailHold(int button, QListViewItem *item,const QPoint&,int  )
{
    if (!mailView->currentItem()) return;
    MAILLIB::ATYPE mailtype = ((MailListViewItem*)mailView->currentItem() )->wrapperType();
    /* just the RIGHT button - or hold on pda */
    if (button!=2) {return;}
    if (!item) return;
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (mailtype==MAILLIB::A_NNTP) {
            m->insertItem(i18n("Read this posting"),this,SLOT(displayMail()));
            m->insertItem(i18n("Copy this posting"),this,SLOT(slotMoveCopyMail()));
            m->insertSeparator();
            m->insertItem(i18n("Copy all selected postings"),this,SLOT(slotMoveCopyAllMail()));
            m->insertItem(i18n("Clear selection"),this,SLOT(clearSelection()));
        } else {
            if (folderView->currentisDraft()) {
                m->insertItem(i18n("Edit this mail"),this,SLOT(reEditMail()));
            }
            m->insertItem(i18n("Reply to this mail"),this,SLOT(replyMail()));
            m->insertItem(i18n("Read this mail"),this,SLOT(displayMail()));
            m->insertSeparator();
            m->insertItem(i18n("Move/Copy this mail"),this,SLOT(slotMoveCopyMail()));
            m->insertItem(i18n("Delete this mail"),this,SLOT(slotDeleteMail()));
            m->insertSeparator();
            m->insertItem(i18n("Move/Copy all selected mail"),this,SLOT(slotMoveCopyAllMail()));
            m->insertItem(i18n("Delete all selected mails"),this,SLOT(slotDeleteAllMail()));
            m->insertSeparator();
            m->insertItem(i18n("Select all"),this,SLOT(selectAll()));
            m->insertItem(i18n("Clear selection"),this,SLOT(clearSelection()));
        }
        m->setFocus();
        m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
        delete m;
    }
}

void OpieMail::slotShowFolders( bool show )
{
    if ( show && folderView->isHidden() )
    {
        slotAdjustColumns();
        folderView->show();
        //slotAdjustColumns();
    }
    else if ( !show && !folderView->isHidden() )
    {
        slotAdjustColumnsWide();
        folderView->hide();
        //slotAdjustColumnsWide();
    }
}

void OpieMail::refreshMailView(const QValueList<RecMailP>&list)
{
    MailListViewItem*item = 0;
    mailView->clear();

    QValueList<RecMailP>::ConstIterator it;
    for (it = list.begin(); it != list.end();++it)
    {
        item = new MailListViewItem(mailView,item);
        item->storeData((*it));
        item->showEntry();
    }
    mailView->setSorting ( 4, false );
}

void OpieMail::mailLeftClicked( QListViewItem *item )
{
    mailView->clearSelection();
    /* just LEFT button - or tap with stylus on pda */
    //if (button!=1) return;
    if (!item) return;
    if (folderView->currentisDraft()) {
        reEditMail();
    } else {
        displayMail();
    }
}

void OpieMail::slotMoveCopyMail()
{
    if (!mailView->currentItem()) return;
    RecMailP mail = ((MailListViewItem*)mailView->currentItem() )->data();
    AbstractMail*targetMail = 0;
    QString targetFolder = "";
    Selectstore sels;
    folderView->setupFolderselect(&sels);
    if (!sels.exec()) return;
    targetMail = sels.currentMail();
    targetFolder = sels.currentFolder();
    if ( (mail->Wrapper()==targetMail && mail->getMbox()==targetFolder) ||
            targetFolder.isEmpty())
    {
        return;
    }
    if (sels.newFolder() && !targetMail->createMbox(targetFolder))
    {
        QMessageBox::critical(0,i18n("Error creating new Folder"),
                              i18n("<center>Error while creating<br>new folder - breaking.</center>"));
        return;
    }
    sels.hide();
    qApp->processEvents();
    // qDebug("hiding sels ");
    mail->Wrapper()->mvcpMail(mail,targetFolder,targetMail,sels.moveMails());
    folderView->refreshCurrent();
}

void OpieMail::slotMoveCopyAllMail()
{

    if (!mailView->currentItem()) return;
    QValueList<RecMailP> t;
    // if ( QMessageBox::warning(this, i18n("Move/Copy all selected mails"), i18n("Do you really want to copy/move\nall selected mails?" )  , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
    {
        MailListViewItem* item = (MailListViewItem*)mailView->firstChild ();
        while ( item ) {
            if ( item->isSelected() ) {
                t.append( item->data() );
            }
            item = (MailListViewItem*)item->nextSibling();
        }
    }
    // else
    //  return;
    if ( t.count() == 0 )
        return;
    RecMailP mail = t.first();
    AbstractMail*targetMail = 0;
    QString targetFolder = "";
    Selectstore sels;
    folderView->setupFolderselect(&sels);
    if (!sels.exec()) return;
    targetMail = sels.currentMail();
    targetFolder = sels.currentFolder();
    if ( (mail->Wrapper()==targetMail && mail->getMbox()==targetFolder) ||
            targetFolder.isEmpty())
    {
        return;
    }
    if (sels.newFolder() && !targetMail->createMbox(targetFolder))
    {
        QMessageBox::critical(0,i18n("Error creating new Folder"),
                              i18n("<center>Error while creating<br>new folder - breaking.</center>"));
        return;
    }
    sels.hide();
    qApp->processEvents();
    //qDebug("hiding sels ");
    mail->Wrapper()->mvcpMailList(t,targetFolder,targetMail,sels.moveMails());
    folderView->refreshCurrent();
}

void OpieMail::reEditMail()
{
    if (!mailView->currentItem()) return;

    ComposeMail compose( settings, this, 0, true );
    compose.reEditMail(((MailListViewItem*)mailView->currentItem() )->data());
    compose.slotAdjustColumns();
#ifndef DESKTOP_VERSION
    compose.showMaximized();
#else
    compose.resize(640,480);
#endif
    mCurrentComposer = &compose;
    compose.exec();
    folderView->refreshOutgoing();
    mCurrentComposer = 0;
}

void OpieMail::insertAttendees(const QString& uid,const QStringList& nameList,const QStringList& emailList,const QStringList& uidList)
{
    
    if ( mCurrentComposer ) {
        mCurrentComposer->insertAttendees(uid,nameList,emailList,uidList);
    } else {
        ComposeMail compose( settings, this, 0, true );
        compose.slotAdjustColumns();
#ifndef DESKTOP_VERSION
        compose.showMaximized();
#endif
        mCurrentComposer = &compose;
        mCurrentComposer->insertAttendees(uid,nameList,emailList,uidList);
        compose.exec();
        mCurrentComposer = 0;
        folderView->refreshOutgoing();
        raise();
    }
}
