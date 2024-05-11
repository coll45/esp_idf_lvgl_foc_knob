/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __ESP_HIDD_API_H__
#define __ESP_HIDD_API_H__

#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_err.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_hidd_prf_api.h"
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "driver/gpio.h"


#include "usb_device.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_HIDD_EVENT_REG_FINISH = 0,
    ESP_BAT_EVENT_REG,
    ESP_HIDD_EVENT_DEINIT_FINISH,
    ESP_HIDD_EVENT_BLE_CONNECT,
    ESP_HIDD_EVENT_BLE_DISCONNECT,
    ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT,
    ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT,
} esp_hidd_cb_event_t;

/// HID config status
typedef enum {
    ESP_HIDD_STA_CONN_SUCCESS = 0x00,
    ESP_HIDD_STA_CONN_FAIL    = 0x01,
} esp_hidd_sta_conn_state_t;

/// HID init status
typedef enum {
    ESP_HIDD_INIT_OK = 0,
    ESP_HIDD_INIT_FAILED = 1,
} esp_hidd_init_state_t;

/// HID deinit status
typedef enum {
    ESP_HIDD_DEINIT_OK = 0,
    ESP_HIDD_DEINIT_FAILED = 0,
} esp_hidd_deinit_state_t;

#define LEFT_CONTROL_KEY_MASK        (1 << 0)
#define LEFT_SHIFT_KEY_MASK          (1 << 1)
#define LEFT_ALT_KEY_MASK            (1 << 2)
#define LEFT_GUI_KEY_MASK            (1 << 3)
#define RIGHT_CONTROL_KEY_MASK       (1 << 4)
#define RIGHT_SHIFT_KEY_MASK         (1 << 5)
#define RIGHT_ALT_KEY_MASK           (1 << 6)
#define RIGHT_GUI_KEY_MASK           (1 << 7)

typedef uint8_t key_mask_t;
/**
 * @brief HIDD callback parameters union
 */
typedef union {
    /**
	 * @brief ESP_HIDD_EVENT_INIT_FINISH
	 */
    struct hidd_init_finish_evt_param {
        esp_hidd_init_state_t state;				/*!< Initial status */
        esp_gatt_if_t gatts_if;
    } init_finish;							      /*!< HID callback param of ESP_HIDD_EVENT_INIT_FINISH */

    /**
	 * @brief ESP_HIDD_EVENT_DEINIT_FINISH
	 */
    struct hidd_deinit_finish_evt_param {
        esp_hidd_deinit_state_t state;				/*!< De-initial status */
    } deinit_finish;								/*!< HID callback param of ESP_HIDD_EVENT_DEINIT_FINISH */

    /**
     * @brief ESP_HIDD_EVENT_CONNECT
	 */
    struct hidd_connect_evt_param {
        uint16_t conn_id;
        esp_bd_addr_t remote_bda;                   /*!< HID Remote bluetooth connection index */
    } connect;									    /*!< HID callback param of ESP_HIDD_EVENT_CONNECT */

    /**
     * @brief ESP_HIDD_EVENT_DISCONNECT
	 */
    struct hidd_disconnect_evt_param {
        esp_bd_addr_t remote_bda;                   /*!< HID Remote bluetooth device address */
    } disconnect;									/*!< HID callback param of ESP_HIDD_EVENT_DISCONNECT */

    /**
     * @brief ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT
	 */
    struct hidd_vendor_write_evt_param {
        uint16_t conn_id;                           /*!< HID connection index */
        uint16_t report_id;                         /*!< HID report index */
        uint16_t length;                            /*!< data length */
        uint8_t  *data;                             /*!< The pointer to the data */
    } vendor_write;									/*!< HID callback param of ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT */

    /**
     * @brief ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT
     */
    struct hidd_led_write_evt_param {
        uint16_t conn_id;
        uint8_t report_id;
        uint8_t length;
        uint8_t *data;
    } led_write;
} esp_hidd_cb_param_t;


/**
 * @brief HID device event callback function type
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */
typedef void (*esp_hidd_event_cb_t) (esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param);



/**
 *
 * @brief           This function is called to receive hid device callback event
 *
 * @param[in]    callbacks: callback functions
 *
 * @return         ESP_OK - success, other - failed
 *
 */
esp_err_t esp_hidd_register_callbacks(esp_hidd_event_cb_t callbacks);

/**
 *
 * @brief           This function is called to initialize hid device profile
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_hidd_profile_init(void);

/**
 *
 * @brief           This function is called to de-initialize hid device profile
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_hidd_profile_deinit(void);

/**
 *
 * @brief           Get hidd profile version
 *
 * @return          Most 8bit significant is Great version, Least 8bit is Sub version
 *
 */
uint16_t esp_hidd_get_version(void);
esp_err_t ble_hid_init(void);
void ble_hid_media_report(uint8_t report_id,uint8_t key0,uint8_t key1);
void ble_hid_keyboard_report(uint8_t report_id, uint8_t modifier, uint8_t keycode[6]);
void ble_hid_mouse_report(uint8_t report_id, uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal);
void ble_hid_surfacedial_report(uint8_t report_id, uint8_t keycode);
extern bool sec_conn;
#ifdef __cplusplus
}
#endif

#endif /* __ESP_HIDD_API_H__ */
