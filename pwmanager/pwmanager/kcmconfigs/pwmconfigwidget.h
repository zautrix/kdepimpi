/*
    This file is part of PwManager/Pi.
    Copyright (c) 2004 ulf Schenk 

    program is free software; you can redistribute it and/or modify
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

    $Id: pwmconfigwidget.h,v 1.2 2004/10/16 01:02:53 ulf69 Exp $
*/

#ifndef PWMCONFIGWIDGET_H
#define PWMCONFIGWIDGET_H

#include <kprefswidget.h>

class PWMPrefs;
class KURLRequester;
class QComboBox;


class PWMConfigWidget : public KPrefsWidget
{
  Q_OBJECT

  public:
    PWMConfigWidget(PWMPrefs *prefs, QWidget *parent, const char *name = 0 );

  protected:
    /** Implement this to read custom configuration widgets. */
  virtual void usrReadConfig();
    /** Implement this to write custom configuration widgets. */
  virtual void usrWriteConfig();

  private:
    QLineEdit* permissionLineEdit;
    QSpinBox* pwTimeoutSpinBox;
    QSpinBox* lockTimeoutSpinBox;
    KURLRequester* autostartLineEdit;
    QLineEdit* browserLineEdit;
    QLineEdit* xtermLineEdit;
    QComboBox* kcfg_compression;
    QComboBox* kcfg_cryptAlgo;
    QComboBox* kcfg_hashAlgo;

    
 private:
  int getFilePermissions();
  void setFilePermissions(int perm);

};

#endif
