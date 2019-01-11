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

$Id: opieconverter.h,v 1.1 2004/07/26 22:10:28 ulf69 Exp $
*/

#ifndef KABC_OPIECONVERTER_H
#define KABC_OPIECONVERTER_H

#include <qstring.h>

#include "addressee.h"
#include <opie/ocontact.h>

class Categories;

namespace KABC {

class OpieConverter
{
public:

  /**
   * Constructor.
   */
  OpieConverter();

  /**
   * Destructor.
   */
  virtual ~OpieConverter();
  
  bool init();
  void deinit();
  
  /**
   * Converts a vcard string to an addressee.
   *
   * @param contact The opie contact.
   * @param addr    The addressee.
   */
  bool opieToAddressee( const OContact &contact, Addressee &addr );

  /**
   * Converts an addressee to a vcard string.
   *
   * @param addr    The addressee.
   * @param contact The opie contact.
   */
  bool addresseeToOpie( const Addressee &addr, OContact &contact );

  private:
    Categories* catDB;
  
    
};

}
#endif
