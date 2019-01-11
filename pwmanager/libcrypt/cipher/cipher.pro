######################################################################
# Automatically generated by qmake (1.07a) Tue Oct 19 14:06:16 2004
######################################################################

TEMPLATE = lib
INCLUDEPATH += . .. ../crypt ../error

CONFIG += staticlib
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR= ../$(PLATFORM)
TARGET		= kpmicrocipher

# Input
HEADERS += bithelp.h rand-internal.h random.h rmd.h
SOURCES += ac.c \
           arcfour.c \
           blowfish.c \
           cast5.c \
           cipher.c \
           crc.c \
           des.c \
           dsa.c \
           elgamal.c \
           md.c \
           md4.c \
           md5.c \
           primegen.c \
           pubkey.c \
           random.c \
           rfc2268.c \
           rijndael.c \
           rmd160.c \
           rndegd.c \
           rndlinux.c \
           rndunix.c \
           rsa.c \
           serpent.c \
           sha1.c \
           sha256.c \
           sha512.c \
           tiger.c \
           twofish.c

  #         rndw32.c \


HEADERS += ../crypt/ath.h \
           ../crypt/cipher.h \
           ../crypt/g10lib.h \
           ../crypt/gcrypt-module.h \
           ../crypt/gcrypt.h \
           ../crypt/mpi.h \
           ../crypt/secmem.h \
           ../crypt/stdmem.h \
           ../crypt/types.h
SOURCES += ../crypt/ath.c \
           ../crypt/global.c \
           ../crypt/misc.c \
           ../crypt/missing-string.c \
           ../crypt/module.c \
           ../crypt/secmem.c \
           ../crypt/sexp.c \
           ../crypt/stdmem.c




#LIBS += -L../$(PLATFORM)
#LIBS +=  -lgcrypt -lmpi -lerror