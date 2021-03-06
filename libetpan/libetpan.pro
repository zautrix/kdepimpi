######################################################################
# Automatically generated by qmake (1.07a) Fri Mar 18 19:27:39 2005
######################################################################

TEMPLATE = lib
TARGET		= microlibetpan
OBJECTS_DIR = obj
MOC_DIR = moc
DESTDIR=../bin
DEPENDPATH += include/libetpan \
              src/data-types \
              src/engine \
              src/main \
              src/driver/interface \
              src/driver/tools \
              src/low-level/imap \
              src/low-level/imf \
              src/low-level/maildir \
              src/low-level/mbox \
              src/low-level/mh \
              src/low-level/mime \
              src/low-level/nntp \
              src/low-level/pop3 \
              src/low-level/smtp \
              src/driver/implementation/data-message \
              src/driver/implementation/db \
              src/driver/implementation/hotmail \
              src/driver/implementation/imap \
              src/driver/implementation/maildir \
              src/driver/implementation/mbox \
              src/driver/implementation/mh \
              src/driver/implementation/mime-message \
              src/driver/implementation/nntp \
              src/driver/implementation/pop3
INCLUDEPATH += . \
               ./include \
               src/data-types \
               src/engine \
               include/libetpan \
               src/main \
               src/driver/interface \
               src/driver/tools \
               src/low-level/imap \
               src/low-level/imf \
               src/low-level/maildir \
               src/low-level/mbox \
               src/low-level/mh \
               src/low-level/mime \
               src/low-level/nntp \
               src/low-level/pop3 \
               src/low-level/smtp \
               src/driver/implementation/data-message \
               src/driver/implementation/db \
               src/driver/implementation/hotmail \
               src/driver/implementation/imap \
               src/driver/implementation/maildir \
               src/driver/implementation/mbox \
               src/driver/implementation/mh \
               src/driver/implementation/mime-message \
               src/driver/implementation/nntp \
               src/driver/implementation/pop3

# Input
HEADERS += config.h \
           libetpan-config.h \
           include/libetpan/carray.h \
           include/libetpan/charconv.h \
           include/libetpan/chash.h \
           include/libetpan/cinthash.h \
           include/libetpan/clist.h \
           include/libetpan/data_message_driver.h \
           include/libetpan/dbdriver.h \
           include/libetpan/dbdriver_message.h \
           include/libetpan/dbdriver_types.h \
           include/libetpan/dbstorage.h \
           include/libetpan/generic_cache_types.h \
           include/libetpan/hotmailstorage.h \
           include/libetpan/imapdriver.h \
           include/libetpan/imapdriver_cached.h \
           include/libetpan/imapdriver_cached_message.h \
           include/libetpan/imapdriver_message.h \
           include/libetpan/imapdriver_types.h \
           include/libetpan/imapstorage.h \
           include/libetpan/libetpan-config.h \
           include/libetpan/libetpan.h \
           include/libetpan/libetpan_version.h \
           include/libetpan/mail.h \
           include/libetpan/maildir.h \
           include/libetpan/maildir_types.h \
           include/libetpan/maildirdriver.h \
           include/libetpan/maildirdriver_cached.h \
           include/libetpan/maildirdriver_cached_message.h \
           include/libetpan/maildirdriver_message.h \
           include/libetpan/maildirdriver_types.h \
           include/libetpan/maildirstorage.h \
           include/libetpan/maildriver.h \
           include/libetpan/maildriver_errors.h \
           include/libetpan/maildriver_types.h \
           include/libetpan/maildriver_types_helper.h \
           include/libetpan/mailengine.h \
           include/libetpan/mailfolder.h \
           include/libetpan/mailimap.h \
           include/libetpan/mailimap_helper.h \
           include/libetpan/mailimap_socket.h \
           include/libetpan/mailimap_ssl.h \
           include/libetpan/mailimap_types.h \
           include/libetpan/mailimap_types_helper.h \
           include/libetpan/mailimf.h \
           include/libetpan/mailimf_types.h \
           include/libetpan/mailimf_types_helper.h \
           include/libetpan/mailimf_write_file.h \
           include/libetpan/mailimf_write_generic.h \
           include/libetpan/mailimf_write_mem.h \
           include/libetpan/mailmbox.h \
           include/libetpan/mailmbox_types.h \
           include/libetpan/mailmessage.h \
           include/libetpan/mailmessage_types.h \
           include/libetpan/mailmh.h \
           include/libetpan/mailmime.h \
           include/libetpan/mailmime_content.h \
           include/libetpan/mailmime_decode.h \
           include/libetpan/mailmime_disposition.h \
           include/libetpan/mailmime_types.h \
           include/libetpan/mailmime_types_helper.h \
           include/libetpan/mailmime_write_file.h \
           include/libetpan/mailmime_write_generic.h \
           include/libetpan/mailmime_write_mem.h \
           include/libetpan/mailpop3.h \
           include/libetpan/mailpop3_helper.h \
           include/libetpan/mailpop3_socket.h \
           include/libetpan/mailpop3_ssl.h \
           include/libetpan/mailpop3_types.h \
           include/libetpan/mailprivacy.h \
           include/libetpan/mailprivacy_gnupg.h \
           include/libetpan/mailprivacy_smime.h \
           include/libetpan/mailprivacy_tools.h \
           include/libetpan/mailprivacy_types.h \
           include/libetpan/mailsem.h \
           include/libetpan/mailsmtp.h \
           include/libetpan/mailsmtp_helper.h \
           include/libetpan/mailsmtp_socket.h \
           include/libetpan/mailsmtp_ssl.h \
           include/libetpan/mailsmtp_types.h \
           include/libetpan/mailstorage.h \
           include/libetpan/mailstorage_types.h \
           include/libetpan/mailstream.h \
           include/libetpan/mailstream_helper.h \
           include/libetpan/mailstream_low.h \
           include/libetpan/mailstream_socket.h \
           include/libetpan/mailstream_ssl.h \
           include/libetpan/mailstream_types.h \
           include/libetpan/mailthread.h \
           include/libetpan/mailthread_types.h \
           include/libetpan/mboxdriver.h \
           include/libetpan/mboxdriver_cached.h \
           include/libetpan/mboxdriver_cached_message.h \
           include/libetpan/mboxdriver_message.h \
           include/libetpan/mboxdriver_types.h \
           include/libetpan/mboxstorage.h \
           include/libetpan/mhdriver.h \
           include/libetpan/mhdriver_cached.h \
           include/libetpan/mhdriver_cached_message.h \
           include/libetpan/mhdriver_message.h \
           include/libetpan/mhdriver_types.h \
           include/libetpan/mhstorage.h \
           include/libetpan/mime_message_driver.h \
           include/libetpan/mmapstring.h \
           include/libetpan/newsnntp.h \
           include/libetpan/newsnntp_socket.h \
           include/libetpan/newsnntp_ssl.h \
           include/libetpan/newsnntp_types.h \
           include/libetpan/nntpdriver.h \
           include/libetpan/nntpdriver_cached.h \
           include/libetpan/nntpdriver_cached_message.h \
           include/libetpan/nntpdriver_message.h \
           include/libetpan/nntpdriver_types.h \
           include/libetpan/nntpstorage.h \
           include/libetpan/pop3driver.h \
           include/libetpan/pop3driver_cached.h \
           include/libetpan/pop3driver_cached_message.h \
           include/libetpan/pop3driver_message.h \
           include/libetpan/pop3driver_types.h \
           include/libetpan/pop3storage.h \
           src/data-types/base64.h \
           src/data-types/carray.h \
           src/data-types/charconv.h \
           src/data-types/chash.h \
           src/data-types/cinthash.h \
           src/data-types/clist.h \
           src/data-types/connect.h \
           src/data-types/hmac-md5.h \
           src/data-types/mail.h \
           src/data-types/mail_cache_db.h \
           src/data-types/mail_cache_db_types.h \
           src/data-types/maillock.h \
           src/data-types/mailsem.h \
           src/data-types/mailstream.h \
           src/data-types/mailstream_helper.h \
           src/data-types/mailstream_low.h \
           src/data-types/mailstream_socket.h \
           src/data-types/mailstream_ssl.h \
           src/data-types/mailstream_types.h \
           src/data-types/mapping.h \
           src/data-types/md5.h \
           src/data-types/md5global.h \
           src/data-types/mmapstring.h \
           src/engine/mailengine.h \
           src/engine/mailprivacy.h \
           src/engine/mailprivacy_gnupg.h \
           src/engine/mailprivacy_smime.h \
           src/engine/mailprivacy_tools.h \
           src/engine/mailprivacy_types.h \
           src/main/libetpan.h \
           src/main/libetpan_version.h \
           src/driver/interface/maildriver.h \
           src/driver/interface/maildriver_errors.h \
           src/driver/interface/maildriver_tools.h \
           src/driver/interface/maildriver_types.h \
           src/driver/interface/maildriver_types_helper.h \
           src/driver/interface/mailfolder.h \
           src/driver/interface/mailmessage.h \
           src/driver/interface/mailmessage_tools.h \
           src/driver/interface/mailmessage_types.h \
           src/driver/interface/mailstorage.h \
           src/driver/interface/mailstorage_tools.h \
           src/driver/interface/mailstorage_types.h \
           src/driver/tools/generic_cache.h \
           src/driver/tools/generic_cache_types.h \
           src/driver/tools/imfcache.h \
           src/driver/tools/mailthread.h \
           src/driver/tools/mailthread_types.h \
           src/low-level/imap/mailimap.h \
           src/low-level/imap/mailimap_helper.h \
           src/low-level/imap/mailimap_keywords.h \
           src/low-level/imap/mailimap_parser.h \
           src/low-level/imap/mailimap_print.h \
           src/low-level/imap/mailimap_sender.h \
           src/low-level/imap/mailimap_socket.h \
           src/low-level/imap/mailimap_ssl.h \
           src/low-level/imap/mailimap_types.h \
           src/low-level/imap/mailimap_types_helper.h \
           src/low-level/imf/mailimf.h \
           src/low-level/imf/mailimf_types.h \
           src/low-level/imf/mailimf_types_helper.h \
           src/low-level/imf/mailimf_write.h \
           src/low-level/imf/mailimf_write_file.h \
           src/low-level/imf/mailimf_write_generic.h \
           src/low-level/imf/mailimf_write_mem.h \
           src/low-level/maildir/maildir.h \
           src/low-level/maildir/maildir_types.h \
           src/low-level/mbox/mailmbox.h \
           src/low-level/mbox/mailmbox_parse.h \
           src/low-level/mbox/mailmbox_types.h \
           src/low-level/mh/mailmh.h \
           src/low-level/mime/mailmime.h \
           src/low-level/mime/mailmime_content.h \
           src/low-level/mime/mailmime_decode.h \
           src/low-level/mime/mailmime_disposition.h \
           src/low-level/mime/mailmime_types.h \
           src/low-level/mime/mailmime_types_helper.h \
           src/low-level/mime/mailmime_write.h \
           src/low-level/mime/mailmime_write_file.h \
           src/low-level/mime/mailmime_write_generic.h \
           src/low-level/mime/mailmime_write_mem.h \
           src/low-level/nntp/newsnntp.h \
           src/low-level/nntp/newsnntp_socket.h \
           src/low-level/nntp/newsnntp_ssl.h \
           src/low-level/nntp/newsnntp_types.h \
           src/low-level/pop3/mailpop3.h \
           src/low-level/pop3/mailpop3_helper.h \
           src/low-level/pop3/mailpop3_socket.h \
           src/low-level/pop3/mailpop3_ssl.h \
           src/low-level/pop3/mailpop3_types.h \
           src/low-level/smtp/mailsmtp.h \
           src/low-level/smtp/mailsmtp_helper.h \
           src/low-level/smtp/mailsmtp_socket.h \
           src/low-level/smtp/mailsmtp_ssl.h \
           src/low-level/smtp/mailsmtp_types.h \
           src/driver/implementation/data-message/data_message_driver.h \
           src/driver/implementation/db/dbdriver.h \
           src/driver/implementation/db/dbdriver_message.h \
           src/driver/implementation/db/dbdriver_types.h \
           src/driver/implementation/db/dbstorage.h \
           src/driver/implementation/hotmail/hotmailstorage.h \
           src/driver/implementation/imap/imapdriver.h \
           src/driver/implementation/imap/imapdriver_cached.h \
           src/driver/implementation/imap/imapdriver_cached_message.h \
           src/driver/implementation/imap/imapdriver_message.h \
           src/driver/implementation/imap/imapdriver_tools.h \
           src/driver/implementation/imap/imapdriver_types.h \
           src/driver/implementation/imap/imapstorage.h \
           src/driver/implementation/maildir/maildirdriver.h \
           src/driver/implementation/maildir/maildirdriver_cached.h \
           src/driver/implementation/maildir/maildirdriver_cached_message.h \
           src/driver/implementation/maildir/maildirdriver_message.h \
           src/driver/implementation/maildir/maildirdriver_tools.h \
           src/driver/implementation/maildir/maildirdriver_types.h \
           src/driver/implementation/maildir/maildirstorage.h \
           src/driver/implementation/mbox/mboxdriver.h \
           src/driver/implementation/mbox/mboxdriver_cached.h \
           src/driver/implementation/mbox/mboxdriver_cached_message.h \
           src/driver/implementation/mbox/mboxdriver_message.h \
           src/driver/implementation/mbox/mboxdriver_tools.h \
           src/driver/implementation/mbox/mboxdriver_types.h \
           src/driver/implementation/mbox/mboxstorage.h \
           src/driver/implementation/mh/mhdriver.h \
           src/driver/implementation/mh/mhdriver_cached.h \
           src/driver/implementation/mh/mhdriver_cached_message.h \
           src/driver/implementation/mh/mhdriver_message.h \
           src/driver/implementation/mh/mhdriver_tools.h \
           src/driver/implementation/mh/mhdriver_types.h \
           src/driver/implementation/mh/mhstorage.h \
           src/driver/implementation/mime-message/mime_message_driver.h \
           src/driver/implementation/nntp/nntpdriver.h \
           src/driver/implementation/nntp/nntpdriver_cached.h \
           src/driver/implementation/nntp/nntpdriver_cached_message.h \
           src/driver/implementation/nntp/nntpdriver_message.h \
           src/driver/implementation/nntp/nntpdriver_tools.h \
           src/driver/implementation/nntp/nntpdriver_types.h \
           src/driver/implementation/nntp/nntpstorage.h \
           src/driver/implementation/pop3/pop3driver.h \
           src/driver/implementation/pop3/pop3driver_cached.h \
           src/driver/implementation/pop3/pop3driver_cached_message.h \
           src/driver/implementation/pop3/pop3driver_message.h \
           src/driver/implementation/pop3/pop3driver_tools.h \
           src/driver/implementation/pop3/pop3driver_types.h \
           src/driver/implementation/pop3/pop3storage.h
SOURCES += src/data-types/base64.c \
           src/data-types/carray.c \
           src/data-types/charconv.c \
           src/data-types/chash.c \
           src/data-types/cinthash.c \
           src/data-types/clist.c \
           src/data-types/connect.c \
           src/data-types/mail_cache_db.c \
           src/data-types/maillock.c \
           src/data-types/mailsem.c \
           src/data-types/mailstream.c \
           src/data-types/mailstream_helper.c \
           src/data-types/mailstream_low.c \
           src/data-types/mailstream_socket.c \
           src/data-types/mailstream_ssl.c \
           src/data-types/mapping.c \
           src/data-types/md5.c \
           src/data-types/mmapstring.c \
           src/engine/mailengine.c \
           src/engine/mailprivacy.c \
           src/engine/mailprivacy_gnupg.c \
           src/engine/mailprivacy_smime.c \
           src/engine/mailprivacy_tools.c \
           src/main/libetpan_version.c \
           src/driver/interface/maildriver.c \
           src/driver/interface/maildriver_tools.c \
           src/driver/interface/maildriver_types.c \
           src/driver/interface/maildriver_types_helper.c \
           src/driver/interface/mailfolder.c \
           src/driver/interface/mailmessage.c \
           src/driver/interface/mailmessage_tools.c \
           src/driver/interface/mailmessage_types.c \
           src/driver/interface/mailstorage.c \
           src/driver/interface/mailstorage_tools.c \
           src/driver/tools/generic_cache.c \
           src/driver/tools/imfcache.c \
           src/driver/tools/mailthread.c \
           src/driver/tools/mailthread_types.c \
           src/low-level/imap/mailimap.c \
           src/low-level/imap/mailimap_helper.c \
           src/low-level/imap/mailimap_keywords.c \
           src/low-level/imap/mailimap_parser.c \
           src/low-level/imap/mailimap_print.c \
           src/low-level/imap/mailimap_sender.c \
           src/low-level/imap/mailimap_socket.c \
           src/low-level/imap/mailimap_ssl.c \
           src/low-level/imap/mailimap_types.c \
           src/low-level/imap/mailimap_types_helper.c \
           src/low-level/imf/mailimf.c \
           src/low-level/imf/mailimf_types.c \
           src/low-level/imf/mailimf_types_helper.c \
           src/low-level/imf/mailimf_write.c \
           src/low-level/imf/mailimf_write_file.c \
           src/low-level/imf/mailimf_write_generic.c \
           src/low-level/imf/mailimf_write_mem.c \
           src/low-level/maildir/maildir.c \
           src/low-level/mbox/mailmbox.c \
           src/low-level/mbox/mailmbox_parse.c \
           src/low-level/mbox/mailmbox_types.c \
           src/low-level/mh/mailmh.c \
           src/low-level/mime/mailmime.c \
           src/low-level/mime/mailmime_content.c \
           src/low-level/mime/mailmime_decode.c \
           src/low-level/mime/mailmime_disposition.c \
           src/low-level/mime/mailmime_types.c \
           src/low-level/mime/mailmime_types_helper.c \
           src/low-level/mime/mailmime_write.c \
           src/low-level/mime/mailmime_write_file.c \
           src/low-level/mime/mailmime_write_generic.c \
           src/low-level/mime/mailmime_write_mem.c \
           src/low-level/nntp/newsnntp.c \
           src/low-level/nntp/newsnntp_socket.c \
           src/low-level/nntp/newsnntp_ssl.c \
           src/low-level/pop3/mailpop3.c \
           src/low-level/pop3/mailpop3_helper.c \
           src/low-level/pop3/mailpop3_socket.c \
           src/low-level/pop3/mailpop3_ssl.c \
           src/low-level/smtp/mailsmtp.c \
           src/low-level/smtp/mailsmtp_helper.c \
           src/low-level/smtp/mailsmtp_socket.c \
           src/low-level/smtp/mailsmtp_ssl.c \
           src/driver/implementation/data-message/data_message_driver.c \
           src/driver/implementation/db/dbdriver.c \
           src/driver/implementation/db/dbdriver_message.c \
           src/driver/implementation/db/dbstorage.c \
           src/driver/implementation/hotmail/hotmailstorage.c \
           src/driver/implementation/imap/imapdriver.c \
           src/driver/implementation/imap/imapdriver_cached.c \
           src/driver/implementation/imap/imapdriver_cached_message.c \
           src/driver/implementation/imap/imapdriver_message.c \
           src/driver/implementation/imap/imapdriver_tools.c \
           src/driver/implementation/imap/imapstorage.c \
           src/driver/implementation/maildir/maildirdriver.c \
           src/driver/implementation/maildir/maildirdriver_cached.c \
           src/driver/implementation/maildir/maildirdriver_cached_message.c \
           src/driver/implementation/maildir/maildirdriver_message.c \
           src/driver/implementation/maildir/maildirdriver_tools.c \
           src/driver/implementation/maildir/maildirstorage.c \
           src/driver/implementation/mbox/mboxdriver.c \
           src/driver/implementation/mbox/mboxdriver_cached.c \
           src/driver/implementation/mbox/mboxdriver_cached_message.c \
           src/driver/implementation/mbox/mboxdriver_message.c \
           src/driver/implementation/mbox/mboxdriver_tools.c \
           src/driver/implementation/mbox/mboxstorage.c \
           src/driver/implementation/mh/mhdriver.c \
           src/driver/implementation/mh/mhdriver_cached.c \
           src/driver/implementation/mh/mhdriver_cached_message.c \
           src/driver/implementation/mh/mhdriver_message.c \
           src/driver/implementation/mh/mhdriver_tools.c \
           src/driver/implementation/mh/mhstorage.c \
           src/driver/implementation/mime-message/mime_message_driver.c \
           src/driver/implementation/nntp/nntpdriver.c \
           src/driver/implementation/nntp/nntpdriver_cached.c \
           src/driver/implementation/nntp/nntpdriver_cached_message.c \
           src/driver/implementation/nntp/nntpdriver_message.c \
           src/driver/implementation/nntp/nntpdriver_tools.c \
           src/driver/implementation/nntp/nntpstorage.c \
           src/driver/implementation/pop3/pop3driver.c \
           src/driver/implementation/pop3/pop3driver_cached.c \
           src/driver/implementation/pop3/pop3driver_cached_message.c \
           src/driver/implementation/pop3/pop3driver_message.c \
           src/driver/implementation/pop3/pop3driver_tools.c \
           src/driver/implementation/pop3/pop3storage.c
