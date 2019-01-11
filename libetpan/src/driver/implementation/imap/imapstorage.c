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
 * $Id: imapstorage.c,v 1.1.1.1 2005/03/18 20:17:05 zautrix Exp $
 */

#include "imapstorage.h"

#include <stdlib.h>
#include <string.h>

#include "mail.h"
#include "imapdriver.h"
#include "imapdriver_cached.h"
#include "mailstorage_tools.h"
#include "maildriver.h"

/* imap storage */

#define IMAP_DEFAULT_PORT  143
#define IMAPS_DEFAULT_PORT 993

static int imap_mailstorage_connect(struct mailstorage * storage);
static int
imap_mailstorage_get_folder_session(struct mailstorage * storage,
    char * pathname, mailsession ** result);
static void imap_mailstorage_uninitialize(struct mailstorage * storage);

static mailstorage_driver imap_mailstorage_driver = {
  .sto_name               = "imap",
  .sto_connect            = imap_mailstorage_connect,
  .sto_get_folder_session = imap_mailstorage_get_folder_session,
  .sto_uninitialize       = imap_mailstorage_uninitialize,
};

int imap_mailstorage_init(struct mailstorage * storage,
    char * imap_servername, uint16_t imap_port,
    char * imap_command,
    int imap_connection_type, int imap_auth_type,
    char * imap_login, char * imap_password,
    int imap_cached, char * imap_cache_directory)
{
  struct imap_mailstorage * imap_storage;

  imap_storage = malloc(sizeof(* imap_storage));
  if (imap_storage == NULL)
    goto err;

  imap_storage->imap_servername = strdup(imap_servername);
  if (imap_storage->imap_servername == NULL)
    goto free;

  imap_storage->imap_connection_type = imap_connection_type;
  
  if (imap_port == 0) {
    switch (imap_connection_type) {
    case CONNECTION_TYPE_PLAIN:
    case CONNECTION_TYPE_TRY_STARTTLS:
    case CONNECTION_TYPE_STARTTLS:
    case CONNECTION_TYPE_COMMAND:
    case CONNECTION_TYPE_COMMAND_TRY_STARTTLS:
    case CONNECTION_TYPE_COMMAND_STARTTLS:
      imap_port = IMAP_DEFAULT_PORT;
      break;

    case CONNECTION_TYPE_TLS:
    case CONNECTION_TYPE_COMMAND_TLS:
      imap_port = IMAPS_DEFAULT_PORT;
      break;
    }
  }

  imap_storage->imap_port = imap_port;

  if (imap_command != NULL) {
    imap_storage->imap_command = strdup(imap_command);
    if (imap_storage->imap_command == NULL)
      goto free_servername;
  }
  else
    imap_storage->imap_command = NULL;
  
  imap_storage->imap_auth_type = imap_auth_type;
  
  if (imap_login != NULL) {
    imap_storage->imap_login = strdup(imap_login);
    if (imap_storage->imap_login == NULL)
      goto free_command;
  }
  else
    imap_storage->imap_login = NULL;

  if (imap_password != NULL) {
    imap_storage->imap_password = strdup(imap_password);
    if (imap_storage->imap_password == NULL)
      goto free_login;
  }
  else
    imap_storage->imap_password = NULL;

  imap_storage->imap_cached = imap_cached;

  if (imap_cached && (imap_cache_directory != NULL)) {
    imap_storage->imap_cache_directory = strdup(imap_cache_directory);
    if (imap_storage->imap_cache_directory == NULL)
      goto free_password;
  }
  else {
    imap_storage->imap_cached = FALSE;
    imap_storage->imap_cache_directory = NULL;
  }

  storage->sto_data = imap_storage;
  storage->sto_driver = &imap_mailstorage_driver;

  return MAIL_NO_ERROR;

 free_password:
  free(imap_storage->imap_password);
 free_login:
  free(imap_storage->imap_login);
 free_command:
  free(imap_storage->imap_command);
 free_servername:
  free(imap_storage->imap_servername);
 free:
  free(imap_storage);
 err:
  return MAIL_ERROR_MEMORY;
}

static void imap_mailstorage_uninitialize(struct mailstorage * storage)
{
  struct imap_mailstorage * imap_storage;

  imap_storage = storage->sto_data;

  if (imap_storage->imap_cache_directory != NULL)
    free(imap_storage->imap_cache_directory);
  if (imap_storage->imap_password != NULL)
    free(imap_storage->imap_password);
  if (imap_storage->imap_login != NULL)
    free(imap_storage->imap_login);
  if (imap_storage->imap_command != NULL)
    free(imap_storage->imap_command);
  free(imap_storage->imap_servername);
  free(imap_storage);
  
  storage->sto_data = NULL;
}

static int imap_connect(struct mailstorage * storage,
    mailsession ** result)
{
  struct imap_mailstorage * imap_storage;
  mailsession_driver * driver;
  int r;
  int res;
  mailsession * session;

  imap_storage = storage->sto_data;

  if (imap_storage->imap_cached)
    driver = imap_cached_session_driver;
  else
    driver = imap_session_driver;

  r = mailstorage_generic_connect(driver,
      imap_storage->imap_servername,
      imap_storage->imap_port,
      imap_storage->imap_command,
      imap_storage->imap_connection_type,
      IMAPDRIVER_CACHED_SET_CACHE_DIRECTORY,
      imap_storage->imap_cache_directory,
      0, NULL,
      &session);
  switch (r) {
  case MAIL_NO_ERROR_NON_AUTHENTICATED:
  case MAIL_NO_ERROR_AUTHENTICATED:
  case MAIL_NO_ERROR:
    break;
  default:
    res = r;
    goto err;
  }

  r = mailstorage_generic_auth(session, r,
      imap_storage->imap_connection_type,
      imap_storage->imap_login,
      imap_storage->imap_password);
  if (r != MAIL_NO_ERROR) {
    res = r;
    goto free;
  }

  * result = session;

  return MAIL_NO_ERROR;

 free:
  mailsession_free(session);
 err:
  return res;
}

static int imap_mailstorage_connect(struct mailstorage * storage)
{
  mailsession * session;
  int r;
  int res;

  r = imap_connect(storage, &session);
  if (r != MAIL_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailsession_select_folder(session, "INBOX");
  if (r != MAIL_NO_ERROR) {
    mailsession_logout(session);
    res = r;
    goto err;
  }

  storage->sto_session = session;
  storage->sto_driver = &imap_mailstorage_driver;

  return MAIL_NO_ERROR;

 err:
  return res;
}

static int
imap_mailstorage_get_folder_session(struct mailstorage * storage,
    char * pathname, mailsession ** result)
{
  mailsession * session;
  int r;
  int res;

  if (strcasecmp(pathname, "INBOX") == 0) {
    session = storage->sto_session;
  }
  else {
    r = imap_connect(storage, &session);
    if (r != MAIL_NO_ERROR) {
      res = r;
      goto err;
    }

    r = mailsession_select_folder(session, pathname);
    if (r != MAIL_NO_ERROR) {
      mailsession_logout(session);
      res = r;
      goto free;
    }
  }

  * result = session;
  
  return MAIL_NO_ERROR;

 free:
  mailsession_free(session);
 err:
  return res;
}
