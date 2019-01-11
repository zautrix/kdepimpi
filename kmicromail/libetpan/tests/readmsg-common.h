#ifndef READMSG_COMMON_H

#define READMSG_COMMON_H

#include <libetpan/libetpan.h>

#define DEST_CHARSET "iso-8859-1"

enum {
  NO_ERROR,
  ERROR_FILE,
  ERROR_MEMORY,
  ERROR_INVAL,
  ERROR_FETCH,
};

int etpan_mime_is_text(struct mailmime * build_info);

int show_part_info(FILE * f,
    struct mailmime_single_fields * mime_fields,
    struct mailmime_content * content);

int etpan_fetch_message(mailmessage * msg_info,
    struct mailmime * mime_part,
    struct mailmime_single_fields * fields,
    char ** result, size_t * result_len);

struct mailimf_fields * fetch_fields(mailmessage * msg_info,
    struct mailmime * mime);

int fields_write(FILE * f, int * col,
    struct mailimf_fields * fields);

#endif
