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

$Id: findwnd_emb.h,v 1.1 2004/09/24 19:53:20 ulf69 Exp $
*/                                                                      


#ifndef FINDWND_EMB_H
#define FINDWND_EMB_H

#include <kdialogbase.h>


class QLineEdit;
class QCheckBox;
class QRadioButton;


class findWnd : public KDialogBase
{
    Q_OBJECT

public:
    findWnd( QWidget* parent = 0, const char* name = 0 );
    ~findWnd();

    QLineEdit* findLineEdit;
    QCheckBox* exactCheckBox;
    QCheckBox* caseSensCheckBox;
    QRadioButton* descRadioButton;
    QRadioButton* pwRadioButton;
    QRadioButton* commentRadioButton;
    QRadioButton* nameRadioButton;
    QRadioButton* urlRadioButton;
    QRadioButton* launcherRadioButton;

public slots:
    virtual void findButton_slot();
    virtual void selectionChanged_slot();
    virtual void closeButton_slot();
};

#endif // FINDWND_H
