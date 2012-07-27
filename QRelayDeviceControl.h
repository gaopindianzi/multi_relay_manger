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

#define  TCP_CMD_POWER_UP              0
#define  TCP_CMD_IDLE                         1
#define  TCP_CMD_CONNECTING         2
#define  TCP_CMD_WAIT_LINE_OK       3
#define  TCP_CMD_WAIT_ACK               4


//sys_init_bitmask��λ����
#define SYS_IO_NAME           (1<<0)
#define SYS_IO_TIME            (1<<1)

class QRelayDeviceControl : public QObject
{
    Q_OBJECT
public:
    QRelayDeviceControl(QObject * parent);
    virtual ~QRelayDeviceControl();
signals:
    void DeviceInfoChanged(QString hostaddrID);
    void DeviceAckStatus(QString status);
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
    QSharedPointer<device_info_st> & GetDeviceInfo(void) { return pdev_info; }
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
private:  //TCP�ӿ�����
    QTcpSocket    tcp_socket;
    unsigned int    tcp_port;
    QTimer   tcp_updata_timer;
    //����ͼ���Ʊ���
    int                      tcp_sys_state;
    unsigned int     sys_timeout_count;
    QString              sys_current_status;
    unsigned int      cmd_index;
    int                       tcp_cmd_number;
    unsigned int      tcp_ack_timeout_count;
    //��ʼ��λ����
    unsigned char  sys_init_bitmask;
    //����������ͼ���Ʊ���
    unsigned int      io_out_name_index;
    unsigned int      io_out_name_count;
    QVector<QString>  io_out_names;
    //��д��ʱ������ͼ���Ʊ���
    unsigned int      io_out_time_index;
    unsigned int      io_out_time_count;
    QVector<timing_node>  io_out_timing_list;
    //״̬������
    void  SetTcpSysStatus(int newState,QString string);
    void  TcpStartReadIoNames(void);
    void  TcpReadIoNames(void);
    void  TcpAckIoNames(QByteArray & buffer);
    //��ʱ
    void  TcpStartReadTimimgs(void);
    void  TcpReadTimimgs(void);
    void TcpAckReadTimimgs(QByteArray & buffer);
    //����Ķ���
private slots:
    void	tcpconnected ();
    void	tcpdisconnected ();
    void	tcpreadyRead ();
    void tcp_timer();
public:
    bool       is_checked;
    bool       is_online;
    int          index;
};

typedef QSharedPointer<QRelayDeviceControl>   RelayDeviceSharePonterType;

Q_DECLARE_METATYPE(RelayDeviceSharePonterType)


#endif // QRELAYDEVICECONTROL_H
