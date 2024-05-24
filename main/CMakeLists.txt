file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")
file(GLOB_RECURSE DISPLAY ${CMAKE_SOURCE_DIR} "display/*.c")
file(GLOB_RECURSE DIAL ${CMAKE_SOURCE_DIR} "dial/*.cpp")
file(GLOB_RECURSE USB ${CMAKE_SOURCE_DIR} "usb_device/*.c")
file(GLOB_RECURSE POWER ${CMAKE_SOURCE_DIR} "dial_power/*.c")
file(GLOB_RECURSE NVS_DATA ${CMAKE_SOURCE_DIR} "nvs_data/*.c")
file(GLOB_RECURSE WIFI ${CMAKE_SOURCE_DIR} "wifi/*.c")
idf_component_register(SRCS "main.c" ${SRC_UI} ${DIAL} ${DISPLAY} ${USB} ${POWER} ${NVS_DATA} ${WIFI}
                    INCLUDE_DIRS "." "ui" "dial" "display" "usb_device" "dial_power" "nvs_data" "wifi"
                    )
