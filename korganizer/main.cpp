

#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#include <qcopchannel_qws.h> 
#include <qpe/global.h>
#include <stdlib.h>
#else
#include <qapplication.h>
#include <qstring.h>
#include <qwindowsstyle.h> 
#include <qplatinumstyle.h> 
#include <qsgistyle.h> 
#include <stdlib.h>
#endif
#include <qtextcodec.h> 

#include <qdir.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <stdio.h>
#include "mainwindow.h"
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
          printf("KO/Pi command line commands:\n");
          printf("  no command: Start KO/Pi in usual way\n");
          printf("  -help: This output\n");
          printf("Next Option: Open or Show after start:\n");
          printf("  -newTodo: New Todo dialog\n");
          printf("  -newEvent: New Event dialog\n");
          printf("  -showList: List view\n");
          printf("  -showDay: Day view\n");
          printf("  -showWWeek: Work Week view\n");
          printf("  -showWeek: Week view\n");
          printf("  -showTodo: Todo view\n");
          printf("  -showJournal: Journal view\n");
          printf("  -showKO: Next Days view\n");
          printf("  -showWNext: What's Next view\n");  
          printf("  -showNextXView: Next X View\n");
          printf(" -new[Y] and -show[X] may be used togehther\n");
          printf(" KO/Pi is exiting now. Bye!\n");
          exitHelp = true;
      }
  }
  if ( ! exitHelp ) {
      KGlobal::setAppName( "korganizer" );
      QString fileName ;
#ifndef DESKTOP_VERSION
      fileName = getenv("QPEDIR");
      KGlobal::iconLoader()->setIconPath( fileName +"/pics/kdepim/korganizer/");
#else
      fileName  = qApp->applicationDirPath () + "/kdepim/korganizer/";
      KGlobal::iconLoader()->setIconPath(QDir::convertSeparators(fileName));
#endif 
      KStandardDirs::setAppDir( QDir::convertSeparators(locateLocal("data", "korganizer")));

      QApplication::setFont( KPimGlobalPrefs::instance()->mApplicationFont );
      KPimGlobalPrefs::instance()->setGlobalConfig();
      MainWindow m;
#ifndef DESKTOP_VERSION
     
      QObject::connect( &a, SIGNAL (appMessage ( const QCString &, const QByteArray & )),&m, SLOT(receiveStart(  const QCString&, const QByteArray& )));
      a.showMainWidget(&m );
#else
      a.setMainWidget(&m );
      m.show();
      //m.resize( 800, 600 );
      //QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
#endif
      if ( argc > 1 ) {
          QCString command = argv[1];
          if ( argc > 2 )
              command += argv[2];
          m.recieve(command,  QByteArray() );
          
      }
     
#ifndef DESKTOP_VERSION
      // QPEApplication::setTempScreenSaverMode ( QPEApplication::Disable );
#endif
      a.exec();
      dumpMissing();

      KPimGlobalPrefs::instance()->writeConfig();
  }
  qDebug("KO: Bye! ");
}
