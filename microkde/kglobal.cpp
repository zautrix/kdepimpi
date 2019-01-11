#include "kglobal.h"
#include "kstandarddirs.h"
#ifndef QT_4_COMPAT
#include <qkeycode.h>
#endif
#include <qapplication.h>

KLocale *KGlobal::mLocale = 0;
KConfig *KGlobal::mConfig = 0;
KIconLoader *KGlobal::mIconLoader = 0;
KStandardDirs *KGlobal::mDirs = 0;

QString KGlobal::mAppName = "godot";

KLocale *KGlobal::locale()
{
  if ( !mLocale ) {
      //Q_ASSERT(mAppName);

    mLocale = new KLocale();//mAppName);
  }

  return mLocale;
}

//US
void KGlobal::setLocale(KLocale *kg)
{
    mLocale = kg;
}

KConfig *KGlobal::config()
{
  //mConfig is set inside setAppName. Though it has to be the first function you call.
  return mConfig;
}

KGlobal::Size KGlobal::getDesktopSize()
{
#ifdef DESKTOP_VERSION
  return KGlobal::Desktop;
#else
  if ( QApplication::desktop()->width() <= 320 )
    return KGlobal::Small;
  else if ( QApplication::desktop()->width() > 480)
    return KGlobal::Desktop;
  else
    return KGlobal::Medium;
#endif
}

KGlobal::Orientation KGlobal::getOrientation()
{
  if (QApplication::desktop()->width() > QApplication::desktop()->height())
    return KGlobal::Landscape;
  else
    return KGlobal::Portrait;
}

int KGlobal::getDesktopWidth()
{
  return QApplication::desktop()->width();
}

int KGlobal::getDesktopHeight()
{
  return QApplication::desktop()->height();
}


KIconLoader *KGlobal::iconLoader()
{
  if ( !mIconLoader ) {
    mIconLoader = new KIconLoader();
  }

  return mIconLoader;
}

KStandardDirs *KGlobal::dirs()
{
  if ( !mDirs ) {
    mDirs = new KStandardDirs();
  }

  return mDirs;
}

void KGlobal::setAppName( const QString &appName )
{
  mAppName = appName;

  mConfig = new KConfig( locateLocal("config", mAppName + "rc" ) );
}

//US
QString KGlobal::getAppName()
{
  return mAppName;
}
QString KGlobal::formatMessage ( QString mess, int maxlen  )
{
    //int maxlen = 80;
    if ( maxlen == 0 ) {
        maxlen =  QApplication::desktop()->width()/10;
        if ( maxlen > 32 )
            maxlen = (maxlen * 3) / 4;
        if ( maxlen > 100 )
            maxlen = 100;
    }
    int start = 0;
    int end = mess.length();
    QString retVal = "";
    int nl, space;
    while ( (end - start) > maxlen ) {
        nl = mess.find( "\n", start );
        if ( nl > 0 &&  nl  < start + maxlen ) {
            nl += 1;
            retVal += mess.mid( start, nl - start);
            start = nl;
        } else {
            space = mess.findRev( " ", start + maxlen );
            if ( space < start ) {
                retVal += mess.mid( start, maxlen) +"\n";
                start +=  maxlen ;
            } else {
                retVal += mess.mid( start, space - start ) +"\n";
                start =  space+ 1;
            }
        }
    }
    retVal += mess.mid( start, end - start );
    return retVal;
}
int  KGlobal::knumkeykonv( int k )
{
    int key;
switch( k ) {
 case Qt::Key_Q :
     key = Qt::Key_1;
    break;
 case Qt::Key_W :
     key = Qt::Key_2;
    break;
 case Qt::Key_E :
     key = Qt::Key_3;
    break;
 case Qt::Key_R :
     key = Qt::Key_4;
    break;
 case Qt::Key_T :
     key = Qt::Key_5;
    break;
 case Qt::Key_Z :
     key = Qt::Key_6;
    break;
 case Qt::Key_Y :
     key = Qt::Key_6;
    break;
 case Qt::Key_U :
     key = Qt::Key_7;
    break;
 case Qt::Key_I :
     key = Qt::Key_8;
    break;
 case Qt::Key_O :
     key = Qt::Key_9;
    break;
 case Qt::Key_P :
     key = Qt::Key_0;
    break;
   default:
       key = k;
    break;
  } // switch
 return key;
}
