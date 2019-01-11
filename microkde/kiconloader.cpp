
#include "kiconloader.h"
#include "kglobal.h"

#ifndef DESKTOP_VERSION_OEGEL
#include <qdir.h>
QPixmap KIconLoader::loadIcon( const QString& name, KIcon::Group, int,
                               int, QString *, bool ) const
{
    QPixmap pix; 
    QString file;
    file = iconPath() + name+".png";
    pix.load (  file );
    // qDebug("KIconLoader::loadIcon %s -----------",  file.latin1());
    return pix;
}
QIconSet KIconLoader::loadIconSet( const QString& name) const
{
    QPixmap pixmapLoader; 
    QString file;
    file = iconPath() + name+".png";
    pixmapLoader.load( file ); 
    //qDebug("KIconLoader::loadIconSet: %s ************", file.latin1() );
    QIconSet is ( pixmapLoader );
  return is;
}

QPixmap BarIcon( const QString &name )
{ 
    QPixmap pix;
    pix.load ( KGlobal::iconLoader()->iconPath()  + name +".png" );
    return pix;
}

QPixmap DesktopIcon( const QString &name, int )
{
    QPixmap pix;
    pix.load ( KGlobal::iconLoader()->iconPath()  + name +".png" );
    return pix;

}

QPixmap SmallIcon( const QString &name )
{ 
    QPixmap pixmapLoader; 
    QString file;
    file =KGlobal::iconLoader()->iconPath()  + name +".png";
    pixmapLoader.load( file );
    return pixmapLoader;
  
}

QPixmap SmallIconSet( const QString &name )
{
    QPixmap pixmapLoader; 
    QString file;
    file =KGlobal::iconLoader()->iconPath()  + name +".png";
    pixmapLoader.load( file ); 
    return pixmapLoader;
}


#else

#include <qpe/resource.h>
#include <kglobal.h>
QPixmap KIconLoader::loadIcon( const QString& name, KIcon::Group, int,
                               int, QString *, bool ) const
{
  QString px = this->iconPath() + "/" + name;
  
  QPixmap p = Resource::loadPixmap( px );
  QPixmap* pPtr = &p;
  if (pPtr == 0)
    qDebug("KIconLoader::loadIcon: %s not found", px.latin1());
  
  return p;
}

QIconSet KIconLoader::loadIconSet( const QString& name) const
{
  QString px = this->iconPath() + "/" + name;
  
  QIconSet is ;//= Resource::loadIconSet( px );
  QIconSet* isPtr = 0;//LR&is;
  if (isPtr == 0)
    qDebug("KIconLoader::loadIconSet: %s not foun", px.latin1());
  
  return is;
}

QPixmap BarIcon( const QString &name )
{
  QPixmap p = KGlobal::iconLoader()->loadIcon(name, KIcon::Desktop);
  return p;
}

QPixmap DesktopIcon( const QString &name, int )
{
  QPixmap p = KGlobal::iconLoader()->loadIcon(name, KIcon::Desktop);
  return p;
}

QPixmap SmallIcon( const QString &name )
{
  QPixmap p = KGlobal::iconLoader()->loadIcon(name, KIcon::Desktop);
  return p;
}

QPixmap SmallIconSet( const QString &name )
{
  QPixmap p = KGlobal::iconLoader()->loadIcon(name, KIcon::Desktop);
  return p;
}

#endif

//US    
QString KIconLoader::setIconPath( const QString &iconpath)
{
  QString _old = mIconpath;
  mIconpath = iconpath;

  return _old;
}
QString KIconLoader::iconPath(  const QString & name, int ) const
{
  return mIconpath + name + ".png";
}

QString KIconLoader::iconPath( /*US const QString &, int */) const
{
    // LR we set the path at startup
    // if (KGlobal::getDesktopSize() == KGlobal::Small)
    //return mIconpath + "/icons16";

  //Fall back to the defaultpath  
  return mIconpath;
}

