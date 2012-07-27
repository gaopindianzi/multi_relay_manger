#ifndef QDEVICELIST_H
#define QDEVICELIST_H
#include <QDialog>
#include <QObject>
#include <QAbstractItemModel>
#include <QListWidget>
#include "QRelayDeviceControl.h"

class QDeviceList : public QListWidgetItem
{
public:
    QDeviceList (RelayDeviceSharePonterType & pdev, QListWidget * parent = 0, int type = Type );
public:
    RelayDeviceSharePonterType pdevice;
};


#endif // QDEVICELIST_H
