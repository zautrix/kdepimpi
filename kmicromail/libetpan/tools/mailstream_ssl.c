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
 * $Id: mailstream_ssl.c,v 1.1.1.1 2004/07/03 16:34:04 zautrix Exp $
 */

/*
  NOTE :

  The user has to call himself SSL_library_init() if he wants to
  use SSL.
*/

#include "mailstream_ssl.h"
#include <unistd.h>
#include <fcntl.h>

#ifndef CONFIG_H
#define CONFIG_H
#include "config.h"
#endif

/*
  these 3 headers MUST be included before <sys/select.h>
  to insure compatibility with Mac OS X (this is true for 10.2)
*/
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

/* mailstream_low, ssl */

#ifdef USE_SSL
#include <openssl/ssl.h>
#include <pthread.h>
#endif

#ifdef USE_SSL
struct mailstream_ssl_data {
  int fd;
  SSL * ssl_conn;
  SSL_CTX * ssl_ctx;
};
#endif

#ifdef USE_SSL
static pthread_mutex_t ssl_lock = PTHREAD_MUTEX_INITIALIZER;
static int ssl_init_done = 0;
#endif

#ifdef USE_SSL
static int mailstream_low_ssl_close(mailstream_low * s);
static ssize_t mailstream_low_ssl_read(mailstream_low * s,
				       void * buf, size_t count);
static ssize_t mailstream_low_ssl_write(mailstream_low * s,
					const void * buf, size_t count);
static void mailstream_low_ssl_free(mailstream_low * s);
static int mailstream_low_ssl_get_fd(mailstream_low * s);

static mailstream_low_driver local_mailstream_ssl_driver = {
  mailstream_read: mailstream_low_ssl_read,
  mailstream_write: mailstream_low_ssl_write,
  mailstream_close: mailstream_low_ssl_close,
  mailstream_free: mailstream_low_ssl_free,
  mailstream_get_fd: mailstream_low_ssl_get_fd,
};

mailstream_low_driver * mailstream_ssl_driver = &local_mailstream_ssl_driver;
#endif

/* file descriptor must be given in (default) blocking-mode */

#ifdef USE_SSL
static struct mailstream_ssl_data * ssl_data_new(int fd)
{
  struct mailstream_ssl_data * ssl_data;
  SSL * ssl_conn;
  int r;
  SSL_CTX * tmp_ctx;
  int fd_flags;
  int old_fd_flags;
  
  pthread_mutex_lock(&ssl_lock);
  if (!ssl_init_done) {
    SSL_library_init();
    ssl_init_done = 1;
  }
  pthread_mutex_unlock(&ssl_lock);
  
  tmp_ctx = SSL_CTX_new(TLSv1_client_method());
  if (tmp_ctx == NULL)
    goto err;
  
  ssl_conn = (SSL *) SSL_new(tmp_ctx);
  if (ssl_conn == NULL)
    goto free_ctx;
  
  if (SSL_set_fd(ssl_conn, fd) == 0)
    goto free_ssl_conn;
  
  SSL_set_read_ahead(ssl_conn, 1);
  
  r = SSL_connect(ssl_conn);
  if (r <= 0)
    goto free_ssl_conn;
  
  fd_flags = fcntl(fd, F_GETFL, 0);
  old_fd_flags = fd_flags;
  fd_flags |= O_NDELAY;
  r = fcntl(fd, F_SETFL, fd_flags);
  if (r < 0)
    goto free_ssl_conn;
  
  ssl_data = malloc(sizeof(* ssl_data));
  if (ssl_data == NULL)
    goto reset_fd_flags;
  
  ssl_data->fd = fd;
  ssl_data->ssl_conn = ssl_conn;
  ssl_data->ssl_ctx = tmp_ctx;

  return ssl_data;

 reset_fd_flags:
  fcntl(fd, F_SETFL, old_fd_flags);
 free_ctx:
  SSL_CTX_free(tmp_ctx);
 free_ssl_conn:
  SSL_free(ssl_conn);
 err:
  return NULL;
}

static void  ssl_data_free(struct mailstream_ssl_data * ssl_data)
{
  free(ssl_data);
}

static void  ssl_data_close(struct mailstream_ssl_data * ssl_data)
{
  SSL_free(ssl_data->ssl_conn);
  ssl_data->ssl_conn = NULL;
  SSL_CTX_free(ssl_data->ssl_ctx);
  ssl_data->ssl_ctx  = NULL;
  close(ssl_data->fd);
  ssl_data->fd = -1;
}
#endif

mailstream_low * mailstream_low_ssl_open(int fd)
{
#ifdef USE_SSL
  mailstream_low * s;
  struct mailstream_ssl_data * ssl_data;

  ssl_data = ssl_data_new(fd);
  if (ssl_data == NULL)
    goto err;

  s = mailstream_low_new(ssl_data, mailstream_ssl_driver);
  if (s == NULL)
    goto free_ssl_data;

  return s;

 free_ssl_data:
  ssl_data_free(ssl_data);
 err:
  return NULL;
#else
  return NULL;
#endif
}

#ifdef USE_SSL
static int mailstream_low_ssl_close(mailstream_low * s)
{
  struct mailstream_ssl_data * ssl_data;

  ssl_data = (struct mailstream_ssl_data *) s->data;
  ssl_data_close(ssl_data);

  return 0;
}

static void mailstream_low_ssl_free(mailstream_low * s)
{
  struct mailstream_ssl_data * ssl_data;

  ssl_data = (struct mailstream_ssl_data *) s->data;
  ssl_data_free(ssl_data);
  s->data = NULL;

  free(s);
}

static int mailstream_low_ssl_get_fd(mailstream_low * s)
{
  struct mailstream_ssl_data * ssl_data;

  ssl_data = (struct mailstream_ssl_data *) s->data;
  return ssl_data->fd;
}

static ssize_t mailstream_low_ssl_read(mailstream_low * s,
				       void * buf, size_t count)
{
  struct mailstream_ssl_data * ssl_data;
  int r;

  ssl_data = (struct mailstream_ssl_data *) s->data;
  
  while (1) {
    int ssl_r;
    fd_set fds_read;
    struct timeval timeout;
    
    r = SSL_read(ssl_data->ssl_conn, buf, count);
    if (r > 0)
      return r;
    
    ssl_r = SSL_get_error(ssl_data->ssl_conn, r);
    switch (ssl_r) {
    case SSL_ERROR_NONE:
      return r;
      
    case SSL_ERROR_ZERO_RETURN:
      return r;
      
    case SSL_ERROR_WANT_READ:
      timeout = mailstream_network_delay;
      
      FD_ZERO(&fds_read);
      FD_SET(ssl_data->fd, &fds_read);
      r = select(ssl_data->fd + 1, &fds_read, NULL, NULL, &timeout);
      if (r == 0)
        return -1;
      break;
      
    default:
      return r;
    }
  }
}

static ssize_t mailstream_low_ssl_write(mailstream_low * s,
					const void * buf, size_t count)
{
  struct mailstream_ssl_data * ssl_data;

  ssl_data = (struct mailstream_ssl_data *) s->data;
  return SSL_write(ssl_data->ssl_conn, buf, count);
}
#endif

/* mailstream */

mailstream * mailstream_ssl_open(int fd)
{
#ifdef USE_SSL
  mailstream_low * low;
  mailstream * s;

  low = mailstream_low_ssl_open(fd);
  if (low == NULL)
    goto err;

  s = mailstream_new(low, 8192);
  if (s == NULL)
    goto free_low;

  return s;

 free_low:
  mailstream_low_close(low);
 err:
  return NULL;
#else
  return NULL;
#endif
}

