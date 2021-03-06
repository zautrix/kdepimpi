/*
    This file is part of KAddressbook.
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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk

$Id: kde2_xxport.cpp,v 1.3 2004/08/02 18:22:39 ulf69 Exp $
*/

#include <qfile.h>

#include <kdebug.h>
#include <kfiledialog.h>
#ifndef KAB_EMBEDDED
#include <kio/netaccess.h>
#endif //KAB_EMBEDDED
#include <klocale.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kurl.h>

#include "xxportmanager.h"

#include "kde2_xxport.h"

#ifndef KAB_EMBEDDED

class KDE2XXPortFactory : public XXPortFactory
{
  public:
    XXPortObject *xxportObject( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new KDE2XXPort( ab, parent, name );
    }
};

#endif //KAB_EMBEDDED


extern "C"
{
#ifndef KAB_EMBEDDED
  void *init_libkaddrbk_kde2_xxport()
#else //KAB_EMBEDDED
  void *init_microkaddrbk_kde2_xxport()
#endif //KAB_EMBEDDED
  {
    return ( new KDE2XXPortFactory() );
  }
}


KDE2XXPort::KDE2XXPort( KABC::AddressBook *ab, QWidget *parent, const char *name )
  : XXPortObject( ab, parent, name )
{
  createImportAction( i18n( "Import KDE 2 Addressbook..." ) );
}

KABC::AddresseeList KDE2XXPort::importContacts( const QString& ) const
{
  QString fileName = locateLocal( "data", "kabc/std.vcf" );
  if ( !QFile::exists( fileName ) ) {
    KMessageBox::sorry( parentWidget(), i18n( "<qt>Couldn't find a KDE 2 address book <b>%1</b>.</qt>" ).arg( fileName ) );
    return KABC::AddresseeList();
  }

#ifndef KAB_EMBEDDED
  int result = KMessageBox::questionYesNoCancel( parentWidget(),
      i18n( "Override previously imported entries?" ),
      i18n( "Import KDE 2 Addressbook" ) );
#else //KAB_EMBEDDED
  int result = KMessageBox::warningYesNoCancel( parentWidget(),
      i18n( "Override previously imported entries?" ),
      i18n( "Import KDE 2 Addressbook" ) );
#endif //KAB_EMBEDDED

  if ( !result ) return KABC::AddresseeList();

  KProcess proc;

  if ( result == KMessageBox::Yes ) {
    proc << "kab2kabc";
    proc << "--override";
  } else if ( result == KMessageBox::No )
    proc << "kab2kabc";
  else
    kdDebug(5720) << "KAddressBook::importKDE2(): Unknow return value." << endl;

  proc.start( KProcess::Block );

  addressBook()->load();

  return KABC::AddresseeList();
}

#ifndef KAB_EMBEDDED
#include "kde2_xxport.moc"
#endif //KAB_EMBEDDED
