#include "mailfolder.h"

#include "maildriver.h"

int mailfolder_noop(struct mailfolder * folder)
{
  return mailsession_noop(folder->fld_session);
}

int mailfolder_check(struct mailfolder * folder)
{
  return mailsession_check_folder(folder->fld_session);
}

int mailfolder_expunge(struct mailfolder * folder)
{
  return mailsession_expunge_folder(folder->fld_session);
}

int mailfolder_status(struct mailfolder * folder,
    uint32_t * result_messages, uint32_t * result_recent,
    uint32_t * result_unseen)
{
  return mailsession_status_folder(folder->fld_session,
      folder->fld_pathname, result_messages,
      result_recent, result_unseen);
}

int mailfolder_append_message(struct mailfolder * folder,
    char * message, size_t size)
{
  return mailsession_append_message(folder->fld_session, message, size);
}

int mailfolder_append_message_flags(struct mailfolder * folder,
    char * message, size_t size, struct mail_flags * flags)
{
  return mailsession_append_message_flags(folder->fld_session, message,
      size, flags);
}

int mailfolder_get_messages_list(struct mailfolder * folder,
    struct mailmessage_list ** result)
{
  int r;
  struct mailmessage_list * msg_list;
  unsigned int i;
  
  r = mailsession_get_messages_list(folder->fld_session, &msg_list);
  if (r != MAIL_NO_ERROR)
    return r;
  
  for(i = 0 ; i < carray_count(msg_list->msg_tab) ; i ++) {
    mailmessage * msg;
    
    msg = carray_get(msg_list->msg_tab, i);
    msg->msg_folder = folder;
  }
  
  * result = msg_list;
  
  return MAIL_NO_ERROR;
}

int mailfolder_get_envelopes_list(struct mailfolder * folder,
    struct mailmessage_list * result)
{
  return mailsession_get_envelopes_list(folder->fld_session, result);
}

int mailfolder_get_message(struct mailfolder * folder,
    uint32_t num, mailmessage ** result)
{
  mailmessage * msg;
  int r;
  
  r = mailsession_get_message(folder->fld_session, num, &msg);
  if (r != MAIL_NO_ERROR)
    return r;
  
  msg->msg_folder = folder;
  
  * result = msg;
  
  return MAIL_NO_ERROR;
}

int mailfolder_get_message_by_uid(struct mailfolder * folder,
    const char * uid, mailmessage ** result)
{
  mailmessage * msg;
  int r;
  
  r = mailsession_get_message_by_uid(folder->fld_session, uid, &msg);
  if (r != MAIL_NO_ERROR)
    return r;
  
  msg->msg_folder = folder;
  
  * result = msg;
  
  return MAIL_NO_ERROR;
}
