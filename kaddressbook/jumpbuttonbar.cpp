/*                                                                      
    This file is part of KAddressBook.                                  
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>                   
                                                                        
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or   
    (at your option) any later version.                                 
                                                                        
    This program is distributed in the hope that it will be useful,     
    but WITHOUT ANY WARRANTY; without even the implied warranty of      
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        
    GNU General Public License for more details.                        
                                                                        
    You should have received a copy of the GNU General Public License   
    along with this program; if not, write to the Free Software         
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           
                                                                        
    As a special exception, permission is given to link this program    
    with any edition of Qt, and distribute the resulting executable,    
    without including the source code for Qt in the source distribution.
*/                                                                      

#include <qevent.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qtl.h>
#include <qapplication.h>

#include <kabc/addressbook.h>
#include <kabc/field.h>
#include <kdebug.h>
#include <klocale.h>

#include "kabcore.h"

#include "jumpbuttonbar.h"

class JumpButton : public QPushButton
{
  public:
    JumpButton( const QString &text, QWidget *parent,
                const QString &character );

    void setCharacter( const QString &character );
    QString character() const;

  private:
    QString mCharacter;
};

JumpButton::JumpButton( const QString &text, QWidget *parent,
                        const QString &character )
  : QPushButton( text, parent )
{
  mCharacter = character;
}

void JumpButton::setCharacter( const QString &character )
{
  mCharacter = character;
  setText(mCharacter.upper() );
}

QString JumpButton::character() const
{
  return mCharacter;
}

JumpButtonBar::JumpButtonBar( KABCore *core, QWidget *parent, const char *name )
  : QWidget( parent, name ), mCore( core )
{
    if ( QApplication::desktop()->width() < 480 )
        
        mButtonLayout = new QGridLayout( this,  1, 18 );
    else
        mButtonLayout = new QGridLayout( this,  1, 20 );
  mButtonLayout->setAlignment( Qt::AlignTop );

  recreateButtons();
}

JumpButtonBar::~JumpButtonBar()
{
}

QSizePolicy JumpButtonBar::sizePolicy() const
{
#ifndef KAB_EMBEDDED  
  return QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum,
                      QSizePolicy::Vertically );
#else //KAB_EMBEDDED  
  return QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum);
#endif //KAB_EMBEDDED  
}
  
void JumpButtonBar::letterClicked()
{
  JumpButton *button = (JumpButton*)sender();
  QString character = button->character();
  if ( character.length() == 2 )
      character = character.left(1) +"-"+character.right(1);
  if ( !character.isNull() )
      emit jumpToLetter( character );
  //qDebug("emit *%s* ",character.latin1());
}

void JumpButtonBar::recreateButtons()
{
  // the easiest way to remove all buttons ;)
  //mButtons.setAutoDelete( true );
  //mButtons.clear();
  //mButtons.setAutoDelete( false );
  mCharacters.clear();

  QString character;
  
  KABC::AddressBook *ab = mCore->addressBook();
  KABC::AddressBook::Iterator it;
  KABC::Field *field = mCore->currentSearchField();
  if ( field ) {
      setEnabled( true );
  } else {
      setEnabled( false );
      return;
  }
  mCharacters.append( "*");
  for ( it = ab->begin(); it != ab->end(); ++it ) {
      if ( !field->value( *it ).isEmpty() )
          character = field->value( *it )[ 0 ].lower();
      if ( character != "!" ) {
          if ( !character.isEmpty() && !mCharacters.contains( character ) && character.at( 0 ).isLetter ()) 
          mCharacters.append( character );
      }
  }
  if ( mCharacters.count() == 0 ) {
      setEnabled( false );
      return;
  }
  
  int maxRows = mCharacters.count() / 2; // we use 2 columns
  if ( mCharacters.count() % 2 )
    maxRows++;
  sortListLocaleAware( mCharacters );
  bool skip2 = false;
  int skipcount = 0;
  int maxHei = 0;
#ifdef DESKTOP_VERSION
  int maxChar = 16;
  int heightDiff = 200;
  int fixwid = 36;
#else
  //default for 240x320 diaplay:
  int maxChar = 16;
  int heightDiff = 64;
  int fixwid = 20;

  if ( QApplication::desktop()->height() == 480 ) {
      maxChar = 16;
      heightDiff = 135;
      fixwid = 34;
  } else if ( QApplication::desktop()->height() >= 640 ) {
      maxChar = 20;
      heightDiff = 135;
      fixwid = 34;
  }
#endif

  if ( mCharacters.count() > maxChar )
      skipcount = mCharacters.count()- maxChar;
  maxHei = (QApplication::desktop()->height()-heightDiff)/(maxChar);


     maxRows = 28;
     bool skipcurrent = false;
  bool state = isUpdatesEnabled();
  setUpdatesEnabled( false );
  //qDebug("cc %d ",mCharacters.count() );
  JumpButton *button = 0;
  int row = 0, col = 0;
  JumpButton* cur = mButtons.first();
  for ( uint i = 0; i < mCharacters.count(); ++i ) {
      if ( skipcount > 0 && skipcurrent ) {
          --skipcount;
          if ( button ) {
              button->setCharacter( button->character() + mCharacters[ i ]);
          }
      } else {
          if ( cur ) {
              button = cur ;
              cur = mButtons.next();
              button->setCharacter(mCharacters[ i ]);
          } else {
              button = new JumpButton( mCharacters[ i ].upper(), this, mCharacters[ i ] );
              if ( fixwid )
                  button->setFixedWidth( fixwid );
              mButtons.append( button );
              connect( button, SIGNAL( clicked() ), this, SLOT( letterClicked() ) );
              mButtonLayout->addWidget( button, row, col );
          }
          if ( maxHei  )
              button->setMaximumHeight( maxHei );
          button->show();
          
          if ( col == maxRows ) {
              row = 0;
              col++;
          } else
              row++;
      }
      if ( i > 0 )
          skipcurrent = !skipcurrent;
  }
  while ( cur ) {
      cur->hide();
       cur = mButtons.next();
  }

  mButtonLayout->activate();
  setUpdatesEnabled( state );
  update();
}

void JumpButtonBar::sortListLocaleAware( QStringList &list )
{
  QStringList::Iterator beginIt = list.begin();
  QStringList::Iterator endIt = list.end();

  --endIt;
  if ( beginIt == endIt ) // don't need sorting
    return;

  QStringList::Iterator walkBackIt = endIt;
  while ( beginIt != endIt ) {
    QStringList::Iterator j1 = list.begin();
    QStringList::Iterator j2 = j1;
    ++j2;
    while ( j1 != walkBackIt ) {
#ifndef KAB_EMBEDDED      
      if ( QString::localeAwareCompare( *j2, *j1 ) < 0 )
#else //KAB_EMBEDDED      
      if ( QString::compare( *j2, *j1 ) < 0 )
#endif //KAB_EMBEDDED      
        qSwap( *j1, *j2 );

      ++j1;
      ++j2;
    }
    ++beginIt;
    --walkBackIt;
  }
}

#ifndef KAB_EMBEDDED
#include "jumpbuttonbar.moc"
#endif //KAB_EMBEDDED
