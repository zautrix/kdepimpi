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
 * $Id: mailstream_low.c,v 1.1.1.1 2004/07/03 16:34:04 zautrix Exp $
 */

#include "mailstream_low.h"
#include <stdlib.h>

/* general functions */

mailstream_low * mailstream_low_new(void * data,
				    mailstream_low_driver * driver)
{
  mailstream_low * s;

  s = malloc(sizeof(* s));
  if (s == NULL)
    return NULL;

  s->data = data;
  s->driver = driver;

  return s;
}

int mailstream_low_close(mailstream_low * s)
{
  if (s == NULL)
    return -1;
  s->driver->mailstream_close(s);

  return 0;
}

int mailstream_low_get_fd(mailstream_low * s)
{
  if (s == NULL)
    return -1;
  return s->driver->mailstream_get_fd(s);
}

void mailstream_low_free(mailstream_low * s)
{
  s->driver->mailstream_free(s);
}

ssize_t mailstream_low_read(mailstream_low * s, void * buf, size_t count)
{
  if (s == NULL)
    return -1;
  return s->driver->mailstream_read(s, buf, count);
}

ssize_t mailstream_low_write(mailstream_low * s,
    const void * buf, size_t count)
{
  if (s == NULL)
    return -1;
  return s->driver->mailstream_write(s, buf, count);
}
