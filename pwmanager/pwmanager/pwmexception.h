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
 * $Id: pwmexception.h,v 1.3 2006/02/24 19:41:09 zautrix Exp $
 **************************************************************************/  

#ifndef __PWMEXCEPTION_H
#define __PWMEXCEPTION_H

//#include "globalstuff.h"

#include <iostream>
#include <string>
using std::string;
using std::cerr;
using std::cout;
using std::endl;

/* This is an internal function to reduce code-overhead
 * of the BUG(), WARN(), TOD0() and FiXME() macros. Please use
 * these macros instead of calling this function directly.
 */
void pwmFatal(const char *id,
	      const char *file,
	      int line);

/** Use PWM_ASSERT(condition) for debugging assertions.
  * "condition" is eaten up and replaced with a NOP
  * when debugging is disabled.
  *
  * PWM_ASSERT_NOEAT(condition) is the same as PWM_ASSERT(condition),
  * but it does _not_ eat up "condition" and ensures that
  * condition is always evaluated.
  */
#ifdef PWM_ASSERT
# undef PWM_ASSERT
#endif
#ifdef PWM_ASSERT_NOEAT
# undef PWM_ASSERT_NOEAT
#endif
#ifdef PWM_DEBUG
# define PWM_ASSERT(x)	do {							\
				if (unlikely(!(x))) {				\
					cerr << "PWM_ASSERT failed: (" << #x	\
					     << ") in " << __FILE__		\
					     << ":" << __LINE__			\
					     << endl;				\
				}						\
			} while (0)
# define PWM_ASSERT_NOEAT(x)	do { PWM_ASSERT(x); } while (0)
#else // PWM_DEBUG
# define PWM_ASSERT(x)		do { } while (0)
# define PWM_ASSERT_NOEAT(x)	do { if (x) ; } while (0)
#endif // PWM_DEBUG

/** Insert a BUG() into code paths which clearly show
  * a bug in the code and which should, under normal
  * circumstances, never execute.
  */
#ifdef BUG
# undef BUG
#endif
#define BUG()	do { pwmFatal("BUG", __FILE__, __LINE__); } while (0)

/** Use BUG_ON(condition) to print a bug-message if "condition"
  * is true. This is also enabled in non-debugging code.
  */
#ifdef BUG_ON
# undef BUG_ON
#endif
#define BUG_ON(x)	do { if (unlikely(x))	BUG(); } while (0)

/** Insert a WARN() into code-paths which should not
  * execute normally, but if they do it's non-fatal.
  */
#ifdef WARN
# undef WARN
#endif
#define WARN()	do { pwmFatal("badness", __FILE__, __LINE__); } while (0)

/** Same as BUG_ON() but prints a warning-message */
#ifdef WARN_ON
# undef WARN_ON
#endif
#define WARN_ON(x)	do { if (unlikely(x))	WARN(); } while (0)

/** Insert this into code which is incomplete */
#ifdef TODO
# undef TODO
#endif
#define TODO()	do { pwmFatal("TODO", __FILE__, __LINE__); } while (0)

/** Insert this into code which likely contains bugs */
#ifdef FIXME
# undef FIXME
#endif
#define FIXME()	do { pwmFatal("FIXME", __FILE__, __LINE__); } while (0)


/** PwM error codes */
enum PwMerror {
	e_success = 0,

	// file access errors
	e_filename,
	e_readFile,
	e_writeFile,
	e_openFile,
	e_accessFile, // permission error, etc...
	e_fileGeneric,
	e_alreadyOpen,

	// other file errors
	e_fileVer,
	e_fileFormat,		// format error
	e_unsupportedFormat,	// completely unsupported format
	e_setFilePointer,
	e_fileBackup,
	e_fileCorrupt,		// file data has correct format,
				// but is corrupt (checksum error, etc)

	// password errors
	e_wrongPw,
	e_getPw,
	e_weakPw,
	e_noPw,

	// action not implemented errors
	e_hashNotImpl,
	e_cryptNotImpl,

	// argument/parameter errors
	e_incompleteArg,
	e_invalidArg,

	// misc
	e_writeHeader,
	e_serializeDta,
	e_enc,
	e_entryExists,
	e_categoryExists,
	e_maxAllowedEntr,	// no more entries can be added.
	e_outOfMem,
	e_lock,			// error while (un)locking
	e_docNotSaved,		// doc wasn't saved to a file, yet.
	e_docIsEmpty,
	e_binEntry,
	e_normalEntry,
	e_syncError,

	e_generic
};

/** can be used for general exception faults */
class PwMException
{
public:
	enum exceptionId
	{
		EX_GENERIC = 0,
		EX_OPEN,
		EX_CLOSE,
		EX_READ,
		EX_WRITE,
		EX_LOAD_MODULE,
		EX_PARSE
	};

public:
	PwMException(exceptionId id = EX_GENERIC,
		     const char *message = "")
		{
			exId = id;
			exMsg = message;
		}

	exceptionId getId()
			{ return exId; }
	const char* getMessage()
			{ return exMsg; }

protected:
	/** ID of this exception */
	exceptionId exId;
	/** additional error-message for this exception */
	const char *exMsg;
};

void __printInfo(const string &msg);
void __printWarn(const string &msg);
void __printError(const string &msg);

#ifdef PWM_DEBUG
  void __printDebug(const string &msg);
# define printDebug(x)	__printDebug(x)
#else
# define printDebug(x)	do { } while (0)
#endif

#define printInfo(x)	__printInfo(x)
#define printWarn(x)	__printWarn(x)
#define printError(x)	__printError(x)

#include "globalstuff.h"
#endif // __PWMEXCEPTION_H
