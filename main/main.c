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
    power_gpio_init();
    display_init();
    foc_init();
    /* Show LVGL objects */
    lvgl_display_init();
    set_screen_light(50);
    
    /* 创建 Queue */ 
    dial_event_queue_init();

}
