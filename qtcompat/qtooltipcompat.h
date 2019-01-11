#ifndef QTCOMPAT_QTOOLTIP_H
#define QTCOMPAT_QTOOLTIP_H

/******
 *
 *  rob's QToolTip class
 *
 *  Apparently Sharp concluded that ToolTips were not useful on the Zaurus and 
 * left them out of their Qtopia.  Unfortunately, QWhatsThis uses the 
 * QToolTips::palette(), that function returns all 0's, and that means that 
 * QWhatsThis windows come up with black background and black foreground.  By 
 * re-implementing this class, QWhatsThis calls this QToolTip::palette(), and 
 * gets a useful result.
 *
 *   Include this class in your own Zaurus application and QWhatsThis should work 
 * for you as well.
 * 
 *  The contents of this file are released without restriction to the public 
 *  domain.
 *
 *  Copyright (c) rob miller October, 2003
 *
 *****/
#ifdef ADD_TOOLTIP

#include <qpalette.h>
class QToolTip: public Qt {
 public:
  static  QPalette palette();
};

#endif

#endif
