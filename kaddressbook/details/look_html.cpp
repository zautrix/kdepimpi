/*
    This file is part of KAddressBook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

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

#include <addresseeview.h>

#include "look_html.h"
#include <qscrollview.h> 
#include "kabprefs.h"
#include <kabc/addresseeview.h>
KABHtmlView::KABHtmlView( QWidget *parent, const char *name )
  : KABBasicLook( parent, name )
{
    mView = new KABC::AddresseeView( this ); 
    mView->setFont( KABPrefs::instance()->mDetailsFont );
    connect(this, SIGNAL(printMyView()),
          this , SLOT(printMe()));
}

KABHtmlView::~KABHtmlView()
{
}
void KABHtmlView::printMe()
{
    mView->printMe();

}

void KABHtmlView::setAddressee( const KABC::Addressee &addr )
{
  mView->setFont( KABPrefs::instance()->mDetailsFont );
  mView->setAddressee( addr );
}

#ifndef KAB_EMBEDDED
#include "look_html.moc"
#endif //KAB_EMBEDDED
