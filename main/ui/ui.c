// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: dial

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////

#define TAG "UI"
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);

lv_obj_t * ui_Screen1;
lv_obj_t * ui_Screen2;
lv_obj_t * ui_Screen3;
lv_obj_t * ui_Screen_Setting;
uint8_t ui_event_flag = 0;
int16_t enc_num = 0;
int16_t enc_click = 0;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=1
    #error "LV_COLOR_16_SWAP should be 1 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
lv_indev_t* encoder_indev;
static lv_indev_drv_t indev_drv;
_ui_state ui_state;
void ui_send_hid_command(uint8_t hid_id,uint8_t keycode[6],uint8_t state)
{
    Command_HID cmd = {
        .hid_id = hid_id,
        .state = state,
        .hid_data[0] = keycode[0],
        .hid_data[1] = keycode[1],
        .hid_data[2] = keycode[2],
        .hid_data[3] = keycode[3],
        .hid_data[4] = keycode[4],
        .hid_data[5] = keycode[5],
    };
    xQueueOverwrite(HID_Queue, &cmd);
}
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

}
void ui_init(void)
{
    lv_port_indev_init();
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    // ui_Screen2_screen_init();
    lv_disp_load_scr(ui_Screen1);
}
