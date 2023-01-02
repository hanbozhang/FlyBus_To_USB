#include "SBus.h"

enum SBus_receive_State_t S_State = SBUS_HEADER;
uint8_t S_len, S_i, S_u, S_DataSum;

// SBus串口配置
void SBus_Uart_Config(uart_port_t uart_num)
{
    uart_config_t uart_config = {
        .baud_rate = 100000,                   //设置波特率 115200
        .data_bits = 0x03,         //数据位 8位
        .parity = UART_PARITY_EVEN,         //不使用校验
        .stop_bits = UART_STOP_BITS_2,         //停止位 1
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //禁用 RTS CTS
        .source_clk = UART_SCLK_DEFAULT,       //默认40MHZ时钟
    };
    uart_param_config(uart_num, &uart_config);
    uart_set_line_inverse(uart_num,UART_SIGNAL_RXD_INV);
}
// SBus数据解析
void SBus_Receive(uart_port_t uart_num, FlyBus_Channel_t *Data, uint8_t *cach, int cachsize, uint8_t *CHCach)
{
    S_len = uart_read_bytes(uart_num, cach, cachsize, pdMS_TO_TICKS(SBUS_IFT));
#if SBusDubeg == 1
    printf("SBus:ReadLen %d \n\r", S_len);
#endif
#if SBusDubeg == 1
                printf("\n\r");
#endif
    for (S_i = 0; S_i < S_len; S_i++)
    {
#if SBusDubeg == 3
                printf("%x ", cach[S_i]);
#endif
        if (cach[S_i] == 0x0F)
        {
            S_State = SBUS_DATA;
            continue;
        }
        if (S_State == SBUS_DATA)
        {
            CHCach[S_DataSum] = cach[S_i];
            S_DataSum++;
            if ((S_DataSum==23)&&(cach[S_i] == 0x00))
            {
#if SBusDubeg == 1
                printf("End");
                printf("DataSum: %d \n\r", S_DataSum);
#endif
                S_State = SBUS_CHECK;
                S_DataSum = 0;
            }
            if (S_DataSum>23)
            {
                S_State = SBUS_HEADER;
                S_DataSum = 0;
            }

        }
        if (S_State == SBUS_CHECK)
        {
#if SBusDubeg == 1
            printf("CS");
#endif
            Data->ChannelData[0] = (CHCach[0] | CHCach[1] << 8) & 0x07FF;
            // if (Data->ChannelData[0]<=24)
            // {
            //     Data->ChannelLen = 0;
            //     S_State = SBUS_HEADER;
            //     continue;
            // }
            Data->ChannelData[1] = (CHCach[1] >> 3 | CHCach[2] << 5) & 0x07FF;
            Data->ChannelData[2] = (CHCach[2] >> 6 | CHCach[3] << 2 | CHCach[4] << 10) & 0x07FF;
            Data->ChannelData[3] = (CHCach[4] >> 1 | CHCach[5] << 7) & 0x07FF;
            Data->ChannelData[4] = (CHCach[5] >> 4 | CHCach[6] << 4) & 0x07FF;
            Data->ChannelData[5] = (CHCach[6] >> 7 | CHCach[7] << 1 | CHCach[8] << 9) & 0x07FF;
            Data->ChannelData[6] = (CHCach[8] >> 2 | CHCach[9] << 6) & 0x07FF;
            Data->ChannelData[7] = (CHCach[9] >> 5 | CHCach[10] << 3) & 0x07FF;
            Data->ChannelData[8] = (CHCach[11] | CHCach[12] << 8) & 0x07FF;
            Data->ChannelData[9] = (CHCach[12] >> 3 | CHCach[13] << 5) & 0x07FF;
            Data->ChannelData[10] = (CHCach[13] >> 6 | CHCach[14] << 2 | CHCach[16] << 10) & 0x07FF;
            Data->ChannelData[11] = (CHCach[15] >> 1 | CHCach[16] << 7) & 0x07FF;
            Data->ChannelData[12] = (CHCach[16] >> 4 | CHCach[17] << 4) & 0x07FF;
            Data->ChannelData[13] = (CHCach[17] >> 7 | CHCach[18] << 1 | CHCach[20] << 9) & 0x07FF;
            Data->ChannelData[14] = (CHCach[19] >> 2 | CHCach[20] << 6) & 0x07FF;
            Data->ChannelData[15] = (CHCach[20] >> 5 | CHCach[21] << 3) & 0x07FF;
            Data->ChannelData[16] = (CHCach[22] & 0x80) << 3;
            Data->ChannelData[17] = (CHCach[22] & 0x40) << 4;
            // for (S_u = 0; S_u < 18; S_u++)
            // {
            //     Data->ChannelData[S_u] = Data->ChannelData[S_u]-992;
            // }
#if SBusDubeg == 1
            for (S_u = 0; S_u < 18; S_u++)
            {
                printf("CH%d:%d ", S_u + 1, Data->ChannelData[S_u]);
            }
#endif
            Data->ChannelLen = 18;
            S_State = SBUS_HEADER;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(SBUS_IFG));
}
