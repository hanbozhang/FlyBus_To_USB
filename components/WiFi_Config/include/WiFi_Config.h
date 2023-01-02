#ifndef WIFI_CONFIG
#define WIFI_CONFIG

//AP Config
#define WIFI_SSID      "FlyBusToHandleUSB"
#define WIFI_PASS      ""
#define WIFI_CHANNEL   8
#define MAX_STA_CONN   1

//STA Config
#define WIFI_STA_SSID "zhanghan"
#define WIFI_STA_PASS "ABCabc123"

void WiFi_Sta_Init(void);
void WiFi_AP_Init(void);
#endif
