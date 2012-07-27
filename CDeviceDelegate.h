#ifndef CDEVICEDELEGATE_H
#define CDEVICEDELEGATE_H


#include <QObject>
#include <QSharedPointer>
#include <QUdpSocket>
 #include <QBitArray>
#include <QItemDelegate>
#include "multimgr_device_dev.h"
#include "QRelayDeviceControl.h"

class CDeviceDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    CDeviceDelegate ( QObject * parent = 0);
public:
    //QSharedPointer<QRelayDeviceControl>  pdevice;
};

#endif // CDEVICEDELEGATE_H
