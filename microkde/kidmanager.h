#ifndef MINIKDE_KIDMANAGER_H
#define MINIKDE_KIDMANAGER_H

#include <qstring.h>

#include <qobject.h>
class KIdManager : public QObject
{
Q_OBJECT
  public:
    KIdManager( );
    static const QString setId (const QString& idString,const QString& id,const QString& idvalue ) ;
    static const QString getId (const QString& idString,const QString& id ) ;
    static const QString removeId (const QString& idString,const QString& id ) ;
    static const QString setCsum (const QString& idString,const QString& id,const QString& idvalue ) ;
    static const QString getCsum (const QString& idString,const QString& id ) ;
    static bool getNumbers (const QString& idString,const QString& id, int &startProf, int &startIDnum, int &startIDnumlen,int &startID, int& lenID, int &startCsum, int &lenCsum, int & endall) ;
 private:
  
private slots:
};


#endif
