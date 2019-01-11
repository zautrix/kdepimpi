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
 * $Id: charconv.c,v 1.2 2004/11/09 11:57:57 zautrix Exp $
 */

#include "charconv.h"

#include "config.h"
#ifdef HAVE_ICONV
#include <iconv.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "mmapstring.h"
#ifdef HAVE_ICONV

static size_t mail_iconv (iconv_t cd, const char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft,
    char **inrepls, char *outrepl)
{
  /*
    XXX - force conversion of (* inbuf) to (char *)
    because prototype of iconv() is the following :
    
    size_t iconv(iconv_t cd, char **restrict inbuf,
           size_t *restrict inbytesleft, char **restrict outbuf,
           size_t *restrict outbytesleft);
  */ 
  
  size_t ret = 0, ret1;
  char *ib = (char *) *inbuf;
  size_t ibl = *inbytesleft;
  char *ob = *outbuf;
  size_t obl = *outbytesleft;

  for (;;)
  {
    ret1 = iconv (cd, &ib, &ibl, &ob, &obl);
    if (ret1 != (size_t)-1)
      ret += ret1;
    if (ibl && obl && errno == EILSEQ)
    {
      if (inrepls)
      {
	/* Try replacing the input */
	char **t;
	for (t = inrepls; *t; t++)
	{
	  char *ib1 = *t;
	  size_t ibl1 = strlen (*t);
	  char *ob1 = ob;
	  size_t obl1 = obl;
	  iconv (cd, &ib1, &ibl1, &ob1, &obl1);
	  if (!ibl1)
	  {
	    ++ib, --ibl;
	    ob = ob1, obl = obl1;
	    ++ret;
	    break;
	  }
	}
	if (*t)
	  continue;
      }
      if (outrepl)
      {
	/* Try replacing the output */
	size_t n = strlen (outrepl);
	if (n <= obl)
	{
	  memcpy (ob, outrepl, n);
	  ++ib, --ibl;
	  ob += n, obl -= n;
	  ++ret;
	  continue;
	}
      }
    }
    *inbuf = ib, *inbytesleft = ibl;
    *outbuf = ob, *outbytesleft = obl;
    return ret;
  }
}
#endif

int charconv(const char * tocode, const char * fromcode,
    const char * str, size_t length,
    char ** result)
{
#ifndef HAVE_ICONV
  return MAIL_CHARCONV_ERROR_UNKNOWN_CHARSET;
#else
  iconv_t conv;
  size_t r;
  char * out;
  char * pout;
  size_t out_size;
  size_t old_out_size;
  size_t count;
  int res;

  conv = iconv_open(tocode, fromcode);
  if (conv == (iconv_t) -1) {
    res = MAIL_CHARCONV_ERROR_UNKNOWN_CHARSET;
    goto err;
  }

  out_size = 4 * length;

  out = malloc(out_size + 1);
  if (out == NULL) {
    res = MAIL_CHARCONV_ERROR_MEMORY;
    goto close_iconv;
  }

  pout = out;
  old_out_size = out_size;

  r = mail_iconv(conv, &str, &length, &pout, &out_size, NULL, "?");

  if (r == (size_t) -1) {
    res = MAIL_CHARCONV_ERROR_CONV;
    goto free;
  }

  iconv_close(conv);

  * pout = '\0';
  count = old_out_size - out_size;
  pout = realloc(out, count + 1);
  if (pout != NULL)
    out = pout;

  * result = out;

  return MAIL_CHARCONV_NO_ERROR;

 free:
  free(out);
 close_iconv:
  iconv_close(conv);
 err:
  return res;
#endif
};

int charconv_buffer(const char * tocode, const char * fromcode,
		    const char * str, size_t length,
		    char ** result, size_t * result_len)
{
#ifndef HAVE_ICONV
  return MAIL_CHARCONV_ERROR_UNKNOWN_CHARSET;
#else
  iconv_t conv;
  size_t iconv_r;
  int r;
  char * out;
  char * pout;
  size_t out_size;
  size_t old_out_size;
  size_t count;
  MMAPString * mmapstr;
  int res;

  conv = iconv_open(tocode, fromcode);
  if (conv == (iconv_t) -1) {
    res = MAIL_CHARCONV_ERROR_UNKNOWN_CHARSET;
    goto err;
  }

  out_size = 4 * length;

  mmapstr = mmap_string_sized_new(out_size + 1);
  if (mmapstr == NULL) {
    res = MAIL_CHARCONV_ERROR_MEMORY;
    goto err;
  }

  out = mmapstr->str;

  pout = out;
  old_out_size = out_size;

  iconv_r = mail_iconv(conv, &str, &length, &pout, &out_size, NULL, "?");

  if (iconv_r == (size_t) -1) {
    res = MAIL_CHARCONV_ERROR_CONV;
    goto free;
  }

  iconv_close(conv);

  * pout = '\0';

  count = old_out_size - out_size;

  r = mmap_string_ref(mmapstr);
  if (r < 0) {
    res = MAIL_CHARCONV_ERROR_MEMORY;
    goto free;
  }

  * result = out;
  * result_len = count;

  return MAIL_CHARCONV_NO_ERROR;

 free:
  mmap_string_free(mmapstr);
 err:
  return -1;
#endif
};

void charconv_buffer_free(char * str)
{
  mmap_string_unref(str);
}
