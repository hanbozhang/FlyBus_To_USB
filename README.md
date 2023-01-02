# ESP32 SOC兼容列表
| ESP32-S2 | ESP32-S3 |
| -------- | -------- |
|          | YES|

# 项目介绍

本项目为遥控飞行总线转USB摇杆的应用，共支持8组模拟量输出，32组按钮触发以及网页通道映射以及触发值设定，无需安装任何外部程序以及驱动即可配置通道映射。
本项目依赖ESPIDF 5.0环境开发，基于TinyUSB进行的HID应用。
目前仅支持富斯IBUS协议，SBUS尚不支持因为S3开发板不具备硬件反相器功能。
目前仅支持ESP32-S3模组.

## 使用示例

### 目录介绍
```
FLYBUS_TO_USB //主目录
    components //组件库
        ConfigStore //配置组件库
        FlyBus //遥控总线解析组件库
        FlyUSB_config //USB配置组件库
        Led_config //Led指示灯组件库
        Web_config //网页配置组件库
        WiFi_Config //WiFi配置组件库
    main //主程序目录
```
### 配置网络
进入WiFi_Config目录下的WiFi_Config.h文件修改预定义字符即可
```C
//AP Config
#define WIFI_SSID      "FlyBusToHandleUSB"
#define WIFI_PASS      ""
#define WIFI_CHANNEL   8
#define MAX_STA_CONN   1

//STA Config
#define WIFI_STA_SSID "zhanghan"
#define WIFI_STA_PASS "ABCabc123"
```
进入main文件夹下的FlyBus_To_USB_main.c主文件，修改Test_WEB为1或0选择STA或AP模式
```C
//WEB启动页测试 0为AP模式 1为STA模式接入路由器
#define Test_WEB 1
```
### 配置IO口
进入main文件夹下的FlyBus_To_USB_main.c主文件修改下列内容
```C
//飞行总线数据 IO
#define BUS_GPIO GPIO_NUM_18
//WS2812状态指示LED IO
#define LED_GPIO GPIO_NUM_48
```
完成上述修改后就可以编译项目了。
### 配置通道
选择任意CH1-CH18通道即可将响应通道数据映射到对应的USB通道上
![WEB](/IMG/1.PNG)
### 配置按钮
配置按钮前先点击设置
![WEB](/IMG/2.PNG)
#### 保持模式
保持模式为通道拨杆波动到对应档位后会持续保持对应按键的按下状态，直至拨杆位置发生变动。
设置对应通道然后将拨杆拨动至触发位置后通道选择后面的数值会发生变动或不变动这是点击确认即可，点击确认后可拨动拨杆观察按钮后的开关状态是否改变如果改变则表示设置成功。
![WEB](/IMG/4.PNG)
#### 点击模式
点击模式为通道拨杆波动到对应档位后会导致对应按键触发一次点击效果，当下一次再按下后才会再次触发按键效果。
设置对应通道然后将拨杆拨动至触发位置后通道选择后面的数值会发生变动或不变动这是点击确认即可，点击确认后可拨动拨杆观察按钮后的开关状态是否改变如果改变则表示设置成功。
#### 保存配置

点击保存映射关系后，相关配置参数通过post请求发送给ESP32S3这样就完成了映射配置。
![WEB](/IMG/5.PNG)
#### 校准摇杆
进入设备和打印机页面
![WEB](/IMG/11.PNG)
右键ESP手柄 点击游戏控制器设置
![WEB](/IMG/12.PNG)
点击属性
![WEB](/IMG/13.PNG)
点击设置
![WEB](/IMG/14.PNG)
点击校准 完成所有校准步骤就可以正常使用了
![WEB](/IMG/15.PNG)
## 效果展示
![WEB](/IMG/FlyBus_TO_USB.png)
![Handle](/IMG/Handle.PNG)
