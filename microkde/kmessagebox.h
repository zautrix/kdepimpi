#ifndef MINIKDE_KMESSAGEBOX_H
#define MINIKDE_KMESSAGEBOX_H

#include <qstring.h>

#include "klocale.h"
class QWidget;

class KMessageBox
{
  public:
    enum { Ok = 1, Cancel = 2, Yes = 3, No = 4, Continue = 5 };

    static void sorry(QWidget *parent, 
                    const QString &text,
                    const QString &caption = QString::null, bool notify=true);

    static int warningContinueCancel(QWidget *parent, 
                         const QString &text,
                         const QString &caption = i18n("Warning"),
                         const QString &buttonContinue =i18n("Continue"), 
                         const QString &dontAskAgainName = i18n("Cancel"),
                         bool notify=true );

    static int warningYesNoCancel(QWidget *parent, 
                                  const QString &text,
                                  const QString &caption = i18n("Warning"),
                                  const QString &buttonYes = i18n("Yes"), 
                                  const QString &buttonNo = i18n("No"));

    static int questionYesNo(QWidget *parent, 
                         const QString &text,
                         const QString &caption = i18n("Question"));

    static void error(QWidget *parent, 
                    const QString &text, 
                    const QString &caption = i18n("Error"), bool notify=true);

    static void information(QWidget *parent, 
                          const QString &text, 
                          const QString &caption = i18n("Information"),
                          const QString &dontShowAgainName = QString::null, 
                          bool notify=true);
};


#endif
