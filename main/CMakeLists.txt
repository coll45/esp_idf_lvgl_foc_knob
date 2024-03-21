file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")
file(GLOB_RECURSE DISPLAY ${CMAKE_SOURCE_DIR} "display/*.c")
file(GLOB_RECURSE DIAL ${CMAKE_SOURCE_DIR} "dial/*.cpp")
file(GLOB_RECURSE USB ${CMAKE_SOURCE_DIR} "usb_device/*.c")
idf_component_register(SRCS "main.c" ${SRC_UI} ${DIAL} ${DISPLAY} ${USB}
                    INCLUDE_DIRS "." "ui" "dial" "display" "usb_device")
