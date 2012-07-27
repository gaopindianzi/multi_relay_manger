#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemDelegate>
#include <QCheckBox>
#include <QGroupBox>
#include <QTableWidget>
#include <QUdpSocket>
#include <QSharedPointer>
#include "QRelayDeviceControl.h"
#include "CDeviceTableWidgetItem.h"
#include <QMap>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <QThread>
#include "PasswordItemDef.h"

#define    APP_DISPLAY_TIME             1


void dumpthisdata(const char * buffer,int len);

namespace Ui {
    class MainWindow;
}


class QDeviceControlWidget : public QTableWidget
{
    Q_OBJECT
public:
    QDeviceControlWidget(QWidget * parent = 0);    
private slots:
    void  IoSettingDialog(void);
    void  RemoveOneItemActioN(void);
private:   
    void contextMenuEvent(QContextMenuEvent *event);
    QAction * setIoExternSetAction;
};



class QDelay : public QThread
{
public:
    QDelay();
public:
    void run();
    void delay(unsigned int ms);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void CreateDevcieTable(void);
    void CreateAction(void);
    void CreateMenu(void);
    void createContextMenu(void);
    void manualAddDevice(int index);
    void InsertDevice(QSharedPointer<QRelayDeviceControl> & pdev);    
    void retranslateUI(void);
protected: //Ðéº¯Êý
    void setVisible(bool visible);
    void changeEvent(QEvent * event );
    void hideEvent ( QHideEvent * event );
    void showEvent ( QShowEvent * event );
    void closeEvent(QCloseEvent *event);
public:
    void InitUdpSocket(void);
    void processTheDeviceData(QByteArray & data,QHostAddress & sender,quint16 senderport);
    void sleep(unsigned int ms);
private slots:
    void showMinimized(void);
    void UdpreadPendingDatagrams();
    void DeviceInfoChanged(QString hostaddrID);
    void Quit(void);
    void EditDeviceParam(void);
    void EditDeviceIpconfig(void);
    void ClearDeviceTable(void);
    void PasswordConfig(void);
    void DevcieAckStstus(QString ackstr);
    void SelectAll(void);
    void DesecectAll(void);
    void OpenAllListDeviceIoOutput(void);
    void CloseAllListDeviceIoOutput(void);
    void SelectChineseLanguage(void);
    void AboutAction(void);
signals:
    void DeviceUpdata(QSharedPointer<QRelayDeviceControl> & pdev);
private:
    QAction * quitact;
    QAction * edit_device_param_act;
    QAction * edit_device_ipconfig;
    QAction * cleardevicetable;
    QAction * password_manger;
    QAction * secect_all;
    QAction * desecect_all;
    QAction * open_all_device;
    QAction * close_all_device;
    QAction * selectChineseLanguage;
    QAction * about_act;
    QMenu *fileMenu;
    QMenu *toolsMenu;
private:
    QWidget *centralWidget;
    //device table
    QGroupBox *deviceGroupBox;
    QDeviceControlWidget *deviceTable;
private:
    QSharedPointer<QUdpSocket>  pUdpSocket;
    QMap<QString,QSharedPointer<QRelayDeviceControl> >   mydevicemap;
    QList<password_item>   password_list;
private:
    Ui::MainWindow *ui;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QIcon                       sysicon;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // MAINWINDOW_H
