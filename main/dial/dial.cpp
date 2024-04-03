#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "foc_knob.h"
#include "foc_knob_default.h"
#include "esp_simplefoc.h"
#include "iot_button.h"
#include "dial.h"
#include "../usb_device/usb_device.h"

#define TAG "FOC_Knob_Example"
static foc_knob_handle_t foc_knob_handle = NULL;
static int mode = MOTOR_UNBOUND_NO_DETENTS;
static bool motor_shake = false;
int16_t enc_num = 0;
int8_t enc_click = 0;
int8_t press_rotation = 0;
/*update motor parameters based on hardware design*/
BLDCDriver3PWM driver = BLDCDriver3PWM(PHASE_U_GPIO, PHASE_V_GPIO, PHASE_W_GPIO);
BLDCMotor motor = BLDCMotor(MOTOR_PP);
MT6701 mt6701 = MT6701(MT6701_SPI_HOST, (gpio_num_t)MT6701_SPI_SCLK_GPIO, (gpio_num_t)MT6701_SPI_MISO_GPIO, (gpio_num_t)MT6701_SPI_MOSI_GPIO, (gpio_num_t)MT6701_SPI_CS_GPIO);

/*Motor initialization*/
void motor_init(void)
{
    SimpleFOCDebug::enable();
    Serial.begin(115200);
    mt6701.init();
    motor.linkSensor(&mt6701);
    driver.voltage_power_supply = 5;
    driver.voltage_limit = 5;

#if USING_MCPWM
    driver.init(0);
#else
    driver.init({LEDC_CHAN_0, LEDC_CHAN_1, LEDC_CHAN_2});
#endif

    motor.linkDriver(&driver);
    motor.foc_modulation = SpaceVectorPWM;
    motor.controller = MotionControlType::torque;

    motor.useMonitoring(Serial);
    motor.init();                                        // initialize motor
    // motor.sensor_direction = Direction::CCW;
    // motor.zero_electric_angle = 0.81;
    motor.initFOC();                                     // align sensor and start FOC
    
    ESP_LOGI(TAG, "Motor motor.zero_electric_angle %f",motor.zero_electric_angle);
    ESP_LOGI(TAG, "Motor Initialize Successfully");
}

static void button_press_cb(void *arg, void *data)
{
    press_rotation = 1;
    ESP_LOGI(TAG, "press down:");
    usb_device_report(DIAL_PRESS);
    // tud_hid_surfacedial_report(HID_ITF_PROTOCOL_DIAL,DIAL_PRESS);
    foc_knob_change_mode(foc_knob_handle, 2);
    motor_shake = true;
}
static void button_press_up_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "press up:");
    usb_device_report(DIAL_RELEASE);
    // tud_hid_surfacedial_report(HID_ITF_PROTOCOL_DIAL,DIAL_RELEASE);
    foc_knob_change_mode(foc_knob_handle, 1);
}
static void button_single_click_cb(void *arg, void *data)
{
    if(press_rotation)
        enc_click = 1;
    press_rotation = 0;
    ESP_LOGI(TAG, "single click:");
}
static void button_double_click_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "double clic");
}
static void button_long_press_start_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "long press up");
}
static void button_long_press_up_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "long press up");
}
static void foc_knob_inc_cb(void *arg, void *data)
{
    //if press rotation clear flag
    if(press_rotation)
        press_rotation = 0;
        
    enc_num ++;
    foc_knob_state_t state;
    foc_knob_get_state(arg, &state);
    usb_device_report(DIAL_R);
    // tud_hid_surfacedial_report(HID_ITF_PROTOCOL_DIAL,DIAL_R);
    ESP_LOGI(TAG, "foc_knob_inc_cb: position: %" PRId32 "\n", state.position);
}

static void foc_knob_dec_cb(void *arg, void *data)
{
    if(press_rotation)
        press_rotation = 0;

    enc_num --;
    foc_knob_state_t state;
    foc_knob_get_state(arg, &state);
    usb_device_report(DIAL_L);
    // tud_hid_surfacedial_report(HID_ITF_PROTOCOL_DIAL,DIAL_L);
    ESP_LOGI(TAG, "foc_knob_dec_cb: position: %" PRId32 "\n", state.position);
}

static void foc_knob_h_lim_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "foc_knob_h_lim_cb");
}

static void foc_knob_l_lim_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "foc_knob_l_lim_cb");
}

float motor_shake_func(float strength, int delay_cnt)
{
    static int time_cnt = 0;
    if (time_cnt < delay_cnt) {
        time_cnt++;
        return strength;
    } else if (time_cnt < 2 * delay_cnt) {
        time_cnt++;
        return -strength;
    } else {
        time_cnt = 0;
        motor_shake = false;
        return 0;
    }
}

static void motor_task(void *arg)
{
    static float torque = 0;
    while (1) {
        motor.loopFOC();
        if (motor_shake) {
            torque = motor_shake_func(4, 4);
        } else {
            torque = foc_knob_run(foc_knob_handle, motor.shaft_velocity, motor.shaft_angle);
        }
        motor.move(torque);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void foc_init()
{
    button_config_t btn_config = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = 2000,
        .short_press_time = 350,
        .gpio_button_config = {
            .gpio_num = SWITCH_BUTTON,
            .active_level = 0,
        }
    };

    button_handle_t btn =  iot_button_create(&btn_config);
    iot_button_register_cb(btn, BUTTON_PRESS_DOWN, button_press_cb, NULL);
    iot_button_register_cb(btn, BUTTON_PRESS_UP, button_press_up_cb, NULL);
    iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, button_single_click_cb, NULL);
    iot_button_register_cb(btn, BUTTON_DOUBLE_CLICK, button_double_click_cb, NULL);
    iot_button_register_cb(btn, BUTTON_LONG_PRESS_START, button_long_press_start_cb, NULL);
    iot_button_register_cb(btn, BUTTON_LONG_PRESS_UP, button_long_press_up_cb, NULL);

    motor_init();

    foc_knob_config_t cfg = {
        .param_lists = default_foc_knob_param_lst,
        .param_list_num = MOTOR_MAX_MODES,
        .max_torque_out_limit = 5,
        .max_torque = 4,
    };

    foc_knob_handle = foc_knob_create(&cfg);
    foc_knob_change_mode(foc_knob_handle, 1);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_INC, foc_knob_inc_cb, NULL);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_DEC, foc_knob_dec_cb, NULL);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_H_LIM, foc_knob_h_lim_cb, NULL);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_L_LIM, foc_knob_l_lim_cb, NULL);

    xTaskCreate(motor_task, "motor_task", 4096, NULL, 6, NULL);
}
