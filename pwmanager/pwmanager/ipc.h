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
 * $Id: ipc.h,v 1.3 2004/10/23 17:05:41 zautrix Exp $
 **************************************************************************/  

#ifndef __PWM_IPC_H
#define __PWM_IPC_H

#include <qobject.h>
#ifndef _WIN32_
#include <unistd.h>
#else
#include <io.h>
#endif
#ifndef PWM_EMBEDDED
#include <stdio.h>
#else
#include <qsocket.h>
#endif

class QSocketNotifier;

/** very simple interprocess communication class */
class Ipc : public QObject
{
	Q_OBJECT
public:
	/** create a new Ipc communication object */
	Ipc();
	/** create a new Ipc communication object and
	  * connect it to "ipc"
	  */
	Ipc(const Ipc *ipc);
	/** destructor */
	~Ipc();

	/** send data to the other socket end
	  * (To the connected ipc object)
	  */
#ifndef PWM_EMBEDDED
	void send(const char *buf, size_t size)
			{ write(sock[0], buf, size); }
#else
	void send(const char *buf, size_t size)
			{ qSock->writeBlock(buf, size); }
#endif

signals:
	/** a line is available */
	void lineAvailable(const char *buf, size_t size);

protected slots:
	/** received data on socket */
	void receiveData(int s);

protected:
#ifndef PWM_EMBEDDED
	/** stream on "this" end of the socket (sock[0]) */
	FILE *stream;
	/** current receive buffer size */
	size_t rdBufSize;
#else
	QSocket* qSock;
	/** current receive buffer size */
	unsigned int rdBufSize;
#endif

	/** full-duplex socket file desciptors */
	int sock[2];
	/** socket notifier */
	QSocketNotifier *notifier;
	/** are we the host or the client object? */
	bool host;
	/** receive buffer */
	char *rdBuf;
};

#endif // __PWM_IPC_H
