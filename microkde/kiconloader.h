#ifndef MINIKDE_KICONLOADER_H
#define MINIKDE_KICONLOADER_H

#include <qpixmap.h>
#include <qstring.h>
//US
#include <qiconset.h>

class KIcon
{
  public:
    enum Group { NoGroup=-1, Desktop=0, Toolbar, MainToolbar, Small,
	         Panel, LastGroup, User };
    enum StdSizes { SizeSmall=16, SizeMedium=32, SizeLarge=48 };
    enum States { DefaultState, ActiveState, DisabledState, LastState };
};

class KIconLoader
{
  public:
    KIconLoader()
      : mIconpath(0) {}
      
    KIconLoader( const QString &iconpath )
      : mIconpath(iconpath) {}

//US    QPixmap loadIcon( const QString &name, int ); 

    QPixmap loadIcon(const QString& name, KIcon::Group group, int size=0,
		     int state=KIcon::DefaultState, QString *path_store=0,
		     bool canReturnNull=false) const;

//US    
    QString setIconPath( const QString &);
    QString iconPath( /*US const QString &, int */) const;
    QString iconPath( const QString &, int ) const;
    QIconSet loadIconSet( const QString &name) const; 
    
//US to make this class usable for different applications, we have to add a iconpathvariable
  private:
    QString mIconpath;
};

QPixmap BarIcon(const QString& name);

QPixmap DesktopIcon(const QString& name, int);

QPixmap SmallIcon(const QString& name);

QPixmap SmallIconSet( const QString &name );

#endif
