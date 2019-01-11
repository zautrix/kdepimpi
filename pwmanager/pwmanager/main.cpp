/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003, 2004 by Michael Buesch                            *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * copyright (C) 2004 by Ulf Schenk
 * This file is originaly based on version 1.0.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: main.cpp,v 1.11 2008/10/02 20:37:12 zautrix Exp $
 **************************************************************************/  

#ifndef PWM_EMBEDDED
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#else
#include <qdir.h>
#include <kpimglobalprefs.h>
#endif
#include <stdlib.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include "pwmexception.h"
#include "pwminit.h"
void dumpMissing();

#define LICENSE_FILE	(::locate("data", "pwmanager/pwmanager_license_text"))

int PwMApplication::newInstance()
{
	static bool initial = true;
	if (initial) {
		initial = false;
		init = new PwMInit(this);
		init->initializeApp();
	} else {
		BUG_ON(!init);
		printInfo("passing parameters to old instance.");
		init->handleCmdLineArgs(false);
	}
	return EXIT_SUCCESS;
}


static const char *description = I18N_NOOP("PwManager\n"
					   "The convenient way of managing passwords");

#ifndef PWM_EMBEDDED
static KCmdLineOptions options[] =
{
	{ "minimized",		I18N_NOOP("Windows minimized"),		0 },
	{ "mintray",		I18N_NOOP("Windows minimized to tray"),	0 },
	{ "open-deeplocked",	I18N_NOOP("Open all \"files\" deeplocked"),	0 },
	{ "skip-self-test",	I18N_NOOP("Don't run a self-test on startup"),	0 },
	{ "+[files...]",	I18N_NOOP("Files to open on startup"),		0 },
	{ 0, 0, 0 }
};
#endif

#ifdef PWM_DEBUG
static void printDebugConfigureInfo()
{
	cout << "================================" << endl;
	cout << PROG_NAME " version " PACKAGE_VER << endl;
#ifdef CONFIG_KEYCARD
	cout << "CONFIG_KEYCARD:    enabled" << endl;
#else
	cout << "CONFIG_KEYCARD:    disabled" << endl;
#endif
#ifdef CONFIG_KWALLETIF
	cout << "CONFIG_KWALLETIF:  enabled" << endl;
#else
	cout << "CONFIG_KWALLETIF:  disabled" << endl;
#endif
#ifdef BIG_ENDIAN_HOST
	cout << "Endianess:         big-endian" << endl;
#else
	cout << "Endianess:         little-endian" << endl;
#endif
#ifdef WORDS_BIGENDIAN
	cout << "Endianess 2:         big-endian" << endl;
#else
	cout << "Endianess 2:         little-endian" << endl;
#endif

	cout << "sizeof(long):      " << sizeof(long) << endl;
	cout << "================================" << endl;
}
#else // PWM_DEBUG
static inline void printDebugConfigureInfo() { /* nothing */ }
#endif // PWM_DEBUG

#ifndef PWM_EMBEDDED
static void addAuthors(KAboutData *aboutData)
{
	aboutData->addAuthor("Michael Buesch",
			     I18N_NOOP(
			     "main programming and current maintainer"),
			     "mbuesch@freenet.de");
	aboutData->addAuthor("Matt Scifo",
			     I18N_NOOP(
			     "original implementaion of \n"
			     "\"categories\" and the password-tree \n"
			     "in the system-tray. Original implementations of \n"
			     "numerous view-improvements."),
			     "mscifo@o1.com");
	aboutData->addCredit("Elias Probst",
			     I18N_NOOP(
			     "Gentoo ebuild maintainer."),
			     "elias.probst@gmx.de");
	aboutData->addCredit("George Staikos",
			     I18N_NOOP("KWallet"),
			     "staikos@kde.org");
	aboutData->addCredit("Matthew Palmer",
			     I18N_NOOP("rc2 code"),
			     "mjp16@uow.edu.au");
        aboutData->addCredit("Olivier Sessink",
			     I18N_NOOP("gpasman"),
			     "gpasman@nl.linux.org");
	aboutData->addCredit("The libgcrypt developers",
			     I18N_NOOP("Blowfish and SHA1 algorithms"),
			     0, "ftp://ftp.gnupg.org/gcrypt/alpha/libgcrypt/");
	aboutData->addCredit("Troy Engel",
			     I18N_NOOP("kpasman"),
			     "tengel@sonic.net");
	aboutData->addCredit("Wickey",
			     I18N_NOOP("graphics-design in older versions."),
			     "wickey@gmx.at");
	aboutData->addCredit("Ian MacGregor",
			     I18N_NOOP(
			     "original documentation author."));
}
#endif

int main(int argc, char *argv[])
{
	printDebugConfigureInfo();
#ifndef PWM_EMBEDDED
	KAboutData aboutData(PACKAGE_NAME, PROG_NAME,
			     PACKAGE_VER, description, KAboutData::License_File,
			     "(c) 2003, 2004 Michael Buesch and the PwManager Team", 0,
			     "http://passwordmanager.sourceforge.net/",
			     "mbuesch@freenet.de");
	addAuthors(&aboutData);

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);

	KUniqueApplication::addCmdLineOptions();
	if (!KUniqueApplication::start()) {
		printInfo("already running.");
		return EXIT_SUCCESS;
	}
	PwMApplication a;
	aboutData.setLicenseTextFile(LICENSE_FILE);
	return a.exec();
#else

  bool exitHelp = false;
  if ( argc > 1 ) {
      QString command = argv[1];
      if (  command == "-help" ){
          printf("PWM/PI command line commands:\n");
          printf("  no command: Start PWM/PI in usual way\n");
          printf("  -help: This output\n");
          printf(" PWM/PI is exiting now. Bye!\n");
          exitHelp = true;
      }
  }
  if ( ! exitHelp ) {

    PwMApplication a(argc, argv);

    KGlobal::setAppName( "pwmanager" );
#ifndef DESKTOP_VERSION
    //qDebug("width %d ",QApplication::desktop()->width() );
    if ( QApplication::desktop()->width() > 320 )
      KGlobal::iconLoader()->setIconPath(QString(getenv("QPEDIR"))+"/pics/kdepim/pwmanager/icons22/");
    else
        KGlobal::iconLoader()->setIconPath(QString(getenv("QPEDIR"))+"/pics/kdepim/pwmanager/icons16/");
#else
      QString fileName ;
      fileName  = qApp->applicationDirPath () + "/kdepim/pwmanager/icons22/";
      KGlobal::iconLoader()->setIconPath(QDir::convertSeparators(fileName));
      QApplication::addLibraryPath ( qApp->applicationDirPath () );

#endif
      KStandardDirs::setAppDir( QDir::convertSeparators(locateLocal("data", "pwmanager")));
      KPimGlobalPrefs::instance()->setGlobalConfig();
      QApplication::setFont( KPimGlobalPrefs::instance()->mApplicationFont );

      a.newInstance();

      //US       KAddressBookMain m ;

      //US      QObject::connect(&a, SIGNAL (appMessage ( const QCString &, const QByteArray & )), ExternalAppHandler::instance(), SLOT (appMessage ( const QCString &, const QByteArray & )));
      /*US
#ifndef DESKTOP_VERSION
       a.showMainWidget( &m );
#else
      a.setMainWidget( &m );
      m.resize (640, 480 );
      m.show();
#endif
      */
      QObject::connect( &a, SIGNAL( lastWindowClosed()), &a, SLOT (quit()));
      a.exec();
      dumpMissing();
      KPimGlobalPrefs::instance()->writeConfig();
  }
  qDebug("PWMPI: Bye! ");

#endif

}
