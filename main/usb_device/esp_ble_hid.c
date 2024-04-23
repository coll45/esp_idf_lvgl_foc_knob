#include "esp_ble_hid.h"

static const char *TAG = "BLE_HID";

local_param_t s_ble_hid_param = {0};

const uint8_t hid_report_general_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(HID_ITF_PROTOCOL_MEDIA))
};
const uint32_t desc_hid_report_general_len = sizeof(hid_report_general_descriptor);
const uint8_t hid_report_descriptor_dila[] = {
    TUD_HID_REPORT_DESC_DIAL(HID_REPORT_ID(HID_ITF_PROTOCOL_DIAL)),
};
const uint32_t desc_hid_report_dial_len = sizeof(hid_report_descriptor_dila);
static esp_hid_raw_report_map_t ble_report_maps[] = {
    /* This block is compiled for bluedroid as well */
    {
        .data = hid_report_general_descriptor,
        .len = desc_hid_report_general_len,
    },
    {
        .data = hid_report_descriptor_dila,
        .len = desc_hid_report_dial_len,
    }
};
static esp_hid_device_config_t ble_hid_config = {
    .vendor_id          = 0x16C0,
    .product_id         = 0x05DF,
    .version            = 0x0100,
    .device_name        = "SuperDial",
    .manufacturer_name  = "Espressif",
    .serial_number      = "1234567890",
    .report_maps        = ble_report_maps,
    .report_maps_len    = 2
};
void ble_hid_surfacedial_report(uint8_t report_id, uint8_t keycode)
{   
    uint8_t report[2];
    int16_t dial_step;
    if(keycode<2)
    {
      report[0] = keycode;
      report[1] = 0;
    }
    if(keycode == DIAL_R)
    {
      dial_step = DIAL_UNIT<<1;
      report[0] = dial_step&0xFF;
      report[1] = dial_step>>8;
    }
    if(keycode == DIAL_L)
    {
      dial_step = -(DIAL_UNIT)<<1;
      report[0] = dial_step&0xFF;
      report[1] = dial_step>>8;
    }
    esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 1, report_id, report, sizeof(report));
}
void ble_hid_mouse_report(uint8_t report_id,
                            uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal)
{
    hid_mouse_report_t report =
    {
        .buttons = buttons,
        .x       = x,
        .y       = y,
        .wheel   = vertical,
        .pan     = horizontal
    };
    esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, report_id, &report, sizeof(report));
}
void ble_hid_keyboard_report(uint8_t report_id, uint8_t modifier, uint8_t keycode[6])
{
  hid_keyboard_report_t report;

  report.modifier = modifier;
  report.reserved = 0;

  if ( keycode )
  {
    memcpy(report.keycode, keycode, sizeof(report.keycode));
  }else
  {
    tu_memclr(report.keycode, 6);
  }

  esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, report_id, &report, sizeof(report));
}
void ble_hid_media_report(uint8_t report_id,uint8_t key0,uint8_t key1)
{
    uint8_t report[2];
    report[0] = key0;
    report[1] = key1;
    esp_hidd_dev_input_set(s_ble_hid_param.hid_dev, 0, report_id, report, sizeof(report));
}
static void ble_hidd_event_callback(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    esp_hidd_event_t event = (esp_hidd_event_t)id;
    esp_hidd_event_data_t *param = (esp_hidd_event_data_t *)event_data;

    switch (event) {
    case ESP_HIDD_START_EVENT: {
        ESP_LOGI(TAG, "START");
        esp_hid_ble_gap_adv_start();
        break;
    }
    case ESP_HIDD_CONNECT_EVENT: {
        s_ble_hid_param.is_connected = true;
        ESP_LOGI(TAG, "CONNECT");
        break;
    }
    case ESP_HIDD_PROTOCOL_MODE_EVENT: {
        ESP_LOGI(TAG, "PROTOCOL MODE[%u]: %s", param->protocol_mode.map_index, param->protocol_mode.protocol_mode ? "REPORT" : "BOOT");
        break;
    }
    case ESP_HIDD_CONTROL_EVENT: {
        // ESP_LOGI(TAG, "CONTROL[%u]: %sSUSPEND", param->control.map_index, param->control.control ? "EXIT_" : "");
        break;
    }
    case ESP_HIDD_OUTPUT_EVENT: {
        // ESP_LOGI(TAG, "OUTPUT[%u]: %8s ID: %2u, Len: %d, Data:", param->output.map_index, esp_hid_usage_str(param->output.usage), param->output.report_id, param->output.length);
        // ESP_LOG_BUFFER_HEX(TAG, param->output.data, param->output.length);
        break;
    }
    case ESP_HIDD_FEATURE_EVENT: {
        // ESP_LOGI(TAG, "FEATURE[%u]: %8s ID: %2u, Len: %d, Data:", param->feature.map_index, esp_hid_usage_str(param->feature.usage), param->feature.report_id, param->feature.length);
        // ESP_LOG_BUFFER_HEX(TAG, param->feature.data, param->feature.length);
        break;
    }
    case ESP_HIDD_DISCONNECT_EVENT: {
        s_ble_hid_param.is_connected = false;
        ESP_LOGI(TAG, "DISCONNECT: %s", esp_hid_disconnect_reason_str(esp_hidd_dev_transport_get(param->disconnect.dev), param->disconnect.reason));
        esp_hid_ble_gap_adv_start();
        break;
    }
    case ESP_HIDD_STOP_EVENT: {
        ESP_LOGI(TAG, "STOP");
        break;
    }
    default:
        break;
    }
    return;
}
esp_err_t ble_hid_init(void)
{
    esp_err_t ret;
    ret = esp_hid_gap_init(HIDD_BLE_MODE);
    ESP_ERROR_CHECK(ret);

    // ble_hid_config.report_maps[0].len = desc_hid_report_len;
    ret = esp_hid_ble_gap_adv_init(ESP_HID_APPEARANCE_GENERIC, ble_hid_config.device_name);
    ESP_ERROR_CHECK(ret);

    if ((ret = esp_ble_gatts_register_callback(esp_hidd_gatts_event_handler)) != ESP_OK) {
        ESP_LOGE(TAG, "GATTS register callback failed: %d", ret);
    }
    ESP_LOGI(TAG, "setting ble device");
    ESP_ERROR_CHECK(
        esp_hidd_dev_init(&ble_hid_config, ESP_HID_TRANSPORT_BLE, ble_hidd_event_callback, &s_ble_hid_param.hid_dev));
    return ret;
}

