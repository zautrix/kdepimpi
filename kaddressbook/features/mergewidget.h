/*                                                                      
    This file is part of KAddressBook.                                  
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>                   
                                                                        
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

#ifndef MERGEWIDGET_H
#define MERGEWIDGET_H

#include <kdialogbase.h>
#include <klistview.h>

#include "extensionwidget.h"

class QListView;

class KABCore;

namespace KABC {
class AddressBook;
}

class MergeWidget : public ExtensionWidget
{
    Q_OBJECT

  public:
    MergeWidget( KABCore*, QWidget *parent, const char *name = 0 );
    virtual ~MergeWidget();

    void contactsSelectionChanged();

    QString title() const;
    QString identifier() const;

  private slots:
    void mergeAndRemove();
    void merge();

    void selectionContactViewChanged();

  private:
    void updateView();
    void doMerge( const KABC::Addressee &addr );

    KListView *mContactView;
    QPushButton *mMergeAndRemoveButton;
    QPushButton *mMergeButton;

    KABC::Addressee mMasterAddressee;
    bool mBlockUpdate;
};

#ifdef KAB_EMBEDDED
class MergeFactory : public ExtensionFactory
{
  public:
    ExtensionWidget *extension( KABCore *core, QWidget *parent, const char *name )
    {
      return new MergeWidget( core, parent, name );
    }

    QString identifier() const
    {
      return "merge";
    }
};
#endif //KAB_EMBEDDED

#endif
