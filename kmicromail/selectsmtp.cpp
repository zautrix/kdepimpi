#include "selectsmtp.h"
#include <libmailwrapper/mailwrapper.h>

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <klocale.h>
selectsmtp::selectsmtp(QWidget* parent, const char* name, bool modal, WFlags fl)
    : selectstoreui(parent,name,modal,fl)
{
    //m_smtpList.setAutoDelete(false);
    m_smtpList = 0;
    //headlabel->setText(i18n("<center>Select SMTP account to use</center>"));
    headlabel->hide();
    folderSelection->hide();
    folderLabel->hide();
    //accountlabel->setText(i18n("SMTP\nAccount:"));
    Line1->hide();
    newFoldersel->hide();
    newFolderedit->hide();
    newFolderLabel->hide();
    Line2->hide();
    selMove->hide();
    m_current_smtp = 0;
    setCaption(i18n("Select SMTP Account"));
}

selectsmtp::~selectsmtp()
{
}

void selectsmtp::slotAccountselected(int which)
{
    if (!m_smtpList || (unsigned)which>=m_smtpList->count() || which < 0) {
        m_current_smtp = 0;
        return;
    }
    m_current_smtp = m_smtpList->at(which);
}

void selectsmtp::setSelectionlist(QList<SMTPaccount>*list)
{
    m_smtpList = list;
    accountSelection->clear();
    if (!m_smtpList || m_smtpList->count()==0) {
        accountSelection->setEnabled(false);
        return;
    }
    accountSelection->setEnabled(true);
    for (unsigned i = 0; m_smtpList!=0 && i < m_smtpList->count(); ++i) {
        accountSelection->insertItem( m_smtpList->at(i)->getAccountName());
    }
    m_current_smtp = m_smtpList->at(0);
}

SMTPaccount*selectsmtp::selected_smtp()
{
    return m_current_smtp;
}
