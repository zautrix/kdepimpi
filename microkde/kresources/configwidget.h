/*
    This file is part of libkresources.
    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>
    Copyright (c) 2002 Jan-Pascal van Best <janpascal@vanbest.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef KRESOURCES_CONFIGWIDGET_H
#define KRESOURCES_CONFIGWIDGET_H

#include <qwidget.h>

#include <kconfig.h>

#include "resource.h"

namespace KRES {

class ConfigWidget : public QWidget
{
  Q_OBJECT

public:
  ConfigWidget( QWidget *parent = 0, const char *name = 0 );

  /**
    Sets the widget to 'edit' mode. Reimplement this method if you are
    interested in the mode change (to disable some GUI element for
    example). By default the widget is in 'create new' mode.
   */
  virtual void setInEditMode( bool value );

public slots:
  virtual void loadSettings( Resource *resource );
  virtual void saveSettings( Resource *resource );

signals:
  void setReadOnly( bool value );
  void setPersistentReadOnly( bool value );
  void setIncludeInSync(bool);

protected:
  Resource* mResource;
};

}
#endif
