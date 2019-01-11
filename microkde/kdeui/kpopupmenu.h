#ifndef KPOPUPMENU_H
#define KPOPUPMENU_H

#include <qpopupmenu.h>
#ifdef DESKTOP_VERSION
#include <qmenubar.h>
#define QPEMenuBar QMenuBar
#else
#include <qpe/qpemenubar.h>
#endif

class KPopupMenu : public QPopupMenu
{
Q_OBJECT
    public:
    KPopupMenu ( QWidget * parent=0, const char * name=0 );

};


class KMenuBar : public QPEMenuBar
{
Q_OBJECT
    public:
    KMenuBar ( QWidget * parent=0, const char * name=0 );
 signals:
  void lostFocus();
 protected:
  void focusOutEvent ( QFocusEvent * e);
};

#endif
