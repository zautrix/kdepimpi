#ifndef MINIKDE_KAPPLICATION_H
#define MINIKDE_KAPPLICATION_H

#include "qstring.h"
#include <qdialog.h>
#ifdef QDialog 
#undef  QDialog 
#endif

class KApplication
{
  public:
    static int random();

//US
  /**
   * Generates a random string.  It operates in the range [A-Za-z0-9]
   * @param length Generate a string of this length.
   * @return the random string
   */
  static QString randomString(int length);
  static int execDialog( QDialog* ); 
  static void showLicence(); 
  static void testCoords( int* x, int* y, int* wid, int * hei ); 
  static void showFile(QString caption, QString file); 
  static void showText(QString caption, QString text); 
  static bool convert2latin1(QString file); 
  static int createBackup( QString fn, QString dp, int numBup );
};


#endif
