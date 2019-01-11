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
 * This file is originaly based on version 1.1 of pwmanager
 * and was modified to run on embedded devices that run microkde
 *
 * $Id: pwmdoc.h,v 1.19 2008/04/17 13:37:46 zautrix Exp $
 **************************************************************************/  

#ifndef __PWMDOC_H
#define __PWMDOC_H
#ifdef _WIN32_
#define ssize_t unsigned int
#endif
#define PWM_FILE_VER		(static_cast<char>(0x05))

#define PWM_HASH_SHA1		(static_cast<char>(0x01))
#define PWM_HASH_SHA256		(static_cast<char>(0x02))
#define PWM_HASH_SHA384		(static_cast<char>(0x03))
#define PWM_HASH_SHA512		(static_cast<char>(0x04))
#define PWM_HASH_MD5		(static_cast<char>(0x05))
#define PWM_HASH_RMD160		(static_cast<char>(0x06))
#define PWM_HASH_TIGER		(static_cast<char>(0x07))

#define PWM_CRYPT_BLOWFISH	(static_cast<char>(0x01))
#define PWM_CRYPT_AES128	(static_cast<char>(0x02))
#define PWM_CRYPT_AES192	(static_cast<char>(0x03))
#define PWM_CRYPT_AES256	(static_cast<char>(0x04))
#define PWM_CRYPT_3DES		(static_cast<char>(0x05))
#define PWM_CRYPT_TWOFISH	(static_cast<char>(0x06))
#define PWM_CRYPT_TWOFISH128	(static_cast<char>(0x07))

#define PWM_COMPRESS_NONE	(static_cast<char>(0x00))
#define PWM_COMPRESS_GZIP	(static_cast<char>(0x01))
#define PWM_COMPRESS_BZIP2	(static_cast<char>(0x02))

#define DEFAULT_MAX_ENTRIES	(~(static_cast<unsigned int>(0)))
#define FILE_ID_HEADER  	"PWM_PASSWORD_FILE"


#include "pwmexception.h"
#include "pwmdocui.h"

#include <qobject.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <kprocess.h>

#ifndef PWM_EMBEDDED
#include "configuration.h"
#else
#include <kapplication.h>
#include <ksyncmanager.h>
#endif

#include <string>
#include <vector>
#include <utility>

using std::vector;
using std::string;
using std::pair;

/* used in findEntry() function */
#define SEARCH_IN_DESC		(1)
#define SEARCH_IN_NAME		(1 << 1)
#define SEARCH_IN_PW		(1 << 2)
#define SEARCH_IN_COMMENT	(1 << 3)
#define SEARCH_IN_URL		(1 << 4)
#define SEARCH_IN_LAUNCHER	(1 << 5)
#define SEARCH_IN_ALL		(SEARCH_IN_DESC	| SEARCH_IN_NAME	|  \
				 SEARCH_IN_PW	| SEARCH_IN_COMMENT	|  \
				 SEARCH_IN_URL	| SEARCH_IN_LAUNCHER)

/** document deeplocked. Data is out for lunch to disk */
#define DOC_STAT_DEEPLOCKED		(1)
/** encrypted document on disk is dirty. data has to go to disk. */
#define DOC_STAT_DISK_DIRTY		(1 << 1)
/** we are using a chipcard to encrypt the data */
#define DOC_STAT_USE_CHIPCARD		(1 << 2)
/** use "currentPw" to unlock. (This flag is set/unset by a timer) */
#define DOC_STAT_UNLOCK_WITHOUT_PW	(1 << 3)

class PwMDoc;
class PwMView;
class QFile;

/* meta data for a PwMDataItem */
struct PwMMetaData
{
	PwMMetaData()
	 : updateInt (0)
		{ }
	/** creation date of the PwMDataItem to which
	  * this meta data belongs.
	  */
	QDateTime	create;
	/** becomes valid on this date */
	QDateTime	valid;
	/** expire date */
	QDateTime	expire;
	/** update date (last updated at this date) */
	QDateTime	update;
	/** update interval (in minutes). Time since the
	  * last update to remind the user to update the item.
	  * 0 disables.
	  */
	unsigned long updateInt;
 
	//US ENH: enhancements of the filestructure
	/* each entry gets a unique id assigned  */
	string  uniqueid;


	void clear()
		{
			create = QDateTime();
			expire = QDateTime();
			update = QDateTime();
			updateInt = 0;
			uniqueid = KApplication::randomString(8).latin1();
		}

	inline bool isValid() const
		{
			if (valid.isNull())
				return true;
			return (valid < QDateTime::currentDateTime());
		}
	inline bool isExpired() const
		{
			if (expire.isNull())
				return false;
			return (expire < QDateTime::currentDateTime());
		}
	inline bool isUpdateIntOver() const
		{
			if (updateInt == 0 ||
			    update.isNull())
				return false;
			QDateTime d(update);
			return (d.addSecs(updateInt * 60) < QDateTime::currentDateTime());
		}
};

struct PwMDataItem
{
	PwMDataItem()
	 : lockStat (true)
	 , listViewPos (-1)
	 , binary (false)
	 , rev (0)
		{ }

	/** password description */
	string	desc;
	/** user-name */
	string	name;
	/** the password itself */
	string	pw;
	/** some comment */
	string	comment;
	/** an URL string */
	string	url;
	/** launcher. Can be executed as a system() command */
	string	launcher;
	/** locking status. If locked (true), pw is not emitted through getEntry() */
	bool	lockStat;
	/** position of this item in main "list-view"
	  * If -1, the position is not yet specified and should be appended to the list
	  */
	int	listViewPos;
	/** does this entry contain binary data? */
	bool binary;
	/** meta data for this data item. */
	PwMMetaData meta;
	/** data revision counter. This counter can be used
	  * to easily, efficiently determine if this data item
	  * has changed since some time.
	  * This counter is incremented on every update.
	  */
	unsigned int rev;

	void clear(bool clearMeta = true)
	{
		/* NOTE: Don't use .clear() here to be
		 *       backward compatible with gcc-2 (Debian Woody)
		 */
		desc = "";
		name = "";
		pw = "";
		comment = "";
		url = "";
		launcher = "";
		lockStat = true;
		listViewPos = -1;
		binary = false;
		if (clearMeta)
			meta.clear();
	}
  //US ENH: we need this operator to compare two items if we have no unique ids
  //available. Generaly this happens before the first sync

	bool operator==( const PwMDataItem &a ) const
	{
	  //qDebug("oper==%s", a.desc.c_str());
	  if ( desc != a.desc ) return false;
	  if ( name != a.name ) return false;
	  if ( pw != a.pw ) return false;
	  if ( comment != a.comment ) return false;
	  if ( url != a.url ) return false;
	  if ( launcher != a.launcher ) return false;
	  //all other field will not be checked.
	  return true;
	}

  //US ENH: this sync method actually copies all values from the parameter like the =operator
  //does with two exceptions: listViewPos will not be changed, and the launcher only if required.
  bool syncItem(const PwMDataItem &a, bool syncLauncher=true )
  {
    desc = a.desc;
    name = a.name;
    pw = a.pw;
    comment = a.comment;
    url = a.url;
    if (syncLauncher == true)
      launcher = a.launcher;
    meta = a.meta;
    binary = a.binary;
    lockStat = a.lockStat;
    rev = a.rev;

    return true;
  }

};

struct PwMCategoryItem
{
	/** all PwMDataItems (all passwords) within this category */
	vector<PwMDataItem>	d;
	/** category name/description */
	string			name;

	//US ENH: enhancements of the filestructure
	/* each category stores the text for description,name and password */
	string  desc_text;
	string  name_text;
	string  pw_text;
 
	void clear()
	{
		d.clear();
		name = "";
		desc_text = "Description";
		name_text = "Username";
		pw_text = "Password";
	}
};

struct PwMSyncItem
{
  string syncName;
  QDateTime lastSyncDate;
  
  void clear()
	{
		lastSyncDate = QDateTime();
		syncName = "";
	}
};

struct PwMItem
{
  vector<PwMCategoryItem> dta;
  vector<PwMSyncItem> syncDta;

  void clear()
	{
		dta.clear();
		syncDta.clear(); 
	}
};


/** "Function Object" for sort()ing PwMDataItem::listViewPos */
class dta_lvp_greater
{
public:
	bool operator() (const pair<unsigned int, unsigned int> &d1,
			 const pair<unsigned int, unsigned int> &d2)
	{
		return d1.second > d2.second;
	}
};

/** list of PwMDoc documents and it's IDs */
class PwMDocList
{
public:
	struct listItem
	{
		/** document filename (known as ID, here) */
		string docId;
		/** pointer to the document class */
		PwMDoc *doc;
	};

	PwMDocList() {}

	/** add a new item to the list */
	void add(PwMDoc *doc, const string &id);
	/** changes the contents of an existing item */
	void edit(PwMDoc *doc, const string &newId);
	/** remove the given item */
	void del(PwMDoc *doc);
	/** get the item at index */
	listItem getAt(int index)
			{ return docList[index]; }
	/** find an entry with this id */
	bool find(const string &id, listItem *ret = 0);
	/** returns a copy of the list */
	const vector<listItem>* getList() const
			{ return &docList; }


	/** returns a new unique number to extend the name of
	  * an unnamed document.
	  */
	static unsigned int getNewUnnamedNumber()
			{ return unnamedDocCnt++; }

protected:
	/* Hm, I think we shouldn't really use a "list" here, should we?
	 * So I decided to actually use a vector.
	 */
	vector<listItem> docList;
	/** This value is used to get a new number for yet unnamed
	  * documents. It is incremented on every request. So it's
	  * theoretically possible to overflow it, but... :)
	  */
	static unsigned int unnamedDocCnt;
};

/** implements timers for the document */
class DocTimer : public QObject
{
	Q_OBJECT
public:
	enum TimerIDs
	{
		id_mpwTimer,
		id_autoLockTimer,
		id_metaCheckTimer
	};

public:
	DocTimer(PwMDoc *_doc);
	~DocTimer();

	/** start the timer */
	void start(TimerIDs timer);
	/** stop the timer */
	void stop(TimerIDs timer);
	/** get the lock for a timer.
	  * This lock is a recursive lock. When a lock is
	  * held, the timer will be stopped and timeout is
	  * guaranteed to not happen
	  */
	void getLock(TimerIDs timer);
	/** put a recursive timer lock */
	void putLock(TimerIDs timer);

protected slots:
	/** timeout slot for the mpw timer */
	void mpwTimeout();
	/** timeout slot for the autoLock timer */
	void autoLockTimeout();
	/** timeout slot for the metaCheck timer */
	void metaCheckTimeout();

protected:
	/** pointer to the document associated with this timer. */
	PwMDoc *doc;
	/** timer object for mpw timer */
	QTimer *mpwTimer;
	/** timer object for the autoLock timer */
	QTimer *autoLockTimer;
	/** timer object for the metaCheck timer */
	QTimer *metaCheckTimer;
	/** lock counter for the mpw timer */
	unsigned int mpwLock;
	/** lock counter for the autoLock timer */
	unsigned int autoLockLock;
	/** lock counter for the metaCheck timer */
	unsigned int metaCheckLock;
};

/** Document class for PwM */
//US ENH: derived from KSyncInterfaces, to get called by PwM when a sync is required.
// But PwMDoc is handling the sync by itself.
class PwMDoc : public PwMDocUi, public KSyncInterface

{
	Q_OBJECT
	friend class DocTimer;

public:
	/** construtor */
	PwMDoc(QObject* parent = 0, const char *name = 0);
	/** destructor */
	~PwMDoc();

	/** returns a pointer to a list of all open documents */
	static PwMDocList* getOpenDocList()
			{ return &openDocList; }

	/** flag document dirty. dta changed */
	void flagDirty()
		{
			setDocStatFlag(DOC_STAT_DISK_DIRTY);
			emitDataChanged(this);
		}
	/** modified? */
	bool isDirty()
			{ return getDocStatFlag(DOC_STAT_DISK_DIRTY); }
	/** save document to disk */
	PwMerror saveDoc(char compress, const QString *file = 0);
	/** read document from file.
	  * "openLocked is must be set to either of these values:
	  *  0 == open with all entries unlocked
	  *  1 == open with all entries locked
	  *  2 == open deep-locked
	  */
	PwMerror openDoc(const QString *file, int openLocked);
	/** export document to ascii-textfile */
	PwMerror exportToText(const QString *file);
	/** export document to gpasman / kpasman file */
	PwMerror exportToGpasman(const QString *file);
	/** import document from ascii-textfile */
	PwMerror importFromText(const QString *file, int format = -1);
	/** import document from gpasman / kpasman file */
	PwMerror importFromGpasman(const QString *file);
	/** add new entry */
	PwMerror addEntry(const QString &category, PwMDataItem *d,
			  bool dontFlagDirty = false, bool updateMeta = true);
	/** add new category. This function doesn't flag the document dirty! */
	PwMerror addCategory(const QString &category, unsigned int *categoryIndex,
			     bool checkIfExist = true);
	/** rename an existing category */
	bool renameCategory(const QString &category, const QString &newName);
	/** rename an existing category */
	bool renameCategory(unsigned int category, const QString &newName,
			    bool dontFlagDirty = false);
	/** delete an existing category */
	bool delCategory(const QString &category);
	/** delete an existing category */
	bool delCategory(unsigned int category, bool dontFlagDirty = false);
	/** returns a list of all category-names */
	void getCategoryList(vector<string> *list);
	/** returns a list of all category-names */
	void getCategoryList(QStringList *list);
	/** returns a list of all entry-descs in the given category */
	void getEntryList(const QString &category, QStringList *list);
	/** returns a list of all entry-descs in the given category */
	void getEntryList(const QString &category, vector<string> *list);
	/** returns a list of all entry-descs in the given category */
	void getEntryList(unsigned int category, vector<string> *list);
	/** returns a list of all entry-descs in the given category */
	void getEntryList(unsigned int category, QStringList *list);
	/** delete entry */
	bool delEntry(const QString &category, unsigned int index, bool dontFlagDirty = false);
	/** delete entry */
	bool delEntry(unsigned int category, unsigned int index, bool dontFlagDirty = false);
	/** edit entry */
	bool editEntry(const QString &oldCategory, const QString &newCategory,
		       unsigned int index, PwMDataItem *d, bool updateMeta = true);
	/** edit entry */
	bool editEntry(unsigned int oldCategory, const QString &newCategory,
		       unsigned int index, PwMDataItem *d, bool updateMeta = true);
	/** finds the category with the "name" and return it's index */
	bool findCategory(const QString &name, unsigned int *index);
	/** search for an entry "find" and check while searching only for
	  * the data-fields specified by "searchIn". To set the "searchIn"
	  * value, we may use one or more of the SEARCH_IN_* defines at
	  * the top of this header-file. It returns the positions of all
	  * matched entries in "foundPositions". If "breakAfterFound" is true,
	  * the function terminates after the first occurence of the entry
	  * and doesn't go on searching. So foundPositions->size() is never
	  * > 1 if breakAfterFound is true.
	  */
	void findEntry(unsigned int category, PwMDataItem find, unsigned int searchIn,
		       vector<unsigned int> *foundPositions, bool breakAfterFound = false,
		       bool caseSensitive = true, bool exactWordMatch = true,
		       bool sortByLvp = false);
	/** see the above funtion. This function allows to set the category by name. */
	void findEntry(const QString &category, PwMDataItem find, unsigned int searchIn,
		       vector<unsigned int> *foundPositions, bool breakAfterFound = false,
		       bool caseSensitive = true, bool exactWordMatch = true,
		       bool sortByLvp = false);
	/** returns number of entries */
	unsigned int numEntries(const QString &category);
	unsigned int numEntries(unsigned int category)
			{ return dti.dta[category].d.size(); }
	/** returns number of categories */
	unsigned int numCategories()
			{ return dti.dta.size(); }
	/** returns the name of the category at "index" */
	const string* getCategory(unsigned int index)
			{ return (&(dti.dta[index].name)); }

	/** returns the data of item at "index".
	  * It unlocks the entry if it's locked and unlockIfLocked is true.
	  * If the entry is locked, but unlockIfLocked is false, it'll not return
	  * the pw.
	  */
	bool getEntry(const QString &category, unsigned int index,
		      PwMDataItem *d, bool unlockIfLocked = false);
	bool getEntry(unsigned int category, unsigned int index,
		      PwMDataItem *d, bool unlockIfLocked = false);
	/** returns the comment-string by looking at the category
	  * and the listViewPos
	  */
	PwMerror getCommentByLvp(const QString &category, int listViewPos,
				 string *foundComment);
	PwMerror getCommentByLvp_long(const QString &category, int listViewPos,
				 string *foundComment);
	/** checks if a password is already available. (currentPw) */
	bool isPwAvailable()
			{ return (currentPw != ""); }
	/** un/lock entry at "index". If needed, ask for password. */
	bool lockAt(const QString &category, unsigned int index,
		    bool lock = true);
	bool lockAt(unsigned int category, unsigned int index,
		    bool lock = true);
	/** returns the lock-status at "index" */
	bool isLocked(const QString &category, unsigned int index);
	bool isLocked(unsigned int category, unsigned int index)
			{ return dti.dta[category].d[index].lockStat; }
	/** returns the deeplock status */
	bool isDeepLocked()
			{ return getDocStatFlag(DOC_STAT_DEEPLOCKED); }
	/** (un)lock all entries */
	bool lockAll(bool lock);
	/** unlocks all entries tempoarly.
	  * 1st NOTE: Be very careful with this function! :)
	  * 2nd NOTE: After you have called unlockAll_Tempoary(); ,
	  *           please DON'T forget to call unlockAll_Tempoary(true);
	  *           _before_ the user (or someone else) is able to change
	  *           the document!
	  * 3rd NOTE: Please DON'T change "dta" while the data is tempoary
	  *           unlocked! This will cause corruption.
	  */
	bool unlockAll_tempoary(bool revert = false);
	/** deep-(un)locks the document.
	  * deep-locking writes all data to the file, deletes all data
	  * in memory, but doesn't close the document.
	  * deep-locking is only available, if the user previously saved
	  * the doc to a file (with a password).
	  * If "saveToFile" is false, it does NOT write the data to the file!
	  */
	PwMerror deepLock(bool lock = true, bool saveToFile = true);
	/** is unlockable without pw? */
	bool unlockWoPw()
			{ return getDocStatFlag(DOC_STAT_UNLOCK_WITHOUT_PW); }
	/** get the "currentPassword" */
	const QString& getCurrentPw()
			{ return currentPw; }
	/** open a window and request the user to change the mpw */
	void changeCurrentPw();
	/** set the "listViewPos" variable of "dta" */
	void setListViewPos(const QString &category, unsigned int index,
			    int pos);
	/** set the "listViewPos" variable of "dta" */
	void setListViewPos(unsigned int category, unsigned int index,
			    int pos);
	/** get the "listViewPos" variable of "dta" */
	int getListViewPos(const QString &category, unsigned int index);
	/** set the maximum number of entries allowed */
	void setMaxNumEntries(unsigned int num = DEFAULT_MAX_ENTRIES)
			{ maxEntries = num; }
	/** get the maximum number of entries allowed */
	unsigned int getMaxNumEntries()
			{ return maxEntries; }
	/** ensure all listViewPos of all dta items are set. (are ! -1).
	  * If there are some undefined entries, add them to the end of
	  * the listViewPos(itions). */
	void ensureLvp();
	/** execute the "launcher" of this entry */
	bool execLauncher(const QString &category, unsigned int entryIndex);
	/** see above */
	bool execLauncher(unsigned int category, unsigned int entryIndex);
	/** open a browser with the URL-section of the given entry */
	bool goToURL(const QString &category, unsigned int entryIndex);
	/** see above */
	bool goToURL(unsigned int category, unsigned int entryIndex);
	/** returns true if there is no entry present in the document.
	  * Note: The "default" Category is present everytime, so
	  *       it's checked for it's entries.
	  */
	bool isDocEmpty()
		{
			if (numCategories() > 1)
				return false;
			if (numEntries(0))
				return false;
			return true;
		}
	/** returns the filename of this doc */
	const QString& getFilename()
			{ return filename; }
	/** returns the title of the doc */
	QString getTitle();
	/** sets the list-view-pointer hold in the doc */
	void setListViewPointer(PwMView *_listView)
			{ listView = _listView; }
	/** returns the list-view-pointer */
	PwMView * getListViewPointer()
			{ return listView; }
	/** try to delete the doc. The user may be asked to save
	  * the data. The user may cancel the whole operation.
	  * false is returned, then.
	  */
	bool tryDelete();
	/** is the doc deleted? (with tryDelete() ) */
	bool isDeleted()
			{ return deleted; }
	/** returns the document timer object */
	DocTimer * timer()
			{ return _timer; }
	/** get a lock on the dataChanged signal.
	  * If someone is holding a lock, the signal is not emitted.
	  */
	void getDataChangedLock()
			{ ++dataChangedLock; }
	/** put the dataChanged lock */
	void putDataChangedLock()
			{ --dataChangedLock; }
	/** returns the revision count of the item at cat/index */
	unsigned int getEntryRevCnt(unsigned int category, unsigned int index)
			{ return dti.dta[category].d[index].rev; }
	/** returns a const pointer to the entries meta */
	const PwMMetaData * getEntryMeta(unsigned int category, unsigned int index)
			{ return &(dti.dta[category].d[index].meta); }
	/** is the entry at "category" "index" a binary entry? */
	bool isBinEntry(unsigned int category, unsigned int index)
			{ return dti.dta[category].d[index].binary; }

public slots:
	/** wrapper for PwMTray */
	void _deepUnlock();

signals:
	/** the data of the document has changed and must be updated
	  * in all views.
	  * NOTE: use emitDataChanged(PwMDoc *document) to emit this signal!
	  */
	void dataChanged(PwMDoc *document);
	/** the document class is going to close. This signal may be
	  * used to nofify all views, that the user closed the document,
	  * so the views can go down, too.
	  */
	void docClosed(PwMDoc *document);
	/** somebody just opened the document */
	void docOpened(PwMDoc *document);
	/** this document object just got created */
	void docCreated(PwMDoc *document);

public:
	/** emit the dataChanged signal after checking for a lock */
	void emitDataChanged(PwMDoc *document)
		{
			if (!dataChangedLock)
				emit dataChanged(document);
		}

protected:
	/** current file for this doc */
	QString filename;
//US ENH: we need a place where we keep the syncentries. So I invented
// struct PwMItem, that has a vector of PwMCategoryItem and vector of PwMSyncItem 
	/** holds all data */
	PwMItem dti;
	/** maximum number of entries */
	unsigned int maxEntries;
	/** currently used password to encrypt data */
	QString currentPw;
	/** current global document status flags */
	unsigned int curDocStat;
	/** browser process for goToURL() */
	KProcess browserProc;
	/** pointer to the list-view, using this document.
	  * As there can only be one list-view per doc, we
	  * don't need a list here.
	  */
	PwMView *listView;
	/** unnamedNum is used to store the "unnamed counter"
	  * for this document, while it's unnamed. If it's 0,
	  * we have to get a new unique one.
	  */
	unsigned int unnamedNum;
	/** is this doc going to be deleted (executing in destructor context) */
	bool deleted;
	/** document timer */
	DocTimer *_timer;
	/** lock counter for the "dataChanged" signal */
	unsigned int dataChangedLock;

	/** list of all open documents */
	static PwMDocList openDocList;

protected:
	/** serialize "dta" and return it in "d". */
	bool serializeDta(string *d);
	/** de-serialize "d" and overwrite "dta" */
	bool deSerializeDta(const string *d, bool entriesLocked);
	/** write header to file */
	PwMerror writeFileHeader(char keyHash, char dataHash, char crypt, char compress,
				 QString *pw, QFile *f);
	/** write data-hash to file */
	PwMerror writeDataHash(char dataHash, string *d, QFile *f);
	/** check header. Read header info and verify key-hash and filever.
	  * returns length of header in "headerLength" */
	PwMerror checkHeader(char *cryptAlgo, QString *pw, char *compress,
			     unsigned int *headerLength, char *dataHashType,
			     string *dataHash, QFile *f);
	/** check the data-hash */
	PwMerror checkDataHash(char dataHashType, const string *dataHash, const string *dataStream);
	/** encrypt data "d" and write to "filename" */
	PwMerror encrypt(string *d, const QString *pw, QFile *f, char algo,
				 char _hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
);
	/** read data from file beginning at "pos", decrypt and return it */
	PwMerror decrypt(string *d, unsigned int pos, const QString *pw, char algo, 
		 char _hashalgo, //US BUG: pass _hashalgo because we need it in hashPassphrase
QFile *f);
	/** compress the data */
	bool compressDta(string *d, char algo);
	/** uncompress the data */
	bool decompressDta(string *d, char algo);
	/** internal import function for a text-file generated by PwM.
	  * If this is not a valid PwM-exported file, it returns e_fileFormat */
	PwMerror importText_PwM(const QString *file);
	/** PwM-text-import helper function to extract the name/pw/comment out
	  * of one entry-line */
	bool textExtractEntry_PwM(const char *in, ssize_t in_size, string *out);
	/** compare two strings */
	bool compareString(const string &s1, const string &s2, bool caseSensitive,
			   bool exactWordMatch);
	/** clears all document-data */
	void clearDoc();
	/** delete all empty categories */
	void delAllEmptyCat(bool dontFlagDirty);
	/** set a document status flag */
	void setDocStatFlag(unsigned int statFlag)
			{ curDocStat |= statFlag; }
	/** unset a document status flag */
	void unsetDocStatFlag(unsigned int statFlag)
			{ curDocStat &= ~statFlag; }
	/** get a document status flag */
	bool getDocStatFlag(unsigned int statFlag) const
			{ return (curDocStat & statFlag); }
	/** set the "currentPassword" */
	void setCurrentPw(const QString &pw)
		{
			currentPw = pw;
			setDocStatFlag(DOC_STAT_DISK_DIRTY);
		}
	/** make a backup-copy of the given file */
	bool backupFile(const QString &filePath);
	/** copy a file from src to dst */
	bool copyFile(const QString &src, const QString &dst);


 public:
#ifdef PWM_EMBEDDED
	//US ENH: this is the magic function that syncronizes the local doc with the remote doc.
	PwMerror syncronize(KSyncManager* manager, PwMDoc* syncLocal, PwMDoc* syncRemote, int mode );

	//takePwMDataItem returns the following values
	// 0 equal
	// 1 take local
	// 2 take remote
	// 3 cancel 
	int takePwMDataItem( PwMDataItem* local,  PwMDataItem* remote, QDateTime lastSync, int mode , bool full );

	//the following methods are the overwritten callbackmethods from the syncinterface
	virtual bool sync(KSyncManager* manager, QString filename, int mode, QString resource);
    virtual void removeSyncInfo( QString syncProfile);

#endif
	//US ENH: helpermethods to return a whole category entry
	/** returns a pointer to the categoryitem */
	PwMCategoryItem* getCategoryEntry(unsigned int index)
			{ return &(dti.dta[index]); }

 private:
	//US ENH: helpermethods to access the sync data for a certain syncname.
	// It returns the syncdatas index
	bool findSyncData(const QString &syncname, unsigned int *index);

	/** add new syncdataentry */
	PwMerror addSyncDataEntry(PwMSyncItem *d, bool dontFlagDirty = false);

	/** returns a pointer to the syncdata */
	PwMSyncItem* getSyncDataEntry(unsigned int index)
			{ return &(dti.syncDta[index]); }

	/** delete entry */
	bool delSyncDataEntry(unsigned int index, bool dontFlagDirty = false);

	/** returns number of categories */
	unsigned int numSyncDataEntries()
			{ return dti.syncDta.size(); }

	PwMDataItem* findEntryByID(const QString &uid, unsigned int *category, unsigned int *index);

	QStringList getIDEntryList(); 

};

#endif
