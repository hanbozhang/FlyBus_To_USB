#include "IBus.h"

// IBus数据读取状态
enum IBus_receive_State_t IR_State = IBUS_HEADER;
uint16_t I_Check = 0xFFFF;
uint8_t I_DataSum = 0, I_u, I_y, I_i, IR_len;

// IBus串口配置
void IBus_Uart_Config(uart_port_t uart_num)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,                   //设置波特率 115200
        .data_bits = UART_DATA_8_BITS,         //数据位 8位
        .parity = UART_PARITY_DISABLE,         //不使用校验
        .stop_bits = UART_STOP_BITS_1,         //停止位 1
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //禁用 RTS CTS
        .source_clk = UART_SCLK_DEFAULT,       //默认40MHZ时钟
    };
    uart_param_config(uart_num, &uart_config);
    uart_set_line_inverse(uart_num,UART_SIGNAL_INV_DISABLE);
}
// IBus数据解析
void IBus_Receive(uart_port_t uart_num, FlyBus_Channel_t *Data, uint8_t *cach, uint16_t cachsize, uint8_t *CHCach)
{
    IR_len = uart_read_bytes(uart_num, cach, cachsize, pdMS_TO_TICKS(IBUS_IFT));
    if (IR_len<10)
    {
        Data->ChannelLen = 0;
        Data->ChannelISNull=true;
    }
    else
    {
        Data->ChannelISNull=false;
    }
#if IBusDubeg == 2
    printf("\n\r");
    printf("%d ",IR_len);
#endif
    for (I_i = 0; I_i < IR_len; I_i++)
    {
#if IBusDubeg == 3
    printf("%x ",cach[I_i]);
#endif
        //判断数据头
        if (cach[I_i - 1] == 0x20 && cach[I_i] == 0x40)
        {
#if IBusDubeg == 1
            printf("IBus:Start \n\r");
#endif
            I_Check = 0xFF9F;
            IR_State = IBUS_DATA;
            I_DataSum = 0;
            continue;
        }
        //开始接收数据
        if (IR_State == IBUS_DATA)
        {
            CHCach[I_DataSum] = cach[I_i];
            if (I_DataSum < 28)
            {
                I_Check -= cach[I_i];
            }
            I_DataSum++;
            if (I_DataSum > 30)
            {
#if IBusDubeg == 1
                printf("IBus:End \n\r");
#endif
                IR_State = IBUS_CHECK;
            }
        }
        //开始校验并填充数据
        if (IR_State == IBUS_CHECK)
        {
#if IBusDubeg == 1
            printf("IBus:Check \n\r");
#endif
            if (CHCach[29] == (I_Check & 0xFF00) >> 8)
            {
#if IBusDubeg == 1
                printf("IBus:Check OK! /n");
#endif
                //填充1-14通道数据
                for (I_u = 0, I_y = 0; I_u < 14; I_u++, I_y += 2)
                {
                    Data->ChannelData[I_u] = ((CHCach[I_y + 1] & 0x0F) << 8 | CHCach[I_y]);
                }
                //填充15-18通道数据
                for (I_y = 3; I_u < 18; I_u++, I_y += 6)
                {
                    Data->ChannelData[I_u] = ((CHCach[I_y + 2] & 0xF0) << 4 | (CHCach[I_y] & 0xF0) | (CHCach[I_y - 2] & 0xF0) >> 4);
                }
                Data->ChannelLen = 18;
#if IBusDubeg == 1
                for (I_u = 0; I_u < 18;I_u++)
                {
                    printf("CH%d:%d", I_u + 1, Data->ChannelData[I_u]);
                }
#endif
            }
            else
            {
                Data->ChannelLen = 0;
            }
            IR_State = IBUS_HEADER;
            break;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(IBUS_IFG));
}
