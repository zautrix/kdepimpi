
#include "composemail.h"

/* OPIE */
//#include <opie2/ocontactaccess.h>
//#include <opie2/opimcontact.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qlistbox.h>

/* STD */
#include <stdlib.h>

AddressPicker::AddressPicker( QWidget *parent, const char *name, bool modal, WFlags flags )
        : AddressPickerUI( parent, name, modal, flags )
{
    okButton->setIconSet( Resource::loadPixmap( "enter" ) );
    cancelButton->setIconSet( Resource::loadPixmap( "editdelete" ) );

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(close()));
    qDebug("AddressPicker::AddressPicker pending access KA/PI ");
#if 0
    Opie::OPimContactAccess::List::Iterator it;

    QString lineEmail, lineName, contactLine;
    /* what name has to set here???? */
    Opie::OPimContactAccess m_contactdb("opiemail");

    QStringList mails;
    QString pre,suf;
    Opie::OPimContactAccess::List m_list = m_contactdb.sorted( true, 0, 0, 0 );
    for ( it = m_list.begin(); it != m_list.end(); ++it )
    {
        if ((*it).defaultEmail().length()!=0)
        {
            mails = (*it).emailList();
            if ((*it).fileAs().length()>0)
            {
                pre = "\""+(*it).firstName()+" "+(*it).lastName()+"\" <";
                suf = ">";
            }
            else
            {
                pre = "";
                suf = "";
            }
            QStringList::ConstIterator sit = mails.begin();
            for (;sit!=mails.end();++sit)
            {
                contactLine=pre+(*sit)+suf;
                addressList->insertItem(contactLine);
            }
        }
    }
    if ( addressList->count() <= 0 )
    {
#if 0
        // makes this realy sense??
        addressList->insertItem(
            i18n( "There are no entries in the addressbook." ) );
#endif
        addressList->setEnabled( false );
        okButton->setEnabled( false );
    }
    else
    {
        //        addressList->sort();
    }
#endif
}

void AddressPicker::accept()
{
    QListBoxItem *item = addressList->firstItem();
    QString names;

    while ( item )
    {
        if ( item->selected() )
            names += item->text() + ", ";
        item = item->next();
    }
    names.replace( names.length() - 2, 2, "" );

    if ( names.isEmpty() )
    {
        QMessageBox::information(this, i18n("Error"), i18n("<p>You have to select"
                                 " at least one address entry.</p>"), i18n("Ok"));
        return;
    }

    selectedNames = names;
    QDialog::accept();
}

QString AddressPicker::getNames()
{
    QString names = 0;

    AddressPicker picker(0, 0, true);

    picker.showMaximized();
    int ret = picker.exec();
    if ( QDialog::Accepted == ret )
    {
        return picker.selectedNames;
    }

    return 0;
}

