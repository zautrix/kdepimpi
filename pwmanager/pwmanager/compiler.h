/***************************************************************************
 *                                                                         *
 *   Derived from the linux-2.6 tree                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef __PWMANAGER_COMPILER_H
#define __PWMANAGER_COMPILER_H

#ifdef _WIN32_
#  define __builtin_expect(x, expected_value) (x)
#define NOREGPARM	__attribute__((regparm(0)))
#define REGPARM		__attribute__((regparm(3)))
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#else
#ifdef __deprecated
# undef __deprecated
#endif
#if __GNUC__ >= 3
# if __GNUC_MINOR__ > 0
#  define __deprecated	__attribute__((deprecated))
# endif
#elif __GNUC__ == 2
# if __GNUC_MINOR__ < 96
#  ifdef __builtin_expect
#   undef __builtin_expect
#  endif
#  define __builtin_expect(x, expected_value) (x)
# endif
#else
# error "Sorry, your compiler is too old/not supported."
#endif

/*
 * Allow us to mark functions as 'deprecated' and have gcc emit a nice
 * warning for each use, in hopes of speeding the functions removal.
 * Usage is:
 * 		int __deprecated foo(void)
 */
#ifndef __deprecated
# define __deprecated		/* unimplemented */
#endif

/* define likely() and unlikely() */
#ifdef likely
# undef likely
#endif
#ifdef unlikely
# undef unlikely
#endif
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#ifdef NOREGPARM
# undef NOREGPARM
#endif
#define NOREGPARM	__attribute__((regparm(0)))
#ifdef REGPARM
# undef REGPARM
#endif
#define REGPARM		__attribute__((regparm(3)))
#endif
#endif // __PWMANAGER_COMPILER_H
