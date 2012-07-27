#include "editparamdialog.h"
#include "ui_editparamdialog.h"
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QMessageBox>
#include "QDeviceList.h"
#include "multimgr_device_dev.h"
#include <QStatusBar>


#include "debug.h"
#define THISINFO               0
#define THISERROR            1
#define THISASSERT          1

EditParamDialog::EditParamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditParamDialog)
{
    ui->setupUi(this);
    CreateDeviceList();
    connect(ui->pushButtonRemoveFormList,SIGNAL(clicked(bool)),this,SLOT(RemoveClicked(bool)));
    connect(ui->pushButtonReadDeviceInfo,SIGNAL(clicked(bool)),this,SLOT(ReadClicked(bool)));
    connect(ui->pushButtonWrite,SIGNAL(clicked(bool)),this,SLOT(WriteClicked(bool)));
    connect(ui->pushButtonResetDevice,SIGNAL(clicked(bool)),this,SLOT(ResetClicked(bool)));

}

EditParamDialog::~EditParamDialog()
{
    delete ui;
}


void EditParamDialog::CreateDeviceList(void)
{
    list_devices = new QListWidget(this);
    list_devices->setSelectionMode(QAbstractItemView::ContiguousSelection);
    connect(list_devices,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked(QListWidgetItem*)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(list_devices);
    ui->groupBoxDeviceList->setLayout(layout);
    ui->labelStatusBar->setText(tr("status:"));

    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    ui->lineEditPasswordAgain->setEchoMode(QLineEdit::Password);
}
void EditParamDialog::InsertDevice(RelayDeviceSharePonterType & pdev)
{
    int row = list_devices->count();
    QDeviceList * item = new QDeviceList(pdev);
    item->setText(pdev->GetHostAddressString());
    this->list_devices->insertItem(row,item);
    this->list_devices->setCurrentItem(item);
    connect(pdev.data(),SIGNAL(DeviceAckStatus(QString)),this,SLOT(DeviceAckStatus(QString)));
    itemClicked(item);
}
void	EditParamDialog::itemClicked ( QListWidgetItem * item )
{
    QDeviceList * it = (QDeviceList *)item;
    //debuginfo(("item clicked:%s",it->pdevice->GetDeviceName().toAscii().data()));
    ui->lineEditDeviceName->setText(it->pdevice->GetDeviceName());
    ui->lineEditGroupName1->setText(it->pdevice->GetGroup1Name());
    ui->lineEditGroupName2->setText(it->pdevice->GetGroup2Name());
    ui->lineEditBroadcastTime->setText(it->pdevice->GetBroadcastTime());
    ui->lineEditDeviceListionPort->setText(it->pdevice->GetListionPort());
    ui->lineEditHostAddress->setText(it->pdevice->GetRemoteHostAddress());
    ui->lineEditHostPort->setText(it->pdevice->GetRemoteHostPort());
    ui->labelDevcieModel->setText(it->pdevice->GetDeviceModelName());
}
void EditParamDialog::ListCurrentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous )
{
    debuginfo(("item changed..."));
}
void EditParamDialog::DeviceAckStatus(QString ackstr)
{
    debuginfo(("edit param > DeviceAckStatus..."));
    ui->labelStatusBar->setText(tr("Status:") + ackstr);
}

void EditParamDialog::RemoveClicked(bool)
{
    debuginfo(("remove clicked."));
    if(list_devices->count() == 0) {
        return ;
    }
    int row = list_devices->currentRow();
    list_devices->takeItem(row);
}

void EditParamDialog::ReadClicked(bool)
{
    debuginfo(("ReadClicked clicked."));
    if(list_devices->count() == 0) {
        return ;
    }
    QDeviceList * item =(QDeviceList *) list_devices->currentItem();
    item->pdevice->GetDevcieInfoFormDevcie();
    itemClicked(item);
}

void EditParamDialog::WriteClicked(bool)
{
    debuginfo(("WriteClicked clicked."));
    if(list_devices->count() == 0) {
        return ;
    }
    if(ui->checkBoxChnagePassword->isChecked()) {
        if(ui->lineEditPassword->text() != ui->lineEditPasswordAgain->text()) {
            QMessageBox box(QMessageBox::Warning,tr("Warning"),tr("The input password is not the equal!"));
            box.exec();
            return ;
        }
    }

    //遍历所有选中的项目
    QList<QListWidgetItem *> items = list_devices->selectedItems();
    for(int i=0;i<items.size();i++) {
        QDeviceList * item = (QDeviceList *)items.at(i);
        QSharedPointer<device_info_st> & info = item->pdevice->GetDeviceInfo();
        memcpy(&edit_info.command,&info->command,sizeof(device_info_st));
        //根据选项，修改参数，然后再写
        if(ui->checkBoxDeviceName->isChecked()) {
            QString name = ui->lineEditDeviceName->text();
            name.resize(sizeof(edit_info.host_name));
            memcpy(edit_info.host_name,name.toAscii().data(),sizeof(edit_info.host_name));
            edit_info.host_name[sizeof(edit_info.host_name)-1] = 0;
            debuginfo(("change device name to : %s",edit_info.host_name));
        } else {
            debuginfo(("not change device name."));
        }
        if(ui->checkBoxGroup1Name->isChecked()) {
            QString addr = ui->lineEditGroupName1->text();
            addr.resize(sizeof(edit_info.group_name1) + 1);
            memcpy(edit_info.group_name1,addr.toAscii().data(),sizeof(edit_info.group_name1));
            edit_info.host_name[sizeof(edit_info.group_name1) - 1] = 0;
        }
        if(ui->checkBoxGroup2Name->isChecked()) {
            QString addr = ui->lineEditGroupName2->text();
            addr.resize(sizeof(edit_info.group_name2) + 1);
            memcpy(edit_info.group_name2,addr.toAscii().data(),sizeof(edit_info.group_name2));
            edit_info.host_name[sizeof(edit_info.group_name2) - 1] = 0;
        }
        if(ui->checkBoxRemoteIpAdddr->isChecked()) {
            QString addr = ui->lineEditHostAddress->text();
            addr.resize(sizeof(edit_info.remote_host_addr) + 1);
            memcpy(edit_info.remote_host_addr,addr.toAscii().data(),sizeof(edit_info.remote_host_addr));
            edit_info.host_name[sizeof(edit_info.remote_host_addr) - 1] = 0;
        }
        //端口号
        if(ui->checkBoxRemotePort->isChecked()) {
            unsigned int port = ui->lineEditHostPort->text().toUInt();
            edit_info.remote_host_port[0] = (unsigned char)(port&0xFF);
            edit_info.remote_host_port[1] = (unsigned char)(port>>8);
            debuginfo(("remote host port = %d",port));
        }
        if(ui->checkBoxLocalPort->isChecked()) {
            unsigned int devicelistion_port = ui->lineEditDeviceListionPort->text().toUInt();
            edit_info.work_port[0] = (unsigned char)(devicelistion_port&0xFF);
            edit_info.work_port[1] = (unsigned char)(devicelistion_port>>8);
            debuginfo(("devicelistion_port port = %d",devicelistion_port));
        }
        if(ui->checkBoxBroadcastPeriod->isChecked()) {
            unsigned int BroadcastPeriod = ui->lineEditBroadcastTime->text().toUInt();
            BroadcastPeriod = (BroadcastPeriod > 60)?60:BroadcastPeriod;
            BroadcastPeriod = (BroadcastPeriod < 2)?2:BroadcastPeriod;
            edit_info.broadcast_time = (unsigned char)(BroadcastPeriod&0xFF);
            debuginfo(("BroadcastPeriod  = %d",BroadcastPeriod));
        }
        if(ui->checkBoxChnagePassword->isChecked()) {
            debuginfo(("Change password."));
            edit_info.change_password = 1;
            if(ui->lineEditPassword->text().isEmpty()) {
                memset(edit_info.password,0,sizeof(edit_info.password));
                edit_info.cncryption_mode = 0;
            } else {
                QString pwd = ui->lineEditPassword->text();
                pwd.resize(sizeof(edit_info.password) + 1);
                memcpy(edit_info.password,pwd.toAscii().data(),sizeof(edit_info.password));
                unsigned int len = strlen(edit_info.password);
                len = (len > sizeof(edit_info.password))?sizeof(edit_info.password):len;
                edit_info.password[len] = 0;
                edit_info.cncryption_mode = 1;
            }
        } else {
            edit_info.change_password = 0;
        }
        edit_info.to_host = 0;
        edit_info.system_fun_option = 0;
        edit_info.change_ipconfig = 0;
        item->pdevice->WriteNewDeviceInfoToDevice(&edit_info);
    }
}

void EditParamDialog::ResetClicked(bool)
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
