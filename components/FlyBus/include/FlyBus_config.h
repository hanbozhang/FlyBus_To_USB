#ifndef FLYBUS_CONFIG_H
#define FLYBUS_CONFIG_H
#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//Dubeg标识
#define IBusDubeg 0
#define IBus2Dubeg 0
#define SBusDubeg 0

typedef enum
{
    FlyBus_Mode_IBus = 1,
    FlyBus_Mode_IBus2 = 2,
    FlyBus_Mode_SBus = 3,
} FlyBus_Mode;

typedef struct
{
    int16_t ChannelData[32];
    uint8_t ChannelLen;
    bool ChannelISNull;
}FlyBus_Channel_t;
#endif
