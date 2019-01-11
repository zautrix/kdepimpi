#ifndef OPIE_OFILE_SELECTOR_PRIVATE_H
#define OPIE_OFILE_SELECTOR_PRIVATE_H

//US
#pragma message("microkde/ofileselector_p.h")

#include <qmap.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <qlistview.h>

#include <qpe/applnk.h>
#include <qpe/fileselector.h>


/*
 * How to avoid having really two different objects
 * for Extended and ExtendedAll
 * The only difference is the Lister...
 * a) static object?
 * b) leave some object inside the OFileSelector which can be used?
 * c) when switching views tell which view we want o have.. internally we can switch then
 *
 * I'll take c) -zecke
 */


/* the View Interface */
class OFileSelector;
typedef QMap<QString, QStringList> MimeTypes;
class QFileInfo;
class QToolButton;
class OFileViewInterface {
public:
    OFileViewInterface( OFileSelector* selector );
    virtual ~OFileViewInterface();
    virtual QString selectedName()const = 0;
    virtual QString selectedPath()const = 0;
    virtual QString directory()const = 0;
    virtual void reread() = 0;
    virtual int fileCount()const = 0;
    virtual DocLnk selectedDocument()const;
    virtual QWidget* widget( QWidget* parent) = 0;
    virtual void activate( const QString& );
    QString name()const;
protected:
    OFileSelector* selector()const;
    void setName( const QString& );
    bool showNew()const;
    bool showClose()const;
    MimeTypes mimeTypes()const;
    QStringList currentMimeType()const;
    QString startDirectory()const;
protected:
    void ok();
    void cancel();
    void closeMe();
    void fileSelected( const QString& );
    void fileSelected( const DocLnk& );
    void setCurrentFileName( const QString& );
    QString currentFileName()const;

private:
    QString m_name;
    OFileSelector* m_selector;
};


/* THE Document View hosting a FileSelector*/
class ODocumentFileView : public OFileViewInterface {
public:
    ODocumentFileView( OFileSelector* selector );
    ~ODocumentFileView();

    QString selectedName() const;
    QString selectedPath() const;

    QString directory() const;
    void reread();
    int fileCount()const;
    DocLnk selectedDocument()const;

    QWidget* widget( QWidget* parent );

private:
    mutable FileSelector* m_selector;

};

class OFileSelectorItem : public QListViewItem {
public:
    OFileSelectorItem( QListView* view, const QPixmap& pixmap,
                       const QString& path, const QString& date,
                       const QString& size, const QString& mDir,
                       bool isLocked = false, bool isDir = false );
    ~OFileSelectorItem();
    bool isLocked()const;
    bool isDir()const;
    QString directory()const;
    QString path()const;
    QString key(int id, bool )const;

private:
    bool m_locked : 1;
    bool m_isDir    : 1;
    QString m_dir;
};

class OFileViewFileListView : public QWidget {
    Q_OBJECT
public:
    OFileViewFileListView( QWidget* parent, const QString& dir, OFileSelector* selector );
    ~OFileViewFileListView();

    OFileSelectorItem* currentItem()const;
    void reread( bool all = false );
    int fileCount()const;
    QString currentDir()const;
protected:
    bool eventFilter (QObject *o, QEvent *e);
private slots:
    void slotNew(); // will emit newSelected
    void cdUP();
    void cdHome();
    void cdDoc();
    void changeDir( const QString& );
    void slotCurrentChanged( QListViewItem* );
    void slotClicked(int, QListViewItem*, const QPoint&, int );
    void slotDoubleClicked(QListViewItem*);
    void slotFSActivated(int);

protected:

    OFileSelector* selector();

private:
    QMap<QString, QString> m_dev;
    bool m_all : 1;
    OFileSelector* m_sel;
    QPopupMenu* m_fsPop;
    bool compliesMime( const QString& );
    QStringList m_mimes; // used in compy mime
    QString m_currentDir;
    QToolButton *m_btnNew, *m_btnClose;
    void connectSlots();
    void addFile( QFileInfo* info, bool symlink = FALSE );
    void addDir ( QFileInfo* info, bool symlink = FALSE );
    void addSymlink( QFileInfo* info, bool = FALSE );


private:
    QListView* m_view;
};

typedef QMap<QString, QStringList> MimeTypes;

class OFileViewInterface;
class OFileViewFileListView;
class QLineEdit;
class QComboBox;
class QWidgetStack;
class QHBox;
class OFileSelector : public QWidget {
    Q_OBJECT
    friend class OFileViewInterface;
    friend class OFileViewFileListView;
public:
    enum Mode { Open=1, Save=2, FileSelector=4, OPEN=1, SAVE=2, FILESELECTOR=4 };
//    enum OldMode { OPEN=1, SAVE=2, FILESELECTOR = 4 };
    enum Selector { Normal = 0, Extended=1,  ExtendedAll =2, Default=3,  NORMAL=0,EXTENDED=1, EXTENDED_ALL =2, DEFAULT=3 };
//    enum OldSelector { NORMAL = 0, EXTENDED =1, EXTENDED_ALL = 2};

    OFileSelector(QWidget* parent, int mode, int selector,
                  const QString& dirName,
                  const QString& fileName,
                  const MimeTypes& mimetypes = MimeTypes(),
                  bool newVisible = FALSE, bool closeVisible = FALSE );

    OFileSelector(const QString& mimeFilter, QWidget* parent,
                  const char* name = 0,  bool newVisible = TRUE, bool closeVisible = FALSE );
    ~OFileSelector();

    const DocLnk* selected();

    QString selectedName()const;
    QString selectedPath()const;
    QString directory()const;

    DocLnk selectedDocument()const;

    int fileCount()const;
    void reread();

    int mode()const;
    int selector()const;


    void setNewVisible( bool b );
    void setCloseVisible( bool b );
    void setNameVisible( bool b );

signals:
    void dirSelected( const QString& );
    void fileSelected( const DocLnk& );
    void fileSelected( const QString& );
    void newSelected( const DocLnk& );
    void closeMe();
    void ok();
    void cancel();

/* used by the ViewInterface */
private:
    bool showNew()const;
    bool showClose()const;
    MimeTypes mimeTypes()const;
    QStringList currentMimeType()const;

private:
    /* inits the Widgets */
    void initUI();
    /* inits the MimeType ComboBox content + connects signals and slots */
    void initMime();
    /* init the Views :) */
    void initViews();

private:
    QLineEdit* m_lneEdit; // the LineEdit for the Name
    QComboBox *m_cmbView, *m_cmbMime; // two ComboBoxes to select the View and MimeType
    QWidgetStack* m_stack;  // our widget stack which will contain the views
    OFileViewInterface* currentView()const; // returns the currentView
    OFileViewInterface* m_current; // here is the view saved
    bool m_shNew   : 1; // should we show New?
    bool m_shClose : 1; // should we show Close?
    MimeTypes m_mimeType; // list of mimetypes

    QMap<QString, OFileViewInterface*> m_views; // QString translated view name + ViewInterface Ptr
    QHBox* m_nameBox; // the LineEdit + Label is hold here
    QHBox* m_cmbBox; // this holds the two combo boxes

    QString m_startDir;
    int m_mode;
    int m_selector;

    struct Data; // used for future versions
    Data *d;

private slots:
    void slotMimeTypeChanged();

    /* will set the text of the lineedit and emit a fileChanged signal */
    void slotDocLnkBridge( const DocLnk& );
    void slotFileBridge( const QString& );
    void slotViewChange( const QString& );

    bool eventFilter (QObject *o, QEvent *e);

};

#endif
