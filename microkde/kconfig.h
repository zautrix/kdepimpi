#ifndef MINIKDE_KCONFIG_H
#define MINIKDE_KCONFIG_H
#ifdef QT_4_COMPAT
#include <qt4compat.h>
#else


#endif
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qcolor.h>
#include <qfont.h>
#include <qmap.h>
#include <qdatetime.h>

class KConfig
{
  public:
    KConfig( const QString & );
    ~KConfig();
  
    void setTempGroup( const QString &group );
    QString tempGroup() const;

    void setGroup( const QString & );

//US
  /**
   * Returns the name of the group in which we are
   *  searching for keys and from which we are retrieving entries.
   *
   * @return The current group.
   */
    QString group() const;

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
    bool deleteGroup( const QString& group);
    
//US I took the following hasGroup method from a newer version from KDE.
  /**
   * Returns true if the specified group is known about.
   *
   * @param group The group to search for.
   * @return Whether the group exists.
   */
  bool hasGroup(const QString &group) const;
    
    
    QString getFileName();
    
//US added method readIntListEntry    
    QValueList<int> readIntListEntry( const QString &);
    
    int readNumEntry( const QString &, int def=0 );
    QString readEntry( const QString &, const QString &def=QString::null );
    QStringList readListEntry( const QString & );
    bool readBoolEntry( const QString &, bool def=false );
    QColor readColorEntry( const QString &, QColor * );
    QFont readFontEntry( const QString &, QFont * );
    QDateTime readDateTimeEntry( const QString &, const QDateTime *pDefault = 0 );
    QSize readSizeEntry(const QString &, QSize* );
    bool hasKey( const QString &);
        
    void writeEntry( const QString &, const QValueList<int>& );
    void writeEntry( const QString &, int );
    void writeEntry( const QString &key , unsigned int value) { writeEntry( key, int( value ) ); }
    void writeEntry( const char *key , unsigned int value) { writeEntry( QString( key ), value ); }
    void writeEntry( const char *key, int value ) { writeEntry( QString( key ), value ); } 
    void writeEntry( const QString &, const QString & );
    void writeEntry( const char *key, const QString &value ) { writeEntry( QString( key ), value ); }
    void writeEntry( const QString &, const QStringList & );
    void writeEntry( const QString &, bool );
    void writeEntry( const char *key, bool value ) { writeEntry( QString( key ), value ); } 
    void writeEntry( const QString &, const QColor & );
    void writeEntry( const QString &, const QFont & );
    void writeEntry( const QString &, const QDateTime & );
    void writeEntry( const QString &, const QSize & );

    void deleteEntry( const QString &);
        
    void load();
    void sync();

  private:  
    static QString mGroup;
    QString mTempGroup;
    
    QString mFileName;
    
    QMap<QString,bool> mBoolMap;
    QMap<QString,QString> mStringMap;
    QMap<QString,QDateTime> mDateTimeMap;
    
    bool mDirty;
};

#endif
