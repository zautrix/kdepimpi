// CHANGED 2004-08-06 Lutz Rogowski
#include "mailistviewitem.h"
#include <libmailwrapper/abstractmail.h>
#include <qtextstream.h>
#include <kiconloader.h>
#include "koprefs.h"
//#include <qpe/resource.h>

MailListViewItem::MailListViewItem(QListView * parent, MailListViewItem * item )
        :KListViewItem(parent,item),mail_data()
{
}

void MailListViewItem::showEntry()
{
    if ( mail_data->getFlags().testBit( FLAG_ANSWERED ) == true) {
        setPixmap( 0, SmallIcon ( "kmmsgreplied") );
        mKeyMap.insert(0, "r" );
    } else if ( mail_data->getFlags().testBit( FLAG_SEEN ) == true )  {
        /* I think it looks nicer if there are not such a log of icons but only on mails
           replied or new - Alwin*/ 
        //setPixmap( 0,SmallIcon ("kmmsgunseen") );
        mKeyMap.insert(0, "s" );
    } else  {
        setPixmap( 0,SmallIcon ( "kmmsgnew") );
        mKeyMap.insert(0, "u" );
    }
    QString fsize = mail_data->MsgsizeString();
    //  1.23
    // 11.23
    // 111.23
    // 999.23 maxlen
    QString fsort; 
    switch(fsize.length() ) {
    case 6:
        fsort = "00" + fsize ;
        break;
    case 7:
        fsort = "0" + fsize ;
        break;
    default:
        fsort = fsize ;
    break;
    
    }
    
    setText(3, fsize );
    //qDebug("fsize *%s* ",fsize.latin1() );
    //qDebug("fsort *%s* ",fsort.latin1() );
    if ( fsize.right(2) == "kB" ) {
        mKeyMap.insert(3, "k" + fsort);
    } else {
        mKeyMap.insert(3, "M" +fsort );
    }
    setText(1,mail_data->getSubject());
    setText(2,mail_data->getFrom());
    mKeyMap.insert(4,mail_data->getIsoDate());
    setText(4," "+mail_data->getDate());
    if ( KOPrefs::instance()->mShowToField )
        setText(5,mail_data->To()[0]);
}

void MailListViewItem::storeData(const RecMailP&data)
{
    mail_data = data;
}
void MailListViewItem::setSortKey(int column,const QString &key)
{
    mKeyMap.insert(column,key);
}
QString MailListViewItem::key(int column, bool) const
{
    // to make is fast, we use here special cases
    if ( column == 3  ||  column == 4 || column == 0) {
        return *mKeyMap.find(column);
    }
    if ( column == 1 ) {
        if ( text(1).left(4).lower() == "re: " )
            return text(1).mid(4);

    }
    return text(column);
    /*
    QMap<int,QString>::ConstIterator it = mKeyMap.find(column);
    if (it == mKeyMap.end()) return text(column);
    else return *it;
    */
}

const RecMailP& MailListViewItem::data()const
{
    return mail_data;
}

MAILLIB::ATYPE MailListViewItem::wrapperType()
{
    if (!mail_data->Wrapper()) return MAILLIB::A_UNDEFINED;
    return mail_data->Wrapper()->getType();
}
