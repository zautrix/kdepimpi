/*
 * libEtPan! -- a mail stuff library
 *
 * Copyright (C) 2001, 2005 - DINH Viet Hoa
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Id: newsnntp_ssl.c,v 1.1.1.1 2005/03/18 20:17:26 zautrix Exp $
 */

#include "newsnntp_ssl.h"

#include "newsnntp.h"

#include "connect.h"
#include <netinet/in.h>
#include <unistd.h>

#define DEFAULT_NNTPS_PORT 563
#define SERVICE_NAME_NNTPS "nntps"
#define SERVICE_TYPE_TCP "tcp"

int newsnntp_ssl_connect(newsnntp * f, const char * server, uint16_t port)
{
  int s;
  mailstream * stream;

  if (port == 0) {
    port = mail_get_service_port(SERVICE_NAME_NNTPS, SERVICE_TYPE_TCP);
    if (port == 0)
      port = DEFAULT_NNTPS_PORT;
    port = ntohs(port);
  }

  /* Connection */

  s = mail_tcp_connect(server, port);
  if (s == -1)
    return NEWSNNTP_ERROR_CONNECTION_REFUSED;

  stream = mailstream_ssl_open(s);
  if (stream == NULL) {
    close(s);
    return NEWSNNTP_ERROR_CONNECTION_REFUSED;
  }

  return newsnntp_connect(f, stream);
}
