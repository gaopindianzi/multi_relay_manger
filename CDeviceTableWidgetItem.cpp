#include "CDeviceTableWidgetItem.h"
#include <QString>


CDeviceTableWidgetItem::CDeviceTableWidgetItem(RelayDeviceSharePonterType & pdev )
    : QTableWidgetItem(Type)
    , pdevice(pdev)
{
    setData(0,qVariantFromValue(pdevice));
}
