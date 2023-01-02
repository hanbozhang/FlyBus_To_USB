#include "include/FlyBus.h"

//解析函数指针
void (*FlyBus_Receive[3])(uart_port_t uart_num,FlyBus_Channel_t *Data,uint8_t *cach,uint16_t cachsize,uint8_t *CHCach) = {IBus_Receive,IBus2_Receive,SBus_Receive};
//串口配置函数指针
void (*FlyBus_Uart_Config[3])(uart_port_t uart_num) = {IBus_Uart_Config,IBus2_Uart_Config,SBus_Uart_Config};
//飞行遥控总线数据
static FlyBus_Channel_t flyBus_Channel;
//飞行遥控总线类型
static FlyBus_Mode flyBus_mode;

//是否读取总线数据
static bool FlyBus_IsRead_Channel;
//总线数据接收串口号
uart_port_t uart_index;
//串口接收缓存
uint8_t *uartRCach;
//通道原始数据缓存
uint8_t *ChannelRCach;


void FlyBus_Begin(uart_port_t uart_num, uint8_t uart_Rx_Gpio, FlyBus_Mode mode)
{
    uart_driver_install(uart_num, 256, 0, 0, NULL, 0);
    uart_set_pin(uart_num, -1, uart_Rx_Gpio, -1, -1);
    uart_index = uart_num;
    FlyBus_Set_Mode(mode);
    uartRCach=(uint8_t *)malloc(UART_CATCHSIZE + 1);
    ChannelRCach= (uint8_t *)malloc(64);
}
//设置总线模式
void FlyBus_Set_Mode(FlyBus_Mode mode)
{
    FlyBus_IsRead_Channel=false;
    flyBus_mode = mode;
    FlyBus_Uart_Config[flyBus_mode-1](uart_index);
    FlyBus_IsRead_Channel=true;
}
//获取总线模式
uint8_t FlyBus_Get_Mode(void)
{
    return flyBus_mode;
}
//读取通道数据
void FlyBus_Read_CHData(void)
{
    if (FlyBus_IsRead_Channel)
        FlyBus_Receive[flyBus_mode-1](uart_index, &flyBus_Channel, uartRCach, UART_CATCHSIZE, ChannelRCach);
}
//获取总线数据是否为空
bool FlyBus_Get_CHIsNull(void)
{
    return flyBus_Channel.ChannelISNull;
}
uint8_t FlyBus_Get_CHLen(void)
{
    return flyBus_Channel.ChannelLen;
}
uint16_t FlyBus_Get_CHData(uint8_t index)
{
    if (index > 32 || index < 1)
    {
        return 0;
    }
    return flyBus_Channel.ChannelData[index-1];
}
