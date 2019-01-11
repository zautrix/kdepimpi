/*  This file is part of the KDE libraries
    Copyright (C) 2001,2002 Ellis Whitehead <ellis@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef __KSHORTCUT_H
#define __KSHORTCUT_H
/*US
#include <qkeysequence.h>
#include <qstring.h>

class QKeyEvent;
class KKeyNative;
*/
/**
* A KKey object represents a single key with possible modifiers
* (Shift, Ctrl, Alt, Win).  It can represent both keys which are
* understood by Qt as well as those which are additionally supported
* by the underlying system (e.g. X11).
* @see KKeyNative
* @see KKeySequence
* @see KShortcut
*/
/*US
class KKey
{
 public:
*/ 
        /**
	 * The number of flags.
	 * @see ModFlag
	 */
/*US   
	enum { MOD_FLAG_COUNT = 4 };
	enum { QtWIN = (Qt::ALT << 1) };
*/  
	/**
	 * Flags to represent the modifiers. You can combine modifiers
	 * by ORing them.
	 */
/*US
	enum ModFlag {
		SHIFT = 0x01,
		CTRL = 0x02,
		ALT = 0x04,
		WIN = 0x08
	};
*/
	/**
	 * Creates a new null KKey.
	 * @see clear()
	 * @see isNull()
	 * @see null()
	 */
//US	KKey();

	/**
	 * Creates a new key for the given Qt key code.
	 * @param keyQt the qt keycode
	 * @see Qt::Key
	 */
//US	KKey( int keyQt );

	/**
	 * Creates a new key from the first key code of the given key sequence.
	 * @param keySeq the key sequence that contains the key 
	 */
//US	KKey( const QKeySequence& keySeq );

	/**
	 * Extracts the key from the given key event.
	 * @param keyEvent the key event to get the key from
	 */
//US	KKey( const QKeyEvent* keyEvent );

	/**
	 * Copy constructor.
	 */
//US	KKey( const KKey& key );

	/**
	 * Creates a new key from the given description. The form of the description
	 * is "[modifier+[modifier+]]+key", for example "e", "CTRL+q" or 
	 * "CTRL+ALT+DEL". Allowed modifiers are "SHIFT", "CTRL", "ALT", "WIN" and
	 * "META". "WIN" and "META" are equivalent. Modifiers are not case-sensitive. 
	 * @param key the description of the key
	 * @see KKeyServer::Sym::init()
	 */
//US	KKey( const QString& key );
	/**
	 * @internal
	 */
//US	KKey( uint key, uint mod );
//US	~KKey();

 // Initialization methods
	/**
	 * Clears the key. The key is null after calling this function.
	 * @see isNull()
	 */
//US	void clear();

	/**
	 * Initializes the key with the given Qt key code.
	 * @param keyQt the qt keycode
	 * @return true if successful, false otherwise
	 * @see Qt::Key
	 */
//US	bool init( int keyQt );

	/**
	 * Initializes the key with the first key code of the given key sequence.
	 * @param keySeq the key sequence that contains the key 
	 * @return true if successful, false otherwise
	 */
//US	bool init( const QKeySequence& keySeq );

	/**
	 * Initializes the key by extracting the code from the given key event.
	 * @param keyEvent the key event to get the key from
	 * @return true if successful, false otherwise
	 */
//US	bool init( const QKeyEvent* keyEvent );

	/**
	 * Copies the given key.
	 * @param key the key to copy
	 * @return true if successful, false otherwise
	 */
//US	bool init( const KKey& key );

	/**
	 * Initializes the key with the given description. The form of the description
	 * is "[modifier+[modifier+]]+key", for example "e", "CTRL+q" or 
	 * "CTRL+ALT+DEL". Allowed modifiers are "SHIFT", "CTRL", "ALT", "WIN" and
	 * "META". "WIN" and "META" are equivalent. Modifiers are not case-sensitive. 
	 * @param key the description of the key
	 * @return true if successful, false otherwise
	 * @see KKeyServer::Sym::init()
	 */
//US	bool init( const QString& );

	/**
	 * @internal
	 */
//US	bool init( uint key, uint mod );

	/**
	 * Copies the key.
	 */
//US	KKey& operator =( const KKey& key )
//US		{ init( key ); return *this; }

 // Query methods.
	/**
	 * Returns true if the key is null (after @ref clear() or empty
	 * constructor).
	 * @return true if the key is null
	 * @see clear()
	 * @see null()
	 */
//US	bool isNull() const;

	/**
	 * @internal
	 */
//US	bool isValidQt() const;

	/**
	 * @internal
	 */
//US	bool isValidNative() const;

	/**
	 * @internal
	 */
//US	uint sym() const;
	/**
	 * @internal
	 */
//US	uint modFlags() const;

 // Comparison Methods
	/**
	 * Compares this key with the given KKey object. Returns a negative 
	 * number if the given KKey is larger, 0 if they are equal and
	 * a positive number this KKey is larger. The returned value
	 * is the difference between the symbol or, if the symbols 
	 * are equal, the difference between the encoded modifiers.
	 * @param key the key to compare with this key
	 * @return a negative number if the given KKey is larger, 0 if 
	 * they are equal and a positive number this KKey is larger
	 */
//US	int compare( const KKey& key ) const;

	/**
	 * Compares the symbol and modifiers of both keys.
	 * @see compare()
	 */
//US	bool operator == ( const KKey& key ) const
//US		{ return compare( key ) == 0; }
	/**
	 * Compares the symbol and modifiers of both keys.
	 * @see compare()
	 */
//US	bool operator != ( const KKey& key ) const
//US		{ return compare( key ) != 0; }
	/**
	 * Compares the symbol and modifiers of both keys.
	 * @see compare()
	 */
//US	bool operator < ( const KKey& key ) const
//US		{ return compare( key ) < 0; }

 // Conversion methods.
	/**
	 * Returns the qt key code.
	 * @return the qt key code or 0 if there is no key set.
	 * @see Qt::Key
	 */
//US	int keyCodeQt() const;
	
	/**
	 * Returns a human-readable representation of the key in the form
	 * "modifier+key".
	 * @return the string representation of the key
	 */
//US	QString toString() const;

	/**
	 * @internal
	 */
//US  QString toStringInternal() const;

 // Operation methods
	/**
	 * @internal
	 */
//US	void simplify();
	
	/**
	 * Returns a null key.
	 * @return the null key
	 * @see isNull()
	 * @see clear()
	 */
//US	static KKey& null();

	/**
	 * Returns a user-readable representation of the given modifiers.
	 * @param f the modifiers to convert
	 * @return the string representation of the modifiers
	 */
//US	static QString modFlagLabel( ModFlag f );

//US private:
	/*
	 * Under X11, m_key will hold an X11 key symbol.
	 * For Qt/Embedded, it will hold the Qt key code.
	 */
	/**
	 * Returns the native key symbol value key.  Under X11, this is the X
	 * keycode.  Under Qt/Embedded, this is the Qt keycode.
	 * @see /usr/include/X11/keysymdef.h
	 * @see qnamespace.h
	 */
//US	uint m_sym;
	/**
	 * m_mod holds the
	 */
//US	uint m_mod;

//US private:
//US	friend class KKeyNative;
//US};

/**
* A KKeySequence object holds a sequence of up to 4 keys.
* Ex: Ctrl+X,I
* @see KKey
* @see KShortcut
*/

//USclass KKeySequence
//US{
//US public:
        /// Defines the maximum length of the key sequence 
//US        enum { MAX_KEYS = 4 }; 

	/**
	 * Create a new null key sequence.
	 * @see isNull()
	 * @see null()
	 * @see clear()
	 */
//US	KKeySequence();

	/**
	 * Copies the given qt key sequence.
	 * @param keySeq the qt key sequence to copy
	 */
//US	KKeySequence( const QKeySequence& keySeq );

	/**
	 * Create a new key sequence that only contains the given key.
	 * @param key the key to add
	 */
//US	KKeySequence( const KKey& key );

	/**
	 * Create a new key sequence that only contains the given key.
	 * @param key the key to add
	 */
//US	KKeySequence( const KKeyNative& key );

	/**
	 * Copies the given key sequence.
	 * @param keySeq the key sequence to copy
	 */
//US	KKeySequence( const KKeySequence& keySeq );

	/**
	 * Creates a new key sequence that contains the given key sequence. 
	 * The description consists of comma-separated keys as
	 * required by @ref KKey::KKey(const QString&).
	 * @param keySeq the description of the key
	 * @see KKeyServer::Sym::init()
	 * @see KKey::KKey(const QString&)
	 */
//US	KKeySequence( const QString& keySeq );

//US	~KKeySequence();

	/**
	 * Clears the key sequence. The key sequence is null after calling this 
	 * function.
	 * @see isNull()
	 */
//US	void clear();

	/**
	 * Copies the given qt key sequence over this key sequence.
	 * @param keySeq the qt key sequence to copy
	 * @return true if successful, false otherwise
	 */
//US	bool init( const QKeySequence& keySeq );

	/**
	 * Initializes the key sequence to only contain the given key.
	 * @param key the key to set
	 * @return true if successful, false otherwise
	 */
//US	bool init( const KKey& key );

	/**
	 * Initializes the key sequence to only contain the given key.
	 * @param key the key to set
	 * @return true if successful, false otherwise
	 */
//US	bool init( const KKeyNative& key );

	/**
	 * Copies the given key sequence over this key sequence.
	 * @param keySeq the key sequence to copy
	 * @return true if successful, false otherwise
	 */
//US	bool init( const KKeySequence& keySeq );

	/**
	 * Initializes this key sequence to contain the given key sequence.
	 * The description consists of comma-separated keys as
	 * required by @ref KKey::KKey(const QString&).
	 * @param key the description of the key
	 * @return true if successful, false otherwise
	 * @see KKeyServer::Sym::init()
	 * @see KKey::KKey(const QString&)
	 */
//US	bool init( const QString& key );

	/**
	 * Copy the given key sequence into this sequence.
	 */
//US	KKeySequence& operator =( const KKeySequence& seq )
//US		{ init( seq ); return *this; }

	/**
	 * Returns the number of key strokes of this sequence.
	 * @return the number of key strokes
	 * @see MAX_KEYS
	 */
//US	uint count() const;

	/**
	 * Return the @p i'th key of this sequence, or a null key if there
	 * are less then i keys.
	 * @param i the key to retrieve
	 * @return the @p i'th key, or @ref KKey::null() if there are less
	 *         than i keys
	 * @see MAX_KEYS
	 */
//US	const KKey& key( uint i ) const;

	/**
	 * @internal
	 */
//US	bool isTriggerOnRelease() const;

	/**
	 * Sets the @p i'th key of the sequence. You can not introduce gaps
	 * in a sequence, so you must use an @p i <= @ref count(). Also note that
	 * the maximum length of a key sequence is @ref MAX_KEYS.
	 * @param i the position of the new key (<= @ref count(), <= @ref MAX_KEYS)
	 * @param key the key to set
	 * @return true if successful, false otherwise
	 */
//US	bool setKey( uint i, const KKey& key );

	/**
	 * @internal
	 */
//US	void setTriggerOnRelease( bool );

	/**
	 * Returns true if the key sequence is null (after @ref clear() or empty
	 * constructor).
	 * @return true if the key sequence is null
	 * @see clear()
	 * @see null()
	 */
//US  bool isNull() const;

	/**
	 * Returns true if this key sequence begins with the given sequence.
	 * @param keySeq the key sequence to search
	 * @return true if this key sequence begins with the given sequence
	 */
//US	bool startsWith( const KKeySequence& keySeq ) const;

	/**
	 * Compares this object with the given key sequence. Returns a negative 
	 * number if the given KKeySequence is larger, 0 if they are equal and
	 * a positive number this KKeySequence is larger. Key sequences are
	 * compared by comparing the individual keys, starting from the beginning
	 * until an unequal key has been found. If a sequence contains more
	 * keys, it is considered larger.
	 * @param keySeq the key sequence to compare to
	 * @return a negative number if the given KKeySequence is larger, 0 if 
	 * they are equal and a positive number this KKeySequence is larger
	 * @see KKey::sequence
	 */
//US	int compare( const KKeySequence& keySeq ) const;

	/**
	 * Compares the keys of both sequences.
	 * @see compare()
	 */
//US	bool operator == ( const KKeySequence& seq ) const
//US		{ return compare( seq ) == 0; }

	/**
	 * Compares the keys of both sequences.
	 * @see compare()
	 */
//US	bool operator != ( const KKeySequence& seq ) const
//US		{ return compare( seq ) != 0; }

	/**
	 * Compares the keys of both sequences.
	 * @see compare()
	 */
//US	bool operator < ( const KKeySequence& seq ) const
//US		{ return compare( seq ) < 0; }
	// TODO: consider adding Qt::SequenceMatch matches(...) methods for QKeySequence equivalence

	/**
	 * Converts this key sequence to a QKeySequence.
	 * @return the QKeySequence
	 */
//US	QKeySequence qt() const;
	
	/**
	 * Returns the qt key code of the first key.
	 * @return the qt key code of the first key
	 * @see Qt::Key
	 * @see KKey::keyCodeQt()
	 */
//US	int keyCodeQt() const;

	/**
	 * Returns the key sequence as a number of key presses as
	 * returned by @ref KKey::toString(), seperated by commas.
	 * @return the string represenation of this key sequence
	 * @see KKey::toString()
	 */
//US	QString toString() const;

	/**
	 * @internal
	 */
//US  QString toStringInternal() const;

	/**
	 * Returns a null key sequence.
	 * @return the null key sequence
	 * @see isNull()
	 * @see clear()
	 */
//US	static KKeySequence& null();

//US protected:
//US	uchar m_nKeys;
//US	uchar m_bTriggerOnRelease;
	// BCI: m_rgvar should be renamed to m_rgkey for KDE 4.0
//US	KKey m_rgvar[MAX_KEYS];

//US private:
//US	class KKeySequencePrivate* d;
//US	friend class KKeyNative;
//US};

/**
* The KShortcut class is used to represent a keyboard shortcut to an action.
* A shortcut is normally a single key with modifiers, such as Ctrl+V.
* A KShortcut object may also contain an alternate key which will also
* activate the action it's associated to, as long as no other actions have
* defined that key as their primary key.  Ex: Ctrl+V;Shift+Insert.
*/

class KShortcut
{
 public:
        /**
	 * The maximum number of key sequences that can be contained in 
	 * a KShortcut.
         */ 
	enum { MAX_SEQUENCES = 2 };

	/**
	 * Creates a new null shortcut.
	 * @see null()
	 * @see isNull()
	 * @see clear()
	 */
	KShortcut() {}

	/**
	 * Creates a new shortcut with the given Qt key code
	 * as the only key sequence.
	 * @param keyQt the qt keycode
	 * @see Qt::Key
	 */
	KShortcut( int keyQt ) {}

	/**
	 * Creates a new shortcut that contains only the given qt key
	 * sequence.
	 * @param keySeq the qt key sequence to add
	 */
//US	KShortcut( const QKeySequence& keySeq ) {}

	/**
	 * Creates a new shortcut that contains only the given key
	 * in its only sequence.
	 * @param key the key to add
	 */
//US	KShortcut( const KKey& key );

	/**
	 * Creates a new shortcut that contains only the given key
	 * sequence.
	 * @param keySeq the key sequence to add
	 */
//US	KShortcut( const KKeySequence& keySeq );

	/**
	 * Copies the given shortcut.
	 * @param shortcut the shortcut to add
	 */
//US	KShortcut( const KShortcut& shortcut );

	/**
	 * Creates a new key sequence that contains the given key sequence.
	 * The description consists of semicolon-separated keys as
	 * used in @ref KKeySequence::KKeySequence(const QString&).
	 * @param shortcut the description of the key
	 * @see KKeySequence::KKeySequence(const QString&)
	 */
	KShortcut( const char* shortcut ) {}

	/**
	 * Creates a new key sequence that contains the given key sequence.
	 * The description consists of semicolon-separated keys as
	 * used in @ref KKeySequence::KKeySequence(const QString&).
	 * @param shortcut the description of the key
	 * @see KKeySequence::KKeySequence(const QString&)
	 */
	KShortcut( const QString& shortcut ) {}
	~KShortcut() {}

	/**
	 * Clears the shortcut. The shortcut is null after calling this 
	 * function.
	 * @see isNull()
	 */
//US	void clear();

	/**
	 * Initializes the shortcut with the given Qt key code
	 * as the only key sequence.
	 * @param keyQt the qt keycode
	 * @see Qt::Key
	 */
//US	bool init( int keyQt );

	/**
	 * Initializes the shortcut with the given qt key sequence.
	 * @param keySeq the qt key sequence to add
	 */
//US	bool init( const QKeySequence& keySeq );

	/**
	 * Initializes the shortcut with the given key as its only sequence.
	 * @param key the key to add
	 */
//US	bool init( const KKey& key );

	/**
	 * Initializes the shortcut with the given qt key sequence.
	 * @param keySeq the qt key sequence to add
	 */
//US	bool init( const KKeySequence& keySeq );

	/**
	 * Copies the given shortcut.
	 * @param shortcut the shortcut to add
	 */
//US	bool init( const KShortcut& shortcut );

	/**
	 * Initializes the key sequence with the given key sequence.
	 * The description consists of semicolon-separated keys as
	 * used in @ref KKeySequence::KKeySequence(const QString&).
	 * @param shortcut the description of the key
	 * @see KKeySequence::KKeySequence(const QString&)
	 */
//US	bool init( const QString& shortcut );

	/**
	 * Copies the given shortcut over this shortcut.
	 */
//US	KShortcut& operator =( const KShortcut& cut )
//US		{ init( cut ); return *this; }

	/**
	 * Returns the number of sequences that are in this
	 * shortcut.
	 * @return the number of sequences
	 * @ref MAX_SEQUENCES
	 */
//US	uint count() const;

	/**
	 * Returns the @p i'th key sequence of this shortcut.
	 * @param i the number of the key sequence to retrieve
	 * @return the @p i'th sequence or @ref KKeySequence::null() if
	 *         there are less than @p i key sequences
	 * @ref MAX_SEQUENCES
	 */
//US	const KKeySequence& seq( uint i ) const;

	/**
	 * Returns the key code of the first key sequence, or 
	 * null if there is no first key sequence.
	 * @return the key code of the first sequence's first key
	 * @see Qt::Key
	 * @see KKeySequence::keyCodeQt()
	 */
//US	int keyCodeQt() const;

	/**
	 * Returns true if the shortcut is null (after @ref clear() or empty
	 * constructor).
	 * @return true if the shortcut is null
	 * @see clear()
	 * @see null()
	 */
	bool isNull() const { return true; }

	/**
	 * Compares this object with the given shortcut. Returns a negative 
	 * number if the given shortcut is larger, 0 if they are equal and
	 * a positive number this shortcut is larger. Shortcuts are
	 * compared by comparing the individual key sequences, starting from the 
	 * beginning until an unequal key sequences has been found. If a shortcut
	 * contains more key sequences, it is considered larger.
	 * @param shortcut the shortcut to compare to
	 * @return a negative number if the given KShortcut is larger, 0 if 
	 * they are equal and a positive number this KShortcut is larger
	 * @see KKey::compare()
	 * @see KKeyShortcut::compare()
	 */
//US	int compare( const KShortcut& shortcut ) const;

	/**
	 * Compares the sequences of both shortcuts.
	 * @see compare()
	 */
//US	bool operator == ( const KShortcut& cut ) const
//US		{ return compare( cut ) == 0; }

	/**
	 * Compares the sequences of both shortcuts.
	 * @see compare()
	 */
//US	bool operator != ( const KShortcut& cut ) const
//US		{ return compare( cut ) != 0; }

	/**
	 * Compares the sequences of both shortcuts.
	 * @see compare()
	 */
//US	bool operator < ( const KShortcut& cut ) const
//US		{ return compare( cut ) < 0; }

	/**
	 * Checks whether this shortcut contains a sequence that starts
	 * with the given key.
	 * @param key the key to check
	 * @return true if a key sequence starts with the key
	 */
//US	bool contains( const KKey& key ) const;

	/**
	 * Checks whether this shortcut contains a sequence that starts
	 * with the given key.
	 * @param key the key to check
	 * @return true if a key sequence starts with the key
	 */
//US	bool contains( const KKeyNative& key ) const;

	/**
	 * Checks whether this shortcut contains the given sequence.
	 * @param keySeq the key sequence to check
	 * @return true if the shortcut has the given key sequence
	 */
//US	bool contains( const KKeySequence& keySeq ) const;

	/**
	 * Sets the @p i'th key sequence of the shortcut. You can not introduce 
	 * gaps in the list of sequences, so you must use an @i <= @ref count(). 
	 * Also note that the maximum number of key sequences is @ref MAX_SEQUENCES.
	 * @param i the position of the new key sequence(<= @ref count(), 
	 *          <= @ref MAX_SEQUENCES)
	 * @param keySeq the key sequence to set
	 * @return true if successful, false otherwise
	 */
//US	bool setSeq( uint i, const KKeySequence& keySeq );

	/**
	 * Appends the given key sequence.
	 * @param keySeq the key sequence to add
	 * @return true if successful, false otherwise
	 * @see setSeq()
	 * @see MAX_SEQUENCES
	*/
//US	bool append( const KKeySequence& keySeq );

	/**
	 * Appends the given key
	 * @param spec the key to add
	 * @return true if successful, false otherwise
	 * @see setSeq()
	 * @see MAX_SEQUENCES
	 * @since 3.2
	*/
//US	bool append( const KKey& spec );

	/**
	 * Appends the sequences from the given shortcut.
	 * @param cut the shortcut to append
	 * @return true if successful, false otherwise
	 * @see MAX_SEQUENCES
	 * @since 3.2
	*/
//US	bool append( const KShortcut& cut );

	/**
	 * Converts this shortcut to a key sequence. The first key sequence
	 * will be taken.
	 */
//US	operator QKeySequence () const;

	/**
	 * Returns a description of the shortcut as semicolon-separated
	 * ket sequences, as returned by @ref KKeySequence::toString().
	 * @return the string represenation of this shortcut
	 * @see KKey::toString()
	 * @see KKeySequence::toString()
	 */
//US	QString toString() const;

	/**
	 * @internal
	 */
  QString toStringInternal( const KShortcut* pcutDefault = 0 ) const
  {
    return "EMPTY IMPL.";
  }

	/**
	 * Returns a null shortcut.
	 * @return the null shortcut
	 * @see isNull()
	 * @see clear()
	 */
//US	static KShortcut& null();

//US protected:
//US	uint m_nSeqs;
//US	KKeySequence m_rgseq[MAX_SEQUENCES];

//US private:
//US	class KShortcutPrivate* d;
//US	friend class KKeyNative;

//US#ifndef KDE_NO_COMPAT
//US public:
//US	operator int () const    { return keyCodeQt(); }
//US#endif
};

#endif // __KSHORTCUT_H
