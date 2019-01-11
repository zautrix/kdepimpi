#ifndef KADDRESSBOOKCARDVIEW_H
#define KADDRESSBOOKCARDVIEW_H

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

#include <qstring.h>
#ifndef KAB_EMBEDDED
#include <kiconview.h>
#else //KAB_EMBEDDED
#include <klocale.h>
#endif //KAB_EMBEDDED

#include "cardview.h"
#include "kaddressbookview.h"
#include "configurecardviewdialog.h"

class QDragEnterEvent;
class QDragEntryEvent;
class QDropEvent;
class KConfig;
class AddresseeCardView;

/**
  This view uses the CardView class to create a card view. At some
  point in the future I think this will be the default view of
  KAddressBook.
 */
class KAddressBookCardView : public KAddressBookView
{
  Q_OBJECT
    
  public:
    KAddressBookCardView( KABC::AddressBook *ab, QWidget *parent,
                          const char *name = 0 );
    virtual ~KAddressBookCardView();
    void doSearch( const QString&  s,KABC::Field *field  );
    virtual QStringList selectedUids();
    virtual QString type() const { return "Card"; }
    
    virtual void readConfig(KConfig *config);
    virtual void writeConfig(KConfig *);
    virtual void scrollUP();
    virtual void scrollDOWN();
    virtual void setFocusAV();
    
  public slots:
    void refresh(QString uid = QString::null);
    void setSelected(QString uid/*US = QString::null*/, bool selected/*US = true*/);
//US added an additional method without parameter 
    void setSelected();
  
  protected slots:
    void addresseeExecuted(CardViewItem *item);
    void addresseeSelected();
  
  private:
    AddresseeCardView *mCardView;
    bool mShowEmptyFields;
};

class AddresseeCardView : public CardView
{
  Q_OBJECT
  public:
    AddresseeCardView(QWidget *parent, const char *name = 0);
    ~AddresseeCardView();
public slots:
    void printMe();
    
  signals:
    void startAddresseeDrag();
    void addresseeDropped(QDropEvent *);
    
  protected:
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void startDrag();
};


class CardViewFactory : public ViewFactory
{
  public:
    KAddressBookView *view( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new KAddressBookCardView( ab, parent, name );
    }

    QString type() const { return "Card"; }
    
    QString description() const { return i18n( "Rolodex style cards represent contacts." ); }
    
    ViewConfigureWidget *configureWidget( KABC::AddressBook *ab, QWidget *parent,
                                          const char *name = 0 )
    {
      return new ConfigureCardViewWidget( ab, parent, name );
    }
};


#endif
