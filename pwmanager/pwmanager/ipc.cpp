/***************************************************************************
 *                                                                         *
 *   copyright (C) 2004 by Michael Buesch                                  *
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
 * $Id: ipc.cpp,v 1.4 2008/10/02 20:37:12 zautrix Exp $
 **************************************************************************/  
#include <stdlib.h>
#include "ipc.h"
#include "pwmexception.h"

#include <qsocketnotifier.h>
#ifndef _WIN32_
#include <sys/socket.h>
#endif
#ifndef PWM_EMBEDDED
#include <sys/types.h>
#include <stdio.h>
#else
#include <qsocket.h>
#endif

#define END_OF_LINE		'\n'
#define INIT_LINEBUF_LEN	64 /* byte */

#ifndef PWM_EMBEDDED

Ipc::Ipc()
 : stream (0)
 , notifier (0)
 , rdBuf (0)
{
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock)) {
		throw PwMException(PwMException::EX_GENERIC,
				   "Ipc: socketpair() failed");
	}
	rdBufSize = INIT_LINEBUF_LEN;
	rdBuf = static_cast<char *>(malloc(rdBufSize));
	if (!rdBuf) {
		close(sock[0]);
		close(sock[1]);
		throw PwMException(PwMException::EX_GENERIC,
				   "Ipc: OOM");
	}
	stream = fdopen(sock[0], "r");
	if (!stream) {
		close(sock[0]);
		close(sock[1]);
		free(rdBuf);
		throw PwMException(PwMException::EX_GENERIC,
				   "Ipc: fdopen() failed");
	}

	notifier = new QSocketNotifier(sock[0], QSocketNotifier::Read);
	connect(notifier, SIGNAL(activated(int)),
		this, SLOT(receiveData(int)));
	host = true;
}
#else
Ipc::Ipc()
 : notifier (0)
 , rdBuf (0)
{
#ifndef _WIN32_
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock)) {
	  qDebug("Ipc: socketpair() failed");
	}
#endif
	QSocket* qsock = new QSocket();
	qsock->setSocket(sock[0]);

	rdBufSize = INIT_LINEBUF_LEN;
	rdBuf = (char *)(malloc(rdBufSize));
	if (!rdBuf) {
		close(sock[0]);
		close(sock[1]);
		qDebug("Ipc: OOM");
	}

	qsock = new QSocket();
	qsock->setSocket(sock[0]);

	/*US
	stream = fdopen(sock[0], "r");
	if (!stream) {
		close(sock[0]);
		close(sock[1]);
		free(rdBuf);
		qDebug("Ipc: fdopen() failed");
	}
	*/

	notifier = new QSocketNotifier(sock[0], QSocketNotifier::Read);
	connect(notifier, SIGNAL(activated(int)),
		this, SLOT(receiveData(int)));
	host = true;
}

#endif


#ifndef PWM_EMBEDDED

Ipc::Ipc(const Ipc *ipc)
 : stream (0)
 , notifier (0)
 , rdBuf (0)
{
	rdBufSize = INIT_LINEBUF_LEN;
	rdBuf = static_cast<char *>(malloc(rdBufSize));
	if (!rdBuf) {
		throw PwMException(PwMException::EX_GENERIC,
				   "Ipc: OOM");
	}
	sock[0] = ipc->sock[1];
	sock[1] = ipc->sock[0];
	stream = fdopen(sock[0], "r");
	if (!stream) {
		free(rdBuf);
		throw PwMException(PwMException::EX_GENERIC,
				   "Ipc: fdopen() failed");
	}

	notifier = new QSocketNotifier(sock[0], QSocketNotifier::Read);
	connect(notifier, SIGNAL(activated(int)),
		this, SLOT(receiveData(int)));
	host = false;
}

#else

Ipc::Ipc(const Ipc *ipc)
 : notifier (0)
 , rdBuf (0)
{
	rdBufSize = INIT_LINEBUF_LEN;
	rdBuf = (char *)(malloc(rdBufSize));
	if (!rdBuf) {
	  qDebug("Ipc: OOM");
	}
	sock[0] = ipc->sock[1];
	sock[1] = ipc->sock[0];

	qSock = new QSocket();
	qSock->setSocket(sock[0]);

	/*US
	stream = fdopen(sock[0], "r");
	if (!stream) {
		free(rdBuf);
		qDebug("Ipc: fdopen() failed");
	}
	*/

	notifier = new QSocketNotifier(sock[0], QSocketNotifier::Read);
	connect(notifier, SIGNAL(activated(int)),
		this, SLOT(receiveData(int)));
	host = false;
}

#endif

Ipc::~Ipc()
{
#ifdef PWM_EMBEDDED
  delete qSock;
#endif
	delete_ifnot_null(notifier);
	if (rdBuf)
		free(rdBuf);
#ifndef PWM_EMBEDDED
	if (stream)
		fclose(stream);
#endif
	if (host) {
		close(sock[0]);
		close(sock[1]);
	}

}

void Ipc::receiveData(int s)
{
	PWM_ASSERT(s == sock[0]);
	PARAM_UNUSED(s);
#ifndef PWM_EMBEDDED
	ssize_t rd;

	rd = ::getline(&rdBuf, &rdBufSize, stream);
	if (likely(rd > 0)) {
		emit lineAvailable(rdBuf, rd);
	}
#else
	int rd;
	rd = qSock->readLine(rdBuf, rdBufSize);
	if (rd > 0) {
		emit lineAvailable(rdBuf, rd);
	}
#endif
	qDebug("void Ipc::receiveData(int s) has to be implemented.");

}

#ifndef PWM_EMBEDDED
#include "ipc.moc"
#endif
