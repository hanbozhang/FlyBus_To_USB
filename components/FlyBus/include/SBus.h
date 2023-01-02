#ifndef SBUS_H
#define SBUS_H
#include "FlyBus_config.h"

enum SBus_receive_State_t
{
    SBUS_HEADER,
    SBUS_DATA,
    SBUS_CHECK
};
//SBus帧间隔
#define SBUS_IFG 20
//SBus帧时间
#define SBUS_IFT 4
//SBus中间值
#define SBUS_Create_Value 992
//SBus串口配置
void SBus_Uart_Config(uart_port_t uart_num);
//SBus数据解析
void SBus_Receive(uart_port_t uart_num, FlyBus_Channel_t *Data,uint8_t * cach,int cachsize,uint8_t *CHCach);

#endif
