#include "kglobalsettings.h"
#include "kconfig.h"
#include "kglobal.h"
#include "kconfigbase.h"

#include <qapplication.h>

#ifdef DESKTOP_VERSION
  QColor KGlobalSettings::mAlternate = QColor( 235, 235, 235 );
#else
  QColor KGlobalSettings::mAlternate =  QColor( 210, 210, 210 );
#endif 


QFont KGlobalSettings::generalFont()
{
   int size = 12;
   if (QApplication::desktop()->width() < 480 ) {
        size = 10;   
   }
#ifndef DESKTOP_VERSION
   else
    if (QApplication::desktop()->width() >= 480 && QApplication::desktop()->width() <= 640 )
        size = 18;  
#endif 
    QFont f = QApplication::font();
    //qDebug("pointsize %d %s", f.pointSize(),f.family().latin1());
    f.setPointSize( size );
    return f;
}
QFont KGlobalSettings::generalMaxFont()
{
   int size = 12;
   if (QApplication::desktop()->width() < 480 ) {
        size = 10;   
   }
#ifndef DESKTOP_VERSION
   else
    if (QApplication::desktop()->width() >= 480 && QApplication::desktop()->width() <= 640 )
        size = 18;  
#endif 
    QFont f = QApplication::font();
    if ( f.pointSize() > size )
        f.setPointSize( size );
    return f;
}

QString KGlobalSettings::timeTrackerDir()
{
    static QString dir;
    if ( dir.isEmpty() ) {
        dir = locateLocal( "data", "timetrackerdir/d.ttl" );
        dir = dir.left ( dir.length() - 5);
    }
    return dir;
}
QString KGlobalSettings::backupDataDir()
{
    static QString dir;
    if ( dir.isEmpty() ) {
        dir = locateLocal( "data", "backupdir/d.ttl" );
        dir = dir.left ( dir.length() - 5);
    }
    return dir;
}
QString KGlobalSettings::calendarDir()
{
    static QString dir;
    if ( dir.isEmpty() ) {
        dir = locateLocal( "data", "korganizer/d.ttl" );
        dir = dir.left ( dir.length() - 5);
    }
    return dir;
}

QFont KGlobalSettings::toolBarFont()
{
    return  QApplication::font();
}

QColor KGlobalSettings::toolBarHighlightColor()
{
    return QColor( "black" );
}

QColor KGlobalSettings::alternateBackgroundColor()
{
    return mAlternate;

}
void  KGlobalSettings::setAlternateBackgroundColor(QColor c)
{
   mAlternate = c;

}

QRect KGlobalSettings::desktopGeometry( QWidget * )
{
  return QApplication::desktop()->rect();
}

    /**
     * Returns whether KDE runs in single (default) or double click
     * mode.
     * see http://developer.kde.org/documentation/standards/kde/style/mouse/index.html
     * @return true if single click mode, or false if double click mode.
     **/
bool KGlobalSettings::singleClick()
{   
    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, "KDE" );
    return c->readBoolEntry("SingleClick", KDE_DEFAULT_SINGLECLICK);
}
