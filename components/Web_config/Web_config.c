#include "Web_config.h"

// 主页
esp_err_t Index_Page_Event(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req , &Index_Hex , Index_Hex_Len);
    return ESP_OK;
}
//主页
const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = Index_Page_Event
};
// 图标
esp_err_t Index_ICON_Event(httpd_req_t *req)
{
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req , &favicon_ico , favicon_ico_len);
    return ESP_OK;
}
//图标
const httpd_uri_t Index_ICON = {
    .uri = "/favicon.ico",
    .method = HTTP_GET,
    .handler = Index_ICON_Event
};
//手柄通道设置请求
esp_err_t HCH_TO_FCH_Set_Config_Event(httpd_req_t *req)
{
    Store_CHTOUSB_t reqDatas[StoreLen];
    size_t recv_size = 120;
    int ret = httpd_req_recv(req, &reqDatas, recv_size);
    if (ret <= 0)/* 返回 0 表示连接已关闭 */
    {
        /* 检查是否超时 */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            /* 如果是超时，可以调用 httpd_req_recv() 重试
             * 简单起见，这里我们直接响应 HTTP 408（请求超时）错误给客户端 */
            httpd_resp_send_408(req);
        }
        /* 如果发生了错误，返回 ESP_FAIL 可以确保底层套接字被关闭 */
        return ESP_FAIL;
    }
    for (uint8_t i = 0; i < StoreLen; i++)
        ConfigStore_Set_CTUConfig(reqDatas[i]);
    ConfigStore_Save_CTUConfig();
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req ,"ok",2);
    return ESP_OK;
}
const httpd_uri_t HCH_TO_FCH_Set_Config_Console={
    .uri = "/HFSC",
    .method = HTTP_POST,
    .handler = HCH_TO_FCH_Set_Config_Event
};

//手柄通道配置请求
esp_err_t HCH_TO_FCH_Get_Config_Event(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_send(req ,ConfigStore_Get_Configs(),120);
    return ESP_OK;
}
const httpd_uri_t HCH_TO_FCH_Get_Config_Console={
    .uri = "/HFGC",
    .method = HTTP_GET,
    .handler = HCH_TO_FCH_Get_Config_Event
};

//总线通道数据请求
esp_err_t FlyBus_Response_Event(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/octet-stream");
    uint8_t len=18;
    uint16_t rsp[len+1];
    for (size_t i = 0; i < len; i++)
        rsp[i]=FlyBus_Get_CHData(i+1);
    rsp[len]=len;
    httpd_resp_send(req ,&rsp,(len+1)*2);
    return ESP_OK;
}
const httpd_uri_t FlyBus_Response_Console={
    .uri = "/FBR",
    .method = HTTP_GET,
    .handler = FlyBus_Response_Event
};

// HTTP 错误 (404) 处理程序 -将所有请求重定向到根页面
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    //设置状态
    httpd_resp_set_status(req, "302 Temporary Redirect");
    //重定向到“/”根目录
    httpd_resp_set_hdr(req, "Location", "/");
    //iOS需要响应中的内容来检测捕获门户，仅仅重定向是不够的。
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

//启动Web服务
httpd_handle_t Start_Webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 7;
    config.lru_purge_enable = true;

    // 启动httpd服务器
    ESP_LOGI("HTTPD", "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // 设置URI处理程序
        ESP_LOGI("HTTPD", "正在注册URI处理程序");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &Index_ICON);
        httpd_register_uri_handler(server, &HCH_TO_FCH_Set_Config_Console);
        httpd_register_uri_handler(server, &HCH_TO_FCH_Get_Config_Console);
        httpd_register_uri_handler(server, &FlyBus_Response_Console);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    return server;
}
