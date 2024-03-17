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
#include "foc/foc.h"
extern int16_t enc_num;
extern int8_t enc_click;
static const char *TAG = "MAIN";
static lv_indev_t* encoder_indev;
static lv_indev_drv_t indev_drv;
static void encoder_init(void)
{
    // gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
    /*Do it in Init*/
}
/* Will be called by the library to read the encoder */
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    data->enc_diff = enc_num;
    enc_num = 0;

    if(enc_click)
    {
        data->state =  LV_INDEV_STATE_PR;
        enc_click = 0;
    }
    else
        data->state = LV_INDEV_STATE_REL;
}
static void lv_port_indev_init(void)
{

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    // indev_drv.long_press_time = 2000;           // 按下 2s 为长按
    // indev_drv.long_press_repeat_time = 500;    // 间隔 0.5s 发送LV_EVENT_LONG_PRESSED_REPEAT 事件
    encoder_indev = lv_indev_drv_register(&indev_drv);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_encoder, group);` */
    lv_group_t * group = lv_group_create();
    lv_indev_set_group(encoder_indev, group);

	lv_group_add_obj(group, ui_Arc1);
    lv_group_add_obj(group, ui_Button1);

}
void app_main(void)
{
    display_init();
    /* Show LVGL objects */
    lvgl_display_init();

    foc_init();

    lv_port_indev_init();
}
