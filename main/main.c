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
    while (1)
    {   
        if(uxQueueMessagesWaiting(Dial_Queue) != 0)
        {
            if (xQueueReceive(Dial_Queue, &state, 0) == pdTRUE) 
            {   
                ESP_LOGI(TAG, "mune:%d,state:%d",ui_state.index,state);
                ui_dial_event(state);
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

}
void dial_event_queue_init()
{
    Dial_Queue = xQueueCreate(5,/* 消息队列的长度 */ 
                        sizeof(uint8_t));/* 消息的大小 */ 
    if (Dial_Queue != NULL)//判断队列是否创建成功
    {
        printf("Success\n");
        xTaskCreate(dial_event_task, "dial_event_task", 1024 *5, NULL, 10, NULL);
    }
}
void app_main(void)
{
    power_gpio_init();
    display_init();
    /* Show LVGL objects */
    lvgl_display_init();

    foc_init();
    /* 创建 Queue */ 
    dial_event_queue_init();


    // usb_device_init();
}
