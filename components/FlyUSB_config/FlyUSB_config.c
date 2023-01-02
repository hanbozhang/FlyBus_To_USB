#include "include/FlyUSB_config.h"

/************* FlyUSB手柄结构体 ****************/
typedef struct
{
    uint16_t LX;
    uint16_t LY;
    uint16_t RX;
    uint16_t RY;
    uint8_t LZ;
    uint8_t RZ;
    uint8_t Slider1;
    uint8_t Slider2;
    uint32_t Buttons;
}flyHL_Msg_t;

/************* TinyUSB 描述符 ****************/
/**
 * @brief HID 报告描述符
 *
 * 这里使用的是飞行摇杆 定义了8个模拟数据通道以及32个按键通道
 */
uint8_t FlyHandle_hid_report_descriptor[] = {
    0x05, 0x01, // UsagePage(GenericDesktopCtrls)
    0x09, 0x04, // Usage(Joystick)
    0xa1, 0x01, // Collection(Application)

    0x05, 0x01,       // UsagePage(GenericDesktopCtrls)
    0x09, 0x30,       // Usage(X)
    0x09, 0x31,       // Usage(Y)
    0x09, 0x33,       // Usage(RX)
    0x09, 0x34,       // Usage(RY)
    0x16, 0x80, 0x01, // LogicalMinimum(-32767)
    0x26, 0x7F, 0xFF, // LogicalMaximum(32767)
    0x75, 0x10,       // ReportSize(16)
    0x95, 0x04,       // ReportCount(4)
    0x81, 0x02,       // Input

    0x05, 0x01, // UsagePage(GenericDesktopCtrls)
    0x09, 0x32, // Usage(Z)
    0x09, 0x35, // Usage(RZ)
    0x09, 0x36, // Usage(Slider)
    0x09, 0x36, // Usage(Slider)
    0x15, 0x00, // LogicalMinimum(0)
    0x26, 0x00,0xFF, // LogicalMaximum(255)
    0x75, 0x08, // ReportSize(8)
    0x95, 0x04, // ReportCount(4)
    0x81, 0x02, // Input

    0x05, 0x09, // UsagePage(Button)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x09, 0x01, // Usage(Pointer)
    0x95, 0x20, // ReportCount(32)
    0x75, 0x01, // ReportSize(1)
    0x81, 0x02, // Input

    0xC0, // EndCollection
};
/**
 * @brief 配置描述符
 *
 * 这是一个简单的配置描述符，用于定义 1 个配置和 1 个 HID 接口
 */
#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

static const uint8_t hid_configuration_descriptor[] = {
    // 配置号、接口数、字符串索引、总长度、供电方式、功率（mA）
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // 接口编号、字符串索引、引导协议、报告描述符 len、EP 输入地址、包大小、轮询间隔ms
    TUD_HID_DESCRIPTOR(0, 0, false, sizeof(FlyHandle_hid_report_descriptor), 0x81, 16, 10),
};

//手柄信息报表
flyHL_Msg_t FlyHandle_Msg;
//手柄按键操作变量
static uint32_t button_operate=0;

/********* TinyUSB HID 回调 ***************/

// 收到 GET HID 报告描述符请求时调用
// 应用程序返回指向描述符的指针，其内容必须存在足够长的时间才能完成传输
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    // 我们只使用一个接口和一个 HID 报告描述符，因此我们可以忽略参数“实例”
    return FlyHandle_hid_report_descriptor;
}

// 控制请求处理
// 收到控制请求时调用GET_REPORT
// 应用程序必须填充缓冲区报告的内容并返回其长度。
// 返回零将导致堆栈停止请求
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen ;

    return 0 ;
}

// 收到SET_REPORT控制请求或
// 在 OUT 端点上接收的数据（报告 ID = 0，类型 = 0）
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
}

void FHS_USB_Bigen(void){
        ESP_LOGI("USB", "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = hid_configuration_descriptor,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
}

//发送手柄信息报文
void FHS_Send_Hid_FlyHandle(void)
{
    tud_hid_n_report(0, HID_ITF_PROTOCOL_NONE, &FlyHandle_Msg, 16) ;
}
//设置空数据
void FHS_Set_Null(void)
{
    FlyHandle_Msg.LX = 0;
    FlyHandle_Msg.LY = 0;
    FlyHandle_Msg.RX = 0;
    FlyHandle_Msg.RY = 0;
    FlyHandle_Msg.LZ = 0;
    FlyHandle_Msg.RZ = 0;
    FlyHandle_Msg.Slider1 = 0;
    FlyHandle_Msg.Slider2 = 0;
    FlyHandle_Msg.Buttons = 0x00000000;
}
//设置 左X轴摇杆
void FHS_Set_Lx(uint16_t value)
{
    FlyHandle_Msg.LX = value;
}
//设置 右x轴摇杆
void FHS_Set_Rx(uint16_t value)
{
    FlyHandle_Msg.RX = value;
}
//设置 左y轴摇杆
void FHS_Set_Ly(uint16_t value)
{
    FlyHandle_Msg.LY = value;
}
//设置 右y轴摇杆
void FHS_Set_Ry(uint16_t value)
{
    FlyHandle_Msg.RY = value;
}
//设置 左z轴摇杆
void FHS_Set_Lz(uint8_t value)
{
    FlyHandle_Msg.LZ = value;
}
//设置 右z轴摇杆
void FHS_Set_Rz(uint8_t value)
{
    FlyHandle_Msg.RZ = value;
}
//设置 滑块1
void FHS_Set_Slider1(uint8_t value)
{
    FlyHandle_Msg.Slider1 = value;
}
//设置 滑块2
void FHS_Set_Slider2(uint8_t value)
{
    FlyHandle_Msg.Slider2 = value;
}
//设置 Button
void FHS_Set_Buttons(bool value, uint8_t index)
{
    button_operate=0x00000000;
    button_operate=0x1<<(32-index);
    if (value)
    {
        FlyHandle_Msg.Buttons = FlyHandle_Msg.Buttons| button_operate;
    }
    else
    {
        button_operate = ~button_operate;
        FlyHandle_Msg.Buttons = FlyHandle_Msg.Buttons & button_operate;
    }
}
