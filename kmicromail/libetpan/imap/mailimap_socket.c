/*
 * libEtPan! -- a mail stuff library
 *
 * Copyright (C) 2001, 2002 - DINH Viet Hoa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the libEtPan! project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Id: mailimap_socket.c,v 1.1.1.1 2004/07/03 16:33:31 zautrix Exp $
 */

#include "mailimap_socket.h"

#include "mailimap.h"

#include "connect.h"
#include <netinet/in.h>
#include <unistd.h>

#define DEFAULT_IMAP_PORT 143
#define SERVICE_NAME_IMAP "imap2"
#define SERVICE_TYPE_TCP "tcp"

int mailimap_socket_connect(mailimap * f, const char * server, uint16_t port)
{
  int s;
  mailstream * stream;

  if (port == 0) {
    port = mail_get_service_port(SERVICE_NAME_IMAP, SERVICE_TYPE_TCP);
    if (port == 0)
      port = DEFAULT_IMAP_PORT;
    port = ntohs(port);
  }

  /* Connection */

  s = mail_tcp_connect(server, port);
  if (s == -1)
    return MAILIMAP_ERROR_CONNECTION_REFUSED;

  stream = mailstream_socket_open(s);
  if (stream == NULL) {
    close(s);
    return MAILIMAP_ERROR_MEMORY;
  }
  
  return mailimap_connect(f, stream);
}
