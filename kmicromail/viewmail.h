#ifndef VIEWMAIL_H
#define VIEWMAIL_H

#include "viewmailbase.h"
#include <libmailwrapper/mailtypes.h>

#include <qdialog.h>
#include <qapplication.h>

#include <qlistview.h>
#include <qmap.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qvaluelist.h>

//namespace Opie { namespace MM { class OImageScrollView; } }

class AttachItem : public QListViewItem
{
public:
    AttachItem(QListView * parent,QListViewItem *after, const QString&mime,const QString&desc,const QString&file,
        const QString&fsize,int num,const QValueList<int>&path);
    AttachItem(QListViewItem * parent,QListViewItem *after, const QString&mime,const QString&desc,const QString&file,
        const QString&fsize,int num,const QValueList<int>&path);
    int Partnumber() { return _partNum; }
    bool isParentof(const QValueList<int>&path);

private:
    int _partNum;
    /* needed for a better display of attachments */
    QValueList<int> _path;
};

class ViewMail : public ViewMailBase
{
    Q_OBJECT

public:
    ViewMail(  QWidget *parent = 0, const char *name = 0, WFlags fl = 0);
    ~ViewMail();

    void hide();
    void exec();
    void setMail(const RecMailP&mail );
    void setBody(const RecBodyP&body);
    bool deleted;
 signals:
    void showNextMail(ViewMail*);
    void deleteAndDisplayNextMail(ViewMail *);
    void signalDownloadMail();
protected:
    QString deHtml(const QString &string);
    AttachItem* searchParent(const QValueList<int>&path);
    AttachItem* lastChild(AttachItem*parent);

protected slots:
    void slotNextMail();
    void slotReply();
    void slotForward();
    void setText();
    void slotItemClicked( QListViewItem * item , const QPoint & point, int c );
    void slotDeleteMail( );
    void slotShowHtml( bool );
    void slotViewSource();

private:
    void readConfig();
    bool sourceOn;;
    bool _inLoop;
    QString m_mailHtml;
    bool m_gotBody;
    RecBodyP m_body;
    RecMailP m_recMail;
    bool m_showHtml;

    // 0 from  1 subject  2 bodytext 3 date
    QMap <int,QString>  m_mail;
    // 0 to 1 cc 2 bcc
    QMap <int,QStringList> m_mail2;
};

class MailImageDlg:public QDialog
{
    Q_OBJECT
public:
    MailImageDlg(const QString&,QWidget *parent = 0, const char *name = 0, bool modal = true, WFlags f = 0);
    ~MailImageDlg();
    void setName(const QString&);
protected:
    //Opie::MM::OImageScrollView*m_imageview;
};

#endif
