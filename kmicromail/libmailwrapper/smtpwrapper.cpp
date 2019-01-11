#include "smtpwrapper.h"
#include "mailwrapper.h"
#include "abstractmail.h"
#include "logindialog.h"
#include "mailtypes.h"
#include "sendmailprogress.h"

//#include <opie2/odebug.h>
//#include <qt.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <stdlib.h>
#ifndef DESKTOP_VERSION
//#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#endif
#include <libetpan/libetpan.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>


using namespace Opie::Core;
progressMailSend*SMTPwrapper::sendProgress = 0;

SMTPwrapper::SMTPwrapper(SMTPaccount * aSmtp )
    : Generatemail()
{
    m_SmtpAccount = aSmtp;
    KConfig cfg( locateLocal("config", "kopiemailrc" ) );
    cfg.setGroup( "Status" );
    m_queuedMail =  cfg.readNumEntry( "outgoing", 0 );
    emit queuedMails( m_queuedMail );
    connect( this, SIGNAL( queuedMails(int) ), this, SLOT( emitQCop(int) )  );
    m_smtp = 0;
}

SMTPwrapper::~SMTPwrapper()
{
    disc_server();
}

void SMTPwrapper::emitQCop( int queued ) {
#ifndef DESKTOP_VERSION
    // LR : not used in kde-pim
    //QCopEnvelope env( "QPE/Pim", "outgoingMails(int)" );
    //env << queued;
#endif
}

QString SMTPwrapper::mailsmtpError( int errnum ) {
    switch ( errnum ) {
    case MAILSMTP_NO_ERROR:
        return i18n( "No error" );
    case MAILSMTP_ERROR_UNEXPECTED_CODE:
        return i18n( "Unexpected error code" );
    case MAILSMTP_ERROR_SERVICE_NOT_AVAILABLE:
        return i18n( "Service not available" );
    case MAILSMTP_ERROR_STREAM:
        return i18n( "Stream error" );
    case MAILSMTP_ERROR_HOSTNAME:
        return i18n( "gethostname() failed" );
    case MAILSMTP_ERROR_NOT_IMPLEMENTED:
        return i18n( "Not implemented" );
    case MAILSMTP_ERROR_ACTION_NOT_TAKEN:
        return i18n( "Error, action not taken" );
    case MAILSMTP_ERROR_EXCEED_STORAGE_ALLOCATION:
        return i18n( "Data exceeds storage allocation" );
    case MAILSMTP_ERROR_IN_PROCESSING:
        return i18n( "Error in processing" );
    case MAILSMTP_ERROR_STARTTLS_NOT_SUPPORTED:
        return i18n( "Starttls not supported" );
        //      case MAILSMTP_ERROR_INSUFFISANT_SYSTEM_STORAGE:
        //        return i18n( "Insufficient system storage" );
    case MAILSMTP_ERROR_MAILBOX_UNAVAILABLE:
        return i18n( "Mailbox unavailable" );
    case MAILSMTP_ERROR_MAILBOX_NAME_NOT_ALLOWED:
        return i18n( "Mailbox name not allowed" );
    case MAILSMTP_ERROR_BAD_SEQUENCE_OF_COMMAND:
        return i18n( "Bad command sequence" );
    case MAILSMTP_ERROR_USER_NOT_LOCAL:
        return i18n( "User not local" );
    case MAILSMTP_ERROR_TRANSACTION_FAILED:
        return i18n( "Transaction failed" );
    case MAILSMTP_ERROR_MEMORY:
        return i18n( "Memory error" );
    case MAILSMTP_ERROR_CONNECTION_REFUSED:
        return i18n( "Connection refused" );
    default:
        return i18n( "Unknown error code" );
    }
}


void SMTPwrapper::progress( size_t current, size_t maximum ) {
    if (SMTPwrapper::sendProgress) {
        SMTPwrapper::sendProgress->setSingleMail(current, maximum );
        qApp->processEvents();
    }
}

void SMTPwrapper::storeMail(const char*mail, size_t length, const QString&box) {
    if (!mail)
        return;
    QString localfolders = AbstractMail::defaultLocalfolder();
    AbstractMail*wrap = AbstractMail::getWrapper(localfolders);
    wrap->createMbox(box);
    wrap->storeMessage(mail,length,box);
    delete wrap;
}

bool SMTPwrapper::smtpSend( mailmime *mail,bool later) {
    clist *rcpts = 0;
    char *from, *data;
    size_t size;

    from = data = 0;

    mailmessage * msg = 0;
    msg = mime_message_init(mail);
    mime_message_set_tmpdir(msg,getenv( "HOME" ));
    int r = mailmessage_fetch(msg,&data,&size);
    mime_message_detach_mime(msg);
    mailmessage_free(msg);
    if (r != MAIL_NO_ERROR || !data) {
        if (data)
            free(data);
        qDebug("Error fetching mime... ");
        return false;
    }
    msg = 0;
    if (later) {
        storeMail(data,size,"Outgoing");
        if (data)
            free( data );
        KConfig cfg( locateLocal("config", "kopiemailrc" ) );
        cfg.setGroup( "Status" );
        cfg.writeEntry( "outgoing", ++m_queuedMail );
        emit queuedMails( m_queuedMail );
        return true;
    }
    from = getFrom( mail );
    rcpts = createRcptList( mail->mm_data.mm_message.mm_fields );
    bool result = smtpSend(from,rcpts,data,size);
    if (data) {
        free(data);
    }
    if (from) {
        free(from);
    }
    if (rcpts)
        smtp_address_list_free( rcpts );
    return result;
}

void SMTPwrapper::storeFailedMail(const char*data,unsigned int size, const char*failuremessage)
{
    if (data) {
        storeMail(data,size,"Sendfailed");
    }
    if (failuremessage) {
        QMessageBox::critical(0,i18n("Error sending mail"),
            failuremessage);
    }
}

int SMTPwrapper::start_smtp_tls()
{
    if (!m_smtp) {
        return MAILSMTP_ERROR_IN_PROCESSING;
    }
    int err = mailesmtp_starttls(m_smtp);
    if (err != MAILSMTP_NO_ERROR) return err;
    mailstream_low * low;
    mailstream_low * new_low;
    low = mailstream_get_low(m_smtp->stream);
    if (!low) {
        return MAILSMTP_ERROR_IN_PROCESSING;
    }
    int fd = mailstream_low_get_fd(low);
    if (fd > -1 && (new_low = mailstream_low_ssl_open(fd))!=0) {
        mailstream_low_free(low);
        mailstream_set_low(m_smtp->stream, new_low);
    } else {
        return MAILSMTP_ERROR_IN_PROCESSING;
    }
    return err;
}

void SMTPwrapper::connect_server()
{
    QString server, user, pass;
    bool ssl;
    uint16_t port;
    ssl = false;
    bool try_tls = false;
    bool force_tls=false;
    QString failuretext = "";

    if (m_smtp || !m_SmtpAccount) {
        return;
    }
    server = m_SmtpAccount->getServer();
    if  ( m_SmtpAccount->ConnectionType() == 3 ) {
        ssl = true;
        try_tls = false;
    } else if (m_SmtpAccount->ConnectionType() == 2) {
        force_tls = true;
        try_tls = true;
    } else if (m_SmtpAccount->ConnectionType() == 1) {
        try_tls = true;
    }
    int result = 1;
    port = m_SmtpAccount->getPort().toUInt();

    m_smtp = mailsmtp_new( 20, &progress );
    if ( m_smtp == NULL ) {
        /* no failure message cause this happens when problems with memory - than we
           we can not display any messagebox */
        return;
    }
    //m_smtp->auth = MAILSMTP_AUTH_LOGIN;

    int err = MAILSMTP_NO_ERROR;
    ; // odebug << "Servername " << server << " at port " << port << "" << oendl; 
    if ( ssl ) {
        qDebug("smtp: ssl_connect ");
        err = mailsmtp_ssl_connect( m_smtp, server.latin1(), port );
    } else {
        ; // odebug << "No SSL session" << oendl; 
        err = mailsmtp_socket_connect( m_smtp, server.latin1(), port );
    }
    if ( err != MAILSMTP_NO_ERROR ) {
        qDebug("Error init SMTP connection" );
        failuretext = i18n("Error init SMTP connection:\n%1").arg(mailsmtpError(err));
        result = 0;
    }

    qDebug("SMTP connection inited ");
    /* switch to tls after init 'cause there it will send the ehlo */
    if (result) {
        err = mailsmtp_init( m_smtp );
        if (err != MAILSMTP_NO_ERROR) {
            result = 0;
            qDebug("Error init SMTP connection ");
            failuretext = i18n("Error init SMTP connection:\n%1").arg(mailsmtpError(err));
        }
    }
    if (result && try_tls) {
        qDebug("Smpt: Try TLS... ");
        err = start_smtp_tls();
        if (err != MAILSMTP_NO_ERROR) {
            try_tls = false;
            qDebug("Smpt: No TLS possible ");
        } else {
            qDebug("Smpt: Using TLS ");
        }
    }

    //qDebug("mailesmtp_ehlo %d ",err );
    if (!try_tls && force_tls) {
        result = 0;
        failuretext = i18n("Error init SMTP tls:%1").arg(mailsmtpError(err));
    }  
    //LR 05-10-22 : qDebug("no elo any more ");
   //  if ( false /*mailesmtp_ehlo(m_smtp) != MAILSMTP_NO_ERROR */) {
//         qDebug("Smpt: ehlo failed "); 
//         result = 0;
//     }
//     else {  
        //qDebug("Smpt: auth is %d -- %d %d",m_smtp->auth, MAILSMTP_AUTH_LOGIN, MAILSMTP_AUTH_PLAIN);
        if ( m_smtp->auth & MAILSMTP_AUTH_LOGIN &&  m_smtp->auth & MAILSMTP_AUTH_PLAIN ) {
            qDebug("Smpt: Using MAILSMTP_AUTH_LOGIN ");
            m_smtp->auth -= MAILSMTP_AUTH_PLAIN;
            //qDebug("Smpt: auth is %d -- %d %d",m_smtp->auth, MAILSMTP_AUTH_LOGIN, MAILSMTP_AUTH_PLAIN);
        }
        //   }
    
    if (result==1 && m_SmtpAccount->getLogin() ) {
        ; // odebug << "smtp with auth" << oendl; 
        if ( m_SmtpAccount->getUser().isEmpty() || m_SmtpAccount->getPassword().isEmpty() ) {
            // get'em
            LoginDialog login( m_SmtpAccount->getUser(),
                m_SmtpAccount->getPassword(), NULL, 0, true );
            login.show();
            if ( QDialog::Accepted == login.exec() ) {
                // ok
                user = login.getUser();
                pass = login.getPassword();
            } else {
                result = 0;
                failuretext=i18n("Login aborted - \nstoring mail to localfolder");
            }
        } else {
            user = m_SmtpAccount->getUser();
            pass = m_SmtpAccount->getPassword();
        }
        ; // odebug << "session->auth: " << m_smtp->auth << "" << oendl; 
        if (result) {
            err = mailsmtp_auth( m_smtp, (char*)user.latin1(), (char*)pass.latin1() );
            if ( err == MAILSMTP_NO_ERROR ) {
                qDebug("Smtp authentification ok ");
            } else {
                failuretext = i18n("Authentification failed");
                result = 0;
            }
        }
    }
    if ( result == 0 ) {
        mailsmtp_free(m_smtp);
        m_smtp = 0;
    }
}

void SMTPwrapper::disc_server()
{
    if (m_smtp) {
        mailsmtp_quit( m_smtp );
        mailsmtp_free( m_smtp );
        m_smtp = 0;
    }
}

int SMTPwrapper::smtpSend(char*from,clist*rcpts,const char*data,size_t size )
{
    int err,result;
    QString failuretext = "";

    connect_server();

    result = 1;
    if (m_smtp) {
        err = mailsmtp_send( m_smtp, from, rcpts, data, size );
        if ( err != MAILSMTP_NO_ERROR ) {
            qDebug("Error sending mail");
            failuretext=i18n("Error sending mail:\n%1").arg(mailsmtpError(err));
            result = 0;
        }
    } else {
        result = 0;
    }

    if (!result) {
        storeFailedMail(data,size,failuretext);
    } else {
        ; // odebug << "Mail sent." << oendl; 
        storeMail(data,size,"Sent");
    }
    return result;
}

bool SMTPwrapper::sendMail(const Opie::Core::OSmartPointer<Mail>&mail,bool later )
{
    mailmime * mimeMail;
    bool result = true;
    mimeMail = createMimeMail(mail );
    if ( mimeMail == 0 ) {
        qDebug("SMTP wrapper:Error creating mail! ");
        return false;
    } else {
        sendProgress = new progressMailSend();
        sendProgress->show();
        sendProgress->setMaxMails(1);
        result = smtpSend( mimeMail,later);
        ; // odebug << "Clean up done" << oendl; 
        sendProgress->hide();
        delete sendProgress;
        sendProgress = 0;
        mailmime_free( mimeMail ); 
        if ( m_smtp ) {
            mailsmtp_free(m_smtp);
            m_smtp = 0;
        }
    }
    return result;
}

int SMTPwrapper::sendQueuedMail(AbstractMail*wrap,const RecMailP&which) {
    size_t curTok = 0;
    mailimf_fields *fields = 0;
    mailimf_field*ffrom = 0;
    clist *rcpts = 0;
    char*from = 0;
    int res = 0;

    encodedString * data = wrap->fetchRawBody(which);
    if (!data)
        return 0;
    int err = mailimf_fields_parse( data->Content(), data->Length(), &curTok, &fields );
    if (err != MAILIMF_NO_ERROR) {
        delete data;
        delete wrap;
        return 0;
    }

    rcpts = createRcptList( fields );
    ffrom = getField(fields, MAILIMF_FIELD_FROM );
    from = getFrom(ffrom);

    if (rcpts && from) {
        res = smtpSend(from,rcpts,data->Content(),data->Length());
    }
    if (fields) {
        mailimf_fields_free(fields);
        fields = 0;
    }
    if (data) {
        delete data;
    }
    if (from) {
        free(from);
    }
    if (rcpts) {
        smtp_address_list_free( rcpts );
    }
    return res;
}

/* this is a special fun */
bool SMTPwrapper::flushOutbox() {
    bool returnValue = true;

    ; // odebug << "Sending the queue" << oendl; 
    if (!m_SmtpAccount) {
        ; // odebug << "No smtp account given" << oendl; 
        return false;
    }

    bool reset_user_value = false;
    QString localfolders = AbstractMail::defaultLocalfolder();
    AbstractMail*wrap = AbstractMail::getWrapper(localfolders);
    if (!wrap) {
        ; // odebug << "memory error" << oendl; 
        return false;
    }
    QString oldPw, oldUser;
    QValueList<RecMailP> mailsToSend;
    QValueList<RecMailP> mailsToRemove;
    QString mbox("Outgoing");
    wrap->listMessages(mbox,mailsToSend);
    if (mailsToSend.count()==0) {
        delete wrap;
        ; // odebug << "No mails to send" << oendl; 
        return false;
    }

    oldPw = m_SmtpAccount->getPassword();
    oldUser = m_SmtpAccount->getUser();
    if (m_SmtpAccount->getLogin() && (m_SmtpAccount->getUser().isEmpty() || m_SmtpAccount->getPassword().isEmpty()) ) {
        // get'em
        QString user,pass;
        LoginDialog login( m_SmtpAccount->getUser(), m_SmtpAccount->getPassword(), NULL, 0, true );
        login.show();
        if ( QDialog::Accepted == login.exec() ) {
            // ok
            user = login.getUser().latin1();
            pass = login.getPassword().latin1();
            reset_user_value = true;
            m_SmtpAccount->setUser(user);
            m_SmtpAccount->setPassword(pass);
        } else {
            return true;
        }
    }


    sendProgress = new progressMailSend();
    sendProgress->show();
    sendProgress->setMaxMails(mailsToSend.count());

    while (returnValue && mailsToSend.count()>0) {
        if (sendQueuedMail(wrap, (*mailsToSend.begin()))==0) {
            QMessageBox::critical(0,i18n("Error sending mail"),
                                  i18n("Error sending queued mail.\nBreaking."));
            returnValue = false;
        }
        mailsToRemove.append((*mailsToSend.begin()));
        mailsToSend.remove(mailsToSend.begin());
        sendProgress->setCurrentMails(mailsToRemove.count());
    }
    if (reset_user_value) {
        m_SmtpAccount->setUser(oldUser);
        m_SmtpAccount->setPassword(oldPw);
    }
  
    KConfig cfg( locateLocal("config", "kopiemailrc" ) );
    cfg.setGroup( "Status" );
    m_queuedMail = mailsToSend.count();
    cfg.writeEntry( "outgoing", m_queuedMail );
    emit queuedMails( m_queuedMail );
    sendProgress->hide();
    delete sendProgress;
    sendProgress = 0;
    wrap->deleteMails(mbox,mailsToRemove);
    delete wrap; 
    if ( m_smtp ) {
        mailsmtp_free(m_smtp);
        m_smtp = 0;
    }
    return returnValue;
}
