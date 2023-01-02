/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "FlyUSB_config.h"
#include "FlyBus.h"
#include "ConfigStore.h"
#include "Led_config.h"
#include "WiFi_Config.h"
#include "Web_config.h"

//映射配置测试
#define Test_CTU 0
//映射配置输出测试
#define Test_GET_CTU 0
//WEB启动页测试 0为AP模式 1为STA模式接入路由器
#define Test_WEB 1
//飞行总线数据 IO
#define BUS_GPIO GPIO_NUM_18
//WS2812状态指示LED IO
#define LED_GPIO GPIO_NUM_48
const uint32_t Bus_LED_COLOR[4]={0x0,0x3FFF0000,0x7FFF0000,0xBDFF0000};
Store_CHTOUSB_t *StCH[40];
uint8_t BusMode = 1, i;
Data_CHTOUSB_t DCT;
bool Fhs_btn;
bool Btn_Trriger_s[32];
bool Btn_Remove_s[32];
bool LED_State[3];

//按键点击删除事件 放置在FHS_Send_Hid_FlyHandle后面
void Btn_Remove_Event(void)
{
    for(i=0;i<32;i++)
    {
        if(Btn_Remove_s[i])
        {
            Btn_Remove_s[i]=false;
            FHS_Set_Buttons(false,i+1);
        }
    }
}
//遥控总线转USB报文数据
void FlyBus_To_HandleUSB(void)
{
    for (i = 0; i < 40; i++)
    {
        DCT = ConfigStore_Get_Config(i);
        //配置关系中只要有参数为空则跳过设置
        if (DCT.type == 0x00 || DCT.usb_channel_id == 0x00 || DCT.flyBus_channel_id == 0x00 || DCT.flyBus_channel_id > 18)
            continue;

        //配置类型为按键时
        if (DCT.type == 0x03)
        {
            if (DCT.usb_channel_id > 32)
                continue;
            Fhs_btn=false;
            //当符合触发值时
            if ((FlyBus_Get_CHData(DCT.flyBus_channel_id) >> 8) == DCT.trigger_value)
            {
                Fhs_btn=true;
            }
            //当Button类型为点击类型是
            if (DCT.button_type == 0x02)
            {
                //如果点击动作已被注册 并且当前处于触发状态 直接跳过
                if(Btn_Trriger_s[DCT.usb_channel_id-1]&&Fhs_btn)
                    continue;
                //如果点击动作已注册 并且处于未触发状态 删除注册
                if(Btn_Trriger_s[DCT.usb_channel_id-1]&&(!Fhs_btn))
                {
                    Btn_Trriger_s[DCT.usb_channel_id-1]=false;
                    printf("注销按钮 %d 动作 \n",DCT.usb_channel_id);
                    continue;
                }
                //如果点击动作未注册 并且处于触发状态 注册触发 注册删除 并发送按钮点击
                if((!Btn_Trriger_s[DCT.usb_channel_id-1])&&(Fhs_btn))
                {
                    Btn_Trriger_s[DCT.usb_channel_id-1]=true;
                    Btn_Remove_s[DCT.usb_channel_id-1]=true;
                    printf("注册按钮 %d 动作 \n",DCT.usb_channel_id);
                }
            }
            FHS_Set_Buttons(Fhs_btn, DCT.usb_channel_id);
        }
        //配置类型为高精度通道时
        if (DCT.type == 0x01)
        {
            switch (DCT.usb_channel_id)
            {
            case 1:
                FHS_Set_Lx(FlyBus_Get_CHData(DCT.flyBus_channel_id));
                break;
            case 2:
                FHS_Set_Ly(FlyBus_Get_CHData(DCT.flyBus_channel_id));
                break;
            case 3:
                FHS_Set_Rx(FlyBus_Get_CHData(DCT.flyBus_channel_id));
                break;
            case 4:
                FHS_Set_Ry(FlyBus_Get_CHData(DCT.flyBus_channel_id));
                break;
            default:
                break;
            }
            continue;
        }
        //配置类型为低精度通道时
        if (DCT.type == 0x02)
        {
            uint8_t CH=(FlyBus_Get_CHData(DCT.flyBus_channel_id)>> 3)-187;
            switch (DCT.usb_channel_id)
            {
            case 1:
                FHS_Set_Lz(CH);
                break;
            case 2:
                FHS_Set_Rz(CH);
                break;
            case 3:
                FHS_Set_Slider1(CH);
                break;
            case 4:
                FHS_Set_Slider2(CH);
                break;
            default:
                break;
            }
            continue;
        }
    }
}

//读取遥控总线数据
static void FlyBus_Read_task(void *arg)
{
    while (1)
    {
        FlyBus_Read_CHData();
    }
}

//遥控总线转手柄USB数据
static void FlyBus_To_UCH_task(void *arg)
{
    while (1)
    {
        FlyBus_To_HandleUSB();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

//LED状态灯闪烁
static void LED_task(void *arg)
{
    LED_Set_RGB(LED_COLOR_Green);
    vTaskDelay(pdMS_TO_TICKS(1000));
    LED_Set_RGB(LED_COLOR_Red);
    vTaskDelay(pdMS_TO_TICKS(1000));
    LED_Set_RGB(LED_COLOR_Blue);
    vTaskDelay(pdMS_TO_TICKS(1000));
    uint8_t Bus_LED_num=0;
    while (1)
    {
        if(!LED_State[0])
        {
            LED_Set_RGB(LED_COLOR_Red);
            continue;
        }
        if (!FlyBus_Get_CHIsNull())
        {
            LED_Set_RGB(LED_COLOR_Green);
            vTaskDelay(pdMS_TO_TICKS(300));
            LED_Set_RGB(Bus_LED_COLOR[FlyBus_Get_Mode()]);
            vTaskDelay(pdMS_TO_TICKS(300));
            Bus_LED_num=0;
        }
        else
        {
            Bus_LED_num++;
        }
        if (Bus_LED_num>=10)
        {
            LED_Set_RGB(LED_COLOR_Black);
        }

        if (LED_State[1])
        {
            LED_Set_RGB(LED_COLOR_Blue);
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

//配置测试代码
void Txst_CTU(void)
{
    Store_CHTOUSB_t value;
    value.usage1=0x05;
    value.usage2=0x01;
    printf("CFG1: Type %d FBCH %d UCH %d \n",CTU_Get_Type(value),CTU_Get_FBCH(value),CTU_Get_UCH(value));
    ConfigStore_Set_CTUConfig(value);
    Store_CHTOUSB_t value2;
    value2.usage1=0x16;
    value2.usage2=0x01;
    printf("CFG1: Type %d FBCH %d UCH %d \n",CTU_Get_Type(value2),CTU_Get_FBCH(value2),CTU_Get_UCH(value2));
    ConfigStore_Set_CTUConfig(value2);
    ConfigStore_Save_CTUConfig();

}

void app_main(void)
{
    //读取配置
    ConfigStore_Begin();
    //获取配置
    LED_State[0]=ConfigStore_Get_CTUConfig();
    //读取总线模式
    if(!ConfigStore_Get_BusMode(&BusMode))
        ConfigStore_Save_BusMode(1);
#if Test_CTU==1
    //配置测试
    Txst_CTU();
#endif
#if Test_GET_CTU==1
    for (size_t i = 0 ; i < 40; i++)
    {
        printf("CTU%d: %d %d \n",i,CfgsCTU[i].usage1,CfgsCTU[i].usage2);
    }
#endif
    //初始化遥控总线
    FlyBus_Begin(UART_NUM_1,BUS_GPIO, BusMode);
    //初始化USB
    FHS_USB_Bigen();
    //初始化LED指示灯
    LED_Begin(LED_GPIO);
#if Test_WEB==1
    WiFi_Sta_Init();
    Start_Webserver();
#else
    WiFi_AP_Init();
    Start_Webserver();
#endif

    //启动总线接收
    xTaskCreate(FlyBus_Read_task, "FlyBus_Read_task", 2048, NULL, 9, NULL);
    //启动数据转换
    xTaskCreate(FlyBus_To_UCH_task, "FlyBus_To_UCH_task", 2048, NULL, 10, NULL);
    //启动LED指示灯
    xTaskCreate(LED_task, "LED_task", 1024, NULL, 11, NULL);
    while (1)
    {
        if (tud_mounted())
        {
            FHS_Send_Hid_FlyHandle();
            Btn_Remove_Event();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
