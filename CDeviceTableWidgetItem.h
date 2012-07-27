#ifndef CDEVICETABLEWIDGETITEM_H
#define CDEVICETABLEWIDGETITEM_H

#include <QObject>
#include <QString>
#include <QTableWidgetItem>
#include <QSharedPointer>
#include "QRelayDeviceControl.h"

class CDeviceTableWidgetItem : public QTableWidgetItem
{
public:
    CDeviceTableWidgetItem( QSharedPointer<QRelayDeviceControl> & pdev );
public:
    QSharedPointer<QRelayDeviceControl>   pdevice;
};


#endif // CDEVICETABLEWIDGETITEM_H
