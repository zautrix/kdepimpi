/*
    This file is part of libkabc.
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

#include "vcardline.h"

using namespace KABC;

class VCardLine::VCardLinePrivate
{
  public:
    QString mGroup;
};

VCardLine::VCardLine()
  : d( 0 )
{
}

VCardLine::VCardLine( const QString &identifier )
  : d( 0 )
{
  mIdentifier = identifier;
}

VCardLine::VCardLine( const QString &identifier, const QString &value )
  : d( 0 )
{
  mIdentifier = identifier;
  mValue.assign( value.data(), value.length() );
}

VCardLine::VCardLine( const VCardLine& line )
  : d( 0 )
{
  mParamMap = line.mParamMap;
  mValue = line.mValue;
  mIdentifier = line.mIdentifier;
}

VCardLine::~VCardLine()
{
  delete d;
  d = 0;
}

VCardLine& VCardLine::operator=( const VCardLine& line )
{
  if ( &line == this )
    return *this;

  mParamMap = line.mParamMap;
  mValue = line.mValue;
  mIdentifier = line.mIdentifier;

  return *this;
}

void VCardLine::setIdentifier( const QString& identifier )
{
  mIdentifier = identifier;
}

QString VCardLine::identifier() const
{
  return mIdentifier;
}

void VCardLine::setValueString( const QString& value )
{
    setValueCString( value.utf8() );
}
void VCardLine::setValueCString( const QCString& value )
{
    mValue.duplicate(  value.data(), value.length() );
}

void VCardLine::setValueBytes( const QByteArray& value )
{
  mValue = value;
}

QString VCardLine::valueString() const
{
  return QString::fromUtf8( mValue.data(), mValue.size() );
}

QByteArray VCardLine::valueBytes() const
{
  return mValue;
}

void VCardLine::setGroup( const QString& group )
{
  if ( !d )
    d = new VCardLinePrivate();

  d->mGroup = group;
}

QString VCardLine::group() const
{
  if ( d )
    return d->mGroup;
  else
    return QString();
}

bool VCardLine::hasGroup() const
{
  if ( !d )
    return false;
  else
    return d->mGroup.isEmpty();
}

QStringList VCardLine::parameterList() const
{
  //return mParamMap.keys();
//US method QMap::keys() not available yet. SO collect the data manually
//US    return mParamMap->keys();

    QStringList result;

    QMap<QString, QStringList>::ConstIterator it;
    for( it = mParamMap.begin(); it != mParamMap.end(); ++it ) {
      result << it.key().latin1();
    }
    return result;
}

void VCardLine::addParameter( const QString& param, const QString& value )
{
  QStringList &list = mParamMap[ param ];
  if ( list.findIndex( value ) == -1 ) // not included yet
    list.append( value );
}

QStringList VCardLine::parameters( const QString& param ) const
{
  ParamMap::ConstIterator it = mParamMap.find( param );
  if ( it == mParamMap.end() )
    return QStringList();
  else
    return *it;
}

QString VCardLine::parameter( const QString& param ) const
{
  ParamMap::ConstIterator it = mParamMap.find( param );
  if ( it == mParamMap.end() )
    return QString::null;
  else {
    if ( (*it).isEmpty() )
      return QString::null;
    else
      return (*it).first();
  }
}
