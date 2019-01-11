/*******************************************************************************
 * main.cpp                                                                    *
 *                                                                             *
 * 
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify        *
 * it under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with this program; if not, write to the Free Software                 *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
 *                                                                             *
 * As a special exception, permission is given to link this program            *
 * with any edition of Qt, and distribute the resulting executable,            *
 * without including the source code for Qt in the source distribution.        *
 *                                                                             *
 ******************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <kabc/addressbook.h>
#include <kabc/stdaddressbook.h>
#include <kabc/resource.h>
#include <kabc/vcardconverter.h>

#include <qdatetime.h>
#include <qfile.h>
#include <qdir.h>
#include <qapplication.h>

#include <stdlib.h>
#include <iostream>

using namespace std;

static const char progName[] = "kdecalendar";
static const char progDisplay[] = "KDE_Addressbook";
static const char progVersion[] = "33.1/3";
static const char progDesc[] = "A command line interface to KDE addressbooks";


static KCmdLineOptions options[] =
    {
        { "dump",
          I18N_NOOP( "Dumps addressbook" ), 0 },
        { "read",
          I18N_NOOP( "Reads addressbook" ), 0 },
        KCmdLineLastOption
    };

int main( int argc, char *argv[] )
{
    KAboutData aboutData(
                         progName,                        // internal program name
                         I18N_NOOP( progDisplay ),        // displayable program name.
                         progVersion,                     // version string
                         I18N_NOOP( progDesc ),           // short porgram description
                         KAboutData::License_GPL,         // license type
                         "(c) 2004, Lutz Rogowski", // copyright statement
                         0,                               // any free form text
                         "",                         // program home page address
                         "bugs.kde.org"                   // bug report email address
                         );


    // KCmdLineArgs::init() final 'true' argument indicates no commandline options
    // for QApplication/KApplication (no KDE or Qt options)
    KCmdLineArgs::init( argc, argv, &aboutData, true );
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

    KInstance ins  ( progName );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    bool read = false;
    if ( args->isSet( "read" ) ) {
        read = true;
        qDebug("read ");
    }
    QString fileName = QDir::homeDirPath ()+"/.kdeaddressbookdump.vcf";

    KABC::StdAddressBook* standardAddressBook = KABC::StdAddressBook::self();
    standardAddressBook->setAutomaticSave( false );
    qDebug("************************************* ");
    qDebug("***************kdeABdump************* ");
    qDebug("************************************* ");
    if ( !read ) {
        KABC::AddressBook::Iterator it;
        KABC::VCardConverter converter;
        QString datastream;
        for( it = standardAddressBook->begin(); it != standardAddressBook->end(); ++it ) { 
            // if ( (*it).isEmpty() || ! (*it).resource() )
             if ( (*it).isEmpty()  )
                continue;
            KABC::Addressee a = ( *it );
            QString vcard = converter.createVCard( a );
            vcard += QString("\r\n");
            datastream += vcard;
        }
         QFile outFile(fileName);
         if ( outFile.open(IO_WriteOnly) ) { 
             QTextStream t( &outFile );  // use a text stream
             t.setEncoding( QTextStream::UnicodeUTF8 );
             t <<datastream;
             t <<  "\r\n\r\n";
             outFile.close();
         }
    } else {
        //Addressee::List aList;//parseVCards( const QString& vcard );
        KABC::Addressee::List list;
        int added = 0, changedC = 0, deleted = 0;
        QFile file( fileName );
        if ( file.open( IO_ReadOnly ) ) {
            QTextStream t( &file );  // use a text stream
            t.setEncoding( QTextStream::UnicodeUTF8 );
            QString data;
            data = t.read();
            file.close();
            KABC::VCardConverter converter;
            list = converter.parseVCards( data );
            qDebug("kdeABdump::file has %d entries", list.count());

            KABC::Addressee::List::Iterator it;
            for ( it = list.begin();it != list.end();++it) {
                (*it).setChanged( true ); 
                bool changed = ((*it).custom( "KADDRESSBOOK", "X-ExternalID" ) == "changed");
                (*it).removeCustom( "KADDRESSBOOK", "X-ExternalID" );
                //qDebug("ext %s ", (*it).custom( "KADDRESSBOOK", "X-ExternalID" ).latin1());
                if ( changed ) {
                    //qDebug("changed Addressee found! ");
                    KABC::Addressee std = standardAddressBook->findByUid( (*it).uid() );
                    if ( ! std.isEmpty() )
                        (*it).setResource(std.resource());
                    standardAddressBook->insertAddressee( (*it) );
                    ++changedC;
                } else {
                    //maybe added?
                    KABC::Addressee std = standardAddressBook->findByUid( (*it).uid() );
                    if (  std.isEmpty() ) {
                        standardAddressBook->insertAddressee( (*it) );
                        ++added;
                    }
                }
            }
            KABC::AddressBook::Iterator itA = standardAddressBook->begin();
            KABC::AddressBook::Iterator it2 ;
            while ( itA != standardAddressBook->end() ) {
                bool found = false;
                KABC::Addressee::List::Iterator itL;
                for ( itL = list.begin();itL != list.end();++itL) {
                    if ( (*itL).uid() == (*itA).uid() ) {
                        found = true;
                        break;
                    }
                }
                if ( !found ) {
                    it2 = itA;
                    ++itA;
                    standardAddressBook->removeAddressee( it2 );
                    ++deleted;
                } else  {
                    ++itA;
                }
            }

            //standardAddressBook->saveAll();
            standardAddressBook->setAutomaticSave( true );
            qDebug("************************************* ");
            qDebug("*************kdeABdump*************** ");
            qDebug("************************************* ");
            qDebug("Addressbook entries\nchanged %d\ndeleted %d\nadded %d\nfrom file %s", changedC,deleted, added, fileName.latin1());
        } else
            qDebug("error open file ");
    }
    standardAddressBook->close();
    // line not needed by KDE 3.4:
    //    delete standardAddressBook;


    //KABC::StdAddressBook::close();
    //StdAddressBook::mSelf = 0;
    qDebug("ente ");
    return 0;
}
