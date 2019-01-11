/*
 * libEtPan! -- a mail stuff library
 *
 * Copyright (C) 2001 - 2003 - DINH Viet Hoa
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
 * $Id: mime_message_driver.c,v 1.1.1.1 2004/07/03 16:34:16 zautrix Exp $
 */

#include "mime_message_driver.h"

#include "libetpan-config.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#include "mailmessage.h"
#include "mailmessage_tools.h"
#include "maildriver_tools.h"

static FILE * get_mime_tmp_file(mailmessage * msg,
    char * filename, size_t size)
{
  int fd;
  mode_t old_mask;
  FILE * f;
  
  if (msg->msg_data == NULL)
    return NULL;
  
  snprintf(filename, size, "%s/libetpan-mime-XXXXXX",
      (char *) msg->msg_data);
  
  old_mask = umask(0077);
  fd = mkstemp(filename);
  umask(old_mask);
  if (fd == -1)
    return NULL;
  
  f = fdopen(fd, "r+");
  if (f == NULL) {
    close(fd);
    unlink(filename);
  }
  
  return f;
}

int mime_message_set_tmpdir(mailmessage * msg, char * tmpdir)
{
  if (msg->msg_data != NULL)
    free(msg->msg_data);
  
  msg->msg_data = strdup(tmpdir);
  if (msg->msg_data == NULL)
    return MAIL_ERROR_MEMORY;
  
  return MAIL_NO_ERROR;
}

void mime_message_detach_mime(mailmessage * msg)
{
  msg->msg_mime = NULL;
}

mailmessage * mime_message_init(struct mailmime * mime)
{
  mailmessage * msg;
  int r;
  
  msg = mailmessage_new();
  if (msg == NULL)
    goto err;

  r = mailmessage_init(msg, NULL, mime_message_driver, 0, 0);
  if (r != MAIL_NO_ERROR)
    goto free;
  
  if (mime != NULL) {
    mailmime_free(msg->msg_mime);
    msg->msg_mime = mime;
  }
  
  return msg;
  
 free:
  mailmessage_free(msg);
 err:
  return NULL;
}

static int initialize(mailmessage * msg)
{
  struct mailmime * mime;
  int res;
  
  mime = mailmime_new_message_data(NULL);
  if (mime == NULL) {
    res = MAIL_ERROR_MEMORY;
    goto err;
  }
  
  msg->msg_mime = mime;
  
  return MAIL_NO_ERROR;
  
 err:
  return res;
}

static void uninitialize(mailmessage * msg)
{
  /* tmp dir name */
  if (msg->msg_data != NULL)
    free(msg->msg_data);
  
  if (msg->msg_mime != NULL)
    mailmime_free(msg->msg_mime);
  msg->msg_mime = NULL;
}

static void flush(mailmessage * msg)
{
  /* do nothing */
}

static void check(mailmessage * msg)
{
  /* do nothing */
}

static void fetch_result_free(mailmessage * msg_info, char * content)
{
  mmap_string_unref(content);
}

static int file_to_mmapstr(FILE * f,
    char ** result, size_t * result_len)
{
  int fd;
  char * data;
  struct stat buf;
  MMAPString * mmapstr;
  int res;
  int r;
  
  fd = fileno(f);
  if (fd == -1) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }

  fflush(f);
  r = fstat(fd, &buf);
  if (r == -1) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }
  
  data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data == MAP_FAILED) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }
  
  mmapstr = mmap_string_new_len(data, buf.st_size);
  if (mmapstr == NULL) {
    res = MAIL_ERROR_MEMORY;
    
    goto unmap;
  }
  
  munmap(data, buf.st_size);
  
  r = mmap_string_ref(mmapstr);
  if (r != 0) {
    res = MAIL_ERROR_MEMORY;
    
    goto err;
  }
  
  * result = mmapstr->str;
  * result_len = mmapstr->len;

  return MAIL_NO_ERROR;

 unmap:
  munmap(data, buf.st_size);
 err:
  return res;
}


static int file_body_to_mmapstr(FILE * f,
    char ** result, size_t * result_len)
{
  int fd;
  char * data;
  struct stat buf;
  MMAPString * mmapstr;
  size_t cur_token;
  int res;
  int r;
  
  fd = fileno(f);
  if (fd == -1) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }
  
  fflush(f);
  r = fstat(fd, &buf);
  if (r == -1) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }
  
  data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data == MAP_FAILED) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }

  cur_token = 0;
  
  /* skip header */
  
  while (1) {
    r = mailimf_ignore_field_parse(data,
        buf.st_size, &cur_token);
    if (r == MAILIMF_NO_ERROR) {
      /* do nothing */
    }
    else
      break;
  }
  
  r = mailimf_crlf_parse(data, buf.st_size, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = maildriver_imf_error_to_mail_error(r);
    goto unmap;
  }
  
  mmapstr = mmap_string_new_len(data + cur_token, buf.st_size - cur_token);
  if (mmapstr == NULL) {
    res = MAIL_ERROR_MEMORY;
    
    goto unmap;
  }
  
  munmap(data, buf.st_size);
  
  r = mmap_string_ref(mmapstr);
  if (r != 0) {
    res = MAIL_ERROR_MEMORY;
    
    goto err;
  }
  
  * result = mmapstr->str;
  * result_len = mmapstr->len;

  return MAIL_NO_ERROR;

 unmap:
  munmap(data, buf.st_size);
 err:
  return res;
}

static int file_body_body_to_mmapstr(FILE * f,
    char ** result, size_t * result_len)
{
  int fd;
  char * data;
  struct stat buf;
  MMAPString * mmapstr;
  size_t cur_token;
  int res;
  int r;
  
  fd = fileno(f);
  if (fd == -1) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }
  
  fflush(f);
  r = fstat(fd, &buf);
  if (r == -1) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }
  
  data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data == MAP_FAILED) {
    res = MAIL_ERROR_FILE;
    
    goto err;
  }

  cur_token = 0;
  
  /* skip header */
  
  /* MIME header */
  
  while (1) {
    r = mailimf_ignore_field_parse(data,
        buf.st_size, &cur_token);
    if (r == MAILIMF_NO_ERROR) {
      /* do nothing */
    }
    else
      break;
  }
  
  r = mailimf_crlf_parse(data, buf.st_size, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = maildriver_imf_error_to_mail_error(r);
    goto unmap;
  }
  
  /* headers */
  
  while (1) {
    r = mailimf_ignore_field_parse(data,
        buf.st_size, &cur_token);
    if (r == MAILIMF_NO_ERROR) {
      /* do nothing */
    }
    else
      break;
  }
  
  r = mailimf_crlf_parse(data, buf.st_size, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = maildriver_imf_error_to_mail_error(r);
    goto unmap;
  }
  
  mmapstr = mmap_string_new_len(data + cur_token, buf.st_size - cur_token);
  if (mmapstr == NULL) {
    res = MAIL_ERROR_MEMORY;
    
    goto unmap;
  }
  
  munmap(data, buf.st_size);
  
  r = mmap_string_ref(mmapstr);
  if (r != 0) {
    res = MAIL_ERROR_MEMORY;
    
    goto err;
  }
  
  * result = mmapstr->str;
  * result_len = mmapstr->len;

  return MAIL_NO_ERROR;

 unmap:
  munmap(data, buf.st_size);
 err:
  return res;
}



static int fetch_section(mailmessage * msg_info,
    struct mailmime * mime,
    char ** result, size_t * result_len)
{
  int r;
  FILE * f;
  int res;
  int col;
  char filename[PATH_MAX];

  if (msg_info->msg_mime == NULL)
    return MAIL_ERROR_INVAL;
  
  f = get_mime_tmp_file(msg_info, filename, sizeof(filename));
  if (f == NULL) {
    res = MAIL_ERROR_FILE;
    goto err;
  }
  
  col = 0;
  r = mailmime_write(f, &col, mime);
  if (r != MAILIMF_NO_ERROR) {
    res = maildriver_imf_error_to_mail_error(r);
    goto close;
  }
  
  if (mime->mm_parent == NULL)
    r = file_to_mmapstr(f, result, result_len);
  else
    r = file_body_to_mmapstr(f, result, result_len);
  
  if (r != MAIL_NO_ERROR) {
    res = r;
    goto close;
  }
  
  fclose(f);
  unlink(filename);
  
  return MAIL_NO_ERROR;

 close:
  fclose(f);
  unlink(filename);
 err:
  return res;
}


static int fetch_section_header(mailmessage * msg_info,
    struct mailmime * mime,
    char ** result, size_t * result_len)
{
  int r;
  FILE * f;
  int res;
  int col;
  char filename[PATH_MAX];

  if (msg_info->msg_mime == NULL)
    return MAIL_ERROR_INVAL;
  
  f = get_mime_tmp_file(msg_info, filename, sizeof(filename));
  if (f == NULL) {
    res = MAIL_ERROR_FILE;
    goto err;
  }
  
  col = 0;
  if (mime->mm_type == MAILMIME_MESSAGE) {
    if (mime->mm_data.mm_message.mm_fields != NULL) {
      r = mailimf_fields_write(f, &col, mime->mm_data.mm_message.mm_fields);
      if (r != MAILIMF_NO_ERROR) {
        res = maildriver_imf_error_to_mail_error(r);
        goto close;
      }
      mailimf_string_write(f, &col, "\r\n", 2);
    }
  }
  
  r = file_to_mmapstr(f, result, result_len);
  if (r != MAIL_NO_ERROR) {
    res = r;
    goto close;
  }
  
  fclose(f);
  unlink(filename);
  
  return MAIL_NO_ERROR;

 close:
  fclose(f);
  unlink(filename);
 err:
  return res;
}


static int fetch_section_mime(mailmessage * msg_info,
    struct mailmime * mime,
    char ** result, size_t * result_len)
{
  int r;
  FILE * f;
  int res;
  int col;
  char filename[PATH_MAX];

  if (msg_info->msg_mime == NULL)
    return MAIL_ERROR_INVAL;
  
  f = get_mime_tmp_file(msg_info, filename, sizeof(filename));
  if (f == NULL) {
    res = MAIL_ERROR_FILE;
    goto err;
  }
  
  col = 0;
  if (mime->mm_content_type != NULL) {
    r = mailmime_content_write(f, &col, mime->mm_content_type);
    if (r != MAILIMF_NO_ERROR) {
      res = maildriver_imf_error_to_mail_error(r);
      goto close;
    }
  }
  if (mime->mm_mime_fields != NULL) {
    r = mailmime_fields_write(f, &col, mime->mm_mime_fields);
    if (r != MAILIMF_NO_ERROR) {
      res = maildriver_imf_error_to_mail_error(r);
      goto close;
    }
  }
  mailimf_string_write(f, &col, "\r\n", 2);
  
  r = file_to_mmapstr(f, result, result_len);
  if (r != MAIL_NO_ERROR) {
    res = r;
    goto close;
  }
  
  fclose(f);
  unlink(filename);

  return MAIL_NO_ERROR;

 close:
  fclose(f);
  unlink(filename);
 err:
  return res;
}



static int fetch_section_body(mailmessage * msg_info,
    struct mailmime * mime,
    char ** result, size_t * result_len)
{
  int r;
  FILE * f;
  int res;
  int col;
  char filename[PATH_MAX];

  if (msg_info->msg_mime == NULL)
    return MAIL_ERROR_INVAL;
  
  f = get_mime_tmp_file(msg_info, filename, sizeof(filename));
  if (f == NULL) {
    res = MAIL_ERROR_FILE;
    goto err;
  }
  
  col = 0;
  if (mime->mm_mime_fields != NULL) {
    r = mailmime_write(f, &col, mime);
    if (r != MAILIMF_NO_ERROR) {
      res = maildriver_imf_error_to_mail_error(r);
      goto close;
    }
  }
  
  if (mime->mm_type == MAILMIME_MESSAGE)
    r = file_body_body_to_mmapstr(f, result, result_len);
  else
    r = file_body_to_mmapstr(f, result, result_len);
  
  if (r != MAIL_NO_ERROR) {
    res = r;
    goto close;
  }
  
  fclose(f);
  unlink(filename);
  
  return MAIL_NO_ERROR;
  
 close:
  fclose(f);
  unlink(filename);
 err:
  return res;
}


static int get_bodystructure(mailmessage * msg_info,
    struct mailmime ** result)
{
  if (msg_info->msg_mime == NULL)
    return MAIL_ERROR_INVAL;
  
  * result = msg_info->msg_mime;
  
  return MAIL_NO_ERROR;
}


static int fetch(mailmessage * msg_info,
    char ** result, size_t * result_len)
{
  return fetch_section(msg_info, msg_info->msg_mime, result, result_len);
}

static int fetch_header(mailmessage * msg_info,
    char ** result, size_t * result_len)
{
  return fetch_section_header(msg_info,
      msg_info->msg_mime, result, result_len);
}

static int fetch_body(mailmessage * msg_info,
    char ** result, size_t * result_len)
{
  return fetch_section_body(msg_info, msg_info->msg_mime, result, result_len);
}


static int fetch_size(mailmessage * msg_info,
    size_t * result)
{
  char * msg;
  int r;

  r = fetch(msg_info, &msg, result);
  if (r != MAIL_NO_ERROR) {
    return r;
  }
  
  fetch_result_free(msg_info, msg);

  return MAIL_NO_ERROR;
}


static mailmessage_driver local_mime_message_driver = {
  .msg_name = "mime",
  
  .msg_initialize = initialize,
  .msg_uninitialize = uninitialize,
  
  .msg_flush = flush,
  .msg_check = check,

  .msg_fetch_result_free = fetch_result_free,
  
  .msg_fetch = fetch,
  .msg_fetch_header = fetch_header,
  .msg_fetch_body = fetch_body,
  .msg_fetch_size = fetch_size,
  .msg_get_bodystructure = get_bodystructure,
  .msg_fetch_section = fetch_section,
  .msg_fetch_section_header = fetch_section_header,
  .msg_fetch_section_mime = fetch_section_mime,
  .msg_fetch_section_body = fetch_section_body,
  .msg_fetch_envelope = mailmessage_generic_fetch_envelope,

  .msg_get_flags = NULL,
};

mailmessage_driver * mime_message_driver = &local_mime_message_driver;
