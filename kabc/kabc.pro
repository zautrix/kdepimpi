TEMPLATE	= lib
CONFIG += qt warn_on 
#release debug
DESTDIR=../bin

TARGET = microkabc

include( ../variables.pri )

INCLUDEPATH += . ./vcard/include ./vcard/include/generated ../microkde ../microkde/kdecore ../microkde/kio/kfile ../microkde/kio/kio ../libkdepim ../qtcompat ../microkde/kdeui ..

#LIBS += -lmicrokde -lldap
LIBS += -L$(QPEDIR)/lib
DEFINES += KAB_EMBEDDED  DESKTOP_VERSION
unix : {

OBJECTS_DIR = obj/unix
MOC_DIR = moc/unix
}
win32: {
DEFINES += _WIN32_
OBJECTS_DIR = obj/win
MOC_DIR = moc/win
}
INTERFACES = \


HEADERS = \
 resource.h \
 stdaddressbook.h \
 agent.h \
 geo.h \
 key.h \
 field.h \
 plugin.h \
 address.h \
 addresseelist.h \
 addresseeview.h \
formatfactory.h \
 formatplugin.h \
 phonenumber.h \
distributionlist.h \
distributionlistdialog.h \
distributionlisteditor.h \
vcardformatplugin.h \
formats/vcardformatplugin2.h \
 picture.h \
 secrecy.h \
 sound.h \
 addressbook.h \
 timezone.h \
 tmpaddressbook.h \
 addressee.h \
 addresseedialog.h \
 vcardconverter.h \
 vcard21parser.h \
 vcardformatimpl.h \
 plugins/file/resourcefile.h \
 plugins/file/resourcefileconfig.h \
 plugins/dir/resourcedir.h \
 plugins/dir/resourcedirconfig.h \
 vcardparser/vcardline.h \
 vcardparser/vcard.h \
 vcardparser/vcardtool.h \
 vcardparser/vcardparser.h \
vcard/include/VCardAdrParam.h \
vcard/include/VCardAdrValue.h \
vcard/include/VCardAgentParam.h \
vcard/include/VCardContentLine.h \
vcard/include/VCardDateParam.h \
vcard/include/VCardDateValue.h \
vcard/include/VCardEmailParam.h \
vcard/include/VCardGeoValue.h \
vcard/include/VCardGroup.h \
vcard/include/VCardImageParam.h \
vcard/include/VCardImageValue.h \
vcard/include/VCardLangValue.h \
vcard/include/VCardNValue.h \
vcard/include/VCardParam.h \
vcard/include/VCardPhoneNumberValue.h \
vcard/include/VCardSourceParam.h \
vcard/include/VCardTelParam.h \
vcard/include/VCardTextParam.h \
vcard/include/VCardTextValue.h \
vcard/include/VCardTextBinParam.h \
vcard/include/VCardURIValue.h \
vcard/include/VCardVCard.h \
vcard/include/VCardEntity.h \
vcard/include/VCardValue.h \
vcard/include/VCardSoundValue.h \
vcard/include/VCardAgentValue.h \
vcard/include/VCardTelValue.h \
vcard/include/VCardTextBinValue.h \
vcard/include/VCardOrgValue.h \
vcard/include/VCardUTCValue.h \
vcard/include/VCardClassValue.h \
vcard/include/VCardFloatValue.h \
vcard/include/VCardTextListValue.h \
vcard/include/generated/AdrParam-generated.h \
vcard/include/generated/AdrValue-generated.h \
vcard/include/generated/AgentParam-generated.h \
vcard/include/generated/ContentLine-generated.h \
vcard/include/generated/DateParam-generated.h \
vcard/include/generated/DateValue-generated.h \
vcard/include/generated/EmailParam-generated.h \
vcard/include/generated/GeoValue-generated.h \
vcard/include/generated/Group-generated.h \
vcard/include/generated/ImageParam-generated.h \
vcard/include/generated/ImageValue-generated.h \
vcard/include/generated/LangValue-generated.h \
vcard/include/generated/NValue-generated.h \
vcard/include/generated/Param-generated.h \
vcard/include/generated/PhoneNumberValue-generated.h \
vcard/include/generated/SourceParam-generated.h \
vcard/include/generated/TelParam-generated.h \
vcard/include/generated/TextParam-generated.h \
vcard/include/generated/TextNSParam-generated.h \
vcard/include/generated/TextValue-generated.h \
vcard/include/generated/TextBinParam-generated.h \
vcard/include/generated/URIValue-generated.h \
vcard/include/generated/VCard-generated.h \
vcard/include/generated/VCardEntity-generated.h \
vcard/include/generated/Value-generated.h \
vcard/include/generated/SoundValue-generated.h \
vcard/include/generated/AgentValue-generated.h \
vcard/include/generated/TelValue-generated.h \
vcard/include/generated/TextBinValue-generated.h \
vcard/include/generated/OrgValue-generated.h \
vcard/include/generated/UTCValue-generated.h \
vcard/include/generated/ClassValue-generated.h \
vcard/include/generated/FloatValue-generated.h \
vcard/include/generated/TextListValue-generated.h

 
# plugins/ldap/resourceldap.h \
# plugins/ldap/resourceldapconfig.h \
#formats/binary/binaryformat.h \
 
#vcard/include/VCardTextNSParam.h \
 
SOURCES = \
distributionlist.cpp \
distributionlistdialog.cpp \
distributionlisteditor.cpp \
vcardformatplugin.cpp \
formats/vcardformatplugin2.cpp \
formatfactory.cpp \
 resource.cpp \
 stdaddressbook.cpp \
 plugin.cpp \
 agent.cpp \
 geo.cpp \
 key.cpp \
 field.cpp \
 addresseeview.cpp \
 address.cpp \
 phonenumber.cpp \
 picture.cpp \
 secrecy.cpp \
 sound.cpp \
 addressbook.cpp \
 timezone.cpp \
 tmpaddressbook.cpp \
 addressee.cpp \
 addresseelist.cpp \
 addresseedialog.cpp \
 vcardconverter.cpp \
 vcard21parser.cpp \
 vcardformatimpl.cpp \
 plugins/file/resourcefile.cpp \
 plugins/file/resourcefileconfig.cpp \
 plugins/dir/resourcedir.cpp \
 plugins/dir/resourcedirconfig.cpp \
 vcardparser/vcardline.cpp \
 vcardparser/vcard.cpp \
 vcardparser/vcardtool.cpp \
 vcardparser/vcardparser.cpp \
vcard/AdrParam.cpp \
vcard/AdrValue.cpp \
vcard/AgentParam.cpp \
vcard/ContentLine.cpp \
vcard/DateParam.cpp \
vcard/DateValue.cpp \
vcard/EmailParam.cpp \
vcard/Entity.cpp \
vcard/Enum.cpp \
vcard/GeoValue.cpp \
vcard/ImageParam.cpp \
vcard/ImageValue.cpp \
vcard/LangValue.cpp \
vcard/NValue.cpp \
vcard/Param.cpp \
vcard/PhoneNumberValue.cpp \
vcard/RToken.cpp \
vcard/SourceParam.cpp \
vcard/TelParam.cpp \
vcard/TextParam.cpp \
vcard/TextValue.cpp \
vcard/TextBinParam.cpp \
vcard/URIValue.cpp \
vcard/VCardv.cpp \
vcard/VCardEntity.cpp \
vcard/Value.cpp \
vcard/SoundValue.cpp \
vcard/AgentValue.cpp \
vcard/TelValue.cpp \
vcard/TextBinValue.cpp \
vcard/OrgValue.cpp \
vcard/UTCValue.cpp \
vcard/ClassValue.cpp \
vcard/FloatValue.cpp \
vcard/TextListValue.cpp


# plugins/ldap/resourceldap.cpp \
# plugins/ldap/resourceldapconfig.cpp \

#formats/binary/binaryformat.cpp \
