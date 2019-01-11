#include "kidmanager.h"

KIdManager::KIdManager()
{
   
}
// :profilename;12;id_withLen12;123456:
// 123456 is the csum
const QString KIdManager::setId (const QString& idString,const QString& prof,const QString& idvalue )
{
    int startProf;
    int startIDnum;
    int startIDnumlen;
    int startID;
    int lenID;
    int startCsum;
    int lenCsum;
    int endall;
    QString newIDString;
    if ( KIdManager::getNumbers (idString, prof, startProf, startIDnum, startIDnumlen,startID, lenID, startCsum, lenCsum, endall) ) {
        newIDString = idString.left(startIDnum ) + QString::number( idvalue.length() ) +";"+idvalue+ ";"+idString.mid(  startCsum );
    } else {
        newIDString = idString + prof+";"+ QString::number( idvalue.length() ) +";"+idvalue +";0:";
    }
    //qDebug("setID:profile:%s*retval:%s*idvalue:%s* ", prof.latin1(), newIDString.latin1() ,idvalue.latin1() );
    return newIDString;
}
const QString KIdManager::getId (const QString& idString,const QString& prof )
{
    int startProf;
    int startIDnum;
    int startIDnumlen;
    int startID;
    int lenID;
    int startCsum;
    int lenCsum;
    int endall;
    QString idval = "";
    if ( KIdManager::getNumbers (idString, prof, startProf, startIDnum, startIDnumlen,startID, lenID, startCsum, lenCsum, endall) )
        idval = idString.mid(  startID, lenID );

    //qDebug("getID:profile:%s*retval:%s*idstring:%s* ", prof.latin1(), idval.latin1() ,idString.latin1() );
    return idval;

}

const QString KIdManager::removeId (const QString& idString,const QString& prof )
{ 
    int startProf;
    int startIDnum;
    int startIDnumlen;
    int startID;
    int lenID;
    int startCsum;
    int lenCsum;
    int endall;
    QString newIDString;
    if ( KIdManager::getNumbers (idString, prof, startProf, startIDnum, startIDnumlen,startID, lenID, startCsum, lenCsum, endall) ) {
        newIDString = idString.left(startProf) + idString.mid(  endall+1 );
    } else {
        newIDString = idString;
    }
    //qDebug("removeID:profile:%s*retval:%s*oldidstring:%s* ", prof.latin1(), newIDString.latin1() ,idString.latin1() );
    return newIDString;
}

const QString KIdManager::setCsum (const QString& idString,const QString& prof,const QString& idCsum )
{
    int startProf;
    int startIDnum;
    int startIDnumlen;
    int startID;
    int lenID;
    int startCsum;
    int lenCsum;
    int endall;
    QString newIDString;
    if ( KIdManager::getNumbers (idString, prof, startProf, startIDnum, startIDnumlen,startID, lenID, startCsum, lenCsum, endall) ) {
        newIDString = idString.left(startCsum) + idCsum+ idString.mid(  endall +1);
    } else {
        newIDString = idString + prof + ";3;_u_;"+ idCsum + ":";
    }
    //qDebug("setCsum:profile:%s*retval:%s*idCsum:%s* ", prof.latin1(), newIDString.latin1() ,idCsum.latin1() );
    return newIDString;
}
const QString KIdManager::getCsum (const QString& idString,const QString& prof )
{
    int startProf;
    int startIDnum;
    int startIDnumlen;
    int startID;
    int lenID;
    int startCsum;
    int lenCsum;
    int endall;
    QString idval = "";
    if ( KIdManager::getNumbers (idString, prof, startProf, startIDnum, startIDnumlen,startID, lenID, startCsum, lenCsum, endall) )
        idval = idString.mid(  startCsum, lenCsum );

    //qDebug("getCsum:profile:%s*retval:%s*idstring:%s* ", prof.latin1(), idval.latin1() ,idString.latin1() );
    return idval; 
}
// :profilename;12;id_withLen12;123456:
bool KIdManager::getNumbers (const QString& idString,const QString& prof, int &startProf, int &startIDnum, int &startIDnumlen,int &startID, int& lenID, int &startCsum, int &lenCsum, int & endall)
{
    startProf = idString.find( ":"+prof+";" );
    if ( startProf >= 0 ) {
        startIDnum = prof.length()+2+startProf;
        startID = idString.find( ";", startIDnum ) +1;
        startIDnumlen = startID - startIDnum - 1;
        if ( startIDnum > 0 ) {
            bool ok;
            lenID = idString.mid ( startIDnum,startIDnumlen).toInt( &ok );
            if (ok) {
                startCsum = startID+lenID+1;
                endall =  idString.find( ":", startCsum )-1;
                if ( endall < 0 ) {
                    qDebug("Error getNumbers: andall not found ");
                    return false;
                }
                lenCsum = endall-startCsum+1;
            }
            else {
                qDebug("Error getNumbers:length is no number:*%s* ", idString.mid ( startIDnum,startIDnumlen).latin1());
                return false;
            }
        } else {
            qDebug("Error in KIdManager::getNumbers.startIDnum <= 0");
            return false;
        }
    } else {
        //qDebug("getnumbers: profile not found *%s* ",prof.latin1() );
        return false;
    }
    return true;
}
