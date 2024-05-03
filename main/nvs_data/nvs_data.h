#pragma once

#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "time.h"
#include "esp_mac.h"
#include "esp_app_desc.h"
#define SET_NVS_LIGHT "light"
#define SET_NVS_LOCK "lock"
#define SET_NVS_SLEEP "sleep"
#define SET_NVS_ROTATION "rotation"
#define SET_NVS_WIFI "wifi"
#define SET_NVS_FIRST_SCREEN "first_screen"
#define NVS_SYS_HID_INDEX "sys_hid_index"
#define NVS_CSM_HID_INDEX "csm_hid_index"   //custom hid index
#define NVS_FOC_ELECTRIC_ANGLE "foc_angle"

typedef struct 
{
    uint8_t set_light;
    uint8_t set_lock;
    uint8_t set_sleep;
    uint8_t set_rotation;
    uint8_t set_wifi;
    uint8_t set_frist_screen;
    uint8_t hid_sys_index;
    uint8_t hid_csm_index;
    char mac[6];
    float foc_angle;
    esp_app_desc_t *app_desc;
}SYS_CONFIG;
extern SYS_CONFIG sys_config;
void nvs_data_init();
void nvs_set_u8_data(const char* key_name,uint8_t value);
uint8_t nvs_get_u8_data(const char* key_name); 
void nvs_set_float_data(const char* key_name,float value);
float nvs_get_float_data(const char* key_name);