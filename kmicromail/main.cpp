// CHANGED 2004-08-06 Lutz Rogowski


#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#include <libkdepim/externalapphandler.h>
#include <stdlib.h>
#else
#include <qapplication.h>
#include <qstring.h>
#include <qwindowsstyle.h> 
#include <qplatinumstyle.h> 
#include <qsgistyle.h> 
#endif
#include "opiemail.h"
#include <qdir.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <stdio.h>
#include "mainwindow.h"
#include "koprefs.h"
#include <libkdepim/kpimglobalprefs.h>
void dumpMissing();
//using namespace Opie::Core;
int main( int argc,  char **argv ) {  

#ifndef DESKTOP_VERSION
    QPEApplication a( argc, argv );
    a.setKeepRunning ();
#else
    QApplication a( argc, argv ); 
    QApplication::setStyle( new QPlatinumStyle ());
#endif
    //a.setFont( KOPrefs::instance()->mAppFont );
    KGlobal::setAppName( "kopiemail" );
    QString fileName ;
#ifndef DESKTOP_VERSION
    fileName = getenv("QPEDIR");
    if ( QApplication::desktop()->width() > 320 )
        KGlobal::iconLoader()->setIconPath( fileName +"/pics/kdepim/kopiemail/icons22/");
    else
        KGlobal::iconLoader()->setIconPath( fileName +"/pics/kdepim/kopiemail/");
#else
    fileName  = qApp->applicationDirPath () + "/kdepim/kopiemail/icons22/";
    KGlobal::iconLoader()->setIconPath(QDir::convertSeparators(fileName));
#endif 
    KStandardDirs::setAppDir( QDir::convertSeparators(locateLocal("data", "kopiemail")));
    KPimGlobalPrefs::instance()->setGlobalConfig();
    QApplication::setFont( KPimGlobalPrefs::instance()->mApplicationFont );
    QApplication::setFont( KOPrefs::instance()->mAppFont );
    OpieMail  mw;
#ifndef DESKTOP_VERSION
    //qDebug("CONNECT ");
    QObject::connect( &a, SIGNAL (appMessage ( const QCString &, const QByteArray & )),&mw, SLOT(message(  const QCString&, const QByteArray& ))); 
    // QObject::connect(&a, SIGNAL (appMessage ( const QCString &, const QByteArray & )), ExternalAppHandler::instance(), SLOT (appMessage ( const QCString &, const QByteArray & )));
    a.showMainWidget(&mw );
#else
    a.setMainWidget(&mw );
    mw.show();
    //m.resize( 800, 600 );
    QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
#endif
    int rv = a.exec(); 
    dumpMissing();

    KPimGlobalPrefs::instance()->writeConfig();
    return rv; 
   
}
