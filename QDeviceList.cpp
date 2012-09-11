#include "QDeviceList.h"


#include "debug.h"
#define THISINFO               0
#define THISERROR            0
#define THISASSERT          0

QDeviceList::QDeviceList (RelayDeviceSharePonterType & pdev, QListWidget * parent , int type )
    :QListWidgetItem(parent,type)
{
    pdevice = pdev;
}


