#ifndef IBUS_H
#define IBUS_H
#include "FlyBus_config.h"

enum IBus_receive_State_t
{
    IBUS_HEADER,
    IBUS_DATA,
    IBUS_CHECK,
};


#define IBusBitLen 25
//IBus帧间隔
#define IBUS_IFG 20
//IBus帧时间
#define IBUS_IFT 4
//IBus串口配置
void IBus_Uart_Config(uart_port_t uart_num);
//IBus数据解析
void IBus_Receive(uart_port_t uart_num, FlyBus_Channel_t *Data,uint8_t * cach,uint16_t cachsize,uint8_t *CHCach);

#endif
