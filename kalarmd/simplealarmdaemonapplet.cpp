#include "simplealarmdaemonapplet.h"

#include "simplealarmdaemonimpl.h"

#include <qcopchannel_qws.h> 
#include <qlabel.h>
#include <qapp.h>
#include <qpe/resource.h>
SimpleAlarmDaemonApplet::SimpleAlarmDaemonApplet()
  : mApplet( 0 ), ref( 0 )
{
  
}

SimpleAlarmDaemonApplet::~SimpleAlarmDaemonApplet()
{
  delete mApplet;
  mApplet = 0;
}


QWidget *SimpleAlarmDaemonApplet::applet( QWidget *parent )
{
  if ( !mApplet ) {
      mApplet = new SimpleAlarmDaemonImpl( parent );
          if ( QApplication::desktop()->width() < 480 ) 
              mApplet->setPixmap( Resource::loadPixmap( "ko16" ) );
          else
              mApplet->setPixmap( Resource::loadPixmap( "ko24" ) );
      QCopChannel* c = new QCopChannel("koalarm",mApplet , "channel" ) ;
      QObject::connect( c, SIGNAL (received ( const QCString &, const QByteArray & )),mApplet, SLOT(recieve(  const QCString&, const QByteArray& )));
      mApplet->show();
  }
  return mApplet;
}

int SimpleAlarmDaemonApplet::position() const
{
  return 7;
}

QRESULT SimpleAlarmDaemonApplet::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
  *iface = 0;
  if ( uuid == IID_QUnknown )
      *iface = this;
  else if ( uuid == IID_TaskbarApplet )
      *iface = this;
  if ( *iface ) {
      (*iface)->addRef();
      return QS_OK;
  }
  return QS_FALSE;
}
Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( SimpleAlarmDaemonApplet )
}
