/*
    This file is part of libkabc.
    Copyright (c) 2004 Ulf Schenk

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

$Id: resourceolaccess.cpp,v 1.3 2005/10/27 10:46:00 zautrix Exp $
*/

#include <sys/types.h>
#include <sys/stat.h>

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
//US #include <qtimer.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
//US #include <ksavefile.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>


#include <libkdepim/ksyncprofile.h>

#include "resourceolaccessconfig.h"
#include "resourceolaccess.h"

#include "stdaddressbook.h"

#include "olaccessconverter.h"
//#define ALLOW_LOCKING
using namespace KABC;
extern "C"
__declspec(dllexport)

  void *init_microkabc_olaccess()
  {
    return new KRES::PluginFactory<Resourceolaccess,ResourceolaccessConfig>();
  }


Resourceolaccess::Resourceolaccess( const KConfig *config  )
  : Resource( config ), mConverter (0)
{
  // we can not choose the filename. Therefore use the default to display
  //mAccess = 0;
  QString fileName;// = SlZDataBase::addressbookFileName();
    init( fileName );
}

Resourceolaccess::Resourceolaccess( const QString &fileName  )
  : Resource( 0 )
{
  //mAccess = 0;
    init( fileName );
}

void Resourceolaccess::init( const QString &fileName )
{
    if (mConverter == 0) {
        mConverter = new OlaccessConverter();
        bool res = mConverter->init(); 
        if ( !res )
            {
                QString msg("Unable to initialize sharp converter. Most likely a problem with the category file");
                qDebug(msg);
                return;
            }
    } 
    setFileName( fileName );
}

Resourceolaccess::~Resourceolaccess()
{
  if (mConverter != 0)
    delete mConverter;

  //if(mAccess != 0)
  //delete mAccess;
}

void Resourceolaccess::writeConfig( KConfig *config )
{
  Resource::writeConfig( config );
}

Ticket *Resourceolaccess::requestSaveTicket()
{

  qDebug("Resourceolaccess::requestSaveTicket: %s", fileName().latin1());

  if ( !addressBook() ) return 0;
  return createTicket( this );
}


bool Resourceolaccess::doOpen()
{ 
    if (!mConverter)
        return false;
    return true;
}

void Resourceolaccess::doClose()
{
  return;
}

bool Resourceolaccess::load()
{
    if (!mConverter)
        return false;
    QString fileN ;//= SlZDataBase::addressbookFileName();
#if 0
    if ( ! mAccess  ) {
        mAccess = new SlZDataBase(fileN,
                                  SlZDataBase::addressbookItems(),
                                  0, true);
    }
    if(mAccess == 0)
        return false;

    qDebug("Resourceolaccess:: %x load: %s",this, fileName().latin1());
    bool res = false;
    CardId id;
    for (bool res=mAccess->first(); res == true; res=mAccess->next())
        {
            id = mAccess->cardId();
            KABC::Addressee addressee;
            res = mConverter->sharpToAddressee( id, mAccess, addressee );
            if ( !addressee.isEmpty() && res )
                {
                    addressee.setResource( this );
                    addressBook()->insertAddressee( addressee );
                }
        }
    if(mAccess != 0)
        delete mAccess;
    mAccess = 0;
#endif
    return true;
}

bool Resourceolaccess::save( Ticket *ticket )
{  
#if 0
    if (!mConverter)
        return false;
    QString fileN = SlZDataBase::addressbookFileName();
    if ( ! mAccess  ) {
        mAccess = new SlZDataBase(fileN,
                                  SlZDataBase::addressbookItems(),
                                  0, false);
    }
    if(mAccess == 0)
        return false;
    qDebug("Resourceolaccess::save: %s", fileName().latin1());
    KABC::AddressBook::Iterator it;
    bool res;
    KABC::Addressee::List changedAddressees;
    typedef QMap<int,QString> AddresseeMap;
    AddresseeMap map;
    CardId id ;
    for ( it = addressBook()->begin(); it != addressBook()->end(); ++it ) {

        if ( (*it).tempSyncStat() != SYNC_TEMPSTATE_NEW_EXTERNAL ) {
            QString uid = (*it).originalExternalUID();
            bool res;
            if ( uid.isEmpty() )
                id = 0;
            else
                id = uid.toUInt();
            KABC::Addressee addressee = (*it);
            if ( (*it).tempSyncStat() == SYNC_TEMPSTATE_ADDED_EXTERNAL ) {
                res = mAccess->startEditCard(id);
                if (res == true)
                    {
                        res = mConverter->addresseeToSharp( (*it), mAccess, id  );
                        if (res == true)
                            {
                                res = mAccess->finishEditCard(&id);;
                                mConverter->setCategories( (*it), mAccess, id  );
                                map.insert(id,(*it).uid());                           
                                if (res == false)
                                    qDebug("Unable to append Contact: %s", addressee.formattedName().latin1());
                             
                            }
                        else
                            {
                                qDebug("Unable to convert Addressee: %s", addressee.formattedName().latin1());
                                mAccess->cancelEditCard();
                            }
                    }

            } else if ( (*it).tempSyncStat() == SYNC_TEMPSTATE_DELETE ) {
                res = mAccess->deleteCard(&id);
                if ( !res ) 
                    qDebug("delete error ");


            } else if ( (*it).tempSyncStat() != SYNC_TEMPSTATE_NEW_EXTERNAL ) {
                //changed
                res = mAccess->startEditCard(id);
                if (res == true)
                    {
                        res = mConverter->addresseeToSharp( (*it), mAccess, id  );
                        if (res == true)
                            {
                                res = mAccess->finishEditCard(&id);
                                mConverter->setCategories( (*it), mAccess, id  );
                                map.insert(id,(*it).uid());
                                if (res == false)
                                    qDebug("Unable to append Contact: %s", addressee.formattedName().latin1());         
                            }
                        else
                            {
                                qDebug("Unable to convert Addressee: %s", addressee.formattedName().latin1());
                                mAccess->cancelEditCard();
                            }
                    }
            }
        }

    }
    AddresseeMap::Iterator itam;
    for ( res=mAccess->first(); res == true; res=mAccess->next())
        {
            id = mAccess->cardId();
            int idint = id;
            itam = map.find( idint );
            if ( itam != map.end() ) {
                KABC::Addressee addressee;
                res = mConverter->sharpToAddressee( id, mAccess, addressee );

                if ( !addressee.isEmpty() && res )
                    {
                        addressee.setResource( this ); 
                        addressee.setUid( itam.data() );
                        addressee.setTempSyncStat( SYNC_TEMPSTATE_NEW_ID );
                        addressBook()->insertAddressee( addressee , false );
                    }
            }
        }
    delete ticket;
    if(mAccess != 0)
        delete mAccess;
    mAccess = 0;
#endif
    return true;
}

bool Resourceolaccess::lock( const QString &lockfileName )
{
    return true;
}

void Resourceolaccess::unlock( const QString &fileName )
{

}

void Resourceolaccess::setFileName( const QString &newFileName )
{
    Resource::setFileName( newFileName );
}

void Resourceolaccess::fileChanged()
{
 
}

void Resourceolaccess::removeAddressee( const Addressee &addr )
{
}

void Resourceolaccess::cleanUp()
{
 
}



