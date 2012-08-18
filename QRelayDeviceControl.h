#ifndef QRELAYDEVICECONTROL_H
#define QRELAYDEVICECONTROL_H

#include <QObject>
#include <QSharedPointer>
#include <QUdpSocket>
#include <QTcpSocket>
 #include <QBitArray>
#include <QTimer>
#include <QTableWidgetItem>
#include <QVector>
#include "multimgr_device_dev.h"
#include "CDeviceDelegate.h"
#include "rc4.h"
#include "PasswordItemDef.h"
#include "command_datatype.h"
#include <QDateTime>

#define  TCP_CMD_POWER_UP              0
#define  TCP_CMD_IDLE                         1
#define  TCP_CMD_CONNECTING         2
#define  TCP_CMD_WAIT_LINE_OK       3
#define  TCP_CMD_WAIT_ACK               4


//sys_init_bitmask的位定义
#define SYS_IO_NAME           (1<<0)
#define SYS_IO_TIME            (1<<1)
#define SYS_IO_VAL              (1<<2)

class QRelayDeviceControl : public QObject
{
    Q_OBJECT
public:
    QRelayDeviceControl(QObject * parent);
    virtual ~QRelayDeviceControl();
signals:
    void DeviceInfoChanged(QString hostaddrID);
    void DeviceAckStatus(QString status);
    //指令完成应答信号
    void DeviceWriteTimingFinished(void);
    void DeviceWriteOneTimingFinished(void);
    void DevcieWriteRtcFinihed(void);
    void DevcieReadTimingFinihed(void);
    void DevcieReadIoNameFinihed(void);
    void DeviceWriteIoNameFinished(int index,QString name);
public slots:
    void InitDeviceAddress(QHostAddress & addr,quint16 port,QSharedPointer<QUdpSocket>  & psocket);
    int   SendRxData(QByteArray & data,QList<password_item> & pwdlist);
    void Updata(void);
    void DeviceUpdate(void);
private slots:
    void TimeoutUpdataInfo(void);
public:
    void GetDevcieInfoFormDevcie(void);
    void SetDeviceInfo(QByteArray & data);
    int   GetDeviceRelayCount(void) { return relay_bitmask.size(); }
    bool devcie_info_is_useful(void) { return bdevcie_info_is_useful; }
    QString GetDeviceAddress(void);
    QString GetDeviceName(void);
    QString GetHostAddressString(void);
    QString GetBroadcastTime(void);
    QString GetListionPort(void);
    QString GetRemoteHostAddress(void);
    QString GetRemoteHostPort(void);
    QString GetDeviceTime(void);
    int         GetTimingNodeNum(void);
    int         GetTimingNodeNum(int index);
    void      WriteNewDeviceInfoToDevice(device_info_st * pst);
    void      SetDeviceName(QString newDeviceName);
    QString GetGroup1Name(void);
    void      SetGroup1Name(QString name);
    QString GetGroup2Name(void);
    void      SetGroup2Name(QString name);
    QString GetStatus(void);
    QString GetAckStatus(void);
    int        GetIoOutNum(void);
    QString GetDeviceModelName(void);
    QString GetDeviceIoOutName(int num);
    void      ReadIoOut(void);
    int        ReadIoOutAck(QByteArray & data);
    void      ConvertIoOutOneBitAndSendCmd(int bit);
    int        ConvertIoOutOneBitAndSendCmdAck(QByteArray & data);
    void      MultiIoOutSet(unsigned int start_index,QBitArray bit_mask);
    int        MultiIoOutSetAck(QByteArray & data);
    void      ResetDevice(void);
    QBitArray   relay_bitmask;
     bool           io_timing_initialized;
    QSharedPointer<device_info_st> & GetDeviceInfo(void) { return pdev_info; }
    QVector<timing_node> & GetDeviceIoOutTimingList(void) { return io_out_timing_list; }
    void     SetDeviceIoOutTimingList(QVector<timing_node> & timinglist);
    QDateTime ConvertTimeNodeToQDT(time_type & ttick);
    QDateTime GetDeviceDateTime(void);
private:
    void      SendCommandData(const char * buffer,int len);
private:
    QString devicename;
    QString devicegroup1;
    QString devicegroup2;
    QString devicestatus;
private:
    QHostAddress   deviceaddr;
    quint16              deviceport;
    QSharedPointer<QUdpSocket>      pUdpSocket;
    QSharedPointer<device_info_st>   pdev_info;
    bool       bdevcie_info_is_useful;
    bool       relay_bitmask_inited;
    int         online_timeout;
    bool       need_encryption;
    QString  password;
    //QEncryptRc4    rc4;
    QTimer * timer;
    QString   ack_status;
private:  //TCP接口数据
    QTcpSocket    tcp_socket;
    unsigned int    tcp_port;
    QTimer   tcp_updata_timer;
    //流程图控制变量
    int                      tcp_sys_state;
    unsigned int     sys_timeout_count;
    QString              sys_current_status;
    unsigned int      cmd_index;
    int                       tcp_cmd_number;
    unsigned int      tcp_ack_timeout_count;
    //初始化位变量
    unsigned char  sys_init_bitmask;
    //读名字流程图控制变量
    unsigned int      io_out_name_index;
    unsigned int      io_out_name_count;
    QVector<QString>  io_out_names;
    //读写定时器流程图控制变量
    unsigned int      io_out_time_index;
    unsigned int      io_out_time_count;
    QVector<timing_node>  io_out_timing_list;

    //状态机函数
    void  SetTcpSysStatus(int newState,QString string);
    void  TcpReadIoNames(void);
    void  TcpAckIoNames(QByteArray & buffer);
    //定时
    void  TcpReadTimimgs(void);
    void  TcpAckReadTimimgs(QByteArray & buffer);
    //TCP模式的读IO值
    void TcpReadIoInValue(void);
    void TcpReReadIoInValue(void);
    void TcpAckReadIoInValue(QByteArray & buffer);
    //TCP写定时器数据
    void TcpReWriteIoOutTiming(void);
    void TcpDoneWriteIoOutTiming(void);
    void TcpAckWriteIoOutTiming(QByteArray & buffer);
    //独立定时写
    //独立与非独立定时器写之间的模式转换，就靠这个变量
    bool tcpTimingAloneSetFlag;
    int    tcpTimingAloneIndex;

    void TcpReWriteOneTiming(void);
    void TcpAckWriteOneTiming(QByteArray & buffer);
    //TCP写RTC定时器
    void TcpReStartWriteRtc(void);
    void TcpDoneWriteRtc(void);
    void TcpAckWriteRtc(QByteArray & buffer);
    //写IO口名字
    void TcpAckWriteIoName(QByteArray & buffer);
    //多余的东东
public:
    void TcpStartReadIoNames(void);
    void TcpWriteIoName(unsigned char addr[2],QString name);
    void TcpStartReadTimimgs(void);
    void TcpStartWriteIoOutTiming(void);
    void TcpWriteOneTiming(int index,bool timevalid);
    void TcpStartWriteRtc(void);
private slots:
    void tcpconnected ();
    void tcpdisconnected ();
    void tcpreadyRead ();
    void tcp_timer();
public:
    bool       is_checked;
    bool       is_online;
    int          index;
};

typedef QSharedPointer<QRelayDeviceControl>   RelayDeviceSharePonterType;

Q_DECLARE_METATYPE(RelayDeviceSharePonterType)


#endif // QRELAYDEVICECONTROL_H
