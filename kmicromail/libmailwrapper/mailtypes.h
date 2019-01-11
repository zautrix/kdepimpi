#ifndef __MAIL_TYPES_H
#define __MAIL_TYPES_H

#define FLAG_ANSWERED 0
#define FLAG_FLAGGED  1
#define FLAG_DELETED  2
#define FLAG_SEEN     3
#define FLAG_DRAFT    4
#define FLAG_RECENT   5

#include <opie2/osmartpointer.h>

#include <qbitarray.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qvaluelist.h>

class AbstractMail;
/* a class to describe mails in a mailbox */
/* Attention!
   From programmers point of view it would make sense to
   store the mail body into this class, too.
   But: not from the point of view of the device.
   Mailbodies can be real large. So we request them when
   needed from the mail-wrapper class direct from the server itself
   (imap) or from a file-based cache (pop3?)
   So there is no interface "const QString&body()" but you should
   make a request to the mailwrapper with this class as parameter to
   get the body. Same words for the attachments.
*/
class RecMail:public Opie::Core::ORefCount
{
public:
    RecMail();
    RecMail(const RecMail&old);
    virtual ~RecMail();
    bool isEqual( RecMail* r1 );

    const unsigned int getNumber()const{return msg_number;}
    void setNumber(unsigned int number){msg_number=number;}
    const QString&getDate()const{ return date; }
    void setDate( const QString&a ) { date = a; }
    const QString&getIsoDate()const{ return isodate; }
    void setIsoDate( const QString&a ) { isodate = a; }
    const QString&getFrom()const{ return from; }
    void setFrom( const QString&a ) { from = a; }
    const QString&getSubject()const { return subject; }
    void setSubject( const QString&s ) { subject = s; }
    const QString&getMbox()const{return mbox;}
    void setMbox(const QString&box){mbox = box;}
    void setMsgid(const QString&id){msg_id=id;}
    const QString&Msgid()const{return msg_id;}
    void setReplyto(const QString&reply){replyto=reply;}
    const QString&Replyto()const{return replyto;}
    void setMsgsize(unsigned int size){msg_size = size;}
    const unsigned int Msgsize()const{return msg_size;}
    const QString MsgsizeString()const {
        double s = msg_size;
        int w = 0;
        s/=1024;
        if (s>999.0) {
            s/=1024.0;
            ++w;
        }
        QString fsize = QString::number( s, 'f', 2 );
        if ( w == 0 ) {
            fsize += "kB" ;
        } else
            fsize += "MB" ;
        return  fsize;
    };
    void setTo(const QStringList&list);
    const QStringList&To()const;
    void setCC(const QStringList&list);
    const QStringList&CC()const;
    void setBcc(const QStringList&list);
    const QStringList&Bcc()const;
    void setInreply(const QStringList&list);
    const QStringList&Inreply()const;
    void setReferences(const QStringList&list);
    const QStringList&References()const;

    const QBitArray&getFlags()const{return msg_flags;}
    void setFlags(const QBitArray&flags){msg_flags = flags;}

    void setWrapper(AbstractMail*wrapper);
    AbstractMail* Wrapper();
    // public for debugging
    QString subject,date,isodate,from,mbox,msg_id,replyto;

protected:
    //QString subject,date,isodate,from,mbox,msg_id,replyto;
    unsigned int msg_number,msg_size;
    QBitArray msg_flags;
    QStringList to,cc,bcc,in_reply_to,references;
    AbstractMail*wrapper;
    void init();
    void copy_old(const RecMail&old);
};

typedef Opie::Core::OSmartPointer<RecMail> RecMailP;
typedef QMap<QString,QString> part_plist_t;

class RecPart:public Opie::Core::ORefCount
{
protected:
    QString m_type,m_subtype,m_identifier,m_encoding,m_description;
    unsigned int m_lines,m_size;
    part_plist_t m_Parameters;
    /* describes the position in the mail */
    QValueList<int> m_poslist;

public:
    RecPart();
    RecPart(const RecPart&);
    virtual ~RecPart();

    const QString&Type()const;
    void setType(const QString&type);
    const QString&Subtype()const;
    void setSubtype(const QString&subtype);
    const QString&Identifier()const;
    void setIdentifier(const QString&identifier);
    const QString&Encoding()const;
    void setEncoding(const QString&encoding);
    const QString&Description()const;
    void setDescription(const QString&desc);
    void setLines(unsigned int lines);
    const unsigned int Lines()const;
    void setSize(unsigned int size);
    const unsigned int Size()const;


    void setParameters(const part_plist_t&list);
    const part_plist_t&Parameters()const;
    void addParameter(const QString&key,const QString&value);
    const QString searchParamter(const QString&key)const;
    void setPositionlist(const QValueList<int>&poslist);
    const QValueList<int>& Positionlist()const;
};

typedef Opie::Core::OSmartPointer<RecPart> RecPartP;

class RecBody:public Opie::Core::ORefCount
{
protected:
    QString m_BodyText;
    QString mCharset;
    QValueList<RecPartP> m_PartsList;
    RecPartP m_description;

public:
    RecBody();
    RecBody(const RecBody&old);
    virtual ~RecBody();
    void setBodytext(const QString&);
    const QString& Bodytext()const;
    void setCharset(const QString&);
    QString getCharset()const;

    void setDescription(const RecPartP&des);
    const RecPartP& Description()const;

    void setParts(const QValueList<RecPartP>&parts);
    const QValueList<RecPartP>& Parts()const;
    void addPart(const RecPartP&part);
};

typedef Opie::Core::OSmartPointer<RecBody> RecBodyP;

class encodedString
{
public:
    encodedString();
    /*
       creates an new content string.
       it makes a deep copy of it!
     */
    encodedString(const char*nContent,unsigned int length);
    /*
      Take over the nContent. Means: it will just copy the pointer, not the content.
       so make sure: No one else frees the string, the string has allocated with
       malloc for compatibility with c-based libs
    */
    encodedString(char*nContent,unsigned int nSize);
    /* copy construkor - makes ALWAYS a deep copy!!!! */
    encodedString(const encodedString&old);
    /* assign operator - makes ALWAYS a deep copy!!!! */
    encodedString& operator=(const encodedString&old);
    /* destructor - cleans the content */
    virtual ~encodedString();

    /* returns a pointer to the content - do not delete yoursel! */
    const char*Content()const;
    /* returns the lengths of the content 'cause it must not be a null-terminated string! */
    const int Length()const;

    /*
       makes a deep copy of nContent!
     */
    void setContent(const char*nContent,int nSize);
    /*
      Take over the nContent. Means: it will just copy the pointer, not the content.
       so make sure: No one else frees the string, the string has allocated with
       malloc for compatibility with c-based libs
    */
    void setContent(char*nContent,int nSize);

protected:
    char * content;
    unsigned int size;

    void init();
    void copy_old(const encodedString&old);
    void clean();
};

struct folderStat
{
    unsigned int message_count;
    unsigned int message_unseen;
    unsigned int message_recent;
    folderStat&operator=(const folderStat&old);
};

#endif
