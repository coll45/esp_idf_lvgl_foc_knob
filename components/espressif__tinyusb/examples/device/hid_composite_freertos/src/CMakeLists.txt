idf_component_register(SRCS "freertos_hook.c" "main.c" "usb_descriptors.c"
                    INCLUDE_DIRS "."
                    REQUIRES boards tinyusb_src)
