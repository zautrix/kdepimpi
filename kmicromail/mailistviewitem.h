// CHANGED 2004-08-06 Lutz Rogowski
#ifndef __MAILLISTVIEWITEM_H
#define __MAILLISTVIEWITEM_H

#include <klistview.h>
#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/maildefines.h>

class MailListViewItem:public KListViewItem
{
public:
    MailListViewItem(QListView * parent, MailListViewItem * after );
    virtual ~MailListViewItem(){}

    void storeData(const RecMailP&data);
    const RecMailP&data()const;
    void showEntry();
    MAILLIB::ATYPE wrapperType();
    QString key(int column, bool) const;
    void setSortKey(int column,const QString &key);
protected:
    RecMailP mail_data;  
 private:
    QMap<int,QString> mKeyMap;
};

#endif
