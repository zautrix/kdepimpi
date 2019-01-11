#ifndef MICROKDE_KGLOBALSETTINGS_H
#define MICROKDE_KGLOBALSETTINGS_H

#include <qfont.h>
#include <qrect.h>


#define KDE_DEFAULT_SINGLECLICK true


class KGlobalSettings
{
  public:
    static QFont generalFont();
    static QFont generalMaxFont();
    static QFont toolBarFont();
    
    static QColor mAlternate;
    static QColor toolBarHighlightColor();
    static QColor alternateBackgroundColor();
    static void  setAlternateBackgroundColor(QColor);
    static QRect desktopGeometry( QWidget * );
    static QString timeTrackerDir();
    static QString backupDataDir();
    static QString calendarDir();
    
    /**
     * Returns whether KDE runs in single (default) or double click
     * mode.
     * see http://developer.kde.org/documentation/standards/kde/style/mouse/index.html
     * @return true if single click mode, or false if double click mode.
     **/
    static bool singleClick();
    
};

#endif
