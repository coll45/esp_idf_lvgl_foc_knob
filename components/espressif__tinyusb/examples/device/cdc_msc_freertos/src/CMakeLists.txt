# This file is for ESP-IDF only
idf_component_register(SRCS "freertos_hook.c" "main.c" "usb_descriptors.c" "msc_disk.c"
                    INCLUDE_DIRS "."
                    REQUIRES boards tinyusb_src)
