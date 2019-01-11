// -*- Mode: C++; -*-
#ifndef SMTPwrapper_H
#define SMTPwrapper_H

//#include <qpe/applnk.h>

#include <qbitarray.h>
#include <qdatetime.h>
#include <libetpan/clist.h>

#include "settings.h"
#include "generatemail.h"

#include <opie2/osmartpointer.h>

class SMTPaccount;
class AbstractMail;

class SMTPwrapper : public Generatemail
{
    Q_OBJECT

public:
    SMTPwrapper(SMTPaccount * aSmtp);
    virtual ~SMTPwrapper();
    bool sendMail(const Opie::Core::OSmartPointer<Mail>& mail,bool later=false );
    bool flushOutbox();

    static progressMailSend*sendProgress;
    virtual Account* getAccount() { return m_SmtpAccount; };

signals:
    void queuedMails( int );

protected:
    mailsmtp *m_smtp;
    SMTPaccount * m_SmtpAccount;

    void connect_server();
    void disc_server();
    int start_smtp_tls();


    bool smtpSend( mailmime *mail,bool later);

    static void storeMail(const char*mail, size_t length, const QString&box);
    static QString mailsmtpError( int err );
    static void progress( size_t current, size_t maximum );

    int smtpSend(char*from,clist*rcpts,const char*data,size_t size);

    void storeMail(mailmime*mail, const QString&box);

    int sendQueuedMail(AbstractMail*wrap,const Opie::Core::OSmartPointer<RecMail>&which);
    void storeFailedMail(const char*data,unsigned int size, const char*failuremessage);

    int m_queuedMail;

protected slots:
    void emitQCop( int queued );

};

#endif
