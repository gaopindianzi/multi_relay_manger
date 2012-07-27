#ifndef __COMMAND_DATATYPE_H__
#define __COMMAND_DATATYPE_H__

#pragma pack(push)//保存对齐状态
#pragma pack(1)  //设置为1字节对齐



typedef   quint8     uint8_t;
typedef   quint16    uint16_t;
typedef   quint32    uint32_t;


//成功与否，实际上只占一位
#define   CMD_ACK_OK                  0x01
#define   CMD_ACK_KO                  0x00

#define  CMD_REQ_START             0
#define  CMD_REQ_NEXT              1
#define  CMD_REQ_DONE              2

#define  CMD_CURRENT_START        0
#define  CMD_CURRENT_DOING        1
#define  CMD_CURRENT_DONE         2

extern uint8_t    command_state;

#define   CMD_GET_IO_OUT_VALUE           1
#define   CMD_GET_IO_IN_VALUE            2
#define   CMD_SET_IO_OUT_VALUE           3
#define   CMD_GET_IO_NAME                4
#define   CMD_SET_IO_NAME                5
#define   CMD_GET_TIMING_INFO            6
#define   CMD_SET_TIMING_INFO            7
#define   CMD_GET_RTC_VALUE              8
#define   CMD_SET_RTC_VALUE              9
#define   CMD_SET_TIMING_ON_MSK          10
#define   CMD_GET_TIMING_ON_MSK          11
#define   CMD_SET_INPUT_CTL_ON_MSK       12
#define   CMD_GET_INPUT_CTL_ON_MSK       13
#define   CMD_REV_IO_SOME_BIT            36
#define   CMD_SET_IP_CONFIG              33
#define   CMD_GET_IP_CONFIG              34
#define   CMD_GET_INPUT_CTL_MODE_INDEX   40
#define   CMD_SET_INPUT_CTL_MODE_INDEX   41

typedef struct _CmdHead
{
  uint8_t  cmd;
  uint16_t cmd_index;
  uint8_t  cmd_option;
  uint16_t cmd_len;
  uint8_t  data_checksum;
} CmdHead;


#define GET_CMD_DATA(pCmd)         ((void *)(((char *)pCmd)+sizeof(CmdHead)))
#define GET_CMD_OK(pcmd)           (((pcmd)->cmd_option)&CMD_ACK_OK)
#define SET_CMD_OK(pcmd,ok)        do{if(!ok){(pcmd)->cmd_option &= ~CMD_ACK_OK;}else{(pcmd)->cmd_option |= CMD_ACK_OK;}}while(0)
#define GET_CMD_STATE(pcmd)        ((((pcmd)->cmd_option)&0xF0)>>4)
#define SET_CMD_STATE(pcmd,mode)   do{(pcmd)->cmd_option &= ~0xF0;(pcmd)->cmd_option |= ((mode)&0xF)<<4; }while(0)



typedef struct _CmdIoIndex
{
  uint8_t    io_msk[4];
} CmdIobitmap;

typedef struct _CmdMode
{
  uint8_t    mode;
} CmdMode;

typedef struct _CmdInputMode
{
  uint8_t    mode[8];
} CmdInputMode;

typedef struct _CmdInputModeIndex
{
	uint8_t    index;
    uint8_t    mode;
} CmdInputModeIndex;

#define INPUT_TRIGGER_FLIP_MODE          0x00  //触发反转模式
#define INPUT_SINGLE_TRIGGER_MODE        0x01  //单触发模式
#define INPUT_TRIGGER_TO_OPEN_MODE       0x02  //触发开通模式
#define INPUT_TRIGGER_TO_OFF_MODE        0x03  //触发关闭模式
#define INPUT_TRIGGER_OFF_MODE           0x07  //控制关闭

typedef struct _CmdIoValidMsk
{
	uint8_t   valid_msk[4];
	uint8_t   invalid_msk[4];
} CmdIoValidMsk;

typedef struct _CmdIpConfigData
{
  uint8_t   ipaddr[4];
  uint8_t   netmask[4];
  uint8_t   gateway[4];
  uint8_t   dns[4];
  uint16_t  port;
  uint16_t  webport;
} CmdIpConfigData;


//加密功能
typedef struct _CmdMcuId
{
  uint8_t  IdLength;
  uint8_t  IdValue[1];
} CmdMcuId;

typedef struct _CmdIoValue
{
  uint8_t    io_count;
  uint8_t    io_value[4];
} CmdIoValue;

typedef struct _CmdIoBit
{
  uint8_t    io_index;
  uint8_t    io_enable;
} CmdIoBit;

typedef struct _CmdBoardInfo
{
	uint8_t  board_name[20];
	uint8_t  input_num;
	uint8_t  output_num;
} CmdBoardInfo;

typedef struct _CmdIoName
{
	uint8_t   io_addr[2];
	uint8_t   io_name[20];
} CmdIoName;

typedef struct _timint_info
{
	uint16_t    time_max_count;
} timint_info;

typedef struct _time_type
{
	uint8_t    year;  //年份 - 1900,比如2011-1900 = 111
	uint8_t    mon;   //月份 - 1，比如 5月 ， mon = 5 - 1 = 4
	uint8_t    day;  //天，1-28/29/30/31
	uint8_t    hour; //小时0-23
	uint8_t    min; //分0-59
	uint8_t    sec; //秒0-59
} time_type;

typedef struct _timing_node
{
	uint8_t    addr[2];
	uint16_t   option;
	//
	time_type  start_time;
	time_type  end_time;
	//
	uint32_t   duty_cycle;
	uint32_t   period;
	//记录状态
	uint8_t    state;
} timing_node;

typedef struct _CmdTimingNode
{
	uint16_t    index;
	timing_node node;
} CmdTimingNode;
//
#define  VALID       1
#define  INVALID     0
//state
#define  TIME_NO_COME    0
#define  TIME_DOING      1
#define  TIME_SUBDO_ON   2
#define  TIME_SUBDO_OFF  3
#define  TIME_OVER       4
//

//option
#define  CYCLE_YEAR      0
#define  CYCLE_MONTH     1
#define  CYCLE_DAY       2
#define  CYCLE_HOUR      3
#define  CYCLE_MINITH    4
#define  CYCLE_SECOND    5
#define  CYCLE_WEEK      6
#define  CYCLE_USER      7
#define  CYCLE_ONCE      8
//option
#define  TIME_VALID      0x01
#define  TIME_SUBCYCLE   0x02
#define  TIME_HOLD_IO    0x04
#define  TIME_DONE       0x08


//-------------------------------------------------------------
#define  SET_IO_TIME_VALID(ptime,valid)  \
	do{  \
	  if(valid) { (ptime)->option |=  TIME_VALID; } else { (ptime)->option &= ~TIME_VALID; }   \
	} while(0)
#define  GET_IO_TIME_VALID(ptime)   \
	     (((ptime)->option)&TIME_VALID)
//-------------------------------------------------------------
//-------------------------------------------------------------
#define  SET_IO_TIME_SUBCYCLEON(ptime,valid)  \
	do{  \
	  if(valid) { (ptime)->option |=  TIME_SUBCYCLE; } else { (ptime)->option &= ~TIME_SUBCYCLE; }   \
    } while(0)
#define  GET_IO_TIME_SUBCYCLEON(ptime)   \
	     ((((ptime)->option)&TIME_SUBCYCLE)?1:0)
//-------------------------------------------------------------
#define  SET_IO_TIME_HOLD_IO(ptime,valid)  \
	do{  \
	  if(valid) { (ptime)->option |=  TIME_HOLD_IO; } else { (ptime)->option &= ~TIME_HOLD_IO; }   \
    } while(0)
#define  GET_IO_TIME_HOLD_IO(ptime)   \
	     (((ptime)->option)&TIME_MS_TICK)
//-------------------------------------------------------------
//-------------------------------------------------------------
#define  SET_IO_TIME_CYCLE_TYPE(ptime,type)  \
	do{ (ptime)->option &= ~(0xF<<8);(ptime)->option |= (((type)&0xF)<<8); } while(0)
#define  GET_IO_TIME_CYCLE_TYPE(ptime)   \
	     ((((ptime)->option)>>8)&0xF)
//-------------------------------------------------------------
#define  SET_IO_TIME_IO_ON_OFF(ptime,valid)  \
	do{  \
	  if(valid) { (ptime)->option |=  TIME_ON_OFF; } else { (ptime)->option &= ~TIME_ON_OFF; }   \
    } while(0)
#define  GET_IO_TIME_IO_ON_OFF(ptime)   \
	     (((ptime)->option)&TIME_ON_OFF)
//-------------------------------------------------------------
#define  SET_IO_TIME_DONE(ptime,valid)  \
	do{  \
	  if(valid) { (ptime)->option |=  TIME_DONE; } else { (ptime)->option &= ~TIME_DONE; }   \
    } while(0)
#define  GET_IO_TIME_DONE(ptime)   \
	     (((ptime)->option)&TIME_DONE)

#pragma pack(pop)//恢复对齐状态


#endif