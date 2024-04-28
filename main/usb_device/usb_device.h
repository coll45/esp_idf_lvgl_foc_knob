#pragma once

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "class/hid/hid_device.h"
#include "../ui/ui.h"
#include "esp_ble_hid.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    uint8_t hid_id;
    uint8_t state;
    char hid_data[6];
}Command_HID;
extern QueueHandle_t HID_Queue;
extern local_param_t s_ble_hid_param;
void usb_device_uninstall(void);
void usb_device_init(void);
#ifdef __cplusplus

}
#endif
