/*                                                                      
    This file is part of KAddressBook.                                  
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>                   
                                                                        
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or   
    (at your option) any later version.                                 
                                                                        
    This program is distributed in the hope that it will be useful,     
    but WITHOUT ANY WARRANTY; without even the implied warranty of      
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        
    GNU General Public License for more details.                        
                                                                        
    You should have received a copy of the GNU General Public License   
    along with this program; if not, write to the Free Software         
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           
                                                                        
    As a special exception, permission is given to link this program    
    with any edition of Qt, and distribute the resulting executable,    
    without including the source code for Qt in the source distribution.
*/                                                                      

#ifndef KADDRESSBOOKICONVIEW_H
#define KADDRESSBOOKICONVIEW_H

#include <qstring.h>
#ifndef KAB_EMBEDDED
#include <kiconview.h>
#else //KAB_EMBEDDED
#include <qiconview.h>
#include <qptrlist.h>
#include <klocale.h>
#endif //KAB_EMBEDDED
#include "kaddressbookview.h"

class QIconViewItem;
class KConfig;
class AddresseeIconView;
class AddresseeIconViewItem;
class QIconDragItem;
class KAddressBookIconView;

namespace KABC { class AddressBook; }

/** This is an example kaddressbook view that is implemented using
* KIconView. This view is not the most useful view, but it displays
* how simple implementing a new view can be.
*/
class KAddressBookIconView : public KAddressBookView
{
  Q_OBJECT
    
  public:
    KAddressBookIconView( KABC::AddressBook *ab, QWidget *parent,
                          const char *name = 0 );
    virtual ~KAddressBookIconView();
    
    virtual QStringList selectedUids();
    virtual QString type() const { return "Icon"; }
    void doSearch( const QString&  s ,KABC::Field *field );
    
    virtual void readConfig(KConfig *config);
    virtual void scrollUP();
    virtual void scrollDOWN();
    virtual void setFocusAV();
    
  public slots:
    void refresh(QString uid = QString::null);
#ifndef KAB_EMBEDDED
//MOC_SKIP_BEGIN
    void setSelected(QString uid = QString::null, bool selected = true);
//MOC_SKIP_END
#else //KAB_EMBEDDED
//US my MOC do not like default parameters ???
    void setSelected(QString uid, bool selected);
#endif //KAB_EMBEDDED
  
  protected slots:
    void addresseeExecuted(QIconViewItem *item);
    void addresseeSelected();
  
  private:
    AddresseeIconView *mIconView;
    QPtrList<AddresseeIconViewItem> mIconList;
};


#ifndef KAB_EMBEDDED
//MOC_SKIP_BEGIN
class AddresseeIconView : public KIconView
//MOC_SKIP_END
#else //KAB_EMBEDDED
class AddresseeIconView : public QIconView
#endif //KAB_EMBEDDED
{
  Q_OBJECT
  
  public:
    AddresseeIconView(QWidget *parent, const char *name);
    ~AddresseeIconView();
    
  signals:
    void addresseeDropped(QDropEvent *);
    void startAddresseeDrag();
  
  protected:
    virtual QDragObject *dragObject();
    
  protected slots:
    void itemDropped(QDropEvent *, const QValueList<QIconDragItem> &);
};

class IconViewFactory : public ViewFactory
{
  public:
    KAddressBookView *view( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new KAddressBookIconView( ab, parent, name );
    }

    QString type() const { return "Icon"; }

    QString description() const { return i18n( "Icons represent contacts. Very simple view." ); }
};
/*
extern "C" {
  void *init_libkaddrbk_iconview()
  {
    return ( new IconViewFactory );
  }
}
*/
#endif
