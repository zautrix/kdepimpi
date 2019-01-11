/*
   Copyright (c) 2000 Matthias Elter <elter@kde.org>
   Copyright (c) 2003 Daniel Molkentin <molkentin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

*/

#include <qhbox.h>
#include <qvbox.h>
#include <qcursor.h>
#include <qlayout.h>

#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>
//US #include <klibloader.h>
#include <krun.h>
#include <kprocess.h>
#include <kglobalsettings.h>

#include "kcmultidialog.h"
//US #include "kcmultidialog.moc"
//US #include "kcmoduleloader.h"

KCMultiDialog::KCMultiDialog(const QString& baseGroup, QWidget *parent, const char *name, bool modal)
  : KDialogBase(IconList, i18n("Configure"), Default |Cancel | Apply | Ok, Ok,
                parent, name, modal, true), d(0L)
{   
    setFont( KGlobalSettings::generalMaxFont() );
    enableButton(Apply, false);
    //connect(this, SIGNAL(aboutToShowPage(QWidget *)), this, SLOT(slotAboutToShow(QWidget *)));

  connect( this, SIGNAL( defaultClicked() ), SLOT( slotDefault() ) );

  _baseGroup = baseGroup;
  mMainWidget = new KJanusWidget( this, "JanusWidget", KJanusWidget::Tabbed );
  setMainWidget(mMainWidget );
#ifdef DESKTOP_VERSION
    resize(640,480);
#else
    //resize(640,480);
    //setMaximumSize( KMIN(KGlobal::getDesktopWidth()-5, 640), KMIN(KGlobal::getDesktopHeight()-20, 480));
    resize(800,800);
    setMaximumSize( 800, 800 );
    //showMaximized();
#endif

}

KCMultiDialog::~KCMultiDialog()
{
//US    moduleDict.setAutoDelete(true);
}

void KCMultiDialog::slotDefault()
{

    int curPageIndex = activePageIndex();

    QPtrListIterator<KCModule> it(modules);
    for (; it.current(); ++it)
    {
       if (pageIndex((QWidget *)(*it)->parent()) == curPageIndex)
       {
          (*it)->defaults();
          clientChanged(true);
          return;
       }
    }

}
void KCMultiDialog::accept()
{
    slotOk();
}
void KCMultiDialog::slotApply()
{
    QPtrListIterator<KCModule> it(modules);
    for (; it.current(); ++it)
      (*it)->save();
    clientChanged(false);

    emit applyClicked();

}


void KCMultiDialog::slotOk()
{
qDebug("KCMultiDialog::slotOk clicked");

    QPtrListIterator<KCModule> it(modules);
    for (; it.current(); ++it)
      (*it)->save();
    QDialog::accept();

    emit okClicked();
}

void KCMultiDialog::slotHelp()
{
/*US
    KURL url( KURL("help:/"), _docPath );

    if (url.protocol() == "help" || url.protocol() == "man" || url.protocol() == "info") {
        KProcess process;
        process << "khelpcenter"
                << url.url();
        process.start(KProcess::DontCare);
		process.detach();
    } else {
        new KRun(url);
    }
*/
}

void KCMultiDialog::clientChanged(bool state)
{
    enableButton(Apply, state);
}

/*US
void KCMultiDialog::addModule(const QString& path, bool withfallback)
{
    kdDebug(1208) << "KCMultiDialog::addModule " << path << endl;

    KCModuleInfo info(path, _baseGroup);

    QHBox* page = addHBoxPage(info.moduleName(), info.comment(),
                              KGlobal::iconLoader()->loadIcon(info.icon(), KIcon::Desktop, KIcon::SizeMedium));
    if(!page) {
        KCModuleLoader::unloadModule(info);
        return;
    }
    moduleDict.insert(page, new LoadInfo(path, withfallback));
    if (modules.isEmpty())
       slotAboutToShow(page);
}
*/
QVBox * KCMultiDialog::getNewVBoxPage( const QString & modulename )
{
    QVBox *page = mMainWidget->addVBoxPage(modulename , QString::null,QPixmap() );
    return page;

}
//US special method for microkde. We dop noty want to load everything dynamically.
void KCMultiDialog::addModule(KCModule* module ) //, const QString& modulename, const QString& iconname)
{

    modules.append(module);
    connect(module, SIGNAL(changed(bool)), this, SLOT(clientChanged(bool)));
//US
  	module->load();


}

void KCMultiDialog::slotAboutToShow(QWidget *page)
{
/*US
    LoadInfo *loadInfo = moduleDict[page];
    if (!loadInfo)
       return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    moduleDict.remove(page);

    KCModuleInfo info(loadInfo->path, _baseGroup);

    KCModule *module = KCModuleLoader::loadModule(info, loadInfo->withfallback);

    if (!module)
    {
        QApplication::restoreOverrideCursor();
        KCModuleLoader::showLastLoaderError(this);
        delete loadInfo;
        return;
    }

    module->reparent(page,0,QPoint(0,0),true);
    connect(module, SIGNAL(changed(bool)), this, SLOT(clientChanged(bool)));
    //setHelp( docpath, QString::null );
    _docPath = info.docPath();
    modules.append(module);

    //KCGlobal::repairAccels( topLevelWidget() );

    delete loadInfo;

    QApplication::restoreOverrideCursor();
*/

qDebug("KCMultiDialog::slotAboutToShow not implemented");
}


bool KCMultiDialog::showPage( int index )
{
  return(mMainWidget->showPage(index) );
}


int KCMultiDialog::activePageIndex() const
{
  return( mMainWidget->activePageIndex() );
}


int KCMultiDialog::pageIndex( QWidget *widget ) const
{
  return( mMainWidget->pageIndex( widget) );
}
