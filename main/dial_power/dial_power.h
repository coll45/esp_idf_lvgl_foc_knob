#pragma once

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_sleep.h"
#include "../dial/dial.h"
#include "../display/display.h"
#include "../nvs_data/nvs_data.h"
#include "time.h"
#include "math.h"
#define EXAMPLE_ADC_UNIT                    ADC_UNIT_1
#define _EXAMPLE_ADC_UNIT_STR(unit)         #unit
#define EXAMPLE_ADC_UNIT_STR(unit)          _EXAMPLE_ADC_UNIT_STR(unit)
#define EXAMPLE_ADC_CONV_MODE               ADC_CONV_SINGLE_UNIT_1
#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_12
#define EXAMPLE_ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define EXAMPLE_ADC_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
#define EXAMPLE_ADC_GET_DATA(p_data)        ((p_data)->type1.data)
#else
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define EXAMPLE_ADC_GET_CHANNEL(p_data)     ((p_data)->type2.channel)
#define EXAMPLE_ADC_GET_DATA(p_data)        ((p_data)->type2.data)
#endif

#define EXAMPLE_READ_LEN                    256

#if CONFIG_IDF_TARGET_ESP32
static adc_channel_t channel[2] = {ADC_CHANNEL_6, ADC_CHANNEL_7};
#else
static adc_channel_t adc_channel[1] = { ADC_CHANNEL_3};   //GPIO4
#endif

#define R1_VOLTAGE 22000  //22K
#define R2_VOLTAGE 22000  //22K
#define REFERENCE_VOL 3300 //3.3V
#define POWER_GPIO GPIO_NUM_18
#define SET_SCREEN_LOCK_TIME_SRC "30s\n1min\n3min\n5min\n10min\n15min\nnever"
#define SET_SYSTEM_SLEEP_TIME_SRC "1min\n3min\n5min\n10min\n15min\n30min\nnever"

void adc_read_init();
void power_gpio_init(void);
void power_on(void);
void power_off(void);
void bat_power_get(void);
uint8_t bat_val_get();
uint32_t bat_adc_get();
void set_new_lock_time(uint8_t val);
void set_new_sleep_time(uint8_t val);