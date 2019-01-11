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
 * $Id: mailstorage.h,v 1.2 2004/09/01 10:23:51 zautrix Exp $
 */

#ifndef MAIL_STORAGE_H

#define MAIL_STORAGE_H

#include <libetpan/maildriver_types.h>
#include <libetpan/mailstorage_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* storage */

/*
  mailstorage_new

  This function creates an empty storage. This storage have to be initialized.
  The "driver" and "data" fields should be initialized.

  @param id  is the name of the storage. It can be NULL.
    The given parameter is no more needed when the creation is finished.
    The given string is duplicated.

  @return The mail storage is returned.
*/

struct mailstorage * mailstorage_new(char * sto_id);

void mailstorage_free(struct mailstorage * storage);

/*
  session will be initialized on success.
*/

int mailstorage_connect(struct mailstorage * storage);

void mailstorage_disconnect(struct mailstorage * storage);

int mailstorage_noop(struct mailstorage * storage);


/* folder */

struct mailfolder * mailfolder_new(struct mailstorage * fld_storage,
    char * fld_pathname, char * fld_virtual_name);

void mailfolder_free(struct mailfolder * folder);

int mailfolder_add_child(struct mailfolder * parent,
        struct mailfolder * child);

int mailfolder_detach_parent(struct mailfolder * folder);

int mailfolder_connect(struct mailfolder * folder);

void mailfolder_disconnect(struct mailfolder * folder);

#ifdef __cplusplus
}
#endif

#endif


