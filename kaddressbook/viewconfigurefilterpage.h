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

#ifndef VIEWCONFIGUREFILTERPAGE
#define VIEWCONFIGUREFILTERPAGE

#include <qwidget.h>

class QButtonGroup;

#ifndef KAB_EMBEDDED
class KComboBox;
#else //KAB_EMBEDDED
class QComboBox;
#endif //KAB_EMBEDDED

class KConfig;

class ViewConfigureFilterPage : public QWidget
{
  Q_OBJECT
  
  public:
    ViewConfigureFilterPage( QWidget *parent, const char *name = 0 );
    ~ViewConfigureFilterPage();
    
    void restoreSettings( KConfig* );
    void saveSettings( KConfig* );
    
  protected slots:
    void buttonClicked( int id );
    
  private:
#ifndef KAB_EMBEDDED
    KComboBox *mFilterCombo;
#else //KAB_EMBEDDED
    QComboBox *mFilterCombo;
#endif //KAB_EMBEDDED
  
    QButtonGroup *mFilterGroup;
};

#endif
