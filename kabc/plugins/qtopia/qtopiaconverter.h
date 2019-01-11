/*
    This file is part of libkabc.
    Copyright (c) 2002 Tobias Koenig <tokoe@kde.org>

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

/*
Enhanced Version of the file for platform independent KDE tools.
Copyright (c) 2004 Ulf Schenk
$Id: qtopiaconverter.h,v 1.5 2004/10/23 12:27:27 zautrix Exp $
*/

#ifndef KABC_QTOPIACONVERTER_H
#define KABC_QTOPIACONVERTER_H

#include <qstring.h>

#include "addressee.h"
#ifdef DESKTOP_VERSION
#include <qdom.h> 
#else
#include <xml/qdom.h> 
#endif
class Categories;

namespace KABC {



class OpieCategories {
 public:
    //friend class KSync::OpieSocket;
    friend bool operator== ( const OpieCategories &a, const OpieCategories &b );
    OpieCategories();
    OpieCategories(const QString &id, const QString &name, const QString &app );
    OpieCategories(const OpieCategories & );
    ~OpieCategories() {};
    OpieCategories &operator=(const OpieCategories & );
    QString id()const;
    QString name()const;
    QString app()const;

 private:
    QString m_name;
    QString m_app;
    QString m_id;
};


 class CategoryEdit {
    public:
        CategoryEdit();
        CategoryEdit(const QString &fileName);
        ~CategoryEdit();

        void save(const QString&) const;
        int addCategory( const QString &name, int id = 0 );
        int addCategory(const QString &appName,  const QString &name,  int id = 0);
        void parse( const QString &fileName );

        QString categoryById(const QString &id, const QString &app )const;
        QStringList categoriesByIds( const QStringList& ids,  const QString& app );

        void clear();
        QValueList<OpieCategories> categories()const {  return m_categories; };
    private:
        /**
         * this function will be used internally to update the kde categories...
         */
        void updateKDE( const QString& app,  const QStringList& categories );
        QMap<int, bool> ids; // from tt Qtopia::UidGen
        QValueList<OpieCategories> m_categories;
    };


class QtopiaConverter
{
public:

  /**
   * Constructor.
   */
  QtopiaConverter();

  /**
   * Destructor.
   */
  virtual ~QtopiaConverter();
  
  bool init();
  void deinit();
  
  /**
   * Converts a vcard string to an addressee.
   *
   * @param contact The qtopia contact.
   * @param addr    The addressee.
   */
  bool qtopiaToAddressee( const QDomElement& el, Addressee &adr );
  /**
   * Converts an addressee to a vcard string.
   *
   * @param addr    The addressee.
   * @param contact The qtopia contact.
   */
  bool addresseeToQtopia( const Addressee &ab, QTextStream *stream );

  private:
  QString categoriesToNumber( const QStringList &list, const QString &app );
  QString escape( const QString& s){ return s;};
    CategoryEdit *m_edit;
    QDate fromString( const QString& );
    QDate dateFromString( const QString& );
    QString dateToString( const QDate& );
  
    
};
}
#endif
