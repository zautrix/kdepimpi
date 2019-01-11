// CHANGED 2004-09-31 Lutz Rogowski
#ifndef __MH_WRAPPER_H
#define __MH_WRAPPER_H

#include "maildefines.h"

#include "genericwrapper.h"
#include <qstring.h>

class encodedString;
struct mailmbox_folder;
class MHwrapper : public Genericwrapper
{
    Q_OBJECT
public:
    MHwrapper(const QString & dir,const QString&name);
    virtual ~MHwrapper();

    virtual void listMessages(const QString & mailbox, QValueList<Opie::Core::OSmartPointer<RecMail> > &target, int maxSizeInKb = 0 );
    virtual QValueList<Opie::Core::OSmartPointer<Folder> >* listFolders();
    virtual void statusFolder(folderStat&target_stat,const QString & mailbox="INBOX");

    virtual void deleteMail(const RecMailP&mail);
    virtual void answeredMail(const RecMailP&mail);
    virtual void mvcpMail(const RecMailP&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);
    virtual void mvcpAllMails(const Opie::Core::OSmartPointer<Folder>&fromFolder,
        const QString&targetFolder,AbstractMail*targetWrapper,bool moveit);

    virtual int createMbox(const QString&folder,const Opie::Core::OSmartPointer<Folder>&f=0,
        const QString&d="",bool s=false);
    virtual int deleteMbox(const Opie::Core::OSmartPointer<Folder>&);

    virtual void storeMessage(const char*msg,size_t length, const QString&folder);

    virtual RecBodyP fetchBody( const RecMailP &mail );
    static void mbox_progress( size_t current, size_t maximum );

    virtual encodedString* fetchRawBody(const RecMailP&mail);
    virtual void deleteMails(const QString & FolderName,const QValueList<Opie::Core::OSmartPointer<RecMail> > &target);
    virtual int deleteAllMail(const Opie::Core::OSmartPointer<Folder>&);
    virtual MAILLIB::ATYPE getType()const;
    virtual const QString&getName()const;
    virtual Account* getAccount() { return 0; };

public slots:
   
protected:
    QString buildPath(const QString&p);
    QString MHPath;
    QString MHName;

    void init_storage();
    void clean_storage();
    bool rmDir(QString folderabspath);

    bool removeMboxfailed;
};

#endif
