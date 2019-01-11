
#include <kpopupmenu.h>
#include <qtimer.h>


KPopupMenu::KPopupMenu ( QWidget * parent, const char * name )
    : QPopupMenu ( parent, name ) {;}



KMenuBar::KMenuBar ( QWidget * parent, const char * name )
    : QPEMenuBar ( parent, name ) {}

void KMenuBar::focusOutEvent ( QFocusEvent * e)
{ 
    QPEMenuBar::focusOutEvent( e );
    QTimer::singleShot( 100, this, SIGNAL ( lostFocus() ) );
    
}
