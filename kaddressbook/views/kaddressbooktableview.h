#ifndef KADDRESSBOOKTABLEVIEW_H
#define KADDRESSBOOKTABLEVIEW_H


#ifndef KAB_EMBEDDED


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <qlistview.h>
#include <qstring.h>
#include <qdialog.h>
#include <qtabdialog.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include "undo.h"

#else //KAB_EMBEDDED
#include "views/configuretableviewdialog.h"
#endif //KAB_EMBEDDED

#include "klocale.h"
#include "kaddressbookview.h"

class QListViewItem;
class QListBox;
class QVBoxLayout;
class KConfig;

class ContactListViewItem;
class ContactListView;


namespace KABC { class AddressBook; }

/**
 * This class is the table view for kaddressbook. This view is a KListView
 * with multiple columns for the selected fields.
 *
 * @short Table View
 * @author Don Sanders <dsanders@kde.org>
 * @version 0.1
 */
class KAddressBookTableView : public KAddressBookView
{
friend class ContactListView;

  Q_OBJECT

  public:
    KAddressBookTableView( KABC::AddressBook *ab, QWidget *parent,
                           const char *name = 0 );
    virtual ~KAddressBookTableView();

    virtual void refresh(QString uid = QString::null);
    virtual QStringList selectedUids();
    virtual void setSelected(QString uid = QString::null, bool selected = false);
    virtual void readConfig(KConfig *config);
    virtual void writeConfig(KConfig *config);
    virtual QString type() const { return "Table"; }
    void doSearch( const QString&  s ,KABC::Field *field );
    virtual void scrollUP();
    virtual void scrollDOWN();
    virtual void setFocusAV();

  public slots:
    virtual void reconstructListView();

  protected slots:
    /** Called whenever the user selects an addressee in the list view.
    */
    void addresseeSelected();
    void addresseeDeleted();

    /** Called whenever the user executes an addressee. In terms of the
    * list view, this is probably a double click
    */
    void addresseeExecuted(QListViewItem*);

  private:
    QVBoxLayout *mainLayout;
    ContactListView *mListView;
};


class TableViewFactory : public ViewFactory
{
  public:
    KAddressBookView *view( KABC::AddressBook *ab, QWidget *parent, const char *name )
    {
      return new KAddressBookTableView( ab, parent, name );
    }

    QString type() const { return "Table"; }

    QString description() const { return i18n( "A listing of contacts in a table. Each cell of "
                                  "the table holds a field of the contact." ); }

    ViewConfigureWidget *configureWidget( KABC::AddressBook *ab, QWidget *parent,
                                          const char *name = 0 )
    {
      return new ConfigureTableViewWidget( ab, parent, name );
    }
};
/*US
extern "C" {
  void *init_libkaddrbk_tableview()
  {
    return ( new TableViewFactory );
  }
}
*/

#endif
