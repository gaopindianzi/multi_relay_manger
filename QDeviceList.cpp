#include "QDeviceList.h"


#include "debug.h"
#define THISINFO               0
#define THISERROR            1
#define THISASSERT          1

QDeviceList::QDeviceList (RelayDeviceSharePonterType & pdev, QListWidget * parent , int type )
    :QListWidgetItem(parent,type)
{
    pdevice = pdev;
}


