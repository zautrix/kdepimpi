/* This file is part of the KDE libraries
   Copyright (C) 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000 Michael Matz <matz@kde.org>

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
//US #include <config.h>
#include <qclipboard.h>
#include <qfile.h>
#include <qtimer.h>
#include <qobjectdict.h>
#include <qwidgetlist.h>
#include <qwidget.h>

#include "kapplication.h"
#include "klibloader.h"
#include "kstandarddirs.h"
#include "kdebug.h"
#include "klocale.h"

/*US
#ifndef NDEBUG
#include "ltdl.h"
#endif
*/

//US do everything through qlibrary
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#include <qtopia/qlibrary.h>
#else
#include <qlibrary.h> 
#endif


template class QAsciiDict<KLibrary>;

#include <stdlib.h> //getenv

/*US
#if HAVE_DLFCN_H
#  include <dlfcn.h>
#endif
 
#ifdef RTLD_GLOBAL
#  define LT_GLOBAL             RTLD_GLOBAL
#else
#  ifdef DL_GLOBAL
#    define LT_GLOBAL           DL_GLOBAL
#  endif
#endif
#ifndef LT_GLOBAL
#  define LT_GLOBAL             0
#endif
*/

/*US
extern "C" {
extern int lt_dlopen_flag;
}
*/

KLibFactory::KLibFactory( QObject* parent, const char* name )
    : QObject( parent, name )
{
}

KLibFactory::~KLibFactory()
{
//    kdDebug(150) << "Deleting KLibFactory " << this << endl;
}

QObject* KLibFactory::create( QObject* parent, const char* name, const char* classname, const QStringList &args )
{
    QObject* obj = createObject( parent, name, classname, args );
    if ( obj )
	emit objectCreated( obj );
    return obj;
}


QObject* KLibFactory::createObject( QObject*, const char*, const char*, const QStringList &)
{
    return 0;
}


// -----------------------------------------------

//US KLibrary::KLibrary( const QString& libname, const QString& filename, void * handle )
KLibrary::KLibrary( const QString& libname, const QString& filename, QLibrary* handle )
{
    /* Make sure, we have a KLibLoader */
    (void) KLibLoader::self();
    m_libname = libname;
    m_filename = filename;
    m_handle = handle;
    m_factory = 0;
    m_timer = 0;
}

KLibrary::~KLibrary()
{
//    kdDebug(150) << "Deleting KLibrary " << this << "  " << m_libname << endl;
    if ( m_timer && m_timer->isActive() )
	m_timer->stop();

    // If any object is remaining, delete
    if ( m_objs.count() > 0 )
	{
	    QPtrListIterator<QObject> it( m_objs );
	    for ( ; it.current() ; ++it )
		{
		    kdDebug(150) << "Factory still has object " << it.current() << " " << it.current()->name () << " Library = " << m_libname << endl;
		    disconnect( it.current(), SIGNAL( destroyed() ),
				this, SLOT( slotObjectDestroyed() ) );
		}
	    m_objs.setAutoDelete(true);
	    m_objs.clear();
	}

    if ( m_factory ) {
//	kdDebug(150) << " ... deleting the factory " << m_factory << endl;
	delete m_factory;
    }
}

QString KLibrary::name() const
{
    return m_libname;
}

QString KLibrary::fileName() const
{
    return m_filename;
}

KLibFactory* KLibrary::factory()
{
    if ( m_factory )
        return m_factory;

        QCString symname;
        symname.sprintf("init_%s", name().latin1() );
    
        void* sym = symbol( symname );
        if ( !sym )
        {
            qDebug("KLibrary: The library %s does not offer an %s function", name().latin1(), symname.data());
#ifndef NDEBUG
//US            qDebug("KLibrary: errorcode: %s", lt_dlerror());
#endif            
            kdWarning(150) << "KLibrary: The library " << name().latin1() << " does not offer an init_" << name().latin1() << " function" << endl;
            return 0;
    }

    typedef KLibFactory* (*t_func)();
    t_func func = (t_func)sym;
    m_factory = func();

    if( !m_factory )
    {
        kdWarning(150) << "KLibrary: The library " << name() << " does not offer a KDE compatible factory" << endl;
        return 0;
    }

    connect( m_factory, SIGNAL( objectCreated( QObject * ) ),
             this, SLOT( slotObjectCreated( QObject * ) ) );

    return m_factory;
}

void* KLibrary::symbol( const char* symname ) const
{
//US     void* sym = lt_dlsym( (lt_dlhandle) m_handle, symname );
         void* sym = m_handle->resolve( symname );    
    if ( !sym )
    {
//US        kdWarning(150) << "KLibrary: " << lt_dlerror() << endl;
        return 0;
    }

    return sym;
}

bool KLibrary::hasSymbol( const char* symname ) const
{
//US    void* sym = lt_dlsym( (lt_dlhandle) m_handle, symname );
         void* sym = m_handle->resolve( symname );    
    return (sym != 0L );
}

void KLibrary::unload() const
{
   if (KLibLoader::s_self)
      KLibLoader::s_self->unloadLibrary(QFile::encodeName(name()));
}

void KLibrary::slotObjectCreated( QObject *obj )
{
  if ( !obj )
    return;

  if ( m_timer && m_timer->isActive() )
    m_timer->stop();

  if ( m_objs.containsRef( obj ) )
      return; // we know this object already

  connect( obj, SIGNAL( destroyed() ),
           this, SLOT( slotObjectDestroyed() ) );

  m_objs.append( obj );
}

void KLibrary::slotObjectDestroyed()
{
  m_objs.removeRef( sender() );

  if ( m_objs.count() == 0 )
  {
//    kdDebug(150) << "KLibrary: shutdown timer for " << name() << " started!"
//                 << endl;

    if ( !m_timer )
    {
      m_timer = new QTimer( this, "klibrary_shutdown_timer" );
      connect( m_timer, SIGNAL( timeout() ),
               this, SLOT( slotTimeout() ) );
    }

    // as long as it's not stable make the timeout short, for debugging
    // pleasure (matz)
    //m_timer->start( 1000*60, true );
    m_timer->start( 1000*10, true );
  }
}

void KLibrary::slotTimeout()
{
  if ( m_objs.count() != 0 )
    return;

  /* Don't go through KLibLoader::unloadLibrary(), because that uses the
     ref counter, but this timeout means to unconditionally close this library
     The destroyed() signal will take care to remove us from all lists.
  */
  delete this;
}

// -------------------------------------------------

/* This helper class is needed, because KLibraries can go away without
   being unloaded. So we need some info about KLibraries even after its
   death. */
class KLibWrapPrivate
{
public:
//US    KLibWrapPrivate(KLibrary *l, lt_dlhandle h);
    KLibWrapPrivate(KLibrary *l, QLibrary* h);

    KLibrary *lib;
    enum {UNKNOWN, UNLOAD, DONT_UNLOAD} unload_mode;
    int ref_count;
//US    lt_dlhandle handle;
    QLibrary *handle;
    QString name;
    QString filename;
};

//US KLibWrapPrivate::KLibWrapPrivate(KLibrary *l, lt_dlhandle h)
KLibWrapPrivate::KLibWrapPrivate(KLibrary *l, QLibrary* h)
 : lib(l), ref_count(1), handle(h), name(l->name()), filename(l->fileName())
{
    unload_mode = UNKNOWN;
/*US    
    if (lt_dlsym(handle, "__kde_do_not_unload") != 0) {
//        kdDebug(150) << "Will not unload " << name << endl;
        unload_mode = DONT_UNLOAD;
    } else if (lt_dlsym(handle, "__kde_do_unload") != 0) {
        unload_mode = UNLOAD;
    }
*/
//US use instead:
    if (h->resolve("__kde_do_not_unload") != 0) {
//        kdDebug(150) << "Will not unload " << name << endl;
        unload_mode = DONT_UNLOAD;
    } else if (h->resolve("__kde_do_unload") != 0) {
        unload_mode = UNLOAD;
    }
}

class KLibLoaderPrivate
{
public:
    QPtrList<KLibWrapPrivate> loaded_stack;
    QPtrList<KLibWrapPrivate> pending_close;
    enum {UNKNOWN, UNLOAD, DONT_UNLOAD} unload_mode;

    QString errorMessage;
};

KLibLoader* KLibLoader::s_self = 0;

KLibLoader* KLibLoader::self()
{
    if ( !s_self )
        s_self = new KLibLoader;
    return s_self;
}

void KLibLoader::cleanUp()
{
  if ( !s_self )
    return;

  delete s_self;
  s_self = 0;
}

KLibLoader::KLibLoader( QObject* parent, const char* name )
    : QObject( parent, name )
{
    s_self = this;
    d = new KLibLoaderPrivate;
//US    lt_dlinit();
    d->unload_mode = KLibLoaderPrivate::UNKNOWN;
    if (getenv("KDE_NOUNLOAD") != 0)
        d->unload_mode = KLibLoaderPrivate::DONT_UNLOAD;
    else if (getenv("KDE_DOUNLOAD") != 0)
        d->unload_mode = KLibLoaderPrivate::UNLOAD;
    d->loaded_stack.setAutoDelete( true );
}

KLibLoader::~KLibLoader()
{
//    kdDebug(150) << "Deleting KLibLoader " << this << "  " << name() << endl;

    QAsciiDictIterator<KLibWrapPrivate> it( m_libs );
    for (; it.current(); ++it )
    {
      kdDebug(150) << "The KLibLoader contains the library " << it.current()->name
        << " (" << it.current()->lib << ")" << endl;
      d->pending_close.append(it.current());
    }

    close_pending(0);

    delete d;
}

//static
QString KLibLoader::findLibrary( const char * name/*US , const KInstance * instance*/ )
{
    QCString libname( name );

    // only append ".la" if there is no extension
    // this allows to load non-libtool libraries as well
    // (mhk, 20000228)
    int pos = libname.findRev('/');
    if (pos < 0)
      pos = 0;
/*US      
    if (libname.find('.', pos) < 0) {
      libname += ".la";
    }
*/    
//US in the microedition we work only with shared libraries.

    if (libname.find('.', pos) < 0) {
#ifdef _WIN32_
      libname += ".dll";
#else
      libname += ".so";
#endif
    }

    // only look up the file if it is not an absolute filename
    // (mhk, 20000228)
    QString libfile;
    if (libname[0] == '/')
      libfile = libname;
    else
    {
//US at this point the libname must exist as real filesname. No expansions will be made later
// in findResources. Because of that we prepend the lib prefix here to the name 
//US I add also the "lib" prefix. I do not how could this could have worked before without it?
#ifndef _WIN32_
    libname.insert(pos, "lib");
#endif
    
    
//US      libfile = instance->dirs()->findResource( "module", libname );
    //qDebug("libname = %s ",libname.data() );
      libfile = KGlobal::dirs()->findResource( "module", libname );
      //qDebug("libfile = %s ",libfile.latin1() );

      if ( libfile.isEmpty() )
      {
//US        libfile = instance->dirs()->findResource( "lib", libname );
        libfile = KGlobal::dirs()->findResource( "lib", libname );
        //qDebug("libfile2 = %s ",libfile.latin1() );
#ifndef NDEBUG
        if ( !libfile.isEmpty() && libname.left(3) == "lib" ) // don't warn for kdeinit modules
          kdDebug(150) << "library " << libname << " not found under 'module' but under 'lib'" << endl;
#endif
      }
      if ( libfile.isEmpty() )
      {
#ifndef NDEBUG
        kdDebug(150) << "library=" << libname << ": No file names " << libname.data() << " found in paths." << endl;
        self()->d->errorMessage = i18n("Library files for \"%1\" not found in paths").arg(libname); 
            
        qDebug("KLibLoader::library could not find library: %s", libname.data());
#endif
            
      }
      else
            self()->d->errorMessage = QString::null;
    }

    //qDebug("return libfile = %s ",libfile.latin1() );
    return libfile;
}


KLibrary* KLibLoader::globalLibrary( const char *name )
{
KLibrary *tmp;
/*US
int olt_dlopen_flag = lt_dlopen_flag;

   lt_dlopen_flag |= LT_GLOBAL;
   kdDebug(150) << "Loading the next library global with flag " 
                << lt_dlopen_flag
                << "." << endl;
*/                
   tmp = library(name);
/*US   
   lt_dlopen_flag = olt_dlopen_flag;
*/
return tmp;
}


KLibrary* KLibLoader::library( const char *name )
{
    if (!name)
        return 0;

    KLibWrapPrivate* wrap = m_libs[name];
    if (wrap) {
      /* Nothing to do to load the library.  */
      wrap->ref_count++;
      return wrap->lib;
    }

    /* Test if this library was loaded at some time, but got
       unloaded meanwhile, whithout being dlclose()'ed.  */
    QPtrListIterator<KLibWrapPrivate> it(d->loaded_stack);
    for (; it.current(); ++it) {
      if (it.current()->name == name)
        wrap = it.current();
    }

    if (wrap) {
      d->pending_close.removeRef(wrap);
      if (!wrap->lib) {
        /* This lib only was in loaded_stack, but not in m_libs.  */
        wrap->lib = new KLibrary( name, wrap->filename, wrap->handle );
      }
      wrap->ref_count++;
    } else {
      QString libfile = findLibrary( name );
      if ( libfile.isEmpty() )
        return 0;
#ifdef DESKTOP_VERSION
      QLibrary *qlib = new QLibrary( libfile.latin1() );
#else
      QLibrary *qlib = new QLibrary( libfile.latin1(), QLibrary::Immediately );
#endif
        
//US      lt_dlhandle handle = lt_dlopen( libfile.latin1() );
//US      if ( !handle )
      if ( !qlib )
      {
          qDebug( "KLibLoader::library could not load library: %s", libfile.latin1());
          d->errorMessage = QString::null;
          return 0;
      }
      else
        d->errorMessage = QString::null;

      KLibrary *lib = new KLibrary( name, libfile, qlib );
      wrap = new KLibWrapPrivate(lib, qlib);
      d->loaded_stack.prepend(wrap);
    }
    m_libs.insert( name, wrap );

    connect( wrap->lib, SIGNAL( destroyed() ),
             this, SLOT( slotLibraryDestroyed() ) );

    return wrap->lib;
}

QString KLibLoader::lastErrorMessage() const
{
    return d->errorMessage;
}

void KLibLoader::unloadLibrary( const char *libname )
{
  KLibWrapPrivate *wrap = m_libs[ libname ];
  if (!wrap)
    return;
  if (--wrap->ref_count)
    return;

//  kdDebug(150) << "closing library " << libname << endl;

  m_libs.remove( libname );

  disconnect( wrap->lib, SIGNAL( destroyed() ),
              this, SLOT( slotLibraryDestroyed() ) );
  close_pending( wrap );
}

KLibFactory* KLibLoader::factory( const char* name )
{
    KLibrary* lib = library( name );
    if ( !lib )
        return 0;

    return lib->factory();
}

void KLibLoader::slotLibraryDestroyed()
{
  const KLibrary *lib = static_cast<const KLibrary *>( sender() );

  QAsciiDictIterator<KLibWrapPrivate> it( m_libs );
  for (; it.current(); ++it )
    if ( it.current()->lib == lib )
    {
      KLibWrapPrivate *wrap = it.current();
      wrap->lib = 0;  /* the KLibrary object is already away */
      m_libs.remove( it.currentKey() );
      close_pending( wrap );
      return;
    }
}

void KLibLoader::close_pending(KLibWrapPrivate *wrap)
{
  if (wrap && !d->pending_close.containsRef( wrap ))
    d->pending_close.append( wrap );

  /* First delete all KLibrary objects in pending_close, but _don't_ unload
     the DSO behind it.  */
  QPtrListIterator<KLibWrapPrivate> it(d->pending_close);
  for (; it.current(); ++it) {
    wrap = it.current();
    if (wrap->lib) {
      disconnect( wrap->lib, SIGNAL( destroyed() ),
                  this, SLOT( slotLibraryDestroyed() ) );
      delete wrap->lib;
      wrap->lib = 0;
    }
  }

  if (d->unload_mode == KLibLoaderPrivate::DONT_UNLOAD) return;

  bool deleted_one = false;
  while ((wrap = d->loaded_stack.first())) {
    /* Let's first see, if we want to try to unload this lib.
       If the env. var KDE_DOUNLOAD is set, we try to unload every lib.
       If not, we look at the lib itself, and unload it only, if it exports
       the symbol __kde_do_unload. */
    if (d->unload_mode != KLibLoaderPrivate::UNLOAD
        && wrap->unload_mode != KLibWrapPrivate::UNLOAD)
      break;

    /* Now ensure, that the libs are only unloaded in the reverse direction
       they were loaded.  */
    if (!d->pending_close.containsRef( wrap )) {
      if (!deleted_one)
        /* Only diagnose, if we really haven't deleted anything. */
//        kdDebug(150) << "try to dlclose " << wrap->name << ": not yet" << endl;
      break;
    }

//    kdDebug(150) << "try to dlclose " << wrap->name << ": yes, done." << endl;

#if 0
#ifndef Q_WS_QWS
    if ( !deleted_one ) {
      /* Only do the hack once in this loop.
         WABA: *HACK*
         We need to make sure to clear the clipboard before unloading a DSO
         because the DSO could have defined an object derived from QMimeSource
         and placed that on the clipboard. */
      /*kapp->clipboard()->clear();*/

      /* Well.. let's do something more subtle... convert the clipboard context
         to text. That should be safe as it only uses objects defined by Qt. */

      QWidgetList *widgetlist = QApplication::topLevelWidgets();
      QWidget *co = widgetlist->first();
      while (co) {
        if (qstrcmp(co->name(), "internal clipboard owner") == 0) {
          if (XGetSelectionOwner(co->x11Display(), XA_PRIMARY) == co->winId())
	    kapp->clipboard()->setText(kapp->clipboard()->text());

          break;
        }
        co = widgetlist->next();
      }
      delete widgetlist;
    }
#else
    // FIXME(E): Implement in Qt Embedded
#endif

#endif // 0
    deleted_one = true;
//US    lt_dlclose(wrap->handle);
    wrap->handle->unload();
    
    d->pending_close.removeRef(wrap);
    /* loaded_stack is AutoDelete, so wrap is freed */
    d->loaded_stack.remove();
  }
}

void KLibLoader::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void KLibFactory::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

//US #include "klibloader.moc"
