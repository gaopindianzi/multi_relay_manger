#ifndef __MOSBUS_INTERFACE_H__
#define __MOSBUS_INTERFACE_H__


typedef struct _modbus_tcp_head
{
	uint8_t idh;
	uint8_t idl;
	uint8_t protocolh;
	uint8_t protocoll;
	uint8_t lengthh;
	uint8_t lengthl;
	uint8_t slave_addr;
	uint8_t function_code;
} modbus_tcp_head;

#define GET_MODBUS_DATA(ptr)     ((void *)(((unsigned char *)ptr)+sizeof(modbus_tcp_head)-2))

typedef struct _modbus_type_fc1_cmd
{
	uint8_t slave_addr;
	uint8_t function_code;
	uint8_t ref_number_h;
	uint8_t ref_number_l;
	uint8_t bit_count_h;
	uint8_t bit_count_l;
} modbus_type_fc1_cmd;

typedef struct _modbus_type_fc1_ack
{
	uint8_t slave_addr;
	uint8_t function_code;
	uint8_t byte_count;
	uint8_t bit_valus[256];  //字节占位符，实际中，也许用不了这么大的数组
} modbus_type_fc1_ack;


typedef struct _modbus_type_fc5_cmd
{
	uint8_t slave_addr;
	uint8_t function_code;
	uint8_t ref_number_h;
	uint8_t ref_number_l;
	uint8_t onoff;
	uint8_t pad;
} modbus_type_fc5_cmd;

typedef struct _modbus_type_fc15_cmd
{
    uint8_t slave_addr;
    uint8_t function_code;
    uint8_t ref_number_h;
    uint8_t ref_number_l;
    uint8_t bit_count_h;
    uint8_t bit_count_l;
    uint8_t byte_count;
} modbus_type_fc15_cmd;


#endif
