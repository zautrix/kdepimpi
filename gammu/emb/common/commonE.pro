######################################################################
# Automatically generated by qmake (1.07a) Fri Jul 30 22:13:34 2004
######################################################################

TEMPLATE = lib
DEPENDPATH += device \
              misc \
              phone \
              protocol \
              service \
              device/bluetoth \
              device/irda \
              device/serial \
              misc/coding \
              phone/alcatel \
              phone/at \
              phone/nokia \
              phone/obex \
              phone/symbian \
              protocol/alcatel \
              protocol/at \
              protocol/nokia \
              protocol/obex \
              protocol/symbian \
              service/backup \
              service/sms \
              phone/nokia/dct3 \
              phone/nokia/dct4
INCLUDEPATH += . \
               misc/coding \
               misc \
               device \
               phone/nokia/dct4 \
               phone/nokia/dct3 \
               phone/at \
               phone/alcatel \
               phone/obex \
               phone/symbian \
               protocol \
               protocol/nokia \
               protocol/at \
               protocol/alcatel \
               protocol/obex \
               protocol/symbian \
               device/serial \
               device/irda \
               device/bluetoth \
               service \
               service/sms \
               service/backup \
               phone/nokia \
               phone

# Input
HEADERS += config.h \
           gammu.h \
           gsmcomon.h \
           gsmstate.h \
           device/devfunc.h \
           misc/cfg.h \
           misc/misc.h \
           phone/pfunc.h \
           protocol/protocol.h \
           service/gsmcal.h \
           service/gsmcall.h \
           service/gsmdata.h \
           service/gsmlogo.h \
           service/gsmmisc.h \
           service/gsmnet.h \
           service/gsmpbk.h \
           service/gsmprof.h \
           service/gsmring.h \
           device/bluetoth/affix.h \
           device/bluetoth/bluetoth.h \
           device/bluetoth/bluez.h \
           device/irda/irda.h \
           device/irda/irda_unx.h \
           device/serial/ser_djg.h \
           device/serial/ser_unx.h \
           misc/coding/coding.h \
           misc/coding/md5.h \
           phone/alcatel/alcatel.h \
           phone/at/atgen.h \
           phone/nokia/ncommon.h \
           phone/nokia/nfunc.h \
           phone/nokia/nfuncold.h \
           phone/obex/obexgen.h \
           phone/symbian/mroutgen.h \
           protocol/alcatel/alcabus.h \
           protocol/at/at.h \
           protocol/nokia/fbus2.h \
           protocol/nokia/mbus2.h \
           protocol/nokia/phonet.h \
           protocol/obex/obex.h \
           protocol/symbian/mrouter.h \
           service/backup/backgen.h \
           service/backup/backics.h \
           service/backup/backldif.h \
           service/backup/backlmb.h \
           service/backup/backtext.h \
           service/backup/backvcf.h \
           service/backup/backvcs.h \
           service/backup/gsmback.h \
           service/sms/gsmems.h \
           service/sms/gsmmulti.h \
           service/sms/gsmsms.h \
           phone/nokia/dct3/dct3comm.h \
           phone/nokia/dct3/dct3func.h \
           phone/nokia/dct3/n6110.h \
           phone/nokia/dct3/n7110.h \
           phone/nokia/dct3/n9210.h \
           phone/nokia/dct4/dct4func.h \
           phone/nokia/dct4/n3320.h \
           phone/nokia/dct3/n0650.h \
           phone/nokia/dct4/n3650.h \
           phone/nokia/dct4/n6510.h
SOURCES +=gsmcomon.c \
gsmstate.c \
misc/misc.c \
misc/cfg.c \
misc/coding/coding.c \
misc/coding/md5.c \
service/sms/gsmsms.c \
service/sms/gsmems.c \
service/sms/gsmmulti.c \
service/gsmcal.c \
service/gsmdata.c \
service/gsmpbk.c \
service/gsmring.c \
service/gsmlogo.c \
service/gsmmisc.c \
service/gsmnet.c \
service/backup/gsmback.c \
service/backup/backldif.c \
service/backup/backlmb.c \
service/backup/backtext.c \
service/backup/backvcs.c \
service/backup/backvcf.c \
service/backup/backics.c \
device/bluetoth/affix.c \
device/bluetoth/bluez.c \
device/bluetoth/bluetoth.c \
device/serial/ser_unx.c \
device/serial/ser_djg.c \
device/irda/irda.c \
device/devfunc.c \
protocol/at/at.c \
protocol/alcatel/alcabus.c \
protocol/nokia/mbus2.c \
protocol/nokia/fbus2.c \
protocol/nokia/phonet.c \
protocol/obex/obex.c \
protocol/symbian/mrouter.c \
phone/pfunc.c \
phone/at/atgen.c \
phone/at/siemens.c \
phone/at/samsung.c \
phone/at/sonyeric.c \
phone/alcatel/alcatel.c \
phone/nokia/dct3/n6110.c \
phone/nokia/dct3/n7110.c \
phone/nokia/dct3/n9210.c \
phone/nokia/dct3/dct3func.c \
phone/nokia/dct4/n3320.c \
phone/nokia/dct4/n3650.c \
phone/nokia/dct4/n6510.c \
phone/nokia/dct3/n0650.c \
phone/nokia/dct4/dct4func.c \
phone/nokia/nauto.c \
phone/nokia/nfunc.c \
phone/nokia/nfuncold.c \
phone/obex/obexgen.c \
phone/symbian/mroutgen.c 

TARGET = microkammu
DESTDIR = $(QPEDIR)/lib
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
CONFIG			=  warn_off release console




      #     device/bluetoth/blue_w32.h \
       #    device/irda/irda_w32.h \
      #     device/serial/ser_w32.h \
 # device/bluetoth/blue_w32.c \