/*                                                                      
    This file is part of PwManager/Platform independent.                                  
    Copyright (c) 2004 Ulf Schenk
                                                                        
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

$Id: editcategory.h,v 1.1 2004/10/29 04:49:32 ulf69 Exp $
*/                                                                      

#ifndef EDITCATEGORY_H
#define EDITCATEGORY_H

#include <qvariant.h>
#include <kdialogbase.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class KLineEdit;
class QPushButton;
class KComboBox;
class QLabel;
class QGroupBox;
class QMultiLineEdit;
class PwMDoc;

class editCategoryWnd : public KDialogBase
{
    Q_OBJECT

public:
    editCategoryWnd( PwMDoc* doc, QWidget* parent = 0, const char* name = 0);
    ~editCategoryWnd();

    void setCurrCategory(const QString &cat);

    KComboBox* categoryComboBox;
    KLineEdit* descLineEdit;
    KLineEdit* usernameLineEdit;
    KLineEdit* pwLineEdit;

    //public slots:
    //    virtual void revealButton_slot();
    //    virtual void generateButton_slot();
    //    virtual void advancedCommentButton_slot(bool on);

  protected slots:
    virtual void slotOk();
    virtual void slotApply();
    virtual void cancel_slot();

    virtual void categorySelected ( const QString & string );
    virtual void widgetModified(const QString &);

 private:
  PwMDoc* doc;

};

#endif // EDITCATEGORY_H
