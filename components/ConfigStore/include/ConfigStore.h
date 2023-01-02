#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

#include <stdio.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define Name "nvs"
#define NameSpace "ConfigStore"
#define ConfigKey_CTUConfig "Cfgs"
#define ConfigKey_BusMode "BusM"
#define StoreLen 40
#define CTU_Get_Type(v) ((v.usage1)&(0x03))
#define CTU_Get_FBCH(v) (v.usage1>>2)
#define CTU_Get_UCH(v) ((v.usage2)&(0x3F))
#define CTU_Get_BTLC(v) (v.usage2>>6)
//设置类型 轴 1 滑块 2 按钮 3
#define CTU_Set_Type(v,n) v.usage1=((v.usage1)|(0x03&(n)));
//设置遥控总线通道
#define CTU_Set_FBCH(v,n) v.usage1=((v.usage1)|(n<<2));
//设置USB通道
#define CTU_Set_UCH(v,n) v.usage2=((v.usage2)|(0x3F&(n)));
//设置按键触发值
#define CTU_Set_BTLC(v,n) v.usage2=((v.usage2)|(n<<6));
//清空数值数据
#define CTU_Clear(v) v.usage1=0;v.usage2=0;v.Trigger_value=0;

//存储结构体
typedef struct
{
    // bit0-1 Free 00|Shaft 01|Slider 10|Button 11
    // bit2-7 Free 000000|FlyBus_Channel_ID 1-63
    uint8_t usage1;
    // bit0-5 Free 000000|USB_Channel_ID 1-63
    // bit6-7 Free 00/11|Button_lock 01|Button_click 10|
    uint8_t usage2;
    // bit0-7 触发值
    uint8_t Trigger_value;
}Store_CHTOUSB_t;
//数据结构体
typedef struct
{
    uint8_t type;
    uint8_t flyBus_channel_id;
    uint8_t usb_channel_id;
    uint8_t button_type;
    uint8_t trigger_value;
}Data_CHTOUSB_t;

static Store_CHTOUSB_t CfgsCTU[StoreLen];


bool ConfigStore_Begin(void);
bool ConfigStore_Save_CTUConfig(void);
bool ConfigStore_Save_BusMode(uint8_t value);
void ConfigStore_Set_CTUConfig(Store_CHTOUSB_t value);
bool ConfigStore_Get_BusMode(uint8_t* value);
bool ConfigStore_Get_CTUConfig(void);
Data_CHTOUSB_t ConfigStore_Get_Config(uint8_t index);
Store_CHTOUSB_t * ConfigStore_Get_Configs(void);

#endif
