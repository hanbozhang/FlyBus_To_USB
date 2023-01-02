#ifndef FLYUSB_CONFIG_H
#define FLYUSB_CONFIG_H
#include <stdio.h>
#include "tinyusb.h"
#include "class/hid/hid_device.h"
#include "esp_log.h"

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance);

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen);

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize);

void FHS_USB_Bigen(void);

void FHS_Send_Hid_FlyHandle(void);

void FHS_Set_Null(void);
void FHS_Set_Lx(uint16_t value);
void FHS_Set_Rx(uint16_t value);
void FHS_Set_Ly(uint16_t value);
void FHS_Set_Ry(uint16_t value);
void FHS_Set_Lz(uint8_t value);
void FHS_Set_Rz(uint8_t value);
void FHS_Set_Slider1(uint8_t value);
void FHS_Set_Slider2(uint8_t value);
void FHS_Set_Buttons(bool value,uint8_t index);

#endif
