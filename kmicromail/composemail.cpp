// CHANGED 2004-08-06 Lutz Rogowski


#ifdef DESKTOP_VERSION
#include <qapplication.h>
#include <kabc/addresseedialog.h>
#include <kabc/stdaddressbook.h>
#include <kabc/addressee.h>
#else
#include <qpe/qpeapplication.h>
#endif //DESKTOP_VERSION
#include <libkdepim/externalapphandler.h>

#include "koprefs.h"
#include <klocale.h>
#include <kglobal.h>

#ifdef  MINIKDE_KDIALOG_H
#undef MINIKDE_KDIALOG_H
#endif


#include "composemail.h"

#include <libmailwrapper/smtpwrapper.h>
#include <libmailwrapper/storemail.h>
#include <libmailwrapper/abstractmail.h>
#include <libmailwrapper/mailtypes.h>

/* OPIE */
//#include <opie2/ofiledialog.h>
//#include <opie2/odebug.h>
#include <kfiledialog.h>
//#include <qpe/resource.h>
#include <qpe/global.h>
//#include <qpe/contact.h>


#include <qcombobox.h>
#include <qcheckbox.h>
#include <qiconset.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlistview.h>

//using namespace Opie::Core;
//using namespace Opie::Ui;
ComposeMail::ComposeMail( Settings *sett, QWidget *parent, const char *name, bool modal )
    : ComposeMailUI( parent, name, modal )
{

    topLevelWidget()->setCaption( i18n("Compose mail - Click OK to send ->") );
    mPickLineEdit = 0;
    mEncoding = KOPrefs::instance()->mCurrentCodeName;
    //managed from opiemail now
    //connect(ExternalAppHandler::instance(), SIGNAL(receivedNameEmailUidListEvent(const QString&, const QStringList&, const QStringList&, const QStringList&)),
    //     this, SLOT(insertAttendees(const QString&, const QStringList&, const QStringList&, const QStringList&)));
    settings = sett;
    m_replyid = ""; 
    if ( KOPrefs::instance()->mUseKapi) {
    KConfig config( locateLocal("config", "kabcrc") );
    config.setGroup( "General" );
    QString whoami_uid =  config.readEntry( "WhoAmI" );

    if ( whoami_uid.isEmpty() ) {
        QMessageBox::information( 0, i18n( "Hint" ),
                                  i18n( "Please apply\n\"Set Who Am I\"\nin KA/Pi to get the from\nfield automatically filled out!\n" ),
                                  i18n( "Ok" ) );
        
       
        fillSettings(); 
    } else
         ExternalAppHandler::instance()->requestDetailsFromKAPI("", "sendbacklist", whoami_uid);

 
#ifdef DESKTOP_VERSION
    KABC::Addressee con =  KABC::StdAddressBook::self()->whoAmI( );
    QStringList mails = con.emails();
    QString defmail = con.preferredEmail();
    if (  mails.count() == 0)
         QMessageBox::information( 0, i18n( "Hint" ),
                                  i18n( "Please apply\n\"Set Who Am I\"\nin KA/Pi to get the from\nfield automatically filled out!\n" ),
                                  i18n( "Ok" ) );
    if (defmail.length()!=0) {
        fromBox->insertItem(defmail);
    }
    QStringList::ConstIterator sit = mails.begin();
    for (;sit!=mails.end();++sit) {
        if ( (*sit)==defmail)
            continue;
        fromBox->insertItem((*sit));
    }
    senderNameEdit->setText(con.formattedName());
#endif

    } else {
        fillSettings();
    }
    checkBoxLater->setChecked( KOPrefs::instance()->mSendLater );

    attList->addColumn( i18n( "Name" ) );
    attList->addColumn( i18n( "Size" ) );
    QList<Account> accounts = settings->getAccounts();

    if ( QApplication::desktop()->width() < 320 )
        smtpAccountBox->setMaximumWidth( 80 );
    Account *it;
    for ( it = accounts.first(); it; it = accounts.next() ) {
        if ( it->getType()==MAILLIB::A_SMTP ) {
           SMTPaccount *smtp = static_cast<SMTPaccount *>(it);
           smtpAccountBox->insertItem( smtp->getAccountName() );
           smtpAccounts.append( smtp );
        }
    }
    connect( toButton, SIGNAL( clicked() ), SLOT( pickAddressTo() ) );
    connect( ccButton, SIGNAL( clicked() ), SLOT( pickAddressCC() ) );
    connect( bccButton, SIGNAL( clicked() ), SLOT( pickAddressBCC() ) );
    connect( replyButton, SIGNAL( clicked() ), SLOT( pickAddressReply() ) );
    connect( addButton, SIGNAL( clicked() ), SLOT( addAttachment() ) );
    connect( deleteButton, SIGNAL( clicked() ), SLOT( removeAttachment() ) );
    connect( SaveButton, SIGNAL( clicked() ), SLOT( saveAsDraft()) );
    mMail = 0;
    warnAttach = true; 
    QIconSet icon;
    //icon = SmallIcon("fileexport");
    icon = SmallIcon("filesave");
    SaveButton->setIconSet (icon ) ;
    if ( QApplication::desktop()->width() < 320 ) {
        SaveButton->setText ("") ;
        SaveButton->setMaximumSize ( SaveButton->sizeHint().height(),SaveButton->sizeHint().height()) ;
    }
    else
        SaveButton->setText (i18n("Save"));
#ifndef DESKTOP_VERSION
    QPEApplication::setStylusOperation( message, QPEApplication::RightOnHold );
    QPEApplication::setStylusOperation( senderNameEdit, QPEApplication::RightOnHold );
    QPEApplication::setStylusOperation( subjectLine, QPEApplication::RightOnHold );
#endif
    message->setFont ( KOPrefs::instance()->mComposeFont );
    message->setWordWrap (QMultiLineEdit::WidgetWidth);
    if ( smtpAccounts.count() > 0 ) {
        fillValues( smtpAccountBox->currentItem() );
    } else {
        QMessageBox::information( 0, i18n( "Problem" ),
                                  i18n( "Please create an\nSMTP account first.\nThe SMTP is needed\nfor sending mail.\n" ),
                                  i18n( "Ok" ) );
        return;
    } 
    connect( smtpAccountBox, SIGNAL( activated(int) ), SLOT( fillValues(int) ) );
    connect( SaveSigButton, SIGNAL( clicked() ), SLOT( saveSig()) );
    if ( smtpAccountBox->count())
        fillValues(0);
#ifdef DESKTOP_VERSION
    if ( QApplication::desktop()->width() <= 800 ) {
        resize( QApplication::desktop()->size() );
    } else
        resize( 800, 600 ); 
    if (   QApplication::desktop()->width() >= 640 )
        senderNameEdit->setFixedWidth( 200  );
#endif
}

void ComposeMail::fillSettings()
{
    if ( QApplication::desktop()->width() < 320 )
        fromBox->setMaximumWidth( 100 );
    QStringList mailList = QStringList::split(";",KOPrefs::instance()->mEmail);
    QStringList::ConstIterator sit = mailList.begin();
    int pref = 0;
    for (;sit!=mailList.end();++sit) {
        fromBox->insertItem((*sit));
    }
    senderNameEdit->setText(KOPrefs::instance()->mName);
}
void ComposeMail::saveSig()
{
    if ( smtpAccountBox->count()) {
        int cur = smtpAccountBox->currentItem ();
        SMTPaccount *smtp = smtpAccounts.at( cur );
        if ( smtp )
            smtp->setSignature( sigMultiLine->text());
    }
}
void ComposeMail::saveAsDraft()
{
  
    Opie::Core::OSmartPointer<Mail> mail= new Mail();
        mail->setMail(fromBox->currentText());
        mail->setTo( toLine->text() );
        mail->setName(senderNameEdit->text());
        mail->setCC( ccLine->text() );
        mail->setBCC( bccLine->text() );
        mail->setReply( replyLine->text() );
        mail->setSubject( subjectLine->text() );
        if (!m_replyid.isEmpty()) {
            QStringList ids;
            ids.append(m_replyid);
            mail->setInreply(ids);
        }
        QString txt = message->text();
        if ( !sigMultiLine->text().isEmpty() ) {
            txt.append( "\n--\n" );
            txt.append( sigMultiLine->text() );
        }
        mail->setMessage( txt );
        mail->setCharset (mEncoding);
        /* only use the default drafts folder name! */
        Storemail wrapper(AbstractMail::draftFolder());
        wrapper.storeMail(mail);

        AttachViewItem *it = (AttachViewItem *) attList->firstChild();
        /* attachments we will ignore! */
        if ( it != 0 ) {
            if ( warnAttach )
            QMessageBox::warning(0,i18n("Store message"),
                i18n("<center>Attachments will not be stored in \"Draft\" folder</center>"));
            warnAttach = false;
        }
        setStatus( i18n("Mail saved as draft!") );
}
void  ComposeMail::clearStatus()
{
    topLevelWidget()->setCaption( i18n("Compose mail - Click OK to send ->") );
}
void  ComposeMail::setStatus( QString status )
{
    topLevelWidget()->setCaption( status );
    QTimer::singleShot ( 10000, this, SLOT( clearStatus() ) ) ;
}
void ComposeMail::pickAddress( )
{

    QLineEdit *line = mPickLineEdit;
    if ( line == 0 )
        return;
#ifdef DESKTOP_VERSION
    //qDebug(" ComposeMail::pickAddress ");
    QString names ;//= AddressPicker::getNames();

    KABC::Addressee::List list = KABC::AddresseeDialog::getAddressees(this);
    uint i=0;
    for (i=0; i < list.count(); i++) {
        if ( !list[i].preferredEmail().isEmpty()) {
            if ( ! names.isEmpty() )
                names+= ",";
            names+=  "\""+list[i].realName() +"\"<" +list[i].preferredEmail() +">";
          
        }
    }


    if ( line->text().isEmpty() ) {
        line->setText( names );
    } else if ( !names.isEmpty() ) {
        line->setText( line->text() + ", " + names );
    }
#else
    bool res = ExternalAppHandler::instance()->requestNameEmailUidListFromKAPI("QPE/Application/ompi", this->name() /* name is here the unique uid*/);
    // the result should now arrive through method insertAttendees
#endif
}
//the map includes name/email pairs, that comes from Ka/Pi
void  ComposeMail::insertAttendees(const QString& uid,const QStringList& nameList,const QStringList& emailList,const QStringList& uidList)
{
    //qDebug("ComposeMail::insertAttendees ");
    raise();

    QString UID = uid;
    if ( uid.left( 18 ) == (this->name() +QString("pick2")) ) {
        mPickLineEdit = toLine;
        UID = this->name();
        subjectLine->setText( uid.mid( 18 ) );
    }
    //qDebug("ccc %s %s ", uid.latin1(), this->name());
    if ( mPickLineEdit == 0 ) { //whoami received
        QString defmail = uidList[0];
        if (  emailList.count() == 0 )
            QMessageBox::information( 0, i18n( "Hint" ),
                                      i18n( "Please apply\n\"Set Who Am I\"\nin KA/Pi to get the from\nfield automatically filled out!\n" ),
                                      i18n( "Ok" ) );
        if (defmail.length()!=0) {
            fromBox->insertItem(defmail);
        }
        QStringList::ConstIterator sit = emailList.begin();
        int pref = 0;
        for (;sit!=emailList.end();++sit) {
            if ( (*sit)==defmail)
                continue;
            fromBox->insertItem((*sit));
        }
        senderNameEdit->setText(nameList[0]);
        return;
    }
    QString names ;
    QLineEdit *line = mPickLineEdit;
    if (UID == this->name())
        {
            for ( int i = 0; i < nameList.count(); i++)
                {
                    QString _name = nameList[i];
                    QString _email = emailList[i];
                    QString _uid = uidList[i];
                    if ( ! _email.isEmpty() ) {
                        if ( ! names.isEmpty() )
                            names+= ",";
                        names+=  "\""+_name +"\"<" +_email +">";
                    }
                }
        }
    if ( line->text().isEmpty() ) {
        line->setText( names );
    } else if ( !names.isEmpty() ) {
        line->setText( line->text() + ", " + names );
    }
}

void ComposeMail::setTo( const QString & to )
{
    toLine->setText( to );
}

void ComposeMail::setSubject( const QString & subject )
{
 subjectLine->setText( subject );
}

void ComposeMail::setInReplyTo( const QString & messageId )
{
    m_replyid = messageId;
}

void ComposeMail::setMessage( const QString & text )
{
  message->setText( text );
}


void ComposeMail::pickAddressTo()
{
    mPickLineEdit = toLine;
    pickAddress( );
}

void ComposeMail::pickAddressCC()
{
    mPickLineEdit = ccLine;
    pickAddress( );
}

void ComposeMail::pickAddressBCC()
{
    mPickLineEdit =  bccLine;
    pickAddress( );
}

void ComposeMail::pickAddressReply()
{
    mPickLineEdit = replyLine;
    pickAddress( );
}

void ComposeMail::fillValues( int  current )
{
#if 0
    SMTPaccount *smtp = smtpAccounts.at( current );
    ccLine->clear();
    if ( smtp->getUseCC() ) {
        ccLine->setText( smtp->getCC() );
    }
    bccLine->clear();
    if ( smtp->getUseBCC() ) {
        bccLine->setText( smtp->getBCC() );
    }
    replyLine->clear();
    if ( smtp->getUseReply() ) {
        replyLine->setText( smtp->getReply() );
    }
#endif
    SMTPaccount *smtp = smtpAccounts.at( current );
    if ( smtp )
        sigMultiLine->setText( smtp->getSignature() );
}

void ComposeMail::slotAdjustColumns()
{
    int currPage = tabWidget->currentPageIndex();

    tabWidget->showPage( attachTab );
    attList->setColumnWidth( 0, attList->visibleWidth() - 80 );
    attList->setColumnWidth( 1, 80 );

    tabWidget->setCurrentPage( currPage );
}

void ComposeMail::addAttachment()
{
    QString lnk = KFileDialog::getOpenFileName( "", "Add Attachment", this );
    if ( !lnk.isEmpty() ) {
        Attachment *att = new Attachment( lnk );
        (void) new AttachViewItem( attList, att );
    }
}

void ComposeMail::removeAttachment()
{
    if ( !attList->currentItem() ) {
        QMessageBox::information( this, i18n( "Error" ),
                                  i18n( "<p>Please select a File.</p>" ),
                                  i18n( "Ok" ) );
    } else {
        attList->takeItem( attList->currentItem() );
    }
}

void ComposeMail::accept()
{
    if ( smtpAccountBox->count() == 0 ) { 
       
        reject();
        return;
    }

    if (! checkBoxLater->isChecked() ) {
        int yesno = QMessageBox::warning(0, subjectLine->text().left ( 25 ),
                                         i18n("Send this message?"),
                                         i18n("Yes"),
                                         i18n("Cancel"));
        
        if (yesno == 1) {
            return;
        }
    }
#if 0
    odebug << "Sending Mail with "
           << smtpAccounts.at( smtpAccountBox->currentItem() )->getAccountName() << oendl;
#endif
    Opie::Core::OSmartPointer<Mail> mail=new Mail;

    SMTPaccount *smtp = smtpAccounts.at( smtpAccountBox->currentItem() );
    mail->setMail(fromBox->currentText());

    if ( !toLine->text().isEmpty() ) {
        mail->setTo( toLine->text() );
    } else {
        QMessageBox::warning(0,i18n("Sending mail"),
            i18n("No Receiver spezified" ) );
        return;
    }  
   
    mail->setName(senderNameEdit->text());
    mail->setCC( ccLine->text() );
    mail->setBCC( bccLine->text() );
    mail->setReply( replyLine->text() );
    mail->setSubject( subjectLine->text() );
    if (!m_replyid.isEmpty()) {
        QStringList ids;
        ids.append(m_replyid);
        mail->setInreply(ids);
    }
    QString txt = message->text();
    if ( !sigMultiLine->text().isEmpty() ) {
        txt.append( "\n--\n" );
        txt.append( sigMultiLine->text() );
    }
    mail->setMessage( txt );
    mail->setCharset (mEncoding);
    AttachViewItem *it = (AttachViewItem *) attList->firstChild();
    while ( it != 0 ) {
        mail->addAttachment( it->getAttachment() );
        it = (AttachViewItem *) it->nextSibling();
    }

    SMTPwrapper wrapper( smtp );
    if ( wrapper.sendMail( mail,checkBoxLater->isChecked() ) )
        setStatus( tr ("Mail sent"));
    else {
        setStatus( tr ("Error: Something went wrong. Nothing sent"));
        return;
    }
        

    QDialog::accept();
}

void ComposeMail::reject()
{
    //qDebug("ComposeMail::reject() ");
    int yesno = QMessageBox::warning(0,i18n("Store message?"),
                                     i18n("Store message into drafts?\n"),
                                     i18n("Yes"),
                                     i18n("No"));

    //qDebug("button %d ", yesno);
    if (yesno == 0) {
        if ( toLine->text().isEmpty() ) {
            QMessageBox::warning(0,i18n("Sending mail"),
                                 i18n("No Receiver spezified" ) );
            return;
        }  
        saveAsDraft();
    }
    if (yesno == 2) {
        qDebug("return ");
        return;
    }
    QDialog::reject();
}

void ComposeMail::setCharset(const QString& charset)
{
    if ( !charset.isEmpty() )
        mEncoding = charset;
    qDebug("ComposeMail::setCharset %s ", mEncoding.latin1());
}
ComposeMail::~ComposeMail()
{
}

void ComposeMail::reEditMail(const RecMailP&current)
{
    RecMailP data = current;
    RecBodyP body = data->Wrapper()->fetchBody(current);

    message->setText(body->Bodytext());
    subjectLine->setText( data->getSubject());
    toLine->setText(data->To().join(","));
    ccLine->setText(data->CC().join(","));
    bccLine->setText(data->Bcc().join(","));
    replyLine->setText(data->Replyto());
    setCharset(body->getCharset());
}

AttachViewItem::AttachViewItem( QListView *parent, Attachment *att )
    : QListViewItem( parent )
{
    attachment = att;
    if ( !attachment->getPixmap().isNull() )
        setPixmap( 0,attachment->getPixmap() );
    setText( 0, att->getName().isEmpty() ? att->getFileName() : att->getName() );
    setText( 1, QString::number( att->getSize() ) );
}

