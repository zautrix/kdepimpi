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

$Id: xxportobject.cpp,v 1.5 2004/10/29 11:24:14 zautrix Exp $
*/

#include <qmap.h>
#include <qfile.h>
#include <qsignalmapper.h>

#include <kaction.h>
#include <kmessagebox.h>
#include <factory.h>

#include "kabcore.h"
#include "addressee.h"
#include "tmpaddressbook.h"
#include "xxportobject.h"


class XXPortObject::XXPortObjectPrivate
{
  public:
    QSignalMapper *mExportMapper;
    QSignalMapper *mImportMapper;
};

XXPortObject::XXPortObject( KABC::AddressBook *ab, QWidget *parent,
                            const char *name )
  : QObject( parent, name ), mAddressBook( ab ), mParentWidget( parent ),
    d( new XXPortObjectPrivate )
{
  d->mExportMapper = new QSignalMapper( this );
  d->mImportMapper = new QSignalMapper( this );

  connect( d->mExportMapper, SIGNAL( mapped( const QString& ) ),
           SLOT( slotExportActivated( const QString& ) ) );
  connect( d->mImportMapper, SIGNAL( mapped( const QString& ) ),
           SLOT( slotImportActivated( const QString& ) ) );
}

XXPortObject::~XXPortObject()
{
  delete d;
  d = 0;
}

bool XXPortObject::exportContacts( const KABC::AddresseeList&, const QString& )
{
  // do nothing
  return false;
}

KABC::AddresseeList XXPortObject::importContacts( const QString& ) const
{
  // do nothing
  return KABC::AddresseeList();
}

void XXPortObject::createImportAction( const QString &label, const QString &data )
{
#ifdef KAB_EMBEDDED
  KABCore* kabcore = (KABCore*)mParentWidget;
#endif //KAB_EMBEDDED

  QString id = "file_import_" + identifier() + ( data.isEmpty() ? QString( "" ) : "_" + data );
#ifndef KAB_EMBEDDED
  KAction *action = new KAction( label, 0, d->mImportMapper, SLOT( map() ), actionCollection(), id.latin1() );
#else //KAB_EMBEDDED
  KAction *action = new KAction( label, 0, d->mImportMapper, SLOT( map() ), kabcore->actionCollection(), id.latin1() );
#endif //KAB_EMBEDDED

  d->mImportMapper->setMapping( action, ( data.isEmpty() ? QString( "<empty>" ) : data ) );

#ifndef KAB_EMBEDDED
  setXMLFile( identifier() + "_xxportui.rc" );
#endif //KAB_EMBEDDED

#ifdef KAB_EMBEDDED
  action->plug((QWidget*)kabcore->getImportMenu());
#endif //KAB_EMBEDDED

}

void XXPortObject::createExportAction( const QString &label, const QString &data )
{
#ifdef KAB_EMBEDDED
  KABCore* kabcore = (KABCore*)mParentWidget;
#endif //KAB_EMBEDDED

  QString id = "file_export_" + identifier() + ( data.isEmpty() ? QString( "" ) : "_" + data );
#ifndef KAB_EMBEDDED
  KAction *action = new KAction( label, 0, d->mExportMapper, SLOT( map() ), actionCollection(), id.latin1() );
#else //KAB_EMBEDDED
  KAction *action = new KAction( label, 0, d->mExportMapper, SLOT( map() ), kabcore->actionCollection(), id.latin1() );
#endif //KAB_EMBEDDED

  d->mExportMapper->setMapping( action, ( data.isEmpty() ? QString( "<empty>" ) : data ) );

#ifndef KAB_EMBEDDED
  setXMLFile( identifier() + "_xxportui.rc" );
#endif //KAB_EMBEDDED

#ifdef KAB_EMBEDDED
  action->plug((QWidget*)kabcore->getExportMenu());
#endif //KAB_EMBEDDED

}

KABC::AddressBook *XXPortObject::addressBook() const
{
  return mAddressBook;
}

QWidget *XXPortObject::parentWidget() const
{
  return mParentWidget;
}

void XXPortObject::slotExportActivated( const QString &data )
{
  emit exportActivated( identifier(), ( data == "<empty>" ? QString::null : data ) );
}

void XXPortObject::slotImportActivated( const QString &data )
{
  emit importActivated( identifier(), ( data == "<empty>" ? QString::null : data ) );
}

/********************************************************************
 *
 *******************************************************************/


XXPortResourceObject::XXPortResourceObject( KABC::AddressBook *ab, QWidget *parent,
                            const char *name )
  : XXPortObject( ab, parent, name ), mFactory(0)
{
  mFactory = KRES::Factory::self( "tmpcontact" );
}

XXPortResourceObject::~XXPortResourceObject()
{
  //do not delete the factory. It is a singleton, and selfcontained
}

bool XXPortResourceObject::isAvailable()
{
  return (mFactory != 0);
}


bool XXPortResourceObject::_exportContacts( const KABC::AddresseeList &list, const QString& identifier, const QString& fileName )
{
  //create new resource
    KABC::Resource* mResource = (KABC::Resource*)(mFactory->resource( this->identifier(), 0 ));

    if (mResource == 0)
        return false;

    if (fileName != QString::null) {
        mResource->setFileName( fileName );
    } else 
        return false;


    KABC::TmpAddressBook tmpAB;

    bool res = tmpAB.addResource( mResource );
    if (res == false)
        {
            delete mResource;
            return false;
        }
    if ( QFile::exists ( fileName ) ) {
        res = tmpAB.load();
        if (res == false)
            {
                //removeResource deletes also the resource object, if the linkcount is 0
                tmpAB.removeResource( mResource );
                return false;
            }


        //Now check if the file has already entries, and ask the user if he wants to delete them first.
        if (tmpAB.begin() != tmpAB.end())
            {
                QString text( i18n( "Do you want to remove<br>all existing entries from<br>%1<br>before exporting.?" ) );
                if ( KMessageBox::questionYesNo( parentWidget(), text.arg( fileName ) ) == KMessageBox::Yes ) {
                    // Clean the database..
                    tmpAB.clear();
                }
            }
    }

    KABC::Addressee::List::ConstIterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        tmpAB.insertAddressee(*it);
    }

    KABC::Ticket * ticket = tmpAB.requestSaveTicket(mResource);

  res = false;
  if (ticket != 0)
    res = tmpAB.save( ticket );

  //removeResource deletes also the resource object, if the linkcount is 0
  tmpAB.removeResource( mResource );

  return res;
}


KABC::AddresseeList XXPortResourceObject::_importContacts( const QString& identifier, const QString& fileName ) const
{


  KABC::Resource* mResource = (KABC::Resource*)(mFactory->resource( this->identifier(), 0 ));

  if (mResource == 0)
    return KABC::AddresseeList();

  KABC::AddresseeList adrlst;

  if (fileName != QString::null)
  	mResource->setFileName( fileName );


  KABC::TmpAddressBook tmpAB;

  bool res = tmpAB.addResource( mResource );
  if (res == false)
  {
	delete mResource;
	return KABC::AddresseeList();
  }

  res = tmpAB.load();

  adrlst = tmpAB.allAddressees();

  //remove resource deletes also the Resourceobject, if the linkcount is 0
  tmpAB.removeResource( mResource );

  return adrlst;
}



#ifndef KAB_EMBEDDED
#include "xxportobject.moc"
#endif //KAB_EMBEDDED
