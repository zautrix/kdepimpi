/* This file is part of the KDE libraries
   Copyright (C) 1999 Sirtaj Singh Kang <taj@kde.org>
   Copyright (C) 1999 Stephan Kulow <coolo@kde.org>
   Copyright (C) 1999 Waldo Bastian <bastian@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/*
 * Author: Stephan Kulow <coolo@kde.org> and Sirtaj Singh Kang <taj@kde.org>
 * Version:	$Id: kstandarddirs.cpp,v 1.16 2010/01/26 19:50:30 zautrix Exp $
 * Generated:	Thu Mar  5 16:05:28 EST 1998
 */

//US #include "config.h"

#include <stdlib.h>
#include <assert.h>
//US#include <errno.h>
//US #ifdef HAVE_SYS_STAT_H
//US #include <sys/stat.h>
//US #endif
//US#include <sys/types.h>
//US#include <dirent.h>
//US#include <pwd.h>

#include <qregexp.h>
#include <qasciidict.h>
#include <qdict.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include <qstringlist.h>

#include "kstandarddirs.h"
#include "kconfig.h"
#include "kdebug.h"
//US #include "kinstance.h"
#include "kshell.h"
//US#include <sys/param.h>
//US#include <unistd.h>

//US
QString KStandardDirs::mAppDir = QString::null;


template class QDict<QStringList>;

#if 0
#include <qtextedit.h>
void ddd( QString op )
{
  static QTextEdit * dot = 0;
  if ( ! dot )
  dot = new QTextEdit();

  dot->show();

  dot->append( op );

}
#endif
class KStandardDirs::KStandardDirsPrivate
{
public:
   KStandardDirsPrivate()
    : restrictionsActive(false),
      dataRestrictionActive(false)
   { }

   bool restrictionsActive;
   bool dataRestrictionActive;
   QAsciiDict<bool> restrictions;
   QStringList xdgdata_prefixes;
   QStringList xdgconf_prefixes;
};

static const char* const types[] = {"html", "icon", "apps", "sound",
			      "data", "locale", "services", "mime",
			      "servicetypes", "config", "exe", "tmp",
			      "wallpaper", "lib", "pixmap", "templates",
			      "module", "qtplugins",
			      "xdgdata-apps", "xdgdata-dirs", "xdgconf-menu", 0 };

static int tokenize( QStringList& token, const QString& str,
		const QString& delim );

KStandardDirs::KStandardDirs( ) : addedCustoms(false)
{
    d = new KStandardDirsPrivate;
    dircache.setAutoDelete(true);
    relatives.setAutoDelete(true);
    absolutes.setAutoDelete(true);
    savelocations.setAutoDelete(true);
    addKDEDefaults();
}

KStandardDirs::~KStandardDirs()
{
    delete d;
}

bool KStandardDirs::isRestrictedResource(const char *type, const QString& relPath) const
{
   if (!d || !d->restrictionsActive)
      return false;

   if (d->restrictions[type])
      return true;

   if (strcmp(type, "data")==0)
   {
      applyDataRestrictions(relPath);
      if (d->dataRestrictionActive)
      {
         d->dataRestrictionActive = false;
         return true;
      }
   }
   return false;
}

void KStandardDirs::applyDataRestrictions(const QString &relPath) const
{
   QString key;
   int i = relPath.find('/');
   if (i != -1)
      key = "data_"+relPath.left(i);
   else
      key = "data_"+relPath;

   if (d && d->restrictions[key.latin1()])
      d->dataRestrictionActive = true;
}


QStringList KStandardDirs::allTypes() const
{
    QStringList list;
    for (int i = 0; types[i] != 0; ++i)
        list.append(QString::fromLatin1(types[i]));
    return list;
}

void KStandardDirs::addPrefix( const QString& _dir )
{
    if (_dir.isNull())
	return;

    QString dir = _dir;
    if (dir.at(dir.length() - 1) != '/')
	dir += '/';

    if (!prefixes.contains(dir)) {
	prefixes.append(dir);
	dircache.clear();
    }
}

void KStandardDirs::addXdgConfigPrefix( const QString& _dir )
{
    if (_dir.isNull())
	return;

    QString dir = _dir;
    if (dir.at(dir.length() - 1) != '/')
	dir += '/';

    if (!d->xdgconf_prefixes.contains(dir)) {
	d->xdgconf_prefixes.append(dir);
	dircache.clear();
    }
}

void KStandardDirs::addXdgDataPrefix( const QString& _dir )
{
    if (_dir.isNull())
	return;

    QString dir = _dir;
    if (dir.at(dir.length() - 1) != '/')
	dir += '/';

    if (!d->xdgdata_prefixes.contains(dir)) {
	d->xdgdata_prefixes.append(dir);
	dircache.clear();
    }
}


QString KStandardDirs::kfsstnd_prefixes()
{
   return prefixes.join(":");
}

bool KStandardDirs::addResourceType( const char *type,
				     const QString& relativename )
{
    if (relativename.isNull())
       return false;

    QStringList *rels = relatives.find(type);
    if (!rels) {
	rels = new QStringList();
	relatives.insert(type, rels);
    }
    QString copy = relativename;
    if (copy.at(copy.length() - 1) != '/')
	copy += '/';
    if (!rels->contains(copy)) {
	rels->prepend(copy);
	dircache.remove(type); // clean the cache
	return true;
    }
    return false;
}

bool KStandardDirs::addResourceDir( const char *type,
				    const QString& absdir)
{
    QStringList *paths = absolutes.find(type);
    if (!paths) {
	paths = new QStringList();
	absolutes.insert(type, paths);
    }
    QString copy = absdir;
    if (copy.at(copy.length() - 1) != '/')
      copy += '/';

    if (!paths->contains(copy)) {
	paths->append(copy);
	dircache.remove(type); // clean the cache
	return true;
    }
    return false;
}

QString KStandardDirs::findResource( const char *type,
				     const QString& filename ) const
{
    if (filename.at(0) == '/')
	return filename; // absolute dirs are absolute dirs, right? :-/

#if 0
kdDebug() << "Find resource: " << type << endl;
for (QStringList::ConstIterator pit = prefixes.begin();
     pit != prefixes.end();
     pit++)
{
  kdDebug() << "Prefix: " << *pit << endl;
}
#endif

    QString dir = findResourceDir(type, filename);
    if (dir.isNull())
	return dir;
    else return dir + filename;
}
/*US
static Q_UINT32 updateHash(const QString &file, Q_UINT32 hash)
{
    QCString cFile = QFile::encodeName(file);
//US     struct stat buff;
//US    if ((access(cFile, R_OK) == 0) &&
//US        (stat( cFile, &buff ) == 0) &&
//US        (S_ISREG( buff.st_mode )))
    QFileInfo pathfnInfo(cFile);
    if (( pathfnInfo.isReadable() == true ) &&
       ( pathfnInfo.isFile()) )
    {
//US       hash = hash + (Q_UINT32) buff.st_ctime;
       hash = hash + (Q_UINT32) pathfnInfo.lastModified();
    }
    return hash;
}
*/
/*US
Q_UINT32 KStandardDirs::calcResourceHash( const char *type,
			      const QString& filename, bool deep) const
{
    Q_UINT32 hash = 0;

    if (filename.at(0) == '/')
    {
        // absolute dirs are absolute dirs, right? :-/
	return updateHash(filename, hash);
    }
    if (d && d->restrictionsActive && (strcmp(type, "data")==0))
       applyDataRestrictions(filename);
    QStringList candidates = resourceDirs(type);
    QString fullPath;

    for (QStringList::ConstIterator it = candidates.begin();
	 it != candidates.end(); it++)
    {
        hash = updateHash(*it + filename, hash);
        if (!deep && hash)
           return hash;
    }
    return hash;
}
*/

QStringList KStandardDirs::findDirs( const char *type,
                                     const QString& reldir ) const
{
    QStringList list;

    checkConfig();

    if (d && d->restrictionsActive && (strcmp(type, "data")==0))
       applyDataRestrictions(reldir);
    QStringList candidates = resourceDirs(type);
    QDir testdir;

    for (QStringList::ConstIterator it = candidates.begin();
         it != candidates.end(); it++) {
        testdir.setPath(*it + reldir);
        if (testdir.exists())
            list.append(testdir.absPath() + '/');
    }

    return list;
}

QString KStandardDirs::findResourceDir( const char *type,
					const QString& filename) const
{
#ifndef NDEBUG
    if (filename.isEmpty()) {
      kdWarning() << "filename for type " << type << " in KStandardDirs::findResourceDir is not supposed to be empty!!" << endl;
      return QString::null;
    }
#endif

    if (d && d->restrictionsActive && (strcmp(type, "data")==0))
       applyDataRestrictions(filename);
    QStringList candidates = resourceDirs(type);
    QString fullPath;
#ifdef DESKTOP_VERSION
#ifdef _WIN32_
    candidates.prepend( qApp->applicationDirPath () +"\\");
#else
    candidates.prepend( qApp->applicationDirPath () +"/");
#endif
#endif
    for (QStringList::ConstIterator it = candidates.begin(); it != candidates.end(); it++)
    {
        //qDebug("looking for dir %s  - file %s", (*it).latin1(),  filename.latin1());
      if (exists(*it + filename))
	return *it;
    }

#ifndef NDEBUG
    if(false && type != "locale")
        qDebug("KStdDirs::findResDir(): can't find %s ", filename.latin1());

#endif

    return QString::null;
}

bool KStandardDirs::exists(const QString &fullPath)
{
//US    struct stat buff;
    QFileInfo fullPathInfo(QFile::encodeName(fullPath));

//US    if (access(QFile::encodeName(fullPath), R_OK) == 0 && fullPathInfo.isReadable())
    if (fullPathInfo.isReadable())
	  {
      if (fullPath.at(fullPath.length() - 1) != '/') {
//US	      if (S_ISREG( buff.st_mode ))
	      if (fullPathInfo.isFile())
		      return true;
	    }
      else {
//US	      if (S_ISDIR( buff.st_mode ))
	      if (fullPathInfo.isDir())
		      return true;
      }
    }
    return false;
}

static void lookupDirectory(const QString& path, const QString &relPart,
			    const QRegExp &regexp,
			    QStringList& list,
			    QStringList& relList,
			    bool recursive, bool uniq)
{
  QString pattern = regexp.pattern();
  if (recursive || pattern.contains('?') || pattern.contains('*'))
  {
    // We look for a set of files.
//US    DIR *dp = opendir( QFile::encodeName(path));
    QDir dp(QFile::encodeName(path));

    if (!dp.exists())
      return;
    static int iii = 0;
    ++iii;
    if ( iii == 5 )
        abort();
    assert(path.at(path.length() - 1) == '/');

//US    struct dirent *ep;
//US    struct stat buff;

    QString _dot(".");
    QString _dotdot("..");

//US    while( ( ep = readdir( dp ) ) != 0L )
    QStringList direntries = dp.entryList();
    QStringList::Iterator it = direntries.begin();

    while ( it != list.end() )            // for each file...
    {

//US      QString fn( QFile::decodeName(ep->d_name));
      QString fn = (*it); // dp.entryList already decodes
      it++;
      if ( fn.isNull() )
          break;

      if (fn == _dot || fn == _dotdot || fn.at(fn.length() - 1).latin1() == '~'   )
	continue;

/*US
      if (!recursive && !regexp.exactMatch(fn))
        continue; // No match
*/
//US this should do the same:
#ifdef QT_4_COMPAT
      int pos = regexp.exactMatch(fn);
#else
      int pos = regexp.match(fn);
#endif
      if (!recursive && !pos == 0)
        continue; // No match

      QString pathfn = path + fn;
/*US
      if ( stat( QFile::encodeName(pathfn), &buff ) != 0 ) {
        kdDebug() << "Error stat'ing " << pathfn << " : " << perror << endl;
        continue; // Couldn't stat (e.g. no read permissions)
      }

      if ( recursive )
      {
        if ( S_ISDIR( buff.st_mode )) {
          lookupDirectory(pathfn + '/', relPart + fn + '/', regexp, list, relList, recursive, uniq);
      }
*/
//US replacement:
      QFileInfo pathfnInfo(QFile::encodeName(pathfn));
      if ( pathfnInfo.isReadable() == false )
      {
//US        kdDebug() << "Error stat'ing " << pathfn << " : " << perror << endl;
        continue; // Couldn't stat (e.g. no read permissions)
      }

      if ( recursive )
      {
        if ( pathfnInfo.isDir()) {
          lookupDirectory(pathfn + '/', relPart + fn + '/', regexp, list, relList, recursive, uniq);
      }


/*US
        if (!regexp.exactMatch(fn))
          continue; // No match
*/
//US this should do the same:
#ifdef QT_4_COMPAT
      int pos = regexp.exactMatch(fn);
#else
        pos = regexp.match(fn);
#endif
        if (!pos == 0)
          continue; // No match
      }

//US      if ( S_ISREG( buff.st_mode))
      if ( pathfnInfo.isFile())
      {
        if (!uniq || !relList.contains(relPart + fn))
        {
          list.append( pathfn );
          relList.append( relPart + fn );
        }
      }
    }
//US    closedir( dp );
  }
  else
  {
     // We look for a single file.
     QString fn = pattern;
     QString pathfn = path + fn;
//US     struct stat buff;
      QFileInfo pathfnInfo(QFile::encodeName(pathfn));


//US     if ( stat( QFile::encodeName(pathfn), &buff ) != 0 )
      if ( pathfnInfo.isReadable() == false )
        return; // File not found

//US     if ( S_ISREG( buff.st_mode))
     if ( pathfnInfo.isFile())
     {
       if (!uniq || !relList.contains(relPart + fn))
       {
         list.append( pathfn );
         relList.append( relPart + fn );
       }
     }
  }
}

static void lookupPrefix(const QString& prefix, const QString& relpath,
                         const QString& relPart,
			 const QRegExp &regexp,
			 QStringList& list,
			 QStringList& relList,
			 bool recursive, bool uniq)
{
    if (relpath.isNull()) {
       lookupDirectory(prefix, relPart, regexp, list,
		       relList, recursive, uniq);
       return;
    }
    QString path;
    QString rest;

    if (relpath.length())
    {
       int slash = relpath.find('/');
       if (slash < 0)
	   rest = relpath.left(relpath.length() - 1);
       else {
	   path = relpath.left(slash);
	   rest = relpath.mid(slash + 1);
       }
    }
    assert(prefix.at(prefix.length() - 1) == '/');

//US    struct stat buff;

    if (path.contains('*') || path.contains('?')) {
	QRegExp pathExp(path, true, true);
//US	DIR *dp = opendir( QFile::encodeName(prefix) );
  QDir dp(QFile::encodeName(prefix));

//US	if (!dp)
  if (!dp.exists())
  {
	    return;
	}

//US	struct dirent *ep;

        QString _dot(".");
        QString _dotdot("..");

//US	while( ( ep = readdir( dp ) ) != 0L )
    QStringList direntries = dp.entryList();
    QStringList::Iterator it = direntries.begin();

    while ( it != list.end() )            // for each file...
    {
//US      QString fn( QFile::decodeName(ep->d_name));
      QString fn = (*it); // dp.entryList() already encodes the strings
      it++;

		if (fn == _dot || fn == _dotdot || fn.at(fn.length() - 1) == '~')
		    continue;

#ifdef DESKTOP_VERSION

		if (pathExp.search(fn) == -1)
		    continue; // No match

#else
//US this should do the same:
        if (pathExp.find(fn, 0) == -1)
          continue; // No match
#endif
		QString rfn = relPart+fn;
		fn = prefix + fn;
//US    if ( stat( QFile::encodeName(fn), &buff ) != 0 )
      QFileInfo fnInfo(QFile::encodeName(fn));
    if ( fnInfo.isReadable() == false )
    {
//US        kdDebug() << "Error statting " << fn << " : " << perror << endl;
		    continue; // Couldn't stat (e.g. no permissions)
		}
//US		if ( S_ISDIR( buff.st_mode ))
    if ( fnInfo.isDir() )

		    lookupPrefix(fn + '/', rest, rfn + '/', regexp, list, relList, recursive, uniq);
	    }

//US	closedir( dp );
    } else {
        // Don't stat, if the dir doesn't exist we will find out
        // when we try to open it.
        lookupPrefix(prefix + path + '/', rest,
                     relPart + path + '/', regexp, list,
                     relList, recursive, uniq);
    }
}

QStringList
KStandardDirs::findAllResources( const char *type,
			         const QString& filter,
				 bool recursive,
			         bool uniq,
                                 QStringList &relList) const
{
    QStringList list;
    if (filter.at(0) == '/') // absolute paths we return
    {
        list.append( filter);
	return list;
    }

    QString filterPath;
    QString filterFile;

    if (filter.length())
    {
       int slash = filter.findRev('/');
       if (slash < 0)
	   filterFile = filter;
       else {
	   filterPath = filter.left(slash + 1);
	   filterFile = filter.mid(slash + 1);
       }
    }
    checkConfig();

    if (d && d->restrictionsActive && (strcmp(type, "data")==0))
       applyDataRestrictions(filter);
    QStringList candidates = resourceDirs(type);
    if (filterFile.isEmpty())
	filterFile = "*";

    QRegExp regExp(filterFile, true, true);
    for (QStringList::ConstIterator it = candidates.begin();
         it != candidates.end(); it++)
    {
        lookupPrefix(*it, filterPath, "", regExp, list,
                     relList, recursive, uniq);
    }
    return list;
}

QStringList
KStandardDirs::findAllResources( const char *type,
			         const QString& filter,
				 bool recursive,
			         bool uniq) const
{
    QStringList relList;
    return findAllResources(type, filter, recursive, uniq, relList);
}

QString
KStandardDirs::realPath(const QString &dirname)
{
#ifdef _WIN32_
  return dirname;
#else
//US    char realpath_buffer[MAXPATHLEN + 1];
//US    memset(realpath_buffer, 0, MAXPATHLEN + 1);
  //qDebug( "dirname length = %d",  dirname.length() );
  int maxLen = dirname.length()*3;
    char realpath_buffer[ maxLen + 1];
    memset(realpath_buffer, 0,  maxLen + 1);

    /* If the path contains symlinks, get the real name */
    if (realpath( QFile::encodeName(dirname).data(), realpath_buffer) != 0) {
        // succes, use result from realpath
        int len = strlen(realpath_buffer);
        realpath_buffer[len] = '/';
        realpath_buffer[len+1] = 0;
        return QFile::decodeName(realpath_buffer);
    }

    return dirname;
#endif
}
/*US
void KStandardDirs::createSpecialResource(const char *type)
{
   char hostname[256];
   hostname[0] = 0;
   gethostname(hostname, 255);
   QString dir = QString("%1%2-%3").arg(localkdedir()).arg(type).arg(hostname);
   char link[1024];
   link[1023] = 0;
   int result = readlink(QFile::encodeName(dir).data(), link, 1023);
   if ((result == -1) && (errno == ENOENT))
   {
      QString srv = findExe(QString::fromLatin1("lnusertemp"), KDEDIR+QString::fromLatin1("/bin"));
      if (srv.isEmpty())
         srv = findExe(QString::fromLatin1("lnusertemp"));
      if (!srv.isEmpty())
      {
         system(QFile::encodeName(srv)+" "+type);
         result = readlink(QFile::encodeName(dir).data(), link, 1023);
      }
   }
   if (result > 0)
   {
      link[result] = 0;
      if (link[0] == '/')
         dir = QFile::decodeName(link);
      else
         dir = QDir::cleanDirPath(dir+QFile::decodeName(link));
   }
   addResourceDir(type, dir+'/');
}
*/

QStringList KStandardDirs::resourceDirs(const char *type) const
{
    QStringList *candidates = dircache.find(type);

    if (!candidates) { // filling cache
/*US
        if (strcmp(type, "socket") == 0)
           const_cast<KStandardDirs *>(this)->createSpecialResource(type);
        else if (strcmp(type, "tmp") == 0)
           const_cast<KStandardDirs *>(this)->createSpecialResource(type);
        else if (strcmp(type, "cache") == 0)
           const_cast<KStandardDirs *>(this)->createSpecialResource(type);
*/
        QDir testdir;

        candidates = new QStringList();
        QStringList *dirs;

        bool restrictionActive = false;
        if (d && d->restrictionsActive)
        {
           if (d->dataRestrictionActive)
              restrictionActive = true;
           else if (d->restrictions["all"])
              restrictionActive = true;
           else if (d->restrictions[type])
              restrictionActive = true;
           d->dataRestrictionActive = false; // Reset
        }

        dirs = relatives.find(type);
        if (dirs)
        {
            bool local = true;
            const QStringList *prefixList = 0;
            if (strncmp(type, "xdgdata-", 8) == 0)
                prefixList = &(d->xdgdata_prefixes);
            else if (strncmp(type, "xdgconf-", 8) == 0)
                prefixList = &(d->xdgconf_prefixes);
            else
                prefixList = &prefixes;

            for (QStringList::ConstIterator pit = prefixList->begin();
                 pit != prefixList->end();
                 pit++)
            {
                for (QStringList::ConstIterator it = dirs->begin();
                     it != dirs->end(); ++it) {
                    QString path = realPath(*pit + *it);
                    testdir.setPath(path);
                    if (local && restrictionActive)
                       continue;
                    if ((local || testdir.exists()) && !candidates->contains(path))
                        candidates->append(path);
                }
                local = false;
            }
        }
        dirs = absolutes.find(type);
        if (dirs)
            for (QStringList::ConstIterator it = dirs->begin();
                 it != dirs->end(); ++it)
            {
                testdir.setPath(*it);
                if (testdir.exists())
                {
                    QString filename = realPath(*it);
                    if (!candidates->contains(filename))
                        candidates->append(filename);
                }
            }
        dircache.insert(type, candidates);
    }

#if 0
    kdDebug() << "found dirs for resource " << type << ":" << endl;
    for (QStringList::ConstIterator pit = candidates->begin();
	 pit != candidates->end();
	 pit++)
    {
	fprintf(stderr, "%s\n", (*pit).latin1());
    }
#endif


  return *candidates;
}

/*US
QString KStandardDirs::findExe( const QString& appname,
				const QString& pstr, bool ignore)
{
    QFileInfo info;

    // absolute path ?
    if (appname.startsWith(QString::fromLatin1("/")))
    {
        info.setFile( appname );
        if( info.exists() && ( ignore || info.isExecutable() )
            && info.isFile() ) {
            return appname;
        }
        return QString::null;
    }

//US    QString p = QString("%1/%2").arg(__KDE_BINDIR).arg(appname);
    QString p = QString("%1/%2").arg(appname).arg(appname);
    qDebug("KStandardDirs::findExe this is probably wrong");

    info.setFile( p );
    if( info.exists() && ( ignore || info.isExecutable() )
         && ( info.isFile() || info.isSymLink() )  ) {
         return p;
    }

    QStringList tokens;
    p = pstr;

    if( p.isNull() ) {
	p = getenv( "PATH" );
    }

    tokenize( tokens, p, ":\b" );

    // split path using : or \b as delimiters
    for( unsigned i = 0; i < tokens.count(); i++ ) {
	p = tokens[ i ];

        if ( p[ 0 ] == '~' )
        {
            int len = p.find( '/' );
            if ( len == -1 )
                len = p.length();
            if ( len == 1 )
                p.replace( 0, 1, QDir::homeDirPath() );
            else
            {
                QString user = p.mid( 1, len - 1 );
                struct passwd *dir = getpwnam( user.local8Bit().data() );
                if ( dir && strlen( dir->pw_dir ) )
                    p.replace( 0, len, QString::fromLocal8Bit( dir->pw_dir ) );
            }
        }

	p += "/";
	p += appname;

	// Check for executable in this tokenized path
	info.setFile( p );

	if( info.exists() && ( ignore || info.isExecutable() )
           && ( info.isFile() || info.isSymLink() )  ) {
	    return p;
	}
    }

    // If we reach here, the executable wasn't found.
    // So return empty string.

    return QString::null;
}

int KStandardDirs::findAllExe( QStringList& list, const QString& appname,
			const QString& pstr, bool ignore )
{
    QString p = pstr;
    QFileInfo info;
    QStringList tokens;

    if( p.isNull() ) {
	p = getenv( "PATH" );
    }

    list.clear();
    tokenize( tokens, p, ":\b" );

    for ( unsigned i = 0; i < tokens.count(); i++ ) {
	p = tokens[ i ];
	p += "/";
	p += appname;

	info.setFile( p );

	if( info.exists() && (ignore || info.isExecutable())
	    && info.isFile() ) {
	    list.append( p );
	}

    }

    return list.count();
}
*/

static int tokenize( QStringList& tokens, const QString& str,
		     const QString& delim )
{
    int len = str.length();
    QString token = "";

    for( int index = 0; index < len; index++)
    {
	if ( delim.find( str[ index ] ) >= 0 )
	{
	    tokens.append( token );
	    token = "";
	}
	else
	{
	    token += str[ index ];
	}
    }
    if ( token.length() > 0 )
    {
	tokens.append( token );
    }

    return tokens.count();
}

QString KStandardDirs::kde_default(const char *type) {
    if (!strcmp(type, "data"))
	return "apps/";
    if (!strcmp(type, "html"))
	return "share/doc/HTML/";
    if (!strcmp(type, "icon"))
	return "share/icons/";
    if (!strcmp(type, "config"))
	return "config/";
    if (!strcmp(type, "pixmap"))
	return "share/pixmaps/";
    if (!strcmp(type, "apps"))
	return "share/applnk/";
    if (!strcmp(type, "sound"))
	return "share/sounds/";
    if (!strcmp(type, "locale"))
	return "share/locale/";
    if (!strcmp(type, "services"))
	return "share/services/";
    if (!strcmp(type, "servicetypes"))
	return "share/servicetypes/";
    if (!strcmp(type, "mime"))
	return "share/mimelnk/";
    if (!strcmp(type, "cgi"))
	return "cgi-bin/";
    if (!strcmp(type, "wallpaper"))
	return "share/wallpapers/";
    if (!strcmp(type, "templates"))
	return "share/templates/";
    if (!strcmp(type, "exe"))
	return "bin/";
    if (!strcmp(type, "lib"))
      	return "lib/";
    if (!strcmp(type, "module"))
      	return "lib/kde3/";
    if (!strcmp(type, "qtplugins"))
        return "lib/kde3/plugins";
    if (!strcmp(type, "xdgdata-apps"))
        return "applications/";
    if (!strcmp(type, "xdgdata-dirs"))
        return "desktop-directories/";
    if (!strcmp(type, "xdgconf-menu"))
        return "menus/";
    if (!strcmp(type, "tmp"))
	return "tmp/";

    qFatal("unknown resource type %s", type);
    return QString::null;
}

QString KStandardDirs::saveLocation(const char *type,
				    const QString& suffix,
				    bool create) const
{
    //qDebug("KStandardDirs::saveLocation called %s %s", type,suffix.latin1() );
    //return "";
    checkConfig();

    QString *pPath = savelocations.find(type);
    if (!pPath)
    {
       QStringList *dirs = relatives.find(type);
       if (!dirs && (
                     (strcmp(type, "socket") == 0) ||
                     (strcmp(type, "tmp") == 0) ||
                     (strcmp(type, "cache") == 0) ))
       {
          (void) resourceDirs(type); // Generate socket|tmp|cache resource.
          dirs = relatives.find(type); // Search again.
       }
       if (dirs)
       {
          // Check for existance of typed directory + suffix
          if (strncmp(type, "xdgdata-", 8) == 0)
             pPath = new QString(realPath(localxdgdatadir() + dirs->last()));
          else if (strncmp(type, "xdgconf-", 8) == 0)
             pPath = new QString(realPath(localxdgconfdir() + dirs->last()));
          else
             pPath = new QString(realPath(localkdedir() + dirs->last()));
       }
       else {
          dirs = absolutes.find(type);
          if (!dirs)
             qFatal("KStandardDirs: The resource type %s is not registered", type);
          pPath = new QString(realPath(dirs->last()));
       }

       savelocations.insert(type, pPath);
    }

    QString fullPath = *pPath + suffix;
//US    struct stat st;
//US    if (stat(QFile::encodeName(fullPath), &st) != 0 || !(S_ISDIR(st.st_mode)))
      QFileInfo fullPathInfo(QFile::encodeName(fullPath));
      if (fullPathInfo.isReadable() || !fullPathInfo.isDir())


    {
	if(!create) {
#ifndef NDEBUG
	    qDebug("save location %s doesn't exist", fullPath.latin1());
#endif
	    return fullPath;
	}
	if(!makeDir(fullPath, 0700)) {
            qWarning("failed to create %s", fullPath.latin1());
	    return fullPath;
	}
        dircache.remove(type);
    }
    return fullPath;
}

QString KStandardDirs::relativeLocation(const char *type, const QString &absPath)
{
    QString fullPath = absPath;
    int i = absPath.findRev('/');
    if (i != -1)
    {
       fullPath = realPath(absPath.left(i+1))+absPath.mid(i+1); // Normalize
    }

    QStringList candidates = resourceDirs(type);

    for (QStringList::ConstIterator it = candidates.begin();
	 it != candidates.end(); it++)
      if (fullPath.startsWith(*it))
      {
	return fullPath.mid((*it).length());
      }

    return absPath;
}


bool KStandardDirs::makeDir(const QString& dir2, int mode)
{
  QString dir = QDir::convertSeparators( dir2  );
#if 0
    //LR

    // we want an absolute path
    if (dir.at(0) != '/')
        return false;

    QString target = dir;
    uint len = target.length();

    // append trailing slash if missing
    if (dir.at(len - 1) != '/')
        target += '/';

    QString base("");
    uint i = 1;

    while( i < len )
    {
//US        struct stat st;
        int pos = target.find('/', i);
        base += target.mid(i - 1, pos - i + 1);
        QCString baseEncoded = QFile::encodeName(base);
        // bail out if we encountered a problem
//US        if (stat(baseEncoded, &st) != 0)
        QFileInfo baseEncodedInfo(baseEncoded);
        if (!baseEncodedInfo.exists())
        {
          // Directory does not exist....
          // Or maybe a dangling symlink ?
//US          if (lstat(baseEncoded, &st) == 0)
          if (baseEncodedInfo.isSymLink()) {
//US              (void)unlink(baseEncoded); // try removing
              QFile(baseEncoded).remove();
          }

//US	  if ( mkdir(baseEncoded, (mode_t) mode) != 0)
    QDir dirObj;
	  if ( dirObj.mkdir(baseEncoded) != true )
    {
//US	    perror("trying to create local folder");
	    return false; // Couldn't create it :-(
	  }
        }
        i = pos + 1;
    }
    return true;
#endif

    // ********************************************
    // new code for WIN32
  QDir dirObj;


    // we want an absolute path
#ifndef _WIN32_
    if (dir.at(0) != '/')
        return false;
#endif

    QString target = dir;
    uint len = target.length();
#ifndef _WIN32_
    // append trailing slash if missing
    if (dir.at(len - 1) != '/')
        target += '/';
#endif

    QString base("");
    uint i = 1;

    while( i < len )
    {
//US        struct stat st;
#ifndef _WIN32_
        int pos = target.find('/', i);
#else
	int pos = target.find('\\', i);
#endif
	if ( pos < 0 )
	  return true;
        base += target.mid(i - 1, pos - i + 1);
	//QMessageBox::information( 0,"cap111", base, 1 );
/*US
        QCString baseEncoded = QFile::encodeName(base);
        // bail out if we encountered a problem
        if (stat(baseEncoded, &st) != 0)
        {
          // Directory does not exist....
          // Or maybe a dangling symlink ?
          if (lstat(baseEncoded, &st) == 0)
              (void)unlink(baseEncoded); // try removing


          if ( mkdir(baseEncoded, (mode_t) mode) != 0) {
            perror("trying to create local folder");
            return false; // Couldn't create it :-(
          }
        }
*/

        if (dirObj.exists(base) == false)
        {
            //qDebug("KStandardDirs::makeDir try to create : %s" , base.latin1());
          if (dirObj.mkdir(base) != true)
          {
            qDebug("KStandardDirs::makeDir could not create: %s" , base.latin1());
            return false;
          }
        }

        i = pos + 1;
    }
    return true;

}

QString readEnvPath(const char *env)
{
  //#ifdef _WIN32_
  //    return "";
  //#else
  QCString c_path;
  if ( getenv(env) != NULL )
    c_path = QString ( getenv(env) );
  if (c_path.isEmpty())
    return QString::null;
  return QFile::decodeName(c_path);
   //#endif

}

void KStandardDirs::addKDEDefaults()
{
 
    //qDebug("ERROR: KStandardDirs::addKDEDefaults() called ");
    //return;
    QStringList kdedirList;

    // begin KDEDIRS
    QString kdedirs = readEnvPath("MICROKDEDIRS");
    if (!kdedirs.isEmpty())
    {
	tokenize(kdedirList, kdedirs, ":");
    }
    else
    {
	QString kdedir = readEnvPath("MICROKDEDIR");
	if (!kdedir.isEmpty())
        {
           kdedir = KShell::tildeExpand(kdedir);
	   kdedirList.append(kdedir);
        }
    }
//US    kdedirList.append(KDEDIR);
//US for embedded, add qtopia dir as kdedir

#ifndef DESKTOP_VERSION
    QString tmp = readEnvPath("QPEDIR");
    if (!tmp.isEmpty())
      kdedirList.append(tmp);

    tmp = readEnvPath("QTDIR");
    if (!tmp.isEmpty())
      kdedirList.append(tmp);

    tmp = readEnvPath("OPIEDIR");
    if (!tmp.isEmpty())
      kdedirList.append(tmp);

#endif

#ifdef __KDE_EXECPREFIX
    QString execPrefix(__KDE_EXECPREFIX);
    if (execPrefix!="NONE")
       kdedirList.append(execPrefix);
#endif

    QString localKdeDir;

//US    if (getuid())
    if (true)
    {
       localKdeDir = readEnvPath("MICROKDEHOME");
       if (!localKdeDir.isEmpty())
       {
#ifdef _WIN32_
          if (localKdeDir.at(localKdeDir.length()-1) != '\\')
             localKdeDir += '\\';
#else
          if (localKdeDir.at(localKdeDir.length()-1) != '/')
             localKdeDir += '/';
#endif
	  //QMessageBox::information( 0,"localKdeDir",localKdeDir, 1 );
       }
       else
       {
           QString confFile;
#ifdef DESKTOP_VERSION           
           confFile = qApp->applicationDirPath ()+ "/.microkdehome" ;
           QFileInfo fi ( confFile );
           if ( !fi.exists() )
                confFile = QDir::homeDirPath() + "/.microkdehome";
           else
               qDebug("Loading path info from " +  confFile );

#else
           confFile = QDir::homeDirPath() + "/.microkdehome";
#endif
           KConfig cfg (   confFile );
           cfg.setGroup("Global");
           localKdeDir = cfg.readEntry( "MICROKDEHOME", QDir::homeDirPath() + "/kdepim/" );
#ifdef DESKTOP_VERSION           
           if ( localKdeDir.startsWith( "LOCAL:" ) ) {
#ifdef _WIN32_
               localKdeDir = qApp->applicationDirPath () + "\\"+ localKdeDir.mid( 6 );
#else
               localKdeDir = qApp->applicationDirPath () + "/"+ localKdeDir.mid( 6 );
#endif
               qDebug("Using local conf dir %s ",localKdeDir.latin1() );
               // <stdlib.h>
#ifdef _WIN32_
	       QString envSt = "LOCALMICROKDEHOME="+localKdeDir;
	       _putenv( envSt.latin1());
#else
               setenv(  "LOCALMICROKDEHOME",  localKdeDir.latin1(), 1 );
#endif
           }
#endif
       }
    }
    else
    {
       // We treat root different to prevent root messing up the
       // file permissions in the users home directory.
       localKdeDir = readEnvPath("MICROKDEROOTHOME");
       if (!localKdeDir.isEmpty())
       {
          if (localKdeDir.at(localKdeDir.length()-1) != '/')
             localKdeDir += '/';
       }
       else
       {
//US          struct passwd *pw = getpwuid(0);
//US          localKdeDir =  QFile::decodeName((pw && pw->pw_dir) ? pw->pw_dir : "/root")  + "/.microkde/";
              qDebug("KStandardDirs::addKDEDefaults: 1 has to be fixed");
       }

    }

//US    localKdeDir =  appDir();

//US
//  qDebug("KStandardDirs::addKDEDefaults: localKdeDir=%s", localKdeDir.latin1());
    if (localKdeDir != "-/")
    {
        localKdeDir = KShell::tildeExpand(localKdeDir);
        addPrefix(localKdeDir);
    }

    for (QStringList::ConstIterator it = kdedirList.begin();
	 it != kdedirList.end(); it++)
    {
        QString dir = KShell::tildeExpand(*it);
	addPrefix(dir);
    }
    // end KDEDIRS

    // begin XDG_CONFIG_XXX
    QStringList xdgdirList;
    QString xdgdirs = readEnvPath("XDG_CONFIG_DIRS");
    if (!xdgdirs.isEmpty())
    {
	tokenize(xdgdirList, xdgdirs, ":");
    }
    else
    {
	xdgdirList.clear();
        xdgdirList.append("/etc/xdg");
    }

    QString localXdgDir = readEnvPath("XDG_CONFIG_HOME");
    if (!localXdgDir.isEmpty())
    {
       if (localXdgDir.at(localXdgDir.length()-1) != '/')
          localXdgDir += '/';
    }
    else
    {
//US       if (getuid())
       if (true)
       {
          localXdgDir =  QDir::homeDirPath() + "/.config/";
       }
       else
       {
//US          struct passwd *pw = getpwuid(0);
//US          localXdgDir =  QFile::decodeName((pw && pw->pw_dir) ? pw->pw_dir : "/root")  + "/.config/";
              qDebug("KStandardDirs::addKDEDefaults: 2 has to be fixed");
       }
    }

    localXdgDir = KShell::tildeExpand(localXdgDir);
    addXdgConfigPrefix(localXdgDir);

    for (QStringList::ConstIterator it = xdgdirList.begin();
	 it != xdgdirList.end(); it++)
    {
        QString dir = KShell::tildeExpand(*it);
	addXdgConfigPrefix(dir);
    }
    // end XDG_CONFIG_XXX

    // begin XDG_DATA_XXX
    xdgdirs = readEnvPath("XDG_DATA_DIRS");
    if (!xdgdirs.isEmpty())
    {
	tokenize(xdgdirList, xdgdirs, ":");
    }
    else
    {
	xdgdirList.clear();
        for (QStringList::ConstIterator it = kdedirList.begin();
           it != kdedirList.end(); it++)
        {
           QString dir = *it;
           if (dir.at(dir.length()-1) != '/')
             dir += '/';
           xdgdirList.append(dir+"share/");
        }

        xdgdirList.append("/usr/local/share/");
        xdgdirList.append("/usr/share/");
    }

    localXdgDir = readEnvPath("XDG_DATA_HOME");
    if (!localXdgDir.isEmpty())
    {
       if (localXdgDir.at(localXdgDir.length()-1) != '/')
          localXdgDir += '/';
    }
    else
    {
//US       if (getuid())
       if (true)
       {
          localXdgDir =  QDir::homeDirPath() + "/.local/share/";
       }
       else
       {
//US          struct passwd *pw = getpwuid(0);
//US          localXdgDir =  QFile::decodeName((pw && pw->pw_dir) ? pw->pw_dir : "/root")  + "/.local/share/";
              qDebug("KStandardDirs::addKDEDefaults: 3 has to be fixed");
       }
    }

    localXdgDir = KShell::tildeExpand(localXdgDir);
    addXdgDataPrefix(localXdgDir);

    for (QStringList::ConstIterator it = xdgdirList.begin();
	 it != xdgdirList.end(); it++)
    {
        QString dir = KShell::tildeExpand(*it);

	addXdgDataPrefix(dir);
    }
    // end XDG_DATA_XXX


    uint index = 0;
    while (types[index] != 0) {
	addResourceType(types[index], kde_default(types[index]));
	index++;
    }

    addResourceDir("home", QDir::homeDirPath());
}

void KStandardDirs::checkConfig() const
{
/*US
    if (!addedCustoms && KGlobal::_instance && KGlobal::_instance->_config)
        const_cast<KStandardDirs*>(this)->addCustomized(KGlobal::_instance->_config);
*/
    if (!addedCustoms && KGlobal::config())
        const_cast<KStandardDirs*>(this)->addCustomized(KGlobal::config());
}

bool KStandardDirs::addCustomized(KConfig *config)
{
    if (addedCustoms) // there are already customized entries
        return false; // we just quite and hope they are the right ones

    // save the numbers of config directories. If this changes,
    // we will return true to give KConfig a chance to reparse
    uint configdirs = resourceDirs("config").count();

    // reading the prefixes in
    QString oldGroup = config->group();
    config->setGroup("Directories");

    QStringList list;
    QStringList::ConstIterator it;
    list = config->readListEntry("prefixes");
    for (it = list.begin(); it != list.end(); it++)
	addPrefix(*it);

    // iterating over all entries in the group Directories
    // to find entries that start with dir_$type
/*US
    QMap<QString, QString> entries = config->entryMap("Directories");

    QMap<QString, QString>::ConstIterator it2;
    for (it2 = entries.begin(); it2 != entries.end(); it2++)
    {
      QString key = it2.key();
      if (key.left(4) == "dir_") {
        // generate directory list, there may be more than 1.
        QStringList dirs = QStringList::split(',', *it2);
        QStringList::Iterator sIt(dirs.begin());
        QString resType = key.mid(4, key.length());
        for (; sIt != dirs.end(); ++sIt) {
          addResourceDir(resType.latin1(), *sIt);
        }
      }
    }

    // Process KIOSK restrictions.
    config->setGroup("KDE Resource Restrictions");
    entries = config->entryMap("KDE Resource Restrictions");
    for (it2 = entries.begin(); it2 != entries.end(); it2++)
    {
	QString key = it2.key();
        if (!config->readBoolEntry(key, true))
        {
           d->restrictionsActive = true;
           d->restrictions.insert(key.latin1(), &d->restrictionsActive); // Anything will do
           dircache.remove(key.latin1());
        }
    }
*/
    // save it for future calls - that will return
    addedCustoms = true;
    config->setGroup(oldGroup);

    // return true if the number of config dirs changed
    return (resourceDirs("config").count() != configdirs);
}

QString KStandardDirs::localkdedir() const
{
    // Return the prefix to use for saving
    return prefixes.first();
}

QString KStandardDirs::localxdgdatadir() const
{
    // Return the prefix to use for saving
    return d->xdgdata_prefixes.first();
}

QString KStandardDirs::localxdgconfdir() const
{
    // Return the prefix to use for saving
    return d->xdgconf_prefixes.first();
}

void KStandardDirs::setAppDir( const QString &appDir )
{
  mAppDir = appDir;

  if ( mAppDir.right( 1 ) != "/" )
    mAppDir += "/";
}

QString KStandardDirs::appDir()
{
  return mAppDir;
}

// just to make code more readable without macros
QString locate( const char *type,
		const QString& filename/*US , const KInstance* inst*/ )
{
//US    return inst->dirs()->findResource(type, filename);
    return KGlobal::dirs()->findResource(type, filename);
}

QString locateLocal( const char *type,
	             const QString& filename/*US , const KInstance* inst*/ )
{

     QString path = locateLocal(type, filename, true /*US, inst*/);


/*
     static int ccc = 0;
     ++ccc;
     if ( ccc > 13 )
         abort();
*/
     //qDebug("locatelocal: %s" , path.latin1());
     return path;

/*US why do we put all files into one directory. It is quit complicated.
why not staying with the original directorystructure ?


  QString escapedFilename = filename;
  escapedFilename.replace( QRegExp( "/" ), "_" );

  QString path = KStandardDirs::appDir() + type + "_" + escapedFilename;

  kdDebug() << "locate: '" << path << "'" << endl;
  qDebug("locate: %s" , path.latin1());
  return path;
*/
//US so my proposal is this:

//  QString escapedFilename = filename;
//  escapedFilename.replace( QRegExp( "/" ), "_" );

#if 0
#ifdef _WIN32_
  QString path = QDir::convertSeparators(KStandardDirs::appDir() + type + "/" + filename);
#else
  QString path = KStandardDirs::appDir() + type + "/" + filename;
#endif

  //US Create the containing dir if needed
  QFileInfo fi ( path );

  // QString dir=pathurl.directory();
  //	QMessageBox::information( 0,"path", path, 1 );

#ifdef _WIN32_
  KStandardDirs::makeDir(path);
#else
  KStandardDirs::makeDir(fi.dirPath( true ));
#endif

  qDebug("locate22: %s" , path.latin1());
  return path;

#endif

}

QString locateLocal( const char *type,
	             const QString& filename, bool createDir/*US , const KInstance* inst*/ )
{
    // try to find slashes. If there are some, we have to
    // create the subdir first
    int slash = filename.findRev('/')+1;
    if (!slash) // only one filename
//US	return inst->dirs()->saveLocation(type, QString::null, createDir) + filename;
      return KGlobal::dirs()->saveLocation(type, QString::null, createDir) + filename;

    // split path from filename
    QString dir = filename.left(slash);
    QString file = filename.mid(slash);
//US    return inst->dirs()->saveLocation(type, dir, createDir) + file;
    return KGlobal::dirs()->saveLocation(type, dir, createDir) + file;

    // ***************************************************************
#if 0

/*US why do we put all files into one directory. It is quit complicated.
why not staying with the original directorystructure ?


  QString escapedFilename = filename;
  escapedFilename.replace( QRegExp( "/" ), "_" );

  QString path = KStandardDirs::appDir() + type + "_" + escapedFilename;

  kdDebug() << "locate: '" << path << "'" << endl;
  qDebug("locate: %s" , path.latin1());
  return path;
*/
//US so my proposal is this:

//  QString escapedFilename = filename;
//  escapedFilename.replace( QRegExp( "/" ), "_" );

#ifdef _WIN32_
  QString path = QDir::convertSeparators(KStandardDirs::appDir() + type + "/" + filename);
#else
  QString path = KStandardDirs::appDir() + type + "/" + filename;
#endif

  //US Create the containing dir if needed
  KURL pathurl;
  pathurl.setPath(path);
  QString dir=pathurl.directory();
  //	QMessageBox::information( 0,"path", path, 1 );
#ifdef _WIN32_
  KStandardDirs::makeDir(path);
#else
  KStandardDirs::makeDir(dir);
#endif

  return path;
#endif
}
