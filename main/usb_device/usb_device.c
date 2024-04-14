#include "usb_device.h"
static const char *TAG = "usb_device";
QueueHandle_t HID_Queue = NULL;
/************* TinyUSB descriptors ****************/


#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

/**
 * @brief HID report descriptor
 *
 * In this example we implement Keyboard + Mouse HID device,
 * so we must define both report descriptors
 */
const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE)),
    TUD_HID_REPORT_DESC_DIAL(HID_REPORT_ID(HID_ITF_PROTOCOL_DIAL)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(HID_ITF_PROTOCOL_MEDIAL))
};

/**
 * @brief String descriptor
 */
const char* hid_string_descriptor[5] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "TinyUSB",             // 1: Manufacturer
    "TinyUSB Device",      // 2: Product
    "123456",              // 3: Serials, should use chip ID
    "Example HID interface",  // 4: HID
};

/**
 * @brief Configuration descriptor
 *
 * This is a simple configuration descriptor that defines 1 configuration and 1 HID interface
 */
static const uint8_t hid_configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16, 10),
};

/********* TinyUSB HID callbacks ***************/

// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
    return hid_report_descriptor;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    printf("Set report :%d\n", report_id);
    
}
void usb_mouse_report(uint8_t state)
{
    if (tud_connected()) {
        switch (state)
        {
        case DIAL_STA_R:
            tud_hid_mouse_report(HID_ITF_PROTOCOL_MOUSE,0x00,10,0,0,0);
            break;
        case DIAL_STA_L:
            tud_hid_mouse_report(HID_ITF_PROTOCOL_MOUSE,0x00,-10,0,0,0);
            break;
        default:
            break;
        }
    }
}
void dial_hid_task()
{
    Command_HID cmd;
    while (1)
    {   
        if (xQueueReceive(HID_Queue, &cmd, portMAX_DELAY) == pdTRUE) 
        {   
            switch (cmd.hid_id)
            {
            case HID_ITF_PROTOCOL_DIAL:
                tud_hid_surfacedial_report(HID_ITF_PROTOCOL_DIAL,cmd.hid_data[0]);
                break;
            case HID_ITF_PROTOCOL_MOUSE:
                tud_hid_mouse_report(HID_ITF_PROTOCOL_MOUSE,cmd.hid_data[0],cmd.hid_data[1],cmd.hid_data[2],0,0);
                break;
            default:
                break;
            }
        }
    }

}
void dial_hid_queue_init()
{
    HID_Queue = xQueueCreate(5,/* 消息队列的长度 */ 
                        sizeof(Command_HID));/* 消息的大小 */ 
    if (HID_Queue != NULL)//判断队列是否创建成功
    {
        printf("dial_hid_task Success\n");
        xTaskCreate(dial_hid_task, "dial_hid_task", 1024 *4, NULL, 10, NULL);
    }
}
void usb_device_init(void)
{ 
    static uint8_t usb_init_flag = 0;
    if(usb_init_flag == 0)
    {
        usb_init_flag = 1;
        dial_hid_queue_init();
        const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = hid_string_descriptor,
        .string_descriptor_count = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]),
        .external_phy = false,
        .configuration_descriptor = hid_configuration_descriptor,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    }
}
void usb_device_report(uint8_t state)
{
    if (tud_connected()) {
        tud_hid_surfacedial_report(HID_ITF_PROTOCOL_DIAL,state);
    }
}
void usb_device_uninstall(void)
{
    //not use
    ESP_ERROR_CHECK(tinyusb_driver_uninstall());
}