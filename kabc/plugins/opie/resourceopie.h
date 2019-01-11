/*
    This file is part of libkabc.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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

$Id: resourceopie.h,v 1.5 2004/10/13 21:27:24 ulf69 Exp $
*/


#ifndef KABC_RESOURCEOPIE_H
#define KABC_RESOURCEOPIE_H

#include <kconfig.h>
#include <kdirwatch.h>

#include <sys/types.h>

#include "resource.h"


class OContactAccess;

namespace KABC {

class ResourceConfigWidget;
class OpieConverter;
/**
  @internal
*/
class ResourceOpie : public Resource
{
  Q_OBJECT

public:

  /**
    Constructor.

    @param cfg The config object where custom resource settings are stored.
  */
  ResourceOpie( const KConfig *cfg  );

  /**
    Construct file resource on file @arg fileName using format @arg formatName.
  */
  ResourceOpie( const QString &fileName  );

  /**
   * Destructor.
   */
  ~ResourceOpie();

  /**
    Writes the config back.
   */
  virtual void writeConfig( KConfig *cfg );

  /**
   * Tries to open the file and checks for the proper format.
   * This method should be called before @ref load().
   */
  virtual bool doOpen();

  /**
   * Closes the file again.
   */
  virtual void doClose();

  /**
   * Requests a save ticket, that is used by @ref save()
   */
  virtual Ticket *requestSaveTicket();

  /**
   * Loads all addressees from file to the address book.
   * Returns true if all addressees could be loaded otherwise false.
   */
  virtual bool load();

  /**
   * Saves all addresses from address book to file.
   * Returns true if all addressees could be saved otherwise false.
   *
   * @param ticket  The ticket returned by @ref requestSaveTicket()
   */
  virtual bool save( Ticket *ticket );


  /**
   * Remove a addressee from its source.
   * This method is mainly called by KABC::AddressBook.
   */
  virtual void removeAddressee( const Addressee& addr );

  /**
   * Set name of file to be used for saving.
   */
  virtual void setFileName( const QString & );

  /**
   * This method is called by an error handler if the application
   * crashed
   */
  virtual void cleanUp();


protected slots:
  void fileChanged();

protected:
  void init( const QString &fileName );

  bool lock( const QString &fileName );
  void unlock( const QString &fileName );

private:
  OContactAccess* mAccess;
  OpieConverter* mConverter;

  QString mLockUniqueName;

  KDirWatch mDirWatch;
};

}

#endif
