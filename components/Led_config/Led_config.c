#include "Led_config.h"

static rmt_channel_handle_t led_chan = NULL;
static rmt_encoder_handle_t led_encoder = NULL;
static uint8_t Rgb[3];
static rmt_transmit_config_t tx_config;

void LED_Begin(uint8_t gpio){
    printf("创建 RMT TX 通道 \n");
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // 选择源时钟
        .gpio_num = gpio,
        .mem_block_symbols = 64, // 增加块尺寸可以使LED不那么闪烁
        .resolution_hz = 10000000,
        .trans_queue_depth = 4, // 设置可以在后台挂起的交易数
    };
    rmt_new_tx_channel(&tx_chan_config, &led_chan);
    printf("安装灯条编码器 \n");
    led_strip_encoder_config_t encoder_config = {
        .resolution = 10000000,
    };
    rmt_new_led_strip_encoder(&encoder_config, &led_encoder);
    printf("启用 RMT TX 通道");
    rmt_enable(led_chan);
    printf("开始 LED \n");
    tx_config.loop_count=0;
}

void LED_Set_RGB(uint32_t rgbw){
    Rgb[0]=LED_GET_Green(rgbw)>>2;
    Rgb[1]=LED_GET_Red(rgbw)>>2;
    Rgb[2]=LED_GET_Blue(rgbw)>>2;
    ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, Rgb, 3, &tx_config));
}

void LED_Close(void){
    LED_Set_RGB(LED_COLOR_Black);
}
