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

#ifndef KRESOURCES_CONFIGDIALOG_H
#define KRESOURCES_CONFIGDIALOG_H

#include <kdialogbase.h>

class KLineEdit;
class QCheckBox;
class KButtonBox;

namespace KRES {
  class Resource;
  class ConfigWidget;

class ConfigDialog : public KDialogBase
{
    Q_OBJECT
  public:
    // Resource=0: create new resource
    ConfigDialog( QWidget *parent, const QString& resourceFamily,
	          Resource* resource, const char *name = 0);

    void setInEditMode( bool value );

  protected slots:
    void accept();
    void setReadOnly( bool value );
    void setIncludeInSync( bool value );
    void setPersistentReadOnly( bool value );
    void slotNameChanged( const QString &text);

  private:
    ConfigWidget *mConfigWidget;
    Resource* mResource;

    KLineEdit *mName;
    QCheckBox *mReadOnly;
    QCheckBox *mIncludeInSync;
//US add a persistent readonly flag. We need that for opie and qtopia addressbooks.
    bool mPersistentReadOnly;
};

}

#endif
