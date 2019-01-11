// CHANGED 2004-09-31 Lutz Rogowski
#ifndef __GENERIC_WRAPPER_H
#define __GENERIC_WRAPPER_H

#include "abstractmail.h"
#include <qmap.h>
#include <qstring.h>
#include <libetpan/clist.h>

class RecMail;
class RecBody;
class encodedString;
struct mailpop3;
struct mailmessage;
struct mailmime;
struct mailmime_mechanism;
struct mailimf_mailbox_list;
struct mailimf_mailbox;
struct mailimf_date_time;
struct mailimf_group;
struct mailimf_address_list;
struct mailsession;
struct mailstorage;
struct mailfolder;
struct mailimf_in_reply_to;

/* this class hold just the funs shared between
 * mbox and pop3 (later mh, too) mail access.
 * it is not desigend to make a instance of it!
 */
class Genericwrapper : public AbstractMail
{
    Q_OBJECT
public:
    Genericwrapper();
    virtual ~Genericwrapper();

    virtual encodedString* fetchDecodedPart(const RecMailP&mail,const RecPartP&part);
    virtual encodedString* fetchRawPart(const RecMailP&mail,const RecPartP&part);
    virtual QString fetchTextPart(const RecMailP&mail,const RecPartP&part);
    virtual void cleanMimeCache();
    virtual int deleteMbox(const Opie::Core::OSmartPointer<Folder>&){return 1;}
    virtual void logout(){};
    virtual void storeMessage(const char*msg,size_t length, const QString&folder){};
    static const QDateTime parseDateTime( mailimf_date_time *date );

protected:
    RecBodyP parseMail( mailmessage * msg );
    QString parseMailboxList( mailimf_mailbox_list *list );
    QString parseMailbox( mailimf_mailbox *box );
    QString parseGroup( mailimf_group *group );
    QString parseAddressList( mailimf_address_list *list );

    void traverseBody(RecBodyP&target,mailmessage*message,mailmime*mime,QValueList<int>recList,unsigned int current_rek=0,int current_count=1);
    static void fillSingleBody(RecPartP&target,mailmessage*message,mailmime*mime);
    static void fillParameters(RecPartP&target,clist*parameters);
    static QString getencoding(mailmime_mechanism*aEnc);
    virtual void parseList(QValueList<Opie::Core::OSmartPointer<RecMail> > &target,mailsession*session,const QString&mailbox,bool mbox_as_to=false, int maxSizeInKb = 0);
    QStringList parseInreplies(mailimf_in_reply_to * in_replies);

    QString msgTempName;
    unsigned int last_msg_id;
    QMap<QString,encodedString*> bodyCache;
    mailstorage * m_storage;
    mailfolder*m_folder;
};

#endif
