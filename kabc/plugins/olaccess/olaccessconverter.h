/*
    This file is part of libkabc.
    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>

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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk
$Id: olaccessconverter.h,v 1.2 2005/10/27 10:34:57 zautrix Exp $
*/

#ifndef KABC_SHARPDTMCONVERTER_H
#define KABC_SHARPDTMCONVERTER_H

#include <qstring.h>


#include "addressee.h"


namespace KABC {

class OlaccessConverter
{
public:

  /**
   * Constructor.
   */
  OlaccessConverter();

  /**
   * Destructor.
   */
  virtual ~OlaccessConverter();

  bool init();
  void deinit();

  /**
   * Converts a given sharp card to an addressee.
   *
   * @param contact The sharp card id, eventhough it might be 0, if the contact is a new one.
   * @param database The sharp database pointer we use for the conversion
   * @param addr    The addressee.
   */
  //bool sharpToAddressee( const CardId &contact, const SlZDataBase* database, Addressee &addr );

  /**
   * Converts an addressee to a sharp dtm contact.
   *
   * @param addr    The addressee.
   * @param database The sharp database pointer we use for the conversion
   * @param contact The sharp card id, eventhough it might be 0, if the contact is a new one.
   */
  //bool addresseeToSharp( const Addressee &addr, SlZDataBase* database , const CardId &contact );
  //bool setCategories( const Addressee &addr, SlZDataBase* database , const CardId &contact );

  private:
  //SlCategory::SlCategories* catDB;
    QDate convertDate( QString );


};

}
#endif
