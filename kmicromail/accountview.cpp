
#include "accountview.h"
#include "accountitem.h"
#include "selectstore.h"

#include <libmailwrapper/settings.h>
#include <libmailwrapper/mailwrapper.h>
#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/abstractmail.h>

/* OPIE */
#include <qpe/qpeapplication.h>

/* QT */
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <qspinbox.h>
#include <klocale.h>
#include <kmessagebox.h>

using namespace Opie::Core;
AccountView::AccountView( QWidget *parent, const char *name, WFlags flags )
        : QListView( parent, name, flags )
{
    //connect( this, SIGNAL( selectionChanged(QListViewItem*) ),
    //       SLOT( refresh(QListViewItem*) ) );
    connect( this, SIGNAL( clicked(QListViewItem*) ),
           SLOT( refresh(QListViewItem*) ) );
    connect( this, SIGNAL( returnPressed(QListViewItem*) ),
           SLOT( refresh(QListViewItem*) ) );
     connect( this, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int) ),this,
          SLOT( slotHold(int,QListViewItem*,const QPoint&,int) ) );
    setSorting(0);
}

AccountView::~AccountView()
{
    imapAccounts.clear();
    mhAccounts.clear();
}

void AccountView::slotContextMenu(int id)
{
    AccountViewItem *view = static_cast<AccountViewItem *>(currentItem());
    if (!view) return;
    view->contextMenuSelected(id);
}

void AccountView::slotHold(int button, QListViewItem * item,const QPoint&,int)
{
    if (button==1) {return;}
    if (!item) return;
    AccountViewItem *view = static_cast<AccountViewItem *>(item);
    QPopupMenu*m = view->getContextMenu();
    if (!m) return;
    connect(m,SIGNAL(activated(int)),this,SLOT(slotContextMenu(int)));
    m->setFocus();
    m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
    delete m;
}

void AccountView::populate( QList<Account> list )
{
    clear();

    imapAccounts.clear();
    mhAccounts.clear();

    mhAccounts.append(new MHviewItem(AbstractMail::defaultLocalfolder(),this));

    Account *it;
    for ( it = list.first(); it; it = list.next() )
    {
        if ( it->getType() == MAILLIB::A_IMAP )
        {
            IMAPaccount *imap = static_cast<IMAPaccount *>(it);
            imapAccounts.append(new IMAPviewItem( imap, this ));
        }
        else if ( it->getType() == MAILLIB::A_POP3 )
        {
            POP3account *pop3 = static_cast<POP3account *>(it);
            /* must not be hold 'cause it isn't required */
            (void) new POP3viewItem( pop3, this );
        }
         else if ( it->getType() == MAILLIB::A_NNTP )
        {
            NNTPaccount *nntp = static_cast<NNTPaccount *>(it);
            /* must not be hold 'cause it isn't required */
            (void) new NNTPviewItem( nntp, this );
       }
     }
}

void AccountView::refresh(QListViewItem *item)
{
    if ( item )
    {
        bool ask = true;
        //qDebug("text -%s- ",item->text( 0 ).latin1() );
        if ( item->text( 0 ) == i18n (" Local Mailfolders") )
            ask = false;
        else {
            if ( item->parent() )
                if ( item->parent()->text( 0 ) == i18n (" Local Mailfolders") )
                    ask = false;
        }
        if ( ask ) {
            int result = KMessageBox::warningContinueCancel(this,
                                                        i18n("Refresh\n%1\n?").arg( item->text(0) ),
                                                        i18n("Refresh"),i18n("Refresh"),i18n("Cancel"),
                                                        true);
            if (result != KMessageBox::Continue) return;
        }
        m_currentItem = item;
        topLevelWidget()->setCaption( i18n ( "Refreshing %1 ... please wait" ). arg ( m_currentItem->text( 0 ) ) ) ;
        QTimer::singleShot( 500, this, SLOT ( refreshCurrentSelected() ) );
     
    }
}
void AccountView::refreshOutgoing()
{
    m_currentItem = currentItem();
    if ( !m_currentItem ) return;
    AccountViewItem *view = static_cast<AccountViewItem *>(m_currentItem);
    if ( !view->getFolder() )
        return;
    QString bName = view->getFolder()->getDisplayName();
    if (bName.startsWith("/")&&bName.length()>1)
    {
        bName.replace(0,1,"");
    }
    int pos = bName.findRev("/");
    if (pos > 0)
    {
        bName.replace(0,pos+1,"");
    }
    //qDebug("name *%s* ",bName.lower().latin1() );
    if ( bName.lower() == "outgoing" || bName.lower() == "sent" ||  bName.lower() == "sendfailed" ) {
        refreshCurrent();
        // qDebug("refresh ");
    }
}
void AccountView::refreshCurrentSelected()
{
    if ( !m_currentItem ) return;
    QValueList<RecMailP> headerlist;
    AccountViewItem *view = static_cast<AccountViewItem *>(m_currentItem);
    view->refresh(headerlist);
    emit refreshMailview(headerlist);
    topLevelWidget()->setCaption( i18n ( "KOpieMail/Pi" ) ) ;
}

void AccountView::refreshCurrent()
{
    m_currentItem = currentItem();
    if ( !m_currentItem ) return;
    topLevelWidget()->setCaption( i18n ( "Refreshing %1 ... please wait" ). arg ( m_currentItem->text( 0 ) ) ) ;
    QTimer::singleShot( 500, this, SLOT ( refreshCurrentSelected() ) );
}

void AccountView::refreshAll()
{
}

RecBodyP AccountView::fetchBody(const RecMailP&aMail)
{
    QListViewItem*item = selectedItem ();
    if (!item) return new RecBody();
    AccountViewItem *view = static_cast<AccountViewItem *>(item);
    return view->fetchBody(aMail);
}

void AccountView::setupFolderselect(Selectstore*sels)
{

#ifndef DESKTOP_VERSION
    sels->showMaximized();
#else
    sels->show();
#endif
    QStringList sFolders;
    unsigned int i = 0;
    for (i=0; i < mhAccounts.count();++i)
    {
        mhAccounts[i]->refresh(false);
        sFolders = mhAccounts[i]->subFolders();
        sels->addAccounts(mhAccounts[i]->getWrapper(),sFolders);
    }
    for (i=0; i < imapAccounts.count();++i)
    {
        if (imapAccounts[i]->offline())
            continue;
        imapAccounts[i]->refreshFolders(false);
        sels->addAccounts(imapAccounts[i]->getWrapper(),imapAccounts[i]->subFolders());
    }
}
void AccountView::downloadMailsInbox(const FolderP&fromFolder,AbstractMail*fromWrapper)
{
#if 0
    AbstractMail*targetMail = 0;
    QString targetFolder = "";
    Selectstore sels;
    setupFolderselect(&sels);
    if (!sels.exec()) return;
    targetMail = sels.currentMail();
    targetFolder = sels.currentFolder();
    if ( (fromWrapper==targetMail && fromFolder->getName()==targetFolder) ||
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
    int maxsize = 0;
    if ( sels.useSize->isChecked()) 
        maxsize = sels.sizeSpinBox->value();
    fromWrapper->mvcpAllMails(fromFolder,targetFolder,targetMail,sels.moveMails(),  maxsize);
#endif   
    fromWrapper->downloadNewMails( fromFolder, mhAccounts[0]->getWrapper());   
    refreshCurrent();

}

void AccountView::downloadMails(const FolderP&fromFolder,AbstractMail*fromWrapper)
{
    AbstractMail*targetMail = 0;
    QString targetFolder = "";
    Selectstore sels;
    setupFolderselect(&sels);
    if (!sels.exec()) return;
    targetMail = sels.currentMail();
    targetFolder = sels.currentFolder();
    if ( (fromWrapper==targetMail && fromFolder->getName()==targetFolder) ||
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
    int maxsize = 0;
    if ( sels.useSize->isChecked()) 
        maxsize = sels.sizeSpinBox->value();
        
    fromWrapper->mvcpAllMails(fromFolder,targetFolder,targetMail,sels.moveMails(),  maxsize);
    refreshCurrent();
}

bool AccountView::currentisDraft()
{
    AccountViewItem *view = static_cast<AccountViewItem *>(currentItem());
    if (!view) return false;
    return view->isDraftfolder();
}
