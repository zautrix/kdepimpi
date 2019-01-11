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

$Id: getmasterpwwnd_emb.h,v 1.2 2004/09/25 01:22:55 ulf69 Exp $
*/                                                                      

#ifndef GETMASTERPWWND_EMB_H
#define GETMASTERPWWND_EMB_H

#include <kdialogbase.h>

class QLineEdit;

class getMasterPwWnd : public KDialogBase
{
    Q_OBJECT

public:
    getMasterPwWnd( QWidget* parent = 0, const char* name = 0);
    ~getMasterPwWnd();

    QLineEdit* pwLineEdit;

public slots:
    virtual void okButton_slot();
    virtual void cancelButton_slot();

    void add0();
    void add1();
    void add2();
    void add3();
    void add4();
    void add5();
    void add6();
    void add7();
    void add8();
    void add9();
    void backspace();
    void clear();

 private:
    void addCharacter(const QString &s);
};

#endif // GETMASTERPWWND_H
