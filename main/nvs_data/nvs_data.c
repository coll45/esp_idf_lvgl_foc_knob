#include "nvs_data.h"
static const char *TAG = "NVS_DATA";
nvs_handle_t nvs_data_handle;   
typedef union
{
    float f;
    unsigned int d;
}FTOD;
static void nvs_default_data_set()
{
    nvs_set_u8_data("flag",1);
    nvs_set_u8_data(SET_NVS_LIGHT,50);
    nvs_set_u8_data(SET_NVS_LOCK,3); //5min
    nvs_set_u8_data(SET_NVS_SLEEP,3); //10min
    nvs_set_u8_data(SET_NVS_ROTATION,0);
    nvs_set_u8_data(SET_NVS_WIFI,0);
    nvs_set_float_data(NVS_FOC_ELECTRIC_ANGLE,0);
    ESP_LOGI(TAG,"nvs_default_data_set");
}
void nvs_data_init()
{
    esp_err_t ret;
    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    if(nvs_get_u8_data("flag") != 1)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        ESP_ERROR_CHECK(ret);
        nvs_default_data_set();
    }
}
void nvs_set_u8_data(const char* key_name,uint8_t value)
{
    esp_err_t ret;
    ret = nvs_open("main", NVS_READWRITE, &nvs_data_handle);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "opening NVS Error (%s)!\n", esp_err_to_name(ret));
    } 
    else
    {
        ret = nvs_set_u8(nvs_data_handle, key_name, value);
        if(ret!=ESP_OK) ESP_LOGE(TAG, "%s set Error",key_name);
    }
    nvs_close(nvs_data_handle);
}
uint8_t nvs_get_u8_data(const char* key_name)
{
    uint8_t value = 0;
    esp_err_t ret;
    ret = nvs_open("main", NVS_READWRITE, &nvs_data_handle);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "opening NVS Error (%s)!\n", esp_err_to_name(ret));
    } 
    else
    {
        ret = nvs_get_u8(nvs_data_handle, key_name, &value);
        if(ret!=ESP_OK) ESP_LOGE(TAG, "%s get Error",key_name);
    }
    nvs_close(nvs_data_handle);
    return value;
}
void nvs_set_float_data(const char* key_name,float value)
{
    FTOD da;
    da.f = value;
    esp_err_t ret;
    ret = nvs_open("main", NVS_READWRITE, &nvs_data_handle);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "opening NVS Error (%s)!\n", esp_err_to_name(ret));
    } 
    else
    {
        ret = nvs_set_u32(nvs_data_handle, key_name, da.d);
        if(ret!=ESP_OK) ESP_LOGE(TAG, "%s set Error",key_name);
    }
    nvs_close(nvs_data_handle);
}
float nvs_get_float_data(const char* key_name)
{
    FTOD da;
    esp_err_t ret;
    ret = nvs_open("main", NVS_READWRITE, &nvs_data_handle);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "opening NVS Error (%s)!\n", esp_err_to_name(ret));
    } 
    else
    {
        ret = nvs_get_u32(nvs_data_handle, key_name, &da.d);
        if(ret!=ESP_OK) ESP_LOGE(TAG, "%s get Error",key_name);
    }
    nvs_close(nvs_data_handle);
    return da.f;
}