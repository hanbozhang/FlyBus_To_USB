#ifndef LED_CONFIG_H
#define LED_CONFIG_H
#include <stdio.h>
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"
#include "esp_log.h"
#define LED_COLOR_Black 0x00000000
#define LED_COLOR_White 0xFFFFFFFF
#define LED_COLOR_Red   0xFF0000FF
#define LED_COLOR_Green 0x00FF00FF
#define LED_COLOR_Blue  0x0000FFFF
#define LED_GET_Red(rgbw) ((rgbw)>>24)
#define LED_GET_Green(rgbw) (((rgbw)>>16)&0xFF)
#define LED_GET_Blue(rgbw) (((rgbw)>>8)&0xFF)

void LED_Begin(uint8_t gpio);

void LED_Set_RGB(uint32_t rgbw);

void LED_Close(void);
#endif
