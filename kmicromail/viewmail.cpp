// CHANGED 2004-08-06 Lutz Rogowski



#include <kfiledialog.h>
#include "koprefs.h"
#include <klocale.h>
#include <kglobal.h>
#include <kapplication.h>

#ifdef  MINIKDE_KDIALOG_H
#undef MINIKDE_KDIALOG_H
#endif

#include "composemail.h"
#include "viewmail.h"

#include <libmailwrapper/settings.h>
#include <libmailwrapper/abstractmail.h>
#include <libmailwrapper/mailtypes.h>

#include <qdialog.h>

#include <qpe/qpeapplication.h>

/* QT */
#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qfile.h>
#include <qlayout.h>

//using namespace Opie::Ui;
//using namespace Opie::Core;

AttachItem::AttachItem(QListView * parent,QListViewItem *after, const QString&mime,const QString&desc,const QString&file,
                       const QString&fsize,int num,const QValueList<int>&path)
        : QListViewItem(parent,after),_partNum(num)
{
    _path=path;
    setText(0, mime);
    setText(1, desc);
    setText(2, file);
    setText(3, fsize);
}

AttachItem::AttachItem(QListViewItem * parent,QListViewItem *after, const QString&mime,const QString&desc,const QString&file,
                       const QString&fsize,int num,const QValueList<int>&path)
        : QListViewItem(parent,after),_partNum(num)
{
    _path=path;
    setText(0, mime);
    setText(1, desc);
    setText(2, file);
    setText(3, fsize);
}

bool AttachItem::isParentof(const QValueList<int>&path)
{
    /* if not set, then no parent */
    if (path.count()==0||_path.count()==0) return false;
    /* the parent must have one digit less then a child */
    if (path.count()!=_path.count()+1) return false;
    for (unsigned int i=0; i < _path.count();++i)
    {
        if (_path[i]!=path[i]) return false;
    }
    return true;
}

AttachItem* ViewMail::searchParent(const QValueList<int>&path)
{
    QListViewItemIterator it( attachments );
    for ( ; it.current(); ++it )
    {
        AttachItem*ati = (AttachItem*)it.current();
        if (ati->isParentof(path)) return ati;
    }
    return 0;
}

AttachItem* ViewMail::lastChild(AttachItem*parent)
{
    if (!parent) return 0;
    AttachItem* item = (AttachItem*)parent->firstChild();
    if (!item) return item;
    AttachItem*temp=0;
    while( (temp=(AttachItem*)item->nextSibling()))
    {
        item = temp;
    }
    return item;
}

void ViewMail::setBody(const RecBodyP&body )
{

    m_body = body;
    m_mail[2] = body->Bodytext();
    m_showHtml =  KOPrefs::instance()->mViewAsHtml;
    if ( m_showHtml ) {
        if ( m_mail[2].find ("<html>",0,false ) > -1 ) {
            qDebug("html mail ");
        } else {
            qDebug("no html mail ");
            m_showHtml = false;
        }
    }
    showHtml->blockSignals( true );
    showHtml->setOn( m_showHtml );
    showHtml->blockSignals( false );
    // qDebug("********text %s ",m_mail[2].latin1() );
    attachbutton->setEnabled(body->Parts().count()>0);
    attachments->setEnabled(body->Parts().count()>0);
    if (body->Parts().count()==0)
    {
        return;
    }
    AttachItem * curItem=0;
    AttachItem * parentItem = 0;
    QString type=body->Description()->Type()+"/"+body->Description()->Subtype();
    QString desc,fsize;
    double s = body->Description()->Size();
    int w;
    w=0;

    while (s>1024)
    {
        s/=1024;
        ++w;
        if (w>=2) break;
    }

    QString q="";
    switch(w)
    {
    case 1:
        q="k";
        break;
    case 2:
        q="M";
        break;
    default:
        break;
    }

    {
        /* I did not found a method to make a CONTENT reset on a QTextStream
           so I use this construct that the stream will re-constructed in each
           loop. To let it work, the textstream is packed into a own area of
           code is it will be destructed after finishing its small job.
        */
        QTextOStream o(&fsize);
        if (w>0) o.precision(2); else o.precision(0);
        o.setf(QTextStream::fixed);
        o << s << " " << q << "Byte";
    }

    curItem=new AttachItem(attachments,curItem,type,"Mailbody","",fsize,-1,body->Description()->Positionlist());
    QString filename = "";

    for (unsigned int i = 0; i < body->Parts().count();++i)
    {
        filename = "";
        type = body->Parts()[i]->Type()+"/"+body->Parts()[i]->Subtype();
        part_plist_t::ConstIterator it = body->Parts()[i]->Parameters().begin();
        for (;it!=body->Parts()[i]->Parameters().end();++it)
        {
            if (it.key().lower()=="name")
            {
                filename=it.data();
            }
        }
        s = body->Parts()[i]->Size();
        w = 0;
        while (s>1024)
        {
            s/=1024;
            ++w;
            if (w>=2) break;
        }
        switch(w)
        {
        case 1:
            q="k";
            break;
        case 2:
            q="M";
            break;
        default:
            q="";
            break;
        }
        QTextOStream o(&fsize);
        if (w>0) o.precision(2); else o.precision(0);
        o.setf(QTextStream::fixed);
        o << s << " " << q << "Byte";
        desc = body->Parts()[i]->Description();
        parentItem = searchParent(body->Parts()[i]->Positionlist());
        if (parentItem)
        {
            AttachItem*temp = lastChild(parentItem);
            if (temp) curItem = temp;
            curItem=new AttachItem(parentItem,curItem,type,desc,filename,fsize,i,body->Parts()[i]->Positionlist());
            attachments->setRootIsDecorated(true);
            curItem = parentItem;
        }
        else
        {
            curItem=new AttachItem(attachments,curItem,type,desc,filename,fsize,i,body->Parts()[i]->Positionlist());
        }
    }
}


void ViewMail::slotViewSource()
{
    
    if ( !sourceOn ) {
        sourceOn = true;
        viewSource->setText(i18n("View Body Text"));
        encodedString*st = 0;
        st = m_recMail->Wrapper()->fetchRawBody(m_recMail);
        if ( st ) {
            QString source = QString::fromUtf8( st->Content(), st->Length());
            browser->setText( source  );
            delete st;
        }
    } else
        setText();
    

}
void ViewMail::slotShowHtml( bool state )
{
    m_showHtml = state;
    setText();
}

void ViewMail::slotItemClicked( QListViewItem * item , const QPoint & point, int )
{
    if (!item )
        return;

    if (  ( ( AttachItem* )item )->Partnumber() == -1 )
    {
        setText();
        return;
    }
    QPopupMenu *menu = new QPopupMenu();
    int ret=0;

    if ( item->text( 0 ).left( 5 ) == "text/" || item->text(0)=="message/rfc822" )
    {
        menu->insertItem( i18n( "Show Text" ), 1 );
    }
    if (item->text(0).left(6)=="image/") {
        menu->insertItem(i18n("Display image preview"),2);
    }
    menu->insertItem( i18n( "Save Attachment" ),  0 );
    menu->insertSeparator(1);

    ret = menu->exec( point, 0 );

    switch(ret)
    {
    case 0:
        {
            //MimeTypes types;
            //types.insert( "all", "*" );
            QString str = KFileDialog::getSaveFileName( "/", item->text( 2 ), this );

            if( !str.isEmpty() )
            {
                encodedString*content = m_recMail->Wrapper()->fetchDecodedPart( m_recMail, m_body->Parts()[ ( ( AttachItem* )item )->Partnumber() ] );
                if (content)
                {
                    QFile output(str);
                    output.open(IO_WriteOnly);
                    output.writeBlock(content->Content(),content->Length());
                    output.close();
                    delete content;
                }
            }
        }
        break ;

    case 2: 
        {
#ifdef DESKTOP_VERSION
            QString tmpfile = locateLocal( "tmp", "opiemail-image");
#else
            QString tmpfile = "/tmp/opiemail-image";
#endif
            encodedString*content = m_recMail->Wrapper()->fetchDecodedPart( m_recMail, m_body->Parts()[ ( ( AttachItem* )item )->Partnumber() ] );
            if (content) {
                QFile output(tmpfile);
                output.open(IO_WriteOnly);
                output.writeBlock(content->Content(),content->Length());
                output.close();
                delete content;
                MailImageDlg iview("");
                iview.setName(tmpfile);
                KApplication::execDialog(&iview);
                output.remove();
            }
        }
        break;
    case 1:
        if (  ( ( AttachItem* )item )->Partnumber() == -1 )
        {
            setText();
        }
        else
        {
            if (  m_recMail->Wrapper() != 0l )
            { // make sure that there is a wrapper , even after delete or simular actions
                browser->setText( m_recMail->Wrapper()->fetchTextPart( m_recMail, m_body->Parts()[ ( ( AttachItem* )item )->Partnumber() ] ) );
            }
        }
        break;
    }
    delete menu;
}


void ViewMail::setMail(const RecMailP&mail )
{

    m_recMail = mail;

    m_mail[0] = mail->getFrom();
    m_mail[1] = mail->getSubject();
    m_mail[3] = mail->getDate();
    m_mail[4] = mail->Msgid();

    m_mail2[0] = mail->To();
    m_mail2[1] = mail->CC();
    m_mail2[2] = mail->Bcc();

    setText();
}

void ViewMail::slotNextMail() 
{ 
    nextMail->blockSignals( true );
    setCaption(i18n("Displaying next mail...please wait!")); 
    qApp->processEvents();
    emit  showNextMail(this); 
    nextMail->blockSignals( false );
}

ViewMail::ViewMail( QWidget *parent, const char *name, WFlags fl)
        : ViewMailBase(parent, name, fl), _inLoop(false)
{
    m_gotBody = false;
    deleted = false;
    sourceOn = false;
    readConfig();
    connect( reply, SIGNAL(activated()), SLOT(slotReply()));
    connect( forward, SIGNAL(activated()), SLOT(slotForward()));
    connect( deleteMail, SIGNAL( activated() ),  SLOT( slotDeleteMail() ) );
    connect( showHtml, SIGNAL( toggled(bool) ), SLOT( slotShowHtml(bool) ) ); 
    connect( closeMail, SIGNAL( activated() ), SLOT( close() ) ); 
    connect( nextMail, SIGNAL( activated() ), SLOT( slotNextMail() ) );
    connect( viewSource, SIGNAL( activated() ), SLOT( slotViewSource() ) );
    connect( downloadMail, SIGNAL( activated() ), SIGNAL( signalDownloadMail() ) );

    attachments->setEnabled(m_gotBody);
    connect( attachments,  SIGNAL( clicked(QListViewItem*,const QPoint&, int) ), SLOT( slotItemClicked(QListViewItem*,const QPoint&, int) ) );

    attachments->setSorting(-1);
}

void ViewMail::readConfig()
{
 
    setFont ( KOPrefs::instance()->mReadFont );
    m_showHtml =  KOPrefs::instance()->mViewAsHtml;
    showHtml->setOn( m_showHtml );
}

void ViewMail::setText()
{

    viewSource->setText(i18n("View Source"));
    sourceOn = false;
    QString toString;
    QString ccString;
    QString bccString;

   
    toString = m_mail2[0].join(",");
    ccString  = m_mail2[1].join(",");
    bccString = m_mail2[2].join(",");
  
#ifdef DESKTOP_VERSION
    setCaption( i18n("Size: ")+m_recMail->MsgsizeString()+" - "+i18n("E-Mail by %1").arg( m_mail[0] ) );
#else
    setCaption( m_recMail->MsgsizeString()+" - "+m_mail[0] );
#endif

    m_mailHtml = "<html><body>"
                 "<table width=\"100%\" border=\"0\"><tr bgcolor=\"#FFDD76\"><td>"
                 "<div align=left><b>" + deHtml( m_mail[1] ) + "</b></div>"
                 "</td></tr><tr bgcolor=\"#EEEEE6\"><td>"
                 "<b>" + i18n( "From" ) + ": </b><font color=#6C86C0>" + deHtml( m_mail[0] ) + "</font><br>"
                 "<b>" + i18n(  "To" ) + ": </b><font color=#6C86C0>" + deHtml( toString ) + "</font><br><b>" +
                 i18n( "Cc" ) + ": </b>" + deHtml( ccString ) + "<br>"
                 "<b>" + i18n( "Date" ) + ": </b> " +  m_mail[3] +
                 "</td></tr></table><font>";

    if ( !m_showHtml )
    {
        browser->setText( QString( m_mailHtml) + deHtml( m_mail[2] ) + "</font></html>" );
    }
    else
    {
        browser->setText( QString( m_mailHtml) + m_mail[2] + "</font></html>" );
    }
    // remove later in favor of a real handling
    m_gotBody = true;
}


ViewMail::~ViewMail()
{
    m_recMail->Wrapper()->cleanMimeCache();
    hide();
}

void ViewMail::hide()
{
    QWidget::hide();

    if (_inLoop)
    {
        _inLoop = false;
        qApp->exit_loop();

    }

}

void ViewMail::exec()
{
    show();

    if (!_inLoop)
    {
        _inLoop = true;
        qApp->enter_loop();
    }

}

QString ViewMail::deHtml(const QString &string)
{
    QString string_ = string;
    string_.replace(QRegExp("&"), "&amp;");
    string_.replace(QRegExp("<"), "&lt;");
    string_.replace(QRegExp(">"), "&gt;");
    string_.replace(QRegExp("\\n"), "<br>");
    return string_;
}

void ViewMail::slotReply()
{
    if (!m_gotBody)
    {
        QMessageBox::information(this, i18n("Error"), i18n("<p>The mail body is not yet downloaded, so you cannot reply yet."), i18n("Ok"));
        return;
    }

    QString rtext;
    rtext += QString("* %1 wrote on %2:\n")        // no i18n on purpose
             .arg(  m_mail[0] )
             .arg( m_mail[3] );

    QString text = m_mail[2];
    QStringList lines = QStringList::split(QRegExp("\\n"), text);
    QStringList::Iterator it;
    for (it = lines.begin(); it != lines.end(); it++)
    {
        rtext += "> " + *it + "\n";
    }
    rtext += "\n";

    QString prefix;
    if ( m_mail[1].find(QRegExp("^Re: .*$")) != -1) prefix = "";
    else prefix = "Re: ";                // no i18n on purpose

    Settings *settings = new Settings();
    ComposeMail composer( settings ,this, 0, true);
    if (m_recMail->Replyto().isEmpty()) {
        composer.setTo(m_recMail->getFrom());
    } else {
        composer.setTo(m_recMail->Replyto());
    }
    composer.setSubject( prefix + m_mail[1] );
    composer.setMessage( rtext );
    composer.setInReplyTo(m_recMail->Msgid());
    composer.setCharset( m_body->getCharset()  );

    if ( QDialog::Accepted == KApplication::execDialog( &composer ) )
    {
        m_recMail->Wrapper()->answeredMail(m_recMail);
    }
    delete settings;
}
  
void ViewMail::slotForward()
{
    if (!m_gotBody)
    {
        QMessageBox::information(this, i18n("Error"), i18n("<p>The mail body is not yet downloaded, so you cannot forward yet."), i18n("Ok"));
        return;
    }

    QString ftext;
    ftext += QString("\n----- Forwarded message from %1 -----\n\n")
             .arg( m_mail[0] );
    if (!m_mail[3].isNull())
        ftext += QString("Date: %1\n")
                 .arg( m_mail[3] );
    if (!m_mail[0].isNull())
        ftext += QString("From: %1\n")
                 .arg( m_mail[0] );
    if (!m_mail[1].isNull())
        ftext += QString("Subject: %1\n")
                 .arg( m_mail[1] );

    ftext += QString("\n%1\n")
             .arg( m_mail[2]);

    ftext += QString("----- End forwarded message -----\n");

    Settings *settings = new Settings();
    ComposeMail composer( settings ,this, 0, true);
    composer.setSubject( "Fwd: " + m_mail[1] );
    composer.setMessage( ftext );
    if ( QDialog::Accepted == KApplication::execDialog( &composer ))
    {
    }
}

void ViewMail::slotDeleteMail( )
{
    if ( QMessageBox::warning(this, i18n("Delete Mail"), QString( i18n("<p>Do you really want to delete this mail? <br><br>" ) + m_mail[0] + " - " + m_mail[1] ) , QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
    {
        emit deleteAndDisplayNextMail( this);
        //m_recMail->Wrapper()->deleteMail( m_recMail );
        //hide();
        deleted = true;
    }
}

MailImageDlg::MailImageDlg(const QString&fname,QWidget *parent, const char *name, bool modal, WFlags f)
    : QDialog(parent,name,modal)
{
    QVBoxLayout*dlglayout = new QVBoxLayout(this);
    dlglayout->setSpacing(2);
    dlglayout->setMargin(1);
    //m_imageview = new Opie::MM::OImageScrollView(this);
    //dlglayout->addWidget(m_imageview);
}

MailImageDlg::~MailImageDlg()
{
}

void MailImageDlg::setName(const QString&fname)
{
    qDebug("viewmail.cpp: MailImageDlg::setName Pending");
    // m_imageview->setImage(fname);
}

