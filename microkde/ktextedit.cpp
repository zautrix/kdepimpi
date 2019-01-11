
#include <ktextedit.h>
#ifndef DESKTOP_VERSION
#include <qpe/qpeapplication.h>
#endif
#include <qevent.h>

KTextEdit::KTextEdit ( QWidget *parent ) : QMultiLineEdit( parent ) 
{
    mAllowPopupMenu = false; 
    mMouseDown = false; 
    mIgnoreMark = false; 
#ifndef DESKTOP_VERSION 
    QPEApplication::setStylusOperation( this, QPEApplication::RightOnHold );
#endif 
}

void KTextEdit::mousePressEvent(QMouseEvent *e)
{
    if (  e->button() == Qt::LeftButton ) {
        mAllowPopupMenu = true;
        mYMousePos = mapToGlobal( (e->pos())).y();
        mXMousePos = mapToGlobal( (e->pos())).x();
    }
     if (  e->button() == Qt::RightButton &&  !mAllowPopupMenu   )
      return;
    if (  e->button() == Qt::LeftButton ) {
        if ( hasMarkedText () )
            mIgnoreMark = !mIgnoreMark;
        if ( mIgnoreMark && hasMarkedText () ) {
            mMouseDown = false;
            return ;
        }
    }
    QMultiLineEdit::mousePressEvent( e );
}

void KTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QMultiLineEdit::mouseReleaseEvent(e);
}

void KTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    int diff = mYMousePos - mapToGlobal( (e->pos())).y();
    if ( diff < 0 ) diff = -diff;
    int diff2 = mXMousePos - mapToGlobal( (e->pos())).x();
    if ( diff2 < 0 ) diff2 = -diff2;
    if ( diff+ diff2 > 20 )
        mAllowPopupMenu = false;
    QMultiLineEdit::mouseMoveEvent(e);
}

