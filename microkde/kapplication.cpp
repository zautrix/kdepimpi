#include <stdlib.h>
#include <stdio.h>

#include "kapplication.h"
#include "ktextedit.h"
#include <qapplication.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qtextbrowser.h>
#include <qregexp.h>

int KApplication::random()
{
  return rand();
}

//US
QString KApplication::randomString(int length)
{
   if (length <=0 ) return QString::null;

   QString str;
   while (length--)
   {
      int r=random() % 62;
      r+=48;
      if (r>57) r+=7;
      if (r>90) r+=6;
      str += char(r);
      // so what if I work backwards?
   }
   return str;
}
int KApplication::execDialog( QDialog* d )
{
    if (QApplication::desktop()->width() <= 640 )    
        d->showMaximized();
    else
        ;//d->resize( 800, 600 );
    return d->exec();
}
void KApplication::showLicence()
{
   KApplication::showFile( "KDE-Pim/Pi licence", "kdepim/licence.txt" );
}

void KApplication::testCoords( int* x, int* y, int* wid, int * hei )
{
    int dWid = QApplication::desktop()->width() ;
    int dHei = QApplication::desktop()->height();
    if ( *x < 0 ) *x = 0;
    if (  *y <  20 ) *y =  20 ;
    if ( *x + *wid > dWid ) {
        *x = 0;
        if ( *wid > dWid )
            *wid = dWid; 
    }
    if ( *y + *hei > dHei ) {
        *y = 20;
        if ( *hei > dHei )
            *hei = dHei; 
    }
}
void KApplication::showFile(QString caption, QString fn)
{
    QString text;
    QString fileName;
#ifndef DESKTOP_VERSION
   fileName = getenv("QPEDIR");
   fileName += "/pics/" + fn ;
#else
   fileName  = qApp->applicationDirPath () + "/" + fn;
#endif 
    QFile file( fileName );
    if (!file.open( IO_ReadOnly ) ) {
        return ;
    } 
    QTextStream ts( &file );
    text = ts.read();
    file.close();
    KApplication::showText( caption, text );
  
}
 
bool KApplication::convert2latin1(QString fileName)
{
    QString text;
    QFile file( fileName );
    if (!file.open( IO_ReadOnly ) ) {
        return false;
        
    } 
    QTextStream ts( &file );
    ts.setEncoding( QTextStream::UnicodeUTF8 );
    text = ts.read();
    file.close();  
    if (!file.open( IO_WriteOnly ) ) {
        return false;
    }
    QTextStream tsIn( &file );
    tsIn.setEncoding( QTextStream::Latin1 );
    tsIn << text.latin1();
    file.close(); 
    return true; 
 

}
void KApplication::showText(QString caption, QString text)
{
    QDialog dia( 0, "name", true ); ;
    dia.setCaption( caption );
    QVBoxLayout* lay = new QVBoxLayout( &dia );
    lay->setSpacing( 3 );
    lay->setMargin( 3 );
    KTextEdit tb ( &dia );
    tb.setWordWrap( QMultiLineEdit::WidgetWidth );
    lay->addWidget( &tb ); 
    tb.setText( text );
#ifdef DESKTOP_VERSION
    dia.resize( 640, 480);
#else
    dia.showMaximized();
#endif
    dia.exec();

}

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qdir.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include "kglobal.h"
#include "klocale.h"

class KBackupPrefs : public QDialog
{
  public:
    KBackupPrefs( QString message , QWidget *parent=0, const char *name=0 ) :
      QDialog( parent, name, true ) 
    {
        setCaption( i18n("Backup Failed!") );
        QVBoxLayout* lay = new QVBoxLayout( this );
        lay->setSpacing( 3 );
        lay->setMargin( 3 );
        QLabel * lab  = new QLabel( message, this );
        lay->addWidget( lab ); 
        QButtonGroup* format = new  QButtonGroup( 1, Qt::Horizontal, i18n("Choose action"), this );
        lay->addWidget( format ); 
        format->setExclusive ( true ) ;
        vcal = new QRadioButton(i18n("Try again now"), format );
        tcal = new QRadioButton(i18n("Try again later"), format );
        ical = new QRadioButton(i18n("Try again tomorrow"), format );
        ocal = new QRadioButton(i18n("Disable backup"), format );
        vcal->setChecked( true );
        QPushButton * ok = new QPushButton( i18n("OK"), this );
        lay->addWidget(ok );
        connect ( ok,SIGNAL(clicked() ),this , SLOT ( accept() )  );
    }
      
    bool again() { return vcal->isChecked(); }
    bool later() { return tcal->isChecked(); }
    bool againTomorrow() { return ical->isChecked(); }
private:
    QRadioButton* vcal, *ical, *ocal, *tcal;
};
int KApplication::createBackup( QString fn, QString dp, int numBup )
{
    if ( numBup < 1) return 3;
    int ret = 3;
    //qDebug("KApplication::createBackup %s --- %s --- %d", fn.latin1(), dp.latin1(), numBup);
    QDir  bupDir ( dp );
    bool tryAgain = true;
    while ( tryAgain ) {
        if ( !bupDir.exists() ) {
            QString bd = dp.right(25);
            if ( dp.length() > 25 )
                bd  = "..." + bd;
            KBackupPrefs noDir( i18n("<b>Backup directory does not exist: </b>") +  bd);
            if ( !noDir.exec() ) return 3;
            if ( noDir.againTomorrow() ) {
                return 0;
            } else if ( noDir.later() ) {
                return 3;
            } else if ( !noDir.again() ) {
                return 2;
            }
        } else {
            tryAgain = false;
        }
    }
    // we have a valid dir!
    QStringList allFileList = bupDir.entryList(QDir::Files);
    QFileInfo fileInfo ( fn );
    QString fName = fileInfo.fileName ();
    QStringList fileList;

    int i;
    for ( i = 0; i < allFileList.count(); ++i ) {
        QString fi = allFileList[i];
        if ( fi. find( fName ) > -1 )
            fileList.append( fi );
    }
    qDebug("KApp: %d backup files exist ", fileList.count());
    int count = fileList.count();
    fileList.sort();
    int remCount = 0;
    while ( count >= numBup ) {
        QString fnr = dp + "/"+fileList[remCount];
        QFile::remove( fnr );
        --count;
        ++remCount;
    }
    QDateTime mRunStart =  QDateTime::currentDateTime();
    QString file = "%1%2%3-%4%5%6-";
    file = file.arg( mRunStart.date().year(), 4).arg( mRunStart.date().month(),2 ).arg( mRunStart.date().day(), 2 ).arg( mRunStart.time().hour(),2  ).arg( mRunStart.time().minute(),2 ).arg( mRunStart.time().second(),2 );
    file.replace ( QRegExp (" "), "0" ); 
    file += fName ;
    file = dp + file;
    QString command;
    int res = 0;
#ifdef _WIN32_
    command = "copy \""+ QDir::convertSeparators( fn)+ "\"  \""+QDir::convertSeparators(file)+ "\" ";
#else
    command = "cp "+ fn+ "  "+file;
#endif
    //qDebug("command %s ",command.latin1() );
    tryAgain  = true; 
    while ( tryAgain ) {
        res = system ( command.latin1() );
        qDebug("KApp: Copy result %d ", res);
        if ( res != 0 ) {
            KBackupPrefs noDir( i18n("<b>The backup copy command failed!</b>"));
            if ( !noDir.exec() ) return 3;
            if ( noDir.againTomorrow() ) {
                return 0;
            } else if ( noDir.later() ) {
                return 3;
            } else if ( !noDir.again() ) {
                return 2;
            }
        } else {
            tryAgain = false;
        }
    }
    return 1;
}
