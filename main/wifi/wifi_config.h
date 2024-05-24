#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__
#include <sys/param.h>

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "lwip/inet.h"

#include "esp_http_server.h"

#include "cJSON.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "../nvs_data/nvs_data.h"
// #include "wifi_data.h"

httpd_handle_t start_webserver(void);
void wifi_init_softap(void);
void wifi_station_task(void *pvParameters);
void wifi_init_sta();
void wifi_init(uint8_t mode);
void get_wifi_current_status(char *src);
void wifi_stop(void);
#endif