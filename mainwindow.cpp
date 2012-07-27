#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QModelIndex>
#include "QCheckBoxDelegate.h"
#include "QOnOffPushButton.h"
#include "CDeviceDelegate.h"
#include "QIpAddrDelegate.h"
#include "QDeviceMainGroupDelegate.h"
#include "QDeviceStatusDelegate.h"
#include "editparamdialog.h"
#include "qeditipconfigdialog.h"
#include "qpasswordmangerdialog.h"
#include "qioexpendsettingdialog.h"
 #include <QCoreApplication>
#include "AboutDialog.h"
#include <QThread>


#include "debug.h"

#define THISINFO            0
#define THISERROR           1
#define THISASSERT          1





QDeviceControlWidget::QDeviceControlWidget(QWidget * parent)
    : QTableWidget(parent)
{
    this->setIoExternSetAction = new QAction(tr("&IoExpendSetting..."), this);
    connect(setIoExternSetAction, SIGNAL(triggered()), this, SLOT(IoSettingDialog()));
}

void QDeviceControlWidget::contextMenuEvent(QContextMenuEvent *event)
{
#if 1 //尚未实现
    debuginfo(("at context event."));
    const QPoint & pos = event->pos();
    QTableWidgetItem * item = this->itemAt(pos);
    if(item) {
        QVariant var = item->data(0);
       RelayDeviceSharePonterType pdev = qVariantValue<RelayDeviceSharePonterType>(var);
        if(pdev) {
            debuginfo(("item is exist: item type:(%d,%d)%s",item->row(),item->column(),pdev->GetDeviceName().toAscii().data()));
            QMenu menu(this);
            setIoExternSetAction->setData(var);
            menu.addAction(setIoExternSetAction);
            menu.exec(event->globalPos());
        } else {
            debuginfo(("pdev is not valid!"));
        }
    } else {
        debuginfo(("item is not exist: item type"));
    }
#endif
}

void  QDeviceControlWidget::IoSettingDialog(void)
{
    QAction * act = (QAction *)sender();
    QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(act->data());
    //debuginfo((" this io devcie is = %s",pdev->GetDeviceName().toAscii().data()));
    QIoExpendSettingDialog dlg(pdev);
    dlg.setWindowTitle(pdev->GetDeviceName());
    dlg.exec();
}

void  QDeviceControlWidget::RemoveOneItemActioN(void)
{
}



QDelay::QDelay()
{
}
void QDelay::run()
{
}

void QDelay::delay(unsigned int ms)
{
    this->sleep(ms);
}































MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    CreateDevcieTable();
    CreateAction();
    CreateMenu();
    createContextMenu();

    QGridLayout *mainLayout = new QGridLayout;
        mainLayout->addWidget(deviceGroupBox, 0, 0, 1, 2);
        centralWidget->setLayout(mainLayout);
        this->resize(1000,300);
        setWindowTitle(tr("Remote multiple device manger"));

        statusBar()->showMessage(tr("Ready"));

        //初始化UDP接口
        InitUdpSocket();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::sleep(unsigned int ms)
{
    QDelay d;
    d.delay(ms);
}

void MainWindow::DevcieAckStstus(QString ackstr)
{
    statusBar()->showMessage(tr("Status:")+ackstr);
}

void MainWindow::CreateAction(void)
{
    this->quitact = new QAction(tr("&Quit"), this);
    connect(quitact, SIGNAL(triggered()), this, SLOT(Quit()));
    //
    this->edit_device_param_act = new QAction(tr("&Edit device parameter..."), this);
    connect(edit_device_param_act, SIGNAL(triggered()), this, SLOT(EditDeviceParam()));

    this->cleardevicetable = new QAction(tr("&Clear all device"), this);
    connect(cleardevicetable, SIGNAL(triggered()), this, SLOT(ClearDeviceTable()));

    this->edit_device_ipconfig = new QAction(tr("&Change deivce ip config..."), this);
    connect(edit_device_ipconfig, SIGNAL(triggered()), this, SLOT(EditDeviceIpconfig()));

    password_manger = new QAction(tr("&Communication password..."),this);
    connect(password_manger,SIGNAL(triggered()),this,SLOT(PasswordConfig()));

    secect_all = new QAction(tr("&Selected all device"),this);
    connect(secect_all,SIGNAL(triggered()),this,SLOT(SelectAll()));
    desecect_all = new QAction(tr("&Deselected all device"),this);
    connect(desecect_all,SIGNAL(triggered()),this,SLOT(DesecectAll()));

    open_all_device = new QAction(tr("&Set all selected device output ON"),this);
    connect(open_all_device,SIGNAL(triggered()),this,SLOT(OpenAllListDeviceIoOutput()));
    close_all_device = new QAction(tr("&Set all selected device output OFF"),this);
    connect(close_all_device,SIGNAL(triggered()),this,SLOT(CloseAllListDeviceIoOutput()));

    selectChineseLanguage = new QAction(tr("Chinese"),this);
    connect(selectChineseLanguage,SIGNAL(triggered()),this,SLOT(SelectChineseLanguageAction()));

    about_act = new QAction(tr("About"),this);
    connect(about_act,SIGNAL(triggered()),this,SLOT(AboutAction()));


    //系统后台图标
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));


    sysicon.addPixmap(QPixmap(":/sys/sys_icon/sources/LOGO.png"));
    trayIconMenu = new QMenu(this);
    //trayIconMenu->addAction(minimizeAction);
    //trayIconMenu->addAction(maximizeAction);

    //trayIconMenu->addSeparator();
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitact);

    trayIcon = new QSystemTrayIcon(sysicon,this);
    trayIcon->setContextMenu(trayIconMenu);

    setWindowIcon(sysicon);
    trayIcon->setIcon(sysicon);
    trayIcon->setVisible(true);
    trayIcon->setToolTip(this->windowTitle());
    trayIcon->show();

}
void MainWindow::showMinimized(void)
{
    debuginfo(("shwo minized..."));
}

void MainWindow::changeEvent(QEvent * event )
{
     if(windowState() & Qt::WindowMinimized)
     {
         debuginfo(("change event."));
         //setWindowState(windowState() & ~Qt::WindowMinimized);
         //hide();
      //此处无论event->accept()或者event->ignore()都不好使。
     }
}

void MainWindow::showEvent ( QShowEvent * event )
{
    debuginfo(("show event ."));
    setVisible(true);
}

void MainWindow::hideEvent ( QHideEvent * event )
{
    debuginfo(("hide event"));
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information;
        trayIcon->showMessage(tr("Note!"),tr("The program will keep running."), icon, 0);
        hide();
        event->ignore();
    }
}

void MainWindow::setVisible(bool visible)
{
     debuginfo(("set visible..."));
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::CreateMenu(void)
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(quitact);
    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(edit_device_param_act);
    toolsMenu->addAction(edit_device_ipconfig);
    toolsMenu->addAction(password_manger);
    toolsMenu->addSeparator();
    toolsMenu->addAction(cleardevicetable);
#if 0
    toolsMenu->addSeparator();
    toolsMenu->addAction(selectChineseLanguage);
#endif
    toolsMenu = menuBar()->addMenu(tr("&Operation"));
    toolsMenu->addAction(secect_all);
    toolsMenu->addAction(desecect_all);
    toolsMenu->addAction(open_all_device);
    toolsMenu->addAction(close_all_device);
    toolsMenu = menuBar()->addMenu(tr("Help"));
    toolsMenu->addAction(about_act);



#if     1 //for debug
    password_item itm;
    itm.alias = "admin";
    itm.pwd = "admin";
    password_list.push_back(itm);
#endif
     //QList<password_item>   password_list;
}

void MainWindow::createContextMenu(void)
{
    deviceTable->setContextMenuPolicy(Qt::DefaultContextMenu); //Qt::ActionsContextMenu);
    deviceTable->addAction(password_manger);
    deviceTable->addAction(cleardevicetable);
}

void MainWindow::Quit(void)
{
    trayIcon->setVisible(false);
    close();
}

void MainWindow::EditDeviceParam(void)
{    
    EditParamDialog dialog;

    this->update();

    int row = deviceTable->rowCount();
    for(int i=0;i<row;i++) {
        QTableWidgetItem * item = deviceTable->item(i,0);
        QVariant var = item->data(0);
        QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(var);
        if(pdev->is_checked) {
            dialog.InsertDevice(pdev);
        }
    }
    dialog.exec();
}
void MainWindow::EditDeviceIpconfig(void)
{
    QEditIpConfigDialog dialog;
    this->update();

    int row = deviceTable->rowCount();
    for(int i=0;i<row;i++) {
        QTableWidgetItem * item = deviceTable->item(i,0);
        QVariant var = item->data(0);
        QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(var);
        if(pdev->is_checked) {
            dialog.InsertDevice(pdev);
        }
    }
    dialog.exec();
}

void MainWindow::PasswordConfig(void)
{
    QPasswordMangerDialog dialog;
    this->update();
    dialog.InitPasswordList(password_list);
    dialog.exec();
    if(dialog.is_ok) {
        password_list = dialog.GetPasswordList();
        debuginfo(("is ok,get password count:%d",password_list.count()));
    } else {
        debuginfo(("is cancel"));
    }
}

void MainWindow::ClearDeviceTable(void)
{
    int rowcount = deviceTable->rowCount();
    for(int i=0;i<rowcount;i++) {
        deviceTable->removeRow(0);
    }
    mydevicemap.clear();
    mydevicemap.empty();
}
void MainWindow::SelectAll(void)
{
    int rowcount = deviceTable->rowCount();
    for(int i=0;i<rowcount;i++) {
        QTableWidgetItem * item = deviceTable->item(i,0);
        QVariant var = item->data(0);
        QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(var);
        pdev->is_checked = true;
        pdev->Updata();
    }
}
void MainWindow::DesecectAll(void)
{
    int rowcount = deviceTable->rowCount();
    for(int i=0;i<rowcount;i++) {
        QTableWidgetItem * item = deviceTable->item(i,0);
        QVariant var = item->data(0);
        QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(var);
        pdev->is_checked = false;
        pdev->Updata();
    }
}
void MainWindow::OpenAllListDeviceIoOutput(void)
{
    int rowcount = deviceTable->rowCount();
    for(int i=0;i<rowcount;i++) {
        QTableWidgetItem * item = deviceTable->item(i,0);
        QVariant var = item->data(0);
        QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(var);
        if(pdev->is_checked) {
            QBitArray bitmsk;
            bitmsk.resize(pdev->GetIoOutNum());
            bitmsk.fill(true,pdev->GetIoOutNum());
            pdev->MultiIoOutSet(0,bitmsk);
        }
    }
}

void MainWindow::CloseAllListDeviceIoOutput(void)
{
    int rowcount = deviceTable->rowCount();
    for(int i=0;i<rowcount;i++) {
        QTableWidgetItem * item = deviceTable->item(i,0);
        QVariant var = item->data(0);
        QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(var);
        if(pdev->is_checked) {
            QBitArray bitmsk;
            bitmsk.resize(pdev->GetIoOutNum());
            bitmsk.fill(false,pdev->GetIoOutNum());
            pdev->MultiIoOutSet(0,bitmsk);
        }
    }
}

void MainWindow::CreateDevcieTable(void)
{
    unsigned int index = 0;
        deviceGroupBox = new QGroupBox(tr("DeviceTable"));
        deviceTable = new QDeviceControlWidget;
        deviceTable->setSelectionMode(QAbstractItemView::NoSelection);
        deviceTable->setItemDelegateForColumn(index++,new QCheckBoxDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QDeviceNameDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QIpAddressDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QDeviceMainGroupDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QDeviceSlaveGroupDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QRelayValueSingalChannalButtonDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QSetOnPushDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QSetOffPushDelegate(this));
        deviceTable->setItemDelegateForColumn(index++,new QDeviceStatusDelegate(this));
#if APP_DISPLAY_TIME
        deviceTable->setItemDelegateForColumn(index++,new QDeviceTimeDelegate(this));
#endif

        QStringList labels;
    //! [22] //! [23]
        labels << tr("S") << tr("DeviceName") << tr("IpAddr")<<tr("Group1") << tr("Group2")<<tr("Control")<<tr("AllOn")<<tr("AllOff")<<tr("Status");
#if APP_DISPLAY_TIME
        labels << tr("DeviceTime");
#endif

        deviceTable->horizontalHeader()->setDefaultSectionSize(100);
        deviceTable->setColumnCount(labels.size());
        deviceTable->setHorizontalHeaderLabels(labels);
        //deviceTable->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
        index = 0;
        deviceTable->horizontalHeader()->resizeSection(index++,50);
        deviceTable->horizontalHeader()->resizeSection(index++,120);
        deviceTable->horizontalHeader()->resizeSection(index++,100);
        deviceTable->horizontalHeader()->resizeSection(index++,80);
        deviceTable->horizontalHeader()->resizeSection(index++,80);

        //deviceTable->horizontalHeader()->setResizeMode(index, QHeaderView::Fixed);
        deviceTable->horizontalHeader()->resizeSection(index++,300);

        deviceTable->horizontalHeader()->resizeSection(index++,60);
        deviceTable->horizontalHeader()->resizeSection(index++,60);
        deviceTable->horizontalHeader()->resizeSection(index++,50);
#if APP_DISPLAY_TIME
        deviceTable->horizontalHeader()->resizeSection(index++,120);
#endif
        deviceTable->verticalHeader()->hide();
        deviceTable->hideColumn(index-1);
        //排版
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(deviceTable);
        deviceGroupBox->setLayout(layout);
}

void MainWindow::manualAddDevice(int index)
{
}
 void MainWindow::InsertDevice(QSharedPointer<QRelayDeviceControl> & pdev)
 {
     int row = deviceTable->rowCount();
     deviceTable->setRowCount(row + 1);
     QTableWidgetItem *item0 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item1 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item2 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item3 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item4 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item5 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item6 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item7 = new CDeviceTableWidgetItem(pdev);
     QTableWidgetItem *item8 = new CDeviceTableWidgetItem(pdev);
#if APP_DISPLAY_TIME
     QTableWidgetItem *item9 = new CDeviceTableWidgetItem(pdev);
#endif
     //
     int index = 0;
     deviceTable->setItem(row, index++, item0);
     deviceTable->setItem(row, index++, item1);
     deviceTable->setItem(row, index++, item2);
     deviceTable->setItem(row, index++, item3);
     deviceTable->setItem(row, index++, item4);
     deviceTable->setItem(row, index++, item5);
     deviceTable->setItem(row, index++, item6);
     deviceTable->setItem(row, index++, item7);
     deviceTable->setItem(row, index++, item8);
#if APP_DISPLAY_TIME
     deviceTable->setItem(row, index++, item9);
#endif

    // item0->setCheckState(Qt::Checked);
    // item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);//表格只读属性

     deviceTable->openPersistentEditor(item0);  //不用双击就可以显示控件的形式
     deviceTable->openPersistentEditor(item2);  //不用双击就可以显示控件的形式
     deviceTable->openPersistentEditor(item5);  //不用双击就可以显示控件的形式
     deviceTable->openPersistentEditor(item6);  //不用双击就可以显示控件的形式
     deviceTable->openPersistentEditor(item7);
     deviceTable->openPersistentEditor(item8);
#if APP_DISPLAY_TIME
     //deviceTable->openPersistentEditor(item9);
#endif
     //
     //添加信号
     //updateGeometries
     //modelReset
     connect(pdev.data(),SIGNAL(DeviceInfoChanged(QString)),this,SLOT(DeviceInfoChanged(QString))); //,Qt::QueuedConnection);
     connect(pdev.data(),SIGNAL(DeviceAckStatus(QString)),this,SLOT(DevcieAckStstus(QString)));
 }

void MainWindow::InitUdpSocket(void)
{
    QSharedPointer<QUdpSocket>  pudp(new QUdpSocket);
    pUdpSocket = pudp;
    pUdpSocket->bind(505);
    connect(pUdpSocket.data(), SIGNAL(readyRead()),
                 this, SLOT(UdpreadPendingDatagrams()));
}

void MainWindow::UdpreadPendingDatagrams()
{
    while (pUdpSocket->hasPendingDatagrams()) {
             QByteArray datagram;
             datagram.resize(pUdpSocket->pendingDatagramSize());
             QHostAddress sender;
             quint16 senderPort;

             pUdpSocket->readDatagram(datagram.data(), 1024,&sender, &senderPort);

             processTheDeviceData(datagram,sender,senderPort);
         }
}

void MainWindow::processTheDeviceData(QByteArray & data,
                                      QHostAddress & sender,quint16 senderport)
{
    //解析报文，分发命令
    QString portstr;
    portstr.sprintf("%d",senderport);
    QString str = sender.toString();
    str += ":";
    str += portstr;
    //debuginfo(("precess udp rx data len(%d)",data.size()));

    //根据IP地址和端口号，查找已经有的数据，如果有了，给它发消息，让他自己处理自己的事情
    QMap<QString,QSharedPointer<QRelayDeviceControl> >::const_iterator i = mydevicemap.find(str);
    if(i != mydevicemap.end()) {
        //debuginfo(("found device,send rx data"));
        (*i)->SendRxData(data,this->password_list);
        //DeviceInfoChanged(str);
    } else {
        //debuginfo(("Not found device, try to insert one device"));
        QSharedPointer<QRelayDeviceControl> pdev(new QRelayDeviceControl(this));
        pdev->InitDeviceAddress(sender,senderport,pUdpSocket);
        pdev->SendRxData(data,this->password_list);
        if(pdev->devcie_info_is_useful()) {
            mydevicemap.insert(str,pdev);
            InsertDevice(pdev);
            pdev->index = mydevicemap.size();
        }
    }
    //如果没有找到，则创建一个，然后再转发消息给他处理
    //设备对象是一个完整的对象，可以处理和拥有自己数据
}

void MainWindow::DeviceInfoChanged(QString  hostaddrID)
{
   // debuginfo(("host:%s",hostaddrID.toAscii().data()));
    QMap<QString,QSharedPointer<QRelayDeviceControl> >::const_iterator it = mydevicemap.find(hostaddrID);
    if(it != mydevicemap.end()) {
        int row = deviceTable->rowCount();
        for(int i=0;i<row;i++) {
            QTableWidgetItem * item = deviceTable->item(i,0);
            QVariant var = item->data(0);
            QSharedPointer<QRelayDeviceControl> pdev = qVariantValue<RelayDeviceSharePonterType>(var);
            if((*it) == pdev) {
                QAbstractItemModel * model = deviceTable->model ();
                int index_max = 9;
#if APP_DISPLAY_TIME
                index_max++;
#endif
                for(int j=0;j<index_max;j++) {
                    QModelIndex index = model->index(i,j);
                    deviceTable->update(index);
                }
                emit DeviceUpdata(pdev);
                break;
            }
        }
    }
}


void MainWindow::retranslateUI(void)
{
    deviceGroupBox->setTitle(tr("DeviceTable"));
    quitact->setText(tr("Exit"));
    edit_device_param_act->setText(tr("Edit Device Parameter..."));
    edit_device_ipconfig->setText(tr("Edit Ip Config..."));
}

void MainWindow::SelectChineseLanguage(void)
{
    retranslateUI();
}



void MainWindow::AboutAction(void)
{
    AboutDialog dlg;
    dlg.exec();
}
