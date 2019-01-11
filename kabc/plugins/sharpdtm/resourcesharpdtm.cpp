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

$Id: resourcesharpdtm.cpp,v 1.19 2005/01/26 01:12:58 zautrix Exp $
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

#include <sl/slzdb.h>

#include <libkdepim/ksyncprofile.h>

#include "resourcesharpdtmconfig.h"
#include "resourcesharpdtm.h"

#include "stdaddressbook.h"

#include "sharpdtmconverter.h"
//#define ALLOW_LOCKING
using namespace KABC;
extern "C"
{
  void *init_microkabc_sharpdtm()
  {
    return new KRES::PluginFactory<ResourceSharpDTM,ResourceSharpDTMConfig>();
  }
}

ResourceSharpDTM::ResourceSharpDTM( const KConfig *config  )
  : Resource( config ), mConverter (0)
{
  // we can not choose the filename. Therefore use the default to display
    mAccess = 0;
    QString fileName = SlZDataBase::addressbookFileName();
    init( fileName );
}

ResourceSharpDTM::ResourceSharpDTM( const QString &fileName  )
  : Resource( 0 )
{
    mAccess = 0;
    init( fileName );
}

void ResourceSharpDTM::init( const QString &fileName )
{
    if (mConverter == 0) {
        mConverter = new SharpDTMConverter();
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

ResourceSharpDTM::~ResourceSharpDTM()
{
  if (mConverter != 0)
    delete mConverter;

  if(mAccess != 0)
    delete mAccess;
}

void ResourceSharpDTM::writeConfig( KConfig *config )
{
  Resource::writeConfig( config );
}

Ticket *ResourceSharpDTM::requestSaveTicket()
{

  qDebug("ResourceSharpDTM::requestSaveTicket: %s", fileName().latin1());

  if ( !addressBook() ) return 0;
  return createTicket( this );
}


bool ResourceSharpDTM::doOpen()
{ 
    if (!mConverter)
        return false;
    return true;
}

void ResourceSharpDTM::doClose()
{
  return;
}

bool ResourceSharpDTM::load()
{
    if (!mConverter)
        return false;
    QString fileN = SlZDataBase::addressbookFileName();
    if ( ! mAccess  ) {
        mAccess = new SlZDataBase(fileN,
                                  SlZDataBase::addressbookItems(),
                                  0, true);
    }
    if(mAccess == 0)
        return false;

    qDebug("ResourceSharpDTM:: %x load: %s",this, fileName().latin1());
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

    return true;
}

bool ResourceSharpDTM::save( Ticket *ticket )
{  
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
    qDebug("ResourceSharpDTM::save: %s", fileName().latin1());
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
    return true;
}

bool ResourceSharpDTM::lock( const QString &lockfileName )
{
    return true;
}

void ResourceSharpDTM::unlock( const QString &fileName )
{

}

void ResourceSharpDTM::setFileName( const QString &newFileName )
{
    Resource::setFileName( newFileName );
}

void ResourceSharpDTM::fileChanged()
{
 
}

void ResourceSharpDTM::removeAddressee( const Addressee &addr )
{
}

void ResourceSharpDTM::cleanUp()
{
 
}



