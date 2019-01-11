
#include "accountitem.h"
#include "accountview.h"
#include "newmaildir.h"
#include "nntpgroupsdlg.h"
#include "defines.h"

#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/abstractmail.h>
#include <libmailwrapper/mailwrapper.h>
/* OPIE */
//#include <qpe/qpeapplication.h>

/* QT */
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <kiconloader.h>
#include <klocale.h>

#define GET_NEW_MAILS 101

using namespace Opie::Core;
#define SETPIX(x) if (!account->getOffline()) {setPixmap( 0,x);} else {setPixmap( 0, PIXMAP_OFFLINE );}
/**
 * POP3 Account stuff
 */
POP3viewItem::POP3viewItem( POP3account *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    SETPIX(PIXMAP_POP3FOLDER);
#if 0
    if (!account->getOffline())
    {
        setPixmap( 0,  );
    }
    else
    {
        setPixmap( 0, PIXMAP_OFFLINE );
    }
#endif
    setText( 0, account->getAccountName() );
    setOpen( true );
}

POP3viewItem::~POP3viewItem()
{
    delete wrapper;
}

AbstractMail *POP3viewItem::getWrapper()
{
    return wrapper;
}

void POP3viewItem::refresh(QValueList<Opie::Core::OSmartPointer<RecMail> > & )
{
    refresh();
}

void POP3viewItem::refresh()
{
    if (account->getOffline()) return;
    QValueList<FolderP> *folders = wrapper->listFolders();
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    QValueList<FolderP>::ConstIterator it;
    QListViewItem*item = 0;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        item = new POP3folderItem( (*it), this , item );
        item->setSelectable( (*it)->may_select());
    }
    delete folders;
}

RECBODYP POP3viewItem::fetchBody( const RecMailP &mail )
{
  
    return wrapper->fetchBody( mail );
}

QPopupMenu * POP3viewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (!account->getOffline())
        {
            m->insertItem(i18n("Get new messages"),GET_NEW_MAILS);
            m->insertItem(i18n("Disconnect"),0);
            m->insertItem(i18n("Set offline"),1);
        }
        else
        {
            m->insertItem(i18n("Get new messages"),GET_NEW_MAILS);
            m->insertItem(i18n("Set online"),1);
        }
    }
    return m;
}

void POP3viewItem::disconnect()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    wrapper->logout();
}

void POP3viewItem::setOnOffline()
{
    if (!account->getOffline())
    {
        disconnect();
    }
    account->setOffline(!account->getOffline());
    account->save();
    SETPIX(PIXMAP_POP3FOLDER);
    refresh();
}

void POP3viewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case 0:
        disconnect();
        break;
    case 1:
        setOnOffline();
        break;
    case GET_NEW_MAILS: // daunlood 
        if (account->getOffline())
            setOnOffline();
        AccountView*bl = accountView();
        if (!bl) return;
        AccountViewItem* in = findSubItem( "inbox" , 0);
        if ( ! in )
            return;
        bl->downloadMailsInbox(in->getFolder() ,getWrapper());
        setOnOffline();
        break;
    }
}

POP3folderItem::~POP3folderItem()
{}

POP3folderItem::POP3folderItem( const FolderP&folderInit, POP3viewItem *parent , QListViewItem*after  )
        : AccountViewItem(folderInit,parent,after )
{
    pop3 = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_POP3FOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

void POP3folderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
        pop3->getWrapper()->listMessages( folder->getName(),target );
}

RECBODYP POP3folderItem::fetchBody(const RecMailP&aMail)
{
    return pop3->getWrapper()->fetchBody(aMail);
}

QPopupMenu * POP3folderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(i18n("Get new messages"),GET_NEW_MAILS);
        m->insertItem(i18n("Refresh header list"),0);
        m->insertItem(i18n("Delete all mails"),1);
        m->insertItem(i18n("Move/Copie all mails"),2);
    }
    return m;
}

void POP3folderItem::downloadMails()
{
    AccountView*bl = pop3->accountView();
    if (!bl) return;
    bl->downloadMails(folder,pop3->getWrapper());
}

void POP3folderItem::contextMenuSelected(int which)
{
    AccountView * view = (AccountView*)listView();
    switch (which)
    {
    case 0:
        /* must be 'cause pop3 lists are cached */
        pop3->getWrapper()->logout();
        view->refreshCurrent();
        break;
    case 1:
        deleteAllMail(pop3->getWrapper(),folder);
        break;
    case 2:
        downloadMails();
        break;
    case GET_NEW_MAILS: // daunlood 
        view->downloadMailsInbox(getFolder() ,pop3->getWrapper());
        break;
    default:
        break;
    }
}

/**
 * NNTP Account stuff
 */
NNTPviewItem::NNTPviewItem( NNTPaccount *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    //FIXME
    SETPIX(PIXMAP_POP3FOLDER);
#if 0
    if (!account->getOffline())
    {
        setPixmap( 0,  );
    }
    else
    {
        setPixmap( 0, PIXMAP_OFFLINE );
    }
#endif
    setText( 0, account->getAccountName() );
    setOpen( true );
}

NNTPviewItem::~NNTPviewItem()
{
    delete wrapper;
}

AbstractMail *NNTPviewItem::getWrapper()
{
    return wrapper;
}

void NNTPviewItem::refresh( QValueList<RecMailP> & )
{
    refresh();
}

void NNTPviewItem::refresh()
{
    if (account->getOffline()) return;
    QValueList<FolderP> *folders = wrapper->listFolders();

    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    QValueList<FolderP>::ConstIterator it;
    QListViewItem*item = 0;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        item = new NNTPfolderItem( (*it), this , item );
        item->setSelectable( (*it)->may_select());
    }
    delete folders;
}

RECBODYP NNTPviewItem::fetchBody( const RecMailP &mail )
{
 
    return wrapper->fetchBody( mail );
}

QPopupMenu * NNTPviewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (!account->getOffline())
        {
            m->insertItem(i18n("Disconnect"),0);
            m->insertItem(i18n("Set offline"),1);
            //m->insertItem(i18n("(Un-)Subscribe groups"),2);
        }
        else
        {
            m->insertItem(i18n("Set online"),1);
        }
    }
    return m;
}

void NNTPviewItem::subscribeGroups()
{
    NNTPGroupsDlg dlg(account);
#ifndef DESKTOP_VERSION
    dlg.showMaximized();
#endif
    if ( dlg.exec()== QDialog::Accepted ){
        refresh();
    }
}

void NNTPviewItem::disconnect()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    wrapper->logout();
}

void NNTPviewItem::setOnOffline()
{
    if (!account->getOffline())
    {
        disconnect();
    }
    account->setOffline(!account->getOffline());
    account->save();
   //FIXME
    SETPIX(PIXMAP_POP3FOLDER);
    refresh();
}

void NNTPviewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case 0:
        disconnect();
        break;
    case 1:
        setOnOffline();
        break;
    case 2:
        subscribeGroups();
        break;
    }
}

NNTPfolderItem::~NNTPfolderItem()
{}

NNTPfolderItem::NNTPfolderItem( const FolderP &folderInit, NNTPviewItem *parent , QListViewItem*after  )
        : AccountViewItem( folderInit, parent,after )
{
    nntp = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_POP3FOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

void NNTPfolderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
        nntp->getWrapper()->listMessages( folder->getName(),target );
}

RECBODYP NNTPfolderItem::fetchBody(const RecMailP&aMail)
{
    return nntp->getWrapper()->fetchBody(aMail);
}

QPopupMenu * NNTPfolderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(i18n("Refresh header list"),0);
        m->insertItem(i18n("Copy all postings"),1);
    }
    return m;
}

void NNTPfolderItem::downloadMails()
{
    AccountView*bl = nntp->accountView();
    if (!bl) return;
    bl->downloadMails(folder,nntp->getWrapper());
}

void NNTPfolderItem::contextMenuSelected(int which)
{
    AccountView * view = (AccountView*)listView();
    switch (which)
    {
    case 0:
        /* must be 'cause pop3 lists are cached */
        nntp->getWrapper()->logout();
        view->refreshCurrent();
        break;
    case 1:
        downloadMails();
        break;
    default:
        break;
    }
}

/**
 * IMAP Account stuff
 */
IMAPviewItem::IMAPviewItem( IMAPaccount *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    SETPIX(PIXMAP_IMAPFOLDER);
    setText( 0, account->getAccountName() );
    setOpen( true );
}

IMAPviewItem::~IMAPviewItem()
{
    delete wrapper;
}

AbstractMail *IMAPviewItem::getWrapper()
{
    return wrapper;
}

void IMAPviewItem::refresh(QValueList<RecMailP>&)
{
    refreshFolders(false);
}

const QStringList&IMAPviewItem::subFolders()
{
    return currentFolders;
}

void IMAPviewItem::refreshFolders(bool force)
{
    if (childCount()>0 && force==false) return;
    if (account->getOffline()) return;

    removeChilds();
    currentFolders.clear();
    QValueList<FolderP> * folders = wrapper->listFolders();

    QValueList<FolderP>::Iterator it;
    QListViewItem*item = 0;
    QListViewItem*titem = 0;
    QString fname,del,search;
    int pos;

    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        if ((*it)->getDisplayName().lower()=="inbox")
        {
            item = new IMAPfolderItem( (*it), this , item );
            folders->remove(it);
            break;
        }
    }
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        fname = (*it)->getDisplayName();
        currentFolders.append((*it)->getName());
        pos = fname.findRev((*it)->Separator());
        if (pos != -1)
        {
            fname = fname.left(pos);
        }
        IMAPfolderItem*pitem = (IMAPfolderItem*)findSubItem(fname);
        if (pitem)
        {
            titem = item;
            item = new IMAPfolderItem( (*it),pitem,pitem->firstChild(),this);
            /* setup the short name */
            item->setText(0,(*it)->getDisplayName().mid(pos+1));
            item = titem;
        }
        else
        {
            item = new IMAPfolderItem( (*it), this , item );
        }
    }
    delete folders;
}

QPopupMenu * IMAPviewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (!account->getOffline())
        {
            m->insertItem(i18n("Get new messages"),GET_NEW_MAILS);
            m->insertItem(i18n("Refresh folder list"),0);
            m->insertItem(i18n("Create new folder"),1);
            m->insertSeparator();
            m->insertItem(i18n("Disconnect"),2);
            m->insertItem(i18n("Set offline"),3);
            m->insertSeparator();
        }
        else
        {
            m->insertItem(i18n("Get new messages"),GET_NEW_MAILS);
            m->insertItem(i18n("Set online"),3);
        }
    }
    return m;
}

void IMAPviewItem::createNewFolder()
{
    Newmdirdlg ndirdlg;

#ifndef DESKTOP_VERSION
    //ndirdlg.showMaximized();
#endif
    if ( ndirdlg.exec() )
    {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = "/";
        IMAPfolderItem*item = (IMAPfolderItem*)firstChild();
        if (item)
        {
            delemiter = item->Delemiter();
        }
        if (wrapper->createMbox(ndir,0,delemiter,makesubs))
        {
            refreshFolders(true);
        }
    }
}

void IMAPviewItem::contextMenuSelected(int id)
{
   
    switch (id)
    {
    case 0:
        refreshFolders(true);
        break;
    case 1:
        createNewFolder();
        break;
    case 2:
        removeChilds();
        wrapper->logout();
        break;
    case 3:
        if (account->getOffline()==false)
        {
            removeChilds();
            wrapper->logout();
        }
        account->setOffline(!account->getOffline());
        account->save();
        SETPIX(PIXMAP_IMAPFOLDER);
        refreshFolders(false);
        break;
    case GET_NEW_MAILS: // daunlood 
        {
            if (account->getOffline()) {
                contextMenuSelected( 3 );
            }
        AccountView*bl = accountView();
        if (!bl) return;
        AccountViewItem* in = findSubItem( "inbox" , 0);
        if ( ! in )
            return;
        bl->downloadMailsInbox(in->getFolder(),getWrapper());
        }
        break;
    default:
        break;
    }
}

RECBODYP IMAPviewItem::fetchBody(const RecMailP&)
{
    return new RecBody();
}

bool IMAPviewItem::offline()
{
    return account->getOffline();
}

IMAPfolderItem::IMAPfolderItem( const FolderP& folderInit, IMAPviewItem *parent , QListViewItem*after )
        : AccountViewItem( folderInit, parent , after )
{
    imap = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

IMAPfolderItem::IMAPfolderItem(const FolderP &folderInit, IMAPfolderItem *parent , QListViewItem*after, IMAPviewItem *master  )
        : AccountViewItem(folderInit, parent,after )
{
    imap = master;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

IMAPfolderItem::~IMAPfolderItem()
{}

const QString& IMAPfolderItem::Delemiter()const
{
    return folder->Separator();
}

void IMAPfolderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
    {
        imap->getWrapper()->listMessages( folder->getName(),target );
    }
    else
    {
        target.clear();
    }
}

RECBODYP IMAPfolderItem::fetchBody(const RecMailP&aMail)
{
    return imap->getWrapper()->fetchBody(aMail);
}

QPopupMenu * IMAPfolderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        if (folder->may_select())
        {
            m->insertItem(i18n("Get new messages"),GET_NEW_MAILS);
            m->insertItem(i18n("Refresh header list"),0);
            m->insertItem(i18n("Move/Copie all mails"),4);
            m->insertItem(i18n("Delete all mails"),1);
        }
        if (folder->no_inferior()==false)
        {
            m->insertItem(i18n("Create new subfolder"),2);
        }
        if (folder->getDisplayName().lower()!="inbox")
        {
            m->insertItem(i18n("Delete folder"),3);
        }
    }
    return m;
}

void IMAPfolderItem::createNewFolder()
{
    Newmdirdlg ndirdlg;
    //ndirdlg.showMaximized();
    if ( ndirdlg.exec() )
    {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = Delemiter();
        if (imap->wrapper->createMbox(ndir,folder,delemiter,makesubs))
        {
            imap->refreshFolders(true);
        }
    }
}

void IMAPfolderItem::deleteFolder()
{
    int yesno = QMessageBox::warning(0,i18n("Delete folder"),
                                     i18n("<center>Realy delete folder <br><b>%1</b><br>and all if it content?</center>").arg(folder->getDisplayName()),
                                     i18n("Yes"),
                                     i18n("No"),QString::null,1,1);
   
    if (yesno == 0)
    {
        if (imap->getWrapper()->deleteMbox(folder))
        {
            QListView*v=listView();
            IMAPviewItem * box = imap;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            imap->refreshFolders(true);
            if (v)
            {
                v->setSelected(box,true);
            }
        }
    }
}

void IMAPfolderItem::downloadMails()
{
    AccountView*bl = imap->accountView();
    if (!bl) return;
    bl->downloadMails(folder,imap->getWrapper());
}

void IMAPfolderItem::contextMenuSelected(int id)
{
  
    AccountView * view = (AccountView*)listView();
    switch(id)
    {
    case 0:
        view->refreshCurrent();
        break;
    case 1:
        deleteAllMail(imap->getWrapper(),folder);
        break;
    case 2:
        createNewFolder();
        break;
    case 3:
        deleteFolder();
        break;
    case 4:
        downloadMails();
        break;
    case GET_NEW_MAILS: // daunlood 
        { 
            if (!view) return;
            view->downloadMailsInbox(getFolder(),imap->getWrapper()); 
        }
        break;
    default:
        break;
    }
}

/**
 * MH Account stuff
 */
/* MH is a little bit different - the top folder can contains messages other than in IMAP and
   POP3 and MBOX */
MHviewItem::MHviewItem( const QString&aPath, AccountView *parent )
        : AccountViewItem( parent )
{
    m_Path = aPath;
    /* be carefull - the space within settext is wanted - thats why the string twice */
    wrapper = AbstractMail::getWrapper( m_Path,"Local Mailfolders");
    setPixmap( 0, PIXMAP_LOCALFOLDER );
    setText( 0, i18n(" Local Mailfolders") );
    setOpen( true );
    folder = 0;
}

MHviewItem::~MHviewItem()
{
    delete wrapper;
}

AbstractMail *MHviewItem::getWrapper()
{
    return wrapper;
}

void MHviewItem::refresh( QValueList<RecMailP> & target)
{
    refresh(false);
    getWrapper()->listMessages( "",target );
}

void MHviewItem::refresh(bool force)
{
    if (childCount()>0 && force==false) return;
    removeChilds();
    currentFolders.clear();
    QValueList<FolderP> *folders = wrapper->listFolders();
    QValueList<FolderP>::ConstIterator it;
    MHfolderItem*item = 0;
    MHfolderItem*pmaster = 0;
    QString fname = "";
    int pos;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        fname = (*it)->getDisplayName();
        /* this folder itself */
        if (fname=="/")
        {
            currentFolders.append(fname);
            folder = (*it);
            continue;
        }
        currentFolders.append(fname);
        pos = fname.findRev("/");
        if (pos > 0)
        {
            fname = fname.left(pos);
            pmaster = (MHfolderItem*)findSubItem(fname);
        }
        else
        {
            pmaster = 0;
        }
        if (pmaster)
        {
            item = new MHfolderItem( (*it), pmaster, item, this );
        }
        else
        {
            item = new MHfolderItem( (*it), this , item );
        }
        item->setSelectable((*it)->may_select());
    }
    delete folders;
}

RECBODYP MHviewItem::fetchBody( const RecMailP &mail )
{
 
    return wrapper->fetchBody( mail );
}

QPopupMenu * MHviewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(i18n("Refresh folder list"),0);
        m->insertItem(i18n("Create new folder"),1);
        m->insertItem(i18n("Delete all mails"),2);
        m->insertItem(i18n("Move/Copie all mails"),3);
    }
    return m;
}

void MHviewItem::createFolder()
{
    Newmdirdlg ndirdlg(0,0,true);
    //ndirdlg.showMaximized();
    if ( ndirdlg.exec() )
    {
        QString ndir = ndirdlg.Newdir();
        if (wrapper->createMbox(ndir))
        {
            refresh(true);
        }
    }
}

void MHviewItem::downloadMails()
{
    AccountView*bl = accountView();
    if (!bl) return;
    bl->downloadMails(folder,getWrapper());
}

QStringList MHviewItem::subFolders()
{
    return currentFolders;
}

void MHviewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case 0:
        refresh(true);
        break;
    case 1:
        createFolder();
        break;
    case 2:
        deleteAllMail(getWrapper(),folder);
        break;
    case 3:
        downloadMails();
        break;
    default:
        break;
    }
}

MHfolderItem::~MHfolderItem()
{}

MHfolderItem::MHfolderItem( const FolderP &folderInit, MHviewItem *parent , QListViewItem*after  )
        : AccountViewItem(folderInit, parent,after )
{
    mbox = parent;
    initName();
}

MHfolderItem::MHfolderItem(const FolderP& folderInit, MHfolderItem *parent, QListViewItem*after, MHviewItem*master)
        : AccountViewItem(folderInit, parent,after )
{
    folder = folderInit;
    mbox = master;
    initName();
}

void MHfolderItem::initName()
{
    QString bName = folder->getDisplayName();
    if (bName.startsWith("/")&&bName.length()>1)
    {
        bName.replace(0,1,"");
    }
    int pos = bName.findRev("/");
    if (pos > 0)
    {
        bName.replace(0,pos+1,"");
    }
    if (bName.lower() == "outgoing")
    {
        setPixmap( 0, PIXMAP_OUTBOXFOLDER );
    }
    else if (bName.lower() == "inbox")
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    } else if (bName.lower() == "drafts") {
        setPixmap(0, SmallIcon ("edit"));
    } else {
        setPixmap( 0, PIXMAP_MBOXFOLDER );
    }
    setText( 0, bName );
}

const FolderP&MHfolderItem::getFolder()const
{
    return folder;
}

void MHfolderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
        mbox->getWrapper()->listMessages( folder->getName(),target );
}

RECBODYP MHfolderItem::fetchBody(const RecMailP&aMail)
{
    return mbox->getWrapper()->fetchBody(aMail);
}

void MHfolderItem::deleteFolder()
{
    int yesno = QMessageBox::warning(0,i18n("Delete folder"),
                                     i18n("<center>Realy delete folder <br><b>%1</b><br>and all if it content?</center>").arg(folder->getDisplayName()),
                                     i18n("Yes"),
                                     i18n("No"),QString::null,1,1);
  
    if (yesno == 0)
    {
        if (mbox->getWrapper()->deleteMbox(folder))
        {
            QListView*v=listView();
            MHviewItem * box = mbox;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            mbox->refresh(true);
            if (v)
            {
                v->setSelected(box,true);
            }
        }
    }
}

QPopupMenu * MHfolderItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        m->insertItem(i18n("Move/Copie all mails"),2);
        m->insertItem(i18n("Delete all mails"),0);
        m->insertItem(i18n("Create new subfolder"),3);
        m->insertItem(i18n("Delete folder"),1);
    }
    return m;
}

void MHfolderItem::downloadMails()
{
    AccountView*bl = mbox->accountView();
    if (!bl) return;
    bl->downloadMails(folder,mbox->getWrapper());
}

void MHfolderItem::createFolder()
{
    Newmdirdlg ndirdlg(0,0,true);
    //ndirdlg.showMaximized();
    if (ndirdlg.exec()  )
    {
        QString ndir = ndirdlg.Newdir();
        if (mbox->getWrapper()->createMbox(ndir,folder))
        {
            QListView*v=listView();
            MHviewItem * box = mbox;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            mbox->refresh(true);
            if (v)
            {
                v->setSelected(box,true);
            }
        }
    }
}

void MHfolderItem::contextMenuSelected(int which)
{
    switch(which)
    {
    case 0:
        deleteAllMail(mbox->getWrapper(),folder);
        break;
    case 1:
        deleteFolder();
        break;
    case 2:
        downloadMails();
        break;
    case 3:
        createFolder();
        break;
    default:
        break;
    }
}

bool MHfolderItem::isDraftfolder()
{
    if (folder && folder->getName()==AbstractMail::defaultLocalfolder()+"/"+AbstractMail::draftFolder()) return true;
    return false;
}

/**
 * Generic stuff
 */

const QString AccountViewItem::contextName="AccountViewItem";

AccountViewItem::AccountViewItem( AccountView *parent )
        : QListViewItem( parent )
{
    init();
    m_Backlink = parent;
}

AccountViewItem::AccountViewItem( QListViewItem *parent)
        : QListViewItem( parent),folder(0)
{
    init();
}

AccountViewItem::AccountViewItem( QListViewItem *parent , QListViewItem*after  )
        :QListViewItem( parent,after ),folder(0)
{
    init();
}

AccountViewItem::AccountViewItem( const Opie::Core::OSmartPointer<Folder>&folderInit,QListViewItem *parent , QListViewItem*after  )
       :QListViewItem( parent,after ),folder(folderInit)
{
    init();
}

void AccountViewItem::init()
{
    m_Backlink = 0;
}

AccountViewItem::~AccountViewItem()
{
    folder = 0;
}

AccountView*AccountViewItem::accountView()
{
    return m_Backlink;
}

void AccountViewItem::deleteAllMail(AbstractMail*wrapper,const FolderP&folder)
{
    if (!wrapper) return;
    QString fname="";
    if (folder) fname = folder->getDisplayName();
    int yesno = QMessageBox::warning(0,i18n("Delete all mails"),
                                     i18n("<center>Realy delete all mails in box <br>%1</center>").
                                     arg(fname),
                                     i18n("Yes"),
                                     i18n("No"),QString::null,1,1);
 
    if (yesno == 0)
    {
        if (wrapper->deleteAllMail(folder))
        {
            AccountView * view = (AccountView*)listView();
            if (view) view->refreshCurrent();
        }
    }
}

void AccountViewItem::removeChilds()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
}

bool AccountViewItem::matchName(const QString&name)const
{
    if (!folder) return false;
    return folder->getDisplayName().lower()==name.lower();
}


AccountViewItem*AccountViewItem::findSubItem(const QString&path,AccountViewItem*start)
{
    AccountViewItem*pitem,*sitem;
    if (!start) pitem = (AccountViewItem*)firstChild();
    else pitem = (AccountViewItem*)start->firstChild();
    while (pitem)
    {
        if (pitem->matchName(path))
        {
            break;
        }
        if (pitem->childCount()>0)
        {
            sitem = findSubItem(path,pitem);
            if (sitem)
            {
                pitem = sitem;
                break;
            }
        }
        pitem=(AccountViewItem*)pitem->nextSibling();
    }
    return pitem;
}

bool AccountViewItem::isDraftfolder()
{
    return false;
}
