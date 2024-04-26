#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"

#define SET_NVS_LIGHT "light"
#define SET_NVS_LOCK "lock"
#define SET_NVS_SLEEP "sleep"
#define SET_NVS_ROTATION "rotation"
#define SET_NVS_WIFI "wifi"
#define NVS_FOC_ELECTRIC_ANGLE "foc_angle"
void nvs_data_init();
void nvs_set_u8_data(const char* key_name,uint8_t value);
uint8_t nvs_get_u8_data(const char* key_name); 
void nvs_set_float_data(const char* key_name,float value);
float nvs_get_float_data(const char* key_name);