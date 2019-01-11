#include "mailtypes.h"

//#include <opie2/odebug.h>

#include <stdlib.h>

using namespace Opie::Core;
RecMail::RecMail()
    :Opie::Core::ORefCount(),subject(""),date(""),from(""),mbox(""),msg_id(""),msg_number(0),msg_size(0),msg_flags(7)
{
    init();
}

RecMail::RecMail(const RecMail&old)
    :Opie::Core::ORefCount(),subject(""),date(""),from(""),mbox(""),msg_id(""),msg_number(0),msg_flags(7)
{
    init();
    copy_old(old);
    // odebug << "Copy constructor RecMail" << oendl; 
}

RecMail::~RecMail()
{
    wrapper = 0;
}
static bool stringCompareRec( const QString& s1, const QString& s2 )
{
    if ( s1.isEmpty() && s2.isEmpty() )
        return true;
    return s1 == s2;
}
#if 0
QString RecMail::MsgsizeString() const
{
 
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
}
#endif
bool RecMail::isEqual( RecMail* r1 ) 
{
    if ( !stringCompareRec( isodate, r1->isodate ) ) {
        //qDebug("date  *%s*   *%s* ", isodate.latin1(), r1->isodate.latin1());
        return false;
    }   
    if ( !stringCompareRec( subject.left(40), r1->subject.left(40) ) ) {
        //qDebug("sub *%s*  *%s*", subject.latin1(), r1->subject.latin1());
        return false;
    }

    //qDebug("date  *%s*   *%s* ", isodate.latin1(), r1->isodate.latin1());
    if ( !stringCompareRec( from.left(40), r1->from.left(40)) ) {
        if ( r1->from.find ( from ) < 0 ) {
            if ( !stringCompareRec( from.simplifyWhiteSpace ().left(40), r1->from.simplifyWhiteSpace ().left(40)) ) {
                //qDebug("from  *%s*   *%s* ", from.left(40).latin1(), r1->from.left(20).latin1());
                return false;
            }
        }
    }

    return true;
}
void RecMail::copy_old(const RecMail&old)
{
    subject = old.subject;
    date = old.date;
    mbox = old.mbox;
    msg_id = old.msg_id;
    msg_size = old.msg_size;
    msg_number = old.msg_number;
    from = old.from;
    msg_flags = old.msg_flags;
    to = old.to;
    cc = old.cc;
    bcc = old.bcc;
    wrapper = old.wrapper;
    in_reply_to = old.in_reply_to;
    references = old.references;
    replyto = old.replyto;
}

void RecMail::init()
{
    to.clear();
    cc.clear();
    bcc.clear();
    in_reply_to.clear();
    references.clear();
    wrapper = 0;
}

void RecMail::setWrapper(AbstractMail*awrapper)
{
    wrapper = awrapper;
}

AbstractMail* RecMail::Wrapper()
{
    return wrapper;
}

void RecMail::setTo(const QStringList&list)
{
    to = list;
}

const QStringList&RecMail::To()const
{
    return to;
}

void RecMail::setCC(const QStringList&list)
{
    cc = list;
}

const QStringList&RecMail::CC()const
{
    return cc;
}

void RecMail::setBcc(const QStringList&list)
{
    bcc = list;
}

const QStringList& RecMail::Bcc()const
{
    return bcc;
}

void RecMail::setInreply(const QStringList&list)
{
    in_reply_to = list;
}

const QStringList& RecMail::Inreply()const
{
    return in_reply_to;
}

void RecMail::setReferences(const QStringList&list)
{
    references = list;
}

const QStringList& RecMail::References()const
{
    return references;
}

RecPart::RecPart()
    : Opie::Core::ORefCount(),
     m_type(""),m_subtype(""),m_identifier(""),m_encoding(""),m_description(""),m_lines(0),m_size(0)
{
    m_Parameters.clear();
    m_poslist.clear();
}

RecPart::RecPart(const RecPart&old)
    : Opie::Core::ORefCount(),
    m_type(""),m_subtype(""),m_identifier(""),m_encoding(""),m_description(""),m_lines(0),m_size(0)
{
    m_type = old.m_type;
    m_subtype = old.m_subtype;
    m_identifier = old.m_identifier;
    m_encoding = old.m_encoding;
    m_description = old.m_description;
    m_lines = old.m_lines;
    m_size = old.m_size;
    m_Parameters = old.m_Parameters;
    m_poslist = old.m_poslist;
    // odebug << "RecPart copy constructor" << oendl; 
}

RecPart::~RecPart()
{
}

void RecPart::setSize(unsigned int size)
{
    m_size = size;
}

const unsigned int RecPart::Size()const
{
    return m_size;
}

void RecPart::setLines(unsigned int lines)
{
    m_lines = lines;
}

const unsigned int RecPart::Lines()const
{
    return m_lines;
}

const QString& RecPart::Type()const
{
    return m_type;
}

void RecPart::setType(const QString&type)
{
    m_type = type;
}

const QString& RecPart::Subtype()const
{
    return m_subtype;
}

void RecPart::setSubtype(const QString&subtype)
{
    m_subtype = subtype;
}

const QString& RecPart::Identifier()const
{
    return m_identifier;
}

void RecPart::setIdentifier(const QString&identifier)
{
    m_identifier = identifier;
}

const QString& RecPart::Encoding()const
{
    return m_encoding;
}

void RecPart::setEncoding(const QString&encoding)
{
    m_encoding = encoding;
}

const QString& RecPart::Description()const
{
    return m_description;
}

void RecPart::setDescription(const QString&desc)
{
    m_description = desc;
}

void RecPart::setParameters(const part_plist_t&list)
{
    m_Parameters = list;
}

const part_plist_t& RecPart::Parameters()const
{
    return m_Parameters;
}

void RecPart::addParameter(const QString&key,const QString&value)
{
    m_Parameters[key]=value;
}

const QString RecPart::searchParamter(const QString&key)const
{
    QString value("");
    part_plist_t::ConstIterator it = m_Parameters.find(key);
    if (it != m_Parameters.end()) {
        value = it.data();
    }
    return value;
}

void RecPart::setPositionlist(const QValueList<int>&poslist)
{
    m_poslist = poslist;
}

const QValueList<int>& RecPart::Positionlist()const
{
    return m_poslist;
}

RecBody::RecBody()
    : Opie::Core::ORefCount(),m_BodyText(),m_description(new RecPart())
{
    m_PartsList.clear();
}

RecBody::RecBody(const RecBody&old)
    :Opie::Core::ORefCount(),m_BodyText(),m_PartsList(),m_description(new RecPart())
{
    m_BodyText = old.m_BodyText;
    m_PartsList = old.m_PartsList;
    m_description = old.m_description;
    mCharset = old.mCharset;
    // odebug << "Recbody copy constructor" << oendl; 
}

RecBody::~RecBody()
{
}

void RecBody::setBodytext(const QString&bodyText)
{
    m_BodyText = bodyText;
}

const QString& RecBody::Bodytext()const
{
    return m_BodyText;
}

void RecBody::setParts(const QValueList<RecPartP>&parts)
{
    m_PartsList.clear();
    m_PartsList = parts;
}

const QValueList<RecPartP>& RecBody::Parts()const
{
    return m_PartsList;
}

void RecBody::addPart(const RecPartP& part)
{
    m_PartsList.append(part);
}

void RecBody::setDescription(const RecPartP&des)
{
    m_description = des;
}

const RecPartP& RecBody::Description()const
{
    return m_description;
}

void RecBody::setCharset(const QString& str)
{
    mCharset = str;
}

QString RecBody::getCharset()const
{
    return mCharset;
}

/* handling encoded content */
encodedString::encodedString()
{
    init();
}

encodedString::encodedString(const char*nContent,unsigned int nSize)
{
    init();
    setContent(nContent,nSize);
}

encodedString::encodedString(char*nContent,unsigned int nSize)
{
    init();
    setContent(nContent,nSize);
}

encodedString::encodedString(const encodedString&old)
{
    init();
    copy_old(old);
    // odebug << "encodedeString: copy constructor!" << oendl; 
}

encodedString& encodedString::operator=(const encodedString&old)
{
    init();
    copy_old(old);
    // odebug << "encodedString: assign operator!" << oendl; 
    return *this;
}

encodedString::~encodedString()
{
    clean();
}

void encodedString::init()
{
    content = 0;
    size = 0;
}

void encodedString::clean()
{
    if (content) {
        free(content);
    }
    content = 0;
    size = 0;
}

void encodedString::copy_old(const encodedString&old)
{
    clean();
    if (old.size>0 && old.content) {
        content = (char*)malloc(old.size*sizeof(char));
        memcpy(content,old.content,size);
        size = old.size;
    }
}

const char*encodedString::Content()const
{
    return content;
}

const int encodedString::Length()const
{
    return size;
}

void encodedString::setContent(const char*nContent,int nSize)
{
    if (nSize>0 && nContent) {
        content = (char*)malloc(nSize*sizeof(char));
        memcpy(content,nContent,nSize);
        size = nSize;
    }
}

void encodedString::setContent(char*nContent,int nSize)
{
    content = nContent;
    size = nSize;
}

folderStat&folderStat::operator=(const folderStat&old)
{
    message_count = old.message_count;
    message_unseen = old.message_unseen;
    message_recent = old.message_recent;
    return *this;
}

