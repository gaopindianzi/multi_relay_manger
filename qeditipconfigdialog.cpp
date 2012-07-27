#include "qeditipconfigdialog.h"
#include "ui_qeditipconfigdialog.h"

#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QHostAddress>
#include <QMessageBox>
#include "QDeviceList.h"
#include "multimgr_device_dev.h"

#include "debug.h"
#define THISINFO               0
#define THISERROR            1
#define THISASSERT          1

QEditIpConfigDialog::QEditIpConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEditIpConfigDialog)
{
    ui->setupUi(this);

    CreateDeviceList();
    connect(ui->pushButtonReadIpconfig,SIGNAL(clicked(bool)),this,SLOT(ReadClicked(bool)));
    connect(ui->pushButtonWriteIpconfig,SIGNAL(clicked(bool)),this,SLOT(WriteClicked(bool)));
    connect(ui->pushButtonResetDevcie,SIGNAL(clicked(bool)),this,SLOT(ResetClicked(bool)));

}

QEditIpConfigDialog::~QEditIpConfigDialog()
{
    delete ui;
}




void QEditIpConfigDialog::CreateDeviceList(void)
{
    list_devices = new QListWidget(this);
    //list_devices->setSelectionMode(QAbstractItemView::ContiguousSelection);
    connect(list_devices,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked(QListWidgetItem*)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(list_devices);
    ui->groupBoxDeviceList->setLayout(layout);
}
void QEditIpConfigDialog::InsertDevice(RelayDeviceSharePonterType & pdev)
{
    int row = list_devices->count();
    QDeviceList * item = new QDeviceList(pdev);
    item->setText(pdev->GetHostAddressString());
    this->list_devices->insertItem(row,item);
    this->list_devices->setCurrentItem(item);
    itemClicked(item);
}
void	QEditIpConfigDialog::itemClicked ( QListWidgetItem * item )
{
    QDeviceList * it = (QDeviceList *)item;
    QSharedPointer<device_info_st> & pinfo = it->pdevice->GetDeviceInfo();
    QString ip;
    const unsigned char * pch = pinfo->local_ip;
    ip.sprintf("%d.%d.%d.%d",pch[0],pch[1],pch[2],pch[3]);
    ui->lineEditCurrIpAddr->setText(ip);

    pch = pinfo->net_mask;
    ip.sprintf("%d.%d.%d.%d",pch[0],pch[1],pch[2],pch[3]);
    ui->lineEditCurrNetmask->setText(ip);

    pch = pinfo->gateway;
    ip.sprintf("%d.%d.%d.%d",pch[0],pch[1],pch[2],pch[3]);
    ui->lineEditCurrGateway->setText(ip);

    pch = pinfo->dns;
    ip.sprintf("%d.%d.%d.%d",pch[0],pch[1],pch[2],pch[3]);
    ui->lineEditCurrDns->setText(ip);
}


void QEditIpConfigDialog::ReadClicked(bool)
{
    debuginfo(("ReadClicked clicked."));
    if(list_devices->count() == 0) {
        return ;
    }
    QDeviceList * item =(QDeviceList *) list_devices->currentItem();
    item->pdevice->GetDevcieInfoFormDevcie();
    itemClicked(item);
}

void QEditIpConfigDialog::WriteClicked(bool)
{
    debuginfo(("WriteClicked clicked."));
    if(list_devices->count() == 0) {
        return ;
    }
    QDeviceList * item =(QDeviceList *) list_devices->currentItem();
    device_info_st edit_info;
    QSharedPointer<device_info_st> & pinfo = item->pdevice->GetDeviceInfo();
    memcpy(&edit_info,&pinfo->command,sizeof(device_info_st));
    edit_info.to_host = 0;
    edit_info.change_password = 0;
    edit_info.cncryption_mode = 0;
    edit_info.system_fun_option = 0;
    edit_info.change_ipconfig = 1;
    QHostAddress addr;
    QMessageBox msgBox;
    if(!addr.setAddress(ui->lineEditNewIpaddr->text())) {
        msgBox.setText(tr("Set New Ip address is not valid."));
        msgBox.exec();
        return ;
    } else {
        qint32 ip = addr.toIPv4Address();
        edit_info.local_ip[0] = (unsigned char)(ip>>24);
        edit_info.local_ip[1] = (unsigned char)(ip>>16);
        edit_info.local_ip[2] = (unsigned char)(ip>>8);
        edit_info.local_ip[3] = (unsigned char)(ip>>0);

        QString str;
        const unsigned char * pch = edit_info.local_ip;
        str.sprintf("ip:%d-%d-%d-%d ",pch[3],pch[2],pch[1],pch[0]);
        debuginfo(("%s",str.toAscii().data()));
    }
    if(!addr.setAddress(ui->lineEditNewNetmask->text())) {
        msgBox.setText(tr("Set New Netmask is not valid."));
        msgBox.exec();
        return ;
    } else {
        qint32 ip = addr.toIPv4Address();
        edit_info.net_mask[0] = (unsigned char)(ip>>24);
        edit_info.net_mask[1] = (unsigned char)(ip>>16);
        edit_info.net_mask[2] = (unsigned char)(ip>>8);
        edit_info.net_mask[3] = (unsigned char)(ip>>0);
    }
    if(!addr.setAddress(ui->lineEditNewGateway->text())) {
        msgBox.setText(tr("Set New Gateway is not valid."));
        msgBox.exec();
        return ;
    } else {
        qint32 ip = addr.toIPv4Address();
        edit_info.gateway[0] = (unsigned char)(ip>>24);
        edit_info.gateway[1] = (unsigned char)(ip>>16);
        edit_info.gateway[2] = (unsigned char)(ip>>8);
        edit_info.gateway[3] = (unsigned char)(ip>>0);
    }
    if(!addr.setAddress(ui->lineEditNewDns->text())) {
        msgBox.setText(tr("Set New DNS is not valid."));
        msgBox.exec();
        return ;
    } else {
        qint32 ip = addr.toIPv4Address();
        edit_info.dns[0] = (unsigned char)(ip>>24);
        edit_info.dns[1] = (unsigned char)(ip>>16);
        edit_info.dns[2] = (unsigned char)(ip>>8);
        edit_info.dns[3] = (unsigned char)(ip>>0);
    }
    debuginfo(("set new ipconfig."));
    item->pdevice->WriteNewDeviceInfoToDevice(&edit_info);
}

void QEditIpConfigDialog::ResetClicked(bool)
{
    debuginfo(("ResetClicked clicked."));
    if(list_devices->count() == 0) {
        return ;
    }
    //遍历所有选中的项目
    QList<QListWidgetItem *> items = list_devices->selectedItems();
    for(int i=0;i<items.size();i++) {
        QDeviceList * it = (QDeviceList *)items.at(i);
        it->pdevice->ResetDevice();
    }
}


