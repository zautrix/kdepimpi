#include <kfiledialog.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qapplication.h>

#ifndef DESKTOP_VERSION
//US orig#include <ofileselector.h>
#include <ofileselector_p.h>
QString KFileDialog::getSaveFileName( const QString & fn,
                                             const QString & cap , QWidget * par )
{
    QString retfile = "";
    QDialog dia ( par, "input-dialog", true ); 
    QVBoxLayout lay( &dia );
    lay.setMargin(7); 
    lay.setSpacing(7); 
    QString file = fn;
    if ( file.isEmpty() )
        file = QDir::homeDirPath()+"/*";
    QFileInfo fi ( file );
    OFileSelector o ( &dia,OFileSelector::FileSelector, OFileSelector::Save, fi.dirPath(true), fi.fileName() );
    QObject::connect (  &o, SIGNAL( ok() ), &dia, SLOT ( accept () ) );
    lay.addWidget( &o);
    // o.setNewVisible( true );
    // o.setNameVisible( true );
    dia.showMaximized();
    if ( cap.isEmpty() )
        dia.setCaption( file );
    else
        dia.setCaption( cap );
    int res = dia.exec();
    if ( res )
        retfile = o.selectedName();
    return retfile;
}

QString KFileDialog::getOpenFileName( const QString & fn,
                                             const QString & cap, QWidget * par )
{
   QString retfile = "";
    QDialog dia ( par, "input-dialog", true ); 
    // QLineEdit lab ( &dia ); 
    QVBoxLayout lay( &dia );
    lay.setMargin(7); 
    lay.setSpacing(7);
    QString file = fn;
    if ( file.isEmpty() )
        file = QDir::homeDirPath()+"/*";;
    QFileInfo fi ( file );
    OFileSelector o ( &dia,OFileSelector::FileSelector, OFileSelector::Open, fi.dirPath(true), fi.fileName() );
    QObject::connect (  &o, SIGNAL( ok() ), &dia, SLOT ( accept () ) );
    lay.addWidget( &o);
    dia.showMaximized();
    dia.setCaption( cap );
    int res = dia.exec();
    if ( res )
        retfile = o.selectedName();
    return retfile;
}
QString KFileDialog::getExistingDirectory( const QString & fn,
                                             const QString & cap, QWidget * par )
{
    return KFileDialog::getSaveFileName( fn, cap, par );
}
#else

#include <qfiledialog.h> 

QString KFileDialog::getSaveFileName( const QString & fn,
                                             const QString & cap , QWidget * par )
{
    return QFileDialog::getSaveFileName( fn, QString::null, par, "openfile", cap );
}
QString KFileDialog::getOpenFileName( const QString & fn,
                                             const QString & cap, QWidget * par )
{

    return QFileDialog::getOpenFileName( fn, QString::null, par, "openfile", cap );
}
QString KFileDialog::getExistingDirectory( const QString & fn,
                                             const QString & cap, QWidget * par )
{
    return QFileDialog::getExistingDirectory( fn, par, "exidtingdir", cap );
}
#endif

