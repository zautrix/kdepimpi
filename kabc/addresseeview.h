/*
    This file is part of libkdepim.

    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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

#ifndef KPIM_ADDRESSEEVIEW_H
#define KPIM_ADDRESSEEVIEW_H

#include <kabc/addressee.h>
#include <kdialogbase.h>

//US #include <ktextbrowser.h>
#include <qtextbrowser.h>

namespace KABC {

//US class AddresseeView : public KTextBrowser
class AddresseeView : public QTextBrowser
{

  public:
    AddresseeView( QWidget *parent = 0, const char *name = 0 );

    /**
      Sets the addressee object. The addressee is displayed immediately.

      @param addr The addressee object.
     */
    void setAddressee( const KABC::Addressee& addr );
    void setSource(const QString& n);
    /**
      Returns the current addressee object.
     */
    //KABC::Addressee addressee() const;
    void printMe(); 
    static bool sFullDetailsMode;
  private: 
    Addressee mCurrentContact;
    //KABC::Addressee mAddressee;
    QString mText;
    QString getPhoneNumbers( KABC::PhoneNumber::List phones, bool preferred );
    void addTag(const QString & tag,const QString & text);
    //class AddresseeViewPrivate;
    //AddresseeViewPrivate *d;
};
class AddresseeChooser : public KDialogBase
{
    Q_OBJECT

  public:
    AddresseeChooser( KABC::Addressee loc, KABC::Addressee rem, bool takeloc, QWidget *parent = 0, const char *name = 0 );

    int executeD( bool local );

  private:
    int mSyncResult;

  private slots:
    void slot_remote();
    void slot_local();

};

}

#endif
