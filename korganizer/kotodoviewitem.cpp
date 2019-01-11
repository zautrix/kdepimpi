/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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
*/

#include <klocale.h>
#include <kdebug.h>
#include <qapp.h>
#include <kglobal.h>

#include <kiconloader.h>
#include "kotodoviewitem.h"
#include "kotodoview.h"
#include "koprefs.h"

KOTodoViewItem::KOTodoViewItem( QListView *parent, Todo *todo, KOTodoView *kotodo)
  : QCheckListItem( parent , "", CheckBox ), mTodo( todo ), mTodoView( kotodo )
{
  construct();
}

KOTodoViewItem::KOTodoViewItem( KOTodoViewItem *parent, Todo *todo, KOTodoView *kotodo )
  : QCheckListItem( parent, "", CheckBox ), mTodo( todo ), mTodoView( kotodo )
{  
  construct();
}

QString KOTodoViewItem::key(int column,bool) const
{
    if ( column == 0 )
        return text(0).lower();
    if ( column == 1 ) {
        if ( mTodo->isCompleted() ) {
            return "6"+QString::number(mTodo->priority())+text(0).lower();
        }
        return QString::number(mTodo->priority())+text(0).lower();
    }
  QMap<int,QString>::ConstIterator it = mKeyMap.find(column);
  if (it == mKeyMap.end()) {
      return text(column).lower();
  } else {
      if (  column == 2 ) {
          return *it+text(0).lower();
      }
    return *it;
  }
}

void KOTodoViewItem:: setup()
{
   
     int h = 20;
     if ( listView () ) {
         QFontMetrics fm ( listView ()->font () );
         h = fm.height();
     }
     setHeight( h );

}
void KOTodoViewItem::setSortKey(int column,const QString &key)
{
  mKeyMap.insert(column,key);
}

#if QT_VERSION >= 0x030000
void KOTodoViewItem::paintBranches(QPainter *p,const QColorGroup & cg,int w,
                                   int y,int h)
{
  QListViewItem::paintBranches(p,cg,w,y,h);
}
#else
#endif

void KOTodoViewItem::construct()
{
    //  qDebug("KOTodoViewItem::construct() ");
  m_init = true;
  QString keyd = "==";
  QString keyt = "==";
  QString skeyd = "==";
  QString skeyt = "==";

  setOn(mTodo->isCompleted());
  setText(0,mTodo->summary());
  setText(1,QString::number(mTodo->priority()));
  setText(2,i18n("%1 %").arg(QString::number(mTodo->percentComplete())));
  if (mTodo->percentComplete()<100) {
      setSortKey(2,QString::number(mTodo->percentComplete()));
  }
  else {
    setSortKey(2,"999");
  }
  if (mTodo->hasDueDate()) {
    setText(3, mTodo->dtDueDateStr());
    QDate d = mTodo->dtDue().date();
    keyd.sprintf("%04d%02d%02d",d.year(),d.month(),d.day());
    // setSortKey(3,keyd);
    if (mTodo->doesFloat()) {
      setText(4,"");
    }
    else {
      setText(4,mTodo->dtDueTimeStr());
      QTime t = mTodo->dtDue().time();
      keyt.sprintf("%02d%02d",t.hour(),t.minute());
      //setSortKey(4,keyt);
    }
  } else {
    setText(3,"");
    setText(4,"");
  }
  setSortKey(3,keyd);
  setSortKey(4,keyt);
#if 0
  if (mTodo->isCompleted()) setSortKey(1,"6" + QString::number(mTodo->priority())+keyd+keyt);
  else setSortKey(1,QString::number(mTodo->priority())+keyd+keyt);
#endif
  keyd = "";
  keyt = "";

  if (mTodo->isRunning() ) {
      QDate d = mTodo->runStart().date();
      QTime t = mTodo->runStart().time();
      skeyt.sprintf("%02d%02d",t.hour(),t.minute());
      skeyd.sprintf("%04d%02d%02d",d.year(),d.month(),d.day());
      keyd = KGlobal::locale()->formatDate( d , true);
      keyt = KGlobal::locale()->formatTime( t );

  } else {

      if (mTodo->hasStartDate()) {
          keyd = mTodo->dtStartDateStr();
          QDate d = mTodo->dtStart().date();
          skeyd.sprintf("%04d%02d%02d",d.year(),d.month(),d.day());
    
          if ( !mTodo->doesFloat()) {
              keyt = mTodo->dtStartTimeStr();
              QTime t = mTodo->dtStart().time();
              skeyt.sprintf("%02d%02d",t.hour(),t.minute());
      
          }
      
      }
  }
  setText(5,keyd);
  setText(6,keyt);
  setSortKey(5,skeyd);
  setSortKey(6,skeyt);

  //setText(7,mTodo->cancelled() ? i18n("Yes") : i18n("No"));
  setText(7,mTodo->categoriesStr());
  setText(8,KOPrefs::instance()->calName( mTodo->calID()));
  setText(9, KGlobal::locale()->formatDateTime( mTodo->lastModified(), true, true ));
  setText(10, KGlobal::locale()->formatDateTime( mTodo->created(), true, true ));
  setText(11, KGlobal::locale()->formatDateTime( mTodo->lastModifiedSub(), true, true ));
  QString key; 
  QDate d;
  QTime t;
  setSortKey(9,mTodo->lastModifiedSortKey());
  d = mTodo->created().date();
  t = mTodo->created().time();
  key.sprintf("%04d%02d%02d%02d%02d%02d",d.year(),d.month(),d.day(),t.hour(),t.minute(),t.second() );
  setSortKey(10,key);
  setSortKey(11,mTodo->lastModifiedSubSortKey());
  
#if 0
  // Find sort id in description. It's the text behind the last '#' character
  // found in the description. White spaces are removed from beginning and end
  // of sort id.
  int pos = mTodo->description().findRev('#');
  if (pos < 0) {
    setText(6,"");
  } else {
    QString str = mTodo->description().mid(pos+1);
    str.stripWhiteSpace();
    setText(6,str);
  }
#endif

  m_known = false;
  m_init = false; 

  setMyPixmap();

}
void KOTodoViewItem::setMyPixmap()
{
  int size = 5;
  QPixmap pixi =  QPixmap( 1, 1 );
  // if ( !mTodo->isCompleted() &&  mTodo->hasDueDate() && mTodo->dtDue() < QDateTime::currentDateTime() ) {
//       pixi = SmallIcon("redcross16"); 
//   } else {
      QPainter p;
 
  int pixSize = 0;
  QPixmap pPix =  QPixmap(  size, size );
  if ( mTodo->description().length() > 0 ) {
      pixi.resize(size, pixSize+size);
      pPix.fill( Qt::darkGreen );
      p.begin( &pixi );
      p. drawPixmap ( 0, pixSize, pPix);
      p.end();
      pixSize += size;
  } 
  if ( mTodo->isAlarmEnabled() && mTodo->alarmEnabled()) {
      pixi.resize(size, pixSize+size);
      pPix.fill( Qt::red );
      p.begin( &pixi );
      p. drawPixmap ( 0, pixSize, pPix);
      p.end();
      pixSize += size;
  }  
  if ( mTodo->doesRecur() ) {
      pixi.resize(size, pixSize+size);
      pPix.fill( Qt::blue );
      p.begin( &pixi );
      p. drawPixmap ( 0, pixSize, pPix);
      p.end();
      pixSize += size;
  } 
  //   }
  if ( pixi.width() > 1 ) {
      setPixmap ( 0,pixi ) ; 
  } else {
      setPixmap ( 0,QPixmap() ) ; 
  }
}
bool KOTodoViewItem::askQuestionstateChange = true;
void KOTodoViewItem::stateChange(bool state)
{
    // qDebug("KOTodoViewItem::stateChange %d ", state);
  // do not change setting on startup
  if ( m_init ) return;
  if (isOn()!=state) {
      setOn(state);
      //qDebug("SETON ");
      return;
  }
  if ( mTodo->isCompleted() == state ) {
      //qDebug("STATECHANGE:nothing to do ");
      return;
  }
  QString keyd = "==";
  QString keyt = "==";
  //qDebug("KOTodoViewItem::stateChange %s ", text(0).latin1());
  if ( mTodo->doesRecur()  ){
      QDateTime start = mTodo->dtStart();
      mTodo->setCompleted(state);
      if ( start != mTodo->dtStart() ) {
          if ( state  && !mTodo->isCompleted() ) {
              setOn( false );
              state = false;
          }
      }
  } else
      mTodo->setCompleted(state);

  if (state) mTodo->setCompleted(QDateTime::currentDateTime());

  if (mTodo->hasDueDate()) {
    setText(3, mTodo->dtDueDateStr());
    QDate d = mTodo->dtDue().date();
    keyd.sprintf("%04d%02d%02d",d.year(),d.month(),d.day());
    setSortKey(3,keyd);
    if (mTodo->doesFloat()) {
      setText(4,"");
    }
    else {
      setText(4,mTodo->dtDueTimeStr());
      QTime t = mTodo->dtDue().time();
      keyt.sprintf("%02d%02d",t.hour(),t.minute());
      setSortKey(4,keyt);
    }
  }
 if (mTodo->hasStartDate()) {
     QString skeyt = "==";
     QString skeyd = "==";
     setText(5, mTodo->dtStartDateStr());
     QDate d = mTodo->dtStart().date();
     skeyd.sprintf("%04d%02d%02d",d.year(),d.month(),d.day());
     
     if (mTodo->doesFloat()) {
         setText(6,"");
     }
     else {
         setText(6,mTodo->dtStartTimeStr());
         QTime t = mTodo->dtStart().time();
         skeyt.sprintf("%02d%02d",t.hour(),t.minute());
         
     }
     setSortKey(5,skeyd);
     setSortKey(6,skeyt);
  } 
  if (mTodo->isCompleted()) setSortKey(1,QString::number(9)+keyd+keyt);
  else setSortKey(1,QString::number(mTodo->priority())+keyd+keyt);
  
  setText(2,i18n("%1 %").arg(QString::number(mTodo->percentComplete())));
  if (mTodo->percentComplete()<100) {
    if (mTodo->isCompleted()) setSortKey(2,QString::number(999));
    else setSortKey(2,QString::number(mTodo->percentComplete()));
  }
  else {
    if (mTodo->isCompleted()) setSortKey(2,QString::number(999));
    else setSortKey(2,QString::number(99));
  }
  mTodoView->modified(true);
  setMyPixmap();
  mTodoView->setTodoModified( mTodo );
  if ( state ) {
    QListViewItem * myChild = firstChild();
    if ( myChild ) { 
      bool asked = false;
      if ( askQuestionstateChange ) {  
	//PENDING LR translate
	int result = KMessageBox::warningContinueCancel(mTodoView,
							i18n("The Todo has sub-todos.\nDo you want to set all sub-todos to complete?"),todo()->summary(),i18n("Yes, set all to complete!") );
	if (result != KMessageBox::Continue) return;
	asked = true;
	askQuestionstateChange = false;
      }
      
      KOTodoViewItem *item;
      while( myChild ) {
	//qDebug("stateCH ");
	item = static_cast<KOTodoViewItem*>(myChild);
	item->stateChange(state);
	myChild = myChild->nextSibling();
      }
      if  ( asked )  { 
	askQuestionstateChange = true;
      }
    }
  } else {
    QListViewItem * myChild = parent();
    if ( myChild ) { 
      bool asked = false;
      if ( askQuestionstateChange ) { 
	//PENDING LR translate 
	int result = KMessageBox::warningContinueCancel(mTodoView,
							i18n("The Todo has a parent.\nDo you want to set the \nparent chain to not complete?"),todo()->summary(),i18n("Yes, set to not complete!") );
	if (result != KMessageBox::Continue) return;

	asked = true;
	askQuestionstateChange = false;
      }
      (static_cast<KOTodoViewItem*>(myChild))->stateChange(state);
      if  ( asked )  { 
	askQuestionstateChange = true;
      }
    }
  }
}

bool KOTodoViewItem::isAlternate()
{
#if 0
    //if ( m_known ) return m_odd;
   //qDebug("test ");
    KOTodoViewItem *item = static_cast<KOTodoViewItem *>(itemAbove());
    if ( item ) {
        m_known = item->m_known;
        if ( m_known ) {
            m_odd = !item->m_odd ;
            return m_odd;
        }
    } else {
        item = static_cast<KOTodoViewItem *>(itemBelow());
        if ( item ) {
            m_known = item->m_known;
            if ( m_known ) {
                m_odd = !item->m_odd ;
                return m_odd;
            }
        }
    }
  KOTodoListView *lv = static_cast<KOTodoListView *>(listView());
  item =  static_cast<KOTodoViewItem *>(lv->firstChild());
  bool previous = true;
  qDebug("WHILE ");
  while ( item ) {
      item->m_odd = !previous;
      item->m_known = true;
      previous = !previous;
      item = static_cast<KOTodoViewItem *>(item->itemBelow());
  }
  return m_odd;
  
#else
  
  //KOTodoListView *lv = static_cast<KOTodoListView *>(listView());
  //if (lv && lv->alternateBackground().isValid())
  {
    KOTodoViewItem *above = static_cast<KOTodoViewItem *>(itemAbove());
    m_known = above ? above->m_known : true;
    if (m_known)
    {
       m_odd = above ? !above->m_odd : false;
    }
    else
    {
       KOTodoViewItem *item;
       bool previous = true;
       if (QListViewItem::parent())
       {
          item = static_cast<KOTodoViewItem *>(QListViewItem::parent());
          if (item)
             previous = item->m_odd;
          item = static_cast<KOTodoViewItem *>(QListViewItem::parent()->firstChild());
       }
       else
       {
           KOTodoListView *lv = static_cast<KOTodoListView *>(listView());
          item = static_cast<KOTodoViewItem *>(lv->firstChild());
       }

       while(item)
       {
          item->m_odd = previous = !previous;
          item->m_known = true;
          item = static_cast<KOTodoViewItem *>(item->nextSibling());
       }
    }
    return m_odd;
  }
  return false;
#endif
}

void KOTodoViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
  QColorGroup _cg = cg;
  QColorGroup::ColorRole role; 
  if ( KOPrefs::instance()->mTodoViewUsesForegroundColor && !mTodo->isRunning())
      role = QColorGroup::Text;
  else
      role = QColorGroup::Base;
  //#ifndef KORG_NOLVALTERNATION
  if (isAlternate())
      _cg.setColor(QColorGroup::Base, static_cast< KOTodoListView* >(listView())->alternateBackground());
  bool setColor = KOPrefs::instance()->mTodoViewUsesCatColors;
  QColor colorToSet;
  if (  column == 0 &&  mTodo->calID() > 1 ) {
      setColor = true;
      colorToSet = KOPrefs::instance()->defaultColor( mTodo->calID() );
  } else  if ( setColor ) {
      QStringList categories = mTodo->categories();
      QString cat = categories.first();
      if ( !cat.isEmpty()) {
          colorToSet = *(KOPrefs::instance()->categoryColor(cat) );
      } else
          setColor = false;  
  }
   bool openMode = !isOpen();
   // maybe we are in flat-display-mode
   if ( !firstChild() )
       openMode = false;
   bool colorRunning = mTodo->isRunning();
   if ( ! colorRunning && openMode )
       colorRunning = mTodo->hasRunningSub();
   if ( colorRunning ) {
       setColor = true;
       colorToSet = KOPrefs::instance()->mTodoRunColor;     
   } else {
       int odue = mTodo->hasDueSubTodo( openMode );
       if (odue == 2) {
           colorToSet = KOPrefs::instance()->mTodoOverdueColor;
           setColor = true;
       } else if ( odue == 1 ) {
           colorToSet = KOPrefs::instance()->mTodoDueTodayColor;     
           setColor = true;
       }
   }
 

  if ( setColor ) {
      _cg.setColor(role,colorToSet );
      if ( role == QColorGroup::Base) {
          int rgb = colorToSet.red();
          rgb += colorToSet.blue()/2;
          rgb += colorToSet.green();
          if ( rgb < 200 )
              _cg.setColor(QColorGroup::Text,Qt::white );
      }
  }
  //#endif
  if ( column > 0 ){
      if ( column == 2 && !KOPrefs::instance()->mTodoViewShowsPercentage ) {
          p->save();
          int progress = (int)(( (width-6)*mTodo->percentComplete())/100.0 + 0.5);
          
          p->fillRect( 0, 0, width, height(), _cg.base() ); // background
          // p->setPen(Qt::black );  //border
          // p->setBrush( KOPrefs::instance()->mHighlightColorKGlobalSettings::baseColor() );  //filling
          QColor fc = KOPrefs::instance()->mHighlightColor;
          if ( mTodo->percentComplete() == 100 )
              fc = darkGreen;
          p->drawRect( 2, 2, width-4, height()-4);
          p->fillRect( 3, 3, progress, height()-6, 
                       fc );
          p->restore();
      } else {
          QCheckListItem::paintCell(p, _cg, column, width, alignment);
      }
      return;
  }

  int align = alignment;
   
    if ( !p )
	return;
    
    p->fillRect( 0, 0, width, height(), _cg.brush( QColorGroup::Base ) );

    QListView *lv = listView();
    if ( !lv )
	return;
    int marg = 2;//lv->itemMargin();
    int r = 0;
    QCheckListItem::Type myType = QCheckListItem::CheckBox;
    int BoxSize = 20;
    int boxOffset = 2;
    int xOffset = 2;
    if (qApp->desktop()->width() < 300 ) {
         BoxSize = 14;
         boxOffset = -1;
         xOffset = 1;
         // marg = 0;
    }
    if ( height() < BoxSize ) {
        boxOffset = boxOffset - ((BoxSize -  height())/2) ;
        // qDebug("boxOffset %d  height %d", boxOffset, height() );
        BoxSize = height();

    }
    //bool winStyle = lv->style() == WindowsStyle;

    int lineStart = 5;
    if ( myType == Controller ) {
        if ( !pixmap( 0 ) )
            r += BoxSize + 4;
    } else {
        ASSERT( lv ); //###
        //	QFontMetrics fm( lv->font() );
        //	int d = fm.height();
        int x = 0;
        int y = (height() - BoxSize) / 2;
        //	p->setPen( QPen( _cg.text(), winStyle ? 2 : 1 ) );
        if ( myType == CheckBox ) {
            if ( isEnabled() )
                p->setPen( QPen( _cg.text(), 1 ) );
            else
                p->setPen( QPen( listView()->palette().color( QPalette::Disabled, QColorGroup::Text ), 1 ) );
            p->drawRect( x+marg, y+2, BoxSize-4, BoxSize-4 );
            lineStart = x+marg;
            /////////////////////
                x++;
                y++;
                if ( isOn() ) {
                    QPointArray a( 7*2 );
                    int i, xx, yy;
                    xx = x+xOffset+marg+(boxOffset/2);
                    yy = y+5+boxOffset;
                    for ( i=0; i<3; i++ ) {
                        a.setPoint( 2*i,   xx, yy );
                        a.setPoint( 2*i+1, xx, yy+2 );
                        // qDebug(" ");
                        xx++; yy++;
                    }
                    yy -= 2;
                    for ( i=3; i<7; i++ ) {
                        a.setPoint( 2*i,   xx, yy );
                        a.setPoint( 2*i+1, xx, yy+2 );
                        xx++; yy--;
                    }
                    p->setPen( darkGreen );
                    p->drawLineSegments( a );
                }
                ////////////////////////
        } 
        r += BoxSize + 4;
    }

    p->translate( r, 0 );
    p->setPen( QPen( _cg.text() ) );
    QListViewItem::paintCell( p, _cg, column, width - r, align );
    if ( mTodo->cancelled () ) {
        p->setPen( black );
        QRect br = p->boundingRect( 1,1,1,1,0,mTodo->summary() );
        int wid = br.width() +lineStart;
        if (  wid > width-3 )
            wid = width-3;
        p->drawLine( lineStart, height()/2+1, wid, height()/2+1  );

    }

}
