/*                                                                      
    This file is part of KAddressBook.                                  
    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>                   
                                                                        
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

#include "addresseeconfig.h"
#include "kabprefs.h"
//US
#include <kstandarddirs.h>
#include <qfileinfo.h>

using namespace KABC;

static AddresseeConfig* extern_AddresseeConfig = 0; 
static  KConfig *  extern_Config = 0; 

AddresseeConfig::AddresseeConfig()
{
  

}
AddresseeConfig::~AddresseeConfig()
{
    delete AddresseeConfig::config();
}


AddresseeConfig* AddresseeConfig::instance()
{
    if ( ! extern_AddresseeConfig )
        extern_AddresseeConfig = new AddresseeConfig();
    return extern_AddresseeConfig;
}
KConfig* AddresseeConfig::config()
{
    if ( ! extern_Config ) {
        QString conf = locateLocal("config", "kaddressrc");
        extern_Config= new KConfig( conf );
    }
    return extern_Config;
}
  

void AddresseeConfig::setUid( const QString & uid )
{
    AddresseeConfig::config()->setGroup( uid );
    mUid = uid;
}

void AddresseeConfig::setAutomaticNameParsing( bool value )
{
    AddresseeConfig::config()->writeEntry( "AutomaticNameParsing", value );
}

 bool AddresseeConfig::automaticNameParsing()
{

    return AddresseeConfig::config()->readBoolEntry( "AutomaticNameParsing",
                               KABPrefs::instance()->mAutomaticNameParsing );
}

void AddresseeConfig::setNoDefaultAddrTypes( const QValueList<int> &types )
{
    AddresseeConfig::config()->writeEntry( "NoDefaultAddrTypes", types );
    AddresseeConfig::config()->sync();
}

 QValueList<int> AddresseeConfig::noDefaultAddrTypes() const
{
    return AddresseeConfig::config()->readIntListEntry( "NoDefaultAddrTypes" );
}

void AddresseeConfig::remove( const QString & uid )
{
    AddresseeConfig::config()->deleteGroup( uid );
}
