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
 * $Id: mailstream.h,v 1.2 2004/09/02 08:57:17 zautrix Exp $
 */

#ifndef MAILSTREAM_H

#define MAILSTREAM_H
//dont forget to change the value in  <libetpan/mailstream.h> as well
#define DEFAULT_NETWORK_TIMEOUT 30
#include <sys/time.h>

#include <libetpan/mailstream_low.h>
#include <libetpan/mailstream_helper.h>
#include <libetpan/mailstream_socket.h>
#include <libetpan/mailstream_ssl.h>
#include <libetpan/mailstream_types.h>

#ifdef __cplusplus
extern "C" {
#endif

mailstream * mailstream_new(mailstream_low * low, size_t buffer_size);
ssize_t mailstream_write(mailstream * s, const void * buf, size_t count);
ssize_t mailstream_read(mailstream * s, void * buf, size_t count);
int mailstream_close(mailstream * s);
int mailstream_flush(mailstream * s);
ssize_t mailstream_feed_read_buffer(mailstream * s);
mailstream_low * mailstream_get_low(mailstream * s);
void mailstream_set_low(mailstream * s, mailstream_low * low);

#ifdef LIBETPAN_MAILSTREAM_DEBUG
extern int mailstream_debug;
#endif

#define LIBETPAN_MAILSTREAM_NETWORK_DELAY
extern struct timeval mailstream_network_delay;

#ifdef __cplusplus
}
#endif

#endif

