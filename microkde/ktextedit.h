#ifndef MICROKDE_KTEXTEDIT_H
#define MICROKDE_KTEXTEDIT_H

#include <qmultilineedit.h>


class KTextEdit : public QMultiLineEdit
{
  public:
    KTextEdit( QWidget *parent )  ;
    void setIgnoreMark( bool b ) { mIgnoreMark = b; }

 private:
    bool mAllowPopupMenu;
    bool mMouseDown;
    bool mIgnoreMark;
    int mYMousePos;
    int mXMousePos;
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
};

#endif
