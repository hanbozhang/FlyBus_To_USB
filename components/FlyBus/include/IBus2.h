#ifndef IBUS2_H
#define IBUS2_H
#include "FlyBus_config.h"

enum IBus2_receive_State_t
{
    IBUS2_HEADER,
    IBUS2_DATA,
    IBUS2_CHECK
};
//SBus帧间隔
#define IBUS2_IFG 10
//SBus帧时间
#define IBUS2_IFT 1
//IBus2串口配置
void IBus2_Uart_Config(uart_port_t uart_num);
//IBus2数据解析
void IBus2_Receive(uart_port_t uart_num, FlyBus_Channel_t *Data,uint8_t * cach,uint16_t cachsize,uint8_t *CHCach);
#endif
