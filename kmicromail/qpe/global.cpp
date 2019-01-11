
#include "global.h"
#include <qstatusbar.h> 
QStatusBar* globalSstatusBarMainWindow = 0;
namespace Global{

    void statusMessage(QString message)
    {
        if ( globalSstatusBarMainWindow )
            globalSstatusBarMainWindow->message( message,15000 );
        qDebug("statusMessage %s ", message.latin1());
    }
}

