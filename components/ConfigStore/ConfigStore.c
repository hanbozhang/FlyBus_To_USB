#include "ConfigStore.h"

bool ConfigStore_Begin(void){
     // 初始化 NVS
    if (nvs_flash_init_partition(Name)!= ESP_OK)
    {
        printf("NVS：初始化失败 ！ \n");
        return false;
    }
    printf("NVS：初始化成功 ！\n");
    return true;
}
bool ConfigStore_Save_CTUConfig(void){
    nvs_handle_t my_handle;
    if (nvs_open_from_partition(Name, NameSpace, NVS_READWRITE, &my_handle)!=ESP_OK)
    {
        printf("NVS:打开存储区失败 ！\n");
        return false;
    }
    nvs_set_blob(my_handle,ConfigKey_CTUConfig,CfgsCTU,sizeof(CfgsCTU));
    if (nvs_commit(my_handle)!=ESP_OK)
    {
        printf("NVS:写入失败 ！\n");
        nvs_close(my_handle);
        return false;
    }
    nvs_close(my_handle);
    printf("NVS:写入成功 CTUConfig！\n");
    return true;
}
bool ConfigStore_Save_BusMode(uint8_t value){
    nvs_handle_t my_handle;
    if (nvs_open_from_partition(Name, NameSpace, NVS_READWRITE, &my_handle)!=ESP_OK)
    {
        printf("NVS:打开存储区失败 ！\n");
        return false;
    }
    nvs_set_u8(my_handle,ConfigKey_BusMode,value);
    if (nvs_commit(my_handle)!=ESP_OK)
    {
        printf("NVS:写入失败 ！\n");
        nvs_close(my_handle);
        return false;
    }
    nvs_close(my_handle);
    printf("NVS:写入成功 ！BusMode\n");
    return true;
}
void ConfigStore_Set_CTUConfig(Store_CHTOUSB_t value){
    switch ((uint8_t)CTU_Get_Type(value))
    {
    case 0x01:
        if (CTU_Get_UCH(value)!=0&&CTU_Get_UCH(value)<=4)
        {
            CfgsCTU[CTU_Get_UCH(value)-1]=value;
            printf("高精度摇杆 遥控通道：%d USB通道：%d \n",CTU_Get_FBCH(value),CTU_Get_UCH(value));
        }
        break;
    case 0x02:
        if (CTU_Get_UCH(value)!=0&&CTU_Get_UCH(value)<=4)
        {
            CfgsCTU[CTU_Get_UCH(value)+3]=value;
            printf("低精度滑块 遥控通道：%d USB通道：%d \n",CTU_Get_FBCH(value),CTU_Get_UCH(value));
        }
        break;
    case 0x03:
        if (CTU_Get_UCH(value)!=0&&CTU_Get_UCH(value)<=32)
        {
            CfgsCTU[CTU_Get_UCH(value)+7]=value;
            printf("按键 遥控通道：%d USB通道：%d 类型：%d 比对值：%d \n",CTU_Get_FBCH(value),CTU_Get_UCH(value),CTU_Get_BTLC(value),value.Trigger_value);
        }
        break;
    default:
        break;
    }
}
//获取全部通道配置
bool ConfigStore_Get_CTUConfig(void){
    nvs_handle_t my_handle;
    size_t requestSize;
    if (nvs_open_from_partition(Name, NameSpace, NVS_READONLY, &my_handle)!=ESP_OK)
    {
        printf("NVS:打开存储区失败 ！\n");
        return false;
    }
    if (nvs_get_blob(my_handle,ConfigKey_CTUConfig,CfgsCTU,&requestSize)!=ESP_OK)
    {
        printf("NVS:读取失败 ！\n");
        nvs_close(my_handle);
        return false;
    }
    nvs_close(my_handle);
    printf("NVS:读取成功 ！大小：%d \n",requestSize);
    return true;
}
//获取通道配置指针
Store_CHTOUSB_t * ConfigStore_Get_Configs(void)
{
    return &CfgsCTU;
}
//获取单个USB通道配置信息 输入范围0-39
Data_CHTOUSB_t ConfigStore_Get_Config(uint8_t index)
{
    Data_CHTOUSB_t req;
    if (index<40)
    {
        req.type=CTU_Get_Type(CfgsCTU[index]);
        req.flyBus_channel_id=CTU_Get_FBCH(CfgsCTU[index]);
        req.usb_channel_id=CTU_Get_UCH(CfgsCTU[index]);
        req.button_type=CTU_Get_BTLC(CfgsCTU[index]);
        req.trigger_value=CfgsCTU[index].Trigger_value;
    }
    return req;
}
bool ConfigStore_Get_BusMode(uint8_t* value){
    nvs_handle_t my_handle;
    if (nvs_open_from_partition(Name, NameSpace, NVS_READONLY, &my_handle)!=ESP_OK)
    {
        printf("NVS:打开存储区失败 ！\n");
        return false;
    }
    if (nvs_get_u8(my_handle,ConfigKey_BusMode,value)!=ESP_OK)
    {
        printf("NVS:读取失败 ！\n");
        nvs_close(my_handle);
        return false;
    }
    nvs_close(my_handle);
    printf("NVS:读取成功 ！类型：%d \n",*value);
    return true;
}
