/*
    This file is part of libkdepim.

    Copyright (c) 2004 Lutz Rogowski <rogowski@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/


#include <qstring.h>
#include <qapplication.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qfile.h>
#include <qlabel.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qdir.h>
#include <kmessagebox.h>
#include <stdlib.h>
#include "phoneaccess.h"
#ifdef DESKTOP_VERSION
#include <qdesktopwidget.h>
#endif
void PhoneAccess::writeConfig( QString device, QString connection, QString model )
{
#ifndef QT_4_COMPAT
#ifdef _WIN32_
    QString fileName = qApp->applicationDirPath () +"\\gammurc";
#else
    QString fileName = QDir::homeDirPath() +"/.gammurc";
#endif
    //qDebug("save %d ", load );
    QString content = "[gammu]\n";;
    bool write = false;
    bool addPort = true, addConnection = true, addModel = true;
    QFile file( fileName );
    if ( QFile::exists(  fileName) ) {
        if (!file.open( IO_ReadOnly ) ) {
            qDebug("Error: cannot open %s ", fileName.latin1() );
            return;
        } 
        QString line;
        while ( file.readLine( line, 1024 ) > 0 ) {
            //qDebug("*%s* ", line.latin1() );
            if ( line.left(7 ) == "[gammu]" ) {
               ; 
            } else 
            if ( line.left(4 ) == "port" ) {
                if ( line == "port = " + device+"\n" ) {
                    content += line ;
                    addPort = false;  
                    //qDebug("port found" );
                }
               
            } else  if ( line.left(5 ) == "model" )  {
                if ( line == "model = " + model +"\n") {
                    content += line ;
                    addModel = false;  
                    //qDebug("model found" );
                }
              
            } else  if ( line.left( 10 ) == "connection" )  {
                if ( line == "connection = " + connection +"\n") {
                    addConnection = false; 
                    content += line ;
                    //qDebug("con found" );
                }
              
            } else {
                content += line ;
            }
        }
        file.close();
    } else {
        if ( ! connection.isEmpty() ) {
            addConnection = true; 
        }
        if ( ! device.isEmpty() ) {
            addPort = true; 

        }
        if ( ! model.isEmpty() ) {
            addModel = true; 
        }
    }
   
    if ( addConnection ) {
        write = true;
        content += "connection = ";
        content += connection;
        content += "\n";
    }
    if ( addPort ) {
        write = true;
        content += "port = ";
        content += device;
        content += "\n";

    }
    if ( addModel ) {
        write = true;
        content += "model = ";
        content += model;
        content += "\n";
    }
    if ( write ) {
        if (!file.open( IO_WriteOnly ) ) {
            qDebug("Error: cannot write file  %s ", fileName.latin1() );
            return;
        } 
        qDebug("Writing file  %s ", fileName.latin1() );
        QTextStream ts( &file );
        ts << content ;
        file.close();
    }
#endif
}


bool PhoneAccess::writeToPhone( QString fileName)
{

#ifdef DESKTOP_VERSION
#ifdef _WIN32_ 
    QString command ="kammu --restore " + fileName ;
#else
    QString command ="./kammu --restore " + fileName ;
#endif
#else
    QString command ="kammu --restore " + fileName ;
#endif
    int ret = 1;
    while ( ret != 0 ) {
        QLabel* status = new QLabel( i18n(" This may take 1-3 minutes!"), 0 );
        int w = 235;
        int h = status->sizeHint().height()+20 ;
        int dw = QApplication::desktop()->width();
        int dh = QApplication::desktop()->height();
        if ( dw > 310 )
            w = 310;
        status->setCaption(i18n("Writing to phone...") );
        status->setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
        status->show();
        status->raise();
        status->update();
        qApp->processEvents();
        status->update();
        qApp->processEvents();
        ret = system (  command.latin1());
        delete status;
        qApp->processEvents();
        if ( ret ) {
            qDebug("Error S::command returned %d.", ret); 
            int retval = KMessageBox::warningContinueCancel(0,
                                                            i18n("Error accessing device!\nPlease turn on connection\nand retry!"),i18n("KDE/Pim phone access"),i18n("Retry"),i18n("Cancel"));
            if ( retval != KMessageBox::Continue )
                return false;
        }
    }
    return true;
}
bool PhoneAccess::readFromPhone( QString fileName)
{

#ifdef DESKTOP_VERSION
#ifdef _WIN32_ 
    QString command ="kammu --backup " + fileName + " -yes" ;
#else
    QString command ="./kammu --backup " + fileName + " -yes" ;
#endif
#else
    QString command ="kammu --backup " + fileName + " -yes" ;
#endif
    int ret = 1;
    while ( ret != 0 ) {
        QLabel* status = new QLabel( i18n(" This may take 1-3 minutes!"), 0 );
        int w = 235;
        int h = status->sizeHint().height()+20 ;
        int dw = QApplication::desktop()->width();
        int dh = QApplication::desktop()->height();
        if ( dw > 310 )
            w = 310;
        status->setCaption(i18n("Reading from phone...") );
        status->setGeometry( (dw-w)/2, (dh - h )/2 ,w,h );
        status->show();
        status->raise();
        status->update();
        qApp->processEvents();
        status->update();
        qApp->processEvents();
        ret = system (  command.latin1() );
        delete status;
        qApp->processEvents();
        if ( ret ) {
            qDebug("Error reading from phone:Command returned %d", ret); 
            int retval = KMessageBox::warningContinueCancel(0,
                                                            i18n("Error accessing device!\nPlease turn on connection\nand retry!"),i18n("KDE/Pim phone access"),i18n("Retry"),i18n("Cancel"));
            if ( retval != KMessageBox::Continue )
                return false;

        }
    }
    qApp->processEvents();
    return true;
}
