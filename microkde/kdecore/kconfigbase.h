/*
   This file is part of the KDE libraries
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997 Matthias Kalle Dalheimer <kalle@kde.org>
   Copyright (c) 2001 Waldo Bastian <bastian@kde.org>

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

// $Id: kconfigbase.h,v 1.1.1.1 2004/06/26 19:02:29 zautrix Exp $

#ifndef _KCONFIGBASE_H
#define _KCONFIGBASE_H

#include "kconfig.h"

/**
  * Helper class to facilitate working with @ref KConfig / @ref KSimpleConfig
  * groups.
  *
  * Careful programmers always set the group of a
  * @ref KConfig @ref KSimpleConfig object to the group they want to read from
  * and set it back to the old one of afterwards. This is usually
  * written as:
  * <pre>
  *
  * QString oldgroup config->group();
  * config->setGroup( "TheGroupThatIWant" );
  * ...
  * config->writeEntry( "Blah", "Blubb" );
  *
  * config->setGroup( oldgroup );
  * </pre>
  *
  * In order to facilitate this task, you can use
  * KConfigGroupSaver. Simply construct such an object ON THE STACK
  * when you want to switch to a new group. Then, when the object goes
  * out of scope, the group will automatically be restored. If you
  * want to use several different groups within a function or method,
  * you can still use KConfigGroupSaver: Simply enclose all work with
  * one group (including the creation of the KConfigGroupSaver object)
  * in one block.
  *
  * @author Matthias Kalle Dalheimer <kalle@kde.org>
  * @version $Id: kconfigbase.h,v 1.1.1.1 2004/06/26 19:02:29 zautrix Exp $
  * @see KConfigBase, KConfig, KSimpleConfig
  * @short Helper class for easier use of KConfig/KSimpleConfig groups
  */
//US I converted the class in a way that it can be used with KConfig objects of microkde
  
class KConfigGroupSaver
{
public:
  /**
   * Constructor. You pass a pointer to the KConfigBase-derived
   * object you want to work with and a string indicating the _new_
   * group.
   *
   * @param config The KConfigBase-derived object this
   *               KConfigGroupSaver works on.
   * @param group  The new group that the config object should switch to.
   */
  KConfigGroupSaver( KConfig* config, QString group )
      /* KDE 4 : make the second parameter const QString & */
      : _config(config), _oldgroup(config->group())
        { _config->setGroup( group ); }

  KConfigGroupSaver( KConfig* config, const char *group )
      : _config(config), _oldgroup(config->group())
        { _config->setGroup( group ); }

  KConfigGroupSaver( KConfig* config, const QCString &group )
      : _config(config), _oldgroup(config->group())
        { _config->setGroup( group ); }

  ~KConfigGroupSaver() { _config->setGroup( _oldgroup ); }

    KConfig* config() { return _config; };

private:
  KConfig* _config;
  QString _oldgroup;

  KConfigGroupSaver(const KConfigGroupSaver&);
  KConfigGroupSaver& operator=(const KConfigGroupSaver&);

};

#endif
