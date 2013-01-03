#include "QRelayDeviceControl.h"
#include <QDebug>
#include <QString>
#include <QTimer>
#include <QMessageBox>
#include <QVector>
#include <QTime>
#include "modbus_interface.h"
#include "command_datatype.h"
#include "rc4.h"

#include "debug.h"
#define THISINFO           0
#define THISERROR          0
#define THISASSERT         0

uint16_t CRC16(unsigned char *Array,unsigned int Len);


void dumpthisdata(const char * buffer,int len)
{
    if(!THISINFO) {
        return ;
    }
    QString str;
    for(int i=0;i<len;i++) {
        QString tmp;
        tmp.sprintf("%2X,",(unsigned char)buffer[i]);
        str += tmp;
    }
    debuginfo(("%s",str.toAscii().data()));
}

QRelayDeviceControl::QRelayDeviceControl(QObject * parent) :
    QObject(parent)
{
    pdev_info = QSharedPointer<device_info_st>(new device_info_st);
    timer = new QTimer(this);
    //UDP��ʱ��
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeoutUpdataInfo()));
    //TCP��ʱ��
    connect(&tcp_updata_timer, SIGNAL(timeout()), this, SLOT(tcp_timer()));
    //
    timer->start(1000);
    tcp_updata_timer.start(1000);
    is_checked = false;
    online_timeout = 0;
    is_online = false;
    need_encryption = false;

    bdevcie_info_is_useful = false;
    relay_bitmask_inited  = false;
    io_timing_initialized = false;


    tcp_port = 2000;
    cmd_index = 0;
    SetTcpSysStatus(TCP_CMD_POWER_UP,tr("none"));
    sys_init_bitmask = SYS_IO_NAME|SYS_IO_TIME|SYS_IO_VAL;

    //TCP�ź�
    connect(&tcp_socket,SIGNAL(connected()),this,SLOT(tcpconnected()));
    connect(&tcp_socket,SIGNAL(disconnected()),this,SLOT(tcpdisconnected()));
    connect(&tcp_socket,SIGNAL(readyRead()),this,SLOT(tcpreadyRead()));


    ack_status = tr("none.");
    //rc4.SetKey((unsigned char *)"admin",strlen("admin"));
}

QRelayDeviceControl::~QRelayDeviceControl()
{
    tcp_socket.close();
}

void	QRelayDeviceControl::tcpconnected ()
{
    debuginfo(("tcp connected."));
    SetTcpSysStatus(TCP_CMD_IDLE,tr("idle"));
}
void	QRelayDeviceControl::tcpdisconnected ()
{    
    debuginfo(("tcp disconnected."));
    SetTcpSysStatus(TCP_CMD_WAIT_LINE_OK,tr("wait line ok..."));
}


QDateTime  QRelayDeviceControl::ConvertTimeNodeToQDT(time_type & ttick)
{
    QDate  d(ttick.year+1900,ttick.mon+1,ttick.day);
    QTime t(ttick.hour,ttick.min,ttick.sec);
    QDateTime dt(d,t);
    return dt;
}


void	QRelayDeviceControl::tcpreadyRead ()
{
    QByteArray arry;
    qint64 size = tcp_socket.bytesAvailable();
    arry.resize((int)size);
    tcp_socket.read(arry.data_ptr()->data,size);
    if(size >= sizeof(CmdHead)) {
        CmdHead  * pcmd = (CmdHead *)arry.data_ptr()->data;
        if(pcmd->cmd_index == cmd_index) {

        } else {
            debugerror(("tcp ack data error!"));
            return ;
        }
        switch(pcmd->cmd)
        {
        case CMD_GET_IO_NAME:
        {
            if(pcmd->cmd_len >= sizeof(CmdIoName)) {
                TcpAckIoNames(arry);
            }
        }
        break;
        case CMD_GET_TIMING_INFO:
        {
            if(tcpTimingAloneSetFlag == false) {
                TcpAckReadTimimgs(arry);
            } else {
            }
        }
        break;
        case CMD_SET_TIMING_INFO:
        {
            if(tcpTimingAloneSetFlag == false) {
                TcpAckWriteIoOutTiming(arry);
            } else {
                TcpAckWriteOneTiming(arry);
            }
        }
        break;
        case CMD_GET_IO_IN_VALUE:
        {
            TcpAckReadIoInValue(arry);
        }
        break;
        case CMD_SET_RTC_VALUE:
        {
            TcpAckWriteRtc(arry);
        }
        break;
        case CMD_SET_IO_NAME:
        {
            TcpAckWriteIoName(arry);
        }
        break;
        default:
            break;
        }
    }
}

void QRelayDeviceControl::tcp_timer()
{
    if(tcp_sys_state == TCP_CMD_WAIT_LINE_OK) {
        if(++sys_timeout_count >= 5) {
            sys_init_bitmask |= SYS_IO_NAME;
            sys_init_bitmask |= SYS_IO_TIME;
            tcp_socket.connectToHost(GetDeviceAddress(),tcp_port);
            SetTcpSysStatus(TCP_CMD_CONNECTING,tr("tcp connecting to the device..."));
        }
    } else if(tcp_sys_state == TCP_CMD_CONNECTING) {
        if(++sys_timeout_count >= 30) {
            tcp_socket.close();
            SetTcpSysStatus(TCP_CMD_WAIT_LINE_OK,tr("tcp waiting line ok..."));
        }
    } else if(tcp_sys_state == TCP_CMD_IDLE) {
        if(sys_init_bitmask & SYS_IO_NAME) {
            sys_init_bitmask &= ~SYS_IO_NAME;
            sys_timeout_count = 0;
            TcpStartReadIoNames();
        } else if(sys_init_bitmask & SYS_IO_TIME) {
            debuginfo(("init bit io times..."));
            sys_init_bitmask &= ~SYS_IO_TIME;
            TcpStartReadTimimgs();
        } else if(sys_init_bitmask&SYS_IO_VAL) {
            sys_init_bitmask &= ~SYS_IO_VAL;
            sys_timeout_count = 0;
            TcpReadIoInValue();
        } else if(++sys_timeout_count >= 8)  {  //����10���ӣ���λ���ǻ��Զ��Ͽ��ġ�
            sys_init_bitmask |= SYS_IO_VAL;  //��ʱ����
            //�ض����ָ���!��Ϊ��Ҫ�û�ʵʱ��Ӧ������û���������
            //���Ǳ���û��Ͳ��������ˡ�
            //ʵ��ʹ�������������һ��ʱ����һ�¶Ͽ��������ӣ�
            //�����û�û��ʵ���޸ĵ�����£����Ͽ����ӣ��Թ������û�����
        }
    } else if(tcp_sys_state == TCP_CMD_WAIT_ACK) {
        if(++sys_timeout_count >= 8) {
            //ָ�ʱ
            SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("sending command timeout,try to wait again..."));
            if(++tcp_ack_timeout_count > 3) {
                //����������
                SetTcpSysStatus(TCP_CMD_WAIT_LINE_OK,tr("sending command timeout,close the tcp connect and wait sometime..."));
            } else {
                //�ж�ʲôָ����·���һ��
                if(tcp_cmd_number == CMD_GET_IO_NAME) {
                    TcpReadIoNames();
                } else if(tcp_cmd_number == CMD_GET_TIMING_INFO) {
                    if(tcpTimingAloneSetFlag == false) {
                        TcpReadTimimgs();
                    } else {
                    }
                } else if(tcp_cmd_number == CMD_GET_IO_IN_VALUE) {
                    TcpReReadIoInValue();
                } else if(tcp_cmd_number == CMD_SET_TIMING_INFO) {
                    if(tcpTimingAloneSetFlag == false) {
                        TcpReWriteIoOutTiming();
                    } else {
                        TcpReWriteOneTiming();
                    }
                } else if(tcp_cmd_number == CMD_SET_RTC_VALUE) {
                    TcpReStartWriteRtc();
                } else { //���ָ��
                    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("continue wait command ack..."));
                }
            }
        }
    }
}

QString QRelayDeviceControl::GetDeviceIoOutName(int num)
{
    if(num < io_out_names.size()) {
        return io_out_names[num];
    }
    return "";
}

void QRelayDeviceControl::TcpReadIoInValue(void)
{
     tcp_ack_timeout_count = 0;
     TcpReReadIoInValue();
}

void QRelayDeviceControl::TcpReReadIoInValue(void)
{
    debuginfo(("start read io in values."));
    QByteArray data;
    data.resize(sizeof(CmdHead));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    pcmd->cmd = tcp_cmd_number = CMD_GET_IO_IN_VALUE;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = 0;
    pcmd->cmd_option = 0;
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("start reading io In values..."));
    this->tcp_socket.write(data.data(),data.size());
}

void QRelayDeviceControl::TcpAckReadIoInValue(QByteArray & buffer)
{
    const CmdHead * rcmd = (const CmdHead *)buffer.data();
    if(rcmd->cmd_len >= sizeof(CmdIobitmap)) {
        const CmdIoValue    *  sio  = (const CmdIoValue *)GET_CMD_DATA(rcmd);
        qint32 msk = sio->io_value[3];
        msk <<= 8;
        msk |= sio->io_value[2];
        msk <<= 8;
        msk |= sio->io_value[1];
        msk <<= 8;
        msk |= sio->io_value[0];
        SetTcpSysStatus(TCP_CMD_IDLE,tr("finished reading io in values."));
        debuginfo(("ack read io value finished."));
    } else {
        debuginfo(("ack read io value error!!!!"));
    }
}

void  QRelayDeviceControl::TcpStartReadIoNames(void)
{
    QByteArray data;

    io_out_name_index = 0;
    io_out_name_count =  GetIoOutNum();
    tcp_ack_timeout_count = 0;

    data.resize(sizeof(CmdHead) + sizeof(CmdIoName));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    CmdIoName * pio = (CmdIoName *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_GET_IO_NAME;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(CmdIoName);
    pcmd->cmd_option = 0;
    pio->io_addr[0] = (unsigned char)(io_out_name_index & 0xFF);
    pio->io_addr[1] = (unsigned char)(io_out_name_index >> 8);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("start reading io names..."));
    this->tcp_socket.write(data.data(),data.size());
}

void  QRelayDeviceControl::TcpReadIoNames(void)
{
    QByteArray data;
    data.resize(sizeof(CmdHead) + sizeof(CmdIoName));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    CmdIoName * pio = (CmdIoName *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_GET_IO_NAME;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(CmdIoName);
    pcmd->cmd_option = 0;
    pio->io_addr[0] = (unsigned char)(io_out_name_index & 0xFF);
    pio->io_addr[1] = (unsigned char)(io_out_name_index >> 8);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("continue reading io names..."));
    this->tcp_socket.write(data.data(),data.size());
}

void  QRelayDeviceControl::TcpAckIoNames(QByteArray & buffer)
{
    CmdHead  * pcmd = (CmdHead *)buffer.data_ptr()->data;
    CmdIoName * pio = (CmdIoName *)GET_CMD_DATA(pcmd);
    //����ĳ·������
    QString aryname(QString::fromLocal8Bit((const char *)pio->io_name));
    debuginfo(("read back io anme:%s",aryname.toAscii().data()));
    io_out_names.resize(this->GetIoOutNum());
    int num = pio->io_addr[1];
    num <<= 8;
    num |= pio->io_addr[0];
    if(num < this->GetIoOutNum()) {
        io_out_names[num] = aryname;
    }
    if(++io_out_name_index >= io_out_name_count) {
        //����������
        sys_init_bitmask &= ~SYS_IO_NAME;
        SetTcpSysStatus(TCP_CMD_IDLE,tr("finished reading io names..."));
        emit DevcieReadIoNameFinihed();
    } else {
        //����������
        TcpReadIoNames();
    }
    tcp_ack_timeout_count = 0;
}

void QRelayDeviceControl::TcpWriteIoName(unsigned char addr[2],QString name)
{
    QByteArray data;

    tcp_ack_timeout_count = 0;

    data.resize(sizeof(CmdHead) + sizeof(CmdIoName));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    CmdIoName * pio = (CmdIoName *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_SET_IO_NAME;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(CmdIoName);
    pcmd->cmd_option = 0;
    //
    pio->io_addr[0] = addr[0];
    pio->io_addr[1] = addr[1];
    const char * pstr = name.toAscii().data();
    char str[20];
    int len = strlen(pstr);
    len = (len>=19)?19:len;
    memcpy(str,pstr,len);
    str[len] = 0;
    memcpy((char *)&pio->io_name[0],str,20);
    //
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("start reading io names..."));
    this->tcp_socket.write(data.data(),data.size());
}

void QRelayDeviceControl::TcpAckWriteIoName(QByteArray & buffer)
{
    CmdHead  * pcmd = (CmdHead *)buffer.data_ptr()->data;
    CmdIoName * pio = (CmdIoName *)GET_CMD_DATA(pcmd);
    //����ĳ·������
    QString aryname(QString::fromLocal8Bit((const char *)pio->io_name));
    debuginfo(("write back io name:%s",aryname.toAscii().data()));


    int adindex = pio->io_addr[1];
    adindex <<= 8;
    adindex += pio->io_addr[0];
    io_out_names.resize(this->GetIoOutNum());
    if(adindex < this->GetIoOutNum()) {
        io_out_names[adindex] = aryname;
    }

    //qDebug("ack write name index=%d",adindex);

    SetTcpSysStatus(TCP_CMD_IDLE,tr("finished write io name..."));
    emit DeviceWriteIoNameFinished(adindex,aryname);
}



void  QRelayDeviceControl::TcpStartReadTimimgs(void)
{
    QByteArray data;

    debuginfo(("start read io timimes..."));

    tcpTimingAloneSetFlag = false;

    io_out_time_index = 0;
    io_out_time_count = 0;
    tcp_ack_timeout_count = 0;

    data.resize(sizeof(CmdHead));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    pcmd->cmd = tcp_cmd_number = CMD_GET_TIMING_INFO;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = 0;
    SET_CMD_STATE(pcmd,CMD_REQ_START);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("start reading io timings..."));
    this->tcp_socket.write(data.data(),data.size());
}

void  QRelayDeviceControl::TcpReadTimimgs(void)
{
    QByteArray data;

    debuginfo(("continue read io timimes..."));

    data.resize(sizeof(CmdHead));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    pcmd->cmd = tcp_cmd_number = CMD_GET_TIMING_INFO;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = 0;
    SET_CMD_STATE(pcmd,CMD_REQ_NEXT);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("continue reading io timings..."));
    this->tcp_socket.write(data.data(),data.size());
}
void QRelayDeviceControl::TcpAckReadTimimgs(QByteArray & buffer)
{
    CmdHead  * rcmd = (CmdHead *)buffer.data_ptr()->data;
    if(GET_CMD_STATE(rcmd) == CMD_CURRENT_START) {
        if(rcmd->cmd_len >= sizeof(timint_info)) {
            const timint_info * rio = (const timint_info *)GET_CMD_DATA(rcmd);
            io_out_time_count = rio->time_max_count; //��������Ҫ�ģ���ʱ������
            io_out_timing_list.clear();
            if(io_out_time_count) {
                //��Ҫ������
                debuginfo(("ack read io timimes, MAX = %d,start read.",io_out_time_count));
                TcpReadTimimgs();
            } else {
                //����Ҫ������
                debuginfo(("ack read io timimes,count=0 ,finished."));
                goto done_read;
            }
        } else {
            //�����ݳ���
        }
    }else if(GET_CMD_STATE(rcmd) == CMD_CURRENT_DOING) {
        if(rcmd->cmd_len >= sizeof(timing_node)) {
            //û��
            const timing_node * rio = (const timing_node *)GET_CMD_DATA(rcmd);
            if(io_out_time_index < io_out_time_count) {
                io_out_timing_list.push_back(*rio);
                debuginfo(("ack read io timimes,continue read %d node",io_out_time_index));
                io_out_time_index++;
                TcpReadTimimgs();
            } else {
                debuginfo(("ack read io timimes,finished read %d time nodes",io_out_time_count));
                goto done_read;
            }
        } else {
            //����ʱ����
        }
    }else if(GET_CMD_STATE(rcmd) == CMD_CURRENT_DONE) {
        debuginfo(("ack read io timimes,done read."));
done_read:
        sys_init_bitmask &= ~SYS_IO_TIME;
        SetTcpSysStatus(TCP_CMD_IDLE,tr("finished reading io timings."));
        io_timing_initialized = true;
        emit DevcieReadTimingFinihed();
    }
    tcp_ack_timeout_count = 0;
}

void  QRelayDeviceControl::SetDeviceIoOutTimingList(QVector<timing_node> & timinglist)
{
    io_out_timing_list = timinglist;
}

void QRelayDeviceControl::TcpWriteOneTiming(int index,bool timevalid)
{
    QByteArray data;

    tcpTimingAloneSetFlag = true;
    tcpTimingAloneIndex = index;

    io_out_time_index = index;
    io_out_time_count = 1;

    debuginfo(("start write io one timing"));


    tcp_ack_timeout_count = 0;

    data.resize(sizeof(CmdHead) + sizeof(CmdTimingNode));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    CmdTimingNode *  rio  = (CmdTimingNode *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_SET_TIMING_INFO;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(CmdTimingNode);
    rio->index = io_out_time_index;


    timing_node node = io_out_timing_list[io_out_time_index];
    if(timevalid) {
        SET_IO_TIME_VALID(&node,true);
    } else {
        SET_IO_TIME_VALID(&node,false);
    }
    rio->node = io_out_timing_list[io_out_time_index] = node;

    SET_CMD_STATE(pcmd,CMD_REQ_NEXT);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("writing io one timing..."));
    this->tcp_socket.write(data.data(),data.size());
}
void QRelayDeviceControl::TcpReWriteOneTiming(void)
{
    QByteArray data;

    tcpTimingAloneSetFlag = true;

    io_out_time_index = tcpTimingAloneIndex;
    io_out_time_count = 1;

    debuginfo(("start re write io one timing"));

    data.resize(sizeof(CmdHead) + sizeof(CmdTimingNode));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    CmdTimingNode *  rio  = (CmdTimingNode *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_SET_TIMING_INFO;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(CmdTimingNode);
    rio->index = io_out_time_index;
    rio->node = io_out_timing_list[io_out_time_index];
    SET_CMD_STATE(pcmd,CMD_REQ_NEXT);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("writing io one timing..."));
    this->tcp_socket.write(data.data(),data.size());
}

void QRelayDeviceControl::TcpAckWriteOneTiming(QByteArray & buffer)
{
    CmdHead  * pcmd = (CmdHead *)buffer.data_ptr()->data;
    if(GET_CMD_OK(pcmd)) {
        if(GET_CMD_STATE(pcmd) == CMD_CURRENT_DOING) {
            debuginfo(("tcp ack write one done,index(%d),all(%d)",io_out_time_index,io_out_time_count));
            SetTcpSysStatus(TCP_CMD_IDLE,tr("done writing one io timings!"));
            //emit DeviceWriteOneTimingFinished();
            ConvertIoOutOneBitAndSendCmd(io_out_time_index);
        } else {
            debugerror(("tcp ack write io out timing error! 2"));
        }
    } else {
        debugerror(("tcp ack write io out timing error! 1"));
    }
}

void QRelayDeviceControl::TcpStartWriteIoOutTiming(void)
{
    QByteArray data;

    tcpTimingAloneSetFlag = false;

    io_out_time_index = 0;
    io_out_time_count = io_out_timing_list.size();

    debuginfo(("start write io timing : all(%d)",io_out_time_count));


    tcp_ack_timeout_count = 0;

    data.resize(sizeof(CmdHead) + sizeof(timint_info));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    timint_info *  rio  = (timint_info *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_SET_TIMING_INFO;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(timint_info);
    rio->time_max_count = io_out_time_count;
    SET_CMD_STATE(pcmd,CMD_REQ_START);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("restart writing io timings..."));
    this->tcp_socket.write(data.data(),data.size());

}

void QRelayDeviceControl::TcpReWriteIoOutTiming(void)
{
    QByteArray data;

    debuginfo(("restart write io timing : %d",io_out_time_index));


    data.resize(sizeof(CmdHead) + sizeof(CmdTimingNode));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    CmdTimingNode *  rio  = (CmdTimingNode *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_SET_TIMING_INFO;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(CmdTimingNode);
    rio->index = io_out_time_index;
    rio->node = io_out_timing_list[io_out_time_index];
    SET_CMD_STATE(pcmd,CMD_REQ_NEXT);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("restart writing io timings..."));
    this->tcp_socket.write(data.data(),data.size());
}

void QRelayDeviceControl::TcpDoneWriteIoOutTiming(void)
{
    QByteArray data;
    data.resize(sizeof(CmdHead));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    pcmd->cmd = tcp_cmd_number = CMD_SET_TIMING_INFO;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = 0;
    SET_CMD_STATE(pcmd,CMD_REQ_DONE);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("send done writing io timings..."));
    this->tcp_socket.write(data.data(),data.size());
}
void QRelayDeviceControl::TcpAckWriteIoOutTiming(QByteArray & buffer)
{
    CmdHead  * pcmd = (CmdHead *)buffer.data_ptr()->data;
    if(GET_CMD_OK(pcmd)) {
        if(GET_CMD_STATE(pcmd) == CMD_CURRENT_START) {
            debuginfo(("tcp ack write start."));
            TcpReWriteIoOutTiming();
        } else if(GET_CMD_STATE(pcmd) == CMD_CURRENT_DOING) {
            debuginfo(("tcp ack write doing....,index(%d),all(%d)",io_out_time_index,io_out_time_count));
            io_out_time_index++;
            if(io_out_time_index < io_out_time_count) {
                TcpReWriteIoOutTiming();
            } else {
                TcpDoneWriteIoOutTiming();
            }
        } else if(GET_CMD_STATE(pcmd) == CMD_CURRENT_DONE) {
            debuginfo(("tcp ack write done."));
            SetTcpSysStatus(TCP_CMD_IDLE,tr("done writing io timings!"));
            emit DeviceWriteTimingFinished();
        } else {
            debugerror(("tcp ack write io out timing error! 2"));
        }
    } else {
        debugerror(("tcp ack write io out timing error! 1"));
    }
}



void QRelayDeviceControl::TcpStartWriteRtc(void)
{
    QByteArray data;

    debuginfo(("start write rtc"));

    io_out_time_index = 0;
    io_out_name_count = 5;
    tcp_ack_timeout_count = 0;


    data.resize(sizeof(CmdHead) + sizeof(time_type));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    time_type  *  rio  = (time_type *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_SET_RTC_VALUE;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(time_type);
    //
    QDateTime nowdt = QDateTime::currentDateTime();
    rio->year = nowdt.date().year() - 1900;
    rio->mon = nowdt.date().month() - 1;
    rio->day  = nowdt.date().day();
    rio->hour = nowdt.time().hour();
    rio->min  = nowdt.time().minute();
    rio->sec  = nowdt.time().second();
    //
    SET_CMD_STATE(pcmd,CMD_REQ_START);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("Start writing new rtc value..."));
    this->tcp_socket.write(data.data(),data.size());
}

void QRelayDeviceControl::TcpReStartWriteRtc(void)
{
    QByteArray data;
    debuginfo(("restart write new rtc"));
    data.resize(sizeof(CmdHead) + sizeof(time_type));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    time_type  *  rio  = (time_type *)GET_CMD_DATA(pcmd);
    pcmd->cmd = tcp_cmd_number = CMD_SET_RTC_VALUE;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = sizeof(time_type);
    //
    QDateTime nowdt = QDateTime::currentDateTime();
    rio->year = nowdt.date().year() - 1900;
    rio->mon = nowdt.date().month() - 1;
    rio->day  = nowdt.date().day();
    rio->hour = nowdt.time().hour();
    rio->min  = nowdt.time().minute();
    rio->sec  = nowdt.time().second();
    //
    SET_CMD_STATE(pcmd,CMD_REQ_START);   //�豸�����һ��BUG
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("Restart write new rtc value..."));
    this->tcp_socket.write(data.data(),data.size());
}

void QRelayDeviceControl::TcpDoneWriteRtc(void)
{
    debuginfo(("done write new rtc"));
    QByteArray data;
    data.resize(sizeof(CmdHead));
    CmdHead  * pcmd = (CmdHead *)data.data_ptr()->data;
    pcmd->cmd = tcp_cmd_number = CMD_SET_RTC_VALUE;
    pcmd->cmd_index = ++cmd_index;
    pcmd->cmd_len = 0;
    SET_CMD_STATE(pcmd,CMD_REQ_DONE);
    SetTcpSysStatus(TCP_CMD_WAIT_ACK,tr("Done write new rtc value."));
    this->tcp_socket.write(data.data(),data.size());
}

void QRelayDeviceControl::TcpAckWriteRtc(QByteArray & buffer)
{
    CmdHead  * pcmd = (CmdHead *)buffer.data_ptr()->data;
    if(GET_CMD_OK(pcmd)) {
        if(GET_CMD_STATE(pcmd) == CMD_CURRENT_START) {
            debuginfo(("tcp ack write start."));
            io_out_time_index++;
            if(io_out_time_index < io_out_name_count) {
                TcpReStartWriteRtc();
            } else {
                TcpDoneWriteRtc();
            }
        } else if(GET_CMD_STATE(pcmd) == CMD_CURRENT_DOING) {
            debuginfo(("tcp ack write doing."));
            io_out_time_index++;
            if(io_out_time_index < io_out_name_count) {
                TcpReStartWriteRtc();
            } else {
                TcpDoneWriteRtc();
            }
        } else if(GET_CMD_STATE(pcmd) == CMD_CURRENT_DONE) {
            debuginfo(("tcp ack write done."));
            SetTcpSysStatus(TCP_CMD_IDLE,tr("done writing rtc value."));
            emit DevcieWriteRtcFinihed();
        } else {
            debugerror(("tcp ack write rtc value error! 2"));
        }
    } else {
        debugerror(("tcp ack write rtc value error! 1"));
    }

}


void QRelayDeviceControl::TimeoutUpdataInfo(void)
{
    //debuginfo(("timeout updata..."));
    GetDevcieInfoFormDevcie();
    ReadIoOut();
    if(online_timeout > 0) {
        online_timeout--;
        devicestatus  = tr("On Line");
        is_online = true;
        DeviceUpdate();
    } else {
       // debuginfo(("OffLine ..."));
        devicestatus = tr("Off Line");
        is_online = false;
        DeviceUpdate();
    }
    timer->setInterval(1000);
}


void QRelayDeviceControl::InitDeviceAddress(QHostAddress & addr,quint16 port,QSharedPointer<QUdpSocket>  & psocket)
{
    pUdpSocket = psocket;
    deviceaddr  = addr;
    deviceport  = port;
}

QString QRelayDeviceControl::GetHostAddressString(void)
{
    QString str;
    str.sprintf(":%d",deviceport);
    return deviceaddr.toString() + str;
}
QString QRelayDeviceControl::GetBroadcastTime(void)
{
    QString str;
    str.sprintf("%d",this->pdev_info->broadcast_time);
    return str;
}
QString QRelayDeviceControl::GetListionPort(void)
{
    QString str;
    unsigned int port = this->pdev_info->work_port[1];
    port <<= 8;
    port += this->pdev_info->work_port[0];
    str.sprintf("%d", port);
    return str;
}
QString QRelayDeviceControl::GetRemoteHostAddress(void)
{
    QString str(QString::fromAscii(&pdev_info->remote_host_addr[0]));
    return str;
}

QString QRelayDeviceControl::GetAckStatus(void)
{
    return ack_status;
}

QString QRelayDeviceControl::GetRemoteHostPort(void)
{
    QString str;
    unsigned int port = this->pdev_info->remote_host_port[1];
    port <<= 8;
    port += this->pdev_info->remote_host_port[0];
    str.sprintf("%d", port);
    return str;
}

int         QRelayDeviceControl::GetTimingNodeNum(void)
{
    return io_out_time_count;
}

int         QRelayDeviceControl::GetTimingNodeNum(int index)
{
    int count = 0;
    for(int i=0;i<io_out_timing_list.size();i++) {
        timing_node * pnode = &io_out_timing_list[i];
        int addr = pnode->addr[1];
        addr *= 256;
        addr += pnode->addr[0];
        if(index == addr) {
            count++;
        }
    }
    return count;
}

void QRelayDeviceControl::SetDeviceName(QString name)
{
    device_info_st  newinfo;
    memcpy(&newinfo,&pdev_info->command,sizeof(device_info_st));
    name.resize(sizeof(newinfo.host_name));
    size_t strlen = name.size();
    memcpy(newinfo.host_name,name.toAscii().data(),strlen-1);
    newinfo.host_name[strlen-1] = 0;
    newinfo.command = CMD_SET_DEVICE_INFO;
    newinfo.command_len = sizeof(device_info_st);
    newinfo.change_password = 0;
    newinfo.to_host = 0;
    newinfo.change_ipconfig = 0;
    unsigned int crc = CRC16((unsigned char *)&newinfo,sizeof(device_info_st) - 2);
    newinfo.crc[0] = crc & 0xFF;
    newinfo.crc[1] = crc >> 8;
    //�����µ���Ϣ
    ack_status = tr("sending set device name command...");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
    SendCommandData((const char *)&newinfo,sizeof(newinfo));
}
void QRelayDeviceControl::SetGroup1Name(QString name)
{
    device_info_st  newinfo;
    memcpy(&newinfo,&pdev_info->command,sizeof(device_info_st));
    name.resize(sizeof(newinfo.group_name1));
    size_t strlen = name.size();
    memcpy(newinfo.group_name1,name.toAscii().data(),strlen-1);
    newinfo.group_name1[strlen-1] = 0;
    newinfo.command = CMD_SET_DEVICE_INFO;
    newinfo.command_len = sizeof(device_info_st);
    newinfo.change_password = 0;
    newinfo.to_host = 0;
    newinfo.change_ipconfig = 0;
    unsigned int crc = CRC16((unsigned char *)&newinfo,sizeof(device_info_st) - 2);
    newinfo.crc[0] = crc & 0xFF;
    newinfo.crc[1] = crc >> 8;
    //�����µ���Ϣ
    ack_status = tr("sending set device group command...");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
    SendCommandData((const char *)&newinfo,sizeof(newinfo));
}
void QRelayDeviceControl::SetGroup2Name(QString name)
{
    device_info_st  newinfo;
    memcpy(&newinfo,&pdev_info->command,sizeof(device_info_st));
    name.resize(sizeof(newinfo.group_name2));
    size_t strlen = name.size();
    memcpy(newinfo.group_name2,name.toAscii().data(),strlen-1);
    newinfo.group_name2[strlen-1] = 0;
    newinfo.command = CMD_SET_DEVICE_INFO;
    newinfo.command_len = sizeof(device_info_st);
    newinfo.change_password = 0;
    newinfo.to_host = 0;
    newinfo.change_ipconfig = 0;
    unsigned int crc = CRC16((unsigned char *)&newinfo,sizeof(device_info_st) - 2);
    newinfo.crc[0] = crc & 0xFF;
    newinfo.crc[1] = crc >> 8;
    //�����µ���Ϣ
    ack_status = tr("sending set device group command...");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
    SendCommandData((const char *)&newinfo,sizeof(newinfo));
}
void QRelayDeviceControl::WriteNewDeviceInfoToDevice(device_info_st * pst)
{
    device_info_st  newinfo;
    memcpy(&newinfo,pst,sizeof(device_info_st));
    newinfo.command = CMD_SET_DEVICE_INFO;
    newinfo.command_len = sizeof(device_info_st);
    newinfo.to_host = 0;
    unsigned int crc = CRC16((unsigned char *)&newinfo,sizeof(device_info_st) - 2);
    newinfo.crc[0] = crc & 0xFF;
    newinfo.crc[1] = crc >> 8;
    //�����µ���Ϣ
    ack_status = tr("start writing new device infamation command...");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
    SendCommandData((const char *)&newinfo,sizeof(device_info_st));
}

void QRelayDeviceControl::ResetDevice(void)
{
    reset_device_st rst;
    rst.command = CMD_RESET_DEVICE;
    rst.command_len = sizeof(reset_device_st);
    for(unsigned int i=0;i<sizeof(rst.pad);i++) {
        rst.pad[i] = (unsigned char)rand();
    }
    unsigned int crc = CRC16((unsigned char *)&rst,sizeof(reset_device_st) - 2);
    rst.crc[0] = (unsigned char)(crc & 0xFF);
    rst.crc[1] = (unsigned char)(crc >> 8);
    //�����µ���Ϣ
    ack_status = tr("Reset device...");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
    SendCommandData((const char *)&rst,sizeof(reset_device_st));
}

void QRelayDeviceControl::GetDevcieInfoFormDevcie(void)
{
    unsigned char buffer[32];
    buffer[0] = CMD_GET_DEVICE_INFO; 
    SendCommandData((const char *)buffer,sizeof(buffer));
}

void QRelayDeviceControl::ConvertIoOutOneBitAndSendCmd(int bit)
{
   // return ;
    QByteArray sb;
    unsigned int buflen = sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + sizeof(modbus_type_fc5_cmd) - 2;
    sb.resize(buflen);
    modbus_command_st  * mst = (modbus_command_st *)sb.data();
    mst->command = CMD_MODBUSPACK_SEND;
    mst->command_len = (unsigned char)sb.size();
    modbus_tcp_head * mh = (modbus_tcp_head *)GET_MODBUS_COMMAND_DATA(mst);
    mh->idh = 0;
    mh->idl = 0;
    mh->protocolh = 0;
    mh->protocoll = 0;
    mh->lengthh = (unsigned char)(sizeof(modbus_type_fc5_cmd) >> 8);
    mh->lengthl = (unsigned char)(sizeof(modbus_type_fc5_cmd) & 0xFF);
    modbus_type_fc5_cmd * pfc  = (modbus_type_fc5_cmd *)GET_MODBUS_DATA(mh);
    pfc->slave_addr = 0;
    pfc->function_code = 0x05;
    pfc->ref_number_h = (512+bit) >> 8;
    pfc->ref_number_l = (512+bit) & 0xFF;
    int max = relay_bitmask.size();
    max = ((bit+1)>max)?(bit+1):max;
    relay_bitmask.resize(max);
    if(relay_bitmask[bit]) {
        pfc->onoff = 0x00;
    } else {
        pfc->onoff = 0xFF;
    }
    pfc->pad = 0x00;
    unsigned int crc = CRC16(&mst->command_len,sb.size() - 3);
    debuginfo(("send ConvertIoOutOneBitAndSendCmd CRC(0x%X)",crc));
    mst->crc[0] = (unsigned char)(crc  & 0xFF);
    mst->crc[1] = (unsigned char)(crc >> 8);
	dumpthisdata((const char *)mst,sb.size());
    ack_status = tr("set io out bits...");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
    SendCommandData((const char *)mst,sb.size());
}
int QRelayDeviceControl::ConvertIoOutOneBitAndSendCmdAck(QByteArray & data)
{
    int buflen = sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + sizeof(modbus_type_fc5_cmd) - 2;
    if(data.size() != buflen) {
        debugerror(("convert io out ack data len error!"));
        return -1;
    }
    //debuginfo(("convert io ack."));
    modbus_command_st  * mst = (modbus_command_st *)data.data();
    unsigned int crc = CRC16(&mst->command_len,buflen-3);
    if(mst->crc[0] != (unsigned char)(crc&0xFF) || mst->crc[1] != (unsigned char)(crc>>8)) {
        debugerror(("convert io out data CRC(0x%X) ERROR!",crc));
        return -1;
    }
    modbus_tcp_head * mh = (modbus_tcp_head *)GET_MODBUS_COMMAND_DATA(mst);
    modbus_type_fc5_cmd * pfc  = (modbus_type_fc5_cmd *)GET_MODBUS_DATA(mh);
    unsigned int bit = pfc->ref_number_h;
    bit <<= 8;
    bit |= pfc->ref_number_l;
    bit -= 512; //
    debuginfo(("convert io ack bit = %d",bit));
    unsigned int max = relay_bitmask.size();
    max = ((bit+1)>max)?(bit+1):max;
    relay_bitmask.resize(max);
    if(pfc->onoff) {
        relay_bitmask[bit] = true;
    } else {
        relay_bitmask[bit] = false;
    }
    return 0;
}

void QRelayDeviceControl::ReadIoOut(void)
{
    int buflen = sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + sizeof(modbus_type_fc1_cmd) - 2;
    QByteArray sb;
    sb.resize(buflen);
    modbus_command_st * mst = (modbus_command_st *)sb.data();
    modbus_tcp_head * mh = (modbus_tcp_head *)GET_MODBUS_COMMAND_DATA(mst);
    modbus_type_fc1_cmd * fc = (modbus_type_fc1_cmd *)GET_MODBUS_DATA(mh);
    mh->idh = 0;
    mh->idl = 0;
    mh->protocolh = 0;
    mh->protocoll = 0;
    mh->lengthh = (unsigned char)(sizeof(modbus_type_fc1_cmd) >> 8);
    mh->lengthl = (unsigned char)(sizeof(modbus_type_fc1_cmd) & 0xFF);
    fc->slave_addr = 0;
    fc->function_code = 0x01;
    fc->ref_number_h = (512>>8);
    fc->ref_number_l = (512&0xFF);
    fc->bit_count_h = (unsigned char)(this->GetIoOutNum() >> 8);
    fc->bit_count_l = (unsigned char)(this->GetIoOutNum() & 0xFF);
    mst->command = CMD_MODBUSPACK_SEND;
    mst->command_len = sb.size();
    unsigned int crc = CRC16(&mst->command_len,sb.size() - 3);
   // dumpthisdata((const char *)mst,sb.size());
    mst->crc[0] = (unsigned char)(crc  & 0xFF);
    mst->crc[1] = (unsigned char)(crc >> 8);
    SendCommandData((const char *)mst,sb.size());    
}
int QRelayDeviceControl::ReadIoOutAck(QByteArray & data)
{
    unsigned int rx_len  = data.size();
    if(rx_len < sizeof(modbus_command_st)) {
        debugerror(("read io out ack data < modbus_command_st size.ERROR!"));
        return -1;
    }
    modbus_command_st * mst = (modbus_command_st *)data.data();
    //dumpthisdata((const char *)mst,data.size());
    unsigned int crc = CRC16(&mst->command_len,rx_len - 3);
    if(mst->crc[0] != (unsigned char)(crc&0xFF) || mst->crc[1] != (unsigned char)(crc>>8)) {
        debugerror(("read io out ack data CRC(0x%X) ERROR!",crc));
        return -1;
    }
    modbus_tcp_head * mh = (modbus_tcp_head *)GET_MODBUS_COMMAND_DATA(mst);
    modbus_type_fc1_ack * fc = (modbus_type_fc1_ack *)GET_MODBUS_DATA(mh);
    if(rx_len < (sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + 4 - 2)) {
        debugerror(("read io out ack data < (sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + 2.ERROR"));
        return -1;
    }
    if(rx_len < (sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + fc->byte_count + 3 - 2)) {
        debugerror(("read io out ack data <  (sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + fc->byte_count + 3 - 2.ERROR"));
        return -1;
    }
    //����ȷ��
   // debuginfo(("read io out,ALL OK,byte_count=%d",fc->byte_count));
    int max = relay_bitmask.size();
    max = ((fc->byte_count*8) > max)?(fc->byte_count*8):max;
    relay_bitmask.resize(max);
    for(int i=0;i<fc->byte_count*8;i++) {
        relay_bitmask[i] = (fc->bit_valus[i/8]&(1<<(i%8)))?true:false;
    }
    relay_bitmask_inited = true;
    return 0;
}


void QRelayDeviceControl::MultiIoOutSet(unsigned int start_index,QBitArray bit_mask)
{
    int bitbytes = ((bit_mask.size()+7)/8);
    int len = sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + 7 + bitbytes;
    QByteArray sb;
    sb.resize(len);
    modbus_command_st * mst = (modbus_command_st *)sb.data();
    modbus_tcp_head * mh = (modbus_tcp_head *)GET_MODBUS_COMMAND_DATA(mst);
    modbus_type_fc15_cmd * fc = (modbus_type_fc15_cmd *)GET_MODBUS_DATA(mh);
    mst->command = CMD_MODBUSPACK_SEND;
    mst->command_len = sb.size();
    mh->function_code = 0x0F;
    mh->slave_addr = 0;
    mh->idh = 0;
    mh->idl = 0;
    mh->protocolh = 0;
    mh->protocoll = 0;
    mh->lengthh = (unsigned char)((7 + bitbytes) >> 8);
    mh->lengthl = (unsigned char)((7 + bitbytes) & 0xFF);
    fc->ref_number_h = (unsigned char)((512+start_index)>>8); //�޸ĸ����Ĵ���
    fc->ref_number_l = (unsigned char)((512+start_index)&0xFF);
    fc->bit_count_h = (unsigned char)(bit_mask.size() >> 8);
    fc->bit_count_l = (unsigned char)(bit_mask.size() & 0xFF);
    fc->byte_count = (unsigned char)bitbytes;
    unsigned char * pbit_val = (unsigned char *)&fc->byte_count;
    ++pbit_val;
    memset(pbit_val,0,bitbytes);
    for(int i=0;i<bit_mask.size();i++) {
        pbit_val[i/8] |= (bit_mask[i])?(1<<(i%8)):0;
    }
    //���Լ���CRC��
    unsigned int crc = CRC16(&mst->command_len,sb.size() - 3);
    mst->crc[0] = (unsigned char)(crc  & 0xFF);
    mst->crc[1] = (unsigned char)(crc >> 8);
    ack_status = tr("set io out bits...");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
    SendCommandData((const char *)mst,sb.size());
}

int QRelayDeviceControl::MultiIoOutSetAck(QByteArray & data)
{
    int ret = -1;
    int len = sizeof(modbus_command_st) + sizeof(modbus_tcp_head) + 4;
    if(data.size() != len) {
        debugerror(("multi io set ack data error!"));
        return -1;
    } else {
        modbus_command_st * mst = (modbus_command_st *)data.data();
        unsigned int crc = CRC16(&mst->command_len,data.size() - 3);
        if(mst->crc[0] != (unsigned char)(crc&0xFF) || mst->crc[1] != (unsigned char)(crc>>8)) {
            debugerror(("MultiIoOutSetAck data CRC(0x%X) ERROR!",crc));
        } else {
            //debuginfo(("MultiIoOutSetAck ack ok."));
            ret = 0;
        }
    }
    return ret;
}

int  QRelayDeviceControl::GetIoOutNum(void)
{
    //���ݰ��ӵ��ͺţ�ȷ���������������
    switch(pdev_info->device_model) {
    case EXT_BOARD_IS_2CHIN_2CHOUT_BOX:return 2;
    case EXT_BOARD_IS_4CHIN_4CHOUT: return 4;
    case EXT_BOARD_IS_8CHIN_8CHOUT_V2: return 8;
    case RELAY_PLATFORM_16CHIN_16CHOUT_30A:
    case RELAY_PLATFORM_16CHOUT_HOST_RESET:
    case EXT_BOARD_IS_16CHOUT:return 16;
    default:
        return 2;
    }
}
QString QRelayDeviceControl::GetDeviceModelName(void)
{
    //���ݰ��ӵ��ͺţ��������
    switch(pdev_info->device_model) {
    default:
    case EXT_BOARD_IS_2CHIN_2CHOUT_BOX:return tr("2 ch dig input,2 ch relay output.");
    case EXT_BOARD_IS_4CHIN_4CHOUT: return tr("4 ch dig input,4 ch relay output.");
    case EXT_BOARD_IS_8CHIN_8CHOUT_V2: return tr("8 ch dig input,8 ch relay output.");
    case EXT_BOARD_IS_16CHOUT:return tr("16 ch relay output.");
    case RELAY_PLATFORM_16CHOUT_HOST_RESET: return tr("16 ch remote host reseter.");
    case RELAY_PLATFORM_16CHIN_16CHOUT_30A: return tr("16 ch input and 16 ch 30A relay output.");
    }
}

void QRelayDeviceControl::SendCommandData(const char * buffer,int len)
{
    //debuginfo(("send command data to %s:%d",this->deviceaddr.toString().toAscii().data(),this->deviceport));
    if(need_encryption && !password.isEmpty()) {
        QEncryptRc4 rcc;
        rcc.UseKey(password);
        QByteArray src,buf;
        src.resize(len);
        memcpy(src.data(),buffer,len);
        rcc.Encrypt(src,buf);
        this->pUdpSocket->writeDatagram(buf.data(),len,this->deviceaddr,this->deviceport);
    } else {
        this->pUdpSocket->writeDatagram(buffer,len,this->deviceaddr,this->deviceport);
    }
}

int QRelayDeviceControl::SendRxData(QByteArray & rawdata,QList<password_item> & pwdlist)
{
    int ret = -1;

    //debuginfo(("rx data %d",rawdata.size()));

    QByteArray data;

    need_encryption = false;

    bool use_pwd_list_pwd = true;
    int pwd_count = pwdlist.count();
    int pwd_index = 0;

    QString alis;
    QString passwordstr;

    goto  last_config_password;

encrypting_again:

    need_encryption = true;

    if(!use_pwd_list_pwd) {
        passwordstr = this->password;
        use_pwd_list_pwd = false;
    } else {
        if(pwd_index >= pwd_count) {
            //debuginfo(("not found password!"));
            return -1;
        } else {
            alis = pwdlist.at(pwd_index).alias;
            passwordstr = pwdlist.at(pwd_index++).pwd;
        }
    }
    //debuginfo(("use password:%s:%s",alis.toAscii().data(),passwordstr.toAscii().data()));

    password = passwordstr;

last_config_password:
    if(need_encryption) {
        if(passwordstr.size() > 0) {
            QEncryptRc4 rc;
            //debuginfo(("try to use password:%s",passwordstr.toAscii().data()));
            rc.UseKey(passwordstr);
            rc.Encrypt(rawdata,data);
        } else {
            goto encrypting_again;
        }
    } else {
        data = rawdata;
    }

    device_info_st * pst = (device_info_st *)data.constData();

    if(pst->command == CMD_SET_DEVICE_INFO) {
        unsigned int crc = CRC16((unsigned char *)pst,sizeof(device_info_st) - 2);
        if(pst->command_len != data.size() || data.size() != sizeof(device_info_st)) {
            goto encrypting_again;
        } else if((unsigned char)(crc>>8) == pst->crc[1] && (unsigned char)(crc&0xFF) == pst->crc[0]) {
            SetDeviceInfo(data);
        } else {
            debugerror(("crc ERROR(0x%X)",crc));
            goto encrypting_again;
        }
    } else if(CMD_MODBUSPACK_SEND == pst->command) {
        int minlen = sizeof(modbus_command_st) + sizeof(modbus_tcp_head);
		modbus_command_st * pmst = (modbus_command_st *)pst;
        if(pmst->command_len < minlen) {
            debugerror(("modbus command data size too small...ERROR!,pst->command_len(%d) < minlen(%d)",pmst->command_len,minlen));
            goto encrypting_again;
		} else {
			debuginfo(("modbus command data size:pst->command_len(%d) >= minlen(%d)",pmst->command_len,minlen));
		}
        modbus_tcp_head * mh = (modbus_tcp_head *)GET_MODBUS_COMMAND_DATA(pst);
        switch(mh->function_code)
        {
        case 0x05: //���ٷ�ָ��
        {
            if(ConvertIoOutOneBitAndSendCmdAck(data) != 0) {
                goto encrypting_again;
            }
            ReadIoOut();
            ret = 0;
        }
            break;
        case 0x01:
        {
            if(ReadIoOutAck(data) != 0) {
                goto encrypting_again;
            }
            if(!relay_bitmask_inited) {
                ReadIoOut();
            } else {
                online_timeout = 10;
            }
            ret = 0;
        }
            break;
        case 0x02:
        {
        }
            break;
        case 0x0F:
        {
            if(MultiIoOutSetAck(data) != 0) {
                goto encrypting_again;
            }
            ReadIoOut();
            ret = 0;
        }
            break;
        default:
            break;
        }
    } else if(CMD_RESET_DEVICE == pst->command) {
        debuginfo(("reset command ack data..."));
        if(pst->command_len != data.size()) {
            debuginfo(("reset data len error!"));
            goto encrypting_again;
        }
        unsigned int crc = CRC16((unsigned char *)pst,sizeof(reset_device_st) - 2);        
        dumpthisdata((const char *)pst, (int)(data.size()));
        debuginfo(("caled crc is : 0x%X",crc));
        if(((reset_device_st *)data.data())->crc[0] != (unsigned char)(crc&0xFF) || ((reset_device_st *)data.data())->crc[1] != (unsigned char)(crc>>8)) {
            debuginfo(("reset data crc error!"));
            data.clear();
            goto encrypting_again;
        }
        devicestatus  = tr("Reseting...");
        online_timeout = 0;
        timer->setInterval(5000);
        data.clear();
        debuginfo(("reset ack ok"));
        ret = 0;
    } else {
        goto encrypting_again;
    }
    DeviceUpdate();
    return 0;
}


void QRelayDeviceControl::DeviceUpdate(void)
{
    QString hostaddrid;
    hostaddrid.sprintf("%d",this->deviceport);
    hostaddrid = ":" + hostaddrid;
    hostaddrid = this->deviceaddr.toString() + hostaddrid;
    //debuginfo(("set device info:%s",hostaddrid.toAscii().data()));
	emit DeviceInfoChanged(hostaddrid);
}
void QRelayDeviceControl::Updata(void)
{
    debuginfo(("updata"));
    DeviceUpdate();
}

void QRelayDeviceControl::SetDeviceInfo(QByteArray & data)
{
    //debuginfo(("-----------updata device info-------------"));
    memcpy(&pdev_info->command,data.constData(),sizeof(device_info_st));
    pdev_info->host_name[63] = 0;
    pdev_info->group_name1[31] = 0;
    pdev_info->group_name2[31] = 0;
    devicename   = QString::fromAscii(&pdev_info->host_name[0]);
    devicegroup1 = QString::fromAscii(&pdev_info->group_name1[0]);
    devicegroup2 = QString::fromAscii(&pdev_info->group_name2[0]);

    QString str;
    const unsigned char * pch = pdev_info->local_ip;
    str.sprintf("ip:%d-%d-%d-%d ",pch[3],pch[2],pch[1],pch[0]);
    pch = pdev_info->net_mask;
    QString netmask;
    netmask.sprintf("netmask:%d-%d-%d-%d ",pch[3],pch[2],pch[1],pch[0]);
    pch = pdev_info->gateway;
    QString gateway;
    gateway.sprintf("gateway:%d-%d-%d-%d ",pch[3],pch[2],pch[1],pch[0]);
    pch = pdev_info->dns;
    QString dns;
    dns.sprintf("dns:%d-%d-%d-%d ",pch[3],pch[2],pch[1],pch[0]);

    QString timeout;
    timeout.sprintf("timeout=%d ",pdev_info->broadcast_time);

    QString mac;
    pch = pdev_info->mac;
    mac.sprintf("%X-%X-%X-%X-%X-%X ",pch[0],pch[1],pch[2],pch[3],pch[4],pch[5]);

    str += netmask + gateway + mac + timeout;

    if(!bdevcie_info_is_useful) {
        //�豸��Ч�����ӵ�����TCP�˿�
        debuginfo(("device is useful..."));
        bdevcie_info_is_useful = true;
        //����Tcp����
        tcp_socket.connectToHost(GetDeviceAddress(),tcp_port);
    }

    ack_status = tr("device(") + this->GetDeviceAddress() + tr(") infomation is updated.");
    if(is_checked) {
        emit DeviceAckStatus(ack_status);
    }
}

QString QRelayDeviceControl::GetDeviceAddress(void)
{
    return deviceaddr.toString();
}

QString QRelayDeviceControl::GetDeviceName(void)
{
    return devicename;
}
QString QRelayDeviceControl::GetGroup1Name(void)
{
    return devicegroup1;
}
QString QRelayDeviceControl::GetGroup2Name(void)
{
    return devicegroup2;
}
QString QRelayDeviceControl::GetStatus(void)
{
    return devicestatus;
}

QString QRelayDeviceControl::GetDeviceTime(void)
{
    QString str;
    unsigned char * pbuf = this->pdev_info->device_time;
    str.sprintf("%d-%d-%d %d-%d-%d",1900+pbuf[5],1+pbuf[4],pbuf[3],pbuf[2],pbuf[1],pbuf[0]);
    //debuginfo(("device time:%s",str.toAscii().data()));
    return str;
}

QDateTime QRelayDeviceControl::GetDeviceDateTime(void)
{
    unsigned char * pbuf = this->pdev_info->device_time;
    QDate d(1900+pbuf[5],1+pbuf[4],pbuf[3]);
    QTime t(pbuf[2],pbuf[1],pbuf[0]);
    QDateTime dt(d,t);
    return dt;
}

void  QRelayDeviceControl::SetTcpSysStatus(int newState,QString string)
{
    tcp_sys_state = newState;
    sys_timeout_count = 0;
    sys_current_status = string;
}

uint16_t CRC16(unsigned char *Array,unsigned int Len)
{
    uint16_t IX,IY,CRC;
    CRC=0xFFFF;//set all 1
    if (Len<=0) {
        CRC = 0;
    } else {
        Len--;
        for (IX=0;IX<=Len;IX++)
        {
            CRC=CRC^(uint16_t)(Array[IX]);
            for(IY=0;IY<=7;IY++) {
                if ((CRC&1)!=0) {
                    CRC=(CRC>>1)^0xA001;
                } else {
                    CRC=CRC>>1;
                }
            }
        }
    }
    return CRC;
}

