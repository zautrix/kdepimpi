#include "kdebug.h"
#include "kurl.h"

#include "kstandarddirs.h"
#ifdef  DESKTOP_VERSION
#include <qregexp.h>
#include <qmessagebox.h>

#endif 


#include <qfile.h>
#include <qdir.h>

QString KStandardDirs::mAppDir = QString::null;

QString locate( const char *type, const QString& filename )
{
/*US why do we put all files into one directory. It is quit complicated.
why not staying with the original directorystructure ?

  
  QString escapedFilename = filename;
  escapedFilename.replace( QRegExp( "/" ), "_" );

  QString path = KStandardDirs::appDir() + type + "_" + escapedFilename;

  kdDebug() << "locate: '" << path << "'" << endl;
  qDebug("locate: %s" , path.latin1());
  return path;
*/
//US so my proposal is this:  

//  QString escapedFilename = filename;
//  escapedFilename.replace( QRegExp( "/" ), "_" );

#ifdef _WIN32_
  QString path = QDir::convertSeparators(KStandardDirs::appDir() + type + "/" + filename);
#else
  QString path = KStandardDirs::appDir() + type + "/" + filename;
#endif
  
  //US Create the containing dir if needed
  KURL pathurl;
  pathurl.setPath(path);
  QString dir=pathurl.directory();
  //	QMessageBox::information( 0,"path", path, 1 );
#ifdef _WIN32_
  KStandardDirs::makeDir(path);
#else
  KStandardDirs::makeDir(dir);
#endif 
  
  kdDebug() << "locate: '" << path << "'" << endl;
  qDebug("locate: %s" , path.latin1());
  return path;

  
}

QString locateLocal( const char *type, const QString& filename )
{
  return locate( type, filename );
}

QStringList KStandardDirs::findAllResources( const QString &, const QString &, bool, bool)
{
  return QStringList();
}

QString KStandardDirs::findResourceDir( const QString &, const QString & )
{
  return QString::null;
}

void KStandardDirs::setAppDir( const QString &appDir )
{
  mAppDir = appDir;

  if ( mAppDir.right( 1 ) != "/" ) mAppDir += "/";
}

bool KStandardDirs::makeDir(const QString& dir, int mode)
{
  QDir dirObj;

      
    // we want an absolute path
#ifndef _WIN32_
    if (dir.at(0) != '/')
        return false;
#endif
        
  

    QString target = dir;
    uint len = target.length();
#ifndef _WIN32_
    // append trailing slash if missing
    if (dir.at(len - 1) != '/')
        target += '/';
#endif

    QString base("");
    uint i = 1;

    while( i < len )
    {
//US        struct stat st;
#ifndef _WIN32_
        int pos = target.find('/', i);
#else
	int pos = target.find('\\', i);
#endif
	if ( pos < 0 )
	  return true;
        base += target.mid(i - 1, pos - i + 1);
	//QMessageBox::information( 0,"cap111", base, 1 );
/*US        
        QCString baseEncoded = QFile::encodeName(base);
        // bail out if we encountered a problem
        if (stat(baseEncoded, &st) != 0)
        {
          // Directory does not exist....
          // Or maybe a dangling symlink ?
          if (lstat(baseEncoded, &st) == 0)
              (void)unlink(baseEncoded); // try removing
              

          if ( mkdir(baseEncoded, (mode_t) mode) != 0) {
            perror("trying to create local folder");
            return false; // Couldn't create it :-(
          }
        }
*/        
        
        if (dirObj.exists(base) == false)
        {
          qDebug("KStandardDirs::makeDir try to create : %s" , base.latin1());
          if (dirObj.mkdir(base) != true)
          {
            qDebug("KStandardDirs::makeDir could not create: %s" , base.latin1());
            return false;
          }
        }

        i = pos + 1;
    }
    return true;
}

