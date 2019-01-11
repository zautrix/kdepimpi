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

$Id: picture.cpp,v 1.2 2005/01/14 11:38:15 zautrix Exp $
*/

#include "picture.h"

using namespace KABC;

Picture::Picture()
  : mIntern( false )
{
  mUndefined = true;
}

Picture::Picture( const QString &url )
  : mUrl( url ), mIntern( false )
{
  mUndefined = false;
}

Picture::Picture( const QImage &data )
  : mData( data ), mIntern( true )
{
  mUndefined = false;
}

Picture::~Picture()
{
}

bool Picture::operator==( const Picture &p ) const
{
    //qDebug("compare PIC ");
    if ( mUndefined && p.mUndefined ) {
        //qDebug("compare PIC true 1 ");
        return true;
    }
    if ( mUndefined || p.mUndefined ) {
        //qDebug("compare PIC false 1");
         return false;
    }
    // now we should deal with two defined pics!
    if ( mIntern != p.mIntern ) {
        //qDebug("compare PIC false 2");
        return false;
    }
  if ( mIntern ) {
      //qDebug("mIntern ");
      if ( mData.isNull() && p.mData.isNull() ) {
          //qDebug("compare PIC true 2 ");
          return true;
      }
      if ( mData.isNull() || p.mData.isNull() ){
          //qDebug("compare PIC false 3-1");

          return false;
      }
      if ( mData != p.mData ) {
          //qDebug("compare PIC false 3");
      return false;
      }
  } else {
      if ( mUrl != p.mUrl ) {
          //qDebug("compare PIC false 4");
          return false;
      }
  }
  //qDebug("compare PIC true ");
  return true;
}

bool Picture::operator!=( const Picture &p ) const
{
  return !( p == *this );
}

void Picture::setUrl( const QString &url )
{
  mUrl = url;
  mIntern = false;
  mUndefined = false;
}

void Picture::setData( const QImage &data )
{
  mData = data;
  mIntern = true;
  mUndefined = false;
}

void Picture::setType( const QString &type )
{
  mType = type;
}

bool Picture::isIntern() const
{
  return mIntern;
}

QString Picture::url() const
{
  return mUrl;
}

QImage Picture::data() const
{
  return mData;
}
QPixmap Picture::pixmap() const
{
    QPixmap p;
    p.convertFromImage ( mData );
    return p;
}

QString Picture::type() const
{
  return mType;
}
bool Picture::undefined() const
{
  return mUndefined;
}
 

QString Picture::asString() const
{
  if ( mIntern )
    return "intern picture";
  else
    return mUrl;
}

QDataStream &KABC::operator<<( QDataStream &s, const Picture &picture )
{
  return s << picture.mIntern << picture.mUrl << picture.mType << picture.mData;
}

QDataStream &KABC::operator>>( QDataStream &s, Picture &picture )
{
  s >> picture.mIntern >> picture.mUrl >> picture.mType >> picture.mData;
  return s;
}
