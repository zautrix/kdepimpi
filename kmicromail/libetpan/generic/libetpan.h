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
 * $Id: libetpan.h,v 1.1.1.1 2004/07/03 16:34:12 zautrix Exp $
 */

#ifndef LIBETPAN_H

#define LIBETPAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libetpan/libetpan_version.h>
#include <libetpan/maildriver.h>
#include <libetpan/mailmessage.h>
#include <libetpan/mailstorage.h>
#include <libetpan/mailthread.h>
#include <libetpan/mailsmtp.h>
#include <libetpan/charconv.h>

/* mbox driver */
#include <libetpan/mboxdriver.h>
#include <libetpan/mboxdriver_message.h>
#include <libetpan/mboxdriver_cached.h>
#include <libetpan/mboxdriver_cached_message.h>
#include <libetpan/mboxstorage.h>

/* MH driver */
#include <libetpan/mhdriver.h>
#include <libetpan/mhdriver_message.h>
#include <libetpan/mhdriver_cached.h>
#include <libetpan/mhdriver_cached_message.h>
#include <libetpan/mhstorage.h>

/* IMAP4rev1 driver */
#include <libetpan/imapdriver.h>
#include <libetpan/imapdriver_message.h>
#include <libetpan/imapdriver_cached.h>
#include <libetpan/imapdriver_cached_message.h>
#include <libetpan/imapstorage.h>

/* POP3 driver */
#include <libetpan/pop3driver.h>
#include <libetpan/pop3driver_message.h>
#include <libetpan/pop3driver_cached.h>
#include <libetpan/pop3driver_cached_message.h>
#include <libetpan/pop3storage.h>

/* NNTP driver */
#include <libetpan/nntpdriver.h>
#include <libetpan/nntpdriver_message.h>
#include <libetpan/nntpdriver_cached.h>
#include <libetpan/nntpdriver_cached_message.h>
#include <libetpan/nntpstorage.h>

/* maildir driver */
#include <libetpan/maildirdriver.h>
#include <libetpan/maildirdriver_message.h>
#include <libetpan/maildirdriver_cached.h>
#include <libetpan/maildirdriver_cached_message.h>
#include <libetpan/maildirstorage.h>

/* message which content is given by a MIME structure */
#include <libetpan/mime_message_driver.h>

/* message which content given by a string */
#include <libetpan/data_message_driver.h>

#ifdef __cplusplus
}
#endif

#endif
