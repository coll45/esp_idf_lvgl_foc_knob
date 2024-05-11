/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "esp_lvgl_port.h"
#include "display/display.h"
#include "dial/dial.h"
#include "usb_device/usb_device.h"
#include "dial_power/dial_power.h"
#include "./wifi/wifi_config.h"
#include "esp_ota_ops.h"
#include "nvs_data/nvs_data.h"
static const char *TAG = "MAIN";
QueueHandle_t Dial_Queue = NULL;
void dial_event_task()
{
    uint8_t state;
    uint8_t tick_num = 0;
    while (1)
    {   
        if (xQueueReceive(Dial_Queue, &state, portMAX_DELAY) == pdTRUE) 
        {   
            // ESP_LOGI(TAG, "mune:%d,state:%d",ui_state.index,state);
            switch (ui_state.index)
            {
            case UI_MENU_INTERFACE:
                ui_Screen1_dial_event(state);
                break;
            case UI_HID_INTERFACE:
                ui_Screen2_hid_event(state);
                break;
            case UI_SETTING_INTERFACE:
                ui_Screen_Setting_event(state);
                break;
            case UI_HID_CUSTOM_INTERFACE:
                ui_Screen3_Custom_hid_event(state);
                break;
            default:
                break;
            }
        }
        // vTaskDelay(10 / portTICK_PERIOD_MS);
    }

}
void dial_event_queue_init()
{
    Dial_Queue = xQueueCreate(5,/* 消息队列的长度 */ 
                        sizeof(uint8_t));/* 消息的大小 */ 
    if (Dial_Queue != NULL)//判断队列是否创建成功
    {
        printf("Success\n");
        xTaskCreate(dial_event_task, "dial_event_task", 1024 *5, NULL, 22, NULL);
    }
}
void esp_task_wdt_isr_user_handler()
{
    esp_restart();
}
void app_main(void)
{
    power_gpio_init();
    nvs_data_init();
    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();
    if (configured != running) {
                ESP_LOGI(TAG, "Configured OTA boot partition at offset 0x%08"PRIx32", but running from offset 0x%08"PRIx32,
                        configured->address, running->address);
                ESP_LOGI(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
            }
            ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08"PRIx32")",
                    running->type, running->subtype, running->address);
    esp_ota_set_boot_partition(running);

    /* 创建 Queue */ 
    dial_event_queue_init();
    foc_init();
    display_init();
    
    /* Show LVGL objects */
    lvgl_display_init();
    adc_read_init();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    knob_event_init();
    // wifi_init(1);
}   
