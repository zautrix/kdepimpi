
#include <qdialog.h>
#include "kapplication.h"
#include "defines.h"
#include "editaccounts.h"
/* OPIE */
#include <qpe/qpeapplication.h>

/* QT */
#include <qstringlist.h>

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qspinbox.h>
#include <klocale.h>
#include <kfiledialog.h>

#include <libmailwrapper/nntpwrapper.h>

using namespace Opie::Core;

AccountListItem::AccountListItem( QListView *parent, Account *a)
        : QListViewItem( parent )
{
    account = a;
    setText( 0, account->getAccountName() );
    QString ttext = "";
    switch (account->getType()) {
        case MAILLIB::A_NNTP:
            ttext="NNTP";
            break;
        case MAILLIB::A_POP3:
            ttext = "POP3";
            break;
        case MAILLIB::A_IMAP:
            ttext = "IMAP";
            break;
        case MAILLIB::A_SMTP:
            ttext = "SMTP";
            break;
        default:
            ttext = "UNKNOWN";
            break;
    }
    setText( 1, ttext);
}

EditAccounts::EditAccounts( Settings *s, QWidget *parent, const char *name, bool modal, WFlags flags )
        : EditAccountsUI( parent, name, modal, flags )
{
    settings = s;

    mailList->addColumn( i18n( "Account" ) );
    mailList->addColumn( i18n( "Type" ) );

    newsList->addColumn( i18n( "Account" ) );

    connect( newMail, SIGNAL( clicked() ), SLOT( slotNewMail() ) );
    connect( editMail, SIGNAL( clicked() ), SLOT( slotEditMail() ) );
    connect( deleteMail, SIGNAL( clicked() ), SLOT( slotDeleteMail() ) );
    connect( newNews, SIGNAL( clicked() ), SLOT( slotNewNews() ) );
    connect( editNews, SIGNAL( clicked() ), SLOT( slotEditNews() ) );
    connect( deleteNews, SIGNAL( clicked() ), SLOT( slotDeleteNews() ) );

    slotFillLists();
}

void EditAccounts::slotFillLists()
{
    mailList->clear();
    newsList->clear();

    QList<Account> accounts = settings->getAccounts();
    Account *it;
    for ( it = accounts.first(); it; it = accounts.next() )
    {
        if ( it->getType()==MAILLIB::A_NNTP )
        {
            (void) new AccountListItem( newsList, it );
        }
        else
        {
            (void) new AccountListItem( mailList, it );
        }
    }
}

void EditAccounts::slotNewMail()
{
    QString *selection = new QString();
    SelectMailType selType( selection, this, 0, true );
    selType.show();
    if ( QDialog::Accepted == selType.exec() )
    {
        slotNewAccount( *selection );
    }
}

void EditAccounts::slotNewAccount( const QString &type )
{
    if ( type.compare( i18n("IMAP") ) == 0 )
    {
        IMAPaccount *account = new IMAPaccount();
        IMAPconfig imap( account, this, 0, true );

#ifndef DESKTOP_VERSION
        imap.showMaximized();
#endif
        if ( QDialog::Accepted == imap.exec() )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        }
        else
        {
            account->remove();
        }
    }
    else if ( type.compare( i18n("POP3") ) == 0 )
    {
        POP3account *account = new POP3account();
        POP3config pop3( account, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == KApplication::execDialog( &pop3 ) )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        }
        else
        {
            account->remove();
        }
    }
    else if ( type.compare( i18n("SMTP") ) == 0 )
    {
        SMTPaccount *account = new SMTPaccount();
        SMTPconfig smtp( account, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == KApplication::execDialog( &smtp ) )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();

        }
        else
        {
            account->remove();
        }
    }
    else if ( type.compare( i18n("NNTP") ) == 0 )
    {
        NNTPaccount *account = new NNTPaccount();
        NNTPconfig nntp( account, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == KApplication::execDialog( &nntp ) )
        {
            settings->addAccount( account );
            account->save();
            slotFillLists();
        }
        else
        {
            account->remove();
        }
    }
}

void EditAccounts::slotEditAccount( Account *account )
{
    if ( account->getType() == MAILLIB::A_IMAP )
    {
        IMAPaccount *imapAcc = static_cast<IMAPaccount *>(account);
        IMAPconfig imap( imapAcc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == KApplication::execDialog( &imap ) )
        {
            slotFillLists();
        }
    }
    else if ( account->getType()==MAILLIB::A_POP3 )
    {
        POP3account *pop3Acc = static_cast<POP3account *>(account);
        POP3config pop3( pop3Acc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == KApplication::execDialog( &pop3 ) )
        {
            slotFillLists();
        }
    }
    else if ( account->getType()==MAILLIB::A_SMTP )
    {
        SMTPaccount *smtpAcc = static_cast<SMTPaccount *>(account);
        SMTPconfig smtp( smtpAcc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == KApplication::execDialog( &smtp ) )
        {
            slotFillLists();
        }
    }
    else if ( account->getType()==MAILLIB::A_NNTP)
    {
        NNTPaccount *nntpAcc = static_cast<NNTPaccount *>(account);
        NNTPconfig nntp( nntpAcc, this, 0, true, WStyle_ContextHelp );
        if ( QDialog::Accepted == KApplication::execDialog( &nntp ) )
        {
            slotFillLists();
        }
    }
}

void EditAccounts::slotDeleteAccount( Account *account )
{
    if ( QMessageBox::information( this, i18n( "Question" ),
                                   i18n( "<p>Do you really want to delete the selected Account?</p>" ),
                                   i18n( "Yes" ), i18n( "No" ) ) == 0 )
    {
        settings->delAccount( account );
        slotFillLists();
    }
}

void EditAccounts::slotEditMail()
{
    if ( !mailList->currentItem() )
    {
        QMessageBox::information( this, i18n( "Error" ),
                                  i18n( "<p>Please select an account.</p>" ),
                                  i18n( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) mailList->currentItem())->getAccount();
    slotEditAccount( a );
}

void EditAccounts::slotDeleteMail()
{
    if ( !mailList->currentItem() )
    {
        QMessageBox::information( this, i18n( "Error" ),
                                  i18n( "<p>Please select an account.</p>" ),
                                  i18n( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) mailList->currentItem())->getAccount();
    slotDeleteAccount( a );
}

void EditAccounts::slotNewNews()
{
    slotNewAccount( "NNTP" );
}

void EditAccounts::slotEditNews()
{
    if ( !newsList->currentItem() )
    {
        QMessageBox::information( this, i18n( "Error" ),
                                  i18n( "<p>Please select an account.</p>" ),
                                  i18n( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) newsList->currentItem())->getAccount();
    slotEditAccount( a );
}

void EditAccounts::slotDeleteNews()
{
    if ( !newsList->currentItem() )
    {
        QMessageBox::information( this, i18n( "Error" ),
                                  i18n( "<p>Please select an account.</p>" ),
                                  i18n( "Ok" ) );
        return;
    }

    Account *a = ((AccountListItem *) newsList->currentItem())->getAccount();
    slotDeleteAccount( a );
}

void EditAccounts::slotAdjustColumns()
{
    int currPage = configTab->currentPageIndex();

    configTab->showPage( mailTab );
    mailList->setColumnWidth( 0, mailList->visibleWidth() - 50 );
    mailList->setColumnWidth( 1, 50 );

    configTab->showPage( newsTab );
    newsList->setColumnWidth( 0, newsList->visibleWidth() );

    configTab->setCurrentPage( currPage );
}

void EditAccounts::accept()
{
    settings->saveAccounts();

    QDialog::accept();
}

/**
 * SelectMailType
 */

SelectMailType::SelectMailType( QString *selection, QWidget *parent, const char *name, bool modal, WFlags flags )
        : SelectMailTypeUI( parent, name, modal, flags )
{
    selected = selection;
    selected->replace( 0, selected->length(), typeBox->currentText() );
    connect( typeBox, SIGNAL( activated(const QString&) ), SLOT( slotSelection(const QString&) ) );
}

void SelectMailType::slotSelection( const QString &sel )
{
    selected->replace( 0, selected->length(), sel );
}

/**
 * IMAPconfig
 */

IMAPconfig::IMAPconfig( IMAPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : IMAPconfigUI( parent, name, modal, flags )
{
    data = account;

    //fillValues();

    connect( ComboBox1, SIGNAL( activated(int) ), SLOT( slotConnectionToggle(int) ) );
    ComboBox1->insertItem( "Only if available", 0 );
    ComboBox1->insertItem( "Always, Negotiated", 1 );
    ComboBox1->insertItem( "Connect on secure port", 2 );
    ComboBox1->insertItem( "Run command instead", 3 );
    CommandEdit->hide();
    fillValues();
    // ComboBox1->setCurrentItem( data->ConnectionType() );
}

void IMAPconfig::slotConnectionToggle( int index )
{
    if ( index == 2 )
    {
        portLine->setText( IMAP_SSL_PORT );
        CommandEdit->hide();
    }
    else if (  index == 3 )
    {
        portLine->setText( IMAP_PORT );
        CommandEdit->show();
    }
    else
    {
        portLine->setText( IMAP_PORT );
        CommandEdit->hide();
    }
}

void IMAPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    ComboBox1->setCurrentItem( data->ConnectionType() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
    prefixLine->setText(data->getPrefix());
  localFolder->setText( data->getLocalFolder() );
    int max = data->getMaxMailSize() ;
    if ( max ) {
        CheckBoxDown->setChecked( true );
         SpinBoxDown->setValue ( max );
    } else {
        CheckBoxDown->setChecked( false );
         SpinBoxDown->setValue ( 5 );
    }
    CheckBoxLeaveOn->setChecked( data->getLeaveOnServer() );
}

void IMAPconfig::accept()
{
    if ( localFolder->text().contains("/") ||localFolder->text().contains("\\") ) {
        QMessageBox::information( this, i18n( "Error" ),
                                  i18n( "No paths allowed in\nlocal folder settings.\nPlease specify a folder\nname or leave empty\nto create local folder\nwith account name\nautomatically." ),
                                  i18n( "Ok" ) );
        return;
    }
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setConnectionType( ComboBox1->currentItem() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );
    data->setPrefix(prefixLine->text());  
    data->setLocalFolder( localFolder->text() );
    data->setMaxMailSize( CheckBoxDown->isChecked()?SpinBoxDown->value():0 ) ;
    data->setLeaveOnServer( CheckBoxLeaveOn->isChecked() );

    QDialog::accept();
}

/**
 * POP3config
 */

POP3config::POP3config( POP3account *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : POP3configUI( parent, name, modal, flags )
{
    data = account;
    //fillValues();

    connect( ComboBox1, SIGNAL( activated(int) ), SLOT( slotConnectionToggle(int) ) );
    ComboBox1->insertItem( "Only if available", 0 );
    ComboBox1->insertItem( "Always, Negotiated", 1 );
    ComboBox1->insertItem( "Connect on secure port", 2 );
    ComboBox1->insertItem( "Run command instead", 3 );
    CommandEdit->hide();
    fillValues();
    //ComboBox1->setCurrentItem( data->ConnectionType() );
}

void POP3config::slotConnectionToggle( int index )
{
    // 2 is ssl connection
    if ( index == 2 )
    {
        portLine->setText( POP3_SSL_PORT );
        CommandEdit->hide();
    }
    else if (  index == 3 )
    {
        portLine->setText( POP3_PORT );
        CommandEdit->show();
    }
    else
    {
        portLine->setText( POP3_PORT );
        CommandEdit->hide();
    }
}

void POP3config::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    ComboBox1->setCurrentItem( data->ConnectionType() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
    localFolder->setText( data->getLocalFolder() );
    int max = data->getMaxMailSize() ;
    if ( max ) {
        CheckBoxDown->setChecked( true );
         SpinBoxDown->setValue ( max );
    } else {
        CheckBoxDown->setChecked( false );
         SpinBoxDown->setValue ( 5 );
    }
    CheckBoxLeaveOn->setChecked( data->getLeaveOnServer() );
}

void POP3config::accept()
{
    if ( localFolder->text().contains("/") ||localFolder->text().contains("\\") ) {
        QMessageBox::information( this, i18n( "Error" ),
                                  i18n( "No paths allowed in\nlocal folder settings.\nPlease specify a folder\nname or leave empty\nto create local folder\nwith account name\nautomatically." ),
                                  i18n( "Ok" ) );
        return;
    }
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setConnectionType( ComboBox1->currentItem() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );
    data->setLocalFolder( localFolder->text() );
    data->setMaxMailSize( CheckBoxDown->isChecked()?SpinBoxDown->value():0 ) ;
    data->setLeaveOnServer( CheckBoxLeaveOn->isChecked() );

    QDialog::accept();
}

/**
 * SMTPconfig
 */

SMTPconfig::SMTPconfig( SMTPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : SMTPconfigUI( parent, name, modal, flags )
{
    data = account;

    connect( loginBox, SIGNAL( toggled(bool) ), userLine, SLOT( setEnabled(bool) ) );
    connect( loginBox, SIGNAL( toggled(bool) ), passLine, SLOT( setEnabled(bool) ) );

    // fillValues();
    QIconSet icon;
    //icon = SmallIcon("fileexport");
    icon = SmallIcon("fileopen");
    SignaturButton->setText("");
    SignaturButton->setIconSet (icon ) ;
    SignaturButton->setMaximumSize ( SignaturButton->sizeHint().height()+4,SignaturButton->sizeHint().height()) ;
    connect( SignaturButton, SIGNAL( clicked() ), this, SLOT( chooseSig() ) );
    connect( ComboBox1, SIGNAL( activated(int) ), SLOT( slotConnectionToggle(int) ) );
    ComboBox1->insertItem( "No secure connection, no TLS", 0 );
    ComboBox1->insertItem( "Only if available, try TLS", 1 );
    ComboBox1->insertItem( "Always, use TLS", 2 );
    ComboBox1->insertItem( "Connect on secure port (SSL)", 3 );
    ComboBox1->insertItem( "Run command instead", 4 );
    CommandEdit->hide();
    fillValues();
    //ComboBox1->setCurrentItem( data->ConnectionType() );
}
void SMTPconfig::chooseSig()
{ 
    QString lnk = KFileDialog::getOpenFileName( "", "Choose Signatur File", this );
    if ( !lnk.isEmpty() ) {
        SignaturEdit->setText( lnk );
    }
}
void SMTPconfig::slotConnectionToggle( int index )
{
    // 3 is ssl connection
    if ( index == 3 )
    {
        portLine->setText( SMTP_SSL_PORT );
        CommandEdit->hide();
    }
    else if (  index == 4 )
    {
        portLine->setText( SMTP_PORT );
        CommandEdit->show();
    }
    else
    {
        portLine->setText( SMTP_PORT );
        CommandEdit->hide();
    }
}

void SMTPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    ComboBox1->setCurrentItem( data->ConnectionType() );
    loginBox->setChecked( data->getLogin() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
    SignaturEdit->setText( data->getSigFile() );
}

void SMTPconfig::accept()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setConnectionType( ComboBox1->currentItem() );
    data->setLogin( loginBox->isChecked() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );
    data->setSigFile( SignaturEdit->text() );

    QDialog::accept();
}

/**
 * NNTPconfig
 */

NNTPconfig::NNTPconfig( NNTPaccount *account, QWidget *parent, const char *name, bool modal, WFlags flags )
        : NNTPconfigUI( parent, name, modal, flags )
{
    data = account;

    connect( loginBox, SIGNAL( toggled(bool) ), userLine, SLOT( setEnabled(bool) ) );
    connect( loginBox, SIGNAL( toggled(bool) ), passLine, SLOT( setEnabled(bool) ) );
    connect( GetNGButton,  SIGNAL( clicked() ), this, SLOT( slotGetNG() ) );
    connect( ShowSubcribed,  SIGNAL( clicked() ), this, SLOT( slotShowSub() ) );
    connect( FilterButton,  SIGNAL( clicked() ), this, SLOT( slotShowFilter() ) );
    fillValues();

    connect( sslBox, SIGNAL( toggled(bool) ), SLOT( slotSSL(bool) ) );
}

void NNTPconfig::slotShowSub()
{
    save();
    data->save();
    ListViewGroups->clear();
    for ( QStringList::Iterator it = subscribedGroups.begin(); it != subscribedGroups.end(); ++it ) {
         QCheckListItem *item;
         item = new QCheckListItem( ListViewGroups, (*it), QCheckListItem::CheckBox );
         item->setOn( true );
    }
    topLevelWidget()->setCaption( i18n("%1 groups subscribed").arg( subscribedGroups.count()));
}
void NNTPconfig::slotShowFilter()
{
    save();
    data->save();
    ListViewGroups->clear();
    int count = 0;
    for ( QStringList::Iterator it = allGroups.begin(); it != allGroups.end(); ++it ) {
        QCheckListItem *item;
        if ( GroupFilter->text().isEmpty() || (*it).find( GroupFilter->text() ) >= 0 ) {
            item = new QCheckListItem( ListViewGroups, (*it), QCheckListItem::CheckBox );
            ++count;
            if ( subscribedGroups.contains( (*it) ) >= 1 ) {
                item->setOn( true );
            }
        }
    }
    topLevelWidget()->setCaption( i18n("Filter found %1 groups").arg( count));
}
void NNTPconfig::slotGetNG() {
    save();
    data->save();
    topLevelWidget()->setCaption( i18n("Fetching group list..."));
    qApp->processEvents();
    NNTPwrapper* tmp = new NNTPwrapper( data );
    allGroups =  tmp->listAllNewsgroups();
    topLevelWidget()->setCaption( i18n("Downloaded %1 group names").arg( allGroups.count()));
    
    ListViewGroups->clear();

    for ( QStringList::Iterator it = allGroups.begin(); it != allGroups.end(); ++it ) {
        QCheckListItem *item;
        item = new QCheckListItem( ListViewGroups, (*it), QCheckListItem::CheckBox );
        if ( subscribedGroups.contains( (*it) ) >= 1 ) {
            item->setOn( true );
            
        }
    }
    delete tmp;
}

void NNTPconfig::slotSSL( bool enabled )
{
    if ( enabled )
    {
        portLine->setText( NNTP_SSL_PORT );
    }
    else
    {
        portLine->setText( NNTP_PORT );
    }
}

void NNTPconfig::fillValues()
{
    accountLine->setText( data->getAccountName() );
    serverLine->setText( data->getServer() );
    portLine->setText( data->getPort() );
    sslBox->setChecked( data->getSSL() );
    loginBox->setChecked( data->getLogin() );
    userLine->setText( data->getUser() );
    passLine->setText( data->getPassword() );
    subscribedGroups = data->getGroups();
    /* don't forget that - you will overwrite values if user clicks cancel! */
    for ( QStringList::Iterator it = subscribedGroups.begin(); it != subscribedGroups.end(); ++it ) {
         QCheckListItem *item;
         item = new QCheckListItem( ListViewGroups, (*it), QCheckListItem::CheckBox );
         item->setOn( true );
    }
}

void NNTPconfig::save()
{
    data->setAccountName( accountLine->text() );
    data->setServer( serverLine->text() );
    data->setPort( portLine->text() );
    data->setSSL( sslBox->isChecked() );
    data->setLogin( loginBox->isChecked() );
    data->setUser( userLine->text() );
    data->setPassword( passLine->text() );

    QListViewItemIterator list_it( ListViewGroups );

    for ( ; list_it.current(); ++list_it ) {

        if ( ( (QCheckListItem*)list_it.current() )->isOn() ) {
             if ( subscribedGroups.contains( list_it.current()->text(0) ) < 1 ) 
                 subscribedGroups.append(  list_it.current()->text(0) );
        } else {
             if ( subscribedGroups.contains( list_it.current()->text(0) ) >= 1 ) 
                 subscribedGroups.remove(  list_it.current()->text(0) );
        }

    }
    data->setGroups( subscribedGroups );
}

void NNTPconfig::accept()
{
    save();
    QDialog::accept();
}

