#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#include <qcopchannel_qws.h> 
#include <stdlib.h>
#else
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qplatinumstyle.h>
#include <qmainwindow.h>
#include <qmessagebox.h> 
#include <stdlib.h>
#endif

#include <qtextcodec.h> 
#include <kstandarddirs.h>
#include <qregexp.h>
#include <kglobal.h>
#include <stdio.h>
#include <qdir.h>
#include "kabprefs.h"
#include "kaddressbookmain.h"
#include "externalapphandler.h"
#include <libkdepim/kpimglobalprefs.h>
void dumpMissing();
int main( int argc, char **argv )
{
#ifndef DESKTOP_VERSION
  QPEApplication a( argc, argv );
  a.setKeepRunning ();
#else
  QApplication a( argc, argv );
  QApplication::setStyle( new QPlatinumStyle ());
#ifdef _WIN32_
  QString hdir ( getenv( "HOME") );
  if ( hdir.isEmpty() ) {
    QString hd ("C:/" );
    //QMessageBox::information(0,"hh",QDir::homeDirPath()+" xx" +hd );
    if ( QDir::homeDirPath().lower() == hd.lower() ) {
      _putenv( "HOME=C:");
      //QMessageBox::information(0,"hh",QString ( getenv( "HOME") ) );
    }
  } else {
    QDir app_dir;
    if ( !app_dir.exists(hdir) )
      app_dir.mkdir (hdir);
  }
#endif
#endif

  bool exitHelp = false;
  if ( argc > 1 ) {
      QString command = argv[1];
      if (  command == "-help" ){
          printf("KA/E command line commands:\n");
          printf("  no command: Start KA/E in usual way\n");
          printf("  -help: This output\n");
          printf(" KA/E is exiting now. Bye!\n");
          exitHelp = true;
      }
  }
  if ( ! exitHelp ) {

    KGlobal::setAppName( "kaddressbook" );
#ifndef DESKTOP_VERSION
    if ( QApplication::desktop()->width() > 320 )
        KGlobal::iconLoader()->setIconPath(QString(getenv("QPEDIR"))+"/pics/kdepim/kaddressbook/icons22/");
    else
        KGlobal::iconLoader()->setIconPath(QString(getenv("QPEDIR"))+"/pics/kdepim/kaddressbook/icons16/");
#else
      QString fileName ;
      fileName  = qApp->applicationDirPath () + "/kdepim/kaddressbook/icons22/";
      KGlobal::iconLoader()->setIconPath(QDir::convertSeparators(fileName));
      QApplication::addLibraryPath ( qApp->applicationDirPath () );

#endif
      KStandardDirs::setAppDir( QDir::convertSeparators(locateLocal("data", "kaddressbook")));
    // init language
      KPimGlobalPrefs::instance()->setGlobalConfig();
      QApplication::setFont( KPimGlobalPrefs::instance()->mApplicationFont );
       KAddressBookMain m ;
//US      MainWindow m;

      
      
#ifndef DESKTOP_VERSION
       a.showMainWidget( &m ); 
      
#else
      a.setMainWidget( &m );
      m.show();
#endif 
      m.setCaption(i18n("Loading addressbook data ... please wait" )); 
      a.exec();
#ifdef DESKTOP_VERSION
  KConfig *config = KABPrefs::instance()->getConfig();
  config->setGroup("WidgetLayout");
  QStringList list ;//= config->readListEntry("MainLayout");
  int x,y,w,h;
  QWidget* wid;
  wid = &m;
  x = wid->geometry().x();
  y = wid->geometry().y();
  w = wid->width();
  h = wid->height();
  list.clear();
  list << QString::number( x );
  list << QString::number( y );
  list << QString::number( w );
  list << QString::number( h );
  config->writeEntry("MainLayout",list );
  config->sync();
#endif
  dumpMissing();
  
  KPimGlobalPrefs::instance()->writeConfig();
  }
  qDebug("KA: Bye! ");
}

