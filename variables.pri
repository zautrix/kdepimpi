
# variables settings for compilation for Linux desktop and Qt 3.x.x

unix {
#for statically linking
#CONFIG +=  staticlib
#for raspi font rendering
LIBS += /usr/lib/x86_64-linux-gnu/libfreetype.so /usr/lib/x86_64-linux-gnu/libXft.so.2.3.2
}
win32 {
#disable importol to make it running on windows ME/98
CONFIG +=  importol

}
CONFIG +=  thread
CONFIG +=  release

#with VS2008 we do not have mfc71u dll
#CONFIG += mfc71u


release:CONFIG+= warn_off 
#new in order to make it compiling with VS2008 and qt338
#win32:warn off
win32:DEFINES+=_AFXDLL
win32:QMAKE_CXXFLAGS=-nologo -Zm200 -Zc:wchar_t-


#CONFIG +=  compile_QT4



compile_QT4 :{
INCLUDEPATH += ../qt4compat
DEFINES += QT_4_COMPAT
QT += qt3support
QT += xml
QT += network

}