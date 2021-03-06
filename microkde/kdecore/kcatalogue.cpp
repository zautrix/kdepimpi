/* This file is part of the KDE libraries
   Copyright (c) 2001 Hans Petter Bieker <bieker@kde.org>

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

//US #include <config.h>

#include <qfile.h>

#include <kdebug.h>

#include "kcatalogue.h"

char *k_nl_find_msg(struct kde_loaded_l10nfile *domain_file,
	       const char *msgid);
void k_nl_unload_domain (struct loaded_domain *domain);

#ifndef KDE_USE_FINAL // with --enable-final, we're getting this from libintl.cpp
struct kde_loaded_l10nfile
{
  const char *filename;
  int decided;

  const void *data;

  kde_loaded_l10nfile() : filename(0), decided(0), data(0) {}
};
#endif

class KCataloguePrivate
{
public:
  QString name;

  kde_loaded_l10nfile domain;
};

KCatalogue::KCatalogue(const QString & name)
  : d( new KCataloguePrivate )
{
  d->name = name;
}

KCatalogue::KCatalogue(const KCatalogue & rhs)
  : d( new KCataloguePrivate )
{
  *this = rhs;
}

KCatalogue & KCatalogue::operator=(const KCatalogue & rhs)
{
  d->name = rhs.d->name;
  setFileName( rhs.fileName() );

  return *this;
}

KCatalogue::~KCatalogue()
{
  doUnload();

  delete d;
}

QString KCatalogue::name() const
{
  return d->name;
}

void KCatalogue::setFileName( const QString & fileName )
{
  // nothing to do if the file name is already the same
  if ( this->fileName() == fileName ) return;

  doUnload();

  QCString newFileName = QFile::encodeName( fileName );

  if ( !fileName.isEmpty() )
    {
      // set file name
      char *filename = new char[ newFileName.length() + 1 ];
      ::qstrcpy( filename, newFileName );
      d->domain.filename = filename;
    }
}

QString KCatalogue::fileName() const
{
  return QFile::decodeName( d->domain.filename );
}

const char * KCatalogue::translate(const char * msgid) const
{
    qDebug("KCatalogue::translate has to be fixed %s",msgid  );
//US  return ::k_nl_find_msg( &d->domain, msgid );
  return msgid;

}

void KCatalogue::doUnload()
{
  // use gettext's unloader
  if ( d->domain.data )
  {
//US    ::k_nl_unload_domain( (struct loaded_domain *)d->domain.data );
  qDebug("KCatalogue::doUnload has to be fixed" );
    
  }
  d->domain.data = 0;

  // free name
  delete [] const_cast<char *>(d->domain.filename);
  d->domain.filename = 0;

  d->domain.decided = 0;
}
