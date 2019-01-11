
include( ./variables.pri )
TEMPLATE = subdirs
unix:{ 
staticlib: {
SUBDIRS += kabc/plugins/file kabc/plugins/dir kabc/plugins/qtopia 
}
}
SUBDIRS += libical  libkcal libkdepim microkde kabc korganizer kaddressbook
#libkcal
unix:{ 
#SUBDIRS += gammu/emb/common gammu/emb/gammu 
}
!staticlib: {
SUBDIRS += kabc/plugins/file kabc/plugins/dir kabc/plugins/qtopia 
win32:{
#SUBDIRS += kabc/plugins/olaccess

}
}
unix:{ 
!staticlib: {
SUBDIRS += libetpan kmicromail/libmailwrapper kmicromail pwmanager/libcrypt/cipher  pwmanager/libcrypt/error  pwmanager/libcrypt/mpi  pwmanager/libcrypt/zlib pwmanager/pwmanager
}
}

