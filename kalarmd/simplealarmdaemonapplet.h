#ifndef SIMPLEALARMDAEMONAPPLET_H
#define SIMPLEALARMDAEMONAPPLET_H

#include <qpe/taskbarappletinterface.h>

class SimpleAlarmDaemonImpl;

class SimpleAlarmDaemonApplet : public TaskbarAppletInterface
{
  public:
    SimpleAlarmDaemonApplet();
    virtual ~SimpleAlarmDaemonApplet();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

  private:
    SimpleAlarmDaemonImpl *mApplet;
    ulong ref;
};

#endif
