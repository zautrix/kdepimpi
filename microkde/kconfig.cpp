#include <qfile.h>
#include <qtextstream.h>
#include <qwidget.h>

#include "kdebug.h"

#include "kurl.h"
#include "kstandarddirs.h"
#include "kconfig.h"

QString KConfig::mGroup = "";
//QString KConfig::mGroup = "General";

KConfig::KConfig( const QString &fileName )
  : mFileName( fileName ), mDirty( false )
{
  
  mTempGroup = "";
  load();

}


KConfig::~KConfig()
{
  sync();
}
// we need the temp group for plugins on windows
void KConfig::setTempGroup( const QString &group )
{
  mTempGroup = group;

  if ( mTempGroup.right( 1 ) != "/" ) mTempGroup += "/";
}


QString KConfig::tempGroup() const {
  return mTempGroup;
}

void KConfig::setGroup( const QString &group )
{
  

  mGroup = group;

  if ( mGroup.right( 1 ) != "/" ) mGroup += "/";
}

//US
QString KConfig::group() const {
  return mGroup;
}

//US added method
QValueList<int> KConfig::readIntListEntry( const QString & key)
{
//  qDebug("KConfig::readIntListEntry key=%s:", key.latin1());
  
  QValueList<int> result;

  QMap<QString,QString>::ConstIterator mit = mStringMap.find( mGroup + key );
  
  if ( mit == mStringMap.end() ) {
    return result;
  }
    
  QStringList valuesAsStrings = QStringList::split(":",  *mit );
  bool ok = false;
  bool ok2 = true;
  int val;
  
  for ( QStringList::Iterator sit = valuesAsStrings.begin(); sit != valuesAsStrings.end(); ++sit ) {
    val = (*sit).toInt(&ok);
    result << val;
    if (ok == false) {
        //qDebug("KConfig::readIntListEntry str=%s , int=%n:", (*sit).latin1(), &val);
      ok2 = false;
    }
  }
  
  if (ok2 == false)
  {
   
    qDebug("KConfig::readIntListEntry: error while reading one of the intvalues.");
  }
  
  return result;
}

int KConfig::readNumEntry( const QString & key, int def )
{
  QString res = readEntry(key, QString::number(def ) );
  bool ok = false;
  int result = res.toInt(&ok);
  if ( ok )
      return result;
  return def;
}

QString KConfig::readEntry( const QString &key, const QString &def )
{
  QMap<QString,QString>::ConstIterator it = mStringMap.find( mGroup + key );
  
  if ( it == mStringMap.end() ) {
    return def;
  }
  
  return QString::fromUtf8((*it).latin1());
}

QSize KConfig::readSizeEntry( const QString &key, QSize* def )
{
  QValueList<int> intlist = readIntListEntry(key);

  if (intlist.count() < 2)
    {
      if (def)
      return *def;
      else
      return QSize();
    } 
 
  QSize ret;
  ret.setWidth(intlist[0]);
  ret.setHeight(intlist[1]);

  return ret;
}

QStringList KConfig::readListEntry( const QString &key )
{
    QMap<QString,QString>::ConstIterator it = mStringMap.find( mGroup + key );
    
    if ( it == mStringMap.end() ) {
        return QStringList();
    }
    QStringList temp = QStringList::split(":@:",  QString::fromUtf8((*it).latin1()));
    if ( temp.count() == 1 )
        return  QStringList::split(":",  QString::fromUtf8((*it).latin1()));
    return  temp;
    
}

bool KConfig::readBoolEntry( const QString &key, bool def )
{
  QMap<QString,bool>::ConstIterator it = mBoolMap.find( mGroup + key );
  
  if ( it == mBoolMap.end() ) {
    return def;
  }
  
  return *it;
}

QColor KConfig::readColorEntry( const QString & e, QColor *def )
{

    QStringList l;
    l = readListEntry( e.utf8() );
    if (l.count() != 3 ) {
        if ( def )
            return *def;
        else
            return QColor(); 
    }
    QColor c ( l[0].toInt(), l[1].toInt(), l[2].toInt() );
    return c; 
}

QFont KConfig::readFontEntry( const QString & e, QFont *def )
{
  QStringList font = readListEntry( e );
  if ( font.isEmpty() )
      return *def;
  QFont f;
  f.setFamily( font[0]);
  f.setBold ( font[1] == "bold");
  f.setPointSize ( font[2].toInt());
  f.setItalic( font[3] == "italic" );
  return f;
}

QDateTime KConfig::readDateTimeEntry( const QString &key, const QDateTime *def )
{
  QMap<QString,QDateTime>::ConstIterator it = mDateTimeMap.find( mGroup + key );
  
  if ( it == mDateTimeMap.end() ) {
    if ( def ) return *def;
    else return QDateTime();
  }

  return *it;
}

//US added method
void KConfig::writeEntry( const QString &key, const QValueList<int> &value)
{
  QStringList valuesAsStrings;

  QValueList<int>::ConstIterator it;
  
  for( it = value.begin(); it != value.end(); ++it )
  {
    valuesAsStrings << QString::number(*it);
  }
  
  mStringMap.insert( mGroup + key, valuesAsStrings.join(":") );
  mDirty = true;
}

void KConfig::writeEntry( const QString & key , int num )
{
    writeEntry( key, QString::number ( num ) );
}

void KConfig::writeEntry( const QString &key, const QString &value )
{
  mStringMap.insert( mGroup + key, value.utf8() );

  mDirty = true;
}

void KConfig::writeEntry( const QString &key, const QStringList &value )
{
    mStringMap.insert( mGroup + key, value.join(":@:").utf8() );

  mDirty = true;
}

void KConfig::writeEntry( const QString &key, bool value)
{
  mBoolMap.insert( mGroup + key, value );

  mDirty = true;
}

void KConfig::writeEntry( const QString & e, const QColor & c )
{
    QStringList l;
    l.append( QString::number ( c.red() ) );
    l.append( QString::number ( c.green() ) );
    l.append( QString::number ( c.blue() ) );
    writeEntry(  e.utf8(), l );
}

void KConfig::writeEntry( const QString & e, const QSize & s )
{
  QValueList<int> intlist;
  intlist << s.width() << s.height();
  writeEntry(  e, intlist );
}

void KConfig::writeEntry( const QString & e , const QFont & f )
{
    QStringList font;
    font.append( f.family());
    font.append( (!f.bold ()?"nonbold":"bold") );
    font.append( QString::number ( f.pointSize () ) );
    font.append( !f.italic ()?"nonitalic":"italic" );
    writeEntry( e, font );
}

void KConfig::writeEntry( const QString &key, const QDateTime &dt )
{
  mDateTimeMap.insert( mGroup + key, dt );
}

void KConfig::load()
{
  

  QFile f( mFileName );
  if ( !f.open( IO_ReadOnly ) ) {
      //qDebug("KConfig: could not open file %s ",mFileName.latin1() );
      return;
  }

  mBoolMap.clear();
  mStringMap.clear();
  
  QTextStream t( &f );
  t.setEncoding( QTextStream::Latin1 );
  QString line = t.readLine();

  while ( !line.isNull() ) {
    QStringList tokens = QStringList::split( ",", line );
    if ( tokens[0] == "bool" ) {
      bool value = false;
      if ( tokens[2] == "1" ) value = true;      
      mBoolMap.insert( tokens[1], value );
    } else if ( tokens[0] == "QString" ) {
      QString value = tokens[2];
      mStringMap.insert( tokens[1], value );
    } else if ( tokens[0] == "QDateTime" ) {
#if 0
      int year = tokens[2].toInt();
      QDateTime dt( QDate( year,
                           tokens[3].toInt(),
                           tokens[4].toInt() ),
                    QTime( tokens[5].toInt(), tokens[6].toInt(),
                           tokens[7].toInt() ) );
      mDateTimeMap.insert( tokens[1], dt );
#endif
    }
  
    line = t.readLine();
  }
}

void KConfig::sync()
{

  if ( !mDirty ) return;
  //qDebug("KConfig::sync() %s ",mFileName.latin1() );
  //kdDebug() << "KConfig::sync(): " << mFileName << endl;

//US I took the following code from a newer version of KDE  
    // Create the containing dir if needed
  KURL path;
  path.setPath(mFileName);
  QString dir=path.directory();
  KStandardDirs::makeDir(dir);
  
  QFile f( mFileName );
  if ( !f.open( IO_WriteOnly ) ) {
  
    qDebug("KConfig::sync() Can't open file %s ",mFileName.latin1() );
    
    return;
  }

  QTextStream t( &f );
  t.setEncoding( QTextStream::Latin1 );
  QMap<QString,bool>::ConstIterator itBool;
  for( itBool = mBoolMap.begin(); itBool != mBoolMap.end(); ++itBool ) {
    t << "bool," << itBool.key() << "," << ( *itBool ? "1" : "0"  ) << endl;
  }

  QMap<QString,QString>::ConstIterator itString;
  for( itString = mStringMap.begin(); itString != mStringMap.end(); ++itString ) {
    t << "QString," << itString.key() << "," << (*itString ) << endl;
  }

  QMap<QString,QDateTime>::ConstIterator itDateTime;
  for( itDateTime = mDateTimeMap.begin(); itDateTime != mDateTimeMap.end(); ++itDateTime ) {
    QDateTime dt = *itDateTime;
    t << "QDateTime," << itDateTime.key() << ","
      << dt.date().year() << ","
      << dt.date().month() << ","
      << dt.date().day() << ","
      << dt.time().hour() << ","
      << dt.time().minute() << ","
      << dt.time().second() << endl;
  }

  f.close();

  mDirty = false;
}


//US I took the following deleteGroup method from a newer version from KDE.
/**
   * Deletes a configuration entry group
   *
   * If the group is not empty and bDeep is false, nothing gets
   * deleted and false is returned.
   * If this group is the current group and it is deleted, the
   * current group is undefined and should be set with setGroup()
   * before the next operation on the configuration object.
   *
   * @param group The name of the group
   *         returns true if we deleted at least one entry.
   */
bool  KConfig::deleteGroup( const QString& group)
{
  bool dirty = false;
  int pos;
  
  QMap<QString,bool>::Iterator itBool = mBoolMap.begin();
  QMap<QString,bool>::Iterator delBool;
 
  while ( itBool != mBoolMap.end()  ) {
      pos = itBool.key().find( group );
      if (pos == 0) {
          delBool  = itBool;
          ++itBool;
          mBoolMap.remove(delBool);
          dirty = true;
      } else
          ++itBool;
      
  }
  /*
  for( itBool = mBoolMap.begin(); itBool != mBoolMap.end(); ++itBool )
  {
    pos = itBool.key().find( group );
    if (pos == 0) {
      mBoolMap.remove(itBool);
      dirty = true;
    }
  }
  */
  QMap<QString,QString>::Iterator itString = mStringMap.begin();
  QMap<QString,QString>::Iterator delString ;
  while( itString != mStringMap.end()  ) {
      pos = itString.key().find( group );
      if (pos == 0) {
          delString = itString;
          ++itString;
          mStringMap.remove(delString);
          //qDebug("delte++++++++++++++++++ ");
          dirty = true;
      } else
          ++itString;

  }
  /* this leads to a memory access violation
  for( itString = mStringMap.begin(); itString != mStringMap.end(); ++itString )
  {
    pos = itString.key().find( group );
    if (pos == 0) {
      mStringMap.remove(itString);
      dirty = true;
    }
  }
  */
  QMap<QString,QDateTime>::Iterator itDateTime= mDateTimeMap.begin();
  QMap<QString,QDateTime>::Iterator delDateTime;
  while ( itDateTime != mDateTimeMap.end()  ) {
      pos = itDateTime.key().find( group );
      if (pos == 0) {
          delDateTime = itDateTime;
          ++itDateTime;
          mDateTimeMap.remove(delDateTime);
          dirty = true;
      } else
          ++itDateTime;
      
  }
  /*
  for( itDateTime = mDateTimeMap.begin(); itDateTime != mDateTimeMap.end(); ++itDateTime )
  {
    pos = itDateTime.key().find( group );
    if (pos == 0) {
      mDateTimeMap.remove(itDateTime);
      dirty = true;
    }
  }
  */
  
  if (dirty)
     mDirty = true;
  
  return dirty;

}

//US I took the following hasGroup method from a newer version from KDE.
  /**
   * Returns true if the specified group is known about.
   *
   * @param group The group to search for.
   * @return Whether the group exists.
   */
bool KConfig::hasGroup(const QString &group) const
{
  QMap<QString,bool>::ConstIterator itBool;
  for( itBool = mBoolMap.begin(); itBool != mBoolMap.end(); ++itBool )
  {
    if (itBool.key().find( group ) == 0) {
      return true;
    }
  }

  QMap<QString,QString>::ConstIterator itString;
  for( itString = mStringMap.begin(); itString != mStringMap.end(); ++itString )
  {
    if (itString.key().find( group ) == 0) {
      return true;
    }
  }

  QMap<QString,QDateTime>::ConstIterator itDateTime;
  for( itDateTime = mDateTimeMap.begin(); itDateTime != mDateTimeMap.end(); ++itDateTime )
  {
    if (itDateTime.key().find( group ) == 0) {
      return true;
    }
  }
  
  return false;
}

void KConfig::deleteEntry( const QString &key)
{
  bool dirty = false;
    
  QMap<QString,bool>::Iterator itBool = mBoolMap.find( mGroup + key );
  if ( itBool != mBoolMap.end() ) {
    mBoolMap.remove(itBool);
    dirty = true;
  }
  

  QMap<QString,QString>::Iterator itString = mStringMap.find( mGroup + key );
  if ( itString != mStringMap.end() ) {
    mStringMap.remove(itString);
    dirty = true;
  }


  QMap<QString,QDateTime>::Iterator itDateTime = mDateTimeMap.find( mGroup + key );
  if ( itDateTime != mDateTimeMap.end() ) {
    mDateTimeMap.remove(itDateTime);
    dirty = true;
  }
  
  if (dirty)
     mDirty = true;
  
}

//US
QString KConfig::getFileName()
{
  return mFileName;
}

bool KConfig::hasKey( const QString &key)
{
  QMap<QString,bool>::Iterator itBool = mBoolMap.find( mGroup + key );
  if ( itBool != mBoolMap.end() ) {
    return true;
  }

  QMap<QString,QString>::Iterator itString = mStringMap.find( mGroup + key );
  if ( itString != mStringMap.end() ) {
    return true;
  }

  QMap<QString,QDateTime>::Iterator itDateTime = mDateTimeMap.find( mGroup + key );
  if ( itDateTime != mDateTimeMap.end() ) {
    return true;
  }
  
  return false;
}

