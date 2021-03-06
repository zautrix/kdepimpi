#ifndef CUSTOMLISTVIEWITEM_H
#define CUSTOMLISTVIEWITEM_H

#include <qmap.h>
#include <qstring.h>
#include <klistview.h>

template<class T>
class CustomListViewItem : public KListViewItem
{
  public:
    CustomListViewItem( T data, KListView *parent ) :
      KListViewItem( parent ), mData( data ) { updateItem(); };
    ~CustomListViewItem() {};
    
    void updateItem() {};

    T data() const { return mData; }

    QString key(int column, bool) const
    {
      QMap<int,QString>::ConstIterator it = mKeyMap.find(column);
      if (it == mKeyMap.end()) return text(column);
      else return *it;
    }

    void setSortKey(int column,const QString &key)
    {
      mKeyMap.insert(column,key);
    }

  private:
    T mData;

    QMap<int,QString> mKeyMap;
};

#endif
