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
 * $Id: mailmime.h,v 1.2 2004/09/01 10:23:53 zautrix Exp $
 */

#ifndef MAILMIME_H

#define MAILMIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libetpan/mailimf.h>
#include <libetpan/mailmime_types.h>
#include <libetpan/mailmime_types_helper.h>
#include <libetpan/mailmime_content.h>
#include <libetpan/mailmime_decode.h>
#include <libetpan/mailmime_disposition.h>
#include <libetpan/mailmime_write.h>

int mailmime_content_parse(const char * message, size_t length,
			   size_t * index,
			   struct mailmime_content ** result);

int mailmime_description_parse(const char * message, size_t length,
			       size_t * index,
			       char ** result);

int mailmime_encoding_parse(const char * message, size_t length,
			    size_t * index,
			    struct mailmime_mechanism ** result);

int
mailmime_field_parse(struct mailimf_optional_field * field,
		     struct mailmime_field ** result);

int mailmime_id_parse(const char * message, size_t length,
		      size_t * index, char ** result);

int
mailmime_fields_parse(struct mailimf_fields *
		      fields,
		      struct mailmime_fields **
		      result);

int mailmime_version_parse(const char * message, size_t length,
			   size_t * index,
			   uint32_t * result);

int
mailmime_extension_token_parse(const char * message, size_t length,
			       size_t * index, char ** result);

int mailmime_parameter_parse(const char * message, size_t length,
			     size_t * index,
			     struct mailmime_parameter ** result);

int mailmime_value_parse(const char * message, size_t length,
			 size_t * index, char ** result);

int mailmime_language_parse(const char * message, size_t length,
			    size_t * index,
			    struct mailmime_language ** result);

#ifdef __cplusplus
}
#endif

#endif
