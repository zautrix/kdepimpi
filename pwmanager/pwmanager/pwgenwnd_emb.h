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

$Id: pwgenwnd_emb.h,v 1.1 2004/09/23 19:35:13 ulf69 Exp $
*/                                                                      

#ifndef PWGENWND_EMB_H
#define PWGENWND_EMB_H

#include <kdialogbase.h>

class QLineEdit;
class QSpinBox;
class QCheckBox;

class pwGenWnd : public KDialogBase
{
    Q_OBJECT

public:
    pwGenWnd( QWidget* parent = 0, const char* name = 0);
    ~pwGenWnd();

    QCheckBox* int_charLowerCheckBox;
    QCheckBox* int_charUpperCheckBox;
    QCheckBox* int_charNumCheckBox;
    QCheckBox* int_charSpecCheckBox;
    QCheckBox* int_charBlankCheckBox;
    QCheckBox* int_charUserCheckBox;
    QLineEdit* int_userDefLineEdit;
    QSpinBox* int_lenSpinBox;
    QCheckBox* int_filterCheckBox;

public slots:
    virtual void cancelButton_slot();
    virtual void genButton_slot();

};

#endif // PWGENWND_H
