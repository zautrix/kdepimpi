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
Copyright (c) 2004 Ulf Schenk changes 

$Id: sound.cpp,v 1.1.1.1 2004/06/26 19:01:39 zautrix Exp $
*/

#include "sound.h"

#include <qdatastream.h>

using namespace KABC;

Sound::Sound()
  : mIntern( false )
{
}

Sound::Sound( const QString &url )
  : mUrl( url ), mIntern( false )
{
}

Sound::Sound( const QByteArray &data )
  : mData( data ), mIntern( true )
{
}

Sound::~Sound()
{
}

bool Sound::operator==( const Sound &s ) const
{
  if ( mIntern != s.mIntern ) return false;

  if ( mIntern ) {
    if ( mData != s.mData )
      return false;
  } else {
    if ( mUrl != s.mUrl )
      return false;
  }

  return true;
}

bool Sound::operator!=( const Sound &s ) const
{
  return !( s == *this );
}

void Sound::setUrl( const QString &url )
{
  mUrl = url;
  mIntern = false;
}

void Sound::setData( const QByteArray &data )
{
  mData = data;
  mIntern = true;
}

bool Sound::isIntern() const
{
  return mIntern;
}

QString Sound::url() const
{
  return mUrl;
}

QByteArray Sound::data() const
{
  return mData;
}

QString Sound::asString() const
{
  if ( mIntern )
    return "intern sound";
  else
    return mUrl;
}

QDataStream &KABC::operator<<( QDataStream &s, const Sound &sound )
{
  return s << sound.mIntern << sound.mUrl << sound.mData;
}

QDataStream &KABC::operator>>( QDataStream &s, Sound &sound )
{
  s >> sound.mIntern >> sound.mUrl >> sound.mData;
  return s;
}
