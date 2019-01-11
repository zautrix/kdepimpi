#ifndef MINIKDE_KSTANDARDDIRS_H
#define MINIKDE_KSTANDARDDIRS_H

#include <qstring.h>
#include <qstringlist.h>

QString locate( const char *type, const QString& filename );
QString locateLocal( const char *type, const QString& filename );

class KStandardDirs
{
  public:
    QStringList findAllResources( const QString &, const QString &, bool, bool);
    QString findResourceDir( const QString &, const QString & ); 

    static void setAppDir( const QString & );
    static QString appDir() { return mAppDir; }

  /**
   * Recursively creates still-missing directories in the given path.
   *
   * The resulting permissions will depend on the current umask setting.
   * permission = mode & ~umask.
   *
   * @param dir Absolute path of the directory to be made.
   * @param mode Directory permissions.
   * @return true if successful, false otherwise
   */
  static bool makeDir(const QString& dir, int mode = 0755);
      
  private:
    static QString mAppDir;
};

#endif
