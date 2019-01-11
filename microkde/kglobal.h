#ifndef MINIKDE_KGLOBAL_H
#define MINIKDE_KGLOBAL_H
#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else


#endif
#include "klocale.h"
#include "kconfig.h"
#include "kiconloader.h"
#include <kstandarddirs.h>
#include <qevent.h>
class KStandardDirs;
class KGlobal {
  public:
    static KLocale *locale();
    static KConfig *config();
    static KIconLoader *iconLoader();
    static KStandardDirs *dirs();
    static int knumkeykonv( int );

    static void setAppName( const QString & );
    static QString formatMessage( QString mess, int maxlen ) ;

//US begin: added the following methods for convenience
    static QString getAppName();
    static void setLocale(KLocale *);

    enum Orientation { Portrait, Landscape };
    enum Size { Small = 0, Medium = 1, Desktop = 2};

    static int getDesktopWidth();
    static int getDesktopHeight();
    static KGlobal::Size getDesktopSize();
    static KGlobal::Orientation getOrientation();


  private:
    static KLocale *mLocale;
    static KConfig *mConfig;
    static KIconLoader *mIconLoader;
    static KStandardDirs *mDirs;

    static QString mAppName;
};


/** @ref KGlobal
 * A typesafe function to find the minimum of the two arguments.
 */
#define KMIN(a,b)	kMin(a,b)
/** @ref KGlobal
 * A typesafe function to find the maximum of the two arguments.
 */
#define KMAX(a,b)	kMax(a,b)
/** @ref KGlobal
 * A typesafe function to determine the absolute value of the argument.
 */
#define KABS(a)	kAbs(a)


template<class T>
inline const T& kMin (const T& a, const T& b) { return a < b ? a : b; }

template<class T>
inline const T& kMax (const T& a, const T& b) { return b < a ? a : b; }

template<class T>
inline T kAbs (const T& a) { return a < 0 ? -a : a; }


#endif
