TEMPLATE	= lib
CONFIG += qt warn_on 
TARGET = microkabc


INCLUDEPATH += . $(KDEPIMDIR) vcard/include vcard/include/generated $(KDEPIMDIR)/microkde $(KDEPIMDIR)/microkde/kdecore $(KDEPIMDIR)/microkde/kdeui $(KDEPIMDIR)/microkde/kio/kfile $(KDEPIMDIR)/microkde/kio/kio $(KDEPIMDIR)/libkdepim $(KDEPIMDIR)/qtcompat $(QPEDIR)/include
OBJECTS_DIR = obj/$(PLATFORM)
MOC_DIR = moc/$(PLATFORM)
DESTDIR = $(QPEDIR)/lib
LIBS += -lmicrokde 
LIBS += -lmicrokdepim 
#LIBS += -lldap
LIBS += -L$(QPEDIR)/lib
DEFINES += KAB_EMBEDDED

#enable the following line if you want to get debugoutput while loading adresses
#DEFINES += VCARD_DEBUG 

INTERFACES = \

HEADERS = \
 address.h \
 addressbook.h \
 addressee.h \
 addresseedialog.h \
 addresseelist.h \
 addresseeview.h \
 agent.h \
 distributionlist.h \
 distributionlistdialog.h \
 distributionlisteditor.h \
 field.h \
 formatfactory.h \
 formatplugin.h \
 geo.h \
 key.h \
 phonenumber.h \
 picture.h \
 plugin.h \
 resource.h \
 secrecy.h \
 sound.h \
 stdaddressbook.h \
 timezone.h \
 tmpaddressbook.h \
 vcardconverter.h \
 vcard21parser.h \
 vcardformatimpl.h \
 vcardformatplugin.h \
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
 vcard/include/VCardTextNSParam.h \
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

 
 
 
SOURCES = \
 address.cpp \
 addressbook.cpp \
 addressee.cpp \
 addresseedialog.cpp \
 addresseelist.cpp \
 addresseeview.cpp \
 agent.cpp \
 distributionlist.cpp \
 distributionlistdialog.cpp \
 distributionlisteditor.cpp \
 field.cpp \
 formatfactory.cpp \
 geo.cpp \
 key.cpp \
 phonenumber.cpp \
 picture.cpp \
 plugin.cpp \
 resource.cpp \
 secrecy.cpp \
 sound.cpp \
 stdaddressbook.cpp \
 timezone.cpp \
 tmpaddressbook.cpp \
 vcardconverter.cpp \
 vcard21parser.cpp \
 vcardformatimpl.cpp \
 vcardformatplugin.cpp \
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
