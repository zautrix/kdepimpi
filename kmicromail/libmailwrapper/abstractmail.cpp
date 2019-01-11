// CHANGED 2004-09-31 Lutz Rogowski
#include "abstractmail.h"
#include "imapwrapper.h"
#include "pop3wrapper.h"
#include "nntpwrapper.h"
#include "mhwrapper.h"
#include "mailtypes.h"
#include <qpe/global.h>


#include <qprogressbar.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <klocale.h>
#include <kdecore/kstandarddirs.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdlib.h>
#include <libetpan/mailmime_content.h>
#include <libetpan/mailmime.h>

using namespace Opie::Core;
AbstractMail* AbstractMail::getWrapper(IMAPaccount *a)
{
    return new IMAPwrapper(a);
}

AbstractMail* AbstractMail::getWrapper(POP3account *a)
{
    return new POP3wrapper(a);
}

AbstractMail* AbstractMail::getWrapper(NNTPaccount *a)
{
    return new NNTPwrapper(a);
}

AbstractMail* AbstractMail::getWrapper(const QString&a,const QString&name)
{
    return new MHwrapper(a,name);
}

AbstractMail* AbstractMail::getWrapper(Account*a)
{
    if (!a) return 0;
    switch (a->getType()) {
        case MAILLIB::A_IMAP:
            return new IMAPwrapper((IMAPaccount*)a);
            break;
        case MAILLIB::A_POP3:
            return new POP3wrapper((POP3account*)a);
            break;
        case MAILLIB::A_NNTP:
            return new NNTPwrapper((NNTPaccount*)a);
            break;
        default:
            return 0;
    }
}

encodedString* AbstractMail::decode_String(const encodedString*text,const QString&enc)
{
    // odebug << "Decode string start" << oendl; 
    char*result_text;
    size_t index = 0;
    /* reset for recursive use! */
    size_t target_length = 0;
    result_text = 0;
    int mimetype = MAILMIME_MECHANISM_7BIT;
    if (enc.lower()=="quoted-printable") {
        mimetype = MAILMIME_MECHANISM_QUOTED_PRINTABLE;
    } else if (enc.lower()=="base64") {
        mimetype = MAILMIME_MECHANISM_BASE64;
    } else if (enc.lower()=="8bit") {
        mimetype = MAILMIME_MECHANISM_8BIT;
    } else if (enc.lower()=="binary") {
        mimetype = MAILMIME_MECHANISM_BINARY;
    }
    int err = mailmime_part_parse(text->Content(),text->Length(),&index,mimetype,
    &result_text,&target_length);

    encodedString* result = new encodedString();
    if (err == MAILIMF_NO_ERROR) {
        result->setContent(result_text,target_length);
    }
    //odebug << "Decode string finished" << oendl; 
    return result;
}

QString AbstractMail::convert_String(const char*text)
{
    size_t index = 0;
    char*res = 0;
    int err = MAILIMF_NO_ERROR;

    QString result(text);
    //qDebug("convert_String %s ", text);
    //err = mailmime_encoded_phrase_parse("iso-8859-1",
    //                                      text, strlen(text),&index, "iso-8859-1",&res);
    err = mailmime_encoded_phrase_parse("iso-8859-1",
                                          text, strlen(text),&index, "utf-8",&res);
    if (err == MAILIMF_NO_ERROR && res && strlen(res)) {
        result = QString::fromUtf8(res);
    }
    //qDebug("convert_String:%s ",result.latin1() );
    if (res) free(res);
    return result;
}

/* cp & paste from launcher */
QString AbstractMail::gen_attachment_id()
{
    QFile file( "/proc/sys/kernel/random/uuid" );
    if (!file.open(IO_ReadOnly ) )
        return QString::null;

    QTextStream stream(&file);

    return "{" + stream.read().stripWhiteSpace() + "}";
}

int AbstractMail::createMbox(const QString&,const FolderP&,const QString& ,bool)
{
    return 0;
}

QString AbstractMail::defaultLocalfolder()
{
    //  QString f = getenv( "HOME" );
    QString f = locateLocal( "data", "kopiemail/localmail");
    //  f += "/Applications/opiemail/localmail";
    return f;
}

QString AbstractMail::draftFolder()
{
    return QString("Drafts");
}

/* temporary - will be removed when implemented in all classes */
void AbstractMail::deleteMails(const QString &,const QValueList<Opie::Core::OSmartPointer<RecMail> > &)
{
}
void AbstractMail::deleteMailList(const QValueList<RecMailP>&target)
{
    //qDebug("AbstractMail::deleteMailList:: Please reimplement! ");
    // this is currently re-implemented in pop3wrapper and imapwrapper
    int iii = 0;
    int count = target.count(); 
    QProgressBar wid ( count );
    wid.setCaption( i18n("Deleting ..."));
    wid.show();
    while (iii < count ) {   
        Global::statusMessage(i18n("Delete message %1 of %2").arg(iii).arg(count)); 
        wid.setProgress( iii ); 
        wid.raise();
        qApp->processEvents();
        RecMailP mail = (*target.at( iii ));
        deleteMail(mail);
        ++iii;
    }
}

void AbstractMail::downloadNewMails(const FolderP&fromFolder, AbstractMail*targetMail )
{
    //qDebug("AbstractMail::downloadNewMails %s ", fromFolder->getName().latin1());
    // get local folder
    Account * acc = getAccount();
    if ( !acc ) return;
    QString lfName = acc->getLocalFolder();
    if ( lfName.isEmpty() )
        lfName = acc->getAccountName();
    // create local folder
    if ( !targetMail->createMbox(lfName))
        {
            QMessageBox::critical(0,i18n("Error creating new Folder"),
                                  i18n("Error while creating new folder\n%1\n\nCancelling action.").arg(lfName));
            return;
        }
    QValueList<RecMailP> t;
    listMessages(fromFolder->getName(),t,acc->getMaxMailSize() );
    if ( t.count() == 0 ) {
        qDebug("There are no new messages %s", fromFolder->getName().latin1());
        Global::statusMessage(i18n("There are no new messages"));
        return;
    }
    Global::statusMessage(i18n("%1 :Downloading mails..."). arg(acc->getAccountName()));
    qDebug(i18n("%1 :Downloading mails..."). arg(acc->getAccountName()));
    qApp->processEvents();
    QValueList<RecMailP> e;
    targetMail->listMessages(lfName,e,acc->getMaxMailSize() );
    //qDebug("target has mails %d ", e.count());
    QValueList<RecMailP> n; 
    int iii = 0;
    int count = t.count();  
    while (iii < count ) {  
        RecMailP r = (*t.at( iii ));
        bool found = false; 
        int jjj = 0;
        int countE = e.count();   
        while (jjj < countE ) { 
            RecMailP re = (*e.at( jjj )); 
            if (  re->isEqual(r) ) {
                found = true;
                break;
            }
            ++jjj;   
        }
        if ( !found ) {
            //qDebug("AAAdate  *%s*  ", r->isodate.latin1() );
            n.append( r );
        }
        ++iii;
    } 
    qDebug("Downloaded %d messages ",n.count() );
    if ( n.count() == 0 ) {
        Global::statusMessage(i18n("There are no new messages"));
        return;
    }
    mvcpMailList( n,lfName,targetMail,!acc->getLeaveOnServer());
    Global::statusMessage(i18n("Downloaded %1 messages").arg(n.count()));
    
#if 0
 QValueList<RecMailP> t;
 listMessages(fromFolder->getName(),t, maxSizeInKb);
 mvcpMailList( t,targetFolder,targetWrapper,moveit);
#endif 

}
void AbstractMail::mvcpAllMails(const FolderP&fromFolder,
    const QString&targetFolder,AbstractMail*targetWrapper,bool moveit, int maxSizeInKb)
{
 QValueList<RecMailP> t;
 listMessages(fromFolder->getName(),t, maxSizeInKb);
 mvcpMailList( t,targetFolder,targetWrapper,moveit);

}
void AbstractMail::mvcpMailList(const QValueList<RecMailP>& t,
                                const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    encodedString*st = 0;
    int iii = 0;
    int count = t.count();
    if ( count == 0 )
        return;
    // wel, processevents is qite strange, we need a widget for getting  
    // Global::statusMessage(i18n("Copy2 message %1").arg(iii)); displaye

    QProgressBar wid ( count );
    wid.setCaption( i18n("Copying..."));
    wid.show();
    while (iii < count ) {  
        Global::statusMessage(i18n("Copy message %1 of %2").arg(iii).arg(count));
        wid.setProgress( iii ); 
        wid.raise();
        qApp->processEvents();
        RecMailP r = (*t.at( iii ));
        st = fetchRawBody(r);
        if (st) {
            targetWrapper->storeMessage(st->Content(),st->Length(),targetFolder);
            delete st;
        }
        ++iii;
    } 
    if (moveit) {
        deleteMailList( t );
        //deleteAllMail(fromFolder);
    }
}

void AbstractMail::mvcpMail(const RecMailP&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    encodedString*st = 0;
    st = fetchRawBody(mail);
    if (st) {
        targetWrapper->storeMessage(st->Content(),st->Length(),targetFolder);
        delete st;
    }
    if (moveit) {
        deleteMail(mail);
    }
}
