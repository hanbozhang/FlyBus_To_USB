#ifndef FLYBUS_H
#define FLYBUS_H
#include "FlyBus_config.h"
#include "IBus.h"
#include "IBus2.h"
#include "SBus.h"

#define UART_CATCHSIZE 512


//遥控总线初始化
void FlyBus_Begin(uart_port_t uart_num,uint8_t uart_Rx_Gpio,FlyBus_Mode mode);
//设置总线工作模式
void FlyBus_Set_Mode(FlyBus_Mode mode);
//获取总线工作模式
uint8_t FlyBus_Get_Mode(void);
//读取并解析总线数据
void FlyBus_Read_CHData(void);
//串行总线是否无数据
bool FlyBus_Get_CHIsNull(void);
//获取通道长度
uint8_t FlyBus_Get_CHLen(void);
//获取通道数据
uint16_t FlyBus_Get_CHData(uint8_t index);

#endif
