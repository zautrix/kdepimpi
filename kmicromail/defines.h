#ifndef DEFINE_CONSTANTS_H
#define DEFINE_CONSTANTS_H



#define USER_AGENT          "KOpieMail v1.9.11"

#define PIXMAP_IMAPFOLDER   SmallIcon ( "imapfolder" )
#define PIXMAP_POP3FOLDER   SmallIcon ( "pop3folder" )
#define PIXMAP_INBOXFOLDER  SmallIcon ( "inbox" )
#define PIXMAP_MBOXFOLDER   SmallIcon ( "mboxfolder" )
#define PIXMAP_OUTBOXFOLDER SmallIcon ( "outbox" )
#define PIXMAP_LOCALFOLDER  SmallIcon ( "localfolder" )
#define PIXMAP_OFFLINE      SmallIcon ( "notconnected" )

#define IMAP_PORT           "143"
#define IMAP_SSL_PORT       "993"
#define SMTP_PORT           "25"
#define SMTP_SSL_PORT       "465"
#define POP3_PORT           "110"
#define POP3_SSL_PORT       "995"
#define NNTP_PORT           "119"
#define NNTP_SSL_PORT       "563"

/* used for decoding imapfoldername */
#define UNDEFINED 64
#define MAXLINE  76
#define UTF16MASK       0x03FFUL
#define UTF16SHIFT      10
#define UTF16BASE       0x10000UL
#define UTF16HIGHSTART  0xD800UL
#define UTF16HIGHEND    0xDBFFUL
#define UTF16LOSTART    0xDC00UL
#define UTF16LOEND      0xDFFFUL

#endif
