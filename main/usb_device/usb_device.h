#pragma once

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "class/hid/hid_device.h"


#ifdef __cplusplus
extern "C" {
#endif
void usb_device_uninstall(void);
void usb_device_init(void);
void usb_device_report(uint8_t state);
#ifdef __cplusplus

}
#endif
