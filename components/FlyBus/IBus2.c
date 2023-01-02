#include "IBus2.h"

enum IBus2_receive_State_t I2R_State = IBUS2_HEADER;
uint16_t I2R_len, I2_i;
uint8_t I2_u, I2_DataSum = 0, I2_D, I2_C;

// IBus2串口配置
void IBus2_Uart_Config(uart_port_t uart_num)
{
    uart_set_baudrate(uart_num, 1500000);
    uart_set_line_inverse(uart_num, UART_SIGNAL_INV_DISABLE);
}
// IBus2数据解析
void IBus2_Receive(uart_port_t uart_num, FlyBus_Channel_t *Data, uint8_t *cach, uint16_t cachsize, uint8_t *CHCach)
{
    I2R_len = uart_read_bytes(uart_num, cach, cachsize, pdMS_TO_TICKS(IBUS2_IFT));
#if IBus2Dubeg == 2
    printf("IBus2:ReadLen %d \n\r", I2R_len);
#endif
    if (I2R_len < 10)
    {
        Data->ChannelLen = 0;
        Data->ChannelISNull = true;
    }
    else
    {
        Data->ChannelISNull = false;
    }
    for (I2_i = 0; I2_i < I2R_len; I2_i++)
    {
        //接收数据
        if (I2R_State == IBUS2_DATA)
        {
            if (I2_DataSum < 27)
            {
                CHCach[I2_DataSum] = cach[I2_i];
                I2_DataSum++;
#if IBus2Dubeg == 2
                printf("%x ", cach[I2_i]);
#endif
                continue;
            }
            else
            {
                I2R_State = IBUS2_CHECK;
#if IBus2Dubeg == 1
                printf("\n\r IBus2:DataSum %d \n\r", I2_DataSum);
#endif
            }
        }
        //校验并填充数据
        if (I2R_State == IBUS2_CHECK)
        {
#if IBus2Dubeg == 1
            printf("\n\r IBus2:CHECK \n\r");
#endif
            for (I2_u = 0; I2_u < 9; I2_u++)
            {
                I2_D = I2_u * 2;
                I2_C = I2_u * 3;
                Data->ChannelData[I2_D] = (((CHCach[I2_C + 1] & 0xF0) << 4) | CHCach[I2_C]);
                Data->ChannelData[I2_D + 1] = ((CHCach[I2_C + 1] & 0x0F) | (CHCach[I2_C + 2] << 4));
            }
            Data->ChannelLen = 18;
            I2R_State = IBUS2_HEADER;
#if IBus2Dubeg == 1
            for (I2_u = 0; I2_u < 18; I2_u++)
            {
                printf("CH%d:%d", I2_u + 1, Data->ChannelData[I2_u]);
            }
#endif
        }
        //判断数据头
        if (cach[I2_i - 1] == 0x1F || cach[I2_i] == 0x38)
        {
#if IBus2Dubeg == 1
            printf("\n\r IBus2:Start \n\r");
#endif
            I2R_State = IBUS2_DATA;
            I2_DataSum = 0;
            continue;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(IBUS2_IFG));
}
