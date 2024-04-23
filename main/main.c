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

#include "esp_ota_ops.h"
#include "nvs_flash.h"
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
void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

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
    power_gpio_init();
    display_init();
    /* Show LVGL objects */
    lvgl_display_init();
    set_screen_light(50);
    foc_init();
    /* 创建 Queue */ 
    dial_event_queue_init();

    ble_hid_init();
}   
