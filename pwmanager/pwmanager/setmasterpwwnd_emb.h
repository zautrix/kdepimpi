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

$Id: setmasterpwwnd_emb.h,v 1.1 2004/09/24 19:53:20 ulf69 Exp $
*/                                                                      

#ifndef SETMASTERPWWND_EMB_H
#define SETMASTERPWWND_EMB_H

#include <kdialogbase.h>

class QLineEdit;
class QPushButton;
class QLabel;
class QTabWidget;
class QWidget;

class setMasterPwWnd : public KDialogBase
{
    Q_OBJECT

public:
    setMasterPwWnd( QWidget* parent = 0, const char* name = 0 );
    ~setMasterPwWnd();

    QTabWidget* mainTab;
    QWidget* tab;
    QWidget* tab_2;
    QLineEdit* pwEdit_1;
    QLineEdit* pwEdit_2;
    QPushButton* selCardButton;
    QPushButton* genCardButton;
    QLabel* curCardIdLabel;

public slots:
    virtual void okButton_slot();
    virtual void cancelButton_slot();
    virtual void genCardButton_slot();
    virtual void selCardButton_slot();

};

#endif // SETMASTERPWWND_H
