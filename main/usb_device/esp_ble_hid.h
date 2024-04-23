/* SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#if CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#else
#include "esp_bt_defs.h"
#if CONFIG_BT_BLE_ENABLED
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#endif
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#endif

#include "esp_hidd.h"
#include "esp_hid_gap.h"
#include "class/hid/hid_device.h"
typedef struct
{
    esp_hidd_dev_t *hid_dev;
    uint8_t protocol_mode;
    bool is_connected;
} local_param_t;

/**
 * @brief Initialize tinyusb HID device.
 *
 * @return
 *    - ESP_OK: Success
 *    - ESP_ERR_NO_MEM: No memory
 */
esp_err_t ble_hid_init(void);
void ble_hid_media_report(uint8_t report_id,uint8_t key0,uint8_t key1);
void ble_hid_keyboard_report(uint8_t report_id, uint8_t modifier, uint8_t keycode[6]);
void ble_hid_mouse_report(uint8_t report_id, uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal);
void ble_hid_surfacedial_report(uint8_t report_id, uint8_t keycode);

#ifdef __cplusplus
}
#endif
