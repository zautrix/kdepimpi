// CHANGED 2004-09-31 Lutz Rogowski
#ifndef __POP3WRAPPER
#define __POP3WRAPPER

#include "mailwrapper.h"
#include "genericwrapper.h"
#include <qstring.h>

class encodedString;
struct mailstorage;
struct mailfolder;

class POP3wrapper : public Genericwrapper
{
    Q_OBJECT

public:
    POP3wrapper( POP3account *a );
    virtual ~POP3wrapper();
    /* mailbox will be ignored */
    virtual void listMessages(const QString & mailbox, QValueList<Opie::Core::OSmartPointer<RecMail> > &target, int maxSizeInKb = 0 );
    virtual QValueList<Opie::Core::OSmartPointer<Folder> >* listFolders();
    /* mailbox will be ignored */
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMailP&mail);
    virtual void deleteMailList(const QValueList<RecMailP>&target);
    virtual void answeredMail(const RecMailP&mail);
    virtual int deleteAllMail(const Opie::Core::OSmartPointer<Folder>&);

    virtual RecBodyP fetchBody( const RecMailP &mail );
    virtual encodedString* fetchRawBody(const RecMailP&mail);
    virtual void logout();
    virtual MAILLIB::ATYPE getType()const;
    virtual const QString&getName()const;
    static void pop3_progress( size_t current, size_t maximum );
    virtual Account* getAccount() { return account; };
protected:
    void login();
    POP3account *account;
    mailstorage*m_pop3;
};

#endif
