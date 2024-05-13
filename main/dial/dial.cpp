#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_simplefoc.h"
#include "iot_button.h"
#include "../usb_device/usb_device.h"
#include "../ui/ui.h"
#include "../nvs_data/nvs_data.h"
#include "dial.h"

#define TAG "FOC_Knob_Example"
static foc_knob_handle_t foc_knob_handle = NULL;
static int mode = MOTOR_UNBOUND_NO_DETENTS;
static bool motor_shake = false;
int8_t press_rotation = 0;
/*update motor parameters based on hardware design*/
BLDCDriver3PWM driver = BLDCDriver3PWM(PHASE_U_GPIO, PHASE_V_GPIO, PHASE_W_GPIO);
BLDCMotor motor = BLDCMotor(MOTOR_PP);
MT6701 mt6701 = MT6701(MT6701_SPI_HOST, (gpio_num_t)MT6701_SPI_SCLK_GPIO, (gpio_num_t)MT6701_SPI_MISO_GPIO, (gpio_num_t)MT6701_SPI_MOSI_GPIO, (gpio_num_t)MT6701_SPI_CS_GPIO);
extern QueueHandle_t Dial_Queue;
void dial_publish(uint8_t state) {
    uint8_t send_state;
    send_state = state;
    xQueueSend(Dial_Queue,&send_state,0);
    // if(ui_state.index == UI_HID_INTERFACE)
    //     xQueueSend(HID_Queue,&send_state,0);
}
void set_motor_zero_electric_angle()
{
    if(nvs_get_float_data(NVS_FOC_ELECTRIC_ANGLE)>0)//如果有值那么设置成0
        nvs_set_float_data(NVS_FOC_ELECTRIC_ANGLE,0);
    else
        nvs_set_float_data(NVS_FOC_ELECTRIC_ANGLE,motor.zero_electric_angle);
}
/*Motor initialization*/
void motor_init(void)
{
    SimpleFOCDebug::enable();
    Serial.begin(115200);
    mt6701.init();
    motor.linkSensor(&mt6701);
    driver.voltage_power_supply = 5;
    driver.voltage_limit = 4;

#if USING_MCPWM
    driver.init(0);
#else
    driver.init({LEDC_CHAN_0, LEDC_CHAN_1, LEDC_CHAN_2});
#endif

    motor.linkDriver(&driver);
    motor.foc_modulation = SpaceVectorPWM;
    motor.controller = MotionControlType::torque;

    motor.PID_velocity.P = 4;
    motor.PID_velocity.I = 0;
    motor.PID_velocity.D = 0.04;
    motor.PID_velocity.output_ramp = 10000;
    motor.PID_velocity.limit = 10;

    motor.useMonitoring(Serial);
    motor.init();                                        // initialize motor
    float ee_angle = 0;
    ee_angle = sys_config.foc_angle;
    if(ee_angle>0)
    {
        motor.sensor_direction = Direction::CCW;
        motor.zero_electric_angle = ee_angle;
    }
    motor.initFOC();                                     // align sensor and start FOC
    
    printf("Motor motor.zero_electric_angle %f\n",motor.zero_electric_angle);
    ESP_LOGI(TAG, "Motor Initialize Successfully");
}

static void button_press_cb(void *arg, void *data)
{
    //按键按下
    press_rotation = 1;
    // ESP_LOGI(TAG, "press down:");
    dial_publish(DIAL_STA_PRESS);
    motor_shake = true;
}
static void button_press_up_cb(void *arg, void *data)
{
    //按键弹起
    if(press_rotation != 1)
        press_rotation = 0;
    // ESP_LOGI(TAG, "press up:");
    dial_publish(DIAL_STA_RELEASE);
}
static void button_single_click_cb(void *arg, void *data)
{
    //按键单击（期间如果旋钮旋转就不会触发按键单击
    if(press_rotation == 1)
        dial_publish(DIAL_STA_CLICK);
    press_rotation = 0;
    ESP_LOGI(TAG, "single click:");
}
static void button_double_click_cb(void *arg, void *data)
{
    //单击双击
    //  if(press_rotation == 1)
        dial_publish(DIAL_STA_DOUBLE_CLICK);
    press_rotation = 0;
    ESP_LOGI(TAG, "double clic");
}
static void button_long_press_start_cb(void *arg, void *data)
{
    //按键长按
    if(press_rotation == 1)
        dial_publish(DIAL_STA_LONG_PRESS);
    // ESP_LOGI(TAG, "long press start");
}
static void button_long_press_up_cb(void *arg, void *data)
{
    //按键长按抬起
    press_rotation = 0;
    dial_publish(DIAL_STA_LONG_PRESS_UP);
    // ESP_LOGI(TAG, "long press up");
}
static void foc_knob_inc_cb(void *arg, void *data)
{
    //判断按键按下flag，如果按下了会从1变到2
    if(press_rotation == 1)
        press_rotation = 2;
    if(gpio_get_level(SWITCH_BUTTON))
    {
        dial_publish(DIAL_STA_R);
    }
    else
    {
        dial_publish(DIAL_STA_P_R);
    }
    foc_knob_state_t state;
    foc_knob_get_state(arg, &state);
    // ESP_LOGI(TAG, "foc_knob_inc_cb: position: %" PRId32 "\n", state.position);
}

static void foc_knob_dec_cb(void *arg, void *data)
{   
    //if press rotation flag
    if(press_rotation == 1)
        press_rotation = 2;
    if(gpio_get_level(SWITCH_BUTTON))
    {
        dial_publish(DIAL_STA_L);
    }
    else
    {
        dial_publish(DIAL_STA_P_L);
    }
    foc_knob_state_t state;
    foc_knob_get_state(arg, &state);
    // ESP_LOGI(TAG, "foc_knob_dec_cb: position: %" PRId32 "\n", state.position);
}

static void foc_knob_h_lim_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "foc_knob_h_lim_cb");
}

static void foc_knob_l_lim_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "foc_knob_l_lim_cb");
}
static float motor_pid_cb(float P, float D, float limit, float error)
{
    motor.PID_velocity.limit = limit;
    motor.PID_velocity.P = P;
    motor.PID_velocity.D = D;
    return motor.PID_velocity(error);
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
float get_motor_shaft_angle(void)
{
    return motor.shaft_angle;
}
float get_motor_shaft_velocity(void)
{
    return motor.shaft_velocity;
}
uint16_t get_fknob_shaft_angle(void)
{
    foc_knob_state_t state;
    foc_knob_get_state(foc_knob_handle, &state);
    float adjusted_sub_position, raw_angle, adjusted_angle; 
    uint16_t img_angle;
    
    adjusted_sub_position = state.angle_to_detent_center * state.position_width_radians;
    raw_angle = state.position * state.position_width_radians;
    adjusted_angle = (raw_angle - adjusted_sub_position);
    if (adjusted_angle > 0)
        img_angle = (uint32_t)(adjusted_angle * 573) % 3600;
    else
        img_angle = 3600 - (uint32_t)(fabs(adjusted_angle) * 573) % 3600;
    return img_angle;
}
void foc_knob_set_param(foc_knob_param_t param)
{
    foc_knob_set_param_list(foc_knob_handle, param);
}
void knob_event_init()
{
    button_config_t btn_config = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = 2000,
        .short_press_time = 500,
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
}
void foc_init()
{
    motor_init();

    foc_knob_config_t cfg = {
        .param_lists = default_foc_knob_param_lst,
        .param_list_num = MOTOR_MAX_MODES,
        .max_torque_out_limit = 5,
        .max_torque = 5,
        .pid_cb = motor_pid_cb,
    };
    foc_knob_handle = foc_knob_create(&cfg);
    // foc_knob_change_mode(foc_knob_handle, 1);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_INC, foc_knob_inc_cb, NULL);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_DEC, foc_knob_dec_cb, NULL);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_H_LIM, foc_knob_h_lim_cb, NULL);
    foc_knob_register_cb(foc_knob_handle, FOC_KNOB_L_LIM, foc_knob_l_lim_cb, NULL);
    xTaskCreate(motor_task, "motor_task", 4096, NULL, 24, NULL);
    
}
