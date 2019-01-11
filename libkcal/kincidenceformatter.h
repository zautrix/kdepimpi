#ifndef KINCIDENCENFORMATTER_H
#define KINCIDENCENFORMATTER_H

#include <qstring.h>
#include <qobject.h>

#include "incidence.h"
#include "event.h"
#include "todo.h"
#include "journal.h"

using namespace KCal;

class KIncidenceFormatter : public QObject
{
  public:
    static KIncidenceFormatter* instance();
    KIncidenceFormatter();
    ~KIncidenceFormatter();
    QString getFormattedText( Incidence * inc , bool details = false, bool created = false, bool modified = false );
 
    void setEvent(Event *event);
    void setTodo(Todo *event );
    void setJournal(Journal*  );

  protected:
    int mColorMode;
    void addTag(const QString & tag,const QString & text);

    void formatCategories(Incidence *event);
    void formatAttendees(Incidence *event);
    void formatReadOnly(Incidence *event);

  private:
    QString deTag(QString text);
    bool mSyncMode;
    bool mDetails, mCreated ,mModified;

    QString mText;
    Incidence* mCurrentIncidence; 
    static KIncidenceFormatter* mInstance; 
};

#endif
