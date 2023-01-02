#include <stdio.h>
#include <string.h>
#include "WiFi_Config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"

/* FreeRTOS 事件组，用于在我们连接时发出信号*/
static EventGroupHandle_t s_wifi_event_group;
//重试次数
static int s_retry_num = 0;

// WIFI连接注册事件否则无法手机无法连接WIFI
static void wifi_AP_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
{
    //如果是连接请求
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *w_add_event = (wifi_event_ap_staconnected_t *)event_data;
    }
    //如果是断开请求
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *w_rem_event = (wifi_event_ap_stadisconnected_t *)event_data;
    }
}

//连接AP事件
static void wifi_STA_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 8)
        {
            esp_wifi_connect();
            s_retry_num++;
            printf( "重试连接到 AP \n");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, BIT1);
        }
        printf( "连接到 AP 失败 \n");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        printf("获得的IP地址: %d %d %d %d \n", IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group,BIT0);
    }
}

void WiFi_AP_Init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    //配置默认的AP
    esp_netif_create_default_wifi_ap();
    // WIFI配置
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    //初始化WIFI
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_AP_event_handler,
                                                        NULL,
                                                        NULL));
    //配置WIFI
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = WIFI_CHANNEL,
            .password = WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    if (strlen(WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    printf("WiFi初始化完成. SSID:%s password:%s channel:%d",
           WIFI_SSID, WIFI_PASS, WIFI_CHANNEL);
}
void WiFi_Sta_Init(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    // WIFI连接事件注册
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_STA_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    // IP获取事件注册
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_STA_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_STA_SSID,
            .password = WIFI_STA_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("WIFI初始化完成 ！");

    /* 等待，直到连接建立 （WIFI_CONNECTED_BIT） 或连接失败最大值
     * 重试次数 （WIFI_FAIL_BIT）。位由 event_handler（） 设置（见上文） */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           BIT0 | BIT1,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits（） 在调用返回之前返回位，因此我们可以测试实际哪个事件
     *发生。*/
    if (bits & BIT0)
    {
        printf("connected to ap SSID:%s password:%s",
                 WIFI_STA_SSID, WIFI_STA_PASS);
    }
    else if (bits & BIT1)
    {
        printf( "Failed to connect to SSID:%s, password:%s",
                 WIFI_STA_SSID, WIFI_STA_PASS);
    }
    else
    {
        printf( "意外事件");
    }
}
