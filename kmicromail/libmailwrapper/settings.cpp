#include <stdlib.h>
#include <qdir.h>

#include <qtextcodec.h>
#include <qtextstream.h>
#include <qfile.h>

//#include <opie2/odebug.h>
#include <kconfig.h>

#include <kstandarddirs.h>
#include "settings.h"
//#include "defines.h"

#define IMAP_PORT           "143"
#define IMAP_SSL_PORT       "993"
#define SMTP_PORT           "25"
#define SMTP_SSL_PORT       "465"
#define POP3_PORT           "110"
#define POP3_SSL_PORT       "995"
#define NNTP_PORT           "119"
#define NNTP_SSL_PORT       "563"


Settings::Settings()
    : QObject()
{  
    accounts.setAutoDelete( true ); ;
   updateAccounts();
   //qDebug("++++++++++++++++++new settings ");
}

void Settings::checkDirectory()
{
    return;
    locateLocal("data", "kopiemail" );
    /*
    if ( !QDir( (QString) getenv( "HOME" ) + "/Applications/opiemail/" ).exists() ) {
        system( "mkdir -p $HOME/Applications/opiemail" );
        qDebug("$HOME/Applications/opiemail created ");
    }
    */
}

QList<Account> Settings::getAccounts()
{
    return accounts;
}

void Settings::addAccount( Account *account )
{
    accounts.append( account );
}

void Settings::delAccount( Account *account )
{
    account->remove();
    accounts.remove( account );
}

void Settings::updateAccounts()
{
    accounts.clear();
    QDir dir( locateLocal("data", "kopiemail" ) );
    QStringList::Iterator it;

    QStringList imap = dir.entryList( "imap-*" );
    for ( it = imap.begin(); it != imap.end(); it++ ) {
        IMAPaccount *account = new IMAPaccount( (*it).replace(0, 5, "") );
        accounts.append( account );
    }

    QStringList pop3 = dir.entryList( "pop3-*" );
    for ( it = pop3.begin(); it != pop3.end(); it++ ) {
        POP3account *account = new POP3account( (*it).replace(0, 5, "") );
        accounts.append( account );
    }

    QStringList smtp = dir.entryList( "smtp-*" );
    for ( it = smtp.begin(); it != smtp.end(); it++ ) {
        SMTPaccount *account = new SMTPaccount( (*it).replace(0, 5, "") );
        accounts.append( account );
    }

    QStringList nntp = dir.entryList( "nntp-*" );
    for ( it = nntp.begin(); it != nntp.end(); it++ ) {
        NNTPaccount *account = new NNTPaccount( (*it).replace(0, 5, "") );
        accounts.append( account );
    }

    readAccounts();
}

void Settings::saveAccounts()
{
    checkDirectory();
    Account *it;

    for ( it = accounts.first(); it; it = accounts.next() ) {
        it->save();
    }
}

void Settings::readAccounts()
{
    checkDirectory();
    Account *it;

    for ( it = accounts.first(); it; it = accounts.next() ) {
        it->read();
    }
}

Account::Account()
{
    accountName = "changeMe";
    type = MAILLIB::A_UNDEFINED;
    ssl = false;
    connectionType = 1;
    offline = false;  
    maxMailSize = 0;
    leaveOnServer = false;
}

void Account::remove()
{
    QFile file( getFileName() );
    file.remove();
}

void Account::setPasswordList(const QStringList &str)
{
    password = "";
    int i;
    for ( i = 0; i < (int)str.count()  ; ++i ) {
        QChar c ( (str[i].toUInt()-131)/((int)str.count()- (i%3)));
        password.append( c );
    }
    //qDebug("password %s ", password.latin1());
}
QStringList Account::getPasswordList()
{
    int i;
    int len = password.length();
    QStringList str;
    
    for ( i = 0; i < len  ; ++i ) {
        int val = password.at(i).unicode()*(len-(i%3))+131;
        str.append( QString::number( val ) );
        // qDebug("append %s ", str[i].latin1());
    }
    return str;
}

IMAPaccount::IMAPaccount()
    : Account()
{
    file = IMAPaccount::getUniqueFileName();
    accountName = "New IMAP Account";
    ssl = false;
    connectionType = 1;
    type = MAILLIB::A_IMAP;
    port = IMAP_PORT;
}

IMAPaccount::IMAPaccount( QString filename )
    : Account()
{
    file = filename;
    accountName = "New IMAP Account";
    ssl = false;
    connectionType = 1;
    type = MAILLIB::A_IMAP;
    port = IMAP_PORT;
}

QString IMAPaccount::getUniqueFileName()
{
    int num = 0;
    QString unique;

    QDir dir( locateLocal("data", "kopiemail" ) );

    QStringList imap = dir.entryList( "imap-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "imap-" + unique ) > 0 );

    return unique;
}

void IMAPaccount::read()
{
    KConfig *conf = new KConfig( getFileName() );
    conf->setGroup( "IMAP Account" );
    accountName = conf->readEntry( "Account","" );
    if (accountName.isNull()) accountName = "";
    server = conf->readEntry( "Server","" );
    if (server.isNull()) server="";
    port = conf->readEntry( "Port","" );
    if (port.isNull()) port="143";
    connectionType = conf->readNumEntry( "ConnectionType" );
    ssl = conf->readBoolEntry( "SSL",false );
    user = conf->readEntry( "User","" );
    if (user.isNull()) user = "";
    //password = conf->readEntryCrypt( "Password","" );
    setPasswordList( conf->readListEntry( "FolderHistory"));
    if (password.isNull()) password = "";
    prefix = conf->readEntry("MailPrefix","");
    if (prefix.isNull()) prefix = "";
    offline = conf->readBoolEntry("Offline",false);
    localFolder = conf->readEntry( "LocalFolder" );
    maxMailSize = conf->readNumEntry( "MaxSize",0 );
    int lf = conf->readNumEntry( "LastFetch",0 );
    QDateTime dt ( QDate ( 2004, 1, 1 ), QTime( 0,0,0) );
    leaveOnServer = conf->readBoolEntry("LeaveOnServer",false);
    if ( lf < 0 ) lf = 0;
    lastFetch = dt.addSecs( lf );
    delete conf;
}

void IMAPaccount::save()
{
 
    Settings::checkDirectory();

    KConfig *conf = new KConfig( getFileName() );
    conf->setGroup( "IMAP Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "ConnectionType", connectionType );
    conf->writeEntry( "User", user );
    //conf->writeEntryCrypt( "Password", password );
    conf->writeEntry( "FolderHistory",getPasswordList() );
    conf->writeEntry( "MailPrefix",prefix);
    conf->writeEntry( "Offline",offline);
    conf->writeEntry( "LocalFolder", localFolder );
    conf->writeEntry( "MaxSize", maxMailSize );
    QDateTime dt ( QDate ( 2004, 1, 1 ), QTime( 0,0,0) );
    int lf = dt.secsTo ( lastFetch );
    conf->writeEntry( "LastFetch", lf );
    conf->writeEntry( "LeaveOnServer", leaveOnServer);
    conf->sync();
    delete conf;
}


QString IMAPaccount::getFileName()
{
    return locateLocal("data", "kopiemail" ) +"/imap-" + file;
}

POP3account::POP3account()
    : Account()
{
    file = POP3account::getUniqueFileName();
    accountName = "New POP3 Account";
    ssl = false;
    connectionType = 1;
    type = MAILLIB::A_POP3;
    port = POP3_PORT;
}

POP3account::POP3account( QString filename )
    : Account()
{
    file = filename;
    accountName = "New POP3 Account";
    ssl = false;
    connectionType = 1;
    type = MAILLIB::A_POP3;
    port = POP3_PORT;
}

QString POP3account::getUniqueFileName()
{
    int num = 0;
    QString unique;

    QDir dir( locateLocal("data", "kopiemail" ) );

    QStringList imap = dir.entryList( "pop3-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "pop3-" + unique ) > 0 );

    return unique;
}

void POP3account::read()
{
    KConfig *conf = new KConfig( getFileName());
    conf->setGroup( "POP3 Account" );
    accountName = conf->readEntry( "Account" );
    server = conf->readEntry( "Server" );
    port = conf->readEntry( "Port" );
    ssl = conf->readBoolEntry( "SSL" );
    connectionType = conf->readNumEntry( "ConnectionType" );
    user = conf->readEntry( "User" );
    //password = conf->readEntryCrypt( "Password" );
    setPasswordList( conf->readListEntry( "FolderHistory"));
    offline = conf->readBoolEntry("Offline",false);
    localFolder = conf->readEntry( "LocalFolder" );
    maxMailSize = conf->readNumEntry( "MaxSize",0 );
    int lf = conf->readNumEntry( "LastFetch",0 );
    QDateTime dt ( QDate ( 2004, 1, 1 ), QTime( 0,0,0) );
    leaveOnServer = conf->readBoolEntry("LeaveOnServer",false);
    lastFetch = dt.addSecs( lf );
    delete conf;
}

void POP3account::save()
{
    Settings::checkDirectory();

    KConfig *conf = new KConfig( getFileName() );
    conf->setGroup( "POP3 Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "ConnectionType", connectionType );
    conf->writeEntry( "User", user );
    //conf->writeEntryCrypt( "Password", password );
    conf->writeEntry( "FolderHistory",getPasswordList() );
    conf->writeEntry( "Offline",offline);
    conf->writeEntry( "LocalFolder", localFolder );
    conf->writeEntry( "MaxSize", maxMailSize );
    QDateTime dt ( QDate ( 2004, 1, 1 ), QTime( 0,0,0) );
    int lf = dt.secsTo ( lastFetch );
    conf->writeEntry( "LastFetch", lf );
    conf->writeEntry( "LeaveOnServer", leaveOnServer);
    conf->sync();
    delete conf;
}


QString POP3account::getFileName()
{
    return locateLocal("data", "kopiemail" ) +"/pop3-" + file;
}

SMTPaccount::SMTPaccount()
    : Account()
{
    file = SMTPaccount::getUniqueFileName();
    accountName = "New SMTP Account";
    ssl = false;
    connectionType = 1;
    login = false;
    useCC = false;
    useBCC = false;
    useReply = false;
    type = MAILLIB::A_SMTP;
    port = SMTP_PORT;
}

SMTPaccount::SMTPaccount( QString filename )
    : Account()
{
    file = filename;
    accountName = "New SMTP Account";
    ssl = false;
    connectionType = 1;
    login = false;
    type = MAILLIB::A_SMTP;
    port = SMTP_PORT;
}

QString SMTPaccount::getSignature()
{
    QFileInfo fi ( signature );
    if ( ! fi.exists() )
        return QString();

    QFile file( signature );
    if (!file.open( IO_ReadOnly ) ) {
        return QString();
        
    } 
    QTextStream ts( &file );
    ts.setCodec( QTextCodec::codecForName("utf8") );
    QString text = ts.read();
    file.close();
    
    return text;

}
void SMTPaccount::setSignature( QString b )
{

    QFileInfo fi ( signature );
    if ( ! fi.exists() ) {
        QString filedir( locateLocal("data", "kopiemail" ) );
         signature = filedir+ "/" + getAccountName() +".sig";
         qDebug("new sig %s ", signature.latin1());
         save();
    }
    QFile fileIn( signature );
    if (!fileIn.open( IO_WriteOnly ) ) {
        qDebug("OM: Cannot write signature file %s ", signature.latin1() );
        return ;
    } 
    QTextStream tsIn( &fileIn );
    tsIn.setCodec( QTextCodec::codecForName("utf8") );
    tsIn << b ;
    fileIn.close();  
}
QString SMTPaccount::getUniqueFileName()
{
    int num = 0;
    QString unique;

    QDir dir( locateLocal("data", "kopiemail" ) );

    QStringList imap = dir.entryList( "smtp-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "smtp-" + unique ) > 0 );

    return unique;
}

void SMTPaccount::read()
{
    KConfig *conf = new KConfig( getFileName() );
    conf->setGroup( "SMTP Account" );
    accountName = conf->readEntry( "Account" );
    server = conf->readEntry( "Server" );
    port = conf->readEntry( "Port" );
    ssl = conf->readBoolEntry( "SSL" );
    connectionType = conf->readNumEntry( "ConnectionType" );
    login = conf->readBoolEntry( "Login" );
    user = conf->readEntry( "User" );
    signature = conf->readEntry( "SigFile" );
    setPasswordList( conf->readListEntry( "FolderHistory"));
    delete conf;
}

void SMTPaccount::save()
{
    Settings::checkDirectory();

    KConfig *conf = new KConfig( getFileName() );
    conf->setGroup( "SMTP Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "ConnectionType", connectionType );
    conf->writeEntry( "Login", login );
    conf->writeEntry( "User", user );
    conf->writeEntry( "SigFile", signature );
    //conf->writeEntryCrypt( "Password", password );
    conf->writeEntry( "FolderHistory",getPasswordList() );
    conf->sync();
    delete conf;
}


QString SMTPaccount::getFileName()
{
    return locateLocal("data", "kopiemail" ) +"/smtp-" + file;
}

NNTPaccount::NNTPaccount()
    : Account()
{
    file = NNTPaccount::getUniqueFileName();
    accountName = "New NNTP Account";
    ssl = false;
    login = false;
    type = MAILLIB::A_NNTP;
    port = NNTP_PORT;
}

NNTPaccount::NNTPaccount( QString filename )
    : Account()
{
    file = filename;
    accountName = "New NNTP Account";
    ssl = false;
    login = false;
    type = MAILLIB::A_NNTP;
    port = NNTP_PORT;
}

QString NNTPaccount::getUniqueFileName()
{
    int num = 0;
    QString unique;

    QDir dir( locateLocal("data", "kopiemail" ) );

    QStringList imap = dir.entryList( "nntp-*" );
    do {
        unique.setNum( num++ );
    } while ( imap.contains( "nntp-" + unique ) > 0 );

    return unique;
}

void NNTPaccount::read()
{
    KConfig *conf = new KConfig( getFileName() );
    conf->setGroup( "NNTP Account" );
    accountName = conf->readEntry( "Account" );
    server = conf->readEntry( "Server" );
    port = conf->readEntry( "Port" );
    ssl = conf->readBoolEntry( "SSL" );
    login = conf->readBoolEntry( "Login" );
    user = conf->readEntry( "User" );
    //password = conf->readEntryCrypt( "Password" );
    setPasswordList( conf->readListEntry( "FolderHistory"));
    subscribedGroups = conf->readListEntry( "Subscribed");
    delete conf;
}

void NNTPaccount::save()
{
    Settings::checkDirectory();

    KConfig *conf = new KConfig( getFileName() );
    conf->setGroup( "NNTP Account" );
    conf->writeEntry( "Account", accountName );
    conf->writeEntry( "Server", server );
    conf->writeEntry( "Port", port );
    conf->writeEntry( "SSL", ssl );
    conf->writeEntry( "Login", login );
    conf->writeEntry( "User", user );
    //conf->writeEntryCrypt( "Password", password );
    conf->writeEntry( "FolderHistory",getPasswordList() );
    conf->writeEntry( "Subscribed" , subscribedGroups );
    conf->sync();
    delete conf;
}


QString NNTPaccount::getFileName()
{
    return locateLocal("data", "kopiemail" ) +"/nntp-" + file;
}
