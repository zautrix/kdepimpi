/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003, 2004 by Michael Buesch                            *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * copyright (C) 2004 by Ulf Schenk
 * This file is originaly based on version 1.0.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: globalstuff.h,v 1.4 2004/10/20 00:15:27 ulf69 Exp $
 **************************************************************************/  

#ifndef __GLOBALSTUFF_H
#define __GLOBALSTUFF_H

#ifndef PWM_EMBEDDED
#include "config.h"
#endif

#include "compiler.h"

  //US BUG: the following code caused compile errors with certain gcccompilers (2.95).
  // Because of that I replaced it with a Qt version, which should do the same.
#include <string>

#ifndef PWM_EMBEDDED
#include <sstream>
#else
#include <qstring.h>
#include <qtextstream.h>
#endif

#ifndef CONFIG_KEYCARD
class QWidget;
void no_keycard_support_msg_box(QWidget *parentWidget);
#endif // CONFIG_KEYCARD

#ifdef PROG_NAME
# undef PROG_NAME
#endif
#define PROG_NAME	"PwM/Pi"

#ifdef PACKAGE_NAME
# undef PACKAGE_NAME
#endif
#define PACKAGE_NAME	"pwm-pi"

#ifdef PACKAGE_VER
# undef PACKAGE_VER
#endif
#define PACKAGE_VER	"1.0.1"

#ifdef CONFIG_DEBUG
# define PWM_DEBUG
#else
# undef PWM_DEBUG
#endif

#ifdef QT_MAKE_VERSION
# undef QT_MAKE_VERSION
#endif
#define QT_MAKE_VERSION(a,b,c)	(((a) << 16) | ((b) << 8) | (c))

/** remove "unused parameter" warnings */
#ifdef PARAM_UNUSED
# undef PARAM_UNUSED
#endif
#define PARAM_UNUSED(x)	(void)x

/** return the number of elements in an array */
#ifdef array_size
# undef array_size
#endif
#define array_size(x)	(sizeof(x) / sizeof((x)[0]))

//US BUG: the following code caused compile errors with certain gcccompilers (2.95).
// Because of that I replaced it with a Qt version, which should do the same.
#ifndef PWM_EMBEDDED
/** convert something to string using ostringstream */
template <class T> inline
std::string tostr(const T &t)
{
  std::ostringstream s;
  s << t;
  return s.str();
}
#else
/** convert something to string using ostringstream */
template <class T> inline
std::string tostr(const T &t)
{
  QString result;
  QTextOStream(&result) << t;
  return result.latin1();
}
#endif

/** delete the memory and NULL the pointer */
template<class T> inline
void delete_and_null(T *&p)
{
	delete p;
	p = 0;
}
/** delete the memory if the pointer isn't a NULL pointer */
template<class T> inline
void delete_ifnot_null(T *&p)
{
	if (p)
		delete_and_null(p);
}

template<class T> inline
void delete_and_null_array(T *&p)
{
	delete [] p;
	p = 0;
}

template<class T> inline
void delete_ifnot_null_array(T *&p)
{
	if (p)
		delete_and_null_array(p);
}

#endif // GLOBALSTUFF_H
