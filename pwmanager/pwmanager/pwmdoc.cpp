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
 * $Id: pwmdoc.cpp,v 1.32 2005/11/28 05:53:55 zautrix Exp $
 **************************************************************************/  

#include "pwmdoc.h"
#include "pwmview.h"
#include "blowfish.h"
#include "sha1.h"
#include "globalstuff.h"
#include "gpasmanfile.h"
#include "serializer.h"
#include "compressgzip.h"
//US#include "compressbzip2.h"
#include "randomizer.h"
#include "pwminit.h"
#include "libgcryptif.h"
#ifdef PWM_EMBEDDED
#include "pwmprefs.h"
#include "kglobal.h"
#endif

#include <kmessagebox.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <libkcal/syncdefines.h>


#ifdef CONFIG_KWALLETIF
# include "kwalletemu.h"
#endif // CONFIG_KWALLETIF

#include <qdatetime.h>
#include <qsize.h>
#include <qfileinfo.h>
#include <qfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//US#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32_
#include <unistd.h>
#include <stdint.h>
#endif

#ifdef PWM_EMBEDDED
#ifndef Q_LONG
#define Q_LONG long
#endif

#ifndef Q_ULONG
#define Q_ULONG unsigned long
#endif
#endif //PWM_EMBEDDED


//TODO: reset to its normal value.
//LR set to 5 min 
#define META_CHECK_TIMER_INTERVAL	300 /* 10 300*/ /* sek */

using namespace std;


void PwMDocList::add(PwMDoc *doc, const string &id)
{
#ifdef PWM_DEBUG
	// check for existance of object in debug mode only.
	vector<listItem>::iterator begin = docList.begin(),
				   end = docList.end(),
				   i = begin;
	while (i != end) {
		if (i->doc == doc) {
			BUG();
			return;
		}
		++i;
	}
#endif
	listItem newItem;
	newItem.doc = doc;
	newItem.docId = id;
	docList.push_back(newItem);
}

void PwMDocList::edit(PwMDoc *doc, const string &newId)
{
	vector<listItem>::iterator begin = docList.begin(),
				   end = docList.end(),
				   i = begin;
	while (i != end) {
		if (i->doc == doc) {
			i->docId = newId;
			return;
		}
		++i;
	}
}

void PwMDocList::del(PwMDoc *doc)
{
	vector<listItem>::iterator begin = docList.begin(),
				   end = docList.end(),
				   i = begin;
	while (i != end) {
		if (i->doc == doc) {
			docList.erase(i);
			return;
		}
		++i;
	}
}

bool PwMDocList::find(const string &id, listItem *ret)
{
	vector<listItem>::iterator begin = docList.begin(),
				   end = docList.end(),
				   i = begin;
	while (i != end) {
		if (i->docId == id) {
			if (ret)
				*ret = *i;
			return true;
		}
		++i;
	}
	return false;
}



DocTimer::DocTimer(PwMDoc *_doc)
 : doc (_doc)
 , mpwLock (0)
 , autoLockLock (0)
 , metaCheckLock (0)
{
	mpwTimer = new QTimer;
	autoLockTimer = new QTimer;
	metaCheckTimer = new QTimer;
	connect(mpwTimer, SIGNAL(timeout()),
		this, SLOT(mpwTimeout()));
	connect(autoLockTimer, SIGNAL(timeout()),
		this, SLOT(autoLockTimeout()));
	connect(metaCheckTimer, SIGNAL(timeout()),
		this, SLOT(metaCheckTimeout()));
}

DocTimer::~DocTimer()
{
	delete mpwTimer;
	delete autoLockTimer;
	delete metaCheckTimer;
}

void DocTimer::start(TimerIDs timer)
{
	switch (timer) {
		case id_mpwTimer:
			if (mpwTimer->isActive())
				mpwTimer->stop();
			doc->setDocStatFlag(DOC_STAT_UNLOCK_WITHOUT_PW);
			mpwTimer->start(conf()->confGlobPwTimeout() * 1000, true);
			break;
		case id_autoLockTimer:
			if (autoLockTimer->isActive())
				autoLockTimer->stop();
			if (conf()->confGlobLockTimeout() > 0)
				autoLockTimer->start(conf()->confGlobLockTimeout() * 1000, true);
			break;
		case id_metaCheckTimer:
			if (metaCheckTimer->isActive())
				metaCheckTimer->stop();
			metaCheckTimer->start(META_CHECK_TIMER_INTERVAL * 1000, true);
			break;
	}
}

void DocTimer::stop(TimerIDs timer)
{
	switch (timer) {
		case id_mpwTimer:
			mpwTimer->stop();
			break;
		case id_autoLockTimer:
			autoLockTimer->stop();
			break;
		case id_metaCheckTimer:
			metaCheckTimer->stop();
			break;
	}
}

void DocTimer::getLock(TimerIDs timer)
{
	switch (timer) {
		case id_mpwTimer:
			++mpwLock;
			break;
		case id_autoLockTimer:
			++autoLockLock;
			break;
		case id_metaCheckTimer:
			++metaCheckLock;
			break;
	}
}

void DocTimer::putLock(TimerIDs timer)
{
	switch (timer) {
		case id_mpwTimer:
			if (mpwLock)
				--mpwLock;
			break;
		case id_autoLockTimer:
			if (autoLockLock)
				--autoLockLock;
			break;
		case id_metaCheckTimer:
			if (metaCheckLock)
				--metaCheckLock;
			break;
	}
}

void DocTimer::mpwTimeout()
{
	if (mpwLock) {
		mpwTimer->start(1000, true);
		return;
	}
	doc->unsetDocStatFlag(DOC_STAT_UNLOCK_WITHOUT_PW);
}

void DocTimer::autoLockTimeout()
{
	if (autoLockLock) {
		autoLockTimer->start(1000, true);
		return;
	}
	if (conf()->confGlobAutoDeepLock() &&
	    doc->filename != QString::null &&
	    doc->filename != "") {
		doc->deepLock(true);
	} else {
		doc->lockAll(true);
	}
}

void DocTimer::metaCheckTimeout()
{
	if (metaCheckLock) {
		// check again in one second.
		metaCheckTimer->start(1000, true);
		return;
	}
	if (doc->isDeepLocked()) {
		metaCheckTimer->start(META_CHECK_TIMER_INTERVAL * 1000, true);
		return;
	}
	if (doc->isDocEmpty()) {
		metaCheckTimer->start(META_CHECK_TIMER_INTERVAL * 1000, true);
		return;
	}
#ifdef CONFIG_KWALLETIF
	KWalletEmu *kwlEmu = doc->init->kwalletEmu();
	if (kwlEmu)
		kwlEmu->suspendDocSignals();
#endif // CONFIG_KWALLETIF
	/* We simply trigger all views to update their
	 * displayed values. This way they have a chance
	 * to get notified when some meta changes over time.
	 * (for example an entry expired).
	 * The _view_ is responsive for not updating its
	 * contents if nothing really changed!
	 */
	emit doc->dataChanged(doc);
#ifdef CONFIG_KWALLETIF
	if (kwlEmu)
		kwlEmu->resumeDocSignals();
#endif // CONFIG_KWALLETIF
	metaCheckTimer->start(META_CHECK_TIMER_INTERVAL * 1000, true);
}



PwMDocList PwMDoc::openDocList;
unsigned int PwMDocList::unnamedDocCnt = 1;

PwMDoc::PwMDoc(QObject *parent, const char *name)
 : PwMDocUi(parent, name)
 , dataChangedLock (0)
{
	deleted = false;
	unnamedNum = 0;
	getOpenDocList()->add(this, getTitle().latin1());
	curDocStat = 0;
	setMaxNumEntries();
	_timer = new DocTimer(this);
	timer()->start(DocTimer::id_mpwTimer);
	timer()->start(DocTimer::id_autoLockTimer);
	timer()->start(DocTimer::id_metaCheckTimer);
	addCategory(DEFAULT_CATEGORY, 0, false);
	listView = 0;
	emit docCreated(this);
}

PwMDoc::~PwMDoc()
{
	emit docClosed(this);
	getOpenDocList()->del(this);
	delete _timer;
}

PwMerror PwMDoc::saveDoc(char compress, const QString *file)
{
	PwMerror ret, e;
	string serialized;
	QFile f;
	QString tmpFileMoved(QString::null);
	bool wasDeepLocked;
	QString savedFilename(filename);

	if (!file) {
		if (filename == "")
			return e_filename;
		if (isDeepLocked()) {
		  /* We don't need to save any data.
		   * It's already all on disk, because
		   * we are deeplocked.
		   */
		  unsetDocStatFlag(DOC_STAT_DISK_DIRTY);
		  ret = e_success;
          return ret;
		}
 	} else {
		if (*file == "" && filename == "")
			return e_filename;
		if (*file != "")
			filename = *file;
	}

	wasDeepLocked = isDeepLocked();
	if (wasDeepLocked) {
	  /* We are deeplocked. That means all data is already
	   * on disk. BUT we need to do saving procedure,
	   * because *file != savedFilename.
	   * Additionally we need to tempoarly restore
	   * the old "filename", because deepLock() references it.
	   */
	  QString newFilename(filename);
	  filename = savedFilename;
	  getDataChangedLock();
	  e = deepLock(false);
	  putDataChangedLock();
	  filename = newFilename;
	  switch (e) {
	     case e_success:
	       break;
	     case e_wrongPw:
	     case e_noPw:
	       emitDataChanged(this);
	       return e;
	     default:
	       emitDataChanged(this);
	       return e_openFile;
	  }
	}

	if (!isPwAvailable()) {
		/* password is not available. This means, the
		 * document wasn't saved, yet.
		 */
		bool useChipcard = getDocStatFlag(DOC_STAT_USE_CHIPCARD);
		QString pw(requestNewMpw(&useChipcard));
		if (pw != "") {
			currentPw = pw;
		} else {
			return e_noPw;
		}
		if (useChipcard) {
			setDocStatFlag(DOC_STAT_USE_CHIPCARD);
		} else {
			unsetDocStatFlag(DOC_STAT_USE_CHIPCARD);
		}
	}

	int _cryptAlgo = conf()->confGlobCryptAlgo();
	int _hashAlgo = conf()->confGlobHashAlgo();

	// sanity check for the selected algorithms
	if (_cryptAlgo < PWM_CRYPT_BLOWFISH ||
	    _cryptAlgo > PWM_CRYPT_TWOFISH128) {
		printWarn("Invalid Crypto-Algorithm selected! "
			  "Config-file seems to be corrupt. "
			  "Falling back to Blowfish.");
		_cryptAlgo = PWM_CRYPT_BLOWFISH;
	}
	if (_hashAlgo < PWM_HASH_SHA1 ||
	    _hashAlgo > PWM_HASH_TIGER) {
		printWarn("Invalid Hash-Algorithm selected! "
			  "Config-file seems to be corrupt. "
			  "Falling back to SHA1.");
		_hashAlgo = PWM_HASH_SHA1;
	}
	char cryptAlgo = static_cast<char>(_cryptAlgo);
	char hashAlgo = static_cast<char>(_hashAlgo);

	if (conf()->confGlobMakeFileBackup()) {
		if (!backupFile(filename))
			return e_fileBackup;
	}
    int mLastBackupDate = 0;
    KConfig configGlobal (locateLocal("config","pwmanagerbuprc"));
    QFileInfo fileInfo ( filename );
    mLastBackupDate = configGlobal.readNumEntry( "LastBackupDate-"+ fileInfo.fileName (), 0 );
      KConfig config (locateLocal("config","microkdeglobalrc"));
      config.setGroup( "BackupSettings" );
      bool b_enabled =   config.readBoolEntry( "BackupEnabled" );
      if ( b_enabled && QFile::exists(filename)) {
          int num = config.readNumEntry( "BackupNumbers" );
          int d_count = config.readNumEntry( "BackupDayCount" );
          bool stdDir = config.readBoolEntry( "BackupUseDefaultDir" );
          QString bupDir = config.readEntry( "BackupDatadir" );
          QDate reference ( 2000,1,1 );
          int daysTo = reference.daysTo ( QDate::currentDate() ); 
          bool saveDate = false;
          if ( daysTo - d_count >= mLastBackupDate ) {
                qDebug("KA: Last backup was %d days ago ", daysTo - mLastBackupDate );
                if ( stdDir )
                    bupDir = KGlobalSettings::backupDataDir();
                int retval =  KApplication::createBackup(  filename, bupDir, num );
                if ( retval == 0 ) {
                    qDebug("KO: Backup cancelled. Will try again tomorrow ");
                    // retval == 0 : backup skipped for today, try again tomorrow
                    mLastBackupDate = daysTo - d_count+1;
                    saveDate = true;
                } else if ( retval == 1 ){
                    qDebug("KO: Backup created.");
                    // backup ok
                    mLastBackupDate =  daysTo;
                    saveDate = true;
                } else if ( retval == 2 ){
                    qDebug("KO: Backup globally cancelled.");
                    // backup globally cancelled
                    b_enabled = false;
                }
                if ( !b_enabled ) {
                    config.writeEntry( "mBackupEnabled", false );
                }
                if ( saveDate ) {
                    configGlobal.writeEntry( "LastBackupDate-"+ fileInfo.fileName (), mLastBackupDate );
                }
          }
      }
	if (QFile::exists(filename)) {
		/* Move the existing file to some tmp file.
		 * When saving file succeeds, delete tmp file. Otherwise
		 * move tmp file back. See below.
		 */
		Randomizer *rnd = Randomizer::obj();
		char rnd_buf[5];
		sprintf(rnd_buf, "%X%X%X%X", rnd->genRndChar() & 0xFF, rnd->genRndChar() & 0xFF,
			rnd->genRndChar() & 0xFF, rnd->genRndChar() & 0xFF);
		tmpFileMoved = filename + "." + rnd_buf + ".mv";
		if (!copyFile(filename, tmpFileMoved))
			return e_openFile;
		if (!QFile::remove(filename)) {
			printWarn(string("removing orig file ")
				  + filename.latin1()
				  + " failed!");
		}
	}
	f.setName(filename);
	if (!f.open(IO_ReadWrite)) {
		ret = e_openFile;
		goto out_moveback;
	}
	e = writeFileHeader(hashAlgo, hashAlgo,
			    cryptAlgo, compress,
			    &currentPw, &f);
	if (e == e_hashNotImpl) {
		printDebug("PwMDoc::saveDoc(): writeFileHeader() failed: e_hashNotImpl");
		f.close();
		ret = e_hashNotImpl;
		goto out_moveback;
	} else if (e != e_success) {
		printDebug("PwMDoc::saveDoc(): writeFileHeader() failed");
		f.close();
		ret = e_writeHeader;
		goto out_moveback;
	}
	if (!serializeDta(&serialized)) {
		printDebug("PwMDoc::saveDoc(): serializeDta() failed");
		f.close();
		ret = e_serializeDta;
		goto out_moveback;
	}
	e = writeDataHash(hashAlgo, &serialized, &f);
	if (e == e_hashNotImpl) {
		printDebug("PwMDoc::saveDoc(): writeDataHash() failed: e_hashNotImpl");
		f.close();
		ret = e_hashNotImpl;
		goto out_moveback;
	} else if (e != e_success) {
		printDebug("PwMDoc::saveDoc(): writeDataHash() failed");
		f.close();
		ret = e_writeHeader;
		goto out_moveback;
	}
	if (!compressDta(&serialized, compress)) {
		printDebug("PwMDoc::saveDoc(): compressDta() failed");
		f.close();
		ret = e_enc;
		goto out_moveback;
	}
	e = encrypt(&serialized, &currentPw, &f, cryptAlgo, hashAlgo);
	if (e == e_weakPw) {
		printDebug("PwMDoc::saveDoc(): encrypt() failed: e_weakPw");
		f.close();
		ret = e_weakPw;
		goto out_moveback;
	} else if (e == e_cryptNotImpl) {
		printDebug("PwMDoc::saveDoc(): encrypt() failed: e_cryptNotImpl");
		f.close();
		ret = e_cryptNotImpl;
		goto out_moveback;
	} else if (e != e_success) {
		printDebug("PwMDoc::saveDoc(): encrypt() failed");
		f.close();
		ret = e_enc;
		goto out_moveback;
	}
	unsetDocStatFlag(DOC_STAT_DISK_DIRTY);
	f.close();
#ifndef _WIN32_
	if (chmod(filename.latin1(),
		  conf()->confGlobFilePermissions())) {
		printWarn(string("chmod failed: ") + strerror(errno));
	}
#endif
	openDocList.edit(this, getTitle().latin1());
	if (wasDeepLocked) {
	  /* Do _not_ save the data with the deepLock()
	   * call, because this will recurse
	   * into saveDoc()
	   */
	  deepLock(true, false);
	  /* We don't check return value here, because
	   * it won't fail. See NOTE in deepLock()
	   */
	}	
	if (tmpFileMoved != QString::null) {
		// now remove the moved file.
		if (!QFile::remove(tmpFileMoved)) {
			printWarn(string("removing file ")
				  + tmpFileMoved.latin1()
				  + " failed!");
		}
	}
	ret = e_success;
	printDebug(string("writing file { name: ")
		   + filename.latin1() + "   compress: "
		   + tostr(static_cast<int>(compress)) + "   cryptAlgo: "
		   + tostr(static_cast<int>(cryptAlgo)) + "   hashAlgo: "
		   + tostr(static_cast<int>(hashAlgo))
		   + " }");
	goto out;
out_moveback:
	if (tmpFileMoved != QString::null) {
		if (copyFile(tmpFileMoved, filename)) {
			if (!QFile::remove(tmpFileMoved)) {
				printWarn(string("removing tmp file ")
					  + filename.latin1()
					  + " failed!");
			}
		} else {
			printWarn(string("couldn't copy file ")
				  + tmpFileMoved.latin1()
				  + " back to "
				  + filename.latin1());
		}
	}
out:
	return ret;
}

PwMerror PwMDoc::openDoc(const QString *file, int openLocked)
{
	PWM_ASSERT(file);
	PWM_ASSERT(openLocked == 0 || openLocked == 1 || openLocked == 2);
	string decrypted, dataHash;
	PwMerror ret;
	char cryptAlgo, dataHashType, compress;
	unsigned int headerLen;

	if (*file == "")
		return e_readFile;
	filename = *file;
	/* check if this file is already open.
	 * This does not catch symlinks!
	 */
	if (!isDeepLocked()) {
		if (getOpenDocList()->find(filename.latin1()))
			return e_alreadyOpen;
	}
	QFile f(filename);

	if (openLocked == 2) {
		// open deep-locked
		if (!QFile::exists(filename))
			return e_openFile;
		if (deepLock(true, false) != e_success)
			return e_openFile;
		goto out_success;
	}

	if (!f.open(IO_ReadOnly))
		return e_openFile;

	ret = checkHeader(&cryptAlgo, &currentPw, &compress, &headerLen,
			  &dataHashType, &dataHash, &f);
	if (ret != e_success) {
		printDebug("PwMDoc::openDoc(): checkHeader() failed");
		f.close();
		if (ret == e_wrongPw) {
			wrongMpwMsgBox(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
			return ret;
		} else if (ret == e_noPw ||
			   ret == e_fileVer ||
			   ret == e_fileFormat ||
			   ret == e_hashNotImpl) {
			return ret;
		} else
			return e_readFile;
	}
	ret = decrypt(&decrypted, headerLen, &currentPw, cryptAlgo, dataHashType, &f);
	if (ret == e_cryptNotImpl) {
		printDebug("PwMDoc::openDoc(): decrypt() failed: e_cryptNotImpl");
		f.close();
		return e_cryptNotImpl;
	} else if (ret != e_success) {
		printDebug("PwMDoc::openDoc(): decrypt() failed");
		f.close();
		return e_readFile;
	}
	if (!decompressDta(&decrypted, compress)) {
		printDebug("PwMDoc::openDoc(): decompressDta() failed");
		f.close();
		return e_fileCorrupt;
	}
	ret = checkDataHash(dataHashType, &dataHash, &decrypted);
	if (ret == e_hashNotImpl) {
		printDebug("PwMDoc::openDoc(): checkDataHash() failed: e_hashNotImpl");
		f.close();
		return e_hashNotImpl;
	} else if (ret != e_success) {
		printDebug("PwMDoc::openDoc(): checkDataHash() failed");
		f.close();
		return e_fileCorrupt;
	}
	if (!deSerializeDta(&decrypted, openLocked == 1)) {
		printDebug("PwMDoc::openDoc(): deSerializeDta() failed");
		f.close();
		return e_readFile;
	}
	f.close();
	timer()->start(DocTimer::id_mpwTimer);
	timer()->start(DocTimer::id_autoLockTimer);
out_success:
	openDocList.edit(this, getTitle().latin1());
	emit docOpened(this);
	return e_success;
}

PwMerror PwMDoc::writeFileHeader(char keyHash, char dataHash, char crypt, char compress,
				 QString *pw, QFile *f)
{
	PWM_ASSERT(pw);
	PWM_ASSERT(f);
	//US ENH: or maybe a bug: checking here for listView does not make sense because we do not check anywhere else
	//Wenn I sync, I open a doc without a view => listView is 0 => Assertion 
	//US	PWM_ASSERT(listView);
	if (f->writeBlock(FILE_ID_HEADER, strlen(FILE_ID_HEADER)) !=
	    static_cast<Q_LONG>(strlen(FILE_ID_HEADER))) {
		return e_writeFile;
	}
	if (f->putch(PWM_FILE_VER) == -1 ||
	    f->putch(keyHash) == -1 ||
	    f->putch(dataHash) == -1 ||
	    f->putch(crypt) == -1 ||
	    f->putch(compress) == -1 ||
	    f->putch((getDocStatFlag(DOC_STAT_USE_CHIPCARD)) ?
		     (static_cast<char>(0x01)) : (static_cast<char>(0x00))) == -1) {
		return e_writeFile;
	}

	// write bytes of NUL-data. These bytes are reserved for future-use.
	const int bufSize = 64;
	char tmp_buf[bufSize];
	memset(tmp_buf, 0x00, bufSize);
	if (f->writeBlock(tmp_buf, bufSize) != bufSize)
		return e_writeFile;

	switch (keyHash) {
	case PWM_HASH_SHA1: {
		const int hashlen = SHA1_HASH_LEN_BYTE;
		Sha1 hash;
		hash.sha1_write(reinterpret_cast<const byte *>(pw->latin1()), pw->length());
		string ret = hash.sha1_read();
		if (f->writeBlock(ret.c_str(), hashlen) != hashlen)
			return e_writeFile;
		break;
	} 
	case PWM_HASH_SHA256:
		/*... fall through */
	case PWM_HASH_SHA384:
	case PWM_HASH_SHA512:
	case PWM_HASH_MD5:
	case PWM_HASH_RMD160:
	case PWM_HASH_TIGER:
	    {
		if (!LibGCryptIf::available())
			return e_hashNotImpl;
		LibGCryptIf gc;
		PwMerror err;
		unsigned char *buf;
		size_t hashLen;
		err = gc.hash(&buf,
			      &hashLen,
			      reinterpret_cast<const unsigned char *>(pw->latin1()),
			      pw->length(),
			      keyHash);
		if (err != e_success)
			return e_hashNotImpl;
		if (f->writeBlock(reinterpret_cast<const char *>(buf), hashLen)
		    != static_cast<Q_LONG>(hashLen)) {
			delete [] buf;
			return e_hashNotImpl;
		}
		delete [] buf;
		break;
	} 
	default: {
		return e_hashNotImpl;
	} }
	return e_success;
}

PwMerror PwMDoc::checkHeader(char *cryptAlgo, QString *pw, char *compress,
			     unsigned int *headerLength, char *dataHashType,
			     string *dataHash, QFile *f)
{
	PWM_ASSERT(cryptAlgo);
	PWM_ASSERT(pw);
	PWM_ASSERT(headerLength);
	PWM_ASSERT(dataHashType);
	PWM_ASSERT(dataHash);
	PWM_ASSERT(f);
	int tmpRet;
	// check "magic" header
	const char magicHdr[] = FILE_ID_HEADER;
	const int hdrLen = array_size(magicHdr) - 1;
	char tmp[hdrLen];
	if (f->readBlock(tmp, hdrLen) != hdrLen)
		return e_readFile;
	if (memcmp(tmp, magicHdr, hdrLen) != 0)
		return e_fileFormat;
	// read and check file ver
	int fileV = f->getch();
	if (fileV == -1)
		return e_fileFormat;
	if (fileV != PWM_FILE_VER)
		return e_fileVer;
	// read hash hash type
	int keyHash = f->getch();
	if (keyHash == -1)
		return e_fileFormat;
	// read data hash type
	tmpRet = f->getch();
	if (tmpRet == -1)
		return e_fileFormat;
	*dataHashType = tmpRet;
	// read crypt algo
	tmpRet = f->getch();
	if (tmpRet == -1)
		return e_fileFormat;
	*cryptAlgo = tmpRet;
	// get compression-algo
	tmpRet = f->getch();
	if (tmpRet == -1)
		return e_fileFormat;
	*compress = tmpRet;
	// get the MPW-flag
	int mpw_flag = f->getch();
	if (mpw_flag == -1)
		return e_fileFormat;
	if (mpw_flag == 0x01)
		setDocStatFlag(DOC_STAT_USE_CHIPCARD);
	else
		unsetDocStatFlag(DOC_STAT_USE_CHIPCARD);
	// skip the "RESERVED"-bytes
	if (!(f->at(f->at() + 64)))
		return e_fileFormat;

	*pw = requestMpw(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
	if (*pw == "") {
		/* the user didn't give a master-password
		 * or didn't insert a chipcard
		 */
		return e_noPw;
	}
	// verify key-hash
	switch (keyHash) {
	case PWM_HASH_SHA1: {
		// read hash from header
		const int hashLen = SHA1_HASH_LEN_BYTE;
		string readHash;
		int i;
		for (i = 0; i < hashLen; ++i)
			readHash.push_back(f->getch());
		Sha1 hash;
		hash.sha1_write(reinterpret_cast<const byte *>(pw->latin1()), pw->length());
		string ret = hash.sha1_read();
		if (ret != readHash)
			return e_wrongPw;	// hash doesn't match (wrong key)
		break;
	}
	case PWM_HASH_SHA256:
		/*... fall through */
	case PWM_HASH_SHA384:
	case PWM_HASH_SHA512:
	case PWM_HASH_MD5:
	case PWM_HASH_RMD160:
	case PWM_HASH_TIGER: {
		if (!LibGCryptIf::available())
			return e_hashNotImpl;
		LibGCryptIf gc;
		PwMerror err;
		unsigned char *buf;
		size_t hashLen;
		err = gc.hash(&buf,
			      &hashLen,
			      reinterpret_cast<const unsigned char *>(pw->latin1()),
			      pw->length(),
			      keyHash);
		if (err != e_success)
			return e_hashNotImpl;
		string calcHash(reinterpret_cast<const char *>(buf),
				static_cast<string::size_type>(hashLen));
		delete [] buf;
		// read hash from header
		string readHash;
		size_t i;
		for (i = 0; i < hashLen; ++i)
			readHash.push_back(f->getch());
		if (calcHash != readHash)
			return e_wrongPw;	// hash doesn't match (wrong key)
		break;
	} 
	default: {
		return e_hashNotImpl;
	} }
	// read the data-hash from the file
	unsigned int hashLen, i;
	switch (*dataHashType) {
	case PWM_HASH_SHA1:
		hashLen = SHA1_HASH_LEN_BYTE;
		break;
	case PWM_HASH_SHA256:
		/*... fall through */
	case PWM_HASH_SHA384:
	case PWM_HASH_SHA512:
	case PWM_HASH_MD5:
	case PWM_HASH_RMD160:
	case PWM_HASH_TIGER: {
		if (!LibGCryptIf::available())
			return e_hashNotImpl;
		LibGCryptIf gc;
		hashLen = gc.hashLength(*dataHashType);
		if (hashLen == 0)
			return e_hashNotImpl;
		break;
	} 
	default:
		return e_hashNotImpl;
	}
	*dataHash = "";
	for (i = 0; i < hashLen; ++i) {
		tmpRet = f->getch();
		if (tmpRet == -1)
			return e_fileFormat;
		dataHash->push_back(static_cast<char>(tmpRet));
	}
	*headerLength = f->at();
#ifndef PWM_EMBEDDED
	printDebug(string("opening file { compress: ")
		   + tostr(static_cast<int>(*compress)) + "   cryptAlgo: "
		   + tostr(static_cast<int>(*cryptAlgo)) + "   keyHashAlgo: "
		   + tostr(static_cast<int>(keyHash))
		   + " }");
#else
	printDebug(string("opening file { compress: ")
		   + tostr((int)(*compress)) + "   cryptAlgo: "
		   + tostr((int)(*cryptAlgo)) + "   keyHashAlgo: "
		   + tostr((int)(keyHash))
		   + " }");
#endif

	return e_success;
}

PwMerror PwMDoc::writeDataHash(char dataHash, string *d, QFile *f)
{
	PWM_ASSERT(d);
	PWM_ASSERT(f);

	switch (dataHash) {
	case PWM_HASH_SHA1: {
		const int hashLen = SHA1_HASH_LEN_BYTE;
		Sha1 h;
		h.sha1_write(reinterpret_cast<const byte *>(d->c_str()), d->size());
		string hRet = h.sha1_read();
		if (f->writeBlock(hRet.c_str(), hashLen) != hashLen)
		    	return e_writeFile;
		break;
	}
	case PWM_HASH_SHA256:
		/*... fall through */
	case PWM_HASH_SHA384:
	case PWM_HASH_SHA512:
	case PWM_HASH_MD5:
	case PWM_HASH_RMD160:
	case PWM_HASH_TIGER: {
		if (!LibGCryptIf::available())
			return e_hashNotImpl;
		LibGCryptIf gc;
		PwMerror err;
		unsigned char *buf;
		size_t hashLen;
		err = gc.hash(&buf,
			      &hashLen,
			      reinterpret_cast<const unsigned char *>(d->c_str()),
			      d->size(),
			      dataHash);
		if (err != e_success)
			return e_hashNotImpl;
		if (f->writeBlock(reinterpret_cast<const char *>(buf), hashLen)
		    != static_cast<Q_LONG>(hashLen)) {
			delete [] buf;
			return e_hashNotImpl;
		}
		delete [] buf;
		break;
	} 
	default: {
		return e_hashNotImpl;
	} }

	return e_success;
}

bool PwMDoc::backupFile(const QString &filePath)
{
	QFileInfo fi(filePath);
	if (!fi.exists())
		return true; // Yes, true is correct.
	QString pathOnly(fi.dirPath(true));
	QString nameOnly(fi.fileName());
	QString backupPath = pathOnly
			   + "/~"
			   + nameOnly
			   + ".backup";
	return copyFile(filePath, backupPath);
}

bool PwMDoc::copyFile(const QString &src, const QString &dst)
{
	QFileInfo fi(src);
	if (!fi.exists())
		return false;
	if (QFile::exists(dst)) {
		if (!QFile::remove(dst))
			return false;
	}
	QFile srcFd(src);
	if (!srcFd.open(IO_ReadOnly))
		return false;
	QFile dstFd(dst);
	if (!dstFd.open(IO_ReadWrite)) {
		srcFd.close();
		return false;
	}
	const int tmpBuf_size = 512;
	char tmpBuf[tmpBuf_size];
	Q_LONG bytesRead, bytesWritten;

	while (!srcFd.atEnd()) {
		bytesRead = srcFd.readBlock(tmpBuf,
				  static_cast<Q_ULONG>(tmpBuf_size));
		if (bytesRead == -1) {
			srcFd.close();
			dstFd.close();
			return false;
		}
		bytesWritten = dstFd.writeBlock(tmpBuf,
				     static_cast<Q_ULONG>(bytesRead));
		if (bytesWritten != bytesRead) {
			srcFd.close();
			dstFd.close();
			return false;
		}
	}
	srcFd.close();
	dstFd.close();
	return true;
}

PwMerror PwMDoc::addEntry(const QString &category, PwMDataItem *d,
			  bool dontFlagDirty, bool updateMeta)
{
	PWM_ASSERT(d);
	unsigned int cat = 0;

	if (isDeepLocked()) {
		PwMerror ret;
		ret = deepLock(false);
		if (ret != e_success)
			return e_lock;
	}

	addCategory(category, &cat);

	if (numEntries(category) >= maxEntries)
		return e_maxAllowedEntr;

	vector<unsigned int> foundPositions;
	/* historically this was:
	 *	const int searchIn = SEARCH_IN_DESC | SEARCH_IN_NAME |
	 *			     SEARCH_IN_URL | SEARCH_IN_LAUNCHER;
	 * But for now we only search in desc.
	 * That's a tweak to be KWallet compatible. But it should not add
	 * usability-drop onto PwManager, does it?
	 * (And yes, "int" was a bug. Correct is "unsigned int")
	 */
	const unsigned int searchIn = SEARCH_IN_DESC;
	findEntry(cat, *d, searchIn, &foundPositions, true);
	if (foundPositions.size()) {
		// DOH! We found this entry.
		return e_entryExists;
	}

	d->listViewPos = -1;
	d->lockStat = conf()->confGlobNewEntrLockStat();
	if (updateMeta) {
		d->meta.create = QDateTime::currentDateTime();
		d->meta.update = d->meta.create;
	}
	dti.dta[cat].d.push_back(*d);

	delAllEmptyCat(true);

	if (!dontFlagDirty)
		flagDirty();
	return e_success;
}

PwMerror PwMDoc::addCategory(const QString &category, unsigned int *categoryIndex,
			     bool checkIfExist)
{
	if (isDeepLocked()) {
		PwMerror ret;
		ret = deepLock(false);
		if (ret != e_success)
			return e_lock;
	}
	if (checkIfExist) {
		if (findCategory(category, categoryIndex))
			return e_categoryExists;
	}
	PwMCategoryItem item;
	//US ENH: clear item to initialize with default values, or create a constructor
	item.clear();

	item.name = category.latin1();
	dti.dta.push_back(item);
	if (categoryIndex)
		*categoryIndex = dti.dta.size() - 1;
	return e_success;
}

bool PwMDoc::delEntry(const QString &category, unsigned int index, bool dontFlagDirty)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		BUG();
		return false;
	}

	return delEntry(cat, index, dontFlagDirty);
}

bool PwMDoc::delEntry(unsigned int category, unsigned int index, bool dontFlagDirty)
{
	if (isDeepLocked())
		return false;
	if (index > dti.dta[category].d.size() - 1)
		return false;
	getDataChangedLock();
	if (!lockAt(category, index, false)) {
		putDataChangedLock();
		return false;
	}
	putDataChangedLock();
	int lvPos = dti.dta[category].d[index].listViewPos;

	// delete entry
	dti.dta[category].d.erase(dti.dta[category].d.begin() + index);

	unsigned int i, entries = numEntries(category);
	if (!entries) {
		// no more entries in this category, so
		// we can delete it, too.
		BUG_ON(!delCategory(category));
		// delCategory() flags it dirty, so we need not to do so.
		return true;
	}
	for (i = 0; i < entries; ++i) {
		// decrement all listViewPositions that are greater than the deleted.
		if (dti.dta[category].d[i].listViewPos > lvPos)
			--dti.dta[category].d[i].listViewPos;
	}

	if (!dontFlagDirty)
		flagDirty();
	return true;
}

bool PwMDoc::editEntry(const QString &oldCategory, const QString &newCategory,
		       unsigned int index, PwMDataItem *d, bool updateMeta)
{
	PWM_ASSERT(d);
	unsigned int oldCat = 0;

	if (!findCategory(oldCategory, &oldCat)) {
		BUG();
		return false;
	}

	return editEntry(oldCat, newCategory, index, d, updateMeta);
}

bool PwMDoc::editEntry(unsigned int oldCategory, const QString &newCategory,
		       unsigned int index, PwMDataItem *d, bool updateMeta)
{
	if (isDeepLocked())
		return false;
	if (updateMeta) {
		d->meta.update = QDateTime::currentDateTime();
		if (d->meta.create.isNull()) {
			d->meta.create = d->meta.update;
		}
	}
	if (dti.dta[oldCategory].name != newCategory.latin1()) {
		// the user changed the category.
		PwMerror ret;
		d->rev = 0;
		ret = addEntry(newCategory, d, true, false);
		if (ret != e_success)
			return false;
		if (!delEntry(oldCategory, index, true))
			return false;
	} else {
		d->rev = dti.dta[oldCategory].d[index].rev + 1; // increment revision counter.
		dti.dta[oldCategory].d[index] = *d;
	}
	flagDirty();
	return true;
}

unsigned int PwMDoc::numEntries(const QString &category)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		BUG();
		return 0;
	}

	return numEntries(cat);
}

bool PwMDoc::serializeDta(string *d)
{
	PWM_ASSERT(d);
	Serializer ser;
	if (!ser.serialize(dti))
		return false;
	d->assign(ser.getXml());
	if (!d->size())
		return false;
	return true;
}

bool PwMDoc::deSerializeDta(const string *d, bool entriesLocked)
{
	PWM_ASSERT(d);
#ifndef PWM_EMBEDDED
	try {

		Serializer ser(d->c_str());
		ser.setDefaultLockStat(entriesLocked);
		if (!ser.deSerialize(&dti))
			return false;
	} catch (PwMException) {
		return false;
	}
#else
	Serializer ser(d->c_str());
	ser.setDefaultLockStat(entriesLocked);
	if (!ser.deSerialize(&dti))
	  return false;
#endif

	emitDataChanged(this);
	return true;
}

bool PwMDoc::getEntry(const QString &category, unsigned int index,
		      PwMDataItem * d, bool unlockIfLocked)
{
	PWM_ASSERT(d);
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		BUG();
		return false;
	}

	return getEntry(cat, index, d, unlockIfLocked);
}

bool PwMDoc::getEntry(unsigned int category, unsigned int index,
		      PwMDataItem *d, bool unlockIfLocked)
{
	if (index > dti.dta[category].d.size() - 1)
		return false;

	bool locked = isLocked(category, index);
	if (locked) {
		/* this entry is locked. We don't return a password,
		 * until it's unlocked by the user by inserting
		 * chipcard or entering the mpw
		 */
		if (unlockIfLocked) {
			if (!lockAt(category, index, false)) {
				return false;
			}
			locked = false;
		}
	}

	*d = dti.dta[category].d[index];
	if (locked)
		d->pw = LOCKED_STRING.latin1();

	return true;
}
PwMerror PwMDoc::getCommentByLvp(const QString &category, int listViewPos,
				 string *foundComment)
{
	PWM_ASSERT(foundComment);
	unsigned int cat = 0;

	if (!findCategory(category, &cat))
		return e_invalidArg;

	unsigned int i, entries = numEntries(cat);
	for (i = 0; i < entries; ++i) {
		if (dti.dta[cat].d[i].listViewPos == listViewPos) {
			*foundComment = dti.dta[cat].d[i].comment;
			if (dti.dta[cat].d[i].binary)
				return e_binEntry;
			return e_normalEntry;
		}
	}
	BUG();
	return e_generic;
}

PwMerror PwMDoc::getCommentByLvp_long(const QString &category, int listViewPos,
				 string *foundComment)
{
	PWM_ASSERT(foundComment);
	unsigned int cat = 0;

	if (!findCategory(category, &cat))
		return e_invalidArg;

	unsigned int i, entries = numEntries(cat);
	for (i = 0; i < entries; ++i) {
		if (dti.dta[cat].d[i].listViewPos == listViewPos) {
			if (dti.dta[cat].d[i].binary)
				return e_binEntry;
            PwMCategoryItem* catItem = getCategoryEntry(cat);
            QString retval;
            QString tempval = QString (dti.dta[cat].d[i].desc.c_str());
            if ( !tempval.isEmpty() ) {
                retval += "<b>" +QString ( catItem->desc_text.c_str()  )+ ":</b> "+ tempval+"<br>" ;
            }
            tempval = QString (dti.dta[cat].d[i].name.c_str());
            if ( !tempval.isEmpty() ) {
                retval += "<b>" +QString ( catItem->name_text.c_str()  ) + ":</b> "+ tempval+"<br>"  ;
            }
            tempval = QString (dti.dta[cat].d[i].pw.c_str());
            if ( !tempval.isEmpty() ) {
                if ( dti.dta[cat].d[i].lockStat )
                    retval += "<b>" +QString ( catItem->pw_text.c_str()  )+ ": " + i18n("LOCKED") +"</b><br>"  ;
                else
                    retval += "<b>" +QString ( catItem->pw_text.c_str()  )+ ":</b> " + tempval+"<br>"  ;
            }
            tempval = QString (dti.dta[cat].d[i].url.c_str());
            if ( !tempval.isEmpty() ) {
                retval +=  "<b>" +i18n("URL:")+ "</b> " + tempval+"<br>"  ;
            }
            tempval = QString (dti.dta[cat].d[i].launcher.c_str());
            if ( !tempval.isEmpty() ) {
                retval +=  "<b>" +i18n("Launcher:")+ "</b> " + tempval+"<br>"  ;
            }
            tempval = QString (dti.dta[cat].d[i].comment.c_str());
            if ( !tempval.isEmpty() ) {
                tempval.replace(QRegExp ( "\n" ), "<br>" );
                retval +=  "<b>" +i18n("Comment:")+ "</b><br>" + tempval+"<br>"  ;
            }

        string ret ( retval.latin1()  );


        // *foundComment = dti.dta[cat].d[i].comment;
			*foundComment = ret;
			return e_normalEntry;
		}
	}
	BUG();
	return e_generic;
}

bool PwMDoc::compressDta(string *d, char algo)
{
	PWM_ASSERT(d);
	switch (algo) {
		case PWM_COMPRESS_GZIP: {
			CompressGzip comp;
			return comp.compress(d);
		}
#ifndef PWM_EMBEDDED 
	        case PWM_COMPRESS_BZIP2: {
			CompressBzip2 comp;
			return comp.compress(d);
		}
#endif
		case PWM_COMPRESS_NONE: {
			return true;
		} default: {
			BUG();
		}
	}
	return false;
}

bool PwMDoc::decompressDta(string *d, char algo)
{
	PWM_ASSERT(d);
	switch (algo) {
		case PWM_COMPRESS_GZIP: {
			CompressGzip comp;
			return comp.decompress(d);
		}
#ifndef PWM_EMBEDDED 
		case PWM_COMPRESS_BZIP2: {
			CompressBzip2 comp;
			return comp.decompress(d);
		}
#endif
		case PWM_COMPRESS_NONE: {
			return true;
		}
	}
	return false;
}

PwMerror PwMDoc::encrypt(string *d, const QString *pw, QFile *f, char algo,
				 char hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
)
{
	PWM_ASSERT(d);
	PWM_ASSERT(pw);
	PWM_ASSERT(f);

	size_t encSize;
	byte *encrypted = 0;

	switch (algo) {
	case PWM_CRYPT_BLOWFISH: {
		Blowfish::padNull(d);
		encSize = d->length();
		encrypted = new byte[encSize];
		Blowfish bf;
		if (bf.bf_setkey((byte *) pw->latin1(), pw->length())) {
			delete [] encrypted;
			return e_weakPw;
		}
		bf.bf_encrypt((byte *) encrypted, (byte *) d->c_str(), encSize);
		break;
	}
	case PWM_CRYPT_AES128:
		/*... fall through */
	case PWM_CRYPT_AES192:
	case PWM_CRYPT_AES256:
	case PWM_CRYPT_3DES:
	case PWM_CRYPT_TWOFISH:
	case PWM_CRYPT_TWOFISH128: {
		if (!LibGCryptIf::available())
			return e_cryptNotImpl;
		LibGCryptIf gc;
		PwMerror err;
		unsigned char *plain = new unsigned char[d->length() + 1024];
		memcpy(plain, d->c_str(), d->length());
		err = gc.encrypt(&encrypted,
				 &encSize,
				 plain,
				 d->length(),
				 reinterpret_cast<const unsigned char *>(pw->latin1()),
				 pw->length(),
				 algo,
				 hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
				 );
		delete [] plain;
		if (err != e_success)
			return e_cryptNotImpl;
		break;
	}
	default: {
		delete_ifnot_null_array(encrypted);
		return e_cryptNotImpl;
	} }

	// write encrypted data to file
	if (f->writeBlock(reinterpret_cast<const char *>(encrypted),
			  static_cast<Q_ULONG>(encSize))
	    != static_cast<Q_LONG>(encSize)) {
		delete_ifnot_null_array(encrypted);
		return e_writeFile;
	}
	delete_ifnot_null_array(encrypted);
	return e_success;
}

PwMerror PwMDoc::decrypt(string *d, unsigned int pos, const QString *pw,
			 char algo, 
			 char hashalgo, //US BUG: pass _hashalgo because we need it in hashPassphrase
			 QFile *f)
{
	PWM_ASSERT(d);
	PWM_ASSERT(pw);
	PWM_ASSERT(f);

	unsigned int cryptLen = f->size() - pos;
	byte *encrypted = new byte[cryptLen];
	byte *decrypted = new byte[cryptLen];

	f->at(pos);
#ifndef PWM_EMBEDDED
	if (f->readBlock(reinterpret_cast<char *>(encrypted),
			 static_cast<Q_ULONG>(cryptLen))
	    != static_cast<Q_LONG>(cryptLen)) {
		delete [] encrypted;
		delete [] decrypted;
		return e_readFile;
	}
#else
	if (f->readBlock((char *)(encrypted),
			 (unsigned long)(cryptLen))
	    != (long)(cryptLen)) {
		delete [] encrypted;
		delete [] decrypted;
		return e_readFile;
	}
#endif
	switch (algo) {
	case PWM_CRYPT_BLOWFISH: {
		Blowfish bf;
		bf.bf_setkey((byte *) pw->latin1(), pw->length());
		bf.bf_decrypt(decrypted, encrypted, cryptLen);
		break;
	}
	case PWM_CRYPT_AES128:
		/*... fall through */
	case PWM_CRYPT_AES192:
	case PWM_CRYPT_AES256:
	case PWM_CRYPT_3DES:
	case PWM_CRYPT_TWOFISH:
	case PWM_CRYPT_TWOFISH128: {
		if (!LibGCryptIf::available())
			return e_cryptNotImpl;
		LibGCryptIf gc;
		PwMerror err;
		err = gc.decrypt(&decrypted,
				 (size_t*)&cryptLen,
				 encrypted,
				 cryptLen,
				 reinterpret_cast<const unsigned char *>(pw->latin1()),
				 pw->length(),
				 algo,
				 hashalgo //US BUG: pass _hashalgo because we need it in hashPassphrase
);
		if (err != e_success) {
			delete [] encrypted;
			delete [] decrypted;
			return e_cryptNotImpl;
		}
		break;
	}
	default: {
		delete [] encrypted;
		delete [] decrypted;
		return e_cryptNotImpl;
	} }
	delete [] encrypted;
#ifndef PWM_EMBEDDED
	d->assign(reinterpret_cast<const char *>(decrypted),
		  static_cast<string::size_type>(cryptLen));
#else
	d->assign((const char *)(decrypted),
		  (string::size_type)(cryptLen));
#endif
	delete [] decrypted;
	if (algo == PWM_CRYPT_BLOWFISH) {
		if (!Blowfish::unpadNull(d)) {
			BUG();
			return e_readFile;
		}
	}
	return e_success;
}

PwMerror PwMDoc::checkDataHash(char dataHashType, const string *dataHash,
			       const string *dataStream)
{
	PWM_ASSERT(dataHash);
	PWM_ASSERT(dataStream);
	switch(dataHashType) {
	case PWM_HASH_SHA1: {
		Sha1 hash;
		hash.sha1_write((byte*)dataStream->c_str(), dataStream->length());
		string ret = hash.sha1_read();
		if (ret != *dataHash)
			return e_fileCorrupt;
		break;
	}
	case PWM_HASH_SHA256:
		/*... fall through */
	case PWM_HASH_SHA384:
	case PWM_HASH_SHA512:
	case PWM_HASH_MD5:
	case PWM_HASH_RMD160:
	case PWM_HASH_TIGER: {
		if (!LibGCryptIf::available())
			return e_hashNotImpl;
		LibGCryptIf gc;
		PwMerror err;
		unsigned char *buf;
		size_t hashLen;
		err = gc.hash(&buf,
			      &hashLen,
			      reinterpret_cast<const unsigned char *>(dataStream->c_str()),
			      dataStream->length(),
			      dataHashType);
		if (err != e_success)
			return e_hashNotImpl;
		string calcHash(reinterpret_cast<const char *>(buf),
				static_cast<string::size_type>(hashLen));
		delete [] buf;
		if (calcHash != *dataHash)
			return e_fileCorrupt;
		break;
	}
	default:
		return e_hashNotImpl;
	}
	return e_success;
}

bool PwMDoc::lockAt(unsigned int category, unsigned int index,
		    bool lock)
{
	if (index >= numEntries(category)) {
		BUG();
		return false;
	}
	if (lock == dti.dta[category].d[index].lockStat)
		return true;

	if (!lock && currentPw != "") {
		// "unlocking" and "password is already set"
		if (!getDocStatFlag(DOC_STAT_UNLOCK_WITHOUT_PW)) {
			// unlocking without pw not allowed
			QString pw;
			pw = requestMpw(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
			if (pw != "") {
				if (pw != currentPw) {
					wrongMpwMsgBox(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
					return false;
				} else {
					timer()->start(DocTimer::id_mpwTimer);
				}
			} else {
				return false;
			}
		} else {
			timer()->start(DocTimer::id_mpwTimer);
		}
	}

	dti.dta[category].d[index].lockStat = lock;
	dti.dta[category].d[index].rev++; // increment revision counter.

	emitDataChanged(this);
	if (!lock)
		timer()->start(DocTimer::id_autoLockTimer);

	return true;

}

bool PwMDoc::lockAt(const QString &category,unsigned int index,
		    bool lock)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		BUG();
		return false;
	}

	return lockAt(cat, index, lock);
}

bool PwMDoc::lockAll(bool lock)
{
	if (!lock && isDeepLocked()) {
		PwMerror ret;
		ret = deepLock(false);
		if (ret != e_success)
			return false;
		return true;
	}
	if (isDocEmpty()) {
		return true;
	}
	if (!lock && currentPw != "") {
		// unlocking and password is already set
		if (!getDocStatFlag(DOC_STAT_UNLOCK_WITHOUT_PW)) {
			// unlocking without pw not allowed
			QString pw;
			pw = requestMpw(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
			if (pw != "") {
				if (pw != currentPw) {
					wrongMpwMsgBox(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
					return false;
				} else {
					timer()->start(DocTimer::id_mpwTimer);
				}
			} else {
				return false;
			}
		} else {
			timer()->start(DocTimer::id_mpwTimer);
		}
	}

	vector<PwMCategoryItem>::iterator catBegin = dti.dta.begin(),
					  catEnd = dti.dta.end(),
					  catI = catBegin;
	vector<PwMDataItem>::iterator entrBegin, entrEnd, entrI;
	while (catI != catEnd) {
		entrBegin = catI->d.begin();
		entrEnd = catI->d.end();
		entrI = entrBegin;
		while (entrI != entrEnd) {
			entrI->lockStat = lock;
			entrI->rev++; // increment revision counter.
			++entrI;
		}
		++catI;
	}

	emitDataChanged(this);
	if (lock)
		timer()->stop(DocTimer::id_autoLockTimer);
	else
		timer()->start(DocTimer::id_autoLockTimer);

	return true;
}

bool PwMDoc::isLocked(const QString &category, unsigned int index)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		BUG();
		return false;
	}

	return isLocked(cat, index);
}

bool PwMDoc::unlockAll_tempoary(bool revert)
{
	static vector< vector<bool> > *oldLockStates = 0;
	static bool wasDeepLocked;

	if (revert) {	// revert the unlocking
		if (oldLockStates) {
			/* we actually _have_ unlocked something, because
			 * we have allocated space for the oldLockStates.
			 * So, go on and revert them!
			 */
			if (wasDeepLocked) {
				PwMerror ret = deepLock(true);
				if (ret == e_success) {
					/* deep-lock succeed. We are save.
					 * (but if it failed, just go on
					 * lock them normally)
					 */
					delete_and_null(oldLockStates);
					timer()->start(DocTimer::id_autoLockTimer);
					printDebug("tempoary unlocking of dta "
						   "reverted by deep-locking.");
					return true;
				}
				printDebug("deep-lock failed while reverting! "
					   "Falling back to normal-lock.");
			}
			if (unlikely(!wasDeepLocked &&
				     numCategories() != oldLockStates->size())) {
				/* DOH! We have modified "dta" while
				 * it was unlocked tempoary. DON'T DO THIS!
				 */
				BUG();
				delete_and_null(oldLockStates);
				timer()->start(DocTimer::id_autoLockTimer);
				return false;
			}
			vector<PwMCategoryItem>::iterator catBegin = dti.dta.begin(),
							  catEnd = dti.dta.end(),
							  catI = catBegin;
			vector<PwMDataItem>::iterator entrBegin, entrEnd, entrI;
			vector< vector<bool> >::iterator oldCatStatI = oldLockStates->begin();
			vector<bool>::iterator oldEntrStatBegin,
					       oldEntrStatEnd,
					       oldEntrStatI;
			while (catI != catEnd) {
				entrBegin = catI->d.begin();
				entrEnd = catI->d.end();
				entrI = entrBegin;
				if (likely(!wasDeepLocked)) {
					oldEntrStatBegin = oldCatStatI->begin();
					oldEntrStatEnd = oldCatStatI->end();
					oldEntrStatI = oldEntrStatBegin;
					if (unlikely(catI->d.size() != oldCatStatI->size())) {
						/* DOH! We have modified "dta" while
						 * it was unlocked tempoary. DON'T DO THIS!
						 */
						BUG();
						delete_and_null(oldLockStates);
						timer()->start(DocTimer::id_autoLockTimer);
						return false;
					}
				}
				while (entrI != entrEnd) {
					if (wasDeepLocked) {
						/* this is an error-fallback if
						 * deeplock didn't succeed
						 */
						entrI->lockStat = true;
					} else {
						entrI->lockStat = *oldEntrStatI;
					}
					++entrI;
					if (likely(!wasDeepLocked))
						++oldEntrStatI;
				}
				++catI;
				if (likely(!wasDeepLocked))
					++oldCatStatI;
			}
			delete_and_null(oldLockStates);
			if (unlikely(wasDeepLocked)) {
				/* error fallback... */
				unsetDocStatFlag(DOC_STAT_DEEPLOCKED);
				emitDataChanged(this);
				printDebug("WARNING: unlockAll_tempoary(true) "
					   "deeplock fallback!");
			}
			printDebug("tempoary unlocking of dta reverted.");
		} else {
			printDebug("unlockAll_tempoary(true): nothing to do.");
		}
		timer()->start(DocTimer::id_autoLockTimer);
	} else {	// unlock all data tempoary
		if (unlikely(oldLockStates != 0)) {
			/* DOH! We have already unlocked the data tempoarly.
			 * No need to do it twice. ;)
			 */
			BUG();
			return false;
		}
		wasDeepLocked = false;
		bool mustUnlock = false;
		if (isDeepLocked()) {
			PwMerror ret;
			while (1) {
				ret = deepLock(false);
				if (ret == e_success) {
					break;
				} else if (ret == e_wrongPw) {
					wrongMpwMsgBox(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
				} else {
					printDebug("deep-unlocking failed while "
						   "tempoary unlocking!");
					return false;
				}
			}
			wasDeepLocked = true;
			mustUnlock = true;
		} else {
			// first check if it's needed to unlock some entries
			vector<PwMCategoryItem>::iterator catBegin = dti.dta.begin(),
							  catEnd = dti.dta.end(),
							  catI = catBegin;
			vector<PwMDataItem>::iterator entrBegin, entrEnd, entrI;
			while (catI != catEnd) {
				entrBegin = catI->d.begin();
				entrEnd = catI->d.end();
				entrI = entrBegin;
				while (entrI != entrEnd) {
					if (entrI->lockStat == true) {
						mustUnlock = true;
						break;
					}
					++entrI;
				}
				if (mustUnlock)
					break;
				++catI;
			}
		}
		if (!mustUnlock) {
			// nothing to do.
			timer()->stop(DocTimer::id_autoLockTimer);
			printDebug("unlockAll_tempoary(): nothing to do.");
			return true;
		} else if (!wasDeepLocked) {
			if (!getDocStatFlag(DOC_STAT_UNLOCK_WITHOUT_PW) &&
			    currentPw != "") {
				/* we can't unlock without mpw, so
				 * we need to ask for it.
				 */
				QString pw;
				while (1) {
					pw = requestMpw(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
					if (pw == "") {
						return false;
					} else if (pw == currentPw) {
						break;
					}
					wrongMpwMsgBox(getDocStatFlag(DOC_STAT_USE_CHIPCARD));
				}
			}
		}
		timer()->stop(DocTimer::id_autoLockTimer);
		oldLockStates = new vector< vector<bool> >;
		vector<bool> tmp_vec;
		vector<PwMCategoryItem>::iterator catBegin = dti.dta.begin(),
						  catEnd = dti.dta.end(),
						  catI = catBegin;
		vector<PwMDataItem>::iterator entrBegin, entrEnd, entrI;
		while (catI != catEnd) {
			entrBegin = catI->d.begin();
			entrEnd = catI->d.end();
			entrI = entrBegin;
			while (entrI != entrEnd) {
				if (!wasDeepLocked) {
					tmp_vec.push_back(entrI->lockStat);
				}
				entrI->lockStat = false;
				++entrI;
			}
			if (!wasDeepLocked) {
				oldLockStates->push_back(tmp_vec);
				tmp_vec.clear();
			}
			++catI;
		}
		printDebug("tempoary unlocked dta.");
	}

	return true;
}

PwMerror PwMDoc::deepLock(bool lock, bool saveToFile)
{
	PwMerror ret;
	/* NOTE: saveDoc() depends on this function to return
	 *       e_success if saveToFile == false
	 */

	if (lock) {
		if (isDeepLocked())
			return e_lock;
		if (saveToFile) {
			if (isDocEmpty())
				return e_docIsEmpty;
			ret = saveDoc(conf()->confGlobCompression());
			if (ret == e_filename) {
				/* the doc wasn't saved to a file
				 * by the user, yet.
				 */
				cantDeeplock_notSavedMsgBox();
				return e_docNotSaved;
			} else if (ret != e_success) {
				return e_lock;
			}
		}
		timer()->stop(DocTimer::id_autoLockTimer);
		clearDoc();
		PwMDataItem d;
		d.desc = IS_DEEPLOCKED_SHORTMSG.latin1();
		d.comment = IS_DEEPLOCKED_MSG.latin1();
		d.listViewPos = 0;
		addEntry(DEFAULT_CATEGORY, &d, true);
		lockAt(DEFAULT_CATEGORY, 0, true);
		unsetDocStatFlag(DOC_STAT_DISK_DIRTY);
		setDocStatFlag(DOC_STAT_DEEPLOCKED);
	} else {
		if (!isDeepLocked())
			return e_lock;
		ret = openDoc(&filename, (conf()->confGlobUnlockOnOpen())
					  ? 0 : 1);
		if (ret == e_wrongPw) {
			return e_wrongPw;
		} else if (ret != e_success) {
			printDebug(string("PwMDoc::deepLock(false): ERR! openDoc() == ")
				   + tostr(static_cast<int>(ret)));
			return e_lock;
		}
		unsetDocStatFlag(DOC_STAT_DEEPLOCKED);
		timer()->start(DocTimer::id_autoLockTimer);
	}

	emitDataChanged(this);
	return e_success;
}

void PwMDoc::_deepUnlock()
{
	deepLock(false);
}

void PwMDoc::clearDoc()
{
	dti.clear();
	PwMCategoryItem d;
	//US ENH: to initialize all members with meaningfull data.
	d.clear();
	d.name = DEFAULT_CATEGORY.latin1();
	dti.dta.push_back(d);
	currentPw = "";
	unsetDocStatFlag(DOC_STAT_UNLOCK_WITHOUT_PW);
}

void PwMDoc::changeCurrentPw()
{
	if (currentPw == "")
		return; // doc hasn't been saved. No mpw available.
	bool useChipcard = getDocStatFlag(DOC_STAT_USE_CHIPCARD);
	QString pw = requestMpwChange(&currentPw, &useChipcard);
	if (pw == "")
		return;
	if (useChipcard)
		setDocStatFlag(DOC_STAT_USE_CHIPCARD);
	else
		unsetDocStatFlag(DOC_STAT_USE_CHIPCARD);
	setCurrentPw(pw);
}

void PwMDoc::setListViewPos(const QString &category, unsigned int index,
			    int pos)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		BUG();
		return;
	}
	setListViewPos(cat, index, pos);
}

void PwMDoc::setListViewPos(unsigned int category, unsigned int index,
			    int pos)
{
	dti.dta[category].d[index].listViewPos = pos;

/* FIXME workaround: don't flag dirty, because this function sometimes
 * get's called when it shouldn't. It's because PwMView assumes
 * the user resorted the UI on behalf of signal layoutChanged().
 * This is somewhat broken and incorrect, but I've no other
 * solution for now.
 */
//	setDocStatFlag(DOC_STAT_DISK_DIRTY);
}

int PwMDoc::getListViewPos(const QString &category, unsigned int index)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		BUG();
		return -1;
	}

	return dti.dta[cat].d[index].listViewPos;
}

void PwMDoc::findEntry(unsigned int category, PwMDataItem find, unsigned int searchIn,
		       vector<unsigned int> *foundPositions, bool breakAfterFound,
		       bool caseSensitive, bool exactWordMatch, bool sortByLvp)
{
	PWM_ASSERT(foundPositions);
	PWM_ASSERT(searchIn);
	foundPositions->clear();

	unsigned int i, entries = numEntries(category);
	for (i = 0; i < entries; ++i) {
		if (searchIn & SEARCH_IN_DESC) {
			if (!compareString(find.desc, dti.dta[category].d[i].desc,
					  caseSensitive, exactWordMatch)) {
				continue;
			}
		}
		if (searchIn & SEARCH_IN_NAME) {
			if (!compareString(find.name, dti.dta[category].d[i].name,
					  caseSensitive, exactWordMatch)) {
				continue;
			}
		}
		if (searchIn & SEARCH_IN_PW) {
			bool wasLocked = isLocked(category, i);
			getDataChangedLock();
			lockAt(category, i, false);
			if (!compareString(find.pw, dti.dta[category].d[i].pw,
					  caseSensitive, exactWordMatch)) {
				lockAt(category, i, wasLocked);
				putDataChangedLock();
				continue;
			}
			lockAt(category, i, wasLocked);
			putDataChangedLock();
		}
		if (searchIn & SEARCH_IN_COMMENT) {
			if (!compareString(find.comment, dti.dta[category].d[i].comment,
					  caseSensitive, exactWordMatch)) {
				continue;
			}
		}
		if (searchIn & SEARCH_IN_URL) {
			if (!compareString(find.url, dti.dta[category].d[i].url,
					  caseSensitive, exactWordMatch)) {
				continue;
			}
		}
		if (searchIn & SEARCH_IN_LAUNCHER) {
			if (!compareString(find.launcher, dti.dta[category].d[i].launcher,
					  caseSensitive, exactWordMatch)) {
				continue;
			}
		}

		// all selected "searchIn" matched.
		foundPositions->push_back(i);
		if (breakAfterFound)
			break;
	}

	if (sortByLvp && foundPositions->size() > 1) {
		vector< pair<unsigned int /* foundPosition (real doc pos) */,
			     unsigned int /* lvp-pos */> > tmp_vec;
		
		unsigned int i, items = foundPositions->size();
		pair<unsigned int, unsigned int> tmp_pair;
		for (i = 0; i < items; ++i) {
			tmp_pair.first = (*foundPositions)[i];
			tmp_pair.second = dti.dta[category].d[(*foundPositions)[i]].listViewPos;
			tmp_vec.push_back(tmp_pair);
		}
		sort(tmp_vec.begin(), tmp_vec.end(), dta_lvp_greater());
		foundPositions->clear();
		for (i = 0; i < items; ++i) {
			foundPositions->push_back(tmp_vec[i].first);
		}
	}
}

void PwMDoc::findEntry(const QString &category, PwMDataItem find, unsigned int searchIn,
		       vector<unsigned int> *foundPositions, bool breakAfterFound,
		       bool caseSensitive, bool exactWordMatch, bool sortByLvp)
{
	PWM_ASSERT(foundPositions);
	unsigned int cat = 0;

	if (!findCategory(category, &cat)) {
		foundPositions->clear();
		return;
	}

	findEntry(cat, find, searchIn, foundPositions, breakAfterFound,
		  caseSensitive, exactWordMatch, sortByLvp);
}

bool PwMDoc::compareString(const string &s1, const string &s2, bool caseSensitive,
			   bool exactWordMatch)
{
	QString _s1(s1.c_str());
	QString _s2(s2.c_str());
	if (!caseSensitive) {
		_s1 = _s1.lower();
		_s2 = _s2.lower();
	}
	if (exactWordMatch ? (_s1 == _s2) : (_s2.find(_s1) != -1))
		return true;
	return false;
}

bool PwMDoc::findCategory(const QString &name, unsigned int *index)
{
	vector<PwMCategoryItem>::iterator i = dti.dta.begin(),
					  end = dti.dta.end();
	while (i != end) {
		if ((*i).name == name.latin1()) {
			if (index) {
				*index = i - dti.dta.begin();
			}
			return true;
		}
		++i;
	}
	return false;
}

bool PwMDoc::renameCategory(const QString &category, const QString &newName)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat))
		return false;

	return renameCategory(cat, newName);
}

bool PwMDoc::renameCategory(unsigned int category, const QString &newName,
			    bool dontFlagDirty)
{
	if (category > numCategories() - 1)
		return false;

	dti.dta[category].name = newName.latin1();
	if (!dontFlagDirty)
		flagDirty();

	return true;
}

bool PwMDoc::delCategory(const QString &category)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat))
		return false;

	return delCategory(cat);
}

bool PwMDoc::delCategory(unsigned int category, bool dontFlagDirty)
{
	if (category > numCategories() - 1)
		return false;

	// We don't delete it, if it is the last existing
	// category! Instead we rename it to "Default".
	if (numCategories() > 1) {
		dti.dta.erase(dti.dta.begin() + category);
	} else {
		renameCategory(category, DEFAULT_CATEGORY, dontFlagDirty);
		return true;
	}
	if (!dontFlagDirty)
		flagDirty();

	return true;
}

void PwMDoc::delAllEmptyCat(bool dontFlagDirty)
{
	vector<PwMCategoryItem>::iterator begin = dti.dta.begin(),
					  end = dti.dta.end(),
					  i = begin;
	while (i != end) {
		if (i->d.empty()) {
			delCategory(begin - i, dontFlagDirty);
		}
		++i;
	}
}

void PwMDoc::getCategoryList(vector<string> *list)
{
	PWM_ASSERT(list);
	list->clear();
	vector<PwMCategoryItem>::iterator i = dti.dta.begin(),
					  end = dti.dta.end();
	while (i != end) {
		list->push_back(i->name);
		++i;
	}
}

void PwMDoc::getCategoryList(QStringList *list)
{
	PWM_ASSERT(list);
	list->clear();
	vector<PwMCategoryItem>::iterator i = dti.dta.begin(),
					  end = dti.dta.end();
	while (i != end) {
#ifndef PWM_EMBEDDED
		list->push_back(i->name.c_str());
#else
		list->append(i->name.c_str());
#endif
		++i;
	}
}

void PwMDoc::getEntryList(const QString &category, QStringList *list)
{
	PWM_ASSERT(list);
	unsigned int cat = 0;
	if (!findCategory(category, &cat)) {
		list->clear();
		return;
	}
	getEntryList(cat, list);
}

void PwMDoc::getEntryList(const QString &category, vector<string> *list)
{
	PWM_ASSERT(list);
	unsigned int cat = 0;
	if (!findCategory(category, &cat)) {
		list->clear();
		return;
	}
	getEntryList(cat, list);
}

void PwMDoc::getEntryList(unsigned int category, vector<string> *list)
{
	PWM_ASSERT(list);
	list->clear();
	vector<PwMDataItem>::iterator begin = dti.dta[category].d.begin(),
				      end = dti.dta[category].d.end(),
				      i = begin;
	while (i != end) {
		list->push_back(i->desc);
		++i;
	}
}

void PwMDoc::getEntryList(unsigned int category, QStringList *list)
{
	PWM_ASSERT(list);
	list->clear();
	vector<PwMDataItem>::iterator begin = dti.dta[category].d.begin(),
				      end = dti.dta[category].d.end(),
				      i = begin;
	while (i != end) {
#ifndef PWM_EMBEDDED
		list->push_back(i->desc.c_str());
#else
		list->append(i->desc.c_str());
#endif
		++i;
	}
}

bool PwMDoc::execLauncher(const QString &category, unsigned int entryIndex)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat))
		return false;

	return execLauncher(cat, entryIndex);
}

bool PwMDoc::execLauncher(unsigned int category, unsigned int entryIndex)
{
#ifndef _WIN32_
	if (geteuid() == 0) {
		rootAlertMsgBox();
		return false;
	}
#endif
	QString command(dti.dta[category].d[entryIndex].launcher.c_str());
	bool wasLocked = isLocked(category, entryIndex);

	if (command.find("$p") != -1) {
		/* the user requested the password to be included
		 * into the command. We have to ask for the password,
		 * if it's locked. We do that by unlocking the entry
		 */
		if (!lockAt(category, entryIndex, false))
			return false;
	}
#ifndef PWM_EMBEDDED
	command.replace("$d", dti.dta[category].d[entryIndex].desc.c_str());
	command.replace("$n", dti.dta[category].d[entryIndex].name.c_str());
	command.replace("$p", dti.dta[category].d[entryIndex].pw.c_str());
	command.replace("$u", dti.dta[category].d[entryIndex].url.c_str());
	command.replace("$c", dti.dta[category].d[entryIndex].comment.c_str());
#else
	command.replace(QRegExp("$d"), dti.dta[category].d[entryIndex].desc.c_str());
	command.replace(QRegExp("$n"), dti.dta[category].d[entryIndex].name.c_str());
	command.replace(QRegExp("$p"), dti.dta[category].d[entryIndex].pw.c_str());
	command.replace(QRegExp("$u"), dti.dta[category].d[entryIndex].url.c_str());
	command.replace(QRegExp("$c"), dti.dta[category].d[entryIndex].comment.c_str());
#endif
	command.append(" &");

	QString customXterm(conf()->confGlobXtermCommand());
	if (!customXterm.isEmpty())
		command = customXterm + " " + command;

	system(command.latin1());

	lockAt(category, entryIndex, wasLocked);
	return true;
}

bool PwMDoc::goToURL(const QString &category, unsigned int entryIndex)
{
	unsigned int cat = 0;

	if (!findCategory(category, &cat))
		return false;

	return goToURL(cat, entryIndex);
}

bool PwMDoc::goToURL(unsigned int category, unsigned int entryIndex)
{
#ifndef _WIN32_
	if (geteuid() == 0) {
		rootAlertMsgBox();
		return false;
	}
#endif
	QString url(dti.dta[category].d[entryIndex].url.c_str());
	if (url.isEmpty())
		return false;

	QString customBrowser(conf()->confGlobBrowserCommand());
	if (!customBrowser.isEmpty()) {
		browserProc.clearArguments();
		browserProc << customBrowser << url;
		if (browserProc.start(KProcess::DontCare))
			return true;
	}

	browserProc.clearArguments();
	browserProc << "konqueror" << url;
	if (browserProc.start(KProcess::DontCare))
		return true;

	browserProc.clearArguments();
	browserProc << "mozilla" << url;
	if (browserProc.start(KProcess::DontCare))
		return true;

	browserProc.clearArguments();
	browserProc << "opera" << url;
	if (browserProc.start(KProcess::DontCare))
		return true;
	return false;
}

PwMerror PwMDoc::exportToText(const QString *file)
{
	PWM_ASSERT(file);
	if (QFile::exists(*file)) {
		if (!QFile::remove(*file))
			return e_accessFile;
	}
	QFile f(*file);
	if (!f.open(IO_ReadWrite))
		return e_openFile;

	if (!unlockAll_tempoary()) {
		f.close();
		return e_lock;
	}

	// write header
	string header = i18n("Password table generated by\nPwM v").latin1();
	header += PACKAGE_VER;
	header += i18n("\non ").latin1();
	QDate currDate = QDate::currentDate();
	QTime currTime = QTime::currentTime();

#ifndef PWM_EMBEDDED
	header += currDate.toString("ddd MMMM d ").latin1();
	header += currTime.toString("hh:mm:ss ").latin1();
#else
	QString dfs = KGlobal::locale()->dateFormatShort();
	bool ampm = KGlobal::locale()->use12Clock();
	KGlobal::locale()->setDateFormatShort("%A %B %d");
	KGlobal::locale()->setHore24Format(true);

	header += KGlobal::locale()->formatDate(currDate, true, KLocale::Userdefined).latin1();
	header += KGlobal::locale()->formatTime(currTime, true).latin1();
	KGlobal::locale()->setDateFormatShort(dfs);
	KGlobal::locale()->setHore24Format(!ampm);

#endif
	header += tostr(currDate.year());
	header += "\n==============================\n\n";


#ifndef PWM_EMBEDDED
	if (f.writeBlock(header.c_str(), header.length()) != (Q_LONG)header.length()) {
		unlockAll_tempoary(true);
		f.close();
		return e_writeFile;
	}
#else
	if (f.writeBlock(header.c_str(), header.length()) != (long)header.length()) {
		unlockAll_tempoary(true);
		f.close();
		return e_writeFile;
	}
#endif
	unsigned int i, numCat = numCategories();
	unsigned int j, numEnt;
	string exp;
	for (i = 0; i < numCat; ++i) {
		numEnt = numEntries(i);

		exp = "\n== Category: ";
		exp += dti.dta[i].name;
		exp += " ==\n";
#ifndef PWM_EMBEDDED
		if (f.writeBlock(exp.c_str(), exp.length()) != (Q_LONG)exp.length()) {
				unlockAll_tempoary(true);
				f.close();
				return e_writeFile;
		}
#else
		if (f.writeBlock(exp.c_str(), exp.length()) != (long)exp.length()) {
				unlockAll_tempoary(true);
				f.close();
				return e_writeFile;
		}
#endif
		for (j = 0; j < numEnt; ++j) {
			exp = "\n-- ";
			exp += dti.dta[i].d[j].desc;
			exp += " --\n";

			exp += i18n("Username: ").latin1();
			exp += dti.dta[i].d[j].name;
			exp += "\n";

			exp += i18n("Password: ").latin1();
			exp += dti.dta[i].d[j].pw;
			exp += "\n";

			exp += i18n("Comment: ").latin1();
			exp += dti.dta[i].d[j].comment;
			exp += "\n";

			exp += i18n("URL: ").latin1();
			exp += dti.dta[i].d[j].url;
			exp += "\n";

			exp += i18n("Launcher: ").latin1();
			exp += dti.dta[i].d[j].launcher;
			exp += "\n";

#ifndef PWM_EMBEDDED
			if (f.writeBlock(exp.c_str(), exp.length()) != (Q_LONG)exp.length()) {
				unlockAll_tempoary(true);
				f.close();
				return e_writeFile;
			}
#else
			if (f.writeBlock(exp.c_str(), exp.length()) != (long)exp.length()) {
				unlockAll_tempoary(true);
				f.close();
				return e_writeFile;
			}
#endif
		}
	}
	unlockAll_tempoary(true);
	f.close();

	return e_success;
}

PwMerror PwMDoc::importFromText(const QString *file, int format)
{
	PWM_ASSERT(file);
	if (format == 0)
		return importText_PwM(file);
	else if (format == -1) {
		// probe for all formats
		if (importText_PwM(file) == e_success)
			return e_success;
		dti.clear();
		emitDataChanged(this);
		// add next format here...
		return e_fileFormat;
	}
	return e_invalidArg;
}

PwMerror PwMDoc::importText_PwM(const QString *file)
{
#ifndef PWM_EMBEDDED
	PWM_ASSERT(file);
	FILE *f;
	int tmp;
	ssize_t ret;
	string curCat;
	unsigned int entriesRead = 0;
	PwMDataItem currItem;
	f = fopen(file->latin1(), "r");
	if (!f)
		return e_openFile;
	size_t ch_tmp_size = 1024;
	char *ch_tmp = (char*)malloc(ch_tmp_size);
	if (!ch_tmp) {
		fclose(f);
		return e_outOfMem;
	}

	// - check header
	if (getline(&ch_tmp, &ch_tmp_size, f) == -1) // skip first line.
		goto formatError;
	// check version-string and return version in "ch_tmp".
	if (fscanf(f, "PwM v%s", ch_tmp) != 1) {
		// header not recognized as PwM generated header
		goto formatError;
	}
	// set filepointer behind version-string-line previously checked
	if (getline(&ch_tmp, &ch_tmp_size, f) == -1)
		goto formatError;
	// skip next line containing the build-date
	if (getline(&ch_tmp, &ch_tmp_size, f) == -1)
		goto formatError;
	// read header termination line
	if (getline(&ch_tmp, &ch_tmp_size, f) == -1)
		goto formatError;
	if (strcmp(ch_tmp, "==============================\n"))
		goto formatError;

	// - read entries
	do {
		// find beginning of next category
		do {
			tmp = fgetc(f);
		} while (tmp == '\n' && tmp != EOF);
		if (tmp == EOF)
			break;

		// decrement filepos by one
		fseek(f, -1, SEEK_CUR);
		// read cat-name
		if (getline(&ch_tmp, &ch_tmp_size, f) == -1)
			goto formatError;
		// check cat-name format
		if (memcmp(ch_tmp, "== Category: ", 13) != 0)
			goto formatError;
		if (memcmp(ch_tmp + (strlen(ch_tmp) - 1 - 3), " ==", 3) != 0)
			goto formatError;
		// copy cat-name
		curCat.assign(ch_tmp + 13, strlen(ch_tmp) - 1 - 16);

		do {
			// find beginning of next entry
			do {
				tmp = fgetc(f);
			} while (tmp == '\n' && tmp != EOF && tmp != '=');
			if (tmp == EOF)
				break;
			if (tmp == '=') {
				fseek(f, -1, SEEK_CUR);
				break;
			}
			// decrement filepos by one
			fseek(f, -1, SEEK_CUR);
			// read desc-line
			if (getline(&ch_tmp, &ch_tmp_size, f) == -1)
				goto formatError;
			// check desc-line format
			if (memcmp(ch_tmp, "-- ", 3) != 0)
				goto formatError;
			if (memcmp(ch_tmp + (strlen(ch_tmp) - 1 - 3), " --", 3) != 0)
				goto formatError;
			// add desc-line
			currItem.desc.assign(ch_tmp + 3, strlen(ch_tmp) - 1 - 6);

			// read username-line
			if ((ret = getline(&ch_tmp, &ch_tmp_size, f)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.name))
				goto formatError;

			// read pw-line
			if ((ret = getline(&ch_tmp, &ch_tmp_size, f)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.pw))
				goto formatError;

			// read comment-line
			if ((ret = getline(&ch_tmp, &ch_tmp_size, f)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.comment))
				goto formatError;

			// read URL-line
			if ((ret = getline(&ch_tmp, &ch_tmp_size, f)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.url))
				goto formatError;

			// read launcher-line
			if ((ret = getline(&ch_tmp, &ch_tmp_size, f)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.launcher))
				goto formatError;

			currItem.lockStat = true;
			currItem.listViewPos = -1;
			addEntry(curCat.c_str(), &currItem, true);
			++entriesRead;
		} while (1);
	} while (1);
	if (!entriesRead)
		goto formatError;

	free(ch_tmp);
	fclose(f);
	flagDirty();
	return e_success;

      formatError:
	free(ch_tmp);
	fclose(f);
	return e_fileFormat;
#else
	PWM_ASSERT(file);
	QFile f(file->latin1());
	int tmp;
	ssize_t ret;
	string curCat;
	unsigned int entriesRead = 0;
	PwMDataItem currItem;
	bool res = f.open(IO_ReadOnly);
	if (res == false)
	  return e_openFile;

	unsigned int ch_tmp_size = 1024;
	char *ch_tmp = (char*)malloc(ch_tmp_size);
	if (!ch_tmp) {
		f.close();
		return e_outOfMem;
	}

	// - check header
	if (f.readLine(ch_tmp, ch_tmp_size) == -1) // skip first line.
		goto formatError;

	//US read fileversion first, then check if ok.
	if (f.readLine(ch_tmp, ch_tmp_size) == -1)
		goto formatError;

	// check version-string and return version in "ch_tmp".
	//US 	if (fscanf(f, "PwM v%s", ch_tmp) != 1) {
	//US		// header not recognized as PwM generated header
	//US		goto formatError;
	//US	}
	//US set filepointer behind version-string-line previously checked
	//US if (f.readLine(ch_tmp, ch_tmp_size) == -1)
	//US		goto formatError;
	// skip next line containing the build-date
	if (f.readLine(ch_tmp, ch_tmp_size) == -1)
		goto formatError;
	// read header termination line
	if (f.readLine(ch_tmp, ch_tmp_size) == -1)
		goto formatError;
	if (strcmp(ch_tmp, "==============================\n"))
		goto formatError;

	// - read entries
	do {
		// find beginning of next category
		do {
			tmp = f.getch();
		} while (tmp == '\n' && tmp != EOF);
		if (tmp == EOF)
			break;

		// decrement filepos by one
		f.at(f.at()-1);
		// read cat-name
		if (f.readLine(ch_tmp, ch_tmp_size) == -1)
			goto formatError;
		// check cat-name format
		if (memcmp(ch_tmp, "== Category: ", 13) != 0)
			goto formatError;
		if (memcmp(ch_tmp + (strlen(ch_tmp) - 1 - 3), " ==", 3) != 0)
			goto formatError;
		// copy cat-name
		curCat.assign(ch_tmp + 13, strlen(ch_tmp) - 1 - 16);

		do {
			// find beginning of next entry
			do {
				tmp = f.getch();
			} while (tmp == '\n' && tmp != EOF && tmp != '=');
			if (tmp == EOF)
				break;
			if (tmp == '=') {
			        f.at(f.at()-1);
				break;
			}
			// decrement filepos by one
		        f.at(f.at()-1);
			// read desc-line
			if (f.readLine(ch_tmp, ch_tmp_size) == -1)
				goto formatError;
			// check desc-line format
			if (memcmp(ch_tmp, "-- ", 3) != 0)
				goto formatError;
			if (memcmp(ch_tmp + (strlen(ch_tmp) - 1 - 3), " --", 3) != 0)
				goto formatError;
			// add desc-line
			currItem.desc.assign(ch_tmp + 3, strlen(ch_tmp) - 1 - 6);

			// read username-line
			if ((ret = f.readLine(ch_tmp, ch_tmp_size)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.name))
				goto formatError;

			// read pw-line
			if ((ret = f.readLine(ch_tmp, ch_tmp_size)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.pw))
				goto formatError;

			// read comment-line
			if ((ret = f.readLine(ch_tmp, ch_tmp_size)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.comment))
				goto formatError;

			// read URL-line
			if ((ret = f.readLine(ch_tmp, ch_tmp_size)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.url))
				goto formatError;

			// read launcher-line
			if ((ret = f.readLine(ch_tmp, ch_tmp_size)) == -1)
				goto formatError;
			if (!textExtractEntry_PwM(ch_tmp, ret, &currItem.launcher))
				goto formatError;

			currItem.lockStat = true;
			currItem.listViewPos = -1;
			addEntry(curCat.c_str(), &currItem, true);
			++entriesRead;
		} while (1);
	} while (1);
	if (!entriesRead)
		goto formatError;

	free(ch_tmp);
	f.close();
	flagDirty();
	return e_success;

      formatError:
	free(ch_tmp);
	f.close();
	return e_fileFormat;

#endif

}

bool PwMDoc::textExtractEntry_PwM(const char *in, ssize_t in_size, string *out)
{
	PWM_ASSERT(in && out);
	ssize_t i = 0, len = in_size - 1;
	while (i < len) {
		if (in[i] == ':')
			break;
		++i;
	}
	i += 2;
	*out = "";
	out->append(in + i, in_size - i - 1);
	return true;
}

PwMerror PwMDoc::exportToGpasman(const QString *file)
{
	PWM_ASSERT(file);
	GpasmanFile gp;
	int ret;

	if (!unlockAll_tempoary())
		return e_lock;

	QString gpmPassword;
	while (1) {
		gpmPassword = requestNewMpw(0);
		if (gpmPassword == "") {
			unlockAll_tempoary(true);
			return e_noPw;
		}
		if (gpmPassword.length() < 4) {
			gpmPwLenErrMsgBox();
		} else {
			break;
		}
	}

	ret = gp.save_init(file->latin1(), gpmPassword.latin1());
	if (ret != 1) {
		unlockAll_tempoary(true);
		return e_accessFile;
	}

	char *entry[4];
	unsigned int numCat = numCategories(), i;
	unsigned int numEntr, j;
	int descLen, nameLen, pwLen, commentLen;
	for (i = 0; i < numCat; ++i) {
		numEntr = numEntries(i);
		for (j = 0; j < numEntr; ++j) {
			descLen = dti.dta[i].d[j].desc.length();
			nameLen = dti.dta[i].d[j].name.length();
			pwLen = dti.dta[i].d[j].pw.length();
			commentLen = dti.dta[i].d[j].comment.length();
			entry[0] = new char[descLen + 1];
			entry[1] = new char[nameLen + 1];
			entry[2] = new char[pwLen + 1];
			entry[3] = new char[commentLen + 1];
			strcpy(entry[0], descLen == 0 ? " " : dti.dta[i].d[j].desc.c_str());
			strcpy(entry[1], nameLen == 0 ? " " : dti.dta[i].d[j].name.c_str());
			strcpy(entry[2], pwLen == 0 ? " " : dti.dta[i].d[j].pw.c_str());
			strcpy(entry[3], commentLen == 0 ? " " : dti.dta[i].d[j].comment.c_str());
			entry[0][descLen == 0 ? descLen + 1 : descLen] = '\0';
			entry[1][nameLen == 0 ? nameLen + 1 : nameLen] = '\0';
			entry[2][pwLen == 0 ? pwLen + 1 : pwLen] = '\0';
			entry[3][commentLen == 0 ? commentLen + 1 : commentLen] = '\0';

			ret = gp.save_entry(entry);
			if (ret == -1){
				delete [] entry[0];
				delete [] entry[1];
				delete [] entry[2];
				delete [] entry[3];
				gp.save_finalize();
				unlockAll_tempoary(true);
				return e_writeFile;
			}

			delete [] entry[0];
			delete [] entry[1];
			delete [] entry[2];
			delete [] entry[3];
		}
	}
	unlockAll_tempoary(true);
	if (gp.save_finalize() == -1)
		return e_writeFile;

	return e_success;
}

PwMerror PwMDoc::importFromGpasman(const QString *file)
{
	PWM_ASSERT(file);
	QString pw = requestMpw(false);
	if (pw == "")
		return e_noPw;
	GpasmanFile gp;
	int ret, i;
	PwMerror ret2;
	char *entry[4];
	PwMDataItem tmpData;
	ret = gp.load_init(file->latin1(), pw.latin1());
	if (ret != 1)
		return e_accessFile;

	do {
		ret = gp.load_entry(entry);
		if(ret != 1)
			break;
		tmpData.desc = entry[0];
		tmpData.name = entry[1];
		tmpData.pw = entry[2];
		tmpData.comment = entry[3];
		tmpData.lockStat = true;
		tmpData.listViewPos = -1;
		ret2 = addEntry(DEFAULT_CATEGORY, &tmpData, true);
		for (i = 0; i < 4; ++i)
         		free(entry[i]);
		if (ret2 == e_maxAllowedEntr) {
			gp.load_finalize();
			return e_maxAllowedEntr;
		}
	} while (1);
	gp.load_finalize();
	if (isDocEmpty())
		return e_wrongPw; // we assume this.

	flagDirty();
	return e_success;
}


//US: we use the stl sort algorythm to sort all elements in the order 
//of its listViewPos (in the order 1,2,3,5,...,x,-1, -1, -1
struct PwMDataItemListViewPosSort
{
  bool operator()(PwMDataItem* rpStart, PwMDataItem* rpEnd)
  {
    //qDebug("pwMDoc::PwMDataItemListViewPosSort()");
    if ((rpEnd)->listViewPos < 0)
	return false;
    else
      return (rpStart)->listViewPos < (rpEnd)->listViewPos;
  }
};

void PwMDoc::ensureLvp()
{
	if (isDocEmpty())
		return;

	//US ENH BUG: when using syncronizing, this way of sorting
	//is not sufficient, because there might be empty spaces
	// at the beginning. But the old algorythm only can add elements
	//to the end.The result are crashes because of list overflows
	//we need something to fill all gaps.
	vector<PwMDataItem*> sorted;
	vector< PwMDataItem*>::iterator sortedBegin,
					sortedEnd,
					sortedI;
	vector<PwMCategoryItem>::iterator catBegin = dti.dta.begin(),
					  catEnd = dti.dta.end(),
					  catI = catBegin;
	vector<PwMDataItem>::iterator entrBegin, entrEnd, entrI;
	int lvpTop, tmpLvp;

	//qDebug("collect:"); 

	while (catI != catEnd) {
		lvpTop = -1;
		sorted.clear();

		entrBegin = catI->d.begin();
		entrEnd = catI->d.end();
		entrI = entrBegin;

		//US: we use the stl sort algorythm to sort all elements in the order 
		//of its listViewPos (in the order 1,2,2,3,5,...,x,-1, -1, -1
		while (entrI != entrEnd) {
		  //qDebug("found: %s, pos=%i", (*entrI).desc.c_str(), (*entrI).listViewPos);
		  sorted.push_back((PwMDataItem*)&(*entrI));
		  ++entrI;
		}

		sortedBegin = sorted.begin();
		sortedEnd = sorted.end();

		sort(sortedBegin, sortedEnd, PwMDataItemListViewPosSort());

		//		qDebug("resort:"); 
		//now we have all sorted in a collection
		//Now start with the sorted and reset listviewpos.
		sortedBegin = sorted.begin();
		sortedEnd = sorted.end();
		sortedI = sortedBegin;

		while (sortedI != sortedEnd) {
		  //		  qDebug("reset defined: %s, from pos=%i to pos=%i", (*sortedI)->desc.c_str(), (*sortedI)->listViewPos, lvpTop+1);
		  (*sortedI)->listViewPos = ++lvpTop;
		  ++sortedI;
		}

		/*/debug
		entrBegin = catI->d.begin();
		entrEnd = catI->d.end();
		entrI = entrBegin;

		while (entrI != entrEnd) {
		  qDebug("check: %s, pos=%i", (*entrI).desc.c_str(), (*entrI).listViewPos);
		  ++entrI;
		}
		*/

		++catI;
	}
}

QString PwMDoc::getTitle()
{
	/* NOTE: We have to ensure, that the returned title
	 *       is unique and not reused somewhere else while
	 *       this document is valid (open).
	 */
	QString title(getFilename());

	//US ENH: The whole filename on PDAs is too long. So use only the last characters
        if (QApplication::desktop()->width() < 640)
	{
	  if (title.length() > 30)
	    title = "..." + title.right(30);

	}


	if (title.isEmpty()) {
		if (unnamedNum == 0) {
			unnamedNum = PwMDocList::getNewUnnamedNumber();
			PWM_ASSERT(unnamedNum != 0);
		}
		title = DEFAULT_TITLE;
		title += " ";
		title += tostr(unnamedNum).c_str();
	}
	return title;
}

bool PwMDoc::tryDelete()
{
 
	if (deleted)
		return true;
	int ret;
	if (isDirty()) {
		ret = dirtyAskSave(getTitle());
		if (ret == 0) { // save to disk
			if (!saveDocUi(this))
				goto out_ignore;
		} else if (ret == 1) { // don't save and delete
			goto out_accept;
		} else { // cancel operation
			goto out_ignore;
		}
	}
out_accept:
	deleted = true;
	delete this;
	return true;
out_ignore:
	return false;
}



#ifdef PWM_EMBEDDED
//US ENH: this is the magic function that syncronizes the this doc with the remote doc
//US it could have been defined as static, but I did not want to. 
PwMerror PwMDoc::syncronize(KSyncManager* manager, PwMDoc* syncLocal , PwMDoc* syncRemote, int mode )
{
  int addedPasswordsLocal = 0;
  int addedPasswordsRemote = 0;
  int deletedPasswordsRemote = 0;
  int deletedPasswordsLocal = 0;
  int changedLocal = 0;
  int changedRemote = 0;

  PwMSyncItem* syncItemLocal;
  PwMSyncItem* syncItemRemote;

  QString mCurrentSyncName = manager->getCurrentSyncName();
  QString mCurrentSyncDevice = manager->getCurrentSyncDevice();

  bool mSyncLauncher = true;

  bool fullDateRange = false;
  int take;
  //    local->resetTempSyncStat();
  QDateTime mLastSync = QDateTime::currentDateTime();
  QDateTime modifiedSync = mLastSync;

  unsigned int index;
  //Step 1. Find syncinfo in Local file and create if not existent.
  bool found = syncLocal->findSyncData(mCurrentSyncDevice, &index);
  if (found == false)
    {
      PwMSyncItem newSyncItemLocal;
      newSyncItemLocal.syncName = mCurrentSyncDevice.latin1();
      newSyncItemLocal.lastSyncDate = mLastSync;
      syncLocal->addSyncDataEntry(&newSyncItemLocal, true);
      found = syncLocal->findSyncData(mCurrentSyncDevice, &index);
      if (found == false) {
	qDebug("PwMDoc::syncronize : newly created local sync data could not be found");
	return e_syncError;
      }
    }	

  syncItemLocal = syncLocal->getSyncDataEntry(index); 
  qDebug("Last Sync Local %s ", syncItemLocal->lastSyncDate.toString().latin1());

  //Step 2. Find syncinfo in remote file and create if not existent.
  found = syncRemote->findSyncData(mCurrentSyncName, &index);
  if (found == false)
    {
      qDebug("FULLDATE 1");
      fullDateRange = true;
      PwMSyncItem newSyncItemRemote;
      newSyncItemRemote.syncName = mCurrentSyncName.latin1();
      newSyncItemRemote.lastSyncDate = mLastSync;
      syncRemote->addSyncDataEntry(&newSyncItemRemote, true);
      found = syncRemote->findSyncData(mCurrentSyncName, &index);
      if (found == false) {
	qDebug("PwMDoc::syncronize : newly created remote sync data could not be found");
	return e_syncError;
      }
    }
  
  syncItemRemote = syncRemote->getSyncDataEntry(index); 
  qDebug("Last Sync Remote %s ", syncItemRemote->lastSyncDate.toString().latin1());
  //and remove the found entry here. We will reenter it later again.
  //US  syncRemote->delSyncDataEntry(index, true);


  if ( syncItemLocal->lastSyncDate == mLastSync ) {
    qDebug("FULLDATE 2");
    fullDateRange = true;
  }
  
  if ( ! fullDateRange  ) {
        if ( syncItemLocal->lastSyncDate != syncItemRemote->lastSyncDate ) {
            
            fullDateRange = true;
            qDebug("FULLDATE 3 %s %s", syncItemLocal->lastSyncDate.toString().latin1() , syncItemRemote->lastSyncDate.toString().latin1() );
        } 
    }
    // fullDateRange = true; // debug only!
    if ( fullDateRange )
        mLastSync = QDateTime::currentDateTime().addDays( -100*365);
    else
        mLastSync = syncItemLocal->lastSyncDate;


    qDebug("*************************** ");
    qDebug("mLastSync %s ",mLastSync.toString().latin1() );
    QStringList er = syncRemote->getIDEntryList(); 
    PwMDataItem* inRemote ;//= er.first();
    PwMDataItem* inLocal;
    unsigned int catLocal, indexLocal;
    unsigned int catRemote, indexRemote;

    QString uid;
    manager->showProgressBar(0, i18n("Syncing - close to abort!"), er.count());

    int modulo = (er.count()/10)+1;
    unsigned int incCounter = 0;
    while ( incCounter < er.count()) {
	if (manager->isProgressBarCanceled())
            return e_syncError;
        if ( incCounter % modulo == 0 )
	  manager->showProgressBar(incCounter);

        uid = er[ incCounter ]; 
	qDebug("sync uid %s from remote file", uid.latin1());

        qApp->processEvents();

            inLocal = syncLocal->findEntryByID( uid, &catLocal, &indexLocal );
            inRemote = syncRemote->findEntryByID( uid, &catRemote, &indexRemote );
	    PWM_ASSERT(inRemote);
            if ( inLocal != 0 ) { // maybe conflict - same uid in both files
                if ( (take = takePwMDataItem( inLocal, inRemote, mLastSync, mode, fullDateRange) ) ) {
                    qDebug("take %d %s ", take, inLocal->desc.c_str());
                    if ( take == 3 )
                        return e_syncError;
                    if ( take == 1  ) {// take local
		        inRemote->syncItem(*inLocal, mSyncLauncher);
                        ++changedRemote;
                    } else { // take == 2  take remote
		        inLocal->syncItem(*inRemote, mSyncLauncher);
                        ++changedLocal;
                    }
                }
            } else { //  no conflict
                    if ( inRemote->meta.update > mLastSync || mode == 5 ) {
                        inRemote->meta.update = modifiedSync;

			//first check if we have a matching category in the local file
			const string* remotecat = syncRemote->getCategory(catRemote);
			syncLocal->addEntry(remotecat->c_str(), inRemote, true, false);

                        ++addedPasswordsLocal;
                    } else {
                        // pending checkExternSyncAddressee(addresseeRSyncSharp, inR);
		        syncRemote->delEntry(catRemote, indexRemote, true);
                        ++deletedPasswordsRemote; 
                    }
            }

        ++incCounter;
    }


    er.clear();
    QStringList el = syncLocal->getIDEntryList(); 
    modulo = (el.count()/10)+1;

    manager->showProgressBar(0, i18n("Add / remove addressees"), el.count());
    incCounter = 0;
    while ( incCounter < el.count()) {
        qApp->processEvents();
	if (manager->isProgressBarCanceled())
            return e_syncError;
        if ( incCounter % modulo == 0 )
	  manager->showProgressBar(incCounter);
        uid = el[ incCounter ]; 
	qDebug("sync uid %s from local file", uid.latin1());

        inLocal = syncLocal->findEntryByID( uid, &catLocal, &indexLocal );
        inRemote = syncRemote->findEntryByID( uid, &catRemote, &indexRemote );
	PWM_ASSERT(inLocal);

        if ( inRemote == 0 ) { 
                if ( inLocal->meta.update < mLastSync && mode != 4 ) {
                    // pending checkExternSyncAddressee(addresseeLSyncSharp, inL);
		    syncLocal->delEntry(catLocal, indexLocal, true);
                    ++deletedPasswordsLocal; 
                } else {
                    if ( ! manager->mWriteBackExistingOnly ) {
                        ++addedPasswordsRemote; 
                        inLocal->meta.update = modifiedSync;

			//first check if we have a matching category in the remote file
			const string* localcat = syncLocal->getCategory(catLocal);

                        PwMDataItem newEntry;
			newEntry = *inLocal; 
			inRemote = &newEntry;
	
                        //USsyncRemote->insertAddressee( inRemote, false );
			syncRemote->addEntry(localcat->c_str(), inRemote, true, false);

                    }
                }

        }
        ++incCounter;
    }
    el.clear();
    manager->hideProgressBar();

    // Now write the info back into the sync data space of the files

    mLastSync = QDateTime::currentDateTime().addSecs( 1 );
    // get rid of micro seconds
    QTime t =  mLastSync.time();
    mLastSync.setTime( QTime (t.hour (), t.minute (), t.second () )  );

   
    syncItemLocal->lastSyncDate = mLastSync;
    syncItemRemote->lastSyncDate = mLastSync;

    QString mes;
    mes .sprintf(  i18n("Synchronization summary:\n\n %d items added to local\n %d items added to remote\n %d items updated on local\n %d items updated on remote\n %d items deleted on local\n %d items deleted on remote\n"),addedPasswordsLocal, addedPasswordsRemote, changedLocal, changedRemote, deletedPasswordsLocal, deletedPasswordsRemote ); 
    if ( manager->mShowSyncSummary ) {
        KMessageBox::information(0, mes, i18n("PWM/Pi Synchronization") );
    }
    qDebug( mes );
  return e_success;
}


int PwMDoc::takePwMDataItem( PwMDataItem* local,  PwMDataItem* remote, QDateTime lastSync, int mode , bool full )
{
    // 0 equal
    // 1 take local
    // 2 take remote
    // 3 cancel 
    QDateTime localMod = local->meta.update;
    QDateTime remoteMod = remote->meta.update;

     if ( localMod == remoteMod )
       return 0;
        
    qDebug(" %d %d conflict on %s %s ", mode, full, local->desc.c_str(), remote->desc.c_str() );

    //qDebug("%s %d  %s %d", local->lastModified().toString().latin1() , localMod, remote->lastModified().toString().latin1(), remoteMod);  
    //qDebug("%d %d %d %d ", local->lastModified().time().second(), local->lastModified().time().msec(),  remote->lastModified().time().second(), remote->lastModified().time().msec() );
    //full = true; //debug only
    if ( full ) {
      bool equ = ( (*local) == (*remote) );
        if ( equ ) {
	  //qDebug("equal ");
            if ( mode < SYNC_PREF_FORCE_LOCAL )
                return 0;
              
        }//else //debug only
	  //qDebug("not equal %s %s ", local->desc.c_str(), remote->desc.c_str());
    }
    
    int result;
    bool localIsNew;
    //qDebug("%s -- %s mLastCalendarSync %s lastsync %s --- local %s remote %s ",local->summary().latin1(), remote->summary().latin1(),mLastCalendarSync.toString().latin1() ,lastSync.toString().latin1()  ,  local->lastModified().toString().latin1() , remote->lastModified().toString().latin1()  );

    if ( full && mode < SYNC_PREF_NEWEST )
        mode = SYNC_PREF_ASK;

    switch( mode ) {
    case SYNC_PREF_LOCAL:
        if ( lastSync > remoteMod )
            return 1;
        if ( lastSync > localMod )
            return 2;
        return 1;
        break;
    case SYNC_PREF_REMOTE:
        if ( lastSync > remoteMod )
            return 1;
        if ( lastSync > localMod )
            return 2;
        return 2;
        break;
    case SYNC_PREF_NEWEST:
        if ( localMod > remoteMod )
            return 1;
        else
            return 2;
        break;
    case SYNC_PREF_ASK: 
        //qDebug("lsy %s  ---  lo %s  --- re %s ", lastSync.toString().latin1(), localMod.toString().latin1(), remoteMod.toString().latin1()  );
        if ( lastSync > remoteMod )
            return 1;
        if ( lastSync > localMod )
            return 2;
        localIsNew = localMod >= remoteMod;
        //qDebug("conflict! ************************************** ");
        {
	  PwMDataItemChooser acd ( *local,*remote, localIsNew , 0/*this*/ );
            result = acd.executeD(localIsNew);
            return result;
        }
        break;
    case SYNC_PREF_FORCE_LOCAL:
        return 1;
        break;
    case SYNC_PREF_FORCE_REMOTE:
        return 2;
        break;
       
    default:
        // SYNC_PREF_TAKE_BOTH not implemented
        break;
    }
    return 0;
}

void PwMDoc::removeSyncInfo( QString syncProfile)
{
    bool res, found;
    unsigned int count, i;
    if ( syncProfile.isEmpty() ) {
      count = numSyncDataEntries();
      for (i = count; count > 0; count-- ) {
	res = delSyncDataEntry(i-1, false);
	if (res == false) {
	  qDebug("PwMDoc::removeSyncInfo: could not remove syncprofile");
	}
      }
    } else { 
      found = findSyncData(syncProfile, &count);
      if (found == true)
	{
	  res = delSyncDataEntry(count, false);
	  if (res == false) {
	    qDebug("PwMDoc::removeSyncInfo: could not remove %s", syncProfile.latin1());

	  }
	}	
    }
}


//this are the overwritten callbackmethods from the syncinterface
bool PwMDoc::sync(KSyncManager* manager, QString filename, int mode, QString resource)
{
  QString mCurrentSyncDevice = manager->getCurrentSyncDevice();

  //1) unlock local file first if necessary (ask for password)
  if (this->isDeepLocked()) {
    PwMerror ret = this->deepLock(false);
    if (ret != e_success)
      return false;
  }

  //2)  construct and open a new doc on the stack(automatic cleanup of remote file).
  PwMDoc syncTarget(this, "synctarget");
  PwMDoc* pSyncTarget = &syncTarget;


  PwMerror err = pSyncTarget->openDoc(&filename, 1 /*== open with all entries locked*/);

  if (err == e_alreadyOpen) {
    PwMDocList::listItem li;
    if (getOpenDocList()->find(filename.latin1(), &li))
      pSyncTarget = li.doc;
    else {
      qDebug("PwmDoc::sync: sync failed. Error %i while opening file %s",err, filename.latin1());
      return false;
    }
  }
  else if (err != e_success) {
    qDebug("PwmDoc::sync: sync failed. Error %i while opening file %s",err, filename.latin1());
    return false;
  }

  qDebug("PWM file loaded %s,sync mode %d",filename.latin1(), mode );


  //3) unlock remote file first if necessary (ask for password)
  if (pSyncTarget->isDeepLocked()) {
    PwMerror ret = pSyncTarget->deepLock(false);
    if (ret != e_success)
      return false;
  }

  
  err = syncronize(manager, this, pSyncTarget, mode );

  if (err == e_success) {
    if ( manager->mWriteBackFile )	{
      qDebug("Saving remote PWManager file");
      err = pSyncTarget->saveDoc(conf()->confGlobCompression());
      if (err != e_success) {
	qDebug("PwmDoc::sync: Sync failed. Error %i while storing file %s",err, filename.latin1());
	return false;
      }
    }

    flagDirty();
    return true;
  }
  else {
    return false;
  }
}

#endif


bool PwMDoc::findSyncData(const QString &syncname, unsigned int *index)
{
	vector<PwMSyncItem>::iterator i = dti.syncDta.begin(), 
	                              end = dti.syncDta.end();

	while (i != end) {
		if ((*i).syncName == syncname.latin1()) {
			if (index) {
				*index = i - dti.syncDta.begin();
			}
			return true;
		}
		++i;
	}
	return false;
};

/** add new syncdataentry */
PwMerror PwMDoc::addSyncDataEntry(PwMSyncItem *d, bool dontFlagDirty)
{
	PWM_ASSERT(d);

	if (isDeepLocked()) {
		PwMerror ret;
		ret = deepLock(false);
		if (ret != e_success)
			return e_lock;
	}
	unsigned int index;

	const QString tmp = d->syncName.c_str();
	bool exists = findSyncData(d->syncName.c_str(), &index);

	if (exists == true) {
		// DOH! We found this entry.
		return e_entryExists;
	}

	dti.syncDta.push_back(*d);

	if (!dontFlagDirty)
		flagDirty();
	return e_success;
}



/** delete syncdata entry */
bool PwMDoc::delSyncDataEntry(unsigned int index, bool dontFlagDirty)
{
	if (isDeepLocked())
		return false;
	if (index > dti.syncDta.size() - 1)
		return false;

	// delete entry
	dti.syncDta.erase(dti.syncDta.begin() + index);

	if (!dontFlagDirty)
		flagDirty();
	return true;
}


PwMDataItem* PwMDoc::findEntryByID(const QString &uid, unsigned int *category, unsigned int *index)
{
  vector<PwMCategoryItem>::iterator catcounter = dti.dta.begin(),
				  catend = dti.dta.end();

  vector<PwMDataItem>::iterator entrBegin, entrEnd, entrI;

  while (catcounter != catend) {
    entrBegin = catcounter->d.begin();
    entrEnd = catcounter->d.end();
    entrI = entrBegin;
    while (entrI != entrEnd) {
      if ((*entrI).meta.uniqueid == uid.latin1()) {
	if (category)
	  *category = catcounter - dti.dta.begin();
	if (index)
	  *index = entrI - entrBegin;

	return &(*entrI);
      }
      ++entrI;
    }
    ++catcounter;
  }

  return 0;
}

QStringList PwMDoc::getIDEntryList()
{
  QStringList results;

  vector<PwMCategoryItem>::iterator catcounter = dti.dta.begin(),
				  catend = dti.dta.end();

  vector<PwMDataItem>::iterator entrBegin, entrEnd, entrI;

  while (catcounter != catend) {
    entrBegin = catcounter->d.begin();
    entrEnd = catcounter->d.end();
    entrI = entrBegin;
    while (entrI != entrEnd) {
      results.append( (*entrI).meta.uniqueid.c_str() );
      ++entrI;
    }
    ++catcounter;
  }

  return results;
}





#ifndef PWM_EMBEDDED
#include "pwmdoc.moc"
#endif
